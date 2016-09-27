// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/input/InputDevice.h"

namespace blink {

InputDevice::InputDevice(bool firesTouchEvents)
{
    m_firesTouchEvents = firesTouchEvents;
}

InputDevice::InputDevice(const InputDeviceInit& initializer)
{
    m_firesTouchEvents = initializer.firesTouchEvents();
}

InputDevice::~InputDevice()
{
}

InputDevice* InputDevice::firesTouchEventsInputDevice()
{
    DEFINE_STATIC_LOCAL(Persistent<InputDevice>, instance, (InputDevice::create(true)));
    return instance;
}

InputDevice* InputDevice::doesntFireTouchEventsInputDevice()
{
    DEFINE_STATIC_LOCAL(Persistent<InputDevice>, instance, (InputDevice::create(false)));
    return instance;
}

} // namespace blink
