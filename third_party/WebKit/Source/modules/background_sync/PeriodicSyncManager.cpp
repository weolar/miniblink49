// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/background_sync/PeriodicSyncManager.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/background_sync/PeriodicSyncRegistrationOptions.h"
#include "modules/background_sync/SyncCallbacks.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/background_sync/WebSyncProvider.h"
#include "public/platform/modules/background_sync/WebSyncRegistration.h"
#include "wtf/RefPtr.h"


namespace blink {
namespace {

/* This is the minimum period which will be allowed by the Background
 * Sync manager process. It is recorded here in order to be able to
 * respond to syncManager.minAllowablePeriod.
 * The time is expressed in milliseconds,
 */
unsigned long kMinPossiblePeriod = 12 * 60 * 60 * 1000;

WebSyncProvider* backgroundSyncProvider()
{
    WebSyncProvider* webSyncProvider = Platform::current()->backgroundSyncProvider();
    ASSERT(webSyncProvider);
    return webSyncProvider;
}

}

PeriodicSyncManager::PeriodicSyncManager(ServiceWorkerRegistration* registration)
    : m_registration(registration)
{
    ASSERT(registration);
}

unsigned long PeriodicSyncManager::minPossiblePeriod()
{
    return kMinPossiblePeriod;
}

ScriptPromise PeriodicSyncManager::registerFunction(ScriptState* scriptState, const PeriodicSyncRegistrationOptions& options)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Registration failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    WebSyncRegistration::NetworkState networkState;
    String networkStateString = options.networkState();
    if (networkStateString == "any") {
        networkState = WebSyncRegistration::NetworkState::NetworkStateAny;
    } else if (networkStateString == "avoid-cellular") {
        networkState = WebSyncRegistration::NetworkState::NetworkStateAvoidCellular;
    } else {
        networkState = WebSyncRegistration::NetworkState::NetworkStateOnline;
    }
    WebSyncRegistration::PowerState powerState;
    String powerStateString = options.powerState();
    if (powerStateString == "avoid-draining") {
        powerState = WebSyncRegistration::PowerState::PowerStateAvoidDraining;
    } else {
        powerState = WebSyncRegistration::PowerState::PowerStateAuto;
    }
    WebSyncRegistration* webSyncRegistration = new WebSyncRegistration(
        WebSyncRegistration::UNREGISTERED_SYNC_ID,
        WebSyncRegistration::PeriodicityPeriodic,
        options.tag(),
        options.minPeriod(),
        networkState,
        powerState
    );
    backgroundSyncProvider()->registerBackgroundSync(webSyncRegistration, m_registration->webRegistration(), new SyncRegistrationCallbacks(resolver, m_registration));

    return promise;
}

ScriptPromise PeriodicSyncManager::getRegistration(ScriptState* scriptState, const String& syncRegistrationTag)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Operation failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    backgroundSyncProvider()->getRegistration(WebSyncRegistration::PeriodicityPeriodic, syncRegistrationTag, m_registration->webRegistration(), new SyncRegistrationCallbacks(resolver, m_registration));

    return promise;
}

ScriptPromise PeriodicSyncManager::getRegistrations(ScriptState* scriptState)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Operation failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    backgroundSyncProvider()->getRegistrations(WebSyncRegistration::PeriodicityPeriodic, m_registration->webRegistration(), new SyncGetRegistrationsCallbacks(resolver, m_registration));

    return promise;
}

ScriptPromise PeriodicSyncManager::permissionState(ScriptState* scriptState)
{
    if (!m_registration->active())
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Operation failed - no active Service Worker"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    backgroundSyncProvider()->getPermissionStatus(WebSyncRegistration::PeriodicityPeriodic, m_registration->webRegistration(), new SyncGetPermissionStatusCallbacks(resolver, m_registration));

    return promise;
}

DEFINE_TRACE(PeriodicSyncManager)
{
    visitor->trace(m_registration);
}

} // namespace blink
