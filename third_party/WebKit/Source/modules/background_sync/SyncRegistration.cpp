// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/background_sync/SyncRegistration.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/background_sync/SyncCallbacks.h"
#include "modules/background_sync/SyncError.h"
#include "modules/background_sync/SyncRegistrationOptions.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/background_sync/WebSyncProvider.h"
#include "public/platform/modules/background_sync/WebSyncRegistration.h"
#include "wtf/OwnPtr.h"

namespace blink {

SyncRegistration* SyncRegistration::take(ScriptPromiseResolver*, WebSyncRegistration* syncRegistration, ServiceWorkerRegistration* serviceWorkerRegistration)
{
    OwnPtr<WebSyncRegistration> registration = adoptPtr(syncRegistration);
    SyncRegistrationOptions options = SyncRegistrationOptions();
    options.setTag(syncRegistration->tag);
    return new SyncRegistration(syncRegistration->id, options, serviceWorkerRegistration);
}

void SyncRegistration::dispose(WebSyncRegistration* syncRegistration)
{
    if (syncRegistration)
        delete syncRegistration;
}

SyncRegistration::SyncRegistration(int64_t id, const SyncRegistrationOptions& options, ServiceWorkerRegistration* serviceWorkerRegistration)
    : m_id(id)
    , m_tag(options.tag())
    , m_serviceWorkerRegistration(serviceWorkerRegistration)
{
}

SyncRegistration::~SyncRegistration()
{
}

ScriptPromise SyncRegistration::unregister(ScriptState* scriptState)
{
    if (m_id == WebSyncRegistration::UNREGISTERED_SYNC_ID)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "Operation failed - not a valid registration object"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    WebSyncProvider* webSyncProvider = Platform::current()->backgroundSyncProvider();
    ASSERT(webSyncProvider);

    webSyncProvider->unregisterBackgroundSync(WebSyncRegistration::PeriodicityOneShot, m_id, m_tag, m_serviceWorkerRegistration->webRegistration(), new SyncUnregistrationCallbacks(resolver, m_serviceWorkerRegistration));

    return promise;
}

DEFINE_TRACE(SyncRegistration)
{
    visitor->trace(m_serviceWorkerRegistration);
}

} // namespace blink
