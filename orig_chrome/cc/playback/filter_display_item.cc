// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/filter_display_item.h"

#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/output/render_surface_filters.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkXfermode.h"
#include "ui/gfx/skia_util.h"

namespace cc {

FilterDisplayItem::FilterDisplayItem()
{
}

FilterDisplayItem::~FilterDisplayItem()
{
}

void FilterDisplayItem::SetNew(const FilterOperations& filters,
    const gfx::RectF& bounds)
{
    filters_ = filters;
    bounds_ = bounds;

    // FilterOperations doesn't expose its capacity, but size is probably good
    // enough.
    size_t external_memory_usage = filters_.size() * sizeof(filters_.at(0));
    DisplayItem::SetNew(true /* suitable_for_gpu_raster */, 1 /* op_count */,
        external_memory_usage);
}

void FilterDisplayItem::Raster(SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    canvas->save();
    canvas->translate(bounds_.x(), bounds_.y());

    skia::RefPtr<SkImageFilter> image_filter = RenderSurfaceFilters::BuildImageFilter(
        filters_, gfx::SizeF(bounds_.width(), bounds_.height()));
#ifdef SK_SAVE_LAYER_BOUNDS_ARE_FILTERED
    // TODO(senorblanco): remove this once callsites updated (crbug.com/525748)
    SkRect boundaries;
    image_filter->computeFastBounds(
        SkRect::MakeWH(bounds_.width(), bounds_.height()), &boundaries);
#else
    SkRect boundaries = SkRect::MakeWH(bounds_.width(), bounds_.height());
#endif

    SkPaint paint;
    paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
    paint.setImageFilter(image_filter.get());
    canvas->saveLayer(&boundaries, &paint);

    canvas->translate(-bounds_.x(), -bounds_.y());
}

void FilterDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->AddStartingDisplayItem();
    calculator->Save();
}

void FilterDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    array->AppendString(base::StringPrintf("FilterDisplayItem bounds: [%s]",
        bounds_.ToString().c_str()));
}

EndFilterDisplayItem::EndFilterDisplayItem()
{
    DisplayItem::SetNew(true /* suitable_for_gpu_raster */, 0 /* op_count */,
        0 /* external_memory_usage */);
}

EndFilterDisplayItem::~EndFilterDisplayItem()
{
}

void EndFilterDisplayItem::Raster(SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    canvas->restore();
    canvas->restore();
}

void EndFilterDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->Restore();
    calculator->AddEndingDisplayItem();
}

void EndFilterDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    array->AppendString("EndFilterDisplayItem");
}

} // namespace cc
