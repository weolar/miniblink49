// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RASTER_RASTER_BUFFER_H_
#define CC_RASTER_RASTER_BUFFER_H_

#include "cc/base/cc_export.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {
class RasterSource;

class CC_EXPORT RasterBuffer {
public:
    RasterBuffer();
    virtual ~RasterBuffer();

    virtual void Playback(const RasterSource* raster_source,
        const gfx::Rect& raster_full_rect,
        const gfx::Rect& raster_dirty_rect,
        uint64_t new_content_id,
        float scale,
        bool include_images)
        = 0;
};

} // namespace cc

#endif // CC_RASTER_RASTER_BUFFER_H_
