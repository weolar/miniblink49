// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_DEVICE_DEVICE_MANAGER_MANUAL_H_
#define UI_EVENTS_OZONE_DEVICE_DEVICE_MANAGER_MANUAL_H_

#include <vector>

#include "base/macros.h"
#include "base/observer_list.h"
#include "ui/events/ozone/device/device_manager.h"

namespace base {
class FilePath;
}

namespace ui {

class DeviceManagerManual : public DeviceManager {
public:
    DeviceManagerManual();
    ~DeviceManagerManual() override;

private:
    // DeviceManager overrides:
    void ScanDevices(DeviceEventObserver* observer) override;
    void AddObserver(DeviceEventObserver* observer) override;
    void RemoveObserver(DeviceEventObserver* observer) override;

    void OnDevicesScanned(std::vector<base::FilePath>* result);

    bool have_scanned_devices_;
    std::vector<base::FilePath> devices_;
    base::ObserverList<DeviceEventObserver> observers_;

    base::WeakPtrFactory<DeviceManagerManual> weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(DeviceManagerManual);
};

} // namespace ui

#endif // UI_EVENTS_OZONE_DEVICE_DEVICE_MANAGER_MANUAL_H_
