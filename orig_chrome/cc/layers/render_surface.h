// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_RENDER_SURFACE_H_
#define CC_LAYERS_RENDER_SURFACE_H_

#include <vector>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "cc/base/cc_export.h"
#include "cc/layers/layer_lists.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/transform.h"

namespace cc {

class Layer;

class CC_EXPORT RenderSurface {
public:
    explicit RenderSurface(Layer* owning_layer);
    ~RenderSurface();

    // Returns the rect that encloses the RenderSurfaceImpl including any
    // reflection.
    gfx::RectF DrawableContentRect() const;

    void SetContentRect(const gfx::Rect& content_rect)
    {
        content_rect_ = content_rect;
    }
    gfx::Rect content_rect() const { return content_rect_; }

    void SetDrawOpacity(float opacity) { draw_opacity_ = opacity; }
    float draw_opacity() const { return draw_opacity_; }

    void SetDrawOpacityIsAnimating(bool draw_opacity_is_animating)
    {
        draw_opacity_is_animating_ = draw_opacity_is_animating;
    }
    bool draw_opacity_is_animating() const { return draw_opacity_is_animating_; }

    void SetDrawTransform(const gfx::Transform& draw_transform)
    {
        draw_transform_ = draw_transform;
    }
    const gfx::Transform& draw_transform() const { return draw_transform_; }

    void SetScreenSpaceTransform(const gfx::Transform& screen_space_transform)
    {
        screen_space_transform_ = screen_space_transform;
    }
    const gfx::Transform& screen_space_transform() const
    {
        return screen_space_transform_;
    }

    void SetReplicaDrawTransform(const gfx::Transform& replica_draw_transform)
    {
        replica_draw_transform_ = replica_draw_transform;
    }
    const gfx::Transform& replica_draw_transform() const
    {
        return replica_draw_transform_;
    }

    void SetReplicaScreenSpaceTransform(
        const gfx::Transform& replica_screen_space_transform)
    {
        replica_screen_space_transform_ = replica_screen_space_transform;
    }
    const gfx::Transform& replica_screen_space_transform() const
    {
        return replica_screen_space_transform_;
    }

    void SetTargetSurfaceTransformsAreAnimating(bool animating)
    {
        target_surface_transforms_are_animating_ = animating;
    }
    bool target_surface_transforms_are_animating() const
    {
        return target_surface_transforms_are_animating_;
    }
    void SetScreenSpaceTransformsAreAnimating(bool animating)
    {
        screen_space_transforms_are_animating_ = animating;
    }
    bool screen_space_transforms_are_animating() const
    {
        return screen_space_transforms_are_animating_;
    }

    bool is_clipped() const { return is_clipped_; }
    void SetIsClipped(bool is_clipped) { is_clipped_ = is_clipped; }

    gfx::Rect clip_rect() const { return clip_rect_; }
    void SetClipRect(const gfx::Rect& clip_rect) { clip_rect_ = clip_rect; }

    // When false, the RenderSurface does not contribute to another target
    // RenderSurface that is being drawn for the current frame. It could still be
    // drawn to as a target, but its output will not be a part of any other
    // surface.
    bool contributes_to_drawn_surface() const
    {
        return contributes_to_drawn_surface_;
    }
    void set_contributes_to_drawn_surface(bool contributes_to_drawn_surface)
    {
        contributes_to_drawn_surface_ = contributes_to_drawn_surface;
    }

    LayerList& layer_list() { return layer_list_; }
    // A no-op since DelegatedRendererLayers on the main thread don't have any
    // RenderPasses so they can't contribute to a surface.
    void AddContributingDelegatedRenderPassLayer(Layer* layer) { }

    void SetNearestOcclusionImmuneAncestor(RenderSurface* surface)
    {
        nearest_occlusion_immune_ancestor_ = surface;
    }
    const RenderSurface* nearest_occlusion_immune_ancestor() const
    {
        return nearest_occlusion_immune_ancestor_;
    }

    void ClearLayerLists();

private:
    Layer* owning_layer_;

    // Uses this surface's space.
    gfx::Rect content_rect_;

    float draw_opacity_;
    bool draw_opacity_is_animating_;
    gfx::Transform draw_transform_;
    gfx::Transform screen_space_transform_;
    gfx::Transform replica_draw_transform_;
    gfx::Transform replica_screen_space_transform_;
    bool target_surface_transforms_are_animating_;
    bool screen_space_transforms_are_animating_;

    bool is_clipped_;
    bool contributes_to_drawn_surface_;

    // Uses the space of the surface's target surface.
    gfx::Rect clip_rect_;

    LayerList layer_list_;

    // The nearest ancestor target surface that will contain the contents of this
    // surface, and that ignores outside occlusion. This can point to itself.
    RenderSurface* nearest_occlusion_immune_ancestor_;

    DISALLOW_COPY_AND_ASSIGN(RenderSurface);
};

} // namespace cc
#endif // CC_LAYERS_RENDER_SURFACE_H_
