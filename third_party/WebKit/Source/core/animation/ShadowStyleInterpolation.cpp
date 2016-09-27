// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/ShadowStyleInterpolation.h"

#include "core/animation/ColorStyleInterpolation.h"
#include "core/animation/LengthStyleInterpolation.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/CSSValueList.h"
#include "core/css/CSSValuePool.h"
#include "core/css/resolver/StyleBuilder.h"

namespace blink {

bool ShadowStyleInterpolation::canCreateFrom(const CSSValue& start, const CSSValue& end)
{
    return start.isShadowValue() && end.isShadowValue()
        && toCSSShadowValue(start).style == toCSSShadowValue(end).style
        && toCSSShadowValue(start).color && toCSSShadowValue(end).color;
}

PassOwnPtrWillBeRawPtr<InterpolableValue> ShadowStyleInterpolation::lengthToInterpolableValue(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> value)
{
    if (value)
        return LengthStyleInterpolation::toInterpolableValue(*value);
    return LengthStyleInterpolation::toInterpolableValue(*CSSPrimitiveValue::create(0, CSSPrimitiveValue::CSS_PX));
}

PassOwnPtrWillBeRawPtr<InterpolableValue> ShadowStyleInterpolation::toInterpolableValue(const CSSValue& value, NonInterpolableType& nonInterpolableData)
{
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(5);
    const CSSShadowValue* shadowValue = toCSSShadowValue(&value);
    ASSERT(shadowValue);

    result->set(0, lengthToInterpolableValue(shadowValue->x));
    result->set(1, lengthToInterpolableValue(shadowValue->y));
    result->set(2, lengthToInterpolableValue(shadowValue->blur));
    result->set(3, lengthToInterpolableValue(shadowValue->spread));

    if (shadowValue->color && ColorStyleInterpolation::canCreateFrom(*shadowValue->color))
        result->set(4, ColorStyleInterpolation::colorToInterpolableValue(*shadowValue->color));

    if (shadowValue->style)
        nonInterpolableData = (shadowValue->style->getValueID() == CSSValueInset);
    else
        nonInterpolableData = false;

    return result.release();
}

PassRefPtrWillBeRawPtr<CSSValue> ShadowStyleInterpolation::fromInterpolableValue(const InterpolableValue& value, NonInterpolableType nonInterpolableData, InterpolationRange range)
{
    const InterpolableList* shadow = toInterpolableList(&value);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> x = LengthStyleInterpolation::fromInterpolableValue(*shadow->get(0), RangeAll);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> y = LengthStyleInterpolation::fromInterpolableValue(*shadow->get(1), RangeAll);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> blur = LengthStyleInterpolation::fromInterpolableValue(*shadow->get(2), RangeNonNegative);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> spread = LengthStyleInterpolation::fromInterpolableValue(*shadow->get(3), RangeAll);

    RefPtrWillBeRawPtr<CSSPrimitiveValue> color = ColorStyleInterpolation::interpolableValueToColor(*shadow->get(4));
    RefPtrWillBeRawPtr<CSSPrimitiveValue> style = nonInterpolableData ? CSSPrimitiveValue::createIdentifier(CSSValueInset) : CSSPrimitiveValue::createIdentifier(CSSValueNone);

    RefPtrWillBeRawPtr<CSSShadowValue> result = CSSShadowValue::create(x, y, blur, spread, style, color);
    return result.release();
}

bool ShadowStyleInterpolation::usesDefaultStyleInterpolation(const CSSValue& start, const CSSValue& end)
{
    if (start.isValueList() && end.isValueList() && toCSSValueList(start).length() == toCSSValueList(end).length()) {
        const CSSValueList* startList = toCSSValueList(&start);
        const CSSValueList* endList = toCSSValueList(&end);
        for (size_t i = 0; i < toCSSValueList(start).length(); i++) {
            if (startList->item(i)->isShadowValue() && endList->item(i)->isShadowValue()
                && toCSSShadowValue(startList->item(i))->style != toCSSShadowValue(endList->item(i))->style)
                return true;
        }
    }
    return false;
}

} // namespace blink
