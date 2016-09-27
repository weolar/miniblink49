// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSAnimationUpdate_h
#define CSSAnimationUpdate_h

#include "core/animation/AnimationStack.h"
#include "core/animation/Interpolation.h"
#include "core/animation/KeyframeEffectModel.h"
#include "core/animation/css/CSSAnimatableValueFactory.h"
#include "core/animation/css/CSSPropertyEquality.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/layout/LayoutObject.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class Animation;
class InertEffect;

// This class stores the CSS Animations/Transitions information we use during a style recalc.
// This includes updates to animations/transitions as well as the Interpolations to be applied.
class CSSAnimationUpdate final : public NoBaseWillBeGarbageCollectedFinalized<CSSAnimationUpdate> {
public:
    class NewAnimation {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        NewAnimation()
            : styleRuleVersion(0)
        {
        }

        NewAnimation(AtomicString name, PassRefPtrWillBeRawPtr<InertEffect> effect, Timing timing, PassRefPtrWillBeRawPtr<StyleRuleKeyframes> styleRule)
            : name(name)
            , effect(effect)
            , timing(timing)
            , styleRule(styleRule)
            , styleRuleVersion(this->styleRule->version())
        {
        }

        DEFINE_INLINE_TRACE()
        {
            visitor->trace(effect);
            visitor->trace(styleRule);
        }

        AtomicString name;
        RefPtrWillBeMember<InertEffect> effect;
        Timing timing;
        RefPtrWillBeMember<StyleRuleKeyframes> styleRule;
        unsigned styleRuleVersion;
    };

    class UpdatedAnimation {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        UpdatedAnimation()
            : styleRuleVersion(0)
        {
        }

        UpdatedAnimation(AtomicString name, Animation* animation, PassRefPtrWillBeRawPtr<InertEffect> effect, Timing specifiedTiming, PassRefPtrWillBeRawPtr<StyleRuleKeyframes> styleRule)
            : name(name)
            , animation(animation)
            , effect(effect)
            , specifiedTiming(specifiedTiming)
            , styleRule(styleRule)
            , styleRuleVersion(this->styleRule->version())
        {
        }

        DEFINE_INLINE_TRACE()
        {
            visitor->trace(animation);
            visitor->trace(effect);
            visitor->trace(styleRule);
        }

        AtomicString name;
        RawPtrWillBeMember<Animation> animation;
        RefPtrWillBeMember<InertEffect> effect;
        Timing specifiedTiming;
        RefPtrWillBeMember<StyleRuleKeyframes> styleRule;
        unsigned styleRuleVersion;
    };

    class UpdatedAnimationStyle {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        struct CompositableStyleSnapshot {
            DISALLOW_ALLOCATION();

        public:
            RefPtrWillBeMember<AnimatableValue> opacity;
            RefPtrWillBeMember<AnimatableValue> transform;
            RefPtrWillBeMember<AnimatableValue> webkitFilter;

            DEFINE_INLINE_TRACE()
            {
                visitor->trace(opacity);
                visitor->trace(transform);
                visitor->trace(webkitFilter);
            }
        };

        UpdatedAnimationStyle()
        {
        }

        UpdatedAnimationStyle(Animation* animation, KeyframeEffectModelBase* model, const UpdatedAnimationStyle::CompositableStyleSnapshot& snapshot)
            : animation(animation)
            , model(model)
            , snapshot(snapshot)
        {
        }

        DEFINE_INLINE_TRACE()
        {
            visitor->trace(animation);
            visitor->trace(model);
            visitor->trace(snapshot);
        }

        RawPtrWillBeMember<Animation> animation;
        RawPtrWillBeMember<KeyframeEffectModelBase> model;
        CompositableStyleSnapshot snapshot;
    };

    void startAnimation(const AtomicString& animationName, PassRefPtrWillBeRawPtr<InertEffect> effect, const Timing& timing, PassRefPtrWillBeRawPtr<StyleRuleKeyframes> styleRule)
    {
        effect->setName(animationName);
        m_newAnimations.append(NewAnimation(animationName, effect, timing, styleRule));
    }
    // Returns whether animation has been suppressed and should be filtered during style application.
    bool isSuppressedAnimation(const Animation* animation) const { return m_suppressedAnimations.contains(animation); }
    void cancelAnimation(const AtomicString& name, Animation& animation)
    {
        m_cancelledAnimationNames.append(name);
        m_suppressedAnimations.add(&animation);
    }
    void toggleAnimationPaused(const AtomicString& name)
    {
        m_animationsWithPauseToggled.append(name);
    }
    void updateAnimation(const AtomicString& name, Animation* animation, PassRefPtrWillBeRawPtr<InertEffect> effect, const Timing& specifiedTiming,
        PassRefPtrWillBeRawPtr<StyleRuleKeyframes> styleRule)
    {
        m_animationsWithUpdates.append(UpdatedAnimation(name, animation, effect, specifiedTiming, styleRule));
        m_suppressedAnimations.add(animation);
    }
    void updateAnimationStyle(Animation* animation, KeyframeEffectModelBase* model, LayoutObject* layoutObject, const ComputedStyle& newStyle)
    {
        UpdatedAnimationStyle::CompositableStyleSnapshot snapshot;
        if (layoutObject) {
            const ComputedStyle& oldStyle = layoutObject->styleRef();
            if (!CSSPropertyEquality::propertiesEqual(CSSPropertyOpacity, oldStyle, newStyle) && model->affects(PropertyHandle(CSSPropertyOpacity)))
                snapshot.opacity = CSSAnimatableValueFactory::create(CSSPropertyOpacity, newStyle);
            if (!CSSPropertyEquality::propertiesEqual(CSSPropertyTransform, oldStyle, newStyle) && model->affects(PropertyHandle(CSSPropertyTransform)))
                snapshot.transform = CSSAnimatableValueFactory::create(CSSPropertyTransform, newStyle);
            if (!CSSPropertyEquality::propertiesEqual(CSSPropertyWebkitFilter, oldStyle, newStyle) && model->affects(PropertyHandle(CSSPropertyWebkitFilter)))
                snapshot.webkitFilter = CSSAnimatableValueFactory::create(CSSPropertyWebkitFilter, newStyle);
        }

        m_animationsWithStyleUpdates.append(UpdatedAnimationStyle(animation, model, snapshot));
    }

    void startTransition(CSSPropertyID id, const AnimatableValue* from, const AnimatableValue* to, PassRefPtrWillBeRawPtr<InertEffect> effect)
    {
        effect->setName(getPropertyName(id));
        NewTransition newTransition;
        newTransition.id = id;
        newTransition.from = from;
        newTransition.to = to;
        newTransition.effect = effect;
        m_newTransitions.set(id, newTransition);
    }
    bool isCancelledTransition(CSSPropertyID id) const { return m_cancelledTransitions.contains(id); }
    void cancelTransition(CSSPropertyID id) { m_cancelledTransitions.add(id); }
    void finishTransition(CSSPropertyID id) { m_finishedTransitions.add(id); }

    const WillBeHeapVector<NewAnimation>& newAnimations() const { return m_newAnimations; }
    const Vector<AtomicString>& cancelledAnimationNames() const { return m_cancelledAnimationNames; }
    const WillBeHeapHashSet<RawPtrWillBeMember<const Animation>>& suppressedAnimations() const { return m_suppressedAnimations; }
    const Vector<AtomicString>& animationsWithPauseToggled() const { return m_animationsWithPauseToggled; }
    const WillBeHeapVector<UpdatedAnimation>& animationsWithUpdates() const { return m_animationsWithUpdates; }
    const WillBeHeapVector<UpdatedAnimationStyle>& animationsWithStyleUpdates() const { return m_animationsWithStyleUpdates; }

    struct NewTransition {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        DEFINE_INLINE_TRACE()
        {
            visitor->trace(from);
            visitor->trace(to);
            visitor->trace(effect);
        }

        CSSPropertyID id;
        RawPtrWillBeMember<const AnimatableValue> from;
        RawPtrWillBeMember<const AnimatableValue> to;
        RefPtrWillBeMember<InertEffect> effect;
    };
    using NewTransitionMap = WillBeHeapHashMap<CSSPropertyID, NewTransition>;
    const NewTransitionMap& newTransitions() const { return m_newTransitions; }
    const HashSet<CSSPropertyID>& cancelledTransitions() const { return m_cancelledTransitions; }
    const HashSet<CSSPropertyID>& finishedTransitions() const { return m_finishedTransitions; }

    void adoptActiveInterpolationsForAnimations(ActiveInterpolationMap& newMap) { newMap.swap(m_activeInterpolationsForAnimations); }
    void adoptActiveInterpolationsForTransitions(ActiveInterpolationMap& newMap) { newMap.swap(m_activeInterpolationsForTransitions); }
    const ActiveInterpolationMap& activeInterpolationsForAnimations() const { return m_activeInterpolationsForAnimations; }
    const ActiveInterpolationMap& activeInterpolationsForTransitions() const { return m_activeInterpolationsForTransitions; }
    ActiveInterpolationMap& activeInterpolationsForAnimations() { return m_activeInterpolationsForAnimations; }

    bool isEmpty() const
    {
        return m_newAnimations.isEmpty()
            && m_cancelledAnimationNames.isEmpty()
            && m_suppressedAnimations.isEmpty()
            && m_animationsWithPauseToggled.isEmpty()
            && m_animationsWithUpdates.isEmpty()
            && m_animationsWithStyleUpdates.isEmpty()
            && m_newTransitions.isEmpty()
            && m_cancelledTransitions.isEmpty()
            && m_finishedTransitions.isEmpty()
            && m_activeInterpolationsForAnimations.isEmpty()
            && m_activeInterpolationsForTransitions.isEmpty();
    }

    DECLARE_TRACE();

private:
    // Order is significant since it defines the order in which new animations
    // will be started. Note that there may be multiple animations present
    // with the same name, due to the way in which we split up animations with
    // incomplete keyframes.
    WillBeHeapVector<NewAnimation> m_newAnimations;
    Vector<AtomicString> m_cancelledAnimationNames;
    WillBeHeapHashSet<RawPtrWillBeMember<const Animation>> m_suppressedAnimations;
    Vector<AtomicString> m_animationsWithPauseToggled;
    WillBeHeapVector<UpdatedAnimation> m_animationsWithUpdates;
    WillBeHeapVector<UpdatedAnimationStyle> m_animationsWithStyleUpdates;

    NewTransitionMap m_newTransitions;
    HashSet<CSSPropertyID> m_cancelledTransitions;
    HashSet<CSSPropertyID> m_finishedTransitions;

    ActiveInterpolationMap m_activeInterpolationsForAnimations;
    ActiveInterpolationMap m_activeInterpolationsForTransitions;
};

} // namespace blink

WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::CSSAnimationUpdate::NewAnimation);
WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::CSSAnimationUpdate::UpdatedAnimation);
WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::CSSAnimationUpdate::UpdatedAnimationStyle);

#endif
