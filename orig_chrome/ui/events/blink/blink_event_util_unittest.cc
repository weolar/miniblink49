// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/blink/blink_event_util.h"

#include "base/time/time.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "ui/events/gesture_event_details.h"

namespace ui {

using BlinkEventUtilTest = testing::Test;

TEST(BlinkEventUtilTest, NoScalingWith1DSF)
{
    const ui::GestureEventDetails details(ui::ET_GESTURE_SCROLL_UPDATE,
        1,
        1);
    auto event = CreateWebGestureEvent(details,
        base::TimeDelta::FromMilliseconds(1),
        gfx::PointF(1.f, 1.f),
        gfx::PointF(1.f, 1.f),
        0);
    EXPECT_FALSE(ScaleWebInputEvent(event, 1.f));
    EXPECT_TRUE(ScaleWebInputEvent(event, 2.f));
}

} // namespace ui
