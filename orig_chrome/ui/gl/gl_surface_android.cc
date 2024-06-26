// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_surface.h"

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "third_party/khronos/EGL/egl.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/gl/gl_surface_osmesa.h"
#include "ui/gl/gl_surface_stub.h"

namespace gfx {

// static
bool GLSurface::InitializeOneOffInternal()
{
    switch (GetGLImplementation()) {
    case kGLImplementationEGLGLES2:
        if (!GLSurfaceEGL::InitializeOneOff()) {
            LOG(ERROR) << "GLSurfaceEGL::InitializeOneOff failed.";
            return false;
        }
    default:
        break;
    }
    return true;
}

// static
scoped_refptr<GLSurface> GLSurface::CreateViewGLSurface(
    gfx::AcceleratedWidget window)
{
    CHECK_NE(kGLImplementationNone, GetGLImplementation());
    if (GetGLImplementation() == kGLImplementationOSMesaGL) {
        scoped_refptr<GLSurface> surface(new GLSurfaceOSMesaHeadless());
        if (!surface->Initialize())
            return NULL;
        return surface;
    }
    DCHECK(GetGLImplementation() == kGLImplementationEGLGLES2);
    if (window != kNullAcceleratedWidget) {
        scoped_refptr<GLSurface> surface = new NativeViewGLSurfaceEGL(window);
        if (surface->Initialize())
            return surface;
    } else {
        scoped_refptr<GLSurface> surface = new GLSurfaceStub();
        if (surface->Initialize())
            return surface;
    }
    return NULL;
}

// static
scoped_refptr<GLSurface> GLSurface::CreateOffscreenGLSurface(
    const gfx::Size& size,
    GLSurface::Format format)
{
    CHECK_NE(kGLImplementationNone, GetGLImplementation());
    switch (GetGLImplementation()) {
    case kGLImplementationOSMesaGL: {
        scoped_refptr<GLSurface> surface(
            new GLSurfaceOSMesa(OSMesaSurfaceFormatBGRA, size));
        if (!surface->Initialize(format))
            return NULL;

        return surface;
    }
    case kGLImplementationEGLGLES2: {
        scoped_refptr<GLSurface> surface;
        if (GLSurfaceEGL::IsEGLSurfacelessContextSupported() && (size.width() == 0 && size.height() == 0)) {
            surface = new SurfacelessEGL(size);
        } else {
            surface = new PbufferGLSurfaceEGL(size);
        }

        if (!surface->Initialize(format))
            return NULL;
        return surface;
    }
    case kGLImplementationMockGL:
        return new GLSurfaceStub;
    default:
        NOTREACHED();
        return NULL;
    }
}

EGLNativeDisplayType GetPlatformDefaultEGLNativeDisplay()
{
    return EGL_DEFAULT_DISPLAY;
}

} // namespace gfx
