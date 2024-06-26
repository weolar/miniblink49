// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file is auto-generated from
// ui/gl/generate_bindings.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

#include <string>

#include "base/trace_event/trace_event.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_enums.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_version_info.h"
#include "ui/gl/gl_wgl_api_implementation.h"

namespace gfx {

static bool g_debugBindingsInitialized;
DriverWGL g_driver_wgl;

void DriverWGL::InitializeStaticBindings()
{
    fn.wglChoosePixelFormatARBFn = 0;
    fn.wglCopyContextFn = reinterpret_cast<wglCopyContextProc>(GetGLProcAddress("wglCopyContext"));
    fn.wglCreateContextFn = reinterpret_cast<wglCreateContextProc>(
        GetGLProcAddress("wglCreateContext"));
    fn.wglCreateLayerContextFn = reinterpret_cast<wglCreateLayerContextProc>(
        GetGLProcAddress("wglCreateLayerContext"));
    fn.wglCreatePbufferARBFn = 0;
    fn.wglDeleteContextFn = reinterpret_cast<wglDeleteContextProc>(
        GetGLProcAddress("wglDeleteContext"));
    fn.wglDestroyPbufferARBFn = 0;
    fn.wglGetCurrentContextFn = reinterpret_cast<wglGetCurrentContextProc>(
        GetGLProcAddress("wglGetCurrentContext"));
    fn.wglGetCurrentDCFn = reinterpret_cast<wglGetCurrentDCProc>(
        GetGLProcAddress("wglGetCurrentDC"));
    fn.wglGetExtensionsStringARBFn = reinterpret_cast<wglGetExtensionsStringARBProc>(
        GetGLProcAddress("wglGetExtensionsStringARB"));
    fn.wglGetExtensionsStringEXTFn = reinterpret_cast<wglGetExtensionsStringEXTProc>(
        GetGLProcAddress("wglGetExtensionsStringEXT"));
    fn.wglGetPbufferDCARBFn = 0;
    fn.wglMakeCurrentFn = reinterpret_cast<wglMakeCurrentProc>(GetGLProcAddress("wglMakeCurrent"));
    fn.wglQueryPbufferARBFn = 0;
    fn.wglReleasePbufferDCARBFn = 0;
    fn.wglShareListsFn = reinterpret_cast<wglShareListsProc>(GetGLProcAddress("wglShareLists"));
    fn.wglSwapIntervalEXTFn = 0;
    fn.wglSwapLayerBuffersFn = reinterpret_cast<wglSwapLayerBuffersProc>(
        GetGLProcAddress("wglSwapLayerBuffers"));
}

void DriverWGL::InitializeExtensionBindings()
{
    std::string extensions(GetPlatformExtensions());
    extensions += " ";
    ALLOW_UNUSED_LOCAL(extensions);

    ext.b_WGL_ARB_extensions_string = extensions.find("WGL_ARB_extensions_string ") != std::string::npos;
    ext.b_WGL_ARB_pbuffer = extensions.find("WGL_ARB_pbuffer ") != std::string::npos;
    ext.b_WGL_ARB_pixel_format = extensions.find("WGL_ARB_pixel_format ") != std::string::npos;
    ext.b_WGL_EXT_extensions_string = extensions.find("WGL_EXT_extensions_string ") != std::string::npos;
    ext.b_WGL_EXT_swap_control = extensions.find("WGL_EXT_swap_control ") != std::string::npos;

    debug_fn.wglChoosePixelFormatARBFn = 0;
    if (ext.b_WGL_ARB_pixel_format) {
        fn.wglChoosePixelFormatARBFn = reinterpret_cast<wglChoosePixelFormatARBProc>(
            GetGLProcAddress("wglChoosePixelFormatARB"));
    }

    debug_fn.wglCreatePbufferARBFn = 0;
    if (ext.b_WGL_ARB_pbuffer) {
        fn.wglCreatePbufferARBFn = reinterpret_cast<wglCreatePbufferARBProc>(
            GetGLProcAddress("wglCreatePbufferARB"));
    }

    debug_fn.wglDestroyPbufferARBFn = 0;
    if (ext.b_WGL_ARB_pbuffer) {
        fn.wglDestroyPbufferARBFn = reinterpret_cast<wglDestroyPbufferARBProc>(
            GetGLProcAddress("wglDestroyPbufferARB"));
    }

    debug_fn.wglGetPbufferDCARBFn = 0;
    if (ext.b_WGL_ARB_pbuffer) {
        fn.wglGetPbufferDCARBFn = reinterpret_cast<wglGetPbufferDCARBProc>(
            GetGLProcAddress("wglGetPbufferDCARB"));
    }

    debug_fn.wglQueryPbufferARBFn = 0;
    if (ext.b_WGL_ARB_pbuffer) {
        fn.wglQueryPbufferARBFn = reinterpret_cast<wglQueryPbufferARBProc>(
            GetGLProcAddress("wglQueryPbufferARB"));
    }

    debug_fn.wglReleasePbufferDCARBFn = 0;
    if (ext.b_WGL_ARB_pbuffer) {
        fn.wglReleasePbufferDCARBFn = reinterpret_cast<wglReleasePbufferDCARBProc>(
            GetGLProcAddress("wglReleasePbufferDCARB"));
    }

    debug_fn.wglSwapIntervalEXTFn = 0;
    if (ext.b_WGL_EXT_swap_control) {
        fn.wglSwapIntervalEXTFn = reinterpret_cast<wglSwapIntervalEXTProc>(
            GetGLProcAddress("wglSwapIntervalEXT"));
    }

    if (g_debugBindingsInitialized)
        InitializeDebugBindings();
}

extern "C" {

static BOOL GL_BINDING_CALL
Debug_wglChoosePixelFormatARB(HDC dc,
    const int* int_attrib_list,
    const float* float_attrib_list,
    UINT max_formats,
    int* formats,
    UINT* num_formats)
{
    GL_SERVICE_LOG("wglChoosePixelFormatARB"
        << "(" << dc << ", "
        << static_cast<const void*>(int_attrib_list) << ", "
        << static_cast<const void*>(float_attrib_list) << ", "
        << max_formats << ", " << static_cast<const void*>(formats)
        << ", " << static_cast<const void*>(num_formats) << ")");
    BOOL result = g_driver_wgl.debug_fn.wglChoosePixelFormatARBFn(
        dc, int_attrib_list, float_attrib_list, max_formats, formats,
        num_formats);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static BOOL GL_BINDING_CALL Debug_wglCopyContext(HGLRC hglrcSrc,
    HGLRC hglrcDst,
    UINT mask)
{
    GL_SERVICE_LOG("wglCopyContext"
        << "(" << hglrcSrc << ", " << hglrcDst << ", " << mask << ")");
    BOOL result = g_driver_wgl.debug_fn.wglCopyContextFn(hglrcSrc, hglrcDst, mask);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static HGLRC GL_BINDING_CALL Debug_wglCreateContext(HDC hdc)
{
    GL_SERVICE_LOG("wglCreateContext"
        << "(" << hdc << ")");
    HGLRC result = g_driver_wgl.debug_fn.wglCreateContextFn(hdc);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static HGLRC GL_BINDING_CALL Debug_wglCreateLayerContext(HDC hdc,
    int iLayerPlane)
{
    GL_SERVICE_LOG("wglCreateLayerContext"
        << "(" << hdc << ", " << iLayerPlane << ")");
    HGLRC result = g_driver_wgl.debug_fn.wglCreateLayerContextFn(hdc, iLayerPlane);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static HPBUFFERARB GL_BINDING_CALL
Debug_wglCreatePbufferARB(HDC hDC,
    int iPixelFormat,
    int iWidth,
    int iHeight,
    const int* piAttribList)
{
    GL_SERVICE_LOG("wglCreatePbufferARB"
        << "(" << hDC << ", " << iPixelFormat << ", " << iWidth << ", "
        << iHeight << ", " << static_cast<const void*>(piAttribList)
        << ")");
    HPBUFFERARB result = g_driver_wgl.debug_fn.wglCreatePbufferARBFn(
        hDC, iPixelFormat, iWidth, iHeight, piAttribList);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static BOOL GL_BINDING_CALL Debug_wglDeleteContext(HGLRC hglrc)
{
    GL_SERVICE_LOG("wglDeleteContext"
        << "(" << hglrc << ")");
    BOOL result = g_driver_wgl.debug_fn.wglDeleteContextFn(hglrc);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static BOOL GL_BINDING_CALL Debug_wglDestroyPbufferARB(HPBUFFERARB hPbuffer)
{
    GL_SERVICE_LOG("wglDestroyPbufferARB"
        << "(" << hPbuffer << ")");
    BOOL result = g_driver_wgl.debug_fn.wglDestroyPbufferARBFn(hPbuffer);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static HGLRC GL_BINDING_CALL Debug_wglGetCurrentContext()
{
    GL_SERVICE_LOG("wglGetCurrentContext"
        << "("
        << ")");
    HGLRC result = g_driver_wgl.debug_fn.wglGetCurrentContextFn();
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static HDC GL_BINDING_CALL Debug_wglGetCurrentDC()
{
    GL_SERVICE_LOG("wglGetCurrentDC"
        << "("
        << ")");
    HDC result = g_driver_wgl.debug_fn.wglGetCurrentDCFn();
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static const char* GL_BINDING_CALL Debug_wglGetExtensionsStringARB(HDC hDC)
{
    GL_SERVICE_LOG("wglGetExtensionsStringARB"
        << "(" << hDC << ")");
    const char* result = g_driver_wgl.debug_fn.wglGetExtensionsStringARBFn(hDC);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static const char* GL_BINDING_CALL Debug_wglGetExtensionsStringEXT()
{
    GL_SERVICE_LOG("wglGetExtensionsStringEXT"
        << "("
        << ")");
    const char* result = g_driver_wgl.debug_fn.wglGetExtensionsStringEXTFn();
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static HDC GL_BINDING_CALL Debug_wglGetPbufferDCARB(HPBUFFERARB hPbuffer)
{
    GL_SERVICE_LOG("wglGetPbufferDCARB"
        << "(" << hPbuffer << ")");
    HDC result = g_driver_wgl.debug_fn.wglGetPbufferDCARBFn(hPbuffer);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static BOOL GL_BINDING_CALL Debug_wglMakeCurrent(HDC hdc, HGLRC hglrc)
{
    GL_SERVICE_LOG("wglMakeCurrent"
        << "(" << hdc << ", " << hglrc << ")");
    BOOL result = g_driver_wgl.debug_fn.wglMakeCurrentFn(hdc, hglrc);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static BOOL GL_BINDING_CALL Debug_wglQueryPbufferARB(HPBUFFERARB hPbuffer,
    int iAttribute,
    int* piValue)
{
    GL_SERVICE_LOG("wglQueryPbufferARB"
        << "(" << hPbuffer << ", " << iAttribute << ", "
        << static_cast<const void*>(piValue) << ")");
    BOOL result = g_driver_wgl.debug_fn.wglQueryPbufferARBFn(hPbuffer, iAttribute, piValue);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static int GL_BINDING_CALL Debug_wglReleasePbufferDCARB(HPBUFFERARB hPbuffer,
    HDC hDC)
{
    GL_SERVICE_LOG("wglReleasePbufferDCARB"
        << "(" << hPbuffer << ", " << hDC << ")");
    int result = g_driver_wgl.debug_fn.wglReleasePbufferDCARBFn(hPbuffer, hDC);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static BOOL GL_BINDING_CALL Debug_wglShareLists(HGLRC hglrc1, HGLRC hglrc2)
{
    GL_SERVICE_LOG("wglShareLists"
        << "(" << hglrc1 << ", " << hglrc2 << ")");
    BOOL result = g_driver_wgl.debug_fn.wglShareListsFn(hglrc1, hglrc2);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static BOOL GL_BINDING_CALL Debug_wglSwapIntervalEXT(int interval)
{
    GL_SERVICE_LOG("wglSwapIntervalEXT"
        << "(" << interval << ")");
    BOOL result = g_driver_wgl.debug_fn.wglSwapIntervalEXTFn(interval);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}

static BOOL GL_BINDING_CALL Debug_wglSwapLayerBuffers(HDC hdc, UINT fuPlanes)
{
    GL_SERVICE_LOG("wglSwapLayerBuffers"
        << "(" << hdc << ", " << fuPlanes << ")");
    BOOL result = g_driver_wgl.debug_fn.wglSwapLayerBuffersFn(hdc, fuPlanes);
    GL_SERVICE_LOG("GL_RESULT: " << result);
    return result;
}
} // extern "C"

void DriverWGL::InitializeDebugBindings()
{
    if (!debug_fn.wglChoosePixelFormatARBFn) {
        debug_fn.wglChoosePixelFormatARBFn = fn.wglChoosePixelFormatARBFn;
        fn.wglChoosePixelFormatARBFn = Debug_wglChoosePixelFormatARB;
    }
    if (!debug_fn.wglCopyContextFn) {
        debug_fn.wglCopyContextFn = fn.wglCopyContextFn;
        fn.wglCopyContextFn = Debug_wglCopyContext;
    }
    if (!debug_fn.wglCreateContextFn) {
        debug_fn.wglCreateContextFn = fn.wglCreateContextFn;
        fn.wglCreateContextFn = Debug_wglCreateContext;
    }
    if (!debug_fn.wglCreateLayerContextFn) {
        debug_fn.wglCreateLayerContextFn = fn.wglCreateLayerContextFn;
        fn.wglCreateLayerContextFn = Debug_wglCreateLayerContext;
    }
    if (!debug_fn.wglCreatePbufferARBFn) {
        debug_fn.wglCreatePbufferARBFn = fn.wglCreatePbufferARBFn;
        fn.wglCreatePbufferARBFn = Debug_wglCreatePbufferARB;
    }
    if (!debug_fn.wglDeleteContextFn) {
        debug_fn.wglDeleteContextFn = fn.wglDeleteContextFn;
        fn.wglDeleteContextFn = Debug_wglDeleteContext;
    }
    if (!debug_fn.wglDestroyPbufferARBFn) {
        debug_fn.wglDestroyPbufferARBFn = fn.wglDestroyPbufferARBFn;
        fn.wglDestroyPbufferARBFn = Debug_wglDestroyPbufferARB;
    }
    if (!debug_fn.wglGetCurrentContextFn) {
        debug_fn.wglGetCurrentContextFn = fn.wglGetCurrentContextFn;
        fn.wglGetCurrentContextFn = Debug_wglGetCurrentContext;
    }
    if (!debug_fn.wglGetCurrentDCFn) {
        debug_fn.wglGetCurrentDCFn = fn.wglGetCurrentDCFn;
        fn.wglGetCurrentDCFn = Debug_wglGetCurrentDC;
    }
    if (!debug_fn.wglGetExtensionsStringARBFn) {
        debug_fn.wglGetExtensionsStringARBFn = fn.wglGetExtensionsStringARBFn;
        fn.wglGetExtensionsStringARBFn = Debug_wglGetExtensionsStringARB;
    }
    if (!debug_fn.wglGetExtensionsStringEXTFn) {
        debug_fn.wglGetExtensionsStringEXTFn = fn.wglGetExtensionsStringEXTFn;
        fn.wglGetExtensionsStringEXTFn = Debug_wglGetExtensionsStringEXT;
    }
    if (!debug_fn.wglGetPbufferDCARBFn) {
        debug_fn.wglGetPbufferDCARBFn = fn.wglGetPbufferDCARBFn;
        fn.wglGetPbufferDCARBFn = Debug_wglGetPbufferDCARB;
    }
    if (!debug_fn.wglMakeCurrentFn) {
        debug_fn.wglMakeCurrentFn = fn.wglMakeCurrentFn;
        fn.wglMakeCurrentFn = Debug_wglMakeCurrent;
    }
    if (!debug_fn.wglQueryPbufferARBFn) {
        debug_fn.wglQueryPbufferARBFn = fn.wglQueryPbufferARBFn;
        fn.wglQueryPbufferARBFn = Debug_wglQueryPbufferARB;
    }
    if (!debug_fn.wglReleasePbufferDCARBFn) {
        debug_fn.wglReleasePbufferDCARBFn = fn.wglReleasePbufferDCARBFn;
        fn.wglReleasePbufferDCARBFn = Debug_wglReleasePbufferDCARB;
    }
    if (!debug_fn.wglShareListsFn) {
        debug_fn.wglShareListsFn = fn.wglShareListsFn;
        fn.wglShareListsFn = Debug_wglShareLists;
    }
    if (!debug_fn.wglSwapIntervalEXTFn) {
        debug_fn.wglSwapIntervalEXTFn = fn.wglSwapIntervalEXTFn;
        fn.wglSwapIntervalEXTFn = Debug_wglSwapIntervalEXT;
    }
    if (!debug_fn.wglSwapLayerBuffersFn) {
        debug_fn.wglSwapLayerBuffersFn = fn.wglSwapLayerBuffersFn;
        fn.wglSwapLayerBuffersFn = Debug_wglSwapLayerBuffers;
    }
    g_debugBindingsInitialized = true;
}

void DriverWGL::ClearBindings()
{
    memset(this, 0, sizeof(*this));
}

BOOL WGLApiBase::wglChoosePixelFormatARBFn(HDC dc,
    const int* int_attrib_list,
    const float* float_attrib_list,
    UINT max_formats,
    int* formats,
    UINT* num_formats)
{
    return driver_->fn.wglChoosePixelFormatARBFn(dc, int_attrib_list,
        float_attrib_list, max_formats,
        formats, num_formats);
}

BOOL WGLApiBase::wglCopyContextFn(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask)
{
    return driver_->fn.wglCopyContextFn(hglrcSrc, hglrcDst, mask);
}

HGLRC WGLApiBase::wglCreateContextFn(HDC hdc)
{
    return driver_->fn.wglCreateContextFn(hdc);
}

HGLRC WGLApiBase::wglCreateLayerContextFn(HDC hdc, int iLayerPlane)
{
    return driver_->fn.wglCreateLayerContextFn(hdc, iLayerPlane);
}

HPBUFFERARB WGLApiBase::wglCreatePbufferARBFn(HDC hDC,
    int iPixelFormat,
    int iWidth,
    int iHeight,
    const int* piAttribList)
{
    return driver_->fn.wglCreatePbufferARBFn(hDC, iPixelFormat, iWidth, iHeight,
        piAttribList);
}

BOOL WGLApiBase::wglDeleteContextFn(HGLRC hglrc)
{
    return driver_->fn.wglDeleteContextFn(hglrc);
}

BOOL WGLApiBase::wglDestroyPbufferARBFn(HPBUFFERARB hPbuffer)
{
    return driver_->fn.wglDestroyPbufferARBFn(hPbuffer);
}

HGLRC WGLApiBase::wglGetCurrentContextFn()
{
    return driver_->fn.wglGetCurrentContextFn();
}

HDC WGLApiBase::wglGetCurrentDCFn()
{
    return driver_->fn.wglGetCurrentDCFn();
}

const char* WGLApiBase::wglGetExtensionsStringARBFn(HDC hDC)
{
    return driver_->fn.wglGetExtensionsStringARBFn(hDC);
}

const char* WGLApiBase::wglGetExtensionsStringEXTFn()
{
    return driver_->fn.wglGetExtensionsStringEXTFn();
}

HDC WGLApiBase::wglGetPbufferDCARBFn(HPBUFFERARB hPbuffer)
{
    return driver_->fn.wglGetPbufferDCARBFn(hPbuffer);
}

BOOL WGLApiBase::wglMakeCurrentFn(HDC hdc, HGLRC hglrc)
{
    return driver_->fn.wglMakeCurrentFn(hdc, hglrc);
}

BOOL WGLApiBase::wglQueryPbufferARBFn(HPBUFFERARB hPbuffer,
    int iAttribute,
    int* piValue)
{
    return driver_->fn.wglQueryPbufferARBFn(hPbuffer, iAttribute, piValue);
}

int WGLApiBase::wglReleasePbufferDCARBFn(HPBUFFERARB hPbuffer, HDC hDC)
{
    return driver_->fn.wglReleasePbufferDCARBFn(hPbuffer, hDC);
}

BOOL WGLApiBase::wglShareListsFn(HGLRC hglrc1, HGLRC hglrc2)
{
    return driver_->fn.wglShareListsFn(hglrc1, hglrc2);
}

BOOL WGLApiBase::wglSwapIntervalEXTFn(int interval)
{
    return driver_->fn.wglSwapIntervalEXTFn(interval);
}

BOOL WGLApiBase::wglSwapLayerBuffersFn(HDC hdc, UINT fuPlanes)
{
    return driver_->fn.wglSwapLayerBuffersFn(hdc, fuPlanes);
}

BOOL TraceWGLApi::wglChoosePixelFormatARBFn(HDC dc,
    const int* int_attrib_list,
    const float* float_attrib_list,
    UINT max_formats,
    int* formats,
    UINT* num_formats)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglChoosePixelFormatARB")
    return wgl_api_->wglChoosePixelFormatARBFn(dc, int_attrib_list,
        float_attrib_list, max_formats,
        formats, num_formats);
}

BOOL TraceWGLApi::wglCopyContextFn(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglCopyContext")
    return wgl_api_->wglCopyContextFn(hglrcSrc, hglrcDst, mask);
}

HGLRC TraceWGLApi::wglCreateContextFn(HDC hdc)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglCreateContext")
    return wgl_api_->wglCreateContextFn(hdc);
}

HGLRC TraceWGLApi::wglCreateLayerContextFn(HDC hdc, int iLayerPlane)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglCreateLayerContext")
    return wgl_api_->wglCreateLayerContextFn(hdc, iLayerPlane);
}

HPBUFFERARB TraceWGLApi::wglCreatePbufferARBFn(HDC hDC,
    int iPixelFormat,
    int iWidth,
    int iHeight,
    const int* piAttribList)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglCreatePbufferARB")
    return wgl_api_->wglCreatePbufferARBFn(hDC, iPixelFormat, iWidth, iHeight,
        piAttribList);
}

BOOL TraceWGLApi::wglDeleteContextFn(HGLRC hglrc)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglDeleteContext")
    return wgl_api_->wglDeleteContextFn(hglrc);
}

BOOL TraceWGLApi::wglDestroyPbufferARBFn(HPBUFFERARB hPbuffer)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglDestroyPbufferARB")
    return wgl_api_->wglDestroyPbufferARBFn(hPbuffer);
}

HGLRC TraceWGLApi::wglGetCurrentContextFn()
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglGetCurrentContext")
    return wgl_api_->wglGetCurrentContextFn();
}

HDC TraceWGLApi::wglGetCurrentDCFn()
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglGetCurrentDC")
    return wgl_api_->wglGetCurrentDCFn();
}

const char* TraceWGLApi::wglGetExtensionsStringARBFn(HDC hDC)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglGetExtensionsStringARB")
    return wgl_api_->wglGetExtensionsStringARBFn(hDC);
}

const char* TraceWGLApi::wglGetExtensionsStringEXTFn()
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglGetExtensionsStringEXT")
    return wgl_api_->wglGetExtensionsStringEXTFn();
}

HDC TraceWGLApi::wglGetPbufferDCARBFn(HPBUFFERARB hPbuffer)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglGetPbufferDCARB")
    return wgl_api_->wglGetPbufferDCARBFn(hPbuffer);
}

BOOL TraceWGLApi::wglMakeCurrentFn(HDC hdc, HGLRC hglrc)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglMakeCurrent")
    return wgl_api_->wglMakeCurrentFn(hdc, hglrc);
}

BOOL TraceWGLApi::wglQueryPbufferARBFn(HPBUFFERARB hPbuffer,
    int iAttribute,
    int* piValue)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglQueryPbufferARB")
    return wgl_api_->wglQueryPbufferARBFn(hPbuffer, iAttribute, piValue);
}

int TraceWGLApi::wglReleasePbufferDCARBFn(HPBUFFERARB hPbuffer, HDC hDC)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglReleasePbufferDCARB")
    return wgl_api_->wglReleasePbufferDCARBFn(hPbuffer, hDC);
}

BOOL TraceWGLApi::wglShareListsFn(HGLRC hglrc1, HGLRC hglrc2)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglShareLists")
    return wgl_api_->wglShareListsFn(hglrc1, hglrc2);
}

BOOL TraceWGLApi::wglSwapIntervalEXTFn(int interval)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglSwapIntervalEXT")
    return wgl_api_->wglSwapIntervalEXTFn(interval);
}

BOOL TraceWGLApi::wglSwapLayerBuffersFn(HDC hdc, UINT fuPlanes)
{
    TRACE_EVENT_BINARY_EFFICIENT0("gpu", "TraceGLAPI::wglSwapLayerBuffers")
    return wgl_api_->wglSwapLayerBuffersFn(hdc, fuPlanes);
}

} // namespace gfx
