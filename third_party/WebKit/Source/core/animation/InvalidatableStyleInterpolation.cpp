// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/InvalidatableStyleInterpolation.h"

#include "core/animation/StringKeyframe.h"

namespace blink {

InvalidatableStyleInterpolation::InvalidatableStyleInterpolation(
    const Vector<const InterpolationType*>& interpolationTypes,
    const CSSPropertySpecificKeyframe& startKeyframe,
    const CSSPropertySpecificKeyframe& endKeyframe)
    : StyleInterpolation(nullptr, nullptr, interpolationTypes.first()->property())
    , m_interpolationTypes(interpolationTypes)
    , m_startKeyframe(startKeyframe)
    , m_endKeyframe(endKeyframe)
{
    maybeCachePairwiseConversion(nullptr);
    interpolate(0, 0);
}

bool InvalidatableStyleInterpolation::maybeCachePairwiseConversion(const StyleResolverState* state) const
{
    for (const auto& interpolationType : m_interpolationTypes) {
        OwnPtrWillBeRawPtr<PairwisePrimitiveInterpolation> pairwiseConversion = interpolationType->maybeConvertPairwise(m_startKeyframe, m_endKeyframe, state, m_conversionCheckers);
        if (pairwiseConversion) {
            m_cachedValue = pairwiseConversion->initialValue();
            m_cachedConversion = pairwiseConversion.release();
            return true;
        }
    }
    return false;
}

void InvalidatableStyleInterpolation::interpolate(int, double fraction)
{
    m_currentFraction = fraction;
    if (m_cachedConversion)
        m_cachedConversion->interpolate(fraction, m_cachedValue);
    // We defer the interpolation to ensureValidInterpolation() if m_cachedConversion is null.
}

PassOwnPtrWillBeRawPtr<InterpolationValue> InvalidatableStyleInterpolation::convertSingleKeyframe(const CSSPropertySpecificKeyframe& keyframe, const StyleResolverState& state) const
{
    for (const auto& interpolationType : m_interpolationTypes) {
        OwnPtrWillBeRawPtr<InterpolationValue> result = interpolationType->maybeConvertSingle(keyframe, &state, m_conversionCheckers);
        if (result)
            return result.release();
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

bool InvalidatableStyleInterpolation::isCacheValid(const StyleResolverState& state) const
{
    for (const auto& checker : m_conversionCheckers) {
        if (!checker->isValid(state))
            return false;
    }
    return true;
}

void InvalidatableStyleInterpolation::ensureValidInterpolation(const StyleResolverState& state) const
{
    if (m_cachedConversion && isCacheValid(state))
        return;
    m_conversionCheckers.clear();
    if (!maybeCachePairwiseConversion(&state)) {
        m_cachedConversion = FlipPrimitiveInterpolation::create(
            convertSingleKeyframe(m_startKeyframe, state),
            convertSingleKeyframe(m_endKeyframe, state));
    }
    m_cachedConversion->interpolate(m_currentFraction, m_cachedValue);
}

void InvalidatableStyleInterpolation::apply(StyleResolverState& state) const
{
    ensureValidInterpolation(state);
    m_cachedValue->type().apply(m_cachedValue->interpolableValue(), m_cachedValue->nonInterpolableValue(), state);
}

} // namespace blink
