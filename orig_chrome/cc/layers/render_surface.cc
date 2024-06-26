// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/render_surface.h"

#include "cc/base/math_util.h"
#include "cc/layers/layer.h"
#include "ui/gfx/transform.h"

namespace cc {

RenderSurface::RenderSurface(Layer* owning_layer)
    : owning_layer_(owning_layer)
    , draw_opacity_(1)
    , draw_opacity_is_animating_(false)
    , target_surface_transforms_are_animating_(false)
    , screen_space_transforms_are_animating_(false)
    , is_clipped_(false)
    , contributes_to_drawn_surface_(false)
    , nearest_occlusion_immune_ancestor_(nullptr)
{
}

RenderSurface::~RenderSurface()
{
    for (size_t i = 0; i < layer_list_.size(); ++i) {
        DCHECK(!layer_list_.at(i)->render_surface()) << "RenderSurfaces should be cleared from the contributing layers "
                                                     << "before destroying this surface to avoid leaking a circular "
                                                     << "reference on the contributing layer. Probably the "
                                                     << "RenderSurfaceLayerList should just be destroyed before destroying "
                                                     << "any RenderSurfaces on layers.";
    }
}

gfx::RectF RenderSurface::DrawableContentRect() const
{
    gfx::RectF drawable_content_rect = MathUtil::MapClippedRect(draw_transform_, content_rect_);
    if (owning_layer_->has_replica())
        drawable_content_rect.Union(
            MathUtil::MapClippedRect(replica_draw_transform_, content_rect_));
    return drawable_content_rect;
}

void RenderSurface::ClearLayerLists()
{
    layer_list_.clear();
}

} // namespace cc
