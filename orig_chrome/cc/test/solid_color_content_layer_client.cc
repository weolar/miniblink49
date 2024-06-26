// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/solid_color_content_layer_client.h"

#include "cc/playback/display_item_list_settings.h"
#include "cc/playback/drawing_display_item.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/skia_util.h"

namespace cc {

// TODO(pdr): Remove PaintContents as all calls should go through
// PaintContentsToDisplayList.
void SolidColorContentLayerClient::PaintContents(
    SkCanvas* canvas,
    const gfx::Rect& rect,
    PaintingControlSetting painting_control)
{
    scoped_refptr<DisplayItemList> contents = PaintContentsToDisplayList(rect, painting_control);
    contents->Raster(canvas, nullptr, rect, 1.0f);
}

scoped_refptr<DisplayItemList>
SolidColorContentLayerClient::PaintContentsToDisplayList(
    const gfx::Rect& clip,
    PaintingControlSetting painting_control)
{
    SkPictureRecorder recorder;
    skia::RefPtr<SkCanvas> canvas = skia::SharePtr(recorder.beginRecording(gfx::RectToSkRect(clip)));

    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(color_);

    canvas->clear(SK_ColorTRANSPARENT);
    canvas->drawRect(
        SkRect::MakeXYWH(clip.x(), clip.y(), clip.width(), clip.height()), paint);

    DisplayItemListSettings settings;
    settings.use_cached_picture = false;
    scoped_refptr<DisplayItemList> display_list = DisplayItemList::Create(clip, settings);
    auto* item = display_list->CreateAndAppendItem<DrawingDisplayItem>();

    skia::RefPtr<SkPicture> picture = skia::AdoptRef(recorder.endRecordingAsPicture());
    item->SetNew(picture.Pass());

    display_list->Finalize();
    return display_list;
}

bool SolidColorContentLayerClient::FillsBoundsCompletely() const
{
    return false;
}

size_t SolidColorContentLayerClient::GetApproximateUnsharedMemoryUsage() const
{
    return 0;
}

} // namespace cc
