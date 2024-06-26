// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TILES_IMAGE_DECODE_CONTROLLER_H_
#define CC_TILES_IMAGE_DECODE_CONTROLLER_H_

#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "cc/base/cc_export.h"
#include "cc/playback/discardable_image_map.h"
#include "cc/raster/tile_task_runner.h"
#include "skia/ext/refptr.h"

namespace cc {

class ImageDecodeController {
public:
    ImageDecodeController();
    ~ImageDecodeController();

    scoped_refptr<ImageDecodeTask> GetTaskForImage(const PositionImage& image,
        int layer_id,
        uint64_t prepare_tiles_id);

    // Note that this function has to remain thread safe.
    void DecodeImage(const SkImage* image);

    // TODO(vmpstr): This should go away once the controller is decoding images
    // based on priority and memory.
    void AddLayerUsedCount(int layer_id);
    void SubtractLayerUsedCount(int layer_id);

    void OnImageDecodeTaskCompleted(int layer_id,
        const SkImage* image,
        bool was_canceled);

private:
    scoped_refptr<ImageDecodeTask> CreateTaskForImage(const SkImage* image,
        int layer_id,
        uint64_t prepare_tiles_id);

    using ImageTaskMap = base::hash_map<uint32_t, scoped_refptr<ImageDecodeTask>>;
    using LayerImageTaskMap = base::hash_map<int, ImageTaskMap>;
    LayerImageTaskMap image_decode_tasks_;

    using LayerCountMap = base::hash_map<int, int>;
    LayerCountMap used_layer_counts_;
};

} // namespace cc

#endif // CC_TILES_IMAGE_DECODE_CONTROLLER_H_
