// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DeviceLightDispatcher_h
#define DeviceLightDispatcher_h

#include "core/frame/PlatformEventDispatcher.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebDeviceLightListener.h"

namespace blink {

// This class listens to device light data and notifies all registered controllers.
class DeviceLightDispatcher final : public GarbageCollectedFinalized<DeviceLightDispatcher>, public PlatformEventDispatcher, public WebDeviceLightListener {
    USING_GARBAGE_COLLECTED_MIXIN(DeviceLightDispatcher);
public:
    static DeviceLightDispatcher& instance();
    ~DeviceLightDispatcher() override;

    double latestDeviceLightData() const;

    // Inherited from WebDeviceLightListener.
    void didChangeDeviceLight(double) override;

    DECLARE_VIRTUAL_TRACE();

private:
    DeviceLightDispatcher();

    // Inherited from PlatformEventDispatcher.
    void startListening() override;
    void stopListening() override;

    double m_lastDeviceLightData;
};

} // namespace blink

#endif // DeviceLightDispatcher_h
