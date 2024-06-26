// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_GLIB_H_
#define UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_GLIB_H_

#include <stdint.h>

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "ui/events/events_export.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/events/platform/x11/x11_event_source.h"
#include "ui/gfx/x/x11_types.h"

typedef struct _GPollFD GPollFD;
typedef struct _GSource GSource;

namespace ui {

// A PlatformEventSource implementation for X11. Dispatches XEvents and uses
// Glib to be notified for incoming XEvents.
class EVENTS_EXPORT X11EventSourceGlib : public X11EventSourceDelegate,
                                         public PlatformEventSource {
public:
    explicit X11EventSourceGlib(XDisplay* display);
    ~X11EventSourceGlib() override;

    // X11EventSourceDelegate:
    void ProcessXEvent(XEvent* xevent) override;

private:
    // PlatformEventSource:
    void StopCurrentEventStream() override;
    void OnDispatcherListChanged() override;

    void InitXSource(int fd);

    X11EventSource event_source_;

    // The GLib event source for X events.
    GSource* x_source_ = nullptr;

    // The poll attached to |x_source_|.
    scoped_ptr<GPollFD> x_poll_;

    DISALLOW_COPY_AND_ASSIGN(X11EventSourceGlib);
};

} // namespace ui

#endif // UI_EVENTS_PLATFORM_X11_X11_EVENT_SOURCE_GLIB_H_
