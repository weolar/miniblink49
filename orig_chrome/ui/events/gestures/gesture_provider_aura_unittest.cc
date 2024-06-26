// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/scoped_ptr.h"
#include "base/message_loop/message_loop.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/event_utils.h"
#include "ui/events/gestures/gesture_provider_aura.h"

namespace ui {

class GestureProviderAuraTest : public testing::Test,
                                public GestureProviderAuraClient {
public:
    GestureProviderAuraTest() { }

    ~GestureProviderAuraTest() override { }

    void OnGestureEvent(GestureConsumer* raw_input_consumer,
        GestureEvent* event) override { }

    void SetUp() override
    {
        consumer_.reset(new GestureConsumer());
        provider_.reset(new GestureProviderAura(consumer_.get(), this));
    }

    void TearDown() override { provider_.reset(); }

    GestureProviderAura* provider() { return provider_.get(); }

private:
    scoped_ptr<GestureConsumer> consumer_;
    scoped_ptr<GestureProviderAura> provider_;
    base::MessageLoopForUI message_loop_;
};

TEST_F(GestureProviderAuraTest, IgnoresExtraPressEvents)
{
    base::TimeDelta time = ui::EventTimeForNow();
    TouchEvent press1(ET_TOUCH_PRESSED, gfx::Point(10, 10), 0, time);
    EXPECT_TRUE(provider()->OnTouchEvent(&press1));

    time += base::TimeDelta::FromMilliseconds(10);
    TouchEvent press2(ET_TOUCH_PRESSED, gfx::Point(30, 40), 0, time);
    // TODO(tdresser): this redundant press with same id should be
    // ignored; however, there is at least one case where we need to
    // allow a touch press from a currently used touch id. See
    // crbug.com/373125 for details.
    EXPECT_TRUE(provider()->OnTouchEvent(&press2));
}

TEST_F(GestureProviderAuraTest, IgnoresExtraMoveOrReleaseEvents)
{
    base::TimeDelta time = ui::EventTimeForNow();
    TouchEvent press1(ET_TOUCH_PRESSED, gfx::Point(10, 10), 0, time);
    EXPECT_TRUE(provider()->OnTouchEvent(&press1));

    time += base::TimeDelta::FromMilliseconds(10);
    TouchEvent release1(ET_TOUCH_RELEASED, gfx::Point(30, 40), 0, time);
    EXPECT_TRUE(provider()->OnTouchEvent(&release1));

    time += base::TimeDelta::FromMilliseconds(10);
    TouchEvent release2(ET_TOUCH_RELEASED, gfx::Point(30, 45), 0, time);
    EXPECT_FALSE(provider()->OnTouchEvent(&release1));

    time += base::TimeDelta::FromMilliseconds(10);
    TouchEvent move1(ET_TOUCH_MOVED, gfx::Point(70, 75), 0, time);
    EXPECT_FALSE(provider()->OnTouchEvent(&move1));
}

TEST_F(GestureProviderAuraTest, IgnoresIdenticalMoveEvents)
{
    const float kRadiusX = 20.f;
    const float kRadiusY = 30.f;
    const float kAngle = 0.321f;
    const float kForce = 40.f;
    const int kTouchId0 = 5;
    const int kTouchId1 = 3;

    base::TimeDelta time = ui::EventTimeForNow();
    TouchEvent press0_1(ET_TOUCH_PRESSED, gfx::Point(9, 10), kTouchId0, time);
    EXPECT_TRUE(provider()->OnTouchEvent(&press0_1));

    TouchEvent press1_1(ET_TOUCH_PRESSED, gfx::Point(40, 40), kTouchId1, time);
    EXPECT_TRUE(provider()->OnTouchEvent(&press1_1));

    time += base::TimeDelta::FromMilliseconds(10);
    TouchEvent move0_1(ET_TOUCH_MOVED, gfx::Point(10, 10), 0, kTouchId0, time,
        kRadiusX, kRadiusY, kAngle, kForce);
    EXPECT_TRUE(provider()->OnTouchEvent(&move0_1));

    TouchEvent move1_1(ET_TOUCH_MOVED, gfx::Point(100, 200), 0, kTouchId1, time,
        kRadiusX, kRadiusY, kAngle, kForce);
    EXPECT_TRUE(provider()->OnTouchEvent(&move1_1));

    time += base::TimeDelta::FromMilliseconds(10);
    TouchEvent move0_2(ET_TOUCH_MOVED, gfx::Point(10, 10), 0, kTouchId0, time,
        kRadiusX, kRadiusY, kAngle, kForce);
    // Nothing has changed, so ignore the move.
    EXPECT_FALSE(provider()->OnTouchEvent(&move0_2));

    TouchEvent move1_2(ET_TOUCH_MOVED, gfx::Point(100, 200), 0, kTouchId1, time,
        kRadiusX, kRadiusY, kAngle, kForce);
    // Nothing has changed, so ignore the move.
    EXPECT_FALSE(provider()->OnTouchEvent(&move1_2));

    time += base::TimeDelta::FromMilliseconds(10);
    TouchEvent move0_3(ET_TOUCH_MOVED, gfx::Point(), 0, kTouchId0, time, kRadiusX,
        kRadiusY, kAngle, kForce);
    move0_3.set_location_f(gfx::PointF(70, 75.1f));
    move0_3.set_root_location_f(gfx::PointF(70, 75.1f));
    // Position has changed, so don't ignore the move.
    EXPECT_TRUE(provider()->OnTouchEvent(&move0_3));

    time += base::TimeDelta::FromMilliseconds(10);
    TouchEvent move0_4(ET_TOUCH_MOVED, gfx::Point(), 0, kTouchId0, time, kRadiusX,
        kRadiusY + 1, kAngle, kForce);
    move0_4.set_location_f(gfx::PointF(70, 75.1f));
    move0_4.set_root_location_f(gfx::PointF(70, 75.1f));
}

// TODO(jdduke): Test whether event marked as scroll trigger.

} // namespace ui
