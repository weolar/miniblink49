// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_LIBEVENT_H_
#define UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_LIBEVENT_H_

#include "base/macros.h"
#include "base/message_loop/message_pump_libevent.h"
#include "ui/events/events_export.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/events/platform/x11/x11_event_source.h"

namespace ui {

// Interface for classes that want to receive XEvent directly. Only used with
// Ozone X11 currently and only events that can't be translated into ui::Events
// are sent via this path.
class EVENTS_EXPORT XEventDispatcher {
public:
    // Sends XEvent to XEventDispatcher for handling. Returns true if the XEvent
    // was dispatched, otherwise false. After the first XEventDispatcher returns
    // true XEvent dispatching stops.
    virtual bool DispatchXEvent(XEvent* xevent) = 0;

protected:
    virtual ~XEventDispatcher() { }
};

// A PlatformEventSource implementation for Ozone X11. Converts XEvents to
// ui::Events before dispatching.  For X11 specific events a separate list of
// XEventDispatchers is maintained. Uses Libevent to be notified for incoming
// XEvents.
class EVENTS_EXPORT X11EventSourceLibevent
    : public X11EventSourceDelegate,
      public PlatformEventSource,
      public base::MessagePumpLibevent::Watcher {
public:
    explicit X11EventSourceLibevent(XDisplay* display);
    ~X11EventSourceLibevent() override;

    // Adds a XEvent dispatcher to the XEvent dispatcher list.
    void AddXEventDispatcher(XEventDispatcher* dispatcher);

    // Removes a XEvent dispatcher fERrom the XEvent dispatcher list.
    void RemoveXEventDispatcher(XEventDispatcher* dispatcher);

    // X11EventSourceDelegate:
    void ProcessXEvent(XEvent* xevent) override;

private:
    // Registers event watcher with Libevent.
    void AddEventWatcher();

    // Sends XEvent to registered XEventDispatchers.
    void DispatchXEventToXEventDispatchers(XEvent* xevent);

    // PlatformEventSource:
    void StopCurrentEventStream() override;
    void OnDispatcherListChanged() override;

    // base::MessagePumpLibevent::Watcher:
    void OnFileCanReadWithoutBlocking(int fd) override;
    void OnFileCanWriteWithoutBlocking(int fd) override;

    X11EventSource event_source_;

    // Keep track of all XEventDispatcher to send XEvents directly to.
    base::ObserverList<XEventDispatcher> dispatchers_xevent_;

    base::MessagePumpLibevent::FileDescriptorWatcher watcher_controller_;
    bool initialized_ = false;

    DISALLOW_COPY_AND_ASSIGN(X11EventSourceLibevent);
};

} // namespace ui

#endif // UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_LIBEVENT_H_
