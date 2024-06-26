// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TILES_PICTURE_LAYER_TILING_SET_H_
#define CC_TILES_PICTURE_LAYER_TILING_SET_H_

#include <set>
#include <vector>

#include "cc/base/region.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/tiles/picture_layer_tiling.h"
#include "ui/gfx/geometry/size.h"

namespace base {
namespace trace_event {
    class TracedValue;
}
}

namespace cc {

class CC_EXPORT PictureLayerTilingSet {
public:
    enum TilingRangeType {
        HIGHER_THAN_HIGH_RES,
        HIGH_RES,
        BETWEEN_HIGH_AND_LOW_RES,
        LOW_RES,
        LOWER_THAN_LOW_RES
    };
    struct TilingRange {
        TilingRange(size_t start, size_t end)
            : start(start)
            , end(end)
        {
        }

        size_t start;
        size_t end;
    };

    static scoped_ptr<PictureLayerTilingSet> Create(
        WhichTree tree,
        PictureLayerTilingClient* client,
        size_t tiling_interest_area_padding,
        float skewport_target_time_in_seconds,
        int skewport_extrapolation_limit_in_content);

    ~PictureLayerTilingSet();

    const PictureLayerTilingClient* client() const { return client_; }

    void CleanUpTilings(float min_acceptable_high_res_scale,
        float max_acceptable_high_res_scale,
        const std::vector<PictureLayerTiling*>& needed_tilings,
        PictureLayerTilingSet* twin_set);
    void RemoveNonIdealTilings();

    // This function is called on the active tree during activation.
    void UpdateTilingsToCurrentRasterSourceForActivation(
        scoped_refptr<RasterSource> raster_source,
        const PictureLayerTilingSet* pending_twin_set,
        const Region& layer_invalidation,
        float minimum_contents_scale,
        float maximum_contents_scale);

    // This function is called on the sync tree during commit.
    void UpdateTilingsToCurrentRasterSourceForCommit(
        scoped_refptr<RasterSource> raster_source,
        const Region& layer_invalidation,
        float minimum_contents_scale,
        float maximum_contents_scale);

    // This function is called on the sync tree right after commit.
    void UpdateRasterSourceDueToLCDChange(
        const scoped_refptr<RasterSource>& raster_source,
        const Region& layer_invalidation);

    PictureLayerTiling* AddTiling(float contents_scale,
        scoped_refptr<RasterSource> raster_source);
    size_t num_tilings() const { return tilings_.size(); }
    int NumHighResTilings() const;
    PictureLayerTiling* tiling_at(size_t idx) { return tilings_[idx]; }
    const PictureLayerTiling* tiling_at(size_t idx) const
    {
        return tilings_[idx];
    }
    WhichTree tree() const { return tree_; }

    PictureLayerTiling* FindTilingWithScale(float scale) const;
    PictureLayerTiling* FindTilingWithResolution(TileResolution resolution) const;

    void MarkAllTilingsNonIdeal();

    // If a tiling exists whose scale is within |snap_to_existing_tiling_ratio|
    // ratio of |start_scale|, then return that tiling's scale. Otherwise, return
    // |start_scale|. If multiple tilings match the criteria, return the one with
    // the least ratio to |start_scale|.
    float GetSnappedContentsScale(float start_scale,
        float snap_to_existing_tiling_ratio) const;

    // Returns the maximum contents scale of all tilings, or 0 if no tilings
    // exist.
    float GetMaximumContentsScale() const;

    // Removes all tilings with a contents scale < |minimum_scale|.
    void RemoveTilingsBelowScale(float minimum_scale);

    // Removes all tilings with a contents scale > |maximum_scale|.
    void RemoveTilingsAboveScale(float maximum_scale);

    // Remove all tilings.
    void RemoveAllTilings();

    // Remove all tiles; keep all tilings.
    void RemoveAllTiles();

    // Update the rects and priorities for tiles based on the given information.
    bool UpdateTilePriorities(const gfx::Rect& required_rect_in_layer_space,
        float ideal_contents_scale,
        double current_frame_time_in_seconds,
        const Occlusion& occlusion_in_layer_space,
        bool can_require_tiles_for_activation);

    void GetAllPrioritizedTilesForTracing(
        std::vector<PrioritizedTile>* prioritized_tiles) const;

    // For a given rect, iterates through tiles that can fill it.  If no
    // set of tiles with resources can fill the rect, then it will iterate
    // through null tiles with valid geometry_rect() until the rect is full.
    // If all tiles have resources, the union of all geometry_rects will
    // exactly fill rect with no overlap.
    class CC_EXPORT CoverageIterator {
    public:
        CoverageIterator(const PictureLayerTilingSet* set,
            float contents_scale,
            const gfx::Rect& content_rect,
            float ideal_contents_scale);
        ~CoverageIterator();

        // Visible rect (no borders), always in the space of rect,
        // regardless of the relative contents scale of the tiling.
        gfx::Rect geometry_rect() const;
        // Texture rect (in texels) for geometry_rect
        gfx::RectF texture_rect() const;

        Tile* operator->() const;
        Tile* operator*() const;

        CoverageIterator& operator++();
        operator bool() const;

        TileResolution resolution() const;
        PictureLayerTiling* CurrentTiling() const;

    private:
        size_t NextTiling() const;

        const PictureLayerTilingSet* set_;
        float contents_scale_;
        float ideal_contents_scale_;
        PictureLayerTiling::CoverageIterator tiling_iter_;
        size_t current_tiling_;
        size_t ideal_tiling_;

        Region current_region_;
        Region missing_region_;
        Region::Iterator region_iter_;
    };

    void AsValueInto(base::trace_event::TracedValue* array) const;
    size_t GPUMemoryUsageInBytes() const;

    TilingRange GetTilingRange(TilingRangeType type) const;

private:
    explicit PictureLayerTilingSet(
        WhichTree tree,
        PictureLayerTilingClient* client,
        size_t tiling_interest_area_padding,
        float skewport_target_time_in_seconds,
        int skewport_extrapolation_limit_in_content_pixels);

    void CopyTilingsAndPropertiesFromPendingTwin(
        const PictureLayerTilingSet* pending_twin_set,
        const scoped_refptr<RasterSource>& raster_source,
        const Region& layer_invalidation);

    // Remove one tiling.
    void Remove(PictureLayerTiling* tiling);
    void VerifyTilings(const PictureLayerTilingSet* pending_twin_set) const;

    ScopedPtrVector<PictureLayerTiling> tilings_;

    const size_t tiling_interest_area_padding_;
    const float skewport_target_time_in_seconds_;
    const int skewport_extrapolation_limit_in_content_pixels_;
    WhichTree tree_;
    PictureLayerTilingClient* client_;

    friend class Iterator;
    DISALLOW_COPY_AND_ASSIGN(PictureLayerTilingSet);
};

} // namespace cc

#endif // CC_TILES_PICTURE_LAYER_TILING_SET_H_
