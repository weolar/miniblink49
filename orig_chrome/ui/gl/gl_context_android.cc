// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_context.h"

#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/sys_info.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context_egl.h"
#include "ui/gl/gl_context_osmesa.h"
#include "ui/gl/gl_context_stub.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface.h"

namespace gfx {

namespace {

    // Used to render into an already current context+surface,
    // that we do not have ownership of (draw callback).
    // TODO(boliu): Make this inherit from GLContextEGL.
    class GLNonOwnedContext : public GLContextReal {
    public:
        GLNonOwnedContext(GLShareGroup* share_group);

        // Implement GLContext.
        bool Initialize(GLSurface* compatible_surface,
            GpuPreference gpu_preference) override;
        bool MakeCurrent(GLSurface* surface) override;
        void ReleaseCurrent(GLSurface* surface) override { }
        bool IsCurrent(GLSurface* surface) override { return true; }
        void* GetHandle() override { return nullptr; }
        void OnSetSwapInterval(int interval) override { }
        std::string GetExtensions() override;

    protected:
        ~GLNonOwnedContext() override { }

    private:
        EGLDisplay display_;

        DISALLOW_COPY_AND_ASSIGN(GLNonOwnedContext);
    };

    GLNonOwnedContext::GLNonOwnedContext(GLShareGroup* share_group)
        : GLContextReal(share_group)
        , display_(nullptr)
    {
    }

    bool GLNonOwnedContext::Initialize(GLSurface* compatible_surface,
        GpuPreference gpu_preference)
    {
        display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        return true;
    }

    bool GLNonOwnedContext::MakeCurrent(GLSurface* surface)
    {
        SetCurrent(surface);
        SetRealGLApi();
        return true;
    }

    std::string GLNonOwnedContext::GetExtensions()
    {
        const char* extensions = eglQueryString(display_, EGL_EXTENSIONS);
        if (!extensions)
            return GLContext::GetExtensions();

        return GLContext::GetExtensions() + " " + extensions;
    }

} // anonymous namespace

// static
scoped_refptr<GLContext> GLContext::CreateGLContext(
    GLShareGroup* share_group,
    GLSurface* compatible_surface,
    GpuPreference gpu_preference)
{
    scoped_refptr<GLContext> context;
    switch (GetGLImplementation()) {
    case kGLImplementationMockGL:
        return scoped_refptr<GLContext>(new GLContextStub());
    case kGLImplementationOSMesaGL:
        context = new GLContextOSMesa(share_group);
        break;
    default:
        if (compatible_surface->GetHandle())
            context = new GLContextEGL(share_group);
        else
            context = new GLNonOwnedContext(share_group);
        break;
    }

    if (!context->Initialize(compatible_surface, gpu_preference))
        return nullptr;

    return context;
}

}
