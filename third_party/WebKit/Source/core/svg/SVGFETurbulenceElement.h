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

#ifndef SVGFETurbulenceElement_h
#define SVGFETurbulenceElement_h

#include "core/svg/SVGAnimatedEnumeration.h"
#include "core/svg/SVGAnimatedInteger.h"
#include "core/svg/SVGAnimatedNumber.h"
#include "core/svg/SVGAnimatedNumberOptionalNumber.h"
#include "core/svg/SVGFilterPrimitiveStandardAttributes.h"
#include "platform/graphics/filters/FETurbulence.h"
#include "platform/heap/Handle.h"

namespace blink {

enum SVGStitchOptions {
    SVG_STITCHTYPE_UNKNOWN  = 0,
    SVG_STITCHTYPE_STITCH   = 1,
    SVG_STITCHTYPE_NOSTITCH = 2
};
template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGStitchOptions>();

template<> const SVGEnumerationStringEntries& getStaticStringEntries<TurbulenceType>();

class SVGFETurbulenceElement final : public SVGFilterPrimitiveStandardAttributes {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGFETurbulenceElement);

    SVGAnimatedNumber* baseFrequencyX() { return m_baseFrequency->firstNumber(); }
    SVGAnimatedNumber* baseFrequencyY() { return m_baseFrequency->secondNumber(); }
    SVGAnimatedNumber* seed() { return m_seed.get(); }
    SVGAnimatedEnumeration<SVGStitchOptions>* stitchTiles() { return m_stitchTiles.get(); }
    SVGAnimatedEnumeration<TurbulenceType>* type() { return m_type.get(); }
    SVGAnimatedInteger* numOctaves() { return m_numOctaves.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGFETurbulenceElement(Document&);

    bool setFilterEffectAttribute(FilterEffect*, const QualifiedName& attrName) override;
    void svgAttributeChanged(const QualifiedName&) override;
    PassRefPtrWillBeRawPtr<FilterEffect> build(SVGFilterBuilder*, Filter*) override;

    RefPtrWillBeMember<SVGAnimatedNumberOptionalNumber> m_baseFrequency;
    RefPtrWillBeMember<SVGAnimatedNumber> m_seed;
    RefPtrWillBeMember<SVGAnimatedEnumeration<SVGStitchOptions>> m_stitchTiles;
    RefPtrWillBeMember<SVGAnimatedEnumeration<TurbulenceType>> m_type;
    RefPtrWillBeMember<SVGAnimatedInteger> m_numOctaves;
};

} // namespace blink

#endif // SVGFETurbulenceElement_h
