// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_RASTER_SOURCE_HELPER_H_
#define CC_PLAYBACK_RASTER_SOURCE_HELPER_H_

#include "cc/base/cc_export.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/rect.h"

class SkCanvas;

namespace cc {

class CC_EXPORT RasterSourceHelper {
public:
    static void PrepareForPlaybackToCanvas(SkCanvas* canvas,
        const gfx::Rect& canvas_bitmap_rect,
        const gfx::Rect& canvas_playback_rect,
        const gfx::Rect& source_rect,
        float contents_scale,
        SkColor background_color,
        bool clear_canvas_with_debug_color,
        bool requires_clear);
};

} // namespace cc

#endif // CC_PLAYBACK_RASTER_SOURCE_HELPER_H_
