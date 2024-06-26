// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/nine_patch_layer.h"

#include "cc/layers/nine_patch_layer_impl.h"
#include "cc/resources/scoped_ui_resource.h"
#include "cc/resources/ui_resource_bitmap.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

scoped_refptr<NinePatchLayer> NinePatchLayer::Create(
    const LayerSettings& settings)
{
    return make_scoped_refptr(new NinePatchLayer(settings));
}

NinePatchLayer::NinePatchLayer(const LayerSettings& settings)
    : UIResourceLayer(settings)
    , fill_center_(false)
{
}

NinePatchLayer::~NinePatchLayer() { }

scoped_ptr<LayerImpl> NinePatchLayer::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return NinePatchLayerImpl::Create(tree_impl, id());
}

void NinePatchLayer::SetBorder(const gfx::Rect& border)
{
    if (border == border_)
        return;
    border_ = border;
    SetNeedsCommit();
}

void NinePatchLayer::SetAperture(const gfx::Rect& aperture)
{
    if (image_aperture_ == aperture)
        return;

    image_aperture_ = aperture;
    SetNeedsCommit();
}

void NinePatchLayer::SetFillCenter(bool fill_center)
{
    if (fill_center_ == fill_center)
        return;

    fill_center_ = fill_center;
    SetNeedsCommit();
}

void NinePatchLayer::PushPropertiesTo(LayerImpl* layer)
{
    UIResourceLayer::PushPropertiesTo(layer);
    NinePatchLayerImpl* layer_impl = static_cast<NinePatchLayerImpl*>(layer);

    if (!ui_resource_holder_) {
        layer_impl->SetUIResourceId(0);
    } else {
        DCHECK(layer_tree_host());

        layer_impl->SetLayout(image_aperture_, border_, fill_center_);
    }
}

} // namespace cc
