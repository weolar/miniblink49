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

#ifndef DeviceMotionDispatcher_h
#define DeviceMotionDispatcher_h

#include "core/frame/PlatformEventDispatcher.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/device_orientation/WebDeviceMotionListener.h"
#include "wtf/RefPtr.h"

namespace blink {

class DeviceMotionData;
class WebDeviceMotionData;

// This class listens to device motion data and notifies all registered controllers.
class DeviceMotionDispatcher final : public GarbageCollectedFinalized<DeviceMotionDispatcher>, public PlatformEventDispatcher, public WebDeviceMotionListener {
    USING_GARBAGE_COLLECTED_MIXIN(DeviceMotionDispatcher);
public:
    static DeviceMotionDispatcher& instance();
    ~DeviceMotionDispatcher() override;

    // Note that the returned object is owned by this class.
    // FIXME: make the return value const, see crbug.com/233174.
    DeviceMotionData* latestDeviceMotionData();

    // Inherited from WebDeviceMotionListener.
    void didChangeDeviceMotion(const WebDeviceMotionData&) override;

    DECLARE_VIRTUAL_TRACE();

private:
    DeviceMotionDispatcher();

    // Inherited from PlatformEventDispatcher.
    void startListening() override;
    void stopListening() override;

    Member<DeviceMotionData> m_lastDeviceMotionData;
};

} // namespace blink

#endif // DeviceMotionDispatcher_h
