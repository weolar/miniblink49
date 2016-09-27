/*
 * Copyright 2008, The Android Open Source Project
 * Copyright (C) 2012 Research In Motion Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
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

#include "core/events/TouchEvent.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/events/EventDispatcher.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/inspector/ConsoleMessage.h"

namespace blink {

TouchEvent::TouchEvent()
{
}

TouchEvent::TouchEvent(TouchList* touches, TouchList* targetTouches,
        TouchList* changedTouches, const AtomicString& type,
        PassRefPtrWillBeRawPtr<AbstractView> view,
        bool ctrlKey, bool altKey, bool shiftKey, bool metaKey, bool cancelable, bool causesScrollingIfUncanceled,
        double uiCreateTime)
    // Pass a sourceDevice which fires touchevents when creating this touchevent, which is always created from input devices from EventHandler.
    : UIEventWithKeyState(type, true, cancelable, view, 0, ctrlKey, altKey, shiftKey, metaKey, InputDevice::firesTouchEventsInputDevice())
    , m_touches(touches)
    , m_targetTouches(targetTouches)
    , m_changedTouches(changedTouches)
    , m_causesScrollingIfUncanceled(causesScrollingIfUncanceled)
{
    setUICreateTime(uiCreateTime);
}

TouchEvent::~TouchEvent()
{
}

void TouchEvent::initTouchEvent(ScriptState* scriptState, TouchList* touches, TouchList* targetTouches,
        TouchList* changedTouches, const AtomicString& type,
        PassRefPtrWillBeRawPtr<AbstractView> view,
        int, int, int, int,
        bool ctrlKey, bool altKey, bool shiftKey, bool metaKey)
{
    if (dispatched())
        return;

    if (scriptState->world().isIsolatedWorld())
        UIEventWithKeyState::didCreateEventInIsolatedWorld(ctrlKey, altKey, shiftKey, metaKey);

    bool cancelable = true;
    if (type == EventTypeNames::touchcancel)
        cancelable = false;

    initUIEvent(type, true, cancelable, view, 0);

    m_touches = touches;
    m_targetTouches = targetTouches;
    m_changedTouches = changedTouches;
    m_ctrlKey = ctrlKey;
    m_altKey = altKey;
    m_shiftKey = shiftKey;
    m_metaKey = metaKey;
}

const AtomicString& TouchEvent::interfaceName() const
{
    return EventNames::TouchEvent;
}

bool TouchEvent::isTouchEvent() const
{
    return true;
}

void TouchEvent::preventDefault()
{
    UIEventWithKeyState::preventDefault();

    // A common developer error is to wait too long before attempting to stop
    // scrolling by consuming a touchmove event. Generate a warning if this
    // event is uncancelable.
    if (!cancelable() && view() && view()->isLocalDOMWindow() && view()->frame()) {
        toLocalDOMWindow(view())->frame()->console().addMessage(ConsoleMessage::create(JSMessageSource, WarningMessageLevel,
            "Ignored attempt to cancel a " + type() + " event with cancelable=false, for example because scrolling is in progress and cannot be interrupted."));
    }
}
DEFINE_TRACE(TouchEvent)
{
    visitor->trace(m_touches);
    visitor->trace(m_targetTouches);
    visitor->trace(m_changedTouches);
    UIEventWithKeyState::trace(visitor);
}

PassRefPtrWillBeRawPtr<TouchEventDispatchMediator> TouchEventDispatchMediator::create(PassRefPtrWillBeRawPtr<TouchEvent> touchEvent)
{
    return adoptRefWillBeNoop(new TouchEventDispatchMediator(touchEvent));
}

TouchEventDispatchMediator::TouchEventDispatchMediator(PassRefPtrWillBeRawPtr<TouchEvent> touchEvent)
    : EventDispatchMediator(touchEvent)
{
}

TouchEvent& TouchEventDispatchMediator::event() const
{
    return toTouchEvent(EventDispatchMediator::event());
}

bool TouchEventDispatchMediator::dispatchEvent(EventDispatcher& dispatcher) const
{
    event().eventPath().adjustForTouchEvent(event());
    return dispatcher.dispatch();
}

} // namespace blink
