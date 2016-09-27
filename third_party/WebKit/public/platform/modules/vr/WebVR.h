// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebVR_h
#define WebVR_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebString.h"

#if BLINK_IMPLEMENTATION
#include "wtf/Assertions.h"
#endif

namespace blink {

struct WebVRVector3 {
    float x, y, z;
};

struct WebVRVector4 {
    float x, y, z, w;
};

struct WebVRRect {
    int x, y, width, height;
};

// A field of view, given by 4 degrees describing the view from a center point.
struct WebVRFieldOfView {
    float upDegrees;
    float downDegrees;
    float leftDegrees;
    float rightDegrees;
};

// Bit flags to indicate which fields of an WebHMDSensorState are valid.
enum WebVRSensorStateFlags {
    WebVRSensorStateOrientation = 1 << 1,
    WebVRSensorStatePosition = 1 << 2,
    WebVRSensorStateAngularVelocity = 1 << 3,
    WebVRSensorStateLinearVelocity = 1 << 4,
    WebVRSensorStateAngularAcceleration = 1 << 5,
    WebVRSensorStateLinearAcceleration = 1 << 6,
    WebVRSensorStateComplete = (1 << 7) - 1 // All previous states combined.
};

// A bitfield of WebVRSensorStateFlags.
typedef int WebVRSensorStateMask;

// A sensor's position, orientation, velocity, and acceleration state at the
// given timestamp.
struct WebHMDSensorState {
    double timestamp;
    unsigned frameIndex;
    WebVRSensorStateMask flags;
    WebVRVector4 orientation;
    WebVRVector3 position;
    WebVRVector3 angularVelocity;
    WebVRVector3 linearVelocity;
    WebVRVector3 angularAcceleration;
    WebVRVector3 linearAcceleration;
};

// Information about the optical properties for an eye in an HMD.
struct WebVREyeParameters {
    WebVRFieldOfView minimumFieldOfView;
    WebVRFieldOfView maximumFieldOfView;
    WebVRFieldOfView recommendedFieldOfView;
    WebVRVector3 eyeTranslation;
    WebVRRect renderRect;
};

// Information pertaining to Head Mounted Displays.
struct WebVRHMDInfo {
    WebVREyeParameters leftEye;
    WebVREyeParameters rightEye;
};

// Bit flags to indicate what type of data a WebVRDevice describes.
enum WebVRDeviceTypeFlags {
    WebVRDeviceTypePosition = 1 << 1,
    WebVRDeviceTypeHMD = 1 << 2
};

// A bitfield of WebVRDeviceTypeFlags.
typedef int WebVRDeviceTypeMask;

// Describes a single VR hardware unit. May describe multiple capabilities,
// such as position sensors or head mounted display metrics.
struct WebVRDevice {
    // Index for this hardware unit.
    unsigned index;
    // Friendly device name.
    WebString deviceName;
    // Identifies the capabilities of this hardware unit.
    WebVRDeviceTypeMask flags;
    // Will only contain valid data if (flags & HasHMDDevice).
    WebVRHMDInfo hmdInfo;
};

}

#endif // WebVR_h
