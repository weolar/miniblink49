/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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

#include "core/svg/SVGMarkerElement.h"

#include "core/SVGNames.h"
#include "core/layout/svg/LayoutSVGResourceMarker.h"
#include "core/svg/SVGAngleTearOff.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGMarkerUnitsType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(SVGEnumerationBase::StringEntry(SVGMarkerUnitsUserSpaceOnUse, "userSpaceOnUse"));
        entries.append(SVGEnumerationBase::StringEntry(SVGMarkerUnitsStrokeWidth, "strokeWidth"));
    }
    return entries;
}


inline SVGMarkerElement::SVGMarkerElement(Document& document)
    : SVGElement(SVGNames::markerTag, document)
    , SVGFitToViewBox(this)
    , m_refX(SVGAnimatedLength::create(this, SVGNames::refXAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_refY(SVGAnimatedLength::create(this, SVGNames::refYAttr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
    , m_markerWidth(SVGAnimatedLength::create(this, SVGNames::markerWidthAttr, SVGLength::create(SVGLengthMode::Width), ForbidNegativeLengths))
    , m_markerHeight(SVGAnimatedLength::create(this, SVGNames::markerHeightAttr, SVGLength::create(SVGLengthMode::Height), ForbidNegativeLengths))
    , m_orientAngle(SVGAnimatedAngle::create(this))
    , m_markerUnits(SVGAnimatedEnumeration<SVGMarkerUnitsType>::create(this, SVGNames::markerUnitsAttr, SVGMarkerUnitsStrokeWidth))
{
    // Spec: If the markerWidth/markerHeight attribute is not specified, the effect is as if a value of "3" were specified.
    m_markerWidth->setDefaultValueAsString("3");
    m_markerHeight->setDefaultValueAsString("3");

    addToPropertyMap(m_refX);
    addToPropertyMap(m_refY);
    addToPropertyMap(m_markerWidth);
    addToPropertyMap(m_markerHeight);
    addToPropertyMap(m_orientAngle);
    addToPropertyMap(m_markerUnits);
}

DEFINE_TRACE(SVGMarkerElement)
{
    visitor->trace(m_refX);
    visitor->trace(m_refY);
    visitor->trace(m_markerWidth);
    visitor->trace(m_markerHeight);
    visitor->trace(m_orientAngle);
    visitor->trace(m_markerUnits);
    SVGElement::trace(visitor);
    SVGFitToViewBox::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGMarkerElement)

AffineTransform SVGMarkerElement::viewBoxToViewTransform(float viewWidth, float viewHeight) const
{
    return SVGFitToViewBox::viewBoxToViewTransform(viewBox()->currentValue()->value(), preserveAspectRatio()->currentValue(), viewWidth, viewHeight);
}

void SVGMarkerElement::svgAttributeChanged(const QualifiedName& attrName)
{
    bool isLengthAttr = attrName == SVGNames::refXAttr
        || attrName == SVGNames::refYAttr
        || attrName == SVGNames::markerWidthAttr
        || attrName == SVGNames::markerHeightAttr;

    if (isLengthAttr)
        updateRelativeLengthsInformation();

    if (isLengthAttr || attrName == SVGNames::markerUnitsAttr
        || attrName == SVGNames::orientAttr
        || SVGFitToViewBox::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        LayoutSVGResourceContainer* layoutObject = toLayoutSVGResourceContainer(this->layoutObject());
        if (layoutObject)
            layoutObject->invalidateCacheAndMarkForLayout();

        return;
    }

    SVGElement::svgAttributeChanged(attrName);
}

void SVGMarkerElement::childrenChanged(const ChildrenChange& change)
{
    SVGElement::childrenChanged(change);

    if (change.byParser)
        return;

    if (LayoutObject* object = layoutObject())
        object->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::ChildChanged);
}

void SVGMarkerElement::setOrientToAuto()
{
    m_orientAngle->baseValue()->orientType()->setEnumValue(SVGMarkerOrientAuto);
    invalidateSVGAttributes();
    svgAttributeChanged(SVGNames::orientAttr);
}

void SVGMarkerElement::setOrientToAngle(PassRefPtrWillBeRawPtr<SVGAngleTearOff> angle)
{
    ASSERT(angle);
    RefPtrWillBeRawPtr<SVGAngle> target = angle->target();
    m_orientAngle->baseValue()->newValueSpecifiedUnits(target->unitType(), target->valueInSpecifiedUnits());
    invalidateSVGAttributes();
    svgAttributeChanged(SVGNames::orientAttr);
}

LayoutObject* SVGMarkerElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGResourceMarker(this);
}

bool SVGMarkerElement::selfHasRelativeLengths() const
{
    return m_refX->currentValue()->isRelative()
        || m_refY->currentValue()->isRelative()
        || m_markerWidth->currentValue()->isRelative()
        || m_markerHeight->currentValue()->isRelative();
}

} // namespace blink
