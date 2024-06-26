// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_content_layer_client.h"

#include "cc/playback/clip_display_item.h"
#include "cc/playback/display_item_list_settings.h"
#include "cc/playback/drawing_display_item.h"
#include "cc/playback/transform_display_item.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "ui/gfx/skia_util.h"

namespace cc {

FakeContentLayerClient::ImageData::ImageData(const SkImage* img,
    const gfx::Point& point,
    const SkPaint& paint)
    : image(skia::SharePtr(img))
    , point(point)
    , paint(paint)
{
}

FakeContentLayerClient::ImageData::ImageData(const SkImage* img,
    const gfx::Transform& transform,
    const SkPaint& paint)
    : image(skia::SharePtr(img))
    , transform(transform)
    , paint(paint)
{
}

FakeContentLayerClient::ImageData::~ImageData() { }

FakeContentLayerClient::FakeContentLayerClient()
    : fill_with_nonsolid_color_(false)
    , last_canvas_(nullptr)
    , last_painting_control_(PAINTING_BEHAVIOR_NORMAL)
    , reported_memory_usage_(0)
{
}

FakeContentLayerClient::~FakeContentLayerClient()
{
}

void FakeContentLayerClient::PaintContents(
    SkCanvas* canvas,
    const gfx::Rect& paint_rect,
    PaintingControlSetting painting_control)
{
    last_canvas_ = canvas;
    last_painting_control_ = painting_control;

    canvas->clipRect(gfx::RectToSkRect(paint_rect));
    for (RectPaintVector::const_iterator it = draw_rects_.begin();
         it != draw_rects_.end(); ++it) {
        const gfx::RectF& draw_rect = it->first;
        const SkPaint& paint = it->second;
        canvas->drawRect(gfx::RectFToSkRect(draw_rect), paint);
    }

    for (ImageVector::const_iterator it = draw_images_.begin();
         it != draw_images_.end(); ++it) {
        canvas->drawImage(it->image.get(), it->point.x(), it->point.y(),
            &it->paint);
    }

    if (fill_with_nonsolid_color_) {
        gfx::Rect draw_rect = paint_rect;
        bool red = true;
        while (!draw_rect.IsEmpty()) {
            SkPaint paint;
            paint.setColor(red ? SK_ColorRED : SK_ColorBLUE);
            canvas->drawIRect(gfx::RectToSkIRect(draw_rect), paint);
            draw_rect.Inset(1, 1);
        }
    }
}

scoped_refptr<DisplayItemList>
FakeContentLayerClient::PaintContentsToDisplayList(
    const gfx::Rect& clip,
    PaintingControlSetting painting_control)
{
    // Cached picture is used because unit tests expect to be able to
    // use GatherPixelRefs.
    DisplayItemListSettings settings;
    settings.use_cached_picture = true;
    scoped_refptr<DisplayItemList> display_list = DisplayItemList::Create(clip, settings);
    SkPictureRecorder recorder;
    skia::RefPtr<SkCanvas> canvas;
    skia::RefPtr<SkPicture> picture;
    auto* item = display_list->CreateAndAppendItem<ClipDisplayItem>();
    item->SetNew(clip, std::vector<SkRRect>());

    for (RectPaintVector::const_iterator it = draw_rects_.begin();
         it != draw_rects_.end(); ++it) {
        const gfx::RectF& draw_rect = it->first;
        const SkPaint& paint = it->second;
        canvas = skia::SharePtr(recorder.beginRecording(gfx::RectFToSkRect(draw_rect)));
        canvas->drawRect(gfx::RectFToSkRect(draw_rect), paint);
        picture = skia::AdoptRef(recorder.endRecordingAsPicture());
        auto* item = display_list->CreateAndAppendItem<DrawingDisplayItem>();
        item->SetNew(picture.Pass());
    }

    for (ImageVector::const_iterator it = draw_images_.begin();
         it != draw_images_.end(); ++it) {
        if (!it->transform.IsIdentity()) {
            auto* item = display_list->CreateAndAppendItem<TransformDisplayItem>();
            item->SetNew(it->transform);
        }
        canvas = skia::SharePtr(
            recorder.beginRecording(it->image->width(), it->image->height()));
        canvas->drawImage(it->image.get(), it->point.x(), it->point.y(),
            &it->paint);
        picture = skia::AdoptRef(recorder.endRecordingAsPicture());
        auto* item = display_list->CreateAndAppendItem<DrawingDisplayItem>();
        item->SetNew(picture.Pass());
        if (!it->transform.IsIdentity()) {
            display_list->CreateAndAppendItem<EndTransformDisplayItem>();
        }
    }

    if (fill_with_nonsolid_color_) {
        gfx::Rect draw_rect = clip;
        bool red = true;
        while (!draw_rect.IsEmpty()) {
            SkPaint paint;
            paint.setColor(red ? SK_ColorRED : SK_ColorBLUE);
            canvas = skia::SharePtr(recorder.beginRecording(gfx::RectToSkRect(draw_rect)));
            canvas->drawIRect(gfx::RectToSkIRect(draw_rect), paint);
            picture = skia::AdoptRef(recorder.endRecordingAsPicture());
            auto* item = display_list->CreateAndAppendItem<DrawingDisplayItem>();
            item->SetNew(picture.Pass());
            draw_rect.Inset(1, 1);
        }
    }

    display_list->CreateAndAppendItem<EndClipDisplayItem>();

    display_list->Finalize();
    return display_list;
}

bool FakeContentLayerClient::FillsBoundsCompletely() const { return false; }

size_t FakeContentLayerClient::GetApproximateUnsharedMemoryUsage() const
{
    return reported_memory_usage_;
}

} // namespace cc
