// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/display_item_list_bounds_calculator.h"

#include "base/logging.h"
#include "ui/gfx/skia_util.h"

namespace cc {

DisplayItemListBoundsCalculator::DisplayItemListBoundsCalculator()
{
    matrix_stack_.push_back(SkMatrix::I());
}

DisplayItemListBoundsCalculator::~DisplayItemListBoundsCalculator() { }

void DisplayItemListBoundsCalculator::AddStartingDisplayItem()
{
    bounds_.push_back(gfx::RectF());
    started_items_indices_.push_back(bounds_.size() - 1);
}

void DisplayItemListBoundsCalculator::AddEndingDisplayItem()
{
    size_t last_start_index = started_items_indices_.back();
    started_items_indices_.pop_back();

    // Ending bounds match the starting bounds.
    bounds_.push_back(bounds_[last_start_index]);

    // The block that ended just now needs to be considered in the bounds of the
    // enclosing block.
    if (!started_items_indices_.empty())
        bounds_[started_items_indices_.back()].Union(bounds_.back());
}

void DisplayItemListBoundsCalculator::AddDisplayItemWithBounds(
    const SkRect& rect)
{
    SkRect target_rect;
    matrix()->mapRect(&target_rect, rect);
    bounds_.push_back(gfx::SkRectToRectF(target_rect));
    if (!started_items_indices_.empty())
        bounds_[started_items_indices_.back()].Union(bounds_.back());
}

void DisplayItemListBoundsCalculator::Save()
{
    matrix_stack_.push_back(matrix_stack_.back());
}

void DisplayItemListBoundsCalculator::Restore()
{
    DCHECK_GT(matrix_stack_.size(), 1u);
    matrix_stack_.pop_back();
}

void DisplayItemListBoundsCalculator::Finalize()
{
    while (!started_items_indices_.empty()) {
        bounds_[started_items_indices_.back()].Union(bounds_.back());
        started_items_indices_.pop_back();
    }
}

} // namespace cc
