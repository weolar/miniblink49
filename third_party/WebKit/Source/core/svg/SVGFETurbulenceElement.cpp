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

#include "core/svg/SVGFETurbulenceElement.h"

#include "core/SVGNames.h"
#include "core/svg/SVGParserUtilities.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGStitchOptions>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(SVG_STITCHTYPE_STITCH, "stitch"));
        entries.append(SVGEnumerationBase::StringEntry(SVG_STITCHTYPE_NOSTITCH, "noStitch"));
    }
    return entries;
}

template<> const SVGEnumerationStringEntries& getStaticStringEntries<TurbulenceType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(FETURBULENCE_TYPE_FRACTALNOISE, "fractalNoise"));
        entries.append(SVGEnumerationBase::StringEntry(FETURBULENCE_TYPE_TURBULENCE, "turbulence"));
    }
    return entries;
}

inline SVGFETurbulenceElement::SVGFETurbulenceElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feTurbulenceTag, document)
    , m_baseFrequency(SVGAnimatedNumberOptionalNumber::create(this, SVGNames::baseFrequencyAttr))
    , m_seed(SVGAnimatedNumber::create(this, SVGNames::seedAttr, SVGNumber::create(0)))
    , m_stitchTiles(SVGAnimatedEnumeration<SVGStitchOptions>::create(this, SVGNames::stitchTilesAttr, SVG_STITCHTYPE_NOSTITCH))
    , m_type(SVGAnimatedEnumeration<TurbulenceType>::create(this, SVGNames::typeAttr, FETURBULENCE_TYPE_TURBULENCE))
    , m_numOctaves(SVGAnimatedInteger::create(this, SVGNames::numOctavesAttr, SVGInteger::create(1)))
{
    addToPropertyMap(m_baseFrequency);
    addToPropertyMap(m_seed);
    addToPropertyMap(m_stitchTiles);
    addToPropertyMap(m_type);
    addToPropertyMap(m_numOctaves);
}

DEFINE_TRACE(SVGFETurbulenceElement)
{
    visitor->trace(m_baseFrequency);
    visitor->trace(m_seed);
    visitor->trace(m_stitchTiles);
    visitor->trace(m_type);
    visitor->trace(m_numOctaves);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFETurbulenceElement)

bool SVGFETurbulenceElement::setFilterEffectAttribute(FilterEffect* effect, const QualifiedName& attrName)
{
    FETurbulence* turbulence = static_cast<FETurbulence*>(effect);
    if (attrName == SVGNames::typeAttr)
        return turbulence->setType(m_type->currentValue()->enumValue());
    if (attrName == SVGNames::stitchTilesAttr)
        return turbulence->setStitchTiles(m_stitchTiles->currentValue()->enumValue());
    if (attrName == SVGNames::baseFrequencyAttr) {
        bool baseFrequencyXChanged = turbulence->setBaseFrequencyX(baseFrequencyX()->currentValue()->value());
        bool baseFrequencyYChanged = turbulence->setBaseFrequencyY(baseFrequencyY()->currentValue()->value());
        return (baseFrequencyXChanged || baseFrequencyYChanged);
    }
    if (attrName == SVGNames::seedAttr)
        return turbulence->setSeed(m_seed->currentValue()->value());
    if (attrName == SVGNames::numOctavesAttr)
        return turbulence->setNumOctaves(m_numOctaves->currentValue()->value());

    ASSERT_NOT_REACHED();
    return false;
}

void SVGFETurbulenceElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::baseFrequencyAttr
        || attrName == SVGNames::numOctavesAttr
        || attrName == SVGNames::seedAttr
        || attrName == SVGNames::stitchTilesAttr
        || attrName == SVGNames::typeAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        primitiveAttributeChanged(attrName);
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFETurbulenceElement::build(SVGFilterBuilder*, Filter* filter)
{
    if (baseFrequencyX()->currentValue()->value() < 0 || baseFrequencyY()->currentValue()->value() < 0)
        return nullptr;
    return FETurbulence::create(filter, m_type->currentValue()->enumValue(), baseFrequencyX()->currentValue()->value(), baseFrequencyY()->currentValue()->value(), m_numOctaves->currentValue()->value(), m_seed->currentValue()->value(), m_stitchTiles->currentValue()->enumValue() == SVG_STITCHTYPE_STITCH);
}

} // namespace blink
