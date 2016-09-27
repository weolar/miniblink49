/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Oliver Hunt <oliver@nerget.com>
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

#ifndef SVGFESpecularLightingElement_h
#define SVGFESpecularLightingElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedNumber.h"
#include "core/svg/SVGAnimatedNumberOptionalNumber.h"
#include "core/svg/SVGFELightElement.h"
#include "core/svg/SVGFilterPrimitiveStandardAttributes.h"
#include "platform/graphics/filters/FESpecularLighting.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGFESpecularLightingElement final : public SVGFilterPrimitiveStandardAttributes {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGFESpecularLightingElement);
    void lightElementAttributeChanged(const SVGFELightElement*, const QualifiedName&);

    SVGAnimatedNumber* specularConstant() { return m_specularConstant.get(); }
    SVGAnimatedNumber* specularExponent() { return m_specularExponent.get(); }
    SVGAnimatedNumber* surfaceScale() { return m_surfaceScale.get(); }
    SVGAnimatedNumber* kernelUnitLengthX() { return m_kernelUnitLength->firstNumber(); }
    SVGAnimatedNumber* kernelUnitLengthY() { return m_kernelUnitLength->secondNumber(); }
    SVGAnimatedString* in1() { return m_in1.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGFESpecularLightingElement(Document&);

    bool setFilterEffectAttribute(FilterEffect*, const QualifiedName&) override;
    void svgAttributeChanged(const QualifiedName&) override;
    PassRefPtrWillBeRawPtr<FilterEffect> build(SVGFilterBuilder*, Filter*) override;

    static const AtomicString& kernelUnitLengthXIdentifier();
    static const AtomicString& kernelUnitLengthYIdentifier();

    RefPtrWillBeMember<SVGAnimatedNumber> m_specularConstant;
    RefPtrWillBeMember<SVGAnimatedNumber> m_specularExponent;
    RefPtrWillBeMember<SVGAnimatedNumber> m_surfaceScale;
    RefPtrWillBeMember<SVGAnimatedNumberOptionalNumber> m_kernelUnitLength;
    RefPtrWillBeMember<SVGAnimatedString> m_in1;
};

} // namespace blink

#endif // SVGFESpecularLightingElement_h
