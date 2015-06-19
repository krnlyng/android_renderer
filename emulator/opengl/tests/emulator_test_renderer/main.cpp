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
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <EGL/egl.h>
#include <wayland-egl.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>

#include <stdio.h>
#include <string.h>
#include "libOpenglRender/render_api.h"
#include <EventInjector.h>

static int convert_keysym(int sym); // forward

#ifdef __linux__
/* krnylng */
//#include <X11/Xlib.h>
#endif
#ifdef _WIN32

#include <winsock2.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
/* krnlyng, thanks to http://jan.newmarch.name/Wayland/EGL */
/*
struct wl_display *the_wl_display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_surface *surface;
struct wl_egl_window *egl_window;
struct wl_region *region;
struct wl_shell *shell;
struct wl_shell_surface *shell_surface;

static void
global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
           const char *interface, uint32_t version)
{
//    printf("Got a registry event for %s id %d\n", interface, id);
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = (wl_compositor*)wl_registry_bind(registry, 
                      id, 
                      &wl_compositor_interface, 
                      1);
    } else if (strcmp(interface, "wl_shell") == 0) {
    shell = (wl_shell*)wl_registry_bind(registry, id,
                 &wl_shell_interface, 1);
    
    }
}

static void
global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    printf("Got a registry losing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
    global_registry_handler,
    global_registry_remover
};

static void
get_server_references(void) {

    the_wl_display = wl_display_connect(NULL);

    if (the_wl_display == NULL) {
        fprintf(stderr, "Can't connect to display\n");
        exit(1);
    }

    printf("connected to display\n");

    struct wl_registry *registry = wl_display_get_registry(the_wl_display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_dispatch(the_wl_display);
    wl_display_roundtrip(the_wl_display);

    if (compositor == NULL || shell == NULL) {
        fprintf(stderr, "Can't find compositor or shell\n");
        exit(1);
    } else {
        fprintf(stderr, "Found compositor and shell\n");
    }
}

static void
create_opaque_region() {
    region = wl_compositor_create_region(compositor);
    wl_region_add(region, 0, 0,
          960,
          540);
    wl_surface_set_opaque_region(surface, region);
}
*/
int main(int argc, char *argv[])
#endif
{
    int portNum = 22468;
    int winWidth = 320;
    int winHeight = 480;
    int width, height;
    int mouseDown = 0;
    const char* env = getenv("ANDROID_WINDOW_SIZE");
    FBNativeWindowType windowId = NULL;
    EventInjector* injector;
    int consolePort = 5554;

    if (env && sscanf(env, "%dx%d", &width, &height) == 2) {
        winWidth = width;
        winHeight = height;
    }

#ifdef __linux__
    // some OpenGL implementations may call X functions
    // it is safer to synchronize all X calls made by all the
    // rendering threads. (although the calls we do are locked
    // in the FrameBuffer singleton object).
    /* krnlyng */
    //XInitThreads();
#endif

    //
    // Inialize SDL window
    //
    /* krnlyng */
/*    if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO)) {
        fprintf(stderr,"SDL init failed: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Surface *surface = SDL_SetVideoMode(winWidth, winHeight, 32, SDL_SWSURFACE);
    if (surface == NULL) {
        fprintf(stderr,"Failed to set video mode: %s\n", SDL_GetError());
        return -1;
    }
*/

    initLibrary();

    printf("initializing renderer process\n");

    struct wl_display *the_wl_display = wl_display_connect(0);
    if(the_wl_display == NULL)
    {
        printf("Could not connect to display\n");
        return -1;
    }

    /* krnlyng do egl init here */
    bool inited = initOpenGLRenderer(winWidth, winHeight, portNum, 0, 0, the_wl_display);
    if (!inited) {
        return -1;
    }
    printf("renderer process started\n");

    /* TODO */
    SDL_Window *sdl_win = NULL;
/*
    SDL_Window *sdl_win = SDL_CreateWindowFrom((void*)windowId);
    if(sdl_win == NULL)
    {
        fprintf(stderr, "SDL_CreateWindowFrom failed\n");
        return -1;
    }
*/
/*
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
    if(window == NULL)
    {
        fprintf(stderr, "failed to create SDL2 window\n");
        return -1;
    }

    SDL_ShowCursor(0);
*/
    SDL_SysWMinfo  wminfo;
    memset(&wminfo, 0, sizeof(wminfo));
    /* krnlyng */
    SDL_GetWindowWMInfo(sdl_win, &wminfo);
#ifdef _WIN32
    windowId = wminfo.window;
    WSADATA  wsaData;
    int      rc = WSAStartup( MAKEWORD(2,2), &wsaData);
    if (rc != 0) {
            printf( "could not initialize Winsock\n" );
    }
#elif __linux__
    /* krnylng */
    //windowId = (FBNativeWindowType)wminfo.info.wl.surface;
#elif __APPLE__
    windowId = wminfo.nsWindowPtr;
#endif

/*
    Display *x_display = XOpenDisplay(NULL);
    if(x_display == NULL)
    {
        printf("cannot connect to the X server\n");
        return -1;
    }
    Window root = DefaultRootWindow(x_display);

    XSetWindowAttributes attr;
    attr.background_pixel = XWhitePixel(x_display, DefaultScreen(x_display));

    windowId = XCreateWindow(x_display, root,
                        0, 0, 960, 540, 0,
                        CopyFromParent, InputOutput,
                        CopyFromParent, CWEventMask,
                        &attr);

    SDL_Window *sdl_win = SDL_CreateWindowFrom((void*)windowId);*/


    //
    // initialize OpenGL renderer to render in our window
    //
    /*
    bool inited = initOpenGLRenderer(winWidth, winHeight, portNum, 0, 0);
    if (!inited) {
        return -1;
    }
    printf("renderer process started\n");*/

    float zRot = 0.0f;
    inited = createOpenGLSubwindow(windowId, 0, 0,
                                   winWidth, winHeight, zRot, the_wl_display);
    if (!inited) {
        printf("failed to create OpenGL subwindow\n");
        stopOpenGLRenderer();
        return -1;
    }
    int subwinWidth = winWidth;
    int subwinHeight = winHeight;

    injector = new EventInjector(consolePort);

    // Just wait until the window is closed
    SDL_Event ev;

    for (;;) {
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
                                                zRot, the_wl_display);
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
                                                zRot, the_wl_display);
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
        /* krnlyng */
        if(wl_display_dispatch(the_wl_display) < 0)
        {
            printf("wl_display_dispatch failed\n");
            break;
        }
    }
EXIT:
    //
    // stop the renderer
    //
    printf("stopping the renderer process\n");
    stopOpenGLRenderer();

    /* krnlyng */
    wl_display_disconnect(the_wl_display);

    return 0;
}

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
