// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/ServiceWorkerWindowClient.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "core/page/PageVisibilityState.h"
#include "modules/serviceworkers/ServiceWorkerError.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"
#include "public/platform/WebString.h"
#include "wtf/RefPtr.h"

namespace blink {

ServiceWorkerWindowClient* ServiceWorkerWindowClient::take(ScriptPromiseResolver*, PassOwnPtr<ServiceWorkerWindowClient::WebType> webClient)
{
    return ServiceWorkerWindowClient::create(*webClient);
}

ServiceWorkerWindowClient* ServiceWorkerWindowClient::create(const WebServiceWorkerClientInfo& info)
{
    return new ServiceWorkerWindowClient(info);
}

ServiceWorkerWindowClient::ServiceWorkerWindowClient(const WebServiceWorkerClientInfo& info)
    : ServiceWorkerClient(info)
    , m_pageVisibilityState(info.pageVisibilityState)
    , m_isFocused(info.isFocused)
{
}

ServiceWorkerWindowClient::~ServiceWorkerWindowClient()
{
}

String ServiceWorkerWindowClient::visibilityState() const
{
    return pageVisibilityStateString(static_cast<PageVisibilityState>(m_pageVisibilityState));
}

ScriptPromise ServiceWorkerWindowClient::focus(ScriptState* scriptState)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    if (!scriptState->executionContext()->isWindowInteractionAllowed()) {
        resolver->reject(DOMException::create(InvalidAccessError, "Not allowed to focus a window."));
        return promise;
    }
    scriptState->executionContext()->consumeWindowInteraction();

    ServiceWorkerGlobalScopeClient::from(scriptState->executionContext())->focus(uuid(), new CallbackPromiseAdapter<ServiceWorkerWindowClient, ServiceWorkerError>(resolver));
    return promise;
}

DEFINE_TRACE(ServiceWorkerWindowClient)
{
    ServiceWorkerClient::trace(visitor);
}

} // namespace blink
