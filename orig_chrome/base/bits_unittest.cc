// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the unit tests for the bit utilities.

#include "base/bits.h"

#include <limits>

#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace bits {

    TEST(BitsTest, Log2Floor)
    {
        EXPECT_EQ(-1, Log2Floor(0));
        EXPECT_EQ(0, Log2Floor(1));
        EXPECT_EQ(1, Log2Floor(2));
        EXPECT_EQ(1, Log2Floor(3));
        EXPECT_EQ(2, Log2Floor(4));
        for (int i = 3; i < 31; ++i) {
            unsigned int value = 1U << i;
            EXPECT_EQ(i, Log2Floor(value));
            EXPECT_EQ(i, Log2Floor(value + 1));
            EXPECT_EQ(i, Log2Floor(value + 2));
            EXPECT_EQ(i - 1, Log2Floor(value - 1));
            EXPECT_EQ(i - 1, Log2Floor(value - 2));
        }
        EXPECT_EQ(31, Log2Floor(0xffffffffU));
    }

    TEST(BitsTest, Log2Ceiling)
    {
        EXPECT_EQ(-1, Log2Ceiling(0));
        EXPECT_EQ(0, Log2Ceiling(1));
        EXPECT_EQ(1, Log2Ceiling(2));
        EXPECT_EQ(2, Log2Ceiling(3));
        EXPECT_EQ(2, Log2Ceiling(4));
        for (int i = 3; i < 31; ++i) {
            unsigned int value = 1U << i;
            EXPECT_EQ(i, Log2Ceiling(value));
            EXPECT_EQ(i + 1, Log2Ceiling(value + 1));
            EXPECT_EQ(i + 1, Log2Ceiling(value + 2));
            EXPECT_EQ(i, Log2Ceiling(value - 1));
            EXPECT_EQ(i, Log2Ceiling(value - 2));
        }
        EXPECT_EQ(32, Log2Ceiling(0xffffffffU));
    }

    TEST(BitsTest, Align)
    {
        const size_t kSizeTMax = std::numeric_limits<size_t>::max();
        EXPECT_EQ(0ul, Align(0, 4));
        EXPECT_EQ(4ul, Align(1, 4));
        EXPECT_EQ(4096ul, Align(1, 4096));
        EXPECT_EQ(4096ul, Align(4096, 4096));
        EXPECT_EQ(4096ul, Align(4095, 4096));
        EXPECT_EQ(8192ul, Align(4097, 4096));
        EXPECT_EQ(kSizeTMax - 31, Align(kSizeTMax - 62, 32));
        EXPECT_EQ(kSizeTMax / 2 + 1, Align(1, kSizeTMax / 2 + 1));
    }

} // namespace bits
} // namespace base
