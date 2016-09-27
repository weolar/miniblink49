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

#ifndef CSSAnimations_h
#define CSSAnimations_h

#include "core/animation/InertEffect.h"
#include "core/animation/Interpolation.h"
#include "core/animation/css/CSSAnimationData.h"
#include "core/animation/css/CSSAnimationUpdate.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "wtf/HashMap.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class CSSTransitionData;
class Element;
class StylePropertyShorthand;
class StyleResolver;

class CSSAnimations final {
    WTF_MAKE_NONCOPYABLE(CSSAnimations);
    DISALLOW_ALLOCATION();
public:
    CSSAnimations();

    bool isAnimationForInspector(const Animation&);
    bool isTransitionAnimationForInspector(const Animation&) const;

    static const StylePropertyShorthand& propertiesForTransitionAll();
    static bool isAnimatableProperty(CSSPropertyID);
    static PassOwnPtrWillBeRawPtr<CSSAnimationUpdate> calculateUpdate(const Element* animatingElement, Element&, const ComputedStyle&, ComputedStyle* parentStyle, StyleResolver*);

    void setPendingUpdate(PassOwnPtrWillBeRawPtr<CSSAnimationUpdate> update) { m_pendingUpdate = update; }
    void maybeApplyPendingUpdate(Element*);
    bool isEmpty() const { return m_animations.isEmpty() && m_transitions.isEmpty() && !m_pendingUpdate; }
    void cancel();

    DECLARE_TRACE();

private:
    class RunningAnimation final : public RefCountedWillBeGarbageCollectedFinalized<RunningAnimation> {
    public:
        RunningAnimation(PassRefPtrWillBeRawPtr<Animation> animation, CSSAnimationUpdate::NewAnimation newAnimation)
            : animation(animation)
            , specifiedTiming(newAnimation.timing)
            , styleRule(newAnimation.styleRule)
            , styleRuleVersion(newAnimation.styleRuleVersion)
        {
        }

        void update(CSSAnimationUpdate::UpdatedAnimation update)
        {
            styleRule = update.styleRule;
            styleRuleVersion = update.styleRuleVersion;
            specifiedTiming = update.specifiedTiming;
        }

        DEFINE_INLINE_TRACE()
        {
            visitor->trace(animation);
            visitor->trace(styleRule);
        }

        RefPtrWillBeMember<Animation> animation;
        Timing specifiedTiming;
        RefPtrWillBeMember<StyleRuleKeyframes> styleRule;
        unsigned styleRuleVersion;
    };

    struct RunningTransition {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        DEFINE_INLINE_TRACE()
        {
            visitor->trace(animation);
            visitor->trace(from);
            visitor->trace(to);
        }

        RefPtrWillBeMember<Animation> animation;
        RawPtrWillBeMember<const AnimatableValue> from;
        RawPtrWillBeMember<const AnimatableValue> to;
    };

    using AnimationMap = WillBeHeapHashMap<AtomicString, RefPtrWillBeMember<RunningAnimation>>;
    AnimationMap m_animations;

    using TransitionMap = WillBeHeapHashMap<CSSPropertyID, RunningTransition>;
    TransitionMap m_transitions;

    OwnPtrWillBeMember<CSSAnimationUpdate> m_pendingUpdate;

    ActiveInterpolationMap m_previousActiveInterpolationsForAnimations;

    static void calculateAnimationUpdate(CSSAnimationUpdate*, const Element* animatingElement, Element&, const ComputedStyle&, ComputedStyle* parentStyle, StyleResolver*);
    static void calculateTransitionUpdate(CSSAnimationUpdate*, const Element* animatingElement, const ComputedStyle&);
    static void calculateTransitionUpdateForProperty(CSSPropertyID, const CSSTransitionData&, size_t transitionIndex, const ComputedStyle& oldStyle, const ComputedStyle&, const TransitionMap* activeTransitions, CSSAnimationUpdate*, const Element*);

    static void calculateAnimationActiveInterpolations(CSSAnimationUpdate*, const Element* animatingElement, double timelineCurrentTime);
    static void calculateTransitionActiveInterpolations(CSSAnimationUpdate*, const Element* animatingElement, double timelineCurrentTime);

    class AnimationEventDelegate final : public AnimationEffect::EventDelegate {
    public:
        AnimationEventDelegate(Element* animationTarget, const AtomicString& name)
            : m_animationTarget(animationTarget)
            , m_name(name)
            , m_previousPhase(AnimationEffect::PhaseNone)
            , m_previousIteration(nullValue())
        {
        }
        virtual bool requiresIterationEvents(const AnimationEffect&) override;
        virtual void onEventCondition(const AnimationEffect&) override;
        DECLARE_VIRTUAL_TRACE();

    private:
        const Element& animationTarget() const { return *m_animationTarget; }
        EventTarget* eventTarget() const;
        Document& document() const { return m_animationTarget->document(); }

        void maybeDispatch(Document::ListenerType, const AtomicString& eventName, double elapsedTime);
        RawPtrWillBeMember<Element> m_animationTarget;
        const AtomicString m_name;
        AnimationEffect::Phase m_previousPhase;
        double m_previousIteration;
    };

    class TransitionEventDelegate final : public AnimationEffect::EventDelegate {
    public:
        TransitionEventDelegate(Element* transitionTarget, CSSPropertyID property)
            : m_transitionTarget(transitionTarget)
            , m_property(property)
            , m_previousPhase(AnimationEffect::PhaseNone)
        {
        }
        virtual bool requiresIterationEvents(const AnimationEffect&) override { return false; }
        virtual void onEventCondition(const AnimationEffect&) override;
        DECLARE_VIRTUAL_TRACE();

    private:
        const Element& transitionTarget() const { return *m_transitionTarget; }
        EventTarget* eventTarget() const;
        PseudoId pseudoId() const { return m_transitionTarget->pseudoId(); }
        Document& document() const { return m_transitionTarget->document(); }

        RawPtrWillBeMember<Element> m_transitionTarget;
        const CSSPropertyID m_property;
        AnimationEffect::Phase m_previousPhase;
    };
};

} // namespace blink

#endif
