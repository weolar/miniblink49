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

#include "core/svg/SVGFEColorMatrixElement.h"

#include "core/SVGNames.h"
#include "platform/graphics/filters/FilterEffect.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<ColorMatrixType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(FECOLORMATRIX_TYPE_MATRIX, "matrix"));
        entries.append(SVGEnumerationBase::StringEntry(FECOLORMATRIX_TYPE_SATURATE, "saturate"));
        entries.append(SVGEnumerationBase::StringEntry(FECOLORMATRIX_TYPE_HUEROTATE, "hueRotate"));
        entries.append(SVGEnumerationBase::StringEntry(FECOLORMATRIX_TYPE_LUMINANCETOALPHA, "luminanceToAlpha"));
    }
    return entries;
}

inline SVGFEColorMatrixElement::SVGFEColorMatrixElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feColorMatrixTag, document)
    , m_values(SVGAnimatedNumberList::create(this, SVGNames::valuesAttr, SVGNumberList::create()))
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
    , m_type(SVGAnimatedEnumeration<ColorMatrixType>::create(this, SVGNames::typeAttr, FECOLORMATRIX_TYPE_MATRIX))
{
    addToPropertyMap(m_values);
    addToPropertyMap(m_in1);
    addToPropertyMap(m_type);
}

DEFINE_TRACE(SVGFEColorMatrixElement)
{
    visitor->trace(m_values);
    visitor->trace(m_in1);
    visitor->trace(m_type);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFEColorMatrixElement)

bool SVGFEColorMatrixElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
    FEColorMatrix* colorMatrix = static_cast<FEColorMatrix*>(effect);
    if (attrName == SVGNames::typeAttr)
        return colorMatrix->setType(m_type->currentValue()->enumValue());
    if (attrName == SVGNames::valuesAttr)
        return colorMatrix->setValues(m_values->currentValue()->toFloatVector());

    ASSERT_NOT_REACHED();
    return false;
}

void SVGFEColorMatrixElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::typeAttr || attrName == SVGNames::valuesAttr) {
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

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEColorMatrixElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    FilterEffect* input1 = filterBuilder->getEffectById(AtomicString(m_in1->currentValue()->value()));

    if (!input1)
        return nullptr;

    Vector<float> filterValues;
    ColorMatrixType filterType = m_type->currentValue()->enumValue();

    // Use defaults if values is empty (SVG 1.1 15.10).
    if (!hasAttribute(SVGNames::valuesAttr)) {
        switch (filterType) {
        case FECOLORMATRIX_TYPE_MATRIX:
            for (size_t i = 0; i < 20; i++)
                filterValues.append((i % 6) ? 0 : 1);
            break;
        case FECOLORMATRIX_TYPE_HUEROTATE:
            filterValues.append(0);
            break;
        case FECOLORMATRIX_TYPE_SATURATE:
            filterValues.append(1);
            break;
        default:
            break;
        }
    } else {
        RefPtrWillBeRawPtr<SVGNumberList> values = m_values->currentValue();
        size_t size = values->length();

        if ((filterType == FECOLORMATRIX_TYPE_MATRIX && size != 20)
            || (filterType == FECOLORMATRIX_TYPE_HUEROTATE && size != 1)
            || (filterType == FECOLORMATRIX_TYPE_SATURATE && size != 1))
            return nullptr;

        filterValues = values->toFloatVector();
    }

    RefPtrWillBeRawPtr<FilterEffect> effect = FEColorMatrix::create(filter, filterType, filterValues);
    effect->inputEffects().append(input1);
    return effect.release();
}

} // namespace blink
