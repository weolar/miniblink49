// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_DELEGATED_RENDERER_LAYER_IMPL_H_
#define CC_TEST_FAKE_DELEGATED_RENDERER_LAYER_IMPL_H_

#include "cc/layers/delegated_renderer_layer_impl.h"

namespace cc {

class FakeDelegatedRendererLayerImpl : public DelegatedRendererLayerImpl {
public:
    static scoped_ptr<FakeDelegatedRendererLayerImpl> Create(
        LayerTreeImpl* tree_impl, int id)
    {
        return make_scoped_ptr(new FakeDelegatedRendererLayerImpl(tree_impl, id));
    }
    ~FakeDelegatedRendererLayerImpl() override;

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

    int ChildId() const { return ChildIdForTesting(); }
    const RenderPassList& RenderPassesInDrawOrder() const
    {
        return RenderPassesInDrawOrderForTesting();
    }
    ResourceProvider::ResourceIdSet Resources() const;

    void SetFrameDataForRenderPasses(float device_scale_factor,
        const RenderPassList& pass_list);

protected:
    FakeDelegatedRendererLayerImpl(LayerTreeImpl* tree_impl, int id);
};

} // namespace cc

#endif // CC_TEST_FAKE_DELEGATED_RENDERER_LAYER_IMPL_H_
