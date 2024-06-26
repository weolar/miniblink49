// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/format_macros.h"
#include "base/macros.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/ozone/evdev/event_device_info.h"
#include "ui/events/ozone/evdev/event_device_test_util.h"
#include "ui/events/ozone/evdev/event_device_util.h"

namespace ui {

class EventDeviceInfoTest : public testing::Test {
public:
    EventDeviceInfoTest();

private:
    DISALLOW_COPY_AND_ASSIGN(EventDeviceInfoTest);
};

EventDeviceInfoTest::EventDeviceInfoTest()
{
}

TEST_F(EventDeviceInfoTest, BasicCrosKeyboard)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kLinkKeyboard, &devinfo));

    EXPECT_TRUE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_FALSE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, BasicCrosTouchscreen)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kLinkTouchscreen, &devinfo));

    EXPECT_FALSE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_TRUE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, BasicCrosTouchpad)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kLinkTouchpad, &devinfo));

    EXPECT_FALSE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_TRUE(devinfo.HasTouchpad());
    EXPECT_FALSE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, BasicUsbKeyboard)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kHpUsbKeyboard, &devinfo));

    EXPECT_TRUE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_FALSE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, BasicUsbKeyboard_Extra)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kHpUsbKeyboard_Extra, &devinfo));

    EXPECT_FALSE(devinfo.HasKeyboard()); // Has keys, but not a full keyboard.
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_FALSE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, BasicUsbMouse)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kLogitechUsbMouse, &devinfo));

    EXPECT_FALSE(devinfo.HasKeyboard());
    EXPECT_TRUE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_FALSE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, BasicUsbTouchscreen)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kMimoTouch2Touchscreen, &devinfo));

    EXPECT_FALSE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_TRUE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, BasicUsbTablet)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kWacomIntuosPtS_Pen, &devinfo));

    EXPECT_FALSE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_FALSE(devinfo.HasTouchscreen());
    EXPECT_TRUE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, BasicUsbTouchpad)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kWacomIntuosPtS_Finger, &devinfo));

    EXPECT_FALSE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_TRUE(devinfo.HasTouchpad());
    EXPECT_FALSE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, HybridKeyboardWithMouse)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kLogitechTouchKeyboardK400, &devinfo));

    // The touchpad actually exposes mouse (relative) Events.
    EXPECT_TRUE(devinfo.HasKeyboard());
    EXPECT_TRUE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_FALSE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, AbsoluteMouseTouchscreen)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kElo_TouchSystems_2700, &devinfo));

    // This touchscreen uses BTN_LEFT for touch contact.
    EXPECT_FALSE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_TRUE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

TEST_F(EventDeviceInfoTest, OnScreenStylus)
{
    EventDeviceInfo devinfo;
    EXPECT_TRUE(CapabilitiesToDeviceInfo(kWilsonBeachActiveStylus, &devinfo));

    EXPECT_FALSE(devinfo.HasKeyboard());
    EXPECT_FALSE(devinfo.HasMouse());
    EXPECT_FALSE(devinfo.HasTouchpad());
    EXPECT_TRUE(devinfo.HasTouchscreen());
    EXPECT_FALSE(devinfo.HasTablet());
}

} // namespace ui
