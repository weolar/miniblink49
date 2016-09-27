// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/vr/VRController.h"

#include "core/frame/LocalFrame.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

VRController::~VRController()
{
}

void VRController::provideTo(LocalFrame& frame, WebVRClient* client)
{
    ASSERT(RuntimeEnabledFeatures::webVREnabled());

    VRController* controller = new VRController(frame, client);
    WillBeHeapSupplement<LocalFrame>::provideTo(frame, supplementName(), adoptPtrWillBeNoop(controller));
}

VRController* VRController::from(LocalFrame& frame)
{
    return static_cast<VRController*>(WillBeHeapSupplement<LocalFrame>::from(frame, supplementName()));
}

VRController::VRController(LocalFrame& frame, WebVRClient* client)
    : LocalFrameLifecycleObserver(&frame)
    , m_client(client)
{
}

const char* VRController::supplementName()
{
    return "VRController";
}

void VRController::getDevices(WebVRGetDevicesCallback* callback)
{
    // When detached, the client is no longer valid.
    if (!m_client) {
        callback->onError();
        delete callback;
        return;
    }

    // Client is expected to take ownership of the callback
    m_client->getDevices(callback);
}

void VRController::getSensorState(unsigned index, WebHMDSensorState& into)
{
    // When detached, the client is no longer valid.
    if (!m_client)
        return;
    m_client->getSensorState(index, into);
}

void VRController::resetSensor(unsigned index)
{
    // When detached, the client is no longer valid.
    if (!m_client)
        return;
    m_client->resetSensor(index);
}

void VRController::willDetachFrameHost()
{
    m_client = nullptr;
}

DEFINE_TRACE(VRController)
{
    WillBeHeapSupplement<LocalFrame>::trace(visitor);
    LocalFrameLifecycleObserver::trace(visitor);
}

} // namespace blink
