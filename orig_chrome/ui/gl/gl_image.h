// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_IMAGE_H_
#define UI_GL_GL_IMAGE_H_

#include <string>

#include "base/memory/ref_counted.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/overlay_transform.h"
#include "ui/gl/gl_export.h"

namespace base {
namespace trace_event {
    class ProcessMemoryDump;
}
}

namespace gl {

// Encapsulates an image that can be bound and/or copied to a texture, hiding
// platform specific management.
class GL_EXPORT GLImage : public base::RefCounted<GLImage> {
public:
    GLImage() { }

    // Destroys the image.
    virtual void Destroy(bool have_context) = 0;

    // Get the size of the image.
    virtual gfx::Size GetSize() = 0;

    // Get the internal format of the image.
    virtual unsigned GetInternalFormat() = 0;

    // Bind image to texture currently bound to |target|. Returns true on success.
    // It is valid for an implementation to always return false.
    virtual bool BindTexImage(unsigned target) = 0;

    // Release image from texture currently bound to |target|.
    virtual void ReleaseTexImage(unsigned target) = 0;

    // Define texture currently bound to |target| by copying image into it.
    // Returns true on success. It is valid for an implementation to always
    // return false.
    virtual bool CopyTexImage(unsigned target) = 0;

    // Copy |rect| of image to |offset| in texture currently bound to |target|.
    // Returns true on success. It is valid for an implementation to always
    // return false.
    virtual bool CopyTexSubImage(unsigned target,
        const gfx::Point& offset,
        const gfx::Rect& rect)
        = 0;

    // Schedule image as an overlay plane to be shown at swap time for |widget|.
    virtual bool ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
        int z_order,
        gfx::OverlayTransform transform,
        const gfx::Rect& bounds_rect,
        const gfx::RectF& crop_rect)
        = 0;

    // Dumps information about the memory backing the GLImage to a dump named
    // |dump_name|.
    virtual void OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
        uint64_t process_tracing_id,
        const std::string& dump_name)
        = 0;

protected:
    virtual ~GLImage() { }

private:
    friend class base::RefCounted<GLImage>;

    DISALLOW_COPY_AND_ASSIGN(GLImage);
};

} // namespace gl

#endif // UI_GL_GL_IMAGE_H_
