// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HMDVRDevice_h
#define HMDVRDevice_h

#include "modules/vr/VRDevice.h"
#include "modules/vr/VREyeParameters.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/vr/WebVR.h"
#include "wtf/Forward.h"
#include "wtf/text/WTFString.h"

namespace blink {

class VRFieldOfView;

class HMDVRDevice final : public VRDevice {
    DEFINE_WRAPPERTYPEINFO();
public:
    HMDVRDevice(VRHardwareUnit*, unsigned);

    void updateFromWebVRDevice(const WebVRDevice&) override;

    VREyeParameters* getEyeParameters(const String&);
    void setFieldOfView(VRFieldOfView* leftFov = 0, VRFieldOfView* rightFov = 0);

    DECLARE_VIRTUAL_TRACE();

private:
    Member<VREyeParameters> m_eyeParametersLeft;
    Member<VREyeParameters> m_eyeParametersRight;
};

} // namespace blink

#endif // HMDVRDevice_h
