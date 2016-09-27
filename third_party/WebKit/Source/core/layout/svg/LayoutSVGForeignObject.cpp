/*
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2009 Google, Inc.
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

#include "core/layout/svg/LayoutSVGForeignObject.h"

#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutView.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/paint/SVGForeignObjectPainter.h"
#include "core/svg/SVGForeignObjectElement.h"

namespace blink {

LayoutSVGForeignObject::LayoutSVGForeignObject(SVGForeignObjectElement* node)
    : LayoutSVGBlock(node)
    , m_needsTransformUpdate(true)
{
}

LayoutSVGForeignObject::~LayoutSVGForeignObject()
{
}

bool LayoutSVGForeignObject::isChildAllowed(LayoutObject* child, const ComputedStyle& style) const
{
    // Disallow arbitary SVG content. Only allow proper <svg xmlns="svgNS"> subdocuments.
    return !child->isSVG() || child->isSVGRoot();
}

void LayoutSVGForeignObject::paint(const PaintInfo& paintInfo, const LayoutPoint&)
{
    SVGForeignObjectPainter(*this).paint(paintInfo);
}

const AffineTransform& LayoutSVGForeignObject::localToParentTransform() const
{
    m_localToParentTransform = localTransform();
    m_localToParentTransform.translate(m_viewport.x(), m_viewport.y());
    return m_localToParentTransform;
}

void LayoutSVGForeignObject::updateLogicalWidth()
{
    // FIXME: Investigate in size rounding issues
    // FIXME: Remove unnecessary rounding when layout is off ints: webkit.org/b/63656
    setWidth(static_cast<int>(roundf(m_viewport.width())));
}

void LayoutSVGForeignObject::computeLogicalHeight(LayoutUnit, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    // FIXME: Investigate in size rounding issues
    // FIXME: Remove unnecessary rounding when layout is off ints: webkit.org/b/63656
    // FIXME: Is this correct for vertical writing mode?
    computedValues.m_extent = static_cast<int>(roundf(m_viewport.height()));
    computedValues.m_position = logicalTop;
}

void LayoutSVGForeignObject::layout()
{
    ASSERT(needsLayout());

    SVGForeignObjectElement* foreign = toSVGForeignObjectElement(node());

    bool updateCachedBoundariesInParents = false;
    if (m_needsTransformUpdate) {
        m_localTransform = foreign->calculateAnimatedLocalTransform();
        m_needsTransformUpdate = false;
        updateCachedBoundariesInParents = true;
    }

    FloatRect oldViewport = m_viewport;

    // Cache viewport boundaries
    SVGLengthContext lengthContext(foreign);
    FloatPoint viewportLocation(
        lengthContext.valueForLength(styleRef().svgStyle().x(), styleRef(), SVGLengthMode::Width),
        lengthContext.valueForLength(styleRef().svgStyle().y(), styleRef(), SVGLengthMode::Height));
    m_viewport = FloatRect(viewportLocation, FloatSize(
        lengthContext.valueForLength(styleRef().width(), styleRef(), SVGLengthMode::Width),
        lengthContext.valueForLength(styleRef().height(), styleRef(), SVGLengthMode::Height)));
    if (!updateCachedBoundariesInParents)
        updateCachedBoundariesInParents = oldViewport != m_viewport;

    // Set box origin to the foreignObject x/y translation, so positioned objects in XHTML content get correct
    // positions. A regular LayoutBoxModelObject would pull this information from ComputedStyle - in SVG those
    // properties are ignored for non <svg> elements, so we mimic what happens when specifying them through CSS.

    // FIXME: Investigate in location rounding issues - only affects LayoutSVGForeignObject & LayoutSVGText
    setLocation(roundedIntPoint(viewportLocation));

    bool layoutChanged = everHadLayout() && selfNeedsLayout();
    LayoutBlock::layout();
    ASSERT(!needsLayout());

    // If our bounds changed, notify the parents.
    if (updateCachedBoundariesInParents)
        LayoutSVGBlock::setNeedsBoundariesUpdate();

    // Invalidate all resources of this client if our layout changed.
    if (layoutChanged)
        SVGResourcesCache::clientLayoutChanged(this);
}

bool LayoutSVGForeignObject::nodeAtFloatPoint(HitTestResult& result, const FloatPoint& pointInParent, HitTestAction hitTestAction)
{
    // Embedded content is drawn in the foreground phase.
    if (hitTestAction != HitTestForeground)
        return false;

    AffineTransform localTransform = this->localTransform();
    if (!localTransform.isInvertible())
        return false;

    FloatPoint localPoint = localTransform.inverse().mapPoint(pointInParent);

    // Early exit if local point is not contained in clipped viewport area
    if (SVGLayoutSupport::isOverflowHidden(this) && !m_viewport.contains(localPoint))
        return false;

    // FOs establish a stacking context, so we need to hit-test all layers.
    HitTestLocation hitTestLocation(roundedLayoutPoint(localPoint));
    return LayoutBlock::nodeAtPoint(result, hitTestLocation, LayoutPoint(), HitTestForeground)
        || LayoutBlock::nodeAtPoint(result, hitTestLocation, LayoutPoint(), HitTestFloat)
        || LayoutBlock::nodeAtPoint(result, hitTestLocation, LayoutPoint(), HitTestChildBlockBackgrounds);
}

}
