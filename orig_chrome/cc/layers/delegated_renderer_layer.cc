// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/delegated_renderer_layer.h"

#include "cc/layers/delegated_renderer_layer_impl.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

scoped_refptr<DelegatedRendererLayer> DelegatedRendererLayer::Create(
    const LayerSettings& settings,
    const scoped_refptr<DelegatedFrameProvider>& frame_provider)
{
    return scoped_refptr<DelegatedRendererLayer>(
        new DelegatedRendererLayer(settings, frame_provider));
}

DelegatedRendererLayer::DelegatedRendererLayer(
    const LayerSettings& settings,
    const scoped_refptr<DelegatedFrameProvider>& frame_provider)
    : Layer(settings)
    , frame_provider_(frame_provider)
    , should_collect_new_frame_(true)
    , frame_data_(nullptr)
    , weak_ptrs_(this)
{
    frame_provider_->AddObserver(this);
}

DelegatedRendererLayer::~DelegatedRendererLayer()
{
    frame_provider_->RemoveObserver(this);
}

scoped_ptr<LayerImpl> DelegatedRendererLayer::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return DelegatedRendererLayerImpl::Create(tree_impl, layer_id_);
}

void DelegatedRendererLayer::SetLayerTreeHost(LayerTreeHost* host)
{
    if (layer_tree_host() == host) {
        Layer::SetLayerTreeHost(host);
        return;
    }

    if (!host) {
        // The active frame needs to be removed from the active tree and resources
        // returned before the commit is called complete.
        // TODO(danakj): Don't need to do this if the last frame commited was empty
        // or we never commited a frame with resources.
        SetNextCommitWaitsForActivation();
    } else {
        // There is no active frame in the new layer tree host to wait for so no
        // need to call SetNextCommitWaitsForActivation().
        should_collect_new_frame_ = true;
        SetNeedsUpdate();
    }

    Layer::SetLayerTreeHost(host);
}

void DelegatedRendererLayer::PushPropertiesTo(LayerImpl* impl)
{
    Layer::PushPropertiesTo(impl);

    DelegatedRendererLayerImpl* delegated_impl = static_cast<DelegatedRendererLayerImpl*>(impl);

    delegated_impl->CreateChildIdIfNeeded(
        frame_provider_->GetReturnResourcesCallbackForImplThread());

    if (frame_data_)
        delegated_impl->SetFrameData(frame_data_, frame_damage_);
    frame_data_ = nullptr;
    frame_damage_ = gfx::Rect();
}

void DelegatedRendererLayer::ProviderHasNewFrame()
{
    should_collect_new_frame_ = true;
    SetNeedsUpdate();
    // The active frame needs to be replaced and resources returned before the
    // commit is called complete.
    SetNextCommitWaitsForActivation();
}

bool DelegatedRendererLayer::Update()
{
    bool updated = Layer::Update();
    if (!should_collect_new_frame_)
        return updated;

    frame_data_ = frame_provider_->GetFrameDataAndRefResources(this, &frame_damage_);
    should_collect_new_frame_ = false;

    SetNeedsPushProperties();
    return true;
}

bool DelegatedRendererLayer::HasDelegatedContent() const
{
    return true;
}

} // namespace cc
