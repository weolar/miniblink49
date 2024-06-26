// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/picture_pile_impl.h"

#include "cc/debug/lap_timer.h"
#include "cc/test/fake_picture_pile_impl.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/perf/perf_test.h"

namespace cc {
namespace {

    const int kTimeLimitMillis = 2000;
    const int kWarmupRuns = 5;
    const int kTimeCheckInterval = 10;

    const int kTileSize = 100;
    const int kLayerSize = 1000;

    class PicturePileImplPerfTest : public testing::Test {
    public:
        PicturePileImplPerfTest()
            : timer_(kWarmupRuns,
                base::TimeDelta::FromMilliseconds(kTimeLimitMillis),
                kTimeCheckInterval)
        {
        }

        void RunAnalyzeTest(const std::string& test_name, float contents_scale)
        {
            scoped_refptr<PicturePileImpl> pile = FakePicturePileImpl::CreateFilledPile(
                gfx::Size(kTileSize, kTileSize), gfx::Size(kLayerSize, kLayerSize));
            // Content rect that will align with top-left tile at scale 1.0.
            gfx::Rect content_rect(0, 0, kTileSize, kTileSize);

            RasterSource::SolidColorAnalysis analysis;
            timer_.Reset();
            do {
                pile->PerformSolidColorAnalysis(content_rect, contents_scale, &analysis);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult(
                "analyze", "", test_name, timer_.LapsPerSecond(), "runs/s", true);
        }

        void RunRasterTest(const std::string& test_name, float contents_scale)
        {
            scoped_refptr<PicturePileImpl> pile = FakePicturePileImpl::CreateFilledPile(
                gfx::Size(kTileSize, kTileSize), gfx::Size(kLayerSize, kLayerSize));
            // Content rect that will align with top-left tile at scale 1.0.
            gfx::Rect content_rect(0, 0, kTileSize, kTileSize);

            SkBitmap bitmap;
            bitmap.allocN32Pixels(1, 1);
            SkCanvas canvas(bitmap);

            timer_.Reset();
            do {
                pile->PlaybackToCanvas(&canvas, content_rect, content_rect,
                    contents_scale);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult(
                "raster", "", test_name, timer_.LapsPerSecond(), "runs/s", true);
        }

    private:
        LapTimer timer_;
    };

    TEST_F(PicturePileImplPerfTest, Analyze)
    {
        RunAnalyzeTest("1", 1.0f);
        RunAnalyzeTest("4", 0.5f);
        RunAnalyzeTest("100", 0.1f);
    }

    TEST_F(PicturePileImplPerfTest, Raster)
    {
        RunRasterTest("1", 1.0f);
        RunRasterTest("4", 0.5f);
        RunRasterTest("100", 0.1f);
    }

} // namespace
} // namespace cc
