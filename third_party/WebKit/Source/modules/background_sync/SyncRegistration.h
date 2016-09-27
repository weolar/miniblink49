// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SyncRegistration_h
#define SyncRegistration_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/background_sync/SyncRegistrationOptions.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ServiceWorkerRegistration;
class ScriptPromiseResolver;
class ScriptState;
struct WebSyncRegistration;

class SyncRegistration final : public GarbageCollectedFinalized<SyncRegistration>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static SyncRegistration* take(ScriptPromiseResolver*, WebSyncRegistration*, ServiceWorkerRegistration*);
    static void dispose(WebSyncRegistration* registrationRaw);

    virtual ~SyncRegistration();

    bool hasTag() const { return !m_tag.isNull(); }
    String tag() const { return m_tag; }
    void setTag(String value) { m_tag = value; }

    ScriptPromise unregister(ScriptState*);

    DECLARE_TRACE();

private:
    SyncRegistration(int64_t id, const SyncRegistrationOptions&, ServiceWorkerRegistration*);

    int64_t m_id;
    String m_tag;

    Member<ServiceWorkerRegistration> m_serviceWorkerRegistration;
};

} // namespace blink

#endif // SyncRegistration_h
