/*
 * Copyright (C) 2009, 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013, Intel Corporation
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

#ifndef DocumentThreadableLoader_h
#define DocumentThreadableLoader_h

#include "core/CoreExport.h"
#include "core/fetch/RawResource.h"
#include "core/fetch/ResourceOwner.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/loader/ThreadableLoader.h"
#include "platform/Timer.h"
#include "platform/network/HTTPHeaderMap.h"
#include "platform/network/ResourceError.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Document;
class KURL;
class ResourceRequest;
class SecurityOrigin;
class ThreadableLoaderClient;

class CORE_EXPORT DocumentThreadableLoader final : public ThreadableLoader, private ResourceOwner<RawResource>  {
    WTF_MAKE_FAST_ALLOCATED(DocumentThreadableLoader);
    public:
        static void loadResourceSynchronously(Document&, const ResourceRequest&, ThreadableLoaderClient&, const ThreadableLoaderOptions&, const ResourceLoaderOptions&);
        static PassRefPtr<DocumentThreadableLoader> create(Document&, ThreadableLoaderClient*, const ResourceRequest&, const ThreadableLoaderOptions&, const ResourceLoaderOptions&);
        ~DocumentThreadableLoader() override;

        void overrideTimeout(unsigned long timeout) override;

        void cancel() override;
        void setDefersLoading(bool);

    private:
        enum BlockingBehavior {
            LoadSynchronously,
            LoadAsynchronously
        };

        DocumentThreadableLoader(Document&, ThreadableLoaderClient*, BlockingBehavior, const ResourceRequest&, const ThreadableLoaderOptions&, const ResourceLoaderOptions&);

        // ResourceClient
        void notifyFinished(Resource*) override;
        // RawResourceClient
        void dataSent(Resource*, unsigned long long bytesSent, unsigned long long totalBytesToBeSent) override;
        void responseReceived(Resource*, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>) override;
        void setSerializedCachedMetadata(Resource*, const char*, size_t) override;
        void dataReceived(Resource*, const char* data, unsigned dataLength) override;
        void redirectReceived(Resource*, ResourceRequest&, const ResourceResponse&) override;
        void dataDownloaded(Resource*, int) override;
        void didReceiveResourceTiming(Resource*, const ResourceTimingInfo&) override;

        void cancelWithError(const ResourceError&);

        // Notify Inspector and log to console about resource response. Use
        // this method if response is not going to be finished normally.
        void reportResponseReceived(unsigned long identifier, const ResourceResponse&);

        // Methods containing code to handle resource fetch results which is
        // common to both sync and async mode.
        void handleResponse(unsigned long identifier, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>);
        void handleReceivedData(const char* data, unsigned dataLength);
        void handleSuccessfulFinish(unsigned long identifier, double finishTime);

        void didTimeout(Timer<DocumentThreadableLoader>*);
        // Calls the appropriate loading method according to policy and data
        // about origin. Only for handling the initial load (including fallback
        // after consulting ServiceWorker).
        void dispatchInitialRequest(const ResourceRequest&);
        void makeCrossOriginAccessRequest(const ResourceRequest&);
        // Loads m_fallbackRequestForServiceWorker.
        void loadFallbackRequestForServiceWorker();
        // Loads m_actualRequest.
        void loadActualRequest();
        // Clears m_actualRequest and reports access control check failure to
        // m_client.
        void handlePreflightFailure(const String& url, const String& errorDescription);
        // Investigates the response for the preflight request. If successful,
        // the actual request will be made later in handleSuccessfulFinish().
        void handlePreflightResponse(const ResourceResponse&);

        void loadRequest(const ResourceRequest&, ResourceLoaderOptions);
        bool isAllowedRedirect(const KURL&) const;
        bool isAllowedByContentSecurityPolicy(const KURL&, ContentSecurityPolicy::RedirectStatus) const;
        // Returns DoNotAllowStoredCredentials
        // if m_forceDoNotAllowStoredCredentials is set. Otherwise, just
        // returns allowCredentials value of m_resourceLoaderOptions.
        StoredCredentials effectiveAllowCredentials() const;

        SecurityOrigin* securityOrigin() const;

        ThreadableLoaderClient* m_client;
        Document& m_document;

        const ThreadableLoaderOptions m_options;
        // Some items may be overridden by m_forceDoNotAllowStoredCredentials
        // and m_securityOrigin. In such a case, build a ResourceLoaderOptions
        // with up-to-date values from them and this variable, and use it.
        const ResourceLoaderOptions m_resourceLoaderOptions;

        bool m_forceDoNotAllowStoredCredentials;
        RefPtr<SecurityOrigin> m_securityOrigin;

        // True while the initial URL and all the URLs of the redirects
        // this object has followed, if any, are same-origin to
        // securityOrigin().
        bool m_sameOriginRequest;
        // Set to true if the current request is cross-origin and not simple.
        bool m_crossOriginNonSimpleRequest;

        // Set to true when the response data is given to a data consumer
        // handle.
        bool m_isUsingDataConsumerHandle;

        const bool m_async;

        // Holds the original request context (used for sanity checks).
        const WebURLRequest::RequestContext m_requestContext;

        // Holds the original request for fallback in case the Service Worker
        // does not respond.
        OwnPtr<ResourceRequest> m_fallbackRequestForServiceWorker;

        // Holds the original request and options for it during preflight
        // request handling phase.
        OwnPtr<ResourceRequest> m_actualRequest;
        OwnPtr<ResourceLoaderOptions> m_actualOptions;

        HTTPHeaderMap m_simpleRequestHeaders; // stores simple request headers in case of a cross-origin redirect.
        Timer<DocumentThreadableLoader> m_timeoutTimer;
        double m_requestStartedSeconds; // Time an asynchronous fetch request is started

        // Max number of times that this DocumentThreadableLoader can follow
        // cross-origin redirects.
        // This is used to limit the number of redirects.
        // But this value is not the max number of total redirects allowed,
        // because same-origin redirects are not counted here.
        int m_corsRedirectLimit;
    };

} // namespace blink

#endif // DocumentThreadableLoader_h
