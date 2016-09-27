// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/push_messaging/PushSubscriptionCallbacks.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "modules/push_messaging/PushError.h"
#include "modules/push_messaging/PushSubscription.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"

namespace blink {

PushSubscriptionCallbacks::PushSubscriptionCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver, ServiceWorkerRegistration* serviceWorkerRegistration)
    : m_resolver(resolver)
    , m_serviceWorkerRegistration(serviceWorkerRegistration)
{
    ASSERT(m_resolver);
    ASSERT(m_serviceWorkerRegistration);
}

PushSubscriptionCallbacks::~PushSubscriptionCallbacks()
{
}

void PushSubscriptionCallbacks::onSuccess(WebPushSubscription* webPushSubscription)
{
    if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
        return;

    if (!webPushSubscription) {
        m_resolver->resolve(v8::Null(m_resolver->scriptState()->isolate()));
        return;
    }
    m_resolver->resolve(PushSubscription::take(m_resolver.get(), webPushSubscription, m_serviceWorkerRegistration));
}

void PushSubscriptionCallbacks::onError(WebPushError* error)
{
    OwnPtr<WebPushError> ownError = adoptPtr(error);
    if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
        return;
    m_resolver->reject(PushError::take(m_resolver.get(), ownError.release()));
}

} // namespace blink
