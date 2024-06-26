// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/platform_color.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    // Verify SameComponentOrder on this platform.
    TEST(PlatformColorTest, SameComponentOrder)
    {
        bool rgba = !!SK_B32_SHIFT;

        for (size_t i = 0; i <= RESOURCE_FORMAT_MAX; ++i) {
            ResourceFormat format = static_cast<ResourceFormat>(i);
            switch (format) {
            case RGBA_8888:
                EXPECT_EQ(rgba, PlatformColor::SameComponentOrder(format));
                break;
            case RGBA_4444:
                // RGBA_4444 indicates the number of bytes per pixel but the format
                // doesn't actually imply RGBA ordering. It uses the native ordering.
                EXPECT_EQ(true, PlatformColor::SameComponentOrder(format));
                break;
            case BGRA_8888:
                EXPECT_NE(rgba, PlatformColor::SameComponentOrder(format));
                break;
            case ALPHA_8:
            case LUMINANCE_8:
            case RGB_565:
            case ETC1:
            case RED_8:
                EXPECT_FALSE(PlatformColor::SameComponentOrder(format));
                break;
            }
        }
    }

} // namespace
} // namespace cc
