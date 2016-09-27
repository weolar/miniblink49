/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Rob Buis <buis@kde.org>
 * Copyright (C) Research In Motion Limited 2009-2010. All rights reserved.
 * Copyright (C) 2011 Dirk Schulze <krit@webkit.org>
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
#include "core/layout/svg/LayoutSVGResourceClipper.h"

#include "core/SVGNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResources.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/paint/CompositingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/TransformRecorder.h"
#include "core/svg/SVGGeometryElement.h"
#include "core/svg/SVGUseElement.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/paint/ClipPathDisplayItem.h"
#include "platform/graphics/paint/CompositingDisplayItem.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/DrawingDisplayItem.h"
#include "platform/graphics/paint/SkPictureBuilder.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/pathops/SkPathOps.h"

namespace blink {

LayoutSVGResourceClipper::LayoutSVGResourceClipper(SVGClipPathElement* node)
    : LayoutSVGResourceContainer(node)
    , m_inClipExpansion(false)
{
}

LayoutSVGResourceClipper::~LayoutSVGResourceClipper()
{
}

void LayoutSVGResourceClipper::removeAllClientsFromCache(bool markForInvalidation)
{
    m_clipContentPicture.clear();
    m_clipBoundaries = FloatRect();
    markAllClientsForInvalidation(markForInvalidation ? LayoutAndBoundariesInvalidation : ParentOnlyInvalidation);
}

void LayoutSVGResourceClipper::removeClientFromCache(LayoutObject* client, bool markForInvalidation)
{
    ASSERT(client);
    markClientForInvalidation(client, markForInvalidation ? BoundariesInvalidation : ParentOnlyInvalidation);
}

bool LayoutSVGResourceClipper::tryPathOnlyClipping(const LayoutObject& layoutObject, GraphicsContext* context,
    const AffineTransform& animatedLocalTransform, const FloatRect& objectBoundingBox) {
    // If the current clip-path gets clipped itself, we have to fallback to masking.
    if (!style()->svgStyle().clipperResource().isEmpty())
        return false;

    Path clipPath;
    bool usingBuilder = false;
    SkOpBuilder clipPathBuilder;

    for (SVGElement* childElement = Traversal<SVGElement>::firstChild(*element()); childElement; childElement = Traversal<SVGElement>::nextSibling(*childElement)) {
        LayoutObject* childLayoutObject = childElement->layoutObject();
        if (!childLayoutObject)
            continue;
        // Only shapes or paths are supported for direct clipping. We need to fallback to masking for texts.
        if (childLayoutObject->isSVGText())
            return false;
        if (!childElement->isSVGGraphicsElement())
            continue;

        const ComputedStyle* style = childLayoutObject->style();
        if (!style || style->display() == NONE || style->visibility() != VISIBLE)
            continue;
        const SVGComputedStyle& svgStyle = style->svgStyle();
        // Current shape in clip-path gets clipped too. Fallback to masking.
        if (!svgStyle.clipperResource().isEmpty())
            return false;

        // First clip shape.
        if (clipPath.isEmpty()) {
            if (isSVGGeometryElement(childElement))
                toSVGGeometryElement(childElement)->toClipPath(clipPath);
            else if (isSVGUseElement(childElement))
                toSVGUseElement(childElement)->toClipPath(clipPath);

            continue;
        }

        // Multiple shapes require PathOps.
        if (!RuntimeEnabledFeatures::pathOpsSVGClippingEnabled())
            return false;

        // Second clip shape => start using the builder.
        if (!usingBuilder) {
            clipPathBuilder.add(clipPath.skPath(), kUnion_SkPathOp);
            usingBuilder = true;
        }

        Path subPath;
        if (isSVGGeometryElement(childElement))
            toSVGGeometryElement(childElement)->toClipPath(subPath);
        else if (isSVGUseElement(childElement))
            toSVGUseElement(childElement)->toClipPath(subPath);

        clipPathBuilder.add(subPath.skPath(), kUnion_SkPathOp);
    }

    if (usingBuilder) {
        SkPath resolvedPath;
        clipPathBuilder.resolve(&resolvedPath);
        clipPath = resolvedPath;
    }

    // We are able to represent the clip as a path. Continue with direct clipping,
    // and transform the content to userspace if necessary.
    if (clipPathUnits() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
        AffineTransform transform;
        transform.translate(objectBoundingBox.x(), objectBoundingBox.y());
        transform.scaleNonUniform(objectBoundingBox.width(), objectBoundingBox.height());
        clipPath.transform(transform);
    }

    // Transform path by animatedLocalTransform.
    clipPath.transform(animatedLocalTransform);

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        if (!context->displayItemList()->displayItemConstructionIsDisabled())
            context->displayItemList()->createAndAppend<BeginClipPathDisplayItem>(layoutObject, clipPath);
    } else {
        BeginClipPathDisplayItem clipPathDisplayItem(layoutObject, clipPath);
        clipPathDisplayItem.replay(*context);
    }

    return true;
}

PassRefPtr<const SkPicture> LayoutSVGResourceClipper::createContentPicture(AffineTransform& contentTransformation, const FloatRect& targetBoundingBox,
    GraphicsContext* context)
{
    ASSERT(frame());

    if (clipPathUnits() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
        contentTransformation.translate(targetBoundingBox.x(), targetBoundingBox.y());
        contentTransformation.scaleNonUniform(targetBoundingBox.width(), targetBoundingBox.height());
    }

    if (m_clipContentPicture)
        return m_clipContentPicture;

    SubtreeContentTransformScope contentTransformScope(contentTransformation);

    // Using strokeBoundingBox (instead of paintInvalidationRectInLocalCoordinates) to avoid the intersection
    // with local clips/mask, which may yield incorrect results when mixing objectBoundingBox and
    // userSpaceOnUse units (http://crbug.com/294900).
    FloatRect bounds = strokeBoundingBox();

    SkPictureBuilder pictureBuilder(bounds, nullptr, context);

    for (SVGElement* childElement = Traversal<SVGElement>::firstChild(*element()); childElement; childElement = Traversal<SVGElement>::nextSibling(*childElement)) {
        LayoutObject* layoutObject = childElement->layoutObject();
        if (!layoutObject)
            continue;

        const ComputedStyle* style = layoutObject->style();
        if (!style || style->display() == NONE || style->visibility() != VISIBLE)
            continue;

        bool isUseElement = isSVGUseElement(*childElement);
        if (isUseElement) {
            const SVGGraphicsElement* clippingElement = toSVGUseElement(*childElement).targetGraphicsElementForClipping();
            if (!clippingElement)
                continue;

            layoutObject = clippingElement->layoutObject();
            if (!layoutObject)
                continue;
        }

        // Only shapes, paths and texts are allowed for clipping.
        if (!layoutObject->isSVGShape() && !layoutObject->isSVGText())
            continue;

        if (isUseElement)
            layoutObject = childElement->layoutObject();

        // Switch to a paint behavior where all children of this <clipPath> will be laid out using special constraints:
        // - fill-opacity/stroke-opacity/opacity set to 1
        // - masker/filter not applied when laying out the children
        // - fill is set to the initial fill paint server (solid, black)
        // - stroke is set to the initial stroke paint server (none)
        PaintInfo info(&pictureBuilder.context(), LayoutRect::infiniteIntRect(), PaintPhaseForeground, PaintBehaviorRenderingClipPathAsMask);
        layoutObject->paint(info, IntPoint());
    }

    m_clipContentPicture = pictureBuilder.endRecording();
    return m_clipContentPicture;
}

void LayoutSVGResourceClipper::calculateClipContentPaintInvalidationRect()
{
    // This is a rough heuristic to appraise the clip size and doesn't consider clip on clip.
    for (SVGElement* childElement = Traversal<SVGElement>::firstChild(*element()); childElement; childElement = Traversal<SVGElement>::nextSibling(*childElement)) {
        LayoutObject* layoutObject = childElement->layoutObject();
        if (!layoutObject)
            continue;
        if (!layoutObject->isSVGShape() && !layoutObject->isSVGText() && !isSVGUseElement(*childElement))
            continue;
        const ComputedStyle* style = layoutObject->style();
        if (!style || style->display() == NONE || style->visibility() != VISIBLE)
            continue;
        m_clipBoundaries.unite(layoutObject->localToParentTransform().mapRect(layoutObject->paintInvalidationRectInLocalCoordinates()));
    }
    m_clipBoundaries = toSVGClipPathElement(element())->calculateAnimatedLocalTransform().mapRect(m_clipBoundaries);
}

bool LayoutSVGResourceClipper::hitTestClipContent(const FloatRect& objectBoundingBox, const FloatPoint& nodeAtPoint)
{
    FloatPoint point = nodeAtPoint;
    if (!SVGLayoutSupport::pointInClippingArea(this, point))
        return false;

    if (clipPathUnits() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
        AffineTransform transform;
        transform.translate(objectBoundingBox.x(), objectBoundingBox.y());
        transform.scaleNonUniform(objectBoundingBox.width(), objectBoundingBox.height());
        point = transform.inverse().mapPoint(point);
    }

    AffineTransform animatedLocalTransform = toSVGClipPathElement(element())->calculateAnimatedLocalTransform();
    if (!animatedLocalTransform.isInvertible())
        return false;

    point = animatedLocalTransform.inverse().mapPoint(point);

    for (SVGElement* childElement = Traversal<SVGElement>::firstChild(*element()); childElement; childElement = Traversal<SVGElement>::nextSibling(*childElement)) {
        LayoutObject* layoutObject = childElement->layoutObject();
        if (!layoutObject)
            continue;
        if (!layoutObject->isSVGShape() && !layoutObject->isSVGText() && !isSVGUseElement(*childElement))
            continue;
        IntPoint hitPoint;
        HitTestResult result(HitTestRequest::SVGClipContent, hitPoint);
        if (layoutObject->nodeAtFloatPoint(result, point, HitTestForeground))
            return true;
    }

    return false;
}

FloatRect LayoutSVGResourceClipper::resourceBoundingBox(const LayoutObject* object)
{
    // Resource was not layouted yet. Give back the boundingBox of the object.
    if (selfNeedsLayout())
        return object->objectBoundingBox();

    if (m_clipBoundaries.isEmpty())
        calculateClipContentPaintInvalidationRect();

    if (clipPathUnits() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
        FloatRect objectBoundingBox = object->objectBoundingBox();
        AffineTransform transform;
        transform.translate(objectBoundingBox.x(), objectBoundingBox.y());
        transform.scaleNonUniform(objectBoundingBox.width(), objectBoundingBox.height());
        return transform.mapRect(m_clipBoundaries);
    }

    return m_clipBoundaries;
}

}
