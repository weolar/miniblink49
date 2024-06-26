// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/delegated_frame_provider.h"

#include "cc/layers/delegated_frame_resource_collection.h"
#include "cc/layers/delegated_renderer_layer.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/render_pass_draw_quad.h"

namespace cc {

DelegatedFrameProvider::DelegatedFrameProvider(
    const scoped_refptr<DelegatedFrameResourceCollection>& resource_collection,
    scoped_ptr<DelegatedFrameData> frame)
    : resource_collection_(resource_collection)
{
    RenderPass* root_pass = frame->render_pass_list.back();
    frame_size_ = root_pass->output_rect.size();
    DCHECK(!frame_size_.IsEmpty());
    SetFrameData(frame.Pass());
}

DelegatedFrameProvider::~DelegatedFrameProvider()
{
    ReturnedResourceArray returned;
    TransferableResource::ReturnResources(frame_->resource_list, &returned);
    resource_collection_->UnrefResources(returned);
}

void DelegatedFrameProvider::AddObserver(DelegatedRendererLayer* layer)
{
#if DCHECK_IS_ON()
    for (size_t i = 0; i < observers_.size(); ++i)
        DCHECK(observers_[i].layer != layer);
#endif

    observers_.push_back(Observer(layer, gfx::Rect(frame_size_)));

    DCHECK(frame_) << "Must have a frame when given to a DelegatedRendererLayer.";
}

void DelegatedFrameProvider::RemoveObserver(DelegatedRendererLayer* layer)
{
    bool found_observer = false;
    for (size_t i = 0; i < observers_.size(); ++i) {
        if (observers_[i].layer != layer)
            continue;
        observers_.erase(observers_.begin() + i);
        found_observer = true;
        break;
    }
    DCHECK(found_observer);
}

void DelegatedFrameProvider::SetFrameData(
    scoped_ptr<DelegatedFrameData> frame)
{
    DCHECK(frame);
    DCHECK_NE(0u, frame->render_pass_list.size());

    if (frame_) {
        ReturnedResourceArray returned;
        TransferableResource::ReturnResources(frame_->resource_list, &returned);
        resource_collection_->UnrefResources(returned);
    }

    frame_ = frame.Pass();

    resource_collection_->ReceivedResources(frame_->resource_list);
    resource_collection_->RefResources(frame_->resource_list);

    RenderPass* root_pass = frame_->render_pass_list.back();
    DCHECK_EQ(frame_size_.ToString(), root_pass->output_rect.size().ToString())
        << "All frames in a single DelegatedFrameProvider must have the same "
        << "size. Use a new frame provider for frames of a different size.";

    for (size_t i = 0; i < observers_.size(); ++i) {
        observers_[i].damage = gfx::UnionRects(observers_[i].damage, root_pass->damage_rect);
        observers_[i].layer->ProviderHasNewFrame();
    }
}

DelegatedFrameData* DelegatedFrameProvider::GetFrameDataAndRefResources(
    DelegatedRendererLayer* observer,
    gfx::Rect* damage)
{
    bool found_observer = false;
    for (size_t i = 0; i < observers_.size(); ++i) {
        if (observers_[i].layer != observer)
            continue;
        *damage = observers_[i].damage;
        // The observer is now responsible for the damage.
        observers_[i].damage = gfx::Rect();
        found_observer = true;
    }
    DCHECK(found_observer);

    resource_collection_->RefResources(frame_->resource_list);
    return frame_.get();
}

ReturnCallback
DelegatedFrameProvider::GetReturnResourcesCallbackForImplThread()
{
    return resource_collection_->GetReturnResourcesCallbackForImplThread();
}

void DelegatedFrameProvider::UnrefResourcesOnMainThread(
    const ReturnedResourceArray& returned)
{
    resource_collection_->UnrefResources(returned);
}

} // namespace cc
