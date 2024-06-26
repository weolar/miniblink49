// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_TOUCH_EVENT_CONVERTER_EVDEV_H_
#define UI_EVENTS_OZONE_EVDEV_TOUCH_EVENT_CONVERTER_EVDEV_H_

#include <stddef.h>
#include <stdint.h>

#include <bitset>

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop/message_pump_libevent.h"
#include "ui/events/event_constants.h"
#include "ui/events/ozone/evdev/event_converter_evdev.h"
#include "ui/events/ozone/evdev/event_device_info.h"
#include "ui/events/ozone/evdev/events_ozone_evdev_export.h"
#include "ui/events/ozone/evdev/touch_evdev_debug_buffer.h"

namespace ui {

class DeviceEventDispatcherEvdev;
class TouchEvent;
class TouchNoiseFinder;
struct InProgressTouchEvdev;

class EVENTS_OZONE_EVDEV_EXPORT TouchEventConverterEvdev
    : public EventConverterEvdev {
public:
    TouchEventConverterEvdev(int fd,
        base::FilePath path,
        int id,
        const EventDeviceInfo& devinfo,
        DeviceEventDispatcherEvdev* dispatcher);
    ~TouchEventConverterEvdev() override;

    // EventConverterEvdev:
    bool HasTouchscreen() const override;
    gfx::Size GetTouchscreenSize() const override;
    int GetTouchPoints() const override;
    void OnEnabled() override;
    void OnDisabled() override;

    void DumpTouchEventLog(const char* filename) override;

    // Update touch event logging state
    void SetTouchEventLoggingEnabled(bool enabled) override;

    // Unsafe part of initialization.
    virtual void Initialize(const EventDeviceInfo& info);

private:
    friend class MockTouchEventConverterEvdev;

    // Overidden from base::MessagePumpLibevent::Watcher.
    void OnFileCanReadWithoutBlocking(int fd) override;

    virtual void Reinitialize();

    void ProcessMultitouchEvent(const input_event& input);
    void EmulateMultitouchEvent(const input_event& input);
    void ProcessKey(const input_event& input);
    void ProcessAbs(const input_event& input);
    void ProcessSyn(const input_event& input);

    // Returns an EventType to dispatch for |touch|. Returns ET_UNKNOWN if an
    // event should not be dispatched.
    EventType GetEventTypeForTouch(const InProgressTouchEvdev& touch);

    void ReportEvent(const InProgressTouchEvdev& event,
        EventType event_type,
        const base::TimeDelta& delta);
    void ReportEvents(base::TimeDelta delta);

    void UpdateTrackingId(int slot, int tracking_id);
    void ReleaseTouches();

    // Normalize pressure value to [0, 1].
    float ScalePressure(int32_t value);

    int NextTrackingId();

    // Dispatcher for events.
    DeviceEventDispatcherEvdev* dispatcher_;

    // Set if we drop events in kernel (SYN_DROPPED) or in process.
    bool dropped_events_ = false;

    // Device has multitouch capability.
    bool has_mt_ = false;

    // Use BTN_LEFT instead of BT_TOUCH.
    bool quirk_left_mouse_button_ = false;

    // Pressure values.
    int pressure_min_;
    int pressure_max_; // Used to normalize pressure values.

    // Input range for x-axis.
    float x_min_tuxels_;
    float x_num_tuxels_;

    // Input range for y-axis.
    float y_min_tuxels_;
    float y_num_tuxels_;

    // Number of touch points reported by driver
    int touch_points_ = 0;

    // Tracking id counter.
    int next_tracking_id_ = 0;

    // Touch point currently being updated from the /dev/input/event* stream.
    size_t current_slot_ = 0;

    // Flag that indicates if the touch logging enabled or not.
    bool touch_logging_enabled_ = true;

    // In-progress touch points.
    std::vector<InProgressTouchEvdev> events_;

    // Finds touch noise.
    scoped_ptr<TouchNoiseFinder> touch_noise_finder_;

    // Records the recent touch events. It is used to fill the feedback reports
    TouchEventLogEvdev touch_evdev_debug_buffer_;

    DISALLOW_COPY_AND_ASSIGN(TouchEventConverterEvdev);
};

} // namespace ui

#endif // UI_EVENTS_OZONE_EVDEV_TOUCH_EVENT_CONVERTER_EVDEV_H_
