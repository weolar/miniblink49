// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_PICTURE_LAYER_IMPL_H_
#define CC_TEST_FAKE_PICTURE_LAYER_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "cc/layers/picture_layer_impl.h"
#include "cc/playback/raster_source.h"

namespace cc {

class FakePictureLayerImpl : public PictureLayerImpl {
public:
    static scoped_ptr<FakePictureLayerImpl> Create(
        LayerTreeImpl* tree_impl, int id)
    {
        bool is_mask = false;
        return make_scoped_ptr(new FakePictureLayerImpl(tree_impl, id, is_mask));
    }

    static scoped_ptr<FakePictureLayerImpl> CreateMask(LayerTreeImpl* tree_impl,
        int id)
    {
        bool is_mask = true;
        return make_scoped_ptr(new FakePictureLayerImpl(tree_impl, id, is_mask));
    }

    // Create layer from a raster source that covers the entire layer.
    static scoped_ptr<FakePictureLayerImpl> CreateWithRasterSource(
        LayerTreeImpl* tree_impl,
        int id,
        scoped_refptr<RasterSource> raster_source)
    {
        bool is_mask = false;
        return make_scoped_ptr(
            new FakePictureLayerImpl(tree_impl, id, raster_source, is_mask));
    }

    // Create layer from a raster source that only covers part of the layer.
    static scoped_ptr<FakePictureLayerImpl> CreateWithPartialRasterSource(
        LayerTreeImpl* tree_impl,
        int id,
        scoped_refptr<RasterSource> raster_source,
        const gfx::Size& layer_bounds)
    {
        bool is_mask = false;
        return make_scoped_ptr(new FakePictureLayerImpl(
            tree_impl, id, raster_source, is_mask, layer_bounds));
    }

    // Create layer from a raster source that covers the entire layer and is a
    // mask.
    static scoped_ptr<FakePictureLayerImpl> CreateMaskWithRasterSource(
        LayerTreeImpl* tree_impl,
        int id,
        scoped_refptr<RasterSource> raster_source)
    {
        bool is_mask = true;
        return make_scoped_ptr(
            new FakePictureLayerImpl(tree_impl, id, raster_source, is_mask));
    }

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void PushPropertiesTo(LayerImpl* layer_impl) override;
    void AppendQuads(RenderPass* render_pass,
        AppendQuadsData* append_quads_data) override;
    gfx::Size CalculateTileSize(const gfx::Size& content_bounds) const override;

    void DidBecomeActive() override;
    size_t did_become_active_call_count()
    {
        return did_become_active_call_count_;
    }

    bool HasValidTilePriorities() const override;
    void set_has_valid_tile_priorities(bool has_valid_priorities)
    {
        has_valid_tile_priorities_ = has_valid_priorities;
        use_set_valid_tile_priorities_flag_ = true;
    }

    size_t CountTilesRequired(
        TileRequirementCheck is_tile_required_callback) const;
    size_t CountTilesRequiredForActivation() const;
    size_t CountTilesRequiredForDraw() const;

    using PictureLayerImpl::AddTiling;
    using PictureLayerImpl::CanHaveTilings;
    using PictureLayerImpl::CleanUpTilingsOnActiveLayer;
    using PictureLayerImpl::MinimumContentsScale;
    using PictureLayerImpl::SanityCheckTilingState;
    using PictureLayerImpl::UpdateRasterSource;

    using PictureLayerImpl::MaximumTilingContentsScale;
    using PictureLayerImpl::UpdateIdealScales;

    void AddTilingUntilNextDraw(float scale)
    {
        last_append_quads_tilings_.push_back(AddTiling(scale));
    }

    float raster_page_scale() const { return raster_page_scale_; }
    void set_raster_page_scale(float scale) { raster_page_scale_ = scale; }

    float ideal_contents_scale() const { return ideal_contents_scale_; }
    float raster_contents_scale() const { return raster_contents_scale_; }

    PictureLayerTiling* HighResTiling() const;
    PictureLayerTiling* LowResTiling() const;
    size_t num_tilings() const { return tilings_->num_tilings(); }

    PictureLayerTilingSet* tilings() { return tilings_.get(); }
    RasterSource* raster_source() { return raster_source_.get(); }
    void SetRasterSourceOnPending(scoped_refptr<RasterSource> raster_source,
        const Region& invalidation);
    size_t append_quads_count() { return append_quads_count_; }

    const Region& invalidation() const { return invalidation_; }
    void set_invalidation(const Region& region) { invalidation_ = region; }

    gfx::Rect visible_rect_for_tile_priority()
    {
        return visible_rect_for_tile_priority_;
    }

    gfx::Rect viewport_rect_for_tile_priority_in_content_space()
    {
        return viewport_rect_for_tile_priority_in_content_space_;
    }

    void set_fixed_tile_size(const gfx::Size& size) { fixed_tile_size_ = size; }

    void SetIsDrawnRenderSurfaceLayerListMember(bool is);

    void CreateAllTiles();
    void SetAllTilesReady();
    void SetAllTilesReadyInTiling(PictureLayerTiling* tiling);
    void SetTileReady(Tile* tile);
    PictureLayerTilingSet* GetTilings() { return tilings_.get(); }

    // Add the given tiling as a "used" tiling during AppendQuads. This ensures
    // that future calls to UpdateTiles don't delete the tiling.
    void MarkAllTilingsUsed()
    {
        last_append_quads_tilings_.clear();
        for (size_t i = 0; i < tilings_->num_tilings(); ++i)
            last_append_quads_tilings_.push_back(tilings_->tiling_at(i));
    }

    size_t release_resources_count() const { return release_resources_count_; }
    void reset_release_resources_count() { release_resources_count_ = 0; }

    void ReleaseResources() override;

    bool only_used_low_res_last_append_quads() const
    {
        return only_used_low_res_last_append_quads_;
    }

protected:
    FakePictureLayerImpl(LayerTreeImpl* tree_impl,
        int id,
        scoped_refptr<RasterSource> raster_source,
        bool is_mask);
    FakePictureLayerImpl(LayerTreeImpl* tree_impl,
        int id,
        scoped_refptr<RasterSource> raster_source,
        bool is_mask,
        const gfx::Size& layer_bounds);
    FakePictureLayerImpl(LayerTreeImpl* tree_impl, int id, bool is_mask);
    FakePictureLayerImpl(
        LayerTreeImpl* tree_impl,
        int id,
        bool is_mask,
        scoped_refptr<LayerImpl::SyncedScrollOffset> synced_scroll_offset);

private:
    gfx::Size fixed_tile_size_;

    size_t append_quads_count_;
    size_t did_become_active_call_count_;
    bool has_valid_tile_priorities_;
    bool use_set_valid_tile_priorities_flag_;
    size_t release_resources_count_;
};

} // namespace cc

#endif // CC_TEST_FAKE_PICTURE_LAYER_IMPL_H_
