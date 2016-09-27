// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/push_messaging/ServiceWorkerRegistrationPush.h"

#include "modules/push_messaging/PushManager.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"

namespace blink {

ServiceWorkerRegistrationPush::ServiceWorkerRegistrationPush(ServiceWorkerRegistration* registration)
    : m_registration(registration)
{
}

ServiceWorkerRegistrationPush::~ServiceWorkerRegistrationPush()
{
}

const char* ServiceWorkerRegistrationPush::supplementName()
{
    return "ServiceWorkerRegistrationPush";
}

ServiceWorkerRegistrationPush& ServiceWorkerRegistrationPush::from(ServiceWorkerRegistration& registration)
{
    ServiceWorkerRegistrationPush* supplement = static_cast<ServiceWorkerRegistrationPush*>(HeapSupplement<ServiceWorkerRegistration>::from(registration, supplementName()));
    if (!supplement) {
        supplement = new ServiceWorkerRegistrationPush(&registration);
        provideTo(registration, supplementName(), supplement);
    }
    return *supplement;
}

PushManager* ServiceWorkerRegistrationPush::pushManager(ServiceWorkerRegistration& registration)
{
    return ServiceWorkerRegistrationPush::from(registration).pushManager();
}

PushManager* ServiceWorkerRegistrationPush::pushManager()
{
    if (!m_pushManager)
        m_pushManager = PushManager::create(m_registration);
    return m_pushManager.get();
}

DEFINE_TRACE(ServiceWorkerRegistrationPush)
{
    visitor->trace(m_registration);
    visitor->trace(m_pushManager);
    HeapSupplement<ServiceWorkerRegistration>::trace(visitor);
}

} // namespace blink
