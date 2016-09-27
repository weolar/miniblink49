/*
 * Copyright (c) 2009, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/svg/LayoutSVGModelObject.h"

#include "core/layout/LayoutView.h"
#include "core/layout/svg/LayoutSVGContainer.h"
#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/svg/SVGGraphicsElement.h"

namespace blink {

LayoutSVGModelObject::LayoutSVGModelObject(SVGElement* node)
    : LayoutObject(node)
{
}

bool LayoutSVGModelObject::isChildAllowed(LayoutObject* child, const ComputedStyle&) const
{
    return child->isSVG() && !(child->isSVGInline() || child->isSVGInlineText() || child->isSVGGradientStop());
}

LayoutRect LayoutSVGModelObject::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    return SVGLayoutSupport::clippedOverflowRectForPaintInvalidation(*this, paintInvalidationContainer, paintInvalidationState);
}

void LayoutSVGModelObject::mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, MapCoordinatesFlags, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    SVGLayoutSupport::mapLocalToContainer(this, paintInvalidationContainer, transformState, wasFixed, paintInvalidationState);
}

const LayoutObject* LayoutSVGModelObject::pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap& geometryMap) const
{
    return SVGLayoutSupport::pushMappingToContainer(this, ancestorToStopAt, geometryMap);
}

void LayoutSVGModelObject::absoluteRects(Vector<IntRect>& rects, const LayoutPoint& accumulatedOffset) const
{
    IntRect rect = enclosingIntRect(strokeBoundingBox());
    rect.moveBy(roundedIntPoint(accumulatedOffset));
    rects.append(rect);
}

void LayoutSVGModelObject::absoluteQuads(Vector<FloatQuad>& quads, bool* wasFixed) const
{
    quads.append(localToAbsoluteQuad(strokeBoundingBox(), 0 /* mode */, wasFixed));
}

void LayoutSVGModelObject::willBeDestroyed()
{
    SVGResourcesCache::clientDestroyed(this);
    LayoutObject::willBeDestroyed();
}

void LayoutSVGModelObject::computeLayerHitTestRects(LayerHitTestRects& rects) const
{
    // Using just the rect for the SVGRoot is good enough for now.
    SVGLayoutSupport::findTreeRootObject(this)->computeLayerHitTestRects(rects);
}

void LayoutSVGModelObject::addLayerHitTestRects(LayerHitTestRects&, const DeprecatedPaintLayer* currentLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const
{
    // We don't walk into SVG trees at all - just report their container.
}

void LayoutSVGModelObject::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    if (diff.needsFullLayout()) {
        setNeedsBoundariesUpdate();
        if (style()->hasTransform())
            setNeedsTransformUpdate();
    }

    if (isBlendingAllowed()) {
        bool hasBlendModeChanged = (oldStyle && oldStyle->hasBlendMode()) == !style()->hasBlendMode();
        if (parent() && hasBlendModeChanged)
            parent()->descendantIsolationRequirementsChanged(style()->hasBlendMode() ? DescendantIsolationRequired : DescendantIsolationNeedsUpdate);
    }

    LayoutObject::styleDidChange(diff, oldStyle);
    SVGResourcesCache::clientStyleChanged(this, diff, styleRef());
}

bool LayoutSVGModelObject::nodeAtPoint(HitTestResult&, const HitTestLocation&, const LayoutPoint&, HitTestAction)
{
    ASSERT_NOT_REACHED();
    return false;
}

// The SVG addFocusRingRects() method adds rects in local coordinates so the default absoluteFocusRingBoundingBoxRect
// returns incorrect values for SVG objects. Overriding this method provides access to the absolute bounds.
IntRect LayoutSVGModelObject::absoluteFocusRingBoundingBoxRect() const
{
    return localToAbsoluteQuad(FloatQuad(paintInvalidationRectInLocalCoordinates())).enclosingBoundingBox();
}

void LayoutSVGModelObject::invalidateTreeIfNeeded(PaintInvalidationState& paintInvalidationState)
{
    ASSERT(!needsLayout());

    // If we didn't need paint invalidation then our children don't need as well.
    // Skip walking down the tree as everything should be fine below us.
    if (!shouldCheckForPaintInvalidation(paintInvalidationState))
        return;

    invalidatePaintIfNeeded(paintInvalidationState, paintInvalidationState.paintInvalidationContainer());
    clearPaintInvalidationState(paintInvalidationState);

    PaintInvalidationState childPaintInvalidationState(paintInvalidationState, *this);
    invalidatePaintOfSubtreesIfNeeded(childPaintInvalidationState);
}

} // namespace blink
