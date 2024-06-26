// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/transform_display_item.h"

#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event_argument.h"
#include "third_party/skia/include/core/SkCanvas.h"

namespace cc {

TransformDisplayItem::TransformDisplayItem()
    : transform_(gfx::Transform::kSkipInitialization)
{
}

TransformDisplayItem::~TransformDisplayItem()
{
}

void TransformDisplayItem::SetNew(const gfx::Transform& transform)
{
    transform_ = transform;

    DisplayItem::SetNew(true /* suitable_for_gpu_raster */, 1 /* op_count */,
        0 /* external_memory_usage */);
}

void TransformDisplayItem::Raster(SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    canvas->save();
    if (!transform_.IsIdentity())
        canvas->concat(transform_.matrix());
}

void TransformDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->AddStartingDisplayItem();
    calculator->Save();
    calculator->matrix()->postConcat(transform_.matrix());
}

void TransformDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    array->AppendString(base::StringPrintf("TransformDisplayItem transform: [%s]",
        transform_.ToString().c_str()));
}

EndTransformDisplayItem::EndTransformDisplayItem()
{
    DisplayItem::SetNew(true /* suitable_for_gpu_raster */, 0 /* op_count */,
        0 /* external_memory_usage */);
}

EndTransformDisplayItem::~EndTransformDisplayItem()
{
}

void EndTransformDisplayItem::Raster(
    SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    canvas->restore();
}

void EndTransformDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->Restore();
    calculator->AddEndingDisplayItem();
}

void EndTransformDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    array->AppendString("EndTransformDisplayItem");
}

} // namespace cc
