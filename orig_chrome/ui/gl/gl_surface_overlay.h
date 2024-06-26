// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_SURFACE_OVERLAY_H_
#define UI_GL_GL_SURFACE_OVERLAY_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/overlay_transform.h"
#include "ui/gl/gl_image.h"

namespace gfx {

// For saving the properties of a GLImage overlay plane and scheduling it later.
class GLSurfaceOverlay {
public:
    GLSurfaceOverlay(int z_order,
        OverlayTransform transform,
        gl::GLImage* image,
        const Rect& bounds_rect,
        const RectF& crop_rect);
    ~GLSurfaceOverlay();

    // Schedule the image as an overlay plane to be shown at swap time for
    // |widget|.
    bool ScheduleOverlayPlane(AcceleratedWidget widget) const;

private:
    int z_order_;
    OverlayTransform transform_;
    scoped_refptr<gl::GLImage> image_;
    Rect bounds_rect_;
    RectF crop_rect_;
};

} // namespace gfx

#endif // UI_GL_GL_SURFACE_OVERLAY_H_
