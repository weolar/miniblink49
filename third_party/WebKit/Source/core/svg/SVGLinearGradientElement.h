/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef SVGLinearGradientElement_h
#define SVGLinearGradientElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedLength.h"
#include "core/svg/SVGGradientElement.h"
#include "platform/heap/Handle.h"

namespace blink {

struct LinearGradientAttributes;

class SVGLinearGradientElement final : public SVGGradientElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGLinearGradientElement);

    bool collectGradientAttributes(LinearGradientAttributes&);

    SVGAnimatedLength* x1() const { return m_x1.get(); }
    SVGAnimatedLength* y1() const { return m_y1.get(); }
    SVGAnimatedLength* x2() const { return m_x2.get(); }
    SVGAnimatedLength* y2() const { return m_y2.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGLinearGradientElement(Document&);

    void svgAttributeChanged(const QualifiedName&) override;

    LayoutObject* createLayoutObject(const ComputedStyle&) override;

    bool selfHasRelativeLengths() const override;

    RefPtrWillBeMember<SVGAnimatedLength> m_x1;
    RefPtrWillBeMember<SVGAnimatedLength> m_y1;
    RefPtrWillBeMember<SVGAnimatedLength> m_x2;
    RefPtrWillBeMember<SVGAnimatedLength> m_y2;
};

} // namespace blink

#endif // SVGLinearGradientElement_h
