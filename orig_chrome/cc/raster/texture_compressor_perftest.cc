// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "cc/debug/lap_timer.h"
#include "cc/raster/texture_compressor.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/perf/perf_test.h"

namespace cc {
namespace {

    const int kTimeLimitMillis = 2000;
    const int kWarmupRuns = 5;
    const int kTimeCheckInterval = 10;

    const int kImageWidth = 256;
    const int kImageHeight = 256;
    const int kImageChannels = 4;
    const int kImageSizeInBytes = kImageWidth * kImageHeight * kImageChannels;

    std::string FormatName(TextureCompressor::Format format)
    {
        switch (format) {
        case TextureCompressor::kFormatETC1:
            return "ETC1";
        }

        NOTREACHED();
        return "";
    }

    std::string QualityName(TextureCompressor::Quality quality)
    {
        switch (quality) {
        case TextureCompressor::kQualityLow:
            return "Low";
        case TextureCompressor::kQualityMedium:
            return "Medium";
        case TextureCompressor::kQualityHigh:
            return "High";
        }

        NOTREACHED();
        return "";
    }

    class TextureCompressorPerfTest
        : public testing::TestWithParam<
              ::testing::tuple<TextureCompressor::Quality,
                  TextureCompressor::Format>> {
    public:
        TextureCompressorPerfTest()
            : timer_(kWarmupRuns,
                base::TimeDelta::FromMilliseconds(kTimeLimitMillis),
                kTimeCheckInterval)
        {
        }

        void SetUp() override
        {
            TextureCompressor::Format format = ::testing::get<1>(GetParam());
            compressor_ = TextureCompressor::Create(format);
        }

        void RunTest(const std::string& name)
        {
            TextureCompressor::Quality quality = ::testing::get<0>(GetParam());
            timer_.Reset();
            do {
                compressor_->Compress(src_, dst_, kImageWidth, kImageHeight, quality);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            TextureCompressor::Format format = ::testing::get<1>(GetParam());
            std::string str = FormatName(format) + " " + QualityName(quality);
            perf_test::PrintResult("Compress256x256", name, str, timer_.MsPerLap(),
                "us", true);
        }

    protected:
        LapTimer timer_;
        scoped_ptr<TextureCompressor> compressor_;
        uint8_t src_[kImageSizeInBytes];
        uint8_t dst_[kImageSizeInBytes];
    };

    TEST_P(TextureCompressorPerfTest, Compress256x256BlackAndWhiteGradientImage)
    {
        for (int i = 0; i < kImageSizeInBytes; ++i)
            src_[i] = i % 256;

        RunTest("BlackAndWhiteGradientImage");
    }

    TEST_P(TextureCompressorPerfTest, Compress256x256SolidBlackImage)
    {
        memset(src_, 0, kImageSizeInBytes);

        RunTest("SolidBlackImage");
    }

    TEST_P(TextureCompressorPerfTest, Compress256x256SolidColorImage)
    {
        for (int i = 0; i < kImageSizeInBytes; ++i)
            src_[i] = (4 - i % 4) * 50;

        RunTest("SolidColorImage");
    }

    TEST_P(TextureCompressorPerfTest, Compress256x256RandomColorImage)
    {
        unsigned int kImageSeed = 1234567890;
        srand(kImageSeed);
        for (int i = 0; i < kImageSizeInBytes; ++i)
            src_[i] = rand() % 256; // NOLINT

        RunTest("RandomColorImage");
    }

    INSTANTIATE_TEST_CASE_P(
        TextureCompressorPerfTests,
        TextureCompressorPerfTest,
        ::testing::Combine(::testing::Values(TextureCompressor::kQualityLow,
                               TextureCompressor::kQualityMedium,
                               TextureCompressor::kQualityHigh),
            ::testing::Values(TextureCompressor::kFormatETC1)));

} // namespace
} // namespace cc
