// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PeriodicSyncManager_h
#define PeriodicSyncManager_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class PeriodicSyncRegistrationOptions;
class ScriptPromise;
class ScriptState;
class ServiceWorkerRegistration;

class PeriodicSyncManager final : public GarbageCollected<PeriodicSyncManager> , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PeriodicSyncManager* create(ServiceWorkerRegistration* registration)
    {
        return new PeriodicSyncManager(registration);
    }

    unsigned long minPossiblePeriod();

    ScriptPromise registerFunction(ScriptState*);
    ScriptPromise registerFunction(ScriptState*, const PeriodicSyncRegistrationOptions&);
    ScriptPromise getRegistration(ScriptState*, const String&);
    ScriptPromise getRegistrations(ScriptState*);
    ScriptPromise permissionState(ScriptState*);

    DECLARE_TRACE();

private:
    explicit PeriodicSyncManager(ServiceWorkerRegistration*);

    Member<ServiceWorkerRegistration> m_registration;
};

} // namespace blink

#endif // PeriodicSyncManager_h
