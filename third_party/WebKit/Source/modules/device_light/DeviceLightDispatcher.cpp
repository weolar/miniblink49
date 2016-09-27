// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/device_light/DeviceLightDispatcher.h"

#include "modules/device_light/DeviceLightController.h"
#include "public/platform/Platform.h"

namespace blink {

DeviceLightDispatcher& DeviceLightDispatcher::instance()
{
    DEFINE_STATIC_LOCAL(Persistent<DeviceLightDispatcher>, deviceLightDispatcher, (new DeviceLightDispatcher()));
    return *deviceLightDispatcher;
}

DeviceLightDispatcher::DeviceLightDispatcher()
    : m_lastDeviceLightData(-1)
{
}

DeviceLightDispatcher::~DeviceLightDispatcher()
{
}

DEFINE_TRACE(DeviceLightDispatcher)
{
    PlatformEventDispatcher::trace(visitor);
}

void DeviceLightDispatcher::startListening()
{
    Platform::current()->startListening(WebPlatformEventDeviceLight, this);
}

void DeviceLightDispatcher::stopListening()
{
    Platform::current()->stopListening(WebPlatformEventDeviceLight);
    m_lastDeviceLightData = -1;
}

void DeviceLightDispatcher::didChangeDeviceLight(double value)
{
    m_lastDeviceLightData = value;
    notifyControllers();
}

double DeviceLightDispatcher::latestDeviceLightData() const
{
    return m_lastDeviceLightData;
}

} // namespace blink
