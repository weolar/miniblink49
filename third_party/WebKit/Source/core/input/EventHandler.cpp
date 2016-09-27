/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2012 Digia Plc. and/or its subsidiary(-ies)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/input/EventHandler.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/clipboard/DataObject.h"
#include "core/clipboard/DataTransfer.h"
#include "core/dom/Document.h"
#include "core/dom/TouchList.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/Editor.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/SelectionController.h"
#include "core/events/EventPath.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/PointerEvent.h"
#include "core/events/TextEvent.h"
#include "core/events/TouchEvent.h"
#include "core/events/WheelEvent.h"
#include "core/fetch/ImageResource.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLDialogElement.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/HTMLFrameSetElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/layout/HitTestRequest.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutTextControlSingleLine.h"
#include "core/layout/LayoutView.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/page/AutoscrollController.h"
#include "core/page/ChromeClient.h"
#include "core/page/DragController.h"
#include "core/page/DragState.h"
#include "core/page/FocusController.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "core/page/SpatialNavigation.h"
#include "core/page/TouchAdjustment.h"
#include "core/page/scrolling/ScrollState.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/style/ComputedStyle.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "platform/PlatformGestureEvent.h"
#include "platform/PlatformKeyboardEvent.h"
#include "platform/PlatformTouchEvent.h"
#include "platform/PlatformWheelEvent.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/WindowsKeyboardCodes.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/graphics/Image.h"
#include "platform/heap/Handle.h"
#include "platform/scroll/ScrollAnimator.h"
#include "platform/scroll/Scrollbar.h"
#include "wtf/Assertions.h"
#include "wtf/CurrentTime.h"
#include "wtf/StdLibExtras.h"
#include "wtf/TemporaryChange.h"

namespace blink {

using namespace HTMLNames;

// The link drag hysteresis is much larger than the others because there
// needs to be enough space to cancel the link press without starting a link drag,
// and because dragging links is rare.

static const int LinkDragHysteresis = 40;
static const int ImageDragHysteresis = 5;
static const int TextDragHysteresis = 3;
static const int GeneralDragHysteresis = 3;

// The amount of time to wait before sending a fake mouse event triggered
// during a scroll.
static const double fakeMouseMoveInterval = 0.1;

// The amount of time to wait for a cursor update on style and layout changes
// Set to 50Hz, no need to be faster than common screen refresh rate
static const double cursorUpdateInterval = 0.02;

static const int maximumCursorSize = 128;

// It's pretty unlikely that a scale of less than one would ever be used. But all we really
// need to ensure here is that the scale isn't so small that integer overflow can occur when
// dividing cursor sizes (limited above) by the scale.
static const double minimumCursorScale = 0.001;

// The minimum amount of time an element stays active after a ShowPress
// This is roughly 9 frames, which should be long enough to be noticeable.
static const double minimumActiveInterval = 0.15;

#if OS(MACOSX)
static const double TextDragDelay = 0.15;
#else
static const double TextDragDelay = 0.0;
#endif

// Report Overscroll if OverscrollDelta is greater than minimumOverscrollDelta
// to maintain consistency as did in compositor.
static const float minimumOverscrollDelta = 0.1;

enum NoCursorChangeType { NoCursorChange };

enum class DragInitiator { Mouse, Touch };

class OptionalCursor {
public:
    OptionalCursor(NoCursorChangeType) : m_isCursorChange(false) { }
    OptionalCursor(const Cursor& cursor) : m_isCursorChange(true), m_cursor(cursor) { }

    bool isCursorChange() const { return m_isCursorChange; }
    const Cursor& cursor() const { ASSERT(m_isCursorChange); return m_cursor; }

private:
    bool m_isCursorChange;
    Cursor m_cursor;
};

static inline ScrollGranularity wheelGranularityToScrollGranularity(WheelEvent* event)
{
    unsigned deltaMode = event->deltaMode();
    if (deltaMode == WheelEvent::DOM_DELTA_PAGE)
        return ScrollByPage;
    if (deltaMode == WheelEvent::DOM_DELTA_LINE)
        return ScrollByLine;
    ASSERT(deltaMode == WheelEvent::DOM_DELTA_PIXEL);
    return event->hasPreciseScrollingDeltas() ? ScrollByPrecisePixel : ScrollByPixel;
}

// Refetch the event target node if it is removed or currently is the shadow node inside an <input> element.
// If a mouse event handler changes the input element type to one that has a widget associated,
// we'd like to EventHandler::handleMousePressEvent to pass the event to the widget and thus the
// event target node can't still be the shadow node.
static inline bool shouldRefetchEventTarget(const MouseEventWithHitTestResults& mev)
{
    Node* targetNode = mev.innerNode();
    if (!targetNode || !targetNode->parentNode())
        return true;
    return targetNode->isShadowRoot() && isHTMLInputElement(*toShadowRoot(targetNode)->host());
}

void recomputeScrollChain(const LocalFrame& frame, const Node& startNode,
    WillBeHeapDeque<RefPtrWillBeMember<Element>>& scrollChain)
{
    scrollChain.clear();

    ASSERT(startNode.layoutObject());
    LayoutBox* curBox = startNode.layoutObject()->enclosingBox();

    // Scrolling propagates along the containing block chain.
    while (curBox && !curBox->isLayoutView()) {
        Node* curNode = curBox->node();
        // FIXME: this should reject more elements, as part of crbug.com/410974.
        if (curNode && curNode->isElementNode())
            scrollChain.prepend(toElement(curNode));
        curBox = curBox->containingBlock();
    }

    // FIXME: we should exclude the document in some cases, as part
    // of crbug.com/410974.
    scrollChain.prepend(frame.document()->documentElement());
}

EventHandler::EventHandler(LocalFrame* frame)
    : m_frame(frame)
    , m_mousePressed(false)
    , m_capturesDragging(false)
    , m_mouseDownMayStartDrag(false)
    , m_selectionController(SelectionController::create(*frame))
    , m_hoverTimer(this, &EventHandler::hoverTimerFired)
    , m_cursorUpdateTimer(this, &EventHandler::cursorUpdateTimerFired)
    , m_mouseDownMayStartAutoscroll(false)
    , m_fakeMouseMoveEventTimer(this, &EventHandler::fakeMouseMoveEventTimerFired)
    , m_svgPan(false)
    , m_resizeScrollableArea(nullptr)
    , m_eventHandlerWillResetCapturingMouseEventsNode(0)
    , m_clickCount(0)
    , m_shouldOnlyFireDragOverEvent(false)
    , m_accumulatedRootOverscroll(FloatSize())
    , m_mousePositionIsUnknown(true)
    , m_mouseDownTimestamp(0)
    , m_widgetIsLatched(false)
    , m_touchPressed(false)
    , m_inPointerCanceledState(false)
    , m_scrollGestureHandlingNode(nullptr)
    , m_lastGestureScrollOverWidget(false)
    , m_longTapShouldInvokeContextMenu(false)
    , m_activeIntervalTimer(this, &EventHandler::activeIntervalTimerFired)
    , m_lastShowPressTimestamp(0)
    , m_deltaConsumedForScrollSequence(false)
{
}

EventHandler::~EventHandler()
{
    ASSERT(!m_fakeMouseMoveEventTimer.isActive());
}

DEFINE_TRACE(EventHandler)
{
#if ENABLE(OILPAN)
    visitor->trace(m_mousePressNode);
    visitor->trace(m_resizeScrollableArea);
    visitor->trace(m_capturingMouseEventsNode);
    visitor->trace(m_nodeUnderMouse);
    visitor->trace(m_lastNodeUnderMouse);
    visitor->trace(m_lastMouseMoveEventSubframe);
    visitor->trace(m_lastScrollbarUnderMouse);
    visitor->trace(m_clickNode);
    visitor->trace(m_dragTarget);
    visitor->trace(m_frameSetBeingResized);
    visitor->trace(m_latchedWheelEventNode);
    visitor->trace(m_previousWheelScrolledNode);
    visitor->trace(m_scrollbarHandlingScrollGesture);
    visitor->trace(m_targetForTouchID);
    visitor->trace(m_touchSequenceDocument);
    visitor->trace(m_scrollGestureHandlingNode);
    visitor->trace(m_previousGestureScrolledNode);
    visitor->trace(m_lastDeferredTapElement);
    visitor->trace(m_currentScrollChain);
    visitor->trace(m_selectionController);
#endif
}

DragState& EventHandler::dragState()
{
    DEFINE_STATIC_LOCAL(Persistent<DragState>, state, (new DragState()));
    return *state;
}

void EventHandler::clear()
{
    m_hoverTimer.stop();
    m_cursorUpdateTimer.stop();
    m_fakeMouseMoveEventTimer.stop();
    m_activeIntervalTimer.stop();
    m_resizeScrollableArea = nullptr;
    m_nodeUnderMouse = nullptr;
    m_lastNodeUnderMouse = nullptr;
    m_lastMouseMoveEventSubframe = nullptr;
    m_lastScrollbarUnderMouse = nullptr;
    m_clickCount = 0;
    m_clickNode = nullptr;
    m_frameSetBeingResized = nullptr;
    m_dragTarget = nullptr;
    m_shouldOnlyFireDragOverEvent = false;
    m_mousePositionIsUnknown = true;
    m_lastKnownMousePosition = IntPoint();
    m_lastKnownMouseGlobalPosition = IntPoint();
    m_lastMouseDownUserGestureToken.clear();
    m_mousePressNode = nullptr;
    m_mousePressed = false;
    m_capturesDragging = false;
    m_capturingMouseEventsNode = nullptr;
    m_latchedWheelEventNode = nullptr;
    m_previousWheelScrolledNode = nullptr;
    m_targetForTouchID.clear();
    m_touchSequenceDocument.clear();
    m_touchSequenceUserGestureToken.clear();
    m_scrollGestureHandlingNode = nullptr;
    m_lastGestureScrollOverWidget = false;
    m_previousGestureScrolledNode = nullptr;
    m_scrollbarHandlingScrollGesture = nullptr;
    m_touchPressed = false;
    m_pointerIdManager.clear();
    m_inPointerCanceledState = false;
    m_mouseDownMayStartDrag = false;
    m_lastShowPressTimestamp = 0;
    m_lastDeferredTapElement = nullptr;
    m_eventHandlerWillResetCapturingMouseEventsNode = false;
    m_mouseDownMayStartAutoscroll = false;
    m_svgPan = false;
    m_mouseDownPos = IntPoint();
    m_mouseDownTimestamp = 0;
    m_longTapShouldInvokeContextMenu = false;
    m_dragStartPos = LayoutPoint();
    m_offsetFromResizeCorner = LayoutSize();
    m_mouseDown = PlatformMouseEvent();
}

void EventHandler::nodeWillBeRemoved(Node& nodeToBeRemoved)
{
    if (nodeToBeRemoved.containsIncludingShadowDOM(m_clickNode.get())) {
        // We don't dispatch click events if the mousedown node is removed
        // before a mouseup event. It is compatible with IE and Firefox.
        m_clickNode = nullptr;
    }
}

bool EventHandler::handleMousePressEvent(const MouseEventWithHitTestResults& event)
{
    TRACE_EVENT0("blink", "EventHandler::handleMousePressEvent");

    // Reset drag state.
    dragState().m_dragSrc = nullptr;

    cancelFakeMouseMoveEvent();

    m_frame->document()->updateLayoutIgnorePendingStylesheets();

    if (FrameView* frameView = m_frame->view()) {
        if (frameView->isPointInScrollbarCorner(event.event().position()))
            return false;
    }

    bool singleClick = event.event().clickCount() <= 1;

    m_mouseDownMayStartDrag = singleClick;

    selectionController().handleMousePressEvent(event);

    m_mouseDown = event.event();

    if (m_frame->document()->isSVGDocument() && m_frame->document()->accessSVGExtensions().zoomAndPanEnabled()) {
        if (event.event().shiftKey() && singleClick) {
            m_svgPan = true;
            m_frame->document()->accessSVGExtensions().startPan(m_frame->view()->rootFrameToContents(event.event().position()));
            return true;
        }
    }

    // We don't do this at the start of mouse down handling,
    // because we don't want to do it until we know we didn't hit a widget.
    if (singleClick)
        focusDocumentView();

    Node* innerNode = event.innerNode();

    m_mousePressNode = innerNode;
    m_dragStartPos = event.event().position();

    bool swallowEvent = false;
    m_mousePressed = true;
    selectionController().initializeSelectionState();

    if (event.event().clickCount() == 2)
        swallowEvent = selectionController().handleMousePressEventDoubleClick(event);
    else if (event.event().clickCount() >= 3)
        swallowEvent = selectionController().handleMousePressEventTripleClick(event);
    else
        swallowEvent = selectionController().handleMousePressEventSingleClick(event);

    m_mouseDownMayStartAutoscroll = selectionController().mouseDownMayStartSelect()
        || (m_mousePressNode && m_mousePressNode->layoutBox() && m_mousePressNode->layoutBox()->canBeProgramaticallyScrolled());

    return swallowEvent;
}

bool EventHandler::handleMouseDraggedEvent(const MouseEventWithHitTestResults& event)
{
    TRACE_EVENT0("blink", "EventHandler::handleMouseDraggedEvent");

    // While resetting m_mousePressed here may seem out of place, it turns out
    // to be needed to handle some bugs^Wfeatures in Blink mouse event handling:
    // 1. Certain elements, such as <embed>, capture mouse events. They do not
    //    bubble back up. One way for a <embed> to start capturing mouse events
    //    is on a mouse press. The problem is the <embed> node only starts
    //    capturing mouse events *after* m_mousePressed for the containing frame
    //    has already been set to true. As a result, the frame's EventHandler
    //    never sees the mouse release event, which is supposed to reset
    //    m_mousePressed... so m_mousePressed ends up remaining true until the
    //    event handler finally gets another mouse released event. Oops.
    // 2. Dragging doesn't start until after a mouse press event, but a drag
    //    that ends as a result of a mouse release does not send a mouse release
    //    event. As a result, m_mousePressed also ends up remaining true until
    //    the next mouse release event seen by the EventHandler.
    if (event.event().button() != LeftButton)
        m_mousePressed = false;

    if (!m_mousePressed)
        return false;

    if (handleDrag(event, DragInitiator::Mouse))
        return true;

    Node* targetNode = event.innerNode();
    if (!targetNode)
        return false;

    LayoutObject* layoutObject = targetNode->layoutObject();
    if (!layoutObject) {
        Node* parent = ComposedTreeTraversal::parent(*targetNode);
        if (!parent)
            return false;

        layoutObject = parent->layoutObject();
        if (!layoutObject || !layoutObject->isListBox())
            return false;
    }

    m_mouseDownMayStartDrag = false;

    if (m_mouseDownMayStartAutoscroll && !panScrollInProgress()) {
        if (AutoscrollController* controller = autoscrollController()) {
            controller->startAutoscrollForSelection(layoutObject);
            m_mouseDownMayStartAutoscroll = false;
        }
    }

    selectionController().handleMouseDraggedEvent(event, m_mouseDownPos, m_dragStartPos, m_mousePressNode.get(), m_lastKnownMousePosition);
    return true;
}

void EventHandler::updateSelectionForMouseDrag()
{
    selectionController().updateSelectionForMouseDrag(m_mousePressNode.get(), m_dragStartPos, m_lastKnownMousePosition);
}

bool EventHandler::handleMouseReleaseEvent(const MouseEventWithHitTestResults& event)
{
    AutoscrollController* controller = autoscrollController();
    if (controller && controller->autoscrollInProgress())
        stopAutoscroll();

    // Used to prevent mouseMoveEvent from initiating a drag before
    // the mouse is pressed again.
    m_mousePressed = false;
    m_capturesDragging = false;
    m_mouseDownMayStartDrag = false;
    m_mouseDownMayStartAutoscroll = false;

    return selectionController().handleMouseReleaseEvent(event, m_dragStartPos);
}

#if OS(WIN)

void EventHandler::startPanScrolling(LayoutObject* layoutObject)
{
    if (!layoutObject->isBox())
        return;
    AutoscrollController* controller = autoscrollController();
    if (!controller)
        return;
    controller->startPanScrolling(toLayoutBox(layoutObject), lastKnownMousePosition());
    invalidateClick();
}

#endif // OS(WIN)

AutoscrollController* EventHandler::autoscrollController() const
{
    if (Page* page = m_frame->page())
        return &page->autoscrollController();
    return nullptr;
}

bool EventHandler::panScrollInProgress() const
{
    return autoscrollController() && autoscrollController()->panScrollInProgress();
}

HitTestResult EventHandler::hitTestResultAtPoint(const LayoutPoint& point, HitTestRequest::HitTestRequestType hitType, const LayoutSize& padding)
{
    TRACE_EVENT0("blink", "EventHandler::hitTestResultAtPoint");

    ASSERT((hitType & HitTestRequest::ListBased) || padding.isEmpty());

    // We always send hitTestResultAtPoint to the main frame if we have one,
    // otherwise we might hit areas that are obscured by higher frames.
    if (m_frame->page()) {
        LocalFrame* mainFrame = m_frame->localFrameRoot();
        if (mainFrame && m_frame != mainFrame) {
            FrameView* frameView = m_frame->view();
            FrameView* mainView = mainFrame->view();
            if (frameView && mainView) {
                IntPoint mainFramePoint = mainView->rootFrameToContents(frameView->contentsToRootFrame(roundedIntPoint(point)));
                return mainFrame->eventHandler().hitTestResultAtPoint(mainFramePoint, hitType, padding);
            }
        }
    }

    // hitTestResultAtPoint is specifically used to hitTest into all frames, thus it always allows child frame content.
    HitTestRequest request(hitType | HitTestRequest::AllowChildFrameContent);
    HitTestResult result(request, point, padding.height(), padding.width(), padding.height(), padding.width());

    // LayoutView::hitTest causes a layout, and we don't want to hit that until the first
    // layout because until then, there is nothing shown on the screen - the user can't
    // have intentionally clicked on something belonging to this page. Furthermore,
    // mousemove events before the first layout should not lead to a premature layout()
    // happening, which could show a flash of white.
    // See also the similar code in Document::prepareMouseEvent.
    if (!m_frame->contentLayoutObject() || !m_frame->view() || !m_frame->view()->didFirstLayout())
        return result;

    m_frame->contentLayoutObject()->hitTest(result);
    if (!request.readOnly())
        m_frame->document()->updateHoverActiveState(request, result.innerElement());

    return result;
}

void EventHandler::stopAutoscroll()
{
    if (AutoscrollController* controller = autoscrollController())
        controller->stopAutoscroll();
}

ScrollResultOneDimensional EventHandler::scroll(ScrollDirection direction, ScrollGranularity granularity, Node* startNode, Node** stopNode, float delta, IntPoint absolutePoint)
{
    if (!delta)
        return ScrollResultOneDimensional(false);

    Node* node = startNode;

    if (!node)
        node = m_frame->document()->focusedElement();

    if (!node)
        node = m_mousePressNode.get();

    if (!node || !node->layoutObject())
        return ScrollResultOneDimensional(false, delta);

    m_frame->document()->updateLayoutIgnorePendingStylesheets();

    LayoutBox* curBox = node->layoutObject()->enclosingBox();
    while (curBox && !curBox->isLayoutView()) {
        ScrollDirectionPhysical physicalDirection = toPhysicalDirection(
            direction, curBox->isHorizontalWritingMode(), curBox->style()->isFlippedBlocksWritingMode());

        // If we're at the stopNode, we should try to scroll it but we shouldn't bubble past it
        bool shouldStopBubbling = stopNode && *stopNode && curBox->node() == *stopNode;
        ScrollResultOneDimensional result = curBox->scroll(physicalDirection, granularity, delta);

        if (result.didScroll && stopNode)
            *stopNode = curBox->node();

        if (result.didScroll || shouldStopBubbling) {
            setFrameWasScrolledByUser();
            result.didScroll = true;
            return result;
        }

        curBox = curBox->containingBlock();
    }

    return ScrollResultOneDimensional(false, delta);
}

void EventHandler::customizedScroll(const Node& startNode, ScrollState& scrollState)
{
    if (scrollState.fullyConsumed())
        return;

    if (scrollState.deltaX() || scrollState.deltaY())
        m_frame->document()->updateLayoutIgnorePendingStylesheets();

    if (m_currentScrollChain.isEmpty())
        recomputeScrollChain(*m_frame, startNode, m_currentScrollChain);
    scrollState.setScrollChain(m_currentScrollChain);
    scrollState.distributeToScrollChainDescendant();
}

bool EventHandler::bubblingScroll(ScrollDirection direction, ScrollGranularity granularity, Node* startingNode)
{
    // The layout needs to be up to date to determine if we can scroll. We may be
    // here because of an onLoad event, in which case the final layout hasn't been performed yet.
    m_frame->document()->updateLayoutIgnorePendingStylesheets();
    // FIXME: enable scroll customization in this case. See crbug.com/410974.
    if (scroll(direction, granularity, startingNode).didScroll)
        return true;
    LocalFrame* frame = m_frame;
    FrameView* view = frame->view();
    if (view) {
        ScrollDirectionPhysical physicalDirection =
            toPhysicalDirection(direction, view->isVerticalDocument(), view->isFlippedDocument());
        if (view->scrollableArea()->userScroll(physicalDirection, granularity).didScroll) {
            setFrameWasScrolledByUser();
            return true;
        }
    }

    Frame* parentFrame = frame->tree().parent();
    if (!parentFrame || !parentFrame->isLocalFrame())
        return false;
    // FIXME: Broken for OOPI.
    return toLocalFrame(parentFrame)->eventHandler().bubblingScroll(direction, granularity, m_frame->deprecatedLocalOwner());
}

IntPoint EventHandler::lastKnownMousePosition() const
{
    return m_lastKnownMousePosition;
}

static LocalFrame* subframeForTargetNode(Node* node)
{
    if (!node)
        return nullptr;

    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject || !layoutObject->isLayoutPart())
        return nullptr;

    Widget* widget = toLayoutPart(layoutObject)->widget();
    if (!widget || !widget->isFrameView())
        return nullptr;

    return &toFrameView(widget)->frame();
}

static LocalFrame* subframeForHitTestResult(const MouseEventWithHitTestResults& hitTestResult)
{
    if (!hitTestResult.isOverWidget())
        return nullptr;
    return subframeForTargetNode(hitTestResult.innerNode());
}

static bool isSubmitImage(Node* node)
{
    return isHTMLInputElement(node) && toHTMLInputElement(node)->type() == InputTypeNames::image;
}

bool EventHandler::useHandCursor(Node* node, bool isOverLink)
{
    if (!node)
        return false;

    return ((isOverLink || isSubmitImage(node)) && !node->hasEditableStyle());
}

void EventHandler::cursorUpdateTimerFired(Timer<EventHandler>*)
{
    ASSERT(m_frame);
    ASSERT(m_frame->document());

    updateCursor();
}

void EventHandler::updateCursor()
{
    TRACE_EVENT0("input", "EventHandler::updateCursor");

    // We must do a cross-frame hit test because the frame that triggered the cursor
    // update could be occluded by a different frame.
    ASSERT(m_frame == m_frame->localFrameRoot());

    if (m_mousePositionIsUnknown)
        return;

    FrameView* view = m_frame->view();
    if (!view || !view->shouldSetCursor())
        return;

    LayoutView* layoutView = view->layoutView();
    if (!layoutView)
        return;

    m_frame->document()->updateLayout();

    HitTestRequest request(HitTestRequest::ReadOnly | HitTestRequest::AllowChildFrameContent);
    HitTestResult result(request, view->rootFrameToContents(m_lastKnownMousePosition));
    layoutView->hitTest(result);

    if (LocalFrame* frame = result.innerNodeFrame()) {
        OptionalCursor optionalCursor = frame->eventHandler().selectCursor(result);
        if (optionalCursor.isCursorChange()) {
            view->setCursor(optionalCursor.cursor());
        }
    }
}

OptionalCursor EventHandler::selectCursor(const HitTestResult& result)
{
    if (m_resizeScrollableArea && m_resizeScrollableArea->inResizeMode())
        return NoCursorChange;

    Page* page = m_frame->page();
    if (!page)
        return NoCursorChange;
    if (panScrollInProgress())
        return NoCursorChange;

    Node* node = result.innerPossiblyPseudoNode();
    if (!node)
        return selectAutoCursor(result, node, iBeamCursor());

    LayoutObject* layoutObject = node->layoutObject();
    const ComputedStyle* style = layoutObject ? layoutObject->style() : nullptr;

    if (layoutObject) {
        Cursor overrideCursor;
        switch (layoutObject->getCursor(roundedIntPoint(result.localPoint()), overrideCursor)) {
        case SetCursorBasedOnStyle:
            break;
        case SetCursor:
            return overrideCursor;
        case DoNotSetCursor:
            return NoCursorChange;
        }
    }

    if (style && style->cursors()) {
        const CursorList* cursors = style->cursors();
        for (unsigned i = 0; i < cursors->size(); ++i) {
            StyleImage* styleImage = (*cursors)[i].image();
            if (!styleImage)
                continue;
            ImageResource* cachedImage = styleImage->cachedImage();
            if (!cachedImage)
                continue;
            float scale = styleImage->imageScaleFactor();
            bool hotSpotSpecified = (*cursors)[i].hotSpotSpecified();
            // Get hotspot and convert from logical pixels to physical pixels.
            IntPoint hotSpot = (*cursors)[i].hotSpot();
            hotSpot.scale(scale, scale);
            IntSize size = cachedImage->imageForLayoutObject(layoutObject)->size();
            if (cachedImage->errorOccurred())
                continue;
            // Limit the size of cursors (in UI pixels) so that they cannot be
            // used to cover UI elements in chrome.
            size.scale(1 / scale);
            if (size.width() > maximumCursorSize || size.height() > maximumCursorSize)
                continue;

            Image* image = cachedImage->imageForLayoutObject(layoutObject);
            // Ensure no overflow possible in calculations above.
            if (scale < minimumCursorScale)
                continue;
            return Cursor(image, hotSpotSpecified, hotSpot, scale);
        }
    }

    switch (style ? style->cursor() : CURSOR_AUTO) {
    case CURSOR_AUTO: {
        bool horizontalText = !style || style->isHorizontalWritingMode();
        const Cursor& iBeam = horizontalText ? iBeamCursor() : verticalTextCursor();
        return selectAutoCursor(result, node, iBeam);
    }
    case CURSOR_CROSS:
        return crossCursor();
    case CURSOR_POINTER:
        return handCursor();
    case CURSOR_MOVE:
        return moveCursor();
    case CURSOR_ALL_SCROLL:
        return moveCursor();
    case CURSOR_E_RESIZE:
        return eastResizeCursor();
    case CURSOR_W_RESIZE:
        return westResizeCursor();
    case CURSOR_N_RESIZE:
        return northResizeCursor();
    case CURSOR_S_RESIZE:
        return southResizeCursor();
    case CURSOR_NE_RESIZE:
        return northEastResizeCursor();
    case CURSOR_SW_RESIZE:
        return southWestResizeCursor();
    case CURSOR_NW_RESIZE:
        return northWestResizeCursor();
    case CURSOR_SE_RESIZE:
        return southEastResizeCursor();
    case CURSOR_NS_RESIZE:
        return northSouthResizeCursor();
    case CURSOR_EW_RESIZE:
        return eastWestResizeCursor();
    case CURSOR_NESW_RESIZE:
        return northEastSouthWestResizeCursor();
    case CURSOR_NWSE_RESIZE:
        return northWestSouthEastResizeCursor();
    case CURSOR_COL_RESIZE:
        return columnResizeCursor();
    case CURSOR_ROW_RESIZE:
        return rowResizeCursor();
    case CURSOR_TEXT:
        return iBeamCursor();
    case CURSOR_WAIT:
        return waitCursor();
    case CURSOR_HELP:
        return helpCursor();
    case CURSOR_VERTICAL_TEXT:
        return verticalTextCursor();
    case CURSOR_CELL:
        return cellCursor();
    case CURSOR_CONTEXT_MENU:
        return contextMenuCursor();
    case CURSOR_PROGRESS:
        return progressCursor();
    case CURSOR_NO_DROP:
        return noDropCursor();
    case CURSOR_ALIAS:
        return aliasCursor();
    case CURSOR_COPY:
        return copyCursor();
    case CURSOR_NONE:
        return noneCursor();
    case CURSOR_NOT_ALLOWED:
        return notAllowedCursor();
    case CURSOR_DEFAULT:
        return pointerCursor();
    case CURSOR_ZOOM_IN:
        return zoomInCursor();
    case CURSOR_ZOOM_OUT:
        return zoomOutCursor();
    case CURSOR_WEBKIT_GRAB:
        return grabCursor();
    case CURSOR_WEBKIT_GRABBING:
        return grabbingCursor();
    }
    return pointerCursor();
}

OptionalCursor EventHandler::selectAutoCursor(const HitTestResult& result, Node* node, const Cursor& iBeam)
{
    bool editable = (node && node->hasEditableStyle());

    if (useHandCursor(node, result.isOverLink()))
        return handCursor();

    bool inResizer = false;
    LayoutObject* layoutObject = node ? node->layoutObject() : nullptr;
    if (layoutObject && m_frame->view()) {
        DeprecatedPaintLayer* layer = layoutObject->enclosingLayer();
        inResizer = layer->scrollableArea() && layer->scrollableArea()->isPointInResizeControl(result.roundedPointInMainFrame(), ResizerForPointer);
    }

    // During selection, use an I-beam no matter what we're over.
    // If a drag may be starting or we're capturing mouse events for a particular node, don't treat this as a selection.
    if (m_mousePressed && selectionController().mouseDownMayStartSelect()
        && !m_mouseDownMayStartDrag
        && m_frame->selection().isCaretOrRange()
        && !m_capturingMouseEventsNode) {
        return iBeam;
    }

    if ((editable || (layoutObject && layoutObject->isText() && node->canStartSelection())) && !inResizer && !result.scrollbar())
        return iBeam;
    return pointerCursor();
}

static LayoutPoint contentPointFromRootFrame(LocalFrame* frame, const IntPoint& pointInRootFrame)
{
    FrameView* view = frame->view();
    // FIXME: Is it really OK to use the wrong coordinates here when view is 0?
    // Historically the code would just crash; this is clearly no worse than that.
    return view ? view->rootFrameToContents(pointInRootFrame) : pointInRootFrame;
}

bool EventHandler::handleMousePressEvent(const PlatformMouseEvent& mouseEvent)
{
    TRACE_EVENT0("blink", "EventHandler::handleMousePressEvent");

    RefPtrWillBeRawPtr<FrameView> protector(m_frame->view());

    UserGestureIndicator gestureIndicator(DefinitelyProcessingUserGesture);
    m_frame->localFrameRoot()->eventHandler().m_lastMouseDownUserGestureToken = gestureIndicator.currentToken();

    cancelFakeMouseMoveEvent();
    if (m_eventHandlerWillResetCapturingMouseEventsNode)
        m_capturingMouseEventsNode = nullptr;
    m_mousePressed = true;
    m_capturesDragging = true;
    setLastKnownMousePosition(mouseEvent);
    m_mouseDownTimestamp = mouseEvent.timestamp();
    m_mouseDownMayStartDrag = false;
    selectionController().setMouseDownMayStartSelect(false);
    m_mouseDownMayStartAutoscroll = false;
    if (FrameView* view = m_frame->view()) {
        m_mouseDownPos = view->rootFrameToContents(mouseEvent.position());
    } else {
        invalidateClick();
        return false;
    }

    HitTestRequest request(HitTestRequest::Active);
    // Save the document point we generate in case the window coordinate is invalidated by what happens
    // when we dispatch the event.
    LayoutPoint documentPoint = contentPointFromRootFrame(m_frame, mouseEvent.position());
    MouseEventWithHitTestResults mev = m_frame->document()->prepareMouseEvent(request, documentPoint, mouseEvent);

    if (!mev.innerNode()) {
        invalidateClick();
        return false;
    }

    m_mousePressNode = mev.innerNode();

    RefPtrWillBeRawPtr<LocalFrame> subframe = subframeForHitTestResult(mev);
    if (subframe && passMousePressEventToSubframe(mev, subframe.get())) {
        // Start capturing future events for this frame.  We only do this if we didn't clear
        // the m_mousePressed flag, which may happen if an AppKit widget entered a modal event loop.
        m_capturesDragging = subframe->eventHandler().capturesDragging();
        if (m_mousePressed && m_capturesDragging) {
            m_capturingMouseEventsNode = mev.innerNode();
            m_eventHandlerWillResetCapturingMouseEventsNode = true;
        }
        invalidateClick();
        return true;
    }

#if OS(WIN)
    // We store whether pan scrolling is in progress before calling stopAutoscroll()
    // because it will set m_autoscrollType to NoAutoscroll on return.
    bool isPanScrollInProgress = panScrollInProgress();
    stopAutoscroll();
    if (isPanScrollInProgress) {
        // We invalidate the click when exiting pan scrolling so that we don't inadvertently navigate
        // away from the current page (e.g. the click was on a hyperlink). See <rdar://problem/6095023>.
        invalidateClick();
        return true;
    }
#endif

    m_clickCount = mouseEvent.clickCount();
    m_clickNode = mev.innerNode()->isTextNode() ?  ComposedTreeTraversal::parent(*mev.innerNode()) : mev.innerNode();

    if (FrameView* view = m_frame->view()) {
        DeprecatedPaintLayer* layer = mev.innerNode()->layoutObject() ? mev.innerNode()->layoutObject()->enclosingLayer() : nullptr;
        IntPoint p = view->rootFrameToContents(mouseEvent.position());
        if (layer && layer->scrollableArea() && layer->scrollableArea()->isPointInResizeControl(p, ResizerForPointer)) {
            m_resizeScrollableArea = layer->scrollableArea();
            m_resizeScrollableArea->setInResizeMode(true);
            m_offsetFromResizeCorner = LayoutSize(m_resizeScrollableArea->offsetFromResizeCorner(p));
            invalidateClick();
            return true;
        }
    }

    m_frame->selection().setCaretBlinkingSuspended(true);

    bool swallowEvent = !dispatchMouseEvent(EventTypeNames::mousedown, mev.innerNode(), m_clickCount, mouseEvent, true);
    HitTestResult hitTestResult = hitTestResultInFrame(m_frame, mouseEvent.position(), HitTestRequest::ReadOnly);
    swallowEvent = swallowEvent || handleMouseFocus(MouseEventWithHitTestResults(mouseEvent, hitTestResult));
    m_capturesDragging = !swallowEvent || mev.scrollbar();

    // If the hit testing originally determined the event was in a scrollbar, refetch the MouseEventWithHitTestResults
    // in case the scrollbar widget was destroyed when the mouse event was handled.
    if (mev.scrollbar()) {
        const bool wasLastScrollBar = mev.scrollbar() == m_lastScrollbarUnderMouse.get();
        HitTestRequest request(HitTestRequest::ReadOnly | HitTestRequest::Active);
        mev = m_frame->document()->prepareMouseEvent(request, documentPoint, mouseEvent);
        if (wasLastScrollBar && mev.scrollbar() != m_lastScrollbarUnderMouse.get())
            m_lastScrollbarUnderMouse = nullptr;
    }

    if (swallowEvent) {
        // scrollbars should get events anyway, even disabled controls might be scrollable
        passMousePressEventToScrollbar(mev);
    } else {
        if (shouldRefetchEventTarget(mev)) {
            HitTestRequest request(HitTestRequest::ReadOnly | HitTestRequest::Active);
            mev = m_frame->document()->prepareMouseEvent(request, documentPoint, mouseEvent);
        }

        if (passMousePressEventToScrollbar(mev))
            swallowEvent = true;
        else
            swallowEvent = handleMousePressEvent(mev);
    }

    if (mev.hitTestResult().innerNode() && mouseEvent.button() == LeftButton) {
        ASSERT(mouseEvent.type() == PlatformEvent::MousePressed);
        HitTestResult result = mev.hitTestResult();
        result.setToShadowHostIfInUserAgentShadowRoot();
        m_frame->chromeClient().onMouseDown(result.innerNode());
    }

    return swallowEvent;
}

static DeprecatedPaintLayer* layerForNode(Node* node)
{
    if (!node)
        return nullptr;

    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject)
        return nullptr;

    DeprecatedPaintLayer* layer = layoutObject->enclosingLayer();
    if (!layer)
        return nullptr;

    return layer;
}

ScrollableArea* EventHandler::associatedScrollableArea(const DeprecatedPaintLayer* layer) const
{
    if (DeprecatedPaintLayerScrollableArea* scrollableArea = layer->scrollableArea()) {
        if (scrollableArea->scrollsOverflow())
            return scrollableArea;
    }

    return nullptr;
}

bool EventHandler::handleMouseMoveEvent(const PlatformMouseEvent& event)
{
    TRACE_EVENT0("blink", "EventHandler::handleMouseMoveEvent");

    RefPtrWillBeRawPtr<FrameView> protector(m_frame->view());

    HitTestResult hoveredNode = HitTestResult();
    bool result = handleMouseMoveOrLeaveEvent(event, &hoveredNode);

    Page* page = m_frame->page();
    if (!page)
        return result;

    if (DeprecatedPaintLayer* layer = layerForNode(hoveredNode.innerNode())) {
        if (ScrollableArea* layerScrollableArea = associatedScrollableArea(layer))
            layerScrollableArea->mouseMovedInContentArea();
    }

    if (FrameView* frameView = m_frame->view())
        frameView->mouseMovedInContentArea();

    hoveredNode.setToShadowHostIfInUserAgentShadowRoot();
    page->chromeClient().mouseDidMoveOverElement(hoveredNode);

    return result;
}

void EventHandler::handleMouseLeaveEvent(const PlatformMouseEvent& event)
{
    TRACE_EVENT0("blink", "EventHandler::handleMouseLeaveEvent");

    RefPtrWillBeRawPtr<FrameView> protector(m_frame->view());
    handleMouseMoveOrLeaveEvent(event, 0, false, true);
}

bool EventHandler::handleMouseMoveOrLeaveEvent(const PlatformMouseEvent& mouseEvent, HitTestResult* hoveredNode, bool onlyUpdateScrollbars, bool forceLeave)
{
    ASSERT(m_frame);
    ASSERT(m_frame->view());

    setLastKnownMousePosition(mouseEvent);

    if (m_hoverTimer.isActive())
        m_hoverTimer.stop();

    m_cursorUpdateTimer.stop();

    cancelFakeMouseMoveEvent();

    if (m_svgPan) {
        m_frame->document()->accessSVGExtensions().updatePan(m_frame->view()->rootFrameToContents(m_lastKnownMousePosition));
        return true;
    }

    if (m_frameSetBeingResized)
        return !dispatchMouseEvent(EventTypeNames::mousemove, m_frameSetBeingResized.get(), 0, mouseEvent, false);

    // Send events right to a scrollbar if the mouse is pressed.
    if (m_lastScrollbarUnderMouse && m_mousePressed) {
        m_lastScrollbarUnderMouse->mouseMoved(mouseEvent);
        return true;
    }

    // Mouse events simulated from touch should not hit-test again.
    ASSERT(!mouseEvent.fromTouch());

    HitTestRequest::HitTestRequestType hitType = HitTestRequest::Move;
    if (m_mousePressed) {
        hitType |= HitTestRequest::Active;
    } else if (onlyUpdateScrollbars) {
        // Mouse events should be treated as "read-only" if we're updating only scrollbars. This
        // means that :hover and :active freeze in the state they were in, rather than updating
        // for nodes the mouse moves while the window is not key (which will be the case if
        // onlyUpdateScrollbars is true).
        hitType |= HitTestRequest::ReadOnly;
    }

    // Treat any mouse move events as readonly if the user is currently touching the screen.
    if (m_touchPressed)
        hitType |= HitTestRequest::Active | HitTestRequest::ReadOnly;
    HitTestRequest request(hitType);
    MouseEventWithHitTestResults mev = MouseEventWithHitTestResults(mouseEvent, HitTestResult(request, LayoutPoint()));

    // We don't want to do a hit-test in forceLeave scenarios because there might actually be some other frame above this one at the specified co-ordinate.
    // So we must force the hit-test to fail, while still clearing hover/active state.
    if (forceLeave)
        m_frame->document()->updateHoverActiveState(request, 0);
    else
        mev = prepareMouseEvent(request, mouseEvent);

    if (hoveredNode)
        *hoveredNode = mev.hitTestResult();

    Scrollbar* scrollbar = nullptr;

    if (m_resizeScrollableArea && m_resizeScrollableArea->inResizeMode()) {
        m_resizeScrollableArea->resize(mouseEvent, m_offsetFromResizeCorner);
    } else {
        if (!scrollbar)
            scrollbar = mev.scrollbar();

        updateLastScrollbarUnderMouse(scrollbar, !m_mousePressed);
        if (onlyUpdateScrollbars)
            return true;
    }

    bool swallowEvent = false;
    RefPtrWillBeRawPtr<LocalFrame> newSubframe = m_capturingMouseEventsNode.get() ? subframeForTargetNode(m_capturingMouseEventsNode.get()) : subframeForHitTestResult(mev);

    // We want mouseouts to happen first, from the inside out.  First send a move event to the last subframe so that it will fire mouseouts.
    if (m_lastMouseMoveEventSubframe && m_lastMouseMoveEventSubframe->tree().isDescendantOf(m_frame) && m_lastMouseMoveEventSubframe != newSubframe)
        m_lastMouseMoveEventSubframe->eventHandler().handleMouseLeaveEvent(mouseEvent);

    if (newSubframe) {
        // Update over/out state before passing the event to the subframe.
        updateMouseEventTargetNode(mev.innerNode(), mouseEvent, true);

        // Event dispatch in updateMouseEventTargetNode may have caused the subframe of the target
        // node to be detached from its FrameView, in which case the event should not be passed.
        if (newSubframe->view())
            swallowEvent |= passMouseMoveEventToSubframe(mev, newSubframe.get(), hoveredNode);
    } else {
        if (scrollbar && !m_mousePressed)
            scrollbar->mouseMoved(mouseEvent); // Handle hover effects on platforms that support visual feedback on scrollbar hovering.
        if (FrameView* view = m_frame->view()) {
            OptionalCursor optionalCursor = selectCursor(mev.hitTestResult());
            if (optionalCursor.isCursorChange()) {
                view->setCursor(optionalCursor.cursor());
            }
        }
    }

    m_lastMouseMoveEventSubframe = newSubframe;

    if (swallowEvent)
        return true;

    swallowEvent = !dispatchMouseEvent(EventTypeNames::mousemove, mev.innerNode(), 0, mouseEvent, true);
    if (!swallowEvent)
        swallowEvent = handleMouseDraggedEvent(mev);

    return swallowEvent;
}

void EventHandler::invalidateClick()
{
    m_clickCount = 0;
    m_clickNode = nullptr;
}

static ContainerNode* parentForClickEvent(const Node& node)
{
    // IE doesn't dispatch click events for mousedown/mouseup events across form
    // controls.
    if (node.isHTMLElement() && toHTMLElement(node).isInteractiveContent())
        return nullptr;

    return ComposedTreeTraversal::parent(node);
}

bool EventHandler::handleMouseReleaseEvent(const PlatformMouseEvent& mouseEvent)
{
    TRACE_EVENT0("blink", "EventHandler::handleMouseReleaseEvent");

    RefPtrWillBeRawPtr<FrameView> protector(m_frame->view());

    m_frame->selection().setCaretBlinkingSuspended(false);

    OwnPtr<UserGestureIndicator> gestureIndicator;

    if (m_frame->localFrameRoot()->eventHandler().m_lastMouseDownUserGestureToken)
        gestureIndicator = adoptPtr(new UserGestureIndicator(m_frame->localFrameRoot()->eventHandler().m_lastMouseDownUserGestureToken.release()));
    else
        gestureIndicator = adoptPtr(new UserGestureIndicator(DefinitelyProcessingUserGesture));

#if OS(WIN)
    if (Page* page = m_frame->page())
        page->autoscrollController().handleMouseReleaseForPanScrolling(m_frame, mouseEvent);
#endif

    m_mousePressed = false;
    setLastKnownMousePosition(mouseEvent);

    if (m_svgPan) {
        m_svgPan = false;
        m_frame->document()->accessSVGExtensions().updatePan(m_frame->view()->rootFrameToContents(m_lastKnownMousePosition));
        return true;
    }

    if (m_frameSetBeingResized)
        return !dispatchMouseEvent(EventTypeNames::mouseup, m_frameSetBeingResized.get(), m_clickCount, mouseEvent, false);

    if (m_lastScrollbarUnderMouse) {
        invalidateClick();
        m_lastScrollbarUnderMouse->mouseUp(mouseEvent);
        bool setUnder = false;
        return !dispatchMouseEvent(EventTypeNames::mouseup, m_nodeUnderMouse.get(), m_clickCount, mouseEvent, setUnder);
    }

    // Mouse events simulated from touch should not hit-test again.
    ASSERT(!mouseEvent.fromTouch());

    HitTestRequest::HitTestRequestType hitType = HitTestRequest::Release;
    HitTestRequest request(hitType);
    MouseEventWithHitTestResults mev = prepareMouseEvent(request, mouseEvent);
    LocalFrame* subframe = m_capturingMouseEventsNode.get() ? subframeForTargetNode(m_capturingMouseEventsNode.get()) : subframeForHitTestResult(mev);
    if (m_eventHandlerWillResetCapturingMouseEventsNode)
        m_capturingMouseEventsNode = nullptr;
    if (subframe && passMouseReleaseEventToSubframe(mev, subframe))
        return true;

    bool swallowMouseUpEvent = !dispatchMouseEvent(EventTypeNames::mouseup, mev.innerNode(), m_clickCount, mouseEvent, false);

    bool contextMenuEvent = mouseEvent.button() == RightButton;
#if OS(MACOSX)
    // FIXME: The Mac port achieves the same behavior by checking whether the context menu is currently open in WebPage::mouseEvent(). Consider merging the implementations.
    if (mouseEvent.button() == LeftButton && mouseEvent.modifiers() & PlatformEvent::CtrlKey)
        contextMenuEvent = true;
#endif

    bool swallowClickEvent = false;
    if (m_clickCount > 0 && !contextMenuEvent && mev.innerNode() && m_clickNode && mev.innerNode()->canParticipateInComposedTree() && m_clickNode->canParticipateInComposedTree()) {
        // Updates distribution because a 'mouseup' event listener can make the
        // tree dirty at dispatchMouseEvent() invocation above.
        // Unless distribution is updated, commonAncestor would hit ASSERT.
        // Both m_clickNode and mev.innerNode() don't need to be updated
        // because commonAncestor() will exit early if their documents are different.
        m_clickNode->updateDistribution();
        if (Node* clickTargetNode = mev.innerNode()->commonAncestor(*m_clickNode, parentForClickEvent))
            swallowClickEvent = !dispatchMouseEvent(EventTypeNames::click, clickTargetNode, m_clickCount, mouseEvent, true);
    }

    if (m_resizeScrollableArea) {
        m_resizeScrollableArea->setInResizeMode(false);
        m_resizeScrollableArea = nullptr;
    }

    bool swallowMouseReleaseEvent = false;
    if (!swallowMouseUpEvent)
        swallowMouseReleaseEvent = handleMouseReleaseEvent(mev);

    invalidateClick();

    return swallowMouseUpEvent || swallowClickEvent || swallowMouseReleaseEvent;
}

bool EventHandler::dispatchDragEvent(const AtomicString& eventType, Node* dragTarget, const PlatformMouseEvent& event, DataTransfer* dataTransfer)
{
    FrameView* view = m_frame->view();

    // FIXME: We might want to dispatch a dragleave even if the view is gone.
    if (!view)
        return false;

    RefPtrWillBeRawPtr<MouseEvent> me = MouseEvent::create(eventType,
        true, true, m_frame->document()->domWindow(),
        0, event.globalPosition().x(), event.globalPosition().y(), event.position().x(), event.position().y(),
        event.movementDelta().x(), event.movementDelta().y(),
        event.ctrlKey(), event.altKey(), event.shiftKey(), event.metaKey(),
        0, MouseEvent::platformModifiersToButtons(event.modifiers()), nullptr, dataTransfer, false, event.syntheticEventType(), event.timestamp());

    dragTarget->dispatchEvent(me.get(), IGNORE_EXCEPTION);
    return me->defaultPrevented();
}

static bool targetIsFrame(Node* target, LocalFrame*& frame)
{
    if (!isHTMLFrameElementBase(target))
        return false;

    // Cross-process drag and drop is not yet supported.
    if (toHTMLFrameElementBase(target)->contentFrame() && !toHTMLFrameElementBase(target)->contentFrame()->isLocalFrame())
        return false;

    frame = toLocalFrame(toHTMLFrameElementBase(target)->contentFrame());
    return true;
}

static bool findDropZone(Node* target, DataTransfer* dataTransfer)
{
    Element* element = target->isElementNode() ? toElement(target) : target->parentElement();
    for (; element; element = element->parentElement()) {
        bool matched = false;
        AtomicString dropZoneStr = element->fastGetAttribute(webkitdropzoneAttr);

        if (dropZoneStr.isEmpty())
            continue;

        UseCounter::count(element->document(), UseCounter::PrefixedHTMLElementDropzone);

        dropZoneStr = dropZoneStr.lower();

        SpaceSplitString keywords(dropZoneStr, SpaceSplitString::ShouldNotFoldCase);
        if (keywords.isNull())
            continue;

        DragOperation dragOperation = DragOperationNone;
        for (unsigned i = 0; i < keywords.size(); i++) {
            DragOperation op = convertDropZoneOperationToDragOperation(keywords[i]);
            if (op != DragOperationNone) {
                if (dragOperation == DragOperationNone)
                    dragOperation = op;
            } else {
                matched = matched || dataTransfer->hasDropZoneType(keywords[i].string());
            }

            if (matched && dragOperation != DragOperationNone)
                break;
        }
        if (matched) {
            dataTransfer->setDropEffect(convertDragOperationToDropZoneOperation(dragOperation));
            return true;
        }
    }
    return false;
}

bool EventHandler::updateDragAndDrop(const PlatformMouseEvent& event, DataTransfer* dataTransfer)
{
    bool accept = false;

    if (!m_frame->view())
        return false;

    HitTestRequest request(HitTestRequest::ReadOnly);
    MouseEventWithHitTestResults mev = prepareMouseEvent(request, event);

    // Drag events should never go to text nodes (following IE, and proper mouseover/out dispatch)
    RefPtrWillBeRawPtr<Node> newTarget = mev.innerNode();
    if (newTarget && newTarget->isTextNode())
        newTarget = ComposedTreeTraversal::parent(*newTarget);

    if (AutoscrollController* controller = autoscrollController())
        controller->updateDragAndDrop(newTarget.get(), event.position(), event.timestamp());

    if (m_dragTarget != newTarget) {
        // FIXME: this ordering was explicitly chosen to match WinIE. However,
        // it is sometimes incorrect when dragging within subframes, as seen with
        // LayoutTests/fast/events/drag-in-frames.html.
        //
        // Moreover, this ordering conforms to section 7.9.4 of the HTML 5 spec. <http://dev.w3.org/html5/spec/Overview.html#drag-and-drop-processing-model>.
        LocalFrame* targetFrame;
        if (targetIsFrame(newTarget.get(), targetFrame)) {
            if (targetFrame)
                accept = targetFrame->eventHandler().updateDragAndDrop(event, dataTransfer);
        } else if (newTarget) {
            // As per section 7.9.4 of the HTML 5 spec., we must always fire a drag event before firing a dragenter, dragleave, or dragover event.
            if (dragState().m_dragSrc) {
                // for now we don't care if event handler cancels default behavior, since there is none
                dispatchDragSrcEvent(EventTypeNames::drag, event);
            }
            accept = dispatchDragEvent(EventTypeNames::dragenter, newTarget.get(), event, dataTransfer);
            if (!accept)
                accept = findDropZone(newTarget.get(), dataTransfer);
        }

        if (targetIsFrame(m_dragTarget.get(), targetFrame)) {
            if (targetFrame)
                accept = targetFrame->eventHandler().updateDragAndDrop(event, dataTransfer);
        } else if (m_dragTarget) {
            dispatchDragEvent(EventTypeNames::dragleave, m_dragTarget.get(), event, dataTransfer);
        }

        if (newTarget) {
            // We do not explicitly call dispatchDragEvent here because it could ultimately result in the appearance that
            // two dragover events fired. So, we mark that we should only fire a dragover event on the next call to this function.
            m_shouldOnlyFireDragOverEvent = true;
        }
    } else {
        LocalFrame* targetFrame;
        if (targetIsFrame(newTarget.get(), targetFrame)) {
            if (targetFrame)
                accept = targetFrame->eventHandler().updateDragAndDrop(event, dataTransfer);
        } else if (newTarget) {
            // Note, when dealing with sub-frames, we may need to fire only a dragover event as a drag event may have been fired earlier.
            if (!m_shouldOnlyFireDragOverEvent && dragState().m_dragSrc) {
                // for now we don't care if event handler cancels default behavior, since there is none
                dispatchDragSrcEvent(EventTypeNames::drag, event);
            }
            accept = dispatchDragEvent(EventTypeNames::dragover, newTarget.get(), event, dataTransfer);
            if (!accept)
                accept = findDropZone(newTarget.get(), dataTransfer);
            m_shouldOnlyFireDragOverEvent = false;
        }
    }
    m_dragTarget = newTarget;

    return accept;
}

void EventHandler::cancelDragAndDrop(const PlatformMouseEvent& event, DataTransfer* dataTransfer)
{
    LocalFrame* targetFrame;
    if (targetIsFrame(m_dragTarget.get(), targetFrame)) {
        if (targetFrame)
            targetFrame->eventHandler().cancelDragAndDrop(event, dataTransfer);
    } else if (m_dragTarget.get()) {
        if (dragState().m_dragSrc)
            dispatchDragSrcEvent(EventTypeNames::drag, event);
        dispatchDragEvent(EventTypeNames::dragleave, m_dragTarget.get(), event, dataTransfer);
    }
    clearDragState();
}

bool EventHandler::performDragAndDrop(const PlatformMouseEvent& event, DataTransfer* dataTransfer)
{
    LocalFrame* targetFrame;
    bool preventedDefault = false;
    if (targetIsFrame(m_dragTarget.get(), targetFrame)) {
        if (targetFrame)
            preventedDefault = targetFrame->eventHandler().performDragAndDrop(event, dataTransfer);
    } else if (m_dragTarget.get()) {
        preventedDefault = dispatchDragEvent(EventTypeNames::drop, m_dragTarget.get(), event, dataTransfer);
    }
    clearDragState();
    return preventedDefault;
}

void EventHandler::clearDragState()
{
    stopAutoscroll();
    m_dragTarget = nullptr;
    m_capturingMouseEventsNode = nullptr;
    m_shouldOnlyFireDragOverEvent = false;
}

void EventHandler::setCapturingMouseEventsNode(PassRefPtrWillBeRawPtr<Node> n)
{
    m_capturingMouseEventsNode = n;
    m_eventHandlerWillResetCapturingMouseEventsNode = false;
}

MouseEventWithHitTestResults EventHandler::prepareMouseEvent(const HitTestRequest& request, const PlatformMouseEvent& mev)
{
    ASSERT(m_frame);
    ASSERT(m_frame->document());

    return m_frame->document()->prepareMouseEvent(request, contentPointFromRootFrame(m_frame, mev.position()), mev);
}

void EventHandler::updateMouseEventTargetNode(Node* targetNode, const PlatformMouseEvent& mouseEvent, bool fireMouseEvents)
{
    Node* result = targetNode;

    // If we're capturing, we always go right to that node.
    if (m_capturingMouseEventsNode) {
        result = m_capturingMouseEventsNode.get();
    } else {
        // If the target node is a text node, dispatch on the parent node - rdar://4196646
        if (result && result->isTextNode())
            result = ComposedTreeTraversal::parent(*result);
    }
    m_nodeUnderMouse = result;

    if (fireMouseEvents) {
        DeprecatedPaintLayer* layerForLastNode = layerForNode(m_lastNodeUnderMouse.get());
        DeprecatedPaintLayer* layerForNodeUnderMouse = layerForNode(m_nodeUnderMouse.get());
        Page* page = m_frame->page();

        if (m_lastNodeUnderMouse && (!m_nodeUnderMouse || m_nodeUnderMouse->document() != m_frame->document())) {
            // The mouse has moved between frames.
            if (LocalFrame* frame = m_lastNodeUnderMouse->document().frame()) {
                if (FrameView* frameView = frame->view())
                    frameView->mouseExitedContentArea();
            }
        } else if (page && (layerForLastNode && (!layerForNodeUnderMouse || layerForNodeUnderMouse != layerForLastNode))) {
            // The mouse has moved between layers.
            if (ScrollableArea* scrollableAreaForLastNode = associatedScrollableArea(layerForLastNode))
                scrollableAreaForLastNode->mouseExitedContentArea();
        }

        if (m_nodeUnderMouse && (!m_lastNodeUnderMouse || m_lastNodeUnderMouse->document() != m_frame->document())) {
            // The mouse has moved between frames.
            if (LocalFrame* frame = m_nodeUnderMouse->document().frame()) {
                if (FrameView* frameView = frame->view())
                    frameView->mouseEnteredContentArea();
            }
        } else if (page && (layerForNodeUnderMouse && (!layerForLastNode || layerForNodeUnderMouse != layerForLastNode))) {
            // The mouse has moved between layers.
            if (ScrollableArea* scrollableAreaForNodeUnderMouse = associatedScrollableArea(layerForNodeUnderMouse))
                scrollableAreaForNodeUnderMouse->mouseEnteredContentArea();
        }

        if (m_lastNodeUnderMouse && m_lastNodeUnderMouse->document() != m_frame->document()) {
            m_lastNodeUnderMouse = nullptr;
            m_lastScrollbarUnderMouse = nullptr;
        }

        if (m_lastNodeUnderMouse != m_nodeUnderMouse)
            sendMouseEventsForNodeTransition(m_lastNodeUnderMouse.get(), m_nodeUnderMouse.get(), mouseEvent);

        m_lastNodeUnderMouse = m_nodeUnderMouse;
    }
}

void EventHandler::sendMouseEventsForNodeTransition(Node* exitedNode, Node* enteredNode, const PlatformMouseEvent& mouseEvent)
{
    ASSERT(exitedNode != enteredNode);

    // First, dispatch mouseout event (which bubbles to ancestors)
    if (exitedNode)
        exitedNode->dispatchMouseEvent(mouseEvent, EventTypeNames::mouseout, 0, enteredNode);

    // A note on mouseenter and mouseleave: These are non-bubbling events, and they are dispatched if there
    // is a capturing event handler on an ancestor or a normal event handler on the element itself. This special
    // handling is necessary to avoid O(n^2) capturing event handler checks.
    //
    //   Note, however, that this optimization can possibly cause some unanswered/missing/redundant mouseenter or
    // mouseleave events in certain contrived eventhandling scenarios, e.g., when:
    // - the mouseleave handler for a node sets the only capturing-mouseleave-listener in its ancestor, or
    // - DOM mods in any mouseenter/mouseleave handler changes the common ancestor of exited & entered nodes, etc.
    // We think the spec specifies a "frozen" state to avoid such corner cases (check the discussion on "candidate event
    // listeners" at http://www.w3.org/TR/uievents), but our code below preserves one such behavior from past only to
    // match Firefox and IE behavior.
    //
    // TODO(mustaq): Confirm spec conformance, double-check with other browsers.

    // Create lists of all exited/entered ancestors.
    WillBeHeapVector<RefPtrWillBeMember<Node>, 32> exitedAncestors;
    WillBeHeapVector<RefPtrWillBeMember<Node>, 32> enteredAncestors;
    if (exitedNode) {
        exitedNode->updateDistribution();
        for (Node* node = exitedNode; node; node = ComposedTreeTraversal::parent(*node)) {
            exitedAncestors.append(node);
        }
    }
    if (enteredNode) {
        enteredNode->updateDistribution();
        for (Node* node = enteredNode; node; node = ComposedTreeTraversal::parent(*node)) {
            enteredAncestors.append(node);
        }
    }

    size_t numExitedAncestors = exitedAncestors.size();
    size_t numEnteredAncestors = enteredAncestors.size();

    // Locate the common ancestor in the two lists. Start with the assumption that it's off both the lists.
    size_t exitedAncestorIndex = numExitedAncestors;
    size_t enteredAncestorIndex = numEnteredAncestors;
    for (size_t j = 0; j < numExitedAncestors; j++) {
        for (size_t i = 0; i < numEnteredAncestors; i++) {
            if (exitedAncestors[j] == enteredAncestors[i]) {
                exitedAncestorIndex = j;
                enteredAncestorIndex = i;
                break;
            }
        }
        if (exitedAncestorIndex < numExitedAncestors)
            break;
    }

    // Determine if there is a capturing mouseleave listener in an ancestor.
    bool exitedNodeHasCapturingAncestor = false;
    for (size_t j = 0; j < numExitedAncestors; j++) {
        if (exitedAncestors[j]->hasCapturingEventListeners(EventTypeNames::mouseleave))
            exitedNodeHasCapturingAncestor = true;
    }

    // Send mouseleave events to appropriate exited ancestors, in child-to-parent order.
    for (size_t j = 0; j < exitedAncestorIndex; j++) {
        if (exitedNodeHasCapturingAncestor || exitedAncestors[j]->hasEventListeners(EventTypeNames::mouseleave))
            exitedAncestors[j]->dispatchMouseEvent(mouseEvent, EventTypeNames::mouseleave, 0, enteredNode);
    }

    // Dispatch mouseover event (which bubbles to ancestors) after the mouseleave events are sent.
    if (enteredNode)
        enteredNode->dispatchMouseEvent(mouseEvent, EventTypeNames::mouseover, 0, exitedNode);

    // Determine if there is a capturing mouseenter listener in an ancestor. This must be done /after/ dispatching the
    // mouseleave events because the handler for mouseleave might set a capturing mouseenter handler.
    bool enteredNodeHasCapturingAncestor = false;
    for (size_t i = 0; i < numEnteredAncestors; i++) {
        if (enteredAncestors[i]->hasCapturingEventListeners(EventTypeNames::mouseenter))
            enteredNodeHasCapturingAncestor = true;
    }

    // Send mouseenter events to appropriate entered ancestors, in parent-to-child order.
    for (size_t i = enteredAncestorIndex; i > 0; i--) {
        if (enteredNodeHasCapturingAncestor || enteredAncestors[i-1]->hasEventListeners(EventTypeNames::mouseenter))
            enteredAncestors[i-1]->dispatchMouseEvent(mouseEvent, EventTypeNames::mouseenter, 0, exitedNode);
    }
}

// The return value means 'continue default handling.'
// TODO(mustaq): setUnder needs a more informative name.
bool EventHandler::dispatchMouseEvent(const AtomicString& eventType, Node* targetNode, int clickCount, const PlatformMouseEvent& mouseEvent, bool setUnder)
{
    updateMouseEventTargetNode(targetNode, mouseEvent, setUnder);
    return !m_nodeUnderMouse || m_nodeUnderMouse->dispatchMouseEvent(mouseEvent, eventType, clickCount);
}

// The return value means 'swallow event' (was handled), as for other handle* functions.
bool EventHandler::handleMouseFocus(const MouseEventWithHitTestResults& targetedEvent)
{
    const PlatformMouseEvent& mouseEvent = targetedEvent.event();

    // If clicking on a frame scrollbar, do not mess up with content focus.
    if (FrameView* view = m_frame->view()) {
        if (view->scrollbarAtRootFramePoint(mouseEvent.position()))
            return false;
    }

    // The layout needs to be up to date to determine if an element is focusable.
    m_frame->document()->updateLayoutIgnorePendingStylesheets();

    Element* element = nullptr;
    if (m_nodeUnderMouse)
        element = m_nodeUnderMouse->isElementNode() ? toElement(m_nodeUnderMouse) : m_nodeUnderMouse->parentOrShadowHostElement();
    for (; element; element = element->parentOrShadowHostElement()) {
        if (element->isFocusable() && element->isFocusedElementInDocument())
            return false;
        if (element->isMouseFocusable())
            break;
    }
    ASSERT(!element || element->isMouseFocusable());

    // To fix <rdar://problem/4895428> Can't drag selected ToDo, we don't focus
    // a node on mouse down if it's selected and inside a focused node. It will
    // be focused if the user does a mouseup over it, however, because the
    // mouseup will set a selection inside it, which will call
    // FrameSelection::setFocusedNodeIfNeeded.
    if (element && m_frame->selection().isRange()) {
        if (m_frame->selection().toNormalizedRange()->isNodeFullyContained(*element)
            && element->isDescendantOf(m_frame->document()->focusedElement()))
            return false;
    }


    // Only change the focus when clicking scrollbars if it can transfered to a
    // mouse focusable node.
    if (!element && targetedEvent.hitTestResult().scrollbar())
        return true;

    if (Page* page = m_frame->page()) {
        // If focus shift is blocked, we eat the event. Note we should never
        // clear swallowEvent if the page already set it (e.g., by canceling
        // default behavior).
        if (element) {
            if (slideFocusOnShadowHostIfNecessary(*element))
                return true;
            if (!page->focusController().setFocusedElement(element, m_frame, WebFocusTypeMouse))
                return true;
        } else {
            // We call setFocusedElement even with !element in order to blur
            // current focus element when a link is clicked; this is expected by
            // some sites that rely on onChange handlers running from form
            // fields before the button click is processed.
            if (!page->focusController().setFocusedElement(0, m_frame))
                return true;
        }
    }

    return false;
}

bool EventHandler::slideFocusOnShadowHostIfNecessary(const Element& element)
{
    if (element.shadowRoot() && element.shadowRoot()->delegatesFocus()) {
        Document* doc = m_frame->document();
        if (element.containsIncludingShadowDOM(doc->focusedElement())) {
            // If the inner element is already focused, do nothing.
            return true;
        }

        // If the host has a focusable inner element, focus it. Otherwise, the host takes focus.
        Page* page = m_frame->page();
        ASSERT(page);
        Element* next = page->focusController().findFocusableElement(WebFocusTypeForward, *element.shadowRoot());
        if (next && element.containsIncludingShadowDOM(next)) {
            // Use WebFocusTypeForward instead of WebFocusTypeMouse here to mean the focus has slided.
            next->focus(false, WebFocusTypeForward);
            return true;
        }
    }
    return false;
}

bool EventHandler::handleWheelEvent(const PlatformWheelEvent& event)
{
#define RETURN_WHEEL_EVENT_HANDLED() \
    { \
        setFrameWasScrolledByUser(); \
        return true; \
    }

    Document* doc = m_frame->document();

    if (!doc->layoutView())
        return false;

    RefPtrWillBeRawPtr<FrameView> protector(m_frame->view());

    FrameView* view = m_frame->view();
    if (!view)
        return false;

    LayoutPoint vPoint = view->rootFrameToContents(event.position());

    // Pass Active to ensure that hit-test result in current document if otherwise empty
    // TODO(majidvp): Avoid using Active to trigger hit-testing fallback. See http://crbug.com/505825
    HitTestRequest request(HitTestRequest::ReadOnly | HitTestRequest::Active);
    HitTestResult result(request, vPoint);
    doc->layoutView()->hitTest(result);

    Node* node = result.innerNode();
    // Wheel events should not dispatch to text nodes.
    if (node && node->isTextNode())
        node = ComposedTreeTraversal::parent(*node);

    bool isOverWidget;
    if (event.useLatchedEventNode()) {
        if (!m_latchedWheelEventNode) {
            m_latchedWheelEventNode = node;
            m_widgetIsLatched = result.isOverWidget();
        } else {
            node = m_latchedWheelEventNode.get();
        }

        isOverWidget = m_widgetIsLatched;
    } else {
        if (m_latchedWheelEventNode)
            m_latchedWheelEventNode = nullptr;
        if (m_previousWheelScrolledNode)
            m_previousWheelScrolledNode = nullptr;

        isOverWidget = result.isOverWidget();
    }

    if (node) {
        // Figure out which view to send the event to.
        LayoutObject* target = node->layoutObject();

        if (isOverWidget && target && target->isLayoutPart()) {
            Widget* widget = toLayoutPart(target)->widget();
            if (widget && passWheelEventToWidget(event, *widget))
                RETURN_WHEEL_EVENT_HANDLED();
        }

        if (node && !node->dispatchWheelEvent(event))
            RETURN_WHEEL_EVENT_HANDLED();
    }

    // We do another check on the frame view because the event handler can run
    // JS which results in the frame getting destroyed.
    view = m_frame->view();
    if (!view)
        return false;

    ScrollResult scrollResult = view->scrollableArea()->handleWheel(event);
    if (m_frame->settings() && m_frame->settings()->reportWheelOverscroll())
        handleOverscroll(scrollResult);
    if (scrollResult.didScroll())
        RETURN_WHEEL_EVENT_HANDLED();

    return false;
#undef RETURN_WHEEL_EVENT_HANDLED
}

void EventHandler::defaultWheelEventHandler(Node* startNode, WheelEvent* wheelEvent)
{
    if (!startNode || !wheelEvent)
        return;

    // When the wheelEvent do not scroll, we trigger zoom in/out instead.
    if (!wheelEvent->canScroll())
        return;

    Node* stopNode = m_previousWheelScrolledNode.get();
    ScrollGranularity granularity = wheelGranularityToScrollGranularity(wheelEvent);
    IntPoint absolutePosition = roundedIntPoint(wheelEvent->absoluteLocation());

    // Break up into two scrolls if we need to.  Diagonal movement on
    // a MacBook pro is an example of a 2-dimensional mouse wheel event (where both deltaX and deltaY can be set).

    // FIXME: enable scroll customization in this case. See crbug.com/410974.
    if (wheelEvent->railsMode() != Event::RailsModeVertical
        && scroll(ScrollRightIgnoringWritingMode, granularity, startNode, &stopNode, wheelEvent->deltaX(), absolutePosition).didScroll)
        wheelEvent->setDefaultHandled();

    if (wheelEvent->railsMode() != Event::RailsModeHorizontal
        && scroll(ScrollDownIgnoringWritingMode, granularity, startNode, &stopNode, wheelEvent->deltaY(), absolutePosition).didScroll)
        wheelEvent->setDefaultHandled();

    if (!m_latchedWheelEventNode)
        m_previousWheelScrolledNode = stopNode;
}

bool EventHandler::handleGestureShowPress()
{
    m_lastShowPressTimestamp = WTF::currentTime();

    FrameView* view = m_frame->view();
    if (!view)
        return false;
    if (ScrollAnimator* scrollAnimator = view->existingScrollAnimator())
        scrollAnimator->cancelAnimations();
    const FrameView::ScrollableAreaSet* areas = view->scrollableAreas();
    if (!areas)
        return false;
    for (const ScrollableArea* scrollableArea : *areas) {
        ScrollAnimator* animator = scrollableArea->existingScrollAnimator();
        if (animator)
            animator->cancelAnimations();
    }
    return false;
}

bool EventHandler::handleGestureEvent(const PlatformGestureEvent& gestureEvent)
{
    // Propagation to inner frames is handled below this function.
    ASSERT(m_frame == m_frame->localFrameRoot());

    // Scrolling-related gesture events invoke EventHandler recursively for each frame down
    // the chain, doing a single-frame hit-test per frame. This matches handleWheelEvent.
    // FIXME: Add a test that traverses this path, e.g. for devtools overlay.
    if (gestureEvent.isScrollEvent())
        return handleGestureScrollEvent(gestureEvent);

    // Hit test across all frames and do touch adjustment as necessary for the event type.
    GestureEventWithHitTestResults targetedEvent = targetGestureEvent(gestureEvent);

    return handleGestureEvent(targetedEvent);
}

bool EventHandler::handleGestureEvent(const GestureEventWithHitTestResults& targetedEvent)
{
    TRACE_EVENT0("input", "EventHandler::handleGestureEvent");

    // Propagation to inner frames is handled below this function.
    ASSERT(m_frame == m_frame->localFrameRoot());

    // Non-scrolling related gesture events do a single cross-frame hit-test and jump
    // directly to the inner most frame. This matches handleMousePressEvent etc.
    ASSERT(!targetedEvent.event().isScrollEvent());

    // Route to the correct frame.
    if (LocalFrame* innerFrame = targetedEvent.hitTestResult().innerNodeFrame())
        return innerFrame->eventHandler().handleGestureEventInFrame(targetedEvent);

    // No hit test result, handle in root instance. Perhaps we should just return false instead?
    return handleGestureEventInFrame(targetedEvent);
}

bool EventHandler::handleGestureEventInFrame(const GestureEventWithHitTestResults& targetedEvent)
{
    ASSERT(!targetedEvent.event().isScrollEvent());

    RefPtrWillBeRawPtr<Node> eventTarget = targetedEvent.hitTestResult().innerNode();
    RefPtrWillBeRawPtr<Scrollbar> scrollbar = targetedEvent.hitTestResult().scrollbar();
    const PlatformGestureEvent& gestureEvent = targetedEvent.event();

    if (scrollbar) {
        bool eventSwallowed = scrollbar->gestureEvent(gestureEvent);
        if (gestureEvent.type() == PlatformEvent::GestureTapDown && eventSwallowed)
            m_scrollbarHandlingScrollGesture = scrollbar;
        if (eventSwallowed)
            return true;
    }

    if (eventTarget && eventTarget->dispatchGestureEvent(gestureEvent))
        return true;

    switch (gestureEvent.type()) {
    case PlatformEvent::GestureTap:
        return handleGestureTap(targetedEvent);
    case PlatformEvent::GestureShowPress:
        return handleGestureShowPress();
    case PlatformEvent::GestureLongPress:
        return handleGestureLongPress(targetedEvent);
    case PlatformEvent::GestureLongTap:
        return handleGestureLongTap(targetedEvent);
    case PlatformEvent::GestureTwoFingerTap:
        return sendContextMenuEventForGesture(targetedEvent);
    case PlatformEvent::GestureTapDown:
    case PlatformEvent::GesturePinchBegin:
    case PlatformEvent::GesturePinchEnd:
    case PlatformEvent::GesturePinchUpdate:
    case PlatformEvent::GestureTapDownCancel:
    case PlatformEvent::GestureTapUnconfirmed:
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    return false;
}

bool EventHandler::handleGestureScrollEvent(const PlatformGestureEvent& gestureEvent)
{
    TRACE_EVENT0("input", "EventHandler::handleGestureScrollEvent");

    RefPtrWillBeRawPtr<Node> eventTarget = nullptr;
    RefPtrWillBeRawPtr<Scrollbar> scrollbar = nullptr;
    if (gestureEvent.type() != PlatformEvent::GestureScrollBegin) {
        scrollbar = m_scrollbarHandlingScrollGesture.get();
        eventTarget = m_scrollGestureHandlingNode.get();
    }

    if (!eventTarget) {
        Document* document = m_frame->document();
        if (!document->layoutView())
            return false;

        FrameView* view = m_frame->view();
        LayoutPoint viewPoint = view->rootFrameToContents(gestureEvent.position());
        HitTestRequest request(HitTestRequest::ReadOnly);
        HitTestResult result(request, viewPoint);
        document->layoutView()->hitTest(result);

        eventTarget = result.innerNode();

        m_lastGestureScrollOverWidget = result.isOverWidget();
        m_scrollGestureHandlingNode = eventTarget;
        m_previousGestureScrolledNode = nullptr;

        if (!scrollbar)
            scrollbar = result.scrollbar();
    }

    if (scrollbar) {
        bool eventSwallowed = scrollbar->gestureEvent(gestureEvent);
        if (gestureEvent.type() == PlatformEvent::GestureScrollEnd
            || gestureEvent.type() == PlatformEvent::GestureFlingStart
            || !eventSwallowed) {
            m_scrollbarHandlingScrollGesture = nullptr;
        }
        if (eventSwallowed)
            return true;
    }

    if (eventTarget) {
        bool eventSwallowed = handleScrollGestureOnResizer(eventTarget.get(), gestureEvent);
        if (!eventSwallowed)
            eventSwallowed = eventTarget->dispatchGestureEvent(gestureEvent);
        if (eventSwallowed)
            return true;
    }

    switch (gestureEvent.type()) {
    case PlatformEvent::GestureScrollBegin:
        return handleGestureScrollBegin(gestureEvent);
    case PlatformEvent::GestureScrollUpdate:
        return handleGestureScrollUpdate(gestureEvent);
    case PlatformEvent::GestureScrollEnd:
        return handleGestureScrollEnd(gestureEvent);
    case PlatformEvent::GestureFlingStart:
    case PlatformEvent::GesturePinchBegin:
    case PlatformEvent::GesturePinchEnd:
    case PlatformEvent::GesturePinchUpdate:
        return false;
    default:
        ASSERT_NOT_REACHED();
        return false;
    }
}

bool EventHandler::handleGestureTap(const GestureEventWithHitTestResults& targetedEvent)
{
    RefPtrWillBeRawPtr<FrameView> frameView(m_frame->view());
    const PlatformGestureEvent& gestureEvent = targetedEvent.event();
    HitTestRequest::HitTestRequestType hitType = getHitTypeForGestureType(gestureEvent.type());
    uint64_t preDispatchDomTreeVersion = m_frame->document()->domTreeVersion();
    uint64_t preDispatchStyleVersion = m_frame->document()->styleVersion();

    UserGestureIndicator gestureIndicator(DefinitelyProcessingUserGesture);

    HitTestResult currentHitTest = targetedEvent.hitTestResult();

    // We use the adjusted position so the application isn't surprised to see a event with
    // co-ordinates outside the target's bounds.
    IntPoint adjustedPoint = frameView->rootFrameToContents(gestureEvent.position());

    unsigned modifiers = gestureEvent.modifiers();
    PlatformMouseEvent fakeMouseMove(gestureEvent.position(), gestureEvent.globalPosition(),
        NoButton, PlatformEvent::MouseMoved, /* clickCount */ 0,
        static_cast<PlatformEvent::Modifiers>(modifiers),
        PlatformMouseEvent::FromTouch, gestureEvent.timestamp());
    dispatchMouseEvent(EventTypeNames::mousemove, currentHitTest.innerNode(), 0, fakeMouseMove, true);

    // Do a new hit-test in case the mousemove event changed the DOM.
    // Note that if the original hit test wasn't over an element (eg. was over a scrollbar) we
    // don't want to re-hit-test because it may be in the wrong frame (and there's no way the page
    // could have seen the event anyway).
    // Also note that the position of the frame may have changed, so we need to recompute the content
    // co-ordinates (updating layout/style as hitTestResultAtPoint normally would).
    // FIXME: Use a hit-test cache to avoid unnecessary hit tests. http://crbug.com/398920
    if (currentHitTest.innerNode()) {
        LocalFrame* mainFrame = m_frame->localFrameRoot();
        if (mainFrame && mainFrame->view())
            mainFrame->view()->updateAllLifecyclePhases();
        adjustedPoint = frameView->rootFrameToContents(gestureEvent.position());
        currentHitTest = hitTestResultInFrame(m_frame, adjustedPoint, hitType);
    }
    m_clickNode = currentHitTest.innerNode();

    // Capture data for showUnhandledTapUIIfNeeded.
    RefPtrWillBeRawPtr<Node> tappedNode = m_clickNode;
    IntPoint tappedPosition = gestureEvent.position();

    if (m_clickNode && m_clickNode->isTextNode())
        m_clickNode = ComposedTreeTraversal::parent(*m_clickNode);

    PlatformMouseEvent fakeMouseDown(gestureEvent.position(), gestureEvent.globalPosition(),
        LeftButton, PlatformEvent::MousePressed, gestureEvent.tapCount(),
        static_cast<PlatformEvent::Modifiers>(modifiers | PlatformEvent::LeftButtonDown),
        PlatformMouseEvent::FromTouch,  gestureEvent.timestamp());
    bool swallowMouseDownEvent = !dispatchMouseEvent(EventTypeNames::mousedown, currentHitTest.innerNode(), gestureEvent.tapCount(), fakeMouseDown, true);
    if (!swallowMouseDownEvent)
        swallowMouseDownEvent = handleMouseFocus(MouseEventWithHitTestResults(fakeMouseDown, currentHitTest));
    if (!swallowMouseDownEvent)
        swallowMouseDownEvent = handleMousePressEvent(MouseEventWithHitTestResults(fakeMouseDown, currentHitTest));

    if (currentHitTest.innerNode()) {
        ASSERT(gestureEvent.type() == PlatformEvent::GestureTap);
        HitTestResult result = currentHitTest;
        result.setToShadowHostIfInUserAgentShadowRoot();
        m_frame->chromeClient().onMouseDown(result.innerNode());
    }

    // FIXME: Use a hit-test cache to avoid unnecessary hit tests. http://crbug.com/398920
    if (currentHitTest.innerNode()) {
        LocalFrame* mainFrame = m_frame->localFrameRoot();
        if (mainFrame && mainFrame->view())
            mainFrame->view()->updateAllLifecyclePhases();
        adjustedPoint = frameView->rootFrameToContents(gestureEvent.position());
        currentHitTest = hitTestResultInFrame(m_frame, adjustedPoint, hitType);
    }
    PlatformMouseEvent fakeMouseUp(gestureEvent.position(), gestureEvent.globalPosition(),
        LeftButton, PlatformEvent::MouseReleased, gestureEvent.tapCount(),
        static_cast<PlatformEvent::Modifiers>(modifiers),
        PlatformMouseEvent::FromTouch,  gestureEvent.timestamp());
    bool swallowMouseUpEvent = !dispatchMouseEvent(EventTypeNames::mouseup, currentHitTest.innerNode(), gestureEvent.tapCount(), fakeMouseUp, false);

    bool swallowClickEvent = false;
    if (m_clickNode) {
        if (currentHitTest.innerNode()) {
            // Updates distribution because a mouseup (or mousedown) event listener can make the
            // tree dirty at dispatchMouseEvent() invocation above.
            // Unless distribution is updated, commonAncestor would hit ASSERT.
            // Both m_clickNode and currentHitTest.innerNode()) don't need to be updated
            // because commonAncestor() will exit early if their documents are different.
            m_clickNode->updateDistribution();
            Node* clickTargetNode = currentHitTest.innerNode()->commonAncestor(*m_clickNode, parentForClickEvent);
            swallowClickEvent = !dispatchMouseEvent(EventTypeNames::click, clickTargetNode, gestureEvent.tapCount(), fakeMouseUp, true);
        }
        m_clickNode = nullptr;
    }

    if (!swallowMouseUpEvent)
        swallowMouseUpEvent = handleMouseReleaseEvent(MouseEventWithHitTestResults(fakeMouseUp, currentHitTest));

    bool swallowed = swallowMouseDownEvent | swallowMouseUpEvent | swallowClickEvent;
    if (!swallowed && tappedNode && m_frame->page()) {
        bool domTreeChanged = preDispatchDomTreeVersion != m_frame->document()->domTreeVersion();
        bool styleChanged = preDispatchStyleVersion != m_frame->document()->styleVersion();

        IntPoint tappedPositionInViewport = m_frame->page()->frameHost().pinchViewport().rootFrameToViewport(tappedPosition);
        m_frame->chromeClient().showUnhandledTapUIIfNeeded(tappedPositionInViewport, tappedNode.get(), domTreeChanged || styleChanged);
    }
    return swallowed;
}

bool EventHandler::handleGestureLongPress(const GestureEventWithHitTestResults& targetedEvent)
{
    const PlatformGestureEvent& gestureEvent = targetedEvent.event();
    IntPoint adjustedPoint = gestureEvent.position();

    unsigned modifiers = gestureEvent.modifiers();

    // FIXME: Ideally we should try to remove the extra mouse-specific hit-tests here (re-using the
    // supplied HitTestResult), but that will require some overhaul of the touch drag-and-drop code
    // and LongPress is such a special scenario that it's unlikely to matter much in practice.

    m_longTapShouldInvokeContextMenu = false;
    if (m_frame->settings() && m_frame->settings()->touchDragDropEnabled() && m_frame->view()) {
        PlatformMouseEvent mouseDownEvent(adjustedPoint, gestureEvent.globalPosition(), LeftButton, PlatformEvent::MousePressed, 1,
            static_cast<PlatformEvent::Modifiers>(modifiers | PlatformEvent::LeftButtonDown),
            PlatformMouseEvent::FromTouch, WTF::currentTime());
        m_mouseDown = mouseDownEvent;

        PlatformMouseEvent mouseDragEvent(adjustedPoint, gestureEvent.globalPosition(), LeftButton, PlatformEvent::MouseMoved, 1,
            static_cast<PlatformEvent::Modifiers>(modifiers | PlatformEvent::LeftButtonDown),
            PlatformMouseEvent::FromTouch, WTF::currentTime());
        HitTestRequest request(HitTestRequest::ReadOnly);
        MouseEventWithHitTestResults mev = prepareMouseEvent(request, mouseDragEvent);
        m_mouseDownMayStartDrag = true;
        dragState().m_dragSrc = nullptr;
        m_mouseDownPos = m_frame->view()->rootFrameToContents(mouseDragEvent.position());
        RefPtrWillBeRawPtr<FrameView> protector(m_frame->view());
        if (handleDrag(mev, DragInitiator::Touch)) {
            m_longTapShouldInvokeContextMenu = true;
            return true;
        }
    }

    IntPoint hitTestPoint = m_frame->view()->rootFrameToContents(gestureEvent.position());
    HitTestResult result = hitTestResultAtPoint(hitTestPoint);
    if (selectionController().handleGestureLongPress(gestureEvent, result)) {
        focusDocumentView();
        return true;
    }

    return sendContextMenuEventForGesture(targetedEvent);
}

bool EventHandler::handleGestureLongTap(const GestureEventWithHitTestResults& targetedEvent)
{
#if !OS(ANDROID)
    if (m_longTapShouldInvokeContextMenu) {
        m_longTapShouldInvokeContextMenu = false;
        return sendContextMenuEventForGesture(targetedEvent);
    }
#endif
    return false;
}

bool EventHandler::handleScrollGestureOnResizer(Node* eventTarget, const PlatformGestureEvent& gestureEvent)
{
    if (gestureEvent.type() == PlatformEvent::GestureScrollBegin) {
        DeprecatedPaintLayer* layer = eventTarget->layoutObject() ? eventTarget->layoutObject()->enclosingLayer() : nullptr;
        IntPoint p = m_frame->view()->rootFrameToContents(gestureEvent.position());
        if (layer && layer->scrollableArea() && layer->scrollableArea()->isPointInResizeControl(p, ResizerForTouch)) {
            m_resizeScrollableArea = layer->scrollableArea();
            m_resizeScrollableArea->setInResizeMode(true);
            m_offsetFromResizeCorner = LayoutSize(m_resizeScrollableArea->offsetFromResizeCorner(p));
            return true;
        }
    } else if (gestureEvent.type() == PlatformEvent::GestureScrollUpdate) {
        if (m_resizeScrollableArea && m_resizeScrollableArea->inResizeMode()) {
            m_resizeScrollableArea->resize(gestureEvent, m_offsetFromResizeCorner);
            return true;
        }
    } else if (gestureEvent.type() == PlatformEvent::GestureScrollEnd) {
        if (m_resizeScrollableArea && m_resizeScrollableArea->inResizeMode()) {
            m_resizeScrollableArea->setInResizeMode(false);
            m_resizeScrollableArea = nullptr;
            return false;
        }
    }

    return false;
}

bool EventHandler::passScrollGestureEventToWidget(const PlatformGestureEvent& gestureEvent, LayoutObject* layoutObject)
{
    ASSERT(gestureEvent.isScrollEvent());

    if (!m_lastGestureScrollOverWidget)
        return false;

    if (!layoutObject || !layoutObject->isLayoutPart())
        return false;

    Widget* widget = toLayoutPart(layoutObject)->widget();

    if (!widget || !widget->isFrameView())
        return false;

    return toFrameView(widget)->frame().eventHandler().handleGestureScrollEvent(gestureEvent);
}

bool EventHandler::handleGestureScrollEnd(const PlatformGestureEvent& gestureEvent)
{
    RefPtrWillBeRawPtr<Node> node = m_scrollGestureHandlingNode;

    if (node) {
        passScrollGestureEventToWidget(gestureEvent, node->layoutObject());
        if (RuntimeEnabledFeatures::scrollCustomizationEnabled()) {
            RefPtrWillBeRawPtr<ScrollState> scrollState = ScrollState::create(
                0, 0, 0, 0, 0, gestureEvent.inertial(), /* isBeginning */
                false, /* isEnding */ true, /* fromUserInput */ true);
            customizedScroll(*node.get(), *scrollState);
        }
    }

    clearGestureScrollState();
    return false;
}

bool EventHandler::handleGestureScrollBegin(const PlatformGestureEvent& gestureEvent)
{
    Document* document = m_frame->document();
    if (!document->layoutView())
        return false;

    FrameView* view = m_frame->view();
    if (!view)
        return false;

    // If there's no layoutObject on the node, send the event to the nearest ancestor with a layoutObject.
    // Needed for <option> and <optgroup> elements so we can touch scroll <select>s
    while (m_scrollGestureHandlingNode && !m_scrollGestureHandlingNode->layoutObject())
        m_scrollGestureHandlingNode = m_scrollGestureHandlingNode->parentOrShadowHostNode();

    if (!m_scrollGestureHandlingNode) {
        if (RuntimeEnabledFeatures::scrollCustomizationEnabled())
            m_scrollGestureHandlingNode = m_frame->document()->documentElement();
        else
            return false;
    }
    ASSERT(m_scrollGestureHandlingNode);

    passScrollGestureEventToWidget(gestureEvent, m_scrollGestureHandlingNode->layoutObject());
    if (RuntimeEnabledFeatures::scrollCustomizationEnabled()) {
        m_currentScrollChain.clear();
        RefPtrWillBeRawPtr<ScrollState> scrollState = ScrollState::create(
            0, 0, 0, 0, 0, /* inInertialPhase */ false, /* isBeginning */
            true, /* isEnding */ false, /* fromUserInput */ true);
        customizedScroll(*m_scrollGestureHandlingNode.get(), *scrollState);
    } else {
        if (m_frame->isMainFrame())
            m_frame->host()->topControls().scrollBegin();
    }
    return true;
}

void EventHandler::resetOverscroll(bool didScrollX, bool didScrollY)
{
    if (didScrollX)
        m_accumulatedRootOverscroll.setWidth(0);
    if (didScrollY)
        m_accumulatedRootOverscroll.setHeight(0);
}

static inline FloatSize adjustOverscoll(FloatSize unusedDelta)
{
    if (std::abs(unusedDelta.width()) < minimumOverscrollDelta)
        unusedDelta.setWidth(0);
    if (std::abs(unusedDelta.height()) < minimumOverscrollDelta)
        unusedDelta.setHeight(0);

    return unusedDelta;
}

void EventHandler::handleOverscroll(const ScrollResult& scrollResult, const FloatPoint& position, const FloatSize& velocity)
{
    FloatSize unusedDelta(scrollResult.unusedScrollDeltaX, scrollResult.unusedScrollDeltaY);
    unusedDelta = adjustOverscoll(unusedDelta);
    resetOverscroll(scrollResult.didScrollX, scrollResult.didScrollY);
    if (unusedDelta != FloatSize()) {
        m_accumulatedRootOverscroll += unusedDelta;
        m_frame->chromeClient().didOverscroll(unusedDelta, m_accumulatedRootOverscroll, position, velocity);
    }
}

bool EventHandler::handleGestureScrollUpdate(const PlatformGestureEvent& gestureEvent)
{
    ASSERT(gestureEvent.type() == PlatformEvent::GestureScrollUpdate);

    FloatSize delta(gestureEvent.deltaX(), gestureEvent.deltaY());
    if (delta.isZero())
        return false;

    Node* node = m_scrollGestureHandlingNode.get();
    if (node) {
        LayoutObject* layoutObject = node->layoutObject();
        if (!layoutObject)
            return false;

        RefPtrWillBeRawPtr<FrameView> protector(m_frame->view());

        Node* stopNode = nullptr;

        // Try to send the event to the correct view.
        if (passScrollGestureEventToWidget(gestureEvent, layoutObject)) {
            if (gestureEvent.preventPropagation()
                && !RuntimeEnabledFeatures::scrollCustomizationEnabled()) {
                // This is an optimization which doesn't apply with
                // scroll customization enabled.
                m_previousGestureScrolledNode = m_scrollGestureHandlingNode;
            }
            // FIXME: we should allow simultaneous scrolling of nested
            // iframes along perpendicular axes. See crbug.com/466991.
            m_deltaConsumedForScrollSequence = true;
            return true;
        }

        bool scrolled = false;
        if (RuntimeEnabledFeatures::scrollCustomizationEnabled()) {
            RefPtrWillBeRawPtr<ScrollState> scrollState = ScrollState::create(
                gestureEvent.deltaX(), gestureEvent.deltaY(),
                0, gestureEvent.velocityX(), gestureEvent.velocityY(),
                gestureEvent.inertial(), /* isBeginning */
                false, /* isEnding */ false, /* fromUserInput */ true,
                !gestureEvent.preventPropagation(), m_deltaConsumedForScrollSequence);
            if (m_previousGestureScrolledNode) {
                // The ScrollState needs to know what the current
                // native scrolling element is, so that for an
                // inertial scroll that shouldn't propagate, only the
                // currently scrolling element responds.
                ASSERT(m_previousGestureScrolledNode->isElementNode());
                scrollState->setCurrentNativeScrollingElement(toElement(m_previousGestureScrolledNode.get()));
            }
            customizedScroll(*node, *scrollState);
            m_previousGestureScrolledNode = scrollState->currentNativeScrollingElement();
            m_deltaConsumedForScrollSequence = scrollState->deltaConsumedForScrollSequence();
            scrolled = scrollState->deltaX() != gestureEvent.deltaX()
                || scrollState->deltaY() != gestureEvent.deltaY();
        } else {
            if (gestureEvent.preventPropagation())
                stopNode = m_previousGestureScrolledNode.get();

            // First try to scroll the closest scrollable LayoutBox ancestor of |node|.
            ScrollGranularity granularity = ScrollByPrecisePixel;
            ScrollResultOneDimensional result = scroll(ScrollLeftIgnoringWritingMode, granularity, node, &stopNode, delta.width());
            bool horizontalScroll = result.didScroll;
            if (!gestureEvent.preventPropagation())
                stopNode = nullptr;
            result = scroll(ScrollUpIgnoringWritingMode, granularity, node, &stopNode, delta.height());
            bool verticalScroll = result.didScroll;
            scrolled = horizontalScroll || verticalScroll;

            if (gestureEvent.preventPropagation())
                m_previousGestureScrolledNode = stopNode;

            resetOverscroll(horizontalScroll, verticalScroll);
        }
        if (scrolled) {
            setFrameWasScrolledByUser();
            return true;
        }
    }

    if (RuntimeEnabledFeatures::scrollCustomizationEnabled())
        return false;

    // Try to scroll the frame view.
    ScrollResult scrollResult = m_frame->applyScrollDelta(delta, false);
    FloatPoint position = FloatPoint(gestureEvent.position().x(), gestureEvent.position().y());
    FloatSize velocity = FloatSize(gestureEvent.velocityX(), gestureEvent.velocityY());
    if (m_frame->isMainFrame() && m_frame->view() && m_frame->view()->scrollableArea()) {
        ScrollableArea* scrollablearea = m_frame->view()->scrollableArea();
        // TODO(sataya.m) : In Case of android set unusedDelta to 0 to ensure overflow is not reported
        // on non-scrollable axis. Move this check to CC to block OverscrollGlow Animation on non-scrollable axes.
        scrollResult.unusedScrollDeltaX = scrollablearea->scrollSize(HorizontalScrollbar) ? scrollResult.unusedScrollDeltaX : 0;
        scrollResult.unusedScrollDeltaY = scrollablearea->scrollSize(VerticalScrollbar) ? scrollResult.unusedScrollDeltaY : 0;
    }
    handleOverscroll(scrollResult, position, velocity);
    if (scrollResult.didScroll()) {
        setFrameWasScrolledByUser();
        return true;
    }

    return false;
}

void EventHandler::clearGestureScrollState()
{
    m_scrollGestureHandlingNode = nullptr;
    m_previousGestureScrolledNode = nullptr;
    m_deltaConsumedForScrollSequence = false;
    m_currentScrollChain.clear();
    m_accumulatedRootOverscroll = FloatSize();
}

bool EventHandler::isScrollbarHandlingGestures() const
{
    return m_scrollbarHandlingScrollGesture.get();
}

bool EventHandler::shouldApplyTouchAdjustment(const PlatformGestureEvent& event) const
{
    if (m_frame->settings() && !m_frame->settings()->touchAdjustmentEnabled())
        return false;
    return !event.area().isEmpty();
}

bool EventHandler::bestClickableNodeForHitTestResult(const HitTestResult& result, IntPoint& targetPoint, Node*& targetNode)
{
    // FIXME: Unify this with the other best* functions which are very similar.

    TRACE_EVENT0("input", "EventHandler::bestClickableNodeForHitTestResult");
    ASSERT(result.isRectBasedTest());

    // If the touch is over a scrollbar, don't adjust the touch point since touch adjustment only takes into account
    // DOM nodes so a touch over a scrollbar will be adjusted towards nearby nodes. This leads to things like textarea
    // scrollbars being untouchable.
    if (result.scrollbar()) {
        targetNode = 0;
        return false;
    }

    IntPoint touchCenter = m_frame->view()->contentsToRootFrame(result.roundedPointInMainFrame());
    IntRect touchRect = m_frame->view()->contentsToRootFrame(result.hitTestLocation().boundingBox());

    WillBeHeapVector<RefPtrWillBeMember<Node>, 11> nodes;
    copyToVector(result.listBasedTestResult(), nodes);

    // FIXME: the explicit Vector conversion copies into a temporary and is wasteful.
    return findBestClickableCandidate(targetNode, targetPoint, touchCenter, touchRect, WillBeHeapVector<RefPtrWillBeMember<Node>> (nodes));
}

bool EventHandler::bestContextMenuNodeForHitTestResult(const HitTestResult& result, IntPoint& targetPoint, Node*& targetNode)
{
    ASSERT(result.isRectBasedTest());
    IntPoint touchCenter = m_frame->view()->contentsToRootFrame(result.roundedPointInMainFrame());
    IntRect touchRect = m_frame->view()->contentsToRootFrame(result.hitTestLocation().boundingBox());
    WillBeHeapVector<RefPtrWillBeMember<Node>, 11> nodes;
    copyToVector(result.listBasedTestResult(), nodes);

    // FIXME: the explicit Vector conversion copies into a temporary and is wasteful.
    return findBestContextMenuCandidate(targetNode, targetPoint, touchCenter, touchRect, WillBeHeapVector<RefPtrWillBeMember<Node>>(nodes));
}

bool EventHandler::bestZoomableAreaForTouchPoint(const IntPoint& touchCenter, const IntSize& touchRadius, IntRect& targetArea, Node*& targetNode)
{
    if (touchRadius.isEmpty())
        return false;

    IntPoint hitTestPoint = m_frame->view()->rootFrameToContents(touchCenter);

    HitTestRequest::HitTestRequestType hitType = HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::ListBased;
    HitTestResult result = hitTestResultAtPoint(hitTestPoint, hitType, LayoutSize(touchRadius));

    IntRect touchRect(touchCenter - touchRadius, touchRadius + touchRadius);
    WillBeHeapVector<RefPtrWillBeMember<Node>, 11> nodes;
    copyToVector(result.listBasedTestResult(), nodes);

    // FIXME: the explicit Vector conversion copies into a temporary and is wasteful.
    return findBestZoomableArea(targetNode, targetArea, touchCenter, touchRect, WillBeHeapVector<RefPtrWillBeMember<Node>>(nodes));
}

// Update the hover and active state across all frames for this gesture.
// This logic is different than the mouse case because mice send MouseLeave events to frames as they're exited.
// With gestures, a single event conceptually both 'leaves' whatever frame currently had hover and enters a new frame
void EventHandler::updateGestureHoverActiveState(const HitTestRequest& request, Element* innerElement)
{
    ASSERT(m_frame == m_frame->localFrameRoot());

    WillBeHeapVector<LocalFrame*> newHoverFrameChain;
    LocalFrame* newHoverFrameInDocument = innerElement ? innerElement->document().frame() : nullptr;
    // Insert the ancestors of the frame having the new hovered node to the frame chain
    // The frame chain doesn't include the main frame to avoid the redundant work that cleans the hover state.
    // Because the hover state for the main frame is updated by calling Document::updateHoverActiveState
    while (newHoverFrameInDocument && newHoverFrameInDocument != m_frame) {
        newHoverFrameChain.append(newHoverFrameInDocument);
        Frame* parentFrame = newHoverFrameInDocument->tree().parent();
        newHoverFrameInDocument = parentFrame && parentFrame->isLocalFrame() ? toLocalFrame(parentFrame) : nullptr;
    }

    RefPtrWillBeRawPtr<Node> oldHoverNodeInCurDoc = m_frame->document()->hoverNode();
    RefPtrWillBeRawPtr<Node> newInnermostHoverNode = innerElement;

    if (newInnermostHoverNode != oldHoverNodeInCurDoc) {
        size_t indexFrameChain = newHoverFrameChain.size();

        // Clear the hover state on any frames which are no longer in the frame chain of the hovered elemen
        while (oldHoverNodeInCurDoc && oldHoverNodeInCurDoc->isFrameOwnerElement()) {
            LocalFrame* newHoverFrame = nullptr;
            // If we can't get the frame from the new hover frame chain,
            // the newHoverFrame will be null and the old hover state will be cleared.
            if (indexFrameChain > 0)
                newHoverFrame = newHoverFrameChain[--indexFrameChain];

            HTMLFrameOwnerElement* owner = toHTMLFrameOwnerElement(oldHoverNodeInCurDoc.get());
            if (!owner->contentFrame() || !owner->contentFrame()->isLocalFrame())
                break;

            LocalFrame* oldHoverFrame = toLocalFrame(owner->contentFrame());
            Document* doc = oldHoverFrame->document();
            if (!doc)
                break;

            oldHoverNodeInCurDoc = doc->hoverNode();
            // If the old hovered frame is different from the new hovered frame.
            // we should clear the old hovered node from the old hovered frame.
            if (newHoverFrame != oldHoverFrame)
                doc->updateHoverActiveState(request, nullptr);
        }
    }

    // Recursively set the new active/hover states on every frame in the chain of innerElement.
    m_frame->document()->updateHoverActiveState(request, innerElement);
}

GestureEventWithHitTestResults EventHandler::targetGestureEvent(const PlatformGestureEvent& gestureEvent, bool readOnly)
{
    TRACE_EVENT0("input", "EventHandler::targetGestureEvent");

    ASSERT(m_frame == m_frame->localFrameRoot());
    // Scrolling events get hit tested per frame (like wheel events do).
    ASSERT(!gestureEvent.isScrollEvent());

    HitTestRequest::HitTestRequestType hitType = getHitTypeForGestureType(gestureEvent.type());
    double activeInterval = 0;
    bool shouldKeepActiveForMinInterval = false;
    if (readOnly) {
        hitType |= HitTestRequest::ReadOnly;
    } else if (gestureEvent.type() == PlatformEvent::GestureTap) {
        // If the Tap is received very shortly after ShowPress, we want to
        // delay clearing of the active state so that it's visible to the user
        // for at least a couple of frames.
        activeInterval = WTF::currentTime() - m_lastShowPressTimestamp;
        shouldKeepActiveForMinInterval = m_lastShowPressTimestamp && activeInterval < minimumActiveInterval;
        if (shouldKeepActiveForMinInterval)
            hitType |= HitTestRequest::ReadOnly;
    }

    GestureEventWithHitTestResults eventWithHitTestResults = hitTestResultForGestureEvent(gestureEvent, hitType);
    // Now apply hover/active state to the final target.
    HitTestRequest request(hitType | HitTestRequest::AllowChildFrameContent);
    if (!request.readOnly())
        updateGestureHoverActiveState(request, eventWithHitTestResults.hitTestResult().innerElement());

    if (shouldKeepActiveForMinInterval) {
        m_lastDeferredTapElement = eventWithHitTestResults.hitTestResult().innerElement();
        m_activeIntervalTimer.startOneShot(minimumActiveInterval - activeInterval, FROM_HERE);
    }

    return eventWithHitTestResults;
}

GestureEventWithHitTestResults EventHandler::hitTestResultForGestureEvent(const PlatformGestureEvent& gestureEvent, HitTestRequest::HitTestRequestType hitType)
{
    // Perform the rect-based hit-test (or point-based if adjustment is disabled). Note that
    // we don't yet apply hover/active state here because we need to resolve touch adjustment
    // first so that we apply hover/active it to the final adjusted node.
    IntPoint hitTestPoint = m_frame->view()->rootFrameToContents(gestureEvent.position());
    LayoutSize padding;
    if (shouldApplyTouchAdjustment(gestureEvent)) {
        padding = LayoutSize(gestureEvent.area());
        if (!padding.isEmpty()) {
            padding.scale(1.f / 2);
            hitType |= HitTestRequest::ListBased;
        }
    }
    HitTestResult hitTestResult = hitTestResultAtPoint(hitTestPoint, hitType | HitTestRequest::ReadOnly, padding);

    // Adjust the location of the gesture to the most likely nearby node, as appropriate for the
    // type of event.
    PlatformGestureEvent adjustedEvent = gestureEvent;
    applyTouchAdjustment(&adjustedEvent, &hitTestResult);

    // Do a new hit-test at the (adjusted) gesture co-ordinates. This is necessary because
    // rect-based hit testing and touch adjustment sometimes return a different node than
    // what a point-based hit test would return for the same point.
    // FIXME: Fix touch adjustment to avoid the need for a redundant hit test. http://crbug.com/398914
    if (shouldApplyTouchAdjustment(gestureEvent)) {
        LocalFrame* hitFrame = hitTestResult.innerNodeFrame();
        if (!hitFrame)
            hitFrame = m_frame;
        hitTestResult = hitTestResultInFrame(hitFrame, hitFrame->view()->rootFrameToContents(adjustedEvent.position()), (hitType | HitTestRequest::ReadOnly) & ~HitTestRequest::ListBased);
    }

    // If we did a rect-based hit test it must be resolved to the best single node by now to
    // ensure consumers don't accidentally use one of the other candidates.
    ASSERT(!hitTestResult.isRectBasedTest());

    return GestureEventWithHitTestResults(adjustedEvent, hitTestResult);
}

HitTestRequest::HitTestRequestType EventHandler::getHitTypeForGestureType(PlatformEvent::Type type)
{
    HitTestRequest::HitTestRequestType hitType = HitTestRequest::TouchEvent;
    switch (type) {
    case PlatformEvent::GestureShowPress:
    case PlatformEvent::GestureTapUnconfirmed:
        return hitType | HitTestRequest::Active;
    case PlatformEvent::GestureTapDownCancel:
        // A TapDownCancel received when no element is active shouldn't really be changing hover state.
        if (!m_frame->document()->activeHoverElement())
            hitType |= HitTestRequest::ReadOnly;
        return hitType | HitTestRequest::Release;
    case PlatformEvent::GestureTap:
        return hitType | HitTestRequest::Release;
    case PlatformEvent::GestureTapDown:
    case PlatformEvent::GestureLongPress:
    case PlatformEvent::GestureLongTap:
    case PlatformEvent::GestureTwoFingerTap:
        // FIXME: Shouldn't LongTap and TwoFingerTap clear the Active state?
        return hitType | HitTestRequest::Active | HitTestRequest::ReadOnly;
    default:
        ASSERT_NOT_REACHED();
        return hitType | HitTestRequest::Active | HitTestRequest::ReadOnly;
    }
}

void EventHandler::applyTouchAdjustment(PlatformGestureEvent* gestureEvent, HitTestResult* hitTestResult)
{
    if (!shouldApplyTouchAdjustment(*gestureEvent))
        return;

    Node* adjustedNode = nullptr;
    IntPoint adjustedPoint = gestureEvent->position();
    bool adjusted = false;
    switch (gestureEvent->type()) {
    case PlatformEvent::GestureTap:
    case PlatformEvent::GestureTapUnconfirmed:
    case PlatformEvent::GestureTapDown:
    case PlatformEvent::GestureShowPress:
        adjusted = bestClickableNodeForHitTestResult(*hitTestResult, adjustedPoint, adjustedNode);
        break;
    case PlatformEvent::GestureLongPress:
    case PlatformEvent::GestureLongTap:
    case PlatformEvent::GestureTwoFingerTap:
        adjusted = bestContextMenuNodeForHitTestResult(*hitTestResult, adjustedPoint, adjustedNode);
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    // Update the hit-test result to be a point-based result instead of a rect-based result.
    // FIXME: We should do this even when no candidate matches the node filter. crbug.com/398914
    if (adjusted) {
        hitTestResult->resolveRectBasedTest(adjustedNode, m_frame->view()->rootFrameToContents(adjustedPoint));
        gestureEvent->applyTouchAdjustment(adjustedPoint);
    }
}

bool EventHandler::sendContextMenuEvent(const PlatformMouseEvent& event, Node* overrideTargetNode)
{
    FrameView* v = m_frame->view();
    if (!v)
        return false;

    // Clear mouse press state to avoid initiating a drag while context menu is up.
    m_mousePressed = false;
    LayoutPoint positionInContents = v->rootFrameToContents(event.position());
    HitTestRequest request(HitTestRequest::Active);
    MouseEventWithHitTestResults mev = m_frame->document()->prepareMouseEvent(request, positionInContents, event);

    selectionController().sendContextMenuEvent(mev, positionInContents);

    Node* targetNode = overrideTargetNode ? overrideTargetNode : mev.innerNode();
    return !dispatchMouseEvent(EventTypeNames::contextmenu, targetNode, 0, event, false);
}

bool EventHandler::sendContextMenuEventForKey(Element* overrideTargetElement)
{
    FrameView* view = m_frame->view();
    if (!view)
        return false;

    Document* doc = m_frame->document();
    if (!doc)
        return false;

    // Clear mouse press state to avoid initiating a drag while context menu is up.
    m_mousePressed = false;

    static const int kContextMenuMargin = 1;

#if OS(WIN)
    int rightAligned = ::GetSystemMetrics(SM_MENUDROPALIGNMENT);
#else
    int rightAligned = 0;
#endif
    IntPoint locationInRootFrame;

    Element* focusedElement = overrideTargetElement ? overrideTargetElement : doc->focusedElement();
    FrameSelection& selection = m_frame->selection();
    Position start = selection.selection().start();
    PinchViewport& pinchViewport = m_frame->page()->frameHost().pinchViewport();

    if (!overrideTargetElement && start.deprecatedNode() && (selection.rootEditableElement() || selection.isRange())) {
        RefPtrWillBeRawPtr<Range> selectionRange = selection.toNormalizedRange();
        IntRect firstRect = m_frame->editor().firstRectForRange(selectionRange.get());

        int x = rightAligned ? firstRect.maxX() : firstRect.x();
        // In a multiline edit, firstRect.maxY() would endup on the next line, so -1.
        int y = firstRect.maxY() ? firstRect.maxY() - 1 : 0;
        locationInRootFrame = view->contentsToRootFrame(IntPoint(x, y));
    } else if (focusedElement) {
        IntRect clippedRect = focusedElement->boundsInViewportSpace();
        // FIXME: boundsInViewportSpace is actually in the weird scaled but untranslated coordinate space of
        // the old-style pinch viewport. crbug.com/459591.
        locationInRootFrame = flooredIntPoint(pinchViewport.viewportCSSPixelsToRootFrame(clippedRect.center()));
    } else {
        locationInRootFrame = IntPoint(
            rightAligned
                ? pinchViewport.visibleRect().maxX() - kContextMenuMargin
                : pinchViewport.location().x() + kContextMenuMargin,
            pinchViewport.location().y() + kContextMenuMargin);
    }

    m_frame->view()->setCursor(pointerCursor());
    IntPoint locationInViewport = pinchViewport.rootFrameToViewport(locationInRootFrame);
    IntPoint globalPosition = view->hostWindow()->viewportToScreen(IntRect(locationInViewport, IntSize())).location();

    Node* targetNode = overrideTargetElement ? overrideTargetElement : doc->focusedElement();
    if (!targetNode)
        targetNode = doc;

    // Use the focused node as the target for hover and active.
    HitTestRequest request(HitTestRequest::Active);
    HitTestResult result(request, locationInRootFrame);
    result.setInnerNode(targetNode);
    doc->updateHoverActiveState(request, result.innerElement());

    // The contextmenu event is a mouse event even when invoked using the keyboard.
    // This is required for web compatibility.
    PlatformEvent::Type eventType = PlatformEvent::MousePressed;
    if (m_frame->settings()->showContextMenuOnMouseUp())
        eventType = PlatformEvent::MouseReleased;

    PlatformMouseEvent mouseEvent(locationInRootFrame, globalPosition, RightButton, eventType, 1, false, false, false, false, PlatformMouseEvent::RealOrIndistinguishable, WTF::currentTime());

    handleMousePressEvent(mouseEvent);
    return sendContextMenuEvent(mouseEvent, overrideTargetElement);
}

bool EventHandler::sendContextMenuEventForGesture(const GestureEventWithHitTestResults& targetedEvent)
{
    const PlatformGestureEvent& gestureEvent = targetedEvent.event();
    unsigned modifiers = gestureEvent.modifiers();

    // Send MouseMoved event prior to handling (https://crbug.com/485290).
    PlatformMouseEvent fakeMouseMove(gestureEvent.position(), gestureEvent.globalPosition(),
        NoButton, PlatformEvent::MouseMoved, /* clickCount */ 0,
        static_cast<PlatformEvent::Modifiers>(modifiers),
        PlatformMouseEvent::FromTouch, gestureEvent.timestamp());
    dispatchMouseEvent(EventTypeNames::mousemove, targetedEvent.hitTestResult().innerNode(), 0, fakeMouseMove, true);

    PlatformEvent::Type eventType = PlatformEvent::MousePressed;

    if (m_frame->settings()->showContextMenuOnMouseUp())
        eventType = PlatformEvent::MouseReleased;
    else
        modifiers |= PlatformEvent::RightButtonDown;

    PlatformMouseEvent mouseEvent(targetedEvent.event().position(), targetedEvent.event().globalPosition(), RightButton, eventType, 1,
        static_cast<PlatformEvent::Modifiers>(modifiers),
        PlatformMouseEvent::FromTouch, WTF::currentTime());
    // To simulate right-click behavior, we send a right mouse down and then
    // context menu event.
    // FIXME: Send HitTestResults to avoid redundant hit tests.
    handleMousePressEvent(mouseEvent);
    return sendContextMenuEvent(mouseEvent);
    // We do not need to send a corresponding mouse release because in case of
    // right-click, the context menu takes capture and consumes all events.
}

void EventHandler::scheduleHoverStateUpdate()
{
    if (!m_hoverTimer.isActive())
        m_hoverTimer.startOneShot(0, FROM_HERE);
}

void EventHandler::scheduleCursorUpdate()
{
    // We only want one timer for the page, rather than each frame having it's own timer
    // competing which eachother (since there's only one mouse cursor).
    ASSERT(m_frame == m_frame->localFrameRoot());

    if (!m_cursorUpdateTimer.isActive())
        m_cursorUpdateTimer.startOneShot(cursorUpdateInterval, FROM_HERE);
}

bool EventHandler::cursorUpdatePending()
{
    return m_cursorUpdateTimer.isActive();
}

void EventHandler::dispatchFakeMouseMoveEventSoon()
{
    if (m_mousePressed)
        return;

    if (m_mousePositionIsUnknown)
        return;

    Settings* settings = m_frame->settings();
    if (settings && !settings->deviceSupportsMouse())
        return;

    // Reschedule the timer, to prevent dispatching mouse move events
    // during a scroll. This avoids a potential source of scroll jank.
    if (m_fakeMouseMoveEventTimer.isActive())
        m_fakeMouseMoveEventTimer.stop();
    m_fakeMouseMoveEventTimer.startOneShot(fakeMouseMoveInterval, FROM_HERE);
}

void EventHandler::dispatchFakeMouseMoveEventSoonInQuad(const FloatQuad& quad)
{
    FrameView* view = m_frame->view();
    if (!view)
        return;

    if (!quad.containsPoint(view->rootFrameToContents(m_lastKnownMousePosition)))
        return;

    dispatchFakeMouseMoveEventSoon();
}

void EventHandler::fakeMouseMoveEventTimerFired(Timer<EventHandler>* timer)
{
    TRACE_EVENT0("input", "EventHandler::fakeMouseMoveEventTimerFired");
    ASSERT_UNUSED(timer, timer == &m_fakeMouseMoveEventTimer);
    ASSERT(!m_mousePressed);

    Settings* settings = m_frame->settings();
    if (settings && !settings->deviceSupportsMouse())
        return;

    FrameView* view = m_frame->view();
    if (!view)
        return;

    if (!m_frame->page() || !m_frame->page()->focusController().isActive())
        return;

    // Don't dispatch a synthetic mouse move event if the mouse cursor is not visible to the user.
    if (!isCursorVisible())
        return;

    bool shiftKey;
    bool ctrlKey;
    bool altKey;
    bool metaKey;
    PlatformKeyboardEvent::getCurrentModifierState(shiftKey, ctrlKey, altKey, metaKey);
    PlatformMouseEvent fakeMouseMoveEvent(m_lastKnownMousePosition, m_lastKnownMouseGlobalPosition, NoButton, PlatformEvent::MouseMoved, 0, shiftKey, ctrlKey, altKey, metaKey, PlatformMouseEvent::RealOrIndistinguishable, currentTime());
    handleMouseMoveEvent(fakeMouseMoveEvent);
}

void EventHandler::cancelFakeMouseMoveEvent()
{
    m_fakeMouseMoveEventTimer.stop();
}

bool EventHandler::isCursorVisible() const
{
    return m_frame->page()->isCursorVisible();
}

void EventHandler::setResizingFrameSet(HTMLFrameSetElement* frameSet)
{
    m_frameSetBeingResized = frameSet;
}

void EventHandler::resizeScrollableAreaDestroyed()
{
    ASSERT(m_resizeScrollableArea);
    m_resizeScrollableArea = nullptr;
}

void EventHandler::hoverTimerFired(Timer<EventHandler>*)
{
    TRACE_EVENT0("input", "EventHandler::hoverTimerFired");
    m_hoverTimer.stop();

    ASSERT(m_frame);
    ASSERT(m_frame->document());

    if (LayoutView* layoutObject = m_frame->contentLayoutObject()) {
        if (FrameView* view = m_frame->view()) {
            HitTestRequest request(HitTestRequest::Move);
            HitTestResult result(request, view->rootFrameToContents(m_lastKnownMousePosition));
            layoutObject->hitTest(result);
            m_frame->document()->updateHoverActiveState(request, result.innerElement());
        }
    }
}

void EventHandler::activeIntervalTimerFired(Timer<EventHandler>*)
{
    TRACE_EVENT0("input", "EventHandler::activeIntervalTimerFired");
    m_activeIntervalTimer.stop();

    if (m_frame
        && m_frame->document()
        && m_lastDeferredTapElement) {
        // FIXME: Enable condition when http://crbug.com/226842 lands
        // m_lastDeferredTapElement.get() == m_frame->document()->activeElement()
        HitTestRequest request(HitTestRequest::TouchEvent | HitTestRequest::Release);
        m_frame->document()->updateHoverActiveState(request, m_lastDeferredTapElement.get());
    }
    m_lastDeferredTapElement = nullptr;
}

void EventHandler::notifyElementActivated()
{
    // Since another element has been set to active, stop current timer and clear reference.
    if (m_activeIntervalTimer.isActive())
        m_activeIntervalTimer.stop();
    m_lastDeferredTapElement = nullptr;
}

bool EventHandler::handleAccessKey(const PlatformKeyboardEvent& evt)
{
    // FIXME: Ignoring the state of Shift key is what neither IE nor Firefox do.
    // IE matches lower and upper case access keys regardless of Shift key state - but if both upper and
    // lower case variants are present in a document, the correct element is matched based on Shift key state.
    // Firefox only matches an access key if Shift is not pressed, and does that case-insensitively.
    ASSERT(!(accessKeyModifiers() & PlatformEvent::ShiftKey));
    if ((evt.modifiers() & ~PlatformEvent::ShiftKey) != accessKeyModifiers())
        return false;
    String key = evt.unmodifiedText();
    Element* elem = m_frame->document()->getElementByAccessKey(key.lower());
    if (!elem)
        return false;
    elem->accessKeyAction(false);
    return true;
}

bool EventHandler::keyEvent(const PlatformKeyboardEvent& initialKeyEvent)
{
    RefPtrWillBeRawPtr<FrameView> protector(m_frame->view());

    if (initialKeyEvent.windowsVirtualKeyCode() == VK_CAPITAL)
        capsLockStateMayHaveChanged();

#if OS(WIN)
    if (panScrollInProgress()) {
        // If a key is pressed while the panScroll is in progress then we want to stop
        if (initialKeyEvent.type() == PlatformEvent::KeyDown || initialKeyEvent.type() == PlatformEvent::RawKeyDown)
            stopAutoscroll();

        // If we were in panscroll mode, we swallow the key event
        return true;
    }
#endif

    // Check for cases where we are too early for events -- possible unmatched key up
    // from pressing return in the location bar.
    RefPtrWillBeRawPtr<Node> node = eventTargetNodeForDocument(m_frame->document());
    if (!node)
        return false;

    UserGestureIndicator gestureIndicator(DefinitelyProcessingUserGesture);

    // In IE, access keys are special, they are handled after default keydown processing, but cannot be canceled - this is hard to match.
    // On Mac OS X, we process them before dispatching keydown, as the default keydown handler implements Emacs key bindings, which may conflict
    // with access keys. Then we dispatch keydown, but suppress its default handling.
    // On Windows, WebKit explicitly calls handleAccessKey() instead of dispatching a keypress event for WM_SYSCHAR messages.
    // Other platforms currently match either Mac or Windows behavior, depending on whether they send combined KeyDown events.
    bool matchedAnAccessKey = false;
    if (initialKeyEvent.type() == PlatformEvent::KeyDown)
        matchedAnAccessKey = handleAccessKey(initialKeyEvent);

    // FIXME: it would be fair to let an input method handle KeyUp events before DOM dispatch.
    if (initialKeyEvent.type() == PlatformEvent::KeyUp || initialKeyEvent.type() == PlatformEvent::Char)
        return !node->dispatchKeyEvent(initialKeyEvent);

    PlatformKeyboardEvent keyDownEvent = initialKeyEvent;
    if (keyDownEvent.type() != PlatformEvent::RawKeyDown)
        keyDownEvent.disambiguateKeyDownEvent(PlatformEvent::RawKeyDown);
    RefPtrWillBeRawPtr<KeyboardEvent> keydown = KeyboardEvent::create(keyDownEvent, m_frame->document()->domWindow());
    if (matchedAnAccessKey)
        keydown->setDefaultPrevented(true);
    keydown->setTarget(node);

    if (initialKeyEvent.type() == PlatformEvent::RawKeyDown) {
        node->dispatchEvent(keydown, IGNORE_EXCEPTION);
        // If frame changed as a result of keydown dispatch, then return true to avoid sending a subsequent keypress message to the new frame.
        bool changedFocusedFrame = m_frame->page() && m_frame != m_frame->page()->focusController().focusedOrMainFrame();
        return keydown->defaultHandled() || keydown->defaultPrevented() || changedFocusedFrame;
    }

    node->dispatchEvent(keydown, IGNORE_EXCEPTION);
    // If frame changed as a result of keydown dispatch, then return early to avoid sending a subsequent keypress message to the new frame.
    bool changedFocusedFrame = m_frame->page() && m_frame != m_frame->page()->focusController().focusedOrMainFrame();
    bool keydownResult = keydown->defaultHandled() || keydown->defaultPrevented() || changedFocusedFrame;
    if (keydownResult)
        return keydownResult;

    // Focus may have changed during keydown handling, so refetch node.
    // But if we are dispatching a fake backward compatibility keypress, then we pretend that the keypress happened on the original node.
    node = eventTargetNodeForDocument(m_frame->document());
    if (!node)
        return false;

    PlatformKeyboardEvent keyPressEvent = initialKeyEvent;
    keyPressEvent.disambiguateKeyDownEvent(PlatformEvent::Char);
    if (keyPressEvent.text().isEmpty())
        return keydownResult;
    RefPtrWillBeRawPtr<KeyboardEvent> keypress = KeyboardEvent::create(keyPressEvent, m_frame->document()->domWindow());
    keypress->setTarget(node);
    if (keydownResult)
        keypress->setDefaultPrevented(true);
    node->dispatchEvent(keypress, IGNORE_EXCEPTION);

    return keydownResult || keypress->defaultPrevented() || keypress->defaultHandled();
}

static WebFocusType focusDirectionForKey(const AtomicString& keyIdentifier)
{
    DEFINE_STATIC_LOCAL(AtomicString, Down, ("Down", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(AtomicString, Up, ("Up", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(AtomicString, Left, ("Left", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(AtomicString, Right, ("Right", AtomicString::ConstructFromLiteral));

    WebFocusType retVal = WebFocusTypeNone;

    if (keyIdentifier == Down)
        retVal = WebFocusTypeDown;
    else if (keyIdentifier == Up)
        retVal = WebFocusTypeUp;
    else if (keyIdentifier == Left)
        retVal = WebFocusTypeLeft;
    else if (keyIdentifier == Right)
        retVal = WebFocusTypeRight;

    return retVal;
}

void EventHandler::defaultKeyboardEventHandler(KeyboardEvent* event)
{
    if (event->type() == EventTypeNames::keydown) {
        // Clear caret blinking suspended state to make sure that caret blinks
        // when we type again after long pressing on an empty input field.
        if (m_frame && m_frame->selection().isCaretBlinkingSuspended())
            m_frame->selection().setCaretBlinkingSuspended(false);

        m_frame->editor().handleKeyboardEvent(event);
        if (event->defaultHandled())
            return;
        if (event->keyIdentifier() == "U+0009") {
            defaultTabEventHandler(event);
        } else if (event->keyIdentifier() == "U+0008") {
            defaultBackspaceEventHandler(event);
        } else if (event->keyIdentifier() == "U+001B") {
            defaultEscapeEventHandler(event);
        } else {
            WebFocusType type = focusDirectionForKey(AtomicString(event->keyIdentifier()));
            if (type != WebFocusTypeNone)
                defaultArrowEventHandler(type, event);
        }
    }
    if (event->type() == EventTypeNames::keypress) {
        m_frame->editor().handleKeyboardEvent(event);
        if (event->defaultHandled())
            return;
        if (event->charCode() == ' ')
            defaultSpaceEventHandler(event);
    }
}

bool EventHandler::dragHysteresisExceeded(const IntPoint& dragLocationInRootFrame) const
{
    FrameView* view = m_frame->view();
    if (!view)
        return false;
    IntPoint dragLocation = view->rootFrameToContents(dragLocationInRootFrame);
    IntSize delta = dragLocation - m_mouseDownPos;

    int threshold = GeneralDragHysteresis;
    switch (dragState().m_dragType) {
    case DragSourceActionSelection:
        threshold = TextDragHysteresis;
        break;
    case DragSourceActionImage:
        threshold = ImageDragHysteresis;
        break;
    case DragSourceActionLink:
        threshold = LinkDragHysteresis;
        break;
    case DragSourceActionDHTML:
        break;
    case DragSourceActionNone:
        ASSERT_NOT_REACHED();
    }

    return abs(delta.width()) >= threshold || abs(delta.height()) >= threshold;
}

void EventHandler::clearDragDataTransfer()
{
    if (dragState().m_dragDataTransfer) {
        dragState().m_dragDataTransfer->clearDragImage();
        dragState().m_dragDataTransfer->setAccessPolicy(DataTransferNumb);
    }
}

void EventHandler::dragSourceEndedAt(const PlatformMouseEvent& event, DragOperation operation)
{
    // Send a hit test request so that Layer gets a chance to update the :hover and :active pseudoclasses.
    HitTestRequest request(HitTestRequest::Release);
    prepareMouseEvent(request, event);

    if (dragState().m_dragSrc) {
        dragState().m_dragDataTransfer->setDestinationOperation(operation);
        // for now we don't care if event handler cancels default behavior, since there is none
        dispatchDragSrcEvent(EventTypeNames::dragend, event);
    }
    clearDragDataTransfer();
    dragState().m_dragSrc = nullptr;
    // In case the drag was ended due to an escape key press we need to ensure
    // that consecutive mousemove events don't reinitiate the drag and drop.
    m_mouseDownMayStartDrag = false;
}

void EventHandler::updateDragStateAfterEditDragIfNeeded(Element* rootEditableElement)
{
    // If inserting the dragged contents removed the drag source, we still want to fire dragend at the root editble element.
    if (dragState().m_dragSrc && !dragState().m_dragSrc->inDocument())
        dragState().m_dragSrc = rootEditableElement;
}

// returns if we should continue "default processing", i.e., whether eventhandler canceled
bool EventHandler::dispatchDragSrcEvent(const AtomicString& eventType, const PlatformMouseEvent& event)
{
    return !dispatchDragEvent(eventType, dragState().m_dragSrc.get(), event, dragState().m_dragDataTransfer.get());
}

bool EventHandler::handleDrag(const MouseEventWithHitTestResults& event, DragInitiator initiator)
{
    ASSERT(event.event().type() == PlatformEvent::MouseMoved);
    // Callers must protect the reference to FrameView, since this function may dispatch DOM
    // events, causing page/FrameView to go away.
    ASSERT(m_frame);
    ASSERT(m_frame->view());
    if (!m_frame->page())
        return false;

    if (m_mouseDownMayStartDrag) {
        HitTestRequest request(HitTestRequest::ReadOnly);
        HitTestResult result(request, m_mouseDownPos);
        m_frame->contentLayoutObject()->hitTest(result);
        Node* node = result.innerNode();
        if (node) {
            DragController::SelectionDragPolicy selectionDragPolicy = event.event().timestamp() - m_mouseDownTimestamp < TextDragDelay
                ? DragController::DelayedSelectionDragResolution
                : DragController::ImmediateSelectionDragResolution;
            dragState().m_dragSrc = m_frame->page()->dragController().draggableNode(m_frame, node, m_mouseDownPos, selectionDragPolicy, dragState().m_dragType);
        } else {
            dragState().m_dragSrc = nullptr;
        }

        if (!dragState().m_dragSrc)
            m_mouseDownMayStartDrag = false; // no element is draggable
    }

    if (!m_mouseDownMayStartDrag)
        return initiator == DragInitiator::Mouse && !selectionController().mouseDownMayStartSelect() && !m_mouseDownMayStartAutoscroll;

    // We are starting a text/image/url drag, so the cursor should be an arrow
    // FIXME <rdar://7577595>: Custom cursors aren't supported during drag and drop (default to pointer).
    m_frame->view()->setCursor(pointerCursor());

    if (initiator == DragInitiator::Mouse && !dragHysteresisExceeded(event.event().position()))
        return true;

    // Once we're past the hysteresis point, we don't want to treat this gesture as a click
    invalidateClick();

    if (!tryStartDrag(event)) {
        // Something failed to start the drag, clean up.
        clearDragDataTransfer();
        dragState().m_dragSrc = nullptr;
    }

    m_mouseDownMayStartDrag = false;
    // Whether or not the drag actually started, no more default handling (like selection).
    return true;
}

bool EventHandler::tryStartDrag(const MouseEventWithHitTestResults& event)
{
    // The DataTransfer would only be non-empty if we missed a dragEnd.
    // Clear it anyway, just to make sure it gets numbified.
    clearDragDataTransfer();

    dragState().m_dragDataTransfer = createDraggingDataTransfer();

    // Check to see if this a DOM based drag, if it is get the DOM specified drag
    // image and offset
    if (dragState().m_dragType == DragSourceActionDHTML) {
        if (LayoutObject* layoutObject = dragState().m_dragSrc->layoutObject()) {
            FloatPoint absPos = layoutObject->localToAbsolute(FloatPoint(), UseTransforms);
            IntSize delta = m_mouseDownPos - roundedIntPoint(absPos);
            dragState().m_dragDataTransfer->setDragImageElement(dragState().m_dragSrc.get(), IntPoint(delta));
        } else {
            // The layoutObject has disappeared, this can happen if the onStartDrag handler has hidden
            // the element in some way. In this case we just kill the drag.
            return false;
        }
    }

    DragController& dragController = m_frame->page()->dragController();
    if (!dragController.populateDragDataTransfer(m_frame, dragState(), m_mouseDownPos))
        return false;

    // If dispatching dragstart brings about another mouse down -- one way
    // this will happen is if a DevTools user breaks within a dragstart
    // handler and then clicks on the suspended page -- the drag state is
    // reset. Hence, need to check if this particular drag operation can
    // continue even if dispatchEvent() indicates no (direct) cancellation.
    // Do that by checking if m_dragSrc is still set.
    m_mouseDownMayStartDrag = dispatchDragSrcEvent(EventTypeNames::dragstart, m_mouseDown)
        && !m_frame->selection().isInPasswordField() && dragState().m_dragSrc;

    // Invalidate clipboard here against anymore pasteboard writing for security. The drag
    // image can still be changed as we drag, but not the pasteboard data.
    dragState().m_dragDataTransfer->setAccessPolicy(DataTransferImageWritable);

    if (m_mouseDownMayStartDrag) {
        // Dispatching the event could cause Page to go away. Make sure it's still valid before trying to use DragController.
        if (m_frame->page() && dragController.startDrag(m_frame, dragState(), event.event(), m_mouseDownPos))
            return true;
        // Drag was canned at the last minute - we owe m_dragSrc a DRAGEND event
        dispatchDragSrcEvent(EventTypeNames::dragend, event.event());
    }

    return false;
}

bool EventHandler::handleTextInputEvent(const String& text, Event* underlyingEvent, TextEventInputType inputType)
{
    // Platforms should differentiate real commands like selectAll from text input in disguise (like insertNewline),
    // and avoid dispatching text input events from keydown default handlers.
    ASSERT(!underlyingEvent || !underlyingEvent->isKeyboardEvent() || toKeyboardEvent(underlyingEvent)->type() == EventTypeNames::keypress);

    if (!m_frame)
        return false;

    EventTarget* target;
    if (underlyingEvent)
        target = underlyingEvent->target();
    else
        target = eventTargetNodeForDocument(m_frame->document());
    if (!target)
        return false;

    RefPtrWillBeRawPtr<TextEvent> event = TextEvent::create(m_frame->domWindow(), text, inputType);
    event->setUnderlyingEvent(underlyingEvent);

    target->dispatchEvent(event, IGNORE_EXCEPTION);
    return event->defaultHandled();
}

void EventHandler::defaultTextInputEventHandler(TextEvent* event)
{
    if (m_frame->editor().handleTextEvent(event))
        event->setDefaultHandled();
}

void EventHandler::defaultSpaceEventHandler(KeyboardEvent* event)
{
    ASSERT(event->type() == EventTypeNames::keypress);

    if (event->ctrlKey() || event->metaKey() || event->altKey())
        return;

    ScrollDirection direction = event->shiftKey() ? ScrollBlockDirectionBackward : ScrollBlockDirectionForward;

    // FIXME: enable scroll customization in this case. See crbug.com/410974.
    if (scroll(direction, ScrollByPage).didScroll) {
        event->setDefaultHandled();
        return;
    }

    FrameView* view = m_frame->view();
    if (!view)
        return;

    ScrollDirectionPhysical physicalDirection =
        toPhysicalDirection(direction, view->isVerticalDocument(), view->isFlippedDocument());

    if (view->scrollableArea()->userScroll(physicalDirection, ScrollByPage).didScroll)
        event->setDefaultHandled();
}

void EventHandler::defaultBackspaceEventHandler(KeyboardEvent* event)
{
    ASSERT(event->type() == EventTypeNames::keydown);

    if (event->ctrlKey() || event->metaKey() || event->altKey())
        return;

    if (!m_frame->editor().behavior().shouldNavigateBackOnBackspace())
        return;
    bool handledEvent = m_frame->loader().client()->navigateBackForward(event->shiftKey() ? 1 : -1);
    if (handledEvent)
        event->setDefaultHandled();
}

void EventHandler::defaultArrowEventHandler(WebFocusType focusType, KeyboardEvent* event)
{
    ASSERT(event->type() == EventTypeNames::keydown);

    if (event->ctrlKey() || event->metaKey() || event->shiftKey())
        return;

    Page* page = m_frame->page();
    if (!page)
        return;

    if (!isSpatialNavigationEnabled(m_frame))
        return;

    // Arrows and other possible directional navigation keys can be used in design
    // mode editing.
    if (m_frame->document()->inDesignMode())
        return;

    if (page->focusController().advanceFocus(focusType))
        event->setDefaultHandled();
}

void EventHandler::defaultTabEventHandler(KeyboardEvent* event)
{
    ASSERT(event->type() == EventTypeNames::keydown);

    // We should only advance focus on tabs if no special modifier keys are held down.
    if (event->ctrlKey() || event->metaKey())
        return;

#if !OS(MACOSX)
    // Option-Tab is a shortcut based on a system-wide preference on Mac but
    // should be ignored on all other platforms.
    if (event->altKey())
        return;
#endif

    Page* page = m_frame->page();
    if (!page)
        return;
    if (!page->tabKeyCyclesThroughElements())
        return;

    WebFocusType focusType = event->shiftKey() ? WebFocusTypeBackward : WebFocusTypeForward;

    // Tabs can be used in design mode editing.
    if (m_frame->document()->inDesignMode())
        return;

    if (page->focusController().advanceFocus(focusType))
        event->setDefaultHandled();
}

void EventHandler::defaultEscapeEventHandler(KeyboardEvent* event)
{
    if (HTMLDialogElement* dialog = m_frame->document()->activeModalDialog())
        dialog->dispatchEvent(Event::createCancelable(EventTypeNames::cancel));
}

void EventHandler::capsLockStateMayHaveChanged()
{
    if (Element* element = m_frame->document()->focusedElement()) {
        if (LayoutObject* r = element->layoutObject()) {
            if (r->isTextField())
                toLayoutTextControlSingleLine(r)->capsLockStateMayHaveChanged();
        }
    }
}

void EventHandler::setFrameWasScrolledByUser()
{
    if (FrameView* view = m_frame->view())
        view->setWasScrolledByUser(true);
}

bool EventHandler::passMousePressEventToScrollbar(MouseEventWithHitTestResults& mev)
{
    Scrollbar* scrollbar = mev.scrollbar();
    updateLastScrollbarUnderMouse(scrollbar, true);

    if (!scrollbar || !scrollbar->enabled())
        return false;
    setFrameWasScrolledByUser();
    scrollbar->mouseDown(mev.event());
    return true;
}

// If scrollbar (under mouse) is different from last, send a mouse exited. Set
// last to scrollbar if setLast is true; else set last to 0.
void EventHandler::updateLastScrollbarUnderMouse(Scrollbar* scrollbar, bool setLast)
{
    if (m_lastScrollbarUnderMouse != scrollbar) {
        // Send mouse exited to the old scrollbar.
        if (m_lastScrollbarUnderMouse)
            m_lastScrollbarUnderMouse->mouseExited();

        // Send mouse entered if we're setting a new scrollbar.
        if (scrollbar && setLast)
            scrollbar->mouseEntered();

        m_lastScrollbarUnderMouse = setLast ? scrollbar : nullptr;
    }
}

static const AtomicString& touchEventNameForTouchPointState(PlatformTouchPoint::State state)
{
    switch (state) {
    case PlatformTouchPoint::TouchReleased:
        return EventTypeNames::touchend;
    case PlatformTouchPoint::TouchCancelled:
        return EventTypeNames::touchcancel;
    case PlatformTouchPoint::TouchPressed:
        return EventTypeNames::touchstart;
    case PlatformTouchPoint::TouchMoved:
        return EventTypeNames::touchmove;
    case PlatformTouchPoint::TouchStationary:
        // Fall through to default
    default:
        ASSERT_NOT_REACHED();
        return emptyAtom;
    }
}

static const AtomicString& pointerEventNameForTouchPointState(PlatformTouchPoint::State state)
{
    switch (state) {
    case PlatformTouchPoint::TouchReleased:
        return EventTypeNames::pointerup;
    case PlatformTouchPoint::TouchCancelled:
        return EventTypeNames::pointercancel;
    case PlatformTouchPoint::TouchPressed:
        return EventTypeNames::pointerdown;
    case PlatformTouchPoint::TouchMoved:
        return EventTypeNames::pointermove;
    case PlatformTouchPoint::TouchStationary:
        // Fall through to default
    default:
        ASSERT_NOT_REACHED();
        return emptyAtom;
    }
}

HitTestResult EventHandler::hitTestResultInFrame(LocalFrame* frame, const LayoutPoint& point, HitTestRequest::HitTestRequestType hitType)
{
    HitTestResult result(HitTestRequest(hitType), point);

    if (!frame || !frame->contentLayoutObject())
        return result;
    if (frame->view()) {
        IntRect rect = frame->view()->visibleContentRect(IncludeScrollbars);
        if (!rect.contains(roundedIntPoint(point)))
            return result;
    }
    frame->contentLayoutObject()->hitTest(result);
    return result;
}

void EventHandler::dispatchPointerEventsForTouchEvent(const PlatformTouchEvent& event,
    WillBeHeapVector<TouchInfo>& touchInfos)
{
    const String& PointerTypeStrForTouch("touch");

    // Iterate through the touch points, sending PointerEvents to the targets as required.
    for (unsigned i = 0; i < touchInfos.size(); ++i) {
        TouchInfo& touchInfo = touchInfos[i];
        const PlatformTouchPoint& point = touchInfo.point;
        const unsigned& pointerId = point.id();
        const PlatformTouchPoint::State pointState = point.state();

        if (pointState == PlatformTouchPoint::TouchStationary || !touchInfo.knownTarget)
            continue;

        bool pointerReleasedOrCancelled = pointState == PlatformTouchPoint::TouchReleased
            || pointState == PlatformTouchPoint::TouchCancelled;

        if (pointState == PlatformTouchPoint::TouchPressed)
            m_pointerIdManager.add(PointerIdManager::PointerTypeTouch, pointerId);

        const AtomicString& eventName = pointerEventNameForTouchPointState(pointState);

        bool isEnterOrLeave = false;

        PointerEventInit pointerEventInit;
        pointerEventInit.setPointerId(pointerId);
        pointerEventInit.setWidth(touchInfo.adjustedRadius.width());
        pointerEventInit.setHeight(touchInfo.adjustedRadius.height());
        pointerEventInit.setPressure(point.force());
        pointerEventInit.setPointerType(PointerTypeStrForTouch);
        pointerEventInit.setIsPrimary(m_pointerIdManager.isPrimary(PointerIdManager::PointerTypeTouch, pointerId));
        pointerEventInit.setScreenX(point.screenPos().x());
        pointerEventInit.setScreenY(point.screenPos().y());
        pointerEventInit.setClientX(touchInfo.adjustedPagePoint.x());
        pointerEventInit.setClientY(touchInfo.adjustedPagePoint.y());
        pointerEventInit.setButton(0);
        pointerEventInit.setButtons(pointerReleasedOrCancelled ? 0 : 1);

        pointerEventInit.setCtrlKey(event.ctrlKey());
        pointerEventInit.setShiftKey(event.shiftKey());
        pointerEventInit.setAltKey(event.altKey());
        pointerEventInit.setMetaKey(event.metaKey());

        pointerEventInit.setBubbles(!isEnterOrLeave);
        pointerEventInit.setCancelable(!isEnterOrLeave && pointState != PlatformTouchPoint::TouchCancelled);

        RefPtrWillBeRawPtr<PointerEvent> pointerEvent = PointerEvent::create(eventName, pointerEventInit);
        touchInfo.touchTarget->toNode()->dispatchPointerEvent(pointerEvent.get());
        touchInfo.consumed = pointerEvent->defaultPrevented() || pointerEvent->defaultHandled();

        // Remove the released/cancelled id at the end to correctly determine primary id above.
        if (pointerReleasedOrCancelled)
            m_pointerIdManager.remove(PointerIdManager::PointerTypeTouch, pointerId);
    }
}

void EventHandler::sendPointerCancels(WillBeHeapVector<TouchInfo>& touchInfos)
{
    for (unsigned i = 0; i < touchInfos.size(); ++i) {
        TouchInfo& touchInfo = touchInfos[i];
        const PlatformTouchPoint& point = touchInfo.point;
        const unsigned& pointerId = point.id();
        const PlatformTouchPoint::State pointState = point.state();

        if (pointState == PlatformTouchPoint::TouchReleased
            || pointState == PlatformTouchPoint::TouchCancelled)
            continue;

        PointerEventInit pointerEventInit;
        pointerEventInit.setPointerId(pointerId);
        pointerEventInit.setBubbles(true);
        pointerEventInit.setCancelable(false);

        RefPtrWillBeRawPtr<PointerEvent> pointerEvent = PointerEvent::create(
            EventTypeNames::pointercancel, pointerEventInit);
        touchInfo.touchTarget->toNode()->dispatchPointerEvent(pointerEvent.get());

        m_pointerIdManager.remove(PointerIdManager::PointerTypeTouch, pointerId);
    }
}

bool EventHandler::dispatchTouchEvents(const PlatformTouchEvent& event,
    WillBeHeapVector<TouchInfo>& touchInfos, bool freshTouchEvents, bool allTouchReleased)
{
    // Build up the lists to use for the 'touches', 'targetTouches' and
    // 'changedTouches' attributes in the JS event. See
    // http://www.w3.org/TR/touch-events/#touchevent-interface for how these
    // lists fit together.

    // Holds the complete set of touches on the screen.
    RefPtrWillBeRawPtr<TouchList> touches = TouchList::create();

    // A different view on the 'touches' list above, filtered and grouped by
    // event target. Used for the 'targetTouches' list in the JS event.
    using TargetTouchesHeapMap = WillBeHeapHashMap<EventTarget*, RefPtrWillBeMember<TouchList>>;
    TargetTouchesHeapMap touchesByTarget;

    // Array of touches per state, used to assemble the 'changedTouches' list.
    using EventTargetSet = WillBeHeapHashSet<RefPtrWillBeMember<EventTarget>>;
    struct {
        // The touches corresponding to the particular change state this struct
        // instance represents.
        RefPtrWillBeMember<TouchList> m_touches;
        // Set of targets involved in m_touches.
        EventTargetSet m_targets;
    } changedTouches[PlatformTouchPoint::TouchStateEnd];

    for (unsigned i = 0; i < touchInfos.size(); ++i) {
        const TouchInfo& touchInfo = touchInfos[i];
        const PlatformTouchPoint& point = touchInfo.point;
        PlatformTouchPoint::State pointState = point.state();

        if (touchInfo.consumed)
            continue;

        RefPtrWillBeRawPtr<Touch> touch = Touch::create(
            touchInfo.targetFrame.get(),
            touchInfo.touchTarget.get(),
            point.id(),
            point.screenPos(),
            touchInfo.adjustedPagePoint,
            touchInfo.adjustedRadius,
            point.rotationAngle(),
            point.force());

        // Ensure this target's touch list exists, even if it ends up empty, so
        // it can always be passed to TouchEvent::Create below.
        TargetTouchesHeapMap::iterator targetTouchesIterator = touchesByTarget.find(touchInfo.touchTarget.get());
        if (targetTouchesIterator == touchesByTarget.end()) {
            touchesByTarget.set(touchInfo.touchTarget.get(), TouchList::create());
            targetTouchesIterator = touchesByTarget.find(touchInfo.touchTarget.get());
        }

        // touches and targetTouches should only contain information about
        // touches still on the screen, so if this point is released or
        // cancelled it will only appear in the changedTouches list.
        if (pointState != PlatformTouchPoint::TouchReleased && pointState != PlatformTouchPoint::TouchCancelled) {
            touches->append(touch);
            targetTouchesIterator->value->append(touch);
        }

        // Now build up the correct list for changedTouches.
        // Note that  any touches that are in the TouchStationary state (e.g. if
        // the user had several points touched but did not move them all) should
        // never be in the changedTouches list so we do not handle them
        // explicitly here. See https://bugs.webkit.org/show_bug.cgi?id=37609
        // for further discussion about the TouchStationary state.
        if (pointState != PlatformTouchPoint::TouchStationary && touchInfo.knownTarget) {
            ASSERT(pointState < PlatformTouchPoint::TouchStateEnd);
            if (!changedTouches[pointState].m_touches)
                changedTouches[pointState].m_touches = TouchList::create();
            changedTouches[pointState].m_touches->append(touch);
            changedTouches[pointState].m_targets.add(touchInfo.touchTarget);
        }
    }
    if (allTouchReleased) {
        m_touchSequenceDocument.clear();
        m_touchSequenceUserGestureToken.clear();
    }

    bool swallowedEvent = false;

    // Now iterate through the changedTouches list and m_targets within it, sending
    // TouchEvents to the targets as required.
    for (unsigned state = 0; state != PlatformTouchPoint::TouchStateEnd; ++state) {
        if (!changedTouches[state].m_touches)
            continue;

        const AtomicString& eventName(touchEventNameForTouchPointState(static_cast<PlatformTouchPoint::State>(state)));
        const EventTargetSet& targetsForState = changedTouches[state].m_targets;
        for (const RefPtrWillBeMember<EventTarget>& eventTarget : targetsForState) {
            EventTarget* touchEventTarget = eventTarget.get();
            RefPtrWillBeRawPtr<TouchEvent> touchEvent = TouchEvent::create(
                touches.get(), touchesByTarget.get(touchEventTarget), changedTouches[state].m_touches.get(),
                eventName, touchEventTarget->toNode()->document().domWindow(),
                event.ctrlKey(), event.altKey(), event.shiftKey(), event.metaKey(),
                event.cancelable(), event.causesScrollingIfUncanceled(), event.timestamp());

            touchEventTarget->toNode()->dispatchTouchEvent(touchEvent.get());
            swallowedEvent = swallowedEvent || touchEvent->defaultPrevented() || touchEvent->defaultHandled();
        }
    }

    return swallowedEvent;
}

bool EventHandler::handleTouchEvent(const PlatformTouchEvent& event)
{
    TRACE_EVENT0("blink", "EventHandler::handleTouchEvent");

    const Vector<PlatformTouchPoint>& points = event.touchPoints();

    bool freshTouchEvents = true;
    bool allTouchReleased = true;
    for (unsigned i = 0; i < points.size(); ++i) {
        const PlatformTouchPoint& point = points[i];

        if (point.state() != PlatformTouchPoint::TouchPressed)
            freshTouchEvents = false;
        if (point.state() != PlatformTouchPoint::TouchReleased && point.state() != PlatformTouchPoint::TouchCancelled)
            allTouchReleased = false;
    }
    if (freshTouchEvents) {
        // Ideally we'd ASSERT !m_touchSequenceDocument here since we should
        // have cleared the active document when we saw the last release. But we
        // have some tests that violate this, ClusterFuzz could trigger it, and
        // there may be cases where the browser doesn't reliably release all
        // touches. http://crbug.com/345372 tracks this.
        m_touchSequenceDocument.clear();
        m_touchSequenceUserGestureToken.clear();
    }

    OwnPtr<UserGestureIndicator> gestureIndicator;

    if (m_touchSequenceUserGestureToken)
        gestureIndicator = adoptPtr(new UserGestureIndicator(m_touchSequenceUserGestureToken.release()));
    else
        gestureIndicator = adoptPtr(new UserGestureIndicator(DefinitelyProcessingUserGesture));

    m_touchSequenceUserGestureToken = gestureIndicator->currentToken();

    ASSERT(m_frame->view());
    if (m_touchSequenceDocument && (!m_touchSequenceDocument->frame() || !m_touchSequenceDocument->frame()->view())) {
        // If the active touch document has no frame or view, it's probably being destroyed
        // so we can't dispatch events.
        return false;
    }

    // First do hit tests for any new touch points.
    for (unsigned i = 0; i < points.size(); ++i) {
        const PlatformTouchPoint& point = points[i];

        // Touch events implicitly capture to the touched node, and don't change
        // active/hover states themselves (Gesture events do). So we only need
        // to hit-test on touchstart, and it can be read-only.
        if (point.state() == PlatformTouchPoint::TouchPressed) {
            HitTestRequest::HitTestRequestType hitType = HitTestRequest::TouchEvent | HitTestRequest::ReadOnly | HitTestRequest::Active;
            LayoutPoint pagePoint = roundedLayoutPoint(m_frame->view()->rootFrameToContents(point.pos()));
            HitTestResult result;
            if (!m_touchSequenceDocument) {
                result = hitTestResultAtPoint(pagePoint, hitType);
            } else if (m_touchSequenceDocument->frame()) {
                LayoutPoint framePoint = roundedLayoutPoint(m_touchSequenceDocument->frame()->view()->rootFrameToContents(point.pos()));
                result = hitTestResultInFrame(m_touchSequenceDocument->frame(), framePoint, hitType);
            } else {
                continue;
            }

            Node* node = result.innerNode();
            if (!node)
                continue;

            // Touch events should not go to text nodes
            if (node->isTextNode())
                node = ComposedTreeTraversal::parent(*node);

            if (!m_touchSequenceDocument) {
                // Keep track of which document should receive all touch events
                // in the active sequence. This must be a single document to
                // ensure we don't leak Nodes between documents.
                m_touchSequenceDocument = &(result.innerNode()->document());
                ASSERT(m_touchSequenceDocument->frame()->view());
            }

            // Ideally we'd ASSERT(!m_targetForTouchID.contains(point.id())
            // since we shouldn't get a touchstart for a touch that's already
            // down. However EventSender allows this to be violated and there's
            // some tests that take advantage of it. There may also be edge
            // cases in the browser where this happens.
            // See http://crbug.com/345372.
            m_targetForTouchID.set(point.id(), node);

            TouchAction effectiveTouchAction = computeEffectiveTouchAction(*node);
            if (effectiveTouchAction != TouchActionAuto)
                m_frame->page()->chromeClient().setTouchAction(effectiveTouchAction);
        }
    }

    m_touchPressed = !allTouchReleased;

    // If there's no document receiving touch events, or no handlers on the
    // document set to receive the events, then we can skip all the rest of
    // this work.
    if (!m_touchSequenceDocument || !m_touchSequenceDocument->frameHost() || !m_touchSequenceDocument->frameHost()->eventHandlerRegistry().hasEventHandlers(EventHandlerRegistry::TouchEvent) || !m_touchSequenceDocument->frame()) {
        if (allTouchReleased) {
            m_touchSequenceDocument.clear();
            m_touchSequenceUserGestureToken.clear();
        }
        return false;
    }

    // Compute and store the common info used by both PointerEvent and TouchEvent.
    WillBeHeapVector<TouchInfo> touchInfos(points.size());

    for (unsigned i = 0; i < points.size(); ++i) {
        const PlatformTouchPoint& point = points[i];
        PlatformTouchPoint::State pointState = point.state();
        RefPtrWillBeRawPtr<EventTarget> touchTarget = nullptr;

        if (pointState == PlatformTouchPoint::TouchReleased || pointState == PlatformTouchPoint::TouchCancelled) {
            // The target should be the original target for this touch, so get
            // it from the hashmap. As it's a release or cancel we also remove
            // it from the map.
            touchTarget = m_targetForTouchID.take(point.id());
        } else {
            // No hittest is performed on move or stationary, since the target
            // is not allowed to change anyway.
            touchTarget = m_targetForTouchID.get(point.id());
        }

        LocalFrame* targetFrame = nullptr;
        bool knownTarget = false;
        if (touchTarget) {
            Document& doc = touchTarget->toNode()->document();
            // If the target node has moved to a new document while it was being touched,
            // we can't send events to the new document because that could leak nodes
            // from one document to another. See http://crbug.com/394339.
            if (&doc == m_touchSequenceDocument.get()) {
                targetFrame = doc.frame();
                knownTarget = true;
            }
        }
        if (!knownTarget) {
            // If we don't have a target registered for the point it means we've
            // missed our opportunity to do a hit test for it (due to some
            // optimization that prevented blink from ever seeing the
            // touchstart), or that the touch started outside the active touch
            // sequence document. We should still include the touch in the
            // Touches list reported to the application (eg. so it can
            // differentiate between a one and two finger gesture), but we won't
            // actually dispatch any events for it. Set the target to the
            // Document so that there's some valid node here. Perhaps this
            // should really be LocalDOMWindow, but in all other cases the target of
            // a Touch is a Node so using the window could be a breaking change.
            // Since we know there was no handler invoked, the specific target
            // should be completely irrelevant to the application.
            touchTarget = m_touchSequenceDocument;
            targetFrame = m_touchSequenceDocument->frame();
        }
        ASSERT(targetFrame);

        // pagePoint should always be in the target element's document coordinates.
        FloatPoint pagePoint = targetFrame->view()->rootFrameToContents(point.pos());
        float scaleFactor = 1.0f / targetFrame->pageZoomFactor();

        TouchInfo& touchInfo = touchInfos[i];
        touchInfo.point = point;
        touchInfo.touchTarget = touchTarget;
        touchInfo.targetFrame = targetFrame;
        touchInfo.adjustedPagePoint = pagePoint.scaledBy(scaleFactor);
        touchInfo.adjustedRadius = point.radius().scaledBy(scaleFactor);
        touchInfo.knownTarget = knownTarget;
        touchInfo.consumed = false;
    }

    if (RuntimeEnabledFeatures::pointerEventEnabled()) {
        if (!m_inPointerCanceledState) {
            dispatchPointerEventsForTouchEvent(event, touchInfos);
            // Note that the disposition of any pointer events affects only the generation of touch
            // events. If all pointer events were handled (and hence no touch events were fired), that
            // is still equivalent to the touch events going unhandled because pointer event handler
            // don't block scroll gesture generation.
        }
    }

    // TODO(crbug.com/507408): If PE handlers always call preventDefault, we won't see TEs until after
    // scrolling starts because the scrolling would suppress upcoming PEs. This sudden "break" in TE
    // suppression can make the visible TEs inconsistent (e.g. touchmove without a touchstart).

    bool swallowedTouchEvent = dispatchTouchEvents(event, touchInfos, freshTouchEvents,
        allTouchReleased);

    if (RuntimeEnabledFeatures::pointerEventEnabled()) {
        if (!m_inPointerCanceledState) {
            // Check if we need to stop firing pointer events because of a touch action.
            // See: www.w3.org/TR/pointerevents/#declaring-candidate-regions-for-default-touch-behaviors
            if (event.causesScrollingIfUncanceled() && !swallowedTouchEvent) {
                m_inPointerCanceledState = true;
                sendPointerCancels(touchInfos);
            }
        } else if (allTouchReleased) {
            m_inPointerCanceledState = false;
        }
    }

    return swallowedTouchEvent;
}

TouchAction EventHandler::intersectTouchAction(TouchAction action1, TouchAction action2)
{
    if (action1 == TouchActionNone || action2 == TouchActionNone)
        return TouchActionNone;
    if (action1 == TouchActionAuto)
        return action2;
    if (action2 == TouchActionAuto)
        return action1;
    if (!(action1 & action2))
        return TouchActionNone;
    return action1 & action2;
}

// touch-action applies to all elements with both width AND height properties.
// According to the CSS Box Model Spec (http://dev.w3.org/csswg/css-box/#the-width-and-height-properties)
// width applies to all elements but non-replaced inline elements, table rows, and row groups and
// height applies to all elements but non-replaced inline elements, table columns, and column groups.
static inline bool supportsTouchAction(const LayoutObject& object)
{
    if (object.isInline() && !object.isReplaced())
        return false;
    if (object.isTableRow() || object.isLayoutTableCol())
        return false;

    return true;
}

TouchAction EventHandler::computeEffectiveTouchAction(const Node& node)
{
    // Start by permitting all actions, then walk the elements supporting
    // touch-action from the target node up to the nearest scrollable ancestor
    // and exclude any prohibited actions.
    TouchAction effectiveTouchAction = TouchActionAuto;
    for (const Node* curNode = &node; curNode; curNode = ComposedTreeTraversal::parent(*curNode)) {
        if (LayoutObject* layoutObject = curNode->layoutObject()) {
            if (supportsTouchAction(*layoutObject)) {
                TouchAction action = layoutObject->style()->touchAction();
                effectiveTouchAction = intersectTouchAction(action, effectiveTouchAction);
                if (effectiveTouchAction == TouchActionNone)
                    break;
            }

            // If we've reached an ancestor that supports a touch action, search no further.
            if (layoutObject->isBox() && toLayoutBox(layoutObject)->scrollsOverflow())
                break;
        }
    }
    return effectiveTouchAction;
}

void EventHandler::setLastKnownMousePosition(const PlatformMouseEvent& event)
{
    m_mousePositionIsUnknown = false;
    m_lastKnownMousePosition = event.position();
    m_lastKnownMouseGlobalPosition = event.globalPosition();
}

bool EventHandler::passMousePressEventToSubframe(MouseEventWithHitTestResults& mev, LocalFrame* subframe)
{
    selectionController().passMousePressEventToSubframe(mev);
    subframe->eventHandler().handleMousePressEvent(mev.event());
    return true;
}

bool EventHandler::passMouseMoveEventToSubframe(MouseEventWithHitTestResults& mev, LocalFrame* subframe, HitTestResult* hoveredNode)
{
    if (m_mouseDownMayStartDrag)
        return false;
    subframe->eventHandler().handleMouseMoveOrLeaveEvent(mev.event(), hoveredNode);
    return true;
}

bool EventHandler::passMouseReleaseEventToSubframe(MouseEventWithHitTestResults& mev, LocalFrame* subframe)
{
    subframe->eventHandler().handleMouseReleaseEvent(mev.event());
    return true;
}

bool EventHandler::passWheelEventToWidget(const PlatformWheelEvent& wheelEvent, Widget& widget)
{
    // If not a FrameView, then probably a plugin widget.  Those will receive
    // the event via an EventTargetNode dispatch when this returns false.
    if (!widget.isFrameView())
        return false;

    return toFrameView(&widget)->frame().eventHandler().handleWheelEvent(wheelEvent);
}

DataTransfer* EventHandler::createDraggingDataTransfer() const
{
    return DataTransfer::create(DataTransfer::DragAndDrop, DataTransferWritable, DataObject::create());
}

void EventHandler::focusDocumentView()
{
    Page* page = m_frame->page();
    if (!page)
        return;
    page->focusController().focusDocumentView(m_frame);
}

unsigned EventHandler::accessKeyModifiers()
{
#if OS(MACOSX)
    return PlatformEvent::CtrlKey | PlatformEvent::AltKey;
#else
    return PlatformEvent::AltKey;
#endif
}

} // namespace blink
