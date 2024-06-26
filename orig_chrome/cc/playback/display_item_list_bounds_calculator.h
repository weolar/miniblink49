// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_DISPLAY_ITEM_LIST_BOUNDS_CALCULATOR_H_
#define CC_PLAYBACK_DISPLAY_ITEM_LIST_BOUNDS_CALCULATOR_H_

#include <vector>

#include "third_party/skia/include/core/SkMatrix.h"
#include "ui/gfx/geometry/rect_f.h"

namespace cc {

class DisplayItemListBoundsCalculator {
public:
    DisplayItemListBoundsCalculator();
    ~DisplayItemListBoundsCalculator();

    void AddStartingDisplayItem();
    void AddEndingDisplayItem();
    void AddDisplayItemWithBounds(const SkRect& rect);
    void Finalize();

    void Save();
    void Restore();
    SkMatrix* matrix() { return &matrix_stack_.back(); }

    void TakeBounds(std::vector<gfx::RectF>* bounds) { bounds->swap(bounds_); }

private:
    std::vector<gfx::RectF> bounds_;
    std::vector<size_t> started_items_indices_;
    std::vector<SkMatrix> matrix_stack_;
};

} // namespace cc

#endif // CC_PLAYBACK_DISPLAY_ITEM_LIST_BOUNDS_CALCULATOR_H_
