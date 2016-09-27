/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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

#include "core/svg/SVGFEGaussianBlurElement.h"

#include "core/SVGNames.h"
#include "platform/graphics/filters/FilterEffect.h"
#include "core/svg/SVGParserUtilities.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"

namespace blink {

inline SVGFEGaussianBlurElement::SVGFEGaussianBlurElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feGaussianBlurTag, document)
    , m_stdDeviation(SVGAnimatedNumberOptionalNumber::create(this, SVGNames::stdDeviationAttr, 0, 0))
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
{
    addToPropertyMap(m_stdDeviation);
    addToPropertyMap(m_in1);
}

DEFINE_TRACE(SVGFEGaussianBlurElement)
{
    visitor->trace(m_stdDeviation);
    visitor->trace(m_in1);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFEGaussianBlurElement)

void SVGFEGaussianBlurElement::setStdDeviation(float x, float y)
{
    stdDeviationX()->baseValue()->setValue(x);
    stdDeviationY()->baseValue()->setValue(y);
    invalidate();
}

void SVGFEGaussianBlurElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::inAttr || attrName == SVGNames::stdDeviationAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEGaussianBlurElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    FilterEffect* input1 = filterBuilder->getEffectById(AtomicString(m_in1->currentValue()->value()));

    if (!input1)
        return nullptr;

    if (stdDeviationX()->currentValue()->value() < 0 || stdDeviationY()->currentValue()->value() < 0)
        return nullptr;

    RefPtrWillBeRawPtr<FilterEffect> effect = FEGaussianBlur::create(filter, stdDeviationX()->currentValue()->value(), stdDeviationY()->currentValue()->value());
    effect->inputEffects().append(input1);
    return effect.release();
}

} // namespace blink
