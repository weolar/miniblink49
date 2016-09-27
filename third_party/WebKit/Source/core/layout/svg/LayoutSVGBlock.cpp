/*
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
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
#include "core/layout/svg/LayoutSVGBlock.h"

#include "core/layout/LayoutView.h"
#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/style/ShadowList.h"
#include "core/svg/SVGElement.h"

namespace blink {

LayoutSVGBlock::LayoutSVGBlock(SVGElement* element)
    : LayoutBlockFlow(element)
{
}

void LayoutSVGBlock::updateFromStyle()
{
    LayoutBlock::updateFromStyle();

    // LayoutSVGlock, used by Layout(SVGText|ForeignObject), is not allowed to call setHasOverflowClip(true).
    // LayoutBlock assumes a layer to be present when the overflow clip functionality is requested. Both
    // Layout(SVGText|ForeignObject) return 'NoDeprecatedPaintLayer' on 'layerTypeRequired'. Fine for LayoutSVGText.
    //
    // If we want to support overflow rules for <foreignObject> we can choose between two solutions:
    // a) make LayoutSVGForeignObject require layers and SVG layer aware
    // b) reactor overflow logic out of Layer (as suggested by dhyatt), which is a large task
    //
    // Until this is resolved, disable overflow support. Opera/FF don't support it as well at the moment (Feb 2010).
    //
    // Note: This does NOT affect overflow handling on outer/inner <svg> elements - this is handled
    // manually by LayoutSVGRoot - which owns the documents enclosing root layer and thus works fine.
    setHasOverflowClip(false);
}

void LayoutSVGBlock::absoluteRects(Vector<IntRect>&, const LayoutPoint&) const
{
    // This code path should never be taken for SVG, as we're assuming useTransforms=true everywhere, absoluteQuads should be used.
    ASSERT_NOT_REACHED();
}

void LayoutSVGBlock::willBeDestroyed()
{
    SVGResourcesCache::clientDestroyed(this);
    LayoutBlockFlow::willBeDestroyed();
}

void LayoutSVGBlock::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    if (diff.needsFullLayout())
        setNeedsBoundariesUpdate();

    if (isBlendingAllowed()) {
        bool hasBlendModeChanged = (oldStyle && oldStyle->hasBlendMode()) == !style()->hasBlendMode();
        if (parent() && hasBlendModeChanged)
            parent()->descendantIsolationRequirementsChanged(style()->hasBlendMode() ? DescendantIsolationRequired : DescendantIsolationNeedsUpdate);
    }

    LayoutBlock::styleDidChange(diff, oldStyle);
    SVGResourcesCache::clientStyleChanged(this, diff, styleRef());
}

void LayoutSVGBlock::mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, MapCoordinatesFlags, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    SVGLayoutSupport::mapLocalToContainer(this, paintInvalidationContainer, transformState, wasFixed, paintInvalidationState);
}

const LayoutObject* LayoutSVGBlock::pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap& geometryMap) const
{
    return SVGLayoutSupport::pushMappingToContainer(this, ancestorToStopAt, geometryMap);
}

LayoutRect LayoutSVGBlock::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    return SVGLayoutSupport::clippedOverflowRectForPaintInvalidation(*this, paintInvalidationContainer, paintInvalidationState);
}

void LayoutSVGBlock::mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, const PaintInvalidationState* paintInvalidationState) const
{
    FloatRect paintInvalidationRect = rect;
    const LayoutSVGRoot& svgRoot = SVGLayoutSupport::mapRectToSVGRootForPaintInvalidation(*this, paintInvalidationRect, rect);
    svgRoot.mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
}

bool LayoutSVGBlock::nodeAtPoint(HitTestResult&, const HitTestLocation&, const LayoutPoint&, HitTestAction)
{
    ASSERT_NOT_REACHED();
    return false;
}

void LayoutSVGBlock::invalidateTreeIfNeeded(PaintInvalidationState& paintInvalidationState)
{
    if (!shouldCheckForPaintInvalidation(paintInvalidationState))
        return;

    ForceHorriblySlowRectMapping slowRectMapping(&paintInvalidationState);
    LayoutBlockFlow::invalidateTreeIfNeeded(paintInvalidationState);
}

}
