// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCircularGeofencingRegion_h
#define WebCircularGeofencingRegion_h

#include "WebCommon.h"
#include "WebString.h"

namespace blink {

struct WebCircularGeofencingRegion {
    double latitude;
    double longitude;
    double radius;

    WebCircularGeofencingRegion()
        : latitude(0)
        , longitude(0)
        , radius(0)
    {
    }
};

} // namespace blink

#endif // WebCircularGeofencingRegion_h
