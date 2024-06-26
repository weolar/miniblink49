// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <linux/input.h>
#include <stddef.h>

#include "ui/events/ozone/evdev/event_converter_evdev.h"

#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/trace_event/trace_event.h"
#include "ui/events/devices/device_util_linux.h"
#include "ui/events/devices/input_device.h"

namespace ui {

EventConverterEvdev::EventConverterEvdev(int fd,
    const base::FilePath& path,
    int id,
    InputDeviceType type,
    const std::string& name,
    uint16_t vendor_id,
    uint16_t product_id)
    : fd_(fd)
    , path_(path)
    , input_device_(id, type, name, GetInputPathInSys(path), vendor_id,
          product_id)
{
}

EventConverterEvdev::~EventConverterEvdev()
{
    DCHECK(!enabled_);
    DCHECK(!watching_);
    if (fd_ >= 0)
        close(fd_);
}

void EventConverterEvdev::Start()
{
    base::MessageLoopForUI::current()->WatchFileDescriptor(
        fd_, true, base::MessagePumpLibevent::WATCH_READ, &controller_, this);
    watching_ = true;
}

void EventConverterEvdev::Stop()
{
    controller_.StopWatchingFileDescriptor();
    watching_ = false;
}

void EventConverterEvdev::SetEnabled(bool enabled)
{
    if (enabled == enabled_)
        return;
    if (enabled) {
        TRACE_EVENT1("evdev", "EventConverterEvdev::OnEnabled", "path",
            path_.value());
        OnEnabled();
    } else {
        TRACE_EVENT1("evdev", "EventConverterEvdev::OnDisabled", "path",
            path_.value());
        OnDisabled();
    }
    enabled_ = enabled;
}

void EventConverterEvdev::OnStopped()
{
}

void EventConverterEvdev::OnEnabled()
{
}

void EventConverterEvdev::OnDisabled()
{
}

void EventConverterEvdev::DumpTouchEventLog(const char* filename)
{
}

void EventConverterEvdev::OnFileCanWriteWithoutBlocking(int fd)
{
    NOTREACHED();
}

bool EventConverterEvdev::HasKeyboard() const
{
    return false;
}

bool EventConverterEvdev::HasMouse() const
{
    return false;
}

bool EventConverterEvdev::HasTouchpad() const
{
    return false;
}

bool EventConverterEvdev::HasTouchscreen() const
{
    return false;
}

bool EventConverterEvdev::HasCapsLockLed() const
{
    return false;
}

gfx::Size EventConverterEvdev::GetTouchscreenSize() const
{
    NOTREACHED();
    return gfx::Size();
}

int EventConverterEvdev::GetTouchPoints() const
{
    NOTREACHED();
    return 0;
}

void EventConverterEvdev::SetKeyFilter(bool enable_filter,
    std::vector<DomCode> allowed_keys)
{
    NOTREACHED();
}

void EventConverterEvdev::SetCapsLockLed(bool enabled)
{
    if (!HasCapsLockLed())
        return;

    input_event events[2];
    memset(&events, 0, sizeof(events));

    events[0].type = EV_LED;
    events[0].code = LED_CAPSL;
    events[0].value = enabled;

    events[1].type = EV_SYN;
    events[1].code = SYN_REPORT;
    events[1].value = 0;

    ssize_t written = write(fd_, events, sizeof(events));

    if (written < 0) {
        if (errno != ENODEV)
            PLOG(ERROR) << "cannot set leds for " << path_.value() << ":";
        Stop();
    } else if (written != sizeof(events)) {
        LOG(ERROR) << "short write setting leds for " << path_.value();
        Stop();
    }
}

void EventConverterEvdev::SetTouchEventLoggingEnabled(bool enabled)
{
}

base::TimeDelta EventConverterEvdev::TimeDeltaFromInputEvent(
    const input_event& event)
{
    return base::TimeDelta::FromMicroseconds(
        static_cast<int64_t>(event.time.tv_sec) * 1000000L + event.time.tv_usec);
}
} // namespace ui
