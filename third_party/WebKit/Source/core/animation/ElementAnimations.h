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

#ifndef ElementAnimations_h
#define ElementAnimations_h

#include "core/animation/AnimationStack.h"
#include "core/animation/css/CSSAnimations.h"
#include "wtf/HashCountedSet.h"
#include "wtf/HashMap.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class CSSAnimations;

using AnimationCountedSet = WillBeHeapHashCountedSet<RawPtrWillBeWeakMember<Animation>>;

class ElementAnimations : public NoBaseWillBeGarbageCollectedFinalized<ElementAnimations> {
    WTF_MAKE_NONCOPYABLE(ElementAnimations);
public:
    ElementAnimations();
    ~ElementAnimations();

    // Animations that are currently active for this element, their effects will be applied
    // during a style recalc. CSS Transitions are included in this stack.
    AnimationStack& defaultStack() { return m_defaultStack; }
    const AnimationStack& defaultStack() const { return m_defaultStack; }
    // Tracks the state of active CSS Animations and Transitions. The individual animations
    // will also be part of the default stack, but the mapping betwen animation name and
    // animation is kept here.
    CSSAnimations& cssAnimations() { return m_cssAnimations; }
    const CSSAnimations& cssAnimations() const { return m_cssAnimations; }

    // Animations which have effects targeting this element.
    AnimationCountedSet& animations() { return m_animations; }

    bool isEmpty() const { return m_defaultStack.isEmpty() && m_cssAnimations.isEmpty() && m_animations.isEmpty(); }

    void restartAnimationOnCompositor();

    void updateAnimationFlags(ComputedStyle&);
    void setAnimationStyleChange(bool animationStyleChange) { m_animationStyleChange = animationStyleChange; }

    const ComputedStyle* baseComputedStyle() const;
    void updateBaseComputedStyle(const ComputedStyle*);
    void clearBaseComputedStyle();

#if !ENABLE(OILPAN)
    void addEffect(KeyframeEffect* effect) { m_effects.append(effect); }
    void notifyEffectDestroyed(KeyframeEffect* effect) { m_effects.remove(m_effects.find(effect)); }
#endif

    DECLARE_TRACE();

private:
    bool isAnimationStyleChange() const;

    AnimationStack m_defaultStack;
    CSSAnimations m_cssAnimations;
    AnimationCountedSet m_animations;
    bool m_animationStyleChange;
    RefPtr<ComputedStyle> m_baseComputedStyle;

#if !ENABLE(OILPAN)
    // FIXME: Oilpan: This is to avoid a reference cycle that keeps Elements alive
    // and won't be needed once the Node hierarchy becomes traceable.
    Vector<KeyframeEffect*> m_effects;
#endif

    // CSSAnimations and DeferredLegacyStyleInterpolation checks if a style change is due to animation.
    friend class CSSAnimations;
    friend class DeferredLegacyStyleInterpolation;
};

} // namespace blink

#endif // ElementAnimations_h
