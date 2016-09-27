/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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
#include "core/layout/svg/LayoutSVGResourceGradient.h"

namespace blink {

LayoutSVGResourceGradient::LayoutSVGResourceGradient(SVGGradientElement* node)
    : LayoutSVGResourcePaintServer(node)
    , m_shouldCollectGradientAttributes(true)
{
}

void LayoutSVGResourceGradient::removeAllClientsFromCache(bool markForInvalidation)
{
    m_gradientMap.clear();
    m_shouldCollectGradientAttributes = true;
    markAllClientsForInvalidation(markForInvalidation ? PaintInvalidation : ParentOnlyInvalidation);
}

void LayoutSVGResourceGradient::removeClientFromCache(LayoutObject* client, bool markForInvalidation)
{
    ASSERT(client);
    m_gradientMap.remove(client);
    markClientForInvalidation(client, markForInvalidation ? PaintInvalidation : ParentOnlyInvalidation);
}

SVGPaintServer LayoutSVGResourceGradient::preparePaintServer(const LayoutObject& object)
{
    clearInvalidationMask();

    // Be sure to synchronize all SVG properties on the gradientElement _before_ processing any further.
    // Otherwhise the call to collectGradientAttributes() in createTileImage(), may cause the SVG DOM property
    // synchronization to kick in, which causes removeAllClientsFromCache() to be called, which in turn deletes our
    // GradientData object! Leaving out the line below will cause svg/dynamic-updates/SVG*GradientElement-svgdom* to crash.
    SVGGradientElement* gradientElement = toSVGGradientElement(element());
    if (!gradientElement)
        return SVGPaintServer::invalid();

    if (m_shouldCollectGradientAttributes) {
        gradientElement->synchronizeAnimatedSVGAttribute(anyQName());
        if (!collectGradientAttributes(gradientElement))
            return SVGPaintServer::invalid();

        m_shouldCollectGradientAttributes = false;
    }

    // Spec: When the geometry of the applicable element has no width or height and objectBoundingBox is specified,
    // then the given effect (e.g. a gradient or a filter) will be ignored.
    FloatRect objectBoundingBox = object.objectBoundingBox();
    if (gradientUnits() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX && objectBoundingBox.isEmpty())
        return SVGPaintServer::invalid();

    OwnPtr<GradientData>& gradientData = m_gradientMap.add(&object, nullptr).storedValue->value;
    if (!gradientData)
        gradientData = adoptPtr(new GradientData);

    // Create gradient object
    if (!gradientData->gradient) {
        buildGradient(gradientData.get());

        // We want the text bounding box applied to the gradient space transform now, so the gradient shader can use it.
        if (gradientUnits() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX && !objectBoundingBox.isEmpty()) {
            gradientData->userspaceTransform.translate(objectBoundingBox.x(), objectBoundingBox.y());
            gradientData->userspaceTransform.scaleNonUniform(objectBoundingBox.width(), objectBoundingBox.height());
        }

        AffineTransform gradientTransform;
        calculateGradientTransform(gradientTransform);

        gradientData->userspaceTransform *= gradientTransform;
    }

    if (!gradientData->gradient)
        return SVGPaintServer::invalid();

    gradientData->gradient->setGradientSpaceTransform(gradientData->userspaceTransform);

    return SVGPaintServer(gradientData->gradient);
}

bool LayoutSVGResourceGradient::isChildAllowed(LayoutObject* child, const ComputedStyle&) const
{
    if (child->isSVGGradientStop())
        return true;

    if (!child->isSVGResourceContainer())
        return false;

    return toLayoutSVGResourceContainer(child)->isSVGPaintServer();
}

void LayoutSVGResourceGradient::addStops(GradientData* gradientData, const Vector<Gradient::ColorStop>& stops) const
{
    ASSERT(gradientData->gradient);

    for (const auto& stop : stops)
        gradientData->gradient->addColorStop(stop);
}

GradientSpreadMethod LayoutSVGResourceGradient::platformSpreadMethodFromSVGType(SVGSpreadMethodType method) const
{
    switch (method) {
    case SVGSpreadMethodUnknown:
    case SVGSpreadMethodPad:
        return SpreadMethodPad;
    case SVGSpreadMethodReflect:
        return SpreadMethodReflect;
    case SVGSpreadMethodRepeat:
        return SpreadMethodRepeat;
    }

    ASSERT_NOT_REACHED();
    return SpreadMethodPad;
}

}
