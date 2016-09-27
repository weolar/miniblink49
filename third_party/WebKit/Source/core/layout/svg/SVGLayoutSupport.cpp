/*
 * Copyright (C) 2007, 2008 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Google, Inc.  All rights reserved.
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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
#include "core/layout/svg/SVGLayoutSupport.h"

#include "core/frame/FrameHost.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/SubtreeLayoutScope.h"
#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/layout/svg/LayoutSVGResourceClipper.h"
#include "core/layout/svg/LayoutSVGResourceFilter.h"
#include "core/layout/svg/LayoutSVGResourceMasker.h"
#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/layout/svg/LayoutSVGShape.h"
#include "core/layout/svg/LayoutSVGText.h"
#include "core/layout/svg/LayoutSVGViewportContainer.h"
#include "core/layout/svg/SVGResources.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/svg/SVGElement.h"
#include "platform/geometry/TransformState.h"
#include "platform/graphics/StrokeData.h"

namespace blink {

static inline LayoutRect enclosingIntRectIfNotEmpty(const FloatRect& rect)
{
    if (rect.isEmpty())
        return LayoutRect();
    return LayoutRect(enclosingIntRect(rect));
}

LayoutRect SVGLayoutSupport::clippedOverflowRectForPaintInvalidation(const LayoutObject& object, const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState)
{
    // Return early for any cases where we don't actually paint
    if (object.styleRef().visibility() != VISIBLE && !object.enclosingLayer()->hasVisibleContent())
        return LayoutRect();

    FloatRect paintInvalidationRect = object.paintInvalidationRectInLocalCoordinates();
    if (int outlineOutset = object.styleRef().outlineOutset())
        paintInvalidationRect.inflate(outlineOutset);

    if (paintInvalidationState && paintInvalidationState->canMapToContainer(paintInvalidationContainer)) {
        // Compute accumulated SVG transform and apply to local paint rect.
        AffineTransform transform = paintInvalidationState->svgTransform() * object.localToParentTransform();
        // FIXME: These are quirks carried forward from the old paint invalidation infrastructure.
        LayoutRect rect = enclosingIntRectIfNotEmpty(transform.mapRect(paintInvalidationRect));
        // Offset by SVG root paint offset and apply clipping as needed.
        rect.move(paintInvalidationState->paintOffset());
        if (paintInvalidationState->isClipped())
            rect.intersect(paintInvalidationState->clipRect());
        return rect;
    }

    LayoutRect rect;
    const LayoutSVGRoot& svgRoot = mapRectToSVGRootForPaintInvalidation(object, paintInvalidationRect, rect);
    svgRoot.mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
    return rect;
}

const LayoutSVGRoot& SVGLayoutSupport::mapRectToSVGRootForPaintInvalidation(const LayoutObject& object, const FloatRect& localPaintInvalidationRect, LayoutRect& rect)
{
    ASSERT(object.isSVG() && !object.isSVGRoot());

    FloatRect paintInvalidationRect = localPaintInvalidationRect;
    // FIXME: Building the transform to the SVG root border box and then doing
    // mapRect() with that would be slightly more efficient, but requires some
    // additions to AffineTransform (preMultiply, preTranslate) to avoid
    // excessive copying and to get a similar fast-path for translations.
    const LayoutObject* parent = &object;
    do {
        paintInvalidationRect = parent->localToParentTransform().mapRect(paintInvalidationRect);
        parent = parent->parent();
    } while (!parent->isSVGRoot());

    const LayoutSVGRoot& svgRoot = toLayoutSVGRoot(*parent);

    paintInvalidationRect = svgRoot.localToBorderBoxTransform().mapRect(paintInvalidationRect);
    rect = enclosingIntRectIfNotEmpty(paintInvalidationRect);
    return svgRoot;
}

void SVGLayoutSupport::mapLocalToContainer(const LayoutObject* object, const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, bool* wasFixed, const PaintInvalidationState* paintInvalidationState)
{
    transformState.applyTransform(object->localToParentTransform());

    if (paintInvalidationState && paintInvalidationState->canMapToContainer(paintInvalidationContainer)) {
        // |svgTransform| contains localToBorderBoxTransform mentioned below.
        transformState.applyTransform(paintInvalidationState->svgTransform());
        transformState.move(paintInvalidationState->paintOffset());
        return;
    }

    LayoutObject* parent = object->parent();

    // At the SVG/HTML boundary (aka LayoutSVGRoot), we apply the localToBorderBoxTransform
    // to map an element from SVG viewport coordinates to CSS box coordinates.
    // LayoutSVGRoot's mapLocalToContainer method expects CSS box coordinates.
    if (parent->isSVGRoot())
        transformState.applyTransform(toLayoutSVGRoot(parent)->localToBorderBoxTransform());

    MapCoordinatesFlags mode = UseTransforms;
    parent->mapLocalToContainer(paintInvalidationContainer, transformState, mode, wasFixed, paintInvalidationState);
}

const LayoutObject* SVGLayoutSupport::pushMappingToContainer(const LayoutObject* object, const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap& geometryMap)
{
    ASSERT_UNUSED(ancestorToStopAt, ancestorToStopAt != object);

    LayoutObject* parent = object->parent();

    // At the SVG/HTML boundary (aka LayoutSVGRoot), we apply the localToBorderBoxTransform
    // to map an element from SVG viewport coordinates to CSS box coordinates.
    // LayoutSVGRoot's mapLocalToContainer method expects CSS box coordinates.
    if (parent->isSVGRoot()) {
        TransformationMatrix matrix(object->localToParentTransform());
        matrix.multiply(toLayoutSVGRoot(parent)->localToBorderBoxTransform());
        geometryMap.push(object, matrix);
    } else {
        geometryMap.push(object, object->localToParentTransform());
    }

    return parent;
}

// Update a bounding box taking into account the validity of the other bounding box.
inline void SVGLayoutSupport::updateObjectBoundingBox(FloatRect& objectBoundingBox, bool& objectBoundingBoxValid, LayoutObject* other, FloatRect otherBoundingBox)
{
    bool otherValid = other->isSVGContainer() ? toLayoutSVGContainer(other)->isObjectBoundingBoxValid() : true;
    if (!otherValid)
        return;

    if (!objectBoundingBoxValid) {
        objectBoundingBox = otherBoundingBox;
        objectBoundingBoxValid = true;
        return;
    }

    objectBoundingBox.uniteEvenIfEmpty(otherBoundingBox);
}

void SVGLayoutSupport::computeContainerBoundingBoxes(const LayoutObject* container, FloatRect& objectBoundingBox, bool& objectBoundingBoxValid, FloatRect& strokeBoundingBox, FloatRect& paintInvalidationBoundingBox)
{
    objectBoundingBox = FloatRect();
    objectBoundingBoxValid = false;
    strokeBoundingBox = FloatRect();

    // When computing the strokeBoundingBox, we use the paintInvalidationRects of the container's children so that the container's stroke includes
    // the resources applied to the children (such as clips and filters). This allows filters applied to containers to correctly bound
    // the children, and also improves inlining of SVG content, as the stroke bound is used in that situation also.
    for (LayoutObject* current = container->slowFirstChild(); current; current = current->nextSibling()) {
        if (current->isSVGHiddenContainer())
            continue;

        // Don't include elements in the union that do not layout.
        if (current->isSVGShape() && toLayoutSVGShape(current)->isShapeEmpty())
            continue;

        const AffineTransform& transform = current->localToParentTransform();
        updateObjectBoundingBox(objectBoundingBox, objectBoundingBoxValid, current,
            transform.mapRect(current->objectBoundingBox()));
        strokeBoundingBox.unite(transform.mapRect(current->paintInvalidationRectInLocalCoordinates()));
    }

    paintInvalidationBoundingBox = strokeBoundingBox;
}

const LayoutSVGRoot* SVGLayoutSupport::findTreeRootObject(const LayoutObject* start)
{
    while (start && !start->isSVGRoot())
        start = start->parent();

    ASSERT(start);
    ASSERT(start->isSVGRoot());
    return toLayoutSVGRoot(start);
}

inline bool SVGLayoutSupport::layoutSizeOfNearestViewportChanged(const LayoutObject* start)
{
    while (start && !start->isSVGRoot() && !start->isSVGViewportContainer())
        start = start->parent();

    ASSERT(start);
    ASSERT(start->isSVGRoot() || start->isSVGViewportContainer());
    if (start->isSVGViewportContainer())
        return toLayoutSVGViewportContainer(start)->isLayoutSizeChanged();

    return toLayoutSVGRoot(start)->isLayoutSizeChanged();
}

bool SVGLayoutSupport::transformToRootChanged(LayoutObject* ancestor)
{
    while (ancestor && !ancestor->isSVGRoot()) {
        if (ancestor->isSVGTransformableContainer())
            return toLayoutSVGContainer(ancestor)->didTransformToRootUpdate();
        if (ancestor->isSVGViewportContainer())
            return toLayoutSVGViewportContainer(ancestor)->didTransformToRootUpdate();
        ancestor = ancestor->parent();
    }

    return false;
}

void SVGLayoutSupport::layoutChildren(LayoutObject* start, bool selfNeedsLayout)
{
    // When hasRelativeLengths() is false, no descendants have relative lengths
    // (hence no one is interested in viewport size changes).
    bool layoutSizeChanged = toSVGElement(start->node())->hasRelativeLengths()
        && layoutSizeOfNearestViewportChanged(start);
    bool transformChanged = transformToRootChanged(start);

    for (LayoutObject* child = start->slowFirstChild(); child; child = child->nextSibling()) {
        bool forceLayout = selfNeedsLayout;

        if (transformChanged) {
            // If the transform changed we need to update the text metrics (note: this also happens for layoutSizeChanged=true).
            if (child->isSVGText())
                toLayoutSVGText(child)->setNeedsTextMetricsUpdate();
            forceLayout = true;
        }

        if (layoutSizeChanged) {
            // When selfNeedsLayout is false and the layout size changed, we have to check whether this child uses relative lengths
            if (SVGElement* element = child->node()->isSVGElement() ? toSVGElement(child->node()) : 0) {
                if (element->hasRelativeLengths()) {
                    // FIXME: this should be done on invalidation, not during layout.
                    // When the layout size changed and when using relative values tell the LayoutSVGShape to update its shape object
                    if (child->isSVGShape()) {
                        toLayoutSVGShape(child)->setNeedsShapeUpdate();
                    } else if (child->isSVGText()) {
                        toLayoutSVGText(child)->setNeedsTextMetricsUpdate();
                        toLayoutSVGText(child)->setNeedsPositioningValuesUpdate();
                    }

                    forceLayout = true;
                }
            }
        }

        SubtreeLayoutScope layoutScope(*child);
        // Resource containers are nasty: they can invalidate clients outside the current SubtreeLayoutScope.
        // Since they only care about viewport size changes (to resolve their relative lengths), we trigger
        // their invalidation directly from SVGSVGElement::svgAttributeChange() or at a higher
        // SubtreeLayoutScope (in LayoutView::layout()).
        if (forceLayout && !child->isSVGResourceContainer())
            layoutScope.setNeedsLayout(child, LayoutInvalidationReason::SvgChanged);

        // Lay out any referenced resources before the child.
        layoutResourcesIfNeeded(child);
        child->layoutIfNeeded();
    }
}

void SVGLayoutSupport::layoutResourcesIfNeeded(const LayoutObject* object)
{
    ASSERT(object);

    SVGResources* resources = SVGResourcesCache::cachedResourcesForLayoutObject(object);
    if (resources)
        resources->layoutIfNeeded();
}

bool SVGLayoutSupport::isOverflowHidden(const LayoutObject* object)
{
    // LayoutSVGRoot should never query for overflow state - it should always clip itself to the initial viewport size.
    ASSERT(!object->isDocumentElement());

    return object->style()->overflowX() == OHIDDEN || object->style()->overflowX() == OSCROLL;
}

void SVGLayoutSupport::intersectPaintInvalidationRectWithResources(const LayoutObject* layoutObject, FloatRect& paintInvalidationRect)
{
    ASSERT(layoutObject);

    SVGResources* resources = SVGResourcesCache::cachedResourcesForLayoutObject(layoutObject);
    if (!resources)
        return;

    if (LayoutSVGResourceFilter* filter = resources->filter())
        paintInvalidationRect = filter->resourceBoundingBox(layoutObject);

    if (LayoutSVGResourceClipper* clipper = resources->clipper())
        paintInvalidationRect.intersect(clipper->resourceBoundingBox(layoutObject));

    if (LayoutSVGResourceMasker* masker = resources->masker())
        paintInvalidationRect.intersect(masker->resourceBoundingBox(layoutObject));
}

bool SVGLayoutSupport::filtersForceContainerLayout(LayoutObject* object)
{
    // If any of this container's children need to be laid out, and a filter is applied
    // to the container, we need to issue paint invalidations the entire container.
    if (!object->normalChildNeedsLayout())
        return false;

    SVGResources* resources = SVGResourcesCache::cachedResourcesForLayoutObject(object);
    if (!resources || !resources->filter())
        return false;

    return true;
}

bool SVGLayoutSupport::pointInClippingArea(LayoutObject* object, const FloatPoint& point)
{
    ASSERT(object);

    // We just take clippers into account to determine if a point is on the node. The Specification may
    // change later and we also need to check maskers.
    SVGResources* resources = SVGResourcesCache::cachedResourcesForLayoutObject(object);
    if (!resources)
        return true;

    if (LayoutSVGResourceClipper* clipper = resources->clipper())
        return clipper->hitTestClipContent(object->objectBoundingBox(), point);

    return true;
}

bool SVGLayoutSupport::transformToUserSpaceAndCheckClipping(LayoutObject* object, const AffineTransform& localTransform, const FloatPoint& pointInParent, FloatPoint& localPoint)
{
    if (!localTransform.isInvertible())
        return false;
    localPoint = localTransform.inverse().mapPoint(pointInParent);
    return pointInClippingArea(object, localPoint);
}

DashArray SVGLayoutSupport::resolveSVGDashArray(const SVGDashArray& svgDashArray, const ComputedStyle& style, const SVGLengthContext& lengthContext)
{
    DashArray dashArray;
    for (const Length& dashLength : svgDashArray.vector())
        dashArray.append(lengthContext.valueForLength(dashLength, style));
    return dashArray;
}

void SVGLayoutSupport::applyStrokeStyleToStrokeData(StrokeData& strokeData, const ComputedStyle& style, const LayoutObject& object)
{
    ASSERT(object.node());
    ASSERT(object.node()->isSVGElement());

    const SVGComputedStyle& svgStyle = style.svgStyle();

    SVGLengthContext lengthContext(toSVGElement(object.node()));
    strokeData.setThickness(lengthContext.valueForLength(svgStyle.strokeWidth()));
    strokeData.setLineCap(svgStyle.capStyle());
    strokeData.setLineJoin(svgStyle.joinStyle());
    strokeData.setMiterLimit(svgStyle.strokeMiterLimit());

    DashArray dashArray = resolveSVGDashArray(*svgStyle.strokeDashArray(), style, lengthContext);
    strokeData.setLineDash(dashArray, lengthContext.valueForLength(svgStyle.strokeDashOffset(), style));
}

bool SVGLayoutSupport::isLayoutableTextNode(const LayoutObject* object)
{
    ASSERT(object->isText());
    // <br> is marked as text, but is not handled by the SVG layout code-path.
    return object->isSVGInlineText() && !toLayoutSVGInlineText(object)->hasEmptyText();
}

bool SVGLayoutSupport::willIsolateBlendingDescendantsForStyle(const ComputedStyle& style)
{
    const SVGComputedStyle& svgStyle = style.svgStyle();

    return style.hasIsolation() || style.opacity() < 1 || style.hasBlendMode()
        || svgStyle.hasFilter() || svgStyle.hasMasker() || svgStyle.hasClipper();
}

bool SVGLayoutSupport::willIsolateBlendingDescendantsForObject(const LayoutObject* object)
{
    if (object->isSVGHiddenContainer())
        return false;
    if (!object->isSVGRoot() && !object->isSVGContainer())
        return false;
    return willIsolateBlendingDescendantsForStyle(object->styleRef());
}

bool SVGLayoutSupport::isIsolationRequired(const LayoutObject* object)
{
    return willIsolateBlendingDescendantsForObject(object) && object->hasNonIsolatedBlendingDescendants();
}

static AffineTransform& currentContentTransformation()
{
    DEFINE_STATIC_LOCAL(AffineTransform, s_currentContentTransformation, ());
    return s_currentContentTransformation;
}

SubtreeContentTransformScope::SubtreeContentTransformScope(const AffineTransform& subtreeContentTransformation)
{
    AffineTransform& contentTransformation = currentContentTransformation();
    m_savedContentTransformation = contentTransformation;
    contentTransformation = subtreeContentTransformation * contentTransformation;
}

SubtreeContentTransformScope::~SubtreeContentTransformScope()
{
    currentContentTransformation() = m_savedContentTransformation;
}

AffineTransform SVGLayoutSupport::deprecatedCalculateTransformToLayer(const LayoutObject* layoutObject)
{
    AffineTransform transform;
    while (layoutObject) {
        transform = layoutObject->localToParentTransform() * transform;
        if (layoutObject->isSVGRoot())
            break;
        layoutObject = layoutObject->parent();
    }

    // Continue walking up the layer tree, accumulating CSS transforms.
    // FIXME: this queries layer compositing state - which is not
    // supported during layout. Hence, the result may not include all CSS transforms.
    DeprecatedPaintLayer* layer = layoutObject ? layoutObject->enclosingLayer() : 0;
    while (layer && layer->isAllowedToQueryCompositingState()) {
        // We can stop at compositing layers, to match the backing resolution.
        // FIXME: should we be computing the transform to the nearest composited layer,
        // or the nearest composited layer that does not paint into its ancestor?
        // I think this is the nearest composited ancestor since we will inherit its
        // transforms in the composited layer tree.
        if (layer->compositingState() != NotComposited)
            break;

        if (TransformationMatrix* layerTransform = layer->transform())
            transform = layerTransform->toAffineTransform() * transform;

        layer = layer->parent();
    }

    return transform;
}

float SVGLayoutSupport::calculateScreenFontSizeScalingFactor(const LayoutObject* layoutObject)
{
    ASSERT(layoutObject);

    // FIXME: trying to compute a device space transform at record time is wrong. All clients
    // should be updated to avoid relying on this information, and the method should be removed.
    AffineTransform ctm = deprecatedCalculateTransformToLayer(layoutObject) * currentContentTransformation();
    ctm.scale(layoutObject->document().frameHost()->deviceScaleFactor());

    return narrowPrecisionToFloat(sqrt((pow(ctm.xScale(), 2) + pow(ctm.yScale(), 2)) / 2));
}

}
