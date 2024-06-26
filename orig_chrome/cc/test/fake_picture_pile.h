// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_PICTURE_PILE_H_
#define CC_TEST_FAKE_PICTURE_PILE_H_

#include "cc/playback/picture_pile.h"
#include "cc/test/fake_content_layer_client.h"

namespace base {
class WaitableEvent;
}

namespace cc {

class FakePicturePile : public PicturePile {
public:
    using PictureMapKey = PicturePile::PictureMapKey;
    using PictureMap = PicturePile::PictureMap;

    FakePicturePile(float min_contents_scale, const gfx::Size& tile_grid_size)
        : PicturePile(min_contents_scale, tile_grid_size)
        , playback_allowed_event_(nullptr)
    {
    }
    ~FakePicturePile() override { }

    static scoped_ptr<FakePicturePile> CreateFilledPile(
        const gfx::Size& tile_size,
        const gfx::Size& layer_bounds);
    static scoped_ptr<FakePicturePile> CreateEmptyPile(
        const gfx::Size& tile_size,
        const gfx::Size& layer_bounds);

    // PicturePile overrides.
    scoped_refptr<RasterSource> CreateRasterSource(
        bool can_use_lcd_text) const override;

    using PicturePile::buffer_pixels;
    using PicturePile::CanRasterSlowTileCheck;
    using PicturePile::Clear;
    using PicturePile::SetMinContentsScale;
    using PicturePile::SetTileGridSize;

    PictureMap& picture_map() { return picture_map_; }
    const gfx::Rect& recorded_viewport() const { return recorded_viewport_; }

    bool CanRasterLayerRect(gfx::Rect layer_rect)
    {
        layer_rect.Intersect(gfx::Rect(tiling_.tiling_size()));
        if (recorded_viewport_.Contains(layer_rect))
            return true;
        return CanRasterSlowTileCheck(layer_rect);
    }

    bool HasRecordings() const { return has_any_recordings_; }

    void SetRecordedViewport(const gfx::Rect& viewport)
    {
        recorded_viewport_ = viewport;
    }

    void SetHasAnyRecordings(bool has_recordings)
    {
        has_any_recordings_ = has_recordings;
    }

    void SetClearCanvasWithDebugColor(bool clear)
    {
        clear_canvas_with_debug_color_ = clear;
    }

    void SetPlaybackAllowedEvent(base::WaitableEvent* event)
    {
        playback_allowed_event_ = event;
    }

    TilingData& tiling() { return tiling_; }

    bool is_solid_color() const { return is_solid_color_; }
    SkColor solid_color() const { return solid_color_; }
    void SetIsSolidColor(bool is_solid) { is_solid_color_ = is_solid; }

    void SetPixelRecordDistance(int d) { pixel_record_distance_ = d; }

    void add_draw_rect(const gfx::RectF& rect)
    {
        client_.add_draw_rect(rect, default_paint_);
    }

    void add_draw_bitmap(const SkBitmap& bitmap, const gfx::Point& point)
    {
        client_.add_draw_bitmap(bitmap, point, default_paint_);
    }

    void add_draw_rect_with_paint(const gfx::RectF& rect, const SkPaint& paint)
    {
        client_.add_draw_rect(rect, paint);
    }

    void add_draw_bitmap_with_paint(const SkBitmap& bitmap,
        const gfx::Point& point,
        const SkPaint& paint)
    {
        client_.add_draw_bitmap(bitmap, point, paint);
    }

    void set_default_paint(const SkPaint& paint) { default_paint_ = paint; }

    void AddRecordingAt(int x, int y);
    void RemoveRecordingAt(int x, int y);
    bool HasRecordingAt(int x, int y) const;
    int num_tiles_x() const { return tiling_.num_tiles_x(); }
    int num_tiles_y() const { return tiling_.num_tiles_y(); }
    void Rerecord();

private:
    base::WaitableEvent* playback_allowed_event_;

    FakeContentLayerClient client_;
    SkPaint default_paint_;
};

} // namespace cc

#endif // CC_TEST_FAKE_PICTURE_PILE_H_
