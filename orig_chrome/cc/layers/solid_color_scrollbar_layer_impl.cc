// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/solid_color_scrollbar_layer_impl.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/layer_tree_settings.h"
#include "cc/trees/occlusion.h"

namespace cc {

scoped_ptr<SolidColorScrollbarLayerImpl> SolidColorScrollbarLayerImpl::Create(
    LayerTreeImpl* tree_impl,
    int id,
    ScrollbarOrientation orientation,
    int thumb_thickness,
    int track_start,
    bool is_left_side_vertical_scrollbar,
    bool is_overlay)
{
    return make_scoped_ptr(
        new SolidColorScrollbarLayerImpl(tree_impl,
            id,
            orientation,
            thumb_thickness,
            track_start,
            is_left_side_vertical_scrollbar,
            is_overlay));
}

SolidColorScrollbarLayerImpl::~SolidColorScrollbarLayerImpl() { }

scoped_ptr<LayerImpl> SolidColorScrollbarLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return SolidColorScrollbarLayerImpl::Create(tree_impl,
        id(),
        orientation(),
        thumb_thickness_,
        track_start_,
        is_left_side_vertical_scrollbar(),
        is_overlay_scrollbar());
}

SolidColorScrollbarLayerImpl::SolidColorScrollbarLayerImpl(
    LayerTreeImpl* tree_impl,
    int id,
    ScrollbarOrientation orientation,
    int thumb_thickness,
    int track_start,
    bool is_left_side_vertical_scrollbar,
    bool is_overlay)
    : ScrollbarLayerImplBase(tree_impl,
        id,
        orientation,
        is_left_side_vertical_scrollbar,
        is_overlay)
    , thumb_thickness_(thumb_thickness)
    , track_start_(track_start)
    , color_(tree_impl->settings().solid_color_scrollbar_color)
{
    SetOpacity(0.f);
}

void SolidColorScrollbarLayerImpl::PushPropertiesTo(LayerImpl* layer)
{
    ScrollbarLayerImplBase::PushPropertiesTo(layer);
}

int SolidColorScrollbarLayerImpl::ThumbThickness() const
{
    if (thumb_thickness_ != -1)
        return thumb_thickness_;

    if (orientation() == HORIZONTAL)
        return bounds().height();
    else
        return bounds().width();
}

int SolidColorScrollbarLayerImpl::ThumbLength() const
{
    return std::max(
        static_cast<int>(visible_to_total_length_ratio() * TrackLength()),
        ThumbThickness());
}

float SolidColorScrollbarLayerImpl::TrackLength() const
{
    if (orientation() == HORIZONTAL)
        return bounds().width() - TrackStart() * 2;
    else
        return bounds().height() + vertical_adjust() - TrackStart() * 2;
}

int SolidColorScrollbarLayerImpl::TrackStart() const { return track_start_; }

bool SolidColorScrollbarLayerImpl::IsThumbResizable() const
{
    return true;
}

void SolidColorScrollbarLayerImpl::AppendQuads(
    RenderPass* render_pass,
    AppendQuadsData* append_quads_data)
{
    SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
    PopulateSharedQuadState(shared_quad_state);

    AppendDebugBorderQuad(render_pass, bounds(), shared_quad_state,
        append_quads_data);

    gfx::Rect thumb_quad_rect(ComputeThumbQuadRect());
    gfx::Rect visible_quad_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        thumb_quad_rect);
    if (visible_quad_rect.IsEmpty())
        return;

    SolidColorDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
    quad->SetNew(
        shared_quad_state, thumb_quad_rect, visible_quad_rect, color_, false);
}

const char* SolidColorScrollbarLayerImpl::LayerTypeAsString() const
{
    return "cc::SolidColorScrollbarLayerImpl";
}

} // namespace cc
