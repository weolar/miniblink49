// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/push_messaging/PushManager.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/push_messaging/PushController.h"
#include "modules/push_messaging/PushError.h"
#include "modules/push_messaging/PushPermissionStatusCallbacks.h"
#include "modules/push_messaging/PushSubscription.h"
#include "modules/push_messaging/PushSubscriptionCallbacks.h"
#include "modules/push_messaging/PushSubscriptionOptions.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/push_messaging/WebPushClient.h"
#include "public/platform/modules/push_messaging/WebPushProvider.h"
#include "public/platform/modules/push_messaging/WebPushSubscriptionOptions.h"
#include "wtf/RefPtr.h"

namespace blink {
namespace {

WebPushProvider* pushProvider()
{
    WebPushProvider* webPushProvider = Platform::current()->pushProvider();
    ASSERT(webPushProvider);
    return webPushProvider;
}

WebPushSubscriptionOptions toWebPushSubscriptionOptions(const PushSubscriptionOptions& options)
{
    WebPushSubscriptionOptions webOptions;
    webOptions.userVisibleOnly = options.userVisibleOnly();
    return webOptions;
}

} // namespace

PushManager::PushManager(ServiceWorkerRegistration* registration)
    : m_registration(registration)
{
    ASSERT(registration);
}

ScriptPromise PushManager::subscribe(ScriptState* scriptState, const PushSubscriptionOptions& options)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Subscription failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    // The document context is the only reasonable context from which to ask the user for permission
    // to use the Push API. The embedder should persist the permission so that later calls in
    // different contexts can succeed.
    if (scriptState->executionContext()->isDocument()) {
        Document* document = toDocument(scriptState->executionContext());
        // FIXME: add test coverage for this condition - https://crbug.com/440431
        if (!document->domWindow() || !document->frame())
            return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(InvalidStateError, "Document is detached from window."));
        PushController::clientFrom(document->frame()).subscribe(m_registration->webRegistration(), toWebPushSubscriptionOptions(options), new PushSubscriptionCallbacks(resolver, m_registration));
    } else {
        pushProvider()->subscribe(m_registration->webRegistration(), toWebPushSubscriptionOptions(options), new PushSubscriptionCallbacks(resolver, m_registration));
    }

    return promise;
}

ScriptPromise PushManager::getSubscription(ScriptState* scriptState)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    pushProvider()->getSubscription(m_registration->webRegistration(), new PushSubscriptionCallbacks(resolver, m_registration));
    return promise;
}

ScriptPromise PushManager::permissionState(ScriptState* scriptState, const PushSubscriptionOptions& options)
{
    if (scriptState->executionContext()->isDocument()) {
        Document* document = toDocument(scriptState->executionContext());
        // FIXME: add test coverage for this condition - https://crbug.com/440431
        if (!document->domWindow() || !document->frame())
            return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(InvalidStateError, "Document is detached from window."));
    }

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    pushProvider()->getPermissionStatus(m_registration->webRegistration(), toWebPushSubscriptionOptions(options), new PushPermissionStatusCallbacks(resolver));
    return promise;
}

DEFINE_TRACE(PushManager)
{
    visitor->trace(m_registration);
}

} // namespace blink
