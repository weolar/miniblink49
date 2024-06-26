// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_image_egl.h"

#include "ui/gl/egl_util.h"
#include "ui/gl/gl_surface_egl.h"

namespace gl {

GLImageEGL::GLImageEGL(const gfx::Size& size)
    : egl_image_(EGL_NO_IMAGE_KHR)
    , size_(size)
{
}

GLImageEGL::~GLImageEGL()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK_EQ(EGL_NO_IMAGE_KHR, egl_image_);
}

bool GLImageEGL::Initialize(EGLenum target,
    EGLClientBuffer buffer,
    const EGLint* attrs)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK_EQ(EGL_NO_IMAGE_KHR, egl_image_);
    egl_image_ = eglCreateImageKHR(gfx::GLSurfaceEGL::GetHardwareDisplay(),
        EGL_NO_CONTEXT,
        target,
        buffer,
        attrs);
    if (egl_image_ == EGL_NO_IMAGE_KHR) {
        DLOG(ERROR) << "Error creating EGLImage: " << ui::GetLastEGLErrorString();
        return false;
    }

    return true;
}

void GLImageEGL::Destroy(bool have_context)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    if (egl_image_ != EGL_NO_IMAGE_KHR) {
        EGLBoolean result = eglDestroyImageKHR(gfx::GLSurfaceEGL::GetHardwareDisplay(), egl_image_);
        if (result == EGL_FALSE) {
            DLOG(ERROR) << "Error destroying EGLImage: "
                        << ui::GetLastEGLErrorString();
        }
        egl_image_ = EGL_NO_IMAGE_KHR;
    }
}

gfx::Size GLImageEGL::GetSize()
{
    return size_;
}

unsigned GLImageEGL::GetInternalFormat() { return GL_RGBA; }

bool GLImageEGL::BindTexImage(unsigned target)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK_NE(EGL_NO_IMAGE_KHR, egl_image_);
    glEGLImageTargetTexture2DOES(target, egl_image_);
    DCHECK_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
    return true;
}

bool GLImageEGL::CopyTexImage(unsigned target)
{
    return false;
}

bool GLImageEGL::CopyTexSubImage(unsigned target,
    const gfx::Point& offset,
    const gfx::Rect& rect)
{
    return false;
}

bool GLImageEGL::ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
    int z_order,
    gfx::OverlayTransform transform,
    const gfx::Rect& bounds_rect,
    const gfx::RectF& crop_rect)
{
    return false;
}

} // namespace gl
