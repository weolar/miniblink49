// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/clip_display_item.h"

#include <string>

#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event_argument.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/skia_util.h"

namespace cc {

ClipDisplayItem::ClipDisplayItem()
{
}

ClipDisplayItem::~ClipDisplayItem()
{
}

void ClipDisplayItem::SetNew(gfx::Rect clip_rect,
    const std::vector<SkRRect>& rounded_clip_rects)
{
    clip_rect_ = clip_rect;
    rounded_clip_rects_ = rounded_clip_rects;

    size_t external_memory_usage = rounded_clip_rects_.capacity() * sizeof(rounded_clip_rects_[0]);

    DisplayItem::SetNew(true /* suitable_for_gpu_raster */, 1 /* op_count */,
        external_memory_usage);
}

void ClipDisplayItem::Raster(SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    canvas->save();
    canvas->clipRect(SkRect::MakeXYWH(clip_rect_.x(), clip_rect_.y(),
        clip_rect_.width(), clip_rect_.height()));
    for (size_t i = 0; i < rounded_clip_rects_.size(); ++i) {
        if (rounded_clip_rects_[i].isRect()) {
            canvas->clipRect(rounded_clip_rects_[i].rect());
        } else {
            bool antialiased = true;
            canvas->clipRRect(rounded_clip_rects_[i], SkRegion::kIntersect_Op,
                antialiased);
        }
    }
}

void ClipDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->AddStartingDisplayItem();
}

void ClipDisplayItem::AsValueInto(base::trace_event::TracedValue* array) const
{
    std::string value = base::StringPrintf("ClipDisplayItem rect: [%s]",
        clip_rect_.ToString().c_str());
    for (const SkRRect& rounded_rect : rounded_clip_rects_) {
        base::StringAppendF(
            &value, " rounded_rect: [rect: [%s]",
            gfx::SkRectToRectF(rounded_rect.rect()).ToString().c_str());
        base::StringAppendF(&value, " radii: [");
        SkVector upper_left_radius = rounded_rect.radii(SkRRect::kUpperLeft_Corner);
        base::StringAppendF(&value, "[%f,%f],", upper_left_radius.x(),
            upper_left_radius.y());
        SkVector upper_right_radius = rounded_rect.radii(SkRRect::kUpperRight_Corner);
        base::StringAppendF(&value, " [%f,%f],", upper_right_radius.x(),
            upper_right_radius.y());
        SkVector lower_right_radius = rounded_rect.radii(SkRRect::kLowerRight_Corner);
        base::StringAppendF(&value, " [%f,%f],", lower_right_radius.x(),
            lower_right_radius.y());
        SkVector lower_left_radius = rounded_rect.radii(SkRRect::kLowerLeft_Corner);
        base::StringAppendF(&value, " [%f,%f]]", lower_left_radius.x(),
            lower_left_radius.y());
    }
    array->AppendString(value);
}

EndClipDisplayItem::EndClipDisplayItem()
{
    DisplayItem::SetNew(true /* suitable_for_gpu_raster */, 0 /* op_count */,
        0 /* external_memory_usage */);
}

EndClipDisplayItem::~EndClipDisplayItem()
{
}

void EndClipDisplayItem::Raster(SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    canvas->restore();
}

void EndClipDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->AddEndingDisplayItem();
}

void EndClipDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    array->AppendString("EndClipDisplayItem");
}

} // namespace cc
