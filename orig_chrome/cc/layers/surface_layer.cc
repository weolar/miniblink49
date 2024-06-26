// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/surface_layer.h"

#include "cc/layers/surface_layer_impl.h"
#include "cc/output/swap_promise.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

class SatisfySwapPromise : public SwapPromise {
public:
    SatisfySwapPromise(SurfaceSequence sequence,
        const SurfaceLayer::SatisfyCallback& satisfy_callback)
        : sequence_(sequence)
        , satisfy_callback_(satisfy_callback)
    {
    }

    ~SatisfySwapPromise() override { }

private:
    void DidActivate() override { }
    void DidSwap(CompositorFrameMetadata* metadata) override
    {
        metadata->satisfies_sequences.push_back(sequence_.sequence);
    }

    void DidNotSwap(DidNotSwapReason reason) override
    {
        satisfy_callback_.Run(sequence_);
    }
    int64 TraceId() const override { return 0; }

    SurfaceSequence sequence_;
    SurfaceLayer::SatisfyCallback satisfy_callback_;

    DISALLOW_COPY_AND_ASSIGN(SatisfySwapPromise);
};

scoped_refptr<SurfaceLayer> SurfaceLayer::Create(
    const LayerSettings& settings,
    const SatisfyCallback& satisfy_callback,
    const RequireCallback& require_callback)
{
    return make_scoped_refptr(
        new SurfaceLayer(settings, satisfy_callback, require_callback));
}

SurfaceLayer::SurfaceLayer(const LayerSettings& settings,
    const SatisfyCallback& satisfy_callback,
    const RequireCallback& require_callback)
    : Layer(settings)
    , surface_scale_(1.f)
    , satisfy_callback_(satisfy_callback)
    , require_callback_(require_callback)
{
}

SurfaceLayer::~SurfaceLayer()
{
    DCHECK(!layer_tree_host());
    DCHECK(destroy_sequence_.is_null());
}

void SurfaceLayer::SetSurfaceId(SurfaceId surface_id,
    float scale,
    const gfx::Size& size)
{
    SatisfyDestroySequence();
    surface_id_ = surface_id;
    surface_size_ = size;
    surface_scale_ = scale;
    CreateNewDestroySequence();

    UpdateDrawsContent(HasDrawableContent());
    SetNeedsPushProperties();
}

scoped_ptr<LayerImpl> SurfaceLayer::CreateLayerImpl(LayerTreeImpl* tree_impl)
{
    return SurfaceLayerImpl::Create(tree_impl, id());
}

bool SurfaceLayer::HasDrawableContent() const
{
    return !surface_id_.is_null() && Layer::HasDrawableContent();
}

void SurfaceLayer::SetLayerTreeHost(LayerTreeHost* host)
{
    if (layer_tree_host() == host) {
        Layer::SetLayerTreeHost(host);
        return;
    }

    SatisfyDestroySequence();
    Layer::SetLayerTreeHost(host);
    CreateNewDestroySequence();
}

void SurfaceLayer::PushPropertiesTo(LayerImpl* layer)
{
    Layer::PushPropertiesTo(layer);
    SurfaceLayerImpl* layer_impl = static_cast<SurfaceLayerImpl*>(layer);

    layer_impl->SetSurfaceId(surface_id_);
    layer_impl->SetSurfaceSize(surface_size_);
    layer_impl->SetSurfaceScale(surface_scale_);
}

void SurfaceLayer::CreateNewDestroySequence()
{
    DCHECK(destroy_sequence_.is_null());
    if (layer_tree_host()) {
        destroy_sequence_ = layer_tree_host()->CreateSurfaceSequence();
        require_callback_.Run(surface_id_, destroy_sequence_);
    }
}

void SurfaceLayer::SatisfyDestroySequence()
{
    if (!layer_tree_host())
        return;
    DCHECK(!destroy_sequence_.is_null());
    scoped_ptr<SatisfySwapPromise> satisfy(
        new SatisfySwapPromise(destroy_sequence_, satisfy_callback_));
    layer_tree_host()->QueueSwapPromise(satisfy.Pass());
    destroy_sequence_ = SurfaceSequence();
}

} // namespace cc
