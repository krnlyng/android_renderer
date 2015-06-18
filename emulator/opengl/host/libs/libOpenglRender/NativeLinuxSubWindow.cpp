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

static wl_display *s_display = NULL;
volatile struct wl_compositor *compositor = NULL;
/*
static void handle_wl_stuff(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    if(strcmp(interface, "wl_compositor") == 0)
    {
        compositor = wl_registry_bind(registry, id, &wl_compositor_interface, version);
    }
}

static const struct wl_registry_listener registry_listener = {
    handle_wl_stuff
};*/

EGLNativeWindowType createSubWindow(FBNativeWindowType p_window,
                                    EGLNativeDisplayType* display_out,
                                    int x, int y,int width, int height){

   // The call to this function is protected by a lock
   // in FrameBuffer so it is safe to check and initialize s_display here
   printf("creating sub window\n");
    if(!s_display) s_display = wl_display_connect(0);
    *display_out = s_display;/*
    if(compositor == NULL)
    {
        struct wl_registry *registry = wl_display_get_registry(s_display);
        wl_registry_add_listener(registry, &registry_listener, NULL);
        while(compositor == NULL) wl_display_dispatch(s_display);
    }*/
//    EGLNativeWindowType win = wl_compositor_create_surface(compositor);
    EGLNativeWindowType win = wl_egl_window_create(p_window, width, height);
    if(win == EGL_NO_SURFACE || win == NULL)
    {
        fprintf(stderr, "wl_egl_window_create failed\n");
    }
    printf("created sub window\n");
    return win;

   /*
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
    wl_egl_window_destroy(win);
    //XDestroyWindow(dis, win);
}
