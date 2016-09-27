/*
 * Copyright (C) 2005, 2006, 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ResourceLoader_h
#define ResourceLoader_h

#include "core/CoreExport.h"
#include "core/fetch/ResourceLoaderOptions.h"
#include "platform/network/ResourceRequest.h"
#include "public/platform/WebURLLoader.h"
#include "public/platform/WebURLLoaderClient.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"

namespace blink {

class Resource;
class KURL;
class ResourceError;
class ResourceFetcher;
class ThreadedDataReceiver;

class CORE_EXPORT ResourceLoader final : public GarbageCollectedFinalized<ResourceLoader>, protected WebURLLoaderClient {
public:
    static ResourceLoader* create(ResourceFetcher*, Resource*, const ResourceRequest&, const ResourceLoaderOptions&);
    ~ResourceLoader() override;
    DECLARE_TRACE();

    void start();
    void changeToSynchronous();

    void cancel();
    void cancel(const ResourceError&);
    void cancelIfNotFinishing();

    Resource* cachedResource() { return m_resource; }
    const ResourceRequest& originalRequest() const { return m_originalRequest; }

    void setDefersLoading(bool);
    bool defersLoading() const { return m_defersLoading; }

    void attachThreadedDataReceiver(PassRefPtrWillBeRawPtr<ThreadedDataReceiver>);

    void releaseResources();

    void didChangePriority(ResourceLoadPriority, int intraPriorityValue);

    // WebURLLoaderClient
    void willSendRequest(WebURLLoader*, WebURLRequest&, const WebURLResponse& redirectResponse) override;
    void didSendData(WebURLLoader*, unsigned long long bytesSent, unsigned long long totalBytesToBeSent) override;
    void didReceiveResponse(WebURLLoader*, const WebURLResponse&) override;
    void didReceiveResponse(WebURLLoader*, const WebURLResponse&, WebDataConsumerHandle*) override;
    void didReceiveData(WebURLLoader*, const char*, int, int encodedDataLength) override;
    void didReceiveCachedMetadata(WebURLLoader*, const char* data, int length) override;
    void didFinishLoading(WebURLLoader*, double finishTime, int64_t encodedDataLength) override;
    void didFail(WebURLLoader*, const WebURLError&) override;
    void didDownloadData(WebURLLoader*, int, int) override;

    const KURL& url() const { return m_request.url(); }
    bool isLoadedBy(ResourceFetcher*) const;

    bool reachedTerminalState() const { return m_state == Terminated; }
    const ResourceRequest& request() const { return m_request; }

    bool loadingMultipartContent() const { return m_loadingMultipartContent; }

private:
    ResourceLoader(ResourceFetcher*, Resource*, const ResourceLoaderOptions&);

    void init(const ResourceRequest&);
    void requestSynchronously();

    void didFinishLoadingOnePart(double finishTime, int64_t encodedDataLength);

    bool responseNeedsAccessControlCheck() const;

    ResourceRequest& applyOptions(ResourceRequest&) const;

    OwnPtr<WebURLLoader> m_loader;
    Member<ResourceFetcher> m_fetcher;

    ResourceRequest m_request;
    ResourceRequest m_originalRequest; // Before redirects.

    bool m_notifiedLoadComplete;

    bool m_defersLoading;
    bool m_loadingMultipartContent;
    OwnPtr<ResourceRequest> m_fallbackRequestForServiceWorker;
    ResourceRequest m_deferredRequest;
    ResourceLoaderOptions m_options;

    enum ResourceLoaderState {
        Initialized,
        Finishing,
        Terminated
    };

    enum ConnectionState {
        ConnectionStateNew,
        ConnectionStateStarted,
        ConnectionStateReceivedResponse,
        ConnectionStateReceivingData,
        ConnectionStateFinishedLoading,
        ConnectionStateCanceled,
        ConnectionStateFailed,
    };

    RawPtrWillBeMember<Resource> m_resource;
    ResourceLoaderState m_state;

    // Used for sanity checking to make sure we don't experience illegal state
    // transitions.
    ConnectionState m_connectionState;

    Vector<char> m_dump;
};

}

#endif
