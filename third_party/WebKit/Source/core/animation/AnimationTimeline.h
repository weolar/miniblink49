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

#ifndef AnimationTimeline_h
#define AnimationTimeline_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/animation/Animation.h"
#include "core/animation/EffectModel.h"
#include "core/dom/Element.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebCompositorAnimationTimeline.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class Document;
class AnimationEffect;

// AnimationTimeline is constructed and owned by Document, and tied to its lifecycle.
class CORE_EXPORT AnimationTimeline : public RefCountedWillBeGarbageCollectedFinalized<AnimationTimeline>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    class PlatformTiming : public NoBaseWillBeGarbageCollectedFinalized<PlatformTiming> {

    public:
        // Calls AnimationTimeline's wake() method after duration seconds.
        virtual void wakeAfter(double duration) = 0;
        virtual void cancelWake() = 0;
        virtual void serviceOnNextFrame() = 0;
        virtual ~PlatformTiming() { }
        DEFINE_INLINE_VIRTUAL_TRACE() { }
    };

    static PassRefPtrWillBeRawPtr<AnimationTimeline> create(Document*, PassOwnPtrWillBeRawPtr<PlatformTiming> = nullptr);
    ~AnimationTimeline();

    void serviceAnimations(TimingUpdateReason);
    void scheduleNextService();

    Animation* play(AnimationEffect*);
    WillBeHeapVector<RefPtrWillBeMember<Animation>> getAnimations();

    void animationAttached(Animation&);
#if !ENABLE(OILPAN)
    void animationDestroyed(Animation* animation)
    {
        ASSERT(m_animations.contains(animation));
        m_animations.remove(animation);
    }
#endif

    bool hasPendingUpdates() const { return !m_animationsNeedingUpdate.isEmpty(); }
    double zeroTime();
    double currentTime(bool& isNull);
    double currentTime();
    double currentTimeInternal(bool& isNull);
    double currentTimeInternal();
    void setCurrentTime(double);
    void setCurrentTimeInternal(double);
    double effectiveTime();
    void pauseAnimationsForTesting(double);

    void setOutdatedAnimation(Animation*);
    void clearOutdatedAnimation(Animation*);
    bool hasOutdatedAnimation() const { return m_outdatedAnimationCount > 0; }
    bool needsAnimationTimingUpdate();

    void setPlaybackRate(double);
    double playbackRate() const;

    WebCompositorAnimationTimeline* compositorTimeline() const { return m_compositorTimeline.get(); }

    Document* document() { return m_document.get(); }
#if !ENABLE(OILPAN)
    void detachFromDocument();
#endif
    void wake();

    DECLARE_TRACE();

protected:
    AnimationTimeline(Document*, PassOwnPtrWillBeRawPtr<PlatformTiming>);

private:
    RawPtrWillBeMember<Document> m_document;
    double m_zeroTime;
    bool m_zeroTimeInitialized;
    unsigned m_outdatedAnimationCount;
    // Animations which will be updated on the next frame
    // i.e. current, in effect, or had timing changed
    WillBeHeapHashSet<RefPtrWillBeMember<Animation>> m_animationsNeedingUpdate;
    WillBeHeapHashSet<RawPtrWillBeWeakMember<Animation>> m_animations;

    double m_playbackRate;

    friend class SMILTimeContainer;
    static const double s_minimumDelay;

    OwnPtrWillBeMember<PlatformTiming> m_timing;
    double m_lastCurrentTimeInternal;

    OwnPtr<WebCompositorAnimationTimeline> m_compositorTimeline;

    class AnimationTimelineTiming final : public PlatformTiming {
    public:
        AnimationTimelineTiming(AnimationTimeline* timeline)
            : m_timeline(timeline)
            , m_timer(this, &AnimationTimelineTiming::timerFired)
        {
            ASSERT(m_timeline);
        }

        virtual void wakeAfter(double duration) override;
        virtual void cancelWake() override;
        virtual void serviceOnNextFrame() override;

        void timerFired(Timer<AnimationTimelineTiming>*) { m_timeline->wake(); }

        DECLARE_VIRTUAL_TRACE();

    private:
        RawPtrWillBeMember<AnimationTimeline> m_timeline;
        Timer<AnimationTimelineTiming> m_timer;
    };

    friend class AnimationAnimationTimelineTest;
};

} // namespace blink

#endif
