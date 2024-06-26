// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/range/range.h"
#include "ui/gfx/range/range_f.h"

namespace {

template <typename T>
class RangeTest : public testing::Test {
};

typedef testing::Types<gfx::Range, gfx::RangeF> RangeTypes;
TYPED_TEST_CASE(RangeTest, RangeTypes);

template <typename T>
void TestContainsAndIntersects(const T& r1,
    const T& r2,
    const T& r3)
{
    EXPECT_TRUE(r1.Intersects(r1));
    EXPECT_TRUE(r1.Contains(r1));
    EXPECT_EQ(T(10, 12), r1.Intersect(r1));

    EXPECT_FALSE(r1.Intersects(r2));
    EXPECT_FALSE(r1.Contains(r2));
    EXPECT_TRUE(r1.Intersect(r2).is_empty());
    EXPECT_FALSE(r2.Intersects(r1));
    EXPECT_FALSE(r2.Contains(r1));
    EXPECT_TRUE(r2.Intersect(r1).is_empty());

    EXPECT_TRUE(r1.Intersects(r3));
    EXPECT_TRUE(r3.Intersects(r1));
    EXPECT_TRUE(r3.Contains(r1));
    EXPECT_FALSE(r1.Contains(r3));
    EXPECT_EQ(T(10, 12), r1.Intersect(r3));
    EXPECT_EQ(T(10, 12), r3.Intersect(r1));

    EXPECT_TRUE(r2.Intersects(r3));
    EXPECT_TRUE(r3.Intersects(r2));
    EXPECT_FALSE(r3.Contains(r2));
    EXPECT_FALSE(r2.Contains(r3));
    EXPECT_EQ(T(5, 8), r2.Intersect(r3));
    EXPECT_EQ(T(5, 8), r3.Intersect(r2));
}

} // namespace

TYPED_TEST(RangeTest, EmptyInit)
{
    TypeParam r;
    EXPECT_EQ(0U, r.start());
    EXPECT_EQ(0U, r.end());
    EXPECT_EQ(0U, r.length());
    EXPECT_FALSE(r.is_reversed());
    EXPECT_TRUE(r.is_empty());
    EXPECT_TRUE(r.IsValid());
    EXPECT_EQ(0U, r.GetMin());
    EXPECT_EQ(0U, r.GetMax());
}

TYPED_TEST(RangeTest, StartEndInit)
{
    TypeParam r(10, 15);
    EXPECT_EQ(10U, r.start());
    EXPECT_EQ(15U, r.end());
    EXPECT_EQ(5U, r.length());
    EXPECT_FALSE(r.is_reversed());
    EXPECT_FALSE(r.is_empty());
    EXPECT_TRUE(r.IsValid());
    EXPECT_EQ(10U, r.GetMin());
    EXPECT_EQ(15U, r.GetMax());
}

TYPED_TEST(RangeTest, StartEndReversedInit)
{
    TypeParam r(10, 5);
    EXPECT_EQ(10U, r.start());
    EXPECT_EQ(5U, r.end());
    EXPECT_EQ(5U, r.length());
    EXPECT_TRUE(r.is_reversed());
    EXPECT_FALSE(r.is_empty());
    EXPECT_TRUE(r.IsValid());
    EXPECT_EQ(5U, r.GetMin());
    EXPECT_EQ(10U, r.GetMax());
}

TYPED_TEST(RangeTest, PositionInit)
{
    TypeParam r(12);
    EXPECT_EQ(12U, r.start());
    EXPECT_EQ(12U, r.end());
    EXPECT_EQ(0U, r.length());
    EXPECT_FALSE(r.is_reversed());
    EXPECT_TRUE(r.is_empty());
    EXPECT_TRUE(r.IsValid());
    EXPECT_EQ(12U, r.GetMin());
    EXPECT_EQ(12U, r.GetMax());
}

TYPED_TEST(RangeTest, InvalidRange)
{
    TypeParam r(TypeParam::InvalidRange());
    EXPECT_EQ(0U, r.length());
    EXPECT_EQ(r.start(), r.end());
    EXPECT_EQ(r.GetMax(), r.GetMin());
    EXPECT_FALSE(r.is_reversed());
    EXPECT_TRUE(r.is_empty());
    EXPECT_FALSE(r.IsValid());
    EXPECT_EQ(r, TypeParam::InvalidRange());
    EXPECT_TRUE(r.EqualsIgnoringDirection(TypeParam::InvalidRange()));
}

TYPED_TEST(RangeTest, Equality)
{
    TypeParam r1(10, 4);
    TypeParam r2(10, 4);
    TypeParam r3(10, 2);
    EXPECT_EQ(r1, r2);
    EXPECT_NE(r1, r3);
    EXPECT_NE(r2, r3);

    TypeParam r4(11, 4);
    EXPECT_NE(r1, r4);
    EXPECT_NE(r2, r4);
    EXPECT_NE(r3, r4);

    TypeParam r5(12, 5);
    EXPECT_NE(r1, r5);
    EXPECT_NE(r2, r5);
    EXPECT_NE(r3, r5);
}

TYPED_TEST(RangeTest, EqualsIgnoringDirection)
{
    TypeParam r1(10, 5);
    TypeParam r2(5, 10);
    EXPECT_TRUE(r1.EqualsIgnoringDirection(r2));
}

TYPED_TEST(RangeTest, SetStart)
{
    TypeParam r(10, 20);
    EXPECT_EQ(10U, r.start());
    EXPECT_EQ(10U, r.length());

    r.set_start(42);
    EXPECT_EQ(42U, r.start());
    EXPECT_EQ(20U, r.end());
    EXPECT_EQ(22U, r.length());
    EXPECT_TRUE(r.is_reversed());
}

TYPED_TEST(RangeTest, SetEnd)
{
    TypeParam r(10, 13);
    EXPECT_EQ(10U, r.start());
    EXPECT_EQ(3U, r.length());

    r.set_end(20);
    EXPECT_EQ(10U, r.start());
    EXPECT_EQ(20U, r.end());
    EXPECT_EQ(10U, r.length());
}

TYPED_TEST(RangeTest, SetStartAndEnd)
{
    TypeParam r;
    r.set_end(5);
    r.set_start(1);
    EXPECT_EQ(1U, r.start());
    EXPECT_EQ(5U, r.end());
    EXPECT_EQ(4U, r.length());
    EXPECT_EQ(1U, r.GetMin());
    EXPECT_EQ(5U, r.GetMax());
}

TYPED_TEST(RangeTest, ReversedRange)
{
    TypeParam r(10, 5);
    EXPECT_EQ(10U, r.start());
    EXPECT_EQ(5U, r.end());
    EXPECT_EQ(5U, r.length());
    EXPECT_TRUE(r.is_reversed());
    EXPECT_TRUE(r.IsValid());
    EXPECT_EQ(5U, r.GetMin());
    EXPECT_EQ(10U, r.GetMax());
}

TYPED_TEST(RangeTest, SetReversedRange)
{
    TypeParam r(10, 20);
    r.set_start(25);
    EXPECT_EQ(25U, r.start());
    EXPECT_EQ(20U, r.end());
    EXPECT_EQ(5U, r.length());
    EXPECT_TRUE(r.is_reversed());
    EXPECT_TRUE(r.IsValid());

    r.set_end(21);
    EXPECT_EQ(25U, r.start());
    EXPECT_EQ(21U, r.end());
    EXPECT_EQ(4U, r.length());
    EXPECT_TRUE(r.IsValid());
    EXPECT_EQ(21U, r.GetMin());
    EXPECT_EQ(25U, r.GetMax());
}

TYPED_TEST(RangeTest, ContainAndIntersect)
{
    {
        SCOPED_TRACE("contain and intersect");
        TypeParam r1(10, 12);
        TypeParam r2(1, 8);
        TypeParam r3(5, 12);
        TestContainsAndIntersects(r1, r2, r3);
    }
    {
        SCOPED_TRACE("contain and intersect: reversed");
        TypeParam r1(12, 10);
        TypeParam r2(8, 1);
        TypeParam r3(12, 5);
        TestContainsAndIntersects(r1, r2, r3);
    }
    // Invalid rect tests
    TypeParam r1(10, 12);
    TypeParam r2(8, 1);
    TypeParam invalid = r1.Intersect(r2);
    EXPECT_FALSE(invalid.IsValid());
    EXPECT_FALSE(invalid.Contains(invalid));
    EXPECT_FALSE(invalid.Contains(r1));
    EXPECT_FALSE(invalid.Intersects(invalid));
    EXPECT_FALSE(invalid.Intersects(r1));
    EXPECT_FALSE(r1.Contains(invalid));
    EXPECT_FALSE(r1.Intersects(invalid));
}

TEST(RangeTest, RangeFConverterTest)
{
    gfx::RangeF range_f(1.2f, 3.9f);
    gfx::Range range = range_f.Floor();
    EXPECT_EQ(1U, range.start());
    EXPECT_EQ(3U, range.end());

    range = range_f.Ceil();
    EXPECT_EQ(2U, range.start());
    EXPECT_EQ(4U, range.end());

    range = range_f.Round();
    EXPECT_EQ(1U, range.start());
    EXPECT_EQ(4U, range.end());

    // Test for negative values.
    range_f.set_start(-1.2f);
    range_f.set_end(-3.8f);
    range = range_f.Floor();
    EXPECT_EQ(0U, range.start());
    EXPECT_EQ(0U, range.end());

    range = range_f.Ceil();
    EXPECT_EQ(0U, range.start());
    EXPECT_EQ(0U, range.end());

    range = range_f.Round();
    EXPECT_EQ(0U, range.start());
    EXPECT_EQ(0U, range.end());
}
