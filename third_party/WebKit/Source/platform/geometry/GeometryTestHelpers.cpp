// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/geometry/GeometryTestHelpers.h"

#include <limits>
#include <math.h>

namespace blink {
namespace GeometryTest {

bool ApproximatelyEqual(float a, float b, float testEpsilon)
{
    float absA = ::fabs(a);
    float absB = ::fabs(b);
    float absErr = ::fabs(a - b);
    if (a == b)
        return true;

    if (a == 0 || b == 0 || absErr < std::numeric_limits<float>::min())
        return absErr < (testEpsilon * std::numeric_limits<float>::min());

    return ((absErr / (absA + absB)) < testEpsilon);
}

::testing::AssertionResult AssertAlmostEqual(const char* actual_expr, const char* expected_expr, float actual, float expected, float testEpsilon)
{
    if (!ApproximatelyEqual(actual, expected, testEpsilon)) {
        return ::testing::AssertionFailure() << "       Value of:" << actual_expr << std::endl
            << "         Actual:" << ::testing::PrintToString(actual) << std::endl
            << "Expected Approx:" << expected_expr << std::endl
            << "       Which is:" << ::testing::PrintToString(expected);
    }

    return ::testing::AssertionSuccess();
}

} // namespace GeometryTest
} // namespace blink
