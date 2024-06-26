// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_PLATFORM_X11_X11_HOTPLUG_EVENT_HANDLER_H_
#define UI_EVENTS_PLATFORM_X11_X11_HOTPLUG_EVENT_HANDLER_H_

#include "base/macros.h"
#include "ui/events/devices/x11/device_list_cache_x11.h"
#include "ui/gfx/x/x11_atom_cache.h"

namespace ui {

class DeviceHotplugEventObserver;

// Parses X11 native devices and propagates the list of active devices to an
// observer.
class X11HotplugEventHandler {
public:
    X11HotplugEventHandler();
    ~X11HotplugEventHandler();

    // Called on an X11 hotplug event.
    void OnHotplugEvent();

private:
    X11AtomCache atom_cache_;

    DISALLOW_COPY_AND_ASSIGN(X11HotplugEventHandler);
};

} // namespace ui

#endif // UI_EVENTS_PLATFORM_X11_X11_HOTPLUG_EVENT_HANDLER_H_
