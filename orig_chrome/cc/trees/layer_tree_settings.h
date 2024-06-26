// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_LAYER_TREE_SETTINGS_H_
#define CC_TREES_LAYER_TREE_SETTINGS_H_

#include <vector>

#include "base/basictypes.h"
#include "cc/base/cc_export.h"
#include "cc/debug/layer_tree_debug_state.h"
#include "cc/output/renderer_settings.h"
#include "cc/scheduler/scheduler_settings.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class CC_EXPORT LayerSettings {
public:
    LayerSettings();
    ~LayerSettings();

    bool use_compositor_animation_timelines;
};

class CC_EXPORT LayerTreeSettings {
public:
    LayerTreeSettings();
    ~LayerTreeSettings();

    RendererSettings renderer_settings;
    bool single_thread_proxy_scheduler;
    bool use_external_begin_frame_source;
    bool main_frame_before_activation_enabled;
    bool using_synchronous_renderer_compositor;
    bool accelerated_animation_enabled;
    bool can_use_lcd_text;
    bool use_distance_field_text;
    bool gpu_rasterization_enabled;
    bool gpu_rasterization_forced;
    int gpu_rasterization_msaa_sample_count;
    float gpu_rasterization_skewport_target_time_in_seconds;
    bool create_low_res_tiling;

    enum ScrollbarAnimator {
        NO_ANIMATOR,
        LINEAR_FADE,
        THINNING,
    };
    ScrollbarAnimator scrollbar_animator;
    int scrollbar_fade_delay_ms;
    int scrollbar_fade_resize_delay_ms;
    int scrollbar_fade_duration_ms;
    SkColor solid_color_scrollbar_color;
    bool timeout_and_draw_when_animation_checkerboards;
    bool layer_transforms_should_scale_layer_contents;
    bool layers_always_allowed_lcd_text;
    float minimum_contents_scale;
    float low_res_contents_scale_factor;
    float top_controls_show_threshold;
    float top_controls_hide_threshold;
    double background_animation_rate;
    gfx::Size default_tile_size;
    gfx::Size max_untiled_layer_size;
    gfx::Size minimum_occlusion_tracking_size;
    size_t tiling_interest_area_padding;
    float skewport_target_time_in_seconds;
    int skewport_extrapolation_limit_in_content_pixels;
    size_t max_memory_for_prepaint_percentage;
    bool strict_layer_property_change_checking;
    bool use_zero_copy;
    bool use_persistent_map_for_gpu_memory_buffers;
    bool enable_elastic_overscroll;
    // An array of image texture targets for each GpuMemoryBuffer format.
    std::vector<unsigned> use_image_texture_targets;
    bool ignore_root_layer_flings;
    size_t scheduled_raster_task_limit;
    bool use_occlusion_for_tile_prioritization;
    bool record_full_layer;
    bool verify_property_trees;
    bool image_decode_tasks_enabled;
    bool use_compositor_animation_timelines;
    bool wait_for_beginframe_interval;
    int max_staging_buffer_usage_in_bytes;

    LayerTreeDebugState initial_debug_state;

    SchedulerSettings ToSchedulerSettings() const;
};

} // namespace cc

#endif // CC_TREES_LAYER_TREE_SETTINGS_H_
