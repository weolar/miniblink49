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

#ifndef ServiceWorkerGlobalScopeClientImpl_h
#define ServiceWorkerGlobalScopeClientImpl_h

#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"
#include "public/platform/WebServiceWorkerClientsInfo.h"
#include "public/platform/WebServiceWorkerSkipWaitingCallbacks.h"
#include "wtf/OwnPtr.h"

namespace blink {

class WebServiceWorkerContextClient;
class WebServiceWorkerResponse;
class WebURL;

class ServiceWorkerGlobalScopeClientImpl final : public NoBaseWillBeGarbageCollectedFinalized<ServiceWorkerGlobalScopeClientImpl>, public ServiceWorkerGlobalScopeClient {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ServiceWorkerGlobalScopeClientImpl);
public:
    static PassOwnPtrWillBeRawPtr<ServiceWorkerGlobalScopeClient> create(WebServiceWorkerContextClient&);
    ~ServiceWorkerGlobalScopeClientImpl() override;

    void getClients(const WebServiceWorkerClientQueryOptions&, WebServiceWorkerClientsCallbacks*) override;
    void openWindow(const WebURL&, WebServiceWorkerClientCallbacks*) override;
    void setCachedMetadata(const WebURL&, const char*, size_t) override;
    void clearCachedMetadata(const WebURL&) override;

    WebURL scope() const override;

    void didHandleActivateEvent(int eventID, WebServiceWorkerEventResult) override;
    void didHandleFetchEvent(int fetchEventID) override;
    void didHandleFetchEvent(int fetchEventID, const WebServiceWorkerResponse&) override;
    void didHandleInstallEvent(int installEventID, WebServiceWorkerEventResult) override;
    void didHandleNotificationClickEvent(int eventID, WebServiceWorkerEventResult) override;
    void didHandlePushEvent(int pushEventID, WebServiceWorkerEventResult) override;
    void didHandleSyncEvent(int syncEventID, WebServiceWorkerEventResult) override;
    void didHandleCrossOriginConnectEvent(int connectEventID, bool acceptConnect) override;
    void postMessageToClient(const WebString& clientUUID, const WebString& message, PassOwnPtr<WebMessagePortChannelArray>) override;
    void postMessageToCrossOriginClient(const WebCrossOriginServiceWorkerClient&, const WebString& message, PassOwnPtr<WebMessagePortChannelArray>) override;
    void skipWaiting(WebServiceWorkerSkipWaitingCallbacks*) override;
    void claim(WebServiceWorkerClientsClaimCallbacks*) override;
    void focus(const WebString& clientUUID, WebServiceWorkerClientCallbacks*) override;
    void stashMessagePort(WebMessagePortChannel*, const WebString& name) override;

    DEFINE_INLINE_VIRTUAL_TRACE() { ServiceWorkerGlobalScopeClient::trace(visitor); }

private:
    explicit ServiceWorkerGlobalScopeClientImpl(WebServiceWorkerContextClient&);

    WebServiceWorkerContextClient& m_client;
};

} // namespace blink

#endif // ServiceWorkerGlobalScopeClientImpl_h
