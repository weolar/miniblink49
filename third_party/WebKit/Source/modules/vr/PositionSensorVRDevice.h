// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PositionSensorVRDevice_h
#define PositionSensorVRDevice_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/vr/VRDevice.h"
#include "modules/vr/VRPositionState.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/text/WTFString.h"

namespace blink {

class PositionSensorVRDevice final : public VRDevice {
    DEFINE_WRAPPERTYPEINFO();
public:
    PositionSensorVRDevice(VRHardwareUnit*, unsigned);

    VRPositionState* getState();
    VRPositionState* getImmediateState();
    void resetSensor();

    DECLARE_VIRTUAL_TRACE();
};

} // namespace blink

#endif // PositionSensorVRDevice_h
