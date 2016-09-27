// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/vr/NavigatorVRDevice.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Navigator.h"
#include "core/page/Page.h"
#include "modules/vr/HMDVRDevice.h"
#include "modules/vr/PositionSensorVRDevice.h"
#include "modules/vr/VRController.h"
#include "modules/vr/VRGetDevicesCallback.h"
#include "modules/vr/VRHardwareUnit.h"
#include "modules/vr/VRHardwareUnitCollection.h"
#include "modules/vr/VRPositionState.h"

namespace blink {

NavigatorVRDevice* NavigatorVRDevice::from(Document& document)
{
    if (!document.frame() || !document.frame()->domWindow())
        return 0;
    Navigator& navigator = *document.frame()->domWindow()->navigator();
    return &from(navigator);
}

NavigatorVRDevice& NavigatorVRDevice::from(Navigator& navigator)
{
    NavigatorVRDevice* supplement = static_cast<NavigatorVRDevice*>(HeapSupplement<Navigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new NavigatorVRDevice(navigator.frame());
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

ScriptPromise NavigatorVRDevice::getVRDevices(ScriptState* scriptState, Navigator& navigator)
{
    return NavigatorVRDevice::from(navigator).getVRDevices(scriptState);
}

ScriptPromise NavigatorVRDevice::getVRDevices(ScriptState* scriptState)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    Document* document = m_frame ? m_frame->document() : 0;

    if (!document || !controller()) {
        DOMException* exception = DOMException::create(InvalidStateError, "The object is no longer associated to a document.");
        resolver->reject(exception);
        return promise;
    }

    controller()->getDevices(new VRGetDevicesCallback(resolver, m_hardwareUnits.get()));

    return promise;
}

VRController* NavigatorVRDevice::controller()
{
    if (!frame())
        return 0;

    return VRController::from(*frame());
}

DEFINE_TRACE(NavigatorVRDevice)
{
    visitor->trace(m_hardwareUnits);

    HeapSupplement<Navigator>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

NavigatorVRDevice::NavigatorVRDevice(LocalFrame* frame)
    : DOMWindowProperty(frame)
{
    m_hardwareUnits = new VRHardwareUnitCollection(controller());
}

NavigatorVRDevice::~NavigatorVRDevice()
{
}

const char* NavigatorVRDevice::supplementName()
{
    return "NavigatorVRDevice";
}

} // namespace blink
