// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/touch_event_converter_evdev.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>

#include <cmath>
#include <limits>

#include "base/bind.h"
#include "base/callback.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event.h"
#include "ui/events/devices/device_data_manager.h"
#include "ui/events/devices/device_util_linux.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/event_switches.h"
#include "ui/events/event_utils.h"
#include "ui/events/ozone/evdev/device_event_dispatcher_evdev.h"
#include "ui/events/ozone/evdev/touch_evdev_types.h"
#include "ui/events/ozone/evdev/touch_noise/touch_noise_finder.h"
#include "ui/ozone/public/input_controller.h"

namespace {

const int kMaxTrackingId = 0xffff; // TRKID_MAX in kernel.

struct TouchCalibration {
    int bezel_left;
    int bezel_right;
    int bezel_top;
    int bezel_bottom;
};

void GetTouchCalibration(TouchCalibration* cal)
{
    std::vector<std::string> parts = base::SplitString(
        base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
            switches::kTouchCalibration),
        ",", base::KEEP_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
    if (parts.size() >= 4) {
        if (!base::StringToInt(parts[0], &cal->bezel_left))
            LOG(ERROR) << "Incorrect left border calibration value passed.";
        if (!base::StringToInt(parts[1], &cal->bezel_right))
            LOG(ERROR) << "Incorrect right border calibration value passed.";
        if (!base::StringToInt(parts[2], &cal->bezel_top))
            LOG(ERROR) << "Incorrect top border calibration value passed.";
        if (!base::StringToInt(parts[3], &cal->bezel_bottom))
            LOG(ERROR) << "Incorrect bottom border calibration value passed.";
    }
}

int32_t AbsCodeToMtCode(int32_t code)
{
    switch (code) {
    case ABS_X:
        return ABS_MT_POSITION_X;
    case ABS_Y:
        return ABS_MT_POSITION_Y;
    case ABS_PRESSURE:
        return ABS_MT_PRESSURE;
    case ABS_DISTANCE:
        return ABS_MT_DISTANCE;
    default:
        return -1;
    }
}

ui::EventPointerType GetPointerTypeFromEvent(
    const ui::InProgressTouchEvdev& event)
{
    return (event.tool_code == BTN_TOOL_PEN)
        ? ui::EventPointerType::POINTER_TYPE_PEN
        : ui::EventPointerType::POINTER_TYPE_TOUCH;
}

const int kTrackingIdForUnusedSlot = -1;

} // namespace

namespace ui {

TouchEventConverterEvdev::TouchEventConverterEvdev(
    int fd,
    base::FilePath path,
    int id,
    const EventDeviceInfo& devinfo,
    DeviceEventDispatcherEvdev* dispatcher)
    : EventConverterEvdev(fd,
        path,
        id,
        devinfo.device_type(),
        devinfo.name(),
        devinfo.vendor_id(),
        devinfo.product_id())
    , dispatcher_(dispatcher)
{
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kExtraTouchNoiseFiltering)) {
        touch_noise_finder_.reset(new TouchNoiseFinder);
    }
    touch_evdev_debug_buffer_.Initialize(devinfo);
}

TouchEventConverterEvdev::~TouchEventConverterEvdev()
{
}

void TouchEventConverterEvdev::Initialize(const EventDeviceInfo& info)
{
    has_mt_ = info.HasMultitouch();

    if (has_mt_) {
        pressure_min_ = info.GetAbsMinimum(ABS_MT_PRESSURE);
        pressure_max_ = info.GetAbsMaximum(ABS_MT_PRESSURE);
        x_min_tuxels_ = info.GetAbsMinimum(ABS_MT_POSITION_X);
        x_num_tuxels_ = info.GetAbsMaximum(ABS_MT_POSITION_X) - x_min_tuxels_ + 1;
        y_min_tuxels_ = info.GetAbsMinimum(ABS_MT_POSITION_Y);
        y_num_tuxels_ = info.GetAbsMaximum(ABS_MT_POSITION_Y) - y_min_tuxels_ + 1;
        touch_points_ = std::min<int>(info.GetAbsMaximum(ABS_MT_SLOT) + 1, kNumTouchEvdevSlots);
        current_slot_ = info.GetAbsValue(ABS_MT_SLOT);
    } else {
        pressure_min_ = info.GetAbsMinimum(ABS_PRESSURE);
        pressure_max_ = info.GetAbsMaximum(ABS_PRESSURE);
        x_min_tuxels_ = info.GetAbsMinimum(ABS_X);
        x_num_tuxels_ = info.GetAbsMaximum(ABS_X) - x_min_tuxels_ + 1;
        y_min_tuxels_ = info.GetAbsMinimum(ABS_Y);
        y_num_tuxels_ = info.GetAbsMaximum(ABS_Y) - y_min_tuxels_ + 1;
        touch_points_ = 1;
        current_slot_ = 0;
    }

    quirk_left_mouse_button_ = !has_mt_ && !info.HasKeyEvent(BTN_TOUCH) && info.HasKeyEvent(BTN_LEFT);

    // Apply --touch-calibration.
    if (type() == INPUT_DEVICE_INTERNAL) {
        TouchCalibration cal = {};
        GetTouchCalibration(&cal);
        x_min_tuxels_ += cal.bezel_left;
        x_num_tuxels_ -= cal.bezel_left + cal.bezel_right;
        y_min_tuxels_ += cal.bezel_top;
        y_num_tuxels_ -= cal.bezel_top + cal.bezel_bottom;

        VLOG(1) << "applying touch calibration: "
                << base::StringPrintf("[%d, %d, %d, %d]", cal.bezel_left,
                       cal.bezel_right, cal.bezel_top,
                       cal.bezel_bottom);
    }

    events_.resize(touch_points_);

    if (has_mt_) {
        for (size_t i = 0; i < events_.size(); ++i) {
            events_[i].x = info.GetAbsMtSlotValueWithDefault(ABS_MT_POSITION_X, i, 0);
            events_[i].y = info.GetAbsMtSlotValueWithDefault(ABS_MT_POSITION_Y, i, 0);
            events_[i].tracking_id = info.GetAbsMtSlotValueWithDefault(
                ABS_MT_TRACKING_ID, i, kTrackingIdForUnusedSlot);
            events_[i].touching = (events_[i].tracking_id >= 0);
            events_[i].slot = i;

            // Dirty the slot so we'll update the consumer at the first opportunity.
            // We can't dispatch here as this is currently called on the worker pool.
            // TODO(spang): Move initialization off worker pool.
            events_[i].altered = true;

            // Optional bits.
            events_[i].radius_x = info.GetAbsMtSlotValueWithDefault(ABS_MT_TOUCH_MAJOR, i, 0) / 2.0f;
            events_[i].radius_y = info.GetAbsMtSlotValueWithDefault(ABS_MT_TOUCH_MINOR, i, 0) / 2.0f;
            events_[i].pressure = ScalePressure(
                info.GetAbsMtSlotValueWithDefault(ABS_MT_PRESSURE, i, 0));
        }
    } else {
        // TODO(spang): Add key state to EventDeviceInfo to allow initial contact.
        // (and make sure to take into account quirk_left_mouse_button_)
        events_[0].x = 0;
        events_[0].y = 0;
        events_[0].tracking_id = kTrackingIdForUnusedSlot;
        events_[0].touching = false;
        events_[0].slot = 0;
        events_[0].radius_x = 0;
        events_[0].radius_y = 0;
        events_[0].pressure = 0;
        events_[0].tool_code = 0;
    }
}

void TouchEventConverterEvdev::Reinitialize()
{
    EventDeviceInfo info;
    if (!info.Initialize(fd_, path_)) {
        LOG(ERROR) << "Failed to synchronize state for touch device: "
                   << path_.value();
        Stop();
        return;
    }

    Initialize(info);
}

bool TouchEventConverterEvdev::HasTouchscreen() const
{
    return true;
}

gfx::Size TouchEventConverterEvdev::GetTouchscreenSize() const
{
    return gfx::Size(x_num_tuxels_, y_num_tuxels_);
}

int TouchEventConverterEvdev::GetTouchPoints() const
{
    return touch_points_;
}

void TouchEventConverterEvdev::OnEnabled()
{
    ReportEvents(EventTimeForNow());
}

void TouchEventConverterEvdev::OnDisabled()
{
    ReleaseTouches();
}

void TouchEventConverterEvdev::OnFileCanReadWithoutBlocking(int fd)
{
    TRACE_EVENT1("evdev",
        "TouchEventConverterEvdev::OnFileCanReadWithoutBlocking", "fd",
        fd);

    input_event inputs[kNumTouchEvdevSlots * 6 + 1];
    ssize_t read_size = read(fd, inputs, sizeof(inputs));
    if (read_size < 0) {
        if (errno == EINTR || errno == EAGAIN)
            return;
        if (errno != ENODEV)
            PLOG(ERROR) << "error reading device " << path_.value();
        Stop();
        return;
    }

    if (!enabled_) {
        dropped_events_ = true;
        return;
    }

    for (unsigned i = 0; i < read_size / sizeof(*inputs); i++) {
        if (!has_mt_) {
            // Emulate the device as an MT device with only 1 slot by inserting extra
            // MT protocol events in the stream.
            EmulateMultitouchEvent(inputs[i]);
        }

        ProcessMultitouchEvent(inputs[i]);
    }
}

void TouchEventConverterEvdev::DumpTouchEventLog(const char* filename)
{
    touch_evdev_debug_buffer_.DumpLog(filename);
}

void TouchEventConverterEvdev::SetTouchEventLoggingEnabled(bool enabled)
{
    touch_logging_enabled_ = enabled;
}

void TouchEventConverterEvdev::ProcessMultitouchEvent(
    const input_event& input)
{
    if (touch_logging_enabled_)
        touch_evdev_debug_buffer_.ProcessEvent(current_slot_, &input);

    if (input.type == EV_SYN) {
        ProcessSyn(input);
    } else if (dropped_events_) {
        // Do nothing. This branch indicates we have lost sync with the driver.
    } else if (input.type == EV_ABS) {
        if (events_.size() <= current_slot_) {
            LOG(ERROR) << "current_slot_ (" << current_slot_
                       << ") >= events_.size() (" << events_.size() << ")";
        } else {
            ProcessAbs(input);
        }
    } else if (input.type == EV_KEY) {
        ProcessKey(input);
    } else if (input.type == EV_MSC) {
        // Ignored.
    } else {
        NOTIMPLEMENTED() << "invalid type: " << input.type;
    }
}

void TouchEventConverterEvdev::EmulateMultitouchEvent(
    const input_event& event)
{
    input_event emulated_event = event;

    if (event.type == EV_ABS) {
        emulated_event.code = AbsCodeToMtCode(event.code);
        if (emulated_event.code >= 0)
            ProcessMultitouchEvent(emulated_event);
    } else if (event.type == EV_KEY) {
        if (event.code == BTN_TOUCH || (quirk_left_mouse_button_ && event.code == BTN_LEFT)) {
            emulated_event.type = EV_ABS;
            emulated_event.code = ABS_MT_TRACKING_ID;
            emulated_event.value = event.value ? NextTrackingId() : kTrackingIdForUnusedSlot;
            ProcessMultitouchEvent(emulated_event);
        }
    }
}

void TouchEventConverterEvdev::ProcessKey(const input_event& input)
{
    switch (input.code) {
    case BTN_TOUCH:
    case BTN_LEFT:
        break;
    case BTN_TOOL_PEN:
        if (input.value > 0) {
            events_[current_slot_].tool_code = input.code;
        } else {
            events_[current_slot_].tool_code = 0;
        }
        break;
    default:
        NOTIMPLEMENTED() << "invalid code for EV_KEY: " << input.code;
    }
}

void TouchEventConverterEvdev::ProcessAbs(const input_event& input)
{
    switch (input.code) {
    case ABS_MT_TOUCH_MAJOR:
        // TODO(spang): If we have all of major, minor, and orientation,
        // we can scale the ellipse correctly. However on the Pixel we get
        // neither minor nor orientation, so this is all we can do.
        events_[current_slot_].radius_x = input.value / 2.0f;
        break;
    case ABS_MT_TOUCH_MINOR:
        events_[current_slot_].radius_y = input.value / 2.0f;
        break;
    case ABS_MT_POSITION_X:
        events_[current_slot_].x = input.value;
        break;
    case ABS_MT_POSITION_Y:
        events_[current_slot_].y = input.value;
        break;
    case ABS_MT_TRACKING_ID:
        UpdateTrackingId(current_slot_, input.value);
        break;
    case ABS_MT_PRESSURE:
        events_[current_slot_].pressure = ScalePressure(input.value);
        break;
    case ABS_MT_SLOT:
        if (input.value >= 0 && static_cast<size_t>(input.value) < events_.size()) {
            current_slot_ = input.value;
        } else {
            LOG(ERROR) << "invalid touch event index: " << input.value;
            return;
        }
        break;
    default:
        DVLOG(5) << "unhandled code for EV_ABS: " << input.code;
        return;
    }
    events_[current_slot_].altered = true;
}

void TouchEventConverterEvdev::ProcessSyn(const input_event& input)
{
    switch (input.code) {
    case SYN_REPORT:
        ReportEvents(EventConverterEvdev::TimeDeltaFromInputEvent(input));
        break;
    case SYN_DROPPED:
        // Some buffer has overrun. We ignore all events up to and
        // including the next SYN_REPORT.
        dropped_events_ = true;
        break;
    default:
        NOTIMPLEMENTED() << "invalid code for EV_SYN: " << input.code;
    }
}

EventType TouchEventConverterEvdev::GetEventTypeForTouch(
    const InProgressTouchEvdev& touch)
{
    if (touch.cancelled)
        return ET_UNKNOWN;

    if (touch_noise_finder_ && touch_noise_finder_->SlotHasNoise(touch.slot)) {
        if (touch.touching && !touch.was_touching)
            return ET_UNKNOWN;
        return ET_TOUCH_CANCELLED;
    }

    if (touch.touching)
        return touch.was_touching ? ET_TOUCH_MOVED : ET_TOUCH_PRESSED;
    return touch.was_touching ? ET_TOUCH_RELEASED : ET_UNKNOWN;
}

void TouchEventConverterEvdev::ReportEvent(const InProgressTouchEvdev& event,
    EventType event_type,
    const base::TimeDelta& timestamp)
{
    PointerDetails details(GetPointerTypeFromEvent(event), event.radius_x,
        event.radius_y, event.pressure,
        /* tilt_x */ 0.0f,
        /* tilt_y */ 0.0f);
    dispatcher_->DispatchTouchEvent(
        TouchEventParams(input_device_.id, event.slot, event_type,
            gfx::PointF(event.x, event.y), details, timestamp));
}

void TouchEventConverterEvdev::ReportEvents(base::TimeDelta delta)
{
    if (dropped_events_) {
        Reinitialize();
        dropped_events_ = false;
    }

    if (touch_noise_finder_)
        touch_noise_finder_->HandleTouches(events_, delta);

    for (size_t i = 0; i < events_.size(); i++) {
        InProgressTouchEvdev* event = &events_[i];
        if (!event->altered)
            continue;

        EventType event_type = GetEventTypeForTouch(*event);
        if (event_type == ET_UNKNOWN || event_type == ET_TOUCH_CANCELLED)
            event->cancelled = true;

        if (event_type != ET_UNKNOWN)
            ReportEvent(*event, event_type, delta);

        event->was_touching = event->touching;
        event->altered = false;
    }
}

void TouchEventConverterEvdev::UpdateTrackingId(int slot, int tracking_id)
{
    InProgressTouchEvdev* event = &events_[slot];

    if (event->tracking_id == tracking_id)
        return;

    event->tracking_id = tracking_id;
    event->touching = (tracking_id >= 0);
    event->altered = true;

    if (tracking_id >= 0)
        event->cancelled = false;
}

void TouchEventConverterEvdev::ReleaseTouches()
{
    for (size_t slot = 0; slot < events_.size(); slot++)
        UpdateTrackingId(slot, kTrackingIdForUnusedSlot);

    ReportEvents(EventTimeForNow());
}

float TouchEventConverterEvdev::ScalePressure(int32_t value)
{
    float pressure = value - pressure_min_;
    if (pressure_max_ - pressure_min_)
        pressure /= pressure_max_ - pressure_min_;
    return pressure;
}

int TouchEventConverterEvdev::NextTrackingId()
{
    return next_tracking_id_++ & kMaxTrackingId;
}

} // namespace ui
