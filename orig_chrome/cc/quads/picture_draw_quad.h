// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_PICTURE_DRAW_QUAD_H_
#define CC_QUADS_PICTURE_DRAW_QUAD_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/playback/raster_source.h"
#include "cc/quads/content_draw_quad_base.h"
#include "cc/resources/resource_provider.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

// Used for on-demand tile rasterization.
class CC_EXPORT PictureDrawQuad : public ContentDrawQuadBase {
public:
    PictureDrawQuad();
    ~PictureDrawQuad() override;

    void SetNew(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        const gfx::RectF& tex_coord_rect,
        const gfx::Size& texture_size,
        bool nearest_neighbor,
        ResourceFormat texture_format,
        const gfx::Rect& content_rect,
        float contents_scale,
        scoped_refptr<RasterSource> raster_source);

    void SetAll(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        bool needs_blending,
        const gfx::RectF& tex_coord_rect,
        const gfx::Size& texture_size,
        bool nearest_neighbor,
        ResourceFormat texture_format,
        const gfx::Rect& content_rect,
        float contents_scale,
        scoped_refptr<RasterSource> raster_source);

    gfx::Rect content_rect;
    float contents_scale;
    scoped_refptr<RasterSource> raster_source;
    ResourceFormat texture_format;

    static const PictureDrawQuad* MaterialCast(const DrawQuad* quad);

private:
    void ExtendValue(base::trace_event::TracedValue* value) const override;
};

} // namespace cc

#endif // CC_QUADS_PICTURE_DRAW_QUAD_H_
