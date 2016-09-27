// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/background_sync/PeriodicSyncRegistration.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/background_sync/PeriodicSyncRegistrationOptions.h"
#include "modules/background_sync/SyncCallbacks.h"
#include "modules/background_sync/SyncError.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/background_sync/WebSyncProvider.h"
#include "public/platform/modules/background_sync/WebSyncRegistration.h"
#include "wtf/OwnPtr.h"

namespace blink {

PeriodicSyncRegistration* PeriodicSyncRegistration::take(ScriptPromiseResolver*, WebSyncRegistration* syncRegistration, ServiceWorkerRegistration* serviceWorkerRegistration)
{
    OwnPtr<WebSyncRegistration> registration = adoptPtr(syncRegistration);
    PeriodicSyncRegistrationOptions options = PeriodicSyncRegistrationOptions();
    options.setMinPeriod(syncRegistration->minPeriodMs);
    switch (syncRegistration->networkState) {
    case WebSyncRegistration::NetworkState::NetworkStateAny:
        options.setNetworkState("any");
        break;
    case WebSyncRegistration::NetworkState::NetworkStateAvoidCellular:
        options.setNetworkState("avoid-cellular");
        break;
    case WebSyncRegistration::NetworkState::NetworkStateOnline:
        options.setNetworkState("online");
        break;
    }
    switch (syncRegistration->powerState) {
    case WebSyncRegistration::PowerState::PowerStateAuto:
        options.setPowerState("auto");
        break;
    case WebSyncRegistration::PowerState::PowerStateAvoidDraining:
        options.setPowerState("avoid-draining");
        break;
    }
    options.setTag(syncRegistration->tag);
    return new PeriodicSyncRegistration(syncRegistration->id, options, serviceWorkerRegistration);
}

void PeriodicSyncRegistration::dispose(WebSyncRegistration* syncRegistration)
{
    if (syncRegistration)
        delete syncRegistration;
}

PeriodicSyncRegistration::PeriodicSyncRegistration(int64_t id, const PeriodicSyncRegistrationOptions& options, ServiceWorkerRegistration* serviceWorkerRegistration)
    : m_id(id)
    , m_minPeriod(options.minPeriod())
    , m_networkState(options.networkState())
    , m_powerState(options.powerState())
    , m_tag(options.tag())
    , m_serviceWorkerRegistration(serviceWorkerRegistration)
{
}

PeriodicSyncRegistration::~PeriodicSyncRegistration()
{
}

ScriptPromise PeriodicSyncRegistration::unregister(ScriptState* scriptState)
{
    if (m_id == WebSyncRegistration::UNREGISTERED_SYNC_ID)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Operation failed - not a valid registration object"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    WebSyncProvider* webSyncProvider = Platform::current()->backgroundSyncProvider();
    ASSERT(webSyncProvider);

    webSyncProvider->unregisterBackgroundSync(WebSyncRegistration::PeriodicityPeriodic, m_id, m_tag, m_serviceWorkerRegistration->webRegistration(), new SyncUnregistrationCallbacks(resolver, m_serviceWorkerRegistration));

    return promise;
}

DEFINE_TRACE(PeriodicSyncRegistration)
{
    visitor->trace(m_serviceWorkerRegistration);
}

} // namespace blink
