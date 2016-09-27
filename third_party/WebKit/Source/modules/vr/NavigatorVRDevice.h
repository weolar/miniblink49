// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorVRDevice_h
#define NavigatorVRDevice_h

#include "bindings/core/v8/ScriptPromise.h"
#include "core/frame/DOMWindowProperty.h"
#include "modules/ModulesExport.h"
#include "modules/vr/VRDevice.h"
#include "modules/vr/VRHardwareUnit.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebVector.h"
#include "wtf/Noncopyable.h"

namespace blink {

class Document;
class Navigator;
class VRController;
class VRHardwareUnitCollection;

class MODULES_EXPORT NavigatorVRDevice final : public GarbageCollectedFinalized<NavigatorVRDevice>, public HeapSupplement<Navigator>, public DOMWindowProperty {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorVRDevice);
    WTF_MAKE_NONCOPYABLE(NavigatorVRDevice);
public:
    static NavigatorVRDevice* from(Document&);
    static NavigatorVRDevice& from(Navigator&);
    virtual ~NavigatorVRDevice();

    static ScriptPromise getVRDevices(ScriptState*, Navigator&);
    ScriptPromise getVRDevices(ScriptState*);

    VRController* controller();

    DECLARE_VIRTUAL_TRACE();

private:
    friend class VRHardwareUnit;
    friend class VRGetDevicesCallback;

    explicit NavigatorVRDevice(LocalFrame*);

    static const char* supplementName();

    Member<VRHardwareUnitCollection> m_hardwareUnits;
};

} // namespace blink

#endif // NavigatorVRDevice_h
