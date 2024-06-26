// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/picture_layer_impl.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

#include "base/time/time.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/base/math_util.h"
#include "cc/debug/debug_colors.h"
#include "cc/debug/micro_benchmark_impl.h"
#include "cc/debug/traced_value.h"
#include "cc/layers/append_quads_data.h"
#include "cc/layers/solid_color_layer_impl.h"
#include "cc/output/begin_frame_args.h"
#include "cc/quads/debug_border_draw_quad.h"
#include "cc/quads/picture_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/tile_draw_quad.h"
#include "cc/tiles/tile_manager.h"
#include "cc/tiles/tiling_set_raster_queue_all.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/occlusion.h"
#include "ui/gfx/geometry/quad_f.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace {
// This must be > 1 as we multiply or divide by this to find a new raster
// scale during pinch.
const float kMaxScaleRatioDuringPinch = 2.0f;

// When creating a new tiling during pinch, snap to an existing
// tiling's scale if the desired scale is within this ratio.
const float kSnapToExistingTilingRatio = 1.2f;

// Even for really wide viewports, at some point GPU raster should use
// less than 4 tiles to fill the viewport. This is set to 256 as a
// sane minimum for now, but we might want to tune this for low-end.
const int kMinHeightForGpuRasteredTile = 256;

// When making odd-sized tiles, round them up to increase the chances
// of using the same tile size.
const int kTileRoundUp = 64;

} // namespace

namespace cc {

PictureLayerImpl::PictureLayerImpl(
    LayerTreeImpl* tree_impl,
    int id,
    bool is_mask,
    scoped_refptr<SyncedScrollOffset> scroll_offset)
    : LayerImpl(tree_impl, id, scroll_offset)
    , twin_layer_(nullptr)
    , tilings_(CreatePictureLayerTilingSet())
    , ideal_page_scale_(0.f)
    , ideal_device_scale_(0.f)
    , ideal_source_scale_(0.f)
    , ideal_contents_scale_(0.f)
    , raster_page_scale_(0.f)
    , raster_device_scale_(0.f)
    , raster_source_scale_(0.f)
    , raster_contents_scale_(0.f)
    , low_res_raster_contents_scale_(0.f)
    , raster_source_scale_is_fixed_(false)
    , was_screen_space_transform_animating_(false)
    , only_used_low_res_last_append_quads_(false)
    , is_mask_(is_mask)
    , nearest_neighbor_(false)
{
    layer_tree_impl()->RegisterPictureLayerImpl(this);
}

PictureLayerImpl::~PictureLayerImpl()
{
    if (twin_layer_)
        twin_layer_->twin_layer_ = nullptr;
    layer_tree_impl()->UnregisterPictureLayerImpl(this);
}

const char* PictureLayerImpl::LayerTypeAsString() const
{
    return "cc::PictureLayerImpl";
}

scoped_ptr<LayerImpl> PictureLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return PictureLayerImpl::Create(tree_impl, id(), is_mask_,
        synced_scroll_offset());
}

void PictureLayerImpl::PushPropertiesTo(LayerImpl* base_layer)
{
    PictureLayerImpl* layer_impl = static_cast<PictureLayerImpl*>(base_layer);
    DCHECK_EQ(layer_impl->is_mask_, is_mask_);

    LayerImpl::PushPropertiesTo(base_layer);

    // Twin relationships should never change once established.
    DCHECK_IMPLIES(twin_layer_, twin_layer_ == layer_impl);
    DCHECK_IMPLIES(twin_layer_, layer_impl->twin_layer_ == this);
    // The twin relationship does not need to exist before the first
    // PushPropertiesTo from pending to active layer since before that the active
    // layer can not have a pile or tilings, it has only been created and inserted
    // into the tree at that point.
    twin_layer_ = layer_impl;
    layer_impl->twin_layer_ = this;

    layer_impl->SetNearestNeighbor(nearest_neighbor_);

    // Solid color layers have no tilings.
    DCHECK_IMPLIES(raster_source_->IsSolidColor(), tilings_->num_tilings() == 0);
    // The pending tree should only have a high res (and possibly low res) tiling.
    DCHECK_LE(tilings_->num_tilings(),
        layer_tree_impl()->create_low_res_tiling() ? 2u : 1u);

    layer_impl->set_gpu_raster_max_texture_size(gpu_raster_max_texture_size_);
    layer_impl->UpdateRasterSource(raster_source_, &invalidation_,
        tilings_.get());
    DCHECK(invalidation_.IsEmpty());

    // After syncing a solid color layer, the active layer has no tilings.
    DCHECK_IMPLIES(raster_source_->IsSolidColor(),
        layer_impl->tilings_->num_tilings() == 0);

    layer_impl->raster_page_scale_ = raster_page_scale_;
    layer_impl->raster_device_scale_ = raster_device_scale_;
    layer_impl->raster_source_scale_ = raster_source_scale_;
    layer_impl->raster_contents_scale_ = raster_contents_scale_;
    layer_impl->low_res_raster_contents_scale_ = low_res_raster_contents_scale_;

    layer_impl->SanityCheckTilingState();

    // We always need to push properties.
    // See http://crbug.com/303943
    // TODO(danakj): Stop always pushing properties since we don't swap tilings.
    needs_push_properties_ = true;
}

void PictureLayerImpl::AppendQuads(RenderPass* render_pass,
    AppendQuadsData* append_quads_data)
{
    // The bounds and the pile size may differ if the pile wasn't updated (ie.
    // PictureLayer::Update didn't happen). In that case the pile will be empty.
    DCHECK_IMPLIES(!raster_source_->GetSize().IsEmpty(),
        bounds() == raster_source_->GetSize())
        << " bounds " << bounds().ToString() << " pile "
        << raster_source_->GetSize().ToString();

    SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();

    if (raster_source_->IsSolidColor()) {
        PopulateSharedQuadState(shared_quad_state);

        AppendDebugBorderQuad(
            render_pass, bounds(), shared_quad_state, append_quads_data);

        SolidColorLayerImpl::AppendSolidQuads(
            render_pass, draw_properties().occlusion_in_content_space,
            shared_quad_state, visible_layer_rect(),
            raster_source_->GetSolidColor(), append_quads_data);
        return;
    }

    float max_contents_scale = MaximumTilingContentsScale();
    PopulateScaledSharedQuadState(shared_quad_state, max_contents_scale);
    Occlusion scaled_occlusion = draw_properties()
                                     .occlusion_in_content_space.GetOcclusionWithGivenDrawTransform(
                                         shared_quad_state->quad_to_target_transform);

    if (current_draw_mode_ == DRAW_MODE_RESOURCELESS_SOFTWARE) {
        AppendDebugBorderQuad(
            render_pass, shared_quad_state->quad_layer_bounds, shared_quad_state,
            append_quads_data, DebugColors::DirectPictureBorderColor(),
            DebugColors::DirectPictureBorderWidth(layer_tree_impl()));

        gfx::Rect geometry_rect = shared_quad_state->visible_quad_layer_rect;
        gfx::Rect opaque_rect = contents_opaque() ? geometry_rect : gfx::Rect();
        gfx::Rect visible_geometry_rect = scaled_occlusion.GetUnoccludedContentRect(geometry_rect);

        // The raster source may not be valid over the entire visible rect,
        // and rastering outside of that may cause incorrect pixels.
        gfx::Rect scaled_recorded_viewport = gfx::ScaleToEnclosingRect(
            raster_source_->RecordedViewport(), max_contents_scale);
        geometry_rect.Intersect(scaled_recorded_viewport);
        opaque_rect.Intersect(scaled_recorded_viewport);
        visible_geometry_rect.Intersect(scaled_recorded_viewport);

        if (visible_geometry_rect.IsEmpty())
            return;

        DCHECK(raster_source_->HasRecordings());
        gfx::Rect quad_content_rect = shared_quad_state->visible_quad_layer_rect;
        gfx::Size texture_size = quad_content_rect.size();
        gfx::RectF texture_rect = gfx::RectF(gfx::SizeF(texture_size));

        PictureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        quad->SetNew(shared_quad_state, geometry_rect, opaque_rect,
            visible_geometry_rect, texture_rect, texture_size,
            nearest_neighbor_, RGBA_8888, quad_content_rect,
            max_contents_scale, raster_source_);
        ValidateQuadResources(quad);
        return;
    }

    AppendDebugBorderQuad(render_pass, shared_quad_state->quad_layer_bounds,
        shared_quad_state, append_quads_data);

    if (ShowDebugBorders()) {
        for (PictureLayerTilingSet::CoverageIterator iter(
                 tilings_.get(), max_contents_scale,
                 shared_quad_state->visible_quad_layer_rect, ideal_contents_scale_);
             iter; ++iter) {
            SkColor color;
            float width;
            if (*iter && iter->draw_info().IsReadyToDraw()) {
                TileDrawInfo::Mode mode = iter->draw_info().mode();
                if (mode == TileDrawInfo::SOLID_COLOR_MODE) {
                    color = DebugColors::SolidColorTileBorderColor();
                    width = DebugColors::SolidColorTileBorderWidth(layer_tree_impl());
                } else if (mode == TileDrawInfo::OOM_MODE) {
                    color = DebugColors::OOMTileBorderColor();
                    width = DebugColors::OOMTileBorderWidth(layer_tree_impl());
                } else if (iter.resolution() == HIGH_RESOLUTION) {
                    color = DebugColors::HighResTileBorderColor();
                    width = DebugColors::HighResTileBorderWidth(layer_tree_impl());
                } else if (iter.resolution() == LOW_RESOLUTION) {
                    color = DebugColors::LowResTileBorderColor();
                    width = DebugColors::LowResTileBorderWidth(layer_tree_impl());
                } else if (iter->contents_scale() > max_contents_scale) {
                    color = DebugColors::ExtraHighResTileBorderColor();
                    width = DebugColors::ExtraHighResTileBorderWidth(layer_tree_impl());
                } else {
                    color = DebugColors::ExtraLowResTileBorderColor();
                    width = DebugColors::ExtraLowResTileBorderWidth(layer_tree_impl());
                }
            } else {
                color = DebugColors::MissingTileBorderColor();
                width = DebugColors::MissingTileBorderWidth(layer_tree_impl());
            }

            DebugBorderDrawQuad* debug_border_quad = render_pass->CreateAndAppendDrawQuad<DebugBorderDrawQuad>();
            gfx::Rect geometry_rect = iter.geometry_rect();
            gfx::Rect visible_geometry_rect = geometry_rect;
            debug_border_quad->SetNew(shared_quad_state,
                geometry_rect,
                visible_geometry_rect,
                color,
                width);
        }
    }

    // Keep track of the tilings that were used so that tilings that are
    // unused can be considered for removal.
    last_append_quads_tilings_.clear();

    // Ignore missing tiles outside of viewport for tile priority. This is
    // normally the same as draw viewport but can be independently overridden by
    // embedders like Android WebView with SetExternalDrawConstraints.
    gfx::Rect scaled_viewport_for_tile_priority = gfx::ScaleToEnclosingRect(
        viewport_rect_for_tile_priority_in_content_space_, max_contents_scale);

    size_t missing_tile_count = 0u;
    size_t on_demand_missing_tile_count = 0u;
    only_used_low_res_last_append_quads_ = true;
    gfx::Rect scaled_recorded_viewport = gfx::ScaleToEnclosingRect(
        raster_source_->RecordedViewport(), max_contents_scale);
    for (PictureLayerTilingSet::CoverageIterator iter(
             tilings_.get(), max_contents_scale,
             shared_quad_state->visible_quad_layer_rect, ideal_contents_scale_);
         iter; ++iter) {
        gfx::Rect geometry_rect = iter.geometry_rect();
        gfx::Rect opaque_rect = contents_opaque() ? geometry_rect : gfx::Rect();
        gfx::Rect visible_geometry_rect = scaled_occlusion.GetUnoccludedContentRect(geometry_rect);
        if (visible_geometry_rect.IsEmpty())
            continue;

        append_quads_data->visible_layer_area += visible_geometry_rect.width() * visible_geometry_rect.height();

        bool has_draw_quad = false;
        if (*iter && iter->draw_info().IsReadyToDraw()) {
            const TileDrawInfo& draw_info = iter->draw_info();
            switch (draw_info.mode()) {
            case TileDrawInfo::RESOURCE_MODE: {
                gfx::RectF texture_rect = iter.texture_rect();

                // The raster_contents_scale_ is the best scale that the layer is
                // trying to produce, even though it may not be ideal. Since that's
                // the best the layer can promise in the future, consider those as
                // complete. But if a tile is ideal scale, we don't want to consider
                // it incomplete and trying to replace it with a tile at a worse
                // scale.
                if (iter->contents_scale() != raster_contents_scale_ && iter->contents_scale() != ideal_contents_scale_ && geometry_rect.Intersects(scaled_viewport_for_tile_priority)) {
                    append_quads_data->num_incomplete_tiles++;
                }

                TileDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TileDrawQuad>();
                quad->SetNew(shared_quad_state, geometry_rect, opaque_rect,
                    visible_geometry_rect, draw_info.resource_id(),
                    texture_rect, draw_info.resource_size(),
                    draw_info.contents_swizzled(), nearest_neighbor_);
                ValidateQuadResources(quad);
                iter->draw_info().set_was_ever_used_to_draw();
                has_draw_quad = true;
                break;
            }
            case TileDrawInfo::SOLID_COLOR_MODE: {
                SolidColorDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
                quad->SetNew(shared_quad_state, geometry_rect, visible_geometry_rect,
                    draw_info.solid_color(), false);
                ValidateQuadResources(quad);
                iter->draw_info().set_was_ever_used_to_draw();
                has_draw_quad = true;
                break;
            }
            case TileDrawInfo::OOM_MODE:
                break; // Checkerboard.
            }
        }

        if (!has_draw_quad) {
            // Checkerboard.
            SkColor color = SafeOpaqueBackgroundColor();
            if (ShowDebugBorders()) {
                // Fill the whole tile with the missing tile color.
                color = DebugColors::OOMTileBorderColor();
            }
            SolidColorDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            quad->SetNew(shared_quad_state, geometry_rect, visible_geometry_rect,
                color, false);
            ValidateQuadResources(quad);

            if (geometry_rect.Intersects(scaled_viewport_for_tile_priority)) {
                append_quads_data->num_missing_tiles++;
                ++missing_tile_count;
            }
            int64 checkerboarded_area = visible_geometry_rect.width() * visible_geometry_rect.height();
            append_quads_data->checkerboarded_visible_content_area += checkerboarded_area;
            // Intersect checkerboard rect with interest rect to generate rect where
            // we checkerboarded and has recording. The area where we don't have
            // recording is not necessarily a Rect, and its area is calculated using
            // subtraction.
            gfx::Rect visible_rect_has_recording = visible_geometry_rect;
            visible_rect_has_recording.Intersect(scaled_recorded_viewport);
            int64 checkerboarded_has_recording_area = visible_rect_has_recording.width() * visible_rect_has_recording.height();
            append_quads_data->checkerboarded_needs_raster_content_area += checkerboarded_has_recording_area;
            append_quads_data->checkerboarded_no_recording_content_area += checkerboarded_area - checkerboarded_has_recording_area;
            continue;
        }

        if (iter.resolution() != HIGH_RESOLUTION) {
            append_quads_data->approximated_visible_content_area += visible_geometry_rect.width() * visible_geometry_rect.height();
        }

        // If we have a draw quad, but it's not low resolution, then
        // mark that we've used something other than low res to draw.
        if (iter.resolution() != LOW_RESOLUTION)
            only_used_low_res_last_append_quads_ = false;

        if (last_append_quads_tilings_.empty() || last_append_quads_tilings_.back() != iter.CurrentTiling()) {
            last_append_quads_tilings_.push_back(iter.CurrentTiling());
        }
    }

    if (missing_tile_count) {
        TRACE_EVENT_INSTANT2("cc",
            "PictureLayerImpl::AppendQuads checkerboard",
            TRACE_EVENT_SCOPE_THREAD,
            "missing_tile_count",
            missing_tile_count,
            "on_demand_missing_tile_count",
            on_demand_missing_tile_count);
    }

    // Aggressively remove any tilings that are not seen to save memory. Note
    // that this is at the expense of doing cause more frequent re-painting. A
    // better scheme would be to maintain a tighter visible_layer_rect for the
    // finer tilings.
    CleanUpTilingsOnActiveLayer(last_append_quads_tilings_);
}

bool PictureLayerImpl::UpdateTiles(bool resourceless_software_draw)
{
    if (!resourceless_software_draw) {
        visible_rect_for_tile_priority_ = visible_layer_rect();
    }

    if (!CanHaveTilings()) {
        ideal_page_scale_ = 0.f;
        ideal_device_scale_ = 0.f;
        ideal_contents_scale_ = 0.f;
        ideal_source_scale_ = 0.f;
        SanityCheckTilingState();
        return false;
    }

    // Remove any non-ideal tilings that were not used last time we generated
    // quads to save memory and processing time. Note that pending tree should
    // only have one or two tilings (high and low res), so only clean up the
    // active layer. This cleans it up here in case AppendQuads didn't run.
    // If it did run, this would not remove any additional tilings.
    if (layer_tree_impl()->IsActiveTree())
        CleanUpTilingsOnActiveLayer(last_append_quads_tilings_);

    UpdateIdealScales();

    if (!raster_contents_scale_ || ShouldAdjustRasterScale()) {
        RecalculateRasterScales();
        AddTilingsForRasterScale();
    }

    if (layer_tree_impl()->IsActiveTree())
        AddLowResolutionTilingIfNeeded();

    DCHECK(raster_page_scale_);
    DCHECK(raster_device_scale_);
    DCHECK(raster_source_scale_);
    DCHECK(raster_contents_scale_);
    DCHECK(low_res_raster_contents_scale_);

    was_screen_space_transform_animating_ = draw_properties().screen_space_transform_is_animating;

    if (screen_space_transform_is_animating())
        raster_source_->SetShouldAttemptToUseDistanceFieldText();

    double current_frame_time_in_seconds = (layer_tree_impl()->CurrentBeginFrameArgs().frame_time - base::TimeTicks()).InSecondsF();
    UpdateViewportRectForTilePriorityInContentSpace();

    // The tiling set can require tiles for activation any of the following
    // conditions are true:
    // - This layer produced a high-res or non-ideal-res tile last frame.
    // - We're in requires high res to draw mode.
    // - We're not in smoothness takes priority mode.
    // To put different, the tiling set can't require tiles for activation if
    // we're in smoothness mode and only used low-res or checkerboard to draw last
    // frame and we don't need high res to draw.
    //
    // The reason for this is that we should be able to activate sooner and get a
    // more up to date recording, so we don't run out of recording on the active
    // tree.
    bool can_require_tiles_for_activation = !only_used_low_res_last_append_quads_ || RequiresHighResToDraw() || !layer_tree_impl()->SmoothnessTakesPriority();

    static const Occlusion kEmptyOcclusion;
    const Occlusion& occlusion_in_content_space = layer_tree_impl()->settings().use_occlusion_for_tile_prioritization
        ? draw_properties().occlusion_in_content_space
        : kEmptyOcclusion;

    // Pass |occlusion_in_content_space| for |occlusion_in_layer_space| since
    // they are the same space in picture layer, as contents scale is always 1.
    bool updated = tilings_->UpdateTilePriorities(
        viewport_rect_for_tile_priority_in_content_space_, ideal_contents_scale_,
        current_frame_time_in_seconds, occlusion_in_content_space,
        can_require_tiles_for_activation);
    return updated;
}

void PictureLayerImpl::UpdateViewportRectForTilePriorityInContentSpace()
{
    // If visible_rect_for_tile_priority_ is empty or
    // viewport_rect_for_tile_priority is set to be different from the device
    // viewport, try to inverse project the viewport into layer space and use
    // that. Otherwise just use visible_rect_for_tile_priority_
    gfx::Rect visible_rect_in_content_space = visible_rect_for_tile_priority_;
    gfx::Rect viewport_rect_for_tile_priority = layer_tree_impl()->ViewportRectForTilePriority();
    if (visible_rect_in_content_space.IsEmpty() || layer_tree_impl()->DeviceViewport() != viewport_rect_for_tile_priority) {
        gfx::Transform view_to_layer(gfx::Transform::kSkipInitialization);
        if (screen_space_transform().GetInverse(&view_to_layer)) {
            // Transform from view space to content space.
            visible_rect_in_content_space = MathUtil::ProjectEnclosingClippedRect(
                view_to_layer, viewport_rect_for_tile_priority);

            // We have to allow for a viewport that is outside of the layer bounds in
            // order to compute tile priorities correctly for offscreen content that
            // is going to make it on screen. However, we also have to limit the
            // viewport since it can be very large due to screen_space_transforms. As
            // a heuristic, we clip to bounds padded by skewport_extrapolation_limit *
            // maximum tiling scale, since this should allow sufficient room for
            // skewport calculations.
            gfx::Rect padded_bounds(bounds());
            int padding_amount = layer_tree_impl()
                                     ->settings()
                                     .skewport_extrapolation_limit_in_content_pixels
                * MaximumTilingContentsScale();
            padded_bounds.Inset(-padding_amount, -padding_amount);
            visible_rect_in_content_space.Intersect(padded_bounds);
        }
    }
    viewport_rect_for_tile_priority_in_content_space_ = visible_rect_in_content_space;
}

PictureLayerImpl* PictureLayerImpl::GetPendingOrActiveTwinLayer() const
{
    if (!twin_layer_ || !twin_layer_->IsOnActiveOrPendingTree())
        return nullptr;
    return twin_layer_;
}

void PictureLayerImpl::UpdateRasterSource(
    scoped_refptr<RasterSource> raster_source,
    Region* new_invalidation,
    const PictureLayerTilingSet* pending_set)
{
    // The bounds and the pile size may differ if the pile wasn't updated (ie.
    // PictureLayer::Update didn't happen). In that case the pile will be empty.
    DCHECK_IMPLIES(!raster_source->GetSize().IsEmpty(),
        bounds() == raster_source->GetSize())
        << " bounds " << bounds().ToString() << " pile "
        << raster_source->GetSize().ToString();

    // The |raster_source_| is initially null, so have to check for that for the
    // first frame.
    bool could_have_tilings = raster_source_.get() && CanHaveTilings();
    raster_source_.swap(raster_source);
    // weolar
    //   std::vector<wchar_t> out;
    //   out.resize(300);
    //   swprintf(out.data(), L"PictureLayerImpl::UpdateRasterSource:0x%p, raster_source_:0x%p IsSolidColor:%d DrawsContent:%d\n",
    //     this, raster_source_.get(), raster_source_->IsSolidColor(), DrawsContent());
    //   OutputDebugStringW(out.data());

    // The |new_invalidation| must be cleared before updating tilings since they
    // access the invalidation through the PictureLayerTilingClient interface.
    invalidation_.Clear();
    invalidation_.Swap(new_invalidation);

    bool can_have_tilings = CanHaveTilings();
    DCHECK_IMPLIES(
        pending_set,
        can_have_tilings == GetPendingOrActiveTwinLayer()->CanHaveTilings());

    // Need to call UpdateTiles again if CanHaveTilings changed.
    if (could_have_tilings != can_have_tilings)
        layer_tree_impl()->set_needs_update_draw_properties();

    if (!can_have_tilings) {
        RemoveAllTilings();
        return;
    }

    // We could do this after doing UpdateTiles, which would avoid doing this for
    // tilings that are going to disappear on the pending tree (if scale changed).
    // But that would also be more complicated, so we just do it here for now.
    if (pending_set) {
        tilings_->UpdateTilingsToCurrentRasterSourceForActivation(
            raster_source_, pending_set, invalidation_, MinimumContentsScale(),
            MaximumContentsScale());
    } else {
        tilings_->UpdateTilingsToCurrentRasterSourceForCommit(
            raster_source_, invalidation_, MinimumContentsScale(),
            MaximumContentsScale());
    }
}

void PictureLayerImpl::UpdateCanUseLCDTextAfterCommit()
{
    // This function is only allowed to be called after commit, due to it not
    // being smart about sharing tiles and because otherwise it would cause
    // flashes by switching out tiles in place that may be currently on screen.
    DCHECK(layer_tree_impl()->IsSyncTree());

    // Don't allow the LCD text state to change once disabled.
    if (!RasterSourceUsesLCDText())
        return;
    if (can_use_lcd_text() == RasterSourceUsesLCDText())
        return;

    // Raster sources are considered const, so in order to update the state
    // a new one must be created and all tiles recreated.
    scoped_refptr<RasterSource> new_raster_source = raster_source_->CreateCloneWithoutLCDText();
    raster_source_.swap(new_raster_source);

    // Synthetically invalidate everything.
    gfx::Rect bounds_rect(bounds());
    invalidation_ = Region(bounds_rect);
    tilings_->UpdateRasterSourceDueToLCDChange(raster_source_, invalidation_);
    SetUpdateRect(bounds_rect);

    DCHECK(!RasterSourceUsesLCDText());
}

bool PictureLayerImpl::RasterSourceUsesLCDText() const
{
    return raster_source_ ? raster_source_->CanUseLCDText()
                          : layer_tree_impl()->settings().can_use_lcd_text;
}

void PictureLayerImpl::NotifyTileStateChanged(const Tile* tile)
{
    if (layer_tree_impl()->IsActiveTree()) {
        gfx::Rect layer_damage_rect = gfx::ScaleToEnclosingRect(
            tile->content_rect(), 1.f / tile->contents_scale());
        AddDamageRect(layer_damage_rect);
    }
    if (tile->draw_info().NeedsRaster()) {
        PictureLayerTiling* tiling = tilings_->FindTilingWithScale(tile->contents_scale());
        if (tiling)
            tiling->set_all_tiles_done(false);
    }
}

void PictureLayerImpl::DidBeginTracing()
{
    raster_source_->DidBeginTracing();
}

void PictureLayerImpl::ReleaseResources()
{
    // Recreate tilings with new settings, since some of those might change when
    // we release resources.
    tilings_ = nullptr;
    ResetRasterScale();
}

void PictureLayerImpl::RecreateResources()
{
    tilings_ = CreatePictureLayerTilingSet();

    // To avoid an edge case after lost context where the tree is up to date but
    // the tilings have not been managed, request an update draw properties
    // to force tilings to get managed.
    layer_tree_impl()->set_needs_update_draw_properties();
}

skia::RefPtr<SkPicture> PictureLayerImpl::GetPicture()
{
    return raster_source_->GetFlattenedPicture();
}

Region PictureLayerImpl::GetInvalidationRegion()
{
    // |invalidation_| gives the invalidation contained in the source frame, but
    // is not cleared after drawing from the layer. However, update_rect() is
    // cleared once the invalidation is drawn, which is useful for debugging
    // visualizations. This method intersects the two to give a more exact
    // representation of what was invalidated that is cleared after drawing.
    return IntersectRegions(invalidation_, update_rect());
}

ScopedTilePtr PictureLayerImpl::CreateTile(const Tile::CreateInfo& info)
{
    int flags = 0;

    // We don't handle solid color masks, so we shouldn't bother analyzing those.
    // Otherwise, always analyze to maximize memory savings.
    if (!is_mask_)
        flags = Tile::USE_PICTURE_ANALYSIS;

    if (contents_opaque())
        flags |= Tile::IS_OPAQUE;

    return layer_tree_impl()->tile_manager()->CreateTile(
        info, id(), layer_tree_impl()->source_frame_number(), flags);
}

const Region* PictureLayerImpl::GetPendingInvalidation()
{
    if (layer_tree_impl()->IsPendingTree())
        return &invalidation_;
    if (layer_tree_impl()->IsRecycleTree())
        return nullptr;
    DCHECK(layer_tree_impl()->IsActiveTree());
    if (PictureLayerImpl* twin_layer = GetPendingOrActiveTwinLayer())
        return &twin_layer->invalidation_;
    return nullptr;
}

const PictureLayerTiling* PictureLayerImpl::GetPendingOrActiveTwinTiling(
    const PictureLayerTiling* tiling) const
{
    PictureLayerImpl* twin_layer = GetPendingOrActiveTwinLayer();
    if (!twin_layer)
        return nullptr;
    return twin_layer->tilings_->FindTilingWithScale(tiling->contents_scale());
}

bool PictureLayerImpl::RequiresHighResToDraw() const
{
    return layer_tree_impl()->RequiresHighResToDraw();
}

gfx::Rect PictureLayerImpl::GetEnclosingRectInTargetSpace() const
{
    return GetScaledEnclosingRectInTargetSpace(MaximumTilingContentsScale());
}

gfx::Size PictureLayerImpl::CalculateTileSize(
    const gfx::Size& content_bounds) const
{
    int max_texture_size = layer_tree_impl()->resource_provider()->max_texture_size();

    if (is_mask_) {
        // Masks are not tiled, so if we can't cover the whole mask with one tile,
        // we shouldn't have such a tiling at all.
        DCHECK_LE(content_bounds.width(), max_texture_size);
        DCHECK_LE(content_bounds.height(), max_texture_size);
        return content_bounds;
    }

    int default_tile_width = 0;
    int default_tile_height = 0;
    if (layer_tree_impl()->use_gpu_rasterization()) {
        // For GPU rasterization, we pick an ideal tile size using the viewport
        // so we don't need any settings. The current approach uses 4 tiles
        // to cover the viewport vertically.
        int viewport_width = gpu_raster_max_texture_size_.width();
        int viewport_height = gpu_raster_max_texture_size_.height();
        default_tile_width = viewport_width;

        // Also, increase the height proportionally as the width decreases, and
        // pad by our border texels to make the tiles exactly match the viewport.
        int divisor = 4;
        if (content_bounds.width() <= viewport_width / 2)
            divisor = 2;
        if (content_bounds.width() <= viewport_width / 4)
            divisor = 1;
        default_tile_height = MathUtil::UncheckedRoundUp(viewport_height, divisor) / divisor;

        // Grow default sizes to account for overlapping border texels.
        default_tile_width += 2 * PictureLayerTiling::kBorderTexels;
        default_tile_height += 2 * PictureLayerTiling::kBorderTexels;

        default_tile_height = std::max(default_tile_height, kMinHeightForGpuRasteredTile);
    } else {
        // For CPU rasterization we use tile-size settings.
        const LayerTreeSettings& settings = layer_tree_impl()->settings();
        int max_untiled_content_width = settings.max_untiled_layer_size.width();
        int max_untiled_content_height = settings.max_untiled_layer_size.height();
        default_tile_width = settings.default_tile_size.width();
        default_tile_height = settings.default_tile_size.height();

        // If the content width is small, increase tile size vertically.
        // If the content height is small, increase tile size horizontally.
        // If both are less than the untiled-size, use a single tile.
        if (content_bounds.width() < default_tile_width)
            default_tile_height = max_untiled_content_height;
        if (content_bounds.height() < default_tile_height)
            default_tile_width = max_untiled_content_width;
        if (content_bounds.width() < max_untiled_content_width && content_bounds.height() < max_untiled_content_height) {
            default_tile_height = max_untiled_content_height;
            default_tile_width = max_untiled_content_width;
        }
    }

    int tile_width = default_tile_width;
    int tile_height = default_tile_height;

    // Clamp the tile width/height to the content width/height to save space.
    if (content_bounds.width() < default_tile_width) {
        tile_width = std::min(tile_width, content_bounds.width());
        tile_width = MathUtil::UncheckedRoundUp(tile_width, kTileRoundUp);
        tile_width = std::min(tile_width, default_tile_width);
    }
    if (content_bounds.height() < default_tile_height) {
        tile_height = std::min(tile_height, content_bounds.height());
        tile_height = MathUtil::UncheckedRoundUp(tile_height, kTileRoundUp);
        tile_height = std::min(tile_height, default_tile_height);
    }

    // Under no circumstance should we be larger than the max texture size.
    tile_width = std::min(tile_width, max_texture_size);
    tile_height = std::min(tile_height, max_texture_size);
    return gfx::Size(tile_width, tile_height);
}

void PictureLayerImpl::GetContentsResourceId(ResourceId* resource_id,
    gfx::Size* resource_size) const
{
    // The bounds and the pile size may differ if the pile wasn't updated (ie.
    // PictureLayer::Update didn't happen). In that case the pile will be empty.
    DCHECK_IMPLIES(!raster_source_->GetSize().IsEmpty(),
        bounds() == raster_source_->GetSize())
        << " bounds " << bounds().ToString() << " pile "
        << raster_source_->GetSize().ToString();
    gfx::Rect content_rect(bounds());
    PictureLayerTilingSet::CoverageIterator iter(
        tilings_.get(), 1.f, content_rect, ideal_contents_scale_);

    // Mask resource not ready yet.
    if (!iter || !*iter) {
        *resource_id = 0;
        return;
    }

    // Masks only supported if they fit on exactly one tile.
    DCHECK(iter.geometry_rect() == content_rect)
        << "iter rect " << iter.geometry_rect().ToString() << " content rect "
        << content_rect.ToString();

    const TileDrawInfo& draw_info = iter->draw_info();
    if (!draw_info.IsReadyToDraw() || draw_info.mode() != TileDrawInfo::RESOURCE_MODE) {
        *resource_id = 0;
        return;
    }

    *resource_id = draw_info.resource_id();
    *resource_size = draw_info.resource_size();
}

void PictureLayerImpl::SetNearestNeighbor(bool nearest_neighbor)
{
    if (nearest_neighbor_ == nearest_neighbor)
        return;

    nearest_neighbor_ = nearest_neighbor;
    NoteLayerPropertyChanged();
}

PictureLayerTiling* PictureLayerImpl::AddTiling(float contents_scale)
{
    DCHECK(CanHaveTilings());
    DCHECK_GE(contents_scale, MinimumContentsScale());
    DCHECK_LE(contents_scale, MaximumContentsScale());
    DCHECK(raster_source_->HasRecordings());
    return tilings_->AddTiling(contents_scale, raster_source_);
}

void PictureLayerImpl::RemoveAllTilings()
{
    tilings_->RemoveAllTilings();
    // If there are no tilings, then raster scales are no longer meaningful.
    ResetRasterScale();
}

void PictureLayerImpl::AddTilingsForRasterScale()
{
    // Reset all resolution enums on tilings, we'll be setting new values in this
    // function.
    tilings_->MarkAllTilingsNonIdeal();

    PictureLayerTiling* high_res = tilings_->FindTilingWithScale(raster_contents_scale_);
    if (!high_res) {
        // We always need a high res tiling, so create one if it doesn't exist.
        high_res = AddTiling(raster_contents_scale_);
    } else if (high_res->may_contain_low_resolution_tiles()) {
        // If the tiling we find here was LOW_RESOLUTION previously, it may not be
        // fully rastered, so destroy the old tiles.
        high_res->Reset();
        // Reset the flag now that we'll make it high res, it will have fully
        // rastered content.
        high_res->reset_may_contain_low_resolution_tiles();
    }
    high_res->set_resolution(HIGH_RESOLUTION);

    if (layer_tree_impl()->IsPendingTree()) {
        // On the pending tree, drop any tilings that are non-ideal since we don't
        // need them to activate anyway.
        tilings_->RemoveNonIdealTilings();
    }

    SanityCheckTilingState();
}

bool PictureLayerImpl::ShouldAdjustRasterScale() const
{
    if (was_screen_space_transform_animating_ != draw_properties().screen_space_transform_is_animating)
        return true;

    if (draw_properties().screen_space_transform_is_animating && raster_contents_scale_ != ideal_contents_scale_ && ShouldAdjustRasterScaleDuringScaleAnimations())
        return true;

    bool is_pinching = layer_tree_impl()->PinchGestureActive();
    if (is_pinching && raster_page_scale_) {
        // We change our raster scale when it is:
        // - Higher than ideal (need a lower-res tiling available)
        // - Too far from ideal (need a higher-res tiling available)
        float ratio = ideal_page_scale_ / raster_page_scale_;
        if (raster_page_scale_ > ideal_page_scale_ || ratio > kMaxScaleRatioDuringPinch)
            return true;
    }

    if (!is_pinching) {
        // When not pinching, match the ideal page scale factor.
        if (raster_page_scale_ != ideal_page_scale_)
            return true;
    }

    // Always match the ideal device scale factor.
    if (raster_device_scale_ != ideal_device_scale_)
        return true;

    // When the source scale changes we want to match it, but not when animating
    // or when we've fixed the scale in place.
    if (!draw_properties().screen_space_transform_is_animating && !raster_source_scale_is_fixed_ && raster_source_scale_ != ideal_source_scale_)
        return true;

    if (raster_contents_scale_ > MaximumContentsScale())
        return true;
    if (raster_contents_scale_ < MinimumContentsScale())
        return true;

    return false;
}

void PictureLayerImpl::AddLowResolutionTilingIfNeeded()
{
    DCHECK(layer_tree_impl()->IsActiveTree());

    if (!layer_tree_impl()->create_low_res_tiling())
        return;

    // We should have a high resolution tiling at raster_contents_scale, so if the
    // low res one is the same then we shouldn't try to override this tiling by
    // marking it as a low res.
    if (raster_contents_scale_ == low_res_raster_contents_scale_)
        return;

    PictureLayerTiling* low_res = tilings_->FindTilingWithScale(low_res_raster_contents_scale_);
    DCHECK_IMPLIES(low_res, low_res->resolution() != HIGH_RESOLUTION);

    // Only create new low res tilings when the transform is static.  This
    // prevents wastefully creating a paired low res tiling for every new high
    // res tiling during a pinch or a CSS animation.
    bool is_pinching = layer_tree_impl()->PinchGestureActive();
    bool is_animating = draw_properties().screen_space_transform_is_animating;
    if (!is_pinching && !is_animating) {
        if (!low_res)
            low_res = AddTiling(low_res_raster_contents_scale_);
        low_res->set_resolution(LOW_RESOLUTION);
    }
}

void PictureLayerImpl::RecalculateRasterScales()
{
    float old_raster_contents_scale = raster_contents_scale_;
    float old_raster_page_scale = raster_page_scale_;
    float old_raster_source_scale = raster_source_scale_;

    raster_device_scale_ = ideal_device_scale_;
    raster_page_scale_ = ideal_page_scale_;
    raster_source_scale_ = ideal_source_scale_;
    raster_contents_scale_ = ideal_contents_scale_;

    // If we're not animating, or leaving an animation, and the
    // ideal_source_scale_ changes, then things are unpredictable, and we fix
    // the raster_source_scale_ in place.
    if (old_raster_source_scale && !draw_properties().screen_space_transform_is_animating && !was_screen_space_transform_animating_ && old_raster_source_scale != ideal_source_scale_)
        raster_source_scale_is_fixed_ = true;

    // TODO(danakj): Adjust raster source scale closer to ideal source scale at
    // a throttled rate. Possibly make use of invalidation_.IsEmpty() on pending
    // tree. This will allow CSS scale changes to get re-rastered at an
    // appropriate rate. (crbug.com/413636)
    if (raster_source_scale_is_fixed_) {
        raster_contents_scale_ /= raster_source_scale_;
        raster_source_scale_ = 1.f;
    }

    // During pinch we completely ignore the current ideal scale, and just use
    // a multiple of the previous scale.
    bool is_pinching = layer_tree_impl()->PinchGestureActive();
    if (is_pinching && old_raster_contents_scale) {
        // See ShouldAdjustRasterScale:
        // - When zooming out, preemptively create new tiling at lower resolution.
        // - When zooming in, approximate ideal using multiple of kMaxScaleRatio.
        bool zooming_out = old_raster_page_scale > ideal_page_scale_;
        float desired_contents_scale = old_raster_contents_scale;
        if (zooming_out) {
            while (desired_contents_scale > ideal_contents_scale_)
                desired_contents_scale /= kMaxScaleRatioDuringPinch;
        } else {
            while (desired_contents_scale < ideal_contents_scale_)
                desired_contents_scale *= kMaxScaleRatioDuringPinch;
        }
        raster_contents_scale_ = tilings_->GetSnappedContentsScale(
            desired_contents_scale, kSnapToExistingTilingRatio);
        raster_page_scale_ = raster_contents_scale_ / raster_device_scale_ / raster_source_scale_;
    }

    // If we're not re-rasterizing during animation, rasterize at the maximum
    // scale that will occur during the animation, if the maximum scale is
    // known. However we want to avoid excessive memory use. If the scale is
    // smaller than what we would choose otherwise, then it's always better off
    // for us memory-wise. But otherwise, we don't choose a scale at which this
    // layer's rastered content would become larger than the viewport.
    if (draw_properties().screen_space_transform_is_animating && !ShouldAdjustRasterScaleDuringScaleAnimations()) {
        bool can_raster_at_maximum_scale = false;
        bool should_raster_at_starting_scale = false;
        float maximum_scale = draw_properties().maximum_animation_contents_scale;
        float starting_scale = draw_properties().starting_animation_contents_scale;
        if (maximum_scale) {
            gfx::Size bounds_at_maximum_scale = gfx::ScaleToCeiledSize(raster_source_->GetSize(), maximum_scale);
            int64 maximum_area = static_cast<int64>(bounds_at_maximum_scale.width()) * static_cast<int64>(bounds_at_maximum_scale.height());
            gfx::Size viewport = layer_tree_impl()->device_viewport_size();
            int64 viewport_area = static_cast<int64>(viewport.width()) * static_cast<int64>(viewport.height());
            if (maximum_area <= viewport_area)
                can_raster_at_maximum_scale = true;
        }
        if (starting_scale && starting_scale > maximum_scale) {
            gfx::Size bounds_at_starting_scale = gfx::ScaleToCeiledSize(raster_source_->GetSize(), starting_scale);
            int64 start_area = static_cast<int64>(bounds_at_starting_scale.width()) * static_cast<int64>(bounds_at_starting_scale.height());
            gfx::Size viewport = layer_tree_impl()->device_viewport_size();
            int64 viewport_area = static_cast<int64>(viewport.width()) * static_cast<int64>(viewport.height());
            if (start_area <= viewport_area)
                should_raster_at_starting_scale = true;
        }
        // Use the computed scales for the raster scale directly, do not try to use
        // the ideal scale here. The current ideal scale may be way too large in the
        // case of an animation with scale, and will be constantly changing.
        if (should_raster_at_starting_scale)
            raster_contents_scale_ = starting_scale;
        else if (can_raster_at_maximum_scale)
            raster_contents_scale_ = maximum_scale;
        else
            raster_contents_scale_ = 1.f * ideal_page_scale_ * ideal_device_scale_;
    }

    raster_contents_scale_ = std::max(raster_contents_scale_, MinimumContentsScale());
    raster_contents_scale_ = std::min(raster_contents_scale_, MaximumContentsScale());
    DCHECK_GE(raster_contents_scale_, MinimumContentsScale());
    DCHECK_LE(raster_contents_scale_, MaximumContentsScale());

    // If this layer would create zero or one tiles at this content scale,
    // don't create a low res tiling.
    gfx::Size raster_bounds = gfx::ScaleToCeiledSize(raster_source_->GetSize(), raster_contents_scale_);
    gfx::Size tile_size = CalculateTileSize(raster_bounds);
    bool tile_covers_bounds = tile_size.width() >= raster_bounds.width() && tile_size.height() >= raster_bounds.height();
    if (tile_size.IsEmpty() || tile_covers_bounds) {
        low_res_raster_contents_scale_ = raster_contents_scale_;
        return;
    }

    float low_res_factor = layer_tree_impl()->settings().low_res_contents_scale_factor;
    low_res_raster_contents_scale_ = std::max(raster_contents_scale_ * low_res_factor, MinimumContentsScale());
    DCHECK_LE(low_res_raster_contents_scale_, raster_contents_scale_);
    DCHECK_GE(low_res_raster_contents_scale_, MinimumContentsScale());
    DCHECK_LE(low_res_raster_contents_scale_, MaximumContentsScale());
}

void PictureLayerImpl::CleanUpTilingsOnActiveLayer(
    const std::vector<PictureLayerTiling*>& used_tilings)
{
    DCHECK(layer_tree_impl()->IsActiveTree());
    if (tilings_->num_tilings() == 0)
        return;

    float min_acceptable_high_res_scale = std::min(
        raster_contents_scale_, ideal_contents_scale_);
    float max_acceptable_high_res_scale = std::max(
        raster_contents_scale_, ideal_contents_scale_);

    PictureLayerImpl* twin = GetPendingOrActiveTwinLayer();
    if (twin && twin->CanHaveTilings()) {
        min_acceptable_high_res_scale = std::min(
            min_acceptable_high_res_scale,
            std::min(twin->raster_contents_scale_, twin->ideal_contents_scale_));
        max_acceptable_high_res_scale = std::max(
            max_acceptable_high_res_scale,
            std::max(twin->raster_contents_scale_, twin->ideal_contents_scale_));
    }

    PictureLayerTilingSet* twin_set = twin ? twin->tilings_.get() : nullptr;
    tilings_->CleanUpTilings(min_acceptable_high_res_scale,
        max_acceptable_high_res_scale, used_tilings,
        twin_set);
    DCHECK_GT(tilings_->num_tilings(), 0u);
    SanityCheckTilingState();
}

float PictureLayerImpl::MinimumContentsScale() const
{
    float setting_min = layer_tree_impl()->settings().minimum_contents_scale;

    // If the contents scale is less than 1 / width (also for height),
    // then it will end up having less than one pixel of content in that
    // dimension.  Bump the minimum contents scale up in this case to prevent
    // this from happening.
    int min_dimension = std::min(raster_source_->GetSize().width(),
        raster_source_->GetSize().height());
    if (!min_dimension)
        return setting_min;

    return std::max(1.f / min_dimension, setting_min);
}

float PictureLayerImpl::MaximumContentsScale() const
{
    // Masks can not have tilings that would become larger than the
    // max_texture_size since they use a single tile for the entire
    // tiling. Other layers can have tilings of any scale.
    if (!is_mask_)
        return std::numeric_limits<float>::max();

    int max_texture_size = layer_tree_impl()->resource_provider()->max_texture_size();
    float max_scale_width = static_cast<float>(max_texture_size) / bounds().width();
    float max_scale_height = static_cast<float>(max_texture_size) / bounds().height();
    float max_scale = std::min(max_scale_width, max_scale_height);
    // We require that multiplying the layer size by the contents scale and
    // ceiling produces a value <= |max_texture_size|. Because for large layer
    // sizes floating point ambiguity may crop up, making the result larger or
    // smaller than expected, we use a slightly smaller floating point value for
    // the scale, to help ensure that the resulting content bounds will never end
    // up larger than |max_texture_size|.
    return nextafterf(max_scale, 0.f);
}

void PictureLayerImpl::ResetRasterScale()
{
    raster_page_scale_ = 0.f;
    raster_device_scale_ = 0.f;
    raster_source_scale_ = 0.f;
    raster_contents_scale_ = 0.f;
    low_res_raster_contents_scale_ = 0.f;
    raster_source_scale_is_fixed_ = false;
}

bool PictureLayerImpl::CanHaveTilings() const
{
    if (raster_source_->IsSolidColor())
        return false;
    if (!DrawsContent())
        return false;
    if (!raster_source_->HasRecordings())
        return false;
    // If the |raster_source_| has a recording it should have non-empty bounds.
    DCHECK(!raster_source_->GetSize().IsEmpty());
    if (MaximumContentsScale() < MinimumContentsScale())
        return false;
    return true;
}

void PictureLayerImpl::SanityCheckTilingState() const
{
#if DCHECK_IS_ON()
    if (!CanHaveTilings()) {
        DCHECK_EQ(0u, tilings_->num_tilings());
        return;
    }
    if (tilings_->num_tilings() == 0)
        return;

    // We should only have one high res tiling.
    DCHECK_EQ(1, tilings_->NumHighResTilings());
#endif
}

bool PictureLayerImpl::ShouldAdjustRasterScaleDuringScaleAnimations() const
{
    return layer_tree_impl()->use_gpu_rasterization();
}

float PictureLayerImpl::MaximumTilingContentsScale() const
{
    float max_contents_scale = tilings_->GetMaximumContentsScale();
    return std::max(max_contents_scale, MinimumContentsScale());
}

scoped_ptr<PictureLayerTilingSet>
PictureLayerImpl::CreatePictureLayerTilingSet()
{
    const LayerTreeSettings& settings = layer_tree_impl()->settings();
    return PictureLayerTilingSet::Create(
        GetTree(), this, settings.tiling_interest_area_padding,
        layer_tree_impl()->use_gpu_rasterization()
            ? settings.gpu_rasterization_skewport_target_time_in_seconds
            : settings.skewport_target_time_in_seconds,
        settings.skewport_extrapolation_limit_in_content_pixels);
}

void PictureLayerImpl::UpdateIdealScales()
{
    DCHECK(CanHaveTilings());

    float min_contents_scale = MinimumContentsScale();
    DCHECK_GT(min_contents_scale, 0.f);

    ideal_page_scale_ = IsAffectedByPageScale()
        ? layer_tree_impl()->current_page_scale_factor()
        : 1.f;
    ideal_device_scale_ = layer_tree_impl()->device_scale_factor();
    ideal_contents_scale_ = std::max(GetIdealContentsScale(), min_contents_scale);
    ideal_source_scale_ = ideal_contents_scale_ / ideal_page_scale_ / ideal_device_scale_;
}

void PictureLayerImpl::GetDebugBorderProperties(
    SkColor* color,
    float* width) const
{
    *color = DebugColors::TiledContentLayerBorderColor();
    *width = DebugColors::TiledContentLayerBorderWidth(layer_tree_impl());
}

void PictureLayerImpl::GetAllPrioritizedTilesForTracing(
    std::vector<PrioritizedTile>* prioritized_tiles) const
{
    if (!tilings_)
        return;
    tilings_->GetAllPrioritizedTilesForTracing(prioritized_tiles);
}

// void PictureLayerImpl::AsValueInto(
//     base::trace_event::TracedValue* state) const {
//   LayerImpl::AsValueInto(state);
//   state->SetDouble("ideal_contents_scale", ideal_contents_scale_);
//   state->SetDouble("geometry_contents_scale", MaximumTilingContentsScale());
//   state->BeginArray("tilings");
//   tilings_->AsValueInto(state);
//   state->EndArray();
//
//   MathUtil::AddToTracedValue("tile_priority_rect",
//                              viewport_rect_for_tile_priority_in_content_space_,
//                              state);
//   MathUtil::AddToTracedValue("visible_rect", visible_layer_rect(), state);
//
//   state->BeginArray("pictures");
//   raster_source_->AsValueInto(state);
//   state->EndArray();
//
//   state->BeginArray("invalidation");
//   invalidation_.AsValueInto(state);
//   state->EndArray();
//
//   state->BeginArray("coverage_tiles");
//   for (PictureLayerTilingSet::CoverageIterator iter(
//            tilings_.get(), 1.f, gfx::Rect(raster_source_->GetSize()),
//            ideal_contents_scale_);
//        iter; ++iter) {
//     state->BeginDictionary();
//
//     MathUtil::AddToTracedValue("geometry_rect", iter.geometry_rect(), state);
//
//     if (*iter)
//       TracedValue::SetIDRef(*iter, state, "tile");
//
//     state->EndDictionary();
//   }
//   state->EndArray();
// }

size_t PictureLayerImpl::GPUMemoryUsageInBytes() const
{
    return tilings_->GPUMemoryUsageInBytes();
}

void PictureLayerImpl::RunMicroBenchmark(MicroBenchmarkImpl* benchmark)
{
    benchmark->RunOnLayer(this);
}

WhichTree PictureLayerImpl::GetTree() const
{
    return layer_tree_impl()->IsActiveTree() ? ACTIVE_TREE : PENDING_TREE;
}

bool PictureLayerImpl::IsOnActiveOrPendingTree() const
{
    return !layer_tree_impl()->IsRecycleTree();
}

bool PictureLayerImpl::HasValidTilePriorities() const
{
    return IsOnActiveOrPendingTree() && IsDrawnRenderSurfaceLayerListMember();
}

} // namespace cc
