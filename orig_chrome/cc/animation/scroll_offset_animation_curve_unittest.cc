// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/scroll_offset_animation_curve.h"

#include "cc/animation/timing_function.h"
#include "cc/base/time_util.h"
#include "cc/test/geometry_test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    TEST(ScrollOffsetAnimationCurveTest, Duration)
    {
        gfx::ScrollOffset target_value(100.f, 200.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value,
                EaseInOutTimingFunction::Create().Pass()));

        curve->SetInitialValue(target_value);
        EXPECT_DOUBLE_EQ(0.0, curve->Duration().InSecondsF());

        // x decreases, y stays the same.
        curve->SetInitialValue(gfx::ScrollOffset(136.f, 200.f));
        EXPECT_DOUBLE_EQ(0.1, curve->Duration().InSecondsF());

        // x increases, y stays the same.
        curve->SetInitialValue(gfx::ScrollOffset(19.f, 200.f));
        EXPECT_DOUBLE_EQ(0.15, curve->Duration().InSecondsF());

        // x stays the same, y decreases.
        curve->SetInitialValue(gfx::ScrollOffset(100.f, 344.f));
        EXPECT_DOUBLE_EQ(0.2, curve->Duration().InSecondsF());

        // x stays the same, y increases.
        curve->SetInitialValue(gfx::ScrollOffset(100.f, 191.f));
        EXPECT_DOUBLE_EQ(0.05, curve->Duration().InSecondsF());

        // x decreases, y decreases.
        curve->SetInitialValue(gfx::ScrollOffset(32500.f, 500.f));
        EXPECT_DOUBLE_EQ(3.0, curve->Duration().InSecondsF());

        // x decreases, y increases.
        curve->SetInitialValue(gfx::ScrollOffset(150.f, 119.f));
        EXPECT_DOUBLE_EQ(0.15, curve->Duration().InSecondsF());

        // x increases, y decreases.
        curve->SetInitialValue(gfx::ScrollOffset(0.f, 14600.f));
        EXPECT_DOUBLE_EQ(2.0, curve->Duration().InSecondsF());

        // x increases, y increases.
        curve->SetInitialValue(gfx::ScrollOffset(95.f, 191.f));
        EXPECT_DOUBLE_EQ(0.05, curve->Duration().InSecondsF());
    }

    TEST(ScrollOffsetAnimationCurveTest, GetValue)
    {
        gfx::ScrollOffset initial_value(2.f, 40.f);
        gfx::ScrollOffset target_value(10.f, 20.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value,
                EaseInOutTimingFunction::Create().Pass()));
        curve->SetInitialValue(initial_value);

        base::TimeDelta duration = curve->Duration();
        EXPECT_GT(curve->Duration().InSecondsF(), 0);
        EXPECT_LT(curve->Duration().InSecondsF(), 0.1);

        EXPECT_EQ(AnimationCurve::SCROLL_OFFSET, curve->Type());
        EXPECT_EQ(duration, curve->Duration());

        EXPECT_VECTOR2DF_EQ(initial_value,
            curve->GetValue(base::TimeDelta::FromSecondsD(-1.0)));
        EXPECT_VECTOR2DF_EQ(initial_value, curve->GetValue(base::TimeDelta()));
        EXPECT_VECTOR2DF_NEAR(gfx::ScrollOffset(6.f, 30.f),
            curve->GetValue(TimeUtil::Scale(duration, 0.5f)),
            0.00025);
        EXPECT_VECTOR2DF_EQ(target_value, curve->GetValue(duration));
        EXPECT_VECTOR2DF_EQ(
            target_value,
            curve->GetValue(duration + base::TimeDelta::FromSecondsD(1.0)));

        // Verify that GetValue takes the timing function into account.
        gfx::ScrollOffset value = curve->GetValue(TimeUtil::Scale(duration, 0.25f));
        EXPECT_NEAR(3.0333f, value.x(), 0.0002f);
        EXPECT_NEAR(37.4168f, value.y(), 0.0002f);
    }

    // Verify that a clone behaves exactly like the original.
    TEST(ScrollOffsetAnimationCurveTest, Clone)
    {
        gfx::ScrollOffset initial_value(2.f, 40.f);
        gfx::ScrollOffset target_value(10.f, 20.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value,
                EaseInOutTimingFunction::Create().Pass()));
        curve->SetInitialValue(initial_value);
        base::TimeDelta duration = curve->Duration();

        scoped_ptr<AnimationCurve> clone(curve->Clone().Pass());

        EXPECT_EQ(AnimationCurve::SCROLL_OFFSET, clone->Type());
        EXPECT_EQ(duration, clone->Duration());

        EXPECT_VECTOR2DF_EQ(initial_value,
            clone->ToScrollOffsetAnimationCurve()->GetValue(
                base::TimeDelta::FromSecondsD(-1.0)));
        EXPECT_VECTOR2DF_EQ(
            initial_value,
            clone->ToScrollOffsetAnimationCurve()->GetValue(base::TimeDelta()));
        EXPECT_VECTOR2DF_NEAR(gfx::ScrollOffset(6.f, 30.f),
            clone->ToScrollOffsetAnimationCurve()->GetValue(
                TimeUtil::Scale(duration, 0.5f)),
            0.00025);
        EXPECT_VECTOR2DF_EQ(
            target_value, clone->ToScrollOffsetAnimationCurve()->GetValue(duration));
        EXPECT_VECTOR2DF_EQ(target_value,
            clone->ToScrollOffsetAnimationCurve()->GetValue(
                duration + base::TimeDelta::FromSecondsD(1.f)));

        // Verify that the timing function was cloned correctly.
        gfx::ScrollOffset value = clone->ToScrollOffsetAnimationCurve()->GetValue(
            TimeUtil::Scale(duration, 0.25f));
        EXPECT_NEAR(3.0333f, value.x(), 0.0002f);
        EXPECT_NEAR(37.4168f, value.y(), 0.0002f);
    }

    TEST(ScrollOffsetAnimationCurveTest, UpdateTarget)
    {
        gfx::ScrollOffset initial_value(0.f, 0.f);
        gfx::ScrollOffset target_value(0.f, 3600.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value, EaseInOutTimingFunction::Create().Pass()));
        curve->SetInitialValue(initial_value);
        EXPECT_EQ(1.0, curve->Duration().InSecondsF());
        EXPECT_EQ(1800.0, curve->GetValue(base::TimeDelta::FromSecondsD(0.5)).y());
        EXPECT_EQ(3600.0, curve->GetValue(base::TimeDelta::FromSecondsD(1.0)).y());

        curve->UpdateTarget(0.5, gfx::ScrollOffset(0.0, 9900.0));

        EXPECT_EQ(2.0, curve->Duration().InSecondsF());
        EXPECT_EQ(1800.0, curve->GetValue(base::TimeDelta::FromSecondsD(0.5)).y());
        EXPECT_NEAR(5566.49, curve->GetValue(base::TimeDelta::FromSecondsD(1.0)).y(),
            0.01);
        EXPECT_EQ(9900.0, curve->GetValue(base::TimeDelta::FromSecondsD(2.0)).y());

        curve->UpdateTarget(1.0, gfx::ScrollOffset(0.0, 7200.0));

        EXPECT_NEAR(1.674, curve->Duration().InSecondsF(), 0.01);
        EXPECT_NEAR(5566.49, curve->GetValue(base::TimeDelta::FromSecondsD(1.0)).y(),
            0.01);
        EXPECT_EQ(7200.0, curve->GetValue(base::TimeDelta::FromSecondsD(1.674)).y());
    }

    TEST(ScrollOffsetAnimationCurveTest, UpdateTargetWithLargeVelocity)
    {
        gfx::ScrollOffset initial_value(0.f, 0.f);
        gfx::ScrollOffset target_value(0.f, 900.f);
        scoped_ptr<ScrollOffsetAnimationCurve> curve(
            ScrollOffsetAnimationCurve::Create(
                target_value, EaseInOutTimingFunction::Create().Pass()));
        curve->SetInitialValue(initial_value);
        EXPECT_EQ(0.5, curve->Duration().InSecondsF());

        EXPECT_EQ(450.0, curve->GetValue(base::TimeDelta::FromSecondsD(0.25)).y());

        // This leads to a new computed velocity larger than 5000.
        curve->UpdateTarget(0.25, gfx::ScrollOffset(0.0, 450.0001));

        EXPECT_NEAR(0.25015, curve->Duration().InSecondsF(), 0.0001);
        EXPECT_NEAR(450.0,
            curve->GetValue(base::TimeDelta::FromSecondsD(0.22501)).y(),
            0.001);
        EXPECT_NEAR(450.0,
            curve->GetValue(base::TimeDelta::FromSecondsD(0.225015)).y(),
            0.001);
    }

} // namespace
} // namespace cc
