/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef SVGComponentTransferFunctionElement_h
#define SVGComponentTransferFunctionElement_h

#include "core/svg/SVGAnimatedEnumeration.h"
#include "core/svg/SVGAnimatedNumber.h"
#include "core/svg/SVGAnimatedNumberList.h"
#include "core/svg/SVGElement.h"
#include "platform/graphics/filters/FEComponentTransfer.h"
#include "platform/heap/Handle.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<ComponentTransferType>();

class SVGComponentTransferFunctionElement : public SVGElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    ComponentTransferFunction transferFunction() const;

    SVGAnimatedNumberList* tableValues() { return m_tableValues.get(); }
    SVGAnimatedNumber* slope() { return m_slope.get(); }
    SVGAnimatedNumber* intercept() { return m_intercept.get(); }
    SVGAnimatedNumber* amplitude() { return m_amplitude.get(); }
    SVGAnimatedNumber* exponent() { return m_exponent.get(); }
    SVGAnimatedNumber* offset() { return m_offset.get(); }
    SVGAnimatedEnumeration<ComponentTransferType>* type() { return m_type.get(); }

    DECLARE_VIRTUAL_TRACE();

protected:
    SVGComponentTransferFunctionElement(const QualifiedName&, Document&);

    void svgAttributeChanged(const QualifiedName&) final;

    bool layoutObjectIsNeeded(const ComputedStyle&) final { return false; }

private:
    RefPtrWillBeMember<SVGAnimatedNumberList> m_tableValues;
    RefPtrWillBeMember<SVGAnimatedNumber> m_slope;
    RefPtrWillBeMember<SVGAnimatedNumber> m_intercept;
    RefPtrWillBeMember<SVGAnimatedNumber> m_amplitude;
    RefPtrWillBeMember<SVGAnimatedNumber> m_exponent;
    RefPtrWillBeMember<SVGAnimatedNumber> m_offset;
    RefPtrWillBeMember<SVGAnimatedEnumeration<ComponentTransferType>> m_type;
};

} // namespace blink

#endif // SVGComponentTransferFunctionElement_h
