// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/drawing_display_item.h"

#include <string>

#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/debug/picture_debug_util.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/utils/SkPictureUtils.h"
#include "ui/gfx/skia_util.h"

namespace cc {

DrawingDisplayItem::DrawingDisplayItem()
{
}

DrawingDisplayItem::~DrawingDisplayItem()
{
}

void DrawingDisplayItem::SetNew(skia::RefPtr<SkPicture> picture)
{
    picture_ = picture /*.Pass()*/;
    DisplayItem::SetNew(picture_->suitableForGpuRasterization(NULL),
        picture_->approximateOpCount(),
        SkPictureUtils::ApproximateBytesUsed(picture_.get()));
}

void DrawingDisplayItem::Raster(SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    // The canvas_playback_rect can be empty to signify no culling is desired.
    if (!canvas_target_playback_rect.IsEmpty()) {
        const SkMatrix& matrix = canvas->getTotalMatrix();
        const SkRect& cull_rect = picture_->cullRect();
        SkRect target_rect;
        matrix.mapRect(&target_rect, cull_rect);
        if (!target_rect.intersect(gfx::RectToSkRect(canvas_target_playback_rect)))
            return;
    }

    // SkPicture always does a wrapping save/restore on the canvas, so it is not
    // necessary here.
    if (callback)
        picture_->playback(canvas, callback);
    else
        canvas->drawPicture(picture_.get());
}

void DrawingDisplayItem::ProcessForBounds(
    DisplayItemListBoundsCalculator* calculator) const
{
    calculator->AddDisplayItemWithBounds(picture_->cullRect());
}

void DrawingDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    array->BeginDictionary();
    array->SetString("name", "DrawingDisplayItem");
    array->SetString(
        "cullRect",
        base::StringPrintf("[%f,%f,%f,%f]", picture_->cullRect().x(),
            picture_->cullRect().y(), picture_->cullRect().width(),
            picture_->cullRect().height()));
    std::string b64_picture;
    PictureDebugUtil::SerializeAsBase64(picture_.get(), &b64_picture);
    array->SetString("skp64", b64_picture);
    array->EndDictionary();
}

void DrawingDisplayItem::CloneTo(DrawingDisplayItem* item) const
{
    item->SetNew(picture_);
}

} // namespace cc
