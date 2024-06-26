// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file implements the GLContextWGL and PbufferGLContext classes.

#include "ui/gl/gl_context_wgl.h"

#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface_wgl.h"

namespace gfx {

GLContextWGL::GLContextWGL(GLShareGroup* share_group)
    : GLContextReal(share_group)
    , context_(nullptr)
{
}

bool GLContextWGL::Initialize(
    GLSurface* compatible_surface, GpuPreference gpu_preference)
{
    // Get the handle of another initialized context in the share group _before_
    // setting context_. Otherwise this context will be considered initialized
    // and could potentially be returned by GetHandle.
    HGLRC share_handle = static_cast<HGLRC>(share_group()->GetHandle());

    context_ = wglCreateContext(
        static_cast<HDC>(compatible_surface->GetHandle()));
    if (!context_) {
        LOG(ERROR) << "Failed to create GL context.";
        Destroy();
        return false;
    }

    if (share_handle) {
        if (!wglShareLists(share_handle, context_)) {
            LOG(ERROR) << "Could not share GL contexts.";
            Destroy();
            return false;
        }
    }

    return true;
}

void GLContextWGL::Destroy()
{
    if (context_) {
        wglDeleteContext(context_);
        context_ = nullptr;
    }
}

bool GLContextWGL::MakeCurrent(GLSurface* surface)
{
    DCHECK(context_);
    if (IsCurrent(surface))
        return true;

    ScopedReleaseCurrent release_current;
    TRACE_EVENT0("gpu", "GLContextWGL::MakeCurrent");

    if (!wglMakeCurrent(static_cast<HDC>(surface->GetHandle()), context_)) {
        LOG(ERROR) << "Unable to make gl context current.";
        return false;
    }

    // Set this as soon as the context is current, since we might call into GL.
    SetRealGLApi();

    SetCurrent(surface);
    if (!InitializeDynamicBindings()) {
        return false;
    }

    if (!surface->OnMakeCurrent(this)) {
        LOG(ERROR) << "Could not make current.";
        return false;
    }

    release_current.Cancel();
    return true;
}

void GLContextWGL::ReleaseCurrent(GLSurface* surface)
{
    if (!IsCurrent(surface))
        return;

    SetCurrent(nullptr);
    wglMakeCurrent(nullptr, nullptr);
}

bool GLContextWGL::IsCurrent(GLSurface* surface)
{
    bool native_context_is_current = wglGetCurrentContext() == context_;

    // If our context is current then our notion of which GLContext is
    // current must be correct. On the other hand, third-party code
    // using OpenGL might change the current context.
    DCHECK(!native_context_is_current || (GetRealCurrent() == this));

    if (!native_context_is_current)
        return false;

    if (surface) {
        if (wglGetCurrentDC() != surface->GetHandle())
            return false;
    }

    return true;
}

void* GLContextWGL::GetHandle()
{
    return context_;
}

void GLContextWGL::OnSetSwapInterval(int interval)
{
    DCHECK(IsCurrent(nullptr));
    if (gfx::g_driver_wgl.ext.b_WGL_EXT_swap_control) {
        wglSwapIntervalEXT(interval);
    } else {
        LOG(WARNING) << "Could not disable vsync: driver does not "
                        "support WGL_EXT_swap_control";
    }
}

std::string GLContextWGL::GetExtensions()
{
    const char* extensions = nullptr;
    if (g_driver_wgl.fn.wglGetExtensionsStringARBFn)
        extensions = wglGetExtensionsStringARB(GLSurfaceWGL::GetDisplayDC());
    else if (g_driver_wgl.fn.wglGetExtensionsStringEXTFn)
        extensions = wglGetExtensionsStringEXT();

    if (extensions)
        return GLContext::GetExtensions() + " " + extensions;

    return GLContext::GetExtensions();
}

GLContextWGL::~GLContextWGL()
{
    Destroy();
}

} // namespace gfx
