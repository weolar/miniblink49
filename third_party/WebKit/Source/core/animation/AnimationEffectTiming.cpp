// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/AnimationEffectTiming.h"

#include "bindings/core/v8/UnionTypesCore.h"
#include "core/animation/AnimationEffect.h"
#include "core/animation/KeyframeEffect.h"
#include "platform/animation/TimingFunction.h"

namespace blink {

PassRefPtrWillBeRawPtr<AnimationEffectTiming> AnimationEffectTiming::create(AnimationEffect* parent)
{
    return adoptRefWillBeNoop(new AnimationEffectTiming(parent));
}

AnimationEffectTiming::AnimationEffectTiming(AnimationEffect* parent)
    : m_parent(parent)
{
}

double AnimationEffectTiming::delay()
{
    return m_parent->specifiedTiming().startDelay * 1000;
}

double AnimationEffectTiming::endDelay()
{
    return m_parent->specifiedTiming().endDelay * 1000;
}

String AnimationEffectTiming::fill()
{
    return Timing::fillModeString(m_parent->specifiedTiming().fillMode);
}

double AnimationEffectTiming::iterationStart()
{
    return m_parent->specifiedTiming().iterationStart;
}

double AnimationEffectTiming::iterations()
{
    return m_parent->specifiedTiming().iterationCount;
}

void AnimationEffectTiming::duration(UnrestrictedDoubleOrString& returnValue)
{
    if (std::isnan(m_parent->specifiedTiming().iterationDuration))
        returnValue.setString("auto");
    else
        returnValue.setUnrestrictedDouble(m_parent->specifiedTiming().iterationDuration * 1000);
}

double AnimationEffectTiming::playbackRate()
{
    return m_parent->specifiedTiming().playbackRate;
}

String AnimationEffectTiming::direction()
{
    return Timing::playbackDirectionString(m_parent->specifiedTiming().direction);
}

String AnimationEffectTiming::easing()
{
    return m_parent->specifiedTiming().timingFunction->toString();
}

void AnimationEffectTiming::setDelay(double delay)
{
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setStartDelay(timing, delay);
    m_parent->updateSpecifiedTiming(timing);
}

void AnimationEffectTiming::setEndDelay(double endDelay)
{
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setEndDelay(timing, endDelay);
    m_parent->updateSpecifiedTiming(timing);
}

void AnimationEffectTiming::setFill(String fill)
{
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setFillMode(timing, fill);
    m_parent->updateSpecifiedTiming(timing);
}

void AnimationEffectTiming::setIterationStart(double iterationStart)
{
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setIterationStart(timing, iterationStart);
    m_parent->updateSpecifiedTiming(timing);
}

void AnimationEffectTiming::setIterations(double iterations)
{
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setIterationCount(timing, iterations);
    m_parent->updateSpecifiedTiming(timing);
}

void AnimationEffectTiming::setDuration(const UnrestrictedDoubleOrString& durationOrAuto)
{
    // Any strings other than "auto" are coerced to "auto".
    double duration = durationOrAuto.isString() ? std::numeric_limits<double>::quiet_NaN() : durationOrAuto.getAsUnrestrictedDouble();
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setIterationDuration(timing, duration);
    m_parent->updateSpecifiedTiming(timing);
}

void AnimationEffectTiming::setPlaybackRate(double playbackRate)
{
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setPlaybackRate(timing, playbackRate);
    m_parent->updateSpecifiedTiming(timing);
}

void AnimationEffectTiming::setDirection(String direction)
{
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setPlaybackDirection(timing, direction);
    m_parent->updateSpecifiedTiming(timing);
}

void AnimationEffectTiming::setEasing(String easing)
{
    Timing timing = m_parent->specifiedTiming();
    TimingInput::setTimingFunction(timing, easing);
    m_parent->updateSpecifiedTiming(timing);
}

DEFINE_TRACE(AnimationEffectTiming)
{
    visitor->trace(m_parent);
}

} // namespace blink
