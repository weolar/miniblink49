// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/LengthPairStyleInterpolation.h"

#include "core/animation/LengthStyleInterpolation.h"
#include "core/css/Pair.h"
#include "core/css/resolver/StyleBuilder.h"

namespace blink {

bool LengthPairStyleInterpolation::canCreateFrom(const CSSValue& value)
{
    return value.isPrimitiveValue() && toCSSPrimitiveValue(value).getPairValue();
}

PassOwnPtrWillBeRawPtr<InterpolableValue> LengthPairStyleInterpolation::lengthPairToInterpolableValue(const CSSValue& value)
{
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(2);
    Pair* pair = toCSSPrimitiveValue(value).getPairValue();
    ASSERT(pair);

    result->set(0, LengthStyleInterpolation::toInterpolableValue(*pair->first()));
    result->set(1, LengthStyleInterpolation::toInterpolableValue(*pair->second()));
    return result.release();
}

PassRefPtrWillBeRawPtr<CSSValue> LengthPairStyleInterpolation::interpolableValueToLengthPair(InterpolableValue* value, InterpolationRange range)
{
    InterpolableList* lengthPair = toInterpolableList(value);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> first = LengthStyleInterpolation::fromInterpolableValue(*lengthPair->get(0), range);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> second = LengthStyleInterpolation::fromInterpolableValue(*lengthPair->get(1), range);
    RefPtrWillBeRawPtr<Pair> result = Pair::create(first, second, Pair::KeepIdenticalValues);

    return CSSPrimitiveValue::create(result.release());
}

void LengthPairStyleInterpolation::apply(StyleResolverState& state) const
{
    StyleBuilder::applyProperty(m_id, state, interpolableValueToLengthPair(m_cachedValue.get(), m_range).get());
}

} // namespace blink
