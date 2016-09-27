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

#include "core/svg/SVGRect.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

SVGRect::SVGRect()
    : m_isValid(true)
{
}

SVGRect::SVGRect(InvalidSVGRectTag)
{
    setInvalid();
}

SVGRect::SVGRect(const FloatRect& rect)
    : m_isValid(true)
    , m_value(rect)
{
}

PassRefPtrWillBeRawPtr<SVGRect> SVGRect::clone() const
{
    return SVGRect::create(m_value);
}

template<typename CharType>
void SVGRect::parse(const CharType*& ptr, const CharType* end, ExceptionState& exceptionState)
{
    const CharType* start = ptr;

    skipOptionalSVGSpaces(ptr, end);

    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    bool valid = parseNumber(ptr, end, x) && parseNumber(ptr, end, y) && parseNumber(ptr, end, width) && parseNumber(ptr, end, height, DisallowWhitespace);

    if (!valid) {
        exceptionState.throwDOMException(SyntaxError, "Problem parsing rect \"" + String(start, end - start) + "\"");
        setInvalid();
        return;
    }

    skipOptionalSVGSpaces(ptr, end);
    if (ptr < end) { // nothing should come after the last, fourth number
        exceptionState.throwDOMException(SyntaxError, "Problem parsing rect \"" + String(start, end - start) + "\"");
        setInvalid();
        return;
    }

    m_value = FloatRect(x, y, width, height);
    m_isValid = true;
}

void SVGRect::setValueAsString(const String& string, ExceptionState& exceptionState)
{
    if (string.isNull()) {
        setInvalid();
        return;
    }
    if (string.isEmpty()) {
        m_value = FloatRect(0.0f, 0.0f, 0.0f, 0.0f);
        m_isValid = true;
        return;
    }

    if (string.is8Bit()) {
        const LChar* ptr = string.characters8();
        const LChar* end = ptr + string.length();
        parse(ptr, end, exceptionState);
        return;
    }

    const UChar* ptr = string.characters16();
    const UChar* end = ptr + string.length();
    parse(ptr, end, exceptionState);
}

String SVGRect::valueAsString() const
{
    StringBuilder builder;
    builder.appendNumber(x());
    builder.append(' ');
    builder.appendNumber(y());
    builder.append(' ');
    builder.appendNumber(width());
    builder.append(' ');
    builder.appendNumber(height());
    return builder.toString();
}

void SVGRect::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    m_value += toSVGRect(other)->value();
}

void SVGRect::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*)
{
    ASSERT(animationElement);
    RefPtrWillBeRawPtr<SVGRect> fromRect = animationElement->animationMode() == ToAnimation ? PassRefPtrWillBeRawPtr<SVGRect>(this) : toSVGRect(fromValue);
    RefPtrWillBeRawPtr<SVGRect> toRect = toSVGRect(toValue);
    RefPtrWillBeRawPtr<SVGRect> toAtEndOfDurationRect = toSVGRect(toAtEndOfDurationValue);

    float animatedX = x();
    float animatedY = y();
    float animatedWidth = width();
    float animatedHeight = height();
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromRect->x(), toRect->x(), toAtEndOfDurationRect->x(), animatedX);
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromRect->y(), toRect->y(), toAtEndOfDurationRect->y(), animatedY);
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromRect->width(), toRect->width(), toAtEndOfDurationRect->width(), animatedWidth);
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromRect->height(), toRect->height(), toAtEndOfDurationRect->height(), animatedHeight);

    m_value = FloatRect(animatedX, animatedY, animatedWidth, animatedHeight);
}

float SVGRect::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement* contextElement)
{
    // FIXME: Distance calculation is not possible for SVGRect right now. We need the distance for every single value.
    return -1;
}

void SVGRect::setInvalid()
{
    m_value = FloatRect(0.0f, 0.0f, 0.0f, 0.0f);
    m_isValid = false;
}

}
