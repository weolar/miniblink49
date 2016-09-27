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

#ifndef SVGRadialGradientElement_h
#define SVGRadialGradientElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedLength.h"
#include "core/svg/SVGGradientElement.h"
#include "platform/heap/Handle.h"

namespace blink {

struct RadialGradientAttributes;

class SVGRadialGradientElement final : public SVGGradientElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGRadialGradientElement);

    bool collectGradientAttributes(RadialGradientAttributes&);

    SVGAnimatedLength* cx() const { return m_cx.get(); }
    SVGAnimatedLength* cy() const { return m_cy.get(); }
    SVGAnimatedLength* r() const { return m_r.get(); }
    SVGAnimatedLength* fx() const { return m_fx.get(); }
    SVGAnimatedLength* fy() const { return m_fy.get(); }
    SVGAnimatedLength* fr() const { return m_fr.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGRadialGradientElement(Document&);

    void svgAttributeChanged(const QualifiedName&) override;

    LayoutObject* createLayoutObject(const ComputedStyle&) override;

    bool selfHasRelativeLengths() const override;

    RefPtrWillBeMember<SVGAnimatedLength> m_cx;
    RefPtrWillBeMember<SVGAnimatedLength> m_cy;
    RefPtrWillBeMember<SVGAnimatedLength> m_r;
    RefPtrWillBeMember<SVGAnimatedLength> m_fx;
    RefPtrWillBeMember<SVGAnimatedLength> m_fy;
    RefPtrWillBeMember<SVGAnimatedLength> m_fr;
};

} // namespace blink

#endif // SVGRadialGradientElement_h
