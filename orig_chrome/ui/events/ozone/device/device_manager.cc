// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/device/device_manager.h"

#include "base/trace_event/trace_event.h"

#if defined(USE_UDEV)
#include "ui/events/ozone/device/udev/device_manager_udev.h"
#else
#include "ui/events/ozone/device/device_manager_manual.h"
#endif

namespace ui {

scoped_ptr<DeviceManager> CreateDeviceManager()
{
    TRACE_EVENT0("ozone", "CreateDeviceManager");
#if defined(USE_UDEV)
    return make_scoped_ptr(new DeviceManagerUdev());
#else
    return make_scoped_ptr(new DeviceManagerManual());
#endif
}

} // namespace ui
