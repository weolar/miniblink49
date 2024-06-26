// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/picture_image_layer_impl.h"

#include <algorithm>

#include "cc/debug/debug_colors.h"
#include "cc/trees/layer_tree_impl.h"

namespace cc {

PictureImageLayerImpl::PictureImageLayerImpl(LayerTreeImpl* tree_impl,
    int id,
    bool is_mask)
    : PictureLayerImpl(tree_impl,
        id,
        is_mask,
        new LayerImpl::SyncedScrollOffset)
{
}

PictureImageLayerImpl::~PictureImageLayerImpl()
{
}

const char* PictureImageLayerImpl::LayerTypeAsString() const
{
    return "cc::PictureImageLayerImpl";
}

scoped_ptr<LayerImpl> PictureImageLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return PictureImageLayerImpl::Create(tree_impl, id(), is_mask_);
}

void PictureImageLayerImpl::GetDebugBorderProperties(
    SkColor* color, float* width) const
{
    *color = DebugColors::ImageLayerBorderColor();
    *width = DebugColors::ImageLayerBorderWidth(layer_tree_impl());
}

bool PictureImageLayerImpl::ShouldAdjustRasterScale() const
{
    return false;
}

void PictureImageLayerImpl::RecalculateRasterScales()
{
    // Don't scale images during rastering to ensure image quality, save memory
    // and avoid frequent re-rastering on change of scale.
    raster_page_scale_ = 1.f;
    raster_device_scale_ = 1.f;
    raster_source_scale_ = std::max(1.f, MinimumContentsScale());
    raster_contents_scale_ = raster_source_scale_;
    // We don't need low res tiles.
    low_res_raster_contents_scale_ = raster_contents_scale_;
}

void PictureImageLayerImpl::UpdateIdealScales()
{
    ideal_contents_scale_ = 1.f;
    ideal_page_scale_ = 1.f;
    ideal_device_scale_ = 1.f;
    ideal_source_scale_ = 1.f;
}

} // namespace cc
