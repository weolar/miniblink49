// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>

#include "base/mac/scoped_nsobject.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/color_profile.h"
#include "ui/gfx/mac/coordinate_conversion.h"
#import "ui/gfx/test/ui_cocoa_test_helper.h"

namespace {

class ColorProfileTest : public ui::CocoaTest {
 public:
  void SetUp() override {
    ui::CocoaTest::SetUp();

    [test_window() setUseDefaultConstraints:NO];

    // Verify the primary screen origin.
    NSRect primary_screen_frame = PrimaryScreenFrame();
    EXPECT_EQ(0, primary_screen_frame.origin.x);
    EXPECT_EQ(0, primary_screen_frame.origin.y);

    // Move the test window onto the screen.
    MoveTestWindowTo(gfx::Rect(0, 0, 200, 200));

    // Verify it is contained by the screen.
    BOOL screen_contains_test_window = NSContainsRect(
        primary_screen_frame, [test_window() frame]);
    EXPECT_TRUE(screen_contains_test_window);
  }

  void MoveTestWindowTo(gfx::Rect bounds) {
    [test_window() setFrame:gfx::ScreenRectToNSRect(bounds) display:NO];
    EXPECT_EQ(bounds.ToString(), TestWindowBounds().ToString());
  }

  gfx::Rect TestWindowBounds() {
    return gfx::ScreenRectFromNSRect([test_window() frame]);
  }

  BOOL TestWindowOnScreen() {
    return NSIntersectsRect(PrimaryScreenFrame(), [test_window() frame]);
  }

  BOOL TestWindowContainedOnScreen() {
    return NSContainsRect(PrimaryScreenFrame(), [test_window() frame]);
  }

  NSRect PrimaryScreenFrame() {
    return [[[NSScreen screens] firstObject] frame];
  }
};

bool TestColorProfileForBounds(const gfx::Rect& bounds) {
  std::vector<char> color_profile;
  return gfx::GetDisplayColorProfile(bounds, &color_profile);
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForOnScreenBounds) {
  MoveTestWindowTo(gfx::Rect(10, 10, 100, 100));
  EXPECT_FALSE(TestWindowBounds().IsEmpty());
  EXPECT_TRUE(TestWindowContainedOnScreen());
  EXPECT_TRUE(TestColorProfileForBounds(TestWindowBounds()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForPartiallyOnScreenBounds) {
  MoveTestWindowTo(gfx::Rect(-50, -50, 80, 80));
  EXPECT_FALSE(TestWindowBounds().IsEmpty());
  EXPECT_TRUE(TestWindowOnScreen());
  EXPECT_TRUE(TestColorProfileForBounds(TestWindowBounds()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForOffScreenBounds) {
  MoveTestWindowTo(gfx::Rect(-100, -100, 10, 10));
  EXPECT_FALSE(TestWindowBounds().IsEmpty());
  EXPECT_FALSE(TestWindowOnScreen());
  EXPECT_FALSE(TestColorProfileForBounds(TestWindowBounds()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForEmptyOnScreenBounds) {
  MoveTestWindowTo(gfx::Rect(10, 10, 0, 0));
  EXPECT_TRUE(TestWindowBounds().IsEmpty());
  EXPECT_FALSE(TestWindowOnScreen());
  EXPECT_FALSE(TestColorProfileForBounds(TestWindowBounds()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForEmptyOffScreenBounds) {
  MoveTestWindowTo(gfx::Rect(-100, -100, 0, 0));
  EXPECT_TRUE(TestWindowBounds().IsEmpty());
  EXPECT_FALSE(TestWindowOnScreen());
  EXPECT_FALSE(TestColorProfileForBounds(TestWindowBounds()));
}

bool TestColorProfileForWindow(NSWindow* window) {
  std::vector<char> color_profile;
  return gfx::GetDisplayColorProfile(window, &color_profile);
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForOnScreenWindow) {
  MoveTestWindowTo(gfx::Rect(10, 10, 100, 100));
  EXPECT_FALSE(TestWindowBounds().IsEmpty());
  EXPECT_TRUE(TestWindowContainedOnScreen());
  EXPECT_TRUE(TestColorProfileForWindow(test_window()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForPartiallyOnScreenWindow) {
  MoveTestWindowTo(gfx::Rect(-50, -50, 80, 80));
  EXPECT_FALSE(TestWindowBounds().IsEmpty());
  EXPECT_TRUE(TestWindowOnScreen());
  EXPECT_TRUE(TestColorProfileForWindow(test_window()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForOffScreenWindow) {
  MoveTestWindowTo(gfx::Rect(-100, -100, 10, 10));
  EXPECT_FALSE(TestWindowBounds().IsEmpty());
  EXPECT_FALSE(TestWindowOnScreen());
  EXPECT_TRUE(TestColorProfileForWindow(test_window()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForEmptyOnScreenWindow) {
  MoveTestWindowTo(gfx::Rect(10, 10, 0, 0));
  EXPECT_TRUE(TestWindowBounds().IsEmpty());
  EXPECT_FALSE(TestWindowOnScreen());
  EXPECT_FALSE(TestColorProfileForWindow(test_window()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForEmptyOffScreenWindow) {
  MoveTestWindowTo(gfx::Rect(-100, -100, 0, 0));
  EXPECT_TRUE(TestWindowBounds().IsEmpty());
  EXPECT_FALSE(TestWindowOnScreen());
  EXPECT_FALSE(TestColorProfileForWindow(test_window()));
}

TEST_F(ColorProfileTest, GetDisplayColorProfileForNullWindow) {
  EXPECT_FALSE(TestColorProfileForWindow(nullptr));
  EXPECT_FALSE(TestColorProfileForWindow(nil));
}

}  // namespace
