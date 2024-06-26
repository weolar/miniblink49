// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_DISPLAY_LIST_RASTER_SOURCE_H_
#define CC_TEST_FAKE_DISPLAY_LIST_RASTER_SOURCE_H_

#include "base/memory/ref_counted.h"
#include "cc/playback/display_list_raster_source.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class DisplayListRecordingSource;

class FakeDisplayListRasterSource : public DisplayListRasterSource {
public:
    static scoped_refptr<FakeDisplayListRasterSource> CreateInfiniteFilled();
    static scoped_refptr<FakeDisplayListRasterSource> CreateFilled(
        const gfx::Size& size);
    static scoped_refptr<FakeDisplayListRasterSource> CreateFilledLCD(
        const gfx::Size& size);
    static scoped_refptr<FakeDisplayListRasterSource> CreateFilledSolidColor(
        const gfx::Size& size);
    static scoped_refptr<FakeDisplayListRasterSource> CreatePartiallyFilled(
        const gfx::Size& size,
        const gfx::Rect& recorded_viewport);
    static scoped_refptr<FakeDisplayListRasterSource> CreateEmpty(
        const gfx::Size& size);

    static scoped_refptr<FakeDisplayListRasterSource> CreateFromRecordingSource(
        const DisplayListRecordingSource* recording_source,
        bool can_use_lcd);
    static scoped_refptr<FakeDisplayListRasterSource>
    CreateFromRecordingSourceWithWaitable(
        const DisplayListRecordingSource* recording_source,
        bool can_use_lcd,
        base::WaitableEvent* playback_allowed_event);

    void PlaybackToCanvas(SkCanvas* canvas,
        const gfx::Rect& canvas_bitmap_rect,
        const gfx::Rect& canvas_playback_rect,
        float contents_scale) const override;

protected:
    FakeDisplayListRasterSource(
        const DisplayListRecordingSource* recording_source,
        bool can_use_lcd);
    FakeDisplayListRasterSource(
        const DisplayListRecordingSource* recording_source,
        bool can_use_lcd,
        base::WaitableEvent* playback_allowed_event);
    ~FakeDisplayListRasterSource() override;

private:
    base::WaitableEvent* playback_allowed_event_;
};

} // namespace cc

#endif // CC_TEST_FAKE_DISPLAY_LIST_RASTER_SOURCE_H_
