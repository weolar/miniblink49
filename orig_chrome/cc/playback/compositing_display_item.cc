// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/compositing_display_item.h"

#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event_argument.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkXfermode.h"
#include "ui/gfx/skia_util.h"

namespace cc {

CompositingDisplayItem::CompositingDisplayItem()
{
}

CompositingDisplayItem::~CompositingDisplayItem()
{
}

void CompositingDisplayItem::SetNew(uint8_t alpha,
    SkXfermode::Mode xfermode,
    SkRect* bounds,
    skia::RefPtr<SkColorFilter> cf)
{
    alpha_ = alpha;
    xfermode_ = xfermode;
    has_bounds_ = !!bounds;
    if (bounds)
        bounds_ = SkRect(*bounds);
    color_filter_ = cf;

    // TODO(pdr): Include color_filter's memory here.
    size_t external_memory_usage = 0;
    DisplayItem::SetNew(true /* suitable_for_gpu_raster */, 1 /* op_count */,
        external_memory_usage);
}

void CompositingDisplayItem::Raster(
    SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    SkPaint paint;
    paint.setXfermodeMode(xfermode_);
    paint.setAlpha(alpha_);
    paint.setColorFilter(color_filter_.get());
    canvas->saveLayer(has_bounds_ ? &bounds_ : nullptr, &paint);
}

void CompositingDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->AddStartingDisplayItem();
    calculator->Save();
}

void CompositingDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    array->AppendString(base::StringPrintf(
        "CompositingDisplayItem alpha: %d, xfermode: %d", alpha_, xfermode_));
    if (has_bounds_)
        array->AppendString(base::StringPrintf(
            ", bounds: [%f, %f, %f, %f]", static_cast<float>(bounds_.x()),
            static_cast<float>(bounds_.y()), static_cast<float>(bounds_.width()),
            static_cast<float>(bounds_.height())));
}

EndCompositingDisplayItem::EndCompositingDisplayItem()
{
    DisplayItem::SetNew(true /* suitable_for_gpu_raster */, 0 /* op_count */,
        0 /* external_memory_usage */);
}

EndCompositingDisplayItem::~EndCompositingDisplayItem()
{
}

void EndCompositingDisplayItem::Raster(
    SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    canvas->restore();
}

void EndCompositingDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->Restore();
    calculator->AddEndingDisplayItem();
}

void EndCompositingDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    array->AppendString("EndCompositingDisplayItem");
}

} // namespace cc
