// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_DELEGATED_RENDERER_LAYER_H_
#define CC_TEST_FAKE_DELEGATED_RENDERER_LAYER_H_

#include "cc/layers/delegated_renderer_layer.h"

namespace cc {

class LayerSettings;

class FakeDelegatedRendererLayer : public DelegatedRendererLayer {
public:
    static scoped_refptr<FakeDelegatedRendererLayer> Create(
        const LayerSettings& settings,
        DelegatedFrameProvider* frame_provider)
    {
        return make_scoped_refptr(
            new FakeDelegatedRendererLayer(settings, frame_provider));
    }

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

protected:
    explicit FakeDelegatedRendererLayer(const LayerSettings& settings,
        DelegatedFrameProvider* frame_provider);
    ~FakeDelegatedRendererLayer() override;
};

} // namespace cc

#endif // CC_TEST_FAKE_DELEGATED_RENDERER_LAYER_H_
