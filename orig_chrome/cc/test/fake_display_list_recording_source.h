// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_DISPLAY_LIST_RECORDING_SOURCE_H_
#define CC_TEST_FAKE_DISPLAY_LIST_RECORDING_SOURCE_H_

#include "cc/base/region.h"
#include "cc/playback/display_list_recording_source.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/trees/layer_tree_settings.h"

namespace base {
class WaitableEvent;
} // namespace base

namespace cc {

// This class provides method for test to add bitmap and draw rect to content
// layer client. This class also provides function to rerecord to generate a new
// display list.
class FakeDisplayListRecordingSource : public DisplayListRecordingSource {
public:
    FakeDisplayListRecordingSource()
        : force_unsuitable_for_gpu_rasterization_(false)
        , playback_allowed_event_(nullptr)
    {
    }
    ~FakeDisplayListRecordingSource() override { }

    static scoped_ptr<FakeDisplayListRecordingSource> CreateRecordingSource(
        const gfx::Rect& recorded_viewport,
        const gfx::Size& layer_bounds)
    {
        scoped_ptr<FakeDisplayListRecordingSource> recording_source(
            new FakeDisplayListRecordingSource);
        recording_source->SetRecordedViewport(recorded_viewport);
        recording_source->SetLayerBounds(layer_bounds);
        return recording_source;
    }

    static scoped_ptr<FakeDisplayListRecordingSource> CreateFilledRecordingSource(
        const gfx::Size& layer_bounds)
    {
        scoped_ptr<FakeDisplayListRecordingSource> recording_source(
            new FakeDisplayListRecordingSource);
        recording_source->SetRecordedViewport(gfx::Rect(layer_bounds));
        recording_source->SetLayerBounds(layer_bounds);
        return recording_source;
    }

    // DisplayListRecordingSource overrides.
    scoped_refptr<RasterSource> CreateRasterSource(
        bool can_use_lcd) const override;
    bool IsSuitableForGpuRasterization() const override;

    void SetRecordedViewport(const gfx::Rect& recorded_viewport)
    {
        recorded_viewport_ = recorded_viewport;
    }

    void SetLayerBounds(const gfx::Size& layer_bounds) { size_ = layer_bounds; }

    void SetClearCanvasWithDebugColor(bool clear)
    {
        clear_canvas_with_debug_color_ = clear;
    }

    void Rerecord()
    {
        Region invalidation = recorded_viewport_;
        UpdateAndExpandInvalidation(&client_, &invalidation, size_,
            recorded_viewport_, 0, RECORD_NORMALLY);
    }

    void add_draw_rect(const gfx::Rect& rect)
    {
        client_.add_draw_rect(rect, default_paint_);
    }

    void add_draw_rect_with_paint(const gfx::Rect& rect, const SkPaint& paint)
    {
        client_.add_draw_rect(rect, paint);
    }

    void add_draw_rectf(const gfx::RectF& rect)
    {
        client_.add_draw_rectf(rect, default_paint_);
    }

    void add_draw_rectf_with_paint(const gfx::RectF& rect, const SkPaint& paint)
    {
        client_.add_draw_rectf(rect, paint);
    }

    void add_draw_image(const SkImage* image, const gfx::Point& point)
    {
        client_.add_draw_image(image, point, default_paint_);
    }

    void add_draw_image_with_transform(const SkImage* image,
        const gfx::Transform& transform)
    {
        client_.add_draw_image_with_transform(image, transform, default_paint_);
    }

    void add_draw_image_with_paint(const SkImage* image,
        const gfx::Point& point,
        const SkPaint& paint)
    {
        client_.add_draw_image(image, point, paint);
    }

    void set_default_paint(const SkPaint& paint) { default_paint_ = paint; }

    void set_reported_memory_usage(size_t reported_memory_usage)
    {
        client_.set_reported_memory_usage(reported_memory_usage);
    }

    void reset_draws() { client_ = FakeContentLayerClient(); }

    void SetUnsuitableForGpuRasterization()
    {
        force_unsuitable_for_gpu_rasterization_ = true;
    }

    void SetPlaybackAllowedEvent(base::WaitableEvent* event)
    {
        playback_allowed_event_ = event;
    }

    DisplayItemList* display_list() const { return display_list_.get(); }
    void set_pixel_record_distance(int distance)
    {
        pixel_record_distance_ = distance;
    }

private:
    FakeContentLayerClient client_;
    SkPaint default_paint_;
    bool force_unsuitable_for_gpu_rasterization_;
    base::WaitableEvent* playback_allowed_event_;
};

} // namespace cc

#endif // CC_TEST_FAKE_DISPLAY_LIST_RECORDING_SOURCE_H_
