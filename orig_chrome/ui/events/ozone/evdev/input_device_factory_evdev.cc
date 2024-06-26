// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/input_device_factory_evdev.h"

#include <fcntl.h>
#include <linux/input.h>
#include <stddef.h>
#include <utility>

#include "base/stl_util.h"
#include "base/thread_task_runner_handle.h"
#include "base/threading/worker_pool.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "ui/events/devices/device_data_manager.h"
#include "ui/events/devices/device_util_linux.h"
#include "ui/events/ozone/evdev/device_event_dispatcher_evdev.h"
#include "ui/events/ozone/evdev/event_converter_evdev_impl.h"
#include "ui/events/ozone/evdev/event_device_info.h"
#include "ui/events/ozone/evdev/tablet_event_converter_evdev.h"
#include "ui/events/ozone/evdev/touch_event_converter_evdev.h"

#if defined(USE_EVDEV_GESTURES)
#include "ui/events/ozone/evdev/libgestures_glue/event_reader_libevdev_cros.h"
#include "ui/events/ozone/evdev/libgestures_glue/gesture_feedback.h"
#include "ui/events/ozone/evdev/libgestures_glue/gesture_interpreter_libevdev_cros.h"
#include "ui/events/ozone/evdev/libgestures_glue/gesture_property_provider.h"
#endif

#ifndef EVIOCSCLOCKID
#define EVIOCSCLOCKID _IOW('E', 0xa0, int)
#endif

namespace ui {

namespace {

    typedef base::Callback<void(scoped_ptr<EventConverterEvdev>)>
        OpenInputDeviceReplyCallback;

    struct OpenInputDeviceParams {
        // Unique identifier for the new device.
        int id;

        // Device path to open.
        base::FilePath path;

        // Dispatcher for events. Call on UI thread only.
        DeviceEventDispatcherEvdev* dispatcher;

        // State shared between devices. Must not be dereferenced on worker thread.
        CursorDelegateEvdev* cursor;
#if defined(USE_EVDEV_GESTURES)
        GesturePropertyProvider* gesture_property_provider;
#endif
    };

#if defined(USE_EVDEV_GESTURES)
    void SetGestureIntProperty(GesturePropertyProvider* provider,
        int id,
        const std::string& name,
        int value)
    {
        GesturesProp* property = provider->GetProperty(id, name);
        if (property) {
            std::vector<int> values(1, value);
            property->SetIntValue(values);
        }
    }

    void SetGestureBoolProperty(GesturePropertyProvider* provider,
        int id,
        const std::string& name,
        bool value)
    {
        GesturesProp* property = provider->GetProperty(id, name);
        if (property) {
            std::vector<bool> values(1, value);
            property->SetBoolValue(values);
        }
    }

#endif

    scoped_ptr<EventConverterEvdev> CreateConverter(
        const OpenInputDeviceParams& params,
        int fd,
        const EventDeviceInfo& devinfo)
    {
#if defined(USE_EVDEV_GESTURES)
        // Touchpad or mouse: use gestures library.
        // EventReaderLibevdevCros -> GestureInterpreterLibevdevCros -> DispatchEvent
        if (devinfo.HasTouchpad() || devinfo.HasMouse()) {
            scoped_ptr<GestureInterpreterLibevdevCros> gesture_interp = make_scoped_ptr(new GestureInterpreterLibevdevCros(
                params.id, params.cursor, params.gesture_property_provider,
                params.dispatcher));
            return make_scoped_ptr(new EventReaderLibevdevCros(
                fd, params.path, params.id, devinfo, std::move(gesture_interp)));
        }
#endif

        // Touchscreen: use TouchEventConverterEvdev.
        if (devinfo.HasTouchscreen()) {
            scoped_ptr<TouchEventConverterEvdev> converter(new TouchEventConverterEvdev(
                fd, params.path, params.id, devinfo, params.dispatcher));
            converter->Initialize(devinfo);
            return std::move(converter);
        }

        // Graphics tablet
        if (devinfo.HasTablet())
            return make_scoped_ptr<EventConverterEvdev>(new TabletEventConverterEvdev(
                fd, params.path, params.id, params.cursor, devinfo, params.dispatcher));

        // Everything else: use EventConverterEvdevImpl.
        return make_scoped_ptr<EventConverterEvdevImpl>(new EventConverterEvdevImpl(
            fd, params.path, params.id, devinfo, params.cursor, params.dispatcher));
    }

    // Open an input device. Opening may put the calling thread to sleep, and
    // therefore should be run on a thread where latency is not critical. We
    // run it on a thread from the worker pool.
    //
    // This takes a TaskRunner and runs the reply on that thread, so that we
    // can hop threads if necessary (back to the UI thread).
    void OpenInputDevice(scoped_ptr<OpenInputDeviceParams> params,
        scoped_refptr<base::TaskRunner> reply_runner,
        const OpenInputDeviceReplyCallback& reply_callback)
    {
        const base::FilePath& path = params->path;
        scoped_ptr<EventConverterEvdev> converter;

        TRACE_EVENT1("evdev", "OpenInputDevice", "path", path.value());

        int fd = open(path.value().c_str(), O_RDWR | O_NONBLOCK);
        if (fd < 0) {
            PLOG(ERROR) << "Cannot open '" << path.value();
            reply_runner->PostTask(
                FROM_HERE, base::Bind(reply_callback, base::Passed(&converter)));
            return;
        }

        // Use monotonic timestamps for events. The touch code in particular
        // expects event timestamps to correlate to the monotonic clock
        // (base::TimeTicks).
        unsigned int clk = CLOCK_MONOTONIC;
        if (ioctl(fd, EVIOCSCLOCKID, &clk))
            PLOG(ERROR) << "failed to set CLOCK_MONOTONIC";

        EventDeviceInfo devinfo;
        if (!devinfo.Initialize(fd, path)) {
            LOG(ERROR) << "Failed to get device information for " << path.value();
            close(fd);
            reply_runner->PostTask(
                FROM_HERE, base::Bind(reply_callback, base::Passed(&converter)));
            return;
        }

        converter = CreateConverter(*params, fd, devinfo);

        // Reply with the constructed converter.
        reply_runner->PostTask(FROM_HERE,
            base::Bind(reply_callback, base::Passed(&converter)));
    }

    // Close an input device. Closing may put the calling thread to sleep, and
    // therefore should be run on a thread where latency is not critical. We
    // run it on the FILE thread.
    void CloseInputDevice(const base::FilePath& path,
        scoped_ptr<EventConverterEvdev> converter)
    {
        TRACE_EVENT1("evdev", "CloseInputDevice", "path", path.value());
        converter.reset();
    }

} // namespace

InputDeviceFactoryEvdev::InputDeviceFactoryEvdev(
    scoped_ptr<DeviceEventDispatcherEvdev> dispatcher,
    CursorDelegateEvdev* cursor)
    : task_runner_(base::ThreadTaskRunnerHandle::Get())
    , cursor_(cursor)
    ,
#if defined(USE_EVDEV_GESTURES)
    gesture_property_provider_(new GesturePropertyProvider)
    ,
#endif
    dispatcher_(std::move(dispatcher))
    , weak_ptr_factory_(this)
{
}

InputDeviceFactoryEvdev::~InputDeviceFactoryEvdev()
{
    STLDeleteValues(&converters_);
}

void InputDeviceFactoryEvdev::AddInputDevice(int id,
    const base::FilePath& path)
{
    scoped_ptr<OpenInputDeviceParams> params(new OpenInputDeviceParams);
    params->id = id;
    params->path = path;
    params->cursor = cursor_;
    params->dispatcher = dispatcher_.get();

#if defined(USE_EVDEV_GESTURES)
    params->gesture_property_provider = gesture_property_provider_.get();
#endif

    OpenInputDeviceReplyCallback reply_callback = base::Bind(&InputDeviceFactoryEvdev::AttachInputDevice,
        weak_ptr_factory_.GetWeakPtr());

    ++pending_device_changes_;

    // Dispatch task to open from the worker pool, since open may block.
    base::WorkerPool::PostTask(FROM_HERE,
        base::Bind(&OpenInputDevice, base::Passed(&params),
            task_runner_, reply_callback),
        false /* task_is_slow */);
}

void InputDeviceFactoryEvdev::RemoveInputDevice(const base::FilePath& path)
{
    DetachInputDevice(path);
}

void InputDeviceFactoryEvdev::OnStartupScanComplete()
{
    startup_devices_enumerated_ = true;
    NotifyDevicesUpdated();
}

void InputDeviceFactoryEvdev::AttachInputDevice(
    scoped_ptr<EventConverterEvdev> converter)
{
    if (converter.get()) {
        const base::FilePath& path = converter->path();

        TRACE_EVENT1("evdev", "AttachInputDevice", "path", path.value());
        DCHECK(task_runner_->RunsTasksOnCurrentThread());

        // If we have an existing device, detach it. We don't want two
        // devices with the same name open at the same time.
        if (converters_[path])
            DetachInputDevice(path);

        // Add initialized device to map.
        converters_[path] = converter.release();
        converters_[path]->Start();
        UpdateDirtyFlags(converters_[path]);

        // Sync settings to new device.
        ApplyInputDeviceSettings();
        ApplyCapsLockLed();
    }

    --pending_device_changes_;
    NotifyDevicesUpdated();
}

void InputDeviceFactoryEvdev::DetachInputDevice(const base::FilePath& path)
{
    TRACE_EVENT1("evdev", "DetachInputDevice", "path", path.value());
    DCHECK(task_runner_->RunsTasksOnCurrentThread());

    // Remove device from map.
    scoped_ptr<EventConverterEvdev> converter(converters_[path]);
    converters_.erase(path);

    if (converter) {
        // Disable the device (to release keys/buttons/etc).
        converter->SetEnabled(false);

        // Cancel libevent notifications from this converter. This part must be
        // on UI since the polling happens on UI.
        converter->Stop();

        UpdateDirtyFlags(converter.get());
        NotifyDevicesUpdated();

        // Dispatch task to close from the worker pool, since close may block.
        base::WorkerPool::PostTask(
            FROM_HERE,
            base::Bind(&CloseInputDevice, path, base::Passed(&converter)), true);
    }
}

void InputDeviceFactoryEvdev::SetCapsLockLed(bool enabled)
{
    caps_lock_led_enabled_ = enabled;
    ApplyCapsLockLed();
}

void InputDeviceFactoryEvdev::UpdateInputDeviceSettings(
    const InputDeviceSettingsEvdev& settings)
{
    input_device_settings_ = settings;
    ApplyInputDeviceSettings();
}

void InputDeviceFactoryEvdev::GetTouchDeviceStatus(
    const GetTouchDeviceStatusReply& reply)
{
    scoped_ptr<std::string> status(new std::string);
#if defined(USE_EVDEV_GESTURES)
    DumpTouchDeviceStatus(gesture_property_provider_.get(), status.get());
#endif
    reply.Run(std::move(status));
}

void InputDeviceFactoryEvdev::GetTouchEventLog(
    const base::FilePath& out_dir,
    const GetTouchEventLogReply& reply)
{
    scoped_ptr<std::vector<base::FilePath>> log_paths(
        new std::vector<base::FilePath>);
#if defined(USE_EVDEV_GESTURES)
    DumpTouchEventLog(converters_, gesture_property_provider_.get(), out_dir,
        std::move(log_paths), reply);
#else
    reply.Run(std::move(log_paths));
#endif
}

base::WeakPtr<InputDeviceFactoryEvdev> InputDeviceFactoryEvdev::GetWeakPtr()
{
    return weak_ptr_factory_.GetWeakPtr();
}

void InputDeviceFactoryEvdev::ApplyInputDeviceSettings()
{
    TRACE_EVENT0("evdev", "ApplyInputDeviceSettings");

    SetIntPropertyForOneType(DT_TOUCHPAD, "Pointer Sensitivity",
        input_device_settings_.touchpad_sensitivity);
    SetIntPropertyForOneType(DT_TOUCHPAD, "Scroll Sensitivity",
        input_device_settings_.touchpad_sensitivity);

    SetBoolPropertyForOneType(DT_TOUCHPAD, "Tap Enable",
        input_device_settings_.tap_to_click_enabled);
    SetBoolPropertyForOneType(DT_TOUCHPAD, "T5R2 Three Finger Click Enable",
        input_device_settings_.three_finger_click_enabled);
    SetBoolPropertyForOneType(DT_TOUCHPAD, "Tap Drag Enable",
        input_device_settings_.tap_dragging_enabled);

    SetBoolPropertyForOneType(DT_MULTITOUCH, "Australian Scrolling",
        input_device_settings_.natural_scroll_enabled);

    SetIntPropertyForOneType(DT_MOUSE, "Pointer Sensitivity",
        input_device_settings_.mouse_sensitivity);
    SetIntPropertyForOneType(DT_MOUSE, "Scroll Sensitivity",
        input_device_settings_.mouse_sensitivity);

    SetBoolPropertyForOneType(DT_TOUCHPAD, "Tap Paused",
        input_device_settings_.tap_to_click_paused);

    for (const auto& it : converters_) {
        EventConverterEvdev* converter = it.second;
        converter->SetEnabled(IsDeviceEnabled(converter));

        if (converter->type() == InputDeviceType::INPUT_DEVICE_INTERNAL && converter->HasKeyboard()) {
            converter->SetKeyFilter(
                input_device_settings_.enable_internal_keyboard_filter,
                input_device_settings_.internal_keyboard_allowed_keys);
        }

        converter->SetTouchEventLoggingEnabled(
            input_device_settings_.touch_event_logging_enabled);
    }
}

void InputDeviceFactoryEvdev::ApplyCapsLockLed()
{
    for (const auto& it : converters_) {
        EventConverterEvdev* converter = it.second;
        converter->SetCapsLockLed(caps_lock_led_enabled_);
    }
}

bool InputDeviceFactoryEvdev::IsDeviceEnabled(
    const EventConverterEvdev* converter)
{
    if (!input_device_settings_.enable_internal_touchpad && converter->type() == InputDeviceType::INPUT_DEVICE_INTERNAL && converter->HasTouchpad())
        return false;

    if (!input_device_settings_.enable_touch_screens && converter->HasTouchscreen())
        return false;

    return input_device_settings_.enable_devices;
}

void InputDeviceFactoryEvdev::UpdateDirtyFlags(
    const EventConverterEvdev* converter)
{
    if (converter->HasTouchscreen())
        touchscreen_list_dirty_ = true;

    if (converter->HasKeyboard())
        keyboard_list_dirty_ = true;

    if (converter->HasMouse())
        mouse_list_dirty_ = true;

    if (converter->HasTouchpad())
        touchpad_list_dirty_ = true;
}

void InputDeviceFactoryEvdev::NotifyDevicesUpdated()
{
    if (!startup_devices_enumerated_ || pending_device_changes_)
        return; // No update until full scan done and no pending operations.
    if (touchscreen_list_dirty_)
        NotifyTouchscreensUpdated();
    if (keyboard_list_dirty_)
        NotifyKeyboardsUpdated();
    if (mouse_list_dirty_)
        NotifyMouseDevicesUpdated();
    if (touchpad_list_dirty_)
        NotifyTouchpadDevicesUpdated();
    if (!startup_devices_opened_) {
        dispatcher_->DispatchDeviceListsComplete();
        startup_devices_opened_ = true;
    }
    touchscreen_list_dirty_ = false;
    keyboard_list_dirty_ = false;
    mouse_list_dirty_ = false;
    touchpad_list_dirty_ = false;
}

void InputDeviceFactoryEvdev::NotifyTouchscreensUpdated()
{
    std::vector<TouchscreenDevice> touchscreens;
    for (auto it = converters_.begin(); it != converters_.end(); ++it) {
        if (it->second->HasTouchscreen()) {
            touchscreens.push_back(TouchscreenDevice(it->second->input_device(),
                it->second->GetTouchscreenSize(), it->second->GetTouchPoints()));
        }
    }

    dispatcher_->DispatchTouchscreenDevicesUpdated(touchscreens);
}

void InputDeviceFactoryEvdev::NotifyKeyboardsUpdated()
{
    std::vector<KeyboardDevice> keyboards;
    for (auto it = converters_.begin(); it != converters_.end(); ++it) {
        if (it->second->HasKeyboard()) {
            keyboards.push_back(KeyboardDevice(it->second->input_device()));
        }
    }

    dispatcher_->DispatchKeyboardDevicesUpdated(keyboards);
}

void InputDeviceFactoryEvdev::NotifyMouseDevicesUpdated()
{
    std::vector<InputDevice> mice;
    for (auto it = converters_.begin(); it != converters_.end(); ++it) {
        if (it->second->HasMouse()) {
            mice.push_back(it->second->input_device());
        }
    }

    dispatcher_->DispatchMouseDevicesUpdated(mice);
}

void InputDeviceFactoryEvdev::NotifyTouchpadDevicesUpdated()
{
    std::vector<InputDevice> touchpads;
    for (auto it = converters_.begin(); it != converters_.end(); ++it) {
        if (it->second->HasTouchpad()) {
            touchpads.push_back(it->second->input_device());
        }
    }

    dispatcher_->DispatchTouchpadDevicesUpdated(touchpads);
}

void InputDeviceFactoryEvdev::SetIntPropertyForOneType(
    const EventDeviceType type,
    const std::string& name,
    int value)
{
#if defined(USE_EVDEV_GESTURES)
    std::vector<int> ids;
    gesture_property_provider_->GetDeviceIdsByType(type, &ids);
    for (size_t i = 0; i < ids.size(); ++i) {
        SetGestureIntProperty(gesture_property_provider_.get(), ids[i], name,
            value);
    }
#endif
    // In the future, we may add property setting codes for other non-gesture
    // devices. One example would be keyboard settings.
    // TODO(sheckylin): See http://crbug.com/398518 for example.
}

void InputDeviceFactoryEvdev::SetBoolPropertyForOneType(
    const EventDeviceType type,
    const std::string& name,
    bool value)
{
#if defined(USE_EVDEV_GESTURES)
    std::vector<int> ids;
    gesture_property_provider_->GetDeviceIdsByType(type, &ids);
    for (size_t i = 0; i < ids.size(); ++i) {
        SetGestureBoolProperty(gesture_property_provider_.get(), ids[i], name,
            value);
    }
#endif
}

} // namespace ui
