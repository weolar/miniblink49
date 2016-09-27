// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PeriodicSyncRegistration_h
#define PeriodicSyncRegistration_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/background_sync/PeriodicSyncRegistrationOptions.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ServiceWorkerRegistration;
class ScriptPromiseResolver;
class ScriptState;
struct WebSyncRegistration;

class PeriodicSyncRegistration final : public GarbageCollectedFinalized<PeriodicSyncRegistration>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PeriodicSyncRegistration* take(ScriptPromiseResolver*, WebSyncRegistration*, ServiceWorkerRegistration*);
    static void dispose(WebSyncRegistration* registrationRaw);

    virtual ~PeriodicSyncRegistration();

    unsigned long minPeriod() const { return m_minPeriod; }
    void setMinPeriod(unsigned long value) { m_minPeriod = value; }

    String networkState() const { return m_networkState; }
    void setNetworkState(String value) { m_networkState = value; }

    String powerState() const { return m_powerState; }
    void setPowerState(String value) { m_powerState = value; }

    String tag() const { return m_tag; }
    void setTag(String value) { m_tag = value; }

    ScriptPromise unregister(ScriptState*);

    DECLARE_TRACE();

private:
    PeriodicSyncRegistration(int64_t id, const PeriodicSyncRegistrationOptions&, ServiceWorkerRegistration*);

    int64_t m_id;
    unsigned long m_minPeriod;
    String m_networkState;
    String m_powerState;
    String m_tag;

    Member<ServiceWorkerRegistration> m_serviceWorkerRegistration;
};

} // namespace blink

#endif // PeriodicSyncRegistration_h
