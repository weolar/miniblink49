// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_DELEGATED_RENDERER_LAYER_H_
#define CC_LAYERS_DELEGATED_RENDERER_LAYER_H_

#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/synchronization/lock.h"
#include "cc/base/cc_export.h"
#include "cc/layers/delegated_frame_provider.h"
#include "cc/layers/layer.h"
#include "cc/resources/returned_resource.h"

namespace cc {

class CC_EXPORT DelegatedRendererLayer : public Layer {
public:
    static scoped_refptr<DelegatedRendererLayer> Create(
        const LayerSettings& settings,
        const scoped_refptr<DelegatedFrameProvider>& frame_provider);

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void SetLayerTreeHost(LayerTreeHost* host) override;
    bool Update() override;
    void PushPropertiesTo(LayerImpl* impl) override;

    // Called by the DelegatedFrameProvider when a new frame is available to be
    // picked up.
    void ProviderHasNewFrame();
    bool HasDelegatedContent() const override;

protected:
    DelegatedRendererLayer(
        const LayerSettings& settings,
        const scoped_refptr<DelegatedFrameProvider>& frame_provider);
    ~DelegatedRendererLayer() override;

private:
    scoped_refptr<DelegatedFrameProvider> frame_provider_;

    bool should_collect_new_frame_;

    DelegatedFrameData* frame_data_;
    gfx::Rect frame_damage_;

    base::WeakPtrFactory<DelegatedRendererLayer> weak_ptrs_;

    DISALLOW_COPY_AND_ASSIGN(DelegatedRendererLayer);
};

} // namespace cc

#endif // CC_LAYERS_DELEGATED_RENDERER_LAYER_H_
