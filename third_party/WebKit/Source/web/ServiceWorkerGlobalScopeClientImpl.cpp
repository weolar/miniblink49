/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "web/ServiceWorkerGlobalScopeClientImpl.h"

#include "modules/fetch/Response.h"
#include "public/platform/WebServiceWorkerResponse.h"
#include "public/platform/WebURL.h"
#include "public/web/WebServiceWorkerContextClient.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

PassOwnPtrWillBeRawPtr<ServiceWorkerGlobalScopeClient> ServiceWorkerGlobalScopeClientImpl::create(WebServiceWorkerContextClient& client)
{
    return adoptPtrWillBeNoop(new ServiceWorkerGlobalScopeClientImpl(client));
}

ServiceWorkerGlobalScopeClientImpl::~ServiceWorkerGlobalScopeClientImpl()
{
}

void ServiceWorkerGlobalScopeClientImpl::getClients(const WebServiceWorkerClientQueryOptions& options, WebServiceWorkerClientsCallbacks* callbacks)
{
    m_client.getClients(options, callbacks);
}

void ServiceWorkerGlobalScopeClientImpl::openWindow(const WebURL& url, WebServiceWorkerClientCallbacks* callbacks)
{
    m_client.openWindow(url, callbacks);
}

void ServiceWorkerGlobalScopeClientImpl::setCachedMetadata(const WebURL& url, const char* data, size_t size)
{
    m_client.setCachedMetadata(url, data, size);
}

void ServiceWorkerGlobalScopeClientImpl::clearCachedMetadata(const WebURL& url)
{
    m_client.clearCachedMetadata(url);
}

WebURL ServiceWorkerGlobalScopeClientImpl::scope() const
{
    return m_client.scope();
}

void ServiceWorkerGlobalScopeClientImpl::didHandleActivateEvent(int eventID, WebServiceWorkerEventResult result)
{
    m_client.didHandleActivateEvent(eventID, result);
}

void ServiceWorkerGlobalScopeClientImpl::didHandleFetchEvent(int fetchEventID)
{
    m_client.didHandleFetchEvent(fetchEventID);
}

void ServiceWorkerGlobalScopeClientImpl::didHandleFetchEvent(int fetchEventID, const WebServiceWorkerResponse& webResponse)
{
    m_client.didHandleFetchEvent(fetchEventID, webResponse);
}

void ServiceWorkerGlobalScopeClientImpl::didHandleInstallEvent(int installEventID, WebServiceWorkerEventResult result)
{
    m_client.didHandleInstallEvent(installEventID, result);
}

void ServiceWorkerGlobalScopeClientImpl::didHandleNotificationClickEvent(int eventID, WebServiceWorkerEventResult result)
{
    m_client.didHandleNotificationClickEvent(eventID, result);
}

void ServiceWorkerGlobalScopeClientImpl::didHandlePushEvent(int pushEventID, WebServiceWorkerEventResult result)
{
    m_client.didHandlePushEvent(pushEventID, result);
}

void ServiceWorkerGlobalScopeClientImpl::didHandleSyncEvent(int syncEventID, WebServiceWorkerEventResult result)
{
    m_client.didHandleSyncEvent(syncEventID, result);
}

void ServiceWorkerGlobalScopeClientImpl::didHandleCrossOriginConnectEvent(int connectEventID, bool acceptConnect)
{
    m_client.didHandleCrossOriginConnectEvent(connectEventID, acceptConnect);
}

void ServiceWorkerGlobalScopeClientImpl::postMessageToClient(const WebString& clientUUID, const WebString& message, PassOwnPtr<WebMessagePortChannelArray> webChannels)
{
    m_client.postMessageToClient(clientUUID, message, webChannels.leakPtr());
}

void ServiceWorkerGlobalScopeClientImpl::postMessageToCrossOriginClient(const WebCrossOriginServiceWorkerClient& client, const WebString& message, PassOwnPtr<WebMessagePortChannelArray> webChannels)
{
    m_client.postMessageToCrossOriginClient(client, message, webChannels.leakPtr());
}

void ServiceWorkerGlobalScopeClientImpl::skipWaiting(WebServiceWorkerSkipWaitingCallbacks* callbacks)
{
    m_client.skipWaiting(callbacks);
}

void ServiceWorkerGlobalScopeClientImpl::claim(WebServiceWorkerClientsClaimCallbacks* callbacks)
{
    m_client.claim(callbacks);
}

void ServiceWorkerGlobalScopeClientImpl::focus(const WebString& clientUUID, WebServiceWorkerClientCallbacks* callback)
{
    m_client.focus(clientUUID, callback);
}

void ServiceWorkerGlobalScopeClientImpl::stashMessagePort(WebMessagePortChannel* channel, const WebString& name)
{
    m_client.stashMessagePort(channel, name);
}

ServiceWorkerGlobalScopeClientImpl::ServiceWorkerGlobalScopeClientImpl(WebServiceWorkerContextClient& client)
    : m_client(client)
{
}

} // namespace blink
