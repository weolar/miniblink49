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

#include "core/svg/SVGAnimatedNumberOptionalNumber.h"

#include "core/svg/SVGElement.h"

namespace blink {

SVGAnimatedNumberOptionalNumber::SVGAnimatedNumberOptionalNumber(SVGElement* contextElement, const QualifiedName& attributeName, float initialFirstValue, float initialSecondValue)
    : SVGAnimatedPropertyCommon<SVGNumberOptionalNumber>(contextElement, attributeName,
        SVGNumberOptionalNumber::create(SVGNumber::create(initialFirstValue), SVGNumber::create(initialSecondValue)))
    , m_firstNumber(SVGAnimatedNumber::create(contextElement, attributeName, baseValue()->firstNumber()))
    , m_secondNumber(SVGAnimatedNumber::create(contextElement, attributeName, baseValue()->secondNumber()))
{
    m_firstNumber->setParentOptionalNumber(this);
    m_secondNumber->setParentOptionalNumber(this);
}

DEFINE_TRACE(SVGAnimatedNumberOptionalNumber)
{
    visitor->trace(m_firstNumber);
    visitor->trace(m_secondNumber);
    SVGAnimatedPropertyCommon<SVGNumberOptionalNumber>::trace(visitor);
}

void SVGAnimatedNumberOptionalNumber::setAnimatedValue(PassRefPtrWillBeRawPtr<SVGPropertyBase> value)
{
    SVGAnimatedPropertyCommon<SVGNumberOptionalNumber>::setAnimatedValue(value);
    m_firstNumber->setAnimatedValue(currentValue()->firstNumber());
    m_secondNumber->setAnimatedValue(currentValue()->secondNumber());
}

void SVGAnimatedNumberOptionalNumber::animationEnded()
{
    SVGAnimatedPropertyCommon<SVGNumberOptionalNumber>::animationEnded();
    m_firstNumber->animationEnded();
    m_secondNumber->animationEnded();
}

bool SVGAnimatedNumberOptionalNumber::needsSynchronizeAttribute()
{
    return m_firstNumber->needsSynchronizeAttribute()
        || m_secondNumber->needsSynchronizeAttribute();
}

}
