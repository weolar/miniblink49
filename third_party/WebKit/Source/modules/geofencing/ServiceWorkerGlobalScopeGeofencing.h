// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerGlobalScopeGeofencing_h
#define ServiceWorkerGlobalScopeGeofencing_h

#include "core/events/EventTarget.h"

namespace blink {

class ServiceWorkerGlobalScopeGeofencing {
public:
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(geofenceenter);
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(geofenceleave);
};

} // namespace blink

#endif // ServiceWorkerGlobalScopeGeofencing_h
