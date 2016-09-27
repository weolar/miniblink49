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

#ifndef ServiceWorkerGlobalScopeClient_h
#define ServiceWorkerGlobalScopeClient_h

#include "core/dom/MessagePort.h"
#include "core/workers/WorkerClients.h"
#include "modules/ModulesExport.h"
#include "public/platform/WebMessagePortChannel.h"
#include "public/platform/WebServiceWorkerClientsClaimCallbacks.h"
#include "public/platform/WebServiceWorkerClientsInfo.h"
#include "public/platform/WebServiceWorkerEventResult.h"
#include "public/platform/WebServiceWorkerSkipWaitingCallbacks.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"

namespace blink {

struct WebCrossOriginServiceWorkerClient;
struct WebServiceWorkerClientQueryOptions;
class ExecutionContext;
class WebServiceWorkerCacheStorage;
class WebServiceWorkerResponse;
class WebURL;
class WorkerClients;

// See WebServiceWorkerContextClient for documentation for the methods in this class.
class MODULES_EXPORT ServiceWorkerGlobalScopeClient : public WillBeHeapSupplement<WorkerClients> {
    WTF_MAKE_NONCOPYABLE(ServiceWorkerGlobalScopeClient);
public:
    virtual ~ServiceWorkerGlobalScopeClient() { }

    // Called from ServiceWorkerClients.
    virtual void getClients(const WebServiceWorkerClientQueryOptions&, WebServiceWorkerClientsCallbacks*) = 0;
    virtual void openWindow(const WebURL&, WebServiceWorkerClientCallbacks*) = 0;
    virtual void setCachedMetadata(const WebURL&, const char*, size_t) = 0;
    virtual void clearCachedMetadata(const WebURL&) = 0;

    virtual WebURL scope() const = 0;

    virtual void didHandleActivateEvent(int eventID, WebServiceWorkerEventResult) = 0;
    // Calling didHandleFetchEvent without response means no response was
    // provided by the service worker in the fetch events, so fallback to native.
    virtual void didHandleFetchEvent(int fetchEventID) = 0;
    virtual void didHandleFetchEvent(int fetchEventID, const WebServiceWorkerResponse&) = 0;
    virtual void didHandleInstallEvent(int installEventID, WebServiceWorkerEventResult) = 0;
    virtual void didHandleNotificationClickEvent(int eventID, WebServiceWorkerEventResult) = 0;
    virtual void didHandlePushEvent(int pushEventID, WebServiceWorkerEventResult) = 0;
    virtual void didHandleSyncEvent(int syncEventID, WebServiceWorkerEventResult) = 0;
    virtual void didHandleCrossOriginConnectEvent(int connectEventID, bool acceptConnect) = 0;
    virtual void postMessageToClient(const WebString& clientUUID, const WebString& message, PassOwnPtr<WebMessagePortChannelArray>) = 0;
    virtual void postMessageToCrossOriginClient(const WebCrossOriginServiceWorkerClient&, const WebString& message, PassOwnPtr<WebMessagePortChannelArray>) = 0;
    virtual void skipWaiting(WebServiceWorkerSkipWaitingCallbacks*) = 0;
    virtual void claim(WebServiceWorkerClientsClaimCallbacks*) = 0;
    virtual void focus(const WebString& clientUUID, WebServiceWorkerClientCallbacks*) = 0;
    virtual void stashMessagePort(WebMessagePortChannel*, const WebString& name) = 0;

    static const char* supplementName();
    static ServiceWorkerGlobalScopeClient* from(ExecutionContext*);

protected:
    ServiceWorkerGlobalScopeClient() { }
};

MODULES_EXPORT void provideServiceWorkerGlobalScopeClientToWorker(WorkerClients*, PassOwnPtrWillBeRawPtr<ServiceWorkerGlobalScopeClient>);

} // namespace blink

#endif // ServiceWorkerGlobalScopeClient_h
