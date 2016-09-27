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
#include "core/svg/SVGFEDropShadowElement.h"

#include "core/SVGNames.h"
#include "core/layout/LayoutObject.h"
#include "core/style/ComputedStyle.h"
#include "core/style/SVGComputedStyle.h"
#include "core/svg/SVGParserUtilities.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"

namespace blink {

inline SVGFEDropShadowElement::SVGFEDropShadowElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feDropShadowTag, document)
    , m_dx(SVGAnimatedNumber::create(this, SVGNames::dxAttr, SVGNumber::create(2)))
    , m_dy(SVGAnimatedNumber::create(this, SVGNames::dyAttr, SVGNumber::create(2)))
    , m_stdDeviation(SVGAnimatedNumberOptionalNumber::create(this, SVGNames::stdDeviationAttr, 2, 2))
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
{
    addToPropertyMap(m_dx);
    addToPropertyMap(m_dy);
    addToPropertyMap(m_stdDeviation);
    addToPropertyMap(m_in1);
}

DEFINE_TRACE(SVGFEDropShadowElement)
{
    visitor->trace(m_dx);
    visitor->trace(m_dy);
    visitor->trace(m_stdDeviation);
    visitor->trace(m_in1);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFEDropShadowElement)

void SVGFEDropShadowElement::setStdDeviation(float x, float y)
{
    stdDeviationX()->baseValue()->setValue(x);
    stdDeviationY()->baseValue()->setValue(y);
    invalidate();
}

void SVGFEDropShadowElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::inAttr
        || attrName == SVGNames::stdDeviationAttr
        || attrName == SVGNames::dxAttr
        || attrName == SVGNames::dyAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEDropShadowElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    LayoutObject* layoutObject = this->layoutObject();
    if (!layoutObject)
        return nullptr;

    if (stdDeviationX()->currentValue()->value() < 0 || stdDeviationY()->currentValue()->value() < 0)
        return nullptr;

    ASSERT(layoutObject->style());
    const SVGComputedStyle& svgStyle = layoutObject->style()->svgStyle();

    Color color = svgStyle.floodColor();
    float opacity = svgStyle.floodOpacity();

    FilterEffect* input1 = filterBuilder->getEffectById(AtomicString(m_in1->currentValue()->value()));
    if (!input1)
        return nullptr;

    RefPtrWillBeRawPtr<FilterEffect> effect = FEDropShadow::create(filter, stdDeviationX()->currentValue()->value(), stdDeviationY()->currentValue()->value(), m_dx->currentValue()->value(), m_dy->currentValue()->value(), color, opacity);
    effect->inputEffects().append(input1);
    return effect.release();
}

} // namespace blink
