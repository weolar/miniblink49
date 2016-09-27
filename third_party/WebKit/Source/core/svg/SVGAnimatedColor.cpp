/*
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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
#include "core/svg/SVGAnimatedColor.h"

#include "core/css/parser/CSSParser.h"
#include "core/layout/LayoutObject.h"
#include "core/svg/ColorDistance.h"
#include "core/svg/SVGAnimateElement.h"

namespace blink {

SVGColorProperty::SVGColorProperty(const String& colorString)
    : SVGPropertyBase(classType())
    , m_styleColor(StyleColor::currentColor())
{
    RGBA32 color;
    if (CSSParser::parseColor(color, colorString.stripWhiteSpace()))
        m_styleColor = StyleColor(color);
}

String SVGColorProperty::valueAsString() const
{
    return m_styleColor.isCurrentColor() ? "currentColor" : m_styleColor.color().serializedAsCSSComponentValue();
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> SVGColorProperty::cloneForAnimation(const String&) const
{
    // SVGAnimatedColor is deprecated. So No SVG DOM animation.
    ASSERT_NOT_REACHED();
    return nullptr;
}

static inline Color fallbackColorForCurrentColor(SVGElement* targetElement)
{
    ASSERT(targetElement);
    if (LayoutObject* targetLayoutObject = targetElement->layoutObject())
        return targetLayoutObject->resolveColor(CSSPropertyColor);
    return Color::transparent;
}

void SVGColorProperty::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement* contextElement)
{
    ASSERT(contextElement);

    Color fallbackColor = fallbackColorForCurrentColor(contextElement);
    Color fromColor = toSVGColorProperty(other)->m_styleColor.resolve(fallbackColor);
    Color toColor = m_styleColor.resolve(fallbackColor);
    m_styleColor = StyleColor(ColorDistance::addColors(fromColor, toColor));
}

void SVGColorProperty::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement)
{
    StyleColor fromStyleColor = toSVGColorProperty(fromValue)->m_styleColor;
    StyleColor toStyleColor = toSVGColorProperty(toValue)->m_styleColor;
    StyleColor toAtEndOfDurationStyleColor = toSVGColorProperty(toAtEndOfDurationValue)->m_styleColor;

    // Apply currentColor rules.
    ASSERT(contextElement);
    Color fallbackColor = fallbackColorForCurrentColor(contextElement);
    Color fromColor = fromStyleColor.resolve(fallbackColor);
    Color toColor = toStyleColor.resolve(fallbackColor);
    Color toAtEndOfDurationColor = toAtEndOfDurationStyleColor.resolve(fallbackColor);
    Color animatedColor = m_styleColor.resolve(fallbackColor);

    ASSERT(animationElement);
    float animatedRed = animatedColor.red();
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromColor.red(), toColor.red(), toAtEndOfDurationColor.red(), animatedRed);

    float animatedGreen = animatedColor.green();
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromColor.green(), toColor.green(), toAtEndOfDurationColor.green(), animatedGreen);

    float animatedBlue = animatedColor.blue();
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromColor.blue(), toColor.blue(), toAtEndOfDurationColor.blue(), animatedBlue);

    float animatedAlpha = animatedColor.alpha();
    animationElement->animateAdditiveNumber(percentage, repeatCount, fromColor.alpha(), toColor.alpha(), toAtEndOfDurationColor.alpha(), animatedAlpha);

    m_styleColor = StyleColor(makeRGBA(roundf(animatedRed), roundf(animatedGreen), roundf(animatedBlue), roundf(animatedAlpha)));
}

float SVGColorProperty::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, SVGElement* contextElement)
{
    ASSERT(contextElement);
    Color fallbackColor = fallbackColorForCurrentColor(contextElement);

    Color fromColor = m_styleColor.resolve(fallbackColor);
    Color toColor = toSVGColorProperty(toValue)->m_styleColor.resolve(fallbackColor);
    return ColorDistance::distance(fromColor, toColor);
}

}
