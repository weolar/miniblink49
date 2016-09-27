/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "web/WebInputEventConversion.h"

#include "core/dom/Touch.h"
#include "core/dom/TouchList.h"
#include "core/events/GestureEvent.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/TouchEvent.h"
#include "core/events/WheelEvent.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/PinchViewport.h"
#include "core/layout/LayoutObject.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "platform/KeyboardCodes.h"
#include "platform/Widget.h"
#include "public/platform/Platform.h"

namespace blink {

static const double millisPerSecond = 1000.0;

static float scaleDeltaToWindow(const Widget* widget, float delta)
{
    float scale = 1;
    if (widget) {
        FrameView* rootView = toFrameView(widget->root());
        if (rootView)
            scale = rootView->inputEventsScaleFactor();
    }
    return delta / scale;
}

static FloatSize scaleSizeToWindow(const Widget* widget, FloatSize size)
{
    return FloatSize(scaleDeltaToWindow(widget, size.width()), scaleDeltaToWindow(widget, size.height()));
}

static FloatPoint convertHitPointToWindow(const Widget* widget, FloatPoint point)
{
    float scale = 1;
    IntSize offset;
    IntPoint pinchViewport;
    FloatSize overscrollOffset;
    if (widget) {
        FrameView* rootView = toFrameView(widget->root());
        if (rootView) {
            scale = rootView->inputEventsScaleFactor();
            offset = rootView->inputEventsOffsetForEmulation();
            pinchViewport = flooredIntPoint(rootView->page()->frameHost().pinchViewport().visibleRect().location());
            overscrollOffset = rootView->page()->frameHost().chromeClient().elasticOverscroll();
        }
    }
    return FloatPoint(
        (point.x() - offset.width()) / scale + pinchViewport.x() + overscrollOffset.width(),
        (point.y() - offset.height()) / scale + pinchViewport.y() + overscrollOffset.height());
}

static unsigned toPlatformEventModifiers(int webModifiers)
{
    unsigned newModifiers = 0;
    if (webModifiers & WebInputEvent::ShiftKey)
        newModifiers |= PlatformEvent::ShiftKey;
    if (webModifiers & WebInputEvent::ControlKey)
        newModifiers |= PlatformEvent::CtrlKey;
    if (webModifiers & WebInputEvent::AltKey)
        newModifiers |= PlatformEvent::AltKey;
    if (webModifiers & WebInputEvent::MetaKey)
        newModifiers |= PlatformEvent::MetaKey;
    return newModifiers;
}

unsigned toPlatformMouseEventModifiers(int webModifiers)
{
    unsigned newModifiers = toPlatformEventModifiers(webModifiers);
    if (webModifiers & WebInputEvent::LeftButtonDown)
        newModifiers |= PlatformEvent::LeftButtonDown;
    if (webModifiers & WebInputEvent::MiddleButtonDown)
        newModifiers |= PlatformEvent::MiddleButtonDown;
    if (webModifiers & WebInputEvent::RightButtonDown)
        newModifiers |= PlatformEvent::RightButtonDown;
    return newModifiers;
}

static unsigned toPlatformModifierFrom(WebMouseEvent::Button button)
{
    if (button == WebMouseEvent::ButtonNone)
        return 0;

    unsigned webMouseButtonToPlatformModifier[] = {
        PlatformEvent::LeftButtonDown,
        PlatformEvent::MiddleButtonDown,
        PlatformEvent::RightButtonDown
    };

    return webMouseButtonToPlatformModifier[button];
}

// MakePlatformMouseEvent -----------------------------------------------------

PlatformMouseEventBuilder::PlatformMouseEventBuilder(Widget* widget, const WebMouseEvent& e)
{
    // FIXME: Widget is always toplevel, unless it's a popup. We may be able
    // to get rid of this once we abstract popups into a WebKit API.
    m_position = widget->convertFromContainingWindow(flooredIntPoint(convertHitPointToWindow(widget, IntPoint(e.x, e.y))));
    m_globalPosition = IntPoint(e.globalX, e.globalY);
    m_movementDelta = IntPoint(scaleDeltaToWindow(widget, e.movementX), scaleDeltaToWindow(widget, e.movementY));
    m_button = static_cast<MouseButton>(e.button);
    m_modifiers = toPlatformMouseEventModifiers(e.modifiers);

    m_timestamp = e.timeStampSeconds;
    m_clickCount = e.clickCount;

    switch (e.type) {
    case WebInputEvent::MouseMove:
    case WebInputEvent::MouseLeave:  // synthesize a move event
        m_type = PlatformEvent::MouseMoved;
        break;

    case WebInputEvent::MouseDown:
        m_type = PlatformEvent::MousePressed;
        break;

    case WebInputEvent::MouseUp:
        m_type = PlatformEvent::MouseReleased;

        // The MouseEvent spec requires that buttons indicates the state
        // immediately after the event takes place. To ensure consistency
        // between platforms here, we explicitly clear the button that is
        // in the process of being released.
        m_modifiers &= ~toPlatformModifierFrom(e.button);
        break;

    default:
        ASSERT_NOT_REACHED();
    }
}

// PlatformWheelEventBuilder --------------------------------------------------

PlatformWheelEventBuilder::PlatformWheelEventBuilder(Widget* widget, const WebMouseWheelEvent& e)
{
    m_position = widget->convertFromContainingWindow(flooredIntPoint(convertHitPointToWindow(widget, FloatPoint(e.x, e.y))));
    m_globalPosition = IntPoint(e.globalX, e.globalY);
    m_deltaX = e.deltaX;
    m_deltaY = e.deltaY;
    m_wheelTicksX = e.wheelTicksX;
    m_wheelTicksY = e.wheelTicksY;
    m_granularity = e.scrollByPage ?
        ScrollByPageWheelEvent : ScrollByPixelWheelEvent;

    m_type = PlatformEvent::Wheel;

    m_modifiers = toPlatformMouseEventModifiers(e.modifiers);

    m_hasPreciseScrollingDeltas = e.hasPreciseScrollingDeltas;
    m_canScroll = e.canScroll;
    m_railsMode = static_cast<PlatformEvent::RailsMode>(e.railsMode);
#if OS(MACOSX)
    m_phase = static_cast<PlatformWheelEventPhase>(e.phase);
    m_momentumPhase = static_cast<PlatformWheelEventPhase>(e.momentumPhase);
    m_timestamp = e.timeStampSeconds;
    m_canRubberbandLeft = e.canRubberbandLeft;
    m_canRubberbandRight = e.canRubberbandRight;
#endif
}

// PlatformGestureEventBuilder --------------------------------------------------

PlatformGestureEventBuilder::PlatformGestureEventBuilder(Widget* widget, const WebGestureEvent& e)
{
    switch (e.type) {
    case WebInputEvent::GestureScrollBegin:
        m_type = PlatformEvent::GestureScrollBegin;
        break;
    case WebInputEvent::GestureScrollEnd:
        m_type = PlatformEvent::GestureScrollEnd;
        break;
    case WebInputEvent::GestureFlingStart:
        m_type = PlatformEvent::GestureFlingStart;
        break;
    case WebInputEvent::GestureScrollUpdate:
        m_type = PlatformEvent::GestureScrollUpdate;
        m_data.m_scroll.m_deltaX = scaleDeltaToWindow(widget, e.data.scrollUpdate.deltaX);
        m_data.m_scroll.m_deltaY = scaleDeltaToWindow(widget, e.data.scrollUpdate.deltaY);
        m_data.m_scroll.m_velocityX = e.data.scrollUpdate.velocityX;
        m_data.m_scroll.m_velocityY = e.data.scrollUpdate.velocityY;
        m_data.m_scroll.m_preventPropagation = e.data.scrollUpdate.preventPropagation;
        m_data.m_scroll.m_inertial = e.data.scrollUpdate.inertial;
        break;
    case WebInputEvent::GestureTap:
        m_type = PlatformEvent::GestureTap;
        m_area = expandedIntSize(scaleSizeToWindow(widget, FloatSize(e.data.tap.width, e.data.tap.height)));
        m_data.m_tap.m_tapCount = e.data.tap.tapCount;
        break;
    case WebInputEvent::GestureTapUnconfirmed:
        m_type = PlatformEvent::GestureTapUnconfirmed;
        m_area = expandedIntSize(scaleSizeToWindow(widget, FloatSize(e.data.tap.width, e.data.tap.height)));
        break;
    case WebInputEvent::GestureTapDown:
        m_type = PlatformEvent::GestureTapDown;
        m_area = expandedIntSize(scaleSizeToWindow(widget, FloatSize(e.data.tapDown.width, e.data.tapDown.height)));
        break;
    case WebInputEvent::GestureShowPress:
        m_type = PlatformEvent::GestureShowPress;
        m_area = expandedIntSize(scaleSizeToWindow(widget, FloatSize(e.data.showPress.width, e.data.showPress.height)));
        break;
    case WebInputEvent::GestureTapCancel:
        m_type = PlatformEvent::GestureTapDownCancel;
        break;
    case WebInputEvent::GestureDoubleTap:
        // DoubleTap gesture is now handled as PlatformEvent::GestureTap with tap_count = 2. So no
        // need to convert to a Platfrom DoubleTap gesture. But in WebViewImpl::handleGestureEvent
        // all WebGestureEvent are converted to PlatformGestureEvent, for completeness and not reach
        // the ASSERT_NOT_REACHED() at the end, convert the DoubleTap to a NoType.
        m_type = PlatformEvent::NoType;
        break;
    case WebInputEvent::GestureTwoFingerTap:
        m_type = PlatformEvent::GestureTwoFingerTap;
        m_area = expandedIntSize(scaleSizeToWindow(widget, FloatSize(e.data.twoFingerTap.firstFingerWidth, e.data.twoFingerTap.firstFingerHeight)));
        break;
    case WebInputEvent::GestureLongPress:
        m_type = PlatformEvent::GestureLongPress;
        m_area = expandedIntSize(scaleSizeToWindow(widget, FloatSize(e.data.longPress.width, e.data.longPress.height)));
        break;
    case WebInputEvent::GestureLongTap:
        m_type = PlatformEvent::GestureLongTap;
        m_area = expandedIntSize(scaleSizeToWindow(widget, FloatSize(e.data.longPress.width, e.data.longPress.height)));
        break;
    case WebInputEvent::GesturePinchBegin:
        m_type = PlatformEvent::GesturePinchBegin;
        break;
    case WebInputEvent::GesturePinchEnd:
        m_type = PlatformEvent::GesturePinchEnd;
        break;
    case WebInputEvent::GesturePinchUpdate:
        m_type = PlatformEvent::GesturePinchUpdate;
        m_data.m_pinchUpdate.m_scale = e.data.pinchUpdate.scale;
        break;
    default:
        ASSERT_NOT_REACHED();
    }
    m_position = widget->convertFromContainingWindow(flooredIntPoint(convertHitPointToWindow(widget, FloatPoint(e.x, e.y))));
    m_globalPosition = IntPoint(e.globalX, e.globalY);
    m_timestamp = e.timeStampSeconds;
    m_modifiers = toPlatformEventModifiers(e.modifiers);
}

// MakePlatformKeyboardEvent --------------------------------------------------

inline PlatformEvent::Type toPlatformKeyboardEventType(WebInputEvent::Type type)
{
    switch (type) {
    case WebInputEvent::KeyUp:
        return PlatformEvent::KeyUp;
    case WebInputEvent::KeyDown:
        return PlatformEvent::KeyDown;
    case WebInputEvent::RawKeyDown:
        return PlatformEvent::RawKeyDown;
    case WebInputEvent::Char:
        return PlatformEvent::Char;
    default:
        ASSERT_NOT_REACHED();
    }
    return PlatformEvent::KeyDown;
}

PlatformKeyboardEventBuilder::PlatformKeyboardEventBuilder(const WebKeyboardEvent& e)
{
    m_type = toPlatformKeyboardEventType(e.type);
    m_text = String(e.text);
    m_unmodifiedText = String(e.unmodifiedText);
    m_keyIdentifier = String(e.keyIdentifier);
    m_autoRepeat = (e.modifiers & WebInputEvent::IsAutoRepeat);
    m_nativeVirtualKeyCode = e.nativeKeyCode;
    m_isKeypad = (e.modifiers & WebInputEvent::IsKeyPad);
    m_isSystemKey = e.isSystemKey;
    // TODO: BUG482880 Fix this initialization to lazy initialization.
    m_code = Platform::current()->domCodeStringFromEnum(e.domCode);
    m_key = Platform::current()->domKeyStringFromEnum(e.domKey);

    m_modifiers = toPlatformEventModifiers(e.modifiers);

    // FIXME: PlatformKeyboardEvents expect a locational version of the keycode (e.g. VK_LSHIFT
    // instead of VK_SHIFT). This should be changed so the location/keycode are stored separately,
    // as in other places in the code.
    m_windowsVirtualKeyCode = e.windowsKeyCode;
    if (e.windowsKeyCode == VK_SHIFT) {
        if (e.modifiers & WebInputEvent::IsLeft)
            m_windowsVirtualKeyCode = VK_LSHIFT;
        else if (e.modifiers & WebInputEvent::IsRight)
            m_windowsVirtualKeyCode = VK_RSHIFT;
    } else if (e.windowsKeyCode == VK_CONTROL) {
        if (e.modifiers & WebInputEvent::IsLeft)
            m_windowsVirtualKeyCode = VK_LCONTROL;
        else if (e.modifiers & WebInputEvent::IsRight)
            m_windowsVirtualKeyCode = VK_RCONTROL;
    } else if (e.windowsKeyCode == VK_MENU) {
        if (e.modifiers & WebInputEvent::IsLeft)
            m_windowsVirtualKeyCode = VK_LMENU;
        else if (e.modifiers & WebInputEvent::IsRight)
            m_windowsVirtualKeyCode = VK_RMENU;
    }

}

void PlatformKeyboardEventBuilder::setKeyType(Type type)
{
    // According to the behavior of Webkit in Windows platform,
    // we need to convert KeyDown to RawKeydown and Char events
    // See WebKit/WebKit/Win/WebView.cpp
    ASSERT(m_type == KeyDown);
    ASSERT(type == RawKeyDown || type == Char);
    m_type = type;

    if (type == RawKeyDown) {
        m_text = String();
        m_unmodifiedText = String();
    } else {
        m_keyIdentifier = String();
        m_windowsVirtualKeyCode = 0;
    }
}

// Please refer to bug http://b/issue?id=961192, which talks about Webkit
// keyboard event handling changes. It also mentions the list of keys
// which don't have associated character events.
bool PlatformKeyboardEventBuilder::isCharacterKey() const
{
    switch (windowsVirtualKeyCode()) {
    case VKEY_BACK:
    case VKEY_ESCAPE:
        return false;
    }
    return true;
}

inline PlatformEvent::Type toPlatformTouchEventType(const WebInputEvent::Type type)
{
    switch (type) {
    case WebInputEvent::TouchStart:
        return PlatformEvent::TouchStart;
    case WebInputEvent::TouchMove:
        return PlatformEvent::TouchMove;
    case WebInputEvent::TouchEnd:
        return PlatformEvent::TouchEnd;
    case WebInputEvent::TouchCancel:
        return PlatformEvent::TouchCancel;
    default:
        ASSERT_NOT_REACHED();
    }
    return PlatformEvent::TouchStart;
}

inline PlatformTouchPoint::State toPlatformTouchPointState(const WebTouchPoint::State state)
{
    switch (state) {
    case WebTouchPoint::StateReleased:
        return PlatformTouchPoint::TouchReleased;
    case WebTouchPoint::StatePressed:
        return PlatformTouchPoint::TouchPressed;
    case WebTouchPoint::StateMoved:
        return PlatformTouchPoint::TouchMoved;
    case WebTouchPoint::StateStationary:
        return PlatformTouchPoint::TouchStationary;
    case WebTouchPoint::StateCancelled:
        return PlatformTouchPoint::TouchCancelled;
    case WebTouchPoint::StateUndefined:
        ASSERT_NOT_REACHED();
    }
    return PlatformTouchPoint::TouchReleased;
}

inline WebTouchPoint::State toWebTouchPointState(const AtomicString& type)
{
    if (type == EventTypeNames::touchend)
        return WebTouchPoint::StateReleased;
    if (type == EventTypeNames::touchcancel)
        return WebTouchPoint::StateCancelled;
    if (type == EventTypeNames::touchstart)
        return WebTouchPoint::StatePressed;
    if (type == EventTypeNames::touchmove)
        return WebTouchPoint::StateMoved;
    return WebTouchPoint::StateUndefined;
}

PlatformTouchPointBuilder::PlatformTouchPointBuilder(Widget* widget, const WebTouchPoint& point)
{
    m_id = point.id;
    m_state = toPlatformTouchPointState(point.state);

    // This assumes convertFromContainingWindow does only translations, not scales.
    FloatPoint floatPos = convertHitPointToWindow(widget, point.position);
    IntPoint flooredPoint = flooredIntPoint(floatPos);
    m_pos = widget->convertFromContainingWindow(flooredPoint) + (floatPos - flooredPoint);

    m_screenPos = FloatPoint(point.screenPosition.x, point.screenPosition.y);
    m_radius = scaleSizeToWindow(widget, FloatSize(point.radiusX, point.radiusY));
    m_rotationAngle = point.rotationAngle;
    m_force = point.force;
}

PlatformTouchEventBuilder::PlatformTouchEventBuilder(Widget* widget, const WebTouchEvent& event)
{
    m_type = toPlatformTouchEventType(event.type);
    m_modifiers = toPlatformEventModifiers(event.modifiers);
    m_timestamp = event.timeStampSeconds;
    m_causesScrollingIfUncanceled = event.causesScrollingIfUncanceled;

    for (unsigned i = 0; i < event.touchesLength; ++i)
        m_touchPoints.append(PlatformTouchPointBuilder(widget, event.touches[i]));

    m_cancelable = event.cancelable;
}

static int getWebInputModifiers(const UIEventWithKeyState& event)
{
    int modifiers = 0;
    if (event.ctrlKey())
        modifiers |= WebInputEvent::ControlKey;
    if (event.shiftKey())
        modifiers |= WebInputEvent::ShiftKey;
    if (event.altKey())
        modifiers |= WebInputEvent::AltKey;
    if (event.metaKey())
        modifiers |= WebInputEvent::MetaKey;
    return modifiers;
}

static FloatPoint convertAbsoluteLocationForLayoutObjectFloat(const LayoutPoint& location, const LayoutObject& layoutObject)
{
    return layoutObject.absoluteToLocal(FloatPoint(location), UseTransforms);
}

static IntPoint convertAbsoluteLocationForLayoutObject(const LayoutPoint& location, const LayoutObject& layoutObject)
{
    return roundedIntPoint(convertAbsoluteLocationForLayoutObjectFloat(location, layoutObject));
}

// FIXME: Change |widget| to const Widget& after RemoteFrames get
// RemoteFrameViews.
static void updateWebMouseEventFromCoreMouseEvent(const MouseRelatedEvent& event, const Widget* widget, const LayoutObject& layoutObject, WebMouseEvent& webEvent)
{
    webEvent.timeStampSeconds = event.timeStamp() / millisPerSecond;
    webEvent.modifiers = getWebInputModifiers(event);

    FrameView* view = widget ? toFrameView(widget->parent()) : 0;
    // FIXME: If view == nullptr, pointInRootFrame will really be pointInRootContent.
    IntPoint pointInRootFrame = IntPoint(event.absoluteLocation().x(), event.absoluteLocation().y());
    if (view)
        pointInRootFrame = view->contentsToRootFrame(pointInRootFrame);
    webEvent.globalX = event.screenX();
    webEvent.globalY = event.screenY();
    webEvent.windowX = pointInRootFrame.x();
    webEvent.windowY = pointInRootFrame.y();
    IntPoint localPoint = convertAbsoluteLocationForLayoutObject(event.absoluteLocation(), layoutObject);
    webEvent.x = localPoint.x();
    webEvent.y = localPoint.y();
}

WebMouseEventBuilder::WebMouseEventBuilder(const Widget* widget, const LayoutObject* layoutObject, const MouseEvent& event)
{
    if (event.type() == EventTypeNames::mousemove)
        type = WebInputEvent::MouseMove;
    else if (event.type() == EventTypeNames::mouseout)
        type = WebInputEvent::MouseLeave;
    else if (event.type() == EventTypeNames::mouseover)
        type = WebInputEvent::MouseEnter;
    else if (event.type() == EventTypeNames::mousedown)
        type = WebInputEvent::MouseDown;
    else if (event.type() == EventTypeNames::mouseup)
        type = WebInputEvent::MouseUp;
    else if (event.type() == EventTypeNames::contextmenu)
        type = WebInputEvent::ContextMenu;
    else
        return; // Skip all other mouse events.

    updateWebMouseEventFromCoreMouseEvent(event, widget, *layoutObject, *this);

    switch (event.button()) {
    case LeftButton:
        button = WebMouseEvent::ButtonLeft;
        break;
    case MiddleButton:
        button = WebMouseEvent::ButtonMiddle;
        break;
    case RightButton:
        button = WebMouseEvent::ButtonRight;
        break;
    }
    if (event.buttonDown()) {
        switch (event.button()) {
        case LeftButton:
            modifiers |= WebInputEvent::LeftButtonDown;
            break;
        case MiddleButton:
            modifiers |= WebInputEvent::MiddleButtonDown;
            break;
        case RightButton:
            modifiers |= WebInputEvent::RightButtonDown;
            break;
        }
    } else
        button = WebMouseEvent::ButtonNone;
    movementX = event.movementX();
    movementY = event.movementY();
    clickCount = event.detail();
}

// Generate a synthetic WebMouseEvent given a TouchEvent (eg. for emulating a mouse
// with touch input for plugins that don't support touch input).
WebMouseEventBuilder::WebMouseEventBuilder(const Widget* widget, const LayoutObject* layoutObject, const TouchEvent& event)
{
    if (!event.touches())
        return;
    if (event.touches()->length() != 1) {
        if (event.touches()->length() || event.type() != EventTypeNames::touchend || !event.changedTouches() || event.changedTouches()->length() != 1)
            return;
    }

    const Touch* touch = event.touches()->length() == 1 ? event.touches()->item(0) : event.changedTouches()->item(0);
    if (touch->identifier())
        return;

    if (event.type() == EventTypeNames::touchstart)
        type = MouseDown;
    else if (event.type() == EventTypeNames::touchmove)
        type = MouseMove;
    else if (event.type() == EventTypeNames::touchend)
        type = MouseUp;
    else
        return;

    timeStampSeconds = event.timeStamp() / millisPerSecond;
    modifiers = getWebInputModifiers(event);

    // The mouse event co-ordinates should be generated from the co-ordinates of the touch point.
    FrameView* view =  toFrameView(widget->parent());
    // FIXME: if view == nullptr, pointInRootFrame will really be pointInRootContent.
    IntPoint pointInRootFrame = roundedIntPoint(touch->absoluteLocation());
    if (view)
        pointInRootFrame = view->contentsToRootFrame(pointInRootFrame);
    IntPoint screenPoint = roundedIntPoint(touch->screenLocation());
    globalX = screenPoint.x();
    globalY = screenPoint.y();
    windowX = pointInRootFrame.x();
    windowY = pointInRootFrame.y();

    button = WebMouseEvent::ButtonLeft;
    modifiers |= WebInputEvent::LeftButtonDown;
    clickCount = (type == MouseDown || type == MouseUp);

    IntPoint localPoint = convertAbsoluteLocationForLayoutObject(touch->absoluteLocation(), *layoutObject);
    x = localPoint.x();
    y = localPoint.y();
}

WebMouseWheelEventBuilder::WebMouseWheelEventBuilder(const Widget* widget, const LayoutObject* layoutObject, const WheelEvent& event)
{
    if (event.type() != EventTypeNames::wheel && event.type() != EventTypeNames::mousewheel)
        return;
    type = WebInputEvent::MouseWheel;
    updateWebMouseEventFromCoreMouseEvent(event, widget, *layoutObject, *this);
    deltaX = -event.deltaX();
    deltaY = -event.deltaY();
    wheelTicksX = event.ticksX();
    wheelTicksY = event.ticksY();
    scrollByPage = event.deltaMode() == WheelEvent::DOM_DELTA_PAGE;
    canScroll = event.canScroll();
    railsMode = static_cast<RailsMode>(event.railsMode());
}

WebKeyboardEventBuilder::WebKeyboardEventBuilder(const KeyboardEvent& event)
{
    if (event.type() == EventTypeNames::keydown)
        type = KeyDown;
    else if (event.type() == EventTypeNames::keyup)
        type = WebInputEvent::KeyUp;
    else if (event.type() == EventTypeNames::keypress)
        type = WebInputEvent::Char;
    else
        return; // Skip all other keyboard events.

    modifiers = getWebInputModifiers(event);
    if (event.location() == KeyboardEvent::DOM_KEY_LOCATION_NUMPAD)
        modifiers |= WebInputEvent::IsKeyPad;
    else if (event.location() == KeyboardEvent::DOM_KEY_LOCATION_LEFT)
        modifiers |= WebInputEvent::IsLeft;
    else if (event.location() == KeyboardEvent::DOM_KEY_LOCATION_RIGHT)
        modifiers |= WebInputEvent::IsRight;

    timeStampSeconds = event.timeStamp() / millisPerSecond;
    windowsKeyCode = event.keyCode();

    // The platform keyevent does not exist if the event was created using
    // initKeyboardEvent.
    if (!event.keyEvent())
        return;
    nativeKeyCode = event.keyEvent()->nativeVirtualKeyCode();
    domCode = Platform::current()->domEnumFromCodeString(event.keyEvent()->code());
    domKey = Platform::current()->domKeyEnumFromString(event.keyEvent()->key());
    unsigned numberOfCharacters = std::min(event.keyEvent()->text().length(), static_cast<unsigned>(textLengthCap));
    for (unsigned i = 0; i < numberOfCharacters; ++i) {
        text[i] = event.keyEvent()->text()[i];
        unmodifiedText[i] = event.keyEvent()->unmodifiedText()[i];
    }
    memcpy(keyIdentifier, event.keyIdentifier().ascii().data(), event.keyIdentifier().length());
}

WebInputEvent::Type toWebKeyboardEventType(PlatformEvent::Type type)
{
    switch (type) {
    case PlatformEvent::KeyUp:
        return WebInputEvent::KeyUp;
    case PlatformEvent::KeyDown:
        return WebInputEvent::KeyDown;
    case PlatformEvent::RawKeyDown:
        return WebInputEvent::RawKeyDown;
    case PlatformEvent::Char:
        return WebInputEvent::Char;
    default:
        return WebInputEvent::Undefined;
    }
}

static WebTouchPoint toWebTouchPoint(const Touch* touch, const LayoutObject* layoutObject, WebTouchPoint::State state)
{
    WebTouchPoint point;
    point.id = touch->identifier();
    point.screenPosition = touch->screenLocation();
    point.position = convertAbsoluteLocationForLayoutObjectFloat(touch->absoluteLocation(), *layoutObject);
    point.radiusX = touch->radiusX();
    point.radiusY = touch->radiusY();
    point.rotationAngle = touch->rotationAngle();
    point.force = touch->force();
    point.state = state;
    return point;
}

static unsigned indexOfTouchPointWithId(const WebTouchPoint* touchPoints, unsigned touchPointsLength, unsigned id)
{
    for (unsigned i = 0; i < touchPointsLength; ++i) {
        if (touchPoints[i].id == static_cast<int>(id))
            return i;
    }
    return std::numeric_limits<unsigned>::max();
}

static void addTouchPointsUpdateStateIfNecessary(WebTouchPoint::State state, TouchList* touches, WebTouchPoint* touchPoints, unsigned* touchPointsLength, const LayoutObject* layoutObject)
{
    unsigned initialTouchPointsLength = *touchPointsLength;
    for (unsigned i = 0; i < touches->length(); ++i) {
        const unsigned pointIndex = *touchPointsLength;
        if (pointIndex >= static_cast<unsigned>(WebTouchEvent::touchesLengthCap))
            return;

        const Touch* touch = touches->item(i);
        unsigned existingPointIndex = indexOfTouchPointWithId(touchPoints, initialTouchPointsLength, touch->identifier());
        if (existingPointIndex != std::numeric_limits<unsigned>::max()) {
            touchPoints[existingPointIndex].state = state;
        } else {
            touchPoints[pointIndex] = toWebTouchPoint(touch, layoutObject, state);
            ++(*touchPointsLength);
        }
    }
}

WebTouchEventBuilder::WebTouchEventBuilder(const LayoutObject* layoutObject, const TouchEvent& event)
{
    if (event.type() == EventTypeNames::touchstart)
        type = TouchStart;
    else if (event.type() == EventTypeNames::touchmove)
        type = TouchMove;
    else if (event.type() == EventTypeNames::touchend)
        type = TouchEnd;
    else if (event.type() == EventTypeNames::touchcancel)
        type = TouchCancel;
    else {
        ASSERT_NOT_REACHED();
        type = Undefined;
        return;
    }

    modifiers = getWebInputModifiers(event);
    timeStampSeconds = event.timeStamp() / millisPerSecond;
    cancelable = event.cancelable();
    causesScrollingIfUncanceled = event.causesScrollingIfUncanceled();

    // Currently touches[] is empty, add stationary points as-is.
    for (unsigned i = 0; i < event.touches()->length() && i < static_cast<unsigned>(WebTouchEvent::touchesLengthCap); ++i) {
        touches[i] = toWebTouchPoint(event.touches()->item(i), layoutObject, WebTouchPoint::StateStationary);
        ++touchesLength;
    }
    // If any existing points are also in the change list, we should update
    // their state, otherwise just add the new points.
    addTouchPointsUpdateStateIfNecessary(toWebTouchPointState(event.type()), event.changedTouches(), touches, &touchesLength, layoutObject);
}

WebGestureEventBuilder::WebGestureEventBuilder(const LayoutObject* layoutObject, const GestureEvent& event)
{
    if (event.type() == EventTypeNames::gestureshowpress)
        type = GestureShowPress;
    else if (event.type() == EventTypeNames::gesturelongpress)
        type = GestureLongPress;
    else if (event.type() == EventTypeNames::gesturetapdown)
        type = GestureTapDown;
    else if (event.type() == EventTypeNames::gesturescrollstart)
        type = GestureScrollBegin;
    else if (event.type() == EventTypeNames::gesturescrollend)
        type = GestureScrollEnd;
    else if (event.type() == EventTypeNames::gesturescrollupdate) {
        type = GestureScrollUpdate;
        data.scrollUpdate.deltaX = event.deltaX();
        data.scrollUpdate.deltaY = event.deltaY();
    } else if (event.type() == EventTypeNames::gesturetap) {
        type = GestureTap;
        data.tap.tapCount = 1;
    }

    timeStampSeconds = event.timeStamp() / millisPerSecond;
    modifiers = getWebInputModifiers(event);

    globalX = event.screenX();
    globalY = event.screenY();
    IntPoint localPoint = convertAbsoluteLocationForLayoutObject(event.absoluteLocation(), *layoutObject);
    x = localPoint.x();
    y = localPoint.y();
}

} // namespace blink
