// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/push_messaging/PushSubscription.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/V8ObjectBuilder.h"
#include "modules/push_messaging/PushError.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/push_messaging/WebPushProvider.h"
#include "public/platform/modules/push_messaging/WebPushSubscription.h"
#include "wtf/OwnPtr.h"

namespace blink {

PushSubscription* PushSubscription::take(ScriptPromiseResolver*, WebPushSubscription* pushSubscription, ServiceWorkerRegistration* serviceWorkerRegistration)
{
    OwnPtr<WebPushSubscription> subscription = adoptPtr(pushSubscription);
    return new PushSubscription(subscription->endpoint, serviceWorkerRegistration);
}

void PushSubscription::dispose(WebPushSubscription* pushSubscription)
{
    if (pushSubscription)
        delete pushSubscription;
}

PushSubscription::PushSubscription(const KURL& endpoint, ServiceWorkerRegistration* serviceWorkerRegistration)
    : m_endpoint(endpoint)
    , m_serviceWorkerRegistration(serviceWorkerRegistration)
{
}

PushSubscription::~PushSubscription()
{
}

KURL PushSubscription::endpoint() const
{
    return m_endpoint;
}

ScriptPromise PushSubscription::unsubscribe(ScriptState* scriptState)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    WebPushProvider* webPushProvider = Platform::current()->pushProvider();
    ASSERT(webPushProvider);

    webPushProvider->unsubscribe(m_serviceWorkerRegistration->webRegistration(), new CallbackPromiseAdapter<bool, PushError>(resolver));
    return promise;
}

ScriptValue PushSubscription::toJSONForBinding(ScriptState* scriptState)
{
    V8ObjectBuilder result(scriptState);
    result.addString("endpoint", endpoint());

    return result.scriptValue();
}

DEFINE_TRACE(PushSubscription)
{
    visitor->trace(m_serviceWorkerRegistration);
}

} // namespace blink
