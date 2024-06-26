// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_DELEGATED_RENDERER_LAYER_IMPL_H_
#define CC_LAYERS_DELEGATED_RENDERER_LAYER_IMPL_H_

#include "base/containers/hash_tables.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/layers/layer_impl.h"

namespace cc {
class DelegatedFrameData;
class RenderPassSink;

class CC_EXPORT DelegatedRendererLayerImpl : public LayerImpl {
public:
    static scoped_ptr<DelegatedRendererLayerImpl> Create(
        LayerTreeImpl* tree_impl, int id)
    {
        return make_scoped_ptr(new DelegatedRendererLayerImpl(tree_impl, id));
    }
    ~DelegatedRendererLayerImpl() override;

    // LayerImpl overrides.
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    bool HasDelegatedContent() const override;
    bool HasContributingDelegatedRenderPasses() const override;
    RenderPassId FirstContributingRenderPassId() const override;
    RenderPassId NextContributingRenderPassId(
        RenderPassId previous) const override;
    void ReleaseResources() override;
    bool WillDraw(DrawMode draw_mode,
        ResourceProvider* resource_provider) override;
    void AppendQuads(RenderPass* render_pass,
        AppendQuadsData* append_quads_data) override;
    void PushPropertiesTo(LayerImpl* layer) override;

    void AppendContributingRenderPasses(RenderPassSink* render_pass_sink);

    // Creates an ID with the resource provider for the child renderer
    // that will be sending quads to the layer. Registers the callback to
    // inform when resources are no longer in use.
    void CreateChildIdIfNeeded(const ReturnCallback& return_callback);

    void SetFrameData(const DelegatedFrameData* frame_data,
        const gfx::Rect& damage_in_frame);

    float inverse_device_scale_factor() const
    {
        return inverse_device_scale_factor_;
    }

protected:
    DelegatedRendererLayerImpl(LayerTreeImpl* tree_impl, int id);

    int ChildIdForTesting() const { return child_id_; }
    const RenderPassList& RenderPassesInDrawOrderForTesting() const
    {
        return render_passes_in_draw_order_;
    }
    const ResourceProvider::ResourceIdSet& ResourcesForTesting() const
    {
        return resources_;
    }

private:
    void ClearChildId();

    void AppendRainbowDebugBorder(RenderPass* render_pass);

    void TakeOwnershipOfResourcesIfOnActiveTree(
        const ResourceProvider::ResourceIdSet& resources);
    void SetRenderPasses(RenderPassList* render_passes_in_draw_order);
    void ClearRenderPasses();

    // Returns |true| if the delegated_render_pass_id is part of the current
    // frame and can be converted.
    bool ConvertDelegatedRenderPassId(RenderPassId delegated_render_pass_id,
        RenderPassId* output_render_pass_id) const;

    void AppendRenderPassQuads(RenderPass* render_pass,
        const RenderPass* delegated_render_pass,
        const gfx::Size& frame_size) const;

    // LayerImpl overrides.
    const char* LayerTypeAsString() const override;

    bool have_render_passes_to_push_;
    float inverse_device_scale_factor_;
    RenderPassList render_passes_in_draw_order_;

    using RenderPassToIndexMap = base::hash_map<RenderPassId, size_t>;
    RenderPassToIndexMap render_passes_index_by_id_;
    ResourceProvider::ResourceIdSet resources_;

    int child_id_;
    bool own_child_id_;

    DISALLOW_COPY_AND_ASSIGN(DelegatedRendererLayerImpl);
};

} // namespace cc

#endif // CC_LAYERS_DELEGATED_RENDERER_LAYER_IMPL_H_
