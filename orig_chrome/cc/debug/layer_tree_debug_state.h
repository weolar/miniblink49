// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_LAYER_TREE_DEBUG_STATE_H_
#define CC_DEBUG_LAYER_TREE_DEBUG_STATE_H_

#include "base/basictypes.h"
#include "cc/base/cc_export.h"

namespace cc {

class CC_EXPORT LayerTreeDebugState {
public:
    LayerTreeDebugState();
    ~LayerTreeDebugState();

    bool show_fps_counter;
    bool show_debug_borders;

    bool show_paint_rects;
    bool show_property_changed_rects;
    bool show_surface_damage_rects;
    bool show_screen_space_rects;
    bool show_replica_screen_space_rects;
    bool show_touch_event_handler_rects;
    bool show_wheel_event_handler_rects;
    bool show_scroll_event_handler_rects;
    bool show_non_fast_scrollable_rects;
    bool show_layer_animation_bounds_rects;

    int slow_down_raster_scale_factor;
    bool rasterize_only_visible_content;
    bool show_picture_borders;

    void SetRecordRenderingStats(bool enabled);
    bool RecordRenderingStats() const;

    bool ShowHudInfo() const;
    bool ShowHudRects() const;
    bool ShowMemoryStats() const;

    static bool Equal(const LayerTreeDebugState& a, const LayerTreeDebugState& b);
    static LayerTreeDebugState Unite(const LayerTreeDebugState& a,
        const LayerTreeDebugState& b);

private:
    bool record_rendering_stats_;
};

} // namespace cc

#endif // CC_DEBUG_LAYER_TREE_DEBUG_STATE_H_
