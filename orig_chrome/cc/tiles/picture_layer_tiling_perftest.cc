// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/lap_timer.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/scoped_resource.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_picture_layer_tiling_client.h"
#include "cc/test/fake_resource_provider.h"
#include "cc/test/test_context_provider.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/tiles/picture_layer_tiling.h"
#include "cc/trees/layer_tree_settings.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/perf/perf_test.h"

namespace cc {

namespace {

    static const int kTimeLimitMillis = 2000;
    static const int kWarmupRuns = 5;
    static const int kTimeCheckInterval = 10;

    class PictureLayerTilingPerfTest : public testing::Test {
    public:
        PictureLayerTilingPerfTest()
            : timer_(kWarmupRuns,
                base::TimeDelta::FromMilliseconds(kTimeLimitMillis),
                kTimeCheckInterval)
            , context_provider_(TestContextProvider::Create())
        {
            output_surface_ = FakeOutputSurface::Create3d(context_provider_).Pass();
            CHECK(output_surface_->BindToClient(&output_surface_client_));

            shared_bitmap_manager_.reset(new TestSharedBitmapManager());
            resource_provider_ = FakeResourceProvider::Create(
                output_surface_.get(), shared_bitmap_manager_.get());
        }

        void SetUp() override
        {
            LayerTreeSettings defaults;
            picture_layer_tiling_client_.SetTileSize(gfx::Size(256, 256));
            scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(
                gfx::Size(256 * 50, 256 * 50));
            picture_layer_tiling_ = PictureLayerTiling::Create(
                PENDING_TREE, 1.f, raster_source, &picture_layer_tiling_client_,
                defaults.tiling_interest_area_padding,
                defaults.skewport_target_time_in_seconds,
                defaults.skewport_extrapolation_limit_in_content_pixels);
            picture_layer_tiling_->CreateAllTilesForTesting();
        }

        void TearDown() override { picture_layer_tiling_.reset(NULL); }

        void RunInvalidateTest(const std::string& test_name, const Region& region)
        {
            timer_.Reset();
            do {
                picture_layer_tiling_->Invalidate(region);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult(
                "invalidation", "", test_name, timer_.LapsPerSecond(), "runs/s", true);
        }

        void RunComputeTilePriorityRectsStationaryTest(
            const std::string& test_name,
            const gfx::Transform& transform)
        {
            gfx::Rect viewport_rect(0, 0, 1024, 768);

            timer_.Reset();
            do {
                picture_layer_tiling_->ComputeTilePriorityRects(
                    viewport_rect, 1.f, timer_.NumLaps() + 1, Occlusion());
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult("compute_tile_priority_rects_stationary",
                "",
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

        void RunComputeTilePriorityRectsScrollingTest(
            const std::string& test_name,
            const gfx::Transform& transform)
        {
            gfx::Size viewport_size(1024, 768);
            gfx::Rect viewport_rect(viewport_size);
            int xoffsets[] = { 10, 0, -10, 0 };
            int yoffsets[] = { 0, 10, 0, -10 };
            int offset_index = 0;
            int offset_count = 0;
            const int max_offset_count = 1000;

            timer_.Reset();
            do {
                picture_layer_tiling_->ComputeTilePriorityRects(
                    viewport_rect, 1.f, timer_.NumLaps() + 1, Occlusion());

                viewport_rect = gfx::Rect(viewport_rect.x() + xoffsets[offset_index],
                    viewport_rect.y() + yoffsets[offset_index],
                    viewport_rect.width(), viewport_rect.height());

                if (++offset_count > max_offset_count) {
                    offset_count = 0;
                    offset_index = (offset_index + 1) % 4;
                }
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult("compute_tile_priority_rects_scrolling",
                "",
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

    private:
        FakePictureLayerTilingClient picture_layer_tiling_client_;
        scoped_ptr<PictureLayerTiling> picture_layer_tiling_;

        LapTimer timer_;

        scoped_refptr<ContextProvider> context_provider_;
        FakeOutputSurfaceClient output_surface_client_;
        scoped_ptr<FakeOutputSurface> output_surface_;
        scoped_ptr<SharedBitmapManager> shared_bitmap_manager_;
        scoped_ptr<ResourceProvider> resource_provider_;
    };

    TEST_F(PictureLayerTilingPerfTest, Invalidate)
    {
        Region one_tile(gfx::Rect(256, 256));
        RunInvalidateTest("1x1", one_tile);

        Region half_region(gfx::Rect(25 * 256, 50 * 256));
        RunInvalidateTest("25x50", half_region);

        Region full_region(gfx::Rect(50 * 256, 50 * 256));
        RunInvalidateTest("50x50", full_region);
    }

#if defined(OS_ANDROID)
    // TODO(vmpstr): Investigate why this is noisy (crbug.com/310220).
    TEST_F(PictureLayerTilingPerfTest, DISABLED_ComputeTilePriorityRects)
    {
#else
    TEST_F(PictureLayerTilingPerfTest, ComputeTilePriorityRects)
    {
#endif // defined(OS_ANDROID)
        gfx::Transform transform;

        RunComputeTilePriorityRectsStationaryTest("no_transform", transform);
        RunComputeTilePriorityRectsScrollingTest("no_transform", transform);

        transform.Rotate(10);
        RunComputeTilePriorityRectsStationaryTest("rotation", transform);
        RunComputeTilePriorityRectsScrollingTest("rotation", transform);

        transform.ApplyPerspectiveDepth(10);
        RunComputeTilePriorityRectsStationaryTest("perspective", transform);
        RunComputeTilePriorityRectsScrollingTest("perspective", transform);
    }

} // namespace
} // namespace cc
