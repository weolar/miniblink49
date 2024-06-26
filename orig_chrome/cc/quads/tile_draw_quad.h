// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_TILE_DRAW_QUAD_H_
#define CC_QUADS_TILE_DRAW_QUAD_H_

#include "cc/quads/content_draw_quad_base.h"

namespace cc {

class CC_EXPORT TileDrawQuad : public ContentDrawQuadBase {
public:
    TileDrawQuad();
    ~TileDrawQuad() override;

    void SetNew(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        unsigned resource_id,
        // |tex_coord_rect| contains non-normalized coordinates.
        // TODO(reveman): Make the use of normalized vs non-normalized
        // coordinates consistent across all quad types: crbug.com/487370
        const gfx::RectF& tex_coord_rect,
        const gfx::Size& texture_size,
        bool swizzle_contents,
        bool nearest_neighbor);

    void SetAll(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        bool needs_blending,
        unsigned resource_id,
        // |tex_coord_rect| contains non-normalized coordinates.
        // TODO(reveman): Make the use of normalized vs non-normalized
        // coordinates consistent across all quad types: crbug.com/487370
        const gfx::RectF& tex_coord_rect,
        const gfx::Size& texture_size,
        bool swizzle_contents,
        bool nearest_neighbor);

    static const TileDrawQuad* MaterialCast(const DrawQuad*);

    ResourceId resource_id() const { return resources.ids[kResourceIdIndex]; }

private:
    static const size_t kResourceIdIndex = 0;

    void ExtendValue(base::trace_event::TracedValue* value) const override;
};

} // namespace cc

#endif // CC_QUADS_TILE_DRAW_QUAD_H_
