/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Oliver Hunt <oliver@nerget.com>
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
#include "core/svg/SVGFELightElement.h"

#include "core/SVGNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/layout/LayoutObject.h"
#include "core/svg/SVGFEDiffuseLightingElement.h"
#include "core/svg/SVGFESpecularLightingElement.h"

namespace blink {

SVGFELightElement::SVGFELightElement(const QualifiedName& tagName, Document& document)
    : SVGElement(tagName, document)
    , m_azimuth(SVGAnimatedNumber::create(this, SVGNames::azimuthAttr, SVGNumber::create()))
    , m_elevation(SVGAnimatedNumber::create(this, SVGNames::elevationAttr, SVGNumber::create()))
    , m_x(SVGAnimatedNumber::create(this, SVGNames::xAttr, SVGNumber::create()))
    , m_y(SVGAnimatedNumber::create(this, SVGNames::yAttr, SVGNumber::create()))
    , m_z(SVGAnimatedNumber::create(this, SVGNames::zAttr, SVGNumber::create()))
    , m_pointsAtX(SVGAnimatedNumber::create(this, SVGNames::pointsAtXAttr, SVGNumber::create()))
    , m_pointsAtY(SVGAnimatedNumber::create(this, SVGNames::pointsAtYAttr, SVGNumber::create()))
    , m_pointsAtZ(SVGAnimatedNumber::create(this, SVGNames::pointsAtZAttr, SVGNumber::create()))
    , m_specularExponent(SVGAnimatedNumber::create(this, SVGNames::specularExponentAttr, SVGNumber::create(1)))
    , m_limitingConeAngle(SVGAnimatedNumber::create(this, SVGNames::limitingConeAngleAttr, SVGNumber::create()))
{
    addToPropertyMap(m_azimuth);
    addToPropertyMap(m_elevation);
    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_z);
    addToPropertyMap(m_pointsAtX);
    addToPropertyMap(m_pointsAtY);
    addToPropertyMap(m_pointsAtZ);
    addToPropertyMap(m_specularExponent);
    addToPropertyMap(m_limitingConeAngle);
}

DEFINE_TRACE(SVGFELightElement)
{
    visitor->trace(m_azimuth);
    visitor->trace(m_elevation);
    visitor->trace(m_x);
    visitor->trace(m_y);
    visitor->trace(m_z);
    visitor->trace(m_pointsAtX);
    visitor->trace(m_pointsAtY);
    visitor->trace(m_pointsAtZ);
    visitor->trace(m_specularExponent);
    visitor->trace(m_limitingConeAngle);
    SVGElement::trace(visitor);
}

SVGFELightElement* SVGFELightElement::findLightElement(const SVGElement& svgElement)
{
    return Traversal<SVGFELightElement>::firstChild(svgElement);
}

FloatPoint3D SVGFELightElement::position() const
{
    return FloatPoint3D(x()->currentValue()->value(), y()->currentValue()->value(), z()->currentValue()->value());
}

FloatPoint3D SVGFELightElement::pointsAt() const
{
    return FloatPoint3D(pointsAtX()->currentValue()->value(), pointsAtY()->currentValue()->value(), pointsAtZ()->currentValue()->value());
}

void SVGFELightElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::azimuthAttr
        || attrName == SVGNames::elevationAttr
        || attrName == SVGNames::xAttr
        || attrName == SVGNames::yAttr
        || attrName == SVGNames::zAttr
        || attrName == SVGNames::pointsAtXAttr
        || attrName == SVGNames::pointsAtYAttr
        || attrName == SVGNames::pointsAtZAttr
        || attrName == SVGNames::specularExponentAttr
        || attrName == SVGNames::limitingConeAngleAttr) {
        ContainerNode* parent = parentNode();
        if (!parent)
            return;

        LayoutObject* layoutObject = parent->layoutObject();
        if (!layoutObject || !layoutObject->isSVGResourceFilterPrimitive())
            return;

        SVGElement::InvalidationGuard invalidationGuard(this);
        if (isSVGFEDiffuseLightingElement(*parent)) {
            toSVGFEDiffuseLightingElement(*parent).lightElementAttributeChanged(this, attrName);
            return;
        }
        if (isSVGFESpecularLightingElement(*parent)) {
            toSVGFESpecularLightingElement(*parent).lightElementAttributeChanged(this, attrName);
            return;
        }

        ASSERT_NOT_REACHED();
    }

    SVGElement::svgAttributeChanged(attrName);
}

void SVGFELightElement::childrenChanged(const ChildrenChange& change)
{
    SVGElement::childrenChanged(change);

    if (!change.byParser) {
        if (ContainerNode* parent = parentNode()) {
            LayoutObject* layoutObject = parent->layoutObject();
            if (layoutObject && layoutObject->isSVGResourceFilterPrimitive())
                markForLayoutAndParentResourceInvalidation(layoutObject);
        }
    }
}

}
