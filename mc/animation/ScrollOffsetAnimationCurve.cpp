// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/ScrollOffsetAnimationCurve.h"

#include <algorithm>
#include <cmath>

#include "mc/animation/TimingFunction.h"
#include "mc/base/TimeUtil.h"
#include "mc/base/Tween.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"

const double kDurationDivisor = 60.0;

namespace mc {

namespace {

static float maximumDimension(const blink::FloatPoint& delta)
{
    return std::max(std::abs(delta.x()), std::abs(delta.y()));
}

static base::TimeDelta durationFromDelta(const blink::FloatPoint& delta)
{
    // The duration of a scroll animation depends on the size of the scroll.
    // The exact relationship between the size and the duration isn't specified
    // by the CSSOM View smooth scroll spec and is instead left up to user agents
    // to decide. The calculation performed here will very likely be further
    // tweaked before the smooth scroll API ships.
    return base::TimeDelta::FromMicroseconds((std::sqrt(maximumDimension(delta)) / kDurationDivisor) * base::Time::kMicrosecondsPerSecond);
}

static WTF::PassOwnPtr<TimingFunction> easeOutWithInitialVelocity(double velocity)
{
    // Based on EaseInOutTimingFunction::Create with first control point rotated.
    if (std::abs(velocity) < 1000.0) {
        const double r2 = 0.42 * 0.42;
        const double v2 = velocity * velocity;
        const double x1 = std::sqrt(r2 / (v2 + 1));
        const double y1 = std::sqrt(r2 * v2 / (v2 + 1));
        return CubicBezierTimingFunction::create(x1, y1, 0.58, 1);
    }

    // For large |velocity|, x1 approaches 0 and y1 approaches 0.42. To avoid the
    // risk of floating point arithmetic involving infinity and NaN, use those
    // values directly rather than computing them above.
    return CubicBezierTimingFunction::create(0, 0.42, 0.58, 1);
}

static blink::FloatPoint deltaFrom(const blink::FloatPoint& a, const blink::FloatPoint& b)
{
    return blink::FloatPoint((float)(a.x() - b.x()), (float)(a.y() - b.y()));
}

}  // namespace

WTF::PassOwnPtr<ScrollOffsetAnimationCurve> ScrollOffsetAnimationCurve::create(const blink::FloatPoint& targetValue, WTF::PassOwnPtr<TimingFunction> timingFunction)
{
    return adoptPtr(new ScrollOffsetAnimationCurve(targetValue, timingFunction));
}

ScrollOffsetAnimationCurve::ScrollOffsetAnimationCurve(const blink::FloatPoint& targetValue, WTF::PassOwnPtr<TimingFunction> timingFunction)
    : m_targetValue(targetValue), m_timingFunction(timingFunction)
{
}

ScrollOffsetAnimationCurve::~ScrollOffsetAnimationCurve() {}

void ScrollOffsetAnimationCurve::setInitialValue(const blink::FloatPoint& initialValue)
{
    m_initialValue = initialValue;
    m_totalAnimationDuration = durationFromDelta(deltaFrom(m_targetValue, m_initialValue));
}

blink::FloatPoint ScrollOffsetAnimationCurve::getValue(base::TimeDelta t) const
{
    base::TimeDelta duration = m_totalAnimationDuration - m_lastRetarget;
    t -= m_lastRetarget;

    if (t <= base::TimeDelta())
        return m_initialValue;

    if (t >= duration)
        return m_targetValue;

    double progress = m_timingFunction->getValue(TimeUtil::divide(t, duration));
    return blink::FloatPoint(
        Tween::floatValueBetween(progress, m_initialValue.x(), m_targetValue.x()),
        Tween::floatValueBetween(progress, m_initialValue.y(), m_targetValue.y()));
}

base::TimeDelta ScrollOffsetAnimationCurve::duration() const
{
    return m_totalAnimationDuration;
}

AnimationCurve::CurveType ScrollOffsetAnimationCurve::type() const
{
    return SCROLL_OFFSET;
}

WTF::PassOwnPtr<AnimationCurve> ScrollOffsetAnimationCurve::clone() const
{
    WTF::PassOwnPtr<TimingFunction> timingFunction((m_timingFunction->clone()));
    WTF::PassOwnPtr<ScrollOffsetAnimationCurve> curveClone = create(m_targetValue, timingFunction);

    curveClone->m_initialValue = m_initialValue;
    curveClone->m_totalAnimationDuration = m_totalAnimationDuration;
    curveClone->m_lastRetarget = m_lastRetarget;
    return curveClone;
}

void ScrollOffsetAnimationCurve::updateTarget(double t, const blink::FloatPoint& newTarget)
{
    blink::FloatPoint currentPosition = getValue(base::TimeDelta::FromSecondsD(t));
    blink::FloatPoint oldDelta = deltaFrom(m_targetValue, m_initialValue);
    blink::FloatPoint newDelta = deltaFrom(newTarget, currentPosition);

    double oldDuration = (m_totalAnimationDuration - m_lastRetarget).InSecondsF();
    double newDuration = durationFromDelta(newDelta).InSecondsF();

    double oldVelocity = m_timingFunction->velocity((t - m_lastRetarget.InSecondsF()) / oldDuration);

    // TimingFunction::Velocity gives the slope of the curve from 0 to 1.
    // To match the "true" velocity in px/sec we must adjust this slope for
    // differences in duration and scroll delta between old and new curves.
    double new_velocity = oldVelocity * (newDuration / oldDuration) * (maximumDimension(oldDelta) / maximumDimension(newDelta));

    m_initialValue = currentPosition;
    m_targetValue = newTarget;
    m_totalAnimationDuration = base::TimeDelta::FromSecondsD(t + newDuration);
    m_lastRetarget = base::TimeDelta::FromSecondsD(t);
    m_timingFunction = easeOutWithInitialVelocity(new_velocity);
}

}  // namespace mc
