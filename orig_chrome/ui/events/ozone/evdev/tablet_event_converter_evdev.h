// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_TABLET_EVENT_CONVERTER_EVDEV_H_
#define UI_EVENTS_OZONE_TABLET_EVENT_CONVERTER_EVDEV_H_

#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/message_loop/message_pump_libevent.h"
#include "ui/events/event.h"
#include "ui/events/ozone/evdev/cursor_delegate_evdev.h"
#include "ui/events/ozone/evdev/event_converter_evdev.h"
#include "ui/events/ozone/evdev/event_device_info.h"
#include "ui/events/ozone/evdev/event_modifiers_evdev.h"
#include "ui/events/ozone/evdev/events_ozone_evdev_export.h"

struct input_event;

namespace ui {

class DeviceEventDispatcherEvdev;

class EVENTS_OZONE_EVDEV_EXPORT TabletEventConverterEvdev
    : public EventConverterEvdev {
public:
    TabletEventConverterEvdev(int fd,
        base::FilePath path,
        int id,
        CursorDelegateEvdev* cursor,
        const EventDeviceInfo& info,
        DeviceEventDispatcherEvdev* dispatcher);
    ~TabletEventConverterEvdev() override;

    // EventConverterEvdev:
    void OnFileCanReadWithoutBlocking(int fd) override;

    void ProcessEvents(const struct input_event* inputs, int count);

private:
    friend class MockTabletEventConverterEvdev;
    void ConvertKeyEvent(const input_event& input);
    void ConvertAbsEvent(const input_event& input);
    void DispatchMouseButton(const input_event& input);
    void UpdateCursor();

    // Flush events delimited by EV_SYN. This is useful for handling
    // non-axis-aligned movement properly.
    void FlushEvents(const input_event& input);

    // Controller for watching the input fd.
    base::MessagePumpLibevent::FileDescriptorWatcher controller_;

    // Shared cursor state.
    CursorDelegateEvdev* cursor_;

    // Dispatcher for events.
    DeviceEventDispatcherEvdev* dispatcher_;

    int y_abs_location_ = 0;
    int x_abs_location_ = 0;
    int x_abs_min_;
    int y_abs_min_;
    int x_abs_range_;
    int y_abs_range_;

    int tilt_x_min_;
    int tilt_x_range_;
    int tilt_y_min_;
    int tilt_y_range_;

    float tilt_x_ = 0.0f;
    float tilt_y_ = 0.0f;
    float pressure_ = 0.0f;
    int pressure_max_;

    // BTN_TOOL_ code for the active device
    int stylus_ = 0;

    // Whether we need to move the cursor
    bool abs_value_dirty_ = false;

    // Set if we drop events in kernel (SYN_DROPPED) or in process.
    bool dropped_events_ = false;

    DISALLOW_COPY_AND_ASSIGN(TabletEventConverterEvdev);
};

} // namespace ui

#endif // UI_EVENTS_OZONE_TABLET_EVENT_CONVERTER_EVDEV_H_
