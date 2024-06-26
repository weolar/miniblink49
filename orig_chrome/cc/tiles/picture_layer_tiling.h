// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TILES_PICTURE_LAYER_TILING_H_
#define CC_TILES_PICTURE_LAYER_TILING_H_

#include <map>
#include <utility>
#include <vector>

#include "base/basictypes.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/base/region.h"
#include "cc/base/tiling_data.h"
#include "cc/tiles/tile.h"
#include "cc/tiles/tile_priority.h"
#include "cc/trees/occlusion.h"
#include "ui/gfx/geometry/rect.h"

namespace base {
namespace trace_event {
    class TracedValue;
}
}

namespace cc {

class PictureLayerTiling;
class PrioritizedTile;
class RasterSource;

class CC_EXPORT PictureLayerTilingClient {
public:
    // Create a tile at the given content_rect (in the contents scale of the
    // tiling) This might return null if the client cannot create such a tile.
    virtual ScopedTilePtr CreateTile(const Tile::CreateInfo& info) = 0;
    virtual gfx::Size CalculateTileSize(
        const gfx::Size& content_bounds) const = 0;
    // This invalidation region defines the area (if any, it can by null) that
    // tiles can not be shared between pending and active trees.
    virtual const Region* GetPendingInvalidation() = 0;
    virtual const PictureLayerTiling* GetPendingOrActiveTwinTiling(
        const PictureLayerTiling* tiling) const = 0;
    virtual bool HasValidTilePriorities() const = 0;
    virtual bool RequiresHighResToDraw() const = 0;

protected:
    virtual ~PictureLayerTilingClient() { }
};

struct TileMapKey {
    TileMapKey(int x, int y)
        : index_x(x)
        , index_y(y)
    {
    }
    explicit TileMapKey(const std::pair<int, int>& index)
        : index_x(index.first)
        , index_y(index.second)
    {
    }

    bool operator==(const TileMapKey& other) const
    {
        return index_x == other.index_x && index_y == other.index_y;
    }

    bool operator<(const TileMapKey& other) const
    {
        uint16 value1_x = static_cast<uint16>(index_x);
        uint16 value1_y = static_cast<uint16>(index_y);
        uint32 value1_32 = value1_x;
        value1_32 = (value1_32 << 16) | value1_y;

        uint16 value2_x = static_cast<uint16>(other.index_x);
        uint16 value2_y = static_cast<uint16>(other.index_y);
        uint32 value2_32 = value2_x;
        value2_32 = (value2_32 << 16) | value2_y;

        return value1_32 < value2_32;
    }

    int index_x;
    int index_y;
};

} // namespace cc

namespace BASE_HASH_NAMESPACE {

#if USING_VC6RT != 1
template <>
struct hash<cc::TileMapKey> {
    size_t operator()(const cc::TileMapKey& key) const
    {
        uint16 value1 = static_cast<uint16>(key.index_x);
        uint16 value2 = static_cast<uint16>(key.index_y);
        uint32 value1_32 = value1;
        return (value1_32 << 16) | value2;
    }
};
#endif

} // namespace BASE_HASH_NAMESPACE

namespace cc {

class CC_EXPORT PictureLayerTiling {
public:
    static const int kBorderTexels = 1;

    PictureLayerTilingClient* client() const { return client_; }
    ~PictureLayerTiling();

    static float CalculateSoonBorderDistance(
        const gfx::Rect& visible_rect_in_content_space,
        float content_to_screen_scale);

    // Create a tiling with no tiles. CreateTile() must be called to add some.
    static scoped_ptr<PictureLayerTiling> Create(
        WhichTree tree,
        float contents_scale,
        scoped_refptr<RasterSource> raster_source,
        PictureLayerTilingClient* client,
        size_t tiling_interest_area_padding,
        float skewport_target_time_in_seconds,
        int skewport_extrapolation_limit_in_content_pixels);

    void SetRasterSourceAndResize(scoped_refptr<RasterSource> raster_source);
    void Invalidate(const Region& layer_invalidation);
    void CreateMissingTilesInLiveTilesRect();
    void TakeTilesAndPropertiesFrom(PictureLayerTiling* pending_twin,
        const Region& layer_invalidation);

    bool IsTileRequiredForActivation(const Tile* tile) const;
    bool IsTileRequiredForDraw(const Tile* tile) const;

    void set_resolution(TileResolution resolution)
    {
        resolution_ = resolution;
        may_contain_low_resolution_tiles_ |= resolution == LOW_RESOLUTION;
    }
    TileResolution resolution() const { return resolution_; }
    bool may_contain_low_resolution_tiles() const
    {
        return may_contain_low_resolution_tiles_;
    }
    void reset_may_contain_low_resolution_tiles()
    {
        may_contain_low_resolution_tiles_ = false;
    }
    void set_can_require_tiles_for_activation(bool can_require_tiles)
    {
        can_require_tiles_for_activation_ = can_require_tiles;
    }

    RasterSource* raster_source() const { return raster_source_.get(); }
    gfx::Size tiling_size() const { return tiling_data_.tiling_size(); }
    gfx::Rect live_tiles_rect() const { return live_tiles_rect_; }
    gfx::Size tile_size() const { return tiling_data_.max_texture_size(); }
    float contents_scale() const { return contents_scale_; }
    const TilingData* tiling_data() const { return &tiling_data_; }

    Tile* TileAt(int i, int j) const
    {
        TileMap::const_iterator iter = tiles_.find(TileMapKey(i, j));
        return iter == tiles_.end() ? nullptr : iter->second;
    }

    bool has_tiles() const { return !tiles_.empty(); }
    // all_tiles_done() can return false negatives.
    bool all_tiles_done() const { return all_tiles_done_; }
    void set_all_tiles_done(bool all_tiles_done)
    {
        all_tiles_done_ = all_tiles_done;
    }

    void VerifyNoTileNeedsRaster() const
    {
#if DCHECK_IS_ON()
        for (const auto tile_pair : tiles_) {
            DCHECK(!tile_pair.second->draw_info().NeedsRaster() || IsTileOccluded(tile_pair.second));
        }
#endif // DCHECK_IS_ON()
    }

    // For testing functionality.
    void CreateAllTilesForTesting()
    {
        SetLiveTilesRect(gfx::Rect(tiling_data_.tiling_size()));
    }
    const TilingData& TilingDataForTesting() const { return tiling_data_; }
    std::vector<Tile*> AllTilesForTesting() const
    {
        std::vector<Tile*> all_tiles;
        for (TileMap::const_iterator it = tiles_.begin(); it != tiles_.end(); ++it)
            all_tiles.push_back(it->second);
        return all_tiles;
    }

    void UpdateAllRequiredStateForTesting()
    {
        for (const auto& key_tile_pair : tiles_)
            UpdateRequiredStatesOnTile(key_tile_pair.second);
    }
    std::map<const Tile*, PrioritizedTile>
    UpdateAndGetAllPrioritizedTilesForTesting() const;

    void SetAllTilesOccludedForTesting()
    {
        gfx::Rect viewport_in_layer_space = ScaleToEnclosingRect(current_visible_rect_, 1.0f / contents_scale_);
        current_occlusion_in_layer_space_ = Occlusion(gfx::Transform(),
            SimpleEnclosedRegion(viewport_in_layer_space),
            SimpleEnclosedRegion(viewport_in_layer_space));
    }
    const gfx::Rect& GetCurrentVisibleRectForTesting() const
    {
        return current_visible_rect_;
    }
    void SetTilePriorityRectsForTesting(
        const gfx::Rect& visible_rect_in_content_space,
        const gfx::Rect& skewport,
        const gfx::Rect& soon_border_rect,
        const gfx::Rect& eventually_rect)
    {
        SetTilePriorityRects(1.0f, visible_rect_in_content_space, skewport,
            soon_border_rect, eventually_rect, Occlusion());
    }

    // Iterate over all tiles to fill content_rect.  Even if tiles are invalid
    // (i.e. no valid resource) this tiling should still iterate over them.
    // The union of all geometry_rect calls for each element iterated over should
    // exactly equal content_rect and no two geometry_rects should intersect.
    class CC_EXPORT CoverageIterator {
    public:
        CoverageIterator();
        CoverageIterator(const PictureLayerTiling* tiling,
            float dest_scale,
            const gfx::Rect& rect);
        ~CoverageIterator();

        // Visible rect (no borders), always in the space of content_rect,
        // regardless of the contents scale of the tiling.
        gfx::Rect geometry_rect() const;
        // Texture rect (in texels) for geometry_rect
        gfx::RectF texture_rect() const;

        Tile* operator->() const { return current_tile_; }
        Tile* operator*() const { return current_tile_; }

        CoverageIterator& operator++();
        operator bool() const { return tile_j_ <= bottom_; }

        int i() const { return tile_i_; }
        int j() const { return tile_j_; }

    private:
        const PictureLayerTiling* tiling_;
        gfx::Rect dest_rect_;
        float dest_to_content_scale_;

        Tile* current_tile_;
        gfx::Rect current_geometry_rect_;
        int tile_i_;
        int tile_j_;
        int left_;
        int top_;
        int right_;
        int bottom_;

        friend class PictureLayerTiling;
    };

    void Reset();

    bool ComputeTilePriorityRects(const gfx::Rect& viewport_in_layer_space,
        float ideal_contents_scale,
        double current_frame_time_in_seconds,
        const Occlusion& occlusion_in_layer_space);

    void GetAllPrioritizedTilesForTracing(
        std::vector<PrioritizedTile>* prioritized_tiles) const;
    void AsValueInto(base::trace_event::TracedValue* array) const;
    size_t GPUMemoryUsageInBytes() const;

protected:
    friend class CoverageIterator;
    friend class PrioritizedTile;
    friend class TilingSetRasterQueueAll;
    friend class TilingSetRasterQueueRequired;
    friend class TilingSetEvictionQueue;

    // PENDING VISIBLE RECT refers to the visible rect that will become current
    // upon activation (ie, the pending tree's visible rect). Tiles in this
    // region that are not part of the current visible rect are all handled
    // here. Note that when processing a pending tree, this rect is the same as
    // the visible rect so no tiles are processed in this case.
    enum PriorityRectType {
        VISIBLE_RECT,
        PENDING_VISIBLE_RECT,
        SKEWPORT_RECT,
        SOON_BORDER_RECT,
        EVENTUALLY_RECT
    };

    using TileMap = base::ScopedPtrHashMap<TileMapKey, ScopedTilePtr>;

    struct FrameVisibleRect {
        gfx::Rect visible_rect_in_content_space;
        double frame_time_in_seconds = 0.0;
    };

    PictureLayerTiling(WhichTree tree,
        float contents_scale,
        scoped_refptr<RasterSource> raster_source,
        PictureLayerTilingClient* client,
        size_t tiling_interest_area_padding,
        float skewport_target_time_in_seconds,
        int skewport_extrapolation_limit_in_content_pixels);
    void SetLiveTilesRect(const gfx::Rect& live_tiles_rect);
    void VerifyLiveTilesRect(bool is_on_recycle_tree) const;
    Tile* CreateTile(const Tile::CreateInfo& info);
    ScopedTilePtr TakeTileAt(int i, int j);
    // Returns true if the Tile existed and was removed from the tiling.
    bool RemoveTileAt(int i, int j);
    bool TilingMatchesTileIndices(const PictureLayerTiling* twin) const;

    // Computes a skewport. The calculation extrapolates the last visible
    // rect and the current visible rect to expand the skewport to where it
    // would be in |skewport_target_time| seconds. Note that the skewport
    // is guaranteed to contain the current visible rect.
    gfx::Rect ComputeSkewport(double current_frame_time_in_seconds,
        const gfx::Rect& visible_rect_in_content_space)
        const;

    // Save the required data for computing tile priorities later.
    void SetTilePriorityRects(float content_to_screen_scale_,
        const gfx::Rect& visible_rect_in_content_space,
        const gfx::Rect& skewport,
        const gfx::Rect& soon_border_rect,
        const gfx::Rect& eventually_rect,
        const Occlusion& occlusion_in_layer_space);

    bool NeedsUpdateForFrameAtTimeAndViewport(
        double frame_time_in_seconds,
        const gfx::Rect& viewport_in_layer_space)
    {
        return frame_time_in_seconds != visible_rect_history_[0].frame_time_in_seconds || viewport_in_layer_space != last_viewport_in_layer_space_;
    }
    void UpdateVisibleRectHistory(
        double frame_time_in_seconds,
        const gfx::Rect& visible_rect_in_content_space)
    {
        visible_rect_history_[1] = visible_rect_history_[0];
        visible_rect_history_[0].frame_time_in_seconds = frame_time_in_seconds;
        visible_rect_history_[0].visible_rect_in_content_space = visible_rect_in_content_space;
        // If we don't have a second history item, set it to the most recent one.
        if (visible_rect_history_[1].frame_time_in_seconds == 0.0)
            visible_rect_history_[1] = visible_rect_history_[0];
    }
    bool IsTileOccludedOnCurrentTree(const Tile* tile) const;
    Tile::CreateInfo CreateInfoForTile(int i, int j) const;
    bool ShouldCreateTileAt(const Tile::CreateInfo& info) const;
    bool IsTileOccluded(const Tile* tile) const;
    void UpdateRequiredStatesOnTile(Tile* tile) const;
    PrioritizedTile MakePrioritizedTile(
        Tile* tile,
        PriorityRectType priority_rect_type) const;
    TilePriority ComputePriorityForTile(
        const Tile* tile,
        PriorityRectType priority_rect_type) const;
    PriorityRectType ComputePriorityRectTypeForTile(const Tile* tile) const;
    bool has_visible_rect_tiles() const { return has_visible_rect_tiles_; }
    bool has_skewport_rect_tiles() const { return has_skewport_rect_tiles_; }
    bool has_soon_border_rect_tiles() const
    {
        return has_soon_border_rect_tiles_;
    }
    bool has_eventually_rect_tiles() const { return has_eventually_rect_tiles_; }

    const gfx::Rect& current_visible_rect() const
    {
        return current_visible_rect_;
    }
    gfx::Rect pending_visible_rect() const
    {
        const PictureLayerTiling* pending_tiling = tree_ == ACTIVE_TREE ? client_->GetPendingOrActiveTwinTiling(this)
                                                                        : this;
        if (pending_tiling)
            return pending_tiling->current_visible_rect();
        return gfx::Rect();
    }
    const gfx::Rect& current_skewport_rect() const
    {
        return current_skewport_rect_;
    }
    const gfx::Rect& current_soon_border_rect() const
    {
        return current_soon_border_rect_;
    }
    const gfx::Rect& current_eventually_rect() const
    {
        return current_eventually_rect_;
    }
    bool has_ever_been_updated() const
    {
        return visible_rect_history_[0].frame_time_in_seconds != 0.0;
    }
    void RemoveTilesInRegion(const Region& layer_region, bool recreate_tiles);

    const size_t tiling_interest_area_padding_;
    const float skewport_target_time_in_seconds_;
    const int skewport_extrapolation_limit_in_content_pixels_;

    // Given properties.
    const float contents_scale_;
    PictureLayerTilingClient* const client_;
    const WhichTree tree_;
    scoped_refptr<RasterSource> raster_source_;
    TileResolution resolution_;
    bool may_contain_low_resolution_tiles_;

    // Internal data.
    TilingData tiling_data_;
    TileMap tiles_; // It is not legal to have a NULL tile in the tiles_ map.
    gfx::Rect live_tiles_rect_;

    gfx::Rect last_viewport_in_layer_space_;
    // State saved for computing velocities based upon finite differences.
    FrameVisibleRect visible_rect_history_[2];

    bool can_require_tiles_for_activation_;

    // Iteration rects in content space.
    gfx::Rect current_visible_rect_;
    gfx::Rect current_skewport_rect_;
    gfx::Rect current_soon_border_rect_;
    gfx::Rect current_eventually_rect_;
    // Other properties used for tile iteration and prioritization.
    float current_content_to_screen_scale_;
    Occlusion current_occlusion_in_layer_space_;

    bool has_visible_rect_tiles_;
    bool has_skewport_rect_tiles_;
    bool has_soon_border_rect_tiles_;
    bool has_eventually_rect_tiles_;
    bool all_tiles_done_;

private:
    DISALLOW_ASSIGN(PictureLayerTiling);
};

} // namespace cc

#endif // CC_TILES_PICTURE_LAYER_TILING_H_
