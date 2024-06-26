// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_EVENT_CONVERTER_EVDEV_H_
#define UI_EVENTS_OZONE_EVDEV_EVENT_CONVERTER_EVDEV_H_

#include <stdint.h>

#include <set>

#include "base/callback.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "ui/events/devices/input_device.h"
#include "ui/events/ozone/evdev/event_dispatch_callback.h"
#include "ui/events/ozone/evdev/events_ozone_evdev_export.h"
#include "ui/gfx/geometry/size.h"

struct input_event;

namespace ui {
enum class DomCode;

class EVENTS_OZONE_EVDEV_EXPORT EventConverterEvdev
    : public base::MessagePumpLibevent::Watcher {
public:
    EventConverterEvdev(int fd,
        const base::FilePath& path,
        int id,
        InputDeviceType type,
        const std::string& name,
        uint16_t vendor_id,
        uint16_t product_id);
    ~EventConverterEvdev() override;

    int id() const { return input_device_.id; }

    const base::FilePath& path() const { return path_; }

    InputDeviceType type() const { return input_device_.type; }

    const InputDevice& input_device() const { return input_device_; }

    // Start reading events.
    void Start();

    // Stop reading events.
    void Stop();

    // Enable or disable this device. A disabled device still polls for
    // input and can update state but must not dispatch any events to UI.
    void SetEnabled(bool enabled);

    // Cleanup after we stop reading events (release buttons, etc).
    virtual void OnStopped();

    // Prepare for disable (e.g. should release keys/buttons/touches).
    virtual void OnDisabled();

    // Start or restart (e.g. should reapply keys/buttons/touches).
    virtual void OnEnabled();

    // Dump recent events into a file.
    virtual void DumpTouchEventLog(const char* filename);

    // Returns true if the converter is used for a keyboard device.
    virtual bool HasKeyboard() const;

    // Returns true if the converter is used for a mouse device;
    virtual bool HasMouse() const;

    // Returns true if the converter is used for a touchpad device.
    virtual bool HasTouchpad() const;

    // Returns true if the converter is used for a touchscreen device.
    virtual bool HasTouchscreen() const;

    // Returns true if the converter is used for a device with a caps lock LED.
    virtual bool HasCapsLockLed() const;

    // Returns the size of the touchscreen device if the converter is used for a
    // touchscreen device.
    virtual gfx::Size GetTouchscreenSize() const;

    // Returns the number of touch points this device supports. Should not be
    // called unless HasTouchscreen() returns true
    virtual int GetTouchPoints() const;

    // Sets which keyboard keys should be processed. If |enable_filter| is
    // false, all keys are allowed and |allowed_keys| is ignored.
    virtual void SetKeyFilter(bool enable_filter,
        std::vector<DomCode> allowed_keys);

    // Update caps lock LED state.
    virtual void SetCapsLockLed(bool enabled);

    // Update touch event logging state.
    virtual void SetTouchEventLoggingEnabled(bool enabled);

    // Helper to generate a base::TimeDelta from an input_event's time
    static base::TimeDelta TimeDeltaFromInputEvent(const input_event& event);

protected:
    // base::MessagePumpLibevent::Watcher:
    void OnFileCanWriteWithoutBlocking(int fd) override;

    // File descriptor to read.
    int fd_;

    // Path to input device.
    base::FilePath path_;

    // Input device information, including id (which uniquely identifies an
    // event converter) and type.
    InputDevice input_device_;

    // Whether we're polling for input on the device.
    bool watching_ = false;

    // Whether events should be dispatched to UI.
    bool enabled_ = false;

    // Controller for watching the input fd.
    base::MessagePumpLibevent::FileDescriptorWatcher controller_;

private:
    DISALLOW_COPY_AND_ASSIGN(EventConverterEvdev);
};

} // namespace ui

#endif // UI_EVENTS_OZONE_EVDEV_EVENT_CONVERTER_EVDEV_H_
