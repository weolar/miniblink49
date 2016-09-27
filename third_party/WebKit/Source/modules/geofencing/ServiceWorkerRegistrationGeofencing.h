// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerRegistrationGeofencing_h
#define ServiceWorkerRegistrationGeofencing_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class Geofencing;
class ServiceWorkerRegistration;

class ServiceWorkerRegistrationGeofencing final : public GarbageCollectedFinalized<ServiceWorkerRegistrationGeofencing>, public HeapSupplement<ServiceWorkerRegistration> {
    USING_GARBAGE_COLLECTED_MIXIN(ServiceWorkerRegistrationGeofencing);
    WTF_MAKE_NONCOPYABLE(ServiceWorkerRegistrationGeofencing);
public:
    virtual ~ServiceWorkerRegistrationGeofencing();
    static ServiceWorkerRegistrationGeofencing& from(ServiceWorkerRegistration&);

    static Geofencing* geofencing(ServiceWorkerRegistration&);
    Geofencing* geofencing();

    DECLARE_VIRTUAL_TRACE();

private:
    explicit ServiceWorkerRegistrationGeofencing(ServiceWorkerRegistration*);
    static const char* supplementName();

    Member<ServiceWorkerRegistration> m_registration;
    Member<Geofencing> m_geofencing;
};

} // namespace blink

#endif // ServiceWorkerRegistrationGeofencing_h
