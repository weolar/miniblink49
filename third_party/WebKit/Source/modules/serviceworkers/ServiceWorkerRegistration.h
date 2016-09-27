// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerRegistration_h
#define ServiceWorkerRegistration_h

#include "core/dom/ActiveDOMObject.h"
#include "core/events/EventTarget.h"
#include "modules/serviceworkers/ServiceWorker.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"
#include "platform/Supplementable.h"
#include "public/platform/WebServiceWorkerRegistration.h"
#include "public/platform/WebServiceWorkerRegistrationProxy.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class ScriptPromise;
class ScriptPromiseResolver;
class ScriptState;
class WebServiceWorkerProvider;

class ServiceWorkerRegistration final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<ServiceWorkerRegistration>
    , public ActiveDOMObject
    , public WebServiceWorkerRegistrationProxy
    , public HeapSupplementable<ServiceWorkerRegistration> {
    DEFINE_WRAPPERTYPEINFO();
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(ServiceWorkerRegistration);
    USING_GARBAGE_COLLECTED_MIXIN(ServiceWorkerRegistration);
public:
    // EventTarget overrides.
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override { return ActiveDOMObject::executionContext(); }

    // WebServiceWorkerRegistrationProxy overrides.
    void dispatchUpdateFoundEvent() override;
    void setInstalling(WebServiceWorker*) override;
    void setWaiting(WebServiceWorker*) override;
    void setActive(WebServiceWorker*) override;

    // For CallbackPromiseAdapter.
    typedef WebServiceWorkerRegistration WebType;
    static ServiceWorkerRegistration* from(ExecutionContext*, WebType* registration);
    static ServiceWorkerRegistration* take(ScriptPromiseResolver*, WebType* registration);
    static void dispose(WebType* registration);

    PassRefPtrWillBeRawPtr<ServiceWorker> installing() { return m_installing.get(); }
    PassRefPtrWillBeRawPtr<ServiceWorker> waiting() { return m_waiting.get(); }
    PassRefPtrWillBeRawPtr<ServiceWorker> active() { return m_active.get(); }

    String scope() const;

    WebServiceWorkerRegistration* webRegistration() { return m_outerRegistration.get(); }

    void update(ScriptState*, ExceptionState&);
    ScriptPromise unregister(ScriptState*);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(updatefound);

    ~ServiceWorkerRegistration() override;

    // Eager finalization needed to promptly release owned WebServiceWorkerRegistration.
    EAGERLY_FINALIZE();
    DECLARE_VIRTUAL_TRACE();

private:
    static ServiceWorkerRegistration* getOrCreate(ExecutionContext*, WebServiceWorkerRegistration*);
    ServiceWorkerRegistration(ExecutionContext*, PassOwnPtr<WebServiceWorkerRegistration>);

    // ActiveDOMObject overrides.
    bool hasPendingActivity() const override;
    void stop() override;

    OwnPtr<WebServiceWorkerRegistration> m_outerRegistration;
    WebServiceWorkerProvider* m_provider;
    RefPtrWillBeMember<ServiceWorker> m_installing;
    RefPtrWillBeMember<ServiceWorker> m_waiting;
    RefPtrWillBeMember<ServiceWorker> m_active;

    bool m_stopped;
};

class ServiceWorkerRegistrationArray {
public:
    typedef WebVector<WebServiceWorkerRegistration*> WebType;
    static HeapVector<Member<ServiceWorkerRegistration>> take(ScriptPromiseResolver* resolver, PassOwnPtr<WebType> webServiceWorkerRegistrations)
    {
        HeapVector<Member<ServiceWorkerRegistration>> registrations;
        for (WebServiceWorkerRegistration* registration : *webServiceWorkerRegistrations)
            registrations.append(ServiceWorkerRegistration::take(resolver, registration));
        return registrations;
    }

    static void dispose(PassOwnPtr<WebType> webServiceWorkerRegistrations)
    {
        for (WebServiceWorkerRegistration* registration : *webServiceWorkerRegistrations)
            ServiceWorkerRegistration::dispose(registration);
    }

private:
    WTF_MAKE_NONCOPYABLE(ServiceWorkerRegistrationArray);
    ServiceWorkerRegistrationArray() = delete;
};

} // namespace blink

#endif // ServiceWorkerRegistration_h
