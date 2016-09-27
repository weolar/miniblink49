// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/navigatorconnect/AcceptConnectionObserver.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptFunction.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/navigatorconnect/ServicePort.h"
#include "modules/navigatorconnect/ServicePortConnectResponse.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"

namespace blink {

// Function that is called when the promise passed to acceptConnection is either
// rejected or fulfilled. Calls the corresponding method on
// AcceptConnectionObserver.
class AcceptConnectionObserver::ThenFunction final : public ScriptFunction {
public:
    enum ResolveType {
        Fulfilled,
        Rejected,
    };

    static v8::Local<v8::Function> createFunction(ScriptState* scriptState, AcceptConnectionObserver* observer, ResolveType type)
    {
        ThenFunction* self = new ThenFunction(scriptState, observer, type);
        return self->bindToV8Function();
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_observer);
        ScriptFunction::trace(visitor);
    }

private:
    ThenFunction(ScriptState* scriptState, AcceptConnectionObserver* observer, ResolveType type)
        : ScriptFunction(scriptState)
        , m_observer(observer)
        , m_resolveType(type)
    {
    }

    ScriptValue call(ScriptValue value) override
    {
        ASSERT(m_observer);
        ASSERT(m_resolveType == Fulfilled || m_resolveType == Rejected);
        if (m_resolveType == Rejected)
            m_observer->responseWasRejected();
        else
            m_observer->responseWasResolved(value);
        m_observer = nullptr;
        return value;
    }

    Member<AcceptConnectionObserver> m_observer;
    ResolveType m_resolveType;
};

AcceptConnectionObserver* AcceptConnectionObserver::create(ExecutionContext* context, int eventID)
{
    return new AcceptConnectionObserver(context, eventID);
}

AcceptConnectionObserver* AcceptConnectionObserver::create(ServicePortCollection* collection, PassOwnPtr<WebServicePortConnectEventCallbacks> callbacks, WebServicePortID portID, const KURL& targetURL)
{
    return new AcceptConnectionObserver(collection, callbacks, portID, targetURL);
}

void AcceptConnectionObserver::contextDestroyed()
{
    ContextLifecycleObserver::contextDestroyed();
    m_state = Done;
}

void AcceptConnectionObserver::didDispatchEvent()
{
    ASSERT(executionContext());
    if (m_state != Initial)
        return;
    responseWasRejected();
}

void AcceptConnectionObserver::acceptConnection(ScriptState* scriptState, ScriptPromise value, ExceptionState& exceptionState)
{
    if (m_state != Initial) {
        exceptionState.throwDOMException(InvalidStateError, "acceptConnection was already called.");
        return;
    }

    m_state = Pending;
    value.then(
        ThenFunction::createFunction(scriptState, this, ThenFunction::Fulfilled),
        ThenFunction::createFunction(scriptState, this, ThenFunction::Rejected));
}

ScriptPromise AcceptConnectionObserver::respondWith(ScriptState* scriptState, ScriptPromise value, ExceptionState& exceptionState)
{
    if (m_state != Initial) {
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(InvalidStateError, "respondWith was already called."));
    }

    m_state = Pending;
    m_resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = m_resolver->promise();
    value.then(
        ThenFunction::createFunction(scriptState, this, ThenFunction::Fulfilled),
        ThenFunction::createFunction(scriptState, this, ThenFunction::Rejected));
    return promise;
}

void AcceptConnectionObserver::responseWasRejected()
{
    ASSERT(executionContext());
    if (m_resolver)
        m_resolver->reject(DOMException::create(AbortError));
    if (m_callbacks) {
        m_callbacks->onError();
    } else {
        ServiceWorkerGlobalScopeClient::from(executionContext())->didHandleCrossOriginConnectEvent(m_eventID, false);
    }
    m_state = Done;
}

void AcceptConnectionObserver::responseWasResolved(const ScriptValue& value)
{
    ASSERT(executionContext());
    if (!m_resolver) {
        // TODO(mek): Get rid of this block when observer is only used for
        // service port connect events.
        if (!value.v8Value()->IsTrue()) {
            responseWasRejected();
            return;
        }
        ServiceWorkerGlobalScopeClient::from(executionContext())->didHandleCrossOriginConnectEvent(m_eventID, true);
        m_state = Done;
        return;
    }

    ScriptState* scriptState = m_resolver->scriptState();
    ExceptionState exceptionState(ExceptionState::UnknownContext, nullptr, nullptr, scriptState->context()->Global(), scriptState->isolate());
    ServicePortConnectResponse response = ScriptValue::to<ServicePortConnectResponse>(scriptState->isolate(), value, exceptionState);
    if (exceptionState.hadException()) {
        exceptionState.reject(m_resolver.get());
        m_resolver = nullptr;
        responseWasRejected();
        return;
    }
    if (!response.hasAccept() || !response.accept()) {
        responseWasRejected();
        return;
    }
    WebServicePort webPort;
    webPort.targetUrl = m_targetURL;
    if (response.hasName())
        webPort.name = response.name();
    if (response.hasData()) {
        webPort.data = SerializedScriptValueFactory::instance().create(scriptState->isolate(), response.data(), nullptr, exceptionState)->toWireString();
        if (exceptionState.hadException()) {
            exceptionState.reject(m_resolver.get());
            m_resolver = nullptr;
            responseWasRejected();
            return;
        }
    }
    webPort.id = m_portID;
    ServicePort* port = ServicePort::create(m_collection, webPort);
    m_collection->addPort(port);
    m_resolver->resolve(port);
    m_callbacks->onSuccess(&webPort);
    m_state = Done;
}

AcceptConnectionObserver::AcceptConnectionObserver(ExecutionContext* context, int eventID)
    : ContextLifecycleObserver(context)
    , m_eventID(eventID)
    , m_portID(-1)
    , m_state(Initial)
{
}

AcceptConnectionObserver::AcceptConnectionObserver(ServicePortCollection* collection, PassOwnPtr<WebServicePortConnectEventCallbacks> callbacks, WebServicePortID portID, const KURL& targetURL)
    : ContextLifecycleObserver(collection->executionContext())
    , m_eventID(-1)
    , m_callbacks(callbacks)
    , m_collection(collection)
    , m_portID(portID)
    , m_targetURL(targetURL)
    , m_state(Initial)
{
}

DEFINE_TRACE(AcceptConnectionObserver)
{
    visitor->trace(m_collection);
    visitor->trace(m_resolver);
    ContextLifecycleObserver::trace(visitor);
}

} // namespace blink
