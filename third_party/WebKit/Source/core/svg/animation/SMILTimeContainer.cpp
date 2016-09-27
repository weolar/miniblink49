/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/svg/animation/SMILTimeContainer.h"

#include "core/animation/AnimationClock.h"
#include "core/animation/AnimationTimeline.h"
#include "core/dom/ElementTraversal.h"
#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/animation/SVGSMILElement.h"

namespace blink {

static const double initialFrameDelay = 0.025;
static const double animationPolicyOnceDuration = 3.000;

#if !ENABLE(OILPAN)
// Every entry-point that calls updateAnimations() should instantiate a
// DiscardScope to prevent deletion of the ownerElement (and hence itself.)
class DiscardScope {
public:
    explicit DiscardScope(SVGSVGElement& timeContainerOwner) : m_discardScopeElement(&timeContainerOwner) { }

private:
    RefPtr<SVGSVGElement> m_discardScopeElement;
};
#endif

SMILTimeContainer::SMILTimeContainer(SVGSVGElement& owner)
    : m_beginTime(0)
    , m_pauseTime(0)
    , m_resumeTime(0)
    , m_accumulatedActiveTime(0)
    , m_presetStartTime(0)
    , m_frameSchedulingState(Idle)
    , m_documentOrderIndexesDirty(false)
    , m_wakeupTimer(this, &SMILTimeContainer::wakeupTimerFired)
    , m_animationPolicyOnceTimer(this, &SMILTimeContainer::animationPolicyTimerFired)
    , m_ownerSVGElement(owner)
#if ENABLE(ASSERT)
    , m_preventScheduledAnimationsChanges(false)
#endif
{
}

SMILTimeContainer::~SMILTimeContainer()
{
    cancelAnimationFrame();
    cancelAnimationPolicyTimer();
    ASSERT(!m_wakeupTimer.isActive());
#if ENABLE(ASSERT)
    ASSERT(!m_preventScheduledAnimationsChanges);
#endif
}

void SMILTimeContainer::schedule(SVGSMILElement* animation, SVGElement* target, const QualifiedName& attributeName)
{
    ASSERT(animation->timeContainer() == this);
    ASSERT(target);
    ASSERT(animation->hasValidAttributeName());
    ASSERT(animation->hasValidAttributeType());
    ASSERT(animation->inActiveDocument());

#if ENABLE(ASSERT)
    ASSERT(!m_preventScheduledAnimationsChanges);
#endif

    ElementAttributePair key(target, attributeName);
    OwnPtrWillBeMember<AnimationsLinkedHashSet>& scheduled = m_scheduledAnimations.add(key, nullptr).storedValue->value;
    if (!scheduled)
        scheduled = adoptPtrWillBeNoop(new AnimationsLinkedHashSet);
    ASSERT(!scheduled->contains(animation));
    scheduled->add(animation);

    SMILTime nextFireTime = animation->nextProgressTime();
    if (nextFireTime.isFinite())
        notifyIntervalsChanged();
}

void SMILTimeContainer::unschedule(SVGSMILElement* animation, SVGElement* target, const QualifiedName& attributeName)
{
    ASSERT(animation->timeContainer() == this);

#if ENABLE(ASSERT)
    ASSERT(!m_preventScheduledAnimationsChanges);
#endif

    ElementAttributePair key(target, attributeName);
    GroupedAnimationsMap::iterator it = m_scheduledAnimations.find(key);
    ASSERT(it != m_scheduledAnimations.end());
    AnimationsLinkedHashSet* scheduled = it->value.get();
    ASSERT(scheduled);
    AnimationsLinkedHashSet::iterator itAnimation = scheduled->find(animation);
    ASSERT(itAnimation != scheduled->end());
    scheduled->remove(itAnimation);

    if (scheduled->isEmpty())
        m_scheduledAnimations.remove(it);
}

bool SMILTimeContainer::hasAnimations() const
{
    return !m_scheduledAnimations.isEmpty();
}

bool SMILTimeContainer::hasPendingSynchronization() const
{
    return m_frameSchedulingState == SynchronizeAnimations && m_wakeupTimer.isActive() && !m_wakeupTimer.nextFireInterval();
}

void SMILTimeContainer::notifyIntervalsChanged()
{
    if (!isStarted())
        return;
    // Schedule updateAnimations() to be called asynchronously so multiple intervals
    // can change with updateAnimations() only called once at the end.
    if (hasPendingSynchronization())
        return;
    cancelAnimationFrame();
    scheduleWakeUp(0, SynchronizeAnimations);
}

SMILTime SMILTimeContainer::elapsed() const
{
    if (!m_beginTime)
        return 0;

    if (isPaused())
        return m_accumulatedActiveTime;

    return currentTime() + m_accumulatedActiveTime - lastResumeTime();
}

bool SMILTimeContainer::isPaused() const
{
    // If animation policy is "none", it is always paused.
    return m_pauseTime || animationPolicy() == ImageAnimationPolicyNoAnimation;
}

bool SMILTimeContainer::isStarted() const
{
    return m_beginTime;
}

void SMILTimeContainer::begin()
{
    RELEASE_ASSERT(!m_beginTime);

    if (!document().isActive())
        return;

    if (!handleAnimationPolicy(RestartOnceTimerIfNotPaused))
        return;

    double now = currentTime();

    // If 'm_presetStartTime' is set, the timeline was modified via setElapsed() before the document began.
    // In this case pass on 'seekToTime=true' to updateAnimations().
    m_beginTime = now - m_presetStartTime;
#if !ENABLE(OILPAN)
    DiscardScope discardScope(m_ownerSVGElement);
#endif
    SMILTime earliestFireTime = updateAnimations(SMILTime(m_presetStartTime), m_presetStartTime ? true : false);
    m_presetStartTime = 0;

    if (m_pauseTime) {
        m_pauseTime = now;
        // If updateAnimations() caused new syncbase instance to be generated,
        // we don't want to cancel those. Excepting that, no frame should've
        // been scheduled at this point.
        ASSERT(m_frameSchedulingState == Idle || m_frameSchedulingState == SynchronizeAnimations);
    } else if (!hasPendingSynchronization()) {
        ASSERT(isTimelineRunning());
        // If the timeline is running, and there's pending animation updates,
        // always perform the first update after the timeline was started using
        // the wake-up mechanism.
        if (earliestFireTime.isFinite()) {
            SMILTime delay = earliestFireTime - elapsed();
            scheduleWakeUp(std::max(initialFrameDelay, delay.value()), SynchronizeAnimations);
        }
    }
}

void SMILTimeContainer::pause()
{
    if (!handleAnimationPolicy(CancelOnceTimer))
        return;

    ASSERT(!isPaused());
    m_pauseTime = currentTime();

    if (m_beginTime) {
        m_accumulatedActiveTime += m_pauseTime - lastResumeTime();
        cancelAnimationFrame();
    }
    m_resumeTime = 0;
}

void SMILTimeContainer::resume()
{
    if (!handleAnimationPolicy(RestartOnceTimer))
        return;

    ASSERT(isPaused());
    m_resumeTime = currentTime();

    m_pauseTime = 0;
    scheduleWakeUp(0, SynchronizeAnimations);
}

void SMILTimeContainer::setElapsed(SMILTime time)
{
    // If the documment didn't begin yet, record a new start time, we'll seek to once its possible.
    if (!m_beginTime) {
        m_presetStartTime = time.value();
        return;
    }

    if (!handleAnimationPolicy(RestartOnceTimerIfNotPaused))
        return;

    cancelAnimationFrame();

    double now = currentTime();
    m_beginTime = now - time.value();
    m_resumeTime = 0;
    if (m_pauseTime) {
        m_pauseTime = now;
        m_accumulatedActiveTime = time.value();
    } else {
        m_accumulatedActiveTime = 0;
    }

#if ENABLE(ASSERT)
    m_preventScheduledAnimationsChanges = true;
#endif
    for (const auto& entry : m_scheduledAnimations) {
        if (!entry.key.first)
            continue;

        AnimationsLinkedHashSet* scheduled = entry.value.get();
        for (SVGSMILElement* element : *scheduled)
            element->reset();
    }
#if ENABLE(ASSERT)
    m_preventScheduledAnimationsChanges = false;
#endif

    updateAnimationsAndScheduleFrameIfNeeded(time, true);
}

bool SMILTimeContainer::isTimelineRunning() const
{
    return m_beginTime && !isPaused();
}

void SMILTimeContainer::scheduleAnimationFrame(SMILTime fireTime)
{
    ASSERT(isTimelineRunning() && fireTime.isFinite());
    ASSERT(!m_wakeupTimer.isActive());

    SMILTime delay = fireTime - elapsed();
    if (delay.value() < AnimationTimeline::s_minimumDelay) {
        serviceOnNextFrame();
    } else {
        scheduleWakeUp(delay.value() - AnimationTimeline::s_minimumDelay, FutureAnimationFrame);
    }
}

void SMILTimeContainer::cancelAnimationFrame()
{
    m_frameSchedulingState = Idle;
    m_wakeupTimer.stop();
}

void SMILTimeContainer::scheduleWakeUp(double delayTime, FrameSchedulingState frameSchedulingState)
{
    ASSERT(frameSchedulingState == SynchronizeAnimations || frameSchedulingState == FutureAnimationFrame);
    m_wakeupTimer.startOneShot(delayTime, FROM_HERE);
    m_frameSchedulingState = frameSchedulingState;
}

void SMILTimeContainer::wakeupTimerFired(Timer<SMILTimeContainer>*)
{
    ASSERT(m_frameSchedulingState == SynchronizeAnimations || m_frameSchedulingState == FutureAnimationFrame);
    if (m_frameSchedulingState == FutureAnimationFrame) {
        ASSERT(isTimelineRunning());
        m_frameSchedulingState = Idle;
        serviceOnNextFrame();
    } else {
        m_frameSchedulingState = Idle;
        updateAnimationsAndScheduleFrameIfNeeded(elapsed());
    }
}

void SMILTimeContainer::scheduleAnimationPolicyTimer()
{
    m_animationPolicyOnceTimer.startOneShot(animationPolicyOnceDuration, FROM_HERE);
}

void SMILTimeContainer::cancelAnimationPolicyTimer()
{
    if (m_animationPolicyOnceTimer.isActive())
        m_animationPolicyOnceTimer.stop();
}

void SMILTimeContainer::animationPolicyTimerFired(Timer<SMILTimeContainer>*)
{
    pause();
}

ImageAnimationPolicy SMILTimeContainer::animationPolicy() const
{
    Settings* settings = document().settings();
    if (!settings)
        return ImageAnimationPolicyAllowed;

    return settings->imageAnimationPolicy();
}

bool SMILTimeContainer::handleAnimationPolicy(AnimationPolicyOnceAction onceAction)
{
    ImageAnimationPolicy policy = animationPolicy();
    // If the animation policy is "none", control is not allowed.
    // returns false to exit flow.
    if (policy == ImageAnimationPolicyNoAnimation)
        return false;
    // If the animation policy is "once",
    if (policy == ImageAnimationPolicyAnimateOnce) {
        switch (onceAction) {
        case RestartOnceTimerIfNotPaused:
            if (isPaused())
                break;
            /* fall through */
        case RestartOnceTimer:
            scheduleAnimationPolicyTimer();
            break;
        case CancelOnceTimer:
            cancelAnimationPolicyTimer();
            break;
        }
    }
    if (policy == ImageAnimationPolicyAllowed) {
        // When the SVG owner element becomes detached from its document,
        // the policy defaults to ImageAnimationPolicyAllowed; there's
        // no way back. If the policy had been "once" prior to that,
        // ensure cancellation of its timer.
        if (onceAction == CancelOnceTimer)
            cancelAnimationPolicyTimer();
    }
    return true;
}

void SMILTimeContainer::updateDocumentOrderIndexes()
{
    unsigned timingElementCount = 0;
    for (SVGSMILElement& element : Traversal<SVGSMILElement>::descendantsOf(m_ownerSVGElement))
        element.setDocumentOrderIndex(timingElementCount++);
    m_documentOrderIndexesDirty = false;
}

struct PriorityCompare {
    PriorityCompare(SMILTime elapsed) : m_elapsed(elapsed) {}
    bool operator()(const RefPtrWillBeMember<SVGSMILElement>& a, const RefPtrWillBeMember<SVGSMILElement>& b)
    {
        // FIXME: This should also consider possible timing relations between the elements.
        SMILTime aBegin = a->intervalBegin();
        SMILTime bBegin = b->intervalBegin();
        // Frozen elements need to be prioritized based on their previous interval.
        aBegin = a->isFrozen() && m_elapsed < aBegin ? a->previousIntervalBegin() : aBegin;
        bBegin = b->isFrozen() && m_elapsed < bBegin ? b->previousIntervalBegin() : bBegin;
        if (aBegin == bBegin)
            return a->documentOrderIndex() < b->documentOrderIndex();
        return aBegin < bBegin;
    }
    SMILTime m_elapsed;
};

Document& SMILTimeContainer::document() const
{
    return m_ownerSVGElement.document();
}

double SMILTimeContainer::currentTime() const
{
    return document().animationClock().currentTime();
}

void SMILTimeContainer::serviceOnNextFrame()
{
    if (document().view()) {
        document().view()->scheduleAnimation();
        m_frameSchedulingState = AnimationFrame;
    }
}

void SMILTimeContainer::serviceAnimations(double monotonicAnimationStartTime)
{
    if (m_frameSchedulingState != AnimationFrame)
        return;

    m_frameSchedulingState = Idle;
    updateAnimationsAndScheduleFrameIfNeeded(elapsed());
}

void SMILTimeContainer::updateAnimationsAndScheduleFrameIfNeeded(SMILTime elapsed, bool seekToTime)
{
    if (!document().isActive())
        return;

#if !ENABLE(OILPAN)
    DiscardScope discardScope(m_ownerSVGElement);
#endif
    SMILTime earliestFireTime = updateAnimations(elapsed, seekToTime);
    // If updateAnimations() ended up triggering a synchronization (most likely
    // via syncbases), then give that priority.
    if (hasPendingSynchronization())
        return;

    if (!isTimelineRunning())
        return;

    if (!earliestFireTime.isFinite())
        return;

    scheduleAnimationFrame(earliestFireTime);
}

SMILTime SMILTimeContainer::updateAnimations(SMILTime elapsed, bool seekToTime)
{
    ASSERT(document().isActive());
    SMILTime earliestFireTime = SMILTime::unresolved();

#if ENABLE(ASSERT)
    // This boolean will catch any attempts to schedule/unschedule scheduledAnimations during this critical section.
    // Similarly, any elements removed will unschedule themselves, so this will catch modification of animationsToApply.
    m_preventScheduledAnimationsChanges = true;
#endif

    if (m_documentOrderIndexesDirty)
        updateDocumentOrderIndexes();

    WillBeHeapHashSet<ElementAttributePair> invalidKeys;
    using AnimationsVector = WillBeHeapVector<RefPtrWillBeMember<SVGSMILElement>>;
    AnimationsVector animationsToApply;
    AnimationsVector scheduledAnimationsInSameGroup;
    for (const auto& entry : m_scheduledAnimations) {
        if (!entry.key.first || entry.value->isEmpty()) {
            invalidKeys.add(entry.key);
            continue;
        }

        // Sort according to priority. Elements with later begin time have higher priority.
        // In case of a tie, document order decides.
        // FIXME: This should also consider timing relationships between the elements. Dependents
        // have higher priority.
        copyToVector(*entry.value, scheduledAnimationsInSameGroup);
        std::sort(scheduledAnimationsInSameGroup.begin(), scheduledAnimationsInSameGroup.end(), PriorityCompare(elapsed));

        SVGSMILElement* resultElement = nullptr;
        for (const auto& itAnimation : scheduledAnimationsInSameGroup) {
            SVGSMILElement* animation = itAnimation.get();
            ASSERT(animation->timeContainer() == this);
            ASSERT(animation->targetElement());
            ASSERT(animation->hasValidAttributeName());
            ASSERT(animation->hasValidAttributeType());

            // Results are accumulated to the first animation that animates and contributes to a particular element/attribute pair.
            if (!resultElement)
                resultElement = animation;

            // This will calculate the contribution from the animation and add it to the resultsElement.
            if (!animation->progress(elapsed, resultElement, seekToTime) && resultElement == animation)
                resultElement = nullptr;

            SMILTime nextFireTime = animation->nextProgressTime();
            if (nextFireTime.isFinite())
                earliestFireTime = std::min(nextFireTime, earliestFireTime);
        }

        if (resultElement)
            animationsToApply.append(resultElement);
    }
    m_scheduledAnimations.removeAll(invalidKeys);

    std::sort(animationsToApply.begin(), animationsToApply.end(), PriorityCompare(elapsed));

    unsigned animationsToApplySize = animationsToApply.size();
    if (!animationsToApplySize) {
#if ENABLE(ASSERT)
        m_preventScheduledAnimationsChanges = false;
#endif
        return earliestFireTime;
    }

    // Apply results to target elements.
    for (unsigned i = 0; i < animationsToApplySize; ++i)
        animationsToApply[i]->applyResultsToTarget();

#if ENABLE(ASSERT)
    m_preventScheduledAnimationsChanges = false;
#endif

    for (unsigned i = 0; i < animationsToApplySize; ++i) {
        if (animationsToApply[i]->inDocument() && animationsToApply[i]->isSVGDiscardElement()) {
            RefPtrWillBeRawPtr<SVGSMILElement> animDiscard = animationsToApply[i];
            RefPtrWillBeRawPtr<SVGElement> targetElement = animDiscard->targetElement();
            if (targetElement && targetElement->inDocument()) {
                targetElement->remove(IGNORE_EXCEPTION);
                ASSERT(!targetElement->inDocument());
            }

            if (animDiscard->inDocument()) {
                animDiscard->remove(IGNORE_EXCEPTION);
                ASSERT(!animDiscard->inDocument());
            }
        }
    }
    return earliestFireTime;
}

DEFINE_TRACE(SMILTimeContainer)
{
#if ENABLE(OILPAN)
    visitor->trace(m_scheduledAnimations);
#endif
}

}
