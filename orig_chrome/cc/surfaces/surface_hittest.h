// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_SURFACE_HITTEST_H_
#define CC_SURFACES_SURFACE_HITTEST_H_

#include <set>

#include "cc/surfaces/surface_id.h"
#include "cc/surfaces/surfaces_export.h"

namespace gfx {
class Point;
class Transform;
}

namespace cc {
class DrawQuad;
class QuadList;
class RenderPass;
class RenderPassId;
class SurfaceManager;

// Performs a hittest in surface quads.
class CC_SURFACES_EXPORT SurfaceHittest {
public:
    explicit SurfaceHittest(SurfaceManager* manager);
    ~SurfaceHittest();

    // Hittests against Surface with SurfaceId |surface_id|, return the contained
    // surface that the point is hitting and the |transformed_point| in the
    // surface space.
    SurfaceId GetTargetSurfaceAtPoint(SurfaceId surface_id,
        const gfx::Point& point,
        gfx::Transform* transform);

private:
    bool GetTargetSurfaceAtPointInternal(
        SurfaceId surface_id,
        const RenderPassId& render_pass_id,
        const gfx::Point& point_in_root_target,
        std::set<const RenderPass*>* referenced_passes,
        SurfaceId* out_surface_id,
        gfx::Transform* out_transform);

    const RenderPass* GetRenderPassForSurfaceById(
        SurfaceId surface_id,
        const RenderPassId& render_pass_id);

    bool PointInQuad(const DrawQuad* quad,
        const gfx::Point& point_in_render_pass_space,
        gfx::Transform* target_to_quad_transform,
        gfx::Point* point_in_quad_space);

    SurfaceManager* const manager_;
};
} // namespace cc

#endif // CC_SURFACES_SURFACE_HITTEST_H_
