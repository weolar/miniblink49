// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/DeviceSingleWindowEventController.h"

#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/page/Page.h"

namespace blink {

DeviceSingleWindowEventController::DeviceSingleWindowEventController(Document& document)
    : PlatformEventController(document.page())
    , DOMWindowLifecycleObserver(document.domWindow())
    , m_needsCheckingNullEvents(true)
    , m_document(document)
{
}

DeviceSingleWindowEventController::~DeviceSingleWindowEventController()
{
}

void DeviceSingleWindowEventController::didUpdateData()
{
    dispatchDeviceEvent(lastEvent());
}

void DeviceSingleWindowEventController::dispatchDeviceEvent(PassRefPtrWillBeRawPtr<Event> prpEvent)
{
    if (!document().domWindow() || document().activeDOMObjectsAreSuspended() || document().activeDOMObjectsAreStopped())
        return;

    RefPtrWillBeRawPtr<Event> event = prpEvent;
    document().domWindow()->dispatchEvent(event);

    if (m_needsCheckingNullEvents) {
        if (isNullEvent(event.get()))
            stopUpdating();
        else
            m_needsCheckingNullEvents = false;
    }
}

void DeviceSingleWindowEventController::didAddEventListener(LocalDOMWindow* window, const AtomicString& eventType)
{
    if (eventType != eventTypeName())
        return;

    if (page() && page()->visibilityState() == PageVisibilityStateVisible)
        startUpdating();

    m_hasEventListener = true;
}

void DeviceSingleWindowEventController::didRemoveEventListener(LocalDOMWindow* window, const AtomicString& eventType)
{
    if (eventType != eventTypeName() || window->hasEventListeners(eventTypeName()))
        return;

    stopUpdating();
    m_hasEventListener = false;
}

void DeviceSingleWindowEventController::didRemoveAllEventListeners(LocalDOMWindow*)
{
    stopUpdating();
    m_hasEventListener = false;
}

DEFINE_TRACE(DeviceSingleWindowEventController)
{
    visitor->trace(m_document);
    PlatformEventController::trace(visitor);
    DOMWindowLifecycleObserver::trace(visitor);
}

} // namespace blink
