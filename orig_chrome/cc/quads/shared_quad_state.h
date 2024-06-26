// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_SHARED_QUAD_STATE_H_
#define CC_QUADS_SHARED_QUAD_STATE_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "third_party/skia/include/core/SkXfermode.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/transform.h"

namespace base {
namespace trace_event {
    class TracedValue;
}
class Value;
}

namespace cc {

// SharedQuadState holds a set of properties that are common across multiple
// DrawQuads. It's purely an optimization - the properties behave in exactly the
// same way as if they were replicated on each DrawQuad. A given SharedQuadState
// can only be shared by DrawQuads that are adjacent in their RenderPass'
// QuadList.
class CC_EXPORT SharedQuadState {
public:
    SharedQuadState();
    ~SharedQuadState();

    void CopyFrom(const SharedQuadState* other);

    void SetAll(const gfx::Transform& quad_to_target_transform,
        const gfx::Size& layer_bounds,
        const gfx::Rect& visible_layer_rect,
        const gfx::Rect& clip_rect,
        bool is_clipped,
        float opacity,
        SkXfermode::Mode blend_mode,
        int sorting_context_id);
    void AsValueInto(base::trace_event::TracedValue* dict) const;

    // Transforms quad rects into the target content space.
    gfx::Transform quad_to_target_transform;
    // The size of the quads' originating layer in the space of the quad rects.
    gfx::Size quad_layer_bounds;
    // The size of the visible area in the quads' originating layer, in the space
    // of the quad rects.
    gfx::Rect visible_quad_layer_rect;
    // This rect lives in the target content space.
    gfx::Rect clip_rect;
    bool is_clipped;
    float opacity;
    SkXfermode::Mode blend_mode;
    int sorting_context_id;
};

} // namespace cc

#endif // CC_QUADS_SHARED_QUAD_STATE_H_
