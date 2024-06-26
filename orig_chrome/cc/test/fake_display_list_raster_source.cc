// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_display_list_raster_source.h"

#include <limits>

#include "base/synchronization/waitable_event.h"
#include "cc/test/fake_display_list_recording_source.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

scoped_refptr<FakeDisplayListRasterSource>
FakeDisplayListRasterSource::CreateInfiniteFilled()
{
    gfx::Size size(std::numeric_limits<int>::max() / 10,
        std::numeric_limits<int>::max() / 10);
    return CreateFilled(size);
}

scoped_refptr<FakeDisplayListRasterSource>
FakeDisplayListRasterSource::CreateFilled(const gfx::Size& size)
{
    auto recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(size);

    SkPaint red_paint;
    red_paint.setColor(SK_ColorRED);
    recording_source->add_draw_rect_with_paint(gfx::Rect(size), red_paint);

    gfx::Size smaller_size(size.width() - 10, size.height() - 10);
    SkPaint green_paint;
    green_paint.setColor(SK_ColorGREEN);
    recording_source->add_draw_rect_with_paint(gfx::Rect(smaller_size),
        green_paint);

    recording_source->Rerecord();

    return make_scoped_refptr(
        new FakeDisplayListRasterSource(recording_source.get(), false));
}

scoped_refptr<FakeDisplayListRasterSource>
FakeDisplayListRasterSource::CreateFilledLCD(const gfx::Size& size)
{
    auto recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(size);

    SkPaint red_paint;
    red_paint.setColor(SK_ColorRED);
    recording_source->add_draw_rect_with_paint(gfx::Rect(size), red_paint);

    gfx::Size smaller_size(size.width() - 10, size.height() - 10);
    SkPaint green_paint;
    green_paint.setColor(SK_ColorGREEN);
    recording_source->add_draw_rect_with_paint(gfx::Rect(smaller_size),
        green_paint);

    recording_source->Rerecord();

    return make_scoped_refptr(
        new FakeDisplayListRasterSource(recording_source.get(), true));
}

scoped_refptr<FakeDisplayListRasterSource>
FakeDisplayListRasterSource::CreateFilledSolidColor(const gfx::Size& size)
{
    auto recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(size);

    SkPaint red_paint;
    red_paint.setColor(SK_ColorRED);
    recording_source->add_draw_rect_with_paint(gfx::Rect(size), red_paint);
    recording_source->Rerecord();
    auto raster_source = make_scoped_refptr(
        new FakeDisplayListRasterSource(recording_source.get(), false));
    if (!raster_source->IsSolidColor())
        ADD_FAILURE() << "Not solid color!";
    return raster_source;
}

scoped_refptr<FakeDisplayListRasterSource>
FakeDisplayListRasterSource::CreatePartiallyFilled(
    const gfx::Size& size,
    const gfx::Rect& recorded_viewport)
{
    DCHECK_IMPLIES(!recorded_viewport.IsEmpty(),
        gfx::Rect(size).Contains(recorded_viewport));
    auto recording_source = FakeDisplayListRecordingSource::CreateRecordingSource(
        recorded_viewport, size);

    SkPaint red_paint;
    red_paint.setColor(SK_ColorRED);
    recording_source->add_draw_rect_with_paint(gfx::Rect(size), red_paint);

    gfx::Size smaller_size(size.width() - 10, size.height() - 10);
    SkPaint green_paint;
    green_paint.setColor(SK_ColorGREEN);
    recording_source->add_draw_rect_with_paint(gfx::Rect(smaller_size),
        green_paint);

    recording_source->Rerecord();
    recording_source->SetRecordedViewport(recorded_viewport);

    return make_scoped_refptr(
        new FakeDisplayListRasterSource(recording_source.get(), false));
}

scoped_refptr<FakeDisplayListRasterSource>
FakeDisplayListRasterSource::CreateEmpty(const gfx::Size& size)
{
    auto recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(size);
    return make_scoped_refptr(
        new FakeDisplayListRasterSource(recording_source.get(), false));
}

scoped_refptr<FakeDisplayListRasterSource>
FakeDisplayListRasterSource::CreateFromRecordingSource(
    const DisplayListRecordingSource* recording_source,
    bool can_use_lcd)
{
    return make_scoped_refptr(
        new FakeDisplayListRasterSource(recording_source, can_use_lcd));
}

scoped_refptr<FakeDisplayListRasterSource>
FakeDisplayListRasterSource::CreateFromRecordingSourceWithWaitable(
    const DisplayListRecordingSource* recording_source,
    bool can_use_lcd,
    base::WaitableEvent* playback_allowed_event)
{
    return make_scoped_refptr(new FakeDisplayListRasterSource(
        recording_source, can_use_lcd, playback_allowed_event));
}

FakeDisplayListRasterSource::FakeDisplayListRasterSource(
    const DisplayListRecordingSource* recording_source,
    bool can_use_lcd)
    : DisplayListRasterSource(recording_source, can_use_lcd)
    , playback_allowed_event_(nullptr)
{
}

FakeDisplayListRasterSource::FakeDisplayListRasterSource(
    const DisplayListRecordingSource* recording_source,
    bool can_use_lcd,
    base::WaitableEvent* playback_allowed_event)
    : DisplayListRasterSource(recording_source, can_use_lcd)
    , playback_allowed_event_(playback_allowed_event)
{
}

FakeDisplayListRasterSource::~FakeDisplayListRasterSource() { }

void FakeDisplayListRasterSource::PlaybackToCanvas(
    SkCanvas* canvas,
    const gfx::Rect& canvas_bitmap_rect,
    const gfx::Rect& canvas_playback_rect,
    float contents_scale) const
{
    if (playback_allowed_event_)
        playback_allowed_event_->Wait();
    DisplayListRasterSource::PlaybackToCanvas(
        canvas, canvas_bitmap_rect, canvas_playback_rect, contents_scale);
}

} // namespace cc
