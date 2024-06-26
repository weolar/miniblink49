// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RASTER_GPU_RASTERIZER_H_
#define CC_RASTER_GPU_RASTERIZER_H_

#include <vector>

#include "cc/base/cc_export.h"
#include "cc/resources/resource_pool.h"
#include "cc/tiles/tile.h"
#include "third_party/skia/include/core/SkMultiPictureDraw.h"

namespace cc {

class ContextProvider;
class ResourceProvider;
class RasterSource;

class CC_EXPORT GpuRasterizer {
public:
    ~GpuRasterizer();

    void RasterizeSource(ResourceProvider::ScopedWriteLockGr* write_lock,
        const RasterSource* raster_source,
        const gfx::Rect& raster_full_rect,
        const gfx::Rect& playback_rect,
        float scale);

    ResourceProvider* resource_provider() { return resource_provider_; }

private:
    GpuRasterizer(ContextProvider* context_provider,
        ResourceProvider* resource_provider,
        bool use_distance_filed_text,
        int msaa_sample_count);

    ResourceProvider* resource_provider_;

    bool use_distance_field_text_;
    int msaa_sample_count_;

    friend class GpuTileTaskWorkerPool;
    DISALLOW_COPY_AND_ASSIGN(GpuRasterizer);
};

} // namespace cc

#endif // CC_RASTER_GPU_RASTERIZER_H_
