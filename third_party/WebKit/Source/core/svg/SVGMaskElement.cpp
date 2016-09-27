/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Alexander Kellett <lypanov@kde.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#include "core/svg/SVGMaskElement.h"

#include "core/layout/svg/LayoutSVGResourceMasker.h"

namespace blink {

inline SVGMaskElement::SVGMaskElement(Document& document)
    : SVGElement(SVGNames::maskTag, document)
    , SVGTests(this)
    , m_x(SVGAnimatedLength::create(this, SVGNames::xAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_y(SVGAnimatedLength::create(this, SVGNames::yAttr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
    , m_width(SVGAnimatedLength::create(this, SVGNames::widthAttr, SVGLength::create(SVGLengthMode::Width), ForbidNegativeLengths))
    , m_height(SVGAnimatedLength::create(this, SVGNames::heightAttr, SVGLength::create(SVGLengthMode::Height), ForbidNegativeLengths))
    , m_maskUnits(SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>::create(this, SVGNames::maskUnitsAttr, SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX))
    , m_maskContentUnits(SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>::create(this, SVGNames::maskContentUnitsAttr, SVGUnitTypes::SVG_UNIT_TYPE_USERSPACEONUSE))
{
    // Spec: If the x/y attribute is not specified, the effect is as if a value of "-10%" were specified.
    m_x->setDefaultValueAsString("-10%");
    m_y->setDefaultValueAsString("-10%");

    // Spec: If the width/height attribute is not specified, the effect is as if a value of "120%" were specified.
    m_width->setDefaultValueAsString("120%");
    m_height->setDefaultValueAsString("120%");

    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);
    addToPropertyMap(m_maskUnits);
    addToPropertyMap(m_maskContentUnits);
}

DEFINE_TRACE(SVGMaskElement)
{
    visitor->trace(m_x);
    visitor->trace(m_y);
    visitor->trace(m_width);
    visitor->trace(m_height);
    visitor->trace(m_maskUnits);
    visitor->trace(m_maskContentUnits);
    SVGElement::trace(visitor);
    SVGTests::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGMaskElement)

bool SVGMaskElement::isPresentationAttribute(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr
        || attrName == SVGNames::widthAttr || attrName == SVGNames::heightAttr)
        return true;
    return SVGElement::isPresentationAttribute(attrName);
}

bool SVGMaskElement::isPresentationAttributeWithSVGDOM(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr
        || attrName == SVGNames::widthAttr || attrName == SVGNames::heightAttr)
        return true;
    return SVGElement::isPresentationAttributeWithSVGDOM(attrName);
}

void SVGMaskElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> property = propertyFromAttribute(name);
    if (property == m_x)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyX, *m_x->currentValue());
    else if (property == m_y)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyY, *m_y->currentValue());
    else if (property == m_width)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyWidth, *m_width->currentValue());
    else if (property == m_height)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyHeight, *m_height->currentValue());
    else
        SVGElement::collectStyleForPresentationAttribute(name, value, style);
}

void SVGMaskElement::svgAttributeChanged(const QualifiedName& attrName)
{
    bool isLengthAttr = attrName == SVGNames::xAttr
        || attrName == SVGNames::yAttr
        || attrName == SVGNames::widthAttr
        || attrName == SVGNames::heightAttr;

    if (isLengthAttr
        || attrName == SVGNames::maskUnitsAttr
        || attrName == SVGNames::maskContentUnitsAttr
        || SVGTests::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        if (isLengthAttr) {
            invalidateSVGPresentationAttributeStyle();
            setNeedsStyleRecalc(LocalStyleChange,
                StyleChangeReasonForTracing::fromAttribute(attrName));
            updateRelativeLengthsInformation();
        }

        LayoutSVGResourceContainer* layoutObject = toLayoutSVGResourceContainer(this->layoutObject());
        if (layoutObject)
            layoutObject->invalidateCacheAndMarkForLayout();

        return;
    }

    SVGElement::svgAttributeChanged(attrName);
}

void SVGMaskElement::childrenChanged(const ChildrenChange& change)
{
    SVGElement::childrenChanged(change);

    if (change.byParser)
        return;

    if (LayoutObject* object = layoutObject())
        object->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::ChildChanged);
}

LayoutObject* SVGMaskElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGResourceMasker(this);
}

bool SVGMaskElement::selfHasRelativeLengths() const
{
    return m_x->currentValue()->isRelative()
        || m_y->currentValue()->isRelative()
        || m_width->currentValue()->isRelative()
        || m_height->currentValue()->isRelative();
}

} // namespace blink
