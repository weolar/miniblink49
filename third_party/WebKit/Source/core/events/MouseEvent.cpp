/*
 * Copyright (C) 2001 Peter Kelly (pmk@post.com)
 * Copyright (C) 2001 Tobias Anton (anton@stud.fbi.fh-darmstadt.de)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2003, 2005, 2006, 2008 Apple Inc. All rights reserved.
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
#include "core/events/MouseEvent.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/clipboard/DataTransfer.h"
#include "core/dom/Element.h"
#include "core/events/EventDispatcher.h"
#include "platform/PlatformMouseEvent.h"

namespace blink {

PassRefPtrWillBeRawPtr<MouseEvent> MouseEvent::create(ScriptState* scriptState, const AtomicString& type, const MouseEventInit& initializer)
{
    if (scriptState && scriptState->world().isIsolatedWorld())
        UIEventWithKeyState::didCreateEventInIsolatedWorld(initializer.ctrlKey(), initializer.altKey(), initializer.shiftKey(), initializer.metaKey());
    return adoptRefWillBeNoop(new MouseEvent(type, initializer));
}

PassRefPtrWillBeRawPtr<MouseEvent> MouseEvent::create(const AtomicString& eventType, PassRefPtrWillBeRawPtr<AbstractView> view, const PlatformMouseEvent& event, int detail, PassRefPtrWillBeRawPtr<Node> relatedTarget)
{
    ASSERT(event.type() == PlatformEvent::MouseMoved || event.button() != NoButton);

    bool isMouseEnterOrLeave = eventType == EventTypeNames::mouseenter || eventType == EventTypeNames::mouseleave;
    bool isCancelable = !isMouseEnterOrLeave;
    bool isBubbling = !isMouseEnterOrLeave;

    return MouseEvent::create(
        eventType, isBubbling, isCancelable, view,
        detail, event.globalPosition().x(), event.globalPosition().y(), event.position().x(), event.position().y(),
        event.movementDelta().x(), event.movementDelta().y(),
        event.ctrlKey(), event.altKey(), event.shiftKey(), event.metaKey(), event.button(),
        platformModifiersToButtons(event.modifiers()),
        relatedTarget, nullptr, false, event.syntheticEventType(), event.timestamp());
}

PassRefPtrWillBeRawPtr<MouseEvent> MouseEvent::create(const AtomicString& type, bool canBubble, bool cancelable, PassRefPtrWillBeRawPtr<AbstractView> view,
    int detail, int screenX, int screenY, int windowX, int windowY,
    int movementX, int movementY,
    bool ctrlKey, bool altKey, bool shiftKey, bool metaKey,
    short button, unsigned short buttons,
    PassRefPtrWillBeRawPtr<EventTarget> relatedTarget, DataTransfer* dataTransfer, bool isSimulated, PlatformMouseEvent::SyntheticEventType syntheticEventType,
    double uiCreateTime)
{
    return adoptRefWillBeNoop(new MouseEvent(type, canBubble, cancelable, view,
        detail, screenX, screenY, windowX, windowY,
        movementX, movementY,
        ctrlKey, altKey, shiftKey, metaKey, button, buttons, relatedTarget, dataTransfer, isSimulated, syntheticEventType, uiCreateTime));
}

MouseEvent::MouseEvent()
    : m_button(0)
    , m_buttons(0)
    , m_relatedTarget(nullptr)
    , m_dataTransfer(nullptr)
    , m_syntheticEventType(PlatformMouseEvent::RealOrIndistinguishable)
{
}

MouseEvent::MouseEvent(const AtomicString& eventType, bool canBubble, bool cancelable, PassRefPtrWillBeRawPtr<AbstractView> view,
    int detail, int screenX, int screenY, int windowX, int windowY,
    int movementX, int movementY,
    bool ctrlKey, bool altKey, bool shiftKey, bool metaKey,
    short button, unsigned short buttons, PassRefPtrWillBeRawPtr<EventTarget> relatedTarget,
    DataTransfer* dataTransfer, bool isSimulated, PlatformMouseEvent::SyntheticEventType syntheticEventType,
    double uiCreateTime)
    : MouseRelatedEvent(eventType, canBubble, cancelable, view, detail, IntPoint(screenX, screenY),
        IntPoint(windowX, windowY),
        IntPoint(movementX, movementY),
        ctrlKey, altKey, shiftKey, metaKey, isSimulated,
        syntheticEventType == PlatformMouseEvent::FromTouch ? InputDevice::firesTouchEventsInputDevice() : InputDevice::doesntFireTouchEventsInputDevice())
    , m_button(button)
    , m_buttons(buttons)
    , m_relatedTarget(relatedTarget)
    , m_dataTransfer(dataTransfer)
    , m_syntheticEventType(syntheticEventType)
{
    setUICreateTime(uiCreateTime);
}

MouseEvent::MouseEvent(const AtomicString& eventType, const MouseEventInit& initializer)
    : MouseRelatedEvent(eventType, initializer.bubbles(), initializer.cancelable(), initializer.view(), initializer.detail(), IntPoint(initializer.screenX(), initializer.screenY()),
        IntPoint(0 /* pageX */, 0 /* pageY */),
        IntPoint(initializer.movementX(), initializer.movementY()),
        initializer.ctrlKey(), initializer.altKey(), initializer.shiftKey(), initializer.metaKey(), false /* isSimulated */, initializer.sourceDevice())
    , m_button(initializer.button())
    , m_buttons(initializer.buttons())
    , m_relatedTarget(initializer.relatedTarget())
    , m_dataTransfer(nullptr)
    , m_syntheticEventType(PlatformMouseEvent::RealOrIndistinguishable)
{
    initCoordinates(IntPoint(initializer.clientX(), initializer.clientY()));
}

MouseEvent::~MouseEvent()
{
}

unsigned short MouseEvent::platformModifiersToButtons(unsigned modifiers)
{
    unsigned short buttons = 0;

    if (modifiers & PlatformEvent::LeftButtonDown)
        buttons |= 1;
    if (modifiers & PlatformEvent::RightButtonDown)
        buttons |= 2;
    if (modifiers & PlatformEvent::MiddleButtonDown)
        buttons |= 4;

    return buttons;
}

void MouseEvent::initMouseEvent(ScriptState* scriptState, const AtomicString& type, bool canBubble, bool cancelable, PassRefPtrWillBeRawPtr<AbstractView> view,
                                int detail, int screenX, int screenY, int clientX, int clientY,
                                bool ctrlKey, bool altKey, bool shiftKey, bool metaKey,
                                short button, PassRefPtrWillBeRawPtr<EventTarget> relatedTarget, unsigned short buttons)
{
    initMouseEventInternal(scriptState, type, canBubble, cancelable, view, detail, screenX, screenY, clientX, clientY, ctrlKey, altKey, shiftKey, metaKey, button, relatedTarget, nullptr, buttons);
}

void MouseEvent::initMouseEventInternal(ScriptState* scriptState, const AtomicString& type, bool canBubble, bool cancelable, PassRefPtrWillBeRawPtr<AbstractView> view,
    int detail, int screenX, int screenY, int clientX, int clientY, bool ctrlKey, bool altKey, bool shiftKey, bool metaKey,
    short button, PassRefPtrWillBeRawPtr<EventTarget> relatedTarget, InputDevice* sourceDevice, unsigned short buttons)
{
    if (dispatched())
        return;

    if (scriptState && scriptState->world().isIsolatedWorld())
        UIEventWithKeyState::didCreateEventInIsolatedWorld(ctrlKey, altKey, shiftKey, metaKey);

    initUIEvent(type, canBubble, cancelable, view, detail);

    m_screenLocation = IntPoint(screenX, screenY);
    m_ctrlKey = ctrlKey;
    m_altKey = altKey;
    m_shiftKey = shiftKey;
    m_metaKey = metaKey;
    m_button = button;
    m_buttons = buttons;
    m_relatedTarget = relatedTarget;

    initCoordinates(IntPoint(clientX, clientY));

    // FIXME: m_isSimulated is not set to false here.
    // FIXME: m_dataTransfer is not set to nullptr here.
}

const AtomicString& MouseEvent::interfaceName() const
{
    return EventNames::MouseEvent;
}

bool MouseEvent::isMouseEvent() const
{
    return true;
}

bool MouseEvent::isDragEvent() const
{
    const AtomicString& t = type();
    return t == EventTypeNames::dragenter || t == EventTypeNames::dragover || t == EventTypeNames::dragleave || t == EventTypeNames::drop
               || t == EventTypeNames::dragstart|| t == EventTypeNames::drag || t == EventTypeNames::dragend;
}

int MouseEvent::which() const
{
    // For the DOM, the return values for left, middle and right mouse buttons are 0, 1, 2, respectively.
    // For the Netscape "which" property, the return values for left, middle and right mouse buttons are 1, 2, 3, respectively.
    // So we must add 1.
    return m_button + 1;
}

Node* MouseEvent::toElement() const
{
    // MSIE extension - "the object toward which the user is moving the mouse pointer"
    if (type() == EventTypeNames::mouseout || type() == EventTypeNames::mouseleave)
        return relatedTarget() ? relatedTarget()->toNode() : 0;

    return target() ? target()->toNode() : 0;
}

Node* MouseEvent::fromElement() const
{
    // MSIE extension - "object from which activation or the mouse pointer is exiting during the event" (huh?)
    if (type() != EventTypeNames::mouseout && type() != EventTypeNames::mouseleave)
        return relatedTarget() ? relatedTarget()->toNode() : 0;

    return target() ? target()->toNode() : 0;
}

DEFINE_TRACE(MouseEvent)
{
    visitor->trace(m_relatedTarget);
    visitor->trace(m_dataTransfer);
    MouseRelatedEvent::trace(visitor);
}

PassRefPtrWillBeRawPtr<SimulatedMouseEvent> SimulatedMouseEvent::create(const AtomicString& eventType, PassRefPtrWillBeRawPtr<AbstractView> view, PassRefPtrWillBeRawPtr<Event> underlyingEvent)
{
    return adoptRefWillBeNoop(new SimulatedMouseEvent(eventType, view, underlyingEvent));
}

SimulatedMouseEvent::~SimulatedMouseEvent()
{
}

SimulatedMouseEvent::SimulatedMouseEvent(const AtomicString& eventType, PassRefPtrWillBeRawPtr<AbstractView> view, PassRefPtrWillBeRawPtr<Event> underlyingEvent)
    : MouseEvent(eventType, true, true, view, 0, 0, 0, 0, 0, 0, 0, false, false, false, false, 0, 0,
        nullptr, nullptr, true, PlatformMouseEvent::RealOrIndistinguishable)
{
    if (UIEventWithKeyState* keyStateEvent = findEventWithKeyState(underlyingEvent.get())) {
        m_ctrlKey = keyStateEvent->ctrlKey();
        m_altKey = keyStateEvent->altKey();
        m_shiftKey = keyStateEvent->shiftKey();
        m_metaKey = keyStateEvent->metaKey();
    }
    setUnderlyingEvent(underlyingEvent);

    if (this->underlyingEvent() && this->underlyingEvent()->isMouseEvent()) {
        MouseEvent* mouseEvent = toMouseEvent(this->underlyingEvent());
        m_screenLocation = mouseEvent->screenLocation();
        initCoordinates(mouseEvent->clientLocation());
    }
}

DEFINE_TRACE(SimulatedMouseEvent)
{
    MouseEvent::trace(visitor);
}

PassRefPtrWillBeRawPtr<MouseEventDispatchMediator> MouseEventDispatchMediator::create(PassRefPtrWillBeRawPtr<MouseEvent> mouseEvent, MouseEventType mouseEventType)
{
    return adoptRefWillBeNoop(new MouseEventDispatchMediator(mouseEvent, mouseEventType));
}

MouseEventDispatchMediator::MouseEventDispatchMediator(PassRefPtrWillBeRawPtr<MouseEvent> mouseEvent, MouseEventType mouseEventType)
    : EventDispatchMediator(mouseEvent), m_mouseEventType(mouseEventType)
{
}

MouseEvent& MouseEventDispatchMediator::event() const
{
    return toMouseEvent(EventDispatchMediator::event());
}

bool MouseEventDispatchMediator::dispatchEvent(EventDispatcher& dispatcher) const
{
    if (isSyntheticMouseEvent()) {
        event().eventPath().adjustForRelatedTarget(dispatcher.node(), event().relatedTarget());
        return dispatcher.dispatch();
    }

    if (isDisabledFormControl(&dispatcher.node()))
        return false;

    if (event().type().isEmpty())
        return true; // Shouldn't happen.

    ASSERT(!event().target() || event().target() != event().relatedTarget());

    EventTarget* relatedTarget = event().relatedTarget();
    event().eventPath().adjustForRelatedTarget(dispatcher.node(), relatedTarget);

    dispatcher.dispatch();
    bool swallowEvent = event().defaultHandled() || event().defaultPrevented();

    if (event().type() != EventTypeNames::click || event().detail() != 2)
        return !swallowEvent;

    // Special case: If it's a double click event, we also send the dblclick event. This is not part
    // of the DOM specs, but is used for compatibility with the ondblclick="" attribute. This is treated
    // as a separate event in other DOM-compliant browsers like Firefox, and so we do the same.
    RefPtrWillBeRawPtr<MouseEvent> doubleClickEvent = MouseEvent::create();
    doubleClickEvent->initMouseEventInternal(nullptr, EventTypeNames::dblclick, event().bubbles(), event().cancelable(), event().view(),
        event().detail(), event().screenX(), event().screenY(), event().clientX(), event().clientY(),
        event().ctrlKey(), event().altKey(), event().shiftKey(), event().metaKey(),
        event().button(), relatedTarget, event().sourceDevice(), event().buttons());
    if (event().defaultHandled())
        doubleClickEvent->setDefaultHandled();
    EventDispatcher::dispatchEvent(dispatcher.node(), MouseEventDispatchMediator::create(doubleClickEvent));
    if (doubleClickEvent->defaultHandled() || doubleClickEvent->defaultPrevented())
        return false;
    return !swallowEvent;
}

} // namespace blink
