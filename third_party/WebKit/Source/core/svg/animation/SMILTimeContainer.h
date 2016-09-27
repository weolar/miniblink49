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

#ifndef SMILTimeContainer_h
#define SMILTimeContainer_h

#include "core/dom/QualifiedName.h"
#include "core/svg/animation/SMILTime.h"
#include "platform/Timer.h"
#include "platform/graphics/ImageAnimationPolicy.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/StringHash.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Document;
class SVGElement;
class SVGSMILElement;
class SVGSVGElement;

class SMILTimeContainer : public RefCountedWillBeGarbageCollectedFinalized<SMILTimeContainer>  {
public:
    static PassRefPtrWillBeRawPtr<SMILTimeContainer> create(SVGSVGElement& owner) { return adoptRefWillBeNoop(new SMILTimeContainer(owner)); }
    ~SMILTimeContainer();

    void schedule(SVGSMILElement*, SVGElement*, const QualifiedName&);
    void unschedule(SVGSMILElement*, SVGElement*, const QualifiedName&);
    void notifyIntervalsChanged();

    SMILTime elapsed() const;

    bool isPaused() const;
    bool isStarted() const;

    void begin();
    void pause();
    void resume();
    void setElapsed(SMILTime);

    void serviceAnimations(double monotonicAnimationStartTime);
    bool hasAnimations() const;

    void setDocumentOrderIndexesDirty() { m_documentOrderIndexesDirty = true; }

    DECLARE_TRACE();

private:
    explicit SMILTimeContainer(SVGSVGElement& owner);

    enum FrameSchedulingState {
        // No frame scheduled.
        Idle,
        // Scheduled a wakeup to update the animation values.
        SynchronizeAnimations,
        // Scheduled a wakeup to trigger an animation frame.
        FutureAnimationFrame,
        // Scheduled a animation frame for continuous update.
        AnimationFrame
    };

    enum AnimationPolicyOnceAction {
        // Restart OnceTimer if the timeline is not paused.
        RestartOnceTimerIfNotPaused,
        // Restart OnceTimer.
        RestartOnceTimer,
        // Cancel OnceTimer.
        CancelOnceTimer
    };

    bool isTimelineRunning() const;
    void scheduleAnimationFrame(SMILTime fireTime);
    void cancelAnimationFrame();
    void wakeupTimerFired(Timer<SMILTimeContainer>*);
    void scheduleAnimationPolicyTimer();
    void cancelAnimationPolicyTimer();
    void animationPolicyTimerFired(Timer<SMILTimeContainer>*);
    ImageAnimationPolicy animationPolicy() const;
    bool handleAnimationPolicy(AnimationPolicyOnceAction);
    void updateAnimationsAndScheduleFrameIfNeeded(SMILTime elapsed, bool seekToTime = false);
    SMILTime updateAnimations(SMILTime elapsed, bool seekToTime = false);
    void serviceOnNextFrame();
    void scheduleWakeUp(double delayTime, FrameSchedulingState);
    bool hasPendingSynchronization() const;

    void updateDocumentOrderIndexes();
    double lastResumeTime() const { return m_resumeTime ? m_resumeTime : m_beginTime; }

    Document& document() const;
    double currentTime() const;

    double m_beginTime;
    double m_pauseTime;
    double m_resumeTime;
    double m_accumulatedActiveTime;
    double m_presetStartTime;

    FrameSchedulingState m_frameSchedulingState;
    bool m_documentOrderIndexesDirty;

    Timer<SMILTimeContainer> m_wakeupTimer;
    Timer<SMILTimeContainer> m_animationPolicyOnceTimer;

    using ElementAttributePair = pair<RawPtrWillBeWeakMember<SVGElement>, QualifiedName>;
    using AnimationsLinkedHashSet = WillBeHeapLinkedHashSet<RawPtrWillBeWeakMember<SVGSMILElement>>;
    using GroupedAnimationsMap = WillBeHeapHashMap<ElementAttributePair, OwnPtrWillBeMember<AnimationsLinkedHashSet>>;
    GroupedAnimationsMap m_scheduledAnimations;

    SVGSVGElement& m_ownerSVGElement;

#if ENABLE(ASSERT)
    bool m_preventScheduledAnimationsChanges;
#endif
};
}

#endif // SMILTimeContainer_h
