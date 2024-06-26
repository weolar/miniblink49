// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/layer_tree_impl.h"

#include <algorithm>
#include <limits>
#include <set>

#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/animation/animation_host.h"
#include "cc/animation/keyframed_animation_curve.h"
#include "cc/animation/scrollbar_animation_controller.h"
#include "cc/animation/scrollbar_animation_controller_linear_fade.h"
#include "cc/animation/scrollbar_animation_controller_thinning.h"
#include "cc/base/math_util.h"
#include "cc/base/synced_property.h"
#include "cc/debug/devtools_instrumentation.h"
#include "cc/debug/traced_value.h"
#include "cc/input/page_scale_animation.h"
#include "cc/layers/heads_up_display_layer_impl.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_iterator.h"
#include "cc/layers/render_surface_impl.h"
#include "cc/layers/scrollbar_layer_impl_base.h"
#include "cc/resources/ui_resource_request.h"
#include "cc/trees/layer_tree_host_common.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/occlusion_tracker.h"
#include "cc/trees/property_tree.h"
#include "cc/trees/property_tree_builder.h"
#include "ui/gfx/geometry/box_f.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/geometry/vector2d_conversions.h"

namespace cc {

LayerTreeImpl::LayerTreeImpl(
    LayerTreeHostImpl* layer_tree_host_impl,
    scoped_refptr<SyncedProperty<ScaleGroup>> page_scale_factor,
    scoped_refptr<SyncedTopControls> top_controls_shown_ratio,
    scoped_refptr<SyncedElasticOverscroll> elastic_overscroll)
    : layer_tree_host_impl_(layer_tree_host_impl)
    , source_frame_number_(-1)
    , hud_layer_(0)
    , background_color_(0)
    , has_transparent_background_(false)
    , currently_scrolling_layer_id_(Layer::INVALID_ID)
    , overscroll_elasticity_layer_id_(Layer::INVALID_ID)
    , page_scale_layer_id_(Layer::INVALID_ID)
    , inner_viewport_scroll_layer_id_(Layer::INVALID_ID)
    , outer_viewport_scroll_layer_id_(Layer::INVALID_ID)
    , page_scale_factor_(page_scale_factor)
    , min_page_scale_factor_(0)
    , max_page_scale_factor_(0)
    , hide_pinch_scrollbars_near_min_scale_(false)
    , device_scale_factor_(1.f)
    , elastic_overscroll_(elastic_overscroll)
    , viewport_size_invalid_(false)
    , needs_update_draw_properties_(true)
    , needs_full_tree_sync_(true)
    , next_activation_forces_redraw_(false)
    , has_ever_been_drawn_(false)
    , render_surface_layer_list_id_(0)
    , top_controls_shrink_blink_size_(false)
    , top_controls_height_(0)
    , top_controls_shown_ratio_(top_controls_shown_ratio)
{
}

LayerTreeImpl::~LayerTreeImpl()
{
    BreakSwapPromises(IsActiveTree() ? SwapPromise::SWAP_FAILS
                                     : SwapPromise::ACTIVATION_FAILS);

    // Need to explicitly clear the tree prior to destroying this so that
    // the LayerTreeImpl pointer is still valid in the LayerImpl dtor.
    DCHECK(!root_layer_);
    DCHECK(layers_with_copy_output_request_.empty());
}

void LayerTreeImpl::Shutdown()
{
    root_layer_ = nullptr;
}

void LayerTreeImpl::ReleaseResources()
{
    if (root_layer_) {
        LayerTreeHostCommon::CallFunctionForSubtree(
            root_layer_.get(), [](LayerImpl* layer) { layer->ReleaseResources(); });
    }
}

void LayerTreeImpl::RecreateResources()
{
    if (root_layer_) {
        LayerTreeHostCommon::CallFunctionForSubtree(
            root_layer_.get(),
            [](LayerImpl* layer) { layer->RecreateResources(); });
    }
}

void LayerTreeImpl::GatherFrameTimingRequestIds(
    std::vector<int64_t>* request_ids)
{
    if (!root_layer_)
        return;

    // TODO(vmpstr): Early out if there are no requests on any of the layers. For
    // that, we need to inform LayerTreeImpl whenever there are requests when we
    // get them.
    LayerTreeHostCommon::CallFunctionForSubtree(
        root_layer_.get(), [request_ids](LayerImpl* layer) {
            layer->GatherFrameTimingRequestIds(request_ids);
        });
}

void LayerTreeImpl::SetRootLayer(scoped_ptr<LayerImpl> layer)
{
    root_layer_ = layer.Pass();

    layer_tree_host_impl_->OnCanDrawStateChangedForTree();
}

LayerImpl* LayerTreeImpl::InnerViewportScrollLayer() const
{
    return LayerById(inner_viewport_scroll_layer_id_);
}

LayerImpl* LayerTreeImpl::OuterViewportScrollLayer() const
{
    return LayerById(outer_viewport_scroll_layer_id_);
}

gfx::ScrollOffset LayerTreeImpl::TotalScrollOffset() const
{
    gfx::ScrollOffset offset;

    if (InnerViewportScrollLayer())
        offset += InnerViewportScrollLayer()->CurrentScrollOffset();

    if (OuterViewportScrollLayer())
        offset += OuterViewportScrollLayer()->CurrentScrollOffset();

    return offset;
}

gfx::ScrollOffset LayerTreeImpl::TotalMaxScrollOffset() const
{
    gfx::ScrollOffset offset;

    if (InnerViewportScrollLayer())
        offset += InnerViewportScrollLayer()->MaxScrollOffset();

    if (OuterViewportScrollLayer())
        offset += OuterViewportScrollLayer()->MaxScrollOffset();

    return offset;
}

scoped_ptr<LayerImpl> LayerTreeImpl::DetachLayerTree()
{
    render_surface_layer_list_.clear();
    set_needs_update_draw_properties();
    return root_layer_.Pass();
}

static void UpdateClipTreeForBoundsDeltaOnLayer(LayerImpl* layer,
    ClipTree* clip_tree)
{
    if (layer && layer->masks_to_bounds()) {
        ClipNode* clip_node = clip_tree->Node(layer->clip_tree_index());
        if (clip_node) {
            DCHECK_EQ(layer->id(), clip_node->owner_id);
            gfx::SizeF bounds = gfx::SizeF(layer->bounds());
            if (clip_node->data.clip.size() != bounds) {
                clip_node->data.clip.set_size(bounds);
                clip_tree->set_needs_update(true);
            }
        }
    }
}

void LayerTreeImpl::UpdatePropertyTreesForBoundsDelta()
{
    DCHECK(IsActiveTree());
    LayerImpl* inner_container = InnerViewportContainerLayer();
    LayerImpl* outer_container = OuterViewportContainerLayer();

    UpdateClipTreeForBoundsDeltaOnLayer(inner_container,
        &property_trees_.clip_tree);
    UpdateClipTreeForBoundsDeltaOnLayer(InnerViewportScrollLayer(),
        &property_trees_.clip_tree);
    UpdateClipTreeForBoundsDeltaOnLayer(outer_container,
        &property_trees_.clip_tree);

    TransformTree& transform_tree = property_trees_.transform_tree;
    if (inner_container)
        transform_tree.SetInnerViewportBoundsDelta(inner_container->bounds_delta());
    if (outer_container)
        transform_tree.SetOuterViewportBoundsDelta(outer_container->bounds_delta());
}

void LayerTreeImpl::PushPropertiesTo(LayerTreeImpl* target_tree)
{
    // The request queue should have been processed and does not require a push.
    DCHECK_EQ(ui_resource_request_queue_.size(), 0u);

    target_tree->SetPropertyTrees(property_trees_);

    if (next_activation_forces_redraw_) {
        target_tree->ForceRedrawNextActivation();
        next_activation_forces_redraw_ = false;
    }

    target_tree->PassSwapPromises(&swap_promise_list_);

    target_tree->set_top_controls_shrink_blink_size(
        top_controls_shrink_blink_size_);
    target_tree->set_top_controls_height(top_controls_height_);
    target_tree->PushTopControls(nullptr);

    target_tree->set_hide_pinch_scrollbars_near_min_scale(
        hide_pinch_scrollbars_near_min_scale_);

    // Active tree already shares the page_scale_factor object with pending
    // tree so only the limits need to be provided.
    target_tree->PushPageScaleFactorAndLimits(nullptr, min_page_scale_factor(),
        max_page_scale_factor());
    target_tree->SetDeviceScaleFactor(device_scale_factor());
    target_tree->elastic_overscroll()->PushPendingToActive();

    target_tree->pending_page_scale_animation_ = pending_page_scale_animation_.Pass();

    target_tree->SetViewportLayersFromIds(
        overscroll_elasticity_layer_id_, page_scale_layer_id_,
        inner_viewport_scroll_layer_id_, outer_viewport_scroll_layer_id_);

    target_tree->RegisterSelection(selection_);

    // This should match the property synchronization in
    // LayerTreeHost::finishCommitOnImplThread().
    target_tree->set_source_frame_number(source_frame_number());
    target_tree->set_background_color(background_color());
    target_tree->set_has_transparent_background(has_transparent_background());

    if (ViewportSizeInvalid())
        target_tree->SetViewportSizeInvalid();
    else
        target_tree->ResetViewportSizeInvalid();

    if (hud_layer())
        target_tree->set_hud_layer(static_cast<HeadsUpDisplayLayerImpl*>(
            LayerTreeHostCommon::FindLayerInSubtree(
                target_tree->root_layer(), hud_layer()->id())));
    else
        target_tree->set_hud_layer(NULL);

    target_tree->has_ever_been_drawn_ = false;
}

LayerImpl* LayerTreeImpl::InnerViewportContainerLayer() const
{
    return InnerViewportScrollLayer()
        ? InnerViewportScrollLayer()->scroll_clip_layer()
        : NULL;
}

LayerImpl* LayerTreeImpl::OuterViewportContainerLayer() const
{
    return OuterViewportScrollLayer()
        ? OuterViewportScrollLayer()->scroll_clip_layer()
        : NULL;
}

LayerImpl* LayerTreeImpl::CurrentlyScrollingLayer() const
{
    DCHECK(IsActiveTree());
    return LayerById(currently_scrolling_layer_id_);
}

void LayerTreeImpl::SetCurrentlyScrollingLayer(LayerImpl* layer)
{
    int new_id = layer ? layer->id() : Layer::INVALID_ID;
    if (currently_scrolling_layer_id_ == new_id)
        return;

    if (CurrentlyScrollingLayer() && CurrentlyScrollingLayer()->scrollbar_animation_controller())
        CurrentlyScrollingLayer()->scrollbar_animation_controller()->DidScrollEnd();
    currently_scrolling_layer_id_ = new_id;
    if (layer && layer->scrollbar_animation_controller())
        layer->scrollbar_animation_controller()->DidScrollBegin();
}

void LayerTreeImpl::ClearCurrentlyScrollingLayer()
{
    SetCurrentlyScrollingLayer(NULL);
}

namespace {

    void ForceScrollbarParameterUpdateAfterScaleChange(LayerImpl* current_layer)
    {
        if (!current_layer)
            return;

        while (current_layer) {
            current_layer->ScrollbarParametersDidChange(false);
            current_layer = current_layer->parent();
        }
    }

} // namespace

float LayerTreeImpl::ClampPageScaleFactorToLimits(
    float page_scale_factor) const
{
    if (min_page_scale_factor_ && page_scale_factor < min_page_scale_factor_)
        page_scale_factor = min_page_scale_factor_;
    else if (max_page_scale_factor_ && page_scale_factor > max_page_scale_factor_)
        page_scale_factor = max_page_scale_factor_;
    return page_scale_factor;
}

void LayerTreeImpl::UpdatePropertyTreeScrollingAndAnimationFromMainThread()
{
    // TODO(enne): This should get replaced by pulling out scrolling and
    // animations into their own trees.  Then scrolls and animations would have
    // their own ways of synchronizing across commits.  This occurs to push
    // updates from scrolling deltas on the compositor thread that have occurred
    // after begin frame and updates from animations that have ticked since begin
    // frame to a newly-committed property tree.
    if (!root_layer())
        return;
    LayerTreeHostCommon::CallFunctionForSubtree(
        root_layer(), [](LayerImpl* layer) {
            layer->UpdatePropertyTreeForScrollingAndAnimationIfNeeded();
        });
}

void LayerTreeImpl::SetPageScaleOnActiveTree(float active_page_scale)
{
    DCHECK(IsActiveTree());
    if (page_scale_factor()->SetCurrent(
            ClampPageScaleFactorToLimits(active_page_scale)))
        DidUpdatePageScale();
}

void LayerTreeImpl::PushPageScaleFromMainThread(float page_scale_factor,
    float min_page_scale_factor,
    float max_page_scale_factor)
{
    PushPageScaleFactorAndLimits(&page_scale_factor, min_page_scale_factor,
        max_page_scale_factor);
}

void LayerTreeImpl::PushPageScaleFactorAndLimits(const float* page_scale_factor,
    float min_page_scale_factor,
    float max_page_scale_factor)
{
    DCHECK(page_scale_factor || IsActiveTree());
    bool changed_page_scale = false;
    if (page_scale_factor) {
        DCHECK(!IsActiveTree() || !layer_tree_host_impl_->pending_tree());
        changed_page_scale |= page_scale_factor_->Delta() != 1.f;
        // TODO(enne): Once CDP goes away, ignore this call below.  The only time
        // the property trees will differ is if there's been a page scale on the
        // compositor thread after the begin frame, which is the delta check above.
        changed_page_scale |= page_scale_factor_->PushFromMainThread(*page_scale_factor);
    }
    if (IsActiveTree()) {
        // TODO(enne): Pushing from pending to active should never require
        // DidUpdatePageScale.  The values should already be set by the fully
        // computed property trees being synced from one tree to another.  Remove
        // this once CDP goes away.
        changed_page_scale |= page_scale_factor_->PushPendingToActive();
    }

    changed_page_scale |= SetPageScaleFactorLimits(min_page_scale_factor, max_page_scale_factor);

    if (changed_page_scale)
        DidUpdatePageScale();
}

void LayerTreeImpl::set_top_controls_shrink_blink_size(bool shrink)
{
    if (top_controls_shrink_blink_size_ == shrink)
        return;

    top_controls_shrink_blink_size_ = shrink;
    if (IsActiveTree())
        layer_tree_host_impl_->UpdateViewportContainerSizes();
}

void LayerTreeImpl::set_top_controls_height(float top_controls_height)
{
    if (top_controls_height_ == top_controls_height)
        return;

    top_controls_height_ = top_controls_height;
    if (IsActiveTree())
        layer_tree_host_impl_->UpdateViewportContainerSizes();
}

bool LayerTreeImpl::SetCurrentTopControlsShownRatio(float ratio)
{
    ratio = std::max(ratio, 0.f);
    ratio = std::min(ratio, 1.f);
    return top_controls_shown_ratio_->SetCurrent(ratio);
}

void LayerTreeImpl::PushTopControlsFromMainThread(
    float top_controls_shown_ratio)
{
    PushTopControls(&top_controls_shown_ratio);
}

void LayerTreeImpl::PushTopControls(const float* top_controls_shown_ratio)
{
    DCHECK(top_controls_shown_ratio || IsActiveTree());

    if (top_controls_shown_ratio) {
        DCHECK(!IsActiveTree() || !layer_tree_host_impl_->pending_tree());
        top_controls_shown_ratio_->PushFromMainThread(*top_controls_shown_ratio);
    }
    if (IsActiveTree()) {
        if (top_controls_shown_ratio_->PushPendingToActive())
            layer_tree_host_impl_->DidChangeTopControlsPosition();
    }
}

bool LayerTreeImpl::SetPageScaleFactorLimits(float min_page_scale_factor,
    float max_page_scale_factor)
{
    if (min_page_scale_factor == min_page_scale_factor_ && max_page_scale_factor == max_page_scale_factor_)
        return false;

    min_page_scale_factor_ = min_page_scale_factor;
    max_page_scale_factor_ = max_page_scale_factor;

    return true;
}

void LayerTreeImpl::DidUpdatePageScale()
{
    if (IsActiveTree())
        page_scale_factor()->SetCurrent(
            ClampPageScaleFactorToLimits(current_page_scale_factor()));

    set_needs_update_draw_properties();

    if (PageScaleLayer() && PageScaleLayer()->transform_tree_index() != -1) {
        TransformNode* node = property_trees_.transform_tree.Node(
            PageScaleLayer()->transform_tree_index());
        node->data.post_local_scale_factor = current_page_scale_factor();
        node->data.needs_local_transform_update = true;
        // TODO(enne): property trees can't ask the layer these things, but
        // the page scale layer should *just* be the page scale.
        DCHECK_EQ(PageScaleLayer()->position().ToString(),
            gfx::PointF().ToString());
        DCHECK_EQ(PageScaleLayer()->transform_origin().ToString(),
            gfx::Point3F().ToString());
        node->data.update_post_local_transform(gfx::PointF(), gfx::Point3F());
        property_trees_.transform_tree.set_needs_update(true);
    }

    ForceScrollbarParameterUpdateAfterScaleChange(PageScaleLayer());
    HideInnerViewportScrollbarsIfNeeded();
}

void LayerTreeImpl::SetDeviceScaleFactor(float device_scale_factor)
{
    if (device_scale_factor == device_scale_factor_)
        return;
    device_scale_factor_ = device_scale_factor;

    if (IsActiveTree())
        layer_tree_host_impl_->SetFullRootLayerDamage();
}

void LayerTreeImpl::HideInnerViewportScrollbarsIfNeeded()
{
    if (!InnerViewportContainerLayer())
        return;

    LayerImpl::ScrollbarSet* scrollbars = InnerViewportContainerLayer()->scrollbars();

    if (!scrollbars)
        return;

    float minimum_scale_to_show_at = min_page_scale_factor() * 1.05f;
    bool hide_scrollbars = hide_pinch_scrollbars_near_min_scale_ && (current_page_scale_factor() < minimum_scale_to_show_at);

    for (LayerImpl::ScrollbarSet::iterator it = scrollbars->begin();
         it != scrollbars->end(); ++it)
        (*it)->SetHideLayerAndSubtree(hide_scrollbars);
}

SyncedProperty<ScaleGroup>* LayerTreeImpl::page_scale_factor()
{
    return page_scale_factor_.get();
}

const SyncedProperty<ScaleGroup>* LayerTreeImpl::page_scale_factor() const
{
    return page_scale_factor_.get();
}

gfx::SizeF LayerTreeImpl::ScrollableViewportSize() const
{
    if (!InnerViewportContainerLayer())
        return gfx::SizeF();

    return gfx::ScaleSize(InnerViewportContainerLayer()->BoundsForScrolling(),
        1.0f / current_page_scale_factor());
}

gfx::Rect LayerTreeImpl::RootScrollLayerDeviceViewportBounds() const
{
    LayerImpl* root_scroll_layer = OuterViewportScrollLayer()
        ? OuterViewportScrollLayer()
        : InnerViewportScrollLayer();
    if (!root_scroll_layer || root_scroll_layer->children().empty())
        return gfx::Rect();
    LayerImpl* layer = root_scroll_layer->children()[0];
    return MathUtil::MapEnclosingClippedRect(layer->screen_space_transform(),
        gfx::Rect(layer->bounds()));
}

void LayerTreeImpl::ApplySentScrollAndScaleDeltasFromAbortedCommit()
{
    DCHECK(IsActiveTree());

    page_scale_factor()->AbortCommit();
    top_controls_shown_ratio()->AbortCommit();
    elastic_overscroll()->AbortCommit();

    if (!root_layer())
        return;

    LayerTreeHostCommon::CallFunctionForSubtree(
        root_layer(), [](LayerImpl* layer) {
            layer->ApplySentScrollDeltasFromAbortedCommit();
        });
}

void LayerTreeImpl::SetViewportLayersFromIds(
    int overscroll_elasticity_layer_id,
    int page_scale_layer_id,
    int inner_viewport_scroll_layer_id,
    int outer_viewport_scroll_layer_id)
{
    overscroll_elasticity_layer_id_ = overscroll_elasticity_layer_id;
    page_scale_layer_id_ = page_scale_layer_id;
    inner_viewport_scroll_layer_id_ = inner_viewport_scroll_layer_id;
    outer_viewport_scroll_layer_id_ = outer_viewport_scroll_layer_id;

    HideInnerViewportScrollbarsIfNeeded();
}

void LayerTreeImpl::ClearViewportLayers()
{
    overscroll_elasticity_layer_id_ = Layer::INVALID_ID;
    page_scale_layer_id_ = Layer::INVALID_ID;
    inner_viewport_scroll_layer_id_ = Layer::INVALID_ID;
    outer_viewport_scroll_layer_id_ = Layer::INVALID_ID;
}

#if DCHECK_IS_ON()
int SanityCheckCopyRequestCounts(LayerImpl* layer)
{
    int count = layer->HasCopyRequest() ? 1 : 0;
    for (size_t i = 0; i < layer->children().size(); ++i) {
        count += SanityCheckCopyRequestCounts(layer->child_at(i));
    }
    DCHECK_EQ(count, layer->num_layer_or_descendants_with_copy_request())
        << ", id: " << layer->id();
    return count;
}
#endif

bool LayerTreeImpl::UpdateDrawProperties(bool update_lcd_text)
{
#if DCHECK_IS_ON()
    if (root_layer())
        SanityCheckCopyRequestCounts(root_layer());
#endif

    if (!needs_update_draw_properties_)
        return true;

    // Calling UpdateDrawProperties must clear this flag, so there can be no
    // early outs before this.
    needs_update_draw_properties_ = false;

    // For max_texture_size.  When the renderer is re-created in
    // CreateAndSetRenderer, the needs update draw properties flag is set
    // again.
    if (!layer_tree_host_impl_->renderer())
        return false;

    // Clear this after the renderer early out, as it should still be
    // possible to hit test even without a renderer.
    render_surface_layer_list_.clear();

    if (!root_layer())
        return false;

    {
        TRACE_EVENT2(
            "cc", "LayerTreeImpl::UpdateDrawProperties::CalculateDrawProperties",
            "IsActive", IsActiveTree(), "SourceFrameNumber", source_frame_number_);
        bool can_render_to_separate_surface = (layer_tree_host_impl_->GetDrawMode() != DRAW_MODE_RESOURCELESS_SOFTWARE);

        ++render_surface_layer_list_id_;

        LayerTreeHostCommon::CalcDrawPropsImplInputs inputs(
            root_layer(), DrawViewportSize(),
            layer_tree_host_impl_->DrawTransform(), device_scale_factor(),
            current_page_scale_factor(), PageScaleLayer(),
            InnerViewportScrollLayer(), OuterViewportScrollLayer(),
            elastic_overscroll()->Current(IsActiveTree()),
            OverscrollElasticityLayer(), resource_provider()->max_texture_size(),
            settings().can_use_lcd_text, settings().layers_always_allowed_lcd_text,
            can_render_to_separate_surface,
            settings().layer_transforms_should_scale_layer_contents,
            settings().verify_property_trees, &render_surface_layer_list_,
            render_surface_layer_list_id_, &property_trees_);
        LayerTreeHostCommon::CalculateDrawProperties(&inputs);
    }

    {
        TRACE_EVENT2("cc", "LayerTreeImpl::UpdateDrawProperties::Occlusion",
            "IsActive", IsActiveTree(), "SourceFrameNumber",
            source_frame_number_);
        OcclusionTracker occlusion_tracker(
            root_layer()->render_surface()->content_rect());
        occlusion_tracker.set_minimum_tracking_size(
            settings().minimum_occlusion_tracking_size);

        // LayerIterator is used here instead of CallFunctionForSubtree to only
        // UpdateTilePriorities on layers that will be visible (and thus have valid
        // draw properties) and not because any ordering is required.
        LayerIterator end = LayerIterator::End(&render_surface_layer_list_);
        for (LayerIterator it = LayerIterator::Begin(&render_surface_layer_list_);
             it != end; ++it) {
            occlusion_tracker.EnterLayer(it);

            // There are very few render targets so this should be cheap to do for
            // each layer instead of something more complicated.
            bool inside_replica = false;
            LayerImpl* layer = it->render_target();
            while (layer && !inside_replica) {
                if (layer->render_target()->has_replica())
                    inside_replica = true;
                layer = layer->render_target()->parent();
            }

            // Don't use occlusion if a layer will appear in a replica, since the
            // tile raster code does not know how to look for the replica and would
            // consider it occluded even though the replica is visible.
            // Since occlusion is only used for browser compositor (i.e.
            // use_occlusion_for_tile_prioritization) and it won't use replicas,
            // this should matter not.

            if (it.represents_itself()) {
                Occlusion occlusion = inside_replica ? Occlusion()
                                                     : occlusion_tracker.GetCurrentOcclusionForLayer(
                                                         it->draw_transform());
                it->draw_properties().occlusion_in_content_space = occlusion;
            }

            if (it.represents_contributing_render_surface()) {
                // Surfaces aren't used by the tile raster code, so they can have
                // occlusion regardless of replicas.
                Occlusion occlusion = occlusion_tracker.GetCurrentOcclusionForContributingSurface(
                    it->render_surface()->draw_transform());
                it->render_surface()->set_occlusion_in_content_space(occlusion);
                // Masks are used to draw the contributing surface, so should have
                // the same occlusion as the surface (nothing inside the surface
                // occludes them).
                if (LayerImpl* mask = it->mask_layer()) {
                    Occlusion mask_occlusion = inside_replica
                        ? Occlusion()
                        : occlusion_tracker.GetCurrentOcclusionForContributingSurface(
                            it->render_surface()->draw_transform() * it->draw_transform());
                    mask->draw_properties().occlusion_in_content_space = mask_occlusion;
                }
                if (LayerImpl* replica = it->replica_layer()) {
                    if (LayerImpl* mask = replica->mask_layer())
                        mask->draw_properties().occlusion_in_content_space = Occlusion();
                }
            }

            occlusion_tracker.LeaveLayer(it);
        }

        unoccluded_screen_space_region_ = occlusion_tracker.ComputeVisibleRegionInScreen();
    }

    // It'd be ideal if this could be done earlier, but when the raster source
    // is updated from the main thread during push properties, update draw
    // properties has not occurred yet and so it's not clear whether or not the
    // layer can or cannot use lcd text.  So, this is the cleanup pass to
    // determine if the raster source needs to be replaced with a non-lcd
    // raster source due to draw properties.
    if (update_lcd_text) {
        // TODO(enne): Make LTHI::sync_tree return this value.
        LayerTreeImpl* sync_tree = layer_tree_host_impl_->proxy()->CommitToActiveTree()
            ? layer_tree_host_impl_->active_tree()
            : layer_tree_host_impl_->pending_tree();
        // If this is not the sync tree, then it is not safe to update lcd text
        // as it causes invalidations and the tiles may be in use.
        DCHECK_EQ(this, sync_tree);
        for (const auto& layer : picture_layers_)
            layer->UpdateCanUseLCDTextAfterCommit();
    }

    {
        TRACE_EVENT_BEGIN2("cc", "LayerTreeImpl::UpdateDrawProperties::UpdateTiles",
            "IsActive", IsActiveTree(), "SourceFrameNumber",
            source_frame_number_);
        const bool resourceless_software_draw = (layer_tree_host_impl_->GetDrawMode() == DRAW_MODE_RESOURCELESS_SOFTWARE);
        size_t layers_updated_count = 0;
        bool tile_priorities_updated = false;
        for (PictureLayerImpl* layer : picture_layers_) {
            if (!layer->IsDrawnRenderSurfaceLayerListMember())
                continue;
            ++layers_updated_count;
            tile_priorities_updated |= layer->UpdateTiles(resourceless_software_draw);
        }

        if (tile_priorities_updated)
            DidModifyTilePriorities();

        TRACE_EVENT_END1("cc", "LayerTreeImpl::UpdateTilePriorities",
            "layers_updated_count", layers_updated_count);
    }

    DCHECK(!needs_update_draw_properties_) << "CalcDrawProperties should not set_needs_update_draw_properties()";
    return true;
}

void LayerTreeImpl::BuildPropertyTreesForTesting()
{
    LayerTreeHostCommon::PreCalculateMetaInformationForTesting(root_layer_.get());
    PropertyTreeBuilder::BuildPropertyTrees(
        root_layer_.get(), PageScaleLayer(), InnerViewportScrollLayer(),
        OuterViewportScrollLayer(), current_page_scale_factor(),
        device_scale_factor(), gfx::Rect(DrawViewportSize()),
        layer_tree_host_impl_->DrawTransform(), &property_trees_);
}

const LayerImplList& LayerTreeImpl::RenderSurfaceLayerList() const
{
    // If this assert triggers, then the list is dirty.
    DCHECK(!needs_update_draw_properties_);
    return render_surface_layer_list_;
}

const Region& LayerTreeImpl::UnoccludedScreenSpaceRegion() const
{
    // If this assert triggers, then the render_surface_layer_list_ is dirty, so
    // the unoccluded_screen_space_region_ is not valid anymore.
    DCHECK(!needs_update_draw_properties_);
    return unoccluded_screen_space_region_;
}

gfx::SizeF LayerTreeImpl::ScrollableSize() const
{
    LayerImpl* root_scroll_layer = OuterViewportScrollLayer()
        ? OuterViewportScrollLayer()
        : InnerViewportScrollLayer();
    if (!root_scroll_layer || root_scroll_layer->children().empty())
        return gfx::SizeF();

    gfx::SizeF content_size = root_scroll_layer->children()[0]->BoundsForScrolling();
    gfx::SizeF viewport_size = root_scroll_layer->scroll_clip_layer()->BoundsForScrolling();

    content_size.SetToMax(viewport_size);
    return content_size;
}

LayerImpl* LayerTreeImpl::LayerById(int id) const
{
    LayerIdMap::const_iterator iter = layer_id_map_.find(id);
    return iter != layer_id_map_.end() ? iter->second : NULL;
}

void LayerTreeImpl::RegisterLayer(LayerImpl* layer)
{
    DCHECK(!LayerById(layer->id()));
    layer_id_map_[layer->id()] = layer;
    if (layer_tree_host_impl_->animation_host())
        layer_tree_host_impl_->animation_host()->RegisterLayer(
            layer->id(),
            IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING);
}

void LayerTreeImpl::UnregisterLayer(LayerImpl* layer)
{
    DCHECK(LayerById(layer->id()));
    if (layer_tree_host_impl_->animation_host())
        layer_tree_host_impl_->animation_host()->UnregisterLayer(
            layer->id(),
            IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING);
    layer_id_map_.erase(layer->id());
}

size_t LayerTreeImpl::NumLayers()
{
    return layer_id_map_.size();
}

void LayerTreeImpl::DidBecomeActive()
{
    if (next_activation_forces_redraw_) {
        layer_tree_host_impl_->SetFullRootLayerDamage();
        next_activation_forces_redraw_ = false;
    }

    // Always reset this flag on activation, as we would only have activated
    // if we were in a good state.
    layer_tree_host_impl_->ResetRequiresHighResToDraw();

    if (root_layer()) {
        LayerTreeHostCommon::CallFunctionForSubtree(
            root_layer(), [](LayerImpl* layer) { layer->DidBecomeActive(); });
    }

    for (auto* swap_promise : swap_promise_list_)
        swap_promise->DidActivate();
    devtools_instrumentation::DidActivateLayerTree(layer_tree_host_impl_->id(),
        source_frame_number_);
}

bool LayerTreeImpl::RequiresHighResToDraw() const
{
    return layer_tree_host_impl_->RequiresHighResToDraw();
}

bool LayerTreeImpl::ViewportSizeInvalid() const
{
    return viewport_size_invalid_;
}

void LayerTreeImpl::SetViewportSizeInvalid()
{
    viewport_size_invalid_ = true;
    layer_tree_host_impl_->OnCanDrawStateChangedForTree();
}

void LayerTreeImpl::ResetViewportSizeInvalid()
{
    viewport_size_invalid_ = false;
    layer_tree_host_impl_->OnCanDrawStateChangedForTree();
}

Proxy* LayerTreeImpl::proxy() const
{
    return layer_tree_host_impl_->proxy();
}

const LayerTreeSettings& LayerTreeImpl::settings() const
{
    return layer_tree_host_impl_->settings();
}

const LayerTreeDebugState& LayerTreeImpl::debug_state() const
{
    return layer_tree_host_impl_->debug_state();
}

const RendererCapabilitiesImpl& LayerTreeImpl::GetRendererCapabilities() const
{
    return layer_tree_host_impl_->GetRendererCapabilities();
}

ContextProvider* LayerTreeImpl::context_provider() const
{
    return output_surface()->context_provider();
}

OutputSurface* LayerTreeImpl::output_surface() const
{
    return layer_tree_host_impl_->output_surface();
}

ResourceProvider* LayerTreeImpl::resource_provider() const
{
    return layer_tree_host_impl_->resource_provider();
}

TileManager* LayerTreeImpl::tile_manager() const
{
    return layer_tree_host_impl_->tile_manager();
}

FrameRateCounter* LayerTreeImpl::frame_rate_counter() const
{
    return layer_tree_host_impl_->fps_counter();
}

MemoryHistory* LayerTreeImpl::memory_history() const
{
    return layer_tree_host_impl_->memory_history();
}

gfx::Size LayerTreeImpl::device_viewport_size() const
{
    return layer_tree_host_impl_->device_viewport_size();
}

DebugRectHistory* LayerTreeImpl::debug_rect_history() const
{
    return layer_tree_host_impl_->debug_rect_history();
}

bool LayerTreeImpl::IsActiveTree() const
{
    return layer_tree_host_impl_->active_tree() == this;
}

bool LayerTreeImpl::IsPendingTree() const
{
    return layer_tree_host_impl_->pending_tree() == this;
}

bool LayerTreeImpl::IsRecycleTree() const
{
    return layer_tree_host_impl_->recycle_tree() == this;
}

bool LayerTreeImpl::IsSyncTree() const
{
    return layer_tree_host_impl_->sync_tree() == this;
}

LayerImpl* LayerTreeImpl::FindActiveTreeLayerById(int id)
{
    LayerTreeImpl* tree = layer_tree_host_impl_->active_tree();
    if (!tree)
        return NULL;
    return tree->LayerById(id);
}

LayerImpl* LayerTreeImpl::FindPendingTreeLayerById(int id)
{
    LayerTreeImpl* tree = layer_tree_host_impl_->pending_tree();
    if (!tree)
        return NULL;
    return tree->LayerById(id);
}

bool LayerTreeImpl::PinchGestureActive() const
{
    return layer_tree_host_impl_->pinch_gesture_active();
}

BeginFrameArgs LayerTreeImpl::CurrentBeginFrameArgs() const
{
    return layer_tree_host_impl_->CurrentBeginFrameArgs();
}

base::TimeDelta LayerTreeImpl::CurrentBeginFrameInterval() const
{
    return layer_tree_host_impl_->CurrentBeginFrameInterval();
}

void LayerTreeImpl::SetNeedsCommit()
{
    layer_tree_host_impl_->SetNeedsCommit();
}

gfx::Rect LayerTreeImpl::DeviceViewport() const
{
    return layer_tree_host_impl_->DeviceViewport();
}

gfx::Size LayerTreeImpl::DrawViewportSize() const
{
    return layer_tree_host_impl_->DrawViewportSize();
}

const gfx::Rect LayerTreeImpl::ViewportRectForTilePriority() const
{
    return layer_tree_host_impl_->ViewportRectForTilePriority();
}

scoped_ptr<ScrollbarAnimationController>
LayerTreeImpl::CreateScrollbarAnimationController(LayerImpl* scrolling_layer)
{
    DCHECK(settings().scrollbar_fade_delay_ms);
    DCHECK(settings().scrollbar_fade_duration_ms);
    base::TimeDelta delay = base::TimeDelta::FromMilliseconds(settings().scrollbar_fade_delay_ms);
    base::TimeDelta resize_delay = base::TimeDelta::FromMilliseconds(
        settings().scrollbar_fade_resize_delay_ms);
    base::TimeDelta duration = base::TimeDelta::FromMilliseconds(settings().scrollbar_fade_duration_ms);
    switch (settings().scrollbar_animator) {
    case LayerTreeSettings::LINEAR_FADE: {
        return ScrollbarAnimationControllerLinearFade::Create(
            scrolling_layer,
            layer_tree_host_impl_,
            delay,
            resize_delay,
            duration);
    }
    case LayerTreeSettings::THINNING: {
        return ScrollbarAnimationControllerThinning::Create(scrolling_layer,
            layer_tree_host_impl_,
            delay,
            resize_delay,
            duration);
    }
    case LayerTreeSettings::NO_ANIMATOR:
        NOTREACHED();
        break;
    }
    return nullptr;
}

void LayerTreeImpl::DidAnimateScrollOffset()
{
    layer_tree_host_impl_->DidAnimateScrollOffset();
}

bool LayerTreeImpl::use_gpu_rasterization() const
{
    return layer_tree_host_impl_->use_gpu_rasterization();
}

GpuRasterizationStatus LayerTreeImpl::GetGpuRasterizationStatus() const
{
    return layer_tree_host_impl_->gpu_rasterization_status();
}

bool LayerTreeImpl::create_low_res_tiling() const
{
    return layer_tree_host_impl_->create_low_res_tiling();
}

void LayerTreeImpl::SetNeedsRedraw()
{
    layer_tree_host_impl_->SetNeedsRedraw();
}

AnimationRegistrar* LayerTreeImpl::GetAnimationRegistrar() const
{
    return layer_tree_host_impl_->animation_registrar();
}

void LayerTreeImpl::GetAllPrioritizedTilesForTracing(
    std::vector<PrioritizedTile>* prioritized_tiles) const
{
    LayerIterator end = LayerIterator::End(&render_surface_layer_list_);
    for (LayerIterator it = LayerIterator::Begin(&render_surface_layer_list_);
         it != end; ++it) {
        if (!it.represents_itself())
            continue;
        LayerImpl* layer_impl = *it;
        layer_impl->GetAllPrioritizedTilesForTracing(prioritized_tiles);
    }
}

void LayerTreeImpl::AsValueInto(base::trace_event::TracedValue* state) const
{
    TracedValue::MakeDictIntoImplicitSnapshot(state, "cc::LayerTreeImpl", this);
    state->SetInteger("source_frame_number", source_frame_number_);

    state->BeginDictionary("root_layer");
    root_layer_->AsValueInto(state);
    state->EndDictionary();

    state->BeginArray("render_surface_layer_list");
    LayerIterator end = LayerIterator::End(&render_surface_layer_list_);
    for (LayerIterator it = LayerIterator::Begin(&render_surface_layer_list_);
         it != end; ++it) {
        if (!it.represents_itself())
            continue;
        TracedValue::AppendIDRef(*it, state);
    }
    state->EndArray();

    state->BeginArray("swap_promise_trace_ids");
    for (auto* swap_promise : swap_promise_list_)
        state->AppendDouble(swap_promise->TraceId());
    state->EndArray();

    state->BeginArray("pinned_swap_promise_trace_ids");
    for (auto* swap_promise : pinned_swap_promise_list_)
        state->AppendDouble(swap_promise->TraceId());
    state->EndArray();
}

void LayerTreeImpl::DistributeRootScrollOffset(
    const gfx::ScrollOffset& root_offset)
{
    if (!InnerViewportScrollLayer())
        return;

    DCHECK(OuterViewportScrollLayer());

    // If we get here, we have both inner/outer viewports, and need to distribute
    // the scroll offset between them.
    gfx::ScrollOffset inner_viewport_offset = InnerViewportScrollLayer()->CurrentScrollOffset();
    gfx::ScrollOffset outer_viewport_offset = OuterViewportScrollLayer()->CurrentScrollOffset();

    // It may be nothing has changed.
    DCHECK(inner_viewport_offset + outer_viewport_offset == TotalScrollOffset());
    if (inner_viewport_offset + outer_viewport_offset == root_offset)
        return;

    gfx::ScrollOffset max_outer_viewport_scroll_offset = OuterViewportScrollLayer()->MaxScrollOffset();

    outer_viewport_offset = root_offset - inner_viewport_offset;
    outer_viewport_offset.SetToMin(max_outer_viewport_scroll_offset);
    outer_viewport_offset.SetToMax(gfx::ScrollOffset());

    OuterViewportScrollLayer()->SetCurrentScrollOffset(outer_viewport_offset);
    inner_viewport_offset = root_offset - outer_viewport_offset;
    InnerViewportScrollLayer()->SetCurrentScrollOffset(inner_viewport_offset);
}

void LayerTreeImpl::QueueSwapPromise(scoped_ptr<SwapPromise> swap_promise)
{
    DCHECK(swap_promise);
    swap_promise_list_.push_back(swap_promise.Pass());
}

void LayerTreeImpl::QueuePinnedSwapPromise(
    scoped_ptr<SwapPromise> swap_promise)
{
    DCHECK(IsActiveTree());
    DCHECK(swap_promise);
    pinned_swap_promise_list_.push_back(swap_promise.Pass());
}

void LayerTreeImpl::PassSwapPromises(
    ScopedPtrVector<SwapPromise>* new_swap_promise)
{
    for (auto* swap_promise : swap_promise_list_)
        swap_promise->DidNotSwap(SwapPromise::SWAP_FAILS);
    swap_promise_list_.clear();
    swap_promise_list_.swap(*new_swap_promise);
}

void LayerTreeImpl::FinishSwapPromises(CompositorFrameMetadata* metadata)
{
    for (auto* swap_promise : swap_promise_list_)
        swap_promise->DidSwap(metadata);
    swap_promise_list_.clear();
    for (auto* swap_promise : pinned_swap_promise_list_)
        swap_promise->DidSwap(metadata);
    pinned_swap_promise_list_.clear();
}

void LayerTreeImpl::BreakSwapPromises(SwapPromise::DidNotSwapReason reason)
{
    for (auto* swap_promise : swap_promise_list_)
        swap_promise->DidNotSwap(reason);
    swap_promise_list_.clear();
    for (auto* swap_promise : pinned_swap_promise_list_)
        swap_promise->DidNotSwap(reason);
    pinned_swap_promise_list_.clear();
}

void LayerTreeImpl::DidModifyTilePriorities()
{
    layer_tree_host_impl_->DidModifyTilePriorities();
}

void LayerTreeImpl::set_ui_resource_request_queue(
    const UIResourceRequestQueue& queue)
{
    ui_resource_request_queue_ = queue;
}

ResourceId LayerTreeImpl::ResourceIdForUIResource(UIResourceId uid) const
{
    return layer_tree_host_impl_->ResourceIdForUIResource(uid);
}

bool LayerTreeImpl::IsUIResourceOpaque(UIResourceId uid) const
{
    return layer_tree_host_impl_->IsUIResourceOpaque(uid);
}

void LayerTreeImpl::ProcessUIResourceRequestQueue()
{
    for (const auto& req : ui_resource_request_queue_) {
        switch (req.GetType()) {
        case UIResourceRequest::UI_RESOURCE_CREATE:
            layer_tree_host_impl_->CreateUIResource(req.GetId(), req.GetBitmap());
            break;
        case UIResourceRequest::UI_RESOURCE_DELETE:
            layer_tree_host_impl_->DeleteUIResource(req.GetId());
            break;
        case UIResourceRequest::UI_RESOURCE_INVALID_REQUEST:
            NOTREACHED();
            break;
        }
    }
    ui_resource_request_queue_.clear();

    // If all UI resource evictions were not recreated by processing this queue,
    // then another commit is required.
    if (layer_tree_host_impl_->EvictedUIResourcesExist())
        layer_tree_host_impl_->SetNeedsCommit();
}

void LayerTreeImpl::RegisterPictureLayerImpl(PictureLayerImpl* layer)
{
    DCHECK(std::find(picture_layers_.begin(), picture_layers_.end(), layer) == picture_layers_.end());
    picture_layers_.push_back(layer);
}

void LayerTreeImpl::UnregisterPictureLayerImpl(PictureLayerImpl* layer)
{
    std::vector<PictureLayerImpl*>::iterator it = std::find(picture_layers_.begin(), picture_layers_.end(), layer);
    DCHECK(it != picture_layers_.end());
    picture_layers_.erase(it);
}

void LayerTreeImpl::AddLayerWithCopyOutputRequest(LayerImpl* layer)
{
    // Only the active tree needs to know about layers with copy requests, as
    // they are aborted if not serviced during draw.
    DCHECK(IsActiveTree());

    // DCHECK(std::find(layers_with_copy_output_request_.begin(),
    //                 layers_with_copy_output_request_.end(),
    //                 layer) == layers_with_copy_output_request_.end());
    // TODO(danakj): Remove this once crash is found crbug.com/309777
    for (size_t i = 0; i < layers_with_copy_output_request_.size(); ++i) {
        CHECK(layers_with_copy_output_request_[i] != layer)
            << i << " of " << layers_with_copy_output_request_.size();
    }
    layers_with_copy_output_request_.push_back(layer);
}

void LayerTreeImpl::RemoveLayerWithCopyOutputRequest(LayerImpl* layer)
{
    // Only the active tree needs to know about layers with copy requests, as
    // they are aborted if not serviced during draw.
    DCHECK(IsActiveTree());

    std::vector<LayerImpl*>::iterator it = std::find(
        layers_with_copy_output_request_.begin(),
        layers_with_copy_output_request_.end(),
        layer);
    DCHECK(it != layers_with_copy_output_request_.end());
    layers_with_copy_output_request_.erase(it);

    // TODO(danakj): Remove this once crash is found crbug.com/309777
    for (size_t i = 0; i < layers_with_copy_output_request_.size(); ++i) {
        CHECK(layers_with_copy_output_request_[i] != layer)
            << i << " of " << layers_with_copy_output_request_.size();
    }
}

const std::vector<LayerImpl*>& LayerTreeImpl::LayersWithCopyOutputRequest()
    const
{
    // Only the active tree needs to know about layers with copy requests, as
    // they are aborted if not serviced during draw.
    DCHECK(IsActiveTree());

    return layers_with_copy_output_request_;
}

template <typename LayerType>
static inline bool LayerClipsSubtree(LayerType* layer)
{
    return layer->masks_to_bounds() || layer->mask_layer();
}

static bool PointHitsRect(
    const gfx::PointF& screen_space_point,
    const gfx::Transform& local_space_to_screen_space_transform,
    const gfx::Rect& local_space_rect,
    float* distance_to_camera)
{
    // If the transform is not invertible, then assume that this point doesn't hit
    // this rect.
    gfx::Transform inverse_local_space_to_screen_space(
        gfx::Transform::kSkipInitialization);
    if (!local_space_to_screen_space_transform.GetInverse(
            &inverse_local_space_to_screen_space))
        return false;

    // Transform the hit test point from screen space to the local space of the
    // given rect.
    bool clipped = false;
    gfx::Point3F planar_point = MathUtil::ProjectPoint3D(
        inverse_local_space_to_screen_space, screen_space_point, &clipped);
    gfx::PointF hit_test_point_in_local_space = gfx::PointF(planar_point.x(), planar_point.y());

    // If ProjectPoint could not project to a valid value, then we assume that
    // this point doesn't hit this rect.
    if (clipped)
        return false;

    if (!gfx::RectF(local_space_rect).Contains(hit_test_point_in_local_space))
        return false;

    if (distance_to_camera) {
        // To compute the distance to the camera, we have to take the planar point
        // and pull it back to world space and compute the displacement along the
        // z-axis.
        gfx::Point3F planar_point_in_screen_space(planar_point);
        local_space_to_screen_space_transform.TransformPoint(
            &planar_point_in_screen_space);
        *distance_to_camera = planar_point_in_screen_space.z();
    }

    return true;
}

static bool PointHitsRegion(const gfx::PointF& screen_space_point,
    const gfx::Transform& screen_space_transform,
    const Region& layer_space_region)
{
    // If the transform is not invertible, then assume that this point doesn't hit
    // this region.
    gfx::Transform inverse_screen_space_transform(
        gfx::Transform::kSkipInitialization);
    if (!screen_space_transform.GetInverse(&inverse_screen_space_transform))
        return false;

    // Transform the hit test point from screen space to the local space of the
    // given region.
    bool clipped = false;
    gfx::PointF hit_test_point_in_layer_space = MathUtil::ProjectPoint(
        inverse_screen_space_transform, screen_space_point, &clipped);

    // If ProjectPoint could not project to a valid value, then we assume that
    // this point doesn't hit this region.
    if (clipped)
        return false;

    return layer_space_region.Contains(
        gfx::ToRoundedPoint(hit_test_point_in_layer_space));
}

static const LayerImpl* GetNextClippingLayer(const LayerImpl* layer)
{
    if (layer->scroll_parent())
        return layer->scroll_parent();
    if (layer->clip_parent())
        return layer->clip_parent();
    return layer->parent();
}

static bool PointIsClippedBySurfaceOrClipRect(
    const gfx::PointF& screen_space_point,
    const LayerImpl* layer)
{
    // Walk up the layer tree and hit-test any render_surfaces and any layer
    // clip rects that are active.
    for (; layer; layer = GetNextClippingLayer(layer)) {
        if (layer->render_surface() && !PointHitsRect(screen_space_point, layer->render_surface()->screen_space_transform(), layer->render_surface()->content_rect(), NULL))
            return true;

        if (LayerClipsSubtree(layer) && !PointHitsRect(screen_space_point, layer->screen_space_transform(), gfx::Rect(layer->bounds()), NULL))
            return true;
    }

    // If we have finished walking all ancestors without having already exited,
    // then the point is not clipped by any ancestors.
    return false;
}

static bool PointHitsLayer(const LayerImpl* layer,
    const gfx::PointF& screen_space_point,
    float* distance_to_intersection)
{
    gfx::Rect content_rect(layer->bounds());
    if (!PointHitsRect(screen_space_point,
            layer->screen_space_transform(),
            content_rect,
            distance_to_intersection))
        return false;

    // At this point, we think the point does hit the layer, but we need to walk
    // up the parents to ensure that the layer was not clipped in such a way
    // that the hit point actually should not hit the layer.
    if (PointIsClippedBySurfaceOrClipRect(screen_space_point, layer))
        return false;

    // Skip the HUD layer.
    if (layer == layer->layer_tree_impl()->hud_layer())
        return false;

    return true;
}

struct FindClosestMatchingLayerDataForRecursion {
    FindClosestMatchingLayerDataForRecursion()
        : closest_match(NULL)
        , closest_distance(-std::numeric_limits<float>::infinity())
    {
    }
    LayerImpl* closest_match;
    // Note that the positive z-axis points towards the camera, so bigger means
    // closer in this case, counterintuitively.
    float closest_distance;
};

template <typename Functor>
static void FindClosestMatchingLayer(
    const gfx::PointF& screen_space_point,
    LayerImpl* layer,
    const Functor& func,
    FindClosestMatchingLayerDataForRecursion* data_for_recursion)
{
    size_t children_size = layer->children().size();
    for (size_t i = 0; i < children_size; ++i) {
        size_t index = children_size - 1 - i;
        FindClosestMatchingLayer(screen_space_point, layer->children()[index], func,
            data_for_recursion);
    }

    float distance_to_intersection = 0.f;
    if (func(layer) && PointHitsLayer(layer, screen_space_point, &distance_to_intersection) && ((!data_for_recursion->closest_match || distance_to_intersection > data_for_recursion->closest_distance))) {
        data_for_recursion->closest_distance = distance_to_intersection;
        data_for_recursion->closest_match = layer;
    }
}

static bool ScrollsAnyDrawnRenderSurfaceLayerListMember(LayerImpl* layer)
{
    if (!layer->scrollable())
        return false;
    if (layer->layer_or_descendant_is_drawn())
        return true;

    if (!layer->scroll_children())
        return false;
    for (std::set<LayerImpl*>::const_iterator it = layer->scroll_children()->begin();
         it != layer->scroll_children()->end();
         ++it) {
        if ((*it)->layer_or_descendant_is_drawn())
            return true;
    }
    return false;
}

struct FindScrollingLayerFunctor {
    bool operator()(LayerImpl* layer) const
    {
        return ScrollsAnyDrawnRenderSurfaceLayerListMember(layer);
    }
};

LayerImpl* LayerTreeImpl::FindFirstScrollingLayerThatIsHitByPoint(
    const gfx::PointF& screen_space_point)
{
    FindClosestMatchingLayerDataForRecursion data_for_recursion;
    FindClosestMatchingLayer(screen_space_point,
        root_layer(),
        FindScrollingLayerFunctor(),
        &data_for_recursion);
    return data_for_recursion.closest_match;
}

struct HitTestVisibleScrollableOrTouchableFunctor {
    bool operator()(LayerImpl* layer) const
    {
        return layer->IsDrawnRenderSurfaceLayerListMember() || ScrollsAnyDrawnRenderSurfaceLayerListMember(layer) || !layer->touch_event_handler_region().IsEmpty() || layer->have_wheel_event_handlers();
    }
};

LayerImpl* LayerTreeImpl::FindLayerThatIsHitByPoint(
    const gfx::PointF& screen_space_point)
{
    if (!root_layer())
        return NULL;
    bool update_lcd_text = false;
    if (!UpdateDrawProperties(update_lcd_text))
        return NULL;
    FindClosestMatchingLayerDataForRecursion data_for_recursion;
    FindClosestMatchingLayer(screen_space_point,
        root_layer(),
        HitTestVisibleScrollableOrTouchableFunctor(),
        &data_for_recursion);
    return data_for_recursion.closest_match;
}

static bool LayerHasTouchEventHandlersAt(const gfx::PointF& screen_space_point,
    LayerImpl* layer_impl)
{
    if (layer_impl->touch_event_handler_region().IsEmpty())
        return false;

    if (!PointHitsRegion(screen_space_point, layer_impl->screen_space_transform(),
            layer_impl->touch_event_handler_region()))
        return false;

    // At this point, we think the point does hit the touch event handler region
    // on the layer, but we need to walk up the parents to ensure that the layer
    // was not clipped in such a way that the hit point actually should not hit
    // the layer.
    if (PointIsClippedBySurfaceOrClipRect(screen_space_point, layer_impl))
        return false;

    return true;
}

struct FindWheelEventLayerFunctor {
    bool operator()(LayerImpl* layer) const
    {
        return layer->have_wheel_event_handlers();
    }
};

LayerImpl* LayerTreeImpl::FindLayerWithWheelHandlerThatIsHitByPoint(
    const gfx::PointF& screen_space_point)
{
    if (!root_layer())
        return NULL;
    bool update_lcd_text = false;
    if (!UpdateDrawProperties(update_lcd_text))
        return NULL;
    FindWheelEventLayerFunctor func;
    FindClosestMatchingLayerDataForRecursion data_for_recursion;
    FindClosestMatchingLayer(screen_space_point, root_layer(), func,
        &data_for_recursion);
    return data_for_recursion.closest_match;
}

struct FindTouchEventLayerFunctor {
    bool operator()(LayerImpl* layer) const
    {
        return LayerHasTouchEventHandlersAt(screen_space_point, layer);
    }
    const gfx::PointF screen_space_point;
};

LayerImpl* LayerTreeImpl::FindLayerThatIsHitByPointInTouchHandlerRegion(
    const gfx::PointF& screen_space_point)
{
    if (!root_layer())
        return NULL;
    bool update_lcd_text = false;
    if (!UpdateDrawProperties(update_lcd_text))
        return NULL;
    FindTouchEventLayerFunctor func = { screen_space_point };
    FindClosestMatchingLayerDataForRecursion data_for_recursion;
    FindClosestMatchingLayer(
        screen_space_point, root_layer(), func, &data_for_recursion);
    return data_for_recursion.closest_match;
}

void LayerTreeImpl::RegisterSelection(const LayerSelection& selection)
{
    selection_ = selection;
}

static ViewportSelectionBound ComputeViewportSelectionBound(
    const LayerSelectionBound& layer_bound,
    LayerImpl* layer,
    float device_scale_factor)
{
    ViewportSelectionBound viewport_bound;
    viewport_bound.type = layer_bound.type;

    if (!layer || layer_bound.type == SELECTION_BOUND_EMPTY)
        return viewport_bound;

    gfx::PointF layer_top = layer_bound.edge_top;
    gfx::PointF layer_bottom = layer_bound.edge_bottom;

    bool clipped = false;
    gfx::PointF screen_top = MathUtil::MapPoint(layer->screen_space_transform(), layer_top, &clipped);
    gfx::PointF screen_bottom = MathUtil::MapPoint(
        layer->screen_space_transform(), layer_bottom, &clipped);

    // MapPoint can produce points with NaN components (even when no inputs are
    // NaN). Since consumers of ViewportSelectionBounds may round |edge_top| or
    // |edge_bottom| (and since rounding will crash on NaN), we return an empty
    // bound instead.
    if (std::isnan(screen_top.x()) || std::isnan(screen_top.y()) || std::isnan(screen_bottom.x()) || std::isnan(screen_bottom.y()))
        return ViewportSelectionBound();

    const float inv_scale = 1.f / device_scale_factor;
    viewport_bound.edge_top = gfx::ScalePoint(screen_top, inv_scale);
    viewport_bound.edge_bottom = gfx::ScalePoint(screen_bottom, inv_scale);

    // The bottom edge point is used for visibility testing as it is the logical
    // focal point for bound selection handles (this may change in the future).
    // Shifting the visibility point fractionally inward ensures that neighboring
    // or logically coincident layers aligned to integral DPI coordinates will not
    // spuriously occlude the bound.
    gfx::Vector2dF visibility_offset = layer_top - layer_bottom;
    visibility_offset.Scale(device_scale_factor / visibility_offset.Length());
    gfx::PointF visibility_point = layer_bottom + visibility_offset;
    if (visibility_point.x() <= 0)
        visibility_point.set_x(visibility_point.x() + device_scale_factor);
    visibility_point = MathUtil::MapPoint(
        layer->screen_space_transform(), visibility_point, &clipped);

    float intersect_distance = 0.f;
    viewport_bound.visible = PointHitsLayer(layer, visibility_point, &intersect_distance);

    return viewport_bound;
}

void LayerTreeImpl::GetViewportSelection(ViewportSelection* selection)
{
    DCHECK(selection);

    selection->start = ComputeViewportSelectionBound(
        selection_.start,
        selection_.start.layer_id ? LayerById(selection_.start.layer_id) : NULL,
        device_scale_factor());
    selection->is_editable = selection_.is_editable;
    selection->is_empty_text_form_control = selection_.is_empty_text_form_control;
    if (selection->start.type == SELECTION_BOUND_CENTER || selection->start.type == SELECTION_BOUND_EMPTY) {
        selection->end = selection->start;
    } else {
        selection->end = ComputeViewportSelectionBound(
            selection_.end,
            selection_.end.layer_id ? LayerById(selection_.end.layer_id) : NULL,
            device_scale_factor());
    }
}

void LayerTreeImpl::InputScrollAnimationFinished()
{
    layer_tree_host_impl_->ScrollEnd();
}

bool LayerTreeImpl::SmoothnessTakesPriority() const
{
    return layer_tree_host_impl_->GetTreePriority() == SMOOTHNESS_TAKES_PRIORITY;
}

BlockingTaskRunner* LayerTreeImpl::BlockingMainThreadTaskRunner() const
{
    return proxy()->blocking_main_thread_task_runner();
}

VideoFrameControllerClient* LayerTreeImpl::GetVideoFrameControllerClient()
    const
{
    return layer_tree_host_impl_;
}

void LayerTreeImpl::SetPendingPageScaleAnimation(
    scoped_ptr<PendingPageScaleAnimation> pending_animation)
{
    pending_page_scale_animation_ = pending_animation.Pass();
}

scoped_ptr<PendingPageScaleAnimation>
LayerTreeImpl::TakePendingPageScaleAnimation()
{
    return pending_page_scale_animation_.Pass();
}

bool LayerTreeImpl::IsAnimatingFilterProperty(const LayerImpl* layer) const
{
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->IsAnimatingFilterProperty(layer->id(), tree_type)
        : false;
}

bool LayerTreeImpl::IsAnimatingOpacityProperty(const LayerImpl* layer) const
{
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->IsAnimatingOpacityProperty(layer->id(), tree_type)
        : false;
}

bool LayerTreeImpl::IsAnimatingTransformProperty(const LayerImpl* layer) const
{
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->IsAnimatingTransformProperty(layer->id(), tree_type)
        : false;
}

bool LayerTreeImpl::HasPotentiallyRunningFilterAnimation(
    const LayerImpl* layer) const
{
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->HasPotentiallyRunningFilterAnimation(layer->id(),
                  tree_type)
        : false;
}

bool LayerTreeImpl::HasPotentiallyRunningOpacityAnimation(
    const LayerImpl* layer) const
{
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->HasPotentiallyRunningOpacityAnimation(layer->id(),
                  tree_type)
        : false;
}

bool LayerTreeImpl::HasPotentiallyRunningTransformAnimation(
    const LayerImpl* layer) const
{
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->HasPotentiallyRunningTransformAnimation(layer->id(),
                  tree_type)
        : false;
}

bool LayerTreeImpl::HasAnyAnimationTargetingProperty(
    const LayerImpl* layer,
    Animation::TargetProperty property) const
{
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->HasAnyAnimationTargetingProperty(layer->id(), property)
        : false;
}

bool LayerTreeImpl::FilterIsAnimatingOnImplOnly(const LayerImpl* layer) const
{
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->FilterIsAnimatingOnImplOnly(layer->id())
        : false;
}

bool LayerTreeImpl::OpacityIsAnimatingOnImplOnly(const LayerImpl* layer) const
{
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->OpacityIsAnimatingOnImplOnly(layer->id())
        : false;
}

bool LayerTreeImpl::TransformIsAnimatingOnImplOnly(
    const LayerImpl* layer) const
{
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->TransformIsAnimatingOnImplOnly(layer->id())
        : false;
}

bool LayerTreeImpl::HasOnlyTranslationTransforms(const LayerImpl* layer) const
{
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->HasOnlyTranslationTransforms(layer->id(), tree_type)
        : true;
}

bool LayerTreeImpl::MaximumTargetScale(const LayerImpl* layer,
    float* max_scale) const
{
    *max_scale = 0.f;
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()->MaximumTargetScale(
            layer->id(), tree_type, max_scale)
        : true;
}

bool LayerTreeImpl::AnimationStartScale(const LayerImpl* layer,
    float* start_scale) const
{
    *start_scale = 0.f;
    LayerTreeType tree_type = IsActiveTree() ? LayerTreeType::ACTIVE : LayerTreeType::PENDING;
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()->AnimationStartScale(
            layer->id(), tree_type, start_scale)
        : true;
}

bool LayerTreeImpl::HasFilterAnimationThatInflatesBounds(
    const LayerImpl* layer) const
{
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->HasFilterAnimationThatInflatesBounds(layer->id())
        : false;
}

bool LayerTreeImpl::HasTransformAnimationThatInflatesBounds(
    const LayerImpl* layer) const
{
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->HasTransformAnimationThatInflatesBounds(layer->id())
        : false;
}

bool LayerTreeImpl::HasAnimationThatInflatesBounds(
    const LayerImpl* layer) const
{
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->HasAnimationThatInflatesBounds(layer->id())
        : false;
}

bool LayerTreeImpl::FilterAnimationBoundsForBox(const LayerImpl* layer,
    const gfx::BoxF& box,
    gfx::BoxF* bounds) const
{
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->FilterAnimationBoundsForBox(layer->id(), box, bounds)
        : false;
}

bool LayerTreeImpl::TransformAnimationBoundsForBox(const LayerImpl* layer,
    const gfx::BoxF& box,
    gfx::BoxF* bounds) const
{
    *bounds = gfx::BoxF();
    return layer_tree_host_impl_->animation_host()
        ? layer_tree_host_impl_->animation_host()
              ->TransformAnimationBoundsForBox(layer->id(), box, bounds)
        : true;
}

} // namespace cc
