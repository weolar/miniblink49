// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits>
#include <set>

#include "cc/base/math_util.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_picture_layer_tiling_client.h"
#include "cc/test/test_context_provider.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/tiles/picture_layer_tiling.h"
#include "cc/tiles/picture_layer_tiling_set.h"
#include "cc/trees/layer_tree_settings.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/quad_f.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {
namespace {

    static gfx::Rect ViewportInLayerSpace(
        const gfx::Transform& transform,
        const gfx::Size& device_viewport)
    {

        gfx::Transform inverse;
        if (!transform.GetInverse(&inverse))
            return gfx::Rect();

        return MathUtil::ProjectEnclosingClippedRect(inverse,
            gfx::Rect(device_viewport));
    }

    class TestablePictureLayerTiling : public PictureLayerTiling {
    public:
        using PictureLayerTiling::SetLiveTilesRect;
        using PictureLayerTiling::TileAt;

        static scoped_ptr<TestablePictureLayerTiling> Create(
            WhichTree tree,
            float contents_scale,
            scoped_refptr<RasterSource> raster_source,
            PictureLayerTilingClient* client,
            const LayerTreeSettings& settings)
        {
            return make_scoped_ptr(new TestablePictureLayerTiling(
                tree, contents_scale, raster_source, client,
                settings.tiling_interest_area_padding,
                settings.skewport_target_time_in_seconds,
                settings.skewport_extrapolation_limit_in_content_pixels));
        }

        gfx::Rect live_tiles_rect() const { return live_tiles_rect_; }

        using PictureLayerTiling::ComputeSkewport;
        using PictureLayerTiling::RemoveTileAt;
        using PictureLayerTiling::RemoveTilesInRegion;

    protected:
        TestablePictureLayerTiling(WhichTree tree,
            float contents_scale,
            scoped_refptr<RasterSource> raster_source,
            PictureLayerTilingClient* client,
            size_t tiling_interest_area_padding,
            float skewport_target_time,
            int skewport_extrapolation_limit)
            : PictureLayerTiling(tree,
                contents_scale,
                raster_source,
                client,
                tiling_interest_area_padding,
                skewport_target_time,
                skewport_extrapolation_limit)
        {
        }
    };

    class PictureLayerTilingIteratorTest : public testing::Test {
    public:
        PictureLayerTilingIteratorTest() { }
        ~PictureLayerTilingIteratorTest() override { }

        void Initialize(const gfx::Size& tile_size,
            float contents_scale,
            const gfx::Size& layer_bounds)
        {
            client_.SetTileSize(tile_size);
            scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
            tiling_ = TestablePictureLayerTiling::Create(PENDING_TREE, contents_scale,
                raster_source, &client_,
                LayerTreeSettings());
            tiling_->set_resolution(HIGH_RESOLUTION);
        }

        void InitializeActive(const gfx::Size& tile_size,
            float contents_scale,
            const gfx::Size& layer_bounds)
        {
            client_.SetTileSize(tile_size);
            scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
            tiling_ = TestablePictureLayerTiling::Create(ACTIVE_TREE, contents_scale,
                raster_source, &client_,
                LayerTreeSettings());
            tiling_->set_resolution(HIGH_RESOLUTION);
        }

        void SetLiveRectAndVerifyTiles(const gfx::Rect& live_tiles_rect)
        {
            tiling_->SetLiveTilesRect(live_tiles_rect);

            std::vector<Tile*> tiles = tiling_->AllTilesForTesting();
            for (std::vector<Tile*>::iterator iter = tiles.begin();
                 iter != tiles.end();
                 ++iter) {
                EXPECT_TRUE(live_tiles_rect.Intersects((*iter)->content_rect()));
            }
        }

        void VerifyTilesExactlyCoverRect(
            float rect_scale,
            const gfx::Rect& request_rect,
            const gfx::Rect& expect_rect)
        {
            EXPECT_TRUE(request_rect.Contains(expect_rect));

            // Iterators are not valid if this ratio is too large (i.e. the
            // tiling is too high-res for a low-res destination rect.)  This is an
            // artifact of snapping geometry to integer coordinates and then mapping
            // back to floating point texture coordinates.
            float dest_to_contents_scale = tiling_->contents_scale() / rect_scale;
            ASSERT_LE(dest_to_contents_scale, 2.0);

            Region remaining = expect_rect;
            for (PictureLayerTiling::CoverageIterator
                     iter(tiling_.get(), rect_scale, request_rect);
                 iter;
                 ++iter) {
                // Geometry cannot overlap previous geometry at all
                gfx::Rect geometry = iter.geometry_rect();
                EXPECT_TRUE(expect_rect.Contains(geometry));
                EXPECT_TRUE(remaining.Contains(geometry));
                remaining.Subtract(geometry);

                // Sanity check that texture coords are within the texture rect.
                gfx::RectF texture_rect = iter.texture_rect();
                EXPECT_GE(texture_rect.x(), 0);
                EXPECT_GE(texture_rect.y(), 0);
                EXPECT_LE(texture_rect.right(), client_.TileSize().width());
                EXPECT_LE(texture_rect.bottom(), client_.TileSize().height());
            }

            // The entire rect must be filled by geometry from the tiling.
            EXPECT_TRUE(remaining.IsEmpty());
        }

        void VerifyTilesExactlyCoverRect(float rect_scale, const gfx::Rect& rect)
        {
            VerifyTilesExactlyCoverRect(rect_scale, rect, rect);
        }

        void VerifyTiles(
            float rect_scale,
            const gfx::Rect& rect,
            base::Callback<void(Tile* tile,
                const gfx::Rect& geometry_rect)>
                callback)
        {
            VerifyTiles(tiling_.get(),
                rect_scale,
                rect,
                callback);
        }

        void VerifyTiles(
            PictureLayerTiling* tiling,
            float rect_scale,
            const gfx::Rect& rect,
            base::Callback<void(Tile* tile,
                const gfx::Rect& geometry_rect)>
                callback)
        {
            Region remaining = rect;
            for (PictureLayerTiling::CoverageIterator iter(tiling, rect_scale, rect);
                 iter;
                 ++iter) {
                remaining.Subtract(iter.geometry_rect());
                callback.Run(*iter, iter.geometry_rect());
            }
            EXPECT_TRUE(remaining.IsEmpty());
        }

        void VerifyTilesCoverNonContainedRect(float rect_scale,
            const gfx::Rect& dest_rect)
        {
            float dest_to_contents_scale = tiling_->contents_scale() / rect_scale;
            gfx::Rect clamped_rect = gfx::ScaleToEnclosingRect(
                gfx::Rect(tiling_->tiling_size()), 1.f / dest_to_contents_scale);
            clamped_rect.Intersect(dest_rect);
            VerifyTilesExactlyCoverRect(rect_scale, dest_rect, clamped_rect);
        }

    protected:
        FakePictureLayerTilingClient client_;
        scoped_ptr<TestablePictureLayerTiling> tiling_;

    private:
        DISALLOW_COPY_AND_ASSIGN(PictureLayerTilingIteratorTest);
    };

    TEST_F(PictureLayerTilingIteratorTest, ResizeDeletesTiles)
    {
        // Verifies that a resize with invalidation for newly exposed pixels will
        // deletes tiles that intersect that invalidation.
        gfx::Size tile_size(100, 100);
        gfx::Size original_layer_size(10, 10);
        InitializeActive(tile_size, 1.f, original_layer_size);
        SetLiveRectAndVerifyTiles(gfx::Rect(original_layer_size));

        // Tiling only has one tile, since its total size is less than one.
        EXPECT_TRUE(tiling_->TileAt(0, 0));

        // Stop creating tiles so that any invalidations are left as holes.
        gfx::Size new_layer_size(200, 200);
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreatePartiallyFilled(new_layer_size,
            gfx::Rect());

        Region invalidation = SubtractRegions(gfx::Rect(tile_size), gfx::Rect(original_layer_size));
        tiling_->SetRasterSourceAndResize(raster_source);
        EXPECT_TRUE(tiling_->TileAt(0, 0));
        tiling_->Invalidate(invalidation);
        EXPECT_FALSE(tiling_->TileAt(0, 0));
    }

    TEST_F(PictureLayerTilingIteratorTest, CreateMissingTilesStaysInsideLiveRect)
    {
        // The tiling has three rows and columns.
        Initialize(gfx::Size(100, 100), 1.f, gfx::Size(250, 250));
        EXPECT_EQ(3, tiling_->TilingDataForTesting().num_tiles_x());
        EXPECT_EQ(3, tiling_->TilingDataForTesting().num_tiles_y());

        // The live tiles rect is at the very edge of the right-most and
        // bottom-most tiles. Their border pixels would still be inside the live
        // tiles rect, but the tiles should not exist just for that.
        int right = tiling_->TilingDataForTesting().TileBounds(2, 2).x();
        int bottom = tiling_->TilingDataForTesting().TileBounds(2, 2).y();

        SetLiveRectAndVerifyTiles(gfx::Rect(right, bottom));
        EXPECT_FALSE(tiling_->TileAt(2, 0));
        EXPECT_FALSE(tiling_->TileAt(2, 1));
        EXPECT_FALSE(tiling_->TileAt(2, 2));
        EXPECT_FALSE(tiling_->TileAt(1, 2));
        EXPECT_FALSE(tiling_->TileAt(0, 2));

        // Verify CreateMissingTilesInLiveTilesRect respects this.
        tiling_->CreateMissingTilesInLiveTilesRect();
        EXPECT_FALSE(tiling_->TileAt(2, 0));
        EXPECT_FALSE(tiling_->TileAt(2, 1));
        EXPECT_FALSE(tiling_->TileAt(2, 2));
        EXPECT_FALSE(tiling_->TileAt(1, 2));
        EXPECT_FALSE(tiling_->TileAt(0, 2));
    }

    TEST_F(PictureLayerTilingIteratorTest, ResizeTilingOverTileBorders)
    {
        // The tiling has four rows and three columns.
        Initialize(gfx::Size(100, 100), 1.f, gfx::Size(250, 350));
        EXPECT_EQ(3, tiling_->TilingDataForTesting().num_tiles_x());
        EXPECT_EQ(4, tiling_->TilingDataForTesting().num_tiles_y());

        // The live tiles rect covers the whole tiling.
        SetLiveRectAndVerifyTiles(gfx::Rect(250, 350));

        // Tiles in the bottom row and right column exist.
        EXPECT_TRUE(tiling_->TileAt(2, 0));
        EXPECT_TRUE(tiling_->TileAt(2, 1));
        EXPECT_TRUE(tiling_->TileAt(2, 2));
        EXPECT_TRUE(tiling_->TileAt(2, 3));
        EXPECT_TRUE(tiling_->TileAt(1, 3));
        EXPECT_TRUE(tiling_->TileAt(0, 3));

        int right = tiling_->TilingDataForTesting().TileBounds(2, 2).x();
        int bottom = tiling_->TilingDataForTesting().TileBounds(2, 3).y();

        // Shrink the tiling so that the last tile row/column is entirely in the
        // border pixels of the interior tiles. That row/column is removed.
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(
            gfx::Size(right + 1, bottom + 1));
        tiling_->SetRasterSourceAndResize(raster_source);
        EXPECT_EQ(2, tiling_->TilingDataForTesting().num_tiles_x());
        EXPECT_EQ(3, tiling_->TilingDataForTesting().num_tiles_y());

        // The live tiles rect was clamped to the raster source size.
        EXPECT_EQ(gfx::Rect(right + 1, bottom + 1), tiling_->live_tiles_rect());

        // Since the row/column is gone, the tiles should be gone too.
        EXPECT_FALSE(tiling_->TileAt(2, 0));
        EXPECT_FALSE(tiling_->TileAt(2, 1));
        EXPECT_FALSE(tiling_->TileAt(2, 2));
        EXPECT_FALSE(tiling_->TileAt(2, 3));
        EXPECT_FALSE(tiling_->TileAt(1, 3));
        EXPECT_FALSE(tiling_->TileAt(0, 3));

        // Growing outside the current right/bottom tiles border pixels should create
        // the tiles again, even though the live rect has not changed size.
        raster_source = FakeDisplayListRasterSource::CreateFilled(
            gfx::Size(right + 2, bottom + 2));
        tiling_->SetRasterSourceAndResize(raster_source);
        EXPECT_EQ(3, tiling_->TilingDataForTesting().num_tiles_x());
        EXPECT_EQ(4, tiling_->TilingDataForTesting().num_tiles_y());

        // Not changed.
        EXPECT_EQ(gfx::Rect(right + 1, bottom + 1), tiling_->live_tiles_rect());

        // The last row/column tiles are inside the live tiles rect.
        EXPECT_TRUE(gfx::Rect(right + 1, bottom + 1).Intersects(tiling_->TilingDataForTesting().TileBounds(2, 0)));
        EXPECT_TRUE(gfx::Rect(right + 1, bottom + 1).Intersects(tiling_->TilingDataForTesting().TileBounds(0, 3)));

        EXPECT_TRUE(tiling_->TileAt(2, 0));
        EXPECT_TRUE(tiling_->TileAt(2, 1));
        EXPECT_TRUE(tiling_->TileAt(2, 2));
        EXPECT_TRUE(tiling_->TileAt(2, 3));
        EXPECT_TRUE(tiling_->TileAt(1, 3));
        EXPECT_TRUE(tiling_->TileAt(0, 3));
    }

    TEST_F(PictureLayerTilingIteratorTest, ResizeLiveTileRectOverTileBorders)
    {
        // The tiling has three rows and columns.
        Initialize(gfx::Size(100, 100), 1.f, gfx::Size(250, 350));
        EXPECT_EQ(3, tiling_->TilingDataForTesting().num_tiles_x());
        EXPECT_EQ(4, tiling_->TilingDataForTesting().num_tiles_y());

        // The live tiles rect covers the whole tiling.
        SetLiveRectAndVerifyTiles(gfx::Rect(250, 350));

        // Tiles in the bottom row and right column exist.
        EXPECT_TRUE(tiling_->TileAt(2, 0));
        EXPECT_TRUE(tiling_->TileAt(2, 1));
        EXPECT_TRUE(tiling_->TileAt(2, 2));
        EXPECT_TRUE(tiling_->TileAt(2, 3));
        EXPECT_TRUE(tiling_->TileAt(1, 3));
        EXPECT_TRUE(tiling_->TileAt(0, 3));

        // Shrink the live tiles rect to the very edge of the right-most and
        // bottom-most tiles. Their border pixels would still be inside the live
        // tiles rect, but the tiles should not exist just for that.
        int right = tiling_->TilingDataForTesting().TileBounds(2, 3).x();
        int bottom = tiling_->TilingDataForTesting().TileBounds(2, 3).y();

        SetLiveRectAndVerifyTiles(gfx::Rect(right, bottom));
        EXPECT_FALSE(tiling_->TileAt(2, 0));
        EXPECT_FALSE(tiling_->TileAt(2, 1));
        EXPECT_FALSE(tiling_->TileAt(2, 2));
        EXPECT_FALSE(tiling_->TileAt(2, 3));
        EXPECT_FALSE(tiling_->TileAt(1, 3));
        EXPECT_FALSE(tiling_->TileAt(0, 3));

        // Including the bottom row and right column again, should create the tiles.
        SetLiveRectAndVerifyTiles(gfx::Rect(right + 1, bottom + 1));
        EXPECT_TRUE(tiling_->TileAt(2, 0));
        EXPECT_TRUE(tiling_->TileAt(2, 1));
        EXPECT_TRUE(tiling_->TileAt(2, 2));
        EXPECT_TRUE(tiling_->TileAt(2, 3));
        EXPECT_TRUE(tiling_->TileAt(1, 2));
        EXPECT_TRUE(tiling_->TileAt(0, 2));

        // Shrink the live tiles rect to the very edge of the left-most and
        // top-most tiles. Their border pixels would still be inside the live
        // tiles rect, but the tiles should not exist just for that.
        int left = tiling_->TilingDataForTesting().TileBounds(0, 0).right();
        int top = tiling_->TilingDataForTesting().TileBounds(0, 0).bottom();

        SetLiveRectAndVerifyTiles(gfx::Rect(left, top, 250 - left, 350 - top));
        EXPECT_FALSE(tiling_->TileAt(0, 3));
        EXPECT_FALSE(tiling_->TileAt(0, 2));
        EXPECT_FALSE(tiling_->TileAt(0, 1));
        EXPECT_FALSE(tiling_->TileAt(0, 0));
        EXPECT_FALSE(tiling_->TileAt(1, 0));
        EXPECT_FALSE(tiling_->TileAt(2, 0));

        // Including the top row and left column again, should create the tiles.
        SetLiveRectAndVerifyTiles(
            gfx::Rect(left - 1, top - 1, 250 - left, 350 - top));
        EXPECT_TRUE(tiling_->TileAt(0, 3));
        EXPECT_TRUE(tiling_->TileAt(0, 2));
        EXPECT_TRUE(tiling_->TileAt(0, 1));
        EXPECT_TRUE(tiling_->TileAt(0, 0));
        EXPECT_TRUE(tiling_->TileAt(1, 0));
        EXPECT_TRUE(tiling_->TileAt(2, 0));
    }

    TEST_F(PictureLayerTilingIteratorTest, ResizeLiveTileRectOverSameTiles)
    {
        // The tiling has four rows and three columns.
        Initialize(gfx::Size(100, 100), 1.f, gfx::Size(250, 350));
        EXPECT_EQ(3, tiling_->TilingDataForTesting().num_tiles_x());
        EXPECT_EQ(4, tiling_->TilingDataForTesting().num_tiles_y());

        // The live tiles rect covers the whole tiling.
        SetLiveRectAndVerifyTiles(gfx::Rect(250, 350));

        // All tiles exist.
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j)
                EXPECT_TRUE(tiling_->TileAt(i, j)) << i << "," << j;
        }

        // Shrink the live tiles rect, but still cover all the tiles.
        SetLiveRectAndVerifyTiles(gfx::Rect(1, 1, 249, 349));

        // All tiles still exist.
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j)
                EXPECT_TRUE(tiling_->TileAt(i, j)) << i << "," << j;
        }

        // Grow the live tiles rect, but still cover all the same tiles.
        SetLiveRectAndVerifyTiles(gfx::Rect(0, 0, 250, 350));

        // All tiles still exist.
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j)
                EXPECT_TRUE(tiling_->TileAt(i, j)) << i << "," << j;
        }
    }

    TEST_F(PictureLayerTilingIteratorTest, ResizeOverBorderPixelsDeletesTiles)
    {
        // Verifies that a resize with invalidation for newly exposed pixels will
        // deletes tiles that intersect that invalidation.
        gfx::Size tile_size(100, 100);
        gfx::Size original_layer_size(99, 99);
        InitializeActive(tile_size, 1.f, original_layer_size);
        SetLiveRectAndVerifyTiles(gfx::Rect(original_layer_size));

        // Tiling only has one tile, since its total size is less than one.
        EXPECT_TRUE(tiling_->TileAt(0, 0));

        // Stop creating tiles so that any invalidations are left as holes.
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreatePartiallyFilled(gfx::Size(200, 200),
            gfx::Rect());
        tiling_->SetRasterSourceAndResize(raster_source);

        Region invalidation = SubtractRegions(gfx::Rect(tile_size), gfx::Rect(original_layer_size));
        EXPECT_TRUE(tiling_->TileAt(0, 0));
        tiling_->Invalidate(invalidation);
        EXPECT_FALSE(tiling_->TileAt(0, 0));

        // The original tile was the same size after resize, but it would include new
        // border pixels.
        EXPECT_EQ(gfx::Rect(original_layer_size),
            tiling_->TilingDataForTesting().TileBounds(0, 0));
    }

    TEST_F(PictureLayerTilingIteratorTest, RemoveOutsideLayerKeepsTiles)
    {
        gfx::Size tile_size(100, 100);
        gfx::Size layer_size(100, 100);
        InitializeActive(tile_size, 1.f, layer_size);
        SetLiveRectAndVerifyTiles(gfx::Rect(layer_size));

        // In all cases here, the tiling should remain with one tile, since the remove
        // region doesn't intersect it.

        bool recreate_tiles = false;
        // Top
        tiling_->RemoveTilesInRegion(gfx::Rect(50, -1, 1, 1), recreate_tiles);
        EXPECT_TRUE(tiling_->TileAt(0, 0));
        // Bottom
        tiling_->RemoveTilesInRegion(gfx::Rect(50, 100, 1, 1), recreate_tiles);
        EXPECT_TRUE(tiling_->TileAt(0, 0));
        // Left
        tiling_->RemoveTilesInRegion(gfx::Rect(-1, 50, 1, 1), recreate_tiles);
        EXPECT_TRUE(tiling_->TileAt(0, 0));
        // Right
        tiling_->RemoveTilesInRegion(gfx::Rect(100, 50, 1, 1), recreate_tiles);
        EXPECT_TRUE(tiling_->TileAt(0, 0));
    }

    TEST_F(PictureLayerTilingIteratorTest, LiveTilesExactlyCoverLiveTileRect)
    {
        Initialize(gfx::Size(100, 100), 1.f, gfx::Size(1099, 801));
        SetLiveRectAndVerifyTiles(gfx::Rect(100, 100));
        SetLiveRectAndVerifyTiles(gfx::Rect(101, 99));
        SetLiveRectAndVerifyTiles(gfx::Rect(1099, 1));
        SetLiveRectAndVerifyTiles(gfx::Rect(1, 801));
        SetLiveRectAndVerifyTiles(gfx::Rect(1099, 1));
        SetLiveRectAndVerifyTiles(gfx::Rect(201, 800));
    }

    TEST_F(PictureLayerTilingIteratorTest, IteratorCoversLayerBoundsNoScale)
    {
        Initialize(gfx::Size(100, 100), 1.f, gfx::Size(1099, 801));
        VerifyTilesExactlyCoverRect(1, gfx::Rect());
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 1099, 801));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(52, 83, 789, 412));

        // With borders, a size of 3x3 = 1 pixel of content.
        Initialize(gfx::Size(3, 3), 1.f, gfx::Size(10, 10));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 1, 1));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 2, 2));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(1, 1, 2, 2));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(3, 2, 5, 2));
    }

    TEST_F(PictureLayerTilingIteratorTest, IteratorCoversLayerBoundsTilingScale)
    {
        Initialize(gfx::Size(200, 100), 2.0f, gfx::Size(1005, 2010));
        VerifyTilesExactlyCoverRect(1, gfx::Rect());
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 1005, 2010));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(50, 112, 512, 381));

        Initialize(gfx::Size(3, 3), 2.0f, gfx::Size(10, 10));
        VerifyTilesExactlyCoverRect(1, gfx::Rect());
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 1, 1));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 2, 2));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(1, 1, 2, 2));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(3, 2, 5, 2));

        Initialize(gfx::Size(100, 200), 0.5f, gfx::Size(1005, 2010));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 1005, 2010));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(50, 112, 512, 381));

        Initialize(gfx::Size(150, 250), 0.37f, gfx::Size(1005, 2010));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 1005, 2010));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(50, 112, 512, 381));

        Initialize(gfx::Size(312, 123), 0.01f, gfx::Size(1005, 2010));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(0, 0, 1005, 2010));
        VerifyTilesExactlyCoverRect(1, gfx::Rect(50, 112, 512, 381));
    }

    TEST_F(PictureLayerTilingIteratorTest, IteratorCoversLayerBoundsBothScale)
    {
        Initialize(gfx::Size(50, 50), 4.0f, gfx::Size(800, 600));
        VerifyTilesExactlyCoverRect(2.0f, gfx::Rect());
        VerifyTilesExactlyCoverRect(2.0f, gfx::Rect(0, 0, 1600, 1200));
        VerifyTilesExactlyCoverRect(2.0f, gfx::Rect(512, 365, 253, 182));

        float scale = 6.7f;
        gfx::Size bounds(800, 600);
        gfx::Rect full_rect(gfx::ScaleToCeiledSize(bounds, scale));
        Initialize(gfx::Size(256, 512), 5.2f, bounds);
        VerifyTilesExactlyCoverRect(scale, full_rect);
        VerifyTilesExactlyCoverRect(scale, gfx::Rect(2014, 1579, 867, 1033));
    }

    TEST_F(PictureLayerTilingIteratorTest, IteratorEmptyRect)
    {
        Initialize(gfx::Size(100, 100), 1.0f, gfx::Size(800, 600));

        gfx::Rect empty;
        PictureLayerTiling::CoverageIterator iter(tiling_.get(), 1.0f, empty);
        EXPECT_FALSE(iter);
    }

    TEST_F(PictureLayerTilingIteratorTest, NonIntersectingRect)
    {
        Initialize(gfx::Size(100, 100), 1.0f, gfx::Size(800, 600));
        gfx::Rect non_intersecting(1000, 1000, 50, 50);
        PictureLayerTiling::CoverageIterator iter(tiling_.get(), 1, non_intersecting);
        EXPECT_FALSE(iter);
    }

    TEST_F(PictureLayerTilingIteratorTest, LayerEdgeTextureCoordinates)
    {
        Initialize(gfx::Size(300, 300), 1.0f, gfx::Size(256, 256));
        // All of these sizes are 256x256, scaled and ceiled.
        VerifyTilesExactlyCoverRect(1.0f, gfx::Rect(0, 0, 256, 256));
        VerifyTilesExactlyCoverRect(0.8f, gfx::Rect(0, 0, 205, 205));
        VerifyTilesExactlyCoverRect(1.2f, gfx::Rect(0, 0, 308, 308));
    }

    TEST_F(PictureLayerTilingIteratorTest, NonContainedDestRect)
    {
        Initialize(gfx::Size(100, 100), 1.0f, gfx::Size(400, 400));

        // Too large in all dimensions
        VerifyTilesCoverNonContainedRect(1.0f, gfx::Rect(-1000, -1000, 2000, 2000));
        VerifyTilesCoverNonContainedRect(1.5f, gfx::Rect(-1000, -1000, 2000, 2000));
        VerifyTilesCoverNonContainedRect(0.5f, gfx::Rect(-1000, -1000, 2000, 2000));

        // Partially covering content, but too large
        VerifyTilesCoverNonContainedRect(1.0f, gfx::Rect(-1000, 100, 2000, 100));
        VerifyTilesCoverNonContainedRect(1.5f, gfx::Rect(-1000, 100, 2000, 100));
        VerifyTilesCoverNonContainedRect(0.5f, gfx::Rect(-1000, 100, 2000, 100));
    }

    TEST(PictureLayerTilingTest, SkewportLimits)
    {
        FakePictureLayerTilingClient client;

        gfx::Rect viewport(0, 0, 100, 100);
        gfx::Size layer_bounds(200, 200);

        client.SetTileSize(gfx::Size(100, 100));
        LayerTreeSettings settings;
        settings.skewport_extrapolation_limit_in_content_pixels = 75;

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, settings);

        tiling->ComputeTilePriorityRects(viewport, 1.f, 1.0, Occlusion());

        // Move viewport down 50 pixels in 0.5 seconds.
        gfx::Rect down_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(0, 50, 100, 100));

        EXPECT_EQ(0, down_skewport.x());
        EXPECT_EQ(50, down_skewport.y());
        EXPECT_EQ(100, down_skewport.width());
        EXPECT_EQ(175, down_skewport.height());
        EXPECT_TRUE(down_skewport.Contains(gfx::Rect(0, 50, 100, 100)));

        // Move viewport down 50 and right 10 pixels.
        gfx::Rect down_right_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(10, 50, 100, 100));

        EXPECT_EQ(10, down_right_skewport.x());
        EXPECT_EQ(50, down_right_skewport.y());
        EXPECT_EQ(120, down_right_skewport.width());
        EXPECT_EQ(175, down_right_skewport.height());
        EXPECT_TRUE(down_right_skewport.Contains(gfx::Rect(10, 50, 100, 100)));

        // Move viewport left.
        gfx::Rect left_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(-50, 0, 100, 100));

        EXPECT_EQ(-125, left_skewport.x());
        EXPECT_EQ(0, left_skewport.y());
        EXPECT_EQ(175, left_skewport.width());
        EXPECT_EQ(100, left_skewport.height());
        EXPECT_TRUE(left_skewport.Contains(gfx::Rect(-50, 0, 100, 100)));

        // Expand viewport.
        gfx::Rect expand_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(-50, -50, 200, 200));

        // x and y moved by -75 (-50 - 75 = -125).
        // right side and bottom side moved by 75 [(350 - 125) - (200 - 50) = 75].
        EXPECT_EQ(-125, expand_skewport.x());
        EXPECT_EQ(-125, expand_skewport.y());
        EXPECT_EQ(350, expand_skewport.width());
        EXPECT_EQ(350, expand_skewport.height());
        EXPECT_TRUE(expand_skewport.Contains(gfx::Rect(-50, -50, 200, 200)));

        // Expand the viewport past the limit in all directions.
        gfx::Rect big_expand_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(-500, -500, 1500, 1500));

        EXPECT_EQ(-575, big_expand_skewport.x());
        EXPECT_EQ(-575, big_expand_skewport.y());
        EXPECT_EQ(1650, big_expand_skewport.width());
        EXPECT_EQ(1650, big_expand_skewport.height());
        EXPECT_TRUE(big_expand_skewport.Contains(gfx::Rect(-500, -500, 1500, 1500)));

        // Shrink the skewport in all directions.
        gfx::Rect shrink_viewport = tiling->ComputeSkewport(1.5, gfx::Rect(0, 0, 100, 100));
        EXPECT_EQ(0, shrink_viewport.x());
        EXPECT_EQ(0, shrink_viewport.y());
        EXPECT_EQ(100, shrink_viewport.width());
        EXPECT_EQ(100, shrink_viewport.height());

        // Move the skewport really far in one direction.
        gfx::Rect move_skewport_far = tiling->ComputeSkewport(1.5, gfx::Rect(0, 5000, 100, 100));
        EXPECT_EQ(0, move_skewport_far.x());
        EXPECT_EQ(5000, move_skewport_far.y());
        EXPECT_EQ(100, move_skewport_far.width());
        EXPECT_EQ(175, move_skewport_far.height());
        EXPECT_TRUE(move_skewport_far.Contains(gfx::Rect(0, 5000, 100, 100)));
    }

    TEST(PictureLayerTilingTest, ComputeSkewportExtremeCases)
    {
        FakePictureLayerTilingClient client;

        gfx::Size layer_bounds(200, 200);
        client.SetTileSize(gfx::Size(100, 100));
        LayerTreeSettings settings;
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, settings);

        gfx::Rect viewport1(-1918, 255860, 4010, 2356);
        gfx::Rect viewport2(-7088, -91738, 14212, 8350);
        gfx::Rect viewport3(-12730024, -158883296, 24607540, 14454512);
        double time = 1.0;
        tiling->ComputeTilePriorityRects(viewport1, 1.f, time, Occlusion());
        time += 0.016;
        EXPECT_TRUE(tiling->ComputeSkewport(time, viewport2).Contains(viewport2));
        tiling->ComputeTilePriorityRects(viewport2, 1.f, time, Occlusion());
        time += 0.016;
        EXPECT_TRUE(tiling->ComputeSkewport(time, viewport3).Contains(viewport3));
    }

    TEST(PictureLayerTilingTest, ComputeSkewport)
    {
        FakePictureLayerTilingClient client;

        gfx::Rect viewport(0, 0, 100, 100);
        gfx::Size layer_bounds(200, 200);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());

        tiling->ComputeTilePriorityRects(viewport, 1.f, 1.0, Occlusion());

        // Move viewport down 50 pixels in 0.5 seconds.
        gfx::Rect down_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(0, 50, 100, 100));

        EXPECT_EQ(0, down_skewport.x());
        EXPECT_EQ(50, down_skewport.y());
        EXPECT_EQ(100, down_skewport.width());
        EXPECT_EQ(200, down_skewport.height());

        // Shrink viewport.
        gfx::Rect shrink_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(25, 25, 50, 50));

        EXPECT_EQ(25, shrink_skewport.x());
        EXPECT_EQ(25, shrink_skewport.y());
        EXPECT_EQ(50, shrink_skewport.width());
        EXPECT_EQ(50, shrink_skewport.height());

        // Move viewport down 50 and right 10 pixels.
        gfx::Rect down_right_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(10, 50, 100, 100));

        EXPECT_EQ(10, down_right_skewport.x());
        EXPECT_EQ(50, down_right_skewport.y());
        EXPECT_EQ(120, down_right_skewport.width());
        EXPECT_EQ(200, down_right_skewport.height());

        // Move viewport left.
        gfx::Rect left_skewport = tiling->ComputeSkewport(1.5, gfx::Rect(-20, 0, 100, 100));

        EXPECT_EQ(-60, left_skewport.x());
        EXPECT_EQ(0, left_skewport.y());
        EXPECT_EQ(140, left_skewport.width());
        EXPECT_EQ(100, left_skewport.height());

        // Expand viewport in 0.2 seconds.
        gfx::Rect expanded_skewport = tiling->ComputeSkewport(1.2, gfx::Rect(-5, -5, 110, 110));

        EXPECT_EQ(-30, expanded_skewport.x());
        EXPECT_EQ(-30, expanded_skewport.y());
        EXPECT_EQ(160, expanded_skewport.width());
        EXPECT_EQ(160, expanded_skewport.height());
    }

    TEST(PictureLayerTilingTest, SkewportThroughUpdateTilePriorities)
    {
        FakePictureLayerTilingClient client;

        gfx::Rect viewport(0, 0, 100, 100);
        gfx::Size layer_bounds(200, 200);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());

        tiling->ComputeTilePriorityRects(viewport, 1.f, 1.0, Occlusion());

        // Move viewport down 50 pixels in 0.5 seconds.
        gfx::Rect viewport_50 = gfx::Rect(0, 50, 100, 100);
        gfx::Rect skewport_50 = tiling->ComputeSkewport(1.5, viewport_50);

        EXPECT_EQ(gfx::Rect(0, 50, 100, 200), skewport_50);
        tiling->ComputeTilePriorityRects(viewport_50, 1.f, 1.5, Occlusion());

        gfx::Rect viewport_100 = gfx::Rect(0, 100, 100, 100);
        gfx::Rect skewport_100 = tiling->ComputeSkewport(2.0, viewport_100);

        EXPECT_EQ(gfx::Rect(0, 100, 100, 200), skewport_100);
        tiling->ComputeTilePriorityRects(viewport_100, 1.f, 2.0, Occlusion());

        // Advance time, but not the viewport.
        gfx::Rect result = tiling->ComputeSkewport(2.5, viewport_100);
        // Since the history did advance, we should still get a skewport but a smaller
        // one.
        EXPECT_EQ(gfx::Rect(0, 100, 100, 150), result);
        tiling->ComputeTilePriorityRects(viewport_100, 1.f, 2.5, Occlusion());

        // Advance time again.
        result = tiling->ComputeSkewport(3.0, viewport_100);
        EXPECT_EQ(viewport_100, result);
        tiling->ComputeTilePriorityRects(viewport_100, 1.f, 3.0, Occlusion());

        // Ensure we have a skewport.
        gfx::Rect viewport_150 = gfx::Rect(0, 150, 100, 100);
        gfx::Rect skewport_150 = tiling->ComputeSkewport(3.5, viewport_150);
        EXPECT_EQ(gfx::Rect(0, 150, 100, 150), skewport_150);
        tiling->ComputeTilePriorityRects(viewport_150, 1.f, 3.5, Occlusion());

        // Advance the viewport, but not the time.
        gfx::Rect viewport_200 = gfx::Rect(0, 200, 100, 100);
        gfx::Rect skewport_200 = tiling->ComputeSkewport(3.5, viewport_200);
        EXPECT_EQ(gfx::Rect(0, 200, 100, 300), skewport_200);

        // Ensure that continued calls with the same value, produce the same skewport.
        tiling->ComputeTilePriorityRects(viewport_150, 1.f, 3.5, Occlusion());
        EXPECT_EQ(gfx::Rect(0, 200, 100, 300), skewport_200);
        tiling->ComputeTilePriorityRects(viewport_150, 1.f, 3.5, Occlusion());
        EXPECT_EQ(gfx::Rect(0, 200, 100, 300), skewport_200);

        tiling->ComputeTilePriorityRects(viewport_200, 1.f, 3.5, Occlusion());

        // This should never happen, but advance the viewport yet again keeping the
        // time the same.
        gfx::Rect viewport_250 = gfx::Rect(0, 250, 100, 100);
        gfx::Rect skewport_250 = tiling->ComputeSkewport(3.5, viewport_250);
        EXPECT_EQ(viewport_250, skewport_250);
        tiling->ComputeTilePriorityRects(viewport_250, 1.f, 3.5, Occlusion());
    }

    TEST(PictureLayerTilingTest, ViewportDistanceWithScale)
    {
        FakePictureLayerTilingClient client;

        gfx::Rect viewport(0, 0, 100, 100);
        gfx::Size layer_bounds(1500, 1500);

        client.SetTileSize(gfx::Size(10, 10));
        LayerTreeSettings settings;

        // Tiling at 0.25 scale: this should create 47x47 tiles of size 10x10.
        // The reason is that each tile has a one pixel border, so tile at (1, 2)
        // for instance begins at (8, 16) pixels. So tile at (46, 46) will begin at
        // (368, 368) and extend to the end of 1500 * 0.25 = 375 edge of the
        // tiling.
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 0.25f, raster_source,
            &client, settings);
        tiling->set_resolution(HIGH_RESOLUTION);
        gfx::Rect viewport_in_content_space = gfx::ScaleToEnclosedRect(viewport, 0.25f);

        tiling->ComputeTilePriorityRects(viewport, 1.f, 1.0, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        // Compute the soon border.
        float inset = PictureLayerTiling::CalculateSoonBorderDistance(
            viewport_in_content_space, 1.0f / 0.25f);
        gfx::Rect soon_rect_in_content_space = viewport_in_content_space;
        soon_rect_in_content_space.Inset(-inset, -inset);

        // Sanity checks.
        for (int i = 0; i < 47; ++i) {
            for (int j = 0; j < 47; ++j) {
                EXPECT_TRUE(tiling->TileAt(i, j)) << "i: " << i << " j: " << j;
            }
        }
        for (int i = 0; i < 47; ++i) {
            EXPECT_FALSE(tiling->TileAt(i, 47)) << "i: " << i;
            EXPECT_FALSE(tiling->TileAt(47, i)) << "i: " << i;
        }

        // No movement in the viewport implies that tiles will either be NOW
        // or EVENTUALLY, with the exception of tiles that are between 0 and 312
        // pixels away from the viewport, which will be in the SOON bin.
        bool have_now = false;
        bool have_eventually = false;
        bool have_soon = false;
        for (int i = 0; i < 47; ++i) {
            for (int j = 0; j < 47; ++j) {
                Tile* tile = tiling->TileAt(i, j);
                PrioritizedTile prioritized_tile = prioritized_tiles[tile];
                TilePriority priority = prioritized_tile.priority();

                gfx::Rect tile_rect = tiling->TilingDataForTesting().TileBounds(i, j);
                if (viewport_in_content_space.Intersects(tile_rect)) {
                    EXPECT_EQ(TilePriority::NOW, priority.priority_bin);
                    EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
                    have_now = true;
                } else if (soon_rect_in_content_space.Intersects(tile_rect)) {
                    EXPECT_EQ(TilePriority::SOON, priority.priority_bin);
                    have_soon = true;
                } else {
                    EXPECT_EQ(TilePriority::EVENTUALLY, priority.priority_bin);
                    EXPECT_GT(priority.distance_to_visible, 0.f);
                    have_eventually = true;
                }
            }
        }

        EXPECT_TRUE(have_now);
        EXPECT_TRUE(have_soon);
        EXPECT_TRUE(have_eventually);

        // Spot check some distances.
        // Tile at 5, 1 should begin at 41x9 in content space (without borders),
        // so the distance to a viewport that ends at 25x25 in content space
        // should be 17 (41 - 25 + 1). In layer space, then that should be
        // 17 / 0.25 = 68 pixels.

        // We can verify that the content rect (with borders) is one pixel off
        // 41,9 8x8 on all sides.
        EXPECT_EQ(tiling->TileAt(5, 1)->content_rect().ToString(), "40,8 10x10");

        TilePriority priority = prioritized_tiles[tiling->TileAt(5, 1)].priority();
        EXPECT_FLOAT_EQ(68.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(2, 5)].priority();
        EXPECT_FLOAT_EQ(68.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(3, 4)].priority();
        EXPECT_FLOAT_EQ(40.f, priority.distance_to_visible);

        // Move the viewport down 40 pixels.
        viewport = gfx::Rect(0, 40, 100, 100);
        viewport_in_content_space = gfx::ScaleToEnclosedRect(viewport, 0.25f);
        gfx::Rect skewport = tiling->ComputeSkewport(2.0, viewport_in_content_space);

        // Compute the soon border.
        inset = PictureLayerTiling::CalculateSoonBorderDistance(
            viewport_in_content_space, 1.0f / 0.25f);
        soon_rect_in_content_space = viewport_in_content_space;
        soon_rect_in_content_space.Inset(-inset, -inset);

        EXPECT_EQ(0, skewport.x());
        EXPECT_EQ(10, skewport.y());
        EXPECT_EQ(25, skewport.width());
        EXPECT_EQ(35, skewport.height());

        tiling->ComputeTilePriorityRects(viewport, 1.f, 2.0, Occlusion());
        prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        have_now = false;
        have_eventually = false;
        have_soon = false;

        // Viewport moved, so we expect to find some NOW tiles, some SOON tiles and
        // some EVENTUALLY tiles.
        for (int i = 0; i < 47; ++i) {
            for (int j = 0; j < 47; ++j) {
                Tile* tile = tiling->TileAt(i, j);
                TilePriority priority = prioritized_tiles[tile].priority();

                gfx::Rect tile_rect = tiling->TilingDataForTesting().TileBounds(i, j);
                if (viewport_in_content_space.Intersects(tile_rect)) {
                    EXPECT_EQ(TilePriority::NOW, priority.priority_bin) << "i: " << i
                                                                        << " j: " << j;
                    EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible) << "i: " << i
                                                                       << " j: " << j;
                    have_now = true;
                } else if (skewport.Intersects(tile_rect) || soon_rect_in_content_space.Intersects(tile_rect)) {
                    EXPECT_EQ(TilePriority::SOON, priority.priority_bin) << "i: " << i
                                                                         << " j: " << j;
                    EXPECT_GT(priority.distance_to_visible, 0.f) << "i: " << i
                                                                 << " j: " << j;
                    have_soon = true;
                } else {
                    EXPECT_EQ(TilePriority::EVENTUALLY, priority.priority_bin)
                        << "i: " << i << " j: " << j;
                    EXPECT_GT(priority.distance_to_visible, 0.f) << "i: " << i
                                                                 << " j: " << j;
                    have_eventually = true;
                }
            }
        }

        EXPECT_TRUE(have_now);
        EXPECT_TRUE(have_soon);
        EXPECT_TRUE(have_eventually);

        priority = prioritized_tiles[tiling->TileAt(5, 1)].priority();
        EXPECT_FLOAT_EQ(68.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(2, 5)].priority();
        EXPECT_FLOAT_EQ(28.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(3, 4)].priority();
        EXPECT_FLOAT_EQ(4.f, priority.distance_to_visible);

        // Change the underlying layer scale.
        tiling->ComputeTilePriorityRects(viewport, 2.0f, 3.0, Occlusion());
        prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        priority = prioritized_tiles[tiling->TileAt(5, 1)].priority();
        EXPECT_FLOAT_EQ(136.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(2, 5)].priority();
        EXPECT_FLOAT_EQ(56.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(3, 4)].priority();
        EXPECT_FLOAT_EQ(8.f, priority.distance_to_visible);

        // Test additional scales.
        tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 0.2f, raster_source,
            &client, LayerTreeSettings());
        tiling->set_resolution(HIGH_RESOLUTION);
        tiling->ComputeTilePriorityRects(viewport, 1.0f, 4.0, Occlusion());
        prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        priority = prioritized_tiles[tiling->TileAt(5, 1)].priority();
        EXPECT_FLOAT_EQ(110.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(2, 5)].priority();
        EXPECT_FLOAT_EQ(70.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(3, 4)].priority();
        EXPECT_FLOAT_EQ(60.f, priority.distance_to_visible);

        tiling->ComputeTilePriorityRects(viewport, 0.5f, 5.0, Occlusion());
        prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        priority = prioritized_tiles[tiling->TileAt(5, 1)].priority();
        EXPECT_FLOAT_EQ(55.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(2, 5)].priority();
        EXPECT_FLOAT_EQ(35.f, priority.distance_to_visible);

        priority = prioritized_tiles[tiling->TileAt(3, 4)].priority();
        EXPECT_FLOAT_EQ(30.f, priority.distance_to_visible);
    }

    static void TileExists(bool exists, Tile* tile,
        const gfx::Rect& geometry_rect)
    {
        EXPECT_EQ(exists, tile != NULL) << geometry_rect.ToString();
    }

    TEST_F(PictureLayerTilingIteratorTest, TilesExist)
    {
        gfx::Size layer_bounds(1099, 801);
        Initialize(gfx::Size(100, 100), 1.f, layer_bounds);
        VerifyTilesExactlyCoverRect(1.f, gfx::Rect(layer_bounds));
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, false));

        tiling_->ComputeTilePriorityRects(
            gfx::Rect(layer_bounds), // visible content rect
            1.f, // current contents scale
            1.0, // current frame time
            Occlusion());
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, true));

        // Make the viewport rect empty. All tiles are killed and become zombies.
        tiling_->ComputeTilePriorityRects(gfx::Rect(), // visible content rect
            1.f, // current contents scale
            2.0, // current frame time
            Occlusion());
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, false));
    }

    TEST_F(PictureLayerTilingIteratorTest, TilesExistGiantViewport)
    {
        gfx::Size layer_bounds(1099, 801);
        Initialize(gfx::Size(100, 100), 1.f, layer_bounds);
        VerifyTilesExactlyCoverRect(1.f, gfx::Rect(layer_bounds));
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, false));

        gfx::Rect giant_rect(-10000000, -10000000, 1000000000, 1000000000);

        tiling_->ComputeTilePriorityRects(
            gfx::Rect(layer_bounds), // visible content rect
            1.f, // current contents scale
            1.0, // current frame time
            Occlusion());
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, true));

        // If the visible content rect is empty, it should still have live tiles.
        tiling_->ComputeTilePriorityRects(giant_rect, // visible content rect
            1.f, // current contents scale
            2.0, // current frame time
            Occlusion());
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, true));
    }

    TEST_F(PictureLayerTilingIteratorTest, TilesExistOutsideViewport)
    {
        gfx::Size layer_bounds(1099, 801);
        Initialize(gfx::Size(100, 100), 1.f, layer_bounds);
        VerifyTilesExactlyCoverRect(1.f, gfx::Rect(layer_bounds));
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, false));

        // This rect does not intersect with the layer, as the layer is outside the
        // viewport.
        gfx::Rect viewport_rect(1100, 0, 1000, 1000);
        EXPECT_FALSE(viewport_rect.Intersects(gfx::Rect(layer_bounds)));

        tiling_->ComputeTilePriorityRects(viewport_rect, // visible content rect
            1.f, // current contents scale
            1.0, // current frame time
            Occlusion());
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, true));
    }

    static void TilesIntersectingRectExist(const gfx::Rect& rect,
        bool intersect_exists,
        Tile* tile,
        const gfx::Rect& geometry_rect)
    {
        bool intersects = rect.Intersects(geometry_rect);
        bool expected_exists = intersect_exists ? intersects : !intersects;
        EXPECT_EQ(expected_exists, tile != NULL)
            << "Rects intersecting " << rect.ToString() << " should exist. "
            << "Current tile rect is " << geometry_rect.ToString();
    }

    TEST_F(PictureLayerTilingIteratorTest,
        TilesExistLargeViewportAndLayerWithSmallVisibleArea)
    {
        gfx::Size layer_bounds(10000, 10000);
        client_.SetTileSize(gfx::Size(100, 100));
        LayerTreeSettings settings;
        settings.tiling_interest_area_padding = 1;

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        tiling_ = TestablePictureLayerTiling::Create(PENDING_TREE, 1.f, raster_source,
            &client_, settings);
        tiling_->set_resolution(HIGH_RESOLUTION);
        VerifyTilesExactlyCoverRect(1.f, gfx::Rect(layer_bounds));
        VerifyTiles(1.f, gfx::Rect(layer_bounds), base::Bind(&TileExists, false));

        gfx::Rect visible_rect(8000, 8000, 50, 50);

        tiling_->ComputeTilePriorityRects(visible_rect, // visible content rect
            1.f, // current contents scale
            1.0, // current frame time
            Occlusion());
        VerifyTiles(1.f,
            gfx::Rect(layer_bounds),
            base::Bind(&TilesIntersectingRectExist, visible_rect, true));
    }

    TEST(ComputeTilePriorityRectsTest, VisibleTiles)
    {
        // The TilePriority of visible tiles should have zero distance_to_visible
        // and time_to_visible.
        FakePictureLayerTilingClient client;

        gfx::Size device_viewport(800, 600);
        gfx::Size last_layer_bounds(200, 200);
        gfx::Size current_layer_bounds(200, 200);
        float current_layer_contents_scale = 1.f;
        gfx::Transform current_screen_transform;
        double current_frame_time_in_seconds = 1.0;

        gfx::Rect viewport_in_layer_space = ViewportInLayerSpace(
            current_screen_transform, device_viewport);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(current_layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());
        tiling->set_resolution(HIGH_RESOLUTION);

        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            current_layer_contents_scale,
            current_frame_time_in_seconds, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        ASSERT_TRUE(tiling->TileAt(0, 0));
        ASSERT_TRUE(tiling->TileAt(0, 1));
        ASSERT_TRUE(tiling->TileAt(1, 0));
        ASSERT_TRUE(tiling->TileAt(1, 1));

        TilePriority priority = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_FLOAT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_FLOAT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_FLOAT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_FLOAT_EQ(TilePriority::NOW, priority.priority_bin);
    }

    TEST(ComputeTilePriorityRectsTest, OffscreenTiles)
    {
        // The TilePriority of offscreen tiles (without movement) should have nonzero
        // distance_to_visible and infinite time_to_visible.
        FakePictureLayerTilingClient client;

        gfx::Size device_viewport(800, 600);
        gfx::Size last_layer_bounds(200, 200);
        gfx::Size current_layer_bounds(200, 200);
        float current_layer_contents_scale = 1.f;
        gfx::Transform last_screen_transform;
        gfx::Transform current_screen_transform;
        double current_frame_time_in_seconds = 1.0;

        current_screen_transform.Translate(850, 0);
        last_screen_transform = current_screen_transform;

        gfx::Rect viewport_in_layer_space = ViewportInLayerSpace(
            current_screen_transform, device_viewport);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(current_layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());
        tiling->set_resolution(HIGH_RESOLUTION);

        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            current_layer_contents_scale,
            current_frame_time_in_seconds, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        ASSERT_TRUE(tiling->TileAt(0, 0));
        ASSERT_TRUE(tiling->TileAt(0, 1));
        ASSERT_TRUE(tiling->TileAt(1, 0));
        ASSERT_TRUE(tiling->TileAt(1, 1));

        TilePriority priority = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        // Furthermore, in this scenario tiles on the right hand side should have a
        // larger distance to visible.
        TilePriority left = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        TilePriority right = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_GT(right.distance_to_visible, left.distance_to_visible);

        left = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        right = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_GT(right.distance_to_visible, left.distance_to_visible);
    }

    TEST(ComputeTilePriorityRectsTest, PartiallyOffscreenLayer)
    {
        // Sanity check that a layer with some tiles visible and others offscreen has
        // correct TilePriorities for each tile.
        FakePictureLayerTilingClient client;

        gfx::Size device_viewport(800, 600);
        gfx::Size last_layer_bounds(200, 200);
        gfx::Size current_layer_bounds(200, 200);
        float current_layer_contents_scale = 1.f;
        gfx::Transform last_screen_transform;
        gfx::Transform current_screen_transform;
        double current_frame_time_in_seconds = 1.0;

        current_screen_transform.Translate(705, 505);
        last_screen_transform = current_screen_transform;

        gfx::Rect viewport_in_layer_space = ViewportInLayerSpace(
            current_screen_transform, device_viewport);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(current_layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());
        tiling->set_resolution(HIGH_RESOLUTION);

        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            current_layer_contents_scale,
            current_frame_time_in_seconds, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        ASSERT_TRUE(tiling->TileAt(0, 0));
        ASSERT_TRUE(tiling->TileAt(0, 1));
        ASSERT_TRUE(tiling->TileAt(1, 0));
        ASSERT_TRUE(tiling->TileAt(1, 1));

        TilePriority priority = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_FLOAT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);
    }

    TEST(ComputeTilePriorityRectsTest, PartiallyOffscreenRotatedLayer)
    {
        // Each tile of a layer may be affected differently by a transform; Check
        // that ComputeTilePriorityRects correctly accounts for the transform between
        // layer space and screen space.
        FakePictureLayerTilingClient client;

        gfx::Size device_viewport(800, 600);
        gfx::Size last_layer_bounds(200, 200);
        gfx::Size current_layer_bounds(200, 200);
        float current_layer_contents_scale = 1.f;
        gfx::Transform last_screen_transform;
        gfx::Transform current_screen_transform;
        double current_frame_time_in_seconds = 1.0;

        // A diagonally rotated layer that is partially off the bottom of the screen.
        // In this configuration, only the top-left tile would be visible.
        current_screen_transform.Translate(600, 750);
        current_screen_transform.RotateAboutZAxis(45);
        last_screen_transform = current_screen_transform;

        gfx::Rect viewport_in_layer_space = ViewportInLayerSpace(
            current_screen_transform, device_viewport);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(current_layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());
        tiling->set_resolution(HIGH_RESOLUTION);

        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            current_layer_contents_scale,
            current_frame_time_in_seconds, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        ASSERT_TRUE(tiling->TileAt(0, 0));
        ASSERT_TRUE(tiling->TileAt(0, 1));
        ASSERT_TRUE(tiling->TileAt(1, 0));
        ASSERT_TRUE(tiling->TileAt(1, 1));

        TilePriority priority = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        // Furthermore, in this scenario the bottom-right tile should have the larger
        // distance to visible.
        TilePriority top_left = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        TilePriority top_right = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        TilePriority bottom_right = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_GT(top_right.distance_to_visible, top_left.distance_to_visible);

        EXPECT_EQ(bottom_right.distance_to_visible, top_right.distance_to_visible);
    }

    TEST(ComputeTilePriorityRectsTest, PerspectiveLayer)
    {
        // Perspective transforms need to take a different code path.
        // This test checks tile priorities of a perspective layer.
        FakePictureLayerTilingClient client;

        gfx::Size device_viewport(800, 600);
        gfx::Rect visible_layer_rect(0, 0, 0, 0); // offscreen.
        gfx::Size last_layer_bounds(200, 200);
        gfx::Size current_layer_bounds(200, 200);
        float current_layer_contents_scale = 1.f;
        gfx::Transform last_screen_transform;
        gfx::Transform current_screen_transform;
        double current_frame_time_in_seconds = 1.0;

        // A 3d perspective layer rotated about its Y axis, translated to almost
        // fully offscreen. The left side will appear closer (i.e. larger in 2d) than
        // the right side, so the top-left tile will technically be closer than the
        // top-right.

        // Translate layer to offscreen
        current_screen_transform.Translate(400.0, 630.0);
        // Apply perspective about the center of the layer
        current_screen_transform.Translate(100.0, 100.0);
        current_screen_transform.ApplyPerspectiveDepth(100.0);
        current_screen_transform.RotateAboutYAxis(10.0);
        current_screen_transform.Translate(-100.0, -100.0);
        last_screen_transform = current_screen_transform;

        // Sanity check that this transform wouldn't cause w<0 clipping.
        bool clipped;
        MathUtil::MapQuad(current_screen_transform,
            gfx::QuadF(gfx::RectF(0, 0, 200, 200)),
            &clipped);
        ASSERT_FALSE(clipped);

        gfx::Rect viewport_in_layer_space = ViewportInLayerSpace(
            current_screen_transform, device_viewport);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(current_layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());
        tiling->set_resolution(HIGH_RESOLUTION);

        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            current_layer_contents_scale,
            current_frame_time_in_seconds, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        ASSERT_TRUE(tiling->TileAt(0, 0));
        ASSERT_TRUE(tiling->TileAt(0, 1));
        ASSERT_TRUE(tiling->TileAt(1, 0));
        ASSERT_TRUE(tiling->TileAt(1, 1));

        // All tiles will have a positive distance_to_visible
        // and an infinite time_to_visible.
        TilePriority priority = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        EXPECT_FLOAT_EQ(priority.distance_to_visible, 0.f);
        EXPECT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_FLOAT_EQ(priority.distance_to_visible, 0.f);
        EXPECT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        // Furthermore, in this scenario the top-left distance_to_visible
        // will be smallest, followed by top-right. The bottom layers
        // will of course be further than the top layers.
        TilePriority top_left = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        TilePriority top_right = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        TilePriority bottom_left = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        TilePriority bottom_right = prioritized_tiles[tiling->TileAt(1, 1)].priority();

        EXPECT_GT(bottom_right.distance_to_visible, top_right.distance_to_visible);

        EXPECT_GT(bottom_left.distance_to_visible, top_left.distance_to_visible);
    }

    TEST(ComputeTilePriorityRectsTest, PerspectiveLayerClippedByW)
    {
        // Perspective transforms need to take a different code path.
        // This test checks tile priorities of a perspective layer.
        FakePictureLayerTilingClient client;

        gfx::Size device_viewport(800, 600);
        gfx::Size last_layer_bounds(200, 200);
        gfx::Size current_layer_bounds(200, 200);
        float current_layer_contents_scale = 1.f;
        gfx::Transform last_screen_transform;
        gfx::Transform current_screen_transform;
        double current_frame_time_in_seconds = 1.0;

        // A 3d perspective layer rotated about its Y axis, translated to almost
        // fully offscreen. The left side will appear closer (i.e. larger in 2d) than
        // the right side, so the top-left tile will technically be closer than the
        // top-right.

        // Translate layer to offscreen
        current_screen_transform.Translate(400.0, 970.0);
        // Apply perspective and rotation about the center of the layer
        current_screen_transform.Translate(100.0, 100.0);
        current_screen_transform.ApplyPerspectiveDepth(10.0);
        current_screen_transform.RotateAboutYAxis(10.0);
        current_screen_transform.Translate(-100.0, -100.0);
        last_screen_transform = current_screen_transform;

        // Sanity check that this transform does cause w<0 clipping for the left side
        // of the layer, but not the right side.
        bool clipped;
        MathUtil::MapQuad(current_screen_transform,
            gfx::QuadF(gfx::RectF(0, 0, 100, 200)),
            &clipped);
        ASSERT_TRUE(clipped);

        MathUtil::MapQuad(current_screen_transform,
            gfx::QuadF(gfx::RectF(100, 0, 100, 200)),
            &clipped);
        ASSERT_FALSE(clipped);

        gfx::Rect viewport_in_layer_space = ViewportInLayerSpace(
            current_screen_transform, device_viewport);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(current_layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());
        tiling->set_resolution(HIGH_RESOLUTION);

        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            current_layer_contents_scale,
            current_frame_time_in_seconds, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        ASSERT_TRUE(tiling->TileAt(0, 0));
        ASSERT_TRUE(tiling->TileAt(0, 1));
        ASSERT_TRUE(tiling->TileAt(1, 0));
        ASSERT_TRUE(tiling->TileAt(1, 1));

        // Left-side tiles will be clipped by the transform, so we have to assume
        // they are visible just in case.
        TilePriority priority = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_FLOAT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        // Right-side tiles will have a positive distance_to_visible
        // and an infinite time_to_visible.
        priority = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_FLOAT_EQ(priority.distance_to_visible, 0.f);
        EXPECT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);
    }

    TEST(ComputeTilePriorityRectsTest, BasicMotion)
    {
        // Test that time_to_visible is computed correctly when
        // there is some motion.
        FakePictureLayerTilingClient client;

        gfx::Size device_viewport(800, 600);
        gfx::Rect visible_layer_rect(0, 0, 0, 0);
        gfx::Size last_layer_bounds(200, 200);
        gfx::Size current_layer_bounds(200, 200);
        float last_layer_contents_scale = 1.f;
        float current_layer_contents_scale = 1.f;
        gfx::Transform last_screen_transform;
        gfx::Transform current_screen_transform;
        double last_frame_time_in_seconds = 1.0;
        double current_frame_time_in_seconds = 2.0;

        // Offscreen layer is coming closer to viewport at 1000 pixels per second.
        current_screen_transform.Translate(1800, 0);
        last_screen_transform.Translate(2800, 0);

        gfx::Rect viewport_in_layer_space = ViewportInLayerSpace(
            current_screen_transform, device_viewport);

        client.SetTileSize(gfx::Size(100, 100));
        LayerTreeSettings settings;

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(current_layer_bounds);
        scoped_ptr<TestablePictureLayerTiling> tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, settings);
        tiling->set_resolution(HIGH_RESOLUTION);

        // previous ("last") frame
        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            last_layer_contents_scale,
            last_frame_time_in_seconds, Occlusion());

        // current frame
        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            current_layer_contents_scale,
            current_frame_time_in_seconds, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        ASSERT_TRUE(tiling->TileAt(0, 0));
        ASSERT_TRUE(tiling->TileAt(0, 1));
        ASSERT_TRUE(tiling->TileAt(1, 0));
        ASSERT_TRUE(tiling->TileAt(1, 1));

        TilePriority priority = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        // time_to_visible for the right hand side layers needs an extra 0.099
        // seconds because this tile is 99 pixels further away.
        priority = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 1)].priority();
        EXPECT_GT(priority.distance_to_visible, 0.f);
        EXPECT_NE(TilePriority::NOW, priority.priority_bin);
    }

    TEST(ComputeTilePriorityRectsTest, RotationMotion)
    {
        // Each tile of a layer may be affected differently by a transform; Check
        // that ComputeTilePriorityRects correctly accounts for the transform between
        // layer space and screen space.

        FakePictureLayerTilingClient client;
        scoped_ptr<TestablePictureLayerTiling> tiling;

        gfx::Size device_viewport(800, 600);
        gfx::Rect visible_layer_rect(0, 0, 0, 0); // offscren.
        gfx::Size last_layer_bounds(200, 200);
        gfx::Size current_layer_bounds(200, 200);
        float last_layer_contents_scale = 1.f;
        float current_layer_contents_scale = 1.f;
        gfx::Transform last_screen_transform;
        gfx::Transform current_screen_transform;
        double last_frame_time_in_seconds = 1.0;
        double current_frame_time_in_seconds = 2.0;

        // Rotation motion is set up specifically so that:
        //  - rotation occurs about the center of the layer
        //  - the top-left tile becomes visible on rotation
        //  - the top-right tile will have an infinite time_to_visible
        //    because it is rotating away from viewport.
        //  - bottom-left layer will have a positive non-zero time_to_visible
        //    because it is rotating toward the viewport.
        current_screen_transform.Translate(400, 550);
        current_screen_transform.RotateAboutZAxis(45);

        last_screen_transform.Translate(400, 550);

        gfx::Rect viewport_in_layer_space = ViewportInLayerSpace(
            current_screen_transform, device_viewport);

        client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(current_layer_bounds);
        tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &client, LayerTreeSettings());
        tiling->set_resolution(HIGH_RESOLUTION);

        // previous ("last") frame
        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            last_layer_contents_scale,
            last_frame_time_in_seconds, Occlusion());

        // current frame
        tiling->ComputeTilePriorityRects(viewport_in_layer_space,
            current_layer_contents_scale,
            current_frame_time_in_seconds, Occlusion());
        auto prioritized_tiles = tiling->UpdateAndGetAllPrioritizedTilesForTesting();

        ASSERT_TRUE(tiling->TileAt(0, 0));
        ASSERT_TRUE(tiling->TileAt(0, 1));
        ASSERT_TRUE(tiling->TileAt(1, 0));
        ASSERT_TRUE(tiling->TileAt(1, 1));

        TilePriority priority = prioritized_tiles[tiling->TileAt(0, 0)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(0, 1)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_EQ(TilePriority::NOW, priority.priority_bin);

        priority = prioritized_tiles[tiling->TileAt(1, 0)].priority();
        EXPECT_FLOAT_EQ(0.f, priority.distance_to_visible);
        EXPECT_EQ(TilePriority::NOW, priority.priority_bin);
    }

    TEST(PictureLayerTilingTest, RecycledTilesCleared)
    {
        // This test performs the following:
        // Setup:
        // - Two tilings, one active one recycled with all tiles shared.
        // Procedure:
        // - Viewport moves somewhere far away and active tiling clears tiles.
        // - Viewport moves back and a new active tiling tile is created.
        // Result:
        // - Recycle tiling does _not_ have the tile in the same location (thus it
        //   will be shared next time a pending tiling is created).

        FakePictureLayerTilingClient active_client;

        active_client.SetTileSize(gfx::Size(100, 100));
        LayerTreeSettings settings;

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(gfx::Size(10000, 10000));
        scoped_ptr<TestablePictureLayerTiling> active_tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &active_client, settings);
        active_tiling->set_resolution(HIGH_RESOLUTION);
        // Create all tiles on this tiling.
        active_tiling->ComputeTilePriorityRects(gfx::Rect(0, 0, 100, 100), 1.0f, 1.0f,
            Occlusion());

        FakePictureLayerTilingClient recycle_client;
        recycle_client.SetTileSize(gfx::Size(100, 100));
        recycle_client.set_twin_tiling(active_tiling.get());

        raster_source = FakeDisplayListRasterSource::CreateFilled(gfx::Size(10000, 10000));
        scoped_ptr<TestablePictureLayerTiling> recycle_tiling = TestablePictureLayerTiling::Create(PENDING_TREE, 1.0f, raster_source,
            &recycle_client, settings);
        recycle_tiling->set_resolution(HIGH_RESOLUTION);

        // Create all tiles on the second tiling. All tiles should be shared.
        recycle_tiling->ComputeTilePriorityRects(gfx::Rect(0, 0, 100, 100), 1.0f,
            1.0f, Occlusion());

        // Set the second tiling as recycled.
        active_client.set_twin_tiling(NULL);
        recycle_client.set_twin_tiling(NULL);

        EXPECT_TRUE(active_tiling->TileAt(0, 0));
        EXPECT_FALSE(recycle_tiling->TileAt(0, 0));

        // Move the viewport far away from the (0, 0) tile.
        active_tiling->ComputeTilePriorityRects(gfx::Rect(9000, 9000, 100, 100), 1.0f,
            2.0, Occlusion());
        // Ensure the tile was deleted.
        EXPECT_FALSE(active_tiling->TileAt(0, 0));
        EXPECT_FALSE(recycle_tiling->TileAt(0, 0));

        // Move the viewport back to (0, 0) tile.
        active_tiling->ComputeTilePriorityRects(gfx::Rect(0, 0, 100, 100), 1.0f, 3.0,
            Occlusion());

        // Ensure that we now have a tile here on both active.
        EXPECT_TRUE(active_tiling->TileAt(0, 0));
        EXPECT_FALSE(recycle_tiling->TileAt(0, 0));
    }

    TEST(PictureLayerTilingTest, RecycledTilesClearedOnReset)
    {
        FakePictureLayerTilingClient active_client;
        active_client.SetTileSize(gfx::Size(100, 100));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(gfx::Size(100, 100));
        scoped_ptr<TestablePictureLayerTiling> active_tiling = TestablePictureLayerTiling::Create(ACTIVE_TREE, 1.0f, raster_source,
            &active_client, LayerTreeSettings());
        active_tiling->set_resolution(HIGH_RESOLUTION);
        // Create all tiles on this tiling.
        active_tiling->ComputeTilePriorityRects(gfx::Rect(0, 0, 100, 100), 1.0f, 1.0f,
            Occlusion());

        FakePictureLayerTilingClient recycle_client;
        recycle_client.SetTileSize(gfx::Size(100, 100));
        recycle_client.set_twin_tiling(active_tiling.get());

        LayerTreeSettings settings;

        raster_source = FakeDisplayListRasterSource::CreateFilled(gfx::Size(100, 100));
        scoped_ptr<TestablePictureLayerTiling> recycle_tiling = TestablePictureLayerTiling::Create(PENDING_TREE, 1.0f, raster_source,
            &recycle_client, settings);
        recycle_tiling->set_resolution(HIGH_RESOLUTION);

        // Create all tiles on the recycle tiling. All tiles should be shared.
        recycle_tiling->ComputeTilePriorityRects(gfx::Rect(0, 0, 100, 100), 1.0f,
            1.0f, Occlusion());

        // Set the second tiling as recycled.
        active_client.set_twin_tiling(NULL);
        recycle_client.set_twin_tiling(NULL);

        EXPECT_TRUE(active_tiling->TileAt(0, 0));
        EXPECT_FALSE(recycle_tiling->TileAt(0, 0));

        // Reset the active tiling. The recycle tiles should be released too.
        active_tiling->Reset();
        EXPECT_FALSE(active_tiling->TileAt(0, 0));
        EXPECT_FALSE(recycle_tiling->TileAt(0, 0));
    }

    TEST_F(PictureLayerTilingIteratorTest, ResizeTilesAndUpdateToCurrent)
    {
        // The tiling has four rows and three columns.
        Initialize(gfx::Size(150, 100), 1.f, gfx::Size(250, 150));
        tiling_->CreateAllTilesForTesting();
        EXPECT_EQ(150, tiling_->TilingDataForTesting().max_texture_size().width());
        EXPECT_EQ(100, tiling_->TilingDataForTesting().max_texture_size().height());
        EXPECT_EQ(4u, tiling_->AllTilesForTesting().size());

        client_.SetTileSize(gfx::Size(250, 200));

        // Tile size in the tiling should still be 150x100.
        EXPECT_EQ(150, tiling_->TilingDataForTesting().max_texture_size().width());
        EXPECT_EQ(100, tiling_->TilingDataForTesting().max_texture_size().height());

        // The layer's size isn't changed, but the tile size was.
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(gfx::Size(250, 150));
        tiling_->SetRasterSourceAndResize(raster_source);

        // Tile size in the tiling should be resized to 250x200.
        EXPECT_EQ(250, tiling_->TilingDataForTesting().max_texture_size().width());
        EXPECT_EQ(200, tiling_->TilingDataForTesting().max_texture_size().height());
        EXPECT_EQ(0u, tiling_->AllTilesForTesting().size());
    }

    // This test runs into floating point issues because of big numbers.
    TEST_F(PictureLayerTilingIteratorTest, GiantRect)
    {
        gfx::Size tile_size(256, 256);
        gfx::Size layer_size(33554432, 33554432);
        float contents_scale = 1.f;

        client_.SetTileSize(tile_size);
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateEmpty(layer_size);
        tiling_ = TestablePictureLayerTiling::Create(PENDING_TREE, contents_scale,
            raster_source, &client_,
            LayerTreeSettings());

        gfx::Rect content_rect(25554432, 25554432, 950, 860);
        VerifyTilesExactlyCoverRect(contents_scale, content_rect);
    }

} // namespace
} // namespace cc
