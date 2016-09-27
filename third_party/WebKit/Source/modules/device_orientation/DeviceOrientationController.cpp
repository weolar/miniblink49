// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/device_orientation/DeviceOrientationController.h"

#include "core/dom/Document.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "modules/EventModules.h"
#include "modules/device_orientation/DeviceOrientationData.h"
#include "modules/device_orientation/DeviceOrientationDispatcher.h"
#include "modules/device_orientation/DeviceOrientationEvent.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"

namespace blink {

DeviceOrientationController::DeviceOrientationController(Document& document)
    : DeviceSingleWindowEventController(document)
{
}

DeviceOrientationController::~DeviceOrientationController()
{
#if !ENABLE(OILPAN)
    stopUpdating();
#endif
}

void DeviceOrientationController::didUpdateData()
{
    if (m_overrideOrientationData)
        return;
    dispatchDeviceEvent(lastEvent());
}

const char* DeviceOrientationController::supplementName()
{
    return "DeviceOrientationController";
}

DeviceOrientationController& DeviceOrientationController::from(Document& document)
{
    DeviceOrientationController* controller = static_cast<DeviceOrientationController*>(WillBeHeapSupplement<Document>::from(document, supplementName()));
    if (!controller) {
        controller = new DeviceOrientationController(document);
        WillBeHeapSupplement<Document>::provideTo(document, supplementName(), adoptPtrWillBeNoop(controller));
    }
    return *controller;
}

void DeviceOrientationController::didAddEventListener(LocalDOMWindow* window, const AtomicString& eventType)
{
    if (eventType != eventTypeName())
        return;

    if (document().frame()) {
        String errorMessage;
        if (document().isPrivilegedContext(errorMessage)) {
            UseCounter::count(document().frame(), UseCounter::DeviceOrientationSecureOrigin);
        } else {
            UseCounter::count(document().frame(), UseCounter::DeviceOrientationInsecureOrigin);
            if (document().frame()->settings()->strictPowerfulFeatureRestrictions())
                return;
        }
    }

    if (!m_hasEventListener)
        Platform::current()->recordRapporURL("DeviceSensors.DeviceOrientation", WebURL(document().url()));

    DeviceSingleWindowEventController::didAddEventListener(window, eventType);
}

DeviceOrientationData* DeviceOrientationController::lastData() const
{
    return m_overrideOrientationData ? m_overrideOrientationData.get() : DeviceOrientationDispatcher::instance().latestDeviceOrientationData();
}

bool DeviceOrientationController::hasLastData()
{
    return lastData();
}

void DeviceOrientationController::registerWithDispatcher()
{
    DeviceOrientationDispatcher::instance().addController(this);
}

void DeviceOrientationController::unregisterWithDispatcher()
{
    DeviceOrientationDispatcher::instance().removeController(this);
}

PassRefPtrWillBeRawPtr<Event> DeviceOrientationController::lastEvent() const
{
    return DeviceOrientationEvent::create(eventTypeName(), lastData());
}

bool DeviceOrientationController::isNullEvent(Event* event) const
{
    DeviceOrientationEvent* orientationEvent = toDeviceOrientationEvent(event);
    return !orientationEvent->orientation()->canProvideEventData();
}

const AtomicString& DeviceOrientationController::eventTypeName() const
{
    return EventTypeNames::deviceorientation;
}

void DeviceOrientationController::setOverride(DeviceOrientationData* deviceOrientationData)
{
    ASSERT(deviceOrientationData);
    m_overrideOrientationData = deviceOrientationData;
    dispatchDeviceEvent(lastEvent());
}

void DeviceOrientationController::clearOverride()
{
    if (!m_overrideOrientationData)
        return;
    m_overrideOrientationData.clear();
    if (lastData())
        didUpdateData();
}

DEFINE_TRACE(DeviceOrientationController)
{
    visitor->trace(m_overrideOrientationData);
    DeviceSingleWindowEventController::trace(visitor);
    WillBeHeapSupplement<Document>::trace(visitor);
}

} // namespace blink
