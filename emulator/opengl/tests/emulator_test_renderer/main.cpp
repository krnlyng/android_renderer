/*
* Copyright (C) 2011 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#undef HAVE_MALLOC_H
/* krnlyng */
//#include <wayland-client.h>
#ifdef X11
#include <SDL.h>
#include <SDL_syswm.h>
#endif

#include <EGL/egl.h>
#ifdef X11
#include <X11/Xlib.h>
#else
#include <wayland-egl.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#endif

#include <stdio.h>
#include <string.h>
#include "libOpenglRender/render_api.h"
#include <EventInjector.h>

#ifdef X11
static int convert_keysym(int sym); // forward
#endif

#ifdef __linux__
/* krnylng */
#ifndef X11
struct touch {
    struct wl_seat *w_seat;
    struct wl_touch *w_touch;
    EventInjector* injector;
    bool down;
    float x;
    float y;
};

/* krnlyng EventInjector multitouch? */

static void touch_handle_down(void *data, struct wl_touch *wl_touch,
    uint32_t serial, uint32_t time, struct wl_surface *surface,
    int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    struct touch *the_touch = (struct touch*)data;

    if(!the_touch->down)
    {
        the_touch->injector->sendMouseDown(wl_fixed_to_double(x_w), wl_fixed_to_double(y_w));
        the_touch->down = 1;
    }
}
 
static void touch_handle_up(void *data, struct wl_touch *wl_touch,
  uint32_t serial, uint32_t time, int32_t id)
{
    struct touch *the_touch = (struct touch*)data;

    if(the_touch->down)
    {
        the_touch->injector->sendMouseUp(the_touch->x, the_touch->y);
        the_touch->down = 0;
    }
}
 
static void
touch_handle_motion(void *data, struct wl_touch *wl_touch,
      uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    struct touch *the_touch = (struct touch*)data;

    if(the_touch->down)
    {
        /* store new x, y for touch up */
        the_touch->x = wl_fixed_to_double(x_w);
        the_touch->y = wl_fixed_to_double(y_w);
        the_touch->injector->sendMouseMotion(the_touch->x, the_touch->y);
    }
}

static void touch_handle_frame(void *data, struct wl_touch *wl_touch)
{
}
 
static void touch_handle_cancel(void *data, struct wl_touch *wl_touch)
{
}

static const struct wl_touch_listener touch_listener = {
    touch_handle_down,
    touch_handle_up,
    touch_handle_motion,
    touch_handle_frame,
    touch_handle_cancel,
};

static void seat_handle_capabilities(void *data, struct wl_seat *the_seat, uint32_t caps)
{
    struct touch *the_touch = (struct touch*)data;

    if(caps & WL_SEAT_CAPABILITY_TOUCH && !the_touch->w_touch)
    {
        printf("acquired wl_touch\n");
        the_touch->w_touch = wl_seat_get_touch(the_touch->w_seat);
        wl_touch_add_listener(the_touch->w_touch, &touch_listener, the_touch);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_TOUCH) && !the_touch->w_touch)
    {
        printf("lost wl_touch\n");
        wl_touch_destroy(the_touch->w_touch);
        the_touch->w_touch = NULL;
    }
}

static const struct wl_seat_listener seat_listener = {
    seat_handle_capabilities
};

static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    struct touch *the_touch = (struct touch*)data;

    if(strcmp(interface, "wl_seat") == 0)
    {
        the_touch->w_seat = (struct wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(the_touch->w_seat, &seat_listener, the_touch);
    }
}

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    printf("Got a registry loosing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
    global_registry_handler,
    global_registry_remover
};
#endif  //X11
#endif
#ifdef _WIN32

#include <winsock2.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main(int argc, char *argv[])
#endif
{
#ifndef X11
    struct touch *the_touch;
#endif
    int portNum = 22468;
    int winWidth = 320;
    int winHeight = 480;
    int width, height;
    //int mouseDown = 0;
    const char* env = getenv("ANDROID_WINDOW_SIZE");
    FBNativeWindowType windowId = NULL;
    int consolePort = 5554;

    if (env && sscanf(env, "%dx%d", &width, &height) == 2) {
        winWidth = width;
        winHeight = height;
    }

    printf("initializing egl, gles dispatch\n");

    initLibrary();

#ifndef X11
    printf("initializing wayland input\n");
    the_touch = (struct touch*)malloc(sizeof(struct touch));
    the_touch->w_seat = NULL;
    the_touch->w_touch = NULL;
    the_touch->injector = new EventInjector(consolePort);
    the_touch->down = false;
    the_touch->x = 0;
    the_touch->y = 0;

    struct wl_display *the_display = wl_display_connect(0);
    if(the_display == NULL)
    {
        printf("Could not connect to display\n");
        return -1;
    }
    struct wl_registry *registry = wl_display_get_registry(the_display);
    wl_registry_add_listener(registry, &registry_listener, the_touch);

    wl_display_dispatch(the_display);
    wl_display_roundtrip(the_display);

    if(the_touch->w_seat == NULL || the_touch->w_touch == NULL)
    {
        fprintf(stderr, "failed to get seat or touch\n");
        return -1;
    }
#endif
    printf("initializing renderer process\n");

    //
    // initialize OpenGL renderer to render in our window
    //
#ifdef X11
    Display *the_display;
    the_display = XOpenDisplay(NULL);
#endif
    bool inited = initOpenGLRenderer(winWidth, winHeight, portNum, 0, 0, (EGLNativeDisplayType)the_display);
    if (!inited) {
        return -1;
    }
    printf("renderer process started\n");

#ifdef X11
#ifdef __linux__
    // some OpenGL implementations may call X functions
    // it is safer to synchronize all X calls made by all the
    // rendering threads. (although the calls we do are locked
    // in the FrameBuffer singleton object).
    XInitThreads();
#endif

    //
    // Inialize SDL window
    //
    if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO)) {
        fprintf(stderr,"SDL init failed: %s\n", SDL_GetError());
        return -1;
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_Window *window = SDL_CreateWindow("emulator_test_renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_OPENGL);
    if(window == NULL) {
        fprintf(stderr, "Failed to initialize SDL screen: %s\n", SDL_GetError());
        return -1;
    }
#else
    SDL_Surface *surface = SDL_SetVideoMode(winWidth, winHeight, 32, SDL_SWSURFACE);
    if (surface == NULL) {
        fprintf(stderr,"Failed to set video mode: %s\n", SDL_GetError());
        return -1;
    }
#endif

    SDL_SysWMinfo  wminfo;
    memset(&wminfo, 0, sizeof(wminfo));
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_VERSION(&wminfo.version);
    SDL_GetWindowWMInfo(window, &wminfo);
#else
    SDL_GetWMInfo(&wminfo);
#endif
#ifdef _WIN32
    windowId = wminfo.window;
    WSADATA  wsaData;
    int      rc = WSAStartup( MAKEWORD(2,2), &wsaData);
    if (rc != 0) {
            printf( "could not initialize Winsock\n" );
    }
#elif __linux__
    windowId = wminfo.info.x11.window;
#elif __APPLE__
    windowId = wminfo.nsWindowPtr;
#endif
#endif

    float zRot = 0.0f;
    inited = createOpenGLSubwindow(windowId, 0, 0,
                                   winWidth, winHeight, zRot, (EGLNativeDisplayType)the_display);
    if (!inited) {
        printf("failed to create OpenGL subwindow\n");
        stopOpenGLRenderer();
        return -1;
    }
#ifdef X11
    int subwinWidth = winWidth;
    int subwinHeight = winHeight;
    EventInjector* injector = new EventInjector(consolePort);
    SDL_Event ev;
    int mouseDown = 0;
#endif

    for (;;) {
#ifndef X11
        the_touch->injector->wait(1000/15);
        the_touch->injector->poll();

#else
        injector->wait(1000/15);
        injector->poll();

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (!mouseDown) {
                    injector->sendMouseDown(ev.button.x, ev.button.y);
                    mouseDown = 1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (mouseDown) {
                    injector->sendMouseUp(ev.button.x,ev.button.y);
                    mouseDown = 0;
                }
                break;
            case SDL_MOUSEMOTION:
                if (mouseDown)
                    injector->sendMouseMotion(ev.button.x,ev.button.y);
                break;

            case SDL_KEYDOWN:
#ifdef __APPLE__
                /* special code to deal with Command-Q properly */
                if (ev.key.keysym.sym == SDLK_q &&
                    ev.key.keysym.mod & KMOD_META) {
                  goto EXIT;
                }
#endif
                injector->sendKeyDown(convert_keysym(ev.key.keysym.sym));

                if (ev.key.keysym.sym == SDLK_KP_MINUS) {
                    subwinWidth /= 2;
                    subwinHeight /= 2;
                    
                    bool stat = destroyOpenGLSubwindow();
                    printf("destroy subwin returned %d\n", stat);
                    stat = createOpenGLSubwindow(windowId,
                                                (winWidth - subwinWidth) / 2,
                                                (winHeight - subwinHeight) / 2,
                                                subwinWidth, subwinHeight, 
                                                zRot, (EGLNativeDisplayType)the_display);
                    printf("create subwin returned %d\n", stat);
                }
                else if (ev.key.keysym.sym == SDLK_KP_PLUS) {
                    subwinWidth *= 2;
                    subwinHeight *= 2;

                    bool stat = destroyOpenGLSubwindow();
                    printf("destroy subwin returned %d\n", stat);
                    stat = createOpenGLSubwindow(windowId,
                                                (winWidth - subwinWidth) / 2,
                                                (winHeight - subwinHeight) / 2,
                                                subwinWidth, subwinHeight, 
                                                zRot, (EGLNativeDisplayType)the_display);
                    printf("create subwin returned %d\n", stat);
                }
                else if (ev.key.keysym.sym == SDLK_KP_MULTIPLY) {
                    zRot += 10.0f;
                    setOpenGLDisplayRotation(zRot);
                }
                else if (ev.key.keysym.sym == SDLK_KP_ENTER) {
                    repaintOpenGLDisplay();
                }
                break;
            case SDL_KEYUP:
                injector->sendKeyUp(convert_keysym(ev.key.keysym.sym));
                break;
            case SDL_QUIT:
                goto EXIT;
            }
        }
#endif
#ifndef X11
        /* krnlyng */
        if(wl_display_dispatch(the_display) < 0)
        {
            printf("wl_display_dispatch failed\n");
            break;
        }
#endif
    }
#ifdef X11
EXIT:
#endif
    //
    // stop the renderer
    //
    printf("stopping the renderer process\n");
    stopOpenGLRenderer();
#ifdef X11
    delete injector;
#else
    /* krnlyng */
    wl_display_disconnect(the_display);

    free(the_touch);

    return 0;
#endif
}
#ifdef X11
static int convert_keysym(int sym)
{
#define  EE(x,y)   SDLK_##x, EventInjector::KEY_##y,
    static const int keymap[] = {
        EE(LEFT,LEFT)
        EE(RIGHT,RIGHT)
        EE(DOWN,DOWN)
        EE(UP,UP)
        EE(RETURN,ENTER)
        EE(F1,SOFT1)
        EE(ESCAPE,BACK)
        EE(HOME,HOME)
        -1
    };
    int nn;
    for (nn = 0; keymap[nn] >= 0; nn += 2) {
        if (keymap[nn] == sym)
            return keymap[nn+1];
    }
    return sym;
}
#endif

