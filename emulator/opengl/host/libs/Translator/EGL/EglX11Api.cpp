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
#include "EglOsApi.h"
#include <string.h>
#include <X11/Xlib.h>
#include <dlfcn.h>
#include <stdio.h>
/* krnlyng */
//#include <GL/glx.h>
#include <utils/threads.h>


class ErrorHandler{
public:
ErrorHandler(EGLNativeDisplayType dpy);
~ErrorHandler();
int getLastError(){ return s_lastErrorCode;};

private:
static int s_lastErrorCode;
int (*m_oldErrorHandler) (Display *, XErrorEvent *);
static android::Mutex s_lock;
static int errorHandlerProc(EGLNativeDisplayType dpy,XErrorEvent* event);

};

/* krnlyng */
class SrfcInfo{
public:
    typedef enum{
                 WINDOW  = 0,
                 PBUFFER = 1, 
                 PIXMAP
                }SurfaceType;
    SrfcInfo(EGLSurface drawable,SurfaceType type):m_type(type),
                                                    m_srfc(drawable){};
    EGLSurface srfc(){return m_srfc;};
private: 
    SurfaceType m_type;
    EGLSurface  m_srfc; 
};

int ErrorHandler::s_lastErrorCode = 0;
android::Mutex ErrorHandler::s_lock;

ErrorHandler::ErrorHandler(EGLNativeDisplayType dpy){
   android::Mutex::Autolock mutex(s_lock);
   XSync(dpy,False);
   s_lastErrorCode = 0;
   m_oldErrorHandler = XSetErrorHandler(errorHandlerProc);
}

ErrorHandler::~ErrorHandler(){
   android::Mutex::Autolock mutex(s_lock);
   XSetErrorHandler(m_oldErrorHandler);
   s_lastErrorCode = 0;
}

int ErrorHandler::errorHandlerProc(EGLNativeDisplayType dpy,XErrorEvent* event){
    android::Mutex::Autolock mutex(s_lock);
    s_lastErrorCode = event->error_code;
    return 0;
}

/* krnlyng */
#define IS_SUCCESS(a) \
        if(a != EGL_TRUE) return false;

void *host_egl = NULL;

EGLBoolean (*host_eglGetConfigAttrib_ptr)(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value) = NULL;
EGLBoolean (*host_eglGetConfigs_ptr)(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config) = NULL;
EGLSurface (*host_eglCreatePbufferSurface_ptr)(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list) = NULL;
EGLBoolean (*host_eglDestroySurface_ptr)(EGLDisplay dpy, EGLSurface surface) = NULL;
EGLContext (*host_eglCreateContext_ptr)(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) = NULL;
EGLBoolean (*host_eglDestroyContext_ptr)(EGLDisplay dpy, EGLContext ctx) = NULL;
EGLBoolean (*host_eglMakeCurrent_ptr)(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) = NULL;
EGLBoolean (*host_eglSwapBuffers_ptr)(EGLDisplay dpy, EGLSurface surface) = NULL;
EGLBoolean (*host_eglWaitNative_ptr)(EGLint engine) = NULL;
EGLBoolean (*host_eglSwapInterval_ptr)(EGLDisplay dpy, EGLint interval) = NULL;

void init_host_egl()
{
    host_egl = dlopen("libEGL.so", RTLD_LAZY);
    if(!host_egl)
    {
        printf("failed to initialize host egl\n");
        exit(-1);
    }

    host_eglGetConfigAttrib_ptr = dlsym(host_egl, "eglGetConfigAttrib");
    host_eglGetConfigs_ptr = dlsym(host_egl, "eglGetConfigs");
    host_eglCreatePbufferSurface_ptr = dlsym(host_egl, "eglCreatePbufferSurface");
    host_eglDestroySurface_ptr = dlsym(host_egl, "eglDestroySurface");
    host_eglCreateContext_ptr = dlsym(host_egl, "eglCreateContext");
    host_eglDestroyContext_ptr = dlsym(host_egl, "eglDestroyContext");
    host_eglMakeCurrent_ptr = dlsym(host_egl, "eglMakeCurrent");
    host_eglSwapBuffers_ptr = dlsym(host_egl, "eglSwapBuffers");
    host_eglWaitNative_ptr = dlsym(host_egl, "eglWaitNative");
    host_eglSwapInterval_ptr = dlsym(host_egl, "eglSwapInterval");
}

EGLBoolean host_eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
    if(!host_egl) init_host_egl();
    return host_eglGetConfigAttrib_ptr(dpy, config, attribute, value);
}

EGLBoolean host_eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    if(!host_egl) init_host_egl();
    return host_eglGetConfigs_ptr(dpy, configs, config_size, num_config);
}

EGLSurface host_eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
{
    if(!host_egl) init_host_egl();
    return host_eglCreatePbufferSurface_ptr(dpy, config, attrib_list);
}

EGLBoolean host_eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
    if(!host_egl) init_host_egl();
    return host_eglDestroySurface_ptr(dpy, surface);
}

EGLContext host_eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
    if(!host_egl) init_host_egl();
    return host_eglCreateContext_ptr(dpy, config, share_context, attrib_list);
}

EGLBoolean host_eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
    if(!host_egl) init_host_egl();
    return host_eglDestroyContext_ptr(dpy, ctx);
}

EGLBoolean host_eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    if(!host_egl) init_host_egl();
    return host_eglMakeCurrent_ptr(dpy, draw, read, ctx);
}

EGLBoolean host_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    if(!host_egl) init_host_egl();
    return host_eglSwapBuffers_ptr(dpy, surface);
}

EGLBoolean host_eglWaitNative(EGLint engine)
{
    if(!host_egl) init_host_egl();
    return host_eglWaitNative_ptr(engine);
}

EGLBoolean host_eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
    if(!host_egl) init_host_egl();
    return host_eglSwapInterval_ptr(dpy, interval);
}

namespace EglOS {

EGLNativeDisplayType getDefaultDisplay() {return XOpenDisplay(0);}

bool releaseDisplay(EGLNativeDisplayType dpy) {
    return XCloseDisplay(dpy);
}

EglConfig* pixelFormatToConfig(EGLNativeDisplayType dpy,int renderableType,EGLNativePixelFormatType* frmt){

    int  bSize,red,green,blue,alpha,depth,stencil;
    int  supportedSurfaces,visualType,visualId;
    int  caveat,transparentType,samples;
    int  tRed=0,tGreen=0,tBlue=0;
    int  pMaxWidth,pMaxHeight,pMaxPixels;
    int  tmp;
    int  configId,level,renderable;
    int  doubleBuffer;

    /* krnlyng */
    IS_SUCCESS(host_eglGetConfigAttrib(dpy, *frmt, EGL_TRANSPARENT_TYPE, &transparentType));
    /*IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_TRANSPARENT_TYPE,&tmp));
    if(tmp == GLX_TRANSPARENT_INDEX) {
        return NULL; // not supporting transparent index
    } else if( tmp == GLX_NONE) {
        transparentType = EGL_NONE;
    } else {
        transparentType = EGL_TRANSPARENT_RGB;
    }
    */
        /* krnlyng */
        IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_TRANSPARENT_RED_VALUE,&tRed));
        IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_TRANSPARENT_GREEN_VALUE,&tGreen));
        IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_TRANSPARENT_BLUE_VALUE,&tBlue));
        //IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_TRANSPARENT_RED_VALUE,&tRed));
        //IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_TRANSPARENT_GREEN_VALUE,&tGreen));
        //IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_TRANSPARENT_BLUE_VALUE,&tBlue));


    //
    // filter out single buffer configurations
    //
    /* krnlyng */
    doubleBuffer = 1;
    //IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_DOUBLEBUFFER,&doubleBuffer));
    //if (!doubleBuffer) return NULL;

    /* krnlyng */
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_BUFFER_SIZE,&bSize));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_RED_SIZE,&red));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_GREEN_SIZE,&green));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_BLUE_SIZE,&blue));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_ALPHA_SIZE,&alpha));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_DEPTH_SIZE,&depth));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_STENCIL_SIZE,&stencil));

    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_NATIVE_RENDERABLE,&renderable));

    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_NATIVE_VISUAL_TYPE,&visualType));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_NATIVE_VISUAL_ID,&visualId));
    /*
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_BUFFER_SIZE,&bSize));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_RED_SIZE,&red));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_GREEN_SIZE,&green));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_BLUE_SIZE,&blue));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_ALPHA_SIZE,&alpha));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_DEPTH_SIZE,&depth));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_STENCIL_SIZE,&stencil));


    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_X_RENDERABLE,&renderable));

    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_X_VISUAL_TYPE,&visualType));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_VISUAL_ID,&visualId));
*/

    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_SURFACE_TYPE,&supportedSurfaces));
    /*
    //supported surfaces types
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_DRAWABLE_TYPE,&tmp));
    supportedSurfaces = 0;
    if(tmp & GLX_WINDOW_BIT && visualId != 0) {
        supportedSurfaces |= EGL_WINDOW_BIT;
    } else {
        visualId = 0;
        visualType = EGL_NONE;
    }
    if(tmp & GLX_PBUFFER_BIT) supportedSurfaces |= EGL_PBUFFER_BIT;
    */
    caveat = 0;
    /* krnlyng */
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_CONFIG_CAVEAT,&caveat));
    /*
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_CONFIG_CAVEAT,&tmp));
    if     (tmp == GLX_NONE) caveat = EGL_NONE;
    else if(tmp == GLX_SLOW_CONFIG) caveat = EGL_SLOW_CONFIG;
    else if(tmp == GLX_NON_CONFORMANT_CONFIG) caveat = EGL_NON_CONFORMANT_CONFIG;
    */
    /* krnlyng */
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_MAX_PBUFFER_WIDTH,&pMaxWidth));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_MAX_PBUFFER_HEIGHT,&pMaxHeight));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_MAX_PBUFFER_HEIGHT,&pMaxPixels));
    /*
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_MAX_PBUFFER_WIDTH,&pMaxWidth));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_MAX_PBUFFER_HEIGHT,&pMaxHeight));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_MAX_PBUFFER_HEIGHT,&pMaxPixels));
    */

    /* krnlyng */
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_LEVEL,&level));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_CONFIG_ID,&configId));
    IS_SUCCESS(host_eglGetConfigAttrib(dpy,*frmt,EGL_SAMPLES,&samples));
    /*
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_LEVEL,&level));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_FBCONFIG_ID,&configId));
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_SAMPLES,&samples));
    */
    //Filter out configs that does not support RGBA
    /* krnlyng hmmm */
    /*
    IS_SUCCESS(glXGetFBConfigAttrib(dpy,*frmt,GLX_RENDER_TYPE,&tmp));
    if (!(tmp & GLX_RGBA_BIT)) {
        return NULL;
    }
    */

    return new EglConfig(red,green,blue,alpha,caveat,configId,depth,level,pMaxWidth,pMaxHeight,
                              pMaxPixels,renderable,renderableType,visualId,visualType,samples,stencil,
                              supportedSurfaces,transparentType,tRed,tGreen,tBlue,*frmt);
}

void queryConfigs(EGLNativeDisplayType dpy,int renderableType,ConfigsList& listOut) {
    int n;
    /* krnlyng */
    //EGLNativePixelFormatType*  frmtList =  glXGetFBConfigs(dpy,0,&n);
    EGLNativePixelFormatType *frmtList;
    host_eglGetConfigs(dpy, NULL, 0, &n);
    frmtList = (EGLNativePixelFormatType*)malloc(sizeof(EGLNativePixelFormatType)*n);
    host_eglGetConfigs(dpy, frmtList, n, &n);
    for(int i =0 ;i < n ; i++) {
        EglConfig* conf = pixelFormatToConfig(dpy,renderableType,&frmtList[i]);
        if(conf) listOut.push_back(conf);
    }
    free(frmtList);
    //XFree(frmtList);
}

bool validNativeWin(EGLNativeDisplayType dpy,EGLNativeWindowType win) {
   Window root;
   int tmp;
   unsigned int utmp;
   ErrorHandler handler(dpy);
   if(!XGetGeometry(dpy,win,&root,&tmp,&tmp,&utmp,&utmp,&utmp,&utmp)) return false;
   return handler.getLastError() == 0;
}

/* krnlyng */
bool validNativeWin(EGLNativeDisplayType dpy,EGLNativeSurfaceType win) {
   Window root;
   int tmp;
   unsigned int utmp;
   ErrorHandler handler(dpy);
   if(!XGetGeometry(dpy,(Drawable)win,&root,&tmp,&tmp,&utmp,&utmp,&utmp,&utmp)) return false;
   return handler.getLastError() == 0;
}

bool validNativePixmap(EGLNativeDisplayType dpy,EGLNativeSurfaceType pix) {
   Window root;
   int tmp;
   unsigned int utmp;
   ErrorHandler handler(dpy);
    /* krnlyng hmm */
   if(!XGetGeometry(dpy,(Drawable) (pix ? pix->srfc() : NULL),&root,&tmp,&tmp,&utmp,&utmp,&utmp,&utmp)) return false;
   return handler.getLastError() == 0;
}

bool checkWindowPixelFormatMatch(EGLNativeDisplayType dpy,EGLNativeWindowType win,EglConfig* cfg,unsigned int* width,unsigned int* height) {
//TODO: to check what does ATI & NVIDIA enforce on win pixelformat
   unsigned int depth,configDepth,border;
   int r,g,b,x,y;
    /* krnlyng */
   IS_SUCCESS(host_eglGetConfigAttrib(dpy,cfg->nativeConfig(),EGL_RED_SIZE,&r));
   IS_SUCCESS(host_eglGetConfigAttrib(dpy,cfg->nativeConfig(),EGL_GREEN_SIZE,&g));
   IS_SUCCESS(host_eglGetConfigAttrib(dpy,cfg->nativeConfig(),EGL_BLUE_SIZE,&b));
    /*
   IS_SUCCESS(glXGetFBConfigAttrib(dpy,cfg->nativeConfig(),GLX_RED_SIZE,&r));
   IS_SUCCESS(glXGetFBConfigAttrib(dpy,cfg->nativeConfig(),GLX_GREEN_SIZE,&g));
   IS_SUCCESS(glXGetFBConfigAttrib(dpy,cfg->nativeConfig(),GLX_BLUE_SIZE,&b));
    */
   configDepth = r + g + b;
   Window root;
   if(!XGetGeometry(dpy,win,&root,&x,&y,width,height,&border,&depth)) return false;
   return depth >= configDepth;
}

bool checkPixmapPixelFormatMatch(EGLNativeDisplayType dpy,EGLNativePixmapType pix,EglConfig* cfg,unsigned int* width,unsigned int* height) {
   unsigned int depth,configDepth,border;
   int r,g,b,x,y;
    /* krnlyng */
   IS_SUCCESS(host_eglGetConfigAttrib(dpy,cfg->nativeConfig(),EGL_RED_SIZE,&r));
   IS_SUCCESS(host_eglGetConfigAttrib(dpy,cfg->nativeConfig(),EGL_GREEN_SIZE,&g));
   IS_SUCCESS(host_eglGetConfigAttrib(dpy,cfg->nativeConfig(),EGL_BLUE_SIZE,&b));
    /*
   IS_SUCCESS(glXGetFBConfigAttrib(dpy,cfg->nativeConfig(),GLX_RED_SIZE,&r));
   IS_SUCCESS(glXGetFBConfigAttrib(dpy,cfg->nativeConfig(),GLX_GREEN_SIZE,&g));
   IS_SUCCESS(glXGetFBConfigAttrib(dpy,cfg->nativeConfig(),GLX_BLUE_SIZE,&b));
    */
   configDepth = r + g + b;
   Window root;
   if(!XGetGeometry(dpy,pix,&root,&x,&y,width,height,&border,&depth)) return false;
   return depth >= configDepth;
}

EGLNativeSurfaceType createPbufferSurface(EGLNativeDisplayType dpy,EglConfig* cfg,EglPbufferSurface* srfc){
    EGLint width,height,largest;
    srfc->getDim(&width,&height,&largest);
/* krnylng */
    EGLint attribs[] = {
                     EGL_WIDTH                   ,width,
                     EGL_HEIGHT                  ,height,
                     EGL_LARGEST_PBUFFER         ,largest,
                     EGL_NONE
                    };
    EGLSurface pb = host_eglCreatePbufferSurface(dpy, cfg->nativeConfig(), attribs);
/*
    int attribs[] = {
                     GLX_PBUFFER_WIDTH           ,width,
                     GLX_PBUFFER_HEIGHT          ,height,
                     GLX_LARGEST_PBUFFER         ,largest,
                     None
                    };
    GLXPbuffer pb = glXCreatePbuffer(dpy,cfg->nativeConfig(),attribs);
*/
    return pb ? new SrfcInfo(pb, SrfcInfo::PBUFFER) : NULL;
}

bool releasePbuffer(EGLNativeDisplayType dis,EGLNativeSurfaceType pb) {
    if (!pb) return false;
    /* krnylng */
    host_eglDestroySurface(dis,pb->srfc());
    //glXDestroyPbuffer(dis,pb->srfc());

    return true;
}

EGLNativeContextType createContext(EGLNativeDisplayType dpy,EglConfig* cfg,EGLNativeContextType sharedContext, int version) {
 ErrorHandler handler(dpy);
    /* krnlyng */
 int attribs_1[] = {
        EGL_CONTEXT_CLIENT_VERSION, 1,
        EGL_NONE
    };
 int attribs_2[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
 //EGLNativeContextType retVal = glXCreateNewContext(dpy,cfg->nativeConfig(),GLX_RGBA_TYPE,sharedContext,true);
  EGLNativeContextType retVal = host_eglCreateContext(dpy,cfg->nativeConfig(),sharedContext, (version == 1) ? attribs_1 : attribs_2); /* krnlyng hmm */
 return handler.getLastError() == 0 ? retVal : NULL;
}

bool destroyContext(EGLNativeDisplayType dpy,EGLNativeContextType ctx) {
    /* krnlyng */
    host_eglDestroyContext(dpy,ctx);
    //glXDestroyContext(dpy,ctx);
    return true;
}

bool makeCurrent(EGLNativeDisplayType dpy,EglSurface* read,EglSurface* draw,EGLNativeContextType ctx){

    ErrorHandler handler(dpy);
    bool retval = false;
    if (!ctx && !read && !draw) {
        // unbind
        /* krnlyng */
        retval = host_eglMakeCurrent(dpy, NULL, NULL, NULL);
        //retval = glXMakeContextCurrent(dpy, NULL, NULL, NULL);
    }
    else if (ctx && read && draw) {
        /* krnlyng */
        retval = host_eglMakeCurrent(dpy,draw->native()->srfc(),read->native()->srfc(),ctx);
        //retval = glXMakeContextCurrent(dpy,draw->native()->srfc(),read->native()->srfc(),ctx);
    }
    return (handler.getLastError() == 0) && retval;
}

void swapBuffers(EGLNativeDisplayType dpy,EGLNativeSurfaceType srfc){
    if (srfc) {
        /* krnlyng */
        host_eglSwapBuffers(dpy,srfc->srfc());
        //glXSwapBuffers(dpy,srfc->srfc());
    }
}

void waitNative() {
    /* krnlyng */
    host_eglWaitNative(EGL_CORE_NATIVE_ENGINE);
    //glXWaitX();
}

void swapInterval(EGLNativeDisplayType dpy,EGLNativeSurfaceType win,int interval){
    /* krnlyng */
    host_eglSwapInterval(dpy,interval);
/*
    const char* extensions = glXQueryExtensionsString(dpy,DefaultScreen(dpy));
    typedef void (*GLXSWAPINTERVALEXT)(Display*,GLXDrawable,int);
    GLXSWAPINTERVALEXT glXSwapIntervalEXT = NULL;

    if(strstr(extensions,"EXT_swap_control")) {
        glXSwapIntervalEXT = (GLXSWAPINTERVALEXT)glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
    }
    if(glXSwapIntervalEXT && win) {
        glXSwapIntervalEXT(dpy,win->srfc(),interval);
    }
*/
}

EGLNativeSurfaceType createWindowSurface(EGLNativeWindowType wnd){
    return new SrfcInfo((EGLSurface)wnd,SrfcInfo::WINDOW); /* krnlyng hmm */
}

EGLNativeSurfaceType createPixmapSurface(EGLNativePixmapType pix){
    return new SrfcInfo((EGLSurface)pix,SrfcInfo::PIXMAP); /* krnlyng hmm */
}

void destroySurface(EGLNativeSurfaceType srfc){
    delete srfc;
};

EGLNativeInternalDisplayType getInternalDisplay(EGLNativeDisplayType dpy){
    return dpy;
}

void deleteDisplay(EGLNativeInternalDisplayType idpy){
}

};
