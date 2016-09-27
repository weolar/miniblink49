// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerRegistrationPush_h
#define ServiceWorkerRegistrationPush_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class PushManager;
class ServiceWorkerRegistration;

class ServiceWorkerRegistrationPush final : public GarbageCollectedFinalized<ServiceWorkerRegistrationPush>, public HeapSupplement<ServiceWorkerRegistration> {
    USING_GARBAGE_COLLECTED_MIXIN(ServiceWorkerRegistrationPush);
    WTF_MAKE_NONCOPYABLE(ServiceWorkerRegistrationPush);
public:
    virtual ~ServiceWorkerRegistrationPush();
    static ServiceWorkerRegistrationPush& from(ServiceWorkerRegistration&);

    static PushManager* pushManager(ServiceWorkerRegistration&);
    PushManager* pushManager();

    DECLARE_VIRTUAL_TRACE();

private:
    explicit ServiceWorkerRegistrationPush(ServiceWorkerRegistration*);
    static const char* supplementName();

    Member<ServiceWorkerRegistration> m_registration;
    Member<PushManager> m_pushManager;
};

} // namespace blink

#endif // ServiceWorkerRegistrationPush_h
