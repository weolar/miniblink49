// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TransformTestHelper_h
#define TransformTestHelper_h

#include "platform/PlatformExport.h"
#include "platform/transforms/TransformationMatrix.h"
#include "wtf/text/WTFString.h"

#include <gtest/gtest.h>

// This file should only be included in test source files.

#define EXPECT_TRANSFORMS_ALMOST_EQ(a, b) \
    EXPECT_PRED_FORMAT2(::blink::TransformTestHelper::transformsAreAlmostEqual, a, b)
#define ASSERT_TRANSFORMS_ALMOST_EQ(a, b) \
    ASSERT_PRED_FORMAT2(::blink::TransformTestHelper::transformsAreAlmostEqual, a, b)

namespace blink {
namespace TransformTestHelper {

// These are helpers for the macros above.

bool floatsAreAlmostEqual(float a, float b)
{
    // This trick was taken from ui/gfx/test/gfx_util.cc.
    return ::testing::FloatLE("a", "b", a, b) && ::testing::FloatLE("b", "a", b, a);
}

String transformationMatrixToString(const TransformationMatrix& matrix)
{
    return String::format(
        "(row-major) [ [ %.2f %.2f %.2f %.2f ] [ %.2f %.2f %.2f %.2f ] [ %.2f %.2f %.2f %.2f ] [ %.2f %.2f %.2f %.2f ] ]",
        matrix.m11(), matrix.m21(), matrix.m31(), matrix.m41(),
        matrix.m12(), matrix.m22(), matrix.m32(), matrix.m42(),
        matrix.m13(), matrix.m23(), matrix.m33(), matrix.m43(),
        matrix.m14(), matrix.m24(), matrix.m34(), matrix.m44());
}

const TransformationMatrix& toTransformationMatrix(const TransformationMatrix& matrix) { return matrix; }

TransformationMatrix toTransformationMatrix(const SkMatrix44& skMatrix44)
{
    TransformationMatrix matrix;
    matrix.setMatrix(
        skMatrix44.getDouble(0, 0),
        skMatrix44.getDouble(1, 0),
        skMatrix44.getDouble(2, 0),
        skMatrix44.getDouble(3, 0),
        skMatrix44.getDouble(0, 1),
        skMatrix44.getDouble(1, 1),
        skMatrix44.getDouble(2, 1),
        skMatrix44.getDouble(3, 1),
        skMatrix44.getDouble(0, 2),
        skMatrix44.getDouble(1, 2),
        skMatrix44.getDouble(2, 2),
        skMatrix44.getDouble(3, 2),
        skMatrix44.getDouble(0, 3),
        skMatrix44.getDouble(1, 3),
        skMatrix44.getDouble(2, 3),
        skMatrix44.getDouble(3, 3));
    return matrix;
}

template <typename T1, typename T2>
::testing::AssertionResult transformsAreAlmostEqual(
    const char* lhsExpr, const char* rhsExpr,
    const T1& lhs, const T2& rhs)
{
    return transformsAreAlmostEqual(lhsExpr, rhsExpr, toTransformationMatrix(lhs), toTransformationMatrix(rhs));
}

template <>
::testing::AssertionResult transformsAreAlmostEqual(
    const char* lhsExpr, const char* rhsExpr,
    const TransformationMatrix& lhs, const TransformationMatrix& rhs)
{
    if (lhs == rhs)
        return ::testing::AssertionSuccess();

    if (floatsAreAlmostEqual(lhs.m11(), rhs.m11())
        && floatsAreAlmostEqual(lhs.m12(), rhs.m12())
        && floatsAreAlmostEqual(lhs.m13(), rhs.m13())
        && floatsAreAlmostEqual(lhs.m14(), rhs.m14())
        && floatsAreAlmostEqual(lhs.m21(), rhs.m21())
        && floatsAreAlmostEqual(lhs.m22(), rhs.m22())
        && floatsAreAlmostEqual(lhs.m23(), rhs.m23())
        && floatsAreAlmostEqual(lhs.m24(), rhs.m24())
        && floatsAreAlmostEqual(lhs.m31(), rhs.m31())
        && floatsAreAlmostEqual(lhs.m32(), rhs.m32())
        && floatsAreAlmostEqual(lhs.m33(), rhs.m33())
        && floatsAreAlmostEqual(lhs.m34(), rhs.m34())
        && floatsAreAlmostEqual(lhs.m41(), rhs.m41())
        && floatsAreAlmostEqual(lhs.m42(), rhs.m42())
        && floatsAreAlmostEqual(lhs.m43(), rhs.m43())
        && floatsAreAlmostEqual(lhs.m44(), rhs.m44()))
        return ::testing::AssertionSuccess();

    return ::testing::AssertionFailure()
        << "Value of: " << rhsExpr
        << "\n  Actual: " << transformationMatrixToString(rhs).ascii().data()
        << "\nExpected: " << lhsExpr
        << "\nWhich is: " << transformationMatrixToString(lhs).ascii().data();
}

} // namespace TransformTestHelper
} // namespace blink

#endif // TransformTestHelper_h
