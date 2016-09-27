/*
   Copyright (C) 1997 Martin Jones (mjones@kde.org)
             (C) 1998 Waldo Bastian (bastian@kde.org)
             (C) 1998, 1999 Torben Weis (weis@kde.org)
             (C) 1999 Lars Knoll (knoll@kde.org)
             (C) 1999 Antti Koivisto (koivisto@kde.org)
   Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef FrameView_h
#define FrameView_h

#include "core/CoreExport.h"
#include "core/frame/FrameViewAutoSizeInfo.h"
#include "core/frame/LayoutSubtreeRootList.h"
#include "core/frame/RootFrameViewport.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/paint/PaintPhase.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/Widget.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/Color.h"
#include "platform/scroll/ScrollTypes.h"
#include "platform/scroll/ScrollableArea.h"
#include "platform/scroll/Scrollbar.h"
#include "public/platform/WebDisplayMode.h"
#include "public/platform/WebRect.h"
#include "wtf/Forward.h"
#include "wtf/HashSet.h"
#include "wtf/ListHashSet.h"
#include "wtf/OwnPtr.h"
#include "wtf/TemporaryChange.h"
#include "wtf/text/WTFString.h"

namespace blink {

class AXObjectCache;
class DocumentLifecycle;
class Cursor;
class Element;
class FloatSize;
class HTMLFrameOwnerElement;
class LayoutPart;
class LocalFrame;
class KURL;
class Node;
class LayoutBox;
class LayoutEmbeddedObject;
class LayoutObject;
class LayoutScrollbarPart;
class LayoutView;
class PaintInvalidationState;
class Page;
class ScrollingCoordinator;
class TracedValue;
struct AnnotatedRegionValue;
struct CompositedSelection;

typedef unsigned long long DOMTimeStamp;

class CORE_EXPORT FrameView final : public Widget, public ScrollableArea {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(FrameView);
public:
    friend class LayoutView;
    friend class Internals;

    static PassRefPtrWillBeRawPtr<FrameView> create(LocalFrame*);
    static PassRefPtrWillBeRawPtr<FrameView> create(LocalFrame*, const IntSize& initialSize);

    virtual ~FrameView();

    virtual void invalidateRect(const IntRect&) override;
    virtual void setFrameRect(const IntRect&) override;

    LocalFrame& frame() const
    {
        ASSERT(m_frame);
        return *m_frame;
    }

    Page* page() const;

    LayoutView* layoutView() const;

    void setCanHaveScrollbars(bool);

    PassRefPtrWillBeRawPtr<Scrollbar> createScrollbar(ScrollbarOrientation);

    void setContentsSize(const IntSize&);
    IntPoint clampOffsetAtScale(const IntPoint& offset, float scale) const;

    void layout();
    bool didFirstLayout() const;
    void scheduleRelayout();
    void scheduleRelayoutOfSubtree(LayoutObject*);
    bool layoutPending() const;
    bool isInPerformLayout() const;

    void clearLayoutSubtreeRoot(const LayoutObject&);
    int layoutCount() const { return m_layoutCount; }

    void countObjectsNeedingLayout(unsigned& needsLayoutObjects, unsigned& totalObjects, bool& isPartial);

    bool needsLayout() const;
    void setNeedsLayout();

    void setNeedsUpdateWidgetPositions() { m_needsUpdateWidgetPositions = true; }

    // Methods for getting/setting the size Blink should use to layout the contents.
    // NOTE: Scrollbar exclusion is based on the FrameView's scrollbars. To exclude
    // scrollbars on the root DeprecatedPaintLayer, use LayoutView::layoutSize.
    IntSize layoutSize(IncludeScrollbarsInRect = ExcludeScrollbars) const;
    void setLayoutSize(const IntSize&);

    // If this is set to false, the layout size will need to be explicitly set by the owner.
    // E.g. WebViewImpl sets its mainFrame's layout size manually
    void setLayoutSizeFixedToFrameSize(bool isFixed) { m_layoutSizeFixedToFrameSize = isFixed; }
    bool layoutSizeFixedToFrameSize() { return m_layoutSizeFixedToFrameSize; }

    bool needsFullPaintInvalidation() const { return m_doFullPaintInvalidation; }
    void setNeedsFullPaintInvalidation() { m_doFullPaintInvalidation = true; }

    void updateAcceleratedCompositingSettings();

    void recalcOverflowAfterStyleChange();

    bool isEnclosedInCompositingLayer() const;

    void resetScrollbars();
    void dispose() override;
    void detachScrollbars();
    void recalculateCustomScrollbarStyle();
    void invalidateAllCustomScrollbarsOnActiveChanged();
    virtual void recalculateScrollbarOverlayStyle();

    void clear();

    bool isTransparent() const;
    void setTransparent(bool isTransparent);

    // True if the FrameView is not transparent, and the base background color is opaque.
    bool hasOpaqueBackground() const;

    Color baseBackgroundColor() const;
    void setBaseBackgroundColor(const Color&);
    void updateBackgroundRecursively(const Color&, bool);

    void adjustViewSize();

    // |unobscuredRect| receives the clip rect that is not clipped to the root window. It may be nullptr.
    IntRect clipRectsForFrameOwner(const HTMLFrameOwnerElement*, IntRect* unobscuredRect) const;

    // Scale used to convert incoming input events.
    float inputEventsScaleFactor() const;

    // Offset used to convert incoming input events while emulating device metics.
    IntSize inputEventsOffsetForEmulation() const;
    void setInputEventsTransformForEmulation(const IntSize&, float);

    virtual void setScrollPosition(const DoublePoint&, ScrollType, ScrollBehavior = ScrollBehaviorInstant) override;

    void didUpdateElasticOverscroll();

    void viewportSizeChanged(bool widthChanged, bool heightChanged);

    AtomicString mediaType() const;
    void setMediaType(const AtomicString&);
    void adjustMediaTypeForPrinting(bool printing);

    WebDisplayMode displayMode() { return m_displayMode; }
    void setDisplayMode(WebDisplayMode);

    void addSlowRepaintObject();
    void removeSlowRepaintObject();
    bool hasSlowRepaintObjects() const { return m_slowRepaintObjectCount; }

    // Fixed-position objects.
    typedef HashSet<LayoutObject*> ViewportConstrainedObjectSet;
    void addViewportConstrainedObject(LayoutObject*);
    void removeViewportConstrainedObject(LayoutObject*);
    const ViewportConstrainedObjectSet* viewportConstrainedObjects() const { return m_viewportConstrainedObjects.get(); }
    bool hasViewportConstrainedObjects() const { return m_viewportConstrainedObjects && m_viewportConstrainedObjects->size() > 0; }

    void handleLoadCompleted();

    void updateAnnotatedRegions();

    void restoreScrollbar();

    void postLayoutTimerFired(Timer<FrameView>*);

    bool wasScrolledByUser() const;
    void setWasScrolledByUser(bool);

    bool safeToPropagateScrollToParent() const { return m_safeToPropagateScrollToParent; }
    void setSafeToPropagateScrollToParent(bool isSafe) { m_safeToPropagateScrollToParent = isSafe; }

    void addPart(LayoutPart*);
    void removePart(LayoutPart*);

    void updateWidgetPositions();

    void addPartToUpdate(LayoutEmbeddedObject&);

    void setPaintBehavior(PaintBehavior);
    PaintBehavior paintBehavior() const;
    void setIsPainting(bool val) { m_isPainting = val; }
    bool isPainting() const;
    void setLastPaintTime(double val) { m_lastPaintTime = val; }
    bool hasEverPainted() const { return m_lastPaintTime; }
    void setNodeToDraw(Node*);
    Node* nodeToDraw() { return m_nodeToDraw.get(); }

    Color documentBackgroundColor() const;

    // Run all needed lifecycle stages. After calling this method, all frames will be in the lifecycle state PaintInvalidationClean.
    void updateAllLifecyclePhases();

    // Computes the style, layout and compositing lifecycle stages if needed. After calling this method, all frames wil lbe in a lifecycle
    // state >= CompositingClean, and scrolling has been updated.
    void updateLifecycleToCompositingCleanPlusScrolling();

    bool invalidateViewportConstrainedObjects();

    void incrementVisuallyNonEmptyCharacterCount(unsigned);
    void incrementVisuallyNonEmptyPixelCount(const IntSize&);
    void setIsVisuallyNonEmpty() { m_isVisuallyNonEmpty = true; }
    void enableAutoSizeMode(const IntSize& minSize, const IntSize& maxSize);
    void disableAutoSizeMode();

    void forceLayoutForPagination(const FloatSize& pageSize, const FloatSize& originalPageSize, float maximumShrinkFactor);

    enum UrlFragmentBehavior {
        UrlFragmentScroll,
        UrlFragmentDontScroll
    };
    bool processUrlFragment(const KURL&, UrlFragmentBehavior = UrlFragmentScroll);

    // Methods to convert points and rects between the coordinate space of the layoutObject, and this view.
    IntRect convertFromLayoutObject(const LayoutObject&, const IntRect&) const;
    IntRect convertToLayoutObject(const LayoutObject&, const IntRect&) const;
    IntPoint convertFromLayoutObject(const LayoutObject&, const IntPoint&) const;
    IntPoint convertToLayoutObject(const LayoutObject&, const IntPoint&) const;

    bool isFrameViewScrollCorner(LayoutScrollbarPart* scrollCorner) const { return m_scrollCorner == scrollCorner; }

    enum ScrollingReasons {
        Scrollable,
        NotScrollableNoOverflow,
        NotScrollableNotVisible,
        NotScrollableExplicitlyDisabled
    };

    ScrollingReasons scrollingReasons();
    virtual bool isScrollable() override;
    virtual bool isProgrammaticallyScrollable() override;

    enum ScrollbarModesCalculationStrategy { RulesFromWebContentOnly, AnyRule };
    void calculateScrollbarModesForLayout(ScrollbarMode& hMode, ScrollbarMode& vMode, ScrollbarModesCalculationStrategy = AnyRule);

    virtual IntPoint lastKnownMousePosition() const override;
    bool shouldSetCursor() const;

    void setCursor(const Cursor&);

    virtual bool scrollbarsCanBeActive() const override;

    // FIXME: Remove this method once plugin loading is decoupled from layout.
    void flushAnyPendingPostLayoutTasks();

    virtual bool shouldSuspendScrollAnimations() const override;
    virtual void scrollbarStyleChanged() override;

    LayoutBox* embeddedContentBox() const;

    void setTracksPaintInvalidations(bool);
    bool isTrackingPaintInvalidations() const { return m_isTrackingPaintInvalidations; }
    void resetTrackedPaintInvalidations();

    String trackedPaintInvalidationRectsAsText() const;

    using ScrollableAreaSet = WillBeHeapHashSet<RawPtrWillBeMember<ScrollableArea>>;
    void addScrollableArea(ScrollableArea*);
    void removeScrollableArea(ScrollableArea*);
    const ScrollableAreaSet* scrollableAreas() const { return m_scrollableAreas.get(); }

    void addAnimatingScrollableArea(ScrollableArea*);
    void removeAnimatingScrollableArea(ScrollableArea*);
    const ScrollableAreaSet* animatingScrollableAreas() const { return m_animatingScrollableAreas.get(); }

    // With CSS style "resize:" enabled, a little resizer handle will appear at the bottom
    // right of the object. We keep track of these resizer areas for checking if touches
    // (implemented using Scroll gesture) are targeting the resizer.
    typedef HashSet<LayoutBox*> ResizerAreaSet;
    void addResizerArea(LayoutBox&);
    void removeResizerArea(LayoutBox&);
    const ResizerAreaSet* resizerAreas() const { return m_resizerAreas.get(); }

    // This function exists for ports that need to handle wheel events manually.
    // On Mac WebKit1 the underlying NSScrollView just does the scrolling, but on most other platforms
    // we need this function in order to do the scroll ourselves.
    ScrollResult wheelEvent(const PlatformWheelEvent&);

    virtual bool shouldUseIntegerScrollOffset() const override;

    virtual bool isActive() const override;

    // Override scrollbar notifications to update the AXObject cache.
    virtual void didAddScrollbar(Scrollbar*, ScrollbarOrientation) override;

    // FIXME: This should probably be renamed as the 'inSubtreeLayout' parameter
    // passed around the FrameView layout methods can be true while this returns
    // false.
    bool isSubtreeLayout() const { return !m_layoutSubtreeRootList.isEmpty(); }

    // Sets the tickmarks for the FrameView, overriding the default behavior
    // which is to display the tickmarks corresponding to find results.
    // If |m_tickmarks| is empty, the default behavior is restored.
    void setTickmarks(const Vector<IntRect>& tickmarks)
    {
        m_tickmarks = tickmarks;
        invalidatePaintForTickmarks();
    }

    void invalidatePaintForTickmarks() const;

    // Since the compositor can resize the viewport due to top controls and
    // commit scroll offsets before a WebView::resize occurs, we need to adjust
    // our scroll extents to prevent clamping the scroll offsets.
    void setTopControlsViewportAdjustment(float);

    virtual IntPoint maximumScrollPosition() const override;

    // ScrollableArea interface
    virtual void invalidateScrollbarRect(Scrollbar*, const IntRect&) override;
    virtual void getTickmarks(Vector<IntRect>&) const override;
    void scrollTo(const DoublePoint&);
    virtual IntRect scrollableAreaBoundingBox() const override;
    virtual bool scrollAnimatorEnabled() const override;
    virtual bool usesCompositedScrolling() const override;
    virtual GraphicsLayer* layerForScrolling() const override;
    virtual GraphicsLayer* layerForHorizontalScrollbar() const override;
    virtual GraphicsLayer* layerForVerticalScrollbar() const override;
    virtual GraphicsLayer* layerForScrollCorner() const override;
    virtual int scrollSize(ScrollbarOrientation) const override;
    virtual bool isScrollCornerVisible() const override;
    virtual bool userInputScrollable(ScrollbarOrientation) const override;
    virtual bool shouldPlaceVerticalScrollbarOnLeft() const override;
    virtual LayoutRect scrollIntoView(
        const LayoutRect& rectInContent,
        const ScrollAlignment& alignX,
        const ScrollAlignment& alignY) override;

    // The window that hosts the FrameView. The FrameView will communicate scrolls and repaints to the
    // host window in the window's coordinate space.
    HostWindow* hostWindow() const override;

    // Returns a clip rect in host window coordinates. Used to clip the blit on a scroll.
    IntRect windowClipRect(IncludeScrollbarsInRect = ExcludeScrollbars) const;

    typedef WillBeHeapHashSet<RefPtrWillBeMember<Widget>> ChildrenWidgetSet;

    // Functions for child manipulation and inspection.
    virtual void setParent(Widget*) override;
    void removeChild(Widget*);
    void addChild(PassRefPtrWillBeRawPtr<Widget>);
    const ChildrenWidgetSet* children() const { return &m_children; }

    // If the scroll view does not use a native widget, then it will have cross-platform Scrollbars. These functions
    // can be used to obtain those scrollbars.
    virtual Scrollbar* horizontalScrollbar() const override { return m_horizontalScrollbar.get(); }
    virtual Scrollbar* verticalScrollbar() const override { return m_verticalScrollbar.get(); }
    LayoutScrollbarPart* scrollCorner() { return m_scrollCorner; }

    void positionScrollbarLayers();

    // Functions for setting and retrieving the scrolling mode in each axis (horizontal/vertical). The mode has values of
    // AlwaysOff, AlwaysOn, and Auto. AlwaysOff means never show a scrollbar, AlwaysOn means always show a scrollbar.
    // Auto means show a scrollbar only when one is needed.
    // Note that for platforms with native widgets, these modes are considered advisory. In other words the underlying native
    // widget may choose not to honor the requested modes.
    void setScrollbarModes(ScrollbarMode horizontalMode, ScrollbarMode verticalMode, bool horizontalLock = false, bool verticalLock = false);
    void setHorizontalScrollbarMode(ScrollbarMode mode, bool lock = false) { setScrollbarModes(mode, verticalScrollbarMode(), lock, verticalScrollbarLock()); }
    void setVerticalScrollbarMode(ScrollbarMode mode, bool lock = false) { setScrollbarModes(horizontalScrollbarMode(), mode, horizontalScrollbarLock(), lock); }
    ScrollbarMode horizontalScrollbarMode() const { return m_horizontalScrollbarMode; }
    ScrollbarMode verticalScrollbarMode() const { return m_verticalScrollbarMode; }

    void setHorizontalScrollbarLock(bool lock = true) { m_horizontalScrollbarLock = lock; }
    bool horizontalScrollbarLock() const { return m_horizontalScrollbarLock; }
    void setVerticalScrollbarLock(bool lock = true) { m_verticalScrollbarLock = lock; }
    bool verticalScrollbarLock() const { return m_verticalScrollbarLock; }

    void setScrollingModesLock(bool lock = true) { m_horizontalScrollbarLock = m_verticalScrollbarLock = lock; }

    bool canHaveScrollbars() const { return horizontalScrollbarMode() != ScrollbarAlwaysOff || verticalScrollbarMode() != ScrollbarAlwaysOff; }

    // By default, paint events are clipped to the visible area.  If set to
    // false, paint events are no longer clipped.
    bool clipsPaintInvalidations() const { return m_clipsRepaints; }
    void setClipsRepaints(bool);

    // The visible content rect has a location that is the scrolled offset of
    // the document. The width and height are the layout viewport width and
    // height. By default the scrollbars themselves are excluded from this
    // rectangle, but an optional boolean argument allows them to be included.
    virtual IntRect visibleContentRect(IncludeScrollbarsInRect = ExcludeScrollbars) const override;
    IntSize visibleContentSize(IncludeScrollbarsInRect = ExcludeScrollbars) const;

    // Functions for getting/setting the size of the document contained inside the FrameView (as an IntSize or as individual width and height
    // values).
    virtual IntSize contentsSize() const override; // Always at least as big as the visibleWidth()/visibleHeight().
    int contentsWidth() const { return contentsSize().width(); }
    int contentsHeight() const { return contentsSize().height(); }

    // Functions for querying the current scrolled position (both as a point, a size, or as individual X and Y values).
    // Be careful in using the Double version scrollPositionDouble() and scrollOffsetDouble(). They are meant to be
    // used to communicate the fractional scroll position/offset with chromium compositor which can do sub-pixel positioning.
    // Do not call these if the scroll position/offset is used in Blink for positioning. Use the Int version instead.
    virtual IntPoint scrollPosition() const override { return visibleContentRect().location(); }
    virtual DoublePoint scrollPositionDouble() const override { return m_scrollPosition; }
    IntSize scrollOffset() const { return toIntSize(visibleContentRect().location()); } // Gets the scrolled position as an IntSize. Convenient for adding to other sizes.
    DoubleSize scrollOffsetDouble() const { return DoubleSize(m_scrollPosition.x(), m_scrollPosition.y()); }
    DoubleSize pendingScrollDelta() const { return m_pendingScrollDelta; }
    virtual IntPoint minimumScrollPosition() const override; // The minimum position we can be scrolled to.
    int scrollX() const { return scrollPosition().x(); }
    int scrollY() const { return scrollPosition().y(); }

    void cacheCurrentScrollPosition() { m_cachedScrollPosition = scrollPositionDouble(); }
    DoublePoint cachedScrollPosition() const { return m_cachedScrollPosition; }

    // Scroll the actual contents of the view (either blitting or invalidating as needed).
    void scrollContents(const IntSize& scrollDelta);

    // This gives us a means of blocking painting on our scrollbars until the first layout has occurred.
    void setScrollbarsSuppressed(bool suppressed, bool repaintOnUnsuppress = false);
    bool scrollbarsSuppressed() const { return m_scrollbarsSuppressed; }

    // Methods for converting between this frame and other coordinate spaces.
    // For definitions and an explanation of the varous spaces, please see:
    // http://www.chromium.org/developers/design-documents/blink-coordinate-spaces
    IntPoint rootFrameToContents(const IntPoint&) const;
    FloatPoint rootFrameToContents(const FloatPoint&) const;
    IntRect rootFrameToContents(const IntRect&) const;
    IntPoint contentsToRootFrame(const IntPoint&) const;
    IntRect contentsToRootFrame(const IntRect&) const;

    IntRect viewportToContents(const IntRect&) const;
    IntRect contentsToViewport(const IntRect&) const;
    IntPoint contentsToViewport(const IntPoint&) const;
    IntPoint viewportToContents(const IntPoint&) const;

    // FIXME: Some external callers expect to get back a rect that's positioned
    // in viewport space, but sized in CSS pixels. This is an artifact of the
    // old pinch-zoom path. These callers should be converted to expect a rect
    // fully in viewport space. crbug.com/459591.
    IntRect soonToBeRemovedContentsToUnscaledViewport(const IntRect&) const;
    IntPoint soonToBeRemovedUnscaledViewportToContents(const IntPoint&) const;

    // Methods for converting between Frame and Content (i.e. Document) coordinates.
    // Frame coordinates are relative to the top left corner of the frame and so
    // they are affected by scroll offset. Content coordinates are relative to the
    // document's top left corner and thus are not affected by scroll offset.
    IntPoint contentsToFrame(const IntPoint&) const;
    IntRect contentsToFrame(const IntRect&) const;
    IntPoint frameToContents(const IntPoint&) const;
    FloatPoint frameToContents(const FloatPoint&) const;
    IntRect frameToContents(const IntRect&) const;

    // Functions for converting to screen coordinates.
    IntRect contentsToScreen(const IntRect&) const;

    // These functions are used to enable scrollbars to avoid window resizer controls that overlap the scroll view.
    // This happens only on Mac OS X 10.6.
    IntRect windowResizerRect() const;
    bool containsScrollbarsAvoidingResizer() const;
    void adjustScrollbarsAvoidingResizerCount(int overlapDelta);
    void windowResizerRectChanged();

    // For platforms that need to hit test scrollbars from within the engine's event handlers (like Win32).
    Scrollbar* scrollbarAtRootFramePoint(const IntPoint&);
    Scrollbar* scrollbarAtFramePoint(const IntPoint&);

    virtual IntPoint convertChildToSelf(const Widget* child, const IntPoint& point) const override
    {
        IntPoint newPoint = point;
        if (!isFrameViewScrollbar(child))
            newPoint = contentsToFrameAero(point);
        newPoint.moveBy(child->location());
        return newPoint;
    }

    virtual IntPoint convertSelfToChild(const Widget* child, const IntPoint& point) const override
    {
        IntPoint newPoint = point;
        if (!isFrameViewScrollbar(child))
            newPoint = frameToContents(point);
        newPoint.moveBy(-child->location());
        return newPoint;
    }

    // Widget override. Handles painting of the contents of the view as well as the scrollbars.
    virtual void paint(GraphicsContext*, const IntRect&) override;
    void paintContents(GraphicsContext*, const IntRect& damageRect);

    // Widget overrides to ensure that our children's visibility status is kept up to date when we get shown and hidden.
    virtual void show() override;
    virtual void hide() override;
    virtual void setParentVisible(bool) override;

    bool isPointInScrollbarCorner(const IntPoint&);
    bool scrollbarCornerPresent() const;
    virtual IntRect scrollCornerRect() const override;

    virtual IntRect convertFromScrollbarToContainingView(const Scrollbar*, const IntRect&) const override;
    virtual IntRect convertFromContainingViewToScrollbar(const Scrollbar*, const IntRect&) const override;
    virtual IntPoint convertFromScrollbarToContainingView(const Scrollbar*, const IntPoint&) const override;
    virtual IntPoint convertFromContainingViewToScrollbar(const Scrollbar*, const IntPoint&) const override;

    virtual bool isFrameView() const override { return true; }

    DECLARE_VIRTUAL_TRACE();
    void notifyPageThatContentAreaWillPaint() const;
    FrameView* parentFrameView() const;

    // Returns the scrollable area for the frame. For the root frame, this will
    // be the RootFrameViewport, which adds pinch-zoom semantics to scrolling.
    // For non-root frames, this will be the the ScrollableArea used by the
    // FrameView, depending on whether root-layer-scrolls is enabled.
    ScrollableArea* scrollableArea();

    // Used to get at the underlying layoutViewport in the rare instances where
    // we actually want to scroll *just* the layout viewport (e.g. when sending
    // deltas from CC). For typical scrolling cases, use scrollableArea().
    ScrollableArea* layoutViewportScrollableArea();

    int viewportWidth() const;

    LayoutAnalyzer* layoutAnalyzer() { return m_analyzer.get(); }

    // Returns true if the default scrolling direction is vertical. i.e. writing mode
    // is horiziontal. In a vertical document, a spacebar scrolls down.
    bool isVerticalDocument() const;

    // Returns true if the document's writing mode is right-to-left or bottom-to-top.
    bool isFlippedDocument() const;

protected:
    // Scroll the content via the compositor.
    bool scrollContentsFastPath(const IntSize& scrollDelta);

    // Scroll the content by invalidating everything.
    void scrollContentsSlowPath(const IntRect& updateRect);

    // Prevents creation of scrollbars. Used to prevent drawing two sets of
    // overlay scrollbars in the case of the pinch viewport.
    bool scrollbarsDisabled() const;

    // These functions are used to create/destroy scrollbars.
    void setHasHorizontalScrollbar(bool);
    void setHasVerticalScrollbar(bool);

    virtual void invalidateScrollCornerRect(const IntRect&) override;
    virtual ScrollBehavior scrollBehaviorStyle() const override;

    void scrollContentsIfNeeded();

    void setScrollOrigin(const IntPoint&, bool updatePositionAtAll, bool updatePositionSynchronously);

    enum ComputeScrollbarExistenceOption {
        FirstPass,
        Incremental
    };
    void computeScrollbarExistence(bool& newHasHorizontalScrollbar, bool& newHasVerticalScrollbar, const IntSize& docSize, ComputeScrollbarExistenceOption = FirstPass) const;
    void updateScrollbarGeometry();
    IntRect adjustScrollbarRectForResizer(const IntRect&, Scrollbar*);

    // Called to update the scrollbars to accurately reflect the state of the view.
    void updateScrollbars(const DoubleSize& desiredOffset);

    class InUpdateScrollbarsScope {
    public:
        explicit InUpdateScrollbarsScope(FrameView* view)
            : m_scope(view->m_inUpdateScrollbars, true)
        { }
    private:
        TemporaryChange<bool> m_scope;
    };

    // Only for LayoutPart to traverse into sub frames during paint invalidation.
    void invalidateTreeIfNeeded(PaintInvalidationState&);

private:
    explicit FrameView(LocalFrame*);

    virtual void setScrollOffset(const IntPoint&, ScrollType) override;
    virtual void setScrollOffset(const DoublePoint&, ScrollType) override;

    void updateAllLifecyclePhasesInternal();
    void invalidateTreeIfNeededRecursive();
    void scrollContentsIfNeededRecursive();
    void updateStyleAndLayoutIfNeededRecursive();
    void updatePostLifecycleData();

    void reset();
    void init();

    void clearLayoutSubtreeRootsAndMarkContainingBlocks();

    // Called when our frame rect changes (or the rect/scroll position of an ancestor changes).
    virtual void frameRectsChanged() override;

    friend class LayoutPart;

    bool contentsInCompositedLayer() const;

    void applyOverflowToViewport(LayoutObject*, ScrollbarMode& hMode, ScrollbarMode& vMode);

    void updateCounters();
    void forceLayoutParentViewIfNeeded();
    void performPreLayoutTasks();
    void performLayout(bool inSubtreeLayout);
    void scheduleOrPerformPostLayoutTasks();
    void performPostLayoutTasks();

    DocumentLifecycle& lifecycle() const;

    void contentRectangleForPaintInvalidation(const IntRect&);
    virtual void contentsResized() override;
    void scrollbarExistenceDidChange();

    // Override Widget methods to do point conversion via layoutObjects, in order to
    // take transforms into account.
    virtual IntRect convertToContainingView(const IntRect&) const override;
    virtual IntRect convertFromContainingView(const IntRect&) const override;
    virtual IntPoint convertToContainingView(const IntPoint&) const override;
    virtual IntPoint convertFromContainingView(const IntPoint&) const override;

    void updateWidgetPositionsIfNeeded();

    bool wasViewportResized();
    void sendResizeEventIfNeeded();

    void updateScrollableAreaSet();

    void scheduleUpdateWidgetsIfNecessary();
    void updateWidgetsTimerFired(Timer<FrameView>*);
    bool updateWidgets();

    bool processUrlFragmentHelper(const String&, UrlFragmentBehavior);
    void maintainScrollPositionAtAnchor(Node*);
    void scrollToAnchor();
    void scrollPositionChanged();
    void didScrollTimerFired(Timer<FrameView>*);

    void updateLayersAndCompositingAfterScrollIfNeeded();

    static bool computeCompositedSelection(LocalFrame&, CompositedSelection&);
    void updateCompositedSelectionIfNeeded();

    // Returns true if the FrameView's own scrollbars overlay its content when visible.
    bool hasOverlayScrollbars() const;

    // Returns true if the frame should use custom scrollbars. If true, one of
    // either |customScrollbarElement| or |customScrollbarFrame| will be set to
    // the element or frame which owns the scrollbar with the other set to null.
    bool shouldUseCustomScrollbars(Element*& customScrollbarElement, LocalFrame*& customScrollbarFrame) const;

    // Returns true if a scrollbar needs to go from native -> custom or vice versa.
    bool needsScrollbarReconstruction() const;

    bool shouldIgnoreOverflowHidden() const;

    void updateScrollCorner();

    AXObjectCache* axObjectCache() const;
    void removeFromAXObjectCache();

    void setLayoutSizeInternal(const IntSize&);

    bool adjustScrollbarExistence(ComputeScrollbarExistenceOption = FirstPass);
    void adjustScrollbarOpacity();
    void setScrollOffsetFromUpdateScrollbars(const DoubleSize&);

    IntRect rectToCopyOnScroll() const;

    bool isFrameViewScrollbar(const Widget* child) const { return horizontalScrollbar() == child || verticalScrollbar() == child; }

    ScrollingCoordinator* scrollingCoordinator();

    void prepareLayoutAnalyzer();
    PassRefPtr<TracedValue> analyzerCounters();

    // LayoutObject for the viewport-defining element (see Document::viewportDefiningElement).
    LayoutObject* viewportLayoutObject();

    void collectAnnotatedRegions(LayoutObject&, Vector<AnnotatedRegionValue>&);

    typedef WTF::HashMap <const GraphicsLayer*, Vector<std::pair<int64_t, WebRect>>> GraphicsLayerFrameTimingRequests;
    void updateFrameTimingRequestsIfNeeded();
    void collectFrameTimingRequests(GraphicsLayerFrameTimingRequests&);
    void collectFrameTimingRequestsRecursive(GraphicsLayerFrameTimingRequests&);

    LayoutSize m_size;

    typedef HashSet<RefPtr<LayoutEmbeddedObject>> EmbeddedObjectSet;
    EmbeddedObjectSet m_partUpdateSet;

    // FIXME: These are just "children" of the FrameView and should be RefPtrWillBeMember<Widget> instead.
    HashSet<RefPtr<LayoutPart>> m_parts;

    // The RefPtr cycle between LocalFrame and FrameView is broken
    // when a LocalFrame is detached by FrameLoader::detachFromParent().
    // It clears the LocalFrame's m_view reference via setView(nullptr).
    //
    // For Oilpan, Member reference cycles pose no problem, but
    // LocalFrame's FrameView is also cleared by setView(). This additionally
    // triggers FrameView::dispose(), which performs the operations
    // that cannot be delayed until finalization time.
    RefPtrWillBeMember<LocalFrame> m_frame;

    WebDisplayMode m_displayMode;

    bool m_doFullPaintInvalidation;

    bool m_canHaveScrollbars;
    unsigned m_slowRepaintObjectCount;

    bool m_hasPendingLayout;
    LayoutSubtreeRootList m_layoutSubtreeRootList;

    bool m_layoutSchedulingEnabled;
    bool m_inPerformLayout;
    bool m_inSynchronousPostLayout;
    int m_layoutCount;
    unsigned m_nestedLayoutCount;
    Timer<FrameView> m_postLayoutTasksTimer;
    Timer<FrameView> m_updateWidgetsTimer;
    bool m_firstLayoutCallbackPending;

    bool m_firstLayout;
    bool m_isTransparent;
    Color m_baseBackgroundColor;
    IntSize m_lastViewportSize;
    float m_lastZoomFactor;

    AtomicString m_mediaType;
    AtomicString m_mediaTypeWhenNotPrinting;

    // TODO(skobes): Move this to DocumentLoader::InitialScrollState.
    bool m_wasScrolledByUser;

    bool m_safeToPropagateScrollToParent;

    double m_lastPaintTime;

    bool m_isTrackingPaintInvalidations; // Used for testing.

    // In frame coordinates.
    Vector<IntRect> m_trackedPaintInvalidationRects;

    RefPtrWillBeMember<Node> m_nodeToDraw;
    PaintBehavior m_paintBehavior;
    bool m_isPainting;

    unsigned m_visuallyNonEmptyCharacterCount;
    unsigned m_visuallyNonEmptyPixelCount;
    bool m_isVisuallyNonEmpty;
    bool m_firstVisuallyNonEmptyLayoutCallbackPending;

    RefPtrWillBeMember<Node> m_maintainScrollPositionAnchor;

    // layoutObject to hold our custom scroll corner.
    LayoutScrollbarPart* m_scrollCorner;

    OwnPtrWillBeMember<ScrollableAreaSet> m_scrollableAreas;
    OwnPtrWillBeMember<ScrollableAreaSet> m_animatingScrollableAreas;
    OwnPtr<ResizerAreaSet> m_resizerAreas;
    OwnPtr<ViewportConstrainedObjectSet> m_viewportConstrainedObjects;
    OwnPtrWillBeMember<FrameViewAutoSizeInfo> m_autoSizeInfo;

    IntSize m_inputEventsOffsetForEmulation;
    float m_inputEventsScaleFactorForEmulation;

    IntSize m_layoutSize;
    bool m_layoutSizeFixedToFrameSize;

    Timer<FrameView> m_didScrollTimer;

    Vector<IntRect> m_tickmarks;

    float m_topControlsViewportAdjustment;

    bool m_needsUpdateWidgetPositions;

#if ENABLE(ASSERT)
    // Verified when finalizing.
    bool m_hasBeenDisposed;
#endif

    RefPtrWillBeMember<Scrollbar> m_horizontalScrollbar;
    RefPtrWillBeMember<Scrollbar> m_verticalScrollbar;
    ScrollbarMode m_horizontalScrollbarMode;
    ScrollbarMode m_verticalScrollbarMode;

    bool m_horizontalScrollbarLock;
    bool m_verticalScrollbarLock;

    ChildrenWidgetSet m_children;

    DoubleSize m_pendingScrollDelta;
    DoublePoint m_scrollPosition;
    DoublePoint m_cachedScrollPosition;
    IntSize m_contentsSize;

    int m_scrollbarsAvoidingResizer;
    bool m_scrollbarsSuppressed;

    bool m_inUpdateScrollbars;

    bool m_clipsRepaints;

    OwnPtr<LayoutAnalyzer> m_analyzer;

    // Exists only on root frame.
    // TODO(bokan): crbug.com/484188. We should specialize FrameView for the
    // main frame.
    OwnPtrWillBeMember<ScrollableArea> m_viewportScrollableArea;

public:
    virtual IntSize excludeScrollbars(const IntSize& size) const override;
    int topContentInset() { return m_topContentInset; }
    void setTopContentInset(int offset);
    IntRect contentsToRootFrameAero(const IntRect&) const;
    IntPoint contentsToRootFrameAero(const IntPoint& contentsPoint) const;
protected:
    IntPoint contentsToFrameAero(const IntPoint& pointInContentSpace) const;
private:
    int getRootViewTopContentInset() const;
    int m_topContentInset = 0;
};

inline void FrameView::incrementVisuallyNonEmptyCharacterCount(unsigned count)
{
    if (m_isVisuallyNonEmpty)
        return;
    m_visuallyNonEmptyCharacterCount += count;
    // Use a threshold value to prevent very small amounts of visible content from triggering didFirstVisuallyNonEmptyLayout.
    // The first few hundred characters rarely contain the interesting content of the page.
    static const unsigned visualCharacterThreshold = 200;
    if (m_visuallyNonEmptyCharacterCount > visualCharacterThreshold)
        setIsVisuallyNonEmpty();
}

inline void FrameView::incrementVisuallyNonEmptyPixelCount(const IntSize& size)
{
    if (m_isVisuallyNonEmpty)
        return;
    m_visuallyNonEmptyPixelCount += size.width() * size.height();
    // Use a threshold value to prevent very small amounts of visible content from triggering didFirstVisuallyNonEmptyLayout
    static const unsigned visualPixelThreshold = 32 * 32;
    if (m_visuallyNonEmptyPixelCount > visualPixelThreshold)
        setIsVisuallyNonEmpty();
}

DEFINE_TYPE_CASTS(FrameView, Widget, widget, widget->isFrameView(), widget.isFrameView());

} // namespace blink

#endif // FrameView_h
