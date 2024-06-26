// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_DEBUG_RECT_HISTORY_H_
#define CC_DEBUG_DEBUG_RECT_HISTORY_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "cc/layers/layer_lists.h"
#include "ui/gfx/geometry/rect.h"
#include <vector>

namespace cc {

class LayerImpl;
class LayerTreeDebugState;

// There are currently six types of debug rects:
//
// - Paint rects (update rects): regions of a layer that needed to be
// re-uploaded to the texture resource; in most cases implying that they had to
// be repainted, too.
//
// - Property-changed rects: enclosing bounds of layers that cause changes to
// the screen even if the layer did not change internally. (For example, if the
// layer's opacity or position changes.)
//
// - Surface damage rects: the aggregate damage on a target surface that is
// caused by all layers and surfaces that contribute to it. This includes (1)
// paint rects, (2) property- changed rects, and (3) newly exposed areas.
//
// - Screen space rects: this is the region the contents occupy in screen space.
//
// - Replica screen space rects: this is the region the replica's contents
// occupy in screen space.
enum DebugRectType {
    PAINT_RECT_TYPE,
    PROPERTY_CHANGED_RECT_TYPE,
    SURFACE_DAMAGE_RECT_TYPE,
    SCREEN_SPACE_RECT_TYPE,
    REPLICA_SCREEN_SPACE_RECT_TYPE,
    TOUCH_EVENT_HANDLER_RECT_TYPE,
    WHEEL_EVENT_HANDLER_RECT_TYPE,
    SCROLL_EVENT_HANDLER_RECT_TYPE,
    NON_FAST_SCROLLABLE_RECT_TYPE,
    ANIMATION_BOUNDS_RECT_TYPE,
};

struct DebugRect {
    DebugRect(DebugRectType new_type, const gfx::Rect& new_rect)
        : type(new_type)
        , rect(new_rect)
    {
    }

    DebugRectType type;
    gfx::Rect rect;
};

// This class maintains a history of rects of various types that can be used
// for debugging purposes. The overhead of collecting rects is performed only if
// the appropriate LayerTreeSettings are enabled.
class DebugRectHistory {
public:
    static scoped_ptr<DebugRectHistory> Create();

    ~DebugRectHistory();

    // Note: Saving debug rects must happen before layers' change tracking is
    // reset.
    void SaveDebugRectsForCurrentFrame(
        LayerImpl* root_layer,
        LayerImpl* hud_layer,
        const LayerImplList& render_surface_layer_list,
        const LayerTreeDebugState& debug_state);

    const std::vector<DebugRect>& debug_rects() { return debug_rects_; }

private:
    DebugRectHistory();

    void SavePaintRects(LayerImpl* layer);
    void SavePropertyChangedRects(const LayerImplList& render_surface_layer_list,
        LayerImpl* hud_layer);
    void SaveSurfaceDamageRects(
        const LayerImplList& render_surface_layer_list);
    void SaveScreenSpaceRects(
        const LayerImplList& render_surface_layer_list);
    void SaveTouchEventHandlerRects(LayerImpl* layer);
    void SaveTouchEventHandlerRectsCallback(LayerImpl* layer);
    void SaveWheelEventHandlerRects(LayerImpl* layer);
    void SaveWheelEventHandlerRectsCallback(LayerImpl* layer);
    void SaveScrollEventHandlerRects(LayerImpl* layer);
    void SaveScrollEventHandlerRectsCallback(LayerImpl* layer);
    void SaveNonFastScrollableRects(LayerImpl* layer);
    void SaveNonFastScrollableRectsCallback(LayerImpl* layer);
    void SaveLayerAnimationBoundsRects(
        const LayerImplList& render_surface_layer_list);

    std::vector<DebugRect> debug_rects_;

    DISALLOW_COPY_AND_ASSIGN(DebugRectHistory);
};

} // namespace cc

#endif // CC_DEBUG_DEBUG_RECT_HISTORY_H_
