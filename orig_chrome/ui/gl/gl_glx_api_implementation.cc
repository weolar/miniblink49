// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_glx_api_implementation.h"

#include "base/command_line.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "ui/gl/gl_implementation.h"

namespace gfx {

RealGLXApi* g_real_glx;

void InitializeStaticGLBindingsGLX()
{
    g_driver_glx.InitializeStaticBindings();
    if (!g_real_glx) {
        g_real_glx = new RealGLXApi();
    }
    g_real_glx->Initialize(&g_driver_glx);
    g_current_glx_context = g_real_glx;
    g_driver_glx.InitializeExtensionBindings();
}

void InitializeDebugGLBindingsGLX()
{
    g_driver_glx.InitializeDebugBindings();
}

void ClearGLBindingsGLX()
{
    if (g_real_glx) {
        delete g_real_glx;
        g_real_glx = NULL;
    }
    g_current_glx_context = NULL;
    g_driver_glx.ClearBindings();
}

GLXApi::GLXApi()
{
}

GLXApi::~GLXApi()
{
}

GLXApiBase::GLXApiBase()
    : driver_(NULL)
{
}

GLXApiBase::~GLXApiBase()
{
}

void GLXApiBase::InitializeBase(DriverGLX* driver)
{
    driver_ = driver;
}

RealGLXApi::RealGLXApi()
{
}

RealGLXApi::~RealGLXApi()
{
}

void RealGLXApi::Initialize(DriverGLX* driver)
{
    InitializeWithCommandLine(driver, base::CommandLine::ForCurrentProcess());
}

void RealGLXApi::InitializeWithCommandLine(DriverGLX* driver,
    base::CommandLine* command_line)
{
    DCHECK(command_line);
    InitializeBase(driver);

    const std::string disabled_extensions = command_line->GetSwitchValueASCII(
        switches::kDisableGLExtensions);
    disabled_exts_.clear();
    filtered_exts_ = "";
    if (!disabled_extensions.empty()) {
        disabled_exts_ = base::SplitString(disabled_extensions, ", ;",
            base::KEEP_WHITESPACE,
            base::SPLIT_WANT_NONEMPTY);
    }
}

const char* RealGLXApi::glXQueryExtensionsStringFn(Display* dpy,
    int screen)
{
    if (filtered_exts_.size())
        return filtered_exts_.c_str();

    if (!driver_->fn.glXQueryExtensionsStringFn)
        return NULL;

    const char* str = GLXApiBase::glXQueryExtensionsStringFn(dpy, screen);
    if (!str)
        return NULL;

    filtered_exts_ = FilterGLExtensionList(str, disabled_exts_);
    return filtered_exts_.c_str();
}

TraceGLXApi::~TraceGLXApi()
{
}

bool GetGLWindowSystemBindingInfoGLX(GLWindowSystemBindingInfo* info)
{
    Display* display = glXGetCurrentDisplay();
    const int kDefaultScreen = 0;
    const char* vendor = glXQueryServerString(display, kDefaultScreen, GLX_VENDOR);
    const char* version = glXQueryServerString(display, kDefaultScreen, GLX_VERSION);
    const char* extensions = glXQueryServerString(display, kDefaultScreen, GLX_EXTENSIONS);
    *info = GLWindowSystemBindingInfo();
    if (vendor)
        info->vendor = vendor;
    if (version)
        info->version = version;
    if (extensions)
        info->extensions = extensions;
    info->direct_rendering = !!glXIsDirect(display, glXGetCurrentContext());
    return true;
}

} // namespace gfx
