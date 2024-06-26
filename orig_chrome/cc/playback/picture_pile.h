// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_PICTURE_PILE_H_
#define CC_PLAYBACK_PICTURE_PILE_H_

#include <bitset>
#include <utility>
#include <vector>

#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "cc/base/tiling_data.h"
#include "cc/playback/picture.h"

namespace cc {
class PicturePileImpl;

class CC_EXPORT PicturePile : public RecordingSource {
public:
    PicturePile(float min_contents_scale, const gfx::Size& tile_grid_size);
    ~PicturePile() override;

    // RecordingSource overrides.
    bool UpdateAndExpandInvalidation(ContentLayerClient* painter,
        Region* invalidation,
        const gfx::Size& layer_size,
        const gfx::Rect& visible_layer_rect,
        int frame_number,
        RecordingMode recording_mode) override;
    scoped_refptr<RasterSource> CreateRasterSource(
        bool can_use_lcd_text) const override;
    gfx::Size GetSize() const final;
    void SetEmptyBounds() override;
    void SetSlowdownRasterScaleFactor(int factor) override;
    void SetGatherPixelRefs(bool gather_pixel_refs) override;
    void SetBackgroundColor(SkColor background_color) override;
    void SetRequiresClear(bool requires_clear) override;
    bool IsSuitableForGpuRasterization() const override;
    void SetUnsuitableForGpuRasterizationForTesting() override;
    gfx::Size GetTileGridSizeForTesting() const override;

    typedef std::pair<int, int> PictureMapKey;
    typedef base::hash_map<PictureMapKey, scoped_refptr<const Picture>>
        PictureMap;

    // An internal CanRaster check that goes to the picture_map rather than
    // using the recorded_viewport hint.
    bool CanRasterSlowTileCheck(const gfx::Rect& layer_rect) const;

    void Clear();

    void SetMinContentsScale(float min_contents_scale);
    void SetTileGridSize(const gfx::Size& tile_grid_size);

    gfx::Rect PaddedRect(const PictureMapKey& key) const;
    gfx::Rect PadRect(const gfx::Rect& rect) const;

    int buffer_pixels() const { return tiling_.border_texels(); }

    // A picture pile is a tiled set of pictures. The picture map is a map of tile
    // indices to picture infos.
    PictureMap picture_map_;
    TilingData tiling_;

    // If non-empty, all pictures tiles inside this rect are recorded. There may
    // be recordings outside this rect, but everything inside the rect is
    // recorded.
    gfx::Rect recorded_viewport_;
    float min_contents_scale_;
    gfx::Size tile_grid_size_;
    int slow_down_raster_scale_factor_for_debug_;
    bool gather_pixel_refs_;
    // A hint about whether there are any recordings. This may be a false
    // positive.
    bool has_any_recordings_;
    bool clear_canvas_with_debug_color_;
    bool requires_clear_;
    bool is_solid_color_;
    SkColor solid_color_;
    SkColor background_color_;
    int pixel_record_distance_;

private:
    friend class PicturePileImpl;

    void CreatePictures(ContentLayerClient* painter,
        RecordingMode recording_mode,
        const std::vector<gfx::Rect>& record_rects);
    void GetInvalidTileRects(const gfx::Rect& interest_rect,
        std::vector<gfx::Rect>* invalid_tiles);
    bool ApplyInvalidationAndResize(const gfx::Rect& interest_rect,
        Region* invalidation,
        const gfx::Size& layer_size,
        int frame_number);
    void DetermineIfSolidColor();
    void SetBufferPixels(int buffer_pixels);

    bool is_suitable_for_gpu_rasterization_;

    DISALLOW_COPY_AND_ASSIGN(PicturePile);
};

} // namespace cc

#endif // CC_PLAYBACK_PICTURE_PILE_H_
