/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Rob Buis <buis@kde.org>
 * Copyright (C) Research In Motion Limited 2009-2010. All rights reserved.
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

#include "config.h"

#include "core/svg/SVGClipPathElement.h"

#include "core/layout/svg/LayoutSVGResourceClipper.h"

namespace blink {

inline SVGClipPathElement::SVGClipPathElement(Document& document)
    : SVGGraphicsElement(SVGNames::clipPathTag, document)
    , m_clipPathUnits(SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>::create(this, SVGNames::clipPathUnitsAttr, SVGUnitTypes::SVG_UNIT_TYPE_USERSPACEONUSE))
{
    addToPropertyMap(m_clipPathUnits);
}

DEFINE_TRACE(SVGClipPathElement)
{
    visitor->trace(m_clipPathUnits);
    SVGGraphicsElement::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGClipPathElement)

void SVGClipPathElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::clipPathUnitsAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        LayoutSVGResourceContainer* layoutObject = toLayoutSVGResourceContainer(this->layoutObject());
        if (layoutObject)
            layoutObject->invalidateCacheAndMarkForLayout();
        return;
    }

    SVGGraphicsElement::svgAttributeChanged(attrName);
}

void SVGClipPathElement::childrenChanged(const ChildrenChange& change)
{
    SVGGraphicsElement::childrenChanged(change);

    if (change.byParser)
        return;

    if (LayoutObject* object = layoutObject())
        object->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::ChildChanged);
}

LayoutObject* SVGClipPathElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGResourceClipper(this);
}

}
