// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(OS_WIN) || defined(USE_X11) || defined(OS_ANDROID) || defined(USE_OZONE)
#include <EGL/egl.h>
#endif

#include "ui/gl/gl_bindings.h"

#if defined(USE_X11)
#include "ui/gfx/x/x11_types.h"
#endif

#if defined(OS_WIN)
#include "ui/gl/gl_surface_wgl.h"
#endif

#if defined(OS_WIN) || defined(USE_X11) || defined(OS_ANDROID) || defined(USE_OZONE)
#include "ui/gl/gl_surface_egl.h"
#endif

namespace gfx {

std::string DriverOSMESA::GetPlatformExtensions()
{
    return "";
}

#if defined(OS_WIN)
std::string DriverWGL::GetPlatformExtensions()
{
    //   const char* str = nullptr;
    //   str = wglGetExtensionsStringARB(GLSurfaceWGL::GetDisplayDC());
    //   if (str)
    //     return str;
    //   return wglGetExtensionsStringEXT();
    DebugBreak();
    return "";
}
#endif

#if defined(OS_WIN) || defined(USE_X11) || defined(OS_ANDROID) || defined(USE_OZONE)
std::string DriverEGL::GetPlatformExtensions()
{
    EGLDisplay display = GLSurfaceEGL::InitializeDisplay();
    if (display == EGL_NO_DISPLAY)
        return "";
    const char* str = eglQueryString(display, EGL_EXTENSIONS);
    return str ? std::string(str) : "";
}

// static
std::string DriverEGL::GetClientExtensions()
{
    const char* str = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    return str ? std::string(str) : "";
}
#endif

#if defined(USE_X11)
std::string DriverGLX::GetPlatformExtensions()
{
    const char* str = glXQueryExtensionsString(gfx::GetXDisplay(), 0);
    return str ? std::string(str) : "";
}
#endif

} // namespace gfx
