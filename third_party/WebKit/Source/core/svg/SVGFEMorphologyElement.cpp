/*
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#include "core/svg/SVGFEMorphologyElement.h"

#include "core/SVGNames.h"
#include "platform/graphics/filters/FilterEffect.h"
#include "core/svg/SVGParserUtilities.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<MorphologyOperatorType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(FEMORPHOLOGY_OPERATOR_ERODE, "erode"));
        entries.append(SVGEnumerationBase::StringEntry(FEMORPHOLOGY_OPERATOR_DILATE, "dilate"));
    }
    return entries;
}

inline SVGFEMorphologyElement::SVGFEMorphologyElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feMorphologyTag, document)
    , m_radius(SVGAnimatedNumberOptionalNumber::create(this, SVGNames::radiusAttr))
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
    , m_svgOperator(SVGAnimatedEnumeration<MorphologyOperatorType>::create(this, SVGNames::operatorAttr, FEMORPHOLOGY_OPERATOR_ERODE))
{
    addToPropertyMap(m_radius);
    addToPropertyMap(m_in1);
    addToPropertyMap(m_svgOperator);
}

DEFINE_TRACE(SVGFEMorphologyElement)
{
    visitor->trace(m_radius);
    visitor->trace(m_in1);
    visitor->trace(m_svgOperator);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFEMorphologyElement)

bool SVGFEMorphologyElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
    FEMorphology* morphology = static_cast<FEMorphology*>(effect);
    if (attrName == SVGNames::operatorAttr)
        return morphology->setMorphologyOperator(m_svgOperator->currentValue()->enumValue());
    if (attrName == SVGNames::radiusAttr) {
        // Both setRadius functions should be evaluated separately.
        bool isRadiusXChanged = morphology->setRadiusX(radiusX()->currentValue()->value());
        bool isRadiusYChanged = morphology->setRadiusY(radiusY()->currentValue()->value());
        return isRadiusXChanged || isRadiusYChanged;
    }

    ASSERT_NOT_REACHED();
    return false;
}

void SVGFEMorphologyElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::operatorAttr || attrName == SVGNames::radiusAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        primitiveAttributeChanged(attrName);
        return;
    }

    if (attrName == SVGNames::inAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEMorphologyElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    FilterEffect* input1 = filterBuilder->getEffectById(AtomicString(m_in1->currentValue()->value()));
    float xRadius = radiusX()->currentValue()->value();
    float yRadius = radiusY()->currentValue()->value();

    if (!input1)
        return nullptr;

    if (xRadius < 0 || yRadius < 0)
        return nullptr;

    RefPtrWillBeRawPtr<FilterEffect> effect = FEMorphology::create(filter, m_svgOperator->currentValue()->enumValue(), xRadius, yRadius);
    effect->inputEffects().append(input1);
    return effect.release();
}

} // namespace blink
