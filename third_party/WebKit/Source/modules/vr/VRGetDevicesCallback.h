// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRGetDevicesCallback_h
#define VRGetDevicesCallback_h

#include "platform/heap/Handle.h"
#include "public/platform/modules/vr/WebVRClient.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class VRHardwareUnitCollection;
class ScriptPromiseResolver;
class WebVRClient;

class VRGetDevicesCallback final : public WebVRGetDevicesCallback {
public:
    VRGetDevicesCallback(PassRefPtrWillBeRawPtr<ScriptPromiseResolver>, VRHardwareUnitCollection*);
    ~VRGetDevicesCallback() override;

    void onSuccess(WebVector<WebVRDevice>*) override;
    void onError() override;

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
    Persistent<VRHardwareUnitCollection> m_hardwareUnits;
};

} // namespace blink

#endif // VRGetDevicesCallback_h
