/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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
#include "core/layout/LayoutView.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutFlowThread.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutQuote.h"
#include "core/layout/LayoutScrollbarPart.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/page/Page.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/ViewPainter.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "platform/TraceEvent.h"
#include "platform/TracedValue.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/TransformState.h"
#include "platform/graphics/paint/DisplayItemList.h"
//#include <inttypes.h>

namespace blink {

LayoutView::LayoutView(Document* document)
    : LayoutBlockFlow(document)
    , m_frameView(document->view())
    , m_selectionStart(nullptr)
    , m_selectionEnd(nullptr)
    , m_selectionStartPos(-1)
    , m_selectionEndPos(-1)
    , m_pageLogicalHeight(0)
    , m_pageLogicalHeightChanged(false)
    , m_layoutState(nullptr)
    , m_layoutQuoteHead(nullptr)
    , m_layoutCounterCount(0)
    , m_hitTestCount(0)
    , m_hitTestCacheHits(0)
    , m_hitTestCache(HitTestCache::create())
    , m_pendingSelection(PendingSelection::create())
{
    // init LayoutObject attributes
    setInline(false);

    m_minPreferredLogicalWidth = 0;
    m_maxPreferredLogicalWidth = 0;

    setPreferredLogicalWidthsDirty(MarkOnlyThis);

    setPositionState(AbsolutePosition); // to 0,0 :)
}

LayoutView::~LayoutView()
{
}

bool LayoutView::hitTest(HitTestResult& result)
{
    // We have to recursively update layout/style here because otherwise, when the hit test recurses
    // into a child document, it could trigger a layout on the parent document, which can destroy DeprecatedPaintLayer
    // that are higher up in the call stack, leading to crashes.
    // Note that Document::updateLayout calls its parent's updateLayout.
    frameView()->updateLifecycleToCompositingCleanPlusScrolling();
    return hitTestNoLifecycleUpdate(result);
}

bool LayoutView::hitTestNoLifecycleUpdate(HitTestResult& result)
{
    TRACE_EVENT_BEGIN0("blink,devtools.timeline", "HitTest");
    m_hitTestCount++;

    ASSERT(!result.hitTestLocation().isRectBasedTest() || result.hitTestRequest().listBased());

    commitPendingSelection();

    uint64_t domTreeVersion = document().domTreeVersion();
    HitTestResult cacheResult = result;
    bool cacheHit = m_hitTestCache->lookupCachedResult(cacheResult, domTreeVersion);
    bool hitLayer = layer()->hitTest(result);

    // FrameView scrollbars are not the same as Layer scrollbars tested by Layer::hitTestOverflowControls,
    // so we need to test FrameView scrollbars separately here. Note that it's important we do this after
    // the hit test above, because that may overwrite the entire HitTestResult when it finds a hit.
    IntPoint framePoint = frameView()->contentsToFrame(result.hitTestLocation().roundedPoint());
    if (Scrollbar* frameScrollbar = frameView()->scrollbarAtFramePoint(framePoint))
        result.setScrollbar(frameScrollbar);

    if (cacheHit) {
        m_hitTestCacheHits++;
        m_hitTestCache->verifyCachedResult(result, cacheResult);
    }

    if (hitLayer)
        m_hitTestCache->addCachedResult(result, domTreeVersion);

    TRACE_EVENT_END1("blink,devtools.timeline", "HitTest", "endData", InspectorHitTestEvent::endData(result.hitTestRequest(), result.hitTestLocation(), result));
    return hitLayer;
}

void LayoutView::computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit, LogicalExtentComputedValues& computedValues) const
{
    computedValues.m_extent = (!shouldUsePrintingLayout() && m_frameView) ? LayoutUnit(viewLogicalHeightForBoxSizing()) : logicalHeight;
}

void LayoutView::updateLogicalWidth()
{
    if (!shouldUsePrintingLayout() && m_frameView)
        setLogicalWidth(viewLogicalWidthForBoxSizing());
}

bool LayoutView::isChildAllowed(LayoutObject* child, const ComputedStyle&) const
{
    return child->isBox();
}

void LayoutView::layoutContent()
{
    ASSERT(needsLayout());

    LayoutBlockFlow::layout();

#if ENABLE(ASSERT)
    checkLayoutState();
#endif
}

#if ENABLE(ASSERT)
void LayoutView::checkLayoutState()
{
    ASSERT(!m_layoutState->next());
}
#endif

bool LayoutView::shouldDoFullPaintInvalidationForNextLayout() const
{
    // It's hard to predict here which of full paint invalidation or per-descendant paint invalidation costs less.
    // For vertical writing mode or width change it's more likely that per-descendant paint invalidation
    // eventually turns out to be full paint invalidation but with the cost to handle more layout states
    // and discrete paint invalidation rects, so marking full paint invalidation here is more likely to cost less.
    // Otherwise, per-descendant paint invalidation is more likely to avoid unnecessary full paint invalidation.

    if (shouldUsePrintingLayout())
        return true;

    if (!style()->isHorizontalWritingMode())
        return true;

    // The width/height checks below assume horizontal writing mode.
    ASSERT(style()->isHorizontalWritingMode());

    if (size().width() != viewLogicalWidthForBoxSizing())
        return true;

    if (size().height() != viewLogicalHeightForBoxSizing()) {
        // When background-attachment is 'fixed', we treat the viewport (instead of the 'root'
        // i.e. html or body) as the background positioning area, and we should full paint invalidation
        // viewport resize if the background image is not composited and needs full paint invalidation on
        // background positioning area resize.
        if (!m_compositor || !m_compositor->needsFixedRootBackgroundLayer(layer())) {
            if (style()->hasFixedBackgroundImage()
                && mustInvalidateFillLayersPaintOnHeightChange(style()->backgroundLayers()))
                return true;
        }
    }

    return false;
}

void LayoutView::layout()
{
    if (!document().paginated())
        setPageLogicalHeight(0);

    if (shouldUsePrintingLayout())
        m_minPreferredLogicalWidth = m_maxPreferredLogicalWidth = logicalWidth();

    SubtreeLayoutScope layoutScope(*this);

    LayoutRect oldLayoutOverflowRect = layoutOverflowRect();

    // Use calcWidth/Height to get the new width/height, since this will take the full page zoom factor into account.
    bool relayoutChildren = !shouldUsePrintingLayout() && (!m_frameView
        || logicalWidth() != viewLogicalWidthForBoxSizing()
        || logicalHeight() != viewLogicalHeightForBoxSizing());
    if (relayoutChildren) {
        layoutScope.setChildNeedsLayout(this);
        for (LayoutObject* child = firstChild(); child; child = child->nextSibling()) {
            if (child->isSVGRoot())
                continue;

            if ((child->isBox() && toLayoutBox(child)->hasRelativeLogicalHeight())
                || child->style()->logicalHeight().hasPercent()
                || child->style()->logicalMinHeight().hasPercent()
                || child->style()->logicalMaxHeight().hasPercent())
                layoutScope.setChildNeedsLayout(child);
        }

        if (document().svgExtensions())
            document().accessSVGExtensions().invalidateSVGRootsWithRelativeLengthDescendents(&layoutScope);
    }

    ASSERT(!m_layoutState);
    if (!needsLayout())
        return;

    LayoutState rootLayoutState(pageLogicalHeight(), pageLogicalHeightChanged(), *this);

    m_pageLogicalHeightChanged = false;

    layoutContent();

    if (RuntimeEnabledFeatures::slimmingPaintEnabled() && layoutOverflowRect() != oldLayoutOverflowRect) {
        // The document element paints the viewport background, so we need to invalidate it when
        // layout overflow changes.
        // FIXME: Improve viewport background styling/invalidation/painting. crbug.com/475115
        if (Element* documentElement = document().documentElement()) {
            if (LayoutObject* rootObject = documentElement->layoutObject())
                rootObject->setShouldDoFullPaintInvalidation();
        }
    }

#if ENABLE(ASSERT)
    checkLayoutState();
#endif
    clearNeedsLayout();
}

LayoutRect LayoutView::visualOverflowRect() const
{
    // In root layer scrolling mode, the LayoutView performs overflow clipping
    // like a regular scrollable div.
    if (document().settings() && document().settings()->rootLayerScrolls())
        return LayoutBlockFlow::visualOverflowRect();

    // Ditto when not in compositing mode.
    if (!usesCompositing())
        return LayoutBlockFlow::visualOverflowRect();

    // In normal compositing mode, LayoutView doesn't actually apply clipping
    // on its descendants. Instead their visual overflow is propagated to
    // compositor()->m_rootContentLayer for accelerated scrolling.
    return LayoutRect(unscaledDocumentRect());
}

void LayoutView::mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, MapCoordinatesFlags mode, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    ASSERT_UNUSED(wasFixed, !wasFixed || *wasFixed == static_cast<bool>(mode & IsFixed));

    if (!paintInvalidationContainer && mode & UseTransforms && shouldUseTransformFromContainer(0)) {
        TransformationMatrix t;
        getTransformFromContainer(0, LayoutSize(), t);
        transformState.applyTransform(t);
    }

    if ((mode & IsFixed) && m_frameView) {
        transformState.move(toIntSize(m_frameView->scrollPosition()));
        if (hasOverflowClip())
            transformState.move(scrolledContentOffset());
        // IsFixed flag is only applicable within this LayoutView.
        mode &= ~IsFixed;
    }

    if (paintInvalidationContainer == this)
        return;

    if (mode & TraverseDocumentBoundaries) {
        if (LayoutObject* parentDocLayoutObject = frame()->ownerLayoutObject()) {
            transformState.move(-frame()->view()->scrollOffset());
            if (parentDocLayoutObject->isBox())
                transformState.move(toLayoutBox(parentDocLayoutObject)->contentBoxOffset());
            parentDocLayoutObject->mapLocalToContainer(paintInvalidationContainer, transformState, mode, wasFixed, paintInvalidationState);
            return;
        }
    }
}

const LayoutObject* LayoutView::pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap& geometryMap) const
{
    LayoutSize offsetForFixedPosition;
    LayoutSize offset;
    LayoutObject* container = nullptr;

    if (m_frameView) {
        offsetForFixedPosition = LayoutSize(toIntSize(m_frameView->scrollPosition()));
        if (hasOverflowClip())
            offsetForFixedPosition = LayoutSize(scrolledContentOffset());
    }

    if (geometryMap.mapCoordinatesFlags() & TraverseDocumentBoundaries) {
        if (LayoutPart* parentDocLayoutObject = frame()->ownerLayoutObject()) {
            offset = -LayoutSize(m_frameView->scrollOffset());
            offset += parentDocLayoutObject->contentBoxOffset();
            container = parentDocLayoutObject;
        }
    }

    // If a container was specified, and was not 0 or the LayoutView, then we
    // should have found it by now unless we're traversing to a parent document.
    ASSERT_ARG(ancestorToStopAt, !ancestorToStopAt || ancestorToStopAt == this || container);

    if ((!ancestorToStopAt || container) && shouldUseTransformFromContainer(container)) {
        TransformationMatrix t;
        getTransformFromContainer(container, LayoutSize(), t);
        geometryMap.push(this, t, false, false, false, true, offsetForFixedPosition);
    } else {
        geometryMap.push(this, offset, false, false, false, false, offsetForFixedPosition);
    }

    return container;
}

void LayoutView::mapAbsoluteToLocalPoint(MapCoordinatesFlags mode, TransformState& transformState) const
{
    if (mode & IsFixed && m_frameView)
        transformState.move(toIntSize(m_frameView->scrollPosition()));

    if (mode & UseTransforms && shouldUseTransformFromContainer(0)) {
        TransformationMatrix t;
        getTransformFromContainer(0, LayoutSize(), t);
        transformState.applyTransform(t);
    }
}

void LayoutView::computeSelfHitTestRects(Vector<LayoutRect>& rects, const LayoutPoint&) const
{
    // Record the entire size of the contents of the frame. Note that we don't just
    // use the viewport size (containing block) here because we want to ensure this includes
    // all children (so we can avoid walking them explicitly).
    rects.append(LayoutRect(LayoutPoint::zero(), LayoutSize(frameView()->contentsSize())));
}

void LayoutView::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ViewPainter(*this).paint(paintInfo, paintOffset);
}

void LayoutView::paintBoxDecorationBackground(const PaintInfo& paintInfo, const LayoutPoint&)
{
    ViewPainter(*this).paintBoxDecorationBackground(paintInfo);
}

void LayoutView::invalidateTreeIfNeeded(PaintInvalidationState& paintInvalidationState)
{
    ASSERT(!needsLayout());

    // We specifically need to issue paint invalidations for the viewRect since other layoutObjects
    // short-circuit on full-paint invalidation.
    LayoutRect dirtyRect = viewRect();
    if (doingFullPaintInvalidation() && !dirtyRect.isEmpty()) {
        const LayoutBoxModelObject& paintInvalidationContainer = paintInvalidationState.paintInvalidationContainer();
        DeprecatedPaintLayer::mapRectToPaintInvalidationBacking(this, &paintInvalidationContainer, dirtyRect, &paintInvalidationState);
        invalidatePaintUsingContainer(paintInvalidationContainer, dirtyRect, PaintInvalidationFull);
        if (RuntimeEnabledFeatures::slimmingPaintEnabled())
            invalidateDisplayItemClients(paintInvalidationContainer);
    }
    LayoutBlock::invalidateTreeIfNeeded(paintInvalidationState);
}

static void setShouldDoFullPaintInvalidationForViewAndAllDescendantsInternal(LayoutObject* object)
{
    object->setShouldDoFullPaintInvalidation();
    for (LayoutObject* child = object->slowFirstChild(); child; child = child->nextSibling()) {
        setShouldDoFullPaintInvalidationForViewAndAllDescendantsInternal(child);
    }
}

void LayoutView::setShouldDoFullPaintInvalidationForViewAndAllDescendants()
{
    setShouldDoFullPaintInvalidationForViewAndAllDescendantsInternal(this);
}

void LayoutView::invalidatePaintForRectangle(const LayoutRect& paintInvalidationRect, PaintInvalidationReason invalidationReason) const
{
    ASSERT(!paintInvalidationRect.isEmpty());

    if (document().printing() || !m_frameView)
        return;

    ASSERT(layer()->compositingState() == PaintsIntoOwnBacking || !frame()->ownerLayoutObject());

    if (layer()->compositingState() == PaintsIntoOwnBacking) {
        setBackingNeedsPaintInvalidationInRect(paintInvalidationRect, invalidationReason);
    } else {
        m_frameView->contentRectangleForPaintInvalidation(pixelSnappedIntRect(paintInvalidationRect));
    }
}

void LayoutView::invalidatePaintForViewAndCompositedLayers()
{
    setShouldDoFullPaintInvalidation();

    // The only way we know how to hit these ASSERTS below this point is via the Chromium OS login screen.
    DisableCompositingQueryAsserts disabler;

    if (compositor()->inCompositingMode())
        compositor()->fullyInvalidatePaint();
}

void LayoutView::mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, const PaintInvalidationState* invalidationState) const
{
    mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, IsNotFixedPosition, invalidationState);
}

void LayoutView::mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, ViewportConstrainedPosition viewportConstraint, const PaintInvalidationState* state) const
{
    if (document().printing())
        return;

    if (style()->isFlippedBlocksWritingMode()) {
        // We have to flip by hand since the view's logical height has not been determined.  We
        // can use the viewport width and height.
        if (style()->isHorizontalWritingMode())
            rect.setY(viewHeight() - rect.maxY());
        else
            rect.setX(viewWidth() - rect.maxX());
    }

    adjustViewportConstrainedOffset(rect, viewportConstraint);

    // Apply our transform if we have one (because of full page zooming).
    if (!paintInvalidationContainer && layer() && layer()->transform())
        rect = layer()->transform()->mapRect(rect);

    ASSERT(paintInvalidationContainer);
    if (paintInvalidationContainer == this)
        return;

    Element* owner = document().ownerElement();
    if (!owner)
        return;

    if (LayoutBox* obj = owner->layoutBox()) {
        // Intersect the viewport with the paint invalidation rect.
        LayoutRect viewRectangle = viewRect();
        rect.intersect(viewRectangle);

        // Adjust for scroll offset of the view.
        rect.moveBy(-viewRectangle.location());

        // Adjust for frame border.
        rect.move(obj->contentBoxOffset());
        obj->mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, 0);
    }
}

void LayoutView::adjustViewportConstrainedOffset(LayoutRect& rect, ViewportConstrainedPosition viewportConstraint) const
{
    if (viewportConstraint != IsFixedPosition)
        return;

    if (m_frameView) {
        rect.move(toIntSize(m_frameView->scrollPosition()));
        if (hasOverflowClip())
            rect.move(scrolledContentOffset());

        // FIXME: Paint invalidation should happen after scroll updates, so there should be no pending scroll delta.
        // However, we still have paint invalidation during layout, so we can't ASSERT for now. crbug.com/434950.
        // ASSERT(m_frameView->pendingScrollDelta().isZero());
        // If we have a pending scroll, invalidate the previous scroll position.
        if (!m_frameView->pendingScrollDelta().isZero())
            rect.move(-LayoutSize(m_frameView->pendingScrollDelta()));
    }
}

void LayoutView::absoluteRects(Vector<IntRect>& rects, const LayoutPoint& accumulatedOffset) const
{
    rects.append(pixelSnappedIntRect(accumulatedOffset, LayoutSize(layer()->size())));
}

void LayoutView::absoluteQuads(Vector<FloatQuad>& quads, bool* wasFixed) const
{
    if (wasFixed)
        *wasFixed = false;
    quads.append(FloatRect(FloatPoint(), layer()->size()));
}

static LayoutObject* layoutObjectAfterPosition(LayoutObject* object, unsigned offset)
{
    if (!object)
        return nullptr;

    LayoutObject* child = object->childAt(offset);
    return child ? child : object->nextInPreOrderAfterChildren();
}

static LayoutRect selectionRectForLayoutObject(const LayoutObject* object)
{
    if (!object->isRooted())
        return LayoutRect();

    if (!object->canUpdateSelectionOnRootLineBoxes())
        return LayoutRect();

    return object->selectionRectInViewCoordinates();
}

IntRect LayoutView::selectionBounds()
{
    // Now create a single bounding box rect that encloses the whole selection.
    LayoutRect selRect;

    typedef HashSet<const LayoutBlock*> VisitedContainingBlockSet;
    VisitedContainingBlockSet visitedContainingBlocks;

    commitPendingSelection();
    LayoutObject* os = m_selectionStart;
    LayoutObject* stop = layoutObjectAfterPosition(m_selectionEnd, m_selectionEndPos);
    while (os && os != stop) {
        if ((os->canBeSelectionLeaf() || os == m_selectionStart || os == m_selectionEnd) && os->selectionState() != SelectionNone) {
            // Blocks are responsible for painting line gaps and margin gaps. They must be examined as well.
            selRect.unite(selectionRectForLayoutObject(os));
            const LayoutBlock* cb = os->containingBlock();
            while (cb && !cb->isLayoutView()) {
                selRect.unite(selectionRectForLayoutObject(cb));
                VisitedContainingBlockSet::AddResult addResult = visitedContainingBlocks.add(cb);
                if (!addResult.isNewEntry)
                    break;
                cb = cb->containingBlock();
            }
        }

        os = os->nextInPreOrder();
    }

    return pixelSnappedIntRect(selRect);
}

void LayoutView::invalidatePaintForSelection()
{
    HashSet<LayoutBlock*> processedBlocks;

    LayoutObject* end = layoutObjectAfterPosition(m_selectionEnd, m_selectionEndPos);
    for (LayoutObject* o = m_selectionStart; o && o != end; o = o->nextInPreOrder()) {
        if (!o->canBeSelectionLeaf() && o != m_selectionStart && o != m_selectionEnd)
            continue;
        if (o->selectionState() == SelectionNone)
            continue;

        o->setShouldInvalidateSelection();

        // Blocks are responsible for painting line gaps and margin gaps. They must be examined as well.
        for (LayoutBlock* block = o->containingBlock(); block && !block->isLayoutView(); block = block->containingBlock()) {
            if (!processedBlocks.add(block).isNewEntry)
                break;
            block->setShouldInvalidateSelection();
        }
    }
}

// When exploring the LayoutTree looking for the nodes involved in the Selection, sometimes it's
// required to change the traversing direction because the "start" position is below the "end" one.
static inline LayoutObject* getNextOrPrevLayoutObjectBasedOnDirection(const LayoutObject* o, const LayoutObject* stop, bool& continueExploring, bool& exploringBackwards)
{
    LayoutObject* next;
    if (exploringBackwards) {
        next = o->previousInPreOrder();
        continueExploring = next && !(next)->isLayoutView();
    } else {
        next = o->nextInPreOrder();
        continueExploring = next && next != stop;
        exploringBackwards = !next && (next != stop);
        if (exploringBackwards) {
            next = stop->previousInPreOrder();
            continueExploring = next && !next->isLayoutView();
        }
    }

    return next;
}

void LayoutView::setSelection(LayoutObject* start, int startPos, LayoutObject* end, int endPos, SelectionPaintInvalidationMode blockPaintInvalidationMode)
{
    // This code makes no assumptions as to if the layout tree is up to date or not
    // and will not try to update it. Currently clearSelection calls this
    // (intentionally) without updating the layout tree as it doesn't care.
    // Other callers may want to force recalc style before calling this.

    // Make sure both our start and end objects are defined.
    // Check www.msnbc.com and try clicking around to find the case where this happened.
    if ((start && !end) || (end && !start))
        return;

    // Just return if the selection hasn't changed.
    if (m_selectionStart == start && m_selectionStartPos == startPos
        && m_selectionEnd == end && m_selectionEndPos == endPos)
        return;

    // Record the old selected objects.  These will be used later
    // when we compare against the new selected objects.
    int oldStartPos = m_selectionStartPos;
    int oldEndPos = m_selectionEndPos;

    // Objects each have a single selection rect to examine.
    typedef HashMap<LayoutObject*, SelectionState> SelectedObjectMap;
    SelectedObjectMap oldSelectedObjects;
    // FIXME: |newSelectedObjects| doesn't really need to store the SelectionState, it's just more convenient
    // to have it use the same data structure as |oldSelectedObjects|.
    SelectedObjectMap newSelectedObjects;

    // Blocks contain selected objects and fill gaps between them, either on the left, right, or in between lines and blocks.
    // In order to get the paint invalidation rect right, we have to examine left, middle, and right rects individually, since otherwise
    // the union of those rects might remain the same even when changes have occurred.
    typedef HashMap<LayoutBlock*, SelectionState> SelectedBlockMap;
    SelectedBlockMap oldSelectedBlocks;
    // FIXME: |newSelectedBlocks| doesn't really need to store the SelectionState, it's just more convenient
    // to have it use the same data structure as |oldSelectedBlocks|.
    SelectedBlockMap newSelectedBlocks;

    LayoutObject* os = m_selectionStart;
    LayoutObject* stop = layoutObjectAfterPosition(m_selectionEnd, m_selectionEndPos);
    bool exploringBackwards = false;
    bool continueExploring = os && (os != stop);
    while (continueExploring) {
        if ((os->canBeSelectionLeaf() || os == m_selectionStart || os == m_selectionEnd) && os->selectionState() != SelectionNone) {
            // Blocks are responsible for painting line gaps and margin gaps.  They must be examined as well.
            oldSelectedObjects.set(os, os->selectionState());
            if (blockPaintInvalidationMode == PaintInvalidationNewXOROld) {
                LayoutBlock* cb = os->containingBlock();
                while (cb && !cb->isLayoutView()) {
                    SelectedBlockMap::AddResult result = oldSelectedBlocks.add(cb, cb->selectionState());
                    if (!result.isNewEntry)
                        break;
                    cb = cb->containingBlock();
                }
            }
        }

        os = getNextOrPrevLayoutObjectBasedOnDirection(os, stop, continueExploring, exploringBackwards);
    }

    // Now clear the selection.
    SelectedObjectMap::iterator oldObjectsEnd = oldSelectedObjects.end();
    for (SelectedObjectMap::iterator i = oldSelectedObjects.begin(); i != oldObjectsEnd; ++i)
        i->key->setSelectionStateIfNeeded(SelectionNone);

    // set selection start and end
    m_selectionStart = start;
    m_selectionStartPos = startPos;
    m_selectionEnd = end;
    m_selectionEndPos = endPos;

    // Update the selection status of all objects between m_selectionStart and m_selectionEnd
    if (start && start == end) {
        start->setSelectionStateIfNeeded(SelectionBoth);
    } else {
        if (start)
            start->setSelectionStateIfNeeded(SelectionStart);
        if (end)
            end->setSelectionStateIfNeeded(SelectionEnd);
    }

    LayoutObject* o = start;
    stop = layoutObjectAfterPosition(end, endPos);

    while (o && o != stop) {
        if (o != start && o != end && o->canBeSelectionLeaf())
            o->setSelectionStateIfNeeded(SelectionInside);
        o = o->nextInPreOrder();
    }

    layer()->clearBlockSelectionGapsBounds();

    // Now that the selection state has been updated for the new objects, walk them again and
    // put them in the new objects list.
    o = start;
    exploringBackwards = false;
    continueExploring = o && (o != stop);
    while (continueExploring) {
        if ((o->canBeSelectionLeaf() || o == start || o == end) && o->selectionState() != SelectionNone) {
            newSelectedObjects.set(o, o->selectionState());
            LayoutBlock* cb = o->containingBlock();
            while (cb && !cb->isLayoutView()) {
                SelectedBlockMap::AddResult result = newSelectedBlocks.add(cb, cb->selectionState());
                if (!result.isNewEntry)
                    break;
                cb = cb->containingBlock();
            }
        }

        o = getNextOrPrevLayoutObjectBasedOnDirection(o, stop, continueExploring, exploringBackwards);
    }

    if (!m_frameView)
        return;

    // Have any of the old selected objects changed compared to the new selection?
    for (SelectedObjectMap::iterator i = oldSelectedObjects.begin(); i != oldObjectsEnd; ++i) {
        LayoutObject* obj = i->key;
        SelectionState newSelectionState = obj->selectionState();
        SelectionState oldSelectionState = i->value;
        if (newSelectionState != oldSelectionState
            || (m_selectionStart == obj && oldStartPos != m_selectionStartPos)
            || (m_selectionEnd == obj && oldEndPos != m_selectionEndPos)) {
            obj->setShouldInvalidateSelection();
            newSelectedObjects.remove(obj);
        }
    }

    // Any new objects that remain were not found in the old objects dict, and so they need to be updated.
    SelectedObjectMap::iterator newObjectsEnd = newSelectedObjects.end();
    for (SelectedObjectMap::iterator i = newSelectedObjects.begin(); i != newObjectsEnd; ++i)
        i->key->setShouldInvalidateSelection();

    // Have any of the old blocks changed?
    SelectedBlockMap::iterator oldBlocksEnd = oldSelectedBlocks.end();
    for (SelectedBlockMap::iterator i = oldSelectedBlocks.begin(); i != oldBlocksEnd; ++i) {
        LayoutBlock* block = i->key;
        SelectionState newSelectionState = block->selectionState();
        SelectionState oldSelectionState = i->value;
        if (newSelectionState != oldSelectionState) {
            block->setShouldInvalidateSelection();
            newSelectedBlocks.remove(block);
        }
    }

    // Any new blocks that remain were not found in the old blocks dict, and so they need to be updated.
    SelectedBlockMap::iterator newBlocksEnd = newSelectedBlocks.end();
    for (SelectedBlockMap::iterator i = newSelectedBlocks.begin(); i != newBlocksEnd; ++i)
        i->key->setShouldInvalidateSelection();
}

void LayoutView::clearSelection()
{
    // For querying Layer::compositingState()
    // This is correct, since destroying layout objects needs to cause eager paint invalidations.
    DisableCompositingQueryAsserts disabler;

    layer()->invalidatePaintForBlockSelectionGaps();
    setSelection(0, -1, 0, -1, PaintInvalidationNewMinusOld);
}

void LayoutView::setSelection(const FrameSelection& selection)
{
    // No need to create a pending clearSelection() to be executed in PendingSelection::commit()
    // if there's no selection, since it's no-op. This is a frequent code path worth to optimize.
    if (selection.isNone() && !m_selectionStart && !m_selectionEnd && !hasPendingSelection())
        return;
    m_pendingSelection->setSelection(selection);
}

template <typename Strategy>
void LayoutView::commitPendingSelectionAlgorithm()
{
    using PositionType = typename Strategy::PositionType;

    if (!hasPendingSelection())
        return;
    ASSERT(!needsLayout());

    // Skip if pending VisibilePositions became invalid before we reach here.
    if (!m_pendingSelection->isInDocument(document())) {
        m_pendingSelection->clear();
        return;
    }

    // Construct a new VisibleSolution, since m_selection is not necessarily valid, and the following steps
    // assume a valid selection. See <https://bugs.webkit.org/show_bug.cgi?id=69563> and <rdar://problem/10232866>.
    VisibleSelection selection = m_pendingSelection->calcVisibleSelection();
    m_pendingSelection->clear();

    if (!selection.isRange()) {
        clearSelection();
        return;
    }

    // Use the rightmost candidate for the start of the selection, and the leftmost candidate for the end of the selection.
    // Example: foo <a>bar</a>.  Imagine that a line wrap occurs after 'foo', and that 'bar' is selected.   If we pass [foo, 3]
    // as the start of the selection, the selection painting code will think that content on the line containing 'foo' is selected
    // and will fill the gap before 'bar'.
    PositionType startPos = Strategy::selectionStart(selection);
    PositionType candidate = startPos.downstream();
    if (candidate.isCandidate())
        startPos = candidate;
    PositionType endPos = Strategy::selectionEnd(selection);
    candidate = endPos.upstream();
    if (candidate.isCandidate())
        endPos = candidate;

    // We can get into a state where the selection endpoints map to the same VisiblePosition when a selection is deleted
    // because we don't yet notify the FrameSelection of text removal.
    if (startPos.isNull() || endPos.isNull() || Strategy::selectionVisibleStart(selection) == Strategy::selectionVisibleEnd(selection))
        return;
    LayoutObject* startLayoutObject = startPos.anchorNode()->layoutObject();
    LayoutObject* endLayoutObject = endPos.anchorNode()->layoutObject();
    if (!startLayoutObject || !endLayoutObject)
        return;
    ASSERT(startLayoutObject->view() == this && endLayoutObject->view() == this);
    setSelection(startLayoutObject, startPos.deprecatedEditingOffset(), endLayoutObject, endPos.deprecatedEditingOffset());
}

void LayoutView::commitPendingSelection()
{
    if (RuntimeEnabledFeatures::selectionForComposedTreeEnabled())
        return commitPendingSelectionAlgorithm<VisibleSelection::InComposedTree>();
    commitPendingSelectionAlgorithm<VisibleSelection::InDOMTree>();
}

LayoutObject* LayoutView::selectionStart()
{
    commitPendingSelection();
    return m_selectionStart;
}

LayoutObject* LayoutView::selectionEnd()
{
    commitPendingSelection();
    return m_selectionEnd;
}

void LayoutView::selectionStartEnd(int& startPos, int& endPos)
{
    commitPendingSelection();
    startPos = m_selectionStartPos;
    endPos = m_selectionEndPos;
}

bool LayoutView::shouldUsePrintingLayout() const
{
    if (!document().printing() || !m_frameView)
        return false;
    return m_frameView->frame().shouldUsePrintingLayout();
}

LayoutRect LayoutView::viewRect() const
{
    if (shouldUsePrintingLayout())
        return LayoutRect(LayoutPoint(), size());
    if (m_frameView)
        return LayoutRect(m_frameView->visibleContentRect());
    return LayoutRect();
}

IntRect LayoutView::unscaledDocumentRect() const
{
    LayoutRect overflowRect(layoutOverflowRect());
    flipForWritingMode(overflowRect);
    return pixelSnappedIntRect(overflowRect);
}

bool LayoutView::rootBackgroundIsEntirelyFixed() const
{
    return style()->hasEntirelyFixedBackground();
}

LayoutRect LayoutView::backgroundRect(LayoutBox* backgroundLayoutObject) const
{
    return LayoutRect(unscaledDocumentRect());
}

IntRect LayoutView::documentRect() const
{
    FloatRect overflowRect(unscaledDocumentRect());
    if (hasTransformRelatedProperty())
        overflowRect = layer()->currentTransform().mapRect(overflowRect);
    return IntRect(overflowRect);
}

IntSize LayoutView::layoutSize(IncludeScrollbarsInRect scrollbarInclusion) const
{
    if (!m_frameView || shouldUsePrintingLayout())
        return IntSize();

    IntSize result = m_frameView->layoutSize(IncludeScrollbars);
    if (scrollbarInclusion == ExcludeScrollbars)
        result = m_frameView->layoutViewportScrollableArea()->excludeScrollbars(result);
    return result;
}

int LayoutView::viewLogicalWidth(IncludeScrollbarsInRect scrollbarInclusion) const
{
    return style()->isHorizontalWritingMode() ? viewWidth(scrollbarInclusion) : viewHeight(scrollbarInclusion);
}

int LayoutView::viewLogicalHeight(IncludeScrollbarsInRect scrollbarInclusion) const
{
    return style()->isHorizontalWritingMode() ? viewHeight(scrollbarInclusion) : viewWidth(scrollbarInclusion);
}

int LayoutView::viewLogicalWidthForBoxSizing() const
{
    return viewLogicalWidth(document().settings() && document().settings()->rootLayerScrolls() ? IncludeScrollbars : ExcludeScrollbars);
}

int LayoutView::viewLogicalHeightForBoxSizing() const
{
    return viewLogicalHeight(document().settings() && document().settings()->rootLayerScrolls() ? IncludeScrollbars : ExcludeScrollbars);
}

LayoutUnit LayoutView::viewLogicalHeightForPercentages() const
{
    if (shouldUsePrintingLayout())
        return pageLogicalHeight();
    return viewLogicalHeight();
}

float LayoutView::zoomFactor() const
{
    return m_frameView->frame().pageZoomFactor();
}

void LayoutView::updateHitTestResult(HitTestResult& result, const LayoutPoint& point)
{
    if (result.innerNode())
        return;

    Node* node = document().documentElement();
    if (node) {
        LayoutPoint adjustedPoint = point;
        offsetForContents(adjustedPoint);
        result.setNodeAndPosition(node, adjustedPoint);
    }
}

bool LayoutView::usesCompositing() const
{
    return m_compositor && m_compositor->staleInCompositingMode();
}

DeprecatedPaintLayerCompositor* LayoutView::compositor()
{
    if (!m_compositor)
        m_compositor = adoptPtr(new DeprecatedPaintLayerCompositor(*this));

    return m_compositor.get();
}

void LayoutView::setIsInWindow(bool isInWindow)
{
    if (m_compositor)
        m_compositor->setIsInWindow(isInWindow);
}

IntervalArena* LayoutView::intervalArena()
{
    if (!m_intervalArena)
        m_intervalArena = IntervalArena::create();
    return m_intervalArena.get();
}

bool LayoutView::backgroundIsKnownToBeOpaqueInRect(const LayoutRect&) const
{
    // FIXME: Remove this main frame check. Same concept applies to subframes too.
    if (!frame()->isMainFrame())
        return false;

    return m_frameView->hasOpaqueBackground();
}

double LayoutView::layoutViewportWidth() const
{
    float scale = m_frameView ? m_frameView->frame().pageZoomFactor() : 1;
    return viewWidth(IncludeScrollbars) / scale;
}

double LayoutView::layoutViewportHeight() const
{
    float scale = m_frameView ? m_frameView->frame().pageZoomFactor() : 1;
    return viewHeight(IncludeScrollbars) / scale;
}

void LayoutView::willBeDestroyed()
{
    LayoutBlockFlow::willBeDestroyed();
    m_compositor.clear();
}

} // namespace blink
