// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/vr/VRGetDevicesCallback.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "modules/vr/VRHardwareUnitCollection.h"

namespace blink {

VRGetDevicesCallback::VRGetDevicesCallback(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver, VRHardwareUnitCollection* hardwareUnits)
    : m_resolver(resolver)
    , m_hardwareUnits(hardwareUnits)
{
}

VRGetDevicesCallback::~VRGetDevicesCallback()
{
}

void VRGetDevicesCallback::onSuccess(WebVector<WebVRDevice>* devices)
{
    m_resolver->resolve(m_hardwareUnits->updateVRHardwareUnits(devices));
}

void VRGetDevicesCallback::onError()
{
    m_resolver->reject();
}

} // namespace blink
