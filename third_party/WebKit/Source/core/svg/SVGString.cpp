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
#include "core/svg/SVGString.h"

#include "core/svg/SVGAnimateElement.h"

namespace blink {

void SVGString::add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*)
{
    ASSERT_NOT_REACHED();
}

void SVGString::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*)
{
    ASSERT(animationElement);

    String fromString = toSVGString(from)->m_value;
    String toString = toSVGString(to)->m_value;

    animationElement->animateDiscreteType<String>(percentage, fromString, toString, m_value);
}

float SVGString::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*)
{
    // No paced animations for strings.
    return -1;
}

}
