// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/geofencing/ServiceWorkerRegistrationGeofencing.h"

#include "modules/geofencing/Geofencing.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"

namespace blink {

ServiceWorkerRegistrationGeofencing::ServiceWorkerRegistrationGeofencing(ServiceWorkerRegistration* registration)
    : m_registration(registration)
{
}

ServiceWorkerRegistrationGeofencing::~ServiceWorkerRegistrationGeofencing()
{
}

const char* ServiceWorkerRegistrationGeofencing::supplementName()
{
    return "ServiceWorkerRegistrationGeofencing";
}

ServiceWorkerRegistrationGeofencing& ServiceWorkerRegistrationGeofencing::from(ServiceWorkerRegistration& registration)
{
    ServiceWorkerRegistrationGeofencing* supplement = static_cast<ServiceWorkerRegistrationGeofencing*>(HeapSupplement<ServiceWorkerRegistration>::from(registration, supplementName()));
    if (!supplement) {
        supplement = new ServiceWorkerRegistrationGeofencing(&registration);
        provideTo(registration, supplementName(), supplement);
    }
    return *supplement;
}

Geofencing* ServiceWorkerRegistrationGeofencing::geofencing(ServiceWorkerRegistration& registration)
{
    return ServiceWorkerRegistrationGeofencing::from(registration).geofencing();
}

Geofencing* ServiceWorkerRegistrationGeofencing::geofencing()
{
    if (!m_geofencing)
        m_geofencing = Geofencing::create(m_registration);
    return m_geofencing.get();
}

DEFINE_TRACE(ServiceWorkerRegistrationGeofencing)
{
    visitor->trace(m_registration);
    visitor->trace(m_geofencing);
    HeapSupplement<ServiceWorkerRegistration>::trace(visitor);
}

} // namespace blink
