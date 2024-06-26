// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_H_
#define UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_H_

#include <stdint.h>

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "ui/events/events_export.h"
#include "ui/gfx/x/x11_types.h"

typedef union _XEvent XEvent;
typedef unsigned long XID;

namespace ui {

class X11HotplugEventHandler;

// Responsible for notifying X11EventSource when new XEvents are available and
// processing/dispatching XEvents. Implementations will likely be a
// PlatformEventSource.
class X11EventSourceDelegate {
public:
    X11EventSourceDelegate() = default;

    // Processes (if necessary) and handles dispatching XEvents.
    virtual void ProcessXEvent(XEvent* xevent) = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(X11EventSourceDelegate);
};

// Receives X11 events and sends them to X11EventSourceDelegate. Handles
// receiving, pre-process and post-processing XEvents.
class EVENTS_EXPORT X11EventSource {
public:
    X11EventSource(X11EventSourceDelegate* delegate, XDisplay* display);
    ~X11EventSource();

    static X11EventSource* GetInstance();

    // Called when there is a new XEvent available. Processes all (if any)
    // available X events.
    void DispatchXEvents();

    // Blocks on the X11 event queue until we receive notification from the
    // xserver that |w| has been mapped; StructureNotifyMask events on |w| are
    // pulled out from the queue and dispatched out of order.
    //
    // For those that know X11, this is really a wrapper around XWindowEvent
    // which still makes sure the preempted event is dispatched instead of
    // dropped on the floor. This method exists because mapping a window is
    // asynchronous (and we receive an XEvent when mapped), while there are also
    // functions which require a mapped window.
    void BlockUntilWindowMapped(XID window);

    XDisplay* display() { return display_; }

    void StopCurrentEventStream();
    void OnDispatcherListChanged();

protected:
    // Extracts cookie data from |xevent| if it's of GenericType, and dispatches
    // the event. This function also frees up the cookie data after dispatch is
    // complete.
    void ExtractCookieDataDispatchEvent(XEvent* xevent);

    // Handles updates after event has been dispatched.
    void PostDispatchEvent(XEvent* xevent);

private:
    static X11EventSource* instance_;

    X11EventSourceDelegate* delegate_;

    // The connection to the X11 server used to receive the events.
    XDisplay* display_;

    // Keeps track of whether this source should continue to dispatch all the
    // available events.
    bool continue_stream_ = true;

    scoped_ptr<X11HotplugEventHandler> hotplug_event_handler_;

    DISALLOW_COPY_AND_ASSIGN(X11EventSource);
};

} // namespace ui

#endif // UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_H_
