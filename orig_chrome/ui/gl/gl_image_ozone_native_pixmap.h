// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_IMAGE_OZONE_NATIVE_PIXMAP_H_
#define UI_GL_GL_IMAGE_OZONE_NATIVE_PIXMAP_H_

#include <stdint.h>

#include "ui/gfx/buffer_types.h"
#include "ui/gl/gl_image_egl.h"
#include "ui/ozone/public/native_pixmap.h"

namespace gfx {

class GL_EXPORT GLImageOzoneNativePixmap : public gl::GLImageEGL {
public:
    GLImageOzoneNativePixmap(const Size& size, unsigned internalformat);

    bool Initialize(ui::NativePixmap* pixmap, BufferFormat format);

    // Overridden from GLImage:
    unsigned GetInternalFormat() override;
    void Destroy(bool have_context) override;
    bool ScheduleOverlayPlane(AcceleratedWidget widget,
        int z_order,
        OverlayTransform transform,
        const Rect& bounds_rect,
        const RectF& crop_rect) override;
    void OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
        uint64_t process_tracing_id,
        const std::string& dump_name) override;

protected:
    ~GLImageOzoneNativePixmap() override;

private:
    unsigned internalformat_;
    scoped_refptr<ui::NativePixmap> pixmap_;
};

} // namespace gfx

#endif // UI_GL_GL_IMAGE_OZONE_NATIVE_PIXMAP_H_
