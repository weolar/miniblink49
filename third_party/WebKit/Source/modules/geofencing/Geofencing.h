// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Geofencing_h
#define Geofencing_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class GeofencingRegion;
class ScriptPromise;
class ScriptState;
class ServiceWorkerRegistration;

class Geofencing final : public GarbageCollected<Geofencing>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static Geofencing* create(ServiceWorkerRegistration* registration)
    {
        return new Geofencing(registration);
    }

    ScriptPromise registerRegion(ScriptState*, GeofencingRegion*);
    ScriptPromise unregisterRegion(ScriptState*, const String& regionId);
    ScriptPromise getRegisteredRegions(ScriptState*) const;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit Geofencing(ServiceWorkerRegistration*);

    Member<ServiceWorkerRegistration> m_registration;
};

} // namespace blink

#endif // Geofencing_h
