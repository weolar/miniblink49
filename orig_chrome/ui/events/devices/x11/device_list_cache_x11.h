// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_DEVICES_X11_DEVICE_LIST_CACHE_X11_H_
#define UI_EVENTS_DEVICES_X11_DEVICE_LIST_CACHE_X11_H_

#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

#include <map>

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "ui/events/devices/events_devices_export.h"
#include "ui/gfx/x/x11_types.h"

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}

typedef struct _XDisplay Display;

template <typename T, void (*D)(T*)>
struct DeviceList {
    DeviceList()
        : count(0)
    {
    }
    T& operator[](int x) { return devices[x]; }
    const T& operator[](int x) const { return devices[x]; }
    scoped_ptr<T[], gfx::XObjectDeleter<T, void, D>> devices;
    int count;
};

typedef struct DeviceList<XDeviceInfo, XFreeDeviceList> XDeviceList;
typedef struct DeviceList<XIDeviceInfo, XIFreeDeviceInfo> XIDeviceList;

namespace ui {

// A class to cache the current XInput device list. This minimized the
// round-trip time to the X server whenever such a device list is needed. The
// update function will be called on each incoming XI_HierarchyChanged event.
class EVENTS_DEVICES_EXPORT DeviceListCacheX11 {
public:
    static DeviceListCacheX11* GetInstance();

    void UpdateDeviceList(Display* display);

    // Returns the list of devices associated with |display|. Uses the old X11
    // protocol to get the list of the devices.
    const XDeviceList& GetXDeviceList(Display* display);

    // Returns the list of devices associated with |display|. Uses the newer
    // XINPUT2 protocol to get the list of devices. Before making this call, make
    // sure that XInput2 support is available (e.g. by calling
    // IsXInput2Available()).
    const XIDeviceList& GetXI2DeviceList(Display* display);

private:
    friend struct base::DefaultSingletonTraits<DeviceListCacheX11>;

    DeviceListCacheX11();
    ~DeviceListCacheX11();

    XDeviceList x_dev_list_;
    XIDeviceList xi_dev_list_;

    DISALLOW_COPY_AND_ASSIGN(DeviceListCacheX11);
};

} // namespace ui

#endif // UI_EVENTS_DEVICES_X11_DEVICE_LIST_CACHE_X11_H_
