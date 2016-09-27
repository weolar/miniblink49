/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2008 Rob Buis <buis@kde.org>
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

#ifndef SVGTextPositioningElement_h
#define SVGTextPositioningElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedLengthList.h"
#include "core/svg/SVGAnimatedNumberList.h"
#include "core/svg/SVGTextContentElement.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGTextPositioningElement : public SVGTextContentElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    static SVGTextPositioningElement* elementFromLayoutObject(LayoutObject&);

    SVGAnimatedLengthList* x() { return m_x.get(); }
    SVGAnimatedLengthList* y() { return m_y.get(); }
    SVGAnimatedLengthList* dx() { return m_dx.get(); }
    SVGAnimatedLengthList* dy() { return m_dy.get(); }
    SVGAnimatedNumberList* rotate() { return m_rotate.get(); }

    DECLARE_VIRTUAL_TRACE();

protected:
    SVGTextPositioningElement(const QualifiedName&, Document&);

    void svgAttributeChanged(const QualifiedName&) final;
    bool isTextPositioning() const final { return true; }

    RefPtrWillBeMember<SVGAnimatedLengthList> m_x;
    RefPtrWillBeMember<SVGAnimatedLengthList> m_y;
    RefPtrWillBeMember<SVGAnimatedLengthList> m_dx;
    RefPtrWillBeMember<SVGAnimatedLengthList> m_dy;
    RefPtrWillBeMember<SVGAnimatedNumberList> m_rotate;
};

inline bool isSVGTextPositioningElement(const SVGElement& element)
{
    return element.isTextPositioning();
}

DEFINE_SVGELEMENT_TYPE_CASTS_WITH_FUNCTION(SVGTextPositioningElement);

} // namespace blink

#endif // SVGTextPositioningElement_h
