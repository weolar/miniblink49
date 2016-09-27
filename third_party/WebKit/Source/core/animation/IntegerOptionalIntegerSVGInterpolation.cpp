// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/IntegerOptionalIntegerSVGInterpolation.h"

#include "core/SVGNames.h"

namespace blink {

namespace {

// For order, the values must be integers greater than zero.
// For filterRes, negative values are an error. Zero values disable rendering of the element which referenced the filter.
PassRefPtrWillBeRawPtr<SVGInteger> toPositiveInteger(const InterpolableValue* number, int min)
{
    return SVGInteger::create(clampTo<int>(roundf(toInterpolableNumber(number)->value()), min));
}

} // namespace

PassOwnPtrWillBeRawPtr<InterpolableValue> IntegerOptionalIntegerSVGInterpolation::toInterpolableValue(SVGPropertyBase* value)
{
    RefPtrWillBeRawPtr<SVGIntegerOptionalInteger> integerOptionalInteger = toSVGIntegerOptionalInteger(value);
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(2);
    result->set(0, InterpolableNumber::create(integerOptionalInteger->firstInteger()->value()));
    result->set(1, InterpolableNumber::create(integerOptionalInteger->secondInteger()->value()));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGIntegerOptionalInteger> IntegerOptionalIntegerSVGInterpolation::fromInterpolableValue(const InterpolableValue& value, int min)
{
    const InterpolableList& list = toInterpolableList(value);
    return SVGIntegerOptionalInteger::create(
        toPositiveInteger(list.get(0), min),
        toPositiveInteger(list.get(1), min));
}

}
