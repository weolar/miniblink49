// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/RespondWithObserver.h"

#include "bindings/core/v8/ScriptFunction.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/modules/v8/V8Response.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/streams/Stream.h"
#include "modules/fetch/BodyStreamBuffer.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "public/platform/WebServiceWorkerResponse.h"
#include "wtf/Assertions.h"
#include "wtf/RefPtr.h"
#include <v8.h>

namespace blink {

class RespondWithObserver::ThenFunction final : public ScriptFunction {
public:
    enum ResolveType {
        Fulfilled,
        Rejected,
    };

    static v8::Local<v8::Function> createFunction(ScriptState* scriptState, RespondWithObserver* observer, ResolveType type)
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
    ThenFunction(ScriptState* scriptState, RespondWithObserver* observer, ResolveType type)
        : ScriptFunction(scriptState)
        , m_observer(observer)
        , m_resolveType(type)
    {
    }

    ScriptValue call(ScriptValue value) override
    {
        ASSERT(m_observer);
        ASSERT(m_resolveType == Fulfilled || m_resolveType == Rejected);
        if (m_resolveType == Rejected) {
            m_observer->responseWasRejected(WebServiceWorkerResponseErrorPromiseRejected);
            value = ScriptPromise::reject(value.scriptState(), value).scriptValue();
        } else {
            m_observer->responseWasFulfilled(value);
        }
        m_observer = nullptr;
        return value;
    }

    Member<RespondWithObserver> m_observer;
    ResolveType m_resolveType;
};

RespondWithObserver* RespondWithObserver::create(ExecutionContext* context, int eventID, WebURLRequest::FetchRequestMode requestMode, WebURLRequest::FrameType frameType)
{
    return new RespondWithObserver(context, eventID, requestMode, frameType);
}

void RespondWithObserver::contextDestroyed()
{
    ContextLifecycleObserver::contextDestroyed();
    m_state = Done;
}

void RespondWithObserver::didDispatchEvent(bool defaultPrevented)
{
    ASSERT(executionContext());
    if (m_state != Initial)
        return;

    if (defaultPrevented) {
        responseWasRejected(WebServiceWorkerResponseErrorDefaultPrevented);
        return;
    }

    ServiceWorkerGlobalScopeClient::from(executionContext())->didHandleFetchEvent(m_eventID);
    m_state = Done;
}

void RespondWithObserver::respondWith(ScriptState* scriptState, const ScriptValue& value, ExceptionState& exceptionState)
{
    if (m_state != Initial) {
        exceptionState.throwDOMException(InvalidStateError, "The fetch event has already been responded to.");
        return;
    }

    m_state = Pending;
    ScriptPromise::cast(scriptState, value).then(
        ThenFunction::createFunction(scriptState, this, ThenFunction::Fulfilled),
        ThenFunction::createFunction(scriptState, this, ThenFunction::Rejected));
}

void RespondWithObserver::responseWasRejected(WebServiceWorkerResponseError error)
{
    ASSERT(executionContext());
    // The default value of WebServiceWorkerResponse's status is 0, which maps
    // to a network error.
    WebServiceWorkerResponse webResponse;
    webResponse.setError(error);
    ServiceWorkerGlobalScopeClient::from(executionContext())->didHandleFetchEvent(m_eventID, webResponse);
    m_state = Done;
}

void RespondWithObserver::responseWasFulfilled(const ScriptValue& value)
{
    ASSERT(executionContext());
    if (!V8Response::hasInstance(value.v8Value(), toIsolate(executionContext()))) {
        responseWasRejected(WebServiceWorkerResponseErrorNoV8Instance);
        return;
    }
    Response* response = V8Response::toImplWithTypeCheck(toIsolate(executionContext()), value.v8Value());
    // "If one of the following conditions is true, return a network error:
    //   - |response|'s type is |error|.
    //   - |request|'s mode is not |no-cors| and response's type is |opaque|.
    //   - |request| is a client request and |response|'s type is neither
    //     |basic| nor |default|."
    const FetchResponseData::Type responseType = response->response()->type();
    if (responseType == FetchResponseData::ErrorType) {
        responseWasRejected(WebServiceWorkerResponseErrorResponseTypeError);
        return;
    }
    if (m_requestMode != WebURLRequest::FetchRequestModeNoCORS && responseType == FetchResponseData::OpaqueType) {
        responseWasRejected(WebServiceWorkerResponseErrorResponseTypeOpaque);
        return;
    }
    if (m_frameType != WebURLRequest::FrameTypeNone && responseType != FetchResponseData::BasicType && responseType != FetchResponseData::DefaultType) {
        responseWasRejected(WebServiceWorkerResponseErrorResponseTypeNotBasicOrDefault);
        return;
    }
    if (response->bodyUsed()) {
        responseWasRejected(WebServiceWorkerResponseErrorBodyUsed);
        return;
    }

    response->lockBody(Body::PassBody);
    if (OwnPtr<DrainingBodyStreamBuffer> buffer = response->createInternalDrainingStream()) {
        WebServiceWorkerResponse webResponse;
        response->populateWebServiceWorkerResponse(webResponse);
        if (RefPtr<BlobDataHandle> blobDataHandle = buffer->drainAsBlobDataHandle(FetchDataConsumerHandle::Reader::AllowBlobWithInvalidSize)) {
            webResponse.setBlobDataHandle(blobDataHandle);
            ServiceWorkerGlobalScopeClient::from(executionContext())->didHandleFetchEvent(m_eventID, webResponse);
            m_state = Done;
            return;
        }
        Stream* outStream = Stream::create(executionContext(), "");
        webResponse.setStreamURL(outStream->url());
        ServiceWorkerGlobalScopeClient::from(executionContext())->didHandleFetchEvent(m_eventID, webResponse);
        FetchDataLoader* loader = FetchDataLoader::createLoaderAsStream(outStream);
        buffer->startLoading(executionContext(), loader, nullptr);
        m_state = Done;
        return;
    }
    WebServiceWorkerResponse webResponse;
    response->populateWebServiceWorkerResponse(webResponse);
    ServiceWorkerGlobalScopeClient::from(executionContext())->didHandleFetchEvent(m_eventID, webResponse);
    m_state = Done;
}

RespondWithObserver::RespondWithObserver(ExecutionContext* context, int eventID, WebURLRequest::FetchRequestMode requestMode, WebURLRequest::FrameType frameType)
    : ContextLifecycleObserver(context)
    , m_eventID(eventID)
    , m_requestMode(requestMode)
    , m_frameType(frameType)
    , m_state(Initial)
{
}

DEFINE_TRACE(RespondWithObserver)
{
    ContextLifecycleObserver::trace(visitor);
}

} // namespace blink
