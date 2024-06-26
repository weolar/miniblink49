// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_DISPLAY_ITEM_H_
#define CC_PLAYBACK_DISPLAY_ITEM_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/debug/traced_value.h"
#include "cc/playback/display_item_list_bounds_calculator.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "ui/gfx/geometry/rect.h"

class SkCanvas;

namespace cc {

class CC_EXPORT DisplayItem {
public:
    virtual ~DisplayItem() { }

    void SetNew(bool is_suitable_for_gpu_rasterization,
        int approximate_op_count,
        size_t external_memory_usage)
    {
        is_suitable_for_gpu_rasterization_ = is_suitable_for_gpu_rasterization;
        approximate_op_count_ = approximate_op_count;
        external_memory_usage_ = external_memory_usage;
    }

    virtual void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const = 0;
    virtual void AsValueInto(base::trace_event::TracedValue* array) const = 0;
    virtual void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const = 0;

    bool is_suitable_for_gpu_rasterization() const
    {
        return is_suitable_for_gpu_rasterization_;
    }
    int approximate_op_count() const { return approximate_op_count_; }
    size_t external_memory_usage() const { return external_memory_usage_; }

protected:
    DisplayItem();

    bool is_suitable_for_gpu_rasterization_;
    int approximate_op_count_;

    // The size, in bytes, of the memory owned by this display item but not
    // allocated within it (e.g. held through scoped_ptr or vector).
    size_t external_memory_usage_;
};

} // namespace cc

#endif // CC_PLAYBACK_DISPLAY_ITEM_H_
