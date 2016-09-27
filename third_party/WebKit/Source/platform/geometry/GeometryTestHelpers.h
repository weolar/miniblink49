// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GeometryTestHelpers_h
#define GeometryTestHelpers_h

#include <gtest/gtest.h>

namespace blink {
namespace GeometryTest {

bool ApproximatelyEqual(float, float, float testEpsilon);
::testing::AssertionResult AssertAlmostEqual(const char* actual_expr, const char* expected_expr, float actual, float expected, float testEpsilon = 1e-6);

} // namespace GeometryTest
} // namespace blink

#endif
