// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/keyboard_util_evdev.h"

#include <linux/input.h>

#include "ui/events/keycodes/dom/keycode_converter.h"

namespace ui {

namespace {

    const int kXkbKeycodeOffset = 8;

} // namespace

int NativeCodeToEvdevCode(int native_code)
{
    if (native_code == KeycodeConverter::InvalidNativeKeycode())
        return KEY_RESERVED;

    return native_code - kXkbKeycodeOffset;
}

int EvdevCodeToNativeCode(int evdev_code)
{
    if (evdev_code == KEY_RESERVED)
        return KeycodeConverter::InvalidNativeKeycode();

    return evdev_code + kXkbKeycodeOffset;
}

} // namespace ui
