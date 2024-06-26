// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <limits>
#include <set>

#include "base/trace_event/trace_event.h"
#include "cc/base/region.h"
#include "cc/debug/debug_colors.h"
#include "cc/playback/picture_pile_impl.h"
#include "cc/playback/raster_source_helper.h"
#include "skia/ext/analysis_canvas.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "ui/gfx/geometry/rect_conversions.h"

namespace cc {

scoped_refptr<PicturePileImpl> PicturePileImpl::CreateFromPicturePile(
    const PicturePile* other,
    bool can_use_lcd_text)
{
    return make_scoped_refptr(new PicturePileImpl(other, can_use_lcd_text));
}

PicturePileImpl::PicturePileImpl()
    : background_color_(SK_ColorTRANSPARENT)
    , requires_clear_(true)
    , can_use_lcd_text_(true)
    , is_solid_color_(false)
    , solid_color_(SK_ColorTRANSPARENT)
    , has_any_recordings_(false)
    , clear_canvas_with_debug_color_(false)
    , min_contents_scale_(0.f)
    , slow_down_raster_scale_factor_for_debug_(0)
    , should_attempt_to_use_distance_field_text_(false)
    , picture_memory_usage_(0)
{
}

PicturePileImpl::PicturePileImpl(const PicturePile* other,
    bool can_use_lcd_text)
    : picture_map_(other->picture_map_)
    , tiling_(other->tiling_)
    , background_color_(other->background_color_)
    , requires_clear_(other->requires_clear_)
    , can_use_lcd_text_(can_use_lcd_text)
    , is_solid_color_(other->is_solid_color_)
    , solid_color_(other->solid_color_)
    , recorded_viewport_(other->recorded_viewport_)
    , has_any_recordings_(other->has_any_recordings_)
    , clear_canvas_with_debug_color_(other->clear_canvas_with_debug_color_)
    , min_contents_scale_(other->min_contents_scale_)
    , slow_down_raster_scale_factor_for_debug_(
          other->slow_down_raster_scale_factor_for_debug_)
    , should_attempt_to_use_distance_field_text_(false)
    , picture_memory_usage_(0)
{
    // Figure out the picture size upon construction.
    base::hash_set<const Picture*> pictures_seen;
    for (const auto& map_value : picture_map_) {
        const Picture* picture = map_value.second.get();
        if (pictures_seen.insert(picture).second)
            picture_memory_usage_ += picture->ApproximateMemoryUsage();
    }
}

PicturePileImpl::PicturePileImpl(const PicturePileImpl* other,
    bool can_use_lcd_text)
    : picture_map_(other->picture_map_)
    , tiling_(other->tiling_)
    , background_color_(other->background_color_)
    , requires_clear_(other->requires_clear_)
    , can_use_lcd_text_(can_use_lcd_text)
    , is_solid_color_(other->is_solid_color_)
    , solid_color_(other->solid_color_)
    , recorded_viewport_(other->recorded_viewport_)
    , has_any_recordings_(other->has_any_recordings_)
    , clear_canvas_with_debug_color_(other->clear_canvas_with_debug_color_)
    , min_contents_scale_(other->min_contents_scale_)
    , slow_down_raster_scale_factor_for_debug_(
          other->slow_down_raster_scale_factor_for_debug_)
    , should_attempt_to_use_distance_field_text_(
          other->should_attempt_to_use_distance_field_text_)
    , picture_memory_usage_(other->picture_memory_usage_)
{
}

PicturePileImpl::~PicturePileImpl()
{
}

void PicturePileImpl::PlaybackToSharedCanvas(SkCanvas* canvas,
    const gfx::Rect& canvas_rect,
    float contents_scale) const
{
    RasterCommon(canvas, NULL, canvas_rect, contents_scale);
}

void PicturePileImpl::RasterForAnalysis(skia::AnalysisCanvas* canvas,
    const gfx::Rect& canvas_rect,
    float contents_scale) const
{
    RasterCommon(canvas, canvas, canvas_rect, contents_scale);
}

void PicturePileImpl::PlaybackToCanvas(SkCanvas* canvas,
    const gfx::Rect& canvas_bitmap_rect,
    const gfx::Rect& canvas_playback_rect,
    float contents_scale) const
{
    RasterSourceHelper::PrepareForPlaybackToCanvas(
        canvas, canvas_bitmap_rect, canvas_bitmap_rect,
        gfx::Rect(tiling_.tiling_size()), contents_scale, background_color_,
        clear_canvas_with_debug_color_, requires_clear_);
    RasterCommon(canvas, NULL, canvas_bitmap_rect, contents_scale);
}

void PicturePileImpl::CoalesceRasters(const gfx::Rect& canvas_rect,
    const gfx::Rect& content_rect,
    float contents_scale,
    PictureRegionMap* results) const
{
    DCHECK(results);
    // Rasterize the collection of relevant picture piles.
    gfx::Rect layer_rect = gfx::ScaleToEnclosingRect(
        content_rect, 1.f / contents_scale);

    // Make sure pictures don't overlap by keeping track of previous right/bottom.
    int min_content_left = -1;
    int min_content_top = -1;
    int last_row_index = -1;
    int last_col_index = -1;
    gfx::Rect last_content_rect;

    // Coalesce rasters of the same picture into different rects:
    //  - Compute the clip of each of the pile chunks,
    //  - Subtract it from the canvas rect to get difference region
    //  - Later, use the difference region to subtract each of the comprising
    //    rects from the canvas.
    // Note that in essence, we're trying to mimic clipRegion with intersect op
    // that also respects the current canvas transform and clip. In order to use
    // the canvas transform, we must stick to clipRect operations (clipRegion
    // ignores the transform). Intersect then can be written as subtracting the
    // negation of the region we're trying to intersect. Luckily, we know that all
    // of the rects will have to fit into |content_rect|, so we can start with
    // that and subtract chunk rects to get the region that we need to subtract
    // from the canvas. Then, we can use clipRect with difference op to subtract
    // each rect in the region.
    bool include_borders = true;
    for (TilingData::Iterator tile_iter(&tiling_, layer_rect, include_borders);
         tile_iter;
         ++tile_iter) {
        PictureMap::const_iterator map_iter = picture_map_.find(tile_iter.index());
        if (map_iter == picture_map_.end())
            continue;
        const Picture* picture = map_iter->second.get();
        DCHECK(picture);

        // This is intentionally *enclosed* rect, so that the clip is aligned on
        // integral post-scale content pixels and does not extend past the edges
        // of the picture chunk's layer rect.  The min_contents_scale enforces that
        // enough buffer pixels have been added such that the enclosed rect
        // encompasses all invalidated pixels at any larger scale level.
        gfx::Rect chunk_rect = PaddedRect(tile_iter.index());
        gfx::Rect content_clip = gfx::ScaleToEnclosedRect(chunk_rect, contents_scale);
        DCHECK(!content_clip.IsEmpty()) << "Layer rect: "
                                        << picture->LayerRect().ToString()
                                        << "Contents scale: " << contents_scale;
        content_clip.Intersect(canvas_rect);

        // Make sure iterator goes top->bottom.
        DCHECK_GE(tile_iter.index_y(), last_row_index);
        if (tile_iter.index_y() > last_row_index) {
            // First tile in a new row.
            min_content_left = content_clip.x();
            min_content_top = last_content_rect.bottom();
        } else {
            // Make sure iterator goes left->right.
            DCHECK_GT(tile_iter.index_x(), last_col_index);
            min_content_left = last_content_rect.right();
            min_content_top = last_content_rect.y();
        }

        last_col_index = tile_iter.index_x();
        last_row_index = tile_iter.index_y();

        // Only inset if the content_clip is less than then previous min.
        int inset_left = std::max(0, min_content_left - content_clip.x());
        int inset_top = std::max(0, min_content_top - content_clip.y());
        content_clip.Inset(inset_left, inset_top, 0, 0);

        PictureRegionMap::iterator it = results->find(picture);
        Region* clip_region;
        if (it == results->end()) {
            // The clip for a set of coalesced pictures starts out clipping the entire
            // canvas.  Each picture added to the set must subtract its own bounds
            // from the clip region, poking a hole so that the picture is unclipped.
            clip_region = &(*results)[picture];
            *clip_region = canvas_rect;
        } else {
            clip_region = &it->second;
        }

        DCHECK(clip_region->Contains(content_clip))
            << "Content clips should not overlap.";
        clip_region->Subtract(content_clip);
        last_content_rect = content_clip;
    }
}

void PicturePileImpl::RasterCommon(SkCanvas* canvas,
    SkPicture::AbortCallback* callback,
    const gfx::Rect& canvas_rect,
    float contents_scale) const
{
    DCHECK(contents_scale >= min_contents_scale_);

    canvas->translate(-canvas_rect.x(), -canvas_rect.y());
    gfx::Rect content_tiling_rect = gfx::ToEnclosingRect(
        gfx::ScaleRect(gfx::Rect(tiling_.tiling_size()), contents_scale));
    content_tiling_rect.Intersect(canvas_rect);

    canvas->clipRect(gfx::RectToSkRect(content_tiling_rect),
        SkRegion::kIntersect_Op);

    PictureRegionMap picture_region_map;
    CoalesceRasters(
        canvas_rect, content_tiling_rect, contents_scale, &picture_region_map);

#ifndef NDEBUG
    Region total_clip;
#endif // NDEBUG

    // Iterate the coalesced map and use each picture's region
    // to clip the canvas.
    for (PictureRegionMap::iterator it = picture_region_map.begin();
         it != picture_region_map.end();
         ++it) {
        const Picture* picture = it->first;
        Region negated_clip_region = it->second;

#ifndef NDEBUG
        Region positive_clip = content_tiling_rect;
        positive_clip.Subtract(negated_clip_region);
        // Make sure we never rasterize the same region twice.
        DCHECK(!total_clip.Intersects(positive_clip));
        total_clip.Union(positive_clip);
#endif // NDEBUG

        int repeat_count = std::max(1, slow_down_raster_scale_factor_for_debug_);

        for (int j = 0; j < repeat_count; ++j)
            picture->Raster(canvas, callback, negated_clip_region, contents_scale);
    }

#ifndef NDEBUG
    // Fill the clip with debug color. This allows us to
    // distinguish between non painted areas and problems with missing
    // pictures.
    SkPaint paint;
    for (Region::Iterator it(total_clip); it.has_rect(); it.next())
        canvas->clipRect(gfx::RectToSkRect(it.rect()), SkRegion::kDifference_Op);
    paint.setColor(DebugColors::MissingPictureFillColor());
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    canvas->drawPaint(paint);
#endif // NDEBUG
}

skia::RefPtr<SkPicture> PicturePileImpl::GetFlattenedPicture()
{
    TRACE_EVENT0("cc", "PicturePileImpl::GetFlattenedPicture");

    gfx::Rect tiling_rect(tiling_.tiling_size());
    SkPictureRecorder recorder;
    SkCanvas* canvas = recorder.beginRecording(tiling_rect.width(), tiling_rect.height());
    if (!tiling_rect.IsEmpty())
        PlaybackToCanvas(canvas, tiling_rect, tiling_rect, 1.0);
    skia::RefPtr<SkPicture> picture = skia::AdoptRef(recorder.endRecordingAsPicture());

    return picture;
}

size_t PicturePileImpl::GetPictureMemoryUsage() const
{
    return picture_memory_usage_;
}

void PicturePileImpl::PerformSolidColorAnalysis(
    const gfx::Rect& content_rect,
    float contents_scale,
    RasterSource::SolidColorAnalysis* analysis) const
{
    DCHECK(analysis);
    TRACE_EVENT0("cc", "PicturePileImpl::PerformSolidColorAnalysis");

    gfx::Rect layer_rect = gfx::ScaleToEnclosingRect(
        content_rect, 1.0f / contents_scale);

    layer_rect.Intersect(gfx::Rect(tiling_.tiling_size()));

    skia::AnalysisCanvas canvas(layer_rect.width(), layer_rect.height());

    RasterForAnalysis(&canvas, layer_rect, 1.0f);

    analysis->is_solid_color = canvas.GetColorIfSolid(&analysis->solid_color);
}

void PicturePileImpl::GatherPixelRefs(
    const gfx::Rect& content_rect,
    float contents_scale,
    std::vector<SkPixelRef*>* pixel_refs) const
{
    DCHECK_EQ(0u, pixel_refs->size());
    for (PixelRefIterator iter(content_rect, contents_scale, this); iter;
         ++iter) {
        pixel_refs->push_back(*iter);
    }
}

bool PicturePileImpl::CoversRect(const gfx::Rect& content_rect,
    float contents_scale) const
{
    if (tiling_.tiling_size().IsEmpty())
        return false;
    gfx::Rect layer_rect = gfx::ScaleToEnclosingRect(content_rect, 1.f / contents_scale);
    layer_rect.Intersect(gfx::Rect(tiling_.tiling_size()));

    // Common case inside of viewport to avoid the slower map lookups.
    if (recorded_viewport_.Contains(layer_rect)) {
        // Sanity check that there are no false positives in recorded_viewport_.
        DCHECK(CanRasterSlowTileCheck(layer_rect));
        return true;
    }

    return CanRasterSlowTileCheck(layer_rect);
}

gfx::Size PicturePileImpl::GetSize() const
{
    return tiling_.tiling_size();
}

bool PicturePileImpl::IsSolidColor() const
{
    return is_solid_color_;
}

SkColor PicturePileImpl::GetSolidColor() const
{
    DCHECK(IsSolidColor());
    return solid_color_;
}

bool PicturePileImpl::HasRecordings() const
{
    return has_any_recordings_;
}

gfx::Rect PicturePileImpl::PaddedRect(const PictureMapKey& key) const
{
    gfx::Rect padded_rect = tiling_.TileBounds(key.first, key.second);
    padded_rect.Inset(-buffer_pixels(), -buffer_pixels(), -buffer_pixels(),
        -buffer_pixels());
    return padded_rect;
}

bool PicturePileImpl::CanRasterSlowTileCheck(
    const gfx::Rect& layer_rect) const
{
    bool include_borders = false;
    for (TilingData::Iterator tile_iter(&tiling_, layer_rect, include_borders);
         tile_iter; ++tile_iter) {
        PictureMap::const_iterator map_iter = picture_map_.find(tile_iter.index());
        if (map_iter == picture_map_.end())
            return false;
    }
    return true;
}

void PicturePileImpl::SetShouldAttemptToUseDistanceFieldText()
{
    should_attempt_to_use_distance_field_text_ = true;
}

bool PicturePileImpl::ShouldAttemptToUseDistanceFieldText() const
{
    return should_attempt_to_use_distance_field_text_;
}

void PicturePileImpl::AsValueInto(
    base::trace_event::TracedValue* pictures) const
{
    gfx::Rect tiling_rect(tiling_.tiling_size());
    std::set<const void*> appended_pictures;
    bool include_borders = true;
    for (TilingData::Iterator tile_iter(&tiling_, tiling_rect, include_borders);
         tile_iter; ++tile_iter) {
        PictureMap::const_iterator map_iter = picture_map_.find(tile_iter.index());
        if (map_iter == picture_map_.end())
            continue;

        const Picture* picture = map_iter->second.get();
        if (appended_pictures.count(picture) == 0) {
            appended_pictures.insert(picture);
            TracedValue::AppendIDRef(picture, pictures);
        }
    }
}

bool PicturePileImpl::CanUseLCDText() const
{
    return can_use_lcd_text_;
}

scoped_refptr<RasterSource> PicturePileImpl::CreateCloneWithoutLCDText() const
{
    DCHECK(CanUseLCDText());
    bool can_use_lcd_text = false;
    return scoped_refptr<RasterSource>(
        new PicturePileImpl(this, can_use_lcd_text));
}

PicturePileImpl::PixelRefIterator::PixelRefIterator(
    const gfx::Rect& content_rect,
    float contents_scale,
    const PicturePileImpl* picture_pile)
    : picture_pile_(picture_pile)
    , layer_rect_(
          gfx::ScaleToEnclosingRect(content_rect, 1.f / contents_scale))
    , tile_iterator_(&picture_pile_->tiling_,
          layer_rect_,
          false /* include_borders */)
{
    // Early out if there isn't a single tile.
    if (!tile_iterator_)
        return;

    AdvanceToTilePictureWithPixelRefs();
}

PicturePileImpl::PixelRefIterator::~PixelRefIterator()
{
}

PicturePileImpl::PixelRefIterator&
PicturePileImpl::PixelRefIterator::operator++()
{
    ++pixel_ref_iterator_;
    if (pixel_ref_iterator_)
        return *this;

    ++tile_iterator_;
    AdvanceToTilePictureWithPixelRefs();
    return *this;
}

void PicturePileImpl::PixelRefIterator::AdvanceToTilePictureWithPixelRefs()
{
    for (; tile_iterator_; ++tile_iterator_) {
        PictureMap::const_iterator it = picture_pile_->picture_map_.find(tile_iterator_.index());
        if (it == picture_pile_->picture_map_.end())
            continue;

        const Picture* picture = it->second.get();
        if ((processed_pictures_.count(picture) != 0) || !picture->WillPlayBackBitmaps())
            continue;

        processed_pictures_.insert(picture);
        pixel_ref_iterator_ = picture->GetPixelRefMapIterator(layer_rect_);
        if (pixel_ref_iterator_)
            break;
    }
}

void PicturePileImpl::DidBeginTracing()
{
    std::set<const void*> processed_pictures;
    for (const auto& map_pair : picture_map_) {
        const Picture* picture = map_pair.second.get();
        if (processed_pictures.count(picture) == 0) {
            picture->EmitTraceSnapshot();
            processed_pictures.insert(picture);
        }
    }
}

} // namespace cc
