// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_SURFACE_DRAW_QUAD_H_
#define CC_QUADS_SURFACE_DRAW_QUAD_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/quads/draw_quad.h"
#include "cc/surfaces/surface_id.h"

namespace cc {

class CC_EXPORT SurfaceDrawQuad : public DrawQuad {
public:
    SurfaceDrawQuad();

    void SetNew(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& visible_rect,
        SurfaceId surface_id);

    void SetAll(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        bool needs_blending,
        SurfaceId surface_id);

    SurfaceId surface_id;

    static const SurfaceDrawQuad* MaterialCast(const DrawQuad* quad);

private:
    void ExtendValue(base::trace_event::TracedValue* value) const override;
};

} // namespace cc

#endif // CC_QUADS_SURFACE_DRAW_QUAD_H_
