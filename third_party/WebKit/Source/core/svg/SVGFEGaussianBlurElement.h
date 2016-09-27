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

#ifndef SVGFEGaussianBlurElement_h
#define SVGFEGaussianBlurElement_h

#include "core/svg/SVGAnimatedNumberOptionalNumber.h"
#include "core/svg/SVGFilterPrimitiveStandardAttributes.h"
#include "platform/graphics/filters/FEGaussianBlur.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGFEGaussianBlurElement final : public SVGFilterPrimitiveStandardAttributes {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGFEGaussianBlurElement);

    void setStdDeviation(float stdDeviationX, float stdDeviationY);

    SVGAnimatedNumber* stdDeviationX() { return m_stdDeviation->firstNumber(); }
    SVGAnimatedNumber* stdDeviationY() { return m_stdDeviation->secondNumber(); }
    SVGAnimatedString* in1() { return m_in1.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGFEGaussianBlurElement(Document&);

    void svgAttributeChanged(const QualifiedName&) override;
    PassRefPtrWillBeRawPtr<FilterEffect> build(SVGFilterBuilder*, Filter*) override;

    RefPtrWillBeMember<SVGAnimatedNumberOptionalNumber> m_stdDeviation;
    RefPtrWillBeMember<SVGAnimatedString> m_in1;
};

} // namespace blink

#endif // SVGFEGaussianBlurElement_h
