// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PushManager_h
#define PushManager_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"

namespace blink {

class PushSubscriptionOptions;
class ScriptPromise;
class ScriptState;
class ServiceWorkerRegistration;

class PushManager final : public GarbageCollected<PushManager>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PushManager* create(ServiceWorkerRegistration* registration)
    {
        return new PushManager(registration);
    }

    ScriptPromise subscribe(ScriptState*, const PushSubscriptionOptions&);
    ScriptPromise getSubscription(ScriptState*);
    ScriptPromise permissionState(ScriptState*, const PushSubscriptionOptions&);

    DECLARE_TRACE();

private:
    explicit PushManager(ServiceWorkerRegistration*);

    Member<ServiceWorkerRegistration> m_registration;
};

} // namespace blink

#endif // PushManager_h
