// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/geometry/FloatSize.h"

#include "platform/geometry/GeometryTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

TEST(FloatSizeTest, DiagonalLengthTest)
{
    // Sanity check the Pythagorean triples 3-4-5 and 5-12-13
    FloatSize s1 = FloatSize(3.f, 4.f);
    EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, s1.diagonalLength(), 5.f);
    FloatSize s2 = FloatSize(5.f, 12.f);
    EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, s2.diagonalLength(), 13.f);

    // Test very small numbers.
    FloatSize s3 = FloatSize(.5e-20f, .5e-20f);
    EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, s3.diagonalLength(), .707106781186548e-20f);

    // Test very large numbers.
    FloatSize s4 = FloatSize(.5e20f, .5e20f);
    EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, s4.diagonalLength(), .707106781186548e20f);
}

} // namespace blink
