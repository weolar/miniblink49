// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_PICTURE_PILE_IMPL_H_
#define CC_PLAYBACK_PICTURE_PILE_IMPL_H_

#include <map>
#include <set>
#include <vector>

#include "base/time/time.h"
#include "cc/base/cc_export.h"
#include "cc/debug/rendering_stats_instrumentation.h"
#include "cc/playback/picture_pile.h"
#include "cc/playback/pixel_ref_map.h"
#include "cc/playback/raster_source.h"
#include "skia/ext/analysis_canvas.h"
#include "skia/ext/refptr.h"

class SkCanvas;
class SkPicture;
class SkPixelRef;

namespace gfx {
class Rect;
}

namespace cc {

class CC_EXPORT PicturePileImpl : public RasterSource {
public:
    static scoped_refptr<PicturePileImpl> CreateFromPicturePile(
        const PicturePile* other,
        bool can_use_lcd_text);

    // RasterSource overrides. See RasterSource header for full description.
    // When slow-down-raster-scale-factor is set to a value greater than 1, the
    // reported rasterize time (in stats_instrumentation) is the minimum measured
    // value over all runs.
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
    void GatherPixelRefs(const gfx::Rect& content_rect,
        float contents_scale,
        std::vector<SkPixelRef*>* pixel_refs) const override;
    bool CoversRect(const gfx::Rect& content_rect,
        float contents_scale) const override;
    void SetShouldAttemptToUseDistanceFieldText() override;
    bool ShouldAttemptToUseDistanceFieldText() const override;
    gfx::Size GetSize() const override;
    bool IsSolidColor() const override;
    SkColor GetSolidColor() const override;
    bool HasRecordings() const override;
    bool CanUseLCDText() const override;
    scoped_refptr<RasterSource> CreateCloneWithoutLCDText() const override;

    // Tracing functionality.
    void DidBeginTracing() override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    skia::RefPtr<SkPicture> GetFlattenedPicture() override;
    size_t GetPictureMemoryUsage() const override;

    // Iterator used to return SkPixelRefs from this picture pile.
    // Public for testing.
    class CC_EXPORT PixelRefIterator {
    public:
        PixelRefIterator(const gfx::Rect& content_rect,
            float contents_scale,
            const PicturePileImpl* picture_pile);
        ~PixelRefIterator();

        SkPixelRef* operator->() const { return *pixel_ref_iterator_; }
        SkPixelRef* operator*() const { return *pixel_ref_iterator_; }
        PixelRefIterator& operator++();
        operator bool() const { return pixel_ref_iterator_; }

    private:
        void AdvanceToTilePictureWithPixelRefs();

        const PicturePileImpl* picture_pile_;
        gfx::Rect layer_rect_;
        TilingData::Iterator tile_iterator_;
        PixelRefMap::Iterator pixel_ref_iterator_;
        std::set<const void*> processed_pictures_;
    };

protected:
    friend class PicturePile;
    friend class PixelRefIterator;

    using PictureMapKey = PicturePile::PictureMapKey;
    using PictureMap = PicturePile::PictureMap;

    PicturePileImpl();
    explicit PicturePileImpl(const PicturePile* other, bool can_use_lcd_text);
    explicit PicturePileImpl(const PicturePileImpl* other, bool can_use_lcd_text);
    ~PicturePileImpl() override;

    int buffer_pixels() const { return tiling_.border_texels(); }

    // These members are const as this raster source may be in use on another
    // thread and so should not be touched after construction.
    const PictureMap picture_map_;
    const TilingData tiling_;
    const SkColor background_color_;
    const bool requires_clear_;
    const bool can_use_lcd_text_;
    const bool is_solid_color_;
    const SkColor solid_color_;
    const gfx::Rect recorded_viewport_;
    const bool has_any_recordings_;
    const bool clear_canvas_with_debug_color_;
    const float min_contents_scale_;
    const int slow_down_raster_scale_factor_for_debug_;
    // TODO(enne/vmiura): this has a read/write race between raster and compositor
    // threads with multi-threaded Ganesh.  Make this const or remove it.
    bool should_attempt_to_use_distance_field_text_;

    size_t picture_memory_usage_;

private:
    typedef std::map<const Picture*, Region> PictureRegionMap;

    // Called when analyzing a tile. We can use AnalysisCanvas as
    // SkPicture::AbortCallback, which allows us to early out from analysis.
    void RasterForAnalysis(skia::AnalysisCanvas* canvas,
        const gfx::Rect& canvas_rect,
        float contents_scale) const;

    void CoalesceRasters(const gfx::Rect& canvas_rect,
        const gfx::Rect& content_rect,
        float contents_scale,
        PictureRegionMap* result) const;

    void RasterCommon(SkCanvas* canvas,
        SkPicture::AbortCallback* callback,
        const gfx::Rect& canvas_rect,
        float contents_scale) const;

    // An internal CanRaster check that goes to the picture_map rather than
    // using the recorded_viewport hint.
    bool CanRasterSlowTileCheck(const gfx::Rect& layer_rect) const;

    gfx::Rect PaddedRect(const PictureMapKey& key) const;

    DISALLOW_COPY_AND_ASSIGN(PicturePileImpl);
};

} // namespace cc

#endif // CC_PLAYBACK_PICTURE_PILE_IMPL_H_
