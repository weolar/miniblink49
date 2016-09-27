/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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
#include "core/svg/SVGPolyElement.h"

#include "core/layout/svg/LayoutSVGShape.h"
#include "core/svg/SVGAnimatedPointList.h"
#include "core/svg/SVGParserUtilities.h"

namespace blink {

SVGPolyElement::SVGPolyElement(const QualifiedName& tagName, Document& document)
    : SVGGeometryElement(tagName, document)
    , m_points(SVGAnimatedPointList::create(this, SVGNames::pointsAttr, SVGPointList::create()))
{
    addToPropertyMap(m_points);
}

DEFINE_TRACE(SVGPolyElement)
{
    visitor->trace(m_points);
    SVGGeometryElement::trace(visitor);
}

Path SVGPolyElement::asPathFromPoints() const
{
    Path path;

    RefPtrWillBeRawPtr<SVGPointList> pointsValue = points()->currentValue();
    if (pointsValue->isEmpty())
        return path;

    SVGPointList::ConstIterator it = pointsValue->begin();
    SVGPointList::ConstIterator itEnd = pointsValue->end();
    ASSERT(it != itEnd);
    path.moveTo(it->value());
    ++it;

    for (; it != itEnd; ++it)
        path.addLineTo(it->value());

    return path;
}

void SVGPolyElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::pointsAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        LayoutSVGShape* layoutObject = toLayoutSVGShape(this->layoutObject());
        if (!layoutObject)
            return;

        layoutObject->setNeedsShapeUpdate();
        markForLayoutAndParentResourceInvalidation(layoutObject);
        return;
    }

    SVGGeometryElement::svgAttributeChanged(attrName);
}

}
