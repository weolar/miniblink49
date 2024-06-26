// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_delegated_renderer_layer_impl.h"

#include "base/bind.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/draw_quad.h"
#include "cc/resources/returned_resource.h"
#include "cc/trees/layer_tree_impl.h"

namespace cc {

FakeDelegatedRendererLayerImpl::FakeDelegatedRendererLayerImpl(
    LayerTreeImpl* tree_impl,
    int id)
    : DelegatedRendererLayerImpl(tree_impl, id)
{
}

FakeDelegatedRendererLayerImpl::~FakeDelegatedRendererLayerImpl() { }

scoped_ptr<LayerImpl> FakeDelegatedRendererLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return FakeDelegatedRendererLayerImpl::Create(tree_impl, id());
}

ResourceProvider::ResourceIdSet FakeDelegatedRendererLayerImpl::Resources()
    const
{
    return ResourcesForTesting();
}

void NoopReturnCallback(const ReturnedResourceArray& returned,
    BlockingTaskRunner* main_thread_task_runner)
{
}

void FakeDelegatedRendererLayerImpl::SetFrameDataForRenderPasses(
    float device_scale_factor,
    const RenderPassList& pass_list)
{
    scoped_ptr<DelegatedFrameData> delegated_frame(new DelegatedFrameData);
    delegated_frame->device_scale_factor = device_scale_factor;
    RenderPass::CopyAll(pass_list, &delegated_frame->render_pass_list);

    ResourceProvider* resource_provider = layer_tree_impl()->resource_provider();

    for (const auto& pass : delegated_frame->render_pass_list) {
        for (const auto& quad : pass->quad_list) {
            for (ResourceId resource_id : quad->resources) {
                TransferableResource resource;
                resource.id = resource_id;
                resource.mailbox_holder.texture_target = resource_provider->TargetForTesting(resource_id);
                delegated_frame->resource_list.push_back(resource);
            }
        }
    }

    CreateChildIdIfNeeded(base::Bind(&NoopReturnCallback));
    SetFrameData(delegated_frame.get(), gfx::Rect());
}
} // namespace cc
