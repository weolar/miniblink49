// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_IO_SURFACE_DRAW_QUAD_H_
#define CC_QUADS_IO_SURFACE_DRAW_QUAD_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/quads/draw_quad.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class CC_EXPORT IOSurfaceDrawQuad : public DrawQuad {
public:
    enum Orientation {
        FLIPPED,
        UNFLIPPED,
        ORIENTATION_LAST = UNFLIPPED
    };

    IOSurfaceDrawQuad();

    void SetNew(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        const gfx::Size& io_surface_size,
        unsigned io_surface_resource_id,
        Orientation orientation,
        bool allow_overlay);

    void SetAll(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        bool needs_blending,
        const gfx::Size& io_surface_size,
        unsigned io_surface_resource_id,
        Orientation orientation,
        bool allow_overlay);

    gfx::Size io_surface_size;
    Orientation orientation;
    bool allow_overlay;

    ResourceId io_surface_resource_id() const
    {
        return resources.ids[kIOSurfaceResourceIdIndex];
    }

    static const IOSurfaceDrawQuad* MaterialCast(const DrawQuad*);

private:
    static const size_t kIOSurfaceResourceIdIndex = 0;

    void ExtendValue(base::trace_event::TracedValue* value) const override;
};

} // namespace cc

#endif // CC_QUADS_IO_SURFACE_DRAW_QUAD_H_
