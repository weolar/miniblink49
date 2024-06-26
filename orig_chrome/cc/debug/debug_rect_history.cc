// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/debug_rect_history.h"

#include "cc/base/math_util.h"
#include "cc/layers/layer_impl.h"
#include "cc/layers/layer_iterator.h"
#include "cc/layers/layer_utils.h"
#include "cc/layers/render_surface_impl.h"
#include "cc/trees/damage_tracker.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_common.h"
#include "ui/gfx/geometry/rect_conversions.h"

namespace cc {

// static
scoped_ptr<DebugRectHistory> DebugRectHistory::Create()
{
    return make_scoped_ptr(new DebugRectHistory());
}

DebugRectHistory::DebugRectHistory() { }

DebugRectHistory::~DebugRectHistory() { }

void DebugRectHistory::SaveDebugRectsForCurrentFrame(
    LayerImpl* root_layer,
    LayerImpl* hud_layer,
    const LayerImplList& render_surface_layer_list,
    const LayerTreeDebugState& debug_state)
{
    // For now, clear all rects from previous frames. In the future we may want to
    // store all debug rects for a history of many frames.
    debug_rects_.clear();

    if (debug_state.show_touch_event_handler_rects)
        SaveTouchEventHandlerRects(root_layer);

    if (debug_state.show_wheel_event_handler_rects)
        SaveWheelEventHandlerRects(root_layer);

    if (debug_state.show_scroll_event_handler_rects)
        SaveScrollEventHandlerRects(root_layer);

    if (debug_state.show_non_fast_scrollable_rects)
        SaveNonFastScrollableRects(root_layer);

    if (debug_state.show_paint_rects)
        SavePaintRects(root_layer);

    if (debug_state.show_property_changed_rects)
        SavePropertyChangedRects(render_surface_layer_list, hud_layer);

    if (debug_state.show_surface_damage_rects)
        SaveSurfaceDamageRects(render_surface_layer_list);

    if (debug_state.show_screen_space_rects)
        SaveScreenSpaceRects(render_surface_layer_list);

    if (debug_state.show_layer_animation_bounds_rects)
        SaveLayerAnimationBoundsRects(render_surface_layer_list);
}

void DebugRectHistory::SavePaintRects(LayerImpl* layer)
{
    // We would like to visualize where any layer's paint rect (update rect) has
    // changed, regardless of whether this layer is skipped for actual drawing or
    // not. Therefore we traverse recursively over all layers, not just the render
    // surface list.

    Region invalidation_region = layer->GetInvalidationRegion();
    if (!invalidation_region.IsEmpty() && layer->DrawsContent()) {
        for (Region::Iterator it(invalidation_region); it.has_rect(); it.next()) {
            debug_rects_.push_back(DebugRect(
                PAINT_RECT_TYPE, MathUtil::MapEnclosingClippedRect(layer->screen_space_transform(), it.rect())));
        }
    }

    for (unsigned i = 0; i < layer->children().size(); ++i)
        SavePaintRects(layer->children()[i]);
}

void DebugRectHistory::SavePropertyChangedRects(
    const LayerImplList& render_surface_layer_list,
    LayerImpl* hud_layer)
{
    for (size_t i = 0; i < render_surface_layer_list.size(); ++i) {
        size_t surface_index = render_surface_layer_list.size() - 1 - i;
        LayerImpl* render_surface_layer = render_surface_layer_list[surface_index];
        RenderSurfaceImpl* render_surface = render_surface_layer->render_surface();
        DCHECK(render_surface);

        const LayerImplList& layer_list = render_surface->layer_list();
        for (unsigned layer_index = 0;
             layer_index < layer_list.size();
             ++layer_index) {
            LayerImpl* layer = layer_list[layer_index];

            if (LayerTreeHostCommon::RenderSurfaceContributesToTarget<LayerImpl>(
                    layer, render_surface_layer->id()))
                continue;

            if (layer == hud_layer)
                continue;

            if (!layer->LayerPropertyChanged())
                continue;

            debug_rects_.push_back(DebugRect(
                PROPERTY_CHANGED_RECT_TYPE,
                MathUtil::MapEnclosingClippedRect(layer->screen_space_transform(),
                    gfx::Rect(layer->bounds()))));
        }
    }
}

void DebugRectHistory::SaveSurfaceDamageRects(
    const LayerImplList& render_surface_layer_list)
{
    for (size_t i = 0; i < render_surface_layer_list.size(); ++i) {
        size_t surface_index = render_surface_layer_list.size() - 1 - i;
        LayerImpl* render_surface_layer = render_surface_layer_list[surface_index];
        RenderSurfaceImpl* render_surface = render_surface_layer->render_surface();
        DCHECK(render_surface);

        debug_rects_.push_back(DebugRect(
            SURFACE_DAMAGE_RECT_TYPE,
            MathUtil::MapEnclosingClippedRect(
                render_surface->screen_space_transform(),
                render_surface->damage_tracker()->current_damage_rect())));
    }
}

void DebugRectHistory::SaveScreenSpaceRects(
    const LayerImplList& render_surface_layer_list)
{
    for (size_t i = 0; i < render_surface_layer_list.size(); ++i) {
        size_t surface_index = render_surface_layer_list.size() - 1 - i;
        LayerImpl* render_surface_layer = render_surface_layer_list[surface_index];
        RenderSurfaceImpl* render_surface = render_surface_layer->render_surface();
        DCHECK(render_surface);

        debug_rects_.push_back(
            DebugRect(SCREEN_SPACE_RECT_TYPE,
                MathUtil::MapEnclosingClippedRect(
                    render_surface->screen_space_transform(),
                    render_surface->content_rect())));

        if (render_surface_layer->replica_layer()) {
            debug_rects_.push_back(
                DebugRect(REPLICA_SCREEN_SPACE_RECT_TYPE,
                    MathUtil::MapEnclosingClippedRect(
                        render_surface->replica_screen_space_transform(),
                        render_surface->content_rect())));
        }
    }
}

void DebugRectHistory::SaveTouchEventHandlerRects(LayerImpl* layer)
{
    LayerTreeHostCommon::CallFunctionForSubtree(layer, [this](LayerImpl* layer) {
        SaveTouchEventHandlerRectsCallback(layer);
    });
}

void DebugRectHistory::SaveTouchEventHandlerRectsCallback(LayerImpl* layer)
{
    for (Region::Iterator iter(layer->touch_event_handler_region());
         iter.has_rect();
         iter.next()) {
        debug_rects_.push_back(
            DebugRect(TOUCH_EVENT_HANDLER_RECT_TYPE,
                MathUtil::MapEnclosingClippedRect(
                    layer->screen_space_transform(), iter.rect())));
    }
}

void DebugRectHistory::SaveWheelEventHandlerRects(LayerImpl* layer)
{
    LayerTreeHostCommon::CallFunctionForSubtree(layer, [this](LayerImpl* layer) {
        SaveWheelEventHandlerRectsCallback(layer);
    });
}

void DebugRectHistory::SaveWheelEventHandlerRectsCallback(LayerImpl* layer)
{
    if (!layer->have_wheel_event_handlers())
        return;

    debug_rects_.push_back(DebugRect(
        WHEEL_EVENT_HANDLER_RECT_TYPE,
        MathUtil::MapEnclosingClippedRect(layer->screen_space_transform(),
            gfx::Rect(layer->bounds()))));
}

void DebugRectHistory::SaveScrollEventHandlerRects(LayerImpl* layer)
{
    LayerTreeHostCommon::CallFunctionForSubtree(layer, [this](LayerImpl* layer) {
        SaveScrollEventHandlerRectsCallback(layer);
    });
}

void DebugRectHistory::SaveScrollEventHandlerRectsCallback(LayerImpl* layer)
{
    if (!layer->have_scroll_event_handlers())
        return;

    debug_rects_.push_back(DebugRect(
        SCROLL_EVENT_HANDLER_RECT_TYPE,
        MathUtil::MapEnclosingClippedRect(layer->screen_space_transform(),
            gfx::Rect(layer->bounds()))));
}

void DebugRectHistory::SaveNonFastScrollableRects(LayerImpl* layer)
{
    LayerTreeHostCommon::CallFunctionForSubtree(layer, [this](LayerImpl* layer) {
        SaveNonFastScrollableRectsCallback(layer);
    });
}

void DebugRectHistory::SaveNonFastScrollableRectsCallback(LayerImpl* layer)
{
    for (Region::Iterator iter(layer->non_fast_scrollable_region());
         iter.has_rect();
         iter.next()) {
        debug_rects_.push_back(
            DebugRect(NON_FAST_SCROLLABLE_RECT_TYPE,
                MathUtil::MapEnclosingClippedRect(
                    layer->screen_space_transform(), iter.rect())));
    }
}

void DebugRectHistory::SaveLayerAnimationBoundsRects(
    const LayerImplList& render_surface_layer_list)
{
    LayerIterator end = LayerIterator::End(&render_surface_layer_list);
    for (LayerIterator it = LayerIterator::Begin(&render_surface_layer_list);
         it != end; ++it) {
        if (!it.represents_itself())
            continue;

        // TODO(avallee): Figure out if we should show something for a layer who's
        // animating bounds but that we can't compute them.
        gfx::BoxF inflated_bounds;
        if (!LayerUtils::GetAnimationBounds(**it, &inflated_bounds))
            continue;

        debug_rects_.push_back(
            DebugRect(ANIMATION_BOUNDS_RECT_TYPE,
                gfx::ToEnclosingRect(gfx::RectF(inflated_bounds.x(),
                    inflated_bounds.y(),
                    inflated_bounds.width(),
                    inflated_bounds.height()))));
    }
}

} // namespace cc
