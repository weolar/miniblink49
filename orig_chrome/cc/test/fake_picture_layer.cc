// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_picture_layer.h"

#include "cc/test/fake_picture_layer_impl.h"

namespace cc {

FakePictureLayer::FakePictureLayer(const LayerSettings& settings,
    ContentLayerClient* client)
    : PictureLayer(settings, client)
    , update_count_(0)
    , push_properties_count_(0)
    , always_update_resources_(false)
{
    SetBounds(gfx::Size(1, 1));
    SetIsDrawable(true);
}

FakePictureLayer::FakePictureLayer(const LayerSettings& settings,
    ContentLayerClient* client,
    scoped_ptr<RecordingSource> source)
    : PictureLayer(settings, client, source.Pass())
    , update_count_(0)
    , push_properties_count_(0)
    , always_update_resources_(false)
{
    SetBounds(gfx::Size(1, 1));
    SetIsDrawable(true);
}

FakePictureLayer::~FakePictureLayer() { }

scoped_ptr<LayerImpl> FakePictureLayer::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    if (is_mask())
        return FakePictureLayerImpl::CreateMask(tree_impl, layer_id_);
    return FakePictureLayerImpl::Create(tree_impl, layer_id_);
}

bool FakePictureLayer::Update()
{
    bool updated = PictureLayer::Update();
    update_count_++;
    return updated || always_update_resources_;
}

void FakePictureLayer::PushPropertiesTo(LayerImpl* layer)
{
    PictureLayer::PushPropertiesTo(layer);
    push_properties_count_++;
}

} // namespace cc
