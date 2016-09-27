/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Google, Inc.
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
#include "core/layout/svg/LayoutSVGViewportContainer.h"

#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/paint/SVGContainerPainter.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/SVGUseElement.h"
#include "platform/graphics/GraphicsContext.h"

namespace blink {

LayoutSVGViewportContainer::LayoutSVGViewportContainer(SVGElement* node)
    : LayoutSVGContainer(node)
    , m_didTransformToRootUpdate(false)
    , m_isLayoutSizeChanged(false)
    , m_needsTransformUpdate(true)
{
}

void LayoutSVGViewportContainer::determineIfLayoutSizeChanged()
{
    ASSERT(element());
    if (!isSVGSVGElement(*element()))
        return;

    m_isLayoutSizeChanged = toSVGSVGElement(element())->hasRelativeLengths() && selfNeedsLayout();
}

void LayoutSVGViewportContainer::calcViewport()
{
    SVGElement* element = this->element();
    ASSERT(element);
    if (!isSVGSVGElement(*element))
        return;
    SVGSVGElement* svg = toSVGSVGElement(element);
    FloatRect oldViewport = m_viewport;

    SVGLengthContext lengthContext(element);
    m_viewport = FloatRect(svg->x()->currentValue()->value(lengthContext), svg->y()->currentValue()->value(lengthContext), svg->width()->currentValue()->value(lengthContext), svg->height()->currentValue()->value(lengthContext));

    if (oldViewport != m_viewport) {
        setNeedsBoundariesUpdate();
        setNeedsTransformUpdate();
    }
}

bool LayoutSVGViewportContainer::calculateLocalTransform()
{
    m_didTransformToRootUpdate = m_needsTransformUpdate || SVGLayoutSupport::transformToRootChanged(parent());
    if (!m_needsTransformUpdate)
        return false;

    m_localToParentTransform = AffineTransform::translation(m_viewport.x(), m_viewport.y()) * viewportTransform();
    m_needsTransformUpdate = false;
    return true;
}

AffineTransform LayoutSVGViewportContainer::viewportTransform() const
{
    ASSERT(element());
    if (isSVGSVGElement(*element())) {
        SVGSVGElement* svg = toSVGSVGElement(element());
        return svg->viewBoxToViewTransform(m_viewport.width(), m_viewport.height());
    }

    return AffineTransform();
}

bool LayoutSVGViewportContainer::pointIsInsideViewportClip(const FloatPoint& pointInParent)
{
    // Respect the viewport clip (which is in parent coords)
    if (!SVGLayoutSupport::isOverflowHidden(this))
        return true;

    return m_viewport.contains(pointInParent);
}

void LayoutSVGViewportContainer::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    SVGContainerPainter(*this).paint(paintInfo);
}

}
