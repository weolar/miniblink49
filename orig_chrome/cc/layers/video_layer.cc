// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/video_layer.h"

#include "cc/layers/video_layer_impl.h"

namespace cc {

scoped_refptr<VideoLayer> VideoLayer::Create(
    const LayerSettings& settings,
    VideoFrameProvider* provider,
    media::VideoRotation video_rotation)
{
    return make_scoped_refptr(new VideoLayer(settings, provider, video_rotation));
}

VideoLayer::VideoLayer(const LayerSettings& settings,
    VideoFrameProvider* provider,
    media::VideoRotation video_rotation)
    : Layer(settings)
    , provider_(provider)
    , video_rotation_(video_rotation)
{
    DCHECK(provider_);
}

VideoLayer::~VideoLayer() { }

scoped_ptr<LayerImpl> VideoLayer::CreateLayerImpl(LayerTreeImpl* tree_impl)
{
    scoped_ptr<VideoLayerImpl> impl = VideoLayerImpl::Create(tree_impl, id(), provider_, video_rotation_);
    return impl.Pass();
}

bool VideoLayer::Update()
{
    bool updated = Layer::Update();

    // Video layer doesn't update any resources from the main thread side,
    // but repaint rects need to be sent to the VideoLayerImpl via commit.
    //
    // This is the inefficient legacy redraw path for videos.  It's better to
    // communicate this directly to the VideoLayerImpl.
    updated |= !update_rect_.IsEmpty();

    return updated;
}

} // namespace cc
