// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/SVGRootPainter.h"

#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/layout/svg/SVGResources.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/SVGPaintContext.h"
#include "core/paint/TransformRecorder.h"
#include "core/svg/SVGSVGElement.h"
#include "platform/graphics/paint/ClipRecorder.h"
#include "wtf/Optional.h"

namespace blink {

void SVGRootPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    // An empty viewport disables rendering.
    if (m_layoutSVGRoot.pixelSnappedBorderBoxRect().isEmpty())
        return;

    // SVG outlines are painted during PaintPhaseForeground.
    if (paintInfo.phase == PaintPhaseOutline || paintInfo.phase == PaintPhaseSelfOutline)
        return;

    // An empty viewBox also disables rendering.
    // (http://www.w3.org/TR/SVG/coords.html#ViewBoxAttribute)
    SVGSVGElement* svg = toSVGSVGElement(m_layoutSVGRoot.node());
    ASSERT(svg);
    if (svg->hasEmptyViewBox())
        return;

    // Don't paint if we don't have kids, except if we have filters we should paint those.
    if (!m_layoutSVGRoot.firstChild()) {
        SVGResources* resources = SVGResourcesCache::cachedResourcesForLayoutObject(&m_layoutSVGRoot);
        if (!resources || !resources->filter())
            return;
    }

    PaintInfo paintInfoBeforeFiltering(paintInfo);

    // Apply initial viewport clip.
    Optional<ClipRecorder> clipRecorder;
    if (m_layoutSVGRoot.shouldApplyViewportClip()) {
        // TODO(pdr): Clip the paint info cull rect here.
        clipRecorder.emplace(*paintInfoBeforeFiltering.context, m_layoutSVGRoot, paintInfoBeforeFiltering.displayItemTypeForClipping(), LayoutRect(pixelSnappedIntRect(m_layoutSVGRoot.overflowClipRect(paintOffset))));
    }

    // Convert from container offsets (html layoutObjects) to a relative transform (svg layoutObjects).
    // Transform from our paint container's coordinate system to our local coords.
    IntPoint adjustedPaintOffset = roundedIntPoint(paintOffset);
    AffineTransform paintOffsetToBorderBox = AffineTransform::translation(adjustedPaintOffset.x(), adjustedPaintOffset.y()) * m_layoutSVGRoot.localToBorderBoxTransform();
    paintInfoBeforeFiltering.updateCullRectForSVGTransform(paintOffsetToBorderBox);
    TransformRecorder transformRecorder(*paintInfoBeforeFiltering.context, m_layoutSVGRoot, paintOffsetToBorderBox);

    SVGPaintContext paintContext(m_layoutSVGRoot, paintInfoBeforeFiltering);
    if (paintContext.paintInfo().phase == PaintPhaseForeground && !paintContext.applyClipMaskAndFilterIfNecessary())
        return;

    BoxPainter(m_layoutSVGRoot).paint(paintContext.paintInfo(), LayoutPoint());
}

} // namespace blink
