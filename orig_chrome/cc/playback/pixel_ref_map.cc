// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/pixel_ref_map.h"

#include <algorithm>
#include <limits>

#include "cc/base/math_util.h"
#include "cc/playback/display_item_list.h"
#include "cc/playback/picture.h"
#include "skia/ext/pixel_ref_utils.h"

namespace cc {

PixelRefMap::PixelRefMap(const gfx::Size& cell_size)
    : cell_size_(cell_size)
{
    DCHECK(!cell_size.IsEmpty());
}

PixelRefMap::~PixelRefMap()
{
}

void PixelRefMap::GatherPixelRefsFromPicture(SkPicture* picture)
{
    DCHECK(picture);

    int min_x = std::numeric_limits<int>::max();
    int min_y = std::numeric_limits<int>::max();
    int max_x = 0;
    int max_y = 0;

    skia::DiscardablePixelRefList pixel_refs;
    skia::PixelRefUtils::GatherDiscardablePixelRefs(picture, &pixel_refs);
    for (skia::DiscardablePixelRefList::const_iterator it = pixel_refs.begin();
         it != pixel_refs.end(); ++it) {
        gfx::Point min(MathUtil::RoundDown(static_cast<int>(it->pixel_ref_rect.x()),
                           cell_size_.width()),
            MathUtil::RoundDown(static_cast<int>(it->pixel_ref_rect.y()),
                cell_size_.height()));
        gfx::Point max(MathUtil::RoundDown(
                           static_cast<int>(std::ceil(it->pixel_ref_rect.right())),
                           cell_size_.width()),
            MathUtil::RoundDown(
                static_cast<int>(std::ceil(it->pixel_ref_rect.bottom())),
                cell_size_.height()));

        for (int y = min.y(); y <= max.y(); y += cell_size_.height()) {
            for (int x = min.x(); x <= max.x(); x += cell_size_.width()) {
                PixelRefMapKey key(x, y);
                data_hash_map_[key].push_back(it->pixel_ref);
            }
        }

        min_x = std::min(min_x, min.x());
        min_y = std::min(min_y, min.y());
        max_x = std::max(max_x, max.x());
        max_y = std::max(max_y, max.y());
    }

    min_pixel_cell_ = gfx::Point(min_x, min_y);
    max_pixel_cell_ = gfx::Point(max_x, max_y);
}

base::LazyInstance<PixelRefs> PixelRefMap::Iterator::empty_pixel_refs_;

PixelRefMap::Iterator::Iterator()
    : target_pixel_ref_map_(NULL)
    , current_pixel_refs_(empty_pixel_refs_.Pointer())
    , current_index_(0)
    , min_point_(-1, -1)
    , max_point_(-1, -1)
    , current_x_(0)
    , current_y_(0)
{
}

PixelRefMap::Iterator::Iterator(const gfx::Rect& rect, const Picture* picture)
    : target_pixel_ref_map_(&(picture->pixel_refs_))
    , current_pixel_refs_(empty_pixel_refs_.Pointer())
    , current_index_(0)
{
    map_layer_rect_ = picture->layer_rect_;
    PointToFirstPixelRef(rect);
}

PixelRefMap::Iterator::Iterator(const gfx::Rect& rect,
    const DisplayItemList* display_list)
    : target_pixel_ref_map_(display_list->pixel_refs_.get())
    , current_pixel_refs_(empty_pixel_refs_.Pointer())
    , current_index_(0)
{
    map_layer_rect_ = display_list->layer_rect_;
    PointToFirstPixelRef(rect);
}

PixelRefMap::Iterator::~Iterator()
{
}

PixelRefMap::Iterator& PixelRefMap::Iterator::operator++()
{
    ++current_index_;
    // If we're not at the end of the list, then we have the next item.
    if (current_index_ < current_pixel_refs_->size())
        return *this;

    DCHECK(current_y_ <= max_point_.y());
    while (true) {
        gfx::Size cell_size = target_pixel_ref_map_->cell_size_;

        // Advance the current grid cell.
        current_x_ += cell_size.width();
        if (current_x_ > max_point_.x()) {
            current_y_ += cell_size.height();
            current_x_ = min_point_.x();
            if (current_y_ > max_point_.y()) {
                current_pixel_refs_ = empty_pixel_refs_.Pointer();
                current_index_ = 0;
                break;
            }
        }

        // If there are no pixel refs at this grid cell, keep incrementing.
        PixelRefMapKey key(current_x_, current_y_);
        PixelRefHashmap::const_iterator iter = target_pixel_ref_map_->data_hash_map_.find(key);
        if (iter == target_pixel_ref_map_->data_hash_map_.end())
            continue;

        // We found a non-empty list: store it and get the first pixel ref.
        current_pixel_refs_ = &iter->second;
        current_index_ = 0;
        break;
    }
    return *this;
}

void PixelRefMap::Iterator::PointToFirstPixelRef(const gfx::Rect& rect)
{
    gfx::Rect query_rect(rect);
    // Early out if the query rect doesn't intersect this picture.
    if (!query_rect.Intersects(map_layer_rect_) || !target_pixel_ref_map_) {
        min_point_ = gfx::Point(0, 0);
        max_point_ = gfx::Point(0, 0);
        current_x_ = 1;
        current_y_ = 1;
        return;
    }

    // First, subtract the layer origin as cells are stored in layer space.
    query_rect.Offset(-map_layer_rect_.OffsetFromOrigin());

    DCHECK(!target_pixel_ref_map_->cell_size_.IsEmpty());
    gfx::Size cell_size(target_pixel_ref_map_->cell_size_);
    // We have to find a cell_size aligned point that corresponds to
    // query_rect. Point is a multiple of cell_size.
    min_point_ = gfx::Point(MathUtil::RoundDown(query_rect.x(), cell_size.width()),
        MathUtil::RoundDown(query_rect.y(), cell_size.height()));
    max_point_ = gfx::Point(
        MathUtil::RoundDown(query_rect.right() - 1, cell_size.width()),
        MathUtil::RoundDown(query_rect.bottom() - 1, cell_size.height()));

    // Limit the points to known pixel ref boundaries.
    min_point_ = gfx::Point(
        std::max(min_point_.x(), target_pixel_ref_map_->min_pixel_cell_.x()),
        std::max(min_point_.y(), target_pixel_ref_map_->min_pixel_cell_.y()));
    max_point_ = gfx::Point(
        std::min(max_point_.x(), target_pixel_ref_map_->max_pixel_cell_.x()),
        std::min(max_point_.y(), target_pixel_ref_map_->max_pixel_cell_.y()));

    // Make the current x be cell_size.width() less than min point, so that
    // the first increment will point at min_point_.
    current_x_ = min_point_.x() - cell_size.width();
    current_y_ = min_point_.y();
    if (current_y_ <= max_point_.y())
        ++(*this);
}

} // namespace cc
