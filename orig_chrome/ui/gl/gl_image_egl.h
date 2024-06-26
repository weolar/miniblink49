// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_IMAGE_EGL_H_
#define UI_GL_GL_IMAGE_EGL_H_

#include "base/threading/thread_checker.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_image.h"

namespace gl {

class GL_EXPORT GLImageEGL : public GLImage {
public:
    explicit GLImageEGL(const gfx::Size& size);

    bool Initialize(EGLenum target, EGLClientBuffer buffer, const EGLint* attrs);

    // Overridden from GLImage:
    void Destroy(bool have_context) override;
    gfx::Size GetSize() override;
    unsigned GetInternalFormat() override;
    bool BindTexImage(unsigned target) override;
    void ReleaseTexImage(unsigned target) override { }
    bool CopyTexImage(unsigned target) override;
    bool CopyTexSubImage(unsigned target,
        const gfx::Point& offset,
        const gfx::Rect& rect) override;
    bool ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
        int z_order,
        gfx::OverlayTransform transform,
        const gfx::Rect& bounds_rect,
        const gfx::RectF& crop_rect) override;

protected:
    ~GLImageEGL() override;

    EGLImageKHR egl_image_;
    const gfx::Size size_;
    base::ThreadChecker thread_checker_;

private:
    DISALLOW_COPY_AND_ASSIGN(GLImageEGL);
};

} // namespace gl

#endif // UI_GL_GL_IMAGE_EGL_H_
