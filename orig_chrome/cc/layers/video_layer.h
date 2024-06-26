// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_VIDEO_LAYER_H_
#define CC_LAYERS_VIDEO_LAYER_H_

#include "base/callback.h"
#include "cc/base/cc_export.h"
#include "cc/layers/layer.h"
#include "media/base/video_rotation.h"

namespace media {
class VideoFrame;
}

namespace cc {

class VideoFrameProvider;
class VideoLayerImpl;

// A Layer that contains a Video element.
class CC_EXPORT VideoLayer : public Layer {
public:
    static scoped_refptr<VideoLayer> Create(const LayerSettings& settings,
        VideoFrameProvider* provider,
        media::VideoRotation video_rotation);

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

    bool Update() override;

private:
    VideoLayer(const LayerSettings& settings,
        VideoFrameProvider* provider,
        media::VideoRotation video_rotation);
    ~VideoLayer() override;

    // This pointer is only for passing to VideoLayerImpl's constructor. It should
    // never be dereferenced by this class.
    VideoFrameProvider* provider_;

    media::VideoRotation video_rotation_;

    DISALLOW_COPY_AND_ASSIGN(VideoLayer);
};

} // namespace cc

#endif // CC_LAYERS_VIDEO_LAYER_H_
