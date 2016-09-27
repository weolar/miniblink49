/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/device_orientation/DeviceMotionDispatcher.h"

#include "modules/device_orientation/DeviceMotionController.h"
#include "modules/device_orientation/DeviceMotionData.h"
#include "public/platform/Platform.h"

namespace blink {

DeviceMotionDispatcher& DeviceMotionDispatcher::instance()
{
    DEFINE_STATIC_LOCAL(Persistent<DeviceMotionDispatcher>, deviceMotionDispatcher, (new DeviceMotionDispatcher()));
    return *deviceMotionDispatcher;
}

DeviceMotionDispatcher::DeviceMotionDispatcher()
{
}

DeviceMotionDispatcher::~DeviceMotionDispatcher()
{
}

DEFINE_TRACE(DeviceMotionDispatcher)
{
    visitor->trace(m_lastDeviceMotionData);
    PlatformEventDispatcher::trace(visitor);
}

void DeviceMotionDispatcher::startListening()
{
    Platform::current()->startListening(WebPlatformEventDeviceMotion, this);
}

void DeviceMotionDispatcher::stopListening()
{
    Platform::current()->stopListening(WebPlatformEventDeviceMotion);
    m_lastDeviceMotionData.clear();
}

void DeviceMotionDispatcher::didChangeDeviceMotion(const WebDeviceMotionData& motion)
{
    m_lastDeviceMotionData = DeviceMotionData::create(motion);
    notifyControllers();
}

DeviceMotionData* DeviceMotionDispatcher::latestDeviceMotionData()
{
    return m_lastDeviceMotionData.get();
}

} // namespace blink
