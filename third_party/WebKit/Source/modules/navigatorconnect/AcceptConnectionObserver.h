// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AcceptConnectionObserver_h
#define AcceptConnectionObserver_h

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/ContextLifecycleObserver.h"
#include "modules/ModulesExport.h"
#include "modules/navigatorconnect/ServicePortCollection.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/navigator_services/WebServicePort.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class ScriptPromise;
class ScriptState;
class ScriptValue;

// This class observes the service worker's handling of a CrossOriginConnectEvent
// and notified the client of the result. Created for each instance of
// CrossOriginConnectEvent.
class MODULES_EXPORT AcceptConnectionObserver final : public GarbageCollectedFinalized<AcceptConnectionObserver>, public ContextLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(AcceptConnectionObserver);
public:
    static AcceptConnectionObserver* create(ExecutionContext*, int eventID);
    static AcceptConnectionObserver* create(ServicePortCollection*, PassOwnPtr<WebServicePortConnectEventCallbacks>, WebServicePortID, const KURL& targetURL);

    void contextDestroyed() override;

    // Must be called after dispatching the event. Will cause the connection to
    // be rejected if no call to acceptConnection or respondWith was made.
    void didDispatchEvent();

    // Observes the promise and delays calling didHandleCrossOriginConnectEvent()
    // until the given promise is resolved or rejected.
    void acceptConnection(ScriptState*, ScriptPromise, ExceptionState&);

    // Observes the promise and delays calling didHandleServicePortConnectEvent()
    // until the given promise is resolved or rejected. Returns a promise that
    // resolves to a ServicePort when the connection is accepted.
    ScriptPromise respondWith(ScriptState*, ScriptPromise, ExceptionState&);

    void responseWasRejected();
    void responseWasResolved(const ScriptValue&);

    DECLARE_VIRTUAL_TRACE();

private:
    class ThenFunction;

    AcceptConnectionObserver(ExecutionContext*, int eventID);
    AcceptConnectionObserver(ServicePortCollection*, PassOwnPtr<WebServicePortConnectEventCallbacks>, WebServicePortID, const KURL& targetURL);

    int m_eventID;
    OwnPtr<WebServicePortConnectEventCallbacks> m_callbacks;
    Member<ServicePortCollection> m_collection;
    WebServicePortID m_portID;
    KURL m_targetURL;
    RefPtrWillBeMember<ScriptPromiseResolver> m_resolver;

    enum State { Initial, Pending, Done };
    State m_state;
};

} // namespace blink

#endif // AcceptConnectionObserver_h
