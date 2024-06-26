// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_EVENT_CONVERTER_TEST_UTIL_H_
#define UI_EVENTS_OZONE_EVDEV_EVENT_CONVERTER_TEST_UTIL_H_

#include "base/memory/scoped_ptr.h"

#include "ui/events/ozone/evdev/event_dispatch_callback.h"

namespace ui {

class CursorDelegateEvdev;
class DeviceManager;
class DeviceEventDispatcherEvdev;
class EventFactoryEvdev;
class KeyboardLayoutEngine;

scoped_ptr<DeviceManager> CreateDeviceManagerForTest();

scoped_ptr<EventFactoryEvdev> CreateEventFactoryEvdevForTest(
    CursorDelegateEvdev* cursor,
    DeviceManager* device_manager,
    KeyboardLayoutEngine* keyboard_layout_engine,
    const EventDispatchCallback& callback);

scoped_ptr<DeviceEventDispatcherEvdev> CreateDeviceEventDispatcherEvdevForTest(
    EventFactoryEvdev* event_factory);

} // namespace ui

#endif // UI_EVENTS_OZONE_EVDEV_EVENT_CONVERTER_TEST_UTIL_H_
