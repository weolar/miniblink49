// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_DRAW_QUAD_H_
#define CC_QUADS_DRAW_QUAD_H_

#include "base/callback.h"
#include "cc/base/cc_export.h"
#include "cc/base/resource_id.h"
#include "cc/quads/shared_quad_state.h"

namespace base {
namespace trace_event {
    class TracedValue;
}
class Value;
class DictionaryValue;
}

namespace cc {

// DrawQuad is a bag of data used for drawing a quad. Because different
// materials need different bits of per-quad data to render, classes that derive
// from DrawQuad store additional data in their derived instance. The Material
// enum is used to "safely" downcast to the derived class.
// Note: quads contain rects and sizes, which live in different spaces. There is
// the "content space", which is the arbitrary space in which the quad's
// geometry is defined (generally related to the layer that produced the quad,
// e.g. the geometry space for PictureLayerImpls or the layer's coordinate space
// for most other layers). There is also the "target space", which is the space,
// in "physical" pixels, of the render target where the quads is drawn. The
// quad's transform maps the content space to the target space.
class CC_EXPORT DrawQuad {
public:
    enum Material {
        INVALID,
        DEBUG_BORDER,
        IO_SURFACE_CONTENT,
        PICTURE_CONTENT,
        RENDER_PASS,
        SOLID_COLOR,
        STREAM_VIDEO_CONTENT,
        SURFACE_CONTENT,
        TEXTURE_CONTENT,
        TILED_CONTENT,
        YUV_VIDEO_CONTENT,
        MATERIAL_LAST = YUV_VIDEO_CONTENT
    };

    virtual ~DrawQuad();

    Material material;

    // This rect, after applying the quad_transform(), gives the geometry that
    // this quad should draw to. This rect lives in content space.
    gfx::Rect rect;

    // This specifies the region of the quad that is opaque. This rect lives in
    // content space.
    gfx::Rect opaque_rect;

    // Allows changing the rect that gets drawn to make it smaller. This value
    // should be clipped to |rect|. This rect lives in content space.
    gfx::Rect visible_rect;

    // By default blending is used when some part of the quad is not opaque.
    // With this setting, it is possible to force blending on regardless of the
    // opaque area.
    bool needs_blending;

    // Stores state common to a large bundle of quads; kept separate for memory
    // efficiency. There is special treatment to reconstruct these pointers
    // during serialization.
    const SharedQuadState* shared_quad_state;

    bool IsDebugQuad() const { return material == DEBUG_BORDER; }

    bool ShouldDrawWithBlending() const
    {
        if (needs_blending || shared_quad_state->opacity < 1.0f)
            return true;
        if (visible_rect.IsEmpty())
            return false;
        return !opaque_rect.Contains(visible_rect);
    }

    // Is the left edge of this tile aligned with the originating layer's
    // left edge?
    bool IsLeftEdge() const { return !rect.x(); }

    // Is the top edge of this tile aligned with the originating layer's
    // top edge?
    bool IsTopEdge() const { return !rect.y(); }

    // Is the right edge of this tile aligned with the originating layer's
    // right edge?
    bool IsRightEdge() const
    {
        return rect.right() == shared_quad_state->quad_layer_bounds.width();
    }

    // Is the bottom edge of this tile aligned with the originating layer's
    // bottom edge?
    bool IsBottomEdge() const
    {
        return rect.bottom() == shared_quad_state->quad_layer_bounds.height();
    }

    // Is any edge of this tile aligned with the originating layer's
    // corresponding edge?
    bool IsEdge() const
    {
        return IsLeftEdge() || IsTopEdge() || IsRightEdge() || IsBottomEdge();
    }

    void AsValueInto(base::trace_event::TracedValue* value) const;

    struct CC_EXPORT Resources {
        enum : size_t { kMaxResourceIdCount = 4 };
        Resources();

        ResourceId* begin() { return ids; }
        ResourceId* end()
        {
            DCHECK_LE(count, kMaxResourceIdCount);
            return ids + count;
        }

        size_t count;
        ResourceId ids[kMaxResourceIdCount];
    };

    Resources resources;

protected:
    DrawQuad();

    void SetAll(const SharedQuadState* shared_quad_state,
        Material material,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        bool needs_blending);
    virtual void ExtendValue(base::trace_event::TracedValue* value) const = 0;
};

} // namespace cc

#endif // CC_QUADS_DRAW_QUAD_H_
