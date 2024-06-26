// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_DEBUG_BORDER_DRAW_QUAD_H_
#define CC_QUADS_DEBUG_BORDER_DRAW_QUAD_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/quads/draw_quad.h"
#include "third_party/skia/include/core/SkColor.h"

namespace cc {

class CC_EXPORT DebugBorderDrawQuad : public DrawQuad {
public:
    DebugBorderDrawQuad();

    void SetNew(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& visible_rect,
        SkColor color,
        int width);

    void SetAll(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        bool needs_blending,
        SkColor color,
        int width);

    SkColor color;
    int width;

    static const DebugBorderDrawQuad* MaterialCast(const DrawQuad*);

private:
    void ExtendValue(base::trace_event::TracedValue* value) const override;
};

} // namespace cc

#endif // CC_QUADS_DEBUG_BORDER_DRAW_QUAD_H_
