/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2007, 2008, 2009 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Google, Inc.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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

#include "core/layout/svg/LayoutSVGRoot.h"

#include "core/frame/LocalFrame.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutView.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/SVGRootPainter.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/graphics/SVGImage.h"
#include "platform/LengthFunctions.h"

namespace blink {

LayoutSVGRoot::LayoutSVGRoot(SVGElement* node)
    : LayoutReplaced(node)
    , m_objectBoundingBoxValid(false)
    , m_isLayoutSizeChanged(false)
    , m_needsBoundariesOrTransformUpdate(true)
    , m_hasBoxDecorationBackground(false)
    , m_hasNonIsolatedBlendingDescendants(false)
    , m_hasNonIsolatedBlendingDescendantsDirty(false)
{
}

LayoutSVGRoot::~LayoutSVGRoot()
{
}

void LayoutSVGRoot::computeIntrinsicRatioInformation(FloatSize& intrinsicSize, double& intrinsicRatio) const
{
    // Spec: http://www.w3.org/TR/SVG/coords.html#IntrinsicSizing
    // SVG needs to specify how to calculate some intrinsic sizing properties to enable inclusion within other languages.
    // The intrinsic width and height of the viewport of SVG content must be determined from the 'width' and 'height' attributes.
    SVGSVGElement* svg = toSVGSVGElement(node());
    ASSERT(svg);

    // The intrinsic aspect ratio of the viewport of SVG content is necessary for example, when including SVG from an 'object'
    // element in HTML styled with CSS. It is possible (indeed, common) for an SVG graphic to have an intrinsic aspect ratio but
    // not to have an intrinsic width or height. The intrinsic aspect ratio must be calculated based upon the following rules:
    // - The aspect ratio is calculated by dividing a width by a height.
    // - If the 'width' and 'height' of the rootmost 'svg' element are both specified with unit identifiers (in, mm, cm, pt, pc,
    //   px, em, ex) or in user units, then the aspect ratio is calculated from the 'width' and 'height' attributes after
    //   resolving both values to user units.
    intrinsicSize.setWidth(floatValueForLength(svg->intrinsicWidth(), 0));
    intrinsicSize.setHeight(floatValueForLength(svg->intrinsicHeight(), 0));

    if (!isHorizontalWritingMode())
        intrinsicSize = intrinsicSize.transposedSize();

    if (!intrinsicSize.isEmpty()) {
        intrinsicRatio = intrinsicSize.width() / static_cast<double>(intrinsicSize.height());
    } else {
        // - If either/both of the 'width' and 'height' of the rootmost 'svg' element are in percentage units (or omitted), the
        //   aspect ratio is calculated from the width and height values of the 'viewBox' specified for the current SVG document
        //   fragment. If the 'viewBox' is not correctly specified, or set to 'none', the intrinsic aspect ratio cannot be
        //   calculated and is considered unspecified.
        FloatSize viewBoxSize = svg->viewBox()->currentValue()->value().size();
        if (!viewBoxSize.isEmpty()) {
            // The viewBox can only yield an intrinsic ratio, not an intrinsic size.
            intrinsicRatio = viewBoxSize.width() / static_cast<double>(viewBoxSize.height());
            if (!isHorizontalWritingMode())
                intrinsicRatio = 1 / intrinsicRatio;
        }
    }
}

bool LayoutSVGRoot::isEmbeddedThroughSVGImage() const
{
    return SVGImage::isInSVGImage(toSVGSVGElement(node()));
}

bool LayoutSVGRoot::isEmbeddedThroughFrameContainingSVGDocument() const
{
    if (!node())
        return false;

    LocalFrame* frame = node()->document().frame();
    if (!frame)
        return false;

    // If our frame has an owner layoutObject, we're embedded through eg. object/embed/iframe,
    // but we only negotiate if we're in an SVG document inside a embedded object (object/embed).
    if (!frame->ownerLayoutObject() || !frame->ownerLayoutObject()->isEmbeddedObject())
        return false;
    return frame->document()->isSVGDocument();
}

LayoutUnit LayoutSVGRoot::computeReplacedLogicalWidth(ShouldComputePreferred shouldComputePreferred) const
{
    // When we're embedded through SVGImage (border-image/background-image/<html:img>/...) we're forced to resize to a specific size.
    if (!m_containerSize.isEmpty())
        return m_containerSize.width();

    if (isEmbeddedThroughFrameContainingSVGDocument())
        return containingBlock()->availableLogicalWidth();

    if (style()->logicalWidth().isSpecified() || style()->logicalMaxWidth().isSpecified())
        return LayoutReplaced::computeReplacedLogicalWidth(shouldComputePreferred);

    // SVG embedded via SVGImage (background-image/border-image/etc) / Inline SVG.
    return LayoutReplaced::computeReplacedLogicalWidth(shouldComputePreferred);
}

LayoutUnit LayoutSVGRoot::computeReplacedLogicalHeight() const
{
    // When we're embedded through SVGImage (border-image/background-image/<html:img>/...) we're forced to resize to a specific size.
    if (!m_containerSize.isEmpty())
        return m_containerSize.height();

    if (isEmbeddedThroughFrameContainingSVGDocument())
        return containingBlock()->availableLogicalHeight(IncludeMarginBorderPadding);

    if (style()->logicalHeight().isSpecified() || style()->logicalMaxHeight().isSpecified())
        return LayoutReplaced::computeReplacedLogicalHeight();

    // SVG embedded via SVGImage (background-image/border-image/etc) / Inline SVG.
    return LayoutReplaced::computeReplacedLogicalHeight();
}

void LayoutSVGRoot::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);

    bool needsLayout = selfNeedsLayout();

    LayoutSize oldSize = size();
    updateLogicalWidth();
    updateLogicalHeight();
    buildLocalToBorderBoxTransform();

    SVGLayoutSupport::layoutResourcesIfNeeded(this);

    SVGSVGElement* svg = toSVGSVGElement(node());
    ASSERT(svg);
    m_isLayoutSizeChanged = needsLayout || (svg->hasRelativeLengths() && oldSize != size());
    SVGLayoutSupport::layoutChildren(this, needsLayout || SVGLayoutSupport::filtersForceContainerLayout(this));

    if (m_needsBoundariesOrTransformUpdate) {
        updateCachedBoundaries();
        m_needsBoundariesOrTransformUpdate = false;
    }

    m_overflow.clear();
    addVisualEffectOverflow();

    if (!shouldApplyViewportClip()) {
        FloatRect contentPaintInvalidationRect = paintInvalidationRectInLocalCoordinates();
        contentPaintInvalidationRect = m_localToBorderBoxTransform.mapRect(contentPaintInvalidationRect);
        addVisualOverflow(enclosingLayoutRect(contentPaintInvalidationRect));
    }

    updateLayerTransformAfterLayout();
    m_hasBoxDecorationBackground = isDocumentElement() ? calculateHasBoxDecorations() : hasBoxDecorationBackground();
    invalidateBackgroundObscurationStatus();

    clearNeedsLayout();
}

bool LayoutSVGRoot::shouldApplyViewportClip() const
{
    // the outermost svg is clipped if auto, and svg document roots are always clipped
    // When the svg is stand-alone (isDocumentElement() == true) the viewport clipping should always
    // be applied, noting that the window scrollbars should be hidden if overflow=hidden.
    return style()->overflowX() == OHIDDEN
        || style()->overflowX() == OAUTO
        || style()->overflowX() == OSCROLL
        || this->isDocumentElement();
}

void LayoutSVGRoot::paintReplaced(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    SVGRootPainter(*this).paint(paintInfo, paintOffset);
}

void LayoutSVGRoot::willBeDestroyed()
{
    LayoutBlock::removePercentHeightDescendant(const_cast<LayoutSVGRoot*>(this));

    SVGResourcesCache::clientDestroyed(this);
    LayoutReplaced::willBeDestroyed();
}

void LayoutSVGRoot::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    if (diff.needsFullLayout())
        setNeedsBoundariesUpdate();
    if (diff.needsPaintInvalidation()) {
        // Box decorations may have appeared/disappeared - recompute status.
        m_hasBoxDecorationBackground = calculateHasBoxDecorations();
    }

    LayoutReplaced::styleDidChange(diff, oldStyle);
    SVGResourcesCache::clientStyleChanged(this, diff, styleRef());
}

bool LayoutSVGRoot::isChildAllowed(LayoutObject* child, const ComputedStyle&) const
{
    return child->isSVG() && !(child->isSVGInline() || child->isSVGInlineText() || child->isSVGGradientStop());
}

void LayoutSVGRoot::addChild(LayoutObject* child, LayoutObject* beforeChild)
{
    LayoutReplaced::addChild(child, beforeChild);
    SVGResourcesCache::clientWasAddedToTree(child, child->styleRef());

    bool shouldIsolateDescendants = (child->isBlendingAllowed() && child->style()->hasBlendMode()) || child->hasNonIsolatedBlendingDescendants();
    if (shouldIsolateDescendants)
        descendantIsolationRequirementsChanged(DescendantIsolationRequired);
}

void LayoutSVGRoot::removeChild(LayoutObject* child)
{
    SVGResourcesCache::clientWillBeRemovedFromTree(child);
    LayoutReplaced::removeChild(child);

    bool hadNonIsolatedDescendants = (child->isBlendingAllowed() && child->style()->hasBlendMode()) || child->hasNonIsolatedBlendingDescendants();
    if (hadNonIsolatedDescendants)
        descendantIsolationRequirementsChanged(DescendantIsolationNeedsUpdate);
}

bool LayoutSVGRoot::hasNonIsolatedBlendingDescendants() const
{
    if (m_hasNonIsolatedBlendingDescendantsDirty) {
        m_hasNonIsolatedBlendingDescendants = SVGLayoutSupport::computeHasNonIsolatedBlendingDescendants(this);
        m_hasNonIsolatedBlendingDescendantsDirty = false;
    }
    return m_hasNonIsolatedBlendingDescendants;
}

void LayoutSVGRoot::descendantIsolationRequirementsChanged(DescendantIsolationState state)
{
    switch (state) {
    case DescendantIsolationRequired:
        m_hasNonIsolatedBlendingDescendants = true;
        m_hasNonIsolatedBlendingDescendantsDirty = false;
        break;
    case DescendantIsolationNeedsUpdate:
        m_hasNonIsolatedBlendingDescendantsDirty = true;
        break;
    }
}

void LayoutSVGRoot::insertedIntoTree()
{
    LayoutReplaced::insertedIntoTree();
    SVGResourcesCache::clientWasAddedToTree(this, styleRef());
}

void LayoutSVGRoot::willBeRemovedFromTree()
{
    SVGResourcesCache::clientWillBeRemovedFromTree(this);
    LayoutReplaced::willBeRemovedFromTree();
}

// LayoutBox methods will expect coordinates w/o any transforms in coordinates
// relative to our borderBox origin.  This method gives us exactly that.
void LayoutSVGRoot::buildLocalToBorderBoxTransform()
{
    SVGSVGElement* svg = toSVGSVGElement(node());
    ASSERT(svg);
    float scale = style()->effectiveZoom();
    FloatPoint translate = svg->currentTranslate();
    LayoutSize borderAndPadding(borderLeft() + paddingLeft(), borderTop() + paddingTop());
    m_localToBorderBoxTransform = svg->viewBoxToViewTransform(contentWidth() / scale, contentHeight() / scale);
    AffineTransform viewToBorderBoxTransform(scale, 0, 0, scale, borderAndPadding.width() + translate.x(), borderAndPadding.height() + translate.y());
    if (viewToBorderBoxTransform.isIdentity())
        return;
    m_localToBorderBoxTransform = viewToBorderBoxTransform * m_localToBorderBoxTransform;
}

const AffineTransform& LayoutSVGRoot::localToParentTransform() const
{
    // Slightly optimized version of m_localToParentTransform = AffineTransform::translation(x(), y()) * m_localToBorderBoxTransform;
    m_localToParentTransform = m_localToBorderBoxTransform;
    if (location().x())
        m_localToParentTransform.setE(m_localToParentTransform.e() + roundToInt(location().x()));
    if (location().y())
        m_localToParentTransform.setF(m_localToParentTransform.f() + roundToInt(location().y()));
    return m_localToParentTransform;
}

LayoutRect LayoutSVGRoot::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    // This is an open-coded aggregate of SVGLayoutSupport::clippedOverflowRectForPaintInvalidation,
    // LayoutSVGRoot::mapRectToPaintInvalidationBacking and LayoutReplaced::clippedOverflowRectForPaintInvalidation.
    // The reason for this is to optimize/minimize the paint invalidation rect when the box is not "decorated"
    // (does not have background/border/etc.)

    // Return early for any cases where we don't actually paint.
    if (style()->visibility() != VISIBLE && !enclosingLayer()->hasVisibleContent())
        return LayoutRect();

    // Compute the paint invalidation rect of the content of the SVG in the border-box coordinate space.
    FloatRect contentPaintInvalidationRect = paintInvalidationRectInLocalCoordinates();
    contentPaintInvalidationRect = m_localToBorderBoxTransform.mapRect(contentPaintInvalidationRect);

    // Apply initial viewport clip, overflow:visible content is added to visualOverflow
    // but the most common case is that overflow is hidden, so always intersect.
    contentPaintInvalidationRect.intersect(pixelSnappedBorderBoxRect());

    LayoutRect paintInvalidationRect = enclosingLayoutRect(contentPaintInvalidationRect);
    // If the box is decorated or is overflowing, extend it to include the border-box and overflow.
    if (m_hasBoxDecorationBackground || hasOverflowModel()) {
        // The selectionRect can project outside of the overflowRect, so take their union
        // for paint invalidation to avoid selection painting glitches.
        LayoutRect decoratedPaintInvalidationRect = unionRect(localSelectionRect(false), visualOverflowRect());
        paintInvalidationRect.unite(decoratedPaintInvalidationRect);
    }

    // Compute the paint invalidation rect in the parent coordinate space.
    LayoutRect rect(enclosingIntRect(paintInvalidationRect));
    LayoutReplaced::mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
    return rect;
}

void LayoutSVGRoot::mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, const PaintInvalidationState* paintInvalidationState) const
{
    // Note that we don't apply the border-box transform here - it's assumed
    // that whoever called us has done that already.

    // Apply initial viewport clip
    if (shouldApplyViewportClip())
        rect.intersect(LayoutRect(pixelSnappedBorderBoxRect()));

    LayoutReplaced::mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
}

// This method expects local CSS box coordinates.
// Callers with local SVG viewport coordinates should first apply the localToBorderBoxTransform
// to convert from SVG viewport coordinates to local CSS box coordinates.
void LayoutSVGRoot::mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, MapCoordinatesFlags mode, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    ASSERT(mode & ~IsFixed); // We should have no fixed content in the SVG layout tree.
    // We used to have this ASSERT here, but we removed it when enabling layer squashing.
    // See http://crbug.com/364901
    // ASSERT(mode & UseTransforms); // mapping a point through SVG w/o respecting trasnforms is useless.

    LayoutReplaced::mapLocalToContainer(paintInvalidationContainer, transformState, mode | ApplyContainerFlip, wasFixed, paintInvalidationState);
}

const LayoutObject* LayoutSVGRoot::pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap& geometryMap) const
{
    return LayoutReplaced::pushMappingToContainer(ancestorToStopAt, geometryMap);
}

void LayoutSVGRoot::updateCachedBoundaries()
{
    SVGLayoutSupport::computeContainerBoundingBoxes(this, m_objectBoundingBox, m_objectBoundingBoxValid, m_strokeBoundingBox, m_paintInvalidationBoundingBox);
    SVGLayoutSupport::intersectPaintInvalidationRectWithResources(this, m_paintInvalidationBoundingBox);
}

bool LayoutSVGRoot::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction)
{
    LayoutPoint pointInParent = locationInContainer.point() - toLayoutSize(accumulatedOffset);
    LayoutPoint pointInBorderBox = pointInParent - toLayoutSize(location());

    // Only test SVG content if the point is in our content box, or in case we
    // don't clip to the viewport, the visual overflow rect.
    // FIXME: This should be an intersection when rect-based hit tests are supported by nodeAtFloatPoint.
    if (contentBoxRect().contains(pointInBorderBox) || (!shouldApplyViewportClip() && visualOverflowRect().contains(pointInBorderBox))) {
        const AffineTransform& localToParentTransform = this->localToParentTransform();
        if (localToParentTransform.isInvertible()) {
            FloatPoint localPoint = localToParentTransform.inverse().mapPoint(FloatPoint(pointInParent));

            for (LayoutObject* child = lastChild(); child; child = child->previousSibling()) {
                // FIXME: nodeAtFloatPoint() doesn't handle rect-based hit tests yet.
                if (child->nodeAtFloatPoint(result, localPoint, hitTestAction)) {
                    updateHitTestResult(result, pointInBorderBox);
                    if (!result.addNodeToListBasedTestResult(child->node(), locationInContainer))
                        return true;
                }
            }
        }
    }

    // If we didn't early exit above, we've just hit the container <svg> element. Unlike SVG 1.1, 2nd Edition allows container elements to be hit.
    if ((hitTestAction == HitTestBlockBackground || hitTestAction == HitTestChildBlockBackground) && visibleToHitTestRequest(result.hitTestRequest())) {
        // Only return true here, if the last hit testing phase 'BlockBackground' (or 'ChildBlockBackground' - depending on context) is executed.
        // If we'd return true in the 'Foreground' phase, hit testing would stop immediately. For SVG only trees this doesn't matter.
        // Though when we have a <foreignObject> subtree we need to be able to detect hits on the background of a <div> element.
        // If we'd return true here in the 'Foreground' phase, we are not able to detect these hits anymore.
        LayoutRect boundsRect(accumulatedOffset + location(), size());
        if (locationInContainer.intersects(boundsRect)) {
            updateHitTestResult(result, pointInBorderBox);
            if (!result.addNodeToListBasedTestResult(node(), locationInContainer, boundsRect))
                return true;
        }
    }

    return false;
}

}
