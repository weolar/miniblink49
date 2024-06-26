// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/scroll_offset_animation_curve.h"

#include <algorithm>
#include <cmath>

#include "base/logging.h"
#include "cc/animation/timing_function.h"
#include "cc/base/time_util.h"
#include "ui/gfx/animation/tween.h"

const double kDurationDivisor = 60.0;

namespace cc {

namespace {

    static float MaximumDimension(const gfx::Vector2dF& delta)
    {
        return std::max(std::abs(delta.x()), std::abs(delta.y()));
    }

    static base::TimeDelta DurationFromDelta(const gfx::Vector2dF& delta)
    {
        // The duration of a scroll animation depends on the size of the scroll.
        // The exact relationship between the size and the duration isn't specified
        // by the CSSOM View smooth scroll spec and is instead left up to user agents
        // to decide. The calculation performed here will very likely be further
        // tweaked before the smooth scroll API ships.
        return base::TimeDelta::FromMicroseconds(
            (std::sqrt(MaximumDimension(delta)) / kDurationDivisor) * base::Time::kMicrosecondsPerSecond);
    }

    static scoped_ptr<TimingFunction> EaseOutWithInitialVelocity(double velocity)
    {
        // Based on EaseInOutTimingFunction::Create with first control point rotated.
        if (std::abs(velocity) < 1000.0) {
            const double r2 = 0.42 * 0.42;
            const double v2 = velocity * velocity;
            const double x1 = std::sqrt(r2 / (v2 + 1));
            const double y1 = std::sqrt(r2 * v2 / (v2 + 1));
            return CubicBezierTimingFunction::Create(x1, y1, 0.58, 1);
        }

        // For large |velocity|, x1 approaches 0 and y1 approaches 0.42. To avoid the
        // risk of floating point arithmetic involving infinity and NaN, use those
        // values directly rather than computing them above.
        return CubicBezierTimingFunction::Create(0, 0.42, 0.58, 1);
    }

} // namespace

scoped_ptr<ScrollOffsetAnimationCurve> ScrollOffsetAnimationCurve::Create(
    const gfx::ScrollOffset& target_value,
    scoped_ptr<TimingFunction> timing_function)
{
    return make_scoped_ptr(
        new ScrollOffsetAnimationCurve(target_value, timing_function.Pass()));
}

ScrollOffsetAnimationCurve::ScrollOffsetAnimationCurve(
    const gfx::ScrollOffset& target_value,
    scoped_ptr<TimingFunction> timing_function)
    : target_value_(target_value)
    , timing_function_(timing_function.Pass())
{
}

ScrollOffsetAnimationCurve::~ScrollOffsetAnimationCurve() { }

void ScrollOffsetAnimationCurve::SetInitialValue(
    const gfx::ScrollOffset& initial_value)
{
    initial_value_ = initial_value;
    total_animation_duration_ = DurationFromDelta(
        target_value_.DeltaFrom(initial_value_));
}

gfx::ScrollOffset ScrollOffsetAnimationCurve::GetValue(
    base::TimeDelta t) const
{
    base::TimeDelta duration = total_animation_duration_ - last_retarget_;
    t -= last_retarget_;

    if (t <= base::TimeDelta())
        return initial_value_;

    if (t >= duration)
        return target_value_;

    double progress = timing_function_->GetValue(TimeUtil::Divide(t, duration));
    return gfx::ScrollOffset(
        gfx::Tween::FloatValueBetween(
            progress, initial_value_.x(), target_value_.x()),
        gfx::Tween::FloatValueBetween(
            progress, initial_value_.y(), target_value_.y()));
}

base::TimeDelta ScrollOffsetAnimationCurve::Duration() const
{
    return total_animation_duration_;
}

AnimationCurve::CurveType ScrollOffsetAnimationCurve::Type() const
{
    return SCROLL_OFFSET;
}

scoped_ptr<AnimationCurve> ScrollOffsetAnimationCurve::Clone() const
{
    scoped_ptr<TimingFunction> timing_function(
        static_cast<TimingFunction*>(timing_function_->Clone().release()));
    scoped_ptr<ScrollOffsetAnimationCurve> curve_clone = Create(target_value_, timing_function.Pass());
    curve_clone->initial_value_ = initial_value_;
    curve_clone->total_animation_duration_ = total_animation_duration_;
    curve_clone->last_retarget_ = last_retarget_;
    return curve_clone.Pass();
}

void ScrollOffsetAnimationCurve::UpdateTarget(
    double t,
    const gfx::ScrollOffset& new_target)
{
    gfx::ScrollOffset current_position = GetValue(base::TimeDelta::FromSecondsD(t));
    gfx::Vector2dF old_delta = target_value_.DeltaFrom(initial_value_);
    gfx::Vector2dF new_delta = new_target.DeltaFrom(current_position);

    double old_duration = (total_animation_duration_ - last_retarget_).InSecondsF();
    double new_duration = DurationFromDelta(new_delta).InSecondsF();

    double old_velocity = timing_function_->Velocity(
        (t - last_retarget_.InSecondsF()) / old_duration);

    // TimingFunction::Velocity gives the slope of the curve from 0 to 1.
    // To match the "true" velocity in px/sec we must adjust this slope for
    // differences in duration and scroll delta between old and new curves.
    double new_velocity = old_velocity * (new_duration / old_duration) * (MaximumDimension(old_delta) / MaximumDimension(new_delta));

    initial_value_ = current_position;
    target_value_ = new_target;
    total_animation_duration_ = base::TimeDelta::FromSecondsD(t + new_duration);
    last_retarget_ = base::TimeDelta::FromSecondsD(t);
    timing_function_ = EaseOutWithInitialVelocity(new_velocity);
}

} // namespace cc
