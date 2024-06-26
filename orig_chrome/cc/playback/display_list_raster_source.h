// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_DISPLAY_LIST_RASTER_SOURCE_H_
#define CC_PLAYBACK_DISPLAY_LIST_RASTER_SOURCE_H_

#include <vector>

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/debug/rendering_stats_instrumentation.h"
#include "cc/playback/display_list_recording_source.h"
#include "cc/playback/raster_source.h"
#include "skia/ext/analysis_canvas.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkPicture.h"

namespace cc {
class DisplayItemList;

class CC_EXPORT DisplayListRasterSource : public RasterSource {
public:
    static scoped_refptr<DisplayListRasterSource>
    CreateFromDisplayListRecordingSource(const DisplayListRecordingSource* other,
        bool can_use_lcd_text);

    // RasterSource overrides.
    void PlaybackToCanvas(SkCanvas* canvas,
        const gfx::Rect& canvas_bitmap_rect,
        const gfx::Rect& canvas_playback_rect,
        float contents_scale) const override;
    void PlaybackToSharedCanvas(SkCanvas* canvas,
        const gfx::Rect& canvas_rect,
        float contents_scale) const override;
    void PerformSolidColorAnalysis(
        const gfx::Rect& content_rect,
        float contents_scale,
        RasterSource::SolidColorAnalysis* analysis) const override;
    bool IsSolidColor() const override;
    SkColor GetSolidColor() const override;
    gfx::Size GetSize() const override;
    void GetDiscardableImagesInRect(
        const gfx::Rect& layer_rect,
        std::vector<PositionImage>* images) const override;
    bool CoversRect(const gfx::Rect& layer_rect) const override;
    bool HasRecordings() const override;
    gfx::Rect RecordedViewport() const override;
    void SetShouldAttemptToUseDistanceFieldText() override;
    bool ShouldAttemptToUseDistanceFieldText() const override;
    void DidBeginTracing() override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    skia::RefPtr<SkPicture> GetFlattenedPicture() override;
    size_t GetPictureMemoryUsage() const override;
    bool CanUseLCDText() const override;
    scoped_refptr<RasterSource> CreateCloneWithoutLCDText() const override;

protected:
    DisplayListRasterSource();
    DisplayListRasterSource(const DisplayListRecordingSource* other,
        bool can_use_lcd_text);
    DisplayListRasterSource(const DisplayListRasterSource* other,
        bool can_use_lcd_text);
    ~DisplayListRasterSource() override;

    // These members are const as this raster source may be in use on another
    // thread and so should not be touched after construction.
    const scoped_refptr<DisplayItemList> display_list_;
    const size_t painter_reported_memory_usage_;
    const SkColor background_color_;
    const bool requires_clear_;
    const bool can_use_lcd_text_;
    const bool is_solid_color_;
    const SkColor solid_color_;
    const gfx::Rect recorded_viewport_;
    const gfx::Size size_;
    const bool clear_canvas_with_debug_color_;
    const int slow_down_raster_scale_factor_for_debug_;
    // TODO(enne/vmiura): this has a read/write race between raster and compositor
    // threads with multi-threaded Ganesh.  Make this const or remove it.
    bool should_attempt_to_use_distance_field_text_;

private:
    // Called when analyzing a tile. We can use AnalysisCanvas as
    // SkPicture::AbortCallback, which allows us to early out from analysis.
    void RasterForAnalysis(skia::AnalysisCanvas* canvas,
        const gfx::Rect& canvas_rect,
        float contents_scale) const;

    void RasterCommon(SkCanvas* canvas,
        SkPicture::AbortCallback* callback,
        const gfx::Rect& canvas_bitmap_rect,
        const gfx::Rect& canvas_playback_rect,
        float contents_scale) const;

    DISALLOW_COPY_AND_ASSIGN(DisplayListRasterSource);
};

} // namespace cc

#endif // CC_PLAYBACK_DISPLAY_LIST_RASTER_SOURCE_H_
