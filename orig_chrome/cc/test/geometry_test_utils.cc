// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/geometry_test_utils.h"

#include "base/logging.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/transform.h"

namespace cc {

// NOTE: even though transform data types use double precision, we only check
// for equality within single-precision error bounds because many transforms
// originate from single-precision data types such as quads/rects/etc.

void ExpectTransformationMatrixEq(const gfx::Transform& expected,
    const gfx::Transform& actual)
{
    EXPECT_FLOAT_EQ((expected).matrix().get(0, 0), (actual).matrix().get(0, 0));
    EXPECT_FLOAT_EQ((expected).matrix().get(1, 0), (actual).matrix().get(1, 0));
    EXPECT_FLOAT_EQ((expected).matrix().get(2, 0), (actual).matrix().get(2, 0));
    EXPECT_FLOAT_EQ((expected).matrix().get(3, 0), (actual).matrix().get(3, 0));
    EXPECT_FLOAT_EQ((expected).matrix().get(0, 1), (actual).matrix().get(0, 1));
    EXPECT_FLOAT_EQ((expected).matrix().get(1, 1), (actual).matrix().get(1, 1));
    EXPECT_FLOAT_EQ((expected).matrix().get(2, 1), (actual).matrix().get(2, 1));
    EXPECT_FLOAT_EQ((expected).matrix().get(3, 1), (actual).matrix().get(3, 1));
    EXPECT_FLOAT_EQ((expected).matrix().get(0, 2), (actual).matrix().get(0, 2));
    EXPECT_FLOAT_EQ((expected).matrix().get(1, 2), (actual).matrix().get(1, 2));
    EXPECT_FLOAT_EQ((expected).matrix().get(2, 2), (actual).matrix().get(2, 2));
    EXPECT_FLOAT_EQ((expected).matrix().get(3, 2), (actual).matrix().get(3, 2));
    EXPECT_FLOAT_EQ((expected).matrix().get(0, 3), (actual).matrix().get(0, 3));
    EXPECT_FLOAT_EQ((expected).matrix().get(1, 3), (actual).matrix().get(1, 3));
    EXPECT_FLOAT_EQ((expected).matrix().get(2, 3), (actual).matrix().get(2, 3));
    EXPECT_FLOAT_EQ((expected).matrix().get(3, 3), (actual).matrix().get(3, 3));
}

gfx::Transform Inverse(const gfx::Transform& transform)
{
    gfx::Transform result(gfx::Transform::kSkipInitialization);
    bool inverted_successfully = transform.GetInverse(&result);
    DCHECK(inverted_successfully);
    return result;
}

} // namespace cc
