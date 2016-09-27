// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/device_orientation/DeviceMotionController.h"

#include "core/dom/Document.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "modules/EventModules.h"
#include "modules/device_orientation/DeviceMotionData.h"
#include "modules/device_orientation/DeviceMotionDispatcher.h"
#include "modules/device_orientation/DeviceMotionEvent.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"

namespace blink {

DeviceMotionController::DeviceMotionController(Document& document)
    : DeviceSingleWindowEventController(document)
{
}

DeviceMotionController::~DeviceMotionController()
{
#if !ENABLE(OILPAN)
    stopUpdating();
#endif
}

const char* DeviceMotionController::supplementName()
{
    return "DeviceMotionController";
}

DeviceMotionController& DeviceMotionController::from(Document& document)
{
    DeviceMotionController* controller = static_cast<DeviceMotionController*>(WillBeHeapSupplement<Document>::from(document, supplementName()));
    if (!controller) {
        controller = new DeviceMotionController(document);
        WillBeHeapSupplement<Document>::provideTo(document, supplementName(), adoptPtrWillBeNoop(controller));
    }
    return *controller;
}

void DeviceMotionController::didAddEventListener(LocalDOMWindow* window, const AtomicString& eventType)
{
    if (eventType != eventTypeName())
        return;

    if (document().frame()) {
        String errorMessage;
        if (document().isPrivilegedContext(errorMessage)) {
            UseCounter::count(document().frame(), UseCounter::DeviceMotionSecureOrigin);
        } else {
            UseCounter::count(document().frame(), UseCounter::DeviceMotionInsecureOrigin);
            if (document().frame()->settings()->strictPowerfulFeatureRestrictions())
                return;
        }
    }

    if (!m_hasEventListener)
        Platform::current()->recordRapporURL("DeviceSensors.DeviceMotion", WebURL(document().url()));

    DeviceSingleWindowEventController::didAddEventListener(window, eventType);
}

bool DeviceMotionController::hasLastData()
{
    return DeviceMotionDispatcher::instance().latestDeviceMotionData();
}

void DeviceMotionController::registerWithDispatcher()
{
    DeviceMotionDispatcher::instance().addController(this);
}

void DeviceMotionController::unregisterWithDispatcher()
{
    DeviceMotionDispatcher::instance().removeController(this);
}

PassRefPtrWillBeRawPtr<Event> DeviceMotionController::lastEvent() const
{
    return DeviceMotionEvent::create(EventTypeNames::devicemotion, DeviceMotionDispatcher::instance().latestDeviceMotionData());
}

bool DeviceMotionController::isNullEvent(Event* event) const
{
    DeviceMotionEvent* motionEvent = toDeviceMotionEvent(event);
    return !motionEvent->deviceMotionData()->canProvideEventData();
}

const AtomicString& DeviceMotionController::eventTypeName() const
{
    return EventTypeNames::devicemotion;
}

DEFINE_TRACE(DeviceMotionController)
{
    DeviceSingleWindowEventController::trace(visitor);
    WillBeHeapSupplement<Document>::trace(visitor);
}

} // namespace blink
