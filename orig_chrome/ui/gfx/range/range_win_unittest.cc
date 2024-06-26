// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/range/range.h"

TEST(RangeTest, FromCHARRANGE)
{
    CHARRANGE cr = { 10, 32 };
    gfx::Range r(cr, 50);
    EXPECT_EQ(10U, r.start());
    EXPECT_EQ(32U, r.end());
    EXPECT_EQ(22U, r.length());
    EXPECT_FALSE(r.is_reversed());
    EXPECT_TRUE(r.IsValid());
}

TEST(RangeTest, FromReversedCHARRANGE)
{
    CHARRANGE cr = { 20, 10 };
    gfx::Range r(cr, 40);
    EXPECT_EQ(20U, r.start());
    EXPECT_EQ(10U, r.end());
    EXPECT_EQ(10U, r.length());
    EXPECT_TRUE(r.is_reversed());
    EXPECT_TRUE(r.IsValid());
}

TEST(RangeTest, FromCHARRANGETotal)
{
    CHARRANGE cr = { 0, -1 };
    gfx::Range r(cr, 20);
    EXPECT_EQ(0U, r.start());
    EXPECT_EQ(20U, r.end());
    EXPECT_EQ(20U, r.length());
    EXPECT_FALSE(r.is_reversed());
    EXPECT_TRUE(r.IsValid());
}

TEST(RangeTest, ToCHARRANGE)
{
    gfx::Range r(10, 30);
    CHARRANGE cr = r.ToCHARRANGE();
    EXPECT_EQ(10, cr.cpMin);
    EXPECT_EQ(30, cr.cpMax);
}

TEST(RangeTest, ReversedToCHARRANGE)
{
    gfx::Range r(20, 10);
    CHARRANGE cr = r.ToCHARRANGE();
    EXPECT_EQ(20, cr.cpMin);
    EXPECT_EQ(10, cr.cpMax);
}

TEST(RangeTest, FromCHARRANGEInvalid)
{
    CHARRANGE cr = { -1, -1 };
    gfx::Range r(cr, 30);
    EXPECT_FALSE(r.IsValid());
}

TEST(RangeTest, ToCHARRANGEInvalid)
{
    gfx::Range r(gfx::Range::InvalidRange());
    CHARRANGE cr = r.ToCHARRANGE();
    EXPECT_EQ(-1, cr.cpMin);
    EXPECT_EQ(-1, cr.cpMax);
}
