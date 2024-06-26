// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_TEST_DEVICE_DATA_MANAGER_TEST_API_H_
#define UI_EVENTS_TEST_DEVICE_DATA_MANAGER_TEST_API_H_

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "ui/events/devices/events_devices_export.h"

namespace ui {

class DeviceDataManager;

namespace test {

    // Test API class to access internals of the DeviceDataManager class.
    class DeviceDataManagerTestAPI {
    public:
        // Constructs a test api that provides access to the global DeviceDataManager
        // instance that is accessible by DeviceDataManager::GetInstance().
        DeviceDataManagerTestAPI();
        ~DeviceDataManagerTestAPI();

        // Wrapper functions to DeviceDataManager.
        void NotifyObserversTouchscreenDeviceConfigurationChanged();
        void NotifyObserversKeyboardDeviceConfigurationChanged();
        void NotifyObserversMouseDeviceConfigurationChanged();
        void NotifyObserversTouchpadDeviceConfigurationChanged();
        void NotifyObserversDeviceListsComplete();
        void OnDeviceListsComplete();

    private:
        DISALLOW_COPY_AND_ASSIGN(DeviceDataManagerTestAPI);
    };

} // namespace test
} // namespace ui

#endif // UI_EVENTS_TEST_DEVICE_DATA_MANAGER_TEST_API_H_
