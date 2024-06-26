// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/tiles/picture_layer_tiling_set.h"

#include <map>
#include <vector>

#include "cc/resources/resource_provider.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_picture_layer_tiling_client.h"
#include "cc/test/fake_resource_provider.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/trees/layer_tree_settings.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {
namespace {

    scoped_ptr<PictureLayerTilingSet> CreateTilingSet(
        PictureLayerTilingClient* client)
    {
        LayerTreeSettings defaults;
        return PictureLayerTilingSet::Create(
            ACTIVE_TREE, client, defaults.tiling_interest_area_padding,
            defaults.skewport_target_time_in_seconds,
            defaults.skewport_extrapolation_limit_in_content_pixels);
    }

    TEST(PictureLayerTilingSetTest, NoResources)
    {
        FakePictureLayerTilingClient client;
        gfx::Size layer_bounds(1000, 800);
        scoped_ptr<PictureLayerTilingSet> set = CreateTilingSet(&client);
        client.SetTileSize(gfx::Size(256, 256));

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateEmpty(layer_bounds);

        set->AddTiling(1.0, raster_source);
        set->AddTiling(1.5, raster_source);
        set->AddTiling(2.0, raster_source);

        float contents_scale = 2.0;
        gfx::Size content_bounds(
            gfx::ScaleToCeiledSize(layer_bounds, contents_scale));
        gfx::Rect content_rect(content_bounds);

        Region remaining(content_rect);
        PictureLayerTilingSet::CoverageIterator iter(set.get(), contents_scale,
            content_rect, contents_scale);
        for (; iter; ++iter) {
            gfx::Rect geometry_rect = iter.geometry_rect();
            EXPECT_TRUE(content_rect.Contains(geometry_rect));
            ASSERT_TRUE(remaining.Contains(geometry_rect));
            remaining.Subtract(geometry_rect);

            // No tiles have resources, so no iter represents a real tile.
            EXPECT_FALSE(*iter);
        }
        EXPECT_TRUE(remaining.IsEmpty());
    }

    TEST(PictureLayerTilingSetTest, TilingRange)
    {
        FakePictureLayerTilingClient client;
        gfx::Size layer_bounds(10, 10);
        PictureLayerTilingSet::TilingRange higher_than_high_res_range(0, 0);
        PictureLayerTilingSet::TilingRange high_res_range(0, 0);
        PictureLayerTilingSet::TilingRange between_high_and_low_res_range(0, 0);
        PictureLayerTilingSet::TilingRange low_res_range(0, 0);
        PictureLayerTilingSet::TilingRange lower_than_low_res_range(0, 0);
        PictureLayerTiling* high_res_tiling;
        PictureLayerTiling* low_res_tiling;

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);

        scoped_ptr<PictureLayerTilingSet> set = CreateTilingSet(&client);
        set->AddTiling(2.0, raster_source);
        high_res_tiling = set->AddTiling(1.0, raster_source);
        high_res_tiling->set_resolution(HIGH_RESOLUTION);
        set->AddTiling(0.5, raster_source);
        low_res_tiling = set->AddTiling(0.25, raster_source);
        low_res_tiling->set_resolution(LOW_RESOLUTION);
        set->AddTiling(0.125, raster_source);

        higher_than_high_res_range = set->GetTilingRange(PictureLayerTilingSet::HIGHER_THAN_HIGH_RES);
        EXPECT_EQ(0u, higher_than_high_res_range.start);
        EXPECT_EQ(1u, higher_than_high_res_range.end);

        high_res_range = set->GetTilingRange(PictureLayerTilingSet::HIGH_RES);
        EXPECT_EQ(1u, high_res_range.start);
        EXPECT_EQ(2u, high_res_range.end);

        between_high_and_low_res_range = set->GetTilingRange(PictureLayerTilingSet::BETWEEN_HIGH_AND_LOW_RES);
        EXPECT_EQ(2u, between_high_and_low_res_range.start);
        EXPECT_EQ(3u, between_high_and_low_res_range.end);

        low_res_range = set->GetTilingRange(PictureLayerTilingSet::LOW_RES);
        EXPECT_EQ(3u, low_res_range.start);
        EXPECT_EQ(4u, low_res_range.end);

        lower_than_low_res_range = set->GetTilingRange(PictureLayerTilingSet::LOWER_THAN_LOW_RES);
        EXPECT_EQ(4u, lower_than_low_res_range.start);
        EXPECT_EQ(5u, lower_than_low_res_range.end);

        scoped_ptr<PictureLayerTilingSet> set_without_low_res = CreateTilingSet(&client);
        set_without_low_res->AddTiling(2.0, raster_source);
        high_res_tiling = set_without_low_res->AddTiling(1.0, raster_source);
        high_res_tiling->set_resolution(HIGH_RESOLUTION);
        set_without_low_res->AddTiling(0.5, raster_source);
        set_without_low_res->AddTiling(0.25, raster_source);

        higher_than_high_res_range = set_without_low_res->GetTilingRange(
            PictureLayerTilingSet::HIGHER_THAN_HIGH_RES);
        EXPECT_EQ(0u, higher_than_high_res_range.start);
        EXPECT_EQ(1u, higher_than_high_res_range.end);

        high_res_range = set_without_low_res->GetTilingRange(PictureLayerTilingSet::HIGH_RES);
        EXPECT_EQ(1u, high_res_range.start);
        EXPECT_EQ(2u, high_res_range.end);

        between_high_and_low_res_range = set_without_low_res->GetTilingRange(
            PictureLayerTilingSet::BETWEEN_HIGH_AND_LOW_RES);
        EXPECT_EQ(2u, between_high_and_low_res_range.start);
        EXPECT_EQ(4u, between_high_and_low_res_range.end);

        low_res_range = set_without_low_res->GetTilingRange(PictureLayerTilingSet::LOW_RES);
        EXPECT_EQ(0u, low_res_range.end - low_res_range.start);

        lower_than_low_res_range = set_without_low_res->GetTilingRange(
            PictureLayerTilingSet::LOWER_THAN_LOW_RES);
        EXPECT_EQ(0u, lower_than_low_res_range.end - lower_than_low_res_range.start);

        scoped_ptr<PictureLayerTilingSet> set_with_only_high_and_low_res = CreateTilingSet(&client);
        high_res_tiling = set_with_only_high_and_low_res->AddTiling(1.0, raster_source);
        high_res_tiling->set_resolution(HIGH_RESOLUTION);
        low_res_tiling = set_with_only_high_and_low_res->AddTiling(0.5, raster_source);
        low_res_tiling->set_resolution(LOW_RESOLUTION);

        higher_than_high_res_range = set_with_only_high_and_low_res->GetTilingRange(
            PictureLayerTilingSet::HIGHER_THAN_HIGH_RES);
        EXPECT_EQ(0u,
            higher_than_high_res_range.end - higher_than_high_res_range.start);

        high_res_range = set_with_only_high_and_low_res->GetTilingRange(
            PictureLayerTilingSet::HIGH_RES);
        EXPECT_EQ(0u, high_res_range.start);
        EXPECT_EQ(1u, high_res_range.end);

        between_high_and_low_res_range = set_with_only_high_and_low_res->GetTilingRange(
            PictureLayerTilingSet::BETWEEN_HIGH_AND_LOW_RES);
        EXPECT_EQ(0u, between_high_and_low_res_range.end - between_high_and_low_res_range.start);

        low_res_range = set_with_only_high_and_low_res->GetTilingRange(
            PictureLayerTilingSet::LOW_RES);
        EXPECT_EQ(1u, low_res_range.start);
        EXPECT_EQ(2u, low_res_range.end);

        lower_than_low_res_range = set_with_only_high_and_low_res->GetTilingRange(
            PictureLayerTilingSet::LOWER_THAN_LOW_RES);
        EXPECT_EQ(0u, lower_than_low_res_range.end - lower_than_low_res_range.start);

        scoped_ptr<PictureLayerTilingSet> set_with_only_high_res = CreateTilingSet(&client);
        high_res_tiling = set_with_only_high_res->AddTiling(1.0, raster_source);
        high_res_tiling->set_resolution(HIGH_RESOLUTION);

        higher_than_high_res_range = set_with_only_high_res->GetTilingRange(
            PictureLayerTilingSet::HIGHER_THAN_HIGH_RES);
        EXPECT_EQ(0u,
            higher_than_high_res_range.end - higher_than_high_res_range.start);

        high_res_range = set_with_only_high_res->GetTilingRange(PictureLayerTilingSet::HIGH_RES);
        EXPECT_EQ(0u, high_res_range.start);
        EXPECT_EQ(1u, high_res_range.end);

        between_high_and_low_res_range = set_with_only_high_res->GetTilingRange(
            PictureLayerTilingSet::BETWEEN_HIGH_AND_LOW_RES);
        EXPECT_EQ(0u, between_high_and_low_res_range.end - between_high_and_low_res_range.start);

        low_res_range = set_with_only_high_res->GetTilingRange(PictureLayerTilingSet::LOW_RES);
        EXPECT_EQ(0u, low_res_range.end - low_res_range.start);

        lower_than_low_res_range = set_with_only_high_res->GetTilingRange(
            PictureLayerTilingSet::LOWER_THAN_LOW_RES);
        EXPECT_EQ(0u, lower_than_low_res_range.end - lower_than_low_res_range.start);
    }

    class PictureLayerTilingSetTestWithResources : public testing::Test {
    public:
        void RunTest(int num_tilings,
            float min_scale,
            float scale_increment,
            float ideal_contents_scale,
            float expected_scale)
        {
            FakeOutputSurfaceClient output_surface_client;
            scoped_ptr<FakeOutputSurface> output_surface = FakeOutputSurface::Create3d();
            CHECK(output_surface->BindToClient(&output_surface_client));

            scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
                new TestSharedBitmapManager());
            scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(output_surface.get(),
                shared_bitmap_manager.get());

            FakePictureLayerTilingClient client(resource_provider.get());
            client.SetTileSize(gfx::Size(256, 256));
            gfx::Size layer_bounds(1000, 800);
            scoped_ptr<PictureLayerTilingSet> set = CreateTilingSet(&client);
            scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);

            float scale = min_scale;
            for (int i = 0; i < num_tilings; ++i, scale += scale_increment) {
                PictureLayerTiling* tiling = set->AddTiling(scale, raster_source);
                tiling->set_resolution(HIGH_RESOLUTION);
                tiling->CreateAllTilesForTesting();
                std::vector<Tile*> tiles = tiling->AllTilesForTesting();
                client.tile_manager()->InitializeTilesWithResourcesForTesting(tiles);
            }

            float max_contents_scale = scale;
            gfx::Size content_bounds(
                gfx::ScaleToCeiledSize(layer_bounds, max_contents_scale));
            gfx::Rect content_rect(content_bounds);

            Region remaining(content_rect);
            PictureLayerTilingSet::CoverageIterator iter(
                set.get(), max_contents_scale, content_rect, ideal_contents_scale);
            for (; iter; ++iter) {
                gfx::Rect geometry_rect = iter.geometry_rect();
                EXPECT_TRUE(content_rect.Contains(geometry_rect));
                ASSERT_TRUE(remaining.Contains(geometry_rect));
                remaining.Subtract(geometry_rect);

                float scale = iter.CurrentTiling()->contents_scale();
                EXPECT_EQ(expected_scale, scale);

                if (num_tilings)
                    EXPECT_TRUE(*iter);
                else
                    EXPECT_FALSE(*iter);
            }
            EXPECT_TRUE(remaining.IsEmpty());
        }
    };

    TEST_F(PictureLayerTilingSetTestWithResources, NoTilings)
    {
        RunTest(0, 0.f, 0.f, 2.f, 0.f);
    }
    TEST_F(PictureLayerTilingSetTestWithResources, OneTiling_Smaller)
    {
        RunTest(1, 1.f, 0.f, 2.f, 1.f);
    }
    TEST_F(PictureLayerTilingSetTestWithResources, OneTiling_Larger)
    {
        RunTest(1, 3.f, 0.f, 2.f, 3.f);
    }
    TEST_F(PictureLayerTilingSetTestWithResources, TwoTilings_Smaller)
    {
        RunTest(2, 1.f, 1.f, 3.f, 2.f);
    }

    TEST_F(PictureLayerTilingSetTestWithResources, TwoTilings_SmallerEqual)
    {
        RunTest(2, 1.f, 1.f, 2.f, 2.f);
    }

    TEST_F(PictureLayerTilingSetTestWithResources, TwoTilings_LargerEqual)
    {
        RunTest(2, 1.f, 1.f, 1.f, 1.f);
    }

    TEST_F(PictureLayerTilingSetTestWithResources, TwoTilings_Larger)
    {
        RunTest(2, 2.f, 8.f, 1.f, 2.f);
    }

    TEST_F(PictureLayerTilingSetTestWithResources, ManyTilings_Equal)
    {
        RunTest(10, 1.f, 1.f, 5.f, 5.f);
    }

    TEST_F(PictureLayerTilingSetTestWithResources, ManyTilings_NotEqual)
    {
        RunTest(10, 1.f, 1.f, 4.5f, 5.f);
    }

    TEST(PictureLayerTilingSetTest, TileSizeChange)
    {
        FakePictureLayerTilingClient pending_client;
        FakePictureLayerTilingClient active_client;
        scoped_ptr<PictureLayerTilingSet> pending_set = PictureLayerTilingSet::Create(
            PENDING_TREE, &pending_client, 1000, 1.f, 1000);
        scoped_ptr<PictureLayerTilingSet> active_set = PictureLayerTilingSet::Create(
            ACTIVE_TREE, &active_client, 1000, 1.f, 1000);

        gfx::Size layer_bounds(100, 100);
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);

        gfx::Size tile_size1(10, 10);
        gfx::Size tile_size2(30, 30);
        gfx::Size tile_size3(20, 20);

        pending_client.SetTileSize(tile_size1);
        pending_set->AddTiling(1.f, raster_source);
        // New tilings get the correct tile size.
        EXPECT_EQ(tile_size1, pending_set->tiling_at(0)->tile_size());

        // Set some expected things for the tiling set to function.
        pending_set->tiling_at(0)->set_resolution(HIGH_RESOLUTION);
        active_client.set_twin_tiling_set(pending_set.get());

        // Set a priority rect so we get tiles.
        pending_set->UpdateTilePriorities(gfx::Rect(layer_bounds), 1.f, 1.0,
            Occlusion(), false);
        EXPECT_EQ(tile_size1, pending_set->tiling_at(0)->tile_size());

        // The tiles should get the correct size.
        std::vector<Tile*> pending_tiles = pending_set->tiling_at(0)->AllTilesForTesting();
        EXPECT_GT(pending_tiles.size(), 0u);
        for (const auto& tile : pending_tiles)
            EXPECT_EQ(tile_size1, tile->content_rect().size());

        // Update to a new source frame with a new tile size.
        // Note that setting a new raster source can typically only happen after
        // activation, since we can't set the raster source twice on the pending tree
        // without activating. For test, just remove and add a new tiling instead.
        pending_set->RemoveAllTilings();
        pending_set->AddTiling(1.f, raster_source);
        pending_set->tiling_at(0)->set_resolution(HIGH_RESOLUTION);
        pending_client.SetTileSize(tile_size2);
        pending_set->UpdateTilingsToCurrentRasterSourceForCommit(raster_source.get(),
            Region(), 1.f, 1.f);
        // The tiling should get the correct tile size.
        EXPECT_EQ(tile_size2, pending_set->tiling_at(0)->tile_size());

        // Set a priority rect so we get tiles.
        pending_set->UpdateTilePriorities(gfx::Rect(layer_bounds), 1.f, 2.0,
            Occlusion(), false);
        EXPECT_EQ(tile_size2, pending_set->tiling_at(0)->tile_size());

        // Tiles should have the new correct size.
        pending_tiles = pending_set->tiling_at(0)->AllTilesForTesting();
        EXPECT_GT(pending_tiles.size(), 0u);
        for (const auto& tile : pending_tiles)
            EXPECT_EQ(tile_size2, tile->content_rect().size());

        // Clone from the pending to the active tree.
        active_client.SetTileSize(tile_size2);
        active_set->UpdateTilingsToCurrentRasterSourceForActivation(
            raster_source.get(), pending_set.get(), Region(), 1.f, 1.f);
        // The active tiling should get the right tile size.
        EXPECT_EQ(tile_size2, active_set->tiling_at(0)->tile_size());

        // Cloned tiles should have the right size.
        std::vector<Tile*> active_tiles = active_set->tiling_at(0)->AllTilesForTesting();
        EXPECT_GT(active_tiles.size(), 0u);
        for (const auto& tile : active_tiles)
            EXPECT_EQ(tile_size2, tile->content_rect().size());

        // A new source frame with a new tile size.
        pending_client.SetTileSize(tile_size3);
        pending_set->UpdateTilingsToCurrentRasterSourceForCommit(raster_source.get(),
            Region(), 1.f, 1.f);
        // The tiling gets the new size correctly.
        EXPECT_EQ(tile_size3, pending_set->tiling_at(0)->tile_size());

        // Set a priority rect so we get tiles.
        pending_set->UpdateTilePriorities(gfx::Rect(layer_bounds), 1.f, 3.0,
            Occlusion(), false);
        EXPECT_EQ(tile_size3, pending_set->tiling_at(0)->tile_size());

        // Tiles are resized for the new size.
        pending_tiles = pending_set->tiling_at(0)->AllTilesForTesting();
        EXPECT_GT(pending_tiles.size(), 0u);
        for (const auto& tile : pending_tiles)
            EXPECT_EQ(tile_size3, tile->content_rect().size());

        // Now we activate with a different tile size for the active tiling.
        active_client.SetTileSize(tile_size3);
        active_set->UpdateTilingsToCurrentRasterSourceForActivation(
            raster_source.get(), pending_set.get(), Region(), 1.f, 1.f);
        // The active tiling changes its tile size.
        EXPECT_EQ(tile_size3, active_set->tiling_at(0)->tile_size());

        // And its tiles are resized.
        active_tiles = active_set->tiling_at(0)->AllTilesForTesting();
        EXPECT_GT(active_tiles.size(), 0u);
        for (const auto& tile : active_tiles)
            EXPECT_EQ(tile_size3, tile->content_rect().size());
    }

    TEST(PictureLayerTilingSetTest, MaxContentScale)
    {
        FakePictureLayerTilingClient pending_client;
        FakePictureLayerTilingClient active_client;
        scoped_ptr<PictureLayerTilingSet> pending_set = PictureLayerTilingSet::Create(
            PENDING_TREE, &pending_client, 1000, 1.f, 1000);
        scoped_ptr<PictureLayerTilingSet> active_set = PictureLayerTilingSet::Create(
            ACTIVE_TREE, &active_client, 1000, 1.f, 1000);

        gfx::Size layer_bounds(100, 105);
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateEmpty(layer_bounds);

        // Tilings can be added of any scale, the tiling client can controls this.
        pending_set->AddTiling(1.f, raster_source);
        pending_set->AddTiling(2.f, raster_source);
        pending_set->AddTiling(3.f, raster_source);

        // Set some expected things for the tiling set to function.
        pending_set->tiling_at(0)->set_resolution(HIGH_RESOLUTION);
        active_client.set_twin_tiling_set(pending_set.get());

        // Update to a new source frame with a max content scale that is larger than
        // everything.
        float max_content_scale = 3.f;
        pending_set->UpdateTilingsToCurrentRasterSourceForCommit(
            raster_source.get(), Region(), 1.f, max_content_scale);

        // All the tilings are there still.
        EXPECT_EQ(3u, pending_set->num_tilings());

        // Clone from the pending to the active tree with the same max content size.
        active_set->UpdateTilingsToCurrentRasterSourceForActivation(
            raster_source.get(), pending_set.get(), Region(), 1.f, max_content_scale);
        // All the tilings are on the active tree.
        EXPECT_EQ(3u, active_set->num_tilings());

        // Update to a new source frame with a max content scale that will drop one
        // tiling.
        max_content_scale = 2.9f;
        pending_set->UpdateTilingsToCurrentRasterSourceForCommit(
            raster_source.get(), Region(), 1.f, max_content_scale);
        // All the tilings are there still.
        EXPECT_EQ(2u, pending_set->num_tilings());

        pending_set->tiling_at(0)->set_resolution(HIGH_RESOLUTION);

        // Clone from the pending to the active tree with the same max content size.
        active_set->UpdateTilingsToCurrentRasterSourceForActivation(
            raster_source.get(), pending_set.get(), Region(), 1.f, max_content_scale);
        // All the tilings are on the active tree.
        EXPECT_EQ(2u, active_set->num_tilings());
    }

} // namespace
} // namespace cc
