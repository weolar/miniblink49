// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/texture_compressor.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    const int kImageWidth = 256;
    const int kImageHeight = 256;
    const int kImageChannels = 4;
    const int kImageSizeInBytes = kImageWidth * kImageHeight * kImageChannels;

    TEST(TextureCompressorETC1Test, Compress256x256Ratio)
    {
        scoped_ptr<TextureCompressor> compressor = TextureCompressor::Create(TextureCompressor::kFormatETC1);
        uint8_t src[kImageSizeInBytes];
        uint8_t dst[kImageSizeInBytes];
        const unsigned int kImagePoison = 0xDEADBEEF;

        // Poison destination bytes so we can see how much has been
        // overwritten by compression algorithm.
        uint32_t* dst_32 = reinterpret_cast<uint32_t*>(dst);
        for (int i = 0; i < kImageWidth * kImageHeight; i++) {
            dst_32[i] = kImagePoison;
        }

        // Generate test texture.
        for (int i = 0; i < kImageSizeInBytes; i++) {
            src[i] = i % 256;
        }

        compressor->Compress(src, dst, kImageWidth, kImageHeight,
            TextureCompressor::kQualityLow);

        int compressed_size = 0;
        for (compressed_size = 0; compressed_size < kImageWidth * kImageHeight;
             compressed_size++) {
            if (dst_32[compressed_size] == kImagePoison) {
                // Represents size in bytes of the compressed block.
                compressed_size = compressed_size * 4;
                break;
            }
        }

        // Check if compression ratio is 8:1 for RGBA or BGRA images, after discarding
        // alpha channel.
        EXPECT_EQ(kImageSizeInBytes, compressed_size * 8);
    }

} // namespace
} // namespace cc
