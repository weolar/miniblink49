// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/tiles/picture_layer_tiling.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

#include "base/containers/hash_tables.h"
#if USING_VC6RT != 1
#include "base/containers/small_map.h"
#endif
#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/base/math_util.h"
#include "cc/playback/raster_source.h"
#include "cc/tiles/prioritized_tile.h"
#include "cc/tiles/tile.h"
#include "cc/tiles/tile_priority.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/safe_integer_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {
namespace {

    const float kSoonBorderDistanceViewportPercentage = 0.15f;
    const float kMaxSoonBorderDistanceInScreenPixels = 312.f;

} // namespace

scoped_ptr<PictureLayerTiling> PictureLayerTiling::Create(
    WhichTree tree,
    float contents_scale,
    scoped_refptr<RasterSource> raster_source,
    PictureLayerTilingClient* client,
    size_t tiling_interest_area_padding,
    float skewport_target_time_in_seconds,
    int skewport_extrapolation_limit_in_content_pixels)
{
    return make_scoped_ptr(new PictureLayerTiling(
        tree, contents_scale, raster_source, client, tiling_interest_area_padding,
        skewport_target_time_in_seconds,
        skewport_extrapolation_limit_in_content_pixels));
}

PictureLayerTiling::PictureLayerTiling(
    WhichTree tree,
    float contents_scale,
    scoped_refptr<RasterSource> raster_source,
    PictureLayerTilingClient* client,
    size_t tiling_interest_area_padding,
    float skewport_target_time_in_seconds,
    int skewport_extrapolation_limit_in_content_pixels)
    : tiling_interest_area_padding_(tiling_interest_area_padding)
    , skewport_target_time_in_seconds_(skewport_target_time_in_seconds)
    , skewport_extrapolation_limit_in_content_pixels_(
          skewport_extrapolation_limit_in_content_pixels)
    , contents_scale_(contents_scale)
    , client_(client)
    , tree_(tree)
    , raster_source_(raster_source)
    , resolution_(NON_IDEAL_RESOLUTION)
    , may_contain_low_resolution_tiles_(false)
    , tiling_data_(gfx::Size(), gfx::Size(), kBorderTexels)
    , can_require_tiles_for_activation_(false)
    , current_content_to_screen_scale_(0.f)
    , has_visible_rect_tiles_(false)
    , has_skewport_rect_tiles_(false)
    , has_soon_border_rect_tiles_(false)
    , has_eventually_rect_tiles_(false)
    , all_tiles_done_(true)
{
    DCHECK(!raster_source->IsSolidColor());
    gfx::Size content_bounds = gfx::ScaleToCeiledSize(raster_source_->GetSize(), contents_scale);
    gfx::Size tile_size = client_->CalculateTileSize(content_bounds);

    DCHECK(!gfx::ScaleToFlooredSize(raster_source_->GetSize(), contents_scale)
                .IsEmpty())
        << "Tiling created with scale too small as contents become empty."
        << " Layer bounds: " << raster_source_->GetSize().ToString()
        << " Contents scale: " << contents_scale;

    tiling_data_.SetTilingSize(content_bounds);
    tiling_data_.SetMaxTextureSize(tile_size);
}

PictureLayerTiling::~PictureLayerTiling()
{
}

// static
float PictureLayerTiling::CalculateSoonBorderDistance(
    const gfx::Rect& visible_rect_in_content_space,
    float content_to_screen_scale)
{
    float max_dimension = std::max(visible_rect_in_content_space.width(),
        visible_rect_in_content_space.height());
    return std::min(
        kMaxSoonBorderDistanceInScreenPixels / content_to_screen_scale,
        max_dimension * kSoonBorderDistanceViewportPercentage);
}

Tile* PictureLayerTiling::CreateTile(const Tile::CreateInfo& info)
{
    const int i = info.tiling_i_index;
    const int j = info.tiling_j_index;
    TileMapKey key(i, j);
    DCHECK(tiles_.find(key) == tiles_.end());

    if (!raster_source_->CoversRect(info.enclosing_layer_rect))
        return nullptr;

    all_tiles_done_ = false;
    ScopedTilePtr tile = client_->CreateTile(info);
    Tile* raw_ptr = tile.get();
    tiles_.add(key, tile.Pass());
    return raw_ptr;
}

void PictureLayerTiling::CreateMissingTilesInLiveTilesRect()
{
    const PictureLayerTiling* active_twin = tree_ == PENDING_TREE ? client_->GetPendingOrActiveTwinTiling(this)
                                                                  : nullptr;
    const Region* invalidation = active_twin ? client_->GetPendingInvalidation() : nullptr;

    bool include_borders = false;
    for (TilingData::Iterator iter(&tiling_data_, live_tiles_rect_,
             include_borders);
         iter; ++iter) {
        TileMapKey key(iter.index());
        TileMap::iterator find = tiles_.find(key);
        if (find != tiles_.end())
            continue;

        Tile::CreateInfo info = CreateInfoForTile(key.index_x, key.index_y);
        if (ShouldCreateTileAt(info)) {
            Tile* tile = CreateTile(info);

            // If this is the pending tree, then the active twin tiling may contain
            // the previous content ID of these tiles. In that case, we need only
            // partially raster the tile content.
            if (tile && invalidation && TilingMatchesTileIndices(active_twin)) {
                if (const Tile* old_tile = active_twin->TileAt(key.index_x, key.index_y)) {
                    gfx::Rect tile_rect = tile->content_rect();
                    gfx::Rect invalidated;
                    for (Region::Iterator iter(*invalidation); iter.has_rect();
                         iter.next()) {
                        gfx::Rect invalid_content_rect = gfx::ScaleToEnclosingRect(iter.rect(), contents_scale_);
                        invalid_content_rect.Intersect(tile_rect);
                        invalidated.Union(invalid_content_rect);
                    }
                    tile->SetInvalidated(invalidated, old_tile->id());
                }
            }
        }
    }
    VerifyLiveTilesRect(false);
}

void PictureLayerTiling::TakeTilesAndPropertiesFrom(
    PictureLayerTiling* pending_twin,
    const Region& layer_invalidation)
{
    TRACE_EVENT0("cc", "TakeTilesAndPropertiesFrom");
    SetRasterSourceAndResize(pending_twin->raster_source_);

    RemoveTilesInRegion(layer_invalidation, false /* recreate tiles */);

    resolution_ = pending_twin->resolution_;
    bool create_missing_tiles = false;
    if (live_tiles_rect_.IsEmpty()) {
        live_tiles_rect_ = pending_twin->live_tiles_rect();
        create_missing_tiles = true;
    } else {
        SetLiveTilesRect(pending_twin->live_tiles_rect());
    }

    if (tiles_.empty()) {
        tiles_.swap(pending_twin->tiles_);
        all_tiles_done_ = pending_twin->all_tiles_done_;
    } else {
        while (!pending_twin->tiles_.empty()) {
            TileMapKey key = pending_twin->tiles_.begin()->first;
            tiles_.set(key, pending_twin->tiles_.take_and_erase(key));
        }
        all_tiles_done_ &= pending_twin->all_tiles_done_;
    }
    DCHECK(pending_twin->tiles_.empty());
    pending_twin->all_tiles_done_ = true;

    if (create_missing_tiles)
        CreateMissingTilesInLiveTilesRect();

    VerifyLiveTilesRect(false);

    SetTilePriorityRects(pending_twin->current_content_to_screen_scale_,
        pending_twin->current_visible_rect_,
        pending_twin->current_skewport_rect_,
        pending_twin->current_soon_border_rect_,
        pending_twin->current_eventually_rect_,
        pending_twin->current_occlusion_in_layer_space_);
}

void PictureLayerTiling::SetRasterSourceAndResize(
    scoped_refptr<RasterSource> raster_source)
{
    DCHECK(!raster_source->IsSolidColor());
    gfx::Size old_layer_bounds = raster_source_->GetSize();
    raster_source_.swap(raster_source);
    gfx::Size new_layer_bounds = raster_source_->GetSize();
    gfx::Size content_bounds = gfx::ScaleToCeiledSize(new_layer_bounds, contents_scale_);
    gfx::Size tile_size = client_->CalculateTileSize(content_bounds);

    if (tile_size != tiling_data_.max_texture_size()) {
        tiling_data_.SetTilingSize(content_bounds);
        tiling_data_.SetMaxTextureSize(tile_size);
        // When the tile size changes, the TilingData positions no longer work
        // as valid keys to the TileMap, so just drop all tiles and clear the live
        // tiles rect.
        Reset();
        return;
    }

    if (old_layer_bounds == new_layer_bounds)
        return;

    // The SetLiveTilesRect() method would drop tiles outside the new bounds,
    // but may do so incorrectly if resizing the tiling causes the number of
    // tiles in the tiling_data_ to change.
    gfx::Rect content_rect(content_bounds);
    int before_left = tiling_data_.TileXIndexFromSrcCoord(live_tiles_rect_.x());
    int before_top = tiling_data_.TileYIndexFromSrcCoord(live_tiles_rect_.y());
    int before_right = tiling_data_.TileXIndexFromSrcCoord(live_tiles_rect_.right() - 1);
    int before_bottom = tiling_data_.TileYIndexFromSrcCoord(live_tiles_rect_.bottom() - 1);

    // The live_tiles_rect_ is clamped to stay within the tiling size as we
    // change it.
    live_tiles_rect_.Intersect(content_rect);
    tiling_data_.SetTilingSize(content_bounds);

    int after_right = -1;
    int after_bottom = -1;
    if (!live_tiles_rect_.IsEmpty()) {
        after_right = tiling_data_.TileXIndexFromSrcCoord(live_tiles_rect_.right() - 1);
        after_bottom = tiling_data_.TileYIndexFromSrcCoord(live_tiles_rect_.bottom() - 1);
    }

    // There is no recycled twin since this is run on the pending tiling
    // during commit, and on the active tree during activate.
    // Drop tiles outside the new layer bounds if the layer shrank.
    for (int i = after_right + 1; i <= before_right; ++i) {
        for (int j = before_top; j <= before_bottom; ++j)
            RemoveTileAt(i, j);
    }
    for (int i = before_left; i <= after_right; ++i) {
        for (int j = after_bottom + 1; j <= before_bottom; ++j)
            RemoveTileAt(i, j);
    }

    if (after_right > before_right) {
        DCHECK_EQ(after_right, before_right + 1);
        for (int j = before_top; j <= after_bottom; ++j) {
            Tile::CreateInfo info = CreateInfoForTile(after_right, j);
            if (ShouldCreateTileAt(info))
                CreateTile(info);
        }
    }
    if (after_bottom > before_bottom) {
        DCHECK_EQ(after_bottom, before_bottom + 1);
        for (int i = before_left; i <= before_right; ++i) {
            Tile::CreateInfo info = CreateInfoForTile(i, after_bottom);
            if (ShouldCreateTileAt(info))
                CreateTile(info);
        }
    }
}

void PictureLayerTiling::Invalidate(const Region& layer_invalidation)
{
    DCHECK_IMPLIES(tree_ == ACTIVE_TREE,
        !client_->GetPendingOrActiveTwinTiling(this));
    RemoveTilesInRegion(layer_invalidation, true /* recreate tiles */);
}

void PictureLayerTiling::RemoveTilesInRegion(const Region& layer_invalidation,
    bool recreate_tiles)
{
    // We only invalidate the active tiling when it's orphaned: it has no pending
    // twin, so it's slated for removal in the future.
    if (live_tiles_rect_.IsEmpty())
        return;

        // Pick 16 for the size of the SmallMap before it promotes to a hash_map.
        // 4x4 tiles should cover most small invalidations, and walking a vector of
        // 16 is fast enough. If an invalidation is huge we will fall back to a
        // hash_map instead of a vector in the SmallMap.
#if USING_VC6RT != 1
    base::SmallMap<base::hash_map<TileMapKey, gfx::Rect>, 16> remove_tiles;
#else
    base::hash_map<TileMapKey, gfx::Rect> remove_tiles; // weolar
#endif
    gfx::Rect expanded_live_tiles_rect = tiling_data_.ExpandRectToTileBounds(live_tiles_rect_);
    for (Region::Iterator iter(layer_invalidation); iter.has_rect();
         iter.next()) {
        gfx::Rect layer_rect = iter.rect();
        // The pixels which are invalid in content space.
        gfx::Rect invalid_content_rect = gfx::ScaleToEnclosingRect(layer_rect, contents_scale_);
        gfx::Rect coverage_content_rect = invalid_content_rect;
        // Avoid needless work by not bothering to invalidate where there aren't
        // tiles.
        coverage_content_rect.Intersect(expanded_live_tiles_rect);
        if (coverage_content_rect.IsEmpty())
            continue;
        // Since the content_rect needs to invalidate things that only touch a
        // border of a tile, we need to include the borders while iterating.
        bool include_borders = true;
        for (TilingData::Iterator iter(&tiling_data_, coverage_content_rect,
                 include_borders);
             iter; ++iter) {
            // This also adds the TileMapKey to the map.
            remove_tiles[TileMapKey(iter.index())].Union(invalid_content_rect);
        }
    }

    for (const auto& pair : remove_tiles) {
        const TileMapKey& key = pair.first;
        const gfx::Rect& invalid_content_rect = pair.second;
        // TODO(danakj): This old_tile will not exist if we are committing to a
        // pending tree since there is no tile there to remove, which prevents
        // tiles from knowing the invalidation rect and content id. crbug.com/490847
        ScopedTilePtr old_tile = TakeTileAt(key.index_x, key.index_y);
        if (recreate_tiles && old_tile) {
            Tile::CreateInfo info = CreateInfoForTile(key.index_x, key.index_y);
            if (Tile* tile = CreateTile(info))
                tile->SetInvalidated(invalid_content_rect, old_tile->id());
        }
    }
}

Tile::CreateInfo PictureLayerTiling::CreateInfoForTile(int i, int j) const
{
    gfx::Rect tile_rect = tiling_data_.TileBoundsWithBorder(i, j);
    tile_rect.set_size(tiling_data_.max_texture_size());
    gfx::Rect enclosing_layer_rect = gfx::ScaleToEnclosingRect(tile_rect, 1.f / contents_scale_);
    return Tile::CreateInfo(i, j, enclosing_layer_rect, tile_rect,
        contents_scale_);
}

bool PictureLayerTiling::ShouldCreateTileAt(
    const Tile::CreateInfo& info) const
{
    const int i = info.tiling_i_index;
    const int j = info.tiling_j_index;
    // Active tree should always create a tile. The reason for this is that active
    // tree represents content that we draw on screen, which means that whenever
    // we check whether a tile should exist somewhere, the answer is yes. This
    // doesn't mean it will actually be created (if raster source doesn't cover
    // the tile for instance). Pending tree, on the other hand, should only be
    // creating tiles that are different from the current active tree, which is
    // represented by the logic in the rest of the function.
    if (tree_ == ACTIVE_TREE)
        return true;

    // If the pending tree has no active twin, then it needs to create all tiles.
    const PictureLayerTiling* active_twin = client_->GetPendingOrActiveTwinTiling(this);
    if (!active_twin)
        return true;

    // Pending tree will override the entire active tree if indices don't match.
    if (!TilingMatchesTileIndices(active_twin))
        return true;

    // If the active tree can't create a tile, because of its raster source, then
    // the pending tree should create one.
    if (!active_twin->raster_source()->CoversRect(info.enclosing_layer_rect))
        return true;

    const Region* layer_invalidation = client_->GetPendingInvalidation();

    // If this tile is invalidated, then the pending tree should create one.
    if (layer_invalidation && layer_invalidation->Intersects(info.enclosing_layer_rect))
        return true;

    // If the active tree doesn't have a tile here, but it's in the pending tree's
    // visible rect, then the pending tree should create a tile. This can happen
    // if the pending visible rect is outside of the active tree's live tiles
    // rect. In those situations, we need to block activation until we're ready to
    // display content, which will have to come from the pending tree.
    if (!active_twin->TileAt(i, j) && current_visible_rect_.Intersects(info.content_rect))
        return true;

    // In all other cases, the pending tree doesn't need to create a tile.
    return false;
}

bool PictureLayerTiling::TilingMatchesTileIndices(
    const PictureLayerTiling* twin) const
{
    return tiling_data_.max_texture_size() == twin->tiling_data_.max_texture_size();
}

PictureLayerTiling::CoverageIterator::CoverageIterator()
    : tiling_(NULL)
    , current_tile_(NULL)
    , tile_i_(0)
    , tile_j_(0)
    , left_(0)
    , top_(0)
    , right_(-1)
    , bottom_(-1)
{
}

PictureLayerTiling::CoverageIterator::CoverageIterator(
    const PictureLayerTiling* tiling,
    float dest_scale,
    const gfx::Rect& dest_rect)
    : tiling_(tiling)
    , dest_rect_(dest_rect)
    , dest_to_content_scale_(0)
    , current_tile_(NULL)
    , tile_i_(0)
    , tile_j_(0)
    , left_(0)
    , top_(0)
    , right_(-1)
    , bottom_(-1)
{
    DCHECK(tiling_);
    if (dest_rect_.IsEmpty())
        return;

    dest_to_content_scale_ = tiling_->contents_scale_ / dest_scale;

    gfx::Rect content_rect = gfx::ScaleToEnclosingRect(dest_rect_,
        dest_to_content_scale_,
        dest_to_content_scale_);
    // IndexFromSrcCoord clamps to valid tile ranges, so it's necessary to
    // check for non-intersection first.
    content_rect.Intersect(gfx::Rect(tiling_->tiling_size()));
    if (content_rect.IsEmpty())
        return;

    left_ = tiling_->tiling_data_.TileXIndexFromSrcCoord(content_rect.x());
    top_ = tiling_->tiling_data_.TileYIndexFromSrcCoord(content_rect.y());
    right_ = tiling_->tiling_data_.TileXIndexFromSrcCoord(
        content_rect.right() - 1);
    bottom_ = tiling_->tiling_data_.TileYIndexFromSrcCoord(
        content_rect.bottom() - 1);

    tile_i_ = left_ - 1;
    tile_j_ = top_;
    ++(*this);
}

PictureLayerTiling::CoverageIterator::~CoverageIterator()
{
}

PictureLayerTiling::CoverageIterator&
PictureLayerTiling::CoverageIterator::operator++()
{
    if (tile_j_ > bottom_)
        return *this;

    bool first_time = tile_i_ < left_;
    bool new_row = false;
    tile_i_++;
    if (tile_i_ > right_) {
        tile_i_ = left_;
        tile_j_++;
        new_row = true;
        if (tile_j_ > bottom_) {
            current_tile_ = NULL;
            return *this;
        }
    }

    current_tile_ = tiling_->TileAt(tile_i_, tile_j_);

    // Calculate the current geometry rect.  Due to floating point rounding
    // and ToEnclosingRect, tiles might overlap in destination space on the
    // edges.
    gfx::Rect last_geometry_rect = current_geometry_rect_;

    gfx::Rect content_rect = tiling_->tiling_data_.TileBounds(tile_i_, tile_j_);

    current_geometry_rect_ = gfx::ScaleToEnclosingRect(content_rect, 1 / dest_to_content_scale_);

    current_geometry_rect_.Intersect(dest_rect_);
    DCHECK(!current_geometry_rect_.IsEmpty());

    if (first_time)
        return *this;

    // Iteration happens left->right, top->bottom.  Running off the bottom-right
    // edge is handled by the intersection above with dest_rect_.  Here we make
    // sure that the new current geometry rect doesn't overlap with the last.
    int min_left;
    int min_top;
    if (new_row) {
        min_left = dest_rect_.x();
        min_top = last_geometry_rect.bottom();
    } else {
        min_left = last_geometry_rect.right();
        min_top = last_geometry_rect.y();
    }

    int inset_left = std::max(0, min_left - current_geometry_rect_.x());
    int inset_top = std::max(0, min_top - current_geometry_rect_.y());
    current_geometry_rect_.Inset(inset_left, inset_top, 0, 0);

    if (!new_row) {
        DCHECK_EQ(last_geometry_rect.right(), current_geometry_rect_.x());
        DCHECK_EQ(last_geometry_rect.bottom(), current_geometry_rect_.bottom());
        DCHECK_EQ(last_geometry_rect.y(), current_geometry_rect_.y());
    }

    return *this;
}

gfx::Rect PictureLayerTiling::CoverageIterator::geometry_rect() const
{
    return current_geometry_rect_;
}

gfx::RectF PictureLayerTiling::CoverageIterator::texture_rect() const
{
    gfx::PointF tex_origin(tiling_->tiling_data_.TileBoundsWithBorder(tile_i_, tile_j_).origin());

    // Convert from dest space => content space => texture space.
    gfx::RectF texture_rect(current_geometry_rect_);
    texture_rect.Scale(dest_to_content_scale_,
        dest_to_content_scale_);
    texture_rect.Intersect(gfx::RectF(gfx::SizeF(tiling_->tiling_size())));
    if (texture_rect.IsEmpty())
        return texture_rect;
    texture_rect.Offset(-tex_origin.OffsetFromOrigin());

    return texture_rect;
}

ScopedTilePtr PictureLayerTiling::TakeTileAt(int i, int j)
{
    TileMap::iterator found = tiles_.find(TileMapKey(i, j));
    if (found == tiles_.end())
        return nullptr;
    return tiles_.take_and_erase(found);
}

bool PictureLayerTiling::RemoveTileAt(int i, int j)
{
    TileMap::iterator found = tiles_.find(TileMapKey(i, j));
    if (found == tiles_.end())
        return false;
    tiles_.erase(found);
    return true;
}

void PictureLayerTiling::Reset()
{
    live_tiles_rect_ = gfx::Rect();
    tiles_.clear();
    all_tiles_done_ = true;
}

gfx::Rect PictureLayerTiling::ComputeSkewport(
    double current_frame_time_in_seconds,
    const gfx::Rect& visible_rect_in_content_space) const
{
    gfx::Rect skewport = visible_rect_in_content_space;
    if (skewport.IsEmpty())
        return skewport;

    if (visible_rect_history_[1].frame_time_in_seconds == 0.0)
        return skewport;

    double time_delta = current_frame_time_in_seconds - visible_rect_history_[1].frame_time_in_seconds;
    if (time_delta == 0.0)
        return skewport;

    double extrapolation_multiplier = skewport_target_time_in_seconds_ / time_delta;

    int old_x = visible_rect_history_[1].visible_rect_in_content_space.x();
    int old_y = visible_rect_history_[1].visible_rect_in_content_space.y();
    int old_right = visible_rect_history_[1].visible_rect_in_content_space.right();
    int old_bottom = visible_rect_history_[1].visible_rect_in_content_space.bottom();

    int new_x = visible_rect_in_content_space.x();
    int new_y = visible_rect_in_content_space.y();
    int new_right = visible_rect_in_content_space.right();
    int new_bottom = visible_rect_in_content_space.bottom();

    // Compute the maximum skewport based on
    // |skewport_extrapolation_limit_in_content_pixels_|.
    gfx::Rect max_skewport = skewport;
    max_skewport.Inset(-skewport_extrapolation_limit_in_content_pixels_,
        -skewport_extrapolation_limit_in_content_pixels_);

    // Inset the skewport by the needed adjustment.
    skewport.Inset(extrapolation_multiplier * (new_x - old_x),
        extrapolation_multiplier * (new_y - old_y),
        extrapolation_multiplier * (old_right - new_right),
        extrapolation_multiplier * (old_bottom - new_bottom));

    // Ensure that visible rect is contained in the skewport.
    skewport.Union(visible_rect_in_content_space);

    // Clip the skewport to |max_skewport|. This needs to happen after the
    // union in case intersecting would have left the empty rect.
    skewport.Intersect(max_skewport);

    // Due to limits in int's representation, it is possible that the two
    // operations above (union and intersect) result in an empty skewport. To
    // avoid any unpleasant situations like that, union the visible rect again to
    // ensure that skewport.Contains(visible_rect_in_content_space) is always
    // true.
    skewport.Union(visible_rect_in_content_space);

    return skewport;
}

bool PictureLayerTiling::ComputeTilePriorityRects(
    const gfx::Rect& viewport_in_layer_space,
    float ideal_contents_scale,
    double current_frame_time_in_seconds,
    const Occlusion& occlusion_in_layer_space)
{
    // If we have, or had occlusions, mark the tiles as 'not done' to ensure that
    // we reiterate the tiles for rasterization.
    if (occlusion_in_layer_space.HasOcclusion() || current_occlusion_in_layer_space_.HasOcclusion()) {
        set_all_tiles_done(false);
    }

    if (!NeedsUpdateForFrameAtTimeAndViewport(current_frame_time_in_seconds,
            viewport_in_layer_space)) {
        // This should never be zero for the purposes of has_ever_been_updated().
        DCHECK_NE(current_frame_time_in_seconds, 0.0);
        return false; // weolar
    }
    gfx::Rect visible_rect_in_content_space = gfx::ScaleToEnclosingRect(viewport_in_layer_space, contents_scale_);

    if (tiling_size().IsEmpty()) {
        UpdateVisibleRectHistory(current_frame_time_in_seconds,
            visible_rect_in_content_space);
        last_viewport_in_layer_space_ = viewport_in_layer_space;
        return false;
    }

    // Calculate the skewport.
    gfx::Rect skewport = ComputeSkewport(current_frame_time_in_seconds,
        visible_rect_in_content_space);
    DCHECK(skewport.Contains(visible_rect_in_content_space));

    // Calculate the eventually/live tiles rect.
    gfx::Size tile_size = tiling_data_.max_texture_size();

    float content_to_screen_scale = ideal_contents_scale / contents_scale_;
    int pad_in_content_space = static_cast<int>(tiling_interest_area_padding_ / content_to_screen_scale);
    gfx::Rect eventually_rect = visible_rect_in_content_space;
    // If the visible rect is empty, keep the eventually rect as empty.
    if (!eventually_rect.IsEmpty()) {
        eventually_rect.Inset(-pad_in_content_space, -pad_in_content_space);
        eventually_rect = tiling_data_.ExpandRectIgnoringBordersToTileBounds(eventually_rect);
    }

    DCHECK_IMPLIES(!eventually_rect.IsEmpty(),
        gfx::Rect(tiling_size()).Contains(eventually_rect))
        << "tiling_size: " << tiling_size().ToString()
        << " eventually_rect: " << eventually_rect.ToString();

    // Calculate the soon border rect.
    gfx::Rect soon_border_rect = visible_rect_in_content_space;
    float border = CalculateSoonBorderDistance(visible_rect_in_content_space,
        content_to_screen_scale);
    soon_border_rect.Inset(-border, -border, -border, -border);

    UpdateVisibleRectHistory(current_frame_time_in_seconds,
        visible_rect_in_content_space);
    last_viewport_in_layer_space_ = viewport_in_layer_space;

    SetTilePriorityRects(content_to_screen_scale, visible_rect_in_content_space,
        skewport, soon_border_rect, eventually_rect,
        occlusion_in_layer_space);
    SetLiveTilesRect(eventually_rect);
    return true;
}

void PictureLayerTiling::SetTilePriorityRects(
    float content_to_screen_scale,
    const gfx::Rect& visible_rect_in_content_space,
    const gfx::Rect& skewport,
    const gfx::Rect& soon_border_rect,
    const gfx::Rect& eventually_rect,
    const Occlusion& occlusion_in_layer_space)
{
    current_visible_rect_ = visible_rect_in_content_space;
    current_skewport_rect_ = skewport;
    current_soon_border_rect_ = soon_border_rect;
    current_eventually_rect_ = eventually_rect;
    current_occlusion_in_layer_space_ = occlusion_in_layer_space;
    current_content_to_screen_scale_ = content_to_screen_scale;

    gfx::Rect tiling_rect(tiling_size());
    has_visible_rect_tiles_ = tiling_rect.Intersects(current_visible_rect_);
    has_skewport_rect_tiles_ = tiling_rect.Intersects(current_skewport_rect_);
    has_soon_border_rect_tiles_ = tiling_rect.Intersects(current_soon_border_rect_);
    has_eventually_rect_tiles_ = tiling_rect.Intersects(current_eventually_rect_);
}

void PictureLayerTiling::SetLiveTilesRect(
    const gfx::Rect& new_live_tiles_rect)
{
    DCHECK(new_live_tiles_rect.IsEmpty() || gfx::Rect(tiling_size()).Contains(new_live_tiles_rect))
        << "tiling_size: " << tiling_size().ToString()
        << " new_live_tiles_rect: " << new_live_tiles_rect.ToString();
    if (live_tiles_rect_ == new_live_tiles_rect)
        return;

    // Iterate to delete all tiles outside of our new live_tiles rect.
    for (TilingData::DifferenceIterator iter(&tiling_data_, live_tiles_rect_,
             new_live_tiles_rect);
         iter; ++iter) {
        RemoveTileAt(iter.index_x(), iter.index_y());
    }

    // We don't rasterize non ideal resolution tiles, so there is no need to
    // create any new tiles.
    if (resolution_ == NON_IDEAL_RESOLUTION) {
        live_tiles_rect_.Intersect(new_live_tiles_rect);
        VerifyLiveTilesRect(false);
        return;
    }

    // Iterate to allocate new tiles for all regions with newly exposed area.
    for (TilingData::DifferenceIterator iter(&tiling_data_, new_live_tiles_rect,
             live_tiles_rect_);
         iter; ++iter) {
        Tile::CreateInfo info = CreateInfoForTile(iter.index_x(), iter.index_y());
        if (ShouldCreateTileAt(info))
            CreateTile(info);
    }

    live_tiles_rect_ = new_live_tiles_rect;
    VerifyLiveTilesRect(false);
}

void PictureLayerTiling::VerifyLiveTilesRect(bool is_on_recycle_tree) const
{
#if DCHECK_IS_ON()
    for (auto it = tiles_.begin(); it != tiles_.end(); ++it) {
        if (!it->second)
            continue;
        TileMapKey key = it->first;
        DCHECK(key.index_x < tiling_data_.num_tiles_x())
            << this << " " << key.index_x << "," << key.index_y << " num_tiles_x "
            << tiling_data_.num_tiles_x() << " live_tiles_rect "
            << live_tiles_rect_.ToString();
        DCHECK(key.index_y < tiling_data_.num_tiles_y())
            << this << " " << key.index_x << "," << key.index_y << " num_tiles_y "
            << tiling_data_.num_tiles_y() << " live_tiles_rect "
            << live_tiles_rect_.ToString();
        DCHECK(tiling_data_.TileBounds(key.index_x, key.index_y)
                   .Intersects(live_tiles_rect_))
            << this << " " << key.index_x << "," << key.index_y << " tile bounds "
            << tiling_data_.TileBounds(key.index_x, key.index_y).ToString()
            << " live_tiles_rect " << live_tiles_rect_.ToString();
    }
#endif
}

bool PictureLayerTiling::IsTileOccluded(const Tile* tile) const
{
    // If this tile is not occluded on this tree, then it is not occluded.
    if (!IsTileOccludedOnCurrentTree(tile))
        return false;

    // Otherwise, if this is the pending tree, we're done and the tile is
    // occluded.
    if (tree_ == PENDING_TREE)
        return true;

    // On the active tree however, we need to check if this tile will be
    // unoccluded upon activation, in which case it has to be considered
    // unoccluded.
    const PictureLayerTiling* pending_twin = client_->GetPendingOrActiveTwinTiling(this);
    if (pending_twin) {
        // If there's a pending tile in the same position. Or if the pending twin
        // would have to be creating all tiles, then we don't need to worry about
        // occlusion on the twin.
        if (!TilingMatchesTileIndices(pending_twin) || pending_twin->TileAt(tile->tiling_i_index(), tile->tiling_j_index())) {
            return true;
        }
        return pending_twin->IsTileOccludedOnCurrentTree(tile);
    }
    return true;
}

bool PictureLayerTiling::IsTileOccludedOnCurrentTree(const Tile* tile) const
{
    if (!current_occlusion_in_layer_space_.HasOcclusion())
        return false;
    gfx::Rect tile_query_rect = gfx::IntersectRects(tile->content_rect(), current_visible_rect_);
    // Explicitly check if the tile is outside the viewport. If so, we need to
    // return false, since occlusion for this tile is unknown.
    if (tile_query_rect.IsEmpty())
        return false;

    if (contents_scale_ != 1.f) {
        tile_query_rect = gfx::ScaleToEnclosingRect(tile_query_rect, 1.f / contents_scale_);
    }
    return current_occlusion_in_layer_space_.IsOccluded(tile_query_rect);
}

bool PictureLayerTiling::IsTileRequiredForActivation(const Tile* tile) const
{
    if (tree_ == PENDING_TREE) {
        if (!can_require_tiles_for_activation_)
            return false;

        if (resolution_ != HIGH_RESOLUTION)
            return false;

        if (IsTileOccluded(tile))
            return false;

        bool tile_is_visible = tile->content_rect().Intersects(current_visible_rect_);
        if (!tile_is_visible)
            return false;

        if (client_->RequiresHighResToDraw())
            return true;

        const PictureLayerTiling* active_twin = client_->GetPendingOrActiveTwinTiling(this);
        if (!active_twin || !TilingMatchesTileIndices(active_twin))
            return true;

        if (active_twin->raster_source()->GetSize() != raster_source()->GetSize())
            return true;

        if (active_twin->current_visible_rect_ != current_visible_rect_)
            return true;

        Tile* twin_tile = active_twin->TileAt(tile->tiling_i_index(), tile->tiling_j_index());
        if (!twin_tile)
            return false;
        return true;
    }

    DCHECK_EQ(tree_, ACTIVE_TREE);
    const PictureLayerTiling* pending_twin = client_->GetPendingOrActiveTwinTiling(this);
    // If we don't have a pending tree, or the pending tree will overwrite the
    // given tile, then it is not required for activation.
    if (!pending_twin || !TilingMatchesTileIndices(pending_twin) || pending_twin->TileAt(tile->tiling_i_index(), tile->tiling_j_index())) {
        return false;
    }
    // Otherwise, ask the pending twin if this tile is required for activation.
    return pending_twin->IsTileRequiredForActivation(tile);
}

bool PictureLayerTiling::IsTileRequiredForDraw(const Tile* tile) const
{
    if (tree_ == PENDING_TREE)
        return false;

    if (resolution_ != HIGH_RESOLUTION)
        return false;

    bool tile_is_visible = current_visible_rect_.Intersects(tile->content_rect());
    if (!tile_is_visible)
        return false;

    if (IsTileOccludedOnCurrentTree(tile))
        return false;
    return true;
}

void PictureLayerTiling::UpdateRequiredStatesOnTile(Tile* tile) const
{
    DCHECK(tile);
    tile->set_required_for_activation(IsTileRequiredForActivation(tile));
    tile->set_required_for_draw(IsTileRequiredForDraw(tile));
}

PrioritizedTile PictureLayerTiling::MakePrioritizedTile(
    Tile* tile,
    PriorityRectType priority_rect_type) const
{
    DCHECK(tile);
    DCHECK(raster_source()->CoversRect(tile->enclosing_layer_rect()))
        << "Recording rect: "
        << gfx::ScaleToEnclosingRect(tile->content_rect(),
               1.f / tile->contents_scale())
               .ToString();

    return PrioritizedTile(tile, raster_source(),
        ComputePriorityForTile(tile, priority_rect_type),
        IsTileOccluded(tile));
}

std::map<const Tile*, PrioritizedTile>
PictureLayerTiling::UpdateAndGetAllPrioritizedTilesForTesting() const
{
    std::map<const Tile*, PrioritizedTile> result;
    for (const auto& key_tile_pair : tiles_) {
        Tile* tile = key_tile_pair.second;
        UpdateRequiredStatesOnTile(tile);
        PrioritizedTile prioritized_tile = MakePrioritizedTile(tile, ComputePriorityRectTypeForTile(tile));
        result.insert(std::make_pair(prioritized_tile.tile(), prioritized_tile));
    }
    return result;
}

TilePriority PictureLayerTiling::ComputePriorityForTile(
    const Tile* tile,
    PriorityRectType priority_rect_type) const
{
    // TODO(vmpstr): See if this can be moved to iterators.
    DCHECK_EQ(ComputePriorityRectTypeForTile(tile), priority_rect_type);
    DCHECK_EQ(TileAt(tile->tiling_i_index(), tile->tiling_j_index()), tile);

    TilePriority::PriorityBin priority_bin = client_->HasValidTilePriorities()
        ? TilePriority::NOW
        : TilePriority::EVENTUALLY;
    switch (priority_rect_type) {
    case VISIBLE_RECT:
        return TilePriority(resolution_, priority_bin, 0);
    case PENDING_VISIBLE_RECT:
        if (priority_bin < TilePriority::SOON)
            priority_bin = TilePriority::SOON;
        return TilePriority(resolution_, priority_bin, 0);
    case SKEWPORT_RECT:
    case SOON_BORDER_RECT:
        if (priority_bin < TilePriority::SOON)
            priority_bin = TilePriority::SOON;
        break;
    case EVENTUALLY_RECT:
        priority_bin = TilePriority::EVENTUALLY;
        break;
    }

    gfx::Rect tile_bounds = tiling_data_.TileBounds(tile->tiling_i_index(), tile->tiling_j_index());
    DCHECK_GT(current_content_to_screen_scale_, 0.f);
    float distance_to_visible = current_visible_rect_.ManhattanInternalDistance(tile_bounds) * current_content_to_screen_scale_;

    return TilePriority(resolution_, priority_bin, distance_to_visible);
}

PictureLayerTiling::PriorityRectType
PictureLayerTiling::ComputePriorityRectTypeForTile(const Tile* tile) const
{
    DCHECK_EQ(TileAt(tile->tiling_i_index(), tile->tiling_j_index()), tile);
    gfx::Rect tile_bounds = tiling_data_.TileBounds(tile->tiling_i_index(), tile->tiling_j_index());

    if (current_visible_rect_.Intersects(tile_bounds))
        return VISIBLE_RECT;

    if (pending_visible_rect().Intersects(tile_bounds))
        return PENDING_VISIBLE_RECT;

    if (current_skewport_rect_.Intersects(tile_bounds))
        return SKEWPORT_RECT;

    if (current_soon_border_rect_.Intersects(tile_bounds))
        return SOON_BORDER_RECT;

    DCHECK(current_eventually_rect_.Intersects(tile_bounds));
    return EVENTUALLY_RECT;
}

void PictureLayerTiling::GetAllPrioritizedTilesForTracing(
    std::vector<PrioritizedTile>* prioritized_tiles) const
{
    for (const auto& tile_pair : tiles_) {
        Tile* tile = tile_pair.second;
        prioritized_tiles->push_back(
            MakePrioritizedTile(tile, ComputePriorityRectTypeForTile(tile)));
    }
}

void PictureLayerTiling::AsValueInto(
    base::trace_event::TracedValue* state) const
{
    state->SetInteger("num_tiles", base::saturated_cast<int>(tiles_.size()));
    state->SetDouble("content_scale", contents_scale_);
    MathUtil::AddToTracedValue("visible_rect", current_visible_rect_, state);
    MathUtil::AddToTracedValue("skewport_rect", current_skewport_rect_, state);
    MathUtil::AddToTracedValue("soon_rect", current_soon_border_rect_, state);
    MathUtil::AddToTracedValue("eventually_rect", current_eventually_rect_,
        state);
    MathUtil::AddToTracedValue("tiling_size", tiling_size(), state);
}

size_t PictureLayerTiling::GPUMemoryUsageInBytes() const
{
    size_t amount = 0;
    for (TileMap::const_iterator it = tiles_.begin(); it != tiles_.end(); ++it) {
        const Tile* tile = it->second;
        amount += tile->GPUMemoryUsageInBytes();
    }
    return amount;
}

} // namespace cc
