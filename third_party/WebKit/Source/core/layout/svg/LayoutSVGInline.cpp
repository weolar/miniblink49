/*
 * Copyright (C) 2006 Oliver Hunt <ojh16@student.canterbury.ac.nz>
 * Copyright (C) 2006 Apple Inc. All rights reserved.
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

#include "core/layout/svg/LayoutSVGInline.h"

#include "core/layout/svg/LayoutSVGText.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/layout/svg/line/SVGInlineFlowBox.h"
#include "core/svg/SVGAElement.h"

namespace blink {

bool LayoutSVGInline::isChildAllowed(LayoutObject* child, const ComputedStyle& style) const
{
    if (child->isText())
        return SVGLayoutSupport::isLayoutableTextNode(child);

    if (isSVGAElement(*node())) {
        // Disallow direct descendant 'a'.
        if (isSVGAElement(*child->node()))
            return false;
    }

    if (!child->isSVGInline() && !child->isSVGInlineText())
        return false;

    return LayoutInline::isChildAllowed(child, style);
}

LayoutSVGInline::LayoutSVGInline(Element* element)
    : LayoutInline(element)
{
    setAlwaysCreateLineBoxes();
}

InlineFlowBox* LayoutSVGInline::createInlineFlowBox()
{
    InlineFlowBox* box = new SVGInlineFlowBox(*this);
    box->setHasVirtualLogicalHeight();
    return box;
}

FloatRect LayoutSVGInline::objectBoundingBox() const
{
    if (const LayoutObject* object = LayoutSVGText::locateLayoutSVGTextAncestor(this))
        return object->objectBoundingBox();

    return FloatRect();
}

FloatRect LayoutSVGInline::strokeBoundingBox() const
{
    if (const LayoutObject* object = LayoutSVGText::locateLayoutSVGTextAncestor(this))
        return object->strokeBoundingBox();

    return FloatRect();
}

FloatRect LayoutSVGInline::paintInvalidationRectInLocalCoordinates() const
{
    if (const LayoutObject* object = LayoutSVGText::locateLayoutSVGTextAncestor(this))
        return object->paintInvalidationRectInLocalCoordinates();

    return FloatRect();
}

LayoutRect LayoutSVGInline::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    return SVGLayoutSupport::clippedOverflowRectForPaintInvalidation(*this, paintInvalidationContainer, paintInvalidationState);
}

void LayoutSVGInline::mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, MapCoordinatesFlags, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    SVGLayoutSupport::mapLocalToContainer(this, paintInvalidationContainer, transformState, wasFixed, paintInvalidationState);
}

const LayoutObject* LayoutSVGInline::pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap& geometryMap) const
{
    return SVGLayoutSupport::pushMappingToContainer(this, ancestorToStopAt, geometryMap);
}

void LayoutSVGInline::absoluteQuads(Vector<FloatQuad>& quads, bool* wasFixed) const
{
    const LayoutObject* object = LayoutSVGText::locateLayoutSVGTextAncestor(this);
    if (!object)
        return;

    FloatRect textBoundingBox = object->strokeBoundingBox();
    for (InlineFlowBox* box = firstLineBox(); box; box = box->nextLineBox())
        quads.append(localToAbsoluteQuad(FloatRect(textBoundingBox.x() + box->x().toFloat(), textBoundingBox.y() + box->y().toFloat(), box->logicalWidth().toFloat(), box->logicalHeight().toFloat()), false, wasFixed));
}

void LayoutSVGInline::willBeDestroyed()
{
    SVGResourcesCache::clientDestroyed(this);
    LayoutInline::willBeDestroyed();
}

void LayoutSVGInline::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    if (diff.needsFullLayout())
        setNeedsBoundariesUpdate();

    LayoutInline::styleDidChange(diff, oldStyle);
    SVGResourcesCache::clientStyleChanged(this, diff, styleRef());
}

void LayoutSVGInline::addChild(LayoutObject* child, LayoutObject* beforeChild)
{
    LayoutInline::addChild(child, beforeChild);
    SVGResourcesCache::clientWasAddedToTree(child, child->styleRef());

    if (LayoutSVGText* textLayoutObject = LayoutSVGText::locateLayoutSVGTextAncestor(this))
        textLayoutObject->subtreeChildWasAdded(child);
}

void LayoutSVGInline::removeChild(LayoutObject* child)
{
    SVGResourcesCache::clientWillBeRemovedFromTree(child);

    LayoutSVGText* textLayoutObject = LayoutSVGText::locateLayoutSVGTextAncestor(this);
    if (!textLayoutObject) {
        LayoutInline::removeChild(child);
        return;
    }
    Vector<SVGTextLayoutAttributes*, 2> affectedAttributes;
    textLayoutObject->subtreeChildWillBeRemoved(child, affectedAttributes);
    LayoutInline::removeChild(child);
    textLayoutObject->subtreeChildWasRemoved(affectedAttributes);
}

}
