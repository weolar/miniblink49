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

#include "core/svg/SVGFECompositeElement.h"

#include "core/SVGNames.h"
#include "platform/graphics/filters/FilterEffect.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<CompositeOperationType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(FECOMPOSITE_OPERATOR_OVER, "over"));
        entries.append(SVGEnumerationBase::StringEntry(FECOMPOSITE_OPERATOR_IN, "in"));
        entries.append(SVGEnumerationBase::StringEntry(FECOMPOSITE_OPERATOR_OUT, "out"));
        entries.append(SVGEnumerationBase::StringEntry(FECOMPOSITE_OPERATOR_ATOP, "atop"));
        entries.append(SVGEnumerationBase::StringEntry(FECOMPOSITE_OPERATOR_XOR, "xor"));
        entries.append(SVGEnumerationBase::StringEntry(FECOMPOSITE_OPERATOR_ARITHMETIC, "arithmetic"));
        entries.append(SVGEnumerationBase::StringEntry(FECOMPOSITE_OPERATOR_LIGHTER, "lighter"));
    }
    return entries;
}

template<> unsigned short getMaxExposedEnumValue<CompositeOperationType>()
{
    return FECOMPOSITE_OPERATOR_ARITHMETIC;
}

inline SVGFECompositeElement::SVGFECompositeElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feCompositeTag, document)
    , m_k1(SVGAnimatedNumber::create(this, SVGNames::k1Attr, SVGNumber::create()))
    , m_k2(SVGAnimatedNumber::create(this, SVGNames::k2Attr, SVGNumber::create()))
    , m_k3(SVGAnimatedNumber::create(this, SVGNames::k3Attr, SVGNumber::create()))
    , m_k4(SVGAnimatedNumber::create(this, SVGNames::k4Attr, SVGNumber::create()))
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
    , m_in2(SVGAnimatedString::create(this, SVGNames::in2Attr, SVGString::create()))
    , m_svgOperator(SVGAnimatedEnumeration<CompositeOperationType>::create(this, SVGNames::operatorAttr, FECOMPOSITE_OPERATOR_OVER))
{
    addToPropertyMap(m_k1);
    addToPropertyMap(m_k2);
    addToPropertyMap(m_k3);
    addToPropertyMap(m_k4);
    addToPropertyMap(m_in1);
    addToPropertyMap(m_in2);
    addToPropertyMap(m_svgOperator);
}

DEFINE_TRACE(SVGFECompositeElement)
{
    visitor->trace(m_k1);
    visitor->trace(m_k2);
    visitor->trace(m_k3);
    visitor->trace(m_k4);
    visitor->trace(m_in1);
    visitor->trace(m_in2);
    visitor->trace(m_svgOperator);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFECompositeElement)

bool SVGFECompositeElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
    FEComposite* composite = static_cast<FEComposite*>(effect);
    if (attrName == SVGNames::operatorAttr)
        return composite->setOperation(m_svgOperator->currentValue()->enumValue());
    if (attrName == SVGNames::k1Attr)
        return composite->setK1(m_k1->currentValue()->value());
    if (attrName == SVGNames::k2Attr)
        return composite->setK2(m_k2->currentValue()->value());
    if (attrName == SVGNames::k3Attr)
        return composite->setK3(m_k3->currentValue()->value());
    if (attrName == SVGNames::k4Attr)
        return composite->setK4(m_k4->currentValue()->value());

    ASSERT_NOT_REACHED();
    return false;
}


void SVGFECompositeElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::operatorAttr
        || attrName == SVGNames::k1Attr
        || attrName == SVGNames::k2Attr
        || attrName == SVGNames::k3Attr
        || attrName == SVGNames::k4Attr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        primitiveAttributeChanged(attrName);
        return;
    }

    if (attrName == SVGNames::inAttr || attrName == SVGNames::in2Attr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFECompositeElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    FilterEffect* input1 = filterBuilder->getEffectById(AtomicString(m_in1->currentValue()->value()));
    FilterEffect* input2 = filterBuilder->getEffectById(AtomicString(m_in2->currentValue()->value()));

    if (!input1 || !input2)
        return nullptr;

    RefPtrWillBeRawPtr<FilterEffect> effect = FEComposite::create(filter, m_svgOperator->currentValue()->enumValue(), m_k1->currentValue()->value(), m_k2->currentValue()->value(), m_k3->currentValue()->value(), m_k4->currentValue()->value());
    FilterEffectVector& inputEffects = effect->inputEffects();
    inputEffects.reserveCapacity(2);
    inputEffects.append(input1);
    inputEffects.append(input2);
    return effect.release();
}

} // namespace blink
