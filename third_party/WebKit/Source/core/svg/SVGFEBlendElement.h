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

#ifndef SVGFEBlendElement_h
#define SVGFEBlendElement_h

#include "core/svg/SVGAnimatedEnumeration.h"
#include "core/svg/SVGFilterPrimitiveStandardAttributes.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGFEBlendElement final : public SVGFilterPrimitiveStandardAttributes {
    DEFINE_WRAPPERTYPEINFO();
public:
    enum Mode {
        ModeUnknown = 0,
        ModeNormal = 1,
        ModeMultiply = 2,
        ModeScreen = 3,
        ModeDarken = 4,
        ModeLighten = 5,

        // The following modes do not map to IDL constants on
        // SVGFEBlendElement.
        ModeOverlay,
        ModeColorDodge,
        ModeColorBurn,
        ModeHardLight,
        ModeSoftLight,
        ModeDifference,
        ModeExclusion,
        ModeHue,
        ModeSaturation,
        ModeColor,
        ModeLuminosity,
    };

    DECLARE_NODE_FACTORY(SVGFEBlendElement);
    SVGAnimatedString* in1() { return m_in1.get(); }
    SVGAnimatedString* in2() { return m_in2.get(); }
    SVGAnimatedEnumeration<Mode>* mode() { return m_mode.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGFEBlendElement(Document&);

    bool setFilterEffectAttribute(FilterEffect*, const QualifiedName& attrName) override;
    void svgAttributeChanged(const QualifiedName&) override;
    PassRefPtrWillBeRawPtr<FilterEffect> build(SVGFilterBuilder*, Filter*) override;

    RefPtrWillBeMember<SVGAnimatedString> m_in1;
    RefPtrWillBeMember<SVGAnimatedString> m_in2;
    RefPtrWillBeMember<SVGAnimatedEnumeration<Mode>> m_mode;
};

template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGFEBlendElement::Mode>();
template<> unsigned short getMaxExposedEnumValue<SVGFEBlendElement::Mode>();

} // namespace blink

#endif // SVGFEBlendElement_h
