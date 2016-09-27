// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/ServiceWorkerClients.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerLocation.h"
#include "modules/serviceworkers/ServiceWorkerError.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"
#include "modules/serviceworkers/ServiceWorkerWindowClient.h"
#include "public/platform/WebServiceWorkerClientQueryOptions.h"
#include "public/platform/WebServiceWorkerClientsInfo.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

namespace {

class ClientArray {
public:
    typedef WebServiceWorkerClientsInfo WebType;
    static HeapVector<Member<ServiceWorkerClient>> take(ScriptPromiseResolver*, PassOwnPtr<WebType> webClients)
    {
        HeapVector<Member<ServiceWorkerClient>> clients;
        for (size_t i = 0; i < webClients->clients.size(); ++i) {
            const WebServiceWorkerClientInfo& client = webClients->clients[i];
            if (client.clientType == WebServiceWorkerClientTypeWindow)
                clients.append(ServiceWorkerWindowClient::create(client));
            else
                clients.append(ServiceWorkerClient::create(client));
        }
        return clients;
    }

private:
    WTF_MAKE_NONCOPYABLE(ClientArray);
    ClientArray() = delete;
};

WebServiceWorkerClientType getClientType(const String& type)
{
    if (type == "window")
        return WebServiceWorkerClientTypeWindow;
    if (type == "worker")
        return WebServiceWorkerClientTypeWorker;
    if (type == "sharedworker")
        return WebServiceWorkerClientTypeSharedWorker;
    if (type == "all")
        return WebServiceWorkerClientTypeAll;
    ASSERT_NOT_REACHED();
    return WebServiceWorkerClientTypeWindow;
}

} // namespace

ServiceWorkerClients* ServiceWorkerClients::create()
{
    return new ServiceWorkerClients();
}

ServiceWorkerClients::ServiceWorkerClients()
{
}

ScriptPromise ServiceWorkerClients::matchAll(ScriptState* scriptState, const ClientQueryOptions& options)
{
    ExecutionContext* executionContext = scriptState->executionContext();
    // FIXME: May be null due to worker termination: http://crbug.com/413518.
    if (!executionContext)
        return ScriptPromise();

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    WebServiceWorkerClientQueryOptions webOptions;
    webOptions.clientType = getClientType(options.type());
    webOptions.includeUncontrolled = options.includeUncontrolled();
    ServiceWorkerGlobalScopeClient::from(executionContext)->getClients(webOptions, new CallbackPromiseAdapter<ClientArray, ServiceWorkerError>(resolver));
    return promise;
}

ScriptPromise ServiceWorkerClients::claim(ScriptState* scriptState)
{
    ExecutionContext* executionContext = scriptState->executionContext();

    // FIXME: May be null due to worker termination: http://crbug.com/413518.
    if (!executionContext)
        return ScriptPromise();

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    WebServiceWorkerClientsClaimCallbacks* callbacks = new CallbackPromiseAdapter<void, ServiceWorkerError>(resolver);
    ServiceWorkerGlobalScopeClient::from(executionContext)->claim(callbacks);
    return promise;
}

ScriptPromise ServiceWorkerClients::openWindow(ScriptState* scriptState, const String& url)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    ExecutionContext* context = scriptState->executionContext();

    KURL parsedUrl = KURL(toWorkerGlobalScope(context)->location()->url(), url);
    if (!parsedUrl.isValid()) {
        resolver->reject(V8ThrowException::createTypeError(scriptState->isolate(), "'" + url + "' is not a valid URL."));
        return promise;
    }

    if (!context->securityOrigin()->canDisplay(parsedUrl)) {
        resolver->reject(DOMException::create(SecurityError, "'" + parsedUrl.elidedString() + "' cannot be opened."));
        return promise;
    }

    if (!context->isWindowInteractionAllowed()) {
        resolver->reject(DOMException::create(InvalidAccessError, "Not allowed to open a window."));
        return promise;
    }
    context->consumeWindowInteraction();

    ServiceWorkerGlobalScopeClient::from(context)->openWindow(parsedUrl, new CallbackPromiseAdapter<ServiceWorkerWindowClient, ServiceWorkerError>(resolver));
    return promise;
}

} // namespace blink
