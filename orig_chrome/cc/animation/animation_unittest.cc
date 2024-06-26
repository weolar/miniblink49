// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation.h"

#include "cc/test/animation_test_common.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    using base::TimeDelta;

    static base::TimeTicks TicksFromSecondsF(double seconds)
    {
        return base::TimeTicks::FromInternalValue(seconds * base::Time::kMicrosecondsPerSecond);
    }

    scoped_ptr<Animation> CreateAnimation(double iterations,
        double duration,
        double playback_rate)
    {
        scoped_ptr<Animation> to_return(
            Animation::Create(make_scoped_ptr(new FakeFloatAnimationCurve(duration)),
                0, 1, Animation::OPACITY));
        to_return->set_iterations(iterations);
        to_return->set_playback_rate(playback_rate);
        return to_return.Pass();
    }

    scoped_ptr<Animation> CreateAnimation(double iterations, double duration)
    {
        return CreateAnimation(iterations, duration, 1);
    }

    scoped_ptr<Animation> CreateAnimation(double iterations)
    {
        return CreateAnimation(iterations, 1, 1);
    }

    TEST(AnimationTest, TrimTimeZeroIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(0));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeOneIteration)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeOneHalfIteration)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1.5));
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.9, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.9)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeInfiniteIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(-1));
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(-1));
        anim->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_EQ(
            1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateInfiniteIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(-1));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateOneIteration)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateTwoIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateTwoHalfIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2.5));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.50)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.75)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateReverseInfiniteIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(-1));
        anim->set_direction(Animation::DIRECTION_ALTERNATE_REVERSE);
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateReverseOneIteration)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_direction(Animation::DIRECTION_ALTERNATE_REVERSE);
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateReverseTwoIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2));
        anim->set_direction(Animation::DIRECTION_ALTERNATE_REVERSE);
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.75)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeStartTime)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_start_time(TicksFromSecondsF(4));
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.5)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(5.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(6.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeStartTimeReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_start_time(TicksFromSecondsF(4));
        anim->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.5)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(5.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(6.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeTimeOffset)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_time_offset(TimeDelta::FromMilliseconds(4000));
        anim->set_start_time(TicksFromSecondsF(4));
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeTimeOffsetReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_time_offset(TimeDelta::FromMilliseconds(4000));
        anim->set_start_time(TicksFromSecondsF(4));
        anim->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeNegativeTimeOffset)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_time_offset(TimeDelta::FromMilliseconds(-4000));

        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.5)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(5.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeNegativeTimeOffsetReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_time_offset(TimeDelta::FromMilliseconds(-4000));
        anim->set_direction(Animation::DIRECTION_REVERSE);

        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.5)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(5.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePauseResume)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        anim->SetRunState(Animation::PAUSED, TicksFromSecondsF(0.5));
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.0)).InSecondsF());
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(1024.0));
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.0)).InSecondsF());
        EXPECT_EQ(1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePauseResumeReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_direction(Animation::DIRECTION_REVERSE);
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        anim->SetRunState(Animation::PAUSED, TicksFromSecondsF(0.25));
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.0)).InSecondsF());
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(1024.0));
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.0)).InSecondsF());
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.75)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeSuspendResume)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        anim->Suspend(TicksFromSecondsF(0.5));
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.0)).InSecondsF());
        anim->Resume(TicksFromSecondsF(1024));
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.0)).InSecondsF());
        EXPECT_EQ(1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeSuspendResumeReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_direction(Animation::DIRECTION_REVERSE);
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        anim->Suspend(TicksFromSecondsF(0.75));
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.0)).InSecondsF());
        anim->Resume(TicksFromSecondsF(1024));
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.0)).InSecondsF());
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1024.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeZeroDuration)
    {
        scoped_ptr<Animation> anim(CreateAnimation(0, 0));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeStarting)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 5.0));
        anim->SetRunState(Animation::STARTING, TicksFromSecondsF(0.0));
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        anim->set_time_offset(TimeDelta::FromMilliseconds(2000));
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        anim->set_start_time(TicksFromSecondsF(1.0));
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(3.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeNeedsSynchronizedStartTime)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 5.0));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        anim->set_needs_synchronized_start_time(true);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        anim->set_time_offset(TimeDelta::FromMilliseconds(2000));
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        anim->set_start_time(TicksFromSecondsF(1.0));
        anim->set_needs_synchronized_start_time(false);
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(3.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
    }

    TEST(AnimationTest, IsFinishedAtZeroIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(0));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(1.0)));
    }

    TEST(AnimationTest, IsFinishedAtOneIteration)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(-1.0)));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(1.0)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(2.0)));
    }

    TEST(AnimationTest, IsFinishedAtInfiniteIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(-1));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(0.5)));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(1.0)));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(1.5)));
    }

    TEST(AnimationTest, IsFinishedNegativeTimeOffset)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_time_offset(TimeDelta::FromMilliseconds(-500));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));

        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(-1.0)));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(0.5)));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(1.0)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(1.5)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(2.0)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(2.5)));
    }

    TEST(AnimationTest, IsFinishedPositiveTimeOffset)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_time_offset(TimeDelta::FromMilliseconds(500));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));

        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(-1.0)));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(0.5)));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(1.0)));
    }

    TEST(AnimationTest, IsFinishedAtNotRunning)
    {
        scoped_ptr<Animation> anim(CreateAnimation(0));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        anim->SetRunState(Animation::PAUSED, TicksFromSecondsF(0.0));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        anim->SetRunState(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            TicksFromSecondsF(0.0));
        EXPECT_FALSE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        anim->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
        anim->SetRunState(Animation::ABORTED, TicksFromSecondsF(0.0));
        EXPECT_TRUE(anim->IsFinishedAt(TicksFromSecondsF(0.0)));
    }

    TEST(AnimationTest, IsFinished)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_FALSE(anim->is_finished());
        anim->SetRunState(Animation::PAUSED, TicksFromSecondsF(0.0));
        EXPECT_FALSE(anim->is_finished());
        anim->SetRunState(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            TicksFromSecondsF(0.0));
        EXPECT_FALSE(anim->is_finished());
        anim->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));
        EXPECT_TRUE(anim->is_finished());
        anim->SetRunState(Animation::ABORTED, TicksFromSecondsF(0.0));
        EXPECT_TRUE(anim->is_finished());
    }

    TEST(AnimationTest, IsFinishedNeedsSynchronizedStartTime)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(2.0));
        EXPECT_FALSE(anim->is_finished());
        anim->SetRunState(Animation::PAUSED, TicksFromSecondsF(2.0));
        EXPECT_FALSE(anim->is_finished());
        anim->SetRunState(Animation::WAITING_FOR_TARGET_AVAILABILITY,
            TicksFromSecondsF(2.0));
        EXPECT_FALSE(anim->is_finished());
        anim->SetRunState(Animation::FINISHED, TicksFromSecondsF(0.0));
        EXPECT_TRUE(anim->is_finished());
        anim->SetRunState(Animation::ABORTED, TicksFromSecondsF(0.0));
        EXPECT_TRUE(anim->is_finished());
    }

    TEST(AnimationTest, RunStateChangesIgnoredWhileSuspended)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->Suspend(TicksFromSecondsF(0));
        EXPECT_EQ(Animation::PAUSED, anim->run_state());
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_EQ(Animation::PAUSED, anim->run_state());
        anim->Resume(TicksFromSecondsF(0));
        anim->SetRunState(Animation::RUNNING, TicksFromSecondsF(0.0));
        EXPECT_EQ(Animation::RUNNING, anim->run_state());
    }

    TEST(AnimationTest, TrimTimePlaybackNormal)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 1, 1));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePlaybackSlow)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 1, 0.5));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePlaybackFast)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 4, 2));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(
            2, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            3, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(
            4, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(
            4, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePlaybackNormalReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 2, -1));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            2, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePlaybackSlowReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 2, -0.5));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            2, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0)).InSecondsF());
        EXPECT_EQ(1.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(1.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(0.75, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.5)).InSecondsF());
        EXPECT_EQ(
            0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3)).InSecondsF());
        EXPECT_EQ(0.25, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.5)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePlaybackFastReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 2, -2));
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            2, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePlaybackFastInfiniteIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(-1, 4, 4));
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(
            2, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            2, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1000.0)).InSecondsF());
        EXPECT_EQ(2, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1000.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePlaybackNormalDoubleReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 1, -1));
        anim->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimePlaybackFastDoubleReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 4, -2));
        anim->set_direction(Animation::DIRECTION_REVERSE);
        EXPECT_EQ(0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(
            0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(
            1, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(
            2, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(
            3, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(
            4, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(
            4, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateTwoIterationsPlaybackFast)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2, 2, 2));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateTwoIterationsPlaybackFastReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2, 2, 2));
        anim->set_direction(Animation::DIRECTION_ALTERNATE_REVERSE);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.75)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.25)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeAlternateTwoIterationsPlaybackFastDoubleReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2, 2, -2));
        anim->set_direction(Animation::DIRECTION_ALTERNATE_REVERSE);
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.75)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.25)).InSecondsF());
    }

    TEST(AnimationTest,
        TrimTimeAlternateReverseThreeIterationsPlaybackFastAlternateReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(3, 2, -2));
        anim->set_direction(Animation::DIRECTION_ALTERNATE_REVERSE);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.75)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.75)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.25)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.5)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.75)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.0)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.25)).InSecondsF());
    }

    TEST(AnimationTest,
        TrimTimeAlternateReverseTwoIterationsPlaybackNormalAlternate)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2, 2, -1));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(2.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(1.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.5)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.5)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(4.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeIterationStart)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2, 1, 1));
        anim->set_iteration_start(0.5);
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.5)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeIterationStartAlternate)
    {
        scoped_ptr<Animation> anim(CreateAnimation(2, 1, 1));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        anim->set_iteration_start(0.3);
        EXPECT_EQ(0.3, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(0.3, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.8, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.7)).InSecondsF());
        EXPECT_EQ(0.7, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.2)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.7)).InSecondsF());
    }

    TEST(AnimationTest, TrimTimeIterationStartAlternateThreeIterations)
    {
        scoped_ptr<Animation> anim(CreateAnimation(3, 1, 1));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        anim->set_iteration_start(1);
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(-1.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.5)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.5)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(0.5, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.5)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.5)).InSecondsF());
    }

    TEST(AnimationTest,
        TrimTimeIterationStartAlternateThreeIterationsPlaybackReverse)
    {
        scoped_ptr<Animation> anim(CreateAnimation(3, 1, -1));
        anim->set_direction(Animation::DIRECTION_ALTERNATE);
        anim->set_iteration_start(1);
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(0.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(1.0)).InSecondsF());
        EXPECT_EQ(0.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(2.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.0)).InSecondsF());
        EXPECT_EQ(1.0, anim->TrimTimeToCurrentIteration(TicksFromSecondsF(3.5)).InSecondsF());
    }

    TEST(AnimationTest, InEffectFillMode)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1));
        anim->set_fill_mode(Animation::FILL_MODE_NONE);
        EXPECT_FALSE(anim->InEffect(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(1.0)));

        anim->set_fill_mode(Animation::FILL_MODE_FORWARDS);
        EXPECT_FALSE(anim->InEffect(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(1.0)));

        anim->set_fill_mode(Animation::FILL_MODE_BACKWARDS);
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(1.0)));

        anim->set_fill_mode(Animation::FILL_MODE_BOTH);
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(1.0)));
    }

    TEST(AnimationTest, InEffectFillModePlayback)
    {
        scoped_ptr<Animation> anim(CreateAnimation(1, 1, -1));
        anim->set_fill_mode(Animation::FILL_MODE_NONE);
        EXPECT_FALSE(anim->InEffect(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(1.0)));

        anim->set_fill_mode(Animation::FILL_MODE_FORWARDS);
        EXPECT_FALSE(anim->InEffect(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(1.0)));

        anim->set_fill_mode(Animation::FILL_MODE_BACKWARDS);
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(1.0)));

        anim->set_fill_mode(Animation::FILL_MODE_BOTH);
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(-1.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(0.0)));
        EXPECT_TRUE(anim->InEffect(TicksFromSecondsF(1.0)));
    }

} // namespace
} // namespace cc
