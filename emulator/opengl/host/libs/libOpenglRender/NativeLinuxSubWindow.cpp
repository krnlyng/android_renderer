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
#ifndef X11
#include <wayland-egl.h>
#else
#endif

#ifdef X11
static Bool WaitForMapNotify(Display *d, XEvent *e, char *arg)
{
    if (e->type == MapNotify && e->xmap.window == (Window)arg) {
	    return 1;
    }
    return 0;
}
#endif

/* krnlyng, thanks to http://jan.newmarch.name/Wayland/EGL */
#ifndef X11
struct wl_display *the_display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_surface *surface;
struct wl_egl_window *egl_window;
struct wl_region *region;
struct wl_shell *shell;
struct wl_shell_surface *shell_surface;

static void
create_opaque_region(int width, int height) {
    region = wl_compositor_create_region(compositor);
    wl_region_add(region, 0, 0,
          width,
          height);
    wl_surface_set_opaque_region(surface, region);
}

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
get_server_references(EGLNativeDisplayType display) {
    the_display = display;
    if(the_display == NULL)
    {
        fprintf(stderr, "get_server_references: invalid display argument\n");
        return -1;
    }

    struct wl_registry *registry = wl_display_get_registry(the_display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_dispatch(the_display);
    wl_display_roundtrip(the_display);

    if (compositor == NULL || shell == NULL) {
        fprintf(stderr, "Can't find compositor or shell\n");
        return -1;
    } else {
        fprintf(stderr, "Found compositor and shell\n");
    }

    return 0;
}
#endif
EGLNativeWindowType createSubWindow(FBNativeWindowType p_window,
                                    EGLNativeDisplayType display,
                                    int x, int y,int width, int height) {

    /* krnlyng */
    printf("creating sub window\n"); 
#ifndef X11
    if(get_server_references(display) < 0) return NULL;

    surface = wl_compositor_create_surface(compositor);
    if(surface == NULL)
    {
        fprintf(stderr, "Cannot create wl surface\n");
        return NULL;
    }

    shell_surface = wl_shell_get_shell_surface(shell, surface);
    wl_shell_surface_set_toplevel(shell_surface);

    create_opaque_region(width, height);

    EGLNativeWindowType win = wl_egl_window_create(surface, width, height);
    if(win == EGL_NO_SURFACE || win == NULL)
    {
        fprintf(stderr, "wl_egl_window_create failed\n");
    }
    printf("created sub window\n");

    return win;
#else
    XSetWindowAttributes wa;
    wa.event_mask = StructureNotifyMask;
    Window win = XCreateWindow(display,p_window,x,y, width,height,0,CopyFromParent,CopyFromParent,CopyFromParent,CWEventMask,&wa);
    XMapWindow(display,win);
    XEvent e;
    XIfEvent(display, &e, WaitForMapNotify, (char *)win);
    return win;
#endif
}

void destroySubWindow(EGLNativeDisplayType dis,EGLNativeWindowType win){
    /* krnlyng hmm */
#ifndef X11
    //wl_surface_destroy(win);
    wl_egl_window_destroy((struct wl_egl_window*)win);
#else
    XDestroyWindow(dis, win);
#endif
}
