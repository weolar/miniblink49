/*
 * Copyright (C) 2004, 2005, 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"

#include "core/svg/SVGLength.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/SVGNames.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"
#include "platform/animation/AnimationUtilities.h"
#include "wtf/MathExtras.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace {

inline const char* lengthTypeToString(SVGLengthType type)
{
    switch (type) {
    case LengthTypeUnknown:
    case LengthTypeNumber:
        return "";
    case LengthTypePercentage:
        return "%";
    case LengthTypeEMS:
        return "em";
    case LengthTypeEXS:
        return "ex";
    case LengthTypePX:
        return "px";
    case LengthTypeCM:
        return "cm";
    case LengthTypeMM:
        return "mm";
    case LengthTypeIN:
        return "in";
    case LengthTypePT:
        return "pt";
    case LengthTypePC:
        return "pc";
    case LengthTypeREMS:
        return "rem";
    case LengthTypeCHS:
        return "ch";
    }

    ASSERT_NOT_REACHED();
    return "";
}

template<typename CharType>
SVGLengthType stringToLengthType(const CharType*& ptr, const CharType* end)
{
    if (ptr == end)
        return LengthTypeNumber;

    SVGLengthType type = LengthTypeUnknown;
    const CharType firstChar = *ptr++;

    if (firstChar == '%') {
        type = LengthTypePercentage;
    } else if (isHTMLSpace<CharType>(firstChar)) {
        type = LengthTypeNumber;
    } else if (ptr < end) {
        const CharType secondChar = *ptr++;

        if (firstChar == 'p') {
            if (secondChar == 'x')
                type = LengthTypePX;
            if (secondChar == 't')
                type = LengthTypePT;
            if (secondChar == 'c')
                type = LengthTypePC;
        } else if (firstChar == 'e') {
            if (secondChar == 'm')
                type = LengthTypeEMS;
            if (secondChar == 'x')
                type = LengthTypeEXS;
        } else if (firstChar == 'r') {
            if (secondChar == 'e' && ptr < end) {
                const CharType thirdChar = *ptr++;
                if (thirdChar == 'm')
                    type = LengthTypeREMS;
            }
        } else if (firstChar == 'c') {
            if (secondChar == 'h')
                type = LengthTypeCHS;
            if (secondChar == 'm')
                type = LengthTypeCM;
        } else if (firstChar == 'm' && secondChar == 'm') {
            type = LengthTypeMM;
        } else if (firstChar == 'i' && secondChar == 'n') {
            type = LengthTypeIN;
        } else if (isHTMLSpace<CharType>(firstChar) && isHTMLSpace<CharType>(secondChar)) {
            type = LengthTypeNumber;
        }
    }

    if (!skipOptionalSVGSpaces(ptr, end))
        return type;

    return LengthTypeUnknown;
}

} // namespace

SVGLength::SVGLength(SVGLengthMode mode)
    : SVGPropertyBase(classType())
    , m_valueInSpecifiedUnits(0)
    , m_unitMode(static_cast<unsigned>(mode))
    , m_unitType(LengthTypeNumber)
{
    ASSERT(unitMode() == mode);
}

SVGLength::SVGLength(const SVGLength& o)
    : SVGPropertyBase(classType())
    , m_valueInSpecifiedUnits(o.m_valueInSpecifiedUnits)
    , m_unitMode(o.m_unitMode)
    , m_unitType(o.m_unitType)
{
}

PassRefPtrWillBeRawPtr<SVGLength> SVGLength::clone() const
{
    return adoptRefWillBeNoop(new SVGLength(*this));
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> SVGLength::cloneForAnimation(const String& value) const
{
    RefPtrWillBeRawPtr<SVGLength> length = create();

    length->m_unitMode = m_unitMode;
    length->m_unitType = m_unitType;

    TrackExceptionState exceptionState;
    length->setValueAsString(value, exceptionState);
    if (exceptionState.hadException()) {
        length->m_unitType = LengthTypeNumber;
        length->m_valueInSpecifiedUnits = 0;
    }

    return length.release();
}

bool SVGLength::operator==(const SVGLength& other) const
{
    return m_unitMode == other.m_unitMode
        && m_unitType == other.m_unitType
        && m_valueInSpecifiedUnits == other.m_valueInSpecifiedUnits;
}

float SVGLength::value(const SVGLengthContext& context) const
{
    return context.convertValueToUserUnits(m_valueInSpecifiedUnits, unitMode(), unitType());
}

void SVGLength::setValue(float value, const SVGLengthContext& context)
{
    m_valueInSpecifiedUnits = context.convertValueFromUserUnits(value, unitMode(), unitType());
}

void SVGLength::setUnitType(SVGLengthType type)
{
    ASSERT(type != LengthTypeUnknown && type <= LengthTypeCHS);
    m_unitType = type;
}

float SVGLength::valueAsPercentage() const
{
    // LengthTypePercentage is represented with 100% = 100.0. Good for accuracy but could eventually be changed.
    if (m_unitType == LengthTypePercentage) {
        // Note: This division is a source of floating point inaccuracy.
        return m_valueInSpecifiedUnits / 100;
    }

    return m_valueInSpecifiedUnits;
}

float SVGLength::valueAsPercentage100() const
{
    // LengthTypePercentage is represented with 100% = 100.0. Good for accuracy but could eventually be changed.
    if (m_unitType == LengthTypePercentage)
        return m_valueInSpecifiedUnits;

    return m_valueInSpecifiedUnits * 100;
}

float SVGLength::scaleByPercentage(float input) const
{
    float result = input * m_valueInSpecifiedUnits;
    if (m_unitType == LengthTypePercentage) {
        // Delaying division by 100 as long as possible since it introduces floating point errors.
        result = result / 100;
    }
    return result;
}

template<typename CharType>
static bool parseValueInternal(const String& string, float& convertedNumber, SVGLengthType& type)
{
    const CharType* ptr = string.getCharacters<CharType>();
    const CharType* end = ptr + string.length();

    if (!parseNumber(ptr, end, convertedNumber, AllowLeadingWhitespace))
        return false;

    type = stringToLengthType(ptr, end);
    ASSERT(ptr <= end);
    if (type == LengthTypeUnknown)
        return false;

    return true;
}

void SVGLength::setValueAsString(const String& string, ExceptionState& exceptionState)
{
    if (string.isEmpty()) {
        m_unitType = LengthTypeNumber;
        m_valueInSpecifiedUnits = 0;
        return;
    }

    float convertedNumber = 0;
    SVGLengthType type = LengthTypeUnknown;

    bool success = string.is8Bit() ?
        parseValueInternal<LChar>(string, convertedNumber, type) :
        parseValueInternal<UChar>(string, convertedNumber, type);

    if (!success) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + string + "') is invalid.");
        return;
    }

    m_unitType = type;
    m_valueInSpecifiedUnits = convertedNumber;
}

String SVGLength::valueAsString() const
{
    return String::number(m_valueInSpecifiedUnits) + lengthTypeToString(unitType());
}

void SVGLength::newValueSpecifiedUnits(SVGLengthType type, float value)
{
    setUnitType(type);
    m_valueInSpecifiedUnits = value;
}

void SVGLength::convertToSpecifiedUnits(SVGLengthType type, const SVGLengthContext& context)
{
    ASSERT(type != LengthTypeUnknown && type <= LengthTypeCHS);

    float valueInUserUnits = value(context);
    m_unitType = type;
    setValue(valueInUserUnits, context);
}

PassRefPtrWillBeRawPtr<SVGLength> SVGLength::fromCSSPrimitiveValue(CSSPrimitiveValue* value)
{
    ASSERT(value);

    SVGLengthType svgType;
    switch (value->primitiveType()) {
    case CSSPrimitiveValue::CSS_NUMBER:
        svgType = LengthTypeNumber;
        break;
    case CSSPrimitiveValue::CSS_PERCENTAGE:
        svgType = LengthTypePercentage;
        break;
    case CSSPrimitiveValue::CSS_EMS:
        svgType = LengthTypeEMS;
        break;
    case CSSPrimitiveValue::CSS_EXS:
        svgType = LengthTypeEXS;
        break;
    case CSSPrimitiveValue::CSS_PX:
        svgType = LengthTypePX;
        break;
    case CSSPrimitiveValue::CSS_CM:
        svgType = LengthTypeCM;
        break;
    case CSSPrimitiveValue::CSS_MM:
        svgType = LengthTypeMM;
        break;
    case CSSPrimitiveValue::CSS_IN:
        svgType = LengthTypeIN;
        break;
    case CSSPrimitiveValue::CSS_PT:
        svgType = LengthTypePT;
        break;
    case CSSPrimitiveValue::CSS_REMS:
        svgType = LengthTypeREMS;
        break;
    case CSSPrimitiveValue::CSS_CHS:
        svgType = LengthTypeCHS;
        break;
    default:
        ASSERT(value->primitiveType() == CSSPrimitiveValue::CSS_PC);
        svgType = LengthTypePC;
        break;
    };

    RefPtrWillBeRawPtr<SVGLength> length = SVGLength::create();
    length->newValueSpecifiedUnits(svgType, value->getFloatValue());
    return length.release();
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> SVGLength::toCSSPrimitiveValue(PassRefPtrWillBeRawPtr<SVGLength> passLength)
{
    RefPtrWillBeRawPtr<SVGLength> length = passLength;

    CSSPrimitiveValue::UnitType cssType = CSSPrimitiveValue::CSS_UNKNOWN;
    switch (length->unitType()) {
    case LengthTypeUnknown:
        break;
    case LengthTypeNumber:
        cssType = CSSPrimitiveValue::CSS_NUMBER;
        break;
    case LengthTypePercentage:
        cssType = CSSPrimitiveValue::CSS_PERCENTAGE;
        break;
    case LengthTypeEMS:
        cssType = CSSPrimitiveValue::CSS_EMS;
        break;
    case LengthTypeEXS:
        cssType = CSSPrimitiveValue::CSS_EXS;
        break;
    case LengthTypePX:
        cssType = CSSPrimitiveValue::CSS_PX;
        break;
    case LengthTypeCM:
        cssType = CSSPrimitiveValue::CSS_CM;
        break;
    case LengthTypeMM:
        cssType = CSSPrimitiveValue::CSS_MM;
        break;
    case LengthTypeIN:
        cssType = CSSPrimitiveValue::CSS_IN;
        break;
    case LengthTypePT:
        cssType = CSSPrimitiveValue::CSS_PT;
        break;
    case LengthTypePC:
        cssType = CSSPrimitiveValue::CSS_PC;
        break;
    case LengthTypeREMS:
        cssType = CSSPrimitiveValue::CSS_REMS;
        break;
    case LengthTypeCHS:
        cssType = CSSPrimitiveValue::CSS_CHS;
        break;
    };

    return CSSPrimitiveValue::create(length->valueInSpecifiedUnits(), cssType);
}

SVGLengthMode SVGLength::lengthModeForAnimatedLengthAttribute(const QualifiedName& attrName)
{
    typedef HashMap<QualifiedName, SVGLengthMode> LengthModeForLengthAttributeMap;
    DEFINE_STATIC_LOCAL(LengthModeForLengthAttributeMap, s_lengthModeMap, ());

    if (s_lengthModeMap.isEmpty()) {
        s_lengthModeMap.set(SVGNames::xAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::yAttr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::cxAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::cyAttr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::dxAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::dyAttr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::fxAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::fyAttr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::rAttr, SVGLengthMode::Other);
        s_lengthModeMap.set(SVGNames::rxAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::ryAttr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::widthAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::heightAttr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::x1Attr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::x2Attr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::y1Attr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::y2Attr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::refXAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::refYAttr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::markerWidthAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::markerHeightAttr, SVGLengthMode::Height);
        s_lengthModeMap.set(SVGNames::textLengthAttr, SVGLengthMode::Width);
        s_lengthModeMap.set(SVGNames::startOffsetAttr, SVGLengthMode::Width);
    }

    if (s_lengthModeMap.contains(attrName))
        return s_lengthModeMap.get(attrName);

    return SVGLengthMode::Other;
}

PassRefPtrWillBeRawPtr<SVGLength> SVGLength::blend(PassRefPtrWillBeRawPtr<SVGLength> passFrom, float progress) const
{
    RefPtrWillBeRawPtr<SVGLength> from = passFrom;

    SVGLengthType toType = unitType();
    SVGLengthType fromType = from->unitType();
    if ((from->isZero() && isZero())
        || fromType == LengthTypeUnknown
        || toType == LengthTypeUnknown
        || (!from->isZero() && fromType != LengthTypePercentage && toType == LengthTypePercentage)
        || (!isZero() && fromType == LengthTypePercentage && toType != LengthTypePercentage)
        || (!from->isZero() && !isZero() && (fromType == LengthTypeEMS || fromType == LengthTypeEXS || fromType == LengthTypeREMS || fromType == LengthTypeCHS) && fromType != toType))
        return clone();

    RefPtrWillBeRawPtr<SVGLength> length = create();

    if (fromType == LengthTypePercentage || toType == LengthTypePercentage) {
        float fromPercent = from->valueAsPercentage100();
        float toPercent = valueAsPercentage100();
        length->newValueSpecifiedUnits(LengthTypePercentage, blink::blend(fromPercent, toPercent, progress));
        return length;
    }

    if (fromType == toType || from->isZero() || isZero() || fromType == LengthTypeEMS || fromType == LengthTypeEXS || fromType == LengthTypeREMS || fromType == LengthTypeCHS) {
        float fromValue = from->valueInSpecifiedUnits();
        float toValue = valueInSpecifiedUnits();
        if (isZero())
            length->newValueSpecifiedUnits(fromType, blink::blend(fromValue, toValue, progress));
        else
            length->newValueSpecifiedUnits(toType, blink::blend(fromValue, toValue, progress));
        return length;
    }

    ASSERT(!isRelative());
    ASSERT(!from->isRelative());

    SVGLengthContext nonRelativeLengthContext(0);
    float fromValueInUserUnits = nonRelativeLengthContext.convertValueToUserUnits(from->valueInSpecifiedUnits(), from->unitMode(), fromType);
    float fromValue = nonRelativeLengthContext.convertValueFromUserUnits(fromValueInUserUnits, unitMode(), toType);

    float toValue = valueInSpecifiedUnits();
    length->newValueSpecifiedUnits(toType, blink::blend(fromValue, toValue, progress));
    return length;
}

void SVGLength::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement* contextElement)
{
    SVGLengthContext lengthContext(contextElement);
    setValue(value(lengthContext) + toSVGLength(other)->value(lengthContext), lengthContext);
}

void SVGLength::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement)
{
    RefPtrWillBeRawPtr<SVGLength> fromLength = toSVGLength(fromValue);
    RefPtrWillBeRawPtr<SVGLength> toLength = toSVGLength(toValue);
    RefPtrWillBeRawPtr<SVGLength> toAtEndOfDurationLength = toSVGLength(toAtEndOfDurationValue);

    SVGLengthContext lengthContext(contextElement);
    float animatedNumber = value(lengthContext);
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromLength->value(lengthContext), toLength->value(lengthContext), toAtEndOfDurationLength->value(lengthContext), animatedNumber);

    ASSERT(unitMode() == lengthModeForAnimatedLengthAttribute(animationElement->attributeName()));
    m_unitType = percentage < 0.5 ? fromLength->unitType() : toLength->unitType();
    setValue(animatedNumber, lengthContext);
}

float SVGLength::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, SVGElement* contextElement)
{
    SVGLengthContext lengthContext(contextElement);
    RefPtrWillBeRawPtr<SVGLength> toLength = toSVGLength(toValue);

    return fabsf(toLength->value(lengthContext) - value(lengthContext));
}

}
