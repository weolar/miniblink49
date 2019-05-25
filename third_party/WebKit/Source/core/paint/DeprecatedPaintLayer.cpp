/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc. All rights reserved.
 *
 * Portions are Copyright (C) 1998 Netscape Communications Corporation.
 *
 * Other contributors:
 *   Robert O'Callahan <roc+@cs.cmu.edu>
 *   David Baron <dbaron@fas.harvard.edu>
 *   Christian Biesinger <cbiesinger@web.de>
 *   Randall Jesup <rjesup@wgate.com>
 *   Roland Mainz <roland.mainz@informatik.med.uni-giessen.de>
 *   Josh Soref <timeless@mac.com>
 *   Boris Zbarsky <bzbarsky@mit.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Alternatively, the contents of this file may be used under the terms
 * of either the Mozilla Public License Version 1.1, found at
 * http://www.mozilla.org/MPL/ (the "MPL") or the GNU General Public
 * License Version 2.0, found at http://www.fsf.org/copyleft/gpl.html
 * (the "GPL"), in which case the provisions of the MPL or the GPL are
 * applicable instead of those above.  If you wish to allow use of your
 * version of this file only under the terms of one of those two
 * licenses (the MPL or the GPL) and not to allow others to use your
 * version of this file under the LGPL, indicate your decision by
 * deletingthe provisions above and replace them with the notice and
 * other provisions required by the MPL or the GPL, as the case may be.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under any of the LGPL, the MPL or the GPL.
 */

#include "config.h"
#include "core/paint/DeprecatedPaintLayer.h"

#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/css/PseudoStyleRequest.h"
#include "core/dom/Document.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/frame/DeprecatedScheduleStyleRecalcDuringLayout.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFrameElement.h"
#include "core/layout/HitTestRequest.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/HitTestingTransformState.h"
#include "core/layout/LayoutFlowThread.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutReplica.h"
#include "core/layout/LayoutScrollbar.h"
#include "core/layout/LayoutScrollbarPart.h"
#include "core/layout/LayoutTreeAsText.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/layout/svg/ReferenceFilterBuilder.h"
#include "core/page/Page.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "core/paint/FilterEffectBuilder.h"
#include "platform/LengthFunctions.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/geometry/FloatPoint3D.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/TransformState.h"
#include "platform/graphics/filters/ReferenceFilter.h"
#include "platform/graphics/filters/SourceGraphic.h"
#include "platform/transforms/ScaleTransformOperation.h"
#include "platform/transforms/TransformationMatrix.h"
#include "platform/transforms/TranslateTransformOperation.h"
#include "public/platform/Platform.h"
#include "wtf/Partitions.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/CString.h"

namespace blink {

namespace {

static CompositingQueryMode gCompositingQueryMode =
    CompositingQueriesAreOnlyAllowedInCertainDocumentLifecyclePhases;

} // namespace

using namespace HTMLNames;

DeprecatedPaintLayer::DeprecatedPaintLayer(LayoutBoxModelObject* layoutObject, DeprecatedPaintLayerType type)
    : m_layerType(type)
    , m_hasSelfPaintingLayerDescendant(false)
    , m_hasSelfPaintingLayerDescendantDirty(false)
    , m_isRootLayer(layoutObject->isLayoutView())
    , m_visibleContentStatusDirty(true)
    , m_hasVisibleContent(false)
    , m_visibleDescendantStatusDirty(false)
    , m_hasVisibleDescendant(false)
    , m_hasVisibleNonLayerContent(false)
#if ENABLE(ASSERT)
    , m_needsPositionUpdate(true)
#endif
    , m_3DTransformedDescendantStatusDirty(true)
    , m_has3DTransformedDescendant(false)
    , m_containsDirtyOverlayScrollbars(false)
    , m_hasFilterInfo(false)
    , m_needsAncestorDependentCompositingInputsUpdate(true)
    , m_needsDescendantDependentCompositingInputsUpdate(true)
    , m_childNeedsCompositingInputsUpdate(true)
    , m_hasCompositingDescendant(false)
    , m_hasNonCompositedChild(false)
    , m_shouldIsolateCompositedDescendants(false)
    , m_lostGroupedMapping(false)
    , m_layoutObject(layoutObject)
    , m_parent(0)
    , m_previous(0)
    , m_next(0)
    , m_first(0)
    , m_last(0)
    , m_staticInlinePosition(0)
    , m_staticBlockPosition(0)
    , m_enclosingPaginationLayer(0)
    , m_potentialCompositingReasonsFromStyle(CompositingReasonNone)
    , m_compositingReasons(CompositingReasonNone)
    , m_groupedMapping(0)
    , m_clipper(*layoutObject)
{
    updateStackingNode();

    m_isSelfPaintingLayer = shouldBeSelfPaintingLayer();

    if (!layoutObject->slowFirstChild() && layoutObject->style()) {
        m_visibleContentStatusDirty = false;
        m_hasVisibleContent = layoutObject->style()->visibility() == VISIBLE;
    }

    updateScrollableArea();
}

DeprecatedPaintLayer::~DeprecatedPaintLayer()
{
    if (layoutObject()->frame() && layoutObject()->frame()->page()) {
        if (ScrollingCoordinator* scrollingCoordinator = layoutObject()->frame()->page()->scrollingCoordinator())
            scrollingCoordinator->willDestroyLayer(this);
    }

    removeFilterInfoIfNeeded();

    if (groupedMapping()) {
        DisableCompositingQueryAsserts disabler;
        setGroupedMapping(0, InvalidateLayerAndRemoveFromMapping);
    }

    // Child layers will be deleted by their corresponding layout objects, so
    // we don't need to delete them ourselves.

    clearCompositedDeprecatedPaintLayerMapping(true);

    if (m_reflectionInfo)
        m_reflectionInfo->destroy();

    if (m_scrollableArea)
        m_scrollableArea->dispose();
}

String DeprecatedPaintLayer::debugName() const
{
    if (isReflection())
        return layoutObject()->parent()->debugName() + " (reflection)";
    return layoutObject()->debugName();
}

DeprecatedPaintLayerCompositor* DeprecatedPaintLayer::compositor() const
{
    if (!layoutObject()->view())
        return 0;
    return layoutObject()->view()->compositor();
}

void DeprecatedPaintLayer::contentChanged(ContentChangeType changeType)
{
    // updateLayerCompositingState will query compositingReasons for accelerated overflow scrolling.
    // This is tripped by LayoutTests/compositing/content-changed-chicken-egg.html
    DisableCompositingQueryAsserts disabler;

    if (changeType == CanvasChanged)
        compositor()->setNeedsCompositingUpdate(CompositingUpdateAfterCompositingInputChange);

    if (changeType == CanvasContextChanged) {
        compositor()->setNeedsCompositingUpdate(CompositingUpdateAfterCompositingInputChange);

        // Although we're missing test coverage, we need to call
        // GraphicsLayer::setContentsToPlatformLayer with the new platform
        // layer for this canvas.
        // See http://crbug.com/349195
        if (hasCompositedDeprecatedPaintLayerMapping())
            compositedDeprecatedPaintLayerMapping()->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateSubtree);
    }

    if (m_compositedDeprecatedPaintLayerMapping)
        m_compositedDeprecatedPaintLayerMapping->contentChanged(changeType);
}

bool DeprecatedPaintLayer::paintsWithFilters() const
{
    if (!layoutObject()->hasFilter())
        return false;

    // https://code.google.com/p/chromium/issues/detail?id=343759
    DisableCompositingQueryAsserts disabler;
    return !m_compositedDeprecatedPaintLayerMapping || compositingState() != PaintsIntoOwnBacking;
}

LayoutSize DeprecatedPaintLayer::subpixelAccumulation() const
{
    return m_subpixelAccumulation;
}

void DeprecatedPaintLayer::setSubpixelAccumulation(const LayoutSize& size)
{
    m_subpixelAccumulation = size;
}

void DeprecatedPaintLayer::updateLayerPositionsAfterLayout()
{
    TRACE_EVENT0("blink,benchmark", "DeprecatedPaintLayer::updateLayerPositionsAfterLayout");

    m_clipper.clearClipRectsIncludingDescendants();
    updateLayerPositionRecursive();

    {
        // FIXME: Remove incremental compositing updates after fixing the chicken/egg issues
        // https://code.google.com/p/chromium/issues/detail?id=343756
        DisableCompositingQueryAsserts disabler;
        updatePaginationRecursive(enclosingPaginationLayer());
    }
}

void DeprecatedPaintLayer::updateLayerPositionRecursive()
{
    updateLayerPosition();

    if (m_reflectionInfo)
        m_reflectionInfo->reflection()->layout();

    // FIXME(400589): We would like to do this in DeprecatedPaintLayerScrollableArea::updateAfterLayout,
    // but it depends on the size computed by updateLayerPosition.
    if (m_scrollableArea) {
        if (ScrollAnimator* scrollAnimator = m_scrollableArea->existingScrollAnimator())
            scrollAnimator->updateAfterLayout();
    }

    // FIXME: We should be able to remove this call because we don't care about
    // any descendant-dependent flags, but code somewhere else is reading these
    // flags and depending on us to update them.
    updateDescendantDependentFlags();

    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling())
        child->updateLayerPositionRecursive();
}

void DeprecatedPaintLayer::updateHasSelfPaintingLayerDescendant() const
{
    ASSERT(m_hasSelfPaintingLayerDescendantDirty);

    m_hasSelfPaintingLayerDescendant = false;

    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling()) {
        if (child->isSelfPaintingLayer() || child->hasSelfPaintingLayerDescendant()) {
            m_hasSelfPaintingLayerDescendant = true;
            break;
        }
    }

    m_hasSelfPaintingLayerDescendantDirty = false;
}

void DeprecatedPaintLayer::dirtyAncestorChainHasSelfPaintingLayerDescendantStatus()
{
    for (DeprecatedPaintLayer* layer = this; layer; layer = layer->parent()) {
        layer->m_hasSelfPaintingLayerDescendantDirty = true;
        // If we have reached a self-painting layer, we know our parent should have a self-painting descendant
        // in this case, there is no need to dirty our ancestors further.
        if (layer->isSelfPaintingLayer()) {
            ASSERT(!parent() || parent()->m_hasSelfPaintingLayerDescendantDirty || parent()->m_hasSelfPaintingLayerDescendant);
            break;
        }
    }
}

bool DeprecatedPaintLayer::scrollsWithViewport() const
{
    return layoutObject()->style()->position() == FixedPosition && layoutObject()->containerForFixedPosition() == layoutObject()->view();
}

bool DeprecatedPaintLayer::scrollsWithRespectTo(const DeprecatedPaintLayer* other) const
{
    if (scrollsWithViewport() != other->scrollsWithViewport())
        return true;
    return ancestorScrollingLayer() != other->ancestorScrollingLayer();
}

void DeprecatedPaintLayer::updateLayerPositionsAfterOverflowScroll(const DoubleSize& scrollDelta)
{
    m_clipper.clearClipRectsIncludingDescendants();
    updateLayerPositionsAfterScrollRecursive(scrollDelta, isPaintInvalidationContainer());
}

void DeprecatedPaintLayer::updateLayerPositionsAfterScrollRecursive(const DoubleSize& scrollDelta, bool paintInvalidationContainerWasScrolled)
{
    updateLayerPosition();
    if (paintInvalidationContainerWasScrolled && !isPaintInvalidationContainer()) {
        // Paint invalidation rects are in the coordinate space of the paint invalidation container.
        // If it has scrolled, the rect must be adjusted. Note that it is not safe to reset it to
        // the current bounds rect, as the LayoutObject may have moved since the last invalidation.
        // FIXME(416535): Ideally, pending invalidations of scrolling content should be stored in
        // the coordinate space of the scrolling content layer, so that they need no adjustment.
        LayoutRect invalidationRect = m_layoutObject->previousPaintInvalidationRect();
        invalidationRect.move(LayoutSize(scrollDelta));
        m_layoutObject->setPreviousPaintInvalidationRect(invalidationRect);
    }
    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling()) {
        child->updateLayerPositionsAfterScrollRecursive(scrollDelta,
            paintInvalidationContainerWasScrolled && !child->isPaintInvalidationContainer());
    }
}

void DeprecatedPaintLayer::updateTransformationMatrix()
{
    if (m_transform) {
        LayoutBox* box = layoutBox();
        ASSERT(box);
        m_transform->makeIdentity();
        box->style()->applyTransform(*m_transform, LayoutSize(box->pixelSnappedSize()), ComputedStyle::IncludeTransformOrigin, ComputedStyle::IncludeMotionPath, ComputedStyle::IncludeIndependentTransformProperties);
        makeMatrixRenderable(*m_transform, compositor()->hasAcceleratedCompositing());
    }
}

void DeprecatedPaintLayer::updateTransform(const ComputedStyle* oldStyle, const ComputedStyle& newStyle)
{
    if (oldStyle && newStyle.transformDataEquivalent(*oldStyle))
        return;

    // hasTransform() on the layoutObject is also true when there is transform-style: preserve-3d or perspective set,
    // so check style too.
    bool hasTransform = layoutObject()->hasTransformRelatedProperty() && newStyle.hasTransform();
    bool had3DTransform = has3DTransform();

    bool hadTransform = m_transform;
    if (hasTransform != hadTransform) {
        if (hasTransform)
            m_transform = adoptPtr(new TransformationMatrix);
        else
            m_transform.clear();

        // DeprecatedPaintLayers with transforms act as clip rects roots, so clear the cached clip rects here.
        m_clipper.clearClipRectsIncludingDescendants();
    } else if (hasTransform) {
        m_clipper.clearClipRectsIncludingDescendants(AbsoluteClipRects);
    }

    updateTransformationMatrix();

    if (had3DTransform != has3DTransform())
        dirty3DTransformedDescendantStatus();
}

static DeprecatedPaintLayer* enclosingLayerForContainingBlock(DeprecatedPaintLayer* layer)
{
    if (LayoutObject* containingBlock = layer->layoutObject()->containingBlock())
        return containingBlock->enclosingLayer();
    return 0;
}

DeprecatedPaintLayer* DeprecatedPaintLayer::renderingContextRoot()
{
    DeprecatedPaintLayer* renderingContext = 0;

    if (shouldPreserve3D())
        renderingContext = this;

    for (DeprecatedPaintLayer* current = enclosingLayerForContainingBlock(this); current && current->shouldPreserve3D(); current = enclosingLayerForContainingBlock(current))
        renderingContext = current;

    return renderingContext;
}

TransformationMatrix DeprecatedPaintLayer::currentTransform() const
{
    if (!m_transform)
        return TransformationMatrix();
    return *m_transform;
}

TransformationMatrix DeprecatedPaintLayer::renderableTransform(PaintBehavior paintBehavior) const
{
    if (!m_transform)
        return TransformationMatrix();

    if (paintBehavior & PaintBehaviorFlattenCompositingLayers) {
        TransformationMatrix matrix = *m_transform;
        makeMatrixRenderable(matrix, false /* flatten 3d */);
        return matrix;
    }

    return *m_transform;
}

// Convert a bounding box from flow thread coordinates, relative to |layer|, to visual coordinates, relative to |ancestorLayer|.
// See http://www.chromium.org/developers/design-documents/multi-column-layout for more info on these coordinate types.
static void convertFromFlowThreadToVisualBoundingBoxInAncestor(const DeprecatedPaintLayer* layer, const DeprecatedPaintLayer* ancestorLayer, LayoutRect& rect)
{
    DeprecatedPaintLayer* paginationLayer = layer->enclosingPaginationLayer();
    ASSERT(paginationLayer);
    LayoutFlowThread* flowThread = toLayoutFlowThread(paginationLayer->layoutObject());

    // First make the flow thread rectangle relative to the flow thread, not to |layer|.
    LayoutPoint offsetWithinPaginationLayer;
    layer->convertToLayerCoords(paginationLayer, offsetWithinPaginationLayer);
    rect.moveBy(offsetWithinPaginationLayer);

    // Then make the rectangle visual, relative to the fragmentation context. Split our box up into
    // the actual fragment boxes that layout in the columns/pages and unite those together to get
    // our true bounding box.
    rect = flowThread->fragmentsBoundingBox(rect);

    // Finally, make the visual rectangle relative to |ancestorLayer|.
    if (ancestorLayer->enclosingPaginationLayer() != paginationLayer) {
        rect.moveBy(paginationLayer->visualOffsetFromAncestor(ancestorLayer));
        return;
    }
    // The ancestor layer is inside the same pagination layer as |layer|, so we need to subtract
    // the visual distance from the ancestor layer to the pagination layer.
    rect.moveBy(-ancestorLayer->visualOffsetFromAncestor(paginationLayer));
}

void DeprecatedPaintLayer::updatePaginationRecursive(bool needsPaginationUpdate)
{
    m_enclosingPaginationLayer = 0;

    if (layoutObject()->isLayoutFlowThread())
        needsPaginationUpdate = true;

    if (needsPaginationUpdate) {
        // Each paginated layer has to paint on its own. There is no recurring into child layers. Each
        // layer has to be checked individually and genuinely know if it is going to have to split
        // itself up when painting only its contents (and not any other descendant layers). We track an
        // enclosingPaginationLayer instead of using a simple bit, since we want to be able to get back
        // to that layer easily.
        if (LayoutFlowThread* containingFlowThread = layoutObject()->flowThreadContainingBlock())
            m_enclosingPaginationLayer = containingFlowThread->layer();
    }

    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling())
        child->updatePaginationRecursive(needsPaginationUpdate);
}

void DeprecatedPaintLayer::clearPaginationRecursive()
{
    m_enclosingPaginationLayer = 0;
    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling())
        child->clearPaginationRecursive();
}

LayoutPoint DeprecatedPaintLayer::positionFromPaintInvalidationBacking(const LayoutObject* layoutObject, const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState)
{
    FloatPoint point = layoutObject->localToContainerPoint(FloatPoint(), paintInvalidationContainer, 0, 0, paintInvalidationState);

    // FIXME: Eventually we are going to unify coordinates in GraphicsLayer space.
    if (paintInvalidationContainer && paintInvalidationContainer->layer()->groupedMapping())
        mapPointToPaintBackingCoordinates(paintInvalidationContainer, point);

    return LayoutPoint(point);
}

void DeprecatedPaintLayer::mapPointToPaintBackingCoordinates(const LayoutBoxModelObject* paintInvalidationContainer, FloatPoint& point)
{
    DeprecatedPaintLayer* paintInvalidationLayer = paintInvalidationContainer->layer();
    if (!paintInvalidationLayer->groupedMapping()) {
        point.move(paintInvalidationLayer->compositedDeprecatedPaintLayerMapping()->contentOffsetInCompositingLayer());
        return;
    }

    LayoutBoxModelObject* transformedAncestor = paintInvalidationLayer->enclosingTransformedAncestor()->layoutObject();
    if (!transformedAncestor)
        return;

    // |paintInvalidationContainer| may have a local 2D transform on it, so take that into account when mapping into the space of the
    // transformed ancestor.
    point = paintInvalidationContainer->localToContainerPoint(point, transformedAncestor);

    point.moveBy(-paintInvalidationLayer->groupedMapping()->squashingOffsetFromTransformedAncestor());
}

void DeprecatedPaintLayer::mapRectToPaintBackingCoordinates(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect)
{
    DeprecatedPaintLayer* paintInvalidationLayer = paintInvalidationContainer->layer();
    if (!paintInvalidationLayer->groupedMapping()) {
        rect.move(paintInvalidationLayer->compositedDeprecatedPaintLayerMapping()->contentOffsetInCompositingLayer());
        return;
    }

    LayoutBoxModelObject* transformedAncestor = paintInvalidationLayer->enclosingTransformedAncestor()->layoutObject();
    if (!transformedAncestor)
        return;

    // |paintInvalidationContainer| may have a local 2D transform on it, so take that into account when mapping into the space of the
    // transformed ancestor.
    rect = LayoutRect(paintInvalidationContainer->localToContainerQuad(FloatRect(rect), transformedAncestor).boundingBox());

    rect.moveBy(-paintInvalidationLayer->groupedMapping()->squashingOffsetFromTransformedAncestor());
}

void DeprecatedPaintLayer::mapRectToPaintInvalidationBacking(const LayoutObject* layoutObject, const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, const PaintInvalidationState* paintInvalidationState)
{
    if (!paintInvalidationContainer->layer()->groupedMapping()) {
        layoutObject->mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
        return;
    }

    // This code adjusts the paint invalidation rectangle to be in the space of the transformed ancestor of the grouped (i.e. squashed)
    // layer. This is because all layers that squash together need to issue paint invalidations w.r.t. a single container that is
    // an ancestor of all of them, in order to properly take into account any local transforms etc.
    // FIXME: remove this special-case code that works around the paint invalidation code structure.
    layoutObject->mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);

    mapRectToPaintBackingCoordinates(paintInvalidationContainer, rect);
}

LayoutRect DeprecatedPaintLayer::computePaintInvalidationRect(const LayoutObject* layoutObject, const DeprecatedPaintLayer* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState)
{
    if (!paintInvalidationContainer->groupedMapping())
        return layoutObject->computePaintInvalidationRect(paintInvalidationContainer->layoutObject(), paintInvalidationState);

    LayoutRect rect = layoutObject->clippedOverflowRectForPaintInvalidation(paintInvalidationContainer->layoutObject(), paintInvalidationState);
    mapRectToPaintBackingCoordinates(paintInvalidationContainer->layoutObject(), rect);
    return rect;
}

void DeprecatedPaintLayer::dirtyVisibleContentStatus()
{
    compositor()->setNeedsUpdateDescendantDependentFlags();
    m_visibleContentStatusDirty = true;
    if (parent())
        parent()->dirtyAncestorChainVisibleDescendantStatus();
    // Non-self-painting layers paint into their ancestor layer, and count as part of the "visible contents" of the parent, so we need to dirty it.
    if (!isSelfPaintingLayer())
        parent()->dirtyVisibleContentStatus();
}

void DeprecatedPaintLayer::potentiallyDirtyVisibleContentStatus(EVisibility visibility)
{
    if (m_visibleContentStatusDirty)
        return;
    if (hasVisibleContent() == (visibility == VISIBLE))
        return;
    dirtyVisibleContentStatus();
}

void DeprecatedPaintLayer::dirtyAncestorChainVisibleDescendantStatus()
{
    compositor()->setNeedsUpdateDescendantDependentFlags();

    for (DeprecatedPaintLayer* layer = this; layer; layer = layer->parent()) {
        if (layer->m_visibleDescendantStatusDirty)
            break;
        layer->m_visibleDescendantStatusDirty = true;
    }
}

// FIXME: this is quite brute-force. We could be more efficient if we were to
// track state and update it as appropriate as changes are made in the layout tree.
void DeprecatedPaintLayer::updateScrollingStateAfterCompositingChange()
{
    TRACE_EVENT0("blink", "DeprecatedPaintLayer::updateScrollingStateAfterCompositingChange");
    m_hasVisibleNonLayerContent = false;
    for (LayoutObject* r = layoutObject()->slowFirstChild(); r; r = r->nextSibling()) {
        if (!r->hasLayer()) {
            m_hasVisibleNonLayerContent = true;
            break;
        }
    }

    m_hasNonCompositedChild = false;
    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling()) {
        if (child->compositingState() == NotComposited) {
            m_hasNonCompositedChild = true;
            return;
        }
    }
}

void DeprecatedPaintLayer::updateDescendantDependentFlags()
{
    if (m_visibleDescendantStatusDirty) {
        m_hasVisibleDescendant = false;

        for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling()) {
            child->updateDescendantDependentFlags();

            if (child->m_hasVisibleContent || child->m_hasVisibleDescendant) {
                m_hasVisibleDescendant = true;
                break;
            }
        }

        m_visibleDescendantStatusDirty = false;
    }

    if (m_visibleContentStatusDirty) {
        bool previouslyHasVisibleContent = m_hasVisibleContent;
        if (layoutObject()->style()->visibility() == VISIBLE) {
            m_hasVisibleContent = true;
        } else {
            // layer may be hidden but still have some visible content, check for this
            m_hasVisibleContent = false;
            LayoutObject* r = layoutObject()->slowFirstChild();
            while (r) {
                if (r->style()->visibility() == VISIBLE && (!r->hasLayer() || !r->enclosingLayer()->isSelfPaintingLayer())) {
                    m_hasVisibleContent = true;
                    break;
                }
                LayoutObject* layoutObjectFirstChild = r->slowFirstChild();
                if (layoutObjectFirstChild && (!r->hasLayer() || !r->enclosingLayer()->isSelfPaintingLayer())) {
                    r = layoutObjectFirstChild;
                } else if (r->nextSibling()) {
                    r = r->nextSibling();
                } else {
                    do {
                        r = r->parent();
                        if (r == layoutObject())
                            r = 0;
                    } while (r && !r->nextSibling());
                    if (r)
                        r = r->nextSibling();
                }
            }
        }
        m_visibleContentStatusDirty = false;

        if (hasVisibleContent() != previouslyHasVisibleContent) {
            setNeedsCompositingInputsUpdate();
            // We need to tell m_layoutObject to recheck its rect because we
            // pretend that invisible LayoutObjects have 0x0 rects. Changing
            // visibility therefore changes our rect and we need to visit
            // this LayoutObject during the invalidateTreeIfNeeded walk.
            m_layoutObject->setMayNeedPaintInvalidation();
        }
    }
}

void DeprecatedPaintLayer::dirty3DTransformedDescendantStatus()
{
    DeprecatedPaintLayerStackingNode* stackingNode = m_stackingNode->ancestorStackingContextNode();
    if (!stackingNode)
        return;

    stackingNode->layer()->m_3DTransformedDescendantStatusDirty = true;

    // This propagates up through preserve-3d hierarchies to the enclosing flattening layer.
    // Note that preserves3D() creates stacking context, so we can just run up the stacking containers.
    while (stackingNode && stackingNode->layer()->preserves3D()) {
        stackingNode->layer()->m_3DTransformedDescendantStatusDirty = true;
        stackingNode = stackingNode->ancestorStackingContextNode();
    }
}

// Return true if this layer or any preserve-3d descendants have 3d.
bool DeprecatedPaintLayer::update3DTransformedDescendantStatus()
{
    if (m_3DTransformedDescendantStatusDirty) {
        m_has3DTransformedDescendant = false;

        m_stackingNode->updateZOrderLists();

        // Transformed or preserve-3d descendants can only be in the z-order lists, not
        // in the normal flow list, so we only need to check those.
        DeprecatedPaintLayerStackingNodeIterator iterator(*m_stackingNode.get(), PositiveZOrderChildren | NegativeZOrderChildren);
        while (DeprecatedPaintLayerStackingNode* node = iterator.next())
            m_has3DTransformedDescendant |= node->layer()->update3DTransformedDescendantStatus();

        m_3DTransformedDescendantStatusDirty = false;
    }

    // If we live in a 3d hierarchy, then the layer at the root of that hierarchy needs
    // the m_has3DTransformedDescendant set.
    if (preserves3D())
        return has3DTransform() || m_has3DTransformedDescendant;

    return has3DTransform();
}

bool DeprecatedPaintLayer::updateLayerPosition()
{
    LayoutPoint localPoint;
    LayoutPoint inlineBoundingBoxOffset; // We don't put this into the Layer x/y for inlines, so we need to subtract it out when done.

    if (layoutObject()->isInline() && layoutObject()->isLayoutInline()) {
        LayoutInline* inlineFlow = toLayoutInline(layoutObject());
        IntRect lineBox = inlineFlow->linesBoundingBox();
        m_size = lineBox.size();
        inlineBoundingBoxOffset = lineBox.location();
        localPoint.moveBy(inlineBoundingBoxOffset);
    } else if (LayoutBox* box = layoutBox()) {
        m_size = pixelSnappedIntSize(box->size(), box->location());
        localPoint.moveBy(box->topLeftLocation());
    }

    if (!layoutObject()->isOutOfFlowPositioned() && !layoutObject()->isColumnSpanAll() && layoutObject()->parent()) {
        // We must adjust our position by walking up the layout tree looking for the
        // nearest enclosing object with a layer.
        LayoutObject* curr = layoutObject()->parent();
        while (curr && !curr->hasLayer()) {
            if (curr->isBox() && !curr->isTableRow()) {
                // Rows and cells share the same coordinate space (that of the section).
                // Omit them when computing our xpos/ypos.
                localPoint.moveBy(toLayoutBox(curr)->topLeftLocation());
            }
            curr = curr->parent();
        }
        if (curr->isBox() && curr->isTableRow()) {
            // Put ourselves into the row coordinate space.
            localPoint.moveBy(-toLayoutBox(curr)->topLeftLocation());
        }
    }

    // Subtract our parent's scroll offset.
    if (DeprecatedPaintLayer* positionedParent = layoutObject()->isOutOfFlowPositioned() ? enclosingPositionedAncestor() : nullptr) {
        // For positioned layers, we subtract out the enclosing positioned layer's scroll offset.
        if (positionedParent->layoutObject()->hasOverflowClip()) {
            IntSize offset = positionedParent->layoutBox()->scrolledContentOffset();
            localPoint -= offset;
        }

        if (positionedParent->layoutObject()->isRelPositioned() && positionedParent->layoutObject()->isLayoutInline()) {
            LayoutSize offset = toLayoutInline(positionedParent->layoutObject())->offsetForInFlowPositionedInline(*toLayoutBox(layoutObject()));
            localPoint += offset;
        }
    } else if (parent() && parent()->layoutObject()->hasOverflowClip()) {
        IntSize scrollOffset = parent()->layoutBox()->scrolledContentOffset();
        localPoint -= scrollOffset;
    }

    bool positionOrOffsetChanged = false;
    if (layoutObject()->isRelPositioned()) {
        LayoutSize newOffset = layoutObject()->offsetForInFlowPosition();
        positionOrOffsetChanged = newOffset != m_offsetForInFlowPosition;
        m_offsetForInFlowPosition = newOffset;
        localPoint.move(m_offsetForInFlowPosition);
    } else {
        m_offsetForInFlowPosition = LayoutSize();
    }

    // FIXME: We'd really like to just get rid of the concept of a layer rectangle and rely on the layoutObjects.
    localPoint.moveBy(-inlineBoundingBoxOffset);

    if (m_location != localPoint)
        positionOrOffsetChanged = true;
    m_location = localPoint;

#if ENABLE(ASSERT)
    m_needsPositionUpdate = false;
#endif
    return positionOrOffsetChanged;
}

TransformationMatrix DeprecatedPaintLayer::perspectiveTransform() const
{
    if (!layoutObject()->hasTransformRelatedProperty())
        return TransformationMatrix();

    const ComputedStyle& style = layoutObject()->styleRef();
    if (!style.hasPerspective())
        return TransformationMatrix();

    TransformationMatrix t;
    t.applyPerspective(style.perspective());
    return t;
}

FloatPoint DeprecatedPaintLayer::perspectiveOrigin() const
{
    if (!layoutObject()->hasTransformRelatedProperty())
        return FloatPoint();

    const LayoutRect borderBox = toLayoutBox(layoutObject())->borderBoxRect();
    const ComputedStyle& style = layoutObject()->styleRef();

    return FloatPoint(floatValueForLength(style.perspectiveOriginX(), borderBox.width().toFloat()), floatValueForLength(style.perspectiveOriginY(), borderBox.height().toFloat()));
}

static inline bool isFixedPositionedContainer(DeprecatedPaintLayer* layer)
{
    return layer->isRootLayer() || layer->hasTransformRelatedProperty();
}

DeprecatedPaintLayer* DeprecatedPaintLayer::enclosingPositionedAncestor(const DeprecatedPaintLayer* ancestor, bool* skippedAncestor) const
{
    ASSERT(!ancestor || skippedAncestor); // If we have specified an ancestor, surely the caller needs to know whether we skipped it.
    if (skippedAncestor)
        *skippedAncestor = false;
    if (layoutObject()->style()->position() == FixedPosition) {
        DeprecatedPaintLayer* curr = parent();
        while (curr && !isFixedPositionedContainer(curr)) {
            if (skippedAncestor && curr == ancestor)
                *skippedAncestor = true;
            curr = curr->parent();
        }

        return curr;
    }

    DeprecatedPaintLayer* curr = parent();
    while (curr && !curr->isPositionedContainer()) {
        if (skippedAncestor && curr == ancestor)
            *skippedAncestor = true;
        curr = curr->parent();
    }

    return curr;
}

DeprecatedPaintLayer* DeprecatedPaintLayer::enclosingTransformedAncestor() const
{
    DeprecatedPaintLayer* curr = parent();
    while (curr && !curr->isRootLayer() && !curr->layoutObject()->hasTransformRelatedProperty())
        curr = curr->parent();

    return curr;
}

LayoutPoint DeprecatedPaintLayer::computeOffsetFromTransformedAncestor() const
{
    const AncestorDependentCompositingInputs& properties = ancestorDependentCompositingInputs();

    TransformState transformState(TransformState::ApplyTransformDirection, FloatPoint());
    // FIXME: add a test that checks flipped writing mode and ApplyContainerFlip are correct.
    layoutObject()->mapLocalToContainer(properties.transformAncestor ? properties.transformAncestor->layoutObject() : 0, transformState, ApplyContainerFlip);
    transformState.flatten();
    return LayoutPoint(transformState.lastPlanarPoint());
}

const DeprecatedPaintLayer* DeprecatedPaintLayer::compositingContainer() const
{
    if (!stackingNode()->isTreatedAsStackingContextForPainting())
        return parent();
    if (DeprecatedPaintLayerStackingNode* ancestorStackingNode = stackingNode()->ancestorStackingContextNode())
        return ancestorStackingNode->layer();
    return 0;
}

bool DeprecatedPaintLayer::isPaintInvalidationContainer() const
{
    return compositingState() == PaintsIntoOwnBacking || compositingState() == PaintsIntoGroupedBacking;
}

// Note: enclosingCompositingLayer does not include squashed layers. Compositing stacking children of squashed layers
// receive graphics layers that are parented to the compositing ancestor of the squashed layer.
DeprecatedPaintLayer* DeprecatedPaintLayer::enclosingLayerWithCompositedDeprecatedPaintLayerMapping(IncludeSelfOrNot includeSelf) const
{
    ASSERT(isAllowedToQueryCompositingState());

    if ((includeSelf == IncludeSelf) && compositingState() != NotComposited && compositingState() != PaintsIntoGroupedBacking)
        return const_cast<DeprecatedPaintLayer*>(this);

    for (const DeprecatedPaintLayer* curr = compositingContainer(); curr; curr = curr->compositingContainer()) {
        if (curr->compositingState() != NotComposited && curr->compositingState() != PaintsIntoGroupedBacking)
            return const_cast<DeprecatedPaintLayer*>(curr);
    }

    return 0;
}

// Return the enclosingCompositedLayerForPaintInvalidation for the given Layer
// including crossing frame boundaries.
DeprecatedPaintLayer* DeprecatedPaintLayer::enclosingLayerForPaintInvalidationCrossingFrameBoundaries() const
{
    const DeprecatedPaintLayer* layer = this;
    DeprecatedPaintLayer* compositedLayer = 0;
    while (!compositedLayer) {
        compositedLayer = layer->enclosingLayerForPaintInvalidation();
        if (!compositedLayer) {
            LayoutObject* owner = layer->layoutObject()->frame()->ownerLayoutObject();
            if (!owner)
                break;
            layer = owner->enclosingLayer();
        }
    }
    return compositedLayer;
}

DeprecatedPaintLayer* DeprecatedPaintLayer::enclosingLayerForPaintInvalidation() const
{
    ASSERT(isAllowedToQueryCompositingState());

    if (isPaintInvalidationContainer())
        return const_cast<DeprecatedPaintLayer*>(this);

    for (const DeprecatedPaintLayer* curr = compositingContainer(); curr; curr = curr->compositingContainer()) {
        if (curr->isPaintInvalidationContainer())
            return const_cast<DeprecatedPaintLayer*>(curr);
    }

    return 0;
}

void DeprecatedPaintLayer::setNeedsCompositingInputsUpdate()
{
    m_needsAncestorDependentCompositingInputsUpdate = true;
    m_needsDescendantDependentCompositingInputsUpdate = true;

    for (DeprecatedPaintLayer* current = this; current && !current->m_childNeedsCompositingInputsUpdate; current = current->parent())
        current->m_childNeedsCompositingInputsUpdate = true;

    compositor()->setNeedsCompositingUpdate(CompositingUpdateAfterCompositingInputChange);
}

void DeprecatedPaintLayer::updateAncestorDependentCompositingInputs(const AncestorDependentCompositingInputs& compositingInputs)
{
    m_ancestorDependentCompositingInputs = compositingInputs;
    m_needsAncestorDependentCompositingInputsUpdate = false;
}

void DeprecatedPaintLayer::updateDescendantDependentCompositingInputs(const DescendantDependentCompositingInputs& compositingInputs)
{
    m_descendantDependentCompositingInputs = compositingInputs;
    m_needsDescendantDependentCompositingInputsUpdate = false;
}

void DeprecatedPaintLayer::didUpdateCompositingInputs()
{
    ASSERT(!needsCompositingInputsUpdate());
    m_childNeedsCompositingInputsUpdate = false;
    if (m_scrollableArea)
        m_scrollableArea->updateNeedsCompositedScrolling();
}

bool DeprecatedPaintLayer::hasNonIsolatedDescendantWithBlendMode() const
{
    if (descendantDependentCompositingInputs().hasNonIsolatedDescendantWithBlendMode)
        return true;
    if (layoutObject()->isSVGRoot())
        return toLayoutSVGRoot(layoutObject())->hasNonIsolatedBlendingDescendants();
    return false;
}

void DeprecatedPaintLayer::setCompositingReasons(CompositingReasons reasons, CompositingReasons mask)
{
    if ((compositingReasons() & mask) == (reasons & mask))
        return;
    m_compositingReasons = (reasons & mask) | (compositingReasons() & ~mask);
}

void DeprecatedPaintLayer::setHasCompositingDescendant(bool hasCompositingDescendant)
{
    if (m_hasCompositingDescendant == static_cast<unsigned>(hasCompositingDescendant))
        return;

    m_hasCompositingDescendant = hasCompositingDescendant;

    if (hasCompositedDeprecatedPaintLayerMapping())
        compositedDeprecatedPaintLayerMapping()->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateLocal);
}

void DeprecatedPaintLayer::setShouldIsolateCompositedDescendants(bool shouldIsolateCompositedDescendants)
{
    if (m_shouldIsolateCompositedDescendants == static_cast<unsigned>(shouldIsolateCompositedDescendants))
        return;

    m_shouldIsolateCompositedDescendants = shouldIsolateCompositedDescendants;

    if (hasCompositedDeprecatedPaintLayerMapping())
        compositedDeprecatedPaintLayerMapping()->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateLocal);
}

bool DeprecatedPaintLayer::hasAncestorWithFilterOutsets() const
{
    for (const DeprecatedPaintLayer* curr = this; curr; curr = curr->parent()) {
        LayoutBoxModelObject* layoutObject = curr->layoutObject();
        if (layoutObject->style()->hasFilterOutsets())
            return true;
    }
    return false;
}

static void expandClipRectForDescendantsAndReflection(LayoutRect& clipRect, const DeprecatedPaintLayer* layer, const DeprecatedPaintLayer* rootLayer,
    DeprecatedPaintLayer::TransparencyClipBoxBehavior transparencyBehavior, const LayoutSize& subPixelAccumulation, PaintBehavior paintBehavior)
{
    // If we have a mask, then the clip is limited to the border box area (and there is
    // no need to examine child layers).
    if (!layer->layoutObject()->hasMask()) {
        // Note: we don't have to walk z-order lists since transparent elements always establish
        // a stacking container. This means we can just walk the layer tree directly.
        for (DeprecatedPaintLayer* curr = layer->firstChild(); curr; curr = curr->nextSibling()) {
            if (!layer->reflectionInfo() || layer->reflectionInfo()->reflectionLayer() != curr)
                clipRect.unite(DeprecatedPaintLayer::transparencyClipBox(curr, rootLayer, transparencyBehavior, DeprecatedPaintLayer::DescendantsOfTransparencyClipBox, subPixelAccumulation, paintBehavior));
        }
    }

    // If we have a reflection, then we need to account for that when we push the clip.  Reflect our entire
    // current transparencyClipBox to catch all child layers.
    // FIXME: Accelerated compositing will eventually want to do something smart here to avoid incorporating this
    // size into the parent layer.
    if (layer->layoutObject()->hasReflection()) {
        LayoutPoint delta;
        layer->convertToLayerCoords(rootLayer, delta);
        clipRect.move(-delta.x(), -delta.y());
        clipRect.unite(layer->layoutBox()->reflectedRect(clipRect));
        clipRect.moveBy(delta);
    }
}

LayoutRect DeprecatedPaintLayer::transparencyClipBox(const DeprecatedPaintLayer* layer, const DeprecatedPaintLayer* rootLayer, TransparencyClipBoxBehavior transparencyBehavior,
    TransparencyClipBoxMode transparencyMode, const LayoutSize& subPixelAccumulation, PaintBehavior paintBehavior)
{
    // FIXME: Although this function completely ignores CSS-imposed clipping, we did already intersect with the
    // paintDirtyRect, and that should cut down on the amount we have to paint.  Still it
    // would be better to respect clips.

    if (rootLayer != layer && ((transparencyBehavior == PaintingTransparencyClipBox && layer->paintsWithTransform(paintBehavior))
        || (transparencyBehavior == HitTestingTransparencyClipBox && layer->hasTransformRelatedProperty()))) {
        // The best we can do here is to use enclosed bounding boxes to establish a "fuzzy" enough clip to encompass
        // the transformed layer and all of its children.
        const DeprecatedPaintLayer* paginationLayer = transparencyMode == DescendantsOfTransparencyClipBox ? layer->enclosingPaginationLayer() : 0;
        const DeprecatedPaintLayer* rootLayerForTransform = paginationLayer ? paginationLayer : rootLayer;
        LayoutPoint delta;
        layer->convertToLayerCoords(rootLayerForTransform, delta);

        delta.move(subPixelAccumulation);
        IntPoint pixelSnappedDelta = roundedIntPoint(delta);
        TransformationMatrix transform;
        transform.translate(pixelSnappedDelta.x(), pixelSnappedDelta.y());
        if (layer->transform())
            transform = transform * *layer->transform();

        // We don't use fragment boxes when collecting a transformed layer's bounding box, since it always
        // paints unfragmented.
        LayoutRect clipRect = layer->physicalBoundingBox(layer);
        expandClipRectForDescendantsAndReflection(clipRect, layer, layer, transparencyBehavior, subPixelAccumulation, paintBehavior);
        clipRect.expand(layer->layoutObject()->style()->filterOutsets());
        LayoutRect result = transform.mapRect(clipRect);
        if (!paginationLayer)
            return result;

        // We have to break up the transformed extent across our columns.
        // Split our box up into the actual fragment boxes that layout in the columns/pages and unite those together to
        // get our true bounding box.
        LayoutFlowThread* enclosingFlowThread = toLayoutFlowThread(paginationLayer->layoutObject());
        result = enclosingFlowThread->fragmentsBoundingBox(result);

        LayoutPoint rootLayerDelta;
        paginationLayer->convertToLayerCoords(rootLayer, rootLayerDelta);
        result.moveBy(rootLayerDelta);
        return result;
    }

    LayoutRect clipRect = layer->fragmentsBoundingBox(rootLayer);
    expandClipRectForDescendantsAndReflection(clipRect, layer, rootLayer, transparencyBehavior, subPixelAccumulation, paintBehavior);
    clipRect.expand(layer->layoutObject()->style()->filterOutsets());
    clipRect.move(subPixelAccumulation);
    return clipRect;
}

LayoutRect DeprecatedPaintLayer::paintingExtent(const DeprecatedPaintLayer* rootLayer, const LayoutRect& paintDirtyRect, const LayoutSize& subPixelAccumulation, PaintBehavior paintBehavior)
{
    return intersection(transparencyClipBox(this, rootLayer, PaintingTransparencyClipBox, RootOfTransparencyClipBox, subPixelAccumulation, paintBehavior), paintDirtyRect);
}

void* DeprecatedPaintLayer::operator new(size_t sz)
{
    return partitionAlloc(WTF::Partitions::layoutPartition(), sz, "DeprecatedPaintLayer::operator new");
}

void DeprecatedPaintLayer::operator delete(void* ptr)
{
    partitionFree(ptr);
}

void DeprecatedPaintLayer::addChild(DeprecatedPaintLayer* child, DeprecatedPaintLayer* beforeChild)
{
    DeprecatedPaintLayer* prevSibling = beforeChild ? beforeChild->previousSibling() : lastChild();
    if (prevSibling) {
        child->setPreviousSibling(prevSibling);
        prevSibling->setNextSibling(child);
        ASSERT(prevSibling != child);
    } else {
        setFirstChild(child);
    }

    if (beforeChild) {
        beforeChild->setPreviousSibling(child);
        child->setNextSibling(beforeChild);
        ASSERT(beforeChild != child);
    } else {
        setLastChild(child);
    }

    child->m_parent = this;

    setNeedsCompositingInputsUpdate();

    if (!child->stackingNode()->isTreatedAsStackingContextForPainting() && !layoutObject()->documentBeingDestroyed())
        compositor()->setNeedsCompositingUpdate(CompositingUpdateRebuildTree);

    if (child->stackingNode()->isTreatedAsStackingContextForPainting() || child->firstChild()) {
        // Dirty the z-order list in which we are contained. The ancestorStackingContextNode() can be null in the
        // case where we're building up generated content layers. This is ok, since the lists will start
        // off dirty in that case anyway.
        child->stackingNode()->dirtyStackingContextZOrderLists();
    }

    // Non-self-painting children paint into this layer, so the visible contents status of this layer is affected.
    if (!child->isSelfPaintingLayer())
        dirtyVisibleContentStatus();

    dirtyAncestorChainVisibleDescendantStatus();
    dirtyAncestorChainHasSelfPaintingLayerDescendantStatus();

    child->updateDescendantDependentFlags();
}

DeprecatedPaintLayer* DeprecatedPaintLayer::removeChild(DeprecatedPaintLayer* oldChild)
{
    if (oldChild->previousSibling())
        oldChild->previousSibling()->setNextSibling(oldChild->nextSibling());
    if (oldChild->nextSibling())
        oldChild->nextSibling()->setPreviousSibling(oldChild->previousSibling());

    if (m_first == oldChild)
        m_first = oldChild->nextSibling();
    if (m_last == oldChild)
        m_last = oldChild->previousSibling();

    if (!oldChild->stackingNode()->isTreatedAsStackingContextForPainting() && !layoutObject()->documentBeingDestroyed())
        compositor()->setNeedsCompositingUpdate(CompositingUpdateRebuildTree);

    if (oldChild->stackingNode()->isTreatedAsStackingContextForPainting() || oldChild->firstChild()) {
        // Dirty the z-order list in which we are contained.  When called via the
        // reattachment process in removeOnlyThisLayer, the layer may already be disconnected
        // from the main layer tree, so we need to null-check the
        // |stackingContext| value.
        oldChild->stackingNode()->dirtyStackingContextZOrderLists();
    }

    if (layoutObject()->style()->visibility() != VISIBLE)
        dirtyVisibleContentStatus();

    oldChild->setPreviousSibling(0);
    oldChild->setNextSibling(0);
    oldChild->m_parent = 0;

    dirtyAncestorChainHasSelfPaintingLayerDescendantStatus();

    oldChild->updateDescendantDependentFlags();

    if (oldChild->m_hasVisibleContent || oldChild->m_hasVisibleDescendant)
        dirtyAncestorChainVisibleDescendantStatus();

    if (oldChild->enclosingPaginationLayer())
        oldChild->clearPaginationRecursive();

    return oldChild;
}

void DeprecatedPaintLayer::removeOnlyThisLayer()
{
    if (!m_parent)
        return;

    {
        DisableCompositingQueryAsserts disabler; // We need the current compositing status.
        if (isPaintInvalidationContainer()) {
            // Our children will be reparented and contained by a new paint invalidation container,
            // so need paint invalidation. CompositingUpdate can't see this layer (which has been
            // removed) so won't do this for us.
            layoutObject()->setShouldDoFullPaintInvalidationIncludingNonCompositingDescendants();
        }
    }

    m_clipper.clearClipRectsIncludingDescendants();

    DeprecatedPaintLayer* nextSib = nextSibling();

    // Remove the child reflection layer before moving other child layers.
    // The reflection layer should not be moved to the parent.
    if (m_reflectionInfo)
        removeChild(m_reflectionInfo->reflectionLayer());

    // Now walk our kids and reattach them to our parent.
    DeprecatedPaintLayer* current = m_first;
    while (current) {
        DeprecatedPaintLayer* next = current->nextSibling();
        removeChild(current);
        m_parent->addChild(current, nextSib);

        // FIXME: We should call a specialized version of this function.
        current->updateLayerPositionsAfterLayout();
        current = next;
    }

    // Remove us from the parent.
    m_parent->removeChild(this);
    m_layoutObject->destroyLayer();
}

void DeprecatedPaintLayer::insertOnlyThisLayer()
{
    if (!m_parent && layoutObject()->parent()) {
        // We need to connect ourselves when our layoutObject() has a parent.
        // Find our enclosingLayer and add ourselves.
        DeprecatedPaintLayer* parentLayer = layoutObject()->parent()->enclosingLayer();
        ASSERT(parentLayer);
        DeprecatedPaintLayer* beforeChild = !parentLayer->reflectionInfo() || parentLayer->reflectionInfo()->reflectionLayer() != this ? layoutObject()->parent()->findNextLayer(parentLayer, layoutObject()) : 0;
        parentLayer->addChild(this, beforeChild);
    }

    // Remove all descendant layers from the hierarchy and add them to the new position.
    for (LayoutObject* curr = layoutObject()->slowFirstChild(); curr; curr = curr->nextSibling())
        curr->moveLayers(m_parent, this);

    // Clear out all the clip rects.
    m_clipper.clearClipRectsIncludingDescendants();
}

// Returns the layer reached on the walk up towards the ancestor.
static inline const DeprecatedPaintLayer* accumulateOffsetTowardsAncestor(const DeprecatedPaintLayer* layer, const DeprecatedPaintLayer* ancestorLayer, LayoutPoint& location)
{
    ASSERT(ancestorLayer != layer);

    const LayoutBoxModelObject* layoutObject = layer->layoutObject();
    EPosition position = layoutObject->style()->position();

    if (position == FixedPosition && (!ancestorLayer || ancestorLayer == layoutObject->view()->layer())) {
        // If the fixed layer's container is the root, just add in the offset of the view. We can obtain this by calling
        // localToAbsolute() on the LayoutView.
        FloatPoint absPos = layoutObject->localToAbsolute(FloatPoint(), IsFixed);
        location += LayoutSize(absPos.x(), absPos.y());
        return ancestorLayer;
    }

    if (position == FixedPosition) {
        // For a fixed layers, we need to walk up to the root to see if there's a fixed position container
        // (e.g. a transformed layer). It's an error to call convertToLayerCoords() across a layer with a transform,
        // so we should always find the ancestor at or before we find the fixed position container, if
        // the container is transformed.
        DeprecatedPaintLayer* fixedPositionContainerLayer = 0;
        bool foundAncestor = false;
        for (DeprecatedPaintLayer* currLayer = layer->parent(); currLayer; currLayer = currLayer->parent()) {
            if (currLayer == ancestorLayer)
                foundAncestor = true;

            if (isFixedPositionedContainer(currLayer)) {
                fixedPositionContainerLayer = currLayer;
                // A layer that has a transform-related property but not a
                // transform still acts as a fixed-position container.
                // Accumulating offsets across such layers is allowed.
                if (currLayer->transform())
                    ASSERT_UNUSED(foundAncestor, foundAncestor);
                break;
            }
        }

        ASSERT(fixedPositionContainerLayer); // We should have hit the LayoutView's layer at least.

        if (fixedPositionContainerLayer != ancestorLayer) {
            LayoutPoint fixedContainerCoords;
            layer->convertToLayerCoords(fixedPositionContainerLayer, fixedContainerCoords);
            location += fixedContainerCoords;

            if (foundAncestor) {
                LayoutPoint ancestorCoords;
                ancestorLayer->convertToLayerCoords(fixedPositionContainerLayer, ancestorCoords);
                location += -ancestorCoords;
            }
        } else {
            // LayoutView has been handled in the first top-level 'if' block above.
            ASSERT(ancestorLayer != layoutObject->view()->layer());
            ASSERT(ancestorLayer->hasTransformRelatedProperty());

            location += layer->location();
        }
        return foundAncestor ? ancestorLayer : fixedPositionContainerLayer;
    }

    DeprecatedPaintLayer* parentLayer;
    if (position == AbsolutePosition) {
        bool foundAncestorFirst;
        parentLayer = layer->enclosingPositionedAncestor(ancestorLayer, &foundAncestorFirst);

        if (foundAncestorFirst) {
            // Found ancestorLayer before the abs. positioned container, so compute offset of both relative
            // to the positioned ancestor and subtract.

            LayoutPoint thisCoords;
            layer->convertToLayerCoords(parentLayer, thisCoords);

            LayoutPoint ancestorCoords;
            ancestorLayer->convertToLayerCoords(parentLayer, ancestorCoords);

            location += (thisCoords - ancestorCoords);
            return ancestorLayer;
        }
    } else if (layoutObject->isColumnSpanAll()) {
        LayoutBlock* multicolContainer = layoutObject->containingBlock();
        ASSERT(toLayoutBlockFlow(multicolContainer)->multiColumnFlowThread());
        parentLayer = multicolContainer->layer();
        ASSERT(parentLayer);
    } else {
        parentLayer = layer->parent();
    }

    if (!parentLayer)
        return 0;

    location += layer->location();
    return parentLayer;
}

void DeprecatedPaintLayer::convertToLayerCoords(const DeprecatedPaintLayer* ancestorLayer, LayoutPoint& location) const
{
    if (ancestorLayer == this)
        return;

    const DeprecatedPaintLayer* currLayer = this;
    while (currLayer && currLayer != ancestorLayer)
        currLayer = accumulateOffsetTowardsAncestor(currLayer, ancestorLayer, location);
}

void DeprecatedPaintLayer::convertToLayerCoords(const DeprecatedPaintLayer* ancestorLayer, LayoutRect& rect) const
{
    LayoutPoint delta;
    convertToLayerCoords(ancestorLayer, delta);
    rect.moveBy(delta);
}

LayoutPoint DeprecatedPaintLayer::visualOffsetFromAncestor(const DeprecatedPaintLayer* ancestorLayer) const
{
    LayoutPoint offset;
    if (ancestorLayer == this)
        return offset;
    DeprecatedPaintLayer* paginationLayer = enclosingPaginationLayer();
    if (paginationLayer == this)
        paginationLayer = parent()->enclosingPaginationLayer();
    if (!paginationLayer) {
        convertToLayerCoords(ancestorLayer, offset);
        return offset;
    }

    LayoutFlowThread* flowThread = toLayoutFlowThread(paginationLayer->layoutObject());
    convertToLayerCoords(paginationLayer, offset);
    offset = flowThread->flowThreadPointToVisualPoint(offset);
    if (ancestorLayer == paginationLayer)
        return offset;

    if (ancestorLayer->enclosingPaginationLayer() != paginationLayer) {
        offset.moveBy(paginationLayer->visualOffsetFromAncestor(ancestorLayer));
    } else {
        // The ancestor layer is also inside the pagination layer, so we need to subtract the visual
        // distance from the ancestor layer to the pagination layer.
        offset.moveBy(-ancestorLayer->visualOffsetFromAncestor(paginationLayer));
    }
    return offset;
}

void DeprecatedPaintLayer::didUpdateNeedsCompositedScrolling()
{
    updateSelfPaintingLayer();
}

void DeprecatedPaintLayer::updateReflectionInfo(const ComputedStyle* oldStyle)
{
    ASSERT(!oldStyle || !layoutObject()->style()->reflectionDataEquivalent(oldStyle));
    if (layoutObject()->hasReflection()) {
        if (!m_reflectionInfo)
            m_reflectionInfo = adoptPtr(new DeprecatedPaintLayerReflectionInfo(*layoutBox()));
        m_reflectionInfo->updateAfterStyleChange(oldStyle);
    } else if (m_reflectionInfo) {
        m_reflectionInfo->destroy();
        m_reflectionInfo = nullptr;
    }
}

void DeprecatedPaintLayer::updateStackingNode()
{
    ASSERT(!m_stackingNode);
    if (requiresStackingNode())
        m_stackingNode = adoptPtr(new DeprecatedPaintLayerStackingNode(this));
}

bool DeprecatedPaintLayer::isInTopLayer() const
{
    Node* node = layoutObject()->node();
    return node && node->isElementNode() && toElement(node)->isInTopLayer();
}

void DeprecatedPaintLayer::updateScrollableArea()
{
    ASSERT(!m_scrollableArea);
    if (requiresScrollableArea())
        m_scrollableArea = DeprecatedPaintLayerScrollableArea::create(*this);
}

bool DeprecatedPaintLayer::hasOverflowControls() const
{
    return m_scrollableArea && (m_scrollableArea->hasScrollbar() || m_scrollableArea->scrollCorner() || layoutObject()->style()->resize() != RESIZE_NONE);
}

void DeprecatedPaintLayer::appendSingleFragmentIgnoringPagination(DeprecatedPaintLayerFragments& fragments, const DeprecatedPaintLayer* rootLayer, const LayoutRect& dirtyRect, ClipRectsCacheSlot clipRectsCacheSlot, OverlayScrollbarSizeRelevancy inOverlayScrollbarSizeRelevancy, ShouldRespectOverflowClip respectOverflowClip, const LayoutPoint* offsetFromRoot, const LayoutSize& subPixelAccumulation)
{
    DeprecatedPaintLayerFragment fragment;
    ClipRectsContext clipRectsContext(rootLayer, clipRectsCacheSlot, inOverlayScrollbarSizeRelevancy, subPixelAccumulation);
    if (respectOverflowClip == IgnoreOverflowClip)
        clipRectsContext.setIgnoreOverflowClip();
    clipper().calculateRects(clipRectsContext, dirtyRect, fragment.layerBounds, fragment.backgroundRect, fragment.foregroundRect, fragment.outlineRect, offsetFromRoot);
    fragments.append(fragment);
}

void DeprecatedPaintLayer::collectFragments(DeprecatedPaintLayerFragments& fragments, const DeprecatedPaintLayer* rootLayer, const LayoutRect& dirtyRect,
    ClipRectsCacheSlot clipRectsCacheSlot, OverlayScrollbarSizeRelevancy inOverlayScrollbarSizeRelevancy, ShouldRespectOverflowClip respectOverflowClip, const LayoutPoint* offsetFromRoot,
    const LayoutSize& subPixelAccumulation, const LayoutRect* layerBoundingBox)
{
    if (!enclosingPaginationLayer()) {
        // For unpaginated layers, there is only one fragment.
        appendSingleFragmentIgnoringPagination(fragments, rootLayer, dirtyRect, clipRectsCacheSlot, inOverlayScrollbarSizeRelevancy, respectOverflowClip, offsetFromRoot, subPixelAccumulation);
        return;
    }

    // Compute our offset within the enclosing pagination layer.
    LayoutPoint offsetWithinPaginatedLayer;
    convertToLayerCoords(enclosingPaginationLayer(), offsetWithinPaginatedLayer);

    // Calculate clip rects relative to the enclosingPaginationLayer. The purpose of this call is to determine our bounds clipped to intermediate
    // layers between us and the pagination context. It's important to minimize the number of fragments we need to create and this helps with that.
    ClipRectsContext paginationClipRectsContext(enclosingPaginationLayer(), clipRectsCacheSlot, inOverlayScrollbarSizeRelevancy);
    if (respectOverflowClip == IgnoreOverflowClip)
        paginationClipRectsContext.setIgnoreOverflowClip();
    LayoutRect layerBoundsInFlowThread;
    ClipRect backgroundRectInFlowThread;
    ClipRect foregroundRectInFlowThread;
    ClipRect outlineRectInFlowThread;
    clipper().calculateRects(paginationClipRectsContext, LayoutRect(LayoutRect::infiniteIntRect()), layerBoundsInFlowThread, backgroundRectInFlowThread, foregroundRectInFlowThread,
        outlineRectInFlowThread, &offsetWithinPaginatedLayer);

    // Take our bounding box within the flow thread and clip it.
    LayoutRect layerBoundingBoxInFlowThread = layerBoundingBox ? *layerBoundingBox : physicalBoundingBox(enclosingPaginationLayer(), &offsetWithinPaginatedLayer);
    layerBoundingBoxInFlowThread.intersect(backgroundRectInFlowThread.rect());

    // Make the dirty rect relative to the fragmentation context (multicol container, etc.).
    LayoutFlowThread* enclosingFlowThread = toLayoutFlowThread(enclosingPaginationLayer()->layoutObject());
    LayoutPoint offsetOfPaginationLayerFromRoot; // Visual offset from the root layer to the nearest fragmentation context.
    bool rootLayerIsInsidePaginationLayer = rootLayer->enclosingPaginationLayer() == enclosingPaginationLayer();
    if (rootLayerIsInsidePaginationLayer) {
        // The root layer is in the same fragmentation context as this layer, so we need to look
        // inside it and subtract the offset between the fragmentation context and the root layer.
        offsetOfPaginationLayerFromRoot = -rootLayer->visualOffsetFromAncestor(enclosingPaginationLayer());
    } else {
        offsetOfPaginationLayerFromRoot = enclosingPaginationLayer()->visualOffsetFromAncestor(rootLayer);
    }
    LayoutRect dirtyRectInFlowThread(dirtyRect);
    dirtyRectInFlowThread.moveBy(-offsetOfPaginationLayerFromRoot);

    // Tell the flow thread to collect the fragments. We pass enough information to create a minimal number of fragments based off the pages/columns
    // that intersect the actual dirtyRect as well as the pages/columns that intersect our layer's bounding box.
    enclosingFlowThread->collectLayerFragments(fragments, layerBoundingBoxInFlowThread, dirtyRectInFlowThread);

    if (fragments.isEmpty())
        return;

    // Get the parent clip rects of the pagination layer, since we need to intersect with that when painting column contents.
    ClipRect ancestorClipRect = dirtyRect;
    if (const DeprecatedPaintLayer* paginationParentLayer = enclosingPaginationLayer()->parent()) {
        const DeprecatedPaintLayer* ancestorLayer = rootLayerIsInsidePaginationLayer ? paginationParentLayer : rootLayer;
        ClipRectsContext clipRectsContext(ancestorLayer, clipRectsCacheSlot, inOverlayScrollbarSizeRelevancy);
        if (respectOverflowClip == IgnoreOverflowClip)
            clipRectsContext.setIgnoreOverflowClip();
        ancestorClipRect = enclosingPaginationLayer()->clipper().backgroundClipRect(clipRectsContext);
        if (rootLayerIsInsidePaginationLayer)
            ancestorClipRect.moveBy(-rootLayer->visualOffsetFromAncestor(ancestorLayer));
        ancestorClipRect.intersect(dirtyRect);
    }

    const LayoutSize subPixelAccumulationIfNeeded = compositingState() == PaintsIntoOwnBacking ? LayoutSize() : subPixelAccumulation;
    for (size_t i = 0; i < fragments.size(); ++i) {
        DeprecatedPaintLayerFragment& fragment = fragments.at(i);

        // Set our four rects with all clipping applied that was internal to the flow thread.
        fragment.setRects(layerBoundsInFlowThread, backgroundRectInFlowThread, foregroundRectInFlowThread, outlineRectInFlowThread);

        // Shift to the root-relative physical position used when painting the flow thread in this fragment.
        fragment.moveBy(fragment.paginationOffset + offsetOfPaginationLayerFromRoot + subPixelAccumulationIfNeeded);

        // Intersect the fragment with our ancestor's background clip so that e.g., columns in an overflow:hidden block are
        // properly clipped by the overflow.
        fragment.intersect(ancestorClipRect.rect());

        // Now intersect with our pagination clip. This will typically mean we're just intersecting the dirty rect with the column
        // clip, so the column clip ends up being all we apply.
        fragment.intersect(fragment.paginationClip);
    }
}

static inline LayoutRect frameVisibleRect(LayoutObject* layoutObject)
{
    FrameView* frameView = layoutObject->document().view();
    if (!frameView)
        return LayoutRect();

    return LayoutRect(frameView->visibleContentRect());
}

bool DeprecatedPaintLayer::hitTest(HitTestResult& result)
{
    ASSERT(isSelfPaintingLayer() || hasSelfPaintingLayerDescendant());

    // LayoutView should make sure to update layout before entering hit testing
    ASSERT(!layoutObject()->frame()->view()->layoutPending());
    ASSERT(!layoutObject()->document().layoutView()->needsLayout());

    const HitTestRequest& request = result.hitTestRequest();
    const HitTestLocation& hitTestLocation = result.hitTestLocation();

    // Start with frameVisibleRect to ensure we include the scrollbars.
    LayoutRect hitTestArea = frameVisibleRect(layoutObject());
    if (request.ignoreClipping())
        hitTestArea.unite(LayoutRect(layoutObject()->view()->documentRect()));

    DeprecatedPaintLayer* insideLayer = hitTestLayer(this, 0, result, hitTestArea, hitTestLocation, false);
    if (!insideLayer) {
        // We didn't hit any layer. If we are the root layer and the mouse is -- or just was -- down,
        // return ourselves. We do this so mouse events continue getting delivered after a drag has
        // exited the WebView, and so hit testing over a scrollbar hits the content document.
        // In addtion, it is possible for the mouse to stay in the document but there is no element.
        // At that time, the events of the mouse should be fired.
        // TODO(majidvp): This should apply more consistently across different event types and we
        // should not use RequestType for it. Perhaps best for it to be done at a higher level. See
        // http://crbug.com/505825
        LayoutPoint hitPoint = hitTestLocation.point();
        if (!request.isChildFrameHitTest() && ((request.active() || request.release()) || (request.move() && hitTestArea.contains(hitPoint.x(), hitPoint.y()))) && isRootLayer()) {
            layoutObject()->updateHitTestResult(result, toLayoutView(layoutObject())->flipForWritingMode(hitTestLocation.point()));
            insideLayer = this;

            // Don't cache this result since it really wasn't a true hit.
            result.setCacheable(false);
        }
    }

    // Now determine if the result is inside an anchor - if the urlElement isn't already set.
    Node* node = result.innerNode();
    if (node && !result.URLElement())
        result.setURLElement(node->enclosingLinkEventParentOrSelf());

    // Now return whether we were inside this layer (this will always be true for the root
    // layer).
    return insideLayer;
}

Node* DeprecatedPaintLayer::enclosingElement() const
{
    for (LayoutObject* r = layoutObject(); r; r = r->parent()) {
        if (Node* e = r->node())
            return e;
    }
    ASSERT_NOT_REACHED();
    return 0;
}

// Compute the z-offset of the point in the transformState.
// This is effectively projecting a ray normal to the plane of ancestor, finding where that
// ray intersects target, and computing the z delta between those two points.
static double computeZOffset(const HitTestingTransformState& transformState)
{
    // We got an affine transform, so no z-offset
    if (transformState.m_accumulatedTransform.isAffine())
        return 0;

    // Flatten the point into the target plane
    FloatPoint targetPoint = transformState.mappedPoint();

    // Now map the point back through the transform, which computes Z.
    FloatPoint3D backmappedPoint = transformState.m_accumulatedTransform.mapPoint(FloatPoint3D(targetPoint));
    return backmappedPoint.z();
}

PassRefPtr<HitTestingTransformState> DeprecatedPaintLayer::createLocalTransformState(DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* containerLayer,
    const LayoutRect& hitTestRect, const HitTestLocation& hitTestLocation,
    const HitTestingTransformState* containerTransformState,
    const LayoutPoint& translationOffset) const
{
    RefPtr<HitTestingTransformState> transformState;
    LayoutPoint offset;
    if (containerTransformState) {
        // If we're already computing transform state, then it's relative to the container (which we know is non-null).
        transformState = HitTestingTransformState::create(*containerTransformState);
        convertToLayerCoords(containerLayer, offset);
    } else {
        // If this is the first time we need to make transform state, then base it off of hitTestLocation,
        // which is relative to rootLayer.
        transformState = HitTestingTransformState::create(hitTestLocation.transformedPoint(), hitTestLocation.transformedRect(), FloatQuad(hitTestRect));
        convertToLayerCoords(rootLayer, offset);
    }
    offset.moveBy(translationOffset);

    LayoutObject* containerLayoutObject = containerLayer ? containerLayer->layoutObject() : 0;
    if (layoutObject()->shouldUseTransformFromContainer(containerLayoutObject)) {
        TransformationMatrix containerTransform;
        layoutObject()->getTransformFromContainer(containerLayoutObject, toLayoutSize(offset), containerTransform);
        transformState->applyTransform(containerTransform, HitTestingTransformState::AccumulateTransform);
    } else {
        transformState->translate(offset.x(), offset.y(), HitTestingTransformState::AccumulateTransform);
    }

    return transformState;
}


static bool isHitCandidate(const DeprecatedPaintLayer* hitLayer, bool canDepthSort, double* zOffset, const HitTestingTransformState* transformState)
{
    if (!hitLayer)
        return false;

    // The hit layer is depth-sorting with other layers, so just say that it was hit.
    if (canDepthSort)
        return true;

    // We need to look at z-depth to decide if this layer was hit.
    if (zOffset) {
        ASSERT(transformState);
        // This is actually computing our z, but that's OK because the hitLayer is coplanar with us.
        double childZOffset = computeZOffset(*transformState);
        if (childZOffset > *zOffset) {
            *zOffset = childZOffset;
            return true;
        }
        return false;
    }

    return true;
}

// hitTestLocation and hitTestRect are relative to rootLayer.
// A 'flattening' layer is one preserves3D() == false.
// transformState.m_accumulatedTransform holds the transform from the containing flattening layer.
// transformState.m_lastPlanarPoint is the hitTestLocation in the plane of the containing flattening layer.
// transformState.m_lastPlanarQuad is the hitTestRect as a quad in the plane of the containing flattening layer.
//
// If zOffset is non-null (which indicates that the caller wants z offset information),
//  *zOffset on return is the z offset of the hit point relative to the containing flattening layer.
DeprecatedPaintLayer* DeprecatedPaintLayer::hitTestLayer(DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* containerLayer, HitTestResult& result,
    const LayoutRect& hitTestRect, const HitTestLocation& hitTestLocation, bool appliedTransform,
    const HitTestingTransformState* transformState, double* zOffset)
{
    ASSERT(layoutObject()->document().lifecycle().state() >= DocumentLifecycle::CompositingClean);

    if (!isSelfPaintingLayer() && !hasSelfPaintingLayerDescendant())
        return 0;

    ClipRectsCacheSlot clipRectsCacheSlot = result.hitTestRequest().ignoreClipping() ? RootRelativeClipRectsIgnoringViewportClip : RootRelativeClipRects;

    // Apply a transform if we have one.
    if (transform() && !appliedTransform) {
        if (enclosingPaginationLayer())
            return hitTestTransformedLayerInFragments(rootLayer, containerLayer, result, hitTestRect, hitTestLocation, transformState, zOffset, clipRectsCacheSlot);

        // Make sure the parent's clip rects have been calculated.
        if (parent()) {
            ClipRect clipRect = clipper().backgroundClipRect(ClipRectsContext(rootLayer, clipRectsCacheSlot, IncludeOverlayScrollbarSize));
            // Go ahead and test the enclosing clip now.
            if (!clipRect.intersects(hitTestLocation))
                return 0;
        }

        return hitTestLayerByApplyingTransform(rootLayer, containerLayer, result, hitTestRect, hitTestLocation, transformState, zOffset);
    }

    // Ensure our lists and 3d status are up-to-date.
    m_stackingNode->updateLayerListsIfNeeded();
    update3DTransformedDescendantStatus();

    // The natural thing would be to keep HitTestingTransformState on the stack, but it's big, so we heap-allocate.
    RefPtr<HitTestingTransformState> localTransformState;
    if (appliedTransform) {
        // We computed the correct state in the caller (above code), so just reference it.
        ASSERT(transformState);
        localTransformState = const_cast<HitTestingTransformState*>(transformState);
    } else if (transformState || m_has3DTransformedDescendant || preserves3D()) {
        // We need transform state for the first time, or to offset the container state, so create it here.
        localTransformState = createLocalTransformState(rootLayer, containerLayer, hitTestRect, hitTestLocation, transformState);
    }

    // Check for hit test on backface if backface-visibility is 'hidden'
    if (localTransformState && layoutObject()->style()->backfaceVisibility() == BackfaceVisibilityHidden) {
        TransformationMatrix invertedMatrix = localTransformState->m_accumulatedTransform.inverse();
        // If the z-vector of the matrix is negative, the back is facing towards the viewer.
        if (invertedMatrix.m33() < 0)
            return 0;
    }

    RefPtr<HitTestingTransformState> unflattenedTransformState = localTransformState;
    if (localTransformState && !preserves3D()) {
        // Keep a copy of the pre-flattening state, for computing z-offsets for the container
        unflattenedTransformState = HitTestingTransformState::create(*localTransformState);
        // This layer is flattening, so flatten the state passed to descendants.
        localTransformState->flatten();
    }

    // The following are used for keeping track of the z-depth of the hit point of 3d-transformed
    // descendants.
    double localZOffset = -std::numeric_limits<double>::infinity();
    double* zOffsetForDescendantsPtr = 0;
    double* zOffsetForContentsPtr = 0;

    bool depthSortDescendants = false;
    if (preserves3D()) {
        depthSortDescendants = true;
        // Our layers can depth-test with our container, so share the z depth pointer with the container, if it passed one down.
        zOffsetForDescendantsPtr = zOffset ? zOffset : &localZOffset;
        zOffsetForContentsPtr = zOffset ? zOffset : &localZOffset;
    } else if (zOffset) {
        zOffsetForDescendantsPtr = 0;
        // Container needs us to give back a z offset for the hit layer.
        zOffsetForContentsPtr = zOffset;
    }

    // This variable tracks which layer the mouse ends up being inside.
    DeprecatedPaintLayer* candidateLayer = 0;

    // Begin by walking our list of positive layers from highest z-index down to the lowest z-index.
    DeprecatedPaintLayer* hitLayer = hitTestChildren(PositiveZOrderChildren, rootLayer, result, hitTestRect, hitTestLocation,
        localTransformState.get(), zOffsetForDescendantsPtr, zOffset, unflattenedTransformState.get(), depthSortDescendants);
    if (hitLayer) {
        if (!depthSortDescendants)
            return hitLayer;
        candidateLayer = hitLayer;
    }

    // Now check our overflow objects.
    hitLayer = hitTestChildren(NormalFlowChildren, rootLayer, result, hitTestRect, hitTestLocation,
        localTransformState.get(), zOffsetForDescendantsPtr, zOffset, unflattenedTransformState.get(), depthSortDescendants);
    if (hitLayer) {
        if (!depthSortDescendants)
            return hitLayer;
        candidateLayer = hitLayer;
    }

    // Collect the fragments. This will compute the clip rectangles for each layer fragment.
    DeprecatedPaintLayerFragments layerFragments;
    if (appliedTransform)
        appendSingleFragmentIgnoringPagination(layerFragments, rootLayer, hitTestRect, clipRectsCacheSlot, IncludeOverlayScrollbarSize);
    else
        collectFragments(layerFragments, rootLayer, hitTestRect, clipRectsCacheSlot, IncludeOverlayScrollbarSize);

    if (m_scrollableArea && m_scrollableArea->hitTestResizerInFragments(layerFragments, hitTestLocation)) {
        layoutObject()->updateHitTestResult(result, hitTestLocation.point());
        return this;
    }

    // Next we want to see if the mouse pos is inside the child LayoutObjects of the layer. Check
    // every fragment in reverse order.
    if (isSelfPaintingLayer()) {
        // Hit test with a temporary HitTestResult, because we only want to commit to 'result' if we know we're frontmost.
        HitTestResult tempResult(result.hitTestRequest(), result.hitTestLocation());
        bool insideFragmentForegroundRect = false;
        if (hitTestContentsForFragments(layerFragments, tempResult, hitTestLocation, HitTestDescendants, insideFragmentForegroundRect)
            && isHitCandidate(this, false, zOffsetForContentsPtr, unflattenedTransformState.get())) {
            if (result.hitTestRequest().listBased())
                result.append(tempResult);
            else
                result = tempResult;
            if (!depthSortDescendants)
                return this;
            // Foreground can depth-sort with descendant layers, so keep this as a candidate.
            candidateLayer = this;
        } else if (insideFragmentForegroundRect && result.hitTestRequest().listBased()) {
            result.append(tempResult);
        }
    }

    // Now check our negative z-index children.
    hitLayer = hitTestChildren(NegativeZOrderChildren, rootLayer, result, hitTestRect, hitTestLocation,
        localTransformState.get(), zOffsetForDescendantsPtr, zOffset, unflattenedTransformState.get(), depthSortDescendants);
    if (hitLayer) {
        if (!depthSortDescendants)
            return hitLayer;
        candidateLayer = hitLayer;
    }

    // If we found a layer, return. Child layers, and foreground always render in front of background.
    if (candidateLayer)
        return candidateLayer;

    if (isSelfPaintingLayer()) {
        HitTestResult tempResult(result.hitTestRequest(), result.hitTestLocation());
        bool insideFragmentBackgroundRect = false;
        if (hitTestContentsForFragments(layerFragments, tempResult, hitTestLocation, HitTestSelf, insideFragmentBackgroundRect)
            && isHitCandidate(this, false, zOffsetForContentsPtr, unflattenedTransformState.get())) {
            if (result.isRectBasedTest())
                result.append(tempResult);
            else
                result = tempResult;
            return this;
        }
        if (insideFragmentBackgroundRect && result.hitTestRequest().listBased())
            result.append(tempResult);
    }

    return 0;
}

bool DeprecatedPaintLayer::hitTestContentsForFragments(const DeprecatedPaintLayerFragments& layerFragments, HitTestResult& result,
    const HitTestLocation& hitTestLocation, HitTestFilter hitTestFilter, bool& insideClipRect) const
{
    if (layerFragments.isEmpty())
        return false;

    for (int i = layerFragments.size() - 1; i >= 0; --i) {
        const DeprecatedPaintLayerFragment& fragment = layerFragments.at(i);
        if ((hitTestFilter == HitTestSelf && !fragment.backgroundRect.intersects(hitTestLocation))
            || (hitTestFilter == HitTestDescendants && !fragment.foregroundRect.intersects(hitTestLocation)))
            continue;
        insideClipRect = true;
        if (hitTestContents(result, fragment.layerBounds, hitTestLocation, hitTestFilter))
            return true;
    }

    return false;
}

DeprecatedPaintLayer* DeprecatedPaintLayer::hitTestTransformedLayerInFragments(DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* containerLayer, HitTestResult& result,
    const LayoutRect& hitTestRect, const HitTestLocation& hitTestLocation, const HitTestingTransformState* transformState, double* zOffset, ClipRectsCacheSlot clipRectsCacheSlot)
{
    DeprecatedPaintLayerFragments enclosingPaginationFragments;
    LayoutPoint offsetOfPaginationLayerFromRoot;
    // FIXME: We're missing a sub-pixel offset here crbug.com/348728
    LayoutRect transformedExtent = transparencyClipBox(this, enclosingPaginationLayer(), HitTestingTransparencyClipBox, DeprecatedPaintLayer::RootOfTransparencyClipBox, LayoutSize());
    enclosingPaginationLayer()->collectFragments(enclosingPaginationFragments, rootLayer, hitTestRect,
        clipRectsCacheSlot, IncludeOverlayScrollbarSize, RespectOverflowClip, &offsetOfPaginationLayerFromRoot, LayoutSize(), &transformedExtent);

    for (int i = enclosingPaginationFragments.size() - 1; i >= 0; --i) {
        const DeprecatedPaintLayerFragment& fragment = enclosingPaginationFragments.at(i);

        // Apply the page/column clip for this fragment, as well as any clips established by layers in between us and
        // the enclosing pagination layer.
        LayoutRect clipRect = fragment.backgroundRect.rect();

        // Now compute the clips within a given fragment
        if (parent() != enclosingPaginationLayer()) {
            enclosingPaginationLayer()->convertToLayerCoords(rootLayer, offsetOfPaginationLayerFromRoot);
            LayoutRect parentClipRect = clipper().backgroundClipRect(ClipRectsContext(enclosingPaginationLayer(), clipRectsCacheSlot, IncludeOverlayScrollbarSize)).rect();
            parentClipRect.moveBy(fragment.paginationOffset + offsetOfPaginationLayerFromRoot);
            clipRect.intersect(parentClipRect);
        }

        if (!hitTestLocation.intersects(clipRect))
            continue;

        DeprecatedPaintLayer* hitLayer = hitTestLayerByApplyingTransform(rootLayer, containerLayer, result, hitTestRect, hitTestLocation,
            transformState, zOffset, fragment.paginationOffset);
        if (hitLayer)
            return hitLayer;
    }

    return 0;
}

DeprecatedPaintLayer* DeprecatedPaintLayer::hitTestLayerByApplyingTransform(DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* containerLayer, HitTestResult& result,
    const LayoutRect& hitTestRect, const HitTestLocation& hitTestLocation, const HitTestingTransformState* transformState, double* zOffset,
    const LayoutPoint& translationOffset)
{
    // Create a transform state to accumulate this transform.
    RefPtr<HitTestingTransformState> newTransformState = createLocalTransformState(rootLayer, containerLayer, hitTestRect, hitTestLocation, transformState, translationOffset);

    // If the transform can't be inverted, then don't hit test this layer at all.
    if (!newTransformState->m_accumulatedTransform.isInvertible())
        return 0;

    // Compute the point and the hit test rect in the coords of this layer by using the values
    // from the transformState, which store the point and quad in the coords of the last flattened
    // layer, and the accumulated transform which lets up map through preserve-3d layers.
    //
    // We can't just map hitTestLocation and hitTestRect because they may have been flattened (losing z)
    // by our container.
    FloatPoint localPoint = newTransformState->mappedPoint();
    FloatQuad localPointQuad = newTransformState->mappedQuad();
    LayoutRect localHitTestRect = newTransformState->boundsOfMappedArea();
    HitTestLocation newHitTestLocation;
    if (hitTestLocation.isRectBasedTest())
        newHitTestLocation = HitTestLocation(localPoint, localPointQuad);
    else
        newHitTestLocation = HitTestLocation(localPoint);

    // Now do a hit test with the root layer shifted to be us.
    return hitTestLayer(this, containerLayer, result, localHitTestRect, newHitTestLocation, true, newTransformState.get(), zOffset);
}

bool DeprecatedPaintLayer::hitTestContents(HitTestResult& result, const LayoutRect& layerBounds, const HitTestLocation& hitTestLocation, HitTestFilter hitTestFilter) const
{
    ASSERT(isSelfPaintingLayer() || hasSelfPaintingLayerDescendant());

    if (!layoutObject()->hitTest(result, hitTestLocation, toLayoutPoint(layerBounds.location() - layoutBoxLocation()), hitTestFilter)) {
        // It's wrong to set innerNode, but then claim that you didn't hit anything, unless it is
        // a rect-based test.
        ASSERT(!result.innerNode() || (result.hitTestRequest().listBased() && result.listBasedTestResult().size()));
        return false;
    }

    if (!result.innerNode()) {
        // We hit something anonymous, and we didn't find a DOM node ancestor in this layer.

        if (layoutObject()->isLayoutFlowThread()) {
            // For a flow thread it's safe to just say that we didn't hit anything. That means that
            // we'll continue as normally, and eventually hit a column set sibling instead. Column
            // sets are also anonymous, but, unlike flow threads, they don't establish layers, so
            // we'll fall back and hit the multicol container parent (which should have a DOM node).
            return false;
        }

        Node* e = enclosingElement();
        // FIXME: should be a call to result.setNodeAndPosition. What we would really want to do here is to
        // return and look for the nearest non-anonymous ancestor, and ignore aunts and uncles on
        // our way. It's bad to look for it manually like we do here, and give up on setting a local
        // point in the result, because that has bad implications for text selection and
        // caretRangeFromPoint(). See crbug.com/461791
        if (!result.innerNode())
            result.setInnerNode(e);

    }
    return true;
}

DeprecatedPaintLayer* DeprecatedPaintLayer::hitTestChildren(ChildrenIteration childrentoVisit, DeprecatedPaintLayer* rootLayer,
    HitTestResult& result,
    const LayoutRect& hitTestRect, const HitTestLocation& hitTestLocation,
    const HitTestingTransformState* transformState,
    double* zOffsetForDescendants, double* zOffset,
    const HitTestingTransformState* unflattenedTransformState,
    bool depthSortDescendants)
{
    if (!hasSelfPaintingLayerDescendant())
        return 0;

    DeprecatedPaintLayer* resultLayer = 0;
    DeprecatedPaintLayerStackingNodeReverseIterator iterator(*m_stackingNode, childrentoVisit);
    while (DeprecatedPaintLayerStackingNode* child = iterator.next()) {
        DeprecatedPaintLayer* childLayer = child->layer();
        DeprecatedPaintLayer* hitLayer = 0;
        HitTestResult tempResult(result.hitTestRequest(), result.hitTestLocation());
        hitLayer = childLayer->hitTestLayer(rootLayer, this, tempResult, hitTestRect, hitTestLocation, false, transformState, zOffsetForDescendants);

        // If it is a list-based test, we can safely append the temporary result since it might had hit
        // nodes but not necesserily had hitLayer set.
        ASSERT(!result.isRectBasedTest() || result.hitTestRequest().listBased());
        if (result.hitTestRequest().listBased())
            result.append(tempResult);

        if (isHitCandidate(hitLayer, depthSortDescendants, zOffset, unflattenedTransformState)) {
            resultLayer = hitLayer;
            if (!result.hitTestRequest().listBased())
                result = tempResult;
            if (!depthSortDescendants)
                break;
        }
    }

    return resultLayer;
}

void DeprecatedPaintLayer::blockSelectionGapsBoundsChanged()
{
    setNeedsCompositingInputsUpdate();
}

void DeprecatedPaintLayer::addBlockSelectionGapsBounds(const LayoutRect& bounds)
{
    m_blockSelectionGapsBounds.unite(enclosingIntRect(bounds));
    blockSelectionGapsBoundsChanged();
}

void DeprecatedPaintLayer::clearBlockSelectionGapsBounds()
{
    m_blockSelectionGapsBounds = IntRect();
    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling())
        child->clearBlockSelectionGapsBounds();
    blockSelectionGapsBoundsChanged();
}

void DeprecatedPaintLayer::invalidatePaintForBlockSelectionGaps()
{
    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling()) {
        // FIXME: We should not allow paint invalidation out of paint invalidation state. crbug.com/457415
        DisablePaintInvalidationStateAsserts disabler;
        child->invalidatePaintForBlockSelectionGaps();
    }

    if (m_blockSelectionGapsBounds.isEmpty())
        return;

    LayoutRect rect(m_blockSelectionGapsBounds);
    if (layoutObject()->hasOverflowClip()) {
        LayoutBox* box = layoutBox();
        rect.move(-box->scrolledContentOffset());
        if (!scrollableArea()->usesCompositedScrolling())
            rect.intersect(box->overflowClipRect(LayoutPoint()));
    }
    if (layoutObject()->hasClip())
        rect.intersect(toLayoutBox(layoutObject())->clipRect(LayoutPoint()));
    if (!rect.isEmpty()) {
        // FIXME: We should not allow paint invalidation out of paint invalidation state. crbug.com/457415
        DisablePaintInvalidationStateAsserts disabler;
        layoutObject()->invalidatePaintRectangle(rect);
    }
}

IntRect DeprecatedPaintLayer::blockSelectionGapsBounds() const
{
    if (!layoutObject()->isLayoutBlockFlow())
        return IntRect();

    LayoutBlockFlow* layoutBlockFlow = toLayoutBlockFlow(layoutObject());
    LayoutRect gapRects = layoutBlockFlow->selectionGapRectsForPaintInvalidation(layoutBlockFlow);

    return pixelSnappedIntRect(gapRects);
}

bool DeprecatedPaintLayer::hasBlockSelectionGapBounds() const
{
    // FIXME: it would be more accurate to return !blockSelectionGapsBounds().isEmpty(), but this is impossible
    // at the moment because it causes invalid queries to layout-dependent code (crbug.com/372802).
    // ASSERT(layoutObject()->document().lifecycle().state() >= DocumentLifecycle::LayoutClean);

    if (!layoutObject()->isLayoutBlock())
        return false;

    return toLayoutBlock(layoutObject())->shouldPaintSelectionGaps();
}

bool DeprecatedPaintLayer::intersectsDamageRect(const LayoutRect& layerBounds, const LayoutRect& damageRect, const DeprecatedPaintLayer* rootLayer, const LayoutPoint* offsetFromRoot) const
{
    // Always examine the canvas and the root.
    // FIXME: Could eliminate the isDocumentElement() check if we fix background painting so that the LayoutView
    // paints the root's background.
    if (isRootLayer() || layoutObject()->isDocumentElement())
        return true;

    // If we aren't an inline flow, and our layer bounds do intersect the damage rect, then we
    // can go ahead and return true.
    LayoutView* view = layoutObject()->view();
    ASSERT(view);
    if (view && !layoutObject()->isLayoutInline()) {
        if (layerBounds.intersects(damageRect))
            return true;
    }

    // Otherwise we need to compute the bounding box of this single layer and see if it intersects
    // the damage rect.
    return physicalBoundingBox(rootLayer, offsetFromRoot).intersects(damageRect);
}

LayoutRect DeprecatedPaintLayer::logicalBoundingBox() const
{
    // There are three special cases we need to consider.
    // (1) Inline Flows.  For inline flows we will create a bounding box that fully encompasses all of the lines occupied by the
    // inline.  In other words, if some <span> wraps to three lines, we'll create a bounding box that fully encloses the
    // line boxes of all three lines (including overflow on those lines).
    // (2) Left/Top Overflow.  The width/height of layers already includes right/bottom overflow.  However, in the case of left/top
    // overflow, we have to create a bounding box that will extend to include this overflow.
    // (3) Floats.  When a layer has overhanging floats that it paints, we need to make sure to include these overhanging floats
    // as part of our bounding box.  We do this because we are the responsible layer for both hit testing and painting those
    // floats.
    LayoutRect result;
    if (layoutObject()->isInline() && layoutObject()->isLayoutInline()) {
        result = toLayoutInline(layoutObject())->linesVisualOverflowBoundingBox();
    } else if (layoutObject()->isTableRow()) {
        // Our bounding box is just the union of all of our cells' border/overflow rects.
        for (LayoutObject* child = layoutObject()->slowFirstChild(); child; child = child->nextSibling()) {
            if (child->isTableCell()) {
                LayoutRect bbox = toLayoutBox(child)->borderBoxRect();
                result.unite(bbox);
                LayoutRect overflowRect = layoutBox()->visualOverflowRect();
                if (bbox != overflowRect)
                    result.unite(overflowRect);
            }
        }
    } else {
        LayoutBox* box = layoutBox();
        ASSERT(box);
        result = box->borderBoxRect();
        result.unite(box->visualOverflowRect());
    }

    ASSERT(layoutObject()->view());
    return result;
}

static inline LayoutRect flippedLogicalBoundingBox(LayoutRect boundingBox, LayoutObject* layoutObjects)
{
    LayoutRect result = boundingBox;
    if (layoutObjects->isBox())
        toLayoutBox(layoutObjects)->flipForWritingMode(result);
    else
        layoutObjects->containingBlock()->flipForWritingMode(result);
    return result;
}

LayoutRect DeprecatedPaintLayer::physicalBoundingBox(const DeprecatedPaintLayer* ancestorLayer, const LayoutPoint* offsetFromRoot) const
{
    LayoutRect result = flippedLogicalBoundingBox(logicalBoundingBox(), layoutObject());
    if (offsetFromRoot)
        result.moveBy(*offsetFromRoot);
    else
        convertToLayerCoords(ancestorLayer, result);
    return result;
}

LayoutRect DeprecatedPaintLayer::fragmentsBoundingBox(const DeprecatedPaintLayer* ancestorLayer) const
{
    if (!enclosingPaginationLayer())
        return physicalBoundingBox(ancestorLayer);

    LayoutRect result = flippedLogicalBoundingBox(logicalBoundingBox(), layoutObject());
    convertFromFlowThreadToVisualBoundingBoxInAncestor(this, ancestorLayer, result);
    return result;
}

LayoutRect DeprecatedPaintLayer::boundingBoxForCompositingOverlapTest() const
{
    return overlapBoundsIncludeChildren() ? boundingBoxForCompositing() : fragmentsBoundingBox(this);
}

static void expandRectForReflectionAndStackingChildren(const DeprecatedPaintLayer* ancestorLayer, DeprecatedPaintLayer::CalculateBoundsOptions options, LayoutRect& result)
{
    if (ancestorLayer->reflectionInfo() && !ancestorLayer->reflectionInfo()->reflectionLayer()->hasCompositedDeprecatedPaintLayerMapping())
        result.unite(ancestorLayer->reflectionInfo()->reflectionLayer()->boundingBoxForCompositing(ancestorLayer));

    ASSERT(ancestorLayer->stackingNode()->isStackingContext() || !ancestorLayer->stackingNode()->hasPositiveZOrderList());

#if ENABLE(ASSERT)
    LayerListMutationDetector mutationChecker(const_cast<DeprecatedPaintLayer*>(ancestorLayer)->stackingNode());
#endif

    DeprecatedPaintLayerStackingNodeIterator iterator(*ancestorLayer->stackingNode(), AllChildren);
    while (DeprecatedPaintLayerStackingNode* node = iterator.next()) {
        // Here we exclude both directly composited layers and squashing layers
        // because those Layers don't paint into the graphics layer
        // for this Layer. For example, the bounds of squashed Layers
        // will be included in the computation of the appropriate squashing
        // GraphicsLayer.
        if (options != DeprecatedPaintLayer::ApplyBoundsChickenEggHacks && node->layer()->compositingState() != NotComposited)
            continue;
        result.unite(node->layer()->boundingBoxForCompositing(ancestorLayer, options));
    }
}

LayoutRect DeprecatedPaintLayer::physicalBoundingBoxIncludingReflectionAndStackingChildren(const DeprecatedPaintLayer* ancestorLayer, const LayoutPoint& offsetFromRoot) const
{
    LayoutPoint origin;
    LayoutRect result = physicalBoundingBox(ancestorLayer, &origin);

    const_cast<DeprecatedPaintLayer*>(this)->stackingNode()->updateLayerListsIfNeeded();

    expandRectForReflectionAndStackingChildren(this, DoNotApplyBoundsChickenEggHacks, result);

    result.moveBy(offsetFromRoot);
    return result;
}

LayoutRect DeprecatedPaintLayer::boundingBoxForCompositing(const DeprecatedPaintLayer* ancestorLayer, CalculateBoundsOptions options) const
{
    if (!isSelfPaintingLayer())
        return LayoutRect();

    if (!ancestorLayer)
        ancestorLayer = this;

    // FIXME: This could be improved to do a check like hasVisibleNonCompositingDescendantLayers() (bug 92580).
    if (this != ancestorLayer && !hasVisibleContent() && !hasVisibleDescendant())
        return LayoutRect();

    // The root layer is always just the size of the document.
    if (isRootLayer())
        return LayoutRect(m_layoutObject->view()->unscaledDocumentRect());

    // The layer created for the LayoutFlowThread is just a helper for painting and hit-testing,
    // and should not contribute to the bounding box. The LayoutMultiColumnSets will contribute
    // the correct size for the layout content of the multicol container.
    if (layoutObject()->isLayoutFlowThread())
        return LayoutRect();

    LayoutRect result = clipper().localClipRect();
    if (result == LayoutRect::infiniteIntRect()) {
        LayoutPoint origin;
        result = physicalBoundingBox(ancestorLayer, &origin);

        const_cast<DeprecatedPaintLayer*>(this)->stackingNode()->updateLayerListsIfNeeded();

        // Reflections are implemented with Layers that hang off of the reflected layer. However,
        // the reflection layer subtree does not include the subtree of the parent Layer, so
        // a recursive computation of stacking children yields no results. This breaks cases when there are stacking
        // children of the parent, that need to be included in reflected composited bounds.
        // Fix this by including composited bounds of stacking children of the reflected Layer.
        if (hasCompositedDeprecatedPaintLayerMapping() && parent() && parent()->reflectionInfo() && parent()->reflectionInfo()->reflectionLayer() == this)
            expandRectForReflectionAndStackingChildren(parent(), options, result);
        else
            expandRectForReflectionAndStackingChildren(this, options, result);

        // FIXME: We can optimize the size of the composited layers, by not enlarging
        // filtered areas with the outsets if we know that the filter is going to render in hardware.
        // https://bugs.webkit.org/show_bug.cgi?id=81239
        result.expand(m_layoutObject->style()->filterOutsets());
    }

    if (transform() && (paintsWithTransform(PaintBehaviorNormal) || options == ApplyBoundsChickenEggHacks))
        result = transform()->mapRect(result);

    if (enclosingPaginationLayer()) {
        convertFromFlowThreadToVisualBoundingBoxInAncestor(this, ancestorLayer, result);
        return result;
    }
    LayoutPoint delta;
    convertToLayerCoords(ancestorLayer, delta);
    result.moveBy(delta);
    return result;
}

CompositingState DeprecatedPaintLayer::compositingState() const
{
    ASSERT(isAllowedToQueryCompositingState());

    // This is computed procedurally so there is no redundant state variable that
    // can get out of sync from the real actual compositing state.

    if (m_groupedMapping) {
        ASSERT(!m_compositedDeprecatedPaintLayerMapping);
        return PaintsIntoGroupedBacking;
    }

    if (!m_compositedDeprecatedPaintLayerMapping)
        return NotComposited;

    return PaintsIntoOwnBacking;
}

bool DeprecatedPaintLayer::isAllowedToQueryCompositingState() const
{
    if (gCompositingQueryMode == CompositingQueriesAreAllowed)
        return true;
    return layoutObject()->document().lifecycle().state() >= DocumentLifecycle::InCompositingUpdate;
}

CompositedDeprecatedPaintLayerMapping* DeprecatedPaintLayer::compositedDeprecatedPaintLayerMapping() const
{
    ASSERT(isAllowedToQueryCompositingState());
    return m_compositedDeprecatedPaintLayerMapping.get();
}

GraphicsLayer* DeprecatedPaintLayer::graphicsLayerBacking() const
{
    switch (compositingState()) {
    case NotComposited:
        return 0;
    case PaintsIntoGroupedBacking:
        return groupedMapping()->squashingLayer();
    default:
        return compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer();
    }
}

GraphicsLayer* DeprecatedPaintLayer::graphicsLayerBackingForScrolling() const
{
    switch (compositingState()) {
    case NotComposited:
        return 0;
    case PaintsIntoGroupedBacking:
        return groupedMapping()->squashingLayer();
    default:
        return compositedDeprecatedPaintLayerMapping()->scrollingContentsLayer() ? compositedDeprecatedPaintLayerMapping()->scrollingContentsLayer() : compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer();
    }
}

void DeprecatedPaintLayer::ensureCompositedDeprecatedPaintLayerMapping()
{
    if (m_compositedDeprecatedPaintLayerMapping)
        return;

    m_compositedDeprecatedPaintLayerMapping = adoptPtr(new CompositedDeprecatedPaintLayerMapping(*this));
    m_compositedDeprecatedPaintLayerMapping->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateSubtree);

    updateOrRemoveFilterEffectBuilder();
}

void DeprecatedPaintLayer::clearCompositedDeprecatedPaintLayerMapping(bool layerBeingDestroyed)
{
    if (!layerBeingDestroyed) {
        // We need to make sure our decendants get a geometry update. In principle,
        // we could call setNeedsGraphicsLayerUpdate on our children, but that would
        // require walking the z-order lists to find them. Instead, we over-invalidate
        // by marking our parent as needing a geometry update.
        if (DeprecatedPaintLayer* compositingParent = enclosingLayerWithCompositedDeprecatedPaintLayerMapping(ExcludeSelf))
            compositingParent->compositedDeprecatedPaintLayerMapping()->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateSubtree);
    }

    m_compositedDeprecatedPaintLayerMapping.clear();

    if (!layerBeingDestroyed)
        updateOrRemoveFilterEffectBuilder();
}

void DeprecatedPaintLayer::setGroupedMapping(CompositedDeprecatedPaintLayerMapping* groupedMapping, SetGroupMappingOptions options)
{
    if (groupedMapping == m_groupedMapping)
        return;

    if (options == InvalidateLayerAndRemoveFromMapping && m_groupedMapping) {
        m_groupedMapping->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateSubtree);
        m_groupedMapping->removeLayerFromSquashingGraphicsLayer(this);
    }
    m_groupedMapping = groupedMapping;
    ASSERT(!m_groupedMapping || m_groupedMapping->verifyLayerInSquashingVector(this));
    if (options == InvalidateLayerAndRemoveFromMapping && m_groupedMapping)
        m_groupedMapping->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateSubtree);
}

bool DeprecatedPaintLayer::hasCompositedMask() const
{
    return m_compositedDeprecatedPaintLayerMapping && m_compositedDeprecatedPaintLayerMapping->hasMaskLayer();
}

bool DeprecatedPaintLayer::hasCompositedClippingMask() const
{
    return m_compositedDeprecatedPaintLayerMapping && m_compositedDeprecatedPaintLayerMapping->hasChildClippingMaskLayer();
}

bool DeprecatedPaintLayer::paintsWithTransform(PaintBehavior paintBehavior) const
{
    return (transform() || layoutObject()->style()->position() == FixedPosition) && ((paintBehavior & PaintBehaviorFlattenCompositingLayers) || compositingState() != PaintsIntoOwnBacking);
}

bool DeprecatedPaintLayer::backgroundIsKnownToBeOpaqueInRect(const LayoutRect& localRect) const
{
    if (!isSelfPaintingLayer() && !hasSelfPaintingLayerDescendant())
        return false;

    if (paintsWithTransparency(PaintBehaviorNormal))
        return false;

    // We can't use hasVisibleContent(), because that will be true if our layoutObject is hidden, but some child
    // is visible and that child doesn't cover the entire rect.
    if (layoutObject()->style()->visibility() != VISIBLE)
        return false;

    if (paintsWithFilters() && layoutObject()->style()->filter().hasFilterThatAffectsOpacity())
        return false;

    // FIXME: Handle simple transforms.
    if (paintsWithTransform(PaintBehaviorNormal))
        return false;

    // FIXME: Remove this check.
    // This function should not be called when layer-lists are dirty.
    // It is somehow getting triggered during style update.
    if (m_stackingNode->zOrderListsDirty())
        return false;

    // FIXME: We currently only check the immediate layoutObject,
    // which will miss many cases.
    if (layoutObject()->backgroundIsKnownToBeOpaqueInRect(localRect))
        return true;

    // We can't consult child layers if we clip, since they might cover
    // parts of the rect that are clipped out.
    if (layoutObject()->hasOverflowClip())
        return false;

    return childBackgroundIsKnownToBeOpaqueInRect(localRect);
}

bool DeprecatedPaintLayer::childBackgroundIsKnownToBeOpaqueInRect(const LayoutRect& localRect) const
{
    DeprecatedPaintLayerStackingNodeReverseIterator revertseIterator(*m_stackingNode, PositiveZOrderChildren | NormalFlowChildren | NegativeZOrderChildren);
    while (DeprecatedPaintLayerStackingNode* child = revertseIterator.next()) {
        const DeprecatedPaintLayer* childLayer = child->layer();
        // Stop at composited paint boundaries.
        if (childLayer->isPaintInvalidationContainer())
            continue;

        if (!childLayer->canUseConvertToLayerCoords())
            continue;

        LayoutPoint childOffset;
        LayoutRect childLocalRect(localRect);
        childLayer->convertToLayerCoords(this, childOffset);
        childLocalRect.moveBy(-childOffset);

        if (childLayer->backgroundIsKnownToBeOpaqueInRect(childLocalRect))
            return true;
    }
    return false;
}

bool DeprecatedPaintLayer::shouldBeSelfPaintingLayer() const
{
    if (layoutObject()->isLayoutPart() && toLayoutPart(layoutObject())->requiresAcceleratedCompositing())
        return true;
    return m_layerType == NormalDeprecatedPaintLayer
        || (m_scrollableArea && m_scrollableArea->hasOverlayScrollbars())
        || needsCompositedScrolling();
}

void DeprecatedPaintLayer::updateSelfPaintingLayer()
{
    bool isSelfPaintingLayer = shouldBeSelfPaintingLayer();
    if (this->isSelfPaintingLayer() == isSelfPaintingLayer)
        return;

    m_isSelfPaintingLayer = isSelfPaintingLayer;

    if (parent())
        parent()->dirtyAncestorChainHasSelfPaintingLayerDescendantStatus();
}

bool DeprecatedPaintLayer::hasNonEmptyChildLayoutObjects() const
{
    // Some HTML can cause whitespace text nodes to have layoutObjects, like:
    // <div>
    // <img src=...>
    // </div>
    // so test for 0x0 LayoutTexts here
    for (LayoutObject* child = layoutObject()->slowFirstChild(); child; child = child->nextSibling()) {
        if (!child->hasLayer()) {
            if (child->isLayoutInline() || !child->isBox())
                return true;

            if (toLayoutBox(child)->size().width() > 0 || toLayoutBox(child)->size().height() > 0)
                return true;
        }
    }
    return false;
}

bool DeprecatedPaintLayer::hasBoxDecorationsOrBackground() const
{
    return layoutObject()->style()->hasBoxDecorations() || layoutObject()->style()->hasBackground();
}

bool DeprecatedPaintLayer::hasVisibleBoxDecorations() const
{
    if (!hasVisibleContent())
        return false;

    return hasBoxDecorationsOrBackground() || hasOverflowControls();
}

void DeprecatedPaintLayer::updateFilters(const ComputedStyle* oldStyle, const ComputedStyle& newStyle)
{
    if (!newStyle.hasFilter() && (!oldStyle || !oldStyle->hasFilter()))
        return;

    updateOrRemoveFilterClients();
    updateOrRemoveFilterEffectBuilder();
}

bool DeprecatedPaintLayer::attemptDirectCompositingUpdate(StyleDifference diff, const ComputedStyle* oldStyle)
{
    CompositingReasons oldPotentialCompositingReasonsFromStyle = m_potentialCompositingReasonsFromStyle;
    compositor()->updatePotentialCompositingReasonsFromStyle(this);

    // This function implements an optimization for transforms and opacity.
    // A common pattern is for a touchmove handler to update the transform
    // and/or an opacity of an element every frame while the user moves their
    // finger across the screen. The conditions below recognize when the
    // compositing state is set up to receive a direct transform or opacity
    // update.

    if (!diff.hasAtMostPropertySpecificDifferences(StyleDifference::TransformChanged | StyleDifference::OpacityChanged))
        return false;
    // The potentialCompositingReasonsFromStyle could have changed without
    // a corresponding StyleDifference if an animation started or ended.
    if (m_potentialCompositingReasonsFromStyle != oldPotentialCompositingReasonsFromStyle)
        return false;
    // We could add support for reflections if we updated the transform on
    // the reflection layers.
    if (layoutObject()->hasReflection())
        return false;
    // If we're unwinding a scheduleSVGFilterLayerUpdateHack(), then we can't
    // perform a direct compositing update because the filters code is going
    // to produce different output this time around. We can remove this code
    // once we fix the chicken/egg bugs in the filters code and delete the
    // scheduleSVGFilterLayerUpdateHack().
    if (layoutObject()->node() && layoutObject()->node()->svgFilterNeedsLayerUpdate())
        return false;
    if (!m_compositedDeprecatedPaintLayerMapping)
        return false;

    // To cut off almost all the work in the compositing update for
    // this case, we treat inline transforms has having assumed overlap
    // (similar to how we treat animated transforms). Notice that we read
    // CompositingReasonInlineTransform from the m_compositingReasons, which
    // means that the inline transform actually triggered assumed overlap in
    // the overlap map.
    if (diff.transformChanged() && !(m_compositingReasons & CompositingReasonInlineTransform))
        return false;

    // We composite transparent Layers differently from non-transparent
    // Layers even when the non-transparent Layers are already a
    // stacking context.
    if (diff.opacityChanged() && m_layoutObject->style()->hasOpacity() != oldStyle->hasOpacity())
        return false;

    // Changes in pointer-events affect hit test visibility of the scrollable
    // area and its |m_scrollsOverflow| value which determines if the layer
    // requires composited scrolling or not.
    if (m_scrollableArea && m_layoutObject->style()->pointerEvents() != oldStyle->pointerEvents())
        return false;

    updateTransform(oldStyle, layoutObject()->styleRef());

    // FIXME: Consider introducing a smaller graphics layer update scope
    // that just handles transforms and opacity. GraphicsLayerUpdateLocal
    // will also program bounds, clips, and many other properties that could
    // not possibly have changed.
    m_compositedDeprecatedPaintLayerMapping->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateLocal);
    compositor()->setNeedsCompositingUpdate(CompositingUpdateAfterGeometryChange);

    if (m_scrollableArea)
        m_scrollableArea->updateAfterStyleChange(oldStyle);

    return true;
}

void DeprecatedPaintLayer::styleChanged(StyleDifference diff, const ComputedStyle* oldStyle)
{
    if (attemptDirectCompositingUpdate(diff, oldStyle))
        return;

    m_stackingNode->updateIsTreatedAsStackingContextForPainting();
    m_stackingNode->updateStackingNodesAfterStyleChange(oldStyle);

    if (m_scrollableArea)
        m_scrollableArea->updateAfterStyleChange(oldStyle);

    // Overlay scrollbars can make this layer self-painting so we need
    // to recompute the bit once scrollbars have been updated.
    updateSelfPaintingLayer();

    if (!oldStyle || !layoutObject()->style()->reflectionDataEquivalent(oldStyle)) {
        ASSERT(!oldStyle || diff.needsFullLayout());
        updateReflectionInfo(oldStyle);
    }

    updateDescendantDependentFlags();

    updateTransform(oldStyle, layoutObject()->styleRef());
    updateFilters(oldStyle, layoutObject()->styleRef());

    setNeedsCompositingInputsUpdate();
}

bool DeprecatedPaintLayer::scrollsOverflow() const
{
    if (DeprecatedPaintLayerScrollableArea* scrollableArea = this->scrollableArea())
        return scrollableArea->scrollsOverflow();

    return false;
}

FilterOperations DeprecatedPaintLayer::computeFilterOperations(const ComputedStyle& style)
{
    const FilterOperations& filters = style.filter();
    if (filters.hasReferenceFilter()) {
        for (size_t i = 0; i < filters.size(); ++i) {
            FilterOperation* filterOperation = filters.operations().at(i).get();
            if (filterOperation->type() != FilterOperation::REFERENCE)
                continue;
            ReferenceFilterOperation& referenceOperation = toReferenceFilterOperation(*filterOperation);
            // FIXME: Cache the ReferenceFilter if it didn't change.
            RefPtrWillBeRawPtr<ReferenceFilter> referenceFilter = ReferenceFilter::create(style.effectiveZoom());
            referenceFilter->setLastEffect(ReferenceFilterBuilder::build(referenceFilter.get(), toElement(enclosingElement()), referenceFilter->sourceGraphic(),
                referenceOperation));
            referenceOperation.setFilter(referenceFilter.release());
        }
    }

    return filters;
}

void DeprecatedPaintLayer::updateOrRemoveFilterClients()
{
    if (!hasFilter()) {
        removeFilterInfoIfNeeded();
        return;
    }

    if (layoutObject()->style()->filter().hasReferenceFilter())
        ensureFilterInfo()->updateReferenceFilterClients(layoutObject()->style()->filter());
    else if (hasFilterInfo())
        filterInfo()->removeReferenceFilterClients();
}

void DeprecatedPaintLayer::updateOrRemoveFilterEffectBuilder()
{
    // FilterEffectBuilder is only used to render the filters in software mode,
    // so we always need to run updateOrRemoveFilterEffectBuilder after the composited
    // mode might have changed for this layer.
    if (!paintsWithFilters()) {
        // Don't delete the whole filter info here, because we might use it
        // for loading CSS shader files.
        if (DeprecatedPaintLayerFilterInfo* filterInfo = this->filterInfo())
            filterInfo->setBuilder(nullptr);

        return;
    }

    DeprecatedPaintLayerFilterInfo* filterInfo = ensureFilterInfo();
    if (!filterInfo->builder())
        filterInfo->setBuilder(FilterEffectBuilder::create());

    // If the filter fails to build, remove it from the layer. It will still attempt to
    // go through regular processing (e.g. compositing), but never apply anything.
    float zoom = layoutObject()->style() ? layoutObject()->style()->effectiveZoom() : 1.0f;
    if (!filterInfo->builder()->build(toElement(enclosingElement()), computeFilterOperations(layoutObject()->styleRef()), zoom))
        filterInfo->setBuilder(nullptr);
}

void DeprecatedPaintLayer::filterNeedsPaintInvalidation()
{
    {
        DeprecatedScheduleStyleRecalcDuringLayout marker(layoutObject()->document().lifecycle());
        // It's possible for scheduleSVGFilterLayerUpdateHack to schedule a style recalc, which
        // is a problem because this function can be called while performing layout.
        // Presumably this represents an illegal data flow of layout or compositing
        // information into the style system.
        toElement(layoutObject()->node())->scheduleSVGFilterLayerUpdateHack();
    }

    layoutObject()->setShouldDoFullPaintInvalidation();
}

void DeprecatedPaintLayer::addLayerHitTestRects(LayerHitTestRects& rects) const
{
    computeSelfHitTestRects(rects);
    for (DeprecatedPaintLayer* child = firstChild(); child; child = child->nextSibling())
        child->addLayerHitTestRects(rects);
}

void DeprecatedPaintLayer::computeSelfHitTestRects(LayerHitTestRects& rects) const
{
    if (!size().isEmpty()) {
        Vector<LayoutRect> rect;

        if (layoutBox() && layoutBox()->scrollsOverflow()) {
            // For scrolling layers, rects are taken to be in the space of the contents.
            // We need to include the bounding box of the layer in the space of its parent
            // (eg. for border / scroll bars) and if it's composited then the entire contents
            // as well as they may be on another composited layer. Skip reporting contents
            // for non-composited layers as they'll get projected to the same layer as the
            // bounding box.
            if (compositingState() != NotComposited)
                rect.append(m_scrollableArea->overflowRect());

            rects.set(this, rect);
            if (const DeprecatedPaintLayer* parentLayer = parent()) {
                LayerHitTestRects::iterator iter = rects.find(parentLayer);
                if (iter == rects.end()) {
                    rects.add(parentLayer, Vector<LayoutRect>()).storedValue->value.append(physicalBoundingBox(parentLayer));
                } else {
                    iter->value.append(physicalBoundingBox(parentLayer));
                }
            }
        } else {
            rect.append(logicalBoundingBox());
            rects.set(this, rect);
        }
    }
}

DisableCompositingQueryAsserts::DisableCompositingQueryAsserts()
    : m_disabler(gCompositingQueryMode, CompositingQueriesAreAllowed) { }

} // namespace blink

#ifndef NDEBUG
// FIXME: Rename?
void showLayerTree(const blink::DeprecatedPaintLayer* layer)
{
    if (!layer) {
        fprintf(stderr, "Cannot showLayerTree. Root is (nil)\n");
        return;
    }

    if (blink::LocalFrame* frame = layer->layoutObject()->frame()) {
        WTF::String output = externalRepresentation(frame, blink::LayoutAsTextShowAllLayers | blink::LayoutAsTextShowLayerNesting | blink::LayoutAsTextShowCompositedLayers | blink::LayoutAsTextShowAddresses | blink::LayoutAsTextShowIDAndClass | blink::LayoutAsTextDontUpdateLayout | blink::LayoutAsTextShowLayoutState);
        fprintf(stderr, "%s\n", output.utf8().data());
    }
}

void showLayerTree(const blink::LayoutObject* layoutObject)
{
    if (!layoutObject) {
        fprintf(stderr, "Cannot showLayerTree. Root is (nil)\n");
        return;
    }
    showLayerTree(layoutObject->enclosingLayer());
}
#endif
