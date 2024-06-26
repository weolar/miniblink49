// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_RASTER_SOURCE_H_
#define CC_PLAYBACK_RASTER_SOURCE_H_

#include <vector>

#include "base/memory/ref_counted.h"
#include "cc/base/cc_export.h"
#include "cc/debug/traced_value.h"
#include "cc/playback/discardable_image_map.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

class SkCanvas;
class SkPicture;

namespace cc {

class CC_EXPORT RasterSource : public base::RefCountedThreadSafe<RasterSource> {
public:
    struct CC_EXPORT SolidColorAnalysis {
        SolidColorAnalysis()
            : is_solid_color(false)
            , solid_color(SK_ColorTRANSPARENT)
        {
        }
        ~SolidColorAnalysis() { }

        bool is_solid_color;
        SkColor solid_color;
    };

    // Raster a subrect of this RasterSource into the given canvas. It is
    // assumed that contents_scale has already been applied to this canvas.
    // Writes the total number of pixels rasterized and the time spent
    // rasterizing to the stats if the respective pointer is not nullptr.
    // It is assumed that the canvas passed here will only be rasterized by
    // this raster source via this call.
    virtual void PlaybackToCanvas(SkCanvas* canvas,
        const gfx::Rect& canvas_bitmap_rect,
        const gfx::Rect& canvas_playback_rect,
        float contents_scale) const = 0;

    // Similar to above, except that the canvas passed here can (or was already)
    // rasterized into by another raster source. That is, it is not safe to clear
    // the canvas or discard its underlying memory.
    virtual void PlaybackToSharedCanvas(SkCanvas* canvas,
        const gfx::Rect& canvas_rect,
        float contents_scale) const = 0;

    // Analyze to determine if the given rect at given scale is of solid color in
    // this raster source.
    virtual void PerformSolidColorAnalysis(
        const gfx::Rect& content_rect,
        float contents_scale,
        SolidColorAnalysis* analysis) const = 0;

    // Returns true iff the whole raster source is of solid color.
    virtual bool IsSolidColor() const = 0;

    // Returns the color of the raster source if it is solid color. The results
    // are unspecified if IsSolidColor returns false.
    virtual SkColor GetSolidColor() const = 0;

    // Returns the size of this raster source.
    virtual gfx::Size GetSize() const = 0;

    // Populate the given list with all images that may overlap the given
    // rect in layer space.
    virtual void GetDiscardableImagesInRect(
        const gfx::Rect& layer_rect,
        std::vector<PositionImage>* images) const = 0;

    // Return true iff this raster source can raster the given rect in layer
    // space.
    virtual bool CoversRect(const gfx::Rect& layer_rect) const = 0;

    // Returns true if this raster source has anything to rasterize.
    virtual bool HasRecordings() const = 0;

    // Valid rectangle in which everything is recorded and can be rastered from.
    virtual gfx::Rect RecordedViewport() const = 0;

    // Informs the raster source that it should attempt to use distance field text
    // during rasterization.
    virtual void SetShouldAttemptToUseDistanceFieldText() = 0;

    // Return true iff this raster source would benefit from using distance
    // field text.
    virtual bool ShouldAttemptToUseDistanceFieldText() const = 0;

    // Tracing functionality.
    virtual void DidBeginTracing() = 0;
    virtual void AsValueInto(base::trace_event::TracedValue* array) const = 0;
    virtual skia::RefPtr<SkPicture> GetFlattenedPicture() = 0;
    virtual size_t GetPictureMemoryUsage() const = 0;

    // Return true if LCD anti-aliasing may be used when rastering text.
    virtual bool CanUseLCDText() const = 0;

    virtual scoped_refptr<RasterSource> CreateCloneWithoutLCDText() const = 0;

protected:
    friend class base::RefCountedThreadSafe<RasterSource>;

    RasterSource() { }
    virtual ~RasterSource() { }

private:
    DISALLOW_COPY_AND_ASSIGN(RasterSource);
};

} // namespace cc

#endif // CC_PLAYBACK_RASTER_SOURCE_H_
