// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServicePortCollection_h
#define ServicePortCollection_h

#include "bindings/core/v8/ScriptPromise.h"
#include "core/dom/ContextLifecycleObserver.h"
#include "core/events/EventTarget.h"
#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/navigator_services/WebServicePortCallbacks.h"
#include "public/platform/modules/navigator_services/WebServicePortProvider.h"
#include "public/platform/modules/navigator_services/WebServicePortProviderClient.h"
#include "wtf/RefCounted.h"

namespace blink {

class ServicePort;
class ServicePortConnectOptions;
class ServicePortMatchOptions;

class MODULES_EXPORT ServicePortCollection final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<ServicePortCollection>
    , public ContextLifecycleObserver
    , public WebServicePortProviderClient {
    DEFINE_WRAPPERTYPEINFO();
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(ServicePortCollection);
    WTF_MAKE_NONCOPYABLE(ServicePortCollection);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ServicePortCollection);
public:
    static ServicePortCollection* create(ExecutionContext*);
    ~ServicePortCollection() override;

    // Adds a port to this collection to keep track of it and allow delivering
    // events related to it.
    void addPort(ServicePort*);

    // Called when a port is closed to signal the WebServicePortProvider as well
    // as remove the port from this collection.
    void closePort(ServicePort*);

    WebServicePortProvider* provider() { return m_provider.get(); }

    // ServicePortCollection.idl
    ScriptPromise connect(ScriptState*, const String& url, const ServicePortConnectOptions&, ExceptionState&);
    ScriptPromise match(ScriptState*, const ServicePortMatchOptions&);
    ScriptPromise matchAll(ScriptState*, const ServicePortMatchOptions&);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(connect);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(message);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(close);

    // EventTarget overrides.
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    // WebServicePortProviderClient overrides.
    void postMessage(WebServicePortID, const WebString&, const WebMessagePortChannelArray&) override;

    void dispatchConnectEvent(PassOwnPtr<WebServicePortConnectEventCallbacks>, const WebURL& targetURL, const WebString& origin, WebServicePortID);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit ServicePortCollection(ExecutionContext*);

    OwnPtr<WebServicePortProvider> m_provider;
    HeapVector<Member<ServicePort>> m_ports;
};

} // namespace blink

#endif // ServicePortCollection_h
