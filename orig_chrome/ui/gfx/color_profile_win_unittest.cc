// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/color_profile.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace {

bool TestColorProfileUsingScreenBounds(const gfx::Rect& bounds)
{
    std::vector<char> color_profile;
    return gfx::GetDisplayColorProfile(bounds, &color_profile);
}

TEST(ColorProfileTest, GetDisplayColorProfile)
{
    const gfx::Rect in_screen_bounds(10, 10, 100, 100);
    EXPECT_TRUE(TestColorProfileUsingScreenBounds(in_screen_bounds));
}

TEST(ColorProfileTest, GetDisplayColorProfileForOffScreenBounds)
{
    const gfx::Rect off_screen_bounds(-100, -100, 10, 10);
    EXPECT_FALSE(TestColorProfileUsingScreenBounds(off_screen_bounds));
}

TEST(ColorProfileTest, GetDisplayColorProfileForEmptyBounds)
{
    const gfx::Rect empty_screen_bounds(10, 10, 0, 0);
    EXPECT_TRUE(empty_screen_bounds.IsEmpty());
    EXPECT_FALSE(TestColorProfileUsingScreenBounds(empty_screen_bounds));
}

} // namespace
