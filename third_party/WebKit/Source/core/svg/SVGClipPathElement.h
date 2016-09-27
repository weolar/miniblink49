/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef SVGClipPathElement_h
#define SVGClipPathElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedBoolean.h"
#include "core/svg/SVGAnimatedEnumeration.h"
#include "core/svg/SVGGraphicsElement.h"
#include "core/svg/SVGUnitTypes.h"
#include "platform/heap/Handle.h"

namespace blink {

class LayoutObject;

class SVGClipPathElement final : public SVGGraphicsElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGClipPathElement);
    SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>* clipPathUnits() { return m_clipPathUnits.get(); }

    bool supportsFocus() const override { return false; }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGClipPathElement(Document&);

    bool needsPendingResourceHandling() const override { return false; }

    void svgAttributeChanged(const QualifiedName&) override;
    void childrenChanged(const ChildrenChange&) override;

    LayoutObject* createLayoutObject(const ComputedStyle&) override;

    RefPtrWillBeMember<SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>> m_clipPathUnits;
};

} // namespace blink

#endif // SVGClipPathElement_h
