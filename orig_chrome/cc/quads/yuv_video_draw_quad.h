// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_YUV_VIDEO_DRAW_QUAD_H_
#define CC_QUADS_YUV_VIDEO_DRAW_QUAD_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/quads/draw_quad.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class CC_EXPORT YUVVideoDrawQuad : public DrawQuad {
public:
    enum ColorSpace {
        REC_601, // SDTV standard with restricted "studio swing" color range.
        REC_709, // HDTV standard with restricted "studio swing" color range.
        JPEG, // Full color range [0, 255] JPEG color space.
        COLOR_SPACE_LAST = JPEG
    };

    ~YUVVideoDrawQuad() override;

    YUVVideoDrawQuad();

    void SetNew(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        // |*_tex_coord_rect| contains non-normalized coordinates.
        // TODO(reveman): Make the use of normalized vs non-normalized
        // coordinates consistent across all quad types: crbug.com/487370
        const gfx::RectF& ya_tex_coord_rect,
        const gfx::RectF& uv_tex_coord_rect,
        const gfx::Size& ya_tex_size,
        const gfx::Size& uv_tex_size,
        unsigned y_plane_resource_id,
        unsigned u_plane_resource_id,
        unsigned v_plane_resource_id,
        unsigned a_plane_resource_id,
        ColorSpace color_space);

    void SetAll(const SharedQuadState* shared_quad_state,
        const gfx::Rect& rect,
        const gfx::Rect& opaque_rect,
        const gfx::Rect& visible_rect,
        bool needs_blending,
        // |*_tex_coord_rect| contains non-normalized coordinates.
        // TODO(reveman): Make the use of normalized vs non-normalized
        // coordinates consistent across all quad types: crbug.com/487370
        const gfx::RectF& ya_tex_coord_rect,
        const gfx::RectF& uv_tex_coord_rect,
        const gfx::Size& ya_tex_size,
        const gfx::Size& uv_tex_size,
        unsigned y_plane_resource_id,
        unsigned u_plane_resource_id,
        unsigned v_plane_resource_id,
        unsigned a_plane_resource_id,
        ColorSpace color_space);

    gfx::RectF ya_tex_coord_rect;
    gfx::RectF uv_tex_coord_rect;
    gfx::Size ya_tex_size;
    gfx::Size uv_tex_size;
    ColorSpace color_space;

    static const YUVVideoDrawQuad* MaterialCast(const DrawQuad*);

    ResourceId y_plane_resource_id() const
    {
        return resources.ids[kYPlaneResourceIdIndex];
    }
    ResourceId u_plane_resource_id() const
    {
        return resources.ids[kUPlaneResourceIdIndex];
    }
    ResourceId v_plane_resource_id() const
    {
        return resources.ids[kVPlaneResourceIdIndex];
    }
    ResourceId a_plane_resource_id() const
    {
        return resources.ids[kAPlaneResourceIdIndex];
    }

private:
    static const size_t kYPlaneResourceIdIndex = 0;
    static const size_t kUPlaneResourceIdIndex = 1;
    static const size_t kVPlaneResourceIdIndex = 2;
    static const size_t kAPlaneResourceIdIndex = 3;

    void ExtendValue(base::trace_event::TracedValue* value) const override;
};

} // namespace cc

#endif // CC_QUADS_YUV_VIDEO_DRAW_QUAD_H_
