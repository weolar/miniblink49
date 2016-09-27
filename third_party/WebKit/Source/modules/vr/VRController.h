// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRController_h
#define VRController_h

#include "core/frame/LocalFrameLifecycleObserver.h"
#include "modules/ModulesExport.h"
#include "platform/Supplementable.h"
#include "public/platform/modules/vr/WebVR.h"
#include "public/platform/modules/vr/WebVRClient.h"

namespace blink {

class MODULES_EXPORT VRController final
    : public GarbageCollectedFinalized<VRController>
    , public WillBeHeapSupplement<LocalFrame>
    , public LocalFrameLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(VRController);
    WTF_MAKE_NONCOPYABLE(VRController);
public:
    virtual ~VRController();

    void getDevices(WebVRGetDevicesCallback*);

    void getSensorState(unsigned index, WebHMDSensorState& into);

    void resetSensor(unsigned index);

    static void provideTo(LocalFrame&, WebVRClient*);
    static VRController* from(LocalFrame&);
    static const char* supplementName();

    DECLARE_VIRTUAL_TRACE();

private:
    VRController(LocalFrame&, WebVRClient*);

    // Inherited from LocalFrameLifecycleObserver.
    void willDetachFrameHost() override;

    WebVRClient* m_client;
};

} // namespace blink

#endif // VRController_h
