/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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
#include "core/svg/SVGFilterPrimitiveStandardAttributes.h"

#include "core/SVGNames.h"
#include "core/layout/svg/LayoutSVGResourceContainer.h"
#include "core/layout/svg/LayoutSVGResourceFilterPrimitive.h"
#include "core/svg/SVGLength.h"
#include "platform/graphics/filters/FilterEffect.h"

namespace blink {

SVGFilterPrimitiveStandardAttributes::SVGFilterPrimitiveStandardAttributes(const QualifiedName& tagName, Document& document)
    : SVGElement(tagName, document)
    , m_x(SVGAnimatedLength::create(this, SVGNames::xAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_y(SVGAnimatedLength::create(this, SVGNames::yAttr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
    , m_width(SVGAnimatedLength::create(this, SVGNames::widthAttr, SVGLength::create(SVGLengthMode::Width), ForbidNegativeLengths))
    , m_height(SVGAnimatedLength::create(this, SVGNames::heightAttr, SVGLength::create(SVGLengthMode::Height), ForbidNegativeLengths))
    , m_result(SVGAnimatedString::create(this, SVGNames::resultAttr, SVGString::create()))
{
    // Spec: If the x/y attribute is not specified, the effect is as if a value of "0%" were specified.
    m_x->setDefaultValueAsString("0%");
    m_y->setDefaultValueAsString("0%");

    // Spec: If the width/height attribute is not specified, the effect is as if a value of "100%" were specified.
    m_width->setDefaultValueAsString("100%");
    m_height->setDefaultValueAsString("100%");

    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);
    addToPropertyMap(m_result);
}

DEFINE_TRACE(SVGFilterPrimitiveStandardAttributes)
{
    visitor->trace(m_x);
    visitor->trace(m_y);
    visitor->trace(m_width);
    visitor->trace(m_height);
    visitor->trace(m_result);
    SVGElement::trace(visitor);
}

bool SVGFilterPrimitiveStandardAttributes::setFilterEffectAttribute(FilterEffect*, const QualifiedName&)
{
    // When all filters support this method, it will be changed to a pure virtual method.
    ASSERT_NOT_REACHED();
    return false;
}

void SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr
        || attrName == SVGNames::widthAttr || attrName == SVGNames::heightAttr
        || attrName == SVGNames::resultAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    SVGElement::svgAttributeChanged(attrName);
}

void SVGFilterPrimitiveStandardAttributes::childrenChanged(const ChildrenChange& change)
{
    SVGElement::childrenChanged(change);

    if (!change.byParser)
        invalidate();
}

void SVGFilterPrimitiveStandardAttributes::setStandardAttributes(FilterEffect* filterEffect) const
{
    ASSERT(filterEffect);
    if (!filterEffect)
        return;

    if (hasAttribute(SVGNames::xAttr))
        filterEffect->setHasX(true);
    if (hasAttribute(SVGNames::yAttr))
        filterEffect->setHasY(true);
    if (hasAttribute(SVGNames::widthAttr))
        filterEffect->setHasWidth(true);
    if (hasAttribute(SVGNames::heightAttr))
        filterEffect->setHasHeight(true);
}

LayoutObject* SVGFilterPrimitiveStandardAttributes::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGResourceFilterPrimitive(this);
}

bool SVGFilterPrimitiveStandardAttributes::layoutObjectIsNeeded(const ComputedStyle& style)
{
    if (isSVGFilterElement(parentNode()))
        return SVGElement::layoutObjectIsNeeded(style);

    return false;
}

void SVGFilterPrimitiveStandardAttributes::invalidate()
{
    if (LayoutObject* primitiveLayoutObject = layoutObject())
        markForLayoutAndParentResourceInvalidation(primitiveLayoutObject);
}

void SVGFilterPrimitiveStandardAttributes::primitiveAttributeChanged(const QualifiedName& attribute)
{
    if (LayoutObject* primitiveLayoutObject = layoutObject())
        static_cast<LayoutSVGResourceFilterPrimitive*>(primitiveLayoutObject)->primitiveAttributeChanged(attribute);
}

void invalidateFilterPrimitiveParent(SVGElement* element)
{
    if (!element)
        return;

    ContainerNode* parent = element->parentNode();

    if (!parent)
        return;

    LayoutObject* layoutObject = parent->layoutObject();
    if (!layoutObject || !layoutObject->isSVGResourceFilterPrimitive())
        return;

    LayoutSVGResourceContainer::markForLayoutAndParentResourceInvalidation(layoutObject, false);
}

}
