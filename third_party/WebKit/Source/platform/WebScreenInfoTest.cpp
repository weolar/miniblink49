// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebScreenInfo.h"

#include <gtest/gtest.h>

namespace blink {

TEST(WebScreenInfoTest, Equality)
{
    WebScreenInfo screenInfo1;
    WebScreenInfo screenInfo2;

    EXPECT_EQ(screenInfo1, screenInfo2);

    // Change same values in screenInfo1.
    screenInfo1.deviceScaleFactor = 10.f;
    screenInfo1.depth = 3;
    screenInfo1.depthPerComponent = 2;
    screenInfo1.isMonochrome = false;

    EXPECT_NE(screenInfo1, screenInfo2);

    // Set the same values to screenInfo2, they should be equal now.
    screenInfo2.deviceScaleFactor = 10.f;
    screenInfo2.depth = 3;
    screenInfo2.depthPerComponent = 2;
    screenInfo2.isMonochrome = false;

    EXPECT_EQ(screenInfo1, screenInfo2);

    // Set all the known members.
    screenInfo1.deviceScaleFactor = 2.f;
    screenInfo1.depth = 1;
    screenInfo1.depthPerComponent = 1;
    screenInfo1.isMonochrome = false;
    screenInfo1.rect = WebRect(0, 0, 1024, 1024);
    screenInfo1.availableRect = WebRect(0, 0, 1024, 1024);
    screenInfo1.orientationType = blink::WebScreenOrientationLandscapePrimary;
    screenInfo1.orientationAngle = 90;

    EXPECT_NE(screenInfo1, screenInfo2);

    screenInfo2.deviceScaleFactor = 2.f;
    screenInfo2.depth = 1;
    screenInfo2.depthPerComponent = 1;
    screenInfo2.isMonochrome = false;
    screenInfo2.rect = WebRect(0, 0, 1024, 1024);
    screenInfo2.availableRect = WebRect(0, 0, 1024, 1024);
    screenInfo2.orientationType = blink::WebScreenOrientationLandscapePrimary;
    screenInfo2.orientationAngle = 90;

    EXPECT_EQ(screenInfo1, screenInfo2);
}

} // namespace blink
