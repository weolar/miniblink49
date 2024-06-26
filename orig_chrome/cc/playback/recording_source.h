// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_RECORDING_SOURCE_H_
#define CC_PLAYBACK_RECORDING_SOURCE_H_

#include "base/memory/ref_counted.h"
#include "cc/base/cc_export.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace cc {
class ContentLayerClient;
class Region;
class RasterSource;

class CC_EXPORT RecordingSource {
public:
    // TODO(schenney) Remove RECORD_WITH_SK_NULL_CANVAS when we no longer
    // support a non-Slimming Paint path.
    enum RecordingMode {
        RECORD_NORMALLY,
        RECORD_WITH_SK_NULL_CANVAS,
        RECORD_WITH_PAINTING_DISABLED,
        RECORD_WITH_CACHING_DISABLED,
        RECORD_WITH_CONSTRUCTION_DISABLED,
        RECORDING_MODE_COUNT, // Must be the last entry.
    };

    virtual ~RecordingSource() { }
    // Re-record parts of the picture that are invalid.
    // Invalidations are in layer space, and will be expanded to cover everything
    // that was either recorded/changed or that has no recording, leaving out only
    // pieces that we had a recording for and it was not changed.
    // Return true iff the pile was modified.
    virtual bool UpdateAndExpandInvalidation(ContentLayerClient* painter,
        Region* invalidation,
        const gfx::Size& layer_size,
        const gfx::Rect& visible_layer_rect,
        int frame_number,
        RecordingMode recording_mode)
        = 0;

    virtual scoped_refptr<RasterSource> CreateRasterSource(
        bool can_use_lcd_text) const = 0;

    virtual gfx::Size GetSize() const = 0;
    virtual void SetEmptyBounds() = 0;
    virtual void SetSlowdownRasterScaleFactor(int factor) = 0;
    virtual void SetGenerateDiscardableImagesMetadata(bool generate_metadata) = 0;
    virtual void SetBackgroundColor(SkColor background_color) = 0;
    virtual void SetRequiresClear(bool requires_clear) = 0;
    virtual bool IsSuitableForGpuRasterization() const = 0;
};

} // namespace cc

#endif // CC_PLAYBACK_RECORDING_SOURCE_H_
