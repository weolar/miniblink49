// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_PICTURE_LAYER_IMPL_H_
#define CC_LAYERS_PICTURE_LAYER_IMPL_H_

#include <map>
#include <string>
#include <vector>

#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/layers/layer_impl.h"
#include "cc/tiles/picture_layer_tiling.h"
#include "cc/tiles/picture_layer_tiling_set.h"
#include "cc/tiles/tiling_set_eviction_queue.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkPicture.h"

namespace cc {

struct AppendQuadsData;
class MicroBenchmarkImpl;
class Tile;

class CC_EXPORT PictureLayerImpl
    : public LayerImpl,
      NON_EXPORTED_BASE(public PictureLayerTilingClient) {
public:
    static scoped_ptr<PictureLayerImpl> Create(
        LayerTreeImpl* tree_impl,
        int id,
        bool is_mask,
        scoped_refptr<SyncedScrollOffset> scroll_offset)
    {
        return make_scoped_ptr(
            new PictureLayerImpl(tree_impl, id, is_mask, scroll_offset));
    }
    ~PictureLayerImpl() override;

    bool is_mask() const { return is_mask_; }

    // LayerImpl overrides.
    const char* LayerTypeAsString() const override;
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void PushPropertiesTo(LayerImpl* layer) override;
    void AppendQuads(RenderPass* render_pass,
        AppendQuadsData* append_quads_data) override;
    void NotifyTileStateChanged(const Tile* tile) override;
    void DidBeginTracing() override;
    void ReleaseResources() override;
    void RecreateResources() override;
    skia::RefPtr<SkPicture> GetPicture() override;
    Region GetInvalidationRegion() override;

    // PictureLayerTilingClient overrides.
    ScopedTilePtr CreateTile(const Tile::CreateInfo& info) override;
    gfx::Size CalculateTileSize(const gfx::Size& content_bounds) const override;
    const Region* GetPendingInvalidation() override;
    const PictureLayerTiling* GetPendingOrActiveTwinTiling(
        const PictureLayerTiling* tiling) const override;
    bool HasValidTilePriorities() const override;
    bool RequiresHighResToDraw() const override;
    gfx::Rect GetEnclosingRectInTargetSpace() const override;

    void set_gpu_raster_max_texture_size(gfx::Size gpu_raster_max_texture_size)
    {
        gpu_raster_max_texture_size_ = gpu_raster_max_texture_size;
    }
    void UpdateRasterSource(scoped_refptr<RasterSource> raster_source,
        Region* new_invalidation,
        const PictureLayerTilingSet* pending_set);
    bool UpdateTiles(bool resourceless_software_draw);
    void UpdateCanUseLCDTextAfterCommit();
    bool RasterSourceUsesLCDText() const;
    WhichTree GetTree() const;

    // Mask-related functions.
    void GetContentsResourceId(ResourceId* resource_id,
        gfx::Size* resource_size) const override;

    void SetNearestNeighbor(bool nearest_neighbor);

    size_t GPUMemoryUsageInBytes() const override;

    void RunMicroBenchmark(MicroBenchmarkImpl* benchmark) override;

    bool CanHaveTilings() const;

    PictureLayerTilingSet* picture_layer_tiling_set() { return tilings_.get(); }

    // Functions used by tile manager.
    PictureLayerImpl* GetPendingOrActiveTwinLayer() const;
    bool IsOnActiveOrPendingTree() const;

    // Used for benchmarking
    RasterSource* GetRasterSource() const { return raster_source_.get(); }

protected:
    friend class LayerRasterTileIterator;
    using TileRequirementCheck = bool (PictureLayerTiling::*)(const Tile*) const;

    PictureLayerImpl(LayerTreeImpl* tree_impl,
        int id,
        bool is_mask,
        scoped_refptr<SyncedScrollOffset> scroll_offset);
    PictureLayerTiling* AddTiling(float contents_scale);
    void RemoveAllTilings();
    void AddTilingsForRasterScale();
    void AddLowResolutionTilingIfNeeded();
    virtual bool ShouldAdjustRasterScale() const;
    virtual void RecalculateRasterScales();
    void CleanUpTilingsOnActiveLayer(
        const std::vector<PictureLayerTiling*>& used_tilings);
    float MinimumContentsScale() const;
    float MaximumContentsScale() const;
    void ResetRasterScale();
    void UpdateViewportRectForTilePriorityInContentSpace();
    PictureLayerImpl* GetRecycledTwinLayer() const;

    void SanityCheckTilingState() const;
    bool ShouldAdjustRasterScaleDuringScaleAnimations() const;

    void GetDebugBorderProperties(SkColor* color, float* width) const override;
    void GetAllPrioritizedTilesForTracing(
        std::vector<PrioritizedTile>* prioritized_tiles) const override;
    //void AsValueInto(base::trace_event::TracedValue* dict) const override;

    virtual void UpdateIdealScales();
    float MaximumTilingContentsScale() const;
    scoped_ptr<PictureLayerTilingSet> CreatePictureLayerTilingSet();

    PictureLayerImpl* twin_layer_;

    scoped_ptr<PictureLayerTilingSet> tilings_;
    scoped_refptr<RasterSource> raster_source_;
    Region invalidation_;

    float ideal_page_scale_;
    float ideal_device_scale_;
    float ideal_source_scale_;
    float ideal_contents_scale_;

    float raster_page_scale_;
    float raster_device_scale_;
    float raster_source_scale_;
    float raster_contents_scale_;
    float low_res_raster_contents_scale_;

    bool raster_source_scale_is_fixed_;
    bool was_screen_space_transform_animating_;
    bool only_used_low_res_last_append_quads_;
    const bool is_mask_;

    bool nearest_neighbor_;

    // Any draw properties derived from |transform|, |viewport|, and |clip|
    // parameters in LayerTreeHostImpl::SetExternalDrawConstraints are not valid
    // for prioritizing tiles during resourceless software draws. This is because
    // resourceless software draws can have wildly different transforms/viewports
    // from regular draws. Save a copy of the required draw properties of the last
    // frame that has a valid viewport for prioritizing tiles.
    gfx::Rect visible_rect_for_tile_priority_;
    gfx::Rect viewport_rect_for_tile_priority_in_content_space_;

    gfx::Size gpu_raster_max_texture_size_;

    // List of tilings that were used last time we appended quads. This can be
    // used as an optimization not to remove tilings if they are still being
    // drawn. Note that accessing this vector should only be done in the context
    // of comparing pointers, since objects pointed to are not guaranteed to
    // exist.
    std::vector<PictureLayerTiling*> last_append_quads_tilings_;

    DISALLOW_COPY_AND_ASSIGN(PictureLayerImpl);
};

} // namespace cc

#endif // CC_LAYERS_PICTURE_LAYER_IMPL_H_
