// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/solid_color_scrollbar_layer.h"

#include "base/memory/scoped_ptr.h"
#include "cc/layers/layer_impl.h"
#include "cc/layers/solid_color_scrollbar_layer_impl.h"

namespace cc {

scoped_ptr<LayerImpl> SolidColorScrollbarLayer::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    const bool kIsOverlayScrollbar = true;
    return SolidColorScrollbarLayerImpl::Create(tree_impl,
        id(),
        orientation(),
        thumb_thickness_,
        track_start_,
        is_left_side_vertical_scrollbar_,
        kIsOverlayScrollbar);
}

scoped_refptr<SolidColorScrollbarLayer> SolidColorScrollbarLayer::Create(
    const LayerSettings& settings,
    ScrollbarOrientation orientation,
    int thumb_thickness,
    int track_start,
    bool is_left_side_vertical_scrollbar,
    int scroll_layer_id)
{
    return make_scoped_refptr(new SolidColorScrollbarLayer(
        settings, orientation, thumb_thickness, track_start,
        is_left_side_vertical_scrollbar, scroll_layer_id));
}

SolidColorScrollbarLayer::SolidColorScrollbarLayer(
    const LayerSettings& settings,
    ScrollbarOrientation orientation,
    int thumb_thickness,
    int track_start,
    bool is_left_side_vertical_scrollbar,
    int scroll_layer_id)
    : Layer(settings)
    , scroll_layer_id_(Layer::INVALID_ID)
    , clip_layer_id_(scroll_layer_id)
    , orientation_(orientation)
    , thumb_thickness_(thumb_thickness)
    , track_start_(track_start)
    , is_left_side_vertical_scrollbar_(is_left_side_vertical_scrollbar)
{
}

SolidColorScrollbarLayer::~SolidColorScrollbarLayer() { }

ScrollbarLayerInterface* SolidColorScrollbarLayer::ToScrollbarLayer()
{
    return this;
}

void SolidColorScrollbarLayer::PushPropertiesTo(LayerImpl* layer)
{
    Layer::PushPropertiesTo(layer);
    PushScrollClipPropertiesTo(layer);
}

void SolidColorScrollbarLayer::PushScrollClipPropertiesTo(LayerImpl* layer)
{
    SolidColorScrollbarLayerImpl* scrollbar_layer = static_cast<SolidColorScrollbarLayerImpl*>(layer);

    scrollbar_layer->SetScrollLayerAndClipLayerByIds(scroll_layer_id_,
        clip_layer_id_);
}

void SolidColorScrollbarLayer::SetNeedsDisplayRect(const gfx::Rect& rect)
{
    // Never needs repaint.
}

bool SolidColorScrollbarLayer::OpacityCanAnimateOnImplThread() const
{
    return true;
}

int SolidColorScrollbarLayer::ScrollLayerId() const
{
    return scroll_layer_id_;
}

void SolidColorScrollbarLayer::SetScrollLayer(int layer_id)
{
    if (layer_id == scroll_layer_id_)
        return;

    scroll_layer_id_ = layer_id;
    SetNeedsFullTreeSync();
}

void SolidColorScrollbarLayer::SetClipLayer(int layer_id)
{
    if (layer_id == clip_layer_id_)
        return;

    clip_layer_id_ = layer_id;
    SetNeedsFullTreeSync();
}

ScrollbarOrientation SolidColorScrollbarLayer::orientation() const
{
    return orientation_;
}

} // namespace cc
