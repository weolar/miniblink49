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

#ifndef AnimationEffect_h
#define AnimationEffect_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/animation/Timing.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class Animation;
class AnimationEffect;
class AnimationEffectTiming;
class ComputedTimingProperties;

enum TimingUpdateReason {
    TimingUpdateOnDemand,
    TimingUpdateForAnimationFrame
};

static inline bool isNull(double value)
{
    return std::isnan(value);
}

static inline double nullValue()
{
    return std::numeric_limits<double>::quiet_NaN();
}

class CORE_EXPORT AnimationEffect : public RefCountedWillBeGarbageCollectedFinalized<AnimationEffect>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    friend class Animation; // Calls attach/detach, updateInheritedTime.
public:
    // Note that logic in CSSAnimations depends on the order of these values.
    enum Phase {
        PhaseBefore,
        PhaseActive,
        PhaseAfter,
        PhaseNone,
    };

    class EventDelegate : public NoBaseWillBeGarbageCollectedFinalized<EventDelegate> {
    public:
        virtual ~EventDelegate() { }
        virtual bool requiresIterationEvents(const AnimationEffect&) = 0;
        virtual void onEventCondition(const AnimationEffect&) = 0;
        DEFINE_INLINE_VIRTUAL_TRACE() { }
    };

    virtual ~AnimationEffect() { }

    virtual bool isAnimation() const { return false; }

    Phase phase() const { return ensureCalculated().phase; }
    bool isCurrent() const { return ensureCalculated().isCurrent; }
    bool isInEffect() const { return ensureCalculated().isInEffect; }
    bool isInPlay() const { return ensureCalculated().isInPlay; }
    double currentIteration() const { return ensureCalculated().currentIteration; }
    double timeFraction() const { return ensureCalculated().timeFraction; }
    double timeToForwardsEffectChange() const { return ensureCalculated().timeToForwardsEffectChange; }
    double timeToReverseEffectChange() const { return ensureCalculated().timeToReverseEffectChange; }

    double iterationDuration() const;
    double activeDurationInternal() const;
    double startTimeInternal() const { return m_startTime; }
    double endTimeInternal() const { return std::max(startTimeInternal(), startTimeInternal() + specifiedTiming().startDelay + activeDurationInternal() + specifiedTiming().endDelay); }

    const Animation* animation() const { return m_animation; }
    Animation* animation() { return m_animation; }
    const Timing& specifiedTiming() const { return m_timing; }
    PassRefPtrWillBeRawPtr<AnimationEffectTiming> timing();
    void updateSpecifiedTiming(const Timing&);

    void computedTiming(ComputedTimingProperties&);
    ComputedTimingProperties computedTiming();

    void setName(const String& name) { m_name = name; }
    const String& name() const { return m_name; }

    DECLARE_VIRTUAL_TRACE();

protected:
    explicit AnimationEffect(const Timing&, PassOwnPtrWillBeRawPtr<EventDelegate> = nullptr);

    // When AnimationEffect receives a new inherited time via updateInheritedTime
    // it will (if necessary) recalculate timings and (if necessary) call
    // updateChildrenAndEffects.
    void updateInheritedTime(double inheritedTime, TimingUpdateReason) const;
    void invalidate() const { m_needsUpdate = true; }
    bool requiresIterationEvents() const { return m_eventDelegate && m_eventDelegate->requiresIterationEvents(*this); }
    void clearEventDelegate() { m_eventDelegate = nullptr; }

    virtual void attach(Animation* animation)
    {
        m_animation = animation;
    }

    virtual void detach()
    {
        ASSERT(m_animation);
        m_animation = nullptr;
    }

    double repeatedDuration() const;

    virtual void updateChildrenAndEffects() const = 0;
    virtual double intrinsicIterationDuration() const { return 0; }
    virtual double calculateTimeToEffectChange(bool forwards, double localTime, double timeToNextIteration) const = 0;
    virtual void specifiedTimingChanged() { }

    // FIXME: m_parent and m_startTime are placeholders, they depend on timing groups.
    RawPtrWillBeMember<AnimationEffect> m_parent;
    const double m_startTime;
    RawPtrWillBeMember<Animation> m_animation;
    Timing m_timing;
    OwnPtrWillBeMember<EventDelegate> m_eventDelegate;

    mutable struct CalculatedTiming {
        Phase phase;
        double currentIteration;
        double timeFraction;
        bool isCurrent;
        bool isInEffect;
        bool isInPlay;
        double localTime;
        double timeToForwardsEffectChange;
        double timeToReverseEffectChange;
    } m_calculated;
    mutable bool m_needsUpdate;
    mutable double m_lastUpdateTime;
    String m_name;

    const CalculatedTiming& ensureCalculated() const;
};

} // namespace blink

#endif // AnimationEffect_h
