/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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
#include "core/svg/SVGAngle.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"
#include "wtf/MathExtras.h"
#include "wtf/text/WTFString.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGMarkerOrientType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
//         entries.append(std::make_pair(SVGMarkerOrientAuto, "auto"));
//         entries.append(std::make_pair(SVGMarkerOrientAngle, "angle"));
//         entries.append(std::make_pair(SVGMarkerOrientAutoStartReverse, "auto-start-reverse"));
        entries.append(SVGEnumerationBase::StringEntry(SVGMarkerOrientAuto, "auto"));
        entries.append(SVGEnumerationBase::StringEntry(SVGMarkerOrientAngle, "angle"));
        entries.append(SVGEnumerationBase::StringEntry(SVGMarkerOrientAutoStartReverse, "auto-start-reverse"));
    }
    return entries;
}

template<> unsigned short getMaxExposedEnumValue<SVGMarkerOrientType>()
{
    return SVGMarkerOrientAngle;
}

SVGMarkerOrientEnumeration::SVGMarkerOrientEnumeration(SVGAngle* angle)
    : SVGEnumeration<SVGMarkerOrientType>(SVGMarkerOrientAngle)
    , m_angle(angle)
{
}

SVGMarkerOrientEnumeration::~SVGMarkerOrientEnumeration()
{
}

DEFINE_TRACE(SVGMarkerOrientEnumeration)
{
    visitor->trace(m_angle);
    SVGEnumeration<SVGMarkerOrientType>::trace(visitor);
}

void SVGMarkerOrientEnumeration::notifyChange()
{
    ASSERT(m_angle);
    m_angle->orientTypeChanged();
}

void SVGMarkerOrientEnumeration::add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*)
{
    // SVGMarkerOrientEnumeration is only animated via SVGAngle
    ASSERT_NOT_REACHED();
}

void SVGMarkerOrientEnumeration::calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement)
{
    // SVGMarkerOrientEnumeration is only animated via SVGAngle
    ASSERT_NOT_REACHED();
}

float SVGMarkerOrientEnumeration::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement* contextElement)
{
    // SVGMarkerOrientEnumeration is only animated via SVGAngle
    ASSERT_NOT_REACHED();
    return -1.0;
}

SVGAngle::SVGAngle()
    : m_unitType(SVG_ANGLETYPE_UNSPECIFIED)
    , m_valueInSpecifiedUnits(0)
    , m_orientType(SVGMarkerOrientEnumeration::create(this))
{
}

SVGAngle::SVGAngle(SVGAngleType unitType, float valueInSpecifiedUnits, SVGMarkerOrientType orientType)
    : m_unitType(unitType)
    , m_valueInSpecifiedUnits(valueInSpecifiedUnits)
    , m_orientType(SVGMarkerOrientEnumeration::create(this))
{
    m_orientType->setEnumValue(orientType);
}

SVGAngle::~SVGAngle()
{
}

DEFINE_TRACE(SVGAngle)
{
    visitor->trace(m_orientType);
    SVGPropertyHelper<SVGAngle>::trace(visitor);
}

PassRefPtrWillBeRawPtr<SVGAngle> SVGAngle::clone() const
{
    return adoptRefWillBeNoop(new SVGAngle(m_unitType, m_valueInSpecifiedUnits, m_orientType->enumValue()));
}

float SVGAngle::value() const
{
    switch (m_unitType) {
    case SVG_ANGLETYPE_GRAD:
        return grad2deg(m_valueInSpecifiedUnits);
    case SVG_ANGLETYPE_RAD:
        return rad2deg(m_valueInSpecifiedUnits);
    case SVG_ANGLETYPE_TURN:
        return turn2deg(m_valueInSpecifiedUnits);
    case SVG_ANGLETYPE_UNSPECIFIED:
    case SVG_ANGLETYPE_UNKNOWN:
    case SVG_ANGLETYPE_DEG:
        return m_valueInSpecifiedUnits;
    }

    ASSERT_NOT_REACHED();
    return 0;
}

void SVGAngle::setValue(float value)
{
    switch (m_unitType) {
    case SVG_ANGLETYPE_GRAD:
        m_valueInSpecifiedUnits = deg2grad(value);
        break;
    case SVG_ANGLETYPE_RAD:
        m_valueInSpecifiedUnits = deg2rad(value);
        break;
    case SVG_ANGLETYPE_TURN:
        m_valueInSpecifiedUnits = deg2turn(value);
        break;
    case SVG_ANGLETYPE_UNSPECIFIED:
    case SVG_ANGLETYPE_UNKNOWN:
    case SVG_ANGLETYPE_DEG:
        m_valueInSpecifiedUnits = value;
        break;
    }
    m_orientType->setEnumValue(SVGMarkerOrientAngle);
}

template<typename CharType>
static SVGAngle::SVGAngleType stringToAngleType(const CharType*& ptr, const CharType* end)
{
    // If there's no unit given, the angle type is unspecified.
    if (ptr == end)
        return SVGAngle::SVG_ANGLETYPE_UNSPECIFIED;

    SVGAngle::SVGAngleType type = SVGAngle::SVG_ANGLETYPE_UNKNOWN;
    const CharType firstChar = *ptr++;

    if (isHTMLSpace<CharType>(firstChar)) {
        type = SVGAngle::SVG_ANGLETYPE_UNSPECIFIED;
    } else if (end - ptr >= 2) {
        const CharType secondChar = *ptr++;
        const CharType thirdChar = *ptr++;
        if (firstChar == 'd' && secondChar == 'e' && thirdChar == 'g') {
            type = SVGAngle::SVG_ANGLETYPE_DEG;
        } else if (firstChar == 'r' && secondChar == 'a' && thirdChar == 'd') {
            type = SVGAngle::SVG_ANGLETYPE_RAD;
        } else if (ptr != end) {
            const CharType fourthChar = *ptr++;
            if (firstChar == 'g' && secondChar == 'r' && thirdChar == 'a' && fourthChar == 'd')
                type = SVGAngle::SVG_ANGLETYPE_GRAD;
            else if (firstChar == 't' && secondChar == 'u' && thirdChar == 'r' && fourthChar == 'n')
                type = SVGAngle::SVG_ANGLETYPE_TURN;
        }
    }

    if (!skipOptionalSVGSpaces(ptr, end))
        return type;

    return SVGAngle::SVG_ANGLETYPE_UNKNOWN;
}

String SVGAngle::valueAsString() const
{
    switch (m_unitType) {
    case SVG_ANGLETYPE_DEG: {
        DEFINE_STATIC_LOCAL(String, degString, ("deg"));
        return String::number(m_valueInSpecifiedUnits) + degString;
    }
    case SVG_ANGLETYPE_RAD: {
        DEFINE_STATIC_LOCAL(String, radString, ("rad"));
        return String::number(m_valueInSpecifiedUnits) + radString;
    }
    case SVG_ANGLETYPE_GRAD: {
        DEFINE_STATIC_LOCAL(String, gradString, ("grad"));
        return String::number(m_valueInSpecifiedUnits) + gradString;
    }
    case SVG_ANGLETYPE_TURN: {
        DEFINE_STATIC_LOCAL(String, turnString, ("turn"));
        return String::number(m_valueInSpecifiedUnits) + turnString;
    }
    case SVG_ANGLETYPE_UNSPECIFIED:
    case SVG_ANGLETYPE_UNKNOWN:
        return String::number(m_valueInSpecifiedUnits);
    }

    ASSERT_NOT_REACHED();
    return String();
}

template<typename CharType>
static bool parseValue(const String& value, float& valueInSpecifiedUnits, SVGAngle::SVGAngleType& unitType)
{
    const CharType* ptr = value.getCharacters<CharType>();
    const CharType* end = ptr + value.length();

    if (!parseNumber(ptr, end, valueInSpecifiedUnits, AllowLeadingWhitespace))
        return false;

    unitType = stringToAngleType(ptr, end);
    if (unitType == SVGAngle::SVG_ANGLETYPE_UNKNOWN)
        return false;

    return true;
}

void SVGAngle::setValueAsString(const String& value, ExceptionState& exceptionState)
{
    if (value.isEmpty()) {
        newValueSpecifiedUnits(SVG_ANGLETYPE_UNSPECIFIED, 0);
        return;
    }

    if (value == "auto") {
        newValueSpecifiedUnits(SVG_ANGLETYPE_UNSPECIFIED, 0);
        m_orientType->setEnumValue(SVGMarkerOrientAuto);
        return;
    }
    if (value == "auto-start-reverse") {
        newValueSpecifiedUnits(SVG_ANGLETYPE_UNSPECIFIED, 0);
        m_orientType->setEnumValue(SVGMarkerOrientAutoStartReverse);
        return;
    }

    float valueInSpecifiedUnits = 0;
    SVGAngleType unitType = SVG_ANGLETYPE_UNKNOWN;

    bool success = value.is8Bit() ? parseValue<LChar>(value, valueInSpecifiedUnits, unitType)
                                  : parseValue<UChar>(value, valueInSpecifiedUnits, unitType);
    if (!success) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + value + "') is invalid.");
        return;
    }

    m_orientType->setEnumValue(SVGMarkerOrientAngle);
    m_unitType = unitType;
    m_valueInSpecifiedUnits = valueInSpecifiedUnits;
}

void SVGAngle::newValueSpecifiedUnits(SVGAngleType unitType, float valueInSpecifiedUnits)
{
    m_orientType->setEnumValue(SVGMarkerOrientAngle);
    m_unitType = unitType;
    m_valueInSpecifiedUnits = valueInSpecifiedUnits;
}

void SVGAngle::convertToSpecifiedUnits(SVGAngleType unitType, ExceptionState& exceptionState)
{
    if (m_unitType == SVG_ANGLETYPE_UNKNOWN) {
        exceptionState.throwDOMException(NotSupportedError, "Cannot convert from unknown or invalid units.");
        return;
    }

    if (unitType == m_unitType)
        return;

    switch (m_unitType) {
    case SVG_ANGLETYPE_TURN:
        switch (unitType) {
        case SVG_ANGLETYPE_GRAD:
            m_valueInSpecifiedUnits = turn2grad(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_UNSPECIFIED:
        case SVG_ANGLETYPE_DEG:
            m_valueInSpecifiedUnits = turn2deg(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_RAD:
            m_valueInSpecifiedUnits = deg2rad(turn2deg(m_valueInSpecifiedUnits));
            break;
        case SVG_ANGLETYPE_TURN:
        case SVG_ANGLETYPE_UNKNOWN:
            ASSERT_NOT_REACHED();
            break;
        }
        break;
    case SVG_ANGLETYPE_RAD:
        switch (unitType) {
        case SVG_ANGLETYPE_GRAD:
            m_valueInSpecifiedUnits = rad2grad(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_UNSPECIFIED:
        case SVG_ANGLETYPE_DEG:
            m_valueInSpecifiedUnits = rad2deg(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_TURN:
            m_valueInSpecifiedUnits = deg2turn(rad2deg(m_valueInSpecifiedUnits));
            break;
        case SVG_ANGLETYPE_RAD:
        case SVG_ANGLETYPE_UNKNOWN:
            ASSERT_NOT_REACHED();
            break;
        }
        break;
    case SVG_ANGLETYPE_GRAD:
        switch (unitType) {
        case SVG_ANGLETYPE_RAD:
            m_valueInSpecifiedUnits = grad2rad(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_UNSPECIFIED:
        case SVG_ANGLETYPE_DEG:
            m_valueInSpecifiedUnits = grad2deg(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_TURN:
            m_valueInSpecifiedUnits = grad2turn(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_GRAD:
        case SVG_ANGLETYPE_UNKNOWN:
            ASSERT_NOT_REACHED();
            break;
        }
        break;
    case SVG_ANGLETYPE_UNSPECIFIED:
        // Spec: For angles, a unitless value is treated the same as if degrees were specified.
    case SVG_ANGLETYPE_DEG:
        switch (unitType) {
        case SVG_ANGLETYPE_RAD:
            m_valueInSpecifiedUnits = deg2rad(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_GRAD:
            m_valueInSpecifiedUnits = deg2grad(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_TURN:
            m_valueInSpecifiedUnits = deg2turn(m_valueInSpecifiedUnits);
            break;
        case SVG_ANGLETYPE_UNSPECIFIED:
        case SVG_ANGLETYPE_DEG:
            break;
        case SVG_ANGLETYPE_UNKNOWN:
            ASSERT_NOT_REACHED();
            break;
        }
        break;
    case SVG_ANGLETYPE_UNKNOWN:
        ASSERT_NOT_REACHED();
        break;
    }

    m_unitType = unitType;
    m_orientType->setEnumValue(SVGMarkerOrientAngle);
}

void SVGAngle::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    RefPtrWillBeRawPtr<SVGAngle> otherAngle = toSVGAngle(other);

    // Only respect by animations, if from and by are both specified in angles (and not eg. 'auto').
    if (orientType()->enumValue() != SVGMarkerOrientAngle || otherAngle->orientType()->enumValue() != SVGMarkerOrientAngle)
        return;

    setValue(value() + otherAngle->value());
}

void SVGAngle::assign(const SVGAngle& other)
{
    SVGMarkerOrientType otherOrientType = other.orientType()->enumValue();
    if (otherOrientType == SVGMarkerOrientAngle)
        newValueSpecifiedUnits(other.unitType(), other.valueInSpecifiedUnits());
    else
        m_orientType->setEnumValue(otherOrientType);
}

void SVGAngle::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDuration, SVGElement*)
{
    ASSERT(animationElement);
    bool isToAnimation = animationElement->animationMode() == ToAnimation;

    RefPtrWillBeRawPtr<SVGAngle> fromAngle = isToAnimation ? PassRefPtrWillBeRawPtr<SVGAngle>(this) : toSVGAngle(from);
    RefPtrWillBeRawPtr<SVGAngle> toAngle = toSVGAngle(to);
    SVGMarkerOrientType fromOrientType = fromAngle->orientType()->enumValue();
    SVGMarkerOrientType toOrientType = toAngle->orientType()->enumValue();

    if (fromOrientType != toOrientType) {
        // Fall back to discrete animation.
        assign(percentage < 0.5f ? *fromAngle : *toAngle);
        return;
    }

    switch (fromOrientType) {
    // From 'auto' to 'auto', or 'auto-start-reverse' to 'auto-start-reverse'
    case SVGMarkerOrientAuto:
    case SVGMarkerOrientAutoStartReverse:
        orientType()->setEnumValue(fromOrientType);
        return;

    // Regular from angle to angle animation, with all features like additive etc.
    case SVGMarkerOrientAngle:
        {
            float animatedValue = value();
            RefPtrWillBeRawPtr<SVGAngle> toAtEndOfDurationAngle = toSVGAngle(toAtEndOfDuration);

            animationElement->animateAdditiveNumber(percentage, repeatCount, fromAngle->value(), toAngle->value(), toAtEndOfDurationAngle->value(), animatedValue);
            orientType()->setEnumValue(SVGMarkerOrientAngle);
            setValue(animatedValue);
        }
        return;

    // If the enumeration value is not angle or auto, its unknown.
    default:
        m_valueInSpecifiedUnits = 0;
        orientType()->setEnumValue(SVGMarkerOrientUnknown);
        return;
    }
}

float SVGAngle::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    return fabsf(value() - toSVGAngle(other)->value());
}

void SVGAngle::orientTypeChanged()
{
    if (orientType()->enumValue() == SVGMarkerOrientAuto || orientType()->enumValue() == SVGMarkerOrientAutoStartReverse) {
        m_unitType = SVG_ANGLETYPE_UNSPECIFIED;
        m_valueInSpecifiedUnits = 0;
    }
}

}
