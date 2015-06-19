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
#include "NativeSubWindow.h"

#include <stdio.h>
#include <string.h>
#include <wayland-egl.h>

/*
static Bool WaitForMapNotify(Display *d, XEvent *e, char *arg)
{
    if (e->type == MapNotify && e->xmap.window == (Window)arg) {
	    return 1;
    }
    return 0;
}
*/

/* krnlyng, thanks to http://jan.newmarch.name/Wayland/EGL */

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

static int
get_server_references(void) {

    the_wl_display = wl_display_connect(NULL);
    if (the_wl_display == NULL) {
        fprintf(stderr, "Can't connect to display\n");
        return -1;
    }

    printf("connected to display\n");

    struct wl_registry *registry = wl_display_get_registry(the_wl_display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_dispatch(the_wl_display);
    wl_display_roundtrip(the_wl_display);

    if (compositor == NULL || shell == NULL) {
        fprintf(stderr, "Can't find compositor or shell\n");
        return -1;
    } else {
        fprintf(stderr, "Found compositor and shell\n");
    }

    return 0;
}

EGLNativeWindowType createSubWindow(FBNativeWindowType p_window,
                                    EGLNativeDisplayType* display_out,
                                    int x, int y,int width, int height) {

    /* krnlyng */
    printf("creating sub window\n");
    if (!the_wl_display) the_wl_display = wl_display_connect(0);
    *display_out = the_wl_display;

    if(get_server_references() < 0) return NULL;

    surface = wl_compositor_create_surface(compositor);
    if(surface == NULL)
    {
        fprintf(stderr, "Cannot create wl surface\n");
        return NULL;
    }

    shell_surface = wl_shell_get_shell_surface(shell, surface);
    wl_shell_surface_set_toplevel(shell_surface);

    EGLNativeWindowType win = wl_egl_window_create(surface, width, height);
    if(win == EGL_NO_SURFACE || win == NULL)
    {
        fprintf(stderr, "wl_egl_window_create failed\n");
    }
    printf("created sub window\n");

    return win;

   /*
   // The call to this function is protected by a lock
   // in FrameBuffer so it is safe to check and initialize s_display here
   if (!s_display) s_display = XOpenDisplay(NULL);
   *display_out = s_display;

    XSetWindowAttributes wa;
    wa.event_mask = StructureNotifyMask;
    Window win = XCreateWindow(*display_out,p_window,x,y, width,height,0,CopyFromParent,CopyFromParent,CopyFromParent,CWEventMask,&wa);
    XMapWindow(*display_out,win);
    XEvent e;
    XIfEvent(*display_out, &e, WaitForMapNotify, (char *)win);
    return win;*/
}

void destroySubWindow(EGLNativeDisplayType dis,EGLNativeWindowType win){
    /* krnlyng hmm */
    //wl_surface_destroy(win);
    wl_egl_window_destroy((struct wl_egl_window*)win);
    //XDestroyWindow(dis, win);
}
