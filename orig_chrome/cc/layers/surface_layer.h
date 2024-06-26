// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_SURFACE_LAYER_H_
#define CC_LAYERS_SURFACE_LAYER_H_

#include "cc/base/cc_export.h"
#include "cc/layers/layer.h"
#include "cc/surfaces/surface_id.h"
#include "cc/surfaces/surface_sequence.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

// A layer that renders a surface referencing the output of another compositor
// instance or client.
class CC_EXPORT SurfaceLayer : public Layer {
public:
    // This callback is run when a SurfaceSequence needs to be satisfied, but
    // the parent compositor is unable to. It can be called on either the main
    // or impl threads.
    using SatisfyCallback = base::Callback<void(SurfaceSequence)>;

    // This callback is run to require that a specific SurfaceSequence is
    // received before a SurfaceId is destroyed.
    using RequireCallback = base::Callback<void(SurfaceId, SurfaceSequence)>;

    static scoped_refptr<SurfaceLayer> Create(
        const LayerSettings& settings,
        const SatisfyCallback& satisfy_callback,
        const RequireCallback& require_callback);

    void SetSurfaceId(SurfaceId surface_id, float scale, const gfx::Size& size);

    // Layer overrides.
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void SetLayerTreeHost(LayerTreeHost* host) override;
    void PushPropertiesTo(LayerImpl* layer) override;

protected:
    SurfaceLayer(const LayerSettings& settings,
        const SatisfyCallback& satisfy_callback,
        const RequireCallback& require_callback);
    bool HasDrawableContent() const override;

private:
    ~SurfaceLayer() override;
    void CreateNewDestroySequence();
    void SatisfyDestroySequence();

    SurfaceId surface_id_;
    gfx::Size surface_size_;
    float surface_scale_;
    SurfaceSequence destroy_sequence_;
    SatisfyCallback satisfy_callback_;
    RequireCallback require_callback_;

    DISALLOW_COPY_AND_ASSIGN(SurfaceLayer);
};

} // namespace cc

#endif // CC_LAYERS_SURFACE_LAYER_H_
