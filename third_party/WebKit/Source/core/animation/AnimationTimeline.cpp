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
#include "core/animation/AnimationTimeline.h"

#include "core/animation/AnimationClock.h"
#include "core/animation/ElementAnimations.h"
#include "core/dom/Document.h"
#include "core/frame/FrameView.h"
#include "core/loader/DocumentLoader.h"
#include "core/page/Page.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCompositorAnimationTimeline.h"
#include "public/platform/WebCompositorSupport.h"

namespace blink {

namespace {

bool compareAnimations(const RefPtrWillBeMember<Animation>& left, const RefPtrWillBeMember<Animation>& right)
{
    return Animation::hasLowerPriority(left.get(), right.get());
}

}

// This value represents 1 frame at 30Hz plus a little bit of wiggle room.
// TODO: Plumb a nominal framerate through and derive this value from that.
const double AnimationTimeline::s_minimumDelay = 0.04;


PassRefPtrWillBeRawPtr<AnimationTimeline> AnimationTimeline::create(Document* document, PassOwnPtrWillBeRawPtr<PlatformTiming> timing)
{
    return adoptRefWillBeNoop(new AnimationTimeline(document, timing));
}

AnimationTimeline::AnimationTimeline(Document* document, PassOwnPtrWillBeRawPtr<PlatformTiming> timing)
    : m_document(document)
    , m_zeroTime(0) // 0 is used by unit tests which cannot initialize from the loader
    , m_zeroTimeInitialized(false)
    , m_outdatedAnimationCount(0)
    , m_playbackRate(1)
    , m_lastCurrentTimeInternal(0)
{
    if (!timing)
        m_timing = adoptPtrWillBeNoop(new AnimationTimelineTiming(this));
    else
        m_timing = timing;

    if (RuntimeEnabledFeatures::compositorAnimationTimelinesEnabled() && Platform::current()->compositorSupport())
        m_compositorTimeline = adoptPtr(Platform::current()->compositorSupport()->createAnimationTimeline());

    ASSERT(document);
}

AnimationTimeline::~AnimationTimeline()
{
#if !ENABLE(OILPAN)
    for (const auto& animation : m_animations)
        animation->detachFromTimeline();
#endif
}

void AnimationTimeline::animationAttached(Animation& animation)
{
    ASSERT(animation.timeline() == this);
    ASSERT(!m_animations.contains(&animation));
    m_animations.add(&animation);
}

Animation* AnimationTimeline::play(AnimationEffect* child)
{
    if (!m_document)
        return nullptr;

    RefPtrWillBeRawPtr<Animation> animation = Animation::create(child, this);
    ASSERT(m_animations.contains(animation.get()));

    animation->play();
    ASSERT(m_animationsNeedingUpdate.contains(animation));

    return animation.get();
}

WillBeHeapVector<RefPtrWillBeMember<Animation>> AnimationTimeline::getAnimations()
{
    WillBeHeapVector<RefPtrWillBeMember<Animation>> animations;
    for (const auto& animation : m_animations) {
        if (animation->effect() && (animation->effect()->isCurrent() || animation->effect()->isInEffect()))
            animations.append(animation);
    }
    std::sort(animations.begin(), animations.end(), compareAnimations);
    return animations;
}

void AnimationTimeline::wake()
{
    m_timing->serviceOnNextFrame();
}

void AnimationTimeline::serviceAnimations(TimingUpdateReason reason)
{
    TRACE_EVENT0("blink", "AnimationTimeline::serviceAnimations");

    m_lastCurrentTimeInternal = currentTimeInternal();

    m_timing->cancelWake();

    WillBeHeapVector<RawPtrWillBeMember<Animation>> animations;
    animations.reserveInitialCapacity(m_animationsNeedingUpdate.size());
    for (RefPtrWillBeMember<Animation> animation : m_animationsNeedingUpdate)
        animations.append(animation.get());

    std::sort(animations.begin(), animations.end(), Animation::hasLowerPriority);

    for (Animation* animation : animations) {
        if (!animation->update(reason))
            m_animationsNeedingUpdate.remove(animation);
    }

    ASSERT(m_outdatedAnimationCount == 0);

#if ENABLE(ASSERT)
    for (const auto& animation : m_animationsNeedingUpdate)
        ASSERT(!animation->outdated());
#endif
}

void AnimationTimeline::scheduleNextService()
{
    ASSERT(m_outdatedAnimationCount == 0);

    double timeToNextEffect = std::numeric_limits<double>::infinity();
    for (const auto& animation : m_animationsNeedingUpdate) {
        timeToNextEffect = std::min(timeToNextEffect, animation->timeToEffectChange());
    }

    if (timeToNextEffect < s_minimumDelay) {
        m_timing->serviceOnNextFrame();
    } else if (timeToNextEffect != std::numeric_limits<double>::infinity()) {
        m_timing->wakeAfter(timeToNextEffect - s_minimumDelay);
    }
}

void AnimationTimeline::AnimationTimelineTiming::wakeAfter(double duration)
{
    m_timer.startOneShot(duration, FROM_HERE);
}

void AnimationTimeline::AnimationTimelineTiming::cancelWake()
{
    m_timer.stop();
}

void AnimationTimeline::AnimationTimelineTiming::serviceOnNextFrame()
{
    if (m_timeline->m_document && m_timeline->m_document->view())
        m_timeline->m_document->view()->scheduleAnimation();
}

DEFINE_TRACE(AnimationTimeline::AnimationTimelineTiming)
{
    visitor->trace(m_timeline);
    AnimationTimeline::PlatformTiming::trace(visitor);
}

double AnimationTimeline::zeroTime()
{
    if (!m_zeroTimeInitialized && m_document && m_document->loader()) {
        m_zeroTime = m_document->loader()->timing().referenceMonotonicTime();
        m_zeroTimeInitialized = true;
    }
    return m_zeroTime;
}

double AnimationTimeline::currentTime(bool& isNull)
{
    return currentTimeInternal(isNull) * 1000;
}

double AnimationTimeline::currentTimeInternal(bool& isNull)
{
    if (!m_document) {
        isNull = true;
        return std::numeric_limits<double>::quiet_NaN();
    }
    double result = m_playbackRate == 0
        ? zeroTime()
        : (document()->animationClock().currentTime() - zeroTime()) * m_playbackRate;
    isNull = std::isnan(result);
    return result;
}

double AnimationTimeline::currentTime()
{
    return currentTimeInternal() * 1000;
}

double AnimationTimeline::currentTimeInternal()
{
    bool isNull;
    return currentTimeInternal(isNull);
}

void AnimationTimeline::setCurrentTime(double currentTime)
{
    setCurrentTimeInternal(currentTime / 1000);
}

void AnimationTimeline::setCurrentTimeInternal(double currentTime)
{
    if (!document())
        return;
    m_zeroTime = m_playbackRate == 0
        ? currentTime
        : document()->animationClock().currentTime() - currentTime / m_playbackRate;
    m_zeroTimeInitialized = true;

    for (const auto& animation : m_animations) {
        // The Player needs a timing update to pick up a new time.
        animation->setOutdated();
        // Any corresponding compositor animation will need to be restarted. Marking the
        // effect changed forces this.
        animation->setCompositorPending(true);
    }
}

double AnimationTimeline::effectiveTime()
{
    double time = currentTimeInternal();
    return std::isnan(time) ? 0 : time;
}

void AnimationTimeline::pauseAnimationsForTesting(double pauseTime)
{
    for (const auto& animation : m_animationsNeedingUpdate)
        animation->pauseForTesting(pauseTime);
    serviceAnimations(TimingUpdateOnDemand);
}

bool AnimationTimeline::needsAnimationTimingUpdate()
{
    return m_animationsNeedingUpdate.size() && currentTimeInternal() != m_lastCurrentTimeInternal;
}

void AnimationTimeline::clearOutdatedAnimation(Animation* animation)
{
    ASSERT(!animation->outdated());
    m_outdatedAnimationCount--;
}

void AnimationTimeline::setOutdatedAnimation(Animation* animation)
{
    ASSERT(animation->outdated());
    m_outdatedAnimationCount++;
    m_animationsNeedingUpdate.add(animation);
    if (m_document && m_document->page() && !m_document->page()->animator().isServicingAnimations())
        m_timing->serviceOnNextFrame();
}

void AnimationTimeline::setPlaybackRate(double playbackRate)
{
    if (!document())
        return;
    double currentTime = currentTimeInternal();
    m_playbackRate = playbackRate;
    m_zeroTime = playbackRate == 0
        ? currentTime
        : document()->animationClock().currentTime() - currentTime / playbackRate;
    m_zeroTimeInitialized = true;

    for (const auto& animation : m_animations) {
        // Corresponding compositor animation may need to be restarted to pick up
        // the new playback rate. Marking the effect changed forces this.
        animation->setCompositorPending(true);
    }
}

double AnimationTimeline::playbackRate() const
{
    return m_playbackRate;
}

#if !ENABLE(OILPAN)
void AnimationTimeline::detachFromDocument()
{
    // FIXME: AnimationTimeline should keep Document alive.
    m_document = nullptr;
}
#endif

DEFINE_TRACE(AnimationTimeline)
{
#if ENABLE(OILPAN)
    visitor->trace(m_document);
    visitor->trace(m_timing);
    visitor->trace(m_animationsNeedingUpdate);
    visitor->trace(m_animations);
#endif
}

} // namespace
