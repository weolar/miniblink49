/*
 * Copyright (C) 2006, 2007, 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#ifndef EventHandler_h
#define EventHandler_h

#include "core/CoreExport.h"
#include "core/events/PointerIdManager.h"
#include "core/events/TextEventInputType.h"
#include "core/layout/HitTestRequest.h"
#include "core/page/DragActions.h"
#include "core/page/EventWithHitTestResults.h"
#include "core/style/ComputedStyleConstants.h"
#include "platform/Cursor.h"
#include "platform/PlatformMouseEvent.h"
#include "platform/PlatformTouchPoint.h"
#include "platform/Timer.h"
#include "platform/UserGestureIndicator.h"
#include "platform/geometry/LayoutPoint.h"
#include "platform/heap/Handle.h"
#include "platform/scroll/ScrollTypes.h"
#include "public/platform/WebFocusType.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/HashTraits.h"
#include "wtf/RefPtr.h"

namespace blink {

class AutoscrollController;
class DataTransfer;
class DeprecatedPaintLayer;
class DeprecatedPaintLayerScrollableArea;
class Document;
class DragState;
class Element;
class Event;
class EventTarget;
template <typename EventType>
class EventWithHitTestResults;
class FloatPoint;
class FloatQuad;
class HTMLFrameSetElement;
class HitTestRequest;
class HitTestResult;
class KeyboardEvent;
class LayoutObject;
class LocalFrame;
class Node;
class OptionalCursor;
class PlatformGestureEvent;
class PlatformKeyboardEvent;
class PlatformTouchEvent;
class PlatformWheelEvent;
class ScrollableArea;
class Scrollbar;
class ScrollState;
class SelectionController;
class TextEvent;
class WheelEvent;
class Widget;

enum class DragInitiator;

class CORE_EXPORT EventHandler : public NoBaseWillBeGarbageCollectedFinalized<EventHandler> {
    WTF_MAKE_NONCOPYABLE(EventHandler);
public:
    explicit EventHandler(LocalFrame*);
    ~EventHandler();
    DECLARE_TRACE();

    void clear();
    void nodeWillBeRemoved(Node&);

    void updateSelectionForMouseDrag();

#if OS(WIN)
    void startPanScrolling(LayoutObject*);
#endif

    void stopAutoscroll();

    void dispatchFakeMouseMoveEventSoon();
    void dispatchFakeMouseMoveEventSoonInQuad(const FloatQuad&);

    HitTestResult hitTestResultAtPoint(const LayoutPoint&,
        HitTestRequest::HitTestRequestType hitType = HitTestRequest::ReadOnly | HitTestRequest::Active,
        const LayoutSize& padding = LayoutSize());

    bool mousePressed() const { return m_mousePressed; }

    void setCapturingMouseEventsNode(PassRefPtrWillBeRawPtr<Node>); // A caller is responsible for resetting capturing node to 0.

    bool updateDragAndDrop(const PlatformMouseEvent&, DataTransfer*);
    void cancelDragAndDrop(const PlatformMouseEvent&, DataTransfer*);
    bool performDragAndDrop(const PlatformMouseEvent&, DataTransfer*);
    void updateDragStateAfterEditDragIfNeeded(Element* rootEditableElement);

    void scheduleHoverStateUpdate();
    void scheduleCursorUpdate();

    // Return whether a mouse cursor update is currently pending.  Used for testing.
    bool cursorUpdatePending();

    void setResizingFrameSet(HTMLFrameSetElement*);

    void resizeScrollableAreaDestroyed();

    IntPoint lastKnownMousePosition() const;

    // Attempts to scroll the DOM tree. If that fails, scrolls the view.
    // If the view can't be scrolled either, recursively bubble to the parent frame.
    bool bubblingScroll(ScrollDirection, ScrollGranularity, Node* startingNode = nullptr);

    bool handleMouseMoveEvent(const PlatformMouseEvent&);
    void handleMouseLeaveEvent(const PlatformMouseEvent&);

    bool handleMousePressEvent(const PlatformMouseEvent&);
    bool handleMouseReleaseEvent(const PlatformMouseEvent&);
    bool handleWheelEvent(const PlatformWheelEvent&);
    void defaultWheelEventHandler(Node*, WheelEvent*);

    // Called on the local root frame exactly once per gesture event.
    bool handleGestureEvent(const PlatformGestureEvent&);
    bool handleGestureEvent(const GestureEventWithHitTestResults&);

    // Clear the old hover/active state within frames before moving the hover state to the another frame
    void updateGestureHoverActiveState(const HitTestRequest&, Element*);

    // Hit-test the provided (non-scroll) gesture event, applying touch-adjustment and updating
    // hover/active state across all frames if necessary. This should be called at most once
    // per gesture event, and called on the local root frame.
    // Note: This is similar to (the less clearly named) prepareMouseEvent.
    // FIXME: Remove readOnly param when there is only ever a single call to this.
    GestureEventWithHitTestResults targetGestureEvent(const PlatformGestureEvent&, bool readOnly = false);
    GestureEventWithHitTestResults hitTestResultForGestureEvent(const PlatformGestureEvent&, HitTestRequest::HitTestRequestType);
    // Handle the provided non-scroll gesture event. Should be called only on the inner frame.
    bool handleGestureEventInFrame(const GestureEventWithHitTestResults&);

    // Handle the provided scroll gesture event, propagating down to child frames as necessary.
    bool handleGestureScrollEvent(const PlatformGestureEvent&);
    bool handleGestureScrollEnd(const PlatformGestureEvent&);
    bool isScrollbarHandlingGestures() const;

    bool bestClickableNodeForHitTestResult(const HitTestResult&, IntPoint& targetPoint, Node*& targetNode);
    bool bestContextMenuNodeForHitTestResult(const HitTestResult&, IntPoint& targetPoint, Node*& targetNode);
    // FIXME: This doesn't appear to be used outside tests anymore, what path are we using now and is it tested?
    bool bestZoomableAreaForTouchPoint(const IntPoint& touchCenter, const IntSize& touchRadius, IntRect& targetArea, Node*& targetNode);

    bool sendContextMenuEvent(const PlatformMouseEvent&, Node* overrideTargetNode = nullptr);
    bool sendContextMenuEventForKey(Element* overrideTargetElement = nullptr);
    bool sendContextMenuEventForGesture(const GestureEventWithHitTestResults&);

    void setMouseDownMayStartAutoscroll() { m_mouseDownMayStartAutoscroll = true; }

    static unsigned accessKeyModifiers();
    bool handleAccessKey(const PlatformKeyboardEvent&);
    bool keyEvent(const PlatformKeyboardEvent&);
    void defaultKeyboardEventHandler(KeyboardEvent*);

    bool handleTextInputEvent(const String& text, Event* underlyingEvent = nullptr, TextEventInputType = TextEventInputKeyboard);
    void defaultTextInputEventHandler(TextEvent*);

    void dragSourceEndedAt(const PlatformMouseEvent&, DragOperation);

    void focusDocumentView();

    void capsLockStateMayHaveChanged(); // Only called by FrameSelection

    bool handleTouchEvent(const PlatformTouchEvent&);

    bool useHandCursor(Node*, bool isOverLink);

    void notifyElementActivated();

    PassRefPtr<UserGestureToken> takeLastMouseDownGestureToken() { return m_lastMouseDownUserGestureToken.release(); }

    int clickCount() { return m_clickCount; }

    SelectionController& selectionController() const { return *m_selectionController; }

    class TouchInfo {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        DEFINE_INLINE_TRACE()
        {
            visitor->trace(touchTarget);
            visitor->trace(targetFrame);
        }

        PlatformTouchPoint point;
        RefPtrWillBeMember<EventTarget> touchTarget;
        RefPtrWillBeMember<LocalFrame> targetFrame;
        FloatPoint adjustedPagePoint;
        FloatSize adjustedRadius;
        bool knownTarget;
        bool consumed;
    };

private:
    static DragState& dragState();

    DataTransfer* createDraggingDataTransfer() const;

    bool handleMouseMoveOrLeaveEvent(const PlatformMouseEvent&, HitTestResult* hoveredNode = nullptr, bool onlyUpdateScrollbars = false, bool forceLeave = false);
    bool handleMousePressEvent(const MouseEventWithHitTestResults&);
    bool handleMouseFocus(const MouseEventWithHitTestResults&);
    bool handleMouseDraggedEvent(const MouseEventWithHitTestResults&);
    bool handleMouseReleaseEvent(const MouseEventWithHitTestResults&);

    HitTestRequest::HitTestRequestType getHitTypeForGestureType(PlatformEvent::Type);
    void applyTouchAdjustment(PlatformGestureEvent*, HitTestResult*);
    bool handleGestureTap(const GestureEventWithHitTestResults&);
    bool handleGestureLongPress(const GestureEventWithHitTestResults&);
    bool handleGestureLongTap(const GestureEventWithHitTestResults&);
    bool handleGestureScrollUpdate(const PlatformGestureEvent&);
    bool handleGestureScrollBegin(const PlatformGestureEvent&);
    void clearGestureScrollState();

    bool shouldApplyTouchAdjustment(const PlatformGestureEvent&) const;

    OptionalCursor selectCursor(const HitTestResult&);
    OptionalCursor selectAutoCursor(const HitTestResult&, Node*, const Cursor& iBeam);

    void hoverTimerFired(Timer<EventHandler>*);
    void cursorUpdateTimerFired(Timer<EventHandler>*);
    void activeIntervalTimerFired(Timer<EventHandler>*);

    void fakeMouseMoveEventTimerFired(Timer<EventHandler>*);
    void cancelFakeMouseMoveEvent();
    bool isCursorVisible() const;
    void updateCursor();

    ScrollableArea* associatedScrollableArea(const DeprecatedPaintLayer*) const;

    // Scrolls the elements of the DOM tree. Returns true if a node was scrolled.
    // False if we reached the root and couldn't scroll anything.
    // direction - The direction to scroll in. If this is a logical direction, it will be
    //             converted to the physical direction based on a node's writing mode.
    // granularity - The units that the  scroll delta parameter is in.
    // startNode - The node to start bubbling the scroll from. If a node can't scroll,
    //             the scroll bubbles up to the containing block.
    // stopNode - On input, if provided and non-null, the node at which we should stop bubbling on input.
    //            On output, if provided and a node was scrolled stopNode will point to that node.
    // delta - The delta to scroll by, in the units of the granularity parameter. (e.g. pixels, lines, pages, etc.)
    // absolutePoint - For wheel scrolls - the location, in absolute coordinates, where the event occured.
    ScrollResultOneDimensional scroll(ScrollDirection, ScrollGranularity, Node* startNode = nullptr, Node** stopNode = nullptr, float delta = 1.0f, IntPoint absolutePoint = IntPoint());

    void resetOverscroll(bool didScrollX, bool didScrollY);
    void handleOverscroll(const ScrollResult&, const FloatPoint& position = FloatPoint(), const FloatSize& velocity = FloatSize());

    void customizedScroll(const Node& startNode, ScrollState&);

    TouchAction intersectTouchAction(const TouchAction, const TouchAction);
    TouchAction computeEffectiveTouchAction(const Node&);

    HitTestResult hitTestResultInFrame(LocalFrame*, const LayoutPoint&, HitTestRequest::HitTestRequestType hitType = HitTestRequest::ReadOnly | HitTestRequest::Active);

    void invalidateClick();

    void updateMouseEventTargetNode(Node*, const PlatformMouseEvent&, bool);

    /* Dispatches mouseover, mouseout, mouseenter and mouseleave events to appropriate nodes when the mouse pointer moves from one node to another. */
    void sendMouseEventsForNodeTransition(Node*, Node*, const PlatformMouseEvent&);

    MouseEventWithHitTestResults prepareMouseEvent(const HitTestRequest&, const PlatformMouseEvent&);

    bool dispatchMouseEvent(const AtomicString& eventType, Node* target, int clickCount, const PlatformMouseEvent&, bool setUnder);
    bool dispatchDragEvent(const AtomicString& eventType, Node* target, const PlatformMouseEvent&, DataTransfer*);

    void clearDragDataTransfer();

    bool handleDrag(const MouseEventWithHitTestResults&, DragInitiator);
    bool tryStartDrag(const MouseEventWithHitTestResults&);
    void clearDragState();

    bool dispatchDragSrcEvent(const AtomicString& eventType, const PlatformMouseEvent&);

    bool dragHysteresisExceeded(const IntPoint&) const;

    bool passMousePressEventToSubframe(MouseEventWithHitTestResults&, LocalFrame* subframe);
    bool passMouseMoveEventToSubframe(MouseEventWithHitTestResults&, LocalFrame* subframe, HitTestResult* hoveredNode = nullptr);
    bool passMouseReleaseEventToSubframe(MouseEventWithHitTestResults&, LocalFrame* subframe);

    bool passMousePressEventToScrollbar(MouseEventWithHitTestResults&);

    bool passWheelEventToWidget(const PlatformWheelEvent&, Widget&);
    void defaultSpaceEventHandler(KeyboardEvent*);
    void defaultBackspaceEventHandler(KeyboardEvent*);
    void defaultTabEventHandler(KeyboardEvent*);
    void defaultEscapeEventHandler(KeyboardEvent*);
    void defaultArrowEventHandler(WebFocusType, KeyboardEvent*);

    void updateLastScrollbarUnderMouse(Scrollbar*, bool);

    void setFrameWasScrolledByUser();

    bool capturesDragging() const { return m_capturesDragging; }

    bool handleGestureShowPress();

    bool handleScrollGestureOnResizer(Node*, const PlatformGestureEvent&);

    bool passScrollGestureEventToWidget(const PlatformGestureEvent&, LayoutObject*);

    AutoscrollController* autoscrollController() const;
    bool panScrollInProgress() const;
    void setLastKnownMousePosition(const PlatformMouseEvent&);

    bool shouldTopControlsConsumeScroll(FloatSize) const;

    // If the given element is a shadow host and its root has delegatesFocus=false flag,
    // slide focus to its inner element. Returns true if the resulting focus is different from
    // the given element.
    bool slideFocusOnShadowHostIfNecessary(const Element&);

    void dispatchPointerEventsForTouchEvent(const PlatformTouchEvent&, WillBeHeapVector<TouchInfo>&);
    void sendPointerCancels(WillBeHeapVector<TouchInfo>&);

    bool dispatchTouchEvents(const PlatformTouchEvent&, WillBeHeapVector<TouchInfo>&, bool, bool);

    // NOTE: If adding a new field to this class please ensure that it is
    // cleared in |EventHandler::clear()|.

    LocalFrame* const m_frame;

    bool m_mousePressed;
    bool m_capturesDragging;
    RefPtrWillBeMember<Node> m_mousePressNode;

    bool m_mouseDownMayStartDrag;
    const OwnPtrWillBeMember<SelectionController> m_selectionController;

    LayoutPoint m_dragStartPos;

    Timer<EventHandler> m_hoverTimer;

    // TODO(rbyers): Mouse cursor update is page-wide, not per-frame.  Page-wide state
    // should move out of EventHandler to a new PageEventHandler class. crbug.com/449649
    Timer<EventHandler> m_cursorUpdateTimer;

    bool m_mouseDownMayStartAutoscroll;

    Timer<EventHandler> m_fakeMouseMoveEventTimer;

    bool m_svgPan;

    RawPtrWillBeMember<DeprecatedPaintLayerScrollableArea> m_resizeScrollableArea;

    RefPtrWillBeMember<Node> m_capturingMouseEventsNode;
    bool m_eventHandlerWillResetCapturingMouseEventsNode;

    RefPtrWillBeMember<Node> m_nodeUnderMouse;
    RefPtrWillBeMember<Node> m_lastNodeUnderMouse;
    RefPtrWillBeMember<LocalFrame> m_lastMouseMoveEventSubframe;
    RefPtrWillBeMember<Scrollbar> m_lastScrollbarUnderMouse;

    int m_clickCount;
    RefPtrWillBeMember<Node> m_clickNode;

    RefPtrWillBeMember<Node> m_dragTarget;
    bool m_shouldOnlyFireDragOverEvent;

    RefPtrWillBeMember<HTMLFrameSetElement> m_frameSetBeingResized;

    LayoutSize m_offsetFromResizeCorner; // In the coords of m_resizeScrollableArea.

    FloatSize m_accumulatedRootOverscroll;

    bool m_mousePositionIsUnknown;
    // The last mouse movement position this frame has seen in root frame coordinates.
    IntPoint m_lastKnownMousePosition;
    IntPoint m_lastKnownMouseGlobalPosition;
    IntPoint m_mouseDownPos; // In our view's coords.
    double m_mouseDownTimestamp;
    PlatformMouseEvent m_mouseDown;
    RefPtr<UserGestureToken> m_lastMouseDownUserGestureToken;

    RefPtrWillBeMember<Node> m_latchedWheelEventNode;
    bool m_widgetIsLatched;

    RefPtrWillBeMember<Node> m_previousWheelScrolledNode;

    // The target of each active touch point indexed by the touch ID.
    using TouchTargetMap = WillBeHeapHashMap<unsigned, RefPtrWillBeMember<EventTarget>, DefaultHash<unsigned>::Hash, WTF::UnsignedWithZeroKeyHashTraits<unsigned>>;
    TouchTargetMap m_targetForTouchID;

    // If set, the document of the active touch sequence. Unset if no touch sequence active.
    RefPtrWillBeMember<Document> m_touchSequenceDocument;
    RefPtr<UserGestureToken> m_touchSequenceUserGestureToken;

    bool m_touchPressed;

    PointerIdManager m_pointerIdManager;

    // This is set upon sending a pointercancel for touch, prevents PE dispatches for touches until
    // all touch-points become inactive.
    // TODO(mustaq): Consider a state per pointerType, as in PointerIdManager? Exclude mouse?
    bool m_inPointerCanceledState;

    RefPtrWillBeMember<Node> m_scrollGestureHandlingNode;
    bool m_lastGestureScrollOverWidget;
    // The most recent element to scroll natively during this scroll
    // sequence. Null if no native element has scrolled this scroll
    // sequence, or if the most recent element to scroll used scroll
    // customization.
    RefPtrWillBeMember<Node> m_previousGestureScrolledNode;
    RefPtrWillBeMember<Scrollbar> m_scrollbarHandlingScrollGesture;

    double m_maxMouseMovedDuration;

    bool m_longTapShouldInvokeContextMenu;

    Timer<EventHandler> m_activeIntervalTimer;
    double m_lastShowPressTimestamp;
    RefPtrWillBeMember<Element> m_lastDeferredTapElement;

    // Only used with the ScrollCustomization runtime enabled feature.
    WillBeHeapDeque<RefPtrWillBeMember<Element>> m_currentScrollChain;
    // True iff some of the delta has been consumed for the current
    // scroll sequence in this frame, or any child frames. Only used
    // with ScrollCustomization. If some delta has been consumed, a
    // scroll which shouldn't propagate can't cause any element to
    // scroll other than the |m_previousGestureScrolledNode|.
    bool m_deltaConsumedForScrollSequence;
};

} // namespace blink

WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::EventHandler::TouchInfo);

#endif // EventHandler_h
