// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/devices/x11/device_data_manager_x11.h"

#include <vector>

// Generically-named #defines from Xlib that conflict with symbols in GTest.
#undef Bool
#undef None

#include "base/macros.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/devices/device_hotplug_event_observer.h"
#include "ui/events/devices/input_device.h"
#include "ui/events/devices/input_device_event_observer.h"
#include "ui/events/devices/keyboard_device.h"
#include "ui/events/devices/touchscreen_device.h"

namespace ui {
namespace test {
    namespace {

        class TestInputDeviceObserver : public InputDeviceEventObserver {
        public:
            explicit TestInputDeviceObserver(DeviceDataManagerX11* manager)
                : manager_(manager)
                , change_notified_(false)
            {
                if (manager_)
                    manager_->AddObserver(this);
            }

            ~TestInputDeviceObserver() override
            {
                if (manager_)
                    manager_->RemoveObserver(this);
            }

            // InputDeviceEventObserver implementation.
            void OnKeyboardDeviceConfigurationChanged() override
            {
                change_notified_ = true;
            }

            int change_notified() const { return change_notified_; }
            void Reset() { change_notified_ = false; }

        private:
            DeviceDataManager* manager_;
            bool change_notified_;

            DISALLOW_COPY_AND_ASSIGN(TestInputDeviceObserver);
        };

    } //  namespace

    class DeviceDataManagerX11Test : public testing::Test {
    public:
        DeviceDataManagerX11Test() { }
        ~DeviceDataManagerX11Test() override { }

        void SetUp() override { DeviceDataManagerX11::CreateInstance(); }

        void TearDown() override
        {
            SetKeyboardDevices(std::vector<KeyboardDevice>());
        }

        virtual void SetKeyboardDevices(const std::vector<KeyboardDevice>& devices)
        {
            DeviceHotplugEventObserver* manager = DeviceDataManagerX11::GetInstance();
            manager->OnKeyboardDevicesUpdated(devices);
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(DeviceDataManagerX11Test);
    };

    // Tests that the the device data manager notifies observers when a device is
    // disabled and re-enabled.
    TEST_F(DeviceDataManagerX11Test, NotifyOnDisable)
    {
        DeviceDataManagerX11* manager = DeviceDataManagerX11::GetInstance();
        TestInputDeviceObserver observer(manager);
        std::vector<ui::KeyboardDevice> keyboards;
        keyboards.push_back(ui::KeyboardDevice(
            1, ui::InputDeviceType::INPUT_DEVICE_INTERNAL, "Keyboard"));
        keyboards.push_back(ui::KeyboardDevice(
            2, ui::InputDeviceType::INPUT_DEVICE_INTERNAL, "Keyboard"));
        SetKeyboardDevices(keyboards);
        EXPECT_TRUE(observer.change_notified());
        std::vector<KeyboardDevice> devices = manager->keyboard_devices();
        EXPECT_EQ(keyboards.size(), devices.size());
        observer.Reset();
        // Disable the device, should be notified that the device list contains one
        // less device.
        manager->DisableDevice(2);
        EXPECT_TRUE(observer.change_notified());
        devices = manager->keyboard_devices();
        EXPECT_EQ(1u, devices.size());
        KeyboardDevice device = devices.front();
        EXPECT_EQ(1, device.id);
        observer.Reset();
        // Reenable the device, should be notified that the device list contains one
        // more device.
        manager->EnableDevice(2);
        EXPECT_TRUE(observer.change_notified());
        devices = manager->keyboard_devices();
        EXPECT_EQ(keyboards.size(), devices.size());
    }

    // Tests blocking multiple devices.
    TEST_F(DeviceDataManagerX11Test, TestMultipleDisable)
    {
        DeviceDataManagerX11* manager = DeviceDataManagerX11::GetInstance();
        TestInputDeviceObserver observer(manager);
        std::vector<ui::KeyboardDevice> keyboards;
        keyboards.push_back(ui::KeyboardDevice(
            1, ui::InputDeviceType::INPUT_DEVICE_INTERNAL, "Keyboard"));
        keyboards.push_back(ui::KeyboardDevice(
            2, ui::InputDeviceType::INPUT_DEVICE_INTERNAL, "Keyboard"));
        SetKeyboardDevices(keyboards);
        EXPECT_TRUE(observer.change_notified());
        std::vector<KeyboardDevice> devices = manager->keyboard_devices();
        EXPECT_EQ(keyboards.size(), devices.size());
        observer.Reset();
        // Disable the device, should be notified that the device list contains one
        // less device.
        manager->DisableDevice(1);
        EXPECT_TRUE(observer.change_notified());
        devices = manager->keyboard_devices();
        EXPECT_EQ(1u, devices.size());
        observer.Reset();
        // Disable the second device, should be notified that the device list empty.
        manager->DisableDevice(2);
        EXPECT_TRUE(observer.change_notified());
        devices = manager->keyboard_devices();
        EXPECT_EQ(0u, devices.size());
        observer.Reset();
        // Enable the first device, should be notified that one device present.
        manager->EnableDevice(1);
        EXPECT_TRUE(observer.change_notified());
        devices = manager->keyboard_devices();
        EXPECT_EQ(1u, devices.size());
        observer.Reset();
        // Enable the second device, should be notified that both devices present.
        manager->EnableDevice(2);
        EXPECT_TRUE(observer.change_notified());
        devices = manager->keyboard_devices();
        EXPECT_EQ(2u, devices.size());
    }

    TEST_F(DeviceDataManagerX11Test, UnblockOnDeviceUnplugged)
    {
        DeviceDataManagerX11* manager = DeviceDataManagerX11::GetInstance();
        TestInputDeviceObserver observer(manager);
        std::vector<ui::KeyboardDevice> all_keyboards;
        all_keyboards.push_back(ui::KeyboardDevice(
            1, ui::InputDeviceType::INPUT_DEVICE_INTERNAL, "Keyboard"));
        all_keyboards.push_back(ui::KeyboardDevice(
            2, ui::InputDeviceType::INPUT_DEVICE_INTERNAL, "Keyboard"));
        SetKeyboardDevices(all_keyboards);
        EXPECT_TRUE(observer.change_notified());
        std::vector<KeyboardDevice> devices = manager->keyboard_devices();
        EXPECT_EQ(all_keyboards.size(), devices.size());
        observer.Reset();
        // Expect to be notified that the device is no longer available.
        manager->DisableDevice(2);
        EXPECT_TRUE(observer.change_notified());
        devices = manager->keyboard_devices();
        EXPECT_EQ(1u, devices.size());
        observer.Reset();
        // Unplug the disabled device. Should not be notified, since the active list
        // did not change.
        std::vector<ui::KeyboardDevice> subset_keyboards;
        subset_keyboards.push_back(ui::KeyboardDevice(
            1, ui::InputDeviceType::INPUT_DEVICE_INTERNAL, "Keyboard"));
        SetKeyboardDevices(subset_keyboards);
        EXPECT_FALSE(observer.change_notified());
        // Replug in the first device. Should be notified of the new device.
        SetKeyboardDevices(all_keyboards);
        EXPECT_TRUE(observer.change_notified());
        devices = manager->keyboard_devices();
        // Both devices now present.
        EXPECT_EQ(2u, devices.size());
    }

} // namespace test
} // namespace ui
