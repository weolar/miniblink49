// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/geofencing/Geofencing.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/geofencing/CircularGeofencingRegion.h"
#include "modules/geofencing/GeofencingError.h"
#include "modules/geofencing/GeofencingRegion.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCircularGeofencingRegion.h"
#include "public/platform/WebGeofencingProvider.h"
#include "public/platform/WebGeofencingRegistration.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

namespace {

// For CallbackPromiseAdapter to convert a WebVector of regions to a HeapVector.
class RegionArray {
public:
    typedef WebVector<WebGeofencingRegistration> WebType;
    static HeapVector<Member<GeofencingRegion>> take(ScriptPromiseResolver* resolver, PassOwnPtr<WebType> webRegions)
    {
        HeapVector<Member<GeofencingRegion>> regions;
        for (size_t i = 0; i < webRegions->size(); ++i)
            regions.append(CircularGeofencingRegion::create((*webRegions)[i].id, (*webRegions)[i].region));
        return regions;
    }

private:
    RegionArray();
};

} // namespace

Geofencing::Geofencing(ServiceWorkerRegistration* registration)
    : m_registration(registration)
{
}

ScriptPromise Geofencing::registerRegion(ScriptState* scriptState, GeofencingRegion* region)
{
    WebGeofencingProvider* provider = Platform::current()->geofencingProvider();
    if (!provider)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    WebGeofencingCallbacks* callbacks = new CallbackPromiseAdapter<void, GeofencingError>(resolver);
    WebServiceWorkerRegistration* serviceWorkerRegistration = nullptr;
    if (m_registration)
        serviceWorkerRegistration = m_registration->webRegistration();
    provider->registerRegion(region->id(), toCircularGeofencingRegion(region)->webRegion(), serviceWorkerRegistration, callbacks);
    return promise;
}

ScriptPromise Geofencing::unregisterRegion(ScriptState* scriptState, const String& regionId)
{
    WebGeofencingProvider* provider = Platform::current()->geofencingProvider();
    if (!provider)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    WebGeofencingCallbacks* callbacks = new CallbackPromiseAdapter<void, GeofencingError>(resolver);
    WebServiceWorkerRegistration* serviceWorkerRegistration = nullptr;
    if (m_registration)
        serviceWorkerRegistration = m_registration->webRegistration();
    provider->unregisterRegion(regionId, serviceWorkerRegistration, callbacks);
    return promise;
}

ScriptPromise Geofencing::getRegisteredRegions(ScriptState* scriptState) const
{
    WebGeofencingProvider* provider = Platform::current()->geofencingProvider();
    if (!provider)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    WebGeofencingRegionsCallbacks* callbacks = new CallbackPromiseAdapter<RegionArray, GeofencingError>(resolver);
    WebServiceWorkerRegistration* serviceWorkerRegistration = nullptr;
    if (m_registration)
        serviceWorkerRegistration = m_registration->webRegistration();
    provider->getRegisteredRegions(serviceWorkerRegistration, callbacks);
    return promise;
}

DEFINE_TRACE(Geofencing)
{
    visitor->trace(m_registration);
}

} // namespace blink
