/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/animation/AnimationEffect.h"

#include "core/animation/Animation.h"
#include "core/animation/AnimationEffectTiming.h"
#include "core/animation/ComputedTimingProperties.h"
#include "core/animation/TimingCalculations.h"

namespace blink {

namespace {

Timing::FillMode resolvedFillMode(Timing::FillMode fillMode, bool isAnimation)
{
    if (fillMode != Timing::FillModeAuto)
        return fillMode;
    if (isAnimation)
        return Timing::FillModeNone;
    return Timing::FillModeBoth;
}

} // namespace

AnimationEffect::AnimationEffect(const Timing& timing, PassOwnPtrWillBeRawPtr<EventDelegate> eventDelegate)
    : m_parent(nullptr)
    , m_startTime(0)
    , m_animation(nullptr)
    , m_timing(timing)
    , m_eventDelegate(eventDelegate)
    , m_calculated()
    , m_needsUpdate(true)
    , m_lastUpdateTime(nullValue())
{
    m_timing.assertValid();
}

double AnimationEffect::iterationDuration() const
{
    double result = std::isnan(m_timing.iterationDuration) ? intrinsicIterationDuration() : m_timing.iterationDuration;
    ASSERT(result >= 0);
    return result;
}

double AnimationEffect::repeatedDuration() const
{
    const double result = multiplyZeroAlwaysGivesZero(iterationDuration(), m_timing.iterationCount);
    ASSERT(result >= 0);
    return result;
}

double AnimationEffect::activeDurationInternal() const
{
    const double result = m_timing.playbackRate
        ? repeatedDuration() / std::abs(m_timing.playbackRate)
        : std::numeric_limits<double>::infinity();
    ASSERT(result >= 0);
    return result;
}

void AnimationEffect::updateSpecifiedTiming(const Timing& timing)
{
    // FIXME: Test whether the timing is actually different?
    m_timing = timing;
    invalidate();
    if (m_animation)
        m_animation->setOutdated();
    specifiedTimingChanged();
}

void AnimationEffect::computedTiming(ComputedTimingProperties& computedTiming)
{
    // ComputedTimingProperties members.
    computedTiming.setStartTime(startTimeInternal() * 1000);
    computedTiming.setEndTime(endTimeInternal() * 1000);
    computedTiming.setActiveDuration(activeDurationInternal() * 1000);

    // FIXME: These should be null if not in effect, but current dictionary API
    // will treat these as undefined.
    if (ensureCalculated().isInEffect) {
        computedTiming.setLocalTime(ensureCalculated().localTime * 1000);
        computedTiming.setTimeFraction(ensureCalculated().timeFraction);
        computedTiming.setCurrentIteration(ensureCalculated().currentIteration);
    }

    // KeyframeEffectOptions members.
    computedTiming.setDelay(specifiedTiming().startDelay * 1000);
    computedTiming.setEndDelay(specifiedTiming().endDelay * 1000);
    computedTiming.setFill(Timing::fillModeString(resolvedFillMode(specifiedTiming().fillMode, isAnimation())));
    computedTiming.setIterationStart(specifiedTiming().iterationStart);
    computedTiming.setIterations(specifiedTiming().iterationCount);

    UnrestrictedDoubleOrString duration;
    duration.setUnrestrictedDouble(iterationDuration() * 1000);
    computedTiming.setDuration(duration);

    computedTiming.setPlaybackRate(specifiedTiming().playbackRate);
    computedTiming.setDirection(Timing::playbackDirectionString(specifiedTiming().direction));
    computedTiming.setEasing(specifiedTiming().timingFunction->toString());
}

ComputedTimingProperties AnimationEffect::computedTiming()
{
    ComputedTimingProperties result;
    computedTiming(result);
    return result;
}


void AnimationEffect::updateInheritedTime(double inheritedTime, TimingUpdateReason reason) const
{
    bool needsUpdate = m_needsUpdate || (m_lastUpdateTime != inheritedTime && !(isNull(m_lastUpdateTime) && isNull(inheritedTime)));
    m_needsUpdate = false;
    m_lastUpdateTime = inheritedTime;

    const double localTime = inheritedTime - m_startTime;
    double timeToNextIteration = std::numeric_limits<double>::infinity();
    if (needsUpdate) {
        const double activeDuration = this->activeDurationInternal();

        const Phase currentPhase = calculatePhase(activeDuration, localTime, m_timing);
        // FIXME: parentPhase depends on groups being implemented.
        const AnimationEffect::Phase parentPhase = AnimationEffect::PhaseActive;
        const double activeTime = calculateActiveTime(activeDuration, resolvedFillMode(m_timing.fillMode, isAnimation()), localTime, parentPhase, currentPhase, m_timing);

        double currentIteration;
        double timeFraction;
        if (const double iterationDuration = this->iterationDuration()) {
            const double startOffset = multiplyZeroAlwaysGivesZero(m_timing.iterationStart, iterationDuration);
            ASSERT(startOffset >= 0);
            const double scaledActiveTime = calculateScaledActiveTime(activeDuration, activeTime, startOffset, m_timing);
            const double iterationTime = calculateIterationTime(iterationDuration, repeatedDuration(), scaledActiveTime, startOffset, m_timing);

            currentIteration = calculateCurrentIteration(iterationDuration, iterationTime, scaledActiveTime, m_timing);
            timeFraction = calculateTransformedTime(currentIteration, iterationDuration, iterationTime, m_timing) / iterationDuration;

            if (!isNull(iterationTime)) {
                timeToNextIteration = (iterationDuration - iterationTime) / std::abs(m_timing.playbackRate);
                if (activeDuration - activeTime < timeToNextIteration)
                    timeToNextIteration = std::numeric_limits<double>::infinity();
            }
        } else {
            const double localIterationDuration = 1;
            const double localRepeatedDuration = localIterationDuration * m_timing.iterationCount;
            ASSERT(localRepeatedDuration >= 0);
            const double localActiveDuration = m_timing.playbackRate ? localRepeatedDuration / std::abs(m_timing.playbackRate) : std::numeric_limits<double>::infinity();
            ASSERT(localActiveDuration >= 0);
            const double localLocalTime = localTime < m_timing.startDelay ? localTime : localActiveDuration + m_timing.startDelay;
            const AnimationEffect::Phase localCurrentPhase = calculatePhase(localActiveDuration, localLocalTime, m_timing);
            const double localActiveTime = calculateActiveTime(localActiveDuration, resolvedFillMode(m_timing.fillMode, isAnimation()), localLocalTime, parentPhase, localCurrentPhase, m_timing);
            const double startOffset = m_timing.iterationStart * localIterationDuration;
            ASSERT(startOffset >= 0);
            const double scaledActiveTime = calculateScaledActiveTime(localActiveDuration, localActiveTime, startOffset, m_timing);
            const double iterationTime = calculateIterationTime(localIterationDuration, localRepeatedDuration, scaledActiveTime, startOffset, m_timing);

            currentIteration = calculateCurrentIteration(localIterationDuration, iterationTime, scaledActiveTime, m_timing);
            timeFraction = calculateTransformedTime(currentIteration, localIterationDuration, iterationTime, m_timing);
        }

        m_calculated.currentIteration = currentIteration;
        m_calculated.timeFraction = timeFraction;

        m_calculated.phase = currentPhase;
        m_calculated.isInEffect = !isNull(activeTime);
        m_calculated.isInPlay = phase() == PhaseActive && (!m_parent || m_parent->isInPlay());
        m_calculated.isCurrent = phase() == PhaseBefore || isInPlay() || (m_parent && m_parent->isCurrent());
        m_calculated.localTime = m_lastUpdateTime - m_startTime;
    }

    // Test for events even if timing didn't need an update as the animation may have gained a start time.
    // FIXME: Refactor so that we can ASSERT(m_animation) here, this is currently required to be nullable for testing.
    if (reason == TimingUpdateForAnimationFrame && (!m_animation || m_animation->hasStartTime() || m_animation->paused())) {
        if (m_eventDelegate)
            m_eventDelegate->onEventCondition(*this);
    }

    if (needsUpdate)  {
        // FIXME: This probably shouldn't be recursive.
        updateChildrenAndEffects();
        m_calculated.timeToForwardsEffectChange = calculateTimeToEffectChange(true, localTime, timeToNextIteration);
        m_calculated.timeToReverseEffectChange = calculateTimeToEffectChange(false, localTime, timeToNextIteration);
    }
}

const AnimationEffect::CalculatedTiming& AnimationEffect::ensureCalculated() const
{
    if (!m_animation)
        return m_calculated;
    if (m_animation->outdated())
        m_animation->update(TimingUpdateOnDemand);
    ASSERT(!m_animation->outdated());
    return m_calculated;
}

PassRefPtrWillBeRawPtr<AnimationEffectTiming> AnimationEffect::timing()
{
    return AnimationEffectTiming::create(this);
}

DEFINE_TRACE(AnimationEffect)
{
    visitor->trace(m_parent);
    visitor->trace(m_animation);
    visitor->trace(m_eventDelegate);
}

} // namespace blink
