// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/background_sync/SyncManager.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/background_sync/SyncCallbacks.h"
#include "modules/background_sync/SyncRegistrationOptions.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/background_sync/WebSyncProvider.h"
#include "public/platform/modules/background_sync/WebSyncRegistration.h"
#include "wtf/RefPtr.h"


namespace blink {
namespace {

WebSyncProvider* backgroundSyncProvider()
{
    WebSyncProvider* webSyncProvider = Platform::current()->backgroundSyncProvider();
    ASSERT(webSyncProvider);
    return webSyncProvider;
}

}

SyncManager::SyncManager(ServiceWorkerRegistration* registration)
    : m_registration(registration)
{
    ASSERT(registration);
}

ScriptPromise SyncManager::registerFunction(ScriptState* scriptState, const SyncRegistrationOptions& options)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Registration failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    WebSyncRegistration* webSyncRegistration = new WebSyncRegistration(
        WebSyncRegistration::UNREGISTERED_SYNC_ID /* id */,
        WebSyncRegistration::PeriodicityOneShot,
        options.tag(),
        0 /* minPeriod */,
        WebSyncRegistration::NetworkStateOnline /* networkState */,
        WebSyncRegistration::PowerStateAuto /* powerState */
    );
    backgroundSyncProvider()->registerBackgroundSync(webSyncRegistration, m_registration->webRegistration(), new SyncRegistrationCallbacks(resolver, m_registration));

    return promise;
}

ScriptPromise SyncManager::getRegistration(ScriptState* scriptState, const String& syncRegistrationId)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Operation failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    backgroundSyncProvider()->getRegistration(WebSyncRegistration::PeriodicityOneShot, syncRegistrationId, m_registration->webRegistration(), new SyncRegistrationCallbacks(resolver, m_registration));

    return promise;
}

ScriptPromise SyncManager::getRegistrations(ScriptState* scriptState)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Operation failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    backgroundSyncProvider()->getRegistrations(WebSyncRegistration::PeriodicityOneShot, m_registration->webRegistration(), new SyncGetRegistrationsCallbacks(resolver, m_registration));

    return promise;
}

ScriptPromise SyncManager::permissionState(ScriptState* scriptState)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Operation failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    backgroundSyncProvider()->getPermissionStatus(WebSyncRegistration::PeriodicityOneShot, m_registration->webRegistration(), new SyncGetPermissionStatusCallbacks(resolver, m_registration));

    return promise;
}

DEFINE_TRACE(SyncManager)
{
    visitor->trace(m_registration);
}

} // namespace blink
