// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_context_osmesa.h"

#include <GL/osmesa.h>

#include "base/logging.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_surface.h"

namespace gfx {

GLContextOSMesa::GLContextOSMesa(GLShareGroup* share_group)
    : GLContextReal(share_group)
    , context_(nullptr)
    , is_released_(false)
{
}

bool GLContextOSMesa::Initialize(GLSurface* compatible_surface,
    GpuPreference gpu_preference)
{
    DCHECK(!context_);

    OSMesaContext share_handle = static_cast<OSMesaContext>(
        share_group() ? share_group()->GetHandle() : nullptr);

    GLuint format = compatible_surface->GetFormat();
    DCHECK_NE(format, (unsigned)0);
    context_ = OSMesaCreateContextExt(format,
        0, // depth bits
        0, // stencil bits
        0, // accum bits
        share_handle);
    if (!context_) {
        LOG(ERROR) << "OSMesaCreateContextExt failed.";
        return false;
    }

    return true;
}

void GLContextOSMesa::Destroy()
{
    if (context_) {
        OSMesaDestroyContext(static_cast<OSMesaContext>(context_));
        context_ = nullptr;
    }
}

bool GLContextOSMesa::MakeCurrent(GLSurface* surface)
{
    DCHECK(context_);

    gfx::Size size = surface->GetSize();

    ScopedReleaseCurrent release_current;
    if (!OSMesaMakeCurrent(context_,
            surface->GetHandle(),
            GL_UNSIGNED_BYTE,
            size.width(),
            size.height())) {
        LOG(ERROR) << "OSMesaMakeCurrent failed.";
        Destroy();
        return false;
    }
    // Track that we're no longer in a released state to workaround a mesa issue.
    is_released_ = false;

    // Set this as soon as the context is current, since we might call into GL.
    SetRealGLApi();

    // Row 0 is at the top.
    OSMesaPixelStore(OSMESA_Y_UP, 0);

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

void GLContextOSMesa::ReleaseCurrent(GLSurface* surface)
{
    if (!IsCurrent(surface))
        return;

    SetCurrent(nullptr);

    // Calling |OSMesaMakeCurrent| with nullptr here does not work to release the
    // context. As a workaround, keep track of the release state, so that we can
    // correctly determine the state of |IsCurrent|.
    is_released_ = true;
    OSMesaMakeCurrent(nullptr, nullptr, GL_UNSIGNED_BYTE, 0, 0);
}

bool GLContextOSMesa::IsCurrent(GLSurface* surface)
{
    DCHECK(context_);

    // |OSMesaGetCurrentContext| doesn't correctly return nullptr when we release,
    // check the tracked |is_released_|. See |ReleaseCurrent|.
    bool native_context_is_current = !is_released_ && context_ == OSMesaGetCurrentContext();

    // If our context is current then our notion of which GLContext is
    // current must be correct. On the other hand, third-party code
    // using OpenGL might change the current context.
    DCHECK(!native_context_is_current || (GetRealCurrent() == this));

    if (!native_context_is_current)
        return false;

    if (surface) {
        GLint width;
        GLint height;
        GLint format;
        void* buffer = nullptr;
        OSMesaGetColorBuffer(context_, &width, &height, &format, &buffer);
        if (buffer != surface->GetHandle())
            return false;
    }

    return true;
}

void* GLContextOSMesa::GetHandle()
{
    return context_;
}

void GLContextOSMesa::OnSetSwapInterval(int interval)
{
    DCHECK(IsCurrent(nullptr));
}

GLContextOSMesa::~GLContextOSMesa()
{
    Destroy();
}

} // namespace gfx
