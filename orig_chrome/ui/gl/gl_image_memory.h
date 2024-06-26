// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_IMAGE_MEMORY_H_
#define UI_GL_GL_IMAGE_MEMORY_H_

#include "ui/gl/gl_image.h"

#include "base/numerics/safe_math.h"
#include "ui/gfx/buffer_types.h"

namespace gl {

class GL_EXPORT GLImageMemory : public GLImage {
public:
    GLImageMemory(const gfx::Size& size, unsigned internalformat);

    bool Initialize(const unsigned char* memory,
        gfx::BufferFormat format,
        size_t stride);

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

    static unsigned GetInternalFormatForTesting(gfx::BufferFormat format);

protected:
    ~GLImageMemory() override;

    gfx::BufferFormat format() const { return format_; }
    size_t stride() const { return stride_; }

private:
    const gfx::Size size_;
    const unsigned internalformat_;
    const unsigned char* memory_;
    gfx::BufferFormat format_;
    size_t stride_;

    DISALLOW_COPY_AND_ASSIGN(GLImageMemory);
};

} // namespace gl

#endif // UI_GL_GL_IMAGE_MEMORY_H_
