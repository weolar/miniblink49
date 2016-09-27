/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008 Dirk Schulze <krit@webkit.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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
#include "core/svg/SVGLinearGradientElement.h"

#include "core/layout/svg/LayoutSVGResourceLinearGradient.h"
#include "core/svg/LinearGradientAttributes.h"
#include "core/svg/SVGLength.h"

namespace blink {

inline SVGLinearGradientElement::SVGLinearGradientElement(Document& document)
    : SVGGradientElement(SVGNames::linearGradientTag, document)
    , m_x1(SVGAnimatedLength::create(this, SVGNames::x1Attr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_y1(SVGAnimatedLength::create(this, SVGNames::y1Attr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
    , m_x2(SVGAnimatedLength::create(this, SVGNames::x2Attr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_y2(SVGAnimatedLength::create(this, SVGNames::y2Attr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
{

    // Spec: If the x1|y1|y2 attribute is not specified, the effect is as if a value of "0%" were specified.
    m_x1->setDefaultValueAsString("0%");
    m_y1->setDefaultValueAsString("0%");
    m_y2->setDefaultValueAsString("0%");

    // Spec: If the x2 attribute is not specified, the effect is as if a value of "100%" were specified.
    m_x2->setDefaultValueAsString("100%");

    addToPropertyMap(m_x1);
    addToPropertyMap(m_y1);
    addToPropertyMap(m_x2);
    addToPropertyMap(m_y2);
}

DEFINE_TRACE(SVGLinearGradientElement)
{
    visitor->trace(m_x1);
    visitor->trace(m_y1);
    visitor->trace(m_x2);
    visitor->trace(m_y2);
    SVGGradientElement::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGLinearGradientElement)

void SVGLinearGradientElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::x1Attr || attrName == SVGNames::x2Attr
        || attrName == SVGNames::y1Attr || attrName == SVGNames::y2Attr) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        updateRelativeLengthsInformation();

        LayoutSVGResourceContainer* layoutObject = toLayoutSVGResourceContainer(this->layoutObject());
        if (layoutObject)
            layoutObject->invalidateCacheAndMarkForLayout();

        return;
    }

    SVGGradientElement::svgAttributeChanged(attrName);
}

LayoutObject* SVGLinearGradientElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGResourceLinearGradient(this);
}

static void setGradientAttributes(SVGGradientElement* element, LinearGradientAttributes& attributes, bool isLinear = true)
{
    if (!attributes.hasSpreadMethod() && element->spreadMethod()->isSpecified())
        attributes.setSpreadMethod(element->spreadMethod()->currentValue()->enumValue());

    if (!attributes.hasGradientUnits() && element->gradientUnits()->isSpecified())
        attributes.setGradientUnits(element->gradientUnits()->currentValue()->enumValue());

    if (!attributes.hasGradientTransform() && element->gradientTransform()->isSpecified()) {
        AffineTransform transform;
        element->gradientTransform()->currentValue()->concatenate(transform);
        attributes.setGradientTransform(transform);
    }

    if (!attributes.hasStops()) {
        const Vector<Gradient::ColorStop>& stops(element->buildStops());
        if (!stops.isEmpty())
            attributes.setStops(stops);
    }

    if (isLinear) {
        SVGLinearGradientElement* linear = toSVGLinearGradientElement(element);

        if (!attributes.hasX1() && linear->x1()->isSpecified())
            attributes.setX1(linear->x1()->currentValue());

        if (!attributes.hasY1() && linear->y1()->isSpecified())
            attributes.setY1(linear->y1()->currentValue());

        if (!attributes.hasX2() && linear->x2()->isSpecified())
            attributes.setX2(linear->x2()->currentValue());

        if (!attributes.hasY2() && linear->y2()->isSpecified())
            attributes.setY2(linear->y2()->currentValue());
    }
}

bool SVGLinearGradientElement::collectGradientAttributes(LinearGradientAttributes& attributes)
{
    if (!layoutObject())
        return false;

    WillBeHeapHashSet<RawPtrWillBeMember<SVGGradientElement>> processedGradients;
    SVGGradientElement* current = this;

    setGradientAttributes(current, attributes);
    processedGradients.add(current);

    while (true) {
        // Respect xlink:href, take attributes from referenced element
        Node* refNode = SVGURIReference::targetElementFromIRIString(current->href()->currentValue()->value(), treeScope());
        if (refNode && isSVGGradientElement(*refNode)) {
            current = toSVGGradientElement(refNode);

            // Cycle detection
            if (processedGradients.contains(current))
                return true;

            if (!current->layoutObject())
                return false;

            setGradientAttributes(current, attributes, isSVGLinearGradientElement(*current));
            processedGradients.add(current);
        } else {
            return true;
        }
    }

    ASSERT_NOT_REACHED();
    return false;
}

bool SVGLinearGradientElement::selfHasRelativeLengths() const
{
    return m_x1->currentValue()->isRelative()
        || m_y1->currentValue()->isRelative()
        || m_x2->currentValue()->isRelative()
        || m_y2->currentValue()->isRelative();
}

} // namespace blink
