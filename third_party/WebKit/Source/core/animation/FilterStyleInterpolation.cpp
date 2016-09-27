// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/FilterStyleInterpolation.h"

#include "core/animation/LengthStyleInterpolation.h"
#include "core/animation/ListStyleInterpolation.h"
#include "core/css/CSSPrimitiveValue.h"

namespace blink {

namespace {

PassRefPtrWillBeRawPtr<CSSValueList> extendFilterList(const CSSValueList& shortFilterList, const CSSValueList& otherFilterList)
{
    RefPtrWillBeRawPtr<CSSValueList> result = CSSValueList::createSpaceSeparated();
    CSSValueList::const_iterator shortIter = shortFilterList.begin();
    CSSValueList::const_iterator otherIter = otherFilterList.begin();
    while (shortIter != shortFilterList.end()) {
        result->append(*shortIter);
        ++shortIter;
        ++otherIter;
    }
    while (otherIter != otherFilterList.end()) {
        CSSFunctionValue* function = toCSSFunctionValue(otherIter->get());
        RefPtrWillBeRawPtr<CSSValueList> defaultFunction = CSSFunctionValue::create(function->functionType());
        switch (function->functionType()) {
        case CSSValueUrl:
            // Discrete interpolation occurs - see canCreateFrom.
            break;
        case CSSValueGrayscale:
        case CSSValueInvert:
        case CSSValueSepia:
            defaultFunction->append(CSSPrimitiveValue::create(0, CSSPrimitiveValue::CSS_NUMBER));
            break;
        case CSSValueBrightness:
        case CSSValueContrast:
        case CSSValueOpacity:
        case CSSValueSaturate:
            defaultFunction->append(CSSPrimitiveValue::create(1, CSSPrimitiveValue::CSS_NUMBER));
            break;
        case CSSValueHueRotate:
            defaultFunction->append(CSSPrimitiveValue::create(0, CSSPrimitiveValue::CSS_DEG));
            break;
        case CSSValueBlur:
            defaultFunction->append(CSSPrimitiveValue::create(0, CSSPrimitiveValue::CSS_PX));
            break;
        case CSSValueDropShadow:
            // TODO(ericwilligers): Implement drop shadow interpolation.
            return CSSValueList::createSpaceSeparated();
        default:
            ASSERT_NOT_REACHED();
            return CSSValueList::createSpaceSeparated();
        }
        result->append(defaultFunction);
        ++otherIter;
    }
    return result.release();
}

PassRefPtrWillBeRawPtr<FilterStyleInterpolation::FilterListStyleInterpolation> maybeCreateFromList(const CSSValueList& startList, const CSSValueList& endList, CSSPropertyID property)
{
    if (startList.length() < endList.length())
        return ListStyleInterpolation<FilterStyleInterpolation>::maybeCreateFromList(*extendFilterList(startList, endList), endList, property);
    return ListStyleInterpolation<FilterStyleInterpolation>::maybeCreateFromList(startList, *extendFilterList(endList, startList), property);
}

} // namespace

PassRefPtrWillBeRawPtr<FilterStyleInterpolation::FilterListStyleInterpolation> FilterStyleInterpolation::maybeCreateList(const CSSValue& start, const CSSValue& end, CSSPropertyID property)
{
    if (start.isCSSWideKeyword() || end.isCSSWideKeyword())
        return nullptr;

    ASSERT(start.isValueList() || toCSSPrimitiveValue(start).getValueID() == CSSValueNone);
    ASSERT(end.isValueList() || toCSSPrimitiveValue(end).getValueID() == CSSValueNone);

    if (!start.isValueList() && !end.isValueList())
        return nullptr;

    return maybeCreateFromList(
        start.isValueList() ? toCSSValueList(start) : *CSSValueList::createSpaceSeparated(),
        end.isValueList() ? toCSSValueList(end) : *CSSValueList::createSpaceSeparated(),
        property);
}

bool FilterStyleInterpolation::canCreateFrom(const CSSValue& start, const CSSValue& end)
{
    // Discrete interpolation occurs when url or distinct function types occur.
    // http://dev.w3.org/fxtf/filters/#animation-of-filters
    // FIXME: Drop shadow interpolation has not been implemented.
    CSSValueID startFunctionType = toCSSFunctionValue(start).functionType();
    return startFunctionType == toCSSFunctionValue(end).functionType()
        && startFunctionType != CSSValueUrl
        && startFunctionType != CSSValueDropShadow;
}

PassOwnPtrWillBeRawPtr<InterpolableValue> FilterStyleInterpolation::toInterpolableValue(const CSSValue& value, CSSValueID& functionType)
{
    const CSSFunctionValue& filterValue = toCSSFunctionValue(value);
    functionType = filterValue.functionType();
    size_t length = filterValue.length();

    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(length);
    for (size_t i = 0; i < length; ++i) {
        switch (functionType) {
        case CSSValueHueRotate:
            result->set(i, InterpolableNumber::create(toCSSPrimitiveValue(filterValue.item(i))->computeDegrees()));
            break;
        case CSSValueBlur:
            result->set(i, LengthStyleInterpolation::toInterpolableValue(*filterValue.item(i), CSSPropertyWebkitFilter));
            break;
        case CSSValueGrayscale:
        case CSSValueInvert:
        case CSSValueSepia:
        case CSSValueBrightness:
        case CSSValueContrast:
        case CSSValueOpacity:
        case CSSValueSaturate:
            result->set(i, InterpolableNumber::create(toCSSPrimitiveValue(filterValue.item(i))->getDoubleValue()));
            break;
        case CSSValueDropShadow:
        case CSSValueUrl:
        default:
            ASSERT_NOT_REACHED();
        }
    }
    return result.release();
}

PassRefPtrWillBeRawPtr<CSSFunctionValue> FilterStyleInterpolation::fromInterpolableValue(const InterpolableValue& value, CSSValueID functionType, InterpolationRange)
{
    const InterpolableList& list = toInterpolableList(value);
    size_t length = list.length();
    RefPtrWillBeRawPtr<CSSFunctionValue> result = CSSFunctionValue::create(functionType);
    for (size_t i = 0; i < length; ++i) {
        switch (functionType) {
        case CSSValueGrayscale:
        case CSSValueInvert:
        case CSSValueOpacity:
        case CSSValueSepia:
            result->append(CSSPrimitiveValue::create(clampTo<double>(toInterpolableNumber(list.get(i))->value(), 0, 1), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case CSSValueBrightness:
        case CSSValueContrast:
        case CSSValueSaturate:
            result->append(CSSPrimitiveValue::create(clampTo<double>(toInterpolableNumber(list.get(i))->value(), 0), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case CSSValueHueRotate:
            result->append(CSSPrimitiveValue::create(toInterpolableNumber(list.get(i))->value(), CSSPrimitiveValue::CSS_DEG));
            break;
        case CSSValueBlur:
            result->append(LengthStyleInterpolation::fromInterpolableValue(*list.get(i), RangeNonNegative));
            break;
        case CSSValueDropShadow:
        case CSSValueUrl:
        default:
            ASSERT_NOT_REACHED();
            break;
        }
    }
    return result.release();
}

} // namespace blink
