// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/delegated_renderer_layer_impl.h"

#include <algorithm>
#include <utility>

#include "base/bind.h"
#include "base/containers/hash_tables.h"
#include "cc/base/math_util.h"
#include "cc/layers/append_quads_data.h"
#include "cc/layers/render_pass_sink.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/occlusion.h"
#include "ui/gfx/geometry/rect_conversions.h"

namespace cc {

DelegatedRendererLayerImpl::DelegatedRendererLayerImpl(LayerTreeImpl* tree_impl,
    int id)
    : LayerImpl(tree_impl, id)
    , have_render_passes_to_push_(false)
    , inverse_device_scale_factor_(1.0f)
    , child_id_(0)
    , own_child_id_(false)
{
}

DelegatedRendererLayerImpl::~DelegatedRendererLayerImpl()
{
    ClearRenderPasses();
    ClearChildId();
}

bool DelegatedRendererLayerImpl::HasDelegatedContent() const { return true; }

bool DelegatedRendererLayerImpl::HasContributingDelegatedRenderPasses() const
{
    // The root RenderPass for the layer is merged with its target
    // RenderPass in each frame. So we only have extra RenderPasses
    // to merge when we have a non-root RenderPass present.
    return render_passes_in_draw_order_.size() > 1;
}

void DelegatedRendererLayerImpl::PushPropertiesTo(LayerImpl* layer)
{
    LayerImpl::PushPropertiesTo(layer);

    DelegatedRendererLayerImpl* delegated_layer = static_cast<DelegatedRendererLayerImpl*>(layer);

    // If we have a new child_id to give to the active layer, it should
    // have already deleted its old child_id.
    DCHECK(delegated_layer->child_id_ == 0 || delegated_layer->child_id_ == child_id_);
    delegated_layer->inverse_device_scale_factor_ = inverse_device_scale_factor_;
    delegated_layer->child_id_ = child_id_;
    delegated_layer->own_child_id_ = true;
    own_child_id_ = false;

    if (have_render_passes_to_push_) {
        DCHECK(child_id_);
        // This passes ownership of the render passes to the active tree.
        delegated_layer->SetRenderPasses(&render_passes_in_draw_order_);
        // Once resources are on the active tree, give them to the ResourceProvider
        // and release unused resources from the old frame.
        delegated_layer->TakeOwnershipOfResourcesIfOnActiveTree(resources_);
        DCHECK(render_passes_in_draw_order_.empty());
        have_render_passes_to_push_ = false;
    }

    // This is just a copy for testing, since resources are added to the
    // ResourceProvider in the pending tree.
    delegated_layer->resources_ = resources_;
}

void DelegatedRendererLayerImpl::CreateChildIdIfNeeded(
    const ReturnCallback& return_callback)
{
    if (child_id_)
        return;

    ResourceProvider* resource_provider = layer_tree_impl()->resource_provider();
    child_id_ = resource_provider->CreateChild(return_callback);
    own_child_id_ = true;
}

void DelegatedRendererLayerImpl::SetFrameData(
    const DelegatedFrameData* frame_data,
    const gfx::Rect& damage_in_frame)
{
    DCHECK(child_id_) << "CreateChildIdIfNeeded must be called first.";
    DCHECK(frame_data);
    DCHECK(!frame_data->render_pass_list.empty());
    // A frame with an empty root render pass is invalid.
    DCHECK(!frame_data->render_pass_list.back()->output_rect.IsEmpty());

    ResourceProvider* resource_provider = layer_tree_impl()->resource_provider();
    const ResourceProvider::ResourceIdMap& resource_map = resource_provider->GetChildToParentMap(child_id_);

    resource_provider->ReceiveFromChild(child_id_, frame_data->resource_list);

    RenderPassList render_pass_list;
    RenderPass::CopyAll(frame_data->render_pass_list, &render_pass_list);

    bool invalid_frame = false;
    ResourceProvider::ResourceIdSet resources_in_frame;
    size_t reserve_size = frame_data->resource_list.size();
    // #if defined(COMPILER_MSVC)
    //   resources_in_frame.reserve(reserve_size);
    // #elif defined(COMPILER_GCC)
    //   // Pre-standard hash-tables only implement resize, which behaves similarly
    //   // to reserve for these keys. Resizing to 0 may also be broken (particularly
    //   // on stlport).
    //   // TODO(jbauman): Replace with reserve when C++11 is supported everywhere.
    //   if (reserve_size)
    //     resources_in_frame.resize(reserve_size);
    // #endif
    DebugBreak();

    for (const auto& pass : render_pass_list) {
        for (const auto& quad : pass->quad_list) {
            for (ResourceId& resource_id : quad->resources) {
                ResourceProvider::ResourceIdMap::const_iterator it = resource_map.find(resource_id);
                if (it == resource_map.end()) {
                    invalid_frame = true;
                    break;
                }

                DCHECK_EQ(it->first, resource_id);
                ResourceId remapped_id = it->second;
                resources_in_frame.insert(resource_id);
                resource_id = remapped_id;
            }
        }
    }

    if (invalid_frame) {
        // Declare we are still using the last frame's resources. Drops ownership of
        // any invalid resources, keeping only those in use by the active tree.
        resource_provider->DeclareUsedResourcesFromChild(child_id_, resources_);
        return;
    }

    // Save the new frame's resources, but don't give them to the ResourceProvider
    // until they are active, since the resources on the active tree will still be
    // used and we don't want to return them early.
    resources_.swap(resources_in_frame);
    TakeOwnershipOfResourcesIfOnActiveTree(resources_);

    inverse_device_scale_factor_ = 1.0f / frame_data->device_scale_factor;
    // Display size is already set so we can compute what the damage rect
    // will be in layer space. The damage may exceed the visible portion of
    // the frame, so intersect the damage to the layer's bounds.
    RenderPass* new_root_pass = render_pass_list.back();
    gfx::Size frame_size = new_root_pass->output_rect.size();
    gfx::Rect damage_in_layer = gfx::ScaleToEnclosingRect(damage_in_frame, inverse_device_scale_factor_);
    SetUpdateRect(gfx::IntersectRects(
        gfx::UnionRects(update_rect(), damage_in_layer), gfx::Rect(bounds())));

    SetRenderPasses(&render_pass_list);
    have_render_passes_to_push_ = true;
}

void DelegatedRendererLayerImpl::TakeOwnershipOfResourcesIfOnActiveTree(
    const ResourceProvider::ResourceIdSet& resources)
{
    DCHECK(child_id_);
    if (!layer_tree_impl()->IsActiveTree())
        return;
    layer_tree_impl()->resource_provider()->DeclareUsedResourcesFromChild(
        child_id_, resources);
}

void DelegatedRendererLayerImpl::SetRenderPasses(
    RenderPassList* render_passes_in_draw_order)
{
    ClearRenderPasses();

    for (size_t i = 0; i < render_passes_in_draw_order->size(); ++i) {
        RenderPassList::iterator to_take = render_passes_in_draw_order->begin() + i;
        render_passes_index_by_id_.insert(
            RenderPassToIndexMap::value_type((*to_take)->id, i));
        scoped_ptr<RenderPass> taken_render_pass = render_passes_in_draw_order->take(to_take);
        render_passes_in_draw_order_.push_back(taken_render_pass.Pass());
    }

    // Give back an empty array instead of nulls.
    render_passes_in_draw_order->clear();

    // The render passes given here become part of the RenderSurfaceLayerList, so
    // changing them requires recomputing the RenderSurfaceLayerList.
    layer_tree_impl()->set_needs_update_draw_properties();
}

void DelegatedRendererLayerImpl::ClearRenderPasses()
{
    render_passes_index_by_id_.clear();
    render_passes_in_draw_order_.clear();
}

scoped_ptr<LayerImpl> DelegatedRendererLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return DelegatedRendererLayerImpl::Create(tree_impl, id());
}

void DelegatedRendererLayerImpl::ReleaseResources()
{
    ClearRenderPasses();
    ClearChildId();
    have_render_passes_to_push_ = false;
}

static inline size_t IndexToId(size_t index)
{
    return index + 1;
}
static inline size_t IdToIndex(size_t id)
{
    DCHECK_GT(id, 0u);
    return id - 1;
}

RenderPassId DelegatedRendererLayerImpl::FirstContributingRenderPassId() const
{
    return RenderPassId(id(), IndexToId(0));
}

RenderPassId DelegatedRendererLayerImpl::NextContributingRenderPassId(
    RenderPassId previous) const
{
    return RenderPassId(previous.layer_id, previous.index + 1);
}

bool DelegatedRendererLayerImpl::ConvertDelegatedRenderPassId(
    RenderPassId delegated_render_pass_id,
    RenderPassId* output_render_pass_id) const
{
    RenderPassToIndexMap::const_iterator found = render_passes_index_by_id_.find(delegated_render_pass_id);
    if (found == render_passes_index_by_id_.end()) {
        // Be robust against a RenderPass id that isn't part of the frame.
        return false;
    }
    size_t delegated_render_pass_index = found->second;
    *output_render_pass_id = RenderPassId(id(), IndexToId(delegated_render_pass_index));
    return true;
}

void DelegatedRendererLayerImpl::AppendContributingRenderPasses(
    RenderPassSink* render_pass_sink)
{
    DCHECK(HasContributingDelegatedRenderPasses());

    const RenderPass* root_delegated_render_pass = render_passes_in_draw_order_.back();
    gfx::Size frame_size = root_delegated_render_pass->output_rect.size();
    gfx::Transform delegated_frame_to_root_transform = screen_space_transform();
    delegated_frame_to_root_transform.Scale(inverse_device_scale_factor_,
        inverse_device_scale_factor_);

    for (size_t i = 0; i < render_passes_in_draw_order_.size() - 1; ++i) {
        RenderPassId output_render_pass_id;
        bool present = ConvertDelegatedRenderPassId(render_passes_in_draw_order_[i]->id,
            &output_render_pass_id);

        // Don't clash with the RenderPass we generate if we own a RenderSurface.
        DCHECK(present) << render_passes_in_draw_order_[i]->id.layer_id << ", "
                        << render_passes_in_draw_order_[i]->id.index;
        DCHECK_GT(output_render_pass_id.index, 0u);

        scoped_ptr<RenderPass> copy_pass = render_passes_in_draw_order_[i]->Copy(output_render_pass_id);
        copy_pass->transform_to_root_target.ConcatTransform(
            delegated_frame_to_root_transform);
        render_pass_sink->AppendRenderPass(copy_pass.Pass());
    }
}

bool DelegatedRendererLayerImpl::WillDraw(DrawMode draw_mode,
    ResourceProvider* resource_provider)
{
    if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE)
        return false;
    return LayerImpl::WillDraw(draw_mode, resource_provider);
}

void DelegatedRendererLayerImpl::AppendQuads(
    RenderPass* render_pass,
    AppendQuadsData* append_quads_data)
{
    AppendRainbowDebugBorder(render_pass);

    // This list will be empty after a lost context until a new frame arrives.
    if (render_passes_in_draw_order_.empty())
        return;

    RenderPassId target_render_pass_id = render_pass->id;

    const RenderPass* root_delegated_render_pass = render_passes_in_draw_order_.back();

    DCHECK(root_delegated_render_pass->output_rect.origin().IsOrigin());
    gfx::Size frame_size = root_delegated_render_pass->output_rect.size();

    // If the index of the RenderPassId is 0, then it is a RenderPass generated
    // for a layer in this compositor, not the delegating renderer. Then we want
    // to merge our root RenderPass with the target RenderPass. Otherwise, it is
    // some RenderPass which we added from the delegating renderer.
    bool should_merge_root_render_pass_with_target = !target_render_pass_id.index;
    if (should_merge_root_render_pass_with_target) {
        // Verify that the RenderPass we are appending to is created by our
        // render_target.
        DCHECK(target_render_pass_id.layer_id == render_target()->id());

        AppendRenderPassQuads(render_pass,
            root_delegated_render_pass,
            frame_size);
    } else {
        // Verify that the RenderPass we are appending to was created by us.
        DCHECK(target_render_pass_id.layer_id == id());

        size_t render_pass_index = IdToIndex(target_render_pass_id.index);
        const RenderPass* delegated_render_pass = render_passes_in_draw_order_[render_pass_index];
        AppendRenderPassQuads(render_pass,
            delegated_render_pass,
            frame_size);
    }
}

void DelegatedRendererLayerImpl::AppendRainbowDebugBorder(
    RenderPass* render_pass)
{
    if (!ShowDebugBorders())
        return;

    SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
    PopulateSharedQuadState(shared_quad_state);

    SkColor color;
    float border_width;
    GetDebugBorderProperties(&color, &border_width);

    SkColor colors[] = {
        0x80ff0000, // Red.
        0x80ffa500, // Orange.
        0x80ffff00, // Yellow.
        0x80008000, // Green.
        0x800000ff, // Blue.
        0x80ee82ee, // Violet.
    };
    const int kNumColors = arraysize(colors);

    const int kStripeWidth = 300;
    const int kStripeHeight = 300;

    for (int i = 0;; ++i) {
        // For horizontal lines.
        int x = kStripeWidth * i;
        int width = std::min(kStripeWidth, bounds().width() - x - 1);

        // For vertical lines.
        int y = kStripeHeight * i;
        int height = std::min(kStripeHeight, bounds().height() - y - 1);

        gfx::Rect top(x, 0, width, border_width);
        gfx::Rect bottom(x, bounds().height() - border_width, width, border_width);
        gfx::Rect left(0, y, border_width, height);
        gfx::Rect right(bounds().width() - border_width, y, border_width, height);

        if (top.IsEmpty() && left.IsEmpty())
            break;

        if (!top.IsEmpty()) {
            bool force_anti_aliasing_off = false;
            SolidColorDrawQuad* top_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            top_quad->SetNew(shared_quad_state, top, top, colors[i % kNumColors],
                force_anti_aliasing_off);

            SolidColorDrawQuad* bottom_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            bottom_quad->SetNew(shared_quad_state, bottom, bottom,
                colors[kNumColors - 1 - (i % kNumColors)],
                force_anti_aliasing_off);

            if (contents_opaque()) {
                // Draws a stripe filling the layer vertically with the same color and
                // width as the horizontal stipes along the layer's top border.
                SolidColorDrawQuad* solid_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
                // The inner fill is more transparent then the border.
                static const float kFillOpacity = 0.1f;
                SkColor fill_color = SkColorSetA(
                    colors[i % kNumColors],
                    static_cast<uint8_t>(SkColorGetA(colors[i % kNumColors]) * kFillOpacity));
                gfx::Rect fill_rect(x, 0, width, bounds().height());
                solid_quad->SetNew(shared_quad_state, fill_rect, fill_rect, fill_color,
                    force_anti_aliasing_off);
            }
        }
        if (!left.IsEmpty()) {
            bool force_anti_aliasing_off = false;
            SolidColorDrawQuad* left_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            left_quad->SetNew(shared_quad_state, left, left,
                colors[kNumColors - 1 - (i % kNumColors)],
                force_anti_aliasing_off);

            SolidColorDrawQuad* right_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            right_quad->SetNew(shared_quad_state, right, right,
                colors[i % kNumColors], force_anti_aliasing_off);
        }
    }
}

void DelegatedRendererLayerImpl::AppendRenderPassQuads(
    RenderPass* render_pass,
    const RenderPass* delegated_render_pass,
    const gfx::Size& frame_size) const
{
    const SharedQuadState* delegated_shared_quad_state = nullptr;
    SharedQuadState* output_shared_quad_state = nullptr;

    gfx::Transform delegated_frame_to_target_transform = draw_transform();
    delegated_frame_to_target_transform.Scale(inverse_device_scale_factor_,
        inverse_device_scale_factor_);
    bool is_root_delegated_render_pass = delegated_render_pass == render_passes_in_draw_order_.back();
    for (const auto& delegated_quad : delegated_render_pass->quad_list) {
        if (delegated_quad->shared_quad_state != delegated_shared_quad_state) {
            delegated_shared_quad_state = delegated_quad->shared_quad_state;
            output_shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
            output_shared_quad_state->CopyFrom(delegated_shared_quad_state);

            if (is_root_delegated_render_pass) {
                output_shared_quad_state->quad_to_target_transform.ConcatTransform(
                    delegated_frame_to_target_transform);

                if (render_target() == this) {
                    DCHECK(!is_clipped());
                    DCHECK(render_surface());
                    DCHECK_EQ(0u, num_unclipped_descendants());
                    output_shared_quad_state->clip_rect = MathUtil::MapEnclosingClippedRect(
                        delegated_frame_to_target_transform,
                        output_shared_quad_state->clip_rect);
                } else {
                    gfx::Rect clip_rect = drawable_content_rect();
                    if (output_shared_quad_state->is_clipped) {
                        clip_rect.Intersect(MathUtil::MapEnclosingClippedRect(
                            delegated_frame_to_target_transform,
                            output_shared_quad_state->clip_rect));
                    }
                    output_shared_quad_state->clip_rect = clip_rect;
                    output_shared_quad_state->is_clipped = true;
                }

                output_shared_quad_state->opacity *= draw_opacity();
            }
        }
        DCHECK(output_shared_quad_state);

        gfx::Transform quad_content_to_delegated_target_space = output_shared_quad_state->quad_to_target_transform;
        if (!is_root_delegated_render_pass) {
            quad_content_to_delegated_target_space.ConcatTransform(
                delegated_render_pass->transform_to_root_target);
            quad_content_to_delegated_target_space.ConcatTransform(
                delegated_frame_to_target_transform);
        }

        Occlusion occlusion_in_quad_space = draw_properties()
                                                .occlusion_in_content_space.GetOcclusionWithGivenDrawTransform(
                                                    quad_content_to_delegated_target_space);

        gfx::Rect quad_visible_rect = occlusion_in_quad_space.GetUnoccludedContentRect(
            delegated_quad->visible_rect);

        if (quad_visible_rect.IsEmpty())
            continue;

        if (delegated_quad->material != DrawQuad::RENDER_PASS) {
            DrawQuad* output_quad = render_pass->CopyFromAndAppendDrawQuad(
                delegated_quad, output_shared_quad_state);
            output_quad->visible_rect = quad_visible_rect;
            ValidateQuadResources(output_quad);
        } else {
            RenderPassId delegated_contributing_render_pass_id = RenderPassDrawQuad::MaterialCast(delegated_quad)->render_pass_id;
            RenderPassId output_contributing_render_pass_id;

            bool present = ConvertDelegatedRenderPassId(delegated_contributing_render_pass_id,
                &output_contributing_render_pass_id);
            // |present| being false means the child compositor sent an invalid frame.
            DCHECK(present);
            DCHECK(output_contributing_render_pass_id != render_pass->id);

            RenderPassDrawQuad* output_quad = render_pass->CopyFromAndAppendRenderPassDrawQuad(
                RenderPassDrawQuad::MaterialCast(delegated_quad),
                output_shared_quad_state, output_contributing_render_pass_id);
            output_quad->visible_rect = quad_visible_rect;
            ValidateQuadResources(output_quad);
        }
    }
}

const char* DelegatedRendererLayerImpl::LayerTypeAsString() const
{
    return "cc::DelegatedRendererLayerImpl";
}

void DelegatedRendererLayerImpl::ClearChildId()
{
    if (!child_id_)
        return;

    if (own_child_id_) {
        ResourceProvider* provider = layer_tree_impl()->resource_provider();
        provider->DestroyChild(child_id_);
    }

    resources_.clear();
    child_id_ = 0;
}

} // namespace cc
