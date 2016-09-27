// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/LengthSVGInterpolation.h"

#include "core/css/CSSHelper.h"
#include "core/svg/SVGAnimatedLength.h"
#include "core/svg/SVGAnimatedLengthList.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGLengthContext.h"
#include "wtf/StdLibExtras.h"

namespace blink {

PassRefPtrWillBeRawPtr<SVGLengthList> LengthSVGInterpolation::createList(const SVGAnimatedPropertyBase& attribute)
{
    ASSERT(attribute.type() == AnimatedLengthList);
    const SVGAnimatedLengthList& animatedLengthList = static_cast<const SVGAnimatedLengthList&>(attribute);
    return SVGLengthList::create(animatedLengthList.currentValue()->unitMode());
}

PassRefPtrWillBeRawPtr<LengthSVGInterpolation> LengthSVGInterpolation::create(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
{
    NonInterpolableType modeData;
    OwnPtrWillBeRawPtr<InterpolableValue> startValue = toInterpolableValue(toSVGLength(start).get(), attribute.get(), &modeData);
    OwnPtrWillBeRawPtr<InterpolableValue> endValue = toInterpolableValue(toSVGLength(end).get(), attribute.get(), nullptr);
    return adoptRefWillBeNoop(new LengthSVGInterpolation(startValue.release(), endValue.release(), attribute, modeData));
}

namespace {

void populateModeData(const SVGAnimatedPropertyBase* attribute, LengthSVGInterpolation::NonInterpolableType* ptrModeData)
{
    switch (attribute->type()) {
    case AnimatedLength: {
        const SVGAnimatedLength& animatedLength = static_cast<const SVGAnimatedLength&>(*attribute);
        ptrModeData->unitMode = animatedLength.currentValue()->unitMode();
        ptrModeData->negativeValuesMode = animatedLength.negativeValuesMode();
        break;
    }
    case AnimatedLengthList: {
        const SVGAnimatedLengthList& animatedLengthList = static_cast<const SVGAnimatedLengthList&>(*attribute);
        ptrModeData->unitMode = animatedLengthList.currentValue()->unitMode();
        ptrModeData->negativeValuesMode = AllowNegativeLengths;
        break;
    }
    default:
        ASSERT_NOT_REACHED();
    }
}

enum LengthInterpolatedUnit {
    LengthInterpolatedNumber,
    LengthInterpolatedPercentage,
    LengthInterpolatedEMS,
    LengthInterpolatedEXS,
    LengthInterpolatedREMS,
    LengthInterpolatedCHS,
};

static const SVGLengthType unitTypes[] = { LengthTypeNumber, LengthTypePercentage, LengthTypeEMS, LengthTypeEXS, LengthTypeREMS, LengthTypeCHS };

const size_t numLengthInterpolatedUnits = WTF_ARRAY_LENGTH(unitTypes);

LengthInterpolatedUnit convertToInterpolatedUnit(SVGLengthType lengthType, double& value)
{
    switch (lengthType) {
    case LengthTypeUnknown:
    default:
        ASSERT_NOT_REACHED();
    case LengthTypePX:
    case LengthTypeNumber:
        return LengthInterpolatedNumber;
    case LengthTypePercentage:
        return LengthInterpolatedPercentage;
    case LengthTypeEMS:
        return LengthInterpolatedEMS;
    case LengthTypeEXS:
        return LengthInterpolatedEXS;
    case LengthTypeCM:
        value *= cssPixelsPerCentimeter;
        return LengthInterpolatedNumber;
    case LengthTypeMM:
        value *= cssPixelsPerMillimeter;
        return LengthInterpolatedNumber;
    case LengthTypeIN:
        value *= cssPixelsPerInch;
        return LengthInterpolatedNumber;
    case LengthTypePT:
        value *= cssPixelsPerPoint;
        return LengthInterpolatedNumber;
    case LengthTypePC:
        value *= cssPixelsPerPica;
        return LengthInterpolatedNumber;
    case LengthTypeREMS:
        return LengthInterpolatedREMS;
    case LengthTypeCHS:
        return LengthInterpolatedCHS;
    }
}

} // namespace

PassOwnPtrWillBeRawPtr<InterpolableValue> LengthSVGInterpolation::toInterpolableValue(SVGLength* length, const SVGAnimatedPropertyBase* attribute, NonInterpolableType* ptrModeData)
{
    if (ptrModeData)
        populateModeData(attribute, ptrModeData);

    double value = length->valueInSpecifiedUnits();
    LengthInterpolatedUnit unitType = convertToInterpolatedUnit(length->unitType(), value);

    double values[numLengthInterpolatedUnits] = { };
    values[unitType] = value;

    OwnPtrWillBeRawPtr<InterpolableList> listOfValues = InterpolableList::create(numLengthInterpolatedUnits);
    for (size_t i = 0; i < numLengthInterpolatedUnits; ++i)
        listOfValues->set(i, InterpolableNumber::create(values[i]));
    return listOfValues.release();
}

PassRefPtrWillBeRawPtr<SVGLength> LengthSVGInterpolation::fromInterpolableValue(const InterpolableValue& interpolableValue, const NonInterpolableType& modeData, const SVGElement* element)
{
    const InterpolableList& listOfValues = toInterpolableList(interpolableValue);
    ASSERT(element);

    double value = 0;
    SVGLengthType lengthType = LengthTypeNumber;
    unsigned unitTypeCount = 0;
    // We optimise for the common case where only one unit type is involved.
    for (size_t i = 0; i < numLengthInterpolatedUnits; i++) {
        double entry = toInterpolableNumber(listOfValues.get(i))->value();
        if (!entry)
            continue;
        unitTypeCount++;
        if (unitTypeCount > 1)
            break;

        value = entry;
        lengthType = unitTypes[i];
    }

    if (unitTypeCount > 1) {
        value = 0;
        lengthType = LengthTypeNumber;

        // SVGLength does not support calc expressions, so we convert to canonical units.
        SVGLengthContext lengthContext(element);
        for (size_t i = 0; i < numLengthInterpolatedUnits; i++) {
            double entry = toInterpolableNumber(listOfValues.get(i))->value();
            if (entry)
                value += lengthContext.convertValueToUserUnits(entry, modeData.unitMode, unitTypes[i]);
        }
    }

    if (modeData.negativeValuesMode == ForbidNegativeLengths && value < 0)
        value = 0;

    RefPtrWillBeRawPtr<SVGLength> result = SVGLength::create(modeData.unitMode); // defaults to the length 0
    result->setUnitType(lengthType);
    result->setValueInSpecifiedUnits(value);
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> LengthSVGInterpolation::interpolatedValue(SVGElement& targetElement) const
{
    return fromInterpolableValue(*m_cachedValue, m_modeData, &targetElement);
}

}
