// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGeofencingRegistration_h
#define WebGeofencingRegistration_h

#include "WebCircularGeofencingRegion.h"
#include "WebString.h"

namespace blink {

struct WebGeofencingRegistration {
    WebString id;
    WebCircularGeofencingRegion region;

    WebGeofencingRegistration() { }
};

} // namespace blink

#endif // WebGeofencingRegistration_h
