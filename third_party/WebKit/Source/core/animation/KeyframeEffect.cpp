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
#include "core/animation/KeyframeEffect.h"

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/animation/Animation.h"
#include "core/animation/AnimationTimeline.h"
#include "core/animation/CompositorAnimations.h"
#include "core/animation/ElementAnimations.h"
#include "core/animation/Interpolation.h"
#include "core/animation/KeyframeEffectModel.h"
#include "core/animation/KeyframeEffectOptions.h"
#include "core/animation/PropertyHandle.h"
#include "core/dom/Element.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/frame/UseCounter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/svg/SVGElement.h"

namespace blink {

PassRefPtrWillBeRawPtr<KeyframeEffect> KeyframeEffect::create(Element* target, PassRefPtrWillBeRawPtr<EffectModel> model, const Timing& timing, Priority priority, PassOwnPtrWillBeRawPtr<EventDelegate> eventDelegate)
{
    return adoptRefWillBeNoop(new KeyframeEffect(target, model, timing, priority, eventDelegate));
}

PassRefPtrWillBeRawPtr<KeyframeEffect> KeyframeEffect::create(Element* element, const Vector<Dictionary>& keyframeDictionaryVector, double duration, ExceptionState& exceptionState)
{
    ASSERT(RuntimeEnabledFeatures::webAnimationsAPIEnabled());
    if (element)
        UseCounter::count(element->document(), UseCounter::AnimationConstructorKeyframeListEffectObjectTiming);
    return create(element, EffectInput::convert(element, keyframeDictionaryVector, exceptionState), TimingInput::convert(duration));
}
PassRefPtrWillBeRawPtr<KeyframeEffect> KeyframeEffect::create(Element* element, const Vector<Dictionary>& keyframeDictionaryVector, const KeyframeEffectOptions& timingInput, ExceptionState& exceptionState)
{
    ASSERT(RuntimeEnabledFeatures::webAnimationsAPIEnabled());
    if (element)
        UseCounter::count(element->document(), UseCounter::AnimationConstructorKeyframeListEffectObjectTiming);
    return create(element, EffectInput::convert(element, keyframeDictionaryVector, exceptionState), TimingInput::convert(timingInput));
}
PassRefPtrWillBeRawPtr<KeyframeEffect> KeyframeEffect::create(Element* element, const Vector<Dictionary>& keyframeDictionaryVector, ExceptionState& exceptionState)
{
    ASSERT(RuntimeEnabledFeatures::webAnimationsAPIEnabled());
    if (element)
        UseCounter::count(element->document(), UseCounter::AnimationConstructorKeyframeListEffectNoTiming);
    return create(element, EffectInput::convert(element, keyframeDictionaryVector, exceptionState), Timing());
}

KeyframeEffect::KeyframeEffect(Element* target, PassRefPtrWillBeRawPtr<EffectModel> model, const Timing& timing, Priority priority, PassOwnPtrWillBeRawPtr<EventDelegate> eventDelegate)
    : AnimationEffect(timing, eventDelegate)
    , m_target(target)
    , m_model(model)
    , m_sampledEffect(nullptr)
    , m_priority(priority)
{
#if !ENABLE(OILPAN)
    if (m_target)
        m_target->ensureElementAnimations().addEffect(this);
#endif
}

KeyframeEffect::~KeyframeEffect()
{
#if !ENABLE(OILPAN)
    if (m_target)
        m_target->elementAnimations()->notifyEffectDestroyed(this);
#endif
}

void KeyframeEffect::attach(Animation* animation)
{
    if (m_target) {
        m_target->ensureElementAnimations().animations().add(animation);
        m_target->setNeedsAnimationStyleRecalc();
    }
    AnimationEffect::attach(animation);
}

void KeyframeEffect::detach()
{
    if (m_target)
        m_target->elementAnimations()->animations().remove(animation());
    if (m_sampledEffect)
        clearEffects();
    AnimationEffect::detach();
}

void KeyframeEffect::specifiedTimingChanged()
{
    if (animation()) {
        // FIXME: Needs to consider groups when added.
        ASSERT(animation()->effect() == this);
        animation()->setCompositorPending(true);
    }
}

static AnimationStack& ensureAnimationStack(Element* element)
{
    return element->ensureElementAnimations().defaultStack();
}

bool KeyframeEffect::hasMultipleTransformProperties() const
{
    if (!m_target->computedStyle())
        return false;

    unsigned transformPropertyCount = 0;
    if (m_target->computedStyle()->hasTransformOperations())
        transformPropertyCount++;
    if (m_target->computedStyle()->rotate())
        transformPropertyCount++;
    if (m_target->computedStyle()->scale())
        transformPropertyCount++;
    if (m_target->computedStyle()->translate())
        transformPropertyCount++;
    return transformPropertyCount > 1;
}

// Returns true if transform, translate, rotate or scale is composited
// and a motion path or other transform properties
// has been introduced on the element
bool KeyframeEffect::hasIncompatibleStyle()
{
    if (!m_target->computedStyle())
        return false;

    bool affectsTransform = animation()->affects(*m_target, CSSPropertyTransform)
        || animation()->affects(*m_target, CSSPropertyScale)
        || animation()->affects(*m_target, CSSPropertyRotate)
        || animation()->affects(*m_target, CSSPropertyTranslate);

    if (animation()->hasActiveAnimationsOnCompositor()) {
        if (m_target->computedStyle()->hasMotionPath() && affectsTransform)
            return true;
        return hasMultipleTransformProperties();
    }

    return false;
}

void KeyframeEffect::applyEffects()
{
    ASSERT(isInEffect());
    ASSERT(animation());
    if (!m_target || !m_model)
        return;

    if (hasIncompatibleStyle())
        animation()->cancelAnimationOnCompositor();

    double iteration = currentIteration();
    ASSERT(iteration >= 0);
    OwnPtrWillBeRawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>> interpolations = m_sampledEffect ? m_sampledEffect->mutableInterpolations() : nullptr;
    // FIXME: Handle iteration values which overflow int.
    m_model->sample(static_cast<int>(iteration), timeFraction(), iterationDuration(), interpolations);
    if (m_sampledEffect) {
        m_sampledEffect->setInterpolations(interpolations.release());
    } else if (interpolations && !interpolations->isEmpty()) {
        OwnPtrWillBeRawPtr<SampledEffect> sampledEffect = SampledEffect::create(this, interpolations.release());
        m_sampledEffect = sampledEffect.get();
        ensureAnimationStack(m_target).add(sampledEffect.release());
    } else {
        return;
    }

    m_target->setNeedsAnimationStyleRecalc();
    if (m_target->isSVGElement())
        m_sampledEffect->applySVGUpdate(toSVGElement(*m_target));
}

void KeyframeEffect::clearEffects()
{
    ASSERT(animation());
    ASSERT(m_sampledEffect);

    m_sampledEffect->clear();
    m_sampledEffect = nullptr;
    restartAnimationOnCompositor();
    m_target->setNeedsAnimationStyleRecalc();
    invalidate();
}

void KeyframeEffect::updateChildrenAndEffects() const
{
    if (!m_model)
        return;
    if (isInEffect())
        const_cast<KeyframeEffect*>(this)->applyEffects();
    else if (m_sampledEffect)
        const_cast<KeyframeEffect*>(this)->clearEffects();
}

double KeyframeEffect::calculateTimeToEffectChange(bool forwards, double localTime, double timeToNextIteration) const
{
    const double start = startTimeInternal() + specifiedTiming().startDelay;
    const double end = start + activeDurationInternal();

    switch (phase()) {
    case PhaseNone:
        return std::numeric_limits<double>::infinity();
    case PhaseBefore:
        ASSERT(start >= localTime);
        return forwards
            ? start - localTime
            : std::numeric_limits<double>::infinity();
    case PhaseActive:
        if (forwards) {
            // Need service to apply fill / fire events.
            const double timeToEnd = end - localTime;
            if (requiresIterationEvents()) {
                return std::min(timeToEnd, timeToNextIteration);
            }
            return timeToEnd;
        }
        return 0;
    case PhaseAfter:
        ASSERT(localTime >= end);
        // If this KeyframeEffect is still in effect then it will need to update
        // when its parent goes out of effect. We have no way of knowing when
        // that will be, however, so the parent will need to supply it.
        return forwards
            ? std::numeric_limits<double>::infinity()
            : localTime - end;
    default:
        ASSERT_NOT_REACHED();
        return std::numeric_limits<double>::infinity();
    }
}

#if !ENABLE(OILPAN)
void KeyframeEffect::notifyElementDestroyed()
{
    // If our animation is kept alive just by the sampledEffect, we might get our
    // destructor called when we call SampledEffect::clear(), so we need to
    // clear m_sampledEffect first.
    m_target = nullptr;
    clearEventDelegate();
    SampledEffect* sampledEffect = m_sampledEffect;
    m_sampledEffect = nullptr;
    if (sampledEffect)
        sampledEffect->clear();
}
#endif

bool KeyframeEffect::isCandidateForAnimationOnCompositor(double animationPlaybackRate) const
{
    // Do not put transforms on compositor if more than one of them are defined
    // in computed style because they need to be explicitly ordered
    if (!model()
        || !m_target
        || (m_target->computedStyle() && m_target->computedStyle()->hasMotionPath())
        || hasMultipleTransformProperties())
        return false;

    return CompositorAnimations::instance()->isCandidateForAnimationOnCompositor(specifiedTiming(), *m_target, animation(), *model(), animationPlaybackRate);
}

bool KeyframeEffect::maybeStartAnimationOnCompositor(int group, double startTime, double currentTime, double animationPlaybackRate)
{
    ASSERT(!hasActiveAnimationsOnCompositor());
    if (!isCandidateForAnimationOnCompositor(animationPlaybackRate))
        return false;
    if (!CompositorAnimations::instance()->canStartAnimationOnCompositor(*m_target))
        return false;
    if (!CompositorAnimations::instance()->startAnimationOnCompositor(*m_target, group, startTime, currentTime, specifiedTiming(), *animation(), *model(), m_compositorAnimationIds, animationPlaybackRate))
        return false;
    ASSERT(!m_compositorAnimationIds.isEmpty());
    return true;
}

bool KeyframeEffect::hasActiveAnimationsOnCompositor() const
{
    return !m_compositorAnimationIds.isEmpty();
}

bool KeyframeEffect::hasActiveAnimationsOnCompositor(CSSPropertyID property) const
{
    return hasActiveAnimationsOnCompositor() && affects(PropertyHandle(property));
}

bool KeyframeEffect::affects(PropertyHandle property) const
{
    return m_model && m_model->affects(property);
}

bool KeyframeEffect::cancelAnimationOnCompositor()
{
    // FIXME: cancelAnimationOnCompositor is called from withins style recalc.
    // This queries compositingState, which is not necessarily up to date.
    // https://code.google.com/p/chromium/issues/detail?id=339847
    DisableCompositingQueryAsserts disabler;
    if (!hasActiveAnimationsOnCompositor())
        return false;
    if (!m_target || !m_target->layoutObject())
        return false;
    ASSERT(animation());
    for (const auto& compositorAnimationId : m_compositorAnimationIds)
        CompositorAnimations::instance()->cancelAnimationOnCompositor(*m_target, *animation(), compositorAnimationId);
    m_compositorAnimationIds.clear();
    return true;
}

void KeyframeEffect::restartAnimationOnCompositor()
{
    if (cancelAnimationOnCompositor())
        animation()->setCompositorPending(true);
}

void KeyframeEffect::cancelIncompatibleAnimationsOnCompositor()
{
    if (m_target && animation() && model())
        CompositorAnimations::instance()->cancelIncompatibleAnimationsOnCompositor(*m_target, *animation(), *model());
}

void KeyframeEffect::pauseAnimationForTestingOnCompositor(double pauseTime)
{
    ASSERT(hasActiveAnimationsOnCompositor());
    if (!m_target || !m_target->layoutObject())
        return;
    ASSERT(animation());
    for (const auto& compositorAnimationId : m_compositorAnimationIds)
        CompositorAnimations::instance()->pauseAnimationForTestingOnCompositor(*m_target, *animation(), compositorAnimationId, pauseTime);
}

bool KeyframeEffect::canAttachCompositedLayers() const
{
    if (!m_target || !animation())
        return false;

    return CompositorAnimations::instance()->canAttachCompositedLayers(*m_target, *animation());
}

void KeyframeEffect::attachCompositedLayers()
{
    ASSERT(m_target);
    ASSERT(animation());
    CompositorAnimations::instance()->attachCompositedLayers(*m_target, *animation());
}

DEFINE_TRACE(KeyframeEffect)
{
    visitor->trace(m_target);
    visitor->trace(m_model);
    visitor->trace(m_sampledEffect);
    AnimationEffect::trace(visitor);
}

} // namespace blink
