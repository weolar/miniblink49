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
#include "core/paint/DeprecatedPaintLayerScrollableArea.h"

#include "core/css/PseudoStyleRequest.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Node.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/LayoutScrollbar.h"
#include "core/layout/LayoutScrollbarPart.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/page/ChromeClient.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "core/paint/DeprecatedPaintLayerFragment.h"
#include "platform/PlatformGestureEvent.h"
#include "platform/PlatformMouseEvent.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "platform/scroll/ScrollAnimator.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "public/platform/Platform.h"

namespace blink {

const int ResizerControlExpandRatioForTouch = 2;

DeprecatedPaintLayerScrollableArea::DeprecatedPaintLayerScrollableArea(DeprecatedPaintLayer& layer)
    : m_layer(layer)
    , m_inResizeMode(false)
    , m_scrollsOverflow(false)
    , m_scrollDimensionsDirty(true)
    , m_inOverflowRelayout(false)
    , m_nextTopmostScrollChild(0)
    , m_topmostScrollChild(0)
    , m_needsCompositedScrolling(false)
    , m_scrollCorner(nullptr)
    , m_resizer(nullptr)
#if ENABLE(ASSERT)
    , m_hasBeenDisposed(false)
#endif
{
    Node* node = box().node();
    if (node && node->isElementNode()) {
        // We save and restore only the scrollOffset as the other scroll values are recalculated.
        Element* element = toElement(node);
        m_scrollOffset = element->savedLayerScrollOffset();
        if (!m_scrollOffset.isZero())
            scrollAnimator()->setCurrentPosition(FloatPoint(m_scrollOffset.width(), m_scrollOffset.height()));
        element->setSavedLayerScrollOffset(IntSize());
    }

    updateResizerAreaSet();
}

DeprecatedPaintLayerScrollableArea::~DeprecatedPaintLayerScrollableArea()
{
    ASSERT(m_hasBeenDisposed);
}

void DeprecatedPaintLayerScrollableArea::dispose()
{
    if (inResizeMode() && !box().documentBeingDestroyed()) {
        if (LocalFrame* frame = box().frame())
            frame->eventHandler().resizeScrollableAreaDestroyed();
    }

    if (LocalFrame* frame = box().frame()) {
        if (FrameView* frameView = frame->view()) {
            frameView->removeScrollableArea(this);
            frameView->removeAnimatingScrollableArea(this);
        }
    }

    if (box().frame() && box().frame()->page()) {
        if (ScrollingCoordinator* scrollingCoordinator = box().frame()->page()->scrollingCoordinator())
            scrollingCoordinator->willDestroyScrollableArea(this);
    }

    if (!box().documentBeingDestroyed()) {
        Node* node = box().node();
        // FIXME: Make setSavedLayerScrollOffset take DoubleSize. crbug.com/414283.
        if (node && node->isElementNode())
            toElement(node)->setSavedLayerScrollOffset(flooredIntSize(m_scrollOffset));
    }

    if (LocalFrame* frame = box().frame()) {
        if (FrameView* frameView = frame->view())
            frameView->removeResizerArea(box());
    }

    destroyScrollbar(HorizontalScrollbar);
    destroyScrollbar(VerticalScrollbar);

    if (m_scrollCorner)
        m_scrollCorner->destroy();
    if (m_resizer)
        m_resizer->destroy();

    clearScrollAnimators();

#if ENABLE(ASSERT)
    m_hasBeenDisposed = true;
#endif
}

DEFINE_TRACE(DeprecatedPaintLayerScrollableArea)
{
    visitor->trace(m_hBar);
    visitor->trace(m_vBar);
    ScrollableArea::trace(visitor);
}

HostWindow* DeprecatedPaintLayerScrollableArea::hostWindow() const
{
    if (Page* page = box().frame()->page())
        return &page->chromeClient();
    return nullptr;
}

GraphicsLayer* DeprecatedPaintLayerScrollableArea::layerForScrolling() const
{
    return layer()->hasCompositedDeprecatedPaintLayerMapping() ? layer()->compositedDeprecatedPaintLayerMapping()->scrollingContentsLayer() : 0;
}

GraphicsLayer* DeprecatedPaintLayerScrollableArea::layerForHorizontalScrollbar() const
{
    // See crbug.com/343132.
    DisableCompositingQueryAsserts disabler;

    return layer()->hasCompositedDeprecatedPaintLayerMapping() ? layer()->compositedDeprecatedPaintLayerMapping()->layerForHorizontalScrollbar() : 0;
}

GraphicsLayer* DeprecatedPaintLayerScrollableArea::layerForVerticalScrollbar() const
{
    // See crbug.com/343132.
    DisableCompositingQueryAsserts disabler;

    return layer()->hasCompositedDeprecatedPaintLayerMapping() ? layer()->compositedDeprecatedPaintLayerMapping()->layerForVerticalScrollbar() : 0;
}

GraphicsLayer* DeprecatedPaintLayerScrollableArea::layerForScrollCorner() const
{
    // See crbug.com/343132.
    DisableCompositingQueryAsserts disabler;

    return layer()->hasCompositedDeprecatedPaintLayerMapping() ? layer()->compositedDeprecatedPaintLayerMapping()->layerForScrollCorner() : 0;
}

void DeprecatedPaintLayerScrollableArea::invalidateScrollbarRect(Scrollbar* scrollbar, const IntRect& rect)
{
    // See crbug.com/343132.
    DisableCompositingQueryAsserts disabler;

    ASSERT(scrollbar == m_hBar.get() || scrollbar == m_vBar.get());
    ASSERT(scrollbar == m_hBar.get() ? !layerForHorizontalScrollbar() : !layerForVerticalScrollbar());

    IntRect scrollRect = rect;
    // If we are not yet inserted into the tree, there is no need to issue paint invaldiations.
    if (!box().isLayoutView() && !box().parent())
        return;

    if (scrollbar == m_vBar.get())
        scrollRect.move(verticalScrollbarStart(0, box().size().width()), box().borderTop());
    else
        scrollRect.move(horizontalScrollbarStart(0), box().size().height() - box().borderBottom() - scrollbar->height());

    if (scrollRect.isEmpty())
        return;

    box().invalidateDisplayItemClient(*scrollbar);

    LayoutRect paintInvalidationRect = LayoutRect(scrollRect);
    box().flipForWritingMode(paintInvalidationRect);

    IntRect intRect = pixelSnappedIntRect(paintInvalidationRect);

    if (box().frameView()->isInPerformLayout()) {
        addScrollbarDamage(scrollbar, intRect);
    } else {
        // FIXME: We should not allow paint invalidation out of paint invalidation state. crbug.com/457415
        DisablePaintInvalidationStateAsserts disabler;
        // We have invalidated the displayItemClient of the scrollbar, but for now we still need to
        // invalidate the rectangles to trigger repaints.
        box().invalidatePaintRectangleNotInvalidatingDisplayItemClients(LayoutRect(intRect));
    }
}

void DeprecatedPaintLayerScrollableArea::invalidateScrollCornerRect(const IntRect& rect)
{
    ASSERT(!layerForScrollCorner());

    if (m_scrollCorner) {
        m_scrollCorner->invalidatePaintRectangle(LayoutRect(rect));
        if (RuntimeEnabledFeatures::slimmingPaintEnabled())
            box().invalidateDisplayItemClientForNonCompositingDescendantsOf(*m_scrollCorner);
    } else if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        box().invalidateDisplayItemClient(box());
    }
    if (m_resizer) {
        m_resizer->invalidatePaintRectangle(LayoutRect(rect));
        if (RuntimeEnabledFeatures::slimmingPaintEnabled())
            box().invalidateDisplayItemClientForNonCompositingDescendantsOf(*m_resizer);
    }
}

bool DeprecatedPaintLayerScrollableArea::shouldUseIntegerScrollOffset() const
{
    Frame* frame = box().frame();
    if (frame->settings() && !frame->settings()->preferCompositingToLCDTextEnabled())
        return true;

    return ScrollableArea::shouldUseIntegerScrollOffset();
}

bool DeprecatedPaintLayerScrollableArea::isActive() const
{
    Page* page = box().frame()->page();
    return page && page->focusController().isActive();
}

bool DeprecatedPaintLayerScrollableArea::isScrollCornerVisible() const
{
    return !scrollCornerRect().isEmpty();
}

static int cornerStart(const ComputedStyle& style, int minX, int maxX, int thickness)
{
    if (style.shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        return minX + style.borderLeftWidth();
    return maxX - thickness - style.borderRightWidth();
}

static IntRect cornerRect(const ComputedStyle& style, const Scrollbar* horizontalScrollbar, const Scrollbar* verticalScrollbar, const IntRect& bounds)
{
    int horizontalThickness;
    int verticalThickness;
    if (!verticalScrollbar && !horizontalScrollbar) {
        // FIXME: This isn't right. We need to know the thickness of custom scrollbars
        // even when they don't exist in order to set the resizer square size properly.
        horizontalThickness = ScrollbarTheme::theme()->scrollbarThickness();
        verticalThickness = horizontalThickness;
    } else if (verticalScrollbar && !horizontalScrollbar) {
        horizontalThickness = verticalScrollbar->width();
        verticalThickness = horizontalThickness;
    } else if (horizontalScrollbar && !verticalScrollbar) {
        verticalThickness = horizontalScrollbar->height();
        horizontalThickness = verticalThickness;
    } else {
        horizontalThickness = verticalScrollbar->width();
        verticalThickness = horizontalScrollbar->height();
    }
    return IntRect(cornerStart(style, bounds.x(), bounds.maxX(), horizontalThickness),
        bounds.maxY() - verticalThickness - style.borderBottomWidth(),
        horizontalThickness, verticalThickness);
}


IntRect DeprecatedPaintLayerScrollableArea::scrollCornerRect() const
{
    // We have a scrollbar corner when a scrollbar is visible and not filling the entire length of the box.
    // This happens when:
    // (a) A resizer is present and at least one scrollbar is present
    // (b) Both scrollbars are present.
    bool hasHorizontalBar = horizontalScrollbar();
    bool hasVerticalBar = verticalScrollbar();
    bool hasResizer = box().style()->resize() != RESIZE_NONE;
    if ((hasHorizontalBar && hasVerticalBar) || (hasResizer && (hasHorizontalBar || hasVerticalBar)))
        return cornerRect(box().styleRef(), horizontalScrollbar(), verticalScrollbar(), box().pixelSnappedBorderBoxRect());
    return IntRect();
}

IntRect DeprecatedPaintLayerScrollableArea::convertFromScrollbarToContainingView(const Scrollbar* scrollbar, const IntRect& scrollbarRect) const
{
    LayoutView* view = box().view();
    if (!view)
        return scrollbarRect;

    IntRect rect = scrollbarRect;
    rect.move(scrollbarOffset(scrollbar));

    return view->frameView()->convertFromLayoutObject(box(), rect);
}

IntRect DeprecatedPaintLayerScrollableArea::convertFromContainingViewToScrollbar(const Scrollbar* scrollbar, const IntRect& parentRect) const
{
    LayoutView* view = box().view();
    if (!view)
        return parentRect;

    IntRect rect = view->frameView()->convertToLayoutObject(box(), parentRect);
    rect.move(-scrollbarOffset(scrollbar));
    return rect;
}

IntPoint DeprecatedPaintLayerScrollableArea::convertFromScrollbarToContainingView(const Scrollbar* scrollbar, const IntPoint& scrollbarPoint) const
{
    LayoutView* view = box().view();
    if (!view)
        return scrollbarPoint;

    IntPoint point = scrollbarPoint;
    point.move(scrollbarOffset(scrollbar));
    return view->frameView()->convertFromLayoutObject(box(), point);
}

IntPoint DeprecatedPaintLayerScrollableArea::convertFromContainingViewToScrollbar(const Scrollbar* scrollbar, const IntPoint& parentPoint) const
{
    LayoutView* view = box().view();
    if (!view)
        return parentPoint;

    IntPoint point = view->frameView()->convertToLayoutObject(box(), parentPoint);

    point.move(-scrollbarOffset(scrollbar));
    return point;
}

int DeprecatedPaintLayerScrollableArea::scrollSize(ScrollbarOrientation orientation) const
{
    IntSize scrollDimensions = maximumScrollPosition() - minimumScrollPosition();
    return (orientation == HorizontalScrollbar) ? scrollDimensions.width() : scrollDimensions.height();
}

void DeprecatedPaintLayerScrollableArea::setScrollOffset(const IntPoint& newScrollOffset, ScrollType scrollType)
{
    setScrollOffset(DoublePoint(newScrollOffset), scrollType);
}

void DeprecatedPaintLayerScrollableArea::setScrollOffset(const DoublePoint& newScrollOffset, ScrollType)
{
    // Ensure that the dimensions will be computed if they need to be (for overflow:hidden blocks).
    if (m_scrollDimensionsDirty)
        computeScrollDimensions();

    if (scrollOffset() == toDoubleSize(newScrollOffset))
        return;

    DoubleSize scrollDelta = scrollOffset() - toDoubleSize(newScrollOffset);
    m_scrollOffset = toDoubleSize(newScrollOffset);

    LocalFrame* frame = box().frame();
    ASSERT(frame);

    RefPtrWillBeRawPtr<FrameView> frameView = box().frameView();

    TRACE_EVENT1("devtools.timeline", "ScrollLayer", "data", InspectorScrollLayerEvent::data(&box()));

    // FIXME(420741): Resolve circular dependency between scroll offset and
    // compositing state, and remove this disabler.
    DisableCompositingQueryAsserts disabler;

    // Update the positions of our child layers (if needed as only fixed layers should be impacted by a scroll).
    // We don't update compositing layers, because we need to do a deep update from the compositing ancestor.
    if (!frameView->isInPerformLayout()) {
        // If we're in the middle of layout, we'll just update layers once layout has finished.
        layer()->updateLayerPositionsAfterOverflowScroll(scrollDelta);
        // Update regions, scrolling may change the clip of a particular region.
        frameView->updateAnnotatedRegions();
        frameView->setNeedsUpdateWidgetPositions();
        updateCompositingLayersAfterScroll();
    }

    const LayoutBoxModelObject* paintInvalidationContainer = box().containerForPaintInvalidation();
    // The caret rect needs to be invalidated after scrolling
    frame->selection().setCaretRectNeedsUpdate();

    FloatQuad quadForFakeMouseMoveEvent = FloatQuad(layer()->layoutObject()->previousPaintInvalidationRect());

    quadForFakeMouseMoveEvent = paintInvalidationContainer->localToAbsoluteQuad(quadForFakeMouseMoveEvent);
    frame->eventHandler().dispatchFakeMouseMoveEventSoonInQuad(quadForFakeMouseMoveEvent);

    bool requiresPaintInvalidation = true;

    if (box().view()->compositor()->inCompositingMode()) {
        bool onlyScrolledCompositedLayers = scrollsOverflow()
            && !layer()->hasVisibleNonLayerContent()
            && !layer()->hasNonCompositedChild()
            && !layer()->hasBlockSelectionGapBounds()
            && box().style()->backgroundLayers().attachment() != LocalBackgroundAttachment;

        if (usesCompositedScrolling() || onlyScrolledCompositedLayers)
            requiresPaintInvalidation = false;
    }

    // Only the root layer can overlap non-composited fixed-position elements.
    if (!requiresPaintInvalidation && layer()->isRootLayer() && frameView->hasViewportConstrainedObjects()) {
        if (!frameView->invalidateViewportConstrainedObjects())
            requiresPaintInvalidation = true;
    }

    // Just schedule a full paint invalidation of our object.
    // FIXME: This invalidation will be unnecessary in slimming paint phase 2.
    if (requiresPaintInvalidation) {
        if (RuntimeEnabledFeatures::slimmingPaintEnabled())
            box().setShouldDoFullPaintInvalidationIncludingNonCompositingDescendants();
        else
            box().setShouldDoFullPaintInvalidation();
    }

    // Schedule the scroll DOM event.
    if (box().node())
        box().node()->document().enqueueScrollEventForNode(box().node());

    if (AXObjectCache* cache = box().document().existingAXObjectCache())
        cache->handleScrollPositionChanged(&box());
    box().view()->clearHitTestCache();

    // Inform the FrameLoader of the new scroll position, so it can be restored when navigating back.
    if (layer()->isRootLayer())
        frameView->frame().loader().saveScrollState();
}

IntPoint DeprecatedPaintLayerScrollableArea::scrollPosition() const
{
    return IntPoint(flooredIntSize(m_scrollOffset));
}

DoublePoint DeprecatedPaintLayerScrollableArea::scrollPositionDouble() const
{
    return DoublePoint(m_scrollOffset);
}

IntPoint DeprecatedPaintLayerScrollableArea::minimumScrollPosition() const
{
    return -scrollOrigin();
}

IntPoint DeprecatedPaintLayerScrollableArea::maximumScrollPosition() const
{
    if (!box().hasOverflowClip())
        return -scrollOrigin();
    return -scrollOrigin() + IntPoint(pixelSnappedScrollWidth(), pixelSnappedScrollHeight()) - enclosingIntRect(box().clientBoxRect()).size();
}

IntRect DeprecatedPaintLayerScrollableArea::visibleContentRect(IncludeScrollbarsInRect scrollbarInclusion) const
{
    int verticalScrollbarWidth = 0;
    int horizontalScrollbarHeight = 0;
    if (scrollbarInclusion == IncludeScrollbars) {
        verticalScrollbarWidth = (verticalScrollbar() && !verticalScrollbar()->isOverlayScrollbar()) ? verticalScrollbar()->width() : 0;
        horizontalScrollbarHeight = (horizontalScrollbar() && !horizontalScrollbar()->isOverlayScrollbar()) ? horizontalScrollbar()->height() : 0;
    }

    return IntRect(IntPoint(scrollXOffset(), scrollYOffset()),
        IntSize(max(0, layer()->size().width() - verticalScrollbarWidth), max(0, layer()->size().height() - horizontalScrollbarHeight)));
}

int DeprecatedPaintLayerScrollableArea::visibleHeight() const
{
    return layer()->size().height();
}

int DeprecatedPaintLayerScrollableArea::visibleWidth() const
{
    return layer()->size().width();
}

IntSize DeprecatedPaintLayerScrollableArea::contentsSize() const
{
    return IntSize(scrollWidth(), scrollHeight());
}

IntPoint DeprecatedPaintLayerScrollableArea::lastKnownMousePosition() const
{
    return box().frame() ? box().frame()->eventHandler().lastKnownMousePosition() : IntPoint();
}

bool DeprecatedPaintLayerScrollableArea::scrollAnimatorEnabled() const
{
    if (Settings* settings = box().frame()->settings())
        return settings->scrollAnimatorEnabled();
    return false;
}

bool DeprecatedPaintLayerScrollableArea::shouldSuspendScrollAnimations() const
{
    LayoutView* view = box().view();
    if (!view)
        return true;
    return view->frameView()->shouldSuspendScrollAnimations();
}

bool DeprecatedPaintLayerScrollableArea::scrollbarsCanBeActive() const
{
    LayoutView* view = box().view();
    if (!view)
        return false;
    return view->frameView()->scrollbarsCanBeActive();
}

IntRect DeprecatedPaintLayerScrollableArea::scrollableAreaBoundingBox() const
{
    return box().absoluteBoundingBoxRect();
}

void DeprecatedPaintLayerScrollableArea::registerForAnimation()
{
    if (LocalFrame* frame = box().frame()) {
        if (FrameView* frameView = frame->view())
            frameView->addAnimatingScrollableArea(this);
    }
}

void DeprecatedPaintLayerScrollableArea::deregisterForAnimation()
{
    if (LocalFrame* frame = box().frame()) {
        if (FrameView* frameView = frame->view())
            frameView->removeAnimatingScrollableArea(this);
    }
}

bool DeprecatedPaintLayerScrollableArea::userInputScrollable(ScrollbarOrientation orientation) const
{
    if (box().isIntrinsicallyScrollable(orientation))
        return true;

    EOverflow overflowStyle = (orientation == HorizontalScrollbar) ?
        box().style()->overflowX() : box().style()->overflowY();
    return (overflowStyle == OSCROLL || overflowStyle == OAUTO || overflowStyle == OOVERLAY);
}

bool DeprecatedPaintLayerScrollableArea::shouldPlaceVerticalScrollbarOnLeft() const
{
    return box().style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft();
}

int DeprecatedPaintLayerScrollableArea::pageStep(ScrollbarOrientation orientation) const
{
    int length = (orientation == HorizontalScrollbar) ?
        box().pixelSnappedClientWidth() : box().pixelSnappedClientHeight();
    int minPageStep = static_cast<float>(length) * ScrollableArea::minFractionToStepWhenPaging();
    int pageStep = max(minPageStep, length - ScrollableArea::maxOverlapBetweenPages());

    return max(pageStep, 1);
}

LayoutBox& DeprecatedPaintLayerScrollableArea::box() const
{
    return *m_layer.layoutBox();
}

DeprecatedPaintLayer* DeprecatedPaintLayerScrollableArea::layer() const
{
    return &m_layer;
}

LayoutUnit DeprecatedPaintLayerScrollableArea::scrollWidth() const
{
    if (m_scrollDimensionsDirty)
        const_cast<DeprecatedPaintLayerScrollableArea*>(this)->computeScrollDimensions();
    return m_overflowRect.width();
}

LayoutUnit DeprecatedPaintLayerScrollableArea::scrollHeight() const
{
    if (m_scrollDimensionsDirty)
        const_cast<DeprecatedPaintLayerScrollableArea*>(this)->computeScrollDimensions();
    return m_overflowRect.height();
}

int DeprecatedPaintLayerScrollableArea::pixelSnappedScrollWidth() const
{
    return snapSizeToPixel(scrollWidth(), box().clientLeft() + box().location().x());
}

int DeprecatedPaintLayerScrollableArea::pixelSnappedScrollHeight() const
{
    return snapSizeToPixel(scrollHeight(), box().clientTop() + box().location().y());
}

void DeprecatedPaintLayerScrollableArea::computeScrollDimensions()
{
    m_scrollDimensionsDirty = false;

    m_overflowRect = box().layoutOverflowRect();
    box().flipForWritingMode(m_overflowRect);

    int scrollableLeftOverflow = m_overflowRect.x() - box().borderLeft() - (box().style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft() ? box().verticalScrollbarWidth() : 0);
    int scrollableTopOverflow = m_overflowRect.y() - box().borderTop();
    setScrollOrigin(IntPoint(-scrollableLeftOverflow, -scrollableTopOverflow));
}

void DeprecatedPaintLayerScrollableArea::scrollToOffset(const DoubleSize& scrollOffset, ScrollOffsetClamping clamp, ScrollBehavior scrollBehavior)
{
    cancelProgrammaticScrollAnimation();
    DoubleSize newScrollOffset = clamp == ScrollOffsetClamped ? clampScrollOffset(scrollOffset) : scrollOffset;
    if (newScrollOffset != adjustedScrollOffset()) {
        DoublePoint origin(scrollOrigin());
        ScrollableArea::setScrollPosition(-origin + newScrollOffset, ProgrammaticScroll, scrollBehavior);
    }
}

void DeprecatedPaintLayerScrollableArea::updateAfterLayout()
{
    ASSERT(box().hasOverflowClip());

    m_scrollDimensionsDirty = true;
    DoubleSize originalScrollOffset = adjustedScrollOffset();

    computeScrollDimensions();

    // Layout may cause us to be at an invalid scroll position. In this case we need
    // to pull our scroll offsets back to the max (or push them up to the min).
    DoubleSize clampedScrollOffset = clampScrollOffset(adjustedScrollOffset());
    if (clampedScrollOffset != adjustedScrollOffset())
        scrollToOffset(clampedScrollOffset);

    if (originalScrollOffset != adjustedScrollOffset()) {
        DoublePoint origin(scrollOrigin());
        scrollPositionChanged(-origin + adjustedScrollOffset(), ProgrammaticScroll);
    }

    bool hasHorizontalOverflow = this->hasHorizontalOverflow();
    bool hasVerticalOverflow = this->hasVerticalOverflow();

    {
        // Hits in compositing/overflow/automatically-opt-into-composited-scrolling-after-style-change.html.
        DisableCompositingQueryAsserts disabler;

        // overflow:scroll should just enable/disable.
        if (box().style()->overflowX() == OSCROLL && horizontalScrollbar())
            horizontalScrollbar()->setEnabled(hasHorizontalOverflow);
        if (box().style()->overflowY() == OSCROLL && verticalScrollbar())
            verticalScrollbar()->setEnabled(hasVerticalOverflow);
    }
    if (hasOverlayScrollbars()) {
        if (!scrollSize(HorizontalScrollbar))
            setHasHorizontalScrollbar(false);
        if (!scrollSize(VerticalScrollbar))
            setHasVerticalScrollbar(false);
    }
    // overflow:auto may need to lay out again if scrollbars got added/removed.
    bool autoHorizontalScrollBarChanged = box().hasAutoHorizontalScrollbar() && (hasHorizontalScrollbar() != hasHorizontalOverflow);
    bool autoVerticalScrollBarChanged = box().hasAutoVerticalScrollbar() && (hasVerticalScrollbar() != hasVerticalOverflow);

    if (autoHorizontalScrollBarChanged || autoVerticalScrollBarChanged) {
        if (box().hasAutoHorizontalScrollbar())
            setHasHorizontalScrollbar(hasHorizontalOverflow);
        if (box().hasAutoVerticalScrollbar())
            setHasVerticalScrollbar(hasVerticalOverflow);

        if (hasVerticalOverflow || hasHorizontalOverflow)
            updateScrollCornerStyle();

        layer()->updateSelfPaintingLayer();

        // Force an update since we know the scrollbars have changed things.
        if (box().document().hasAnnotatedRegions())
            box().document().setAnnotatedRegionsDirty(true);

        if (box().style()->overflowX() == OAUTO || box().style()->overflowY() == OAUTO) {
            if (!m_inOverflowRelayout) {
                // Our proprietary overflow: overlay value doesn't trigger a layout.
                m_inOverflowRelayout = true;
                SubtreeLayoutScope layoutScope(box());
                layoutScope.setNeedsLayout(&box(), LayoutInvalidationReason::ScrollbarChanged);
                if (box().isLayoutBlock()) {
                    LayoutBlock& block = toLayoutBlock(box());
                    block.scrollbarsChanged(autoHorizontalScrollBarChanged, autoVerticalScrollBarChanged);
                    block.layoutBlock(true);
                } else {
                    box().layout();
                }
                m_inOverflowRelayout = false;
            }
        }
    }

    {
        // Hits in compositing/overflow/automatically-opt-into-composited-scrolling-after-style-change.html.
        DisableCompositingQueryAsserts disabler;

        // Set up the range (and page step/line step).
        if (Scrollbar* horizontalScrollbar = this->horizontalScrollbar()) {
            int clientWidth = box().pixelSnappedClientWidth();
            horizontalScrollbar->setProportion(clientWidth, overflowRect().width());
        }
        if (Scrollbar* verticalScrollbar = this->verticalScrollbar()) {
            int clientHeight = box().pixelSnappedClientHeight();
            verticalScrollbar->setProportion(clientHeight, overflowRect().height());
        }
    }

    bool hasOverflow = hasScrollableHorizontalOverflow() || hasScrollableVerticalOverflow();
    updateScrollableAreaSet(hasOverflow);

    DisableCompositingQueryAsserts disabler;
    positionOverflowControls();
}

ScrollBehavior DeprecatedPaintLayerScrollableArea::scrollBehaviorStyle() const
{
    return box().style()->scrollBehavior();
}

bool DeprecatedPaintLayerScrollableArea::hasHorizontalOverflow() const
{
    ASSERT(!m_scrollDimensionsDirty);

    return pixelSnappedScrollWidth() > box().pixelSnappedClientWidth();
}

bool DeprecatedPaintLayerScrollableArea::hasVerticalOverflow() const
{
    ASSERT(!m_scrollDimensionsDirty);

    return pixelSnappedScrollHeight() > box().pixelSnappedClientHeight();
}

bool DeprecatedPaintLayerScrollableArea::hasScrollableHorizontalOverflow() const
{
    return hasHorizontalOverflow() && box().scrollsOverflowX();
}

bool DeprecatedPaintLayerScrollableArea::hasScrollableVerticalOverflow() const
{
    return hasVerticalOverflow() && box().scrollsOverflowY();
}

static bool overflowRequiresScrollbar(EOverflow overflow)
{
    return overflow == OSCROLL;
}

static bool overflowDefinesAutomaticScrollbar(EOverflow overflow)
{
    return overflow == OAUTO || overflow == OOVERLAY;
}

// This function returns true if the given box requires overflow scrollbars (as
// opposed to the 'viewport' scrollbars managed by the DeprecatedPaintLayerCompositor).
// FIXME: we should use the same scrolling machinery for both the viewport and
// overflow. Currently, we need to avoid producing scrollbars here if they'll be
// handled externally in the RLC.
static bool canHaveOverflowScrollbars(const LayoutBox& box)
{
    bool rootLayerScrolls = box.document().settings() && box.document().settings()->rootLayerScrolls();
    return (rootLayerScrolls || !box.isLayoutView()) && box.document().viewportDefiningElement() != box.node();
}

void DeprecatedPaintLayerScrollableArea::updateAfterStyleChange(const ComputedStyle* oldStyle)
{
    if (!m_scrollDimensionsDirty)
        updateScrollableAreaSet(hasScrollableHorizontalOverflow() || hasScrollableVerticalOverflow());

    if (!canHaveOverflowScrollbars(box()))
        return;

    EOverflow overflowX = box().style()->overflowX();
    EOverflow overflowY = box().style()->overflowY();

    // To avoid doing a relayout in updateScrollbarsAfterLayout, we try to keep any automatic scrollbar that was already present.
    bool needsHorizontalScrollbar = (hasHorizontalScrollbar() && overflowDefinesAutomaticScrollbar(overflowX)) || overflowRequiresScrollbar(overflowX);
    bool needsVerticalScrollbar = (hasVerticalScrollbar() && overflowDefinesAutomaticScrollbar(overflowY)) || overflowRequiresScrollbar(overflowY);
    setHasHorizontalScrollbar(needsHorizontalScrollbar);
    setHasVerticalScrollbar(needsVerticalScrollbar);

    // With overflow: scroll, scrollbars are always visible but may be disabled.
    // When switching to another value, we need to re-enable them (see bug 11985).
    if (needsHorizontalScrollbar && oldStyle && oldStyle->overflowX() == OSCROLL && overflowX != OSCROLL) {
        ASSERT(hasHorizontalScrollbar());
        m_hBar->setEnabled(true);
    }

    if (needsVerticalScrollbar && oldStyle && oldStyle->overflowY() == OSCROLL && overflowY != OSCROLL) {
        ASSERT(hasVerticalScrollbar());
        m_vBar->setEnabled(true);
    }

    // FIXME: Need to detect a swap from custom to native scrollbars (and vice versa).
    if (m_hBar)
        m_hBar->styleChanged();
    if (m_vBar)
        m_vBar->styleChanged();

    updateScrollCornerStyle();
    updateResizerAreaSet();
    updateResizerStyle();
}

bool DeprecatedPaintLayerScrollableArea::updateAfterCompositingChange()
{
    layer()->updateScrollingStateAfterCompositingChange();
    const bool layersChanged = m_topmostScrollChild != m_nextTopmostScrollChild;
    m_topmostScrollChild = m_nextTopmostScrollChild;
    m_nextTopmostScrollChild = nullptr;
    return layersChanged;
}

void DeprecatedPaintLayerScrollableArea::updateAfterOverflowRecalc()
{
    computeScrollDimensions();
    if (Scrollbar* horizontalScrollbar = this->horizontalScrollbar()) {
        int clientWidth = box().pixelSnappedClientWidth();
        horizontalScrollbar->setProportion(clientWidth, overflowRect().width());
    }
    if (Scrollbar* verticalScrollbar = this->verticalScrollbar()) {
        int clientHeight = box().pixelSnappedClientHeight();
        verticalScrollbar->setProportion(clientHeight, overflowRect().height());
    }

    bool hasHorizontalOverflow = this->hasHorizontalOverflow();
    bool hasVerticalOverflow = this->hasVerticalOverflow();
    bool autoHorizontalScrollBarChanged = box().hasAutoHorizontalScrollbar() && (hasHorizontalScrollbar() != hasHorizontalOverflow);
    bool autoVerticalScrollBarChanged = box().hasAutoVerticalScrollbar() && (hasVerticalScrollbar() != hasVerticalOverflow);
    if (autoHorizontalScrollBarChanged || autoVerticalScrollBarChanged)
        box().setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::Unknown);
}

DoubleSize DeprecatedPaintLayerScrollableArea::clampScrollOffset(const DoubleSize& scrollOffset) const
{
    int maxX = scrollWidth() - box().pixelSnappedClientWidth();
    int maxY = scrollHeight() - box().pixelSnappedClientHeight();

    int scrollOffsetHeight = (int)(scrollOffset.height());

//     char* output = (char*)malloc(0x300);
//     sprintf_s(output, 0x299, "ScrollOffset: %p, %p"
//         "scrollOffset:%d, scrollHeight:%d ClientHeight:%d, maxY:%d frameRectHeight:%d borderTop:%d borderBottom:%d, horizontalScrollbarHeight:%d\n", this, &box(),
//         scrollOffsetHeight,
//         scrollHeight().toInt(), box().pixelSnappedClientHeight(),
//         maxY,
//         box().frameRect().height().toInt(),
//         box().borderTop(),
//         box().borderBottom(),
//         box().horizontalScrollbarHeight());
//     if (scrollOffsetHeight != 0) {
//         g_tesBox = &box();
//         OutputDebugStringA(output);
//     }
//     free(output);
    //////////////////////////////////////////////////////////////////////////
    IntSize contentSize;
    IntSize visibleSize;
    if (box().hasOverflowClip()) {
        contentSize = IntSize(pixelSnappedScrollWidth(), pixelSnappedScrollHeight());
        visibleSize = enclosingIntRect(box().overflowClipRect(LayoutPoint())).size();
    }
    IntPoint result = -scrollOrigin() + (contentSize - visibleSize);
    maxX = result.x();
    maxY = result.y();
    //////////////////////////////////////////////////////////////////////////

    double x = std::max(std::min(scrollOffset.width(), static_cast<double>(maxX)), 0.0);
    double y = std::max(std::min(scrollOffset.height(), static_cast<double>(maxY)), 0.0);

    return DoubleSize(x, y);
}

IntRect DeprecatedPaintLayerScrollableArea::rectForHorizontalScrollbar(const IntRect& borderBoxRect) const
{
    if (!m_hBar)
        return IntRect();

    const IntRect& scrollCorner = scrollCornerRect();

    return IntRect(horizontalScrollbarStart(borderBoxRect.x()),
        borderBoxRect.maxY() - box().borderBottom() - m_hBar->height(),
        borderBoxRect.width() - (box().borderLeft() + box().borderRight()) - scrollCorner.width(),
        m_hBar->height());
}

IntRect DeprecatedPaintLayerScrollableArea::rectForVerticalScrollbar(const IntRect& borderBoxRect) const
{
    if (!m_vBar)
        return IntRect();

    const IntRect& scrollCorner = scrollCornerRect();

    return IntRect(verticalScrollbarStart(borderBoxRect.x(), borderBoxRect.maxX()),
        borderBoxRect.y() + box().borderTop(),
        m_vBar->width(),
        borderBoxRect.height() - (box().borderTop() + box().borderBottom()) - scrollCorner.height());
}

LayoutUnit DeprecatedPaintLayerScrollableArea::verticalScrollbarStart(int minX, int maxX) const
{
    if (box().style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        return minX + box().borderLeft();
    return maxX - box().borderRight() - m_vBar->width();
}

LayoutUnit DeprecatedPaintLayerScrollableArea::horizontalScrollbarStart(int minX) const
{
    int x = minX + box().borderLeft();
    if (box().style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        x += m_vBar ? m_vBar->width() : resizerCornerRect(box().pixelSnappedBorderBoxRect(), ResizerForPointer).width();
    return x;
}

IntSize DeprecatedPaintLayerScrollableArea::scrollbarOffset(const Scrollbar* scrollbar) const
{
    if (scrollbar == m_vBar.get())
        return IntSize(verticalScrollbarStart(0, box().size().width()), box().borderTop());

    if (scrollbar == m_hBar.get())
        return IntSize(horizontalScrollbarStart(0), box().size().height() - box().borderBottom() - scrollbar->height());

    ASSERT_NOT_REACHED();
    return IntSize();
}

static inline LayoutObject* layoutObjectForScrollbar(LayoutObject& layoutObject)
{
    if (Node* node = layoutObject.node()) {
        if (ShadowRoot* shadowRoot = node->containingShadowRoot()) {
            if (shadowRoot->type() == ShadowRootType::UserAgent)
                return shadowRoot->host()->layoutObject();
        }
    }

    return &layoutObject;
}

PassRefPtrWillBeRawPtr<Scrollbar> DeprecatedPaintLayerScrollableArea::createScrollbar(ScrollbarOrientation orientation)
{
    RefPtrWillBeRawPtr<Scrollbar> widget = nullptr;
    LayoutObject* actualLayoutObject = layoutObjectForScrollbar(box());
    bool hasCustomScrollbarStyle = actualLayoutObject->isBox() && actualLayoutObject->style()->hasPseudoStyle(SCROLLBAR);
    if (hasCustomScrollbarStyle) {
        widget = LayoutScrollbar::createCustomScrollbar(this, orientation, actualLayoutObject->node());
    } else {
        ScrollbarControlSize scrollbarSize = RegularScrollbar;
        if (actualLayoutObject->style()->hasAppearance())
            scrollbarSize = LayoutTheme::theme().scrollbarControlSizeForPart(actualLayoutObject->style()->appearance());
        widget = Scrollbar::create(this, orientation, scrollbarSize);
        if (orientation == HorizontalScrollbar)
            didAddScrollbar(widget.get(), HorizontalScrollbar);
        else
            didAddScrollbar(widget.get(), VerticalScrollbar);
    }
    box().document().view()->addChild(widget.get());
    return widget.release();
}

void DeprecatedPaintLayerScrollableArea::destroyScrollbar(ScrollbarOrientation orientation)
{
    RefPtrWillBeMember<Scrollbar>& scrollbar = orientation == HorizontalScrollbar ? m_hBar : m_vBar;
    if (!scrollbar)
        return;

    if (!scrollbar->isCustomScrollbar())
        willRemoveScrollbar(scrollbar.get(), orientation);

    toFrameView(scrollbar->parent())->removeChild(scrollbar.get());
    scrollbar->disconnectFromScrollableArea();
    scrollbar = nullptr;
}

void DeprecatedPaintLayerScrollableArea::setHasHorizontalScrollbar(bool hasScrollbar)
{
    if (hasScrollbar == hasHorizontalScrollbar())
        return;

    if (hasScrollbar) {
        // This doesn't hit in any tests, but since the equivalent code in setHasVerticalScrollbar
        // does, presumably this code does as well.
        DisableCompositingQueryAsserts disabler;
        m_hBar = createScrollbar(HorizontalScrollbar);
    } else {
        if (!layerForHorizontalScrollbar())
            m_hBar->invalidate();
        // Otherwise we will remove the layer and just need recompositing.

        destroyScrollbar(HorizontalScrollbar);
    }

    // Destroying or creating one bar can cause our scrollbar corner to come and go. We need to update the opposite scrollbar's style.
    if (m_hBar)
        m_hBar->styleChanged();
    if (m_vBar)
        m_vBar->styleChanged();

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        // These are valid because we want to invalidate display item clients on the current backing.
        DisablePaintInvalidationStateAsserts paintInvalidationAssertDisabler;
        DisableCompositingQueryAsserts compositingAssertDisabler;
        invalidateScrollCorner(scrollCornerRect());
    }

    // Force an update since we know the scrollbars have changed things.
    if (box().document().hasAnnotatedRegions())
        box().document().setAnnotatedRegionsDirty(true);
}

void DeprecatedPaintLayerScrollableArea::setHasVerticalScrollbar(bool hasScrollbar)
{
    if (hasScrollbar == hasVerticalScrollbar())
        return;

    if (hasScrollbar) {
        // Hits in compositing/overflow/automatically-opt-into-composited-scrolling-after-style-change.html
        DisableCompositingQueryAsserts disabler;
        m_vBar = createScrollbar(VerticalScrollbar);
    } else {
        if (!layerForVerticalScrollbar())
            m_vBar->invalidate();
        // Otherwise we will remove the layer and just need recompositing.

        destroyScrollbar(VerticalScrollbar);
    }

    // Destroying or creating one bar can cause our scrollbar corner to come and go. We need to update the opposite scrollbar's style.
    if (m_hBar)
        m_hBar->styleChanged();
    if (m_vBar)
        m_vBar->styleChanged();

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        // These are valid because we want to invalidate display item clients on the current backing.
        DisablePaintInvalidationStateAsserts paintInvalidationAssertDisabler;
        DisableCompositingQueryAsserts compositingAssertDisabler;
        invalidateScrollCorner(scrollCornerRect());
    }

    // Force an update since we know the scrollbars have changed things.
    if (box().document().hasAnnotatedRegions())
        box().document().setAnnotatedRegionsDirty(true);
}

int DeprecatedPaintLayerScrollableArea::verticalScrollbarWidth(OverlayScrollbarSizeRelevancy relevancy) const
{
    if (!m_vBar || (m_vBar->isOverlayScrollbar() && (relevancy == IgnoreOverlayScrollbarSize || !m_vBar->shouldParticipateInHitTesting())))
        return 0;
    return m_vBar->width();
}

int DeprecatedPaintLayerScrollableArea::horizontalScrollbarHeight(OverlayScrollbarSizeRelevancy relevancy) const
{
    if (!m_hBar || (m_hBar->isOverlayScrollbar() && (relevancy == IgnoreOverlayScrollbarSize || !m_hBar->shouldParticipateInHitTesting())))
        return 0;
    return m_hBar->height();
}

void DeprecatedPaintLayerScrollableArea::positionOverflowControls()
{
    if (!hasScrollbar() && !box().canResize())
        return;

    const IntRect borderBox = box().pixelSnappedBorderBoxRect();
    if (Scrollbar* verticalScrollbar = this->verticalScrollbar())
        verticalScrollbar->setFrameRect(rectForVerticalScrollbar(borderBox));

    if (Scrollbar* horizontalScrollbar = this->horizontalScrollbar())
        horizontalScrollbar->setFrameRect(rectForHorizontalScrollbar(borderBox));

    const IntRect& scrollCorner = scrollCornerRect();
    if (m_scrollCorner)
        m_scrollCorner->setFrameRect(LayoutRect(scrollCorner));

    if (m_resizer)
        m_resizer->setFrameRect(LayoutRect(resizerCornerRect(borderBox, ResizerForPointer)));

    // FIXME, this should eventually be removed, once we are certain that composited
    // controls get correctly positioned on a compositor update. For now, conservatively
    // leaving this unchanged.
    if (layer()->hasCompositedDeprecatedPaintLayerMapping())
        layer()->compositedDeprecatedPaintLayerMapping()->positionOverflowControlsLayers();
}

void DeprecatedPaintLayerScrollableArea::updateScrollCornerStyle()
{
    if (!m_scrollCorner && !hasScrollbar())
        return;
    if (!m_scrollCorner && hasOverlayScrollbars())
        return;

    LayoutObject* actualLayoutObject = layoutObjectForScrollbar(box());
    RefPtr<ComputedStyle> corner = box().hasOverflowClip() ? actualLayoutObject->getUncachedPseudoStyle(PseudoStyleRequest(SCROLLBAR_CORNER), actualLayoutObject->style()) : PassRefPtr<ComputedStyle>(nullptr);
    if (corner) {
        if (!m_scrollCorner) {
            m_scrollCorner = LayoutScrollbarPart::createAnonymous(&box().document());
            m_scrollCorner->setParent(&box());
        }
        m_scrollCorner->setStyle(corner.release());
    } else if (m_scrollCorner) {
        m_scrollCorner->destroy();
        m_scrollCorner = nullptr;
    }
}

bool DeprecatedPaintLayerScrollableArea::hitTestOverflowControls(HitTestResult& result, const IntPoint& localPoint)
{
    if (!hasScrollbar() && !box().canResize())
        return false;

    IntRect resizeControlRect;
    if (box().style()->resize() != RESIZE_NONE) {
        resizeControlRect = resizerCornerRect(box().pixelSnappedBorderBoxRect(), ResizerForPointer);
        if (resizeControlRect.contains(localPoint))
            return true;
    }

    int resizeControlSize = max(resizeControlRect.height(), 0);
    if (m_vBar && m_vBar->shouldParticipateInHitTesting()) {
        LayoutRect vBarRect(verticalScrollbarStart(0, box().size().width()),
            box().borderTop(),
            m_vBar->width(),
            box().size().height() - (box().borderTop() + box().borderBottom()) - (m_hBar ? m_hBar->height() : resizeControlSize));
        if (vBarRect.contains(localPoint)) {
            result.setScrollbar(m_vBar.get());
            return true;
        }
    }

    resizeControlSize = max(resizeControlRect.width(), 0);
    if (m_hBar && m_hBar->shouldParticipateInHitTesting()) {
        LayoutRect hBarRect(horizontalScrollbarStart(0),
            box().size().height() - box().borderBottom() - m_hBar->height(),
            box().size().width() - (box().borderLeft() + box().borderRight()) - (m_vBar ? m_vBar->width() : resizeControlSize),
            m_hBar->height());
        if (hBarRect.contains(localPoint)) {
            result.setScrollbar(m_hBar.get());
            return true;
        }
    }

    // FIXME: We should hit test the m_scrollCorner and pass it back through the result.

    return false;
}

IntRect DeprecatedPaintLayerScrollableArea::resizerCornerRect(const IntRect& bounds, ResizerHitTestType resizerHitTestType) const
{
    if (box().style()->resize() == RESIZE_NONE)
        return IntRect();
    IntRect corner = cornerRect(box().styleRef(), horizontalScrollbar(), verticalScrollbar(), bounds);

    if (resizerHitTestType == ResizerForTouch) {
        // We make the resizer virtually larger for touch hit testing. With the
        // expanding ratio k = ResizerControlExpandRatioForTouch, we first move
        // the resizer rect (of width w & height h), by (-w * (k-1), -h * (k-1)),
        // then expand the rect by new_w/h = w/h * k.
        int expandRatio = ResizerControlExpandRatioForTouch - 1;
        corner.move(-corner.width() * expandRatio, -corner.height() * expandRatio);
        corner.expand(corner.width() * expandRatio, corner.height() * expandRatio);
    }

    return corner;
}

IntRect DeprecatedPaintLayerScrollableArea::scrollCornerAndResizerRect() const
{
    IntRect scrollCornerAndResizer = scrollCornerRect();
    if (scrollCornerAndResizer.isEmpty())
        scrollCornerAndResizer = resizerCornerRect(box().pixelSnappedBorderBoxRect(), ResizerForPointer);
    return scrollCornerAndResizer;
}

bool DeprecatedPaintLayerScrollableArea::isPointInResizeControl(const IntPoint& absolutePoint, ResizerHitTestType resizerHitTestType) const
{
    if (!box().canResize())
        return false;

    IntPoint localPoint = roundedIntPoint(box().absoluteToLocal(absolutePoint, UseTransforms));
    IntRect localBounds(0, 0, box().pixelSnappedWidth(), box().pixelSnappedHeight());
    return resizerCornerRect(localBounds, resizerHitTestType).contains(localPoint);
}

bool DeprecatedPaintLayerScrollableArea::hitTestResizerInFragments(const DeprecatedPaintLayerFragments& layerFragments, const HitTestLocation& hitTestLocation) const
{
    if (!box().canResize())
        return false;

    if (layerFragments.isEmpty())
        return false;

    for (int i = layerFragments.size() - 1; i >= 0; --i) {
        const DeprecatedPaintLayerFragment& fragment = layerFragments.at(i);
        if (fragment.backgroundRect.intersects(hitTestLocation) && resizerCornerRect(pixelSnappedIntRect(fragment.layerBounds), ResizerForPointer).contains(hitTestLocation.roundedPoint()))
            return true;
    }

    return false;
}

void DeprecatedPaintLayerScrollableArea::updateResizerAreaSet()
{
    LocalFrame* frame = box().frame();
    if (!frame)
        return;
    FrameView* frameView = frame->view();
    if (!frameView)
        return;
    if (box().canResize())
        frameView->addResizerArea(box());
    else
        frameView->removeResizerArea(box());
}

void DeprecatedPaintLayerScrollableArea::updateResizerStyle()
{
    if (!m_resizer && !box().canResize())
        return;

    LayoutObject* actualLayoutObject = layoutObjectForScrollbar(box());
    RefPtr<ComputedStyle> resizer = box().hasOverflowClip() ? actualLayoutObject->getUncachedPseudoStyle(PseudoStyleRequest(RESIZER), actualLayoutObject->style()) : PassRefPtr<ComputedStyle>(nullptr);
    if (resizer) {
        if (!m_resizer) {
            m_resizer = LayoutScrollbarPart::createAnonymous(&box().document());
            m_resizer->setParent(&box());
        }
        m_resizer->setStyle(resizer.release());
    } else if (m_resizer) {
        m_resizer->destroy();
        m_resizer = nullptr;
    }
}

IntSize DeprecatedPaintLayerScrollableArea::offsetFromResizeCorner(const IntPoint& absolutePoint) const
{
    // Currently the resize corner is either the bottom right corner or the bottom left corner.
    // FIXME: This assumes the location is 0, 0. Is this guaranteed to always be the case?
    IntSize elementSize = layer()->size();
    if (box().style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        elementSize.setWidth(0);
    IntPoint resizerPoint = IntPoint(elementSize);
    IntPoint localPoint = roundedIntPoint(box().absoluteToLocal(absolutePoint, UseTransforms));
    return localPoint - resizerPoint;
}

void DeprecatedPaintLayerScrollableArea::resize(const PlatformEvent& evt, const LayoutSize& oldOffset)
{
    // FIXME: This should be possible on generated content but is not right now.
    if (!inResizeMode() || !box().canResize() || !box().node())
        return;

    ASSERT(box().node()->isElementNode());
    Element* element = toElement(box().node());

    Document& document = element->document();

    IntPoint pos;
    const PlatformGestureEvent* gevt = 0;

    switch (evt.type()) {
    case PlatformEvent::MouseMoved:
        if (!document.frame()->eventHandler().mousePressed())
            return;
        pos = static_cast<const PlatformMouseEvent*>(&evt)->position();
        break;
    case PlatformEvent::GestureScrollUpdate:
        pos = static_cast<const PlatformGestureEvent*>(&evt)->position();
        gevt = static_cast<const PlatformGestureEvent*>(&evt);
        pos = gevt->position();
        pos.move(gevt->deltaX(), gevt->deltaY());
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    float zoomFactor = box().style()->effectiveZoom();

    IntSize newOffset = offsetFromResizeCorner(document.view()->rootFrameToContents(pos));
    newOffset.setWidth(newOffset.width() / zoomFactor);
    newOffset.setHeight(newOffset.height() / zoomFactor);

    LayoutSize currentSize = box().size();
    currentSize.scale(1 / zoomFactor);
    LayoutSize minimumSize = element->minimumSizeForResizing().shrunkTo(currentSize);
    element->setMinimumSizeForResizing(minimumSize);

    LayoutSize adjustedOldOffset = LayoutSize(oldOffset.width() / zoomFactor, oldOffset.height() / zoomFactor);
    if (box().style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft()) {
        newOffset.setWidth(-newOffset.width());
        adjustedOldOffset.setWidth(-adjustedOldOffset.width());
    }

    LayoutSize difference((currentSize + newOffset - adjustedOldOffset).expandedTo(minimumSize) - currentSize);

    bool isBoxSizingBorder = box().style()->boxSizing() == BORDER_BOX;

    EResize resize = box().style()->resize();
    if (resize != RESIZE_VERTICAL && difference.width()) {
        if (element->isFormControlElement()) {
            // Make implicit margins from the theme explicit (see <http://bugs.webkit.org/show_bug.cgi?id=9547>).
            element->setInlineStyleProperty(CSSPropertyMarginLeft, box().marginLeft() / zoomFactor, CSSPrimitiveValue::CSS_PX);
            element->setInlineStyleProperty(CSSPropertyMarginRight, box().marginRight() / zoomFactor, CSSPrimitiveValue::CSS_PX);
        }
        LayoutUnit baseWidth = box().size().width() - (isBoxSizingBorder ? LayoutUnit() : box().borderAndPaddingWidth());
        baseWidth = baseWidth / zoomFactor;
        element->setInlineStyleProperty(CSSPropertyWidth, roundToInt(baseWidth + difference.width()), CSSPrimitiveValue::CSS_PX);
    }

    if (resize != RESIZE_HORIZONTAL && difference.height()) {
        if (element->isFormControlElement()) {
            // Make implicit margins from the theme explicit (see <http://bugs.webkit.org/show_bug.cgi?id=9547>).
            element->setInlineStyleProperty(CSSPropertyMarginTop, box().marginTop() / zoomFactor, CSSPrimitiveValue::CSS_PX);
            element->setInlineStyleProperty(CSSPropertyMarginBottom, box().marginBottom() / zoomFactor, CSSPrimitiveValue::CSS_PX);
        }
        LayoutUnit baseHeight = box().size().height() - (isBoxSizingBorder ? LayoutUnit() : box().borderAndPaddingHeight());
        baseHeight = baseHeight / zoomFactor;
        element->setInlineStyleProperty(CSSPropertyHeight, roundToInt(baseHeight + difference.height()), CSSPrimitiveValue::CSS_PX);
    }

    document.updateLayout();

    // FIXME (Radar 4118564): We should also autoscroll the window as necessary to keep the point under the cursor in view.
}

LayoutRect DeprecatedPaintLayerScrollableArea::scrollIntoView(const LayoutRect& rect, const ScrollAlignment& alignX, const ScrollAlignment& alignY)
{
    LayoutRect localExposeRect(box().absoluteToLocalQuad(FloatQuad(FloatRect(rect)), UseTransforms).boundingBox());
    localExposeRect.move(-box().borderLeft(), -box().borderTop());
    LayoutRect layerBounds(0, 0, box().clientWidth(), box().clientHeight());
    LayoutRect r = ScrollAlignment::getRectToExpose(layerBounds, localExposeRect, alignX, alignY);

    DoubleSize clampedScrollOffset = clampScrollOffset(adjustedScrollOffset() + roundedIntSize(r.location()));
    if (clampedScrollOffset == adjustedScrollOffset())
        return rect;

    DoubleSize oldScrollOffset = adjustedScrollOffset();
    scrollToOffset(clampedScrollOffset);
    DoubleSize scrollOffsetDifference = adjustedScrollOffset() - oldScrollOffset;
    localExposeRect.move(-LayoutSize(scrollOffsetDifference));
    return LayoutRect(box().localToAbsoluteQuad(FloatQuad(FloatRect(localExposeRect)), UseTransforms).boundingBox());
}

void DeprecatedPaintLayerScrollableArea::updateScrollableAreaSet(bool hasOverflow)
{
    LocalFrame* frame = box().frame();
    if (!frame)
        return;

    FrameView* frameView = frame->view();
    if (!frameView)
        return;

    // FIXME: Does this need to be fixed later for OOPI?
    bool isVisibleToHitTest = box().visibleToHitTesting();
    if (HTMLFrameOwnerElement* owner = frame->deprecatedLocalOwner())
        isVisibleToHitTest &= owner->layoutObject() && owner->layoutObject()->visibleToHitTesting();

    bool didScrollOverflow = m_scrollsOverflow;

    m_scrollsOverflow = hasOverflow && isVisibleToHitTest;
    if (didScrollOverflow == scrollsOverflow())
        return;

    if (m_scrollsOverflow) {
        ASSERT(canHaveOverflowScrollbars(box()));
        frameView->addScrollableArea(this);
    } else {
        frameView->removeScrollableArea(this);
    }
}

void DeprecatedPaintLayerScrollableArea::updateCompositingLayersAfterScroll()
{
    DeprecatedPaintLayerCompositor* compositor = box().view()->compositor();
    if (compositor->inCompositingMode()) {
        if (usesCompositedScrolling()) {
            ASSERT(layer()->hasCompositedDeprecatedPaintLayerMapping());
            layer()->compositedDeprecatedPaintLayerMapping()->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateSubtree);
            compositor->setNeedsCompositingUpdate(CompositingUpdateAfterGeometryChange);
        } else {
            layer()->setNeedsCompositingInputsUpdate();
        }
    }
}

bool DeprecatedPaintLayerScrollableArea::usesCompositedScrolling() const
{
    // Scroll form controls on the main thread so they exhibit correct touch scroll event bubbling
    if (box().isIntrinsicallyScrollable(VerticalScrollbar) || box().isIntrinsicallyScrollable(HorizontalScrollbar))
        return false;

    // See https://codereview.chromium.org/176633003/ for the tests that fail without this disabler.
    DisableCompositingQueryAsserts disabler;
    return layer()->hasCompositedDeprecatedPaintLayerMapping() && layer()->compositedDeprecatedPaintLayerMapping()->scrollingLayer();
}

static bool layerNeedsCompositedScrolling(DeprecatedPaintLayerScrollableArea::LCDTextMode mode, const DeprecatedPaintLayer* layer)
{
    if (mode == DeprecatedPaintLayerScrollableArea::ConsiderLCDText && !layer->compositor()->preferCompositingToLCDTextEnabled())
        return false;

    return layer->scrollsOverflow()
        && !layer->hasDescendantWithClipPath()
        && !layer->hasAncestorWithClipPath()
        && !layer->layoutObject()->style()->hasBorderRadius();
}

void DeprecatedPaintLayerScrollableArea::updateNeedsCompositedScrolling(LCDTextMode mode)
{
    const bool needsCompositedScrolling = layerNeedsCompositedScrolling(mode, layer());
    if (static_cast<bool>(m_needsCompositedScrolling) != needsCompositedScrolling) {
        m_needsCompositedScrolling = needsCompositedScrolling;
        layer()->didUpdateNeedsCompositedScrolling();
    }
}

void DeprecatedPaintLayerScrollableArea::setTopmostScrollChild(DeprecatedPaintLayer* scrollChild)
{
    // We only want to track the topmost scroll child for scrollable areas with
    // overlay scrollbars.
    if (!hasOverlayScrollbars())
        return;
    m_nextTopmostScrollChild = scrollChild;
}

} // namespace blink
