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
#include "core/animation/KeyframeEffectModel.h"

#include "core/StylePropertyShorthand.h"
#include "core/animation/AnimationEffect.h"
#include "core/animation/CompositorAnimations.h"
#include "core/animation/css/CSSAnimatableValueFactory.h"
#include "core/animation/css/CSSPropertyEquality.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/Document.h"
#include "platform/animation/AnimationUtilities.h"
#include "platform/geometry/FloatBox.h"
#include "platform/transforms/TransformationMatrix.h"
#include "wtf/text/StringHash.h"

namespace blink {

PropertyHandleSet KeyframeEffectModelBase::properties() const
{
    PropertyHandleSet result;
    for (const auto& keyframe : m_keyframes) {
        for (const auto& property : keyframe->properties())
            result.add(property);
    }
    return result;
}

void KeyframeEffectModelBase::setFrames(KeyframeVector& keyframes)
{
    // TODO(samli): Should also notify/invalidate the animation
    m_keyframes = keyframes;
    m_keyframeGroups = nullptr;
    m_interpolationEffect = nullptr;
}

void KeyframeEffectModelBase::sample(int iteration, double fraction, double iterationDuration, OwnPtrWillBeRawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>>& result) const
{
    ASSERT(iteration >= 0);
    ASSERT(!isNull(fraction));
    ensureKeyframeGroups();
    ensureInterpolationEffect();

    return m_interpolationEffect->getActiveInterpolations(fraction, iterationDuration, result);
}

void KeyframeEffectModelBase::forceConversionsToAnimatableValues(Element& element, const ComputedStyle* baseStyle)
{
    ensureKeyframeGroups();
    snapshotCompositableProperties(element, baseStyle);
    ensureInterpolationEffect(&element, baseStyle);
}

void KeyframeEffectModelBase::snapshotCompositableProperties(Element& element, const ComputedStyle* baseStyle)
{
    ensureKeyframeGroups();
    for (CSSPropertyID id : CompositorAnimations::compositableProperties) {
        PropertyHandle property = PropertyHandle(id);
        if (!affects(property))
            continue;
        for (auto& keyframe : m_keyframeGroups->get(property)->m_keyframes)
            keyframe->populateAnimatableValue(id, element, baseStyle);
    }
}

bool KeyframeEffectModelBase::updateNeutralKeyframeAnimatableValues(CSSPropertyID property, PassRefPtrWillBeRawPtr<AnimatableValue> value)
{
    ASSERT(CompositorAnimations::isCompositableProperty(property));

    if (!value)
        return false;

    ensureKeyframeGroups();
    auto& keyframes = m_keyframeGroups->get(PropertyHandle(property))->m_keyframes;
    ASSERT(keyframes.size() >= 2);

    auto& first = toCSSPropertySpecificKeyframe(*keyframes.first());
    auto& last = toCSSPropertySpecificKeyframe(*keyframes.last());

    if (!first.value())
        first.setAnimatableValue(value);
    if (!last.value())
        last.setAnimatableValue(value);
    return !first.value() || !last.value();
}

KeyframeEffectModelBase::KeyframeVector KeyframeEffectModelBase::normalizedKeyframes(const KeyframeVector& keyframes)
{
    double lastOffset = 0;
    KeyframeVector result;
    result.reserveCapacity(keyframes.size());

    for (const auto& keyframe : keyframes) {
        double offset = keyframe->offset();
        if (!isNull(offset)) {
            ASSERT(offset >= 0);
            ASSERT(offset <= 1);
            ASSERT(offset >= lastOffset);
            lastOffset = offset;
        }
        result.append(keyframe->clone());
    }

    if (result.isEmpty())
        return result;

    if (isNull(result.last()->offset()))
        result.last()->setOffset(1);

    if (result.size() > 1 && isNull(result[0]->offset()))
        result.first()->setOffset(0);

    size_t lastIndex = 0;
    lastOffset = result.first()->offset();
    for (size_t i = 1; i < result.size(); ++i) {
        double offset = result[i]->offset();
        if (!isNull(offset)) {
            for (size_t j = 1; j < i - lastIndex; ++j)
                result[lastIndex + j]->setOffset(lastOffset + (offset - lastOffset) * j / (i - lastIndex));
            lastIndex = i;
            lastOffset = offset;
        }
    }

    return result;
}

bool KeyframeEffectModelBase::isTransformRelatedEffect() const
{
    return affects(PropertyHandle(CSSPropertyTransform))
        || affects(PropertyHandle(CSSPropertyRotate))
        || affects(PropertyHandle(CSSPropertyScale))
        || affects(PropertyHandle(CSSPropertyTranslate));
}

void KeyframeEffectModelBase::ensureKeyframeGroups() const
{
    if (m_keyframeGroups)
        return;

    m_keyframeGroups = adoptPtrWillBeNoop(new KeyframeGroupMap);
    for (const auto& keyframe : normalizedKeyframes(getFrames())) {
        for (const PropertyHandle& property : keyframe->properties()) {
            if (property.isCSSProperty())
                ASSERT_WITH_MESSAGE(!isShorthandProperty(property.cssProperty()), "Web Animations: Encountered shorthand CSS property (%d) in normalized keyframes.", property.cssProperty());
            KeyframeGroupMap::iterator groupIter = m_keyframeGroups->find(property);
            PropertySpecificKeyframeGroup* group;
            if (groupIter == m_keyframeGroups->end())
                group = m_keyframeGroups->add(property, adoptPtrWillBeNoop(new PropertySpecificKeyframeGroup)).storedValue->value.get();
            else
                group = groupIter->value.get();

            group->appendKeyframe(keyframe->createPropertySpecificKeyframe(property));
        }
    }

    // Add synthetic keyframes.
    m_hasSyntheticKeyframes = false;
    for (const auto& entry : *m_keyframeGroups) {
        if (entry.value->addSyntheticKeyframeIfRequired(m_neutralKeyframeEasing))
            m_hasSyntheticKeyframes = true;

        entry.value->removeRedundantKeyframes();
    }
}

void KeyframeEffectModelBase::ensureInterpolationEffect(Element* element, const ComputedStyle* baseStyle) const
{
    if (m_interpolationEffect)
        return;
    m_interpolationEffect = InterpolationEffect::create();

    for (const auto& entry : *m_keyframeGroups) {
        const PropertySpecificKeyframeVector& keyframes = entry.value->keyframes();
        for (size_t i = 0; i < keyframes.size() - 1; i++) {
            double applyFrom = i ? keyframes[i]->offset() : (-std::numeric_limits<double>::infinity());
            double applyTo = i == keyframes.size() - 2 ? std::numeric_limits<double>::infinity() : keyframes[i + 1]->offset();
            if (applyTo == 1)
                applyTo = std::numeric_limits<double>::infinity();

            m_interpolationEffect->addInterpolationsFromKeyframes(entry.key, element, baseStyle, *keyframes[i], *keyframes[i + 1], applyFrom, applyTo);
        }
    }
}

bool KeyframeEffectModelBase::isReplaceOnly()
{
    ensureKeyframeGroups();
    for (const auto& entry : *m_keyframeGroups) {
        for (const auto& keyframe : entry.value->keyframes()) {
            if (keyframe->composite() != EffectModel::CompositeReplace)
                return false;
        }
    }
    return true;
}

DEFINE_TRACE(KeyframeEffectModelBase)
{
    visitor->trace(m_keyframes);
#if ENABLE(OILPAN)
    visitor->trace(m_keyframeGroups);
#endif
    visitor->trace(m_interpolationEffect);
    EffectModel::trace(visitor);
}

Keyframe::PropertySpecificKeyframe::PropertySpecificKeyframe(double offset, PassRefPtr<TimingFunction> easing, EffectModel::CompositeOperation composite)
    : m_offset(offset)
    , m_easing(easing)
    , m_composite(composite)
{
}

void KeyframeEffectModelBase::PropertySpecificKeyframeGroup::appendKeyframe(PassOwnPtrWillBeRawPtr<Keyframe::PropertySpecificKeyframe> keyframe)
{
    ASSERT(m_keyframes.isEmpty() || m_keyframes.last()->offset() <= keyframe->offset());
    m_keyframes.append(keyframe);
}

void KeyframeEffectModelBase::PropertySpecificKeyframeGroup::removeRedundantKeyframes()
{
    // As an optimization, removes keyframes in the following categories, as
    // they will never be used by sample().
    // - End keyframes with the same offset as their neighbor
    // - Interior keyframes with the same offset as both their neighbors
    // Note that synthetic keyframes must be added before this method is
    // called.
    ASSERT(m_keyframes.size() >= 2);
    for (int i = m_keyframes.size() - 1; i >= 0; --i) {
        double offset = m_keyframes[i]->offset();
        bool hasSameOffsetAsPreviousNeighbor = !i || m_keyframes[i - 1]->offset() == offset;
        bool hasSameOffsetAsNextNeighbor = i == static_cast<int>(m_keyframes.size() - 1) || m_keyframes[i + 1]->offset() == offset;
        if (hasSameOffsetAsPreviousNeighbor && hasSameOffsetAsNextNeighbor)
            m_keyframes.remove(i);
    }
    ASSERT(m_keyframes.size() >= 2);
}

bool KeyframeEffectModelBase::PropertySpecificKeyframeGroup::addSyntheticKeyframeIfRequired(PassRefPtr<TimingFunction> easing)
{
    ASSERT(!m_keyframes.isEmpty());

    bool addedSyntheticKeyframe = false;

    if (m_keyframes.first()->offset() != 0.0) {
        m_keyframes.insert(0, m_keyframes.first()->neutralKeyframe(0, easing));
        addedSyntheticKeyframe = true;
    }
    if (m_keyframes.last()->offset() != 1.0) {
        appendKeyframe(m_keyframes.last()->neutralKeyframe(1, easing));
        addedSyntheticKeyframe = true;
    }

    return addedSyntheticKeyframe;
}

DEFINE_TRACE(KeyframeEffectModelBase::PropertySpecificKeyframeGroup)
{
#if ENABLE(OILPAN)
    visitor->trace(m_keyframes);
#endif
}

} // namespace
