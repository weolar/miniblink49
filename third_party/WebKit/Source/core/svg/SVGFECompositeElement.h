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

#ifndef SVGFECompositeElement_h
#define SVGFECompositeElement_h

#include "core/svg/SVGAnimatedEnumeration.h"
#include "core/svg/SVGAnimatedNumber.h"
#include "core/svg/SVGFilterPrimitiveStandardAttributes.h"
#include "platform/graphics/filters/FEComposite.h"
#include "platform/heap/Handle.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<CompositeOperationType>();

class SVGFECompositeElement final : public SVGFilterPrimitiveStandardAttributes {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGFECompositeElement);

    SVGAnimatedNumber* k1() { return m_k1.get(); }
    SVGAnimatedNumber* k2() { return m_k2.get(); }
    SVGAnimatedNumber* k3() { return m_k3.get(); }
    SVGAnimatedNumber* k4() { return m_k4.get(); }
    SVGAnimatedString* in1() { return m_in1.get(); }
    SVGAnimatedString* in2() { return m_in2.get(); }
    SVGAnimatedEnumeration<CompositeOperationType>* svgOperator() { return m_svgOperator.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGFECompositeElement(Document&);

    bool setFilterEffectAttribute(FilterEffect*, const QualifiedName&) override;
    void svgAttributeChanged(const QualifiedName&) override;
    PassRefPtrWillBeRawPtr<FilterEffect> build(SVGFilterBuilder*, Filter*) override;

    RefPtrWillBeMember<SVGAnimatedNumber> m_k1;
    RefPtrWillBeMember<SVGAnimatedNumber> m_k2;
    RefPtrWillBeMember<SVGAnimatedNumber> m_k3;
    RefPtrWillBeMember<SVGAnimatedNumber> m_k4;
    RefPtrWillBeMember<SVGAnimatedString> m_in1;
    RefPtrWillBeMember<SVGAnimatedString> m_in2;
    RefPtrWillBeMember<SVGAnimatedEnumeration<CompositeOperationType>> m_svgOperator;
};

} // namespace blink

#endif // SVGFECompositeElement_h
