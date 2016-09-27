// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/CSSValueInterpolationType.h"

#include "core/css/resolver/StyleBuilder.h"

namespace blink {

PassOwnPtrWillBeRawPtr<InterpolationValue> CSSValueInterpolationType::maybeConvertSingle(const CSSPropertySpecificKeyframe& keyframe, const StyleResolverState*, ConversionCheckers&) const
{
    return InterpolationValue::create(*this, InterpolableList::create(0), DefaultNonInterpolableValue::create(keyframe.value()));
}

void CSSValueInterpolationType::apply(const InterpolableValue&, const NonInterpolableValue* nonInterpolableValue, StyleResolverState& state) const
{
    CSSValue* value = toDefaultNonInterpolableValue(nonInterpolableValue)->cssValue();
    if (value)
        StyleBuilder::applyProperty(m_property, state, value);
}

DEFINE_NON_INTERPOLABLE_VALUE_TYPE(DefaultNonInterpolableValue);

} // namespace blink
