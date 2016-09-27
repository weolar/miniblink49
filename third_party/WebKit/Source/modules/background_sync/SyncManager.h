// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SyncManager_h
#define SyncManager_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class SyncRegistrationOptions;
class ScriptPromise;
class ScriptState;
class ServiceWorkerRegistration;

class SyncManager final : public GarbageCollected<SyncManager> , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static SyncManager* create(ServiceWorkerRegistration* registration)
    {
        return new SyncManager(registration);
    }

    unsigned long minAllowablePeriod();

    ScriptPromise registerFunction(ScriptState*);
    ScriptPromise registerFunction(ScriptState*, const SyncRegistrationOptions&);
    ScriptPromise getRegistration(ScriptState*, const String&);
    ScriptPromise getRegistrations(ScriptState*);
    ScriptPromise permissionState(ScriptState*);

    DECLARE_TRACE();

private:
    explicit SyncManager(ServiceWorkerRegistration*);

    Member<ServiceWorkerRegistration> m_registration;
};

} // namespace blink

#endif // SyncManager_h
