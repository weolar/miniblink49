// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_COMPOSITOR_FRAME_METADATA_H_
#define CC_OUTPUT_COMPOSITOR_FRAME_METADATA_H_

#include <vector>

#include "cc/base/cc_export.h"
#include "cc/output/viewport_selection_bound.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/events/latency_info.h"
#include "ui/gfx/geometry/size_f.h"
#include "ui/gfx/geometry/vector2d_f.h"

namespace cc {

class CC_EXPORT CompositorFrameMetadata {
public:
    CompositorFrameMetadata();
    ~CompositorFrameMetadata();

    // The device scale factor used to generate this compositor frame.
    float device_scale_factor;

    // Scroll offset and scale of the root layer. This can be used for tasks
    // like positioning windowed plugins.
    gfx::Vector2dF root_scroll_offset;
    float page_scale_factor;

    // These limits can be used together with the scroll/scale fields above to
    // determine if scrolling/scaling in a particular direction is possible.
    gfx::SizeF scrollable_viewport_size;
    gfx::SizeF root_layer_size;
    float min_page_scale_factor;
    float max_page_scale_factor;
    bool root_overflow_x_hidden;
    bool root_overflow_y_hidden;

    // Used to position the Android location top bar and page content, whose
    // precise position is computed by the renderer compositor.
    gfx::Vector2dF location_bar_offset;
    gfx::Vector2dF location_bar_content_translation;

    // This color is usually obtained from the background color of the <body>
    // element. It can be used for filling in gutter areas around the frame when
    // it's too small to fill the box the parent reserved for it.
    SkColor root_background_color;

    // Provides selection region updates relative to the current viewport. If the
    // selection is empty or otherwise unused, the bound types will indicate such.
    ViewportSelection selection;

    std::vector<ui::LatencyInfo> latency_info;

    // A set of SurfaceSequences that this frame satisfies (always in the same
    // namespace as the current Surface).
    std::vector<uint32_t> satisfies_sequences;
};

} // namespace cc

#endif // CC_OUTPUT_COMPOSITOR_FRAME_METADATA_H_
