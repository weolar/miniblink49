// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_DEVICE_DEVICE_MANAGER_H_
#define UI_EVENTS_OZONE_DEVICE_DEVICE_MANAGER_H_

#include "base/memory/scoped_ptr.h"
#include "ui/events/ozone/events_ozone_export.h"

namespace ui {

class DeviceEventObserver;

class EVENTS_OZONE_EXPORT DeviceManager {
public:
    virtual ~DeviceManager() { }

    // Scans the currently available devices and notifies |observer| for each
    // device found. If also registering for notifications through AddObserver(),
    // the scan should happen after the registration otherwise the observer may
    // miss events.
    virtual void ScanDevices(DeviceEventObserver* observer) = 0;

    // Registers |observer| for device event notifications.
    virtual void AddObserver(DeviceEventObserver* observer) = 0;

    // Removes |observer| from the list of observers notified.
    virtual void RemoveObserver(DeviceEventObserver* observer) = 0;
};

EVENTS_OZONE_EXPORT scoped_ptr<DeviceManager> CreateDeviceManager();

} // namespace ui

#endif // UI_EVENTS_OZONE_DEVICE_DEVICE_MANAGER_H_
