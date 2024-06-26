// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/event_converter_test_util.h"

#include <stdint.h>

#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/device_event_dispatcher_evdev.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/platform/platform_event_dispatcher.h"

namespace ui {

namespace {

    class TestDeviceManager : public ui::DeviceManager {
    public:
        TestDeviceManager() { }
        ~TestDeviceManager() override { }

        // DeviceManager:
        void ScanDevices(DeviceEventObserver* observer) override { }
        void AddObserver(DeviceEventObserver* observer) override { }
        void RemoveObserver(DeviceEventObserver* observer) override { }
    };

    class TestDeviceEventDispatcherEvdev : public DeviceEventDispatcherEvdev {
    public:
        TestDeviceEventDispatcherEvdev(EventFactoryEvdev* event_factory_evdev)
            : event_factory_evdev_(event_factory_evdev)
        {
        }
        ~TestDeviceEventDispatcherEvdev() override { }

        // DeviceEventDispatcher:
        void DispatchKeyEvent(const KeyEventParams& params) override
        {
            event_factory_evdev_->DispatchKeyEvent(params);
        }

        void DispatchMouseMoveEvent(const MouseMoveEventParams& params) override
        {
            event_factory_evdev_->DispatchMouseMoveEvent(params);
        }

        void DispatchMouseButtonEvent(const MouseButtonEventParams& params) override
        {
            event_factory_evdev_->DispatchMouseButtonEvent(params);
        }

        void DispatchMouseWheelEvent(const MouseWheelEventParams& params) override
        {
            event_factory_evdev_->DispatchMouseWheelEvent(params);
        }

        void DispatchPinchEvent(const PinchEventParams& params) override
        {
            event_factory_evdev_->DispatchPinchEvent(params);
        }

        void DispatchScrollEvent(const ScrollEventParams& params) override
        {
            event_factory_evdev_->DispatchScrollEvent(params);
        }

        void DispatchTouchEvent(const TouchEventParams& params) override
        {
            event_factory_evdev_->DispatchTouchEvent(params);
        }

        void DispatchKeyboardDevicesUpdated(
            const std::vector<KeyboardDevice>& devices) override
        {
            event_factory_evdev_->DispatchKeyboardDevicesUpdated(devices);
        }
        void DispatchTouchscreenDevicesUpdated(
            const std::vector<TouchscreenDevice>& devices) override
        {
            event_factory_evdev_->DispatchTouchscreenDevicesUpdated(devices);
        }
        void DispatchMouseDevicesUpdated(
            const std::vector<InputDevice>& devices) override
        {
            event_factory_evdev_->DispatchMouseDevicesUpdated(devices);
        }
        void DispatchTouchpadDevicesUpdated(
            const std::vector<InputDevice>& devices) override
        {
            event_factory_evdev_->DispatchTouchpadDevicesUpdated(devices);
        }
        void DispatchDeviceListsComplete() override
        {
            event_factory_evdev_->DispatchDeviceListsComplete();
        }

    private:
        EventFactoryEvdev* event_factory_evdev_;
    };

    class TestEventFactoryEvdev : public EventFactoryEvdev {
    public:
        TestEventFactoryEvdev(CursorDelegateEvdev* cursor,
            DeviceManager* device_manager,
            KeyboardLayoutEngine* keyboard_layout_engine,
            const EventDispatchCallback& callback)
            : EventFactoryEvdev(cursor, device_manager, keyboard_layout_engine)
            , callback_(callback)
        {
        }
        ~TestEventFactoryEvdev() override { }

    private:
        uint32_t DispatchEvent(PlatformEvent platform_event) override
        {
            Event* event = static_cast<Event*>(platform_event);
            callback_.Run(event);
            return POST_DISPATCH_NONE;
        }

        EventDispatchCallback callback_;
    };

} // namespace

scoped_ptr<DeviceEventDispatcherEvdev> CreateDeviceEventDispatcherEvdevForTest(
    EventFactoryEvdev* event_factory)
{
    return make_scoped_ptr(new TestDeviceEventDispatcherEvdev(event_factory));
}

scoped_ptr<DeviceManager> CreateDeviceManagerForTest()
{
    return make_scoped_ptr(new TestDeviceManager());
}

scoped_ptr<EventFactoryEvdev> CreateEventFactoryEvdevForTest(
    CursorDelegateEvdev* cursor,
    DeviceManager* device_manager,
    KeyboardLayoutEngine* keyboard_layout_engine,
    const EventDispatchCallback& callback)
{
    return make_scoped_ptr(new TestEventFactoryEvdev(
        cursor, device_manager, keyboard_layout_engine, callback));
}

} // namespace ui
