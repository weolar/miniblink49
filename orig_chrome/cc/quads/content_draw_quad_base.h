// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_CONTENT_DRAW_QUAD_BASE_H_
#define CC_QUADS_CONTENT_DRAW_QUAD_BASE_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/quads/draw_quad.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"

namespace gfx {
class Rect;
}

namespace cc {

class CC_EXPORT ContentDrawQuadBase : public DrawQuad {
public:
    void SetNew(const SharedQuadState* shared_quad_state,
        DrawQuad::Material material,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        const gfx::RectF& tex_coord_rect,
        const gfx::Size& texture_size,
        bool swizzle_contents,
        bool nearest_neighbor);

    void SetAll(const SharedQuadState* shared_quad_state,
        DrawQuad::Material material,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        bool needs_blending,
        const gfx::RectF& tex_coord_rect,
        const gfx::Size& texture_size,
        bool swizzle_contents,
        bool nearest_neighbor);

    gfx::RectF tex_coord_rect;
    gfx::Size texture_size;
    bool swizzle_contents;
    bool nearest_neighbor;

protected:
    ContentDrawQuadBase();
    ~ContentDrawQuadBase() override;
    void ExtendValue(base::trace_event::TracedValue* value) const override;
};

} // namespace cc

#endif // CC_QUADS_CONTENT_DRAW_QUAD_BASE_H_
