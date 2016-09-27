/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef FrameFetchContext_h
#define FrameFetchContext_h

#include "core/CoreExport.h"
#include "core/fetch/FetchContext.h"
#include "core/fetch/ResourceFetcher.h"
#include "platform/heap/Handle.h"
#include "platform/network/ResourceRequest.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class Document;
class DocumentLoader;
class LocalFrame;
class ResourceError;
class ResourceLoader;
class ResourceResponse;
class ResourceRequest;

class CORE_EXPORT FrameFetchContext final : public FetchContext {
public:
    static ResourceFetcher* createContextAndFetcher(DocumentLoader* loader)
    {
        return ResourceFetcher::create(new FrameFetchContext(loader));
    }

    static void provideDocumentToContext(FetchContext& context, Document* document)
    {
        ASSERT(document);
        RELEASE_ASSERT(context.isLiveContext());
        static_cast<FrameFetchContext&>(context).m_document = document;
    }

    ~FrameFetchContext();

    bool isLiveContext() { return true; }

    void addAdditionalRequestHeaders(ResourceRequest&, FetchResourceType) override;
    void setFirstPartyForCookies(ResourceRequest&) override;
    CachePolicy cachePolicy() const override;
    ResourceRequestCachePolicy resourceRequestCachePolicy(const ResourceRequest&, Resource::Type) const override;
    void dispatchDidChangeResourcePriority(unsigned long identifier, ResourceLoadPriority, int intraPriorityValue) override;
    void dispatchWillSendRequest(unsigned long identifier, ResourceRequest&, const ResourceResponse& redirectResponse, const FetchInitiatorInfo& = FetchInitiatorInfo()) override;
    void dispatchDidLoadResourceFromMemoryCache(const ResourceRequest&, const ResourceResponse&) override;
    void dispatchDidReceiveResponse(unsigned long identifier, const ResourceResponse&, ResourceLoader* = 0) override;
    void dispatchDidReceiveData(unsigned long identifier, const char* data, int dataLength, int encodedDataLength) override;
    void dispatchDidDownloadData(unsigned long identifier, int dataLength, int encodedDataLength)  override;
    void dispatchDidFinishLoading(unsigned long identifier, double finishTime, int64_t encodedDataLength) override;
    void dispatchDidFail(unsigned long identifier, const ResourceError&, bool isInternalRequest) override;
    void sendRemainingDelegateMessages(unsigned long identifier, const ResourceResponse&, int dataLength) override;

    bool shouldLoadNewResource(Resource::Type) const override;
    void dispatchWillRequestResource(FetchRequest*) override;
    void willStartLoadingResource(ResourceRequest&) override;
    void didLoadResource() override;

    void addResourceTiming(const ResourceTimingInfo&) override;
    bool allowImage(bool imagesEnabled, const KURL&) const override;
    bool canRequest(Resource::Type, const ResourceRequest&, const KURL&, const ResourceLoaderOptions&, bool forPreload, FetchRequest::OriginRestriction) const override;

    bool isControlledByServiceWorker() const override;
    int64_t serviceWorkerID() const override;

    bool isMainFrame() const override;
    bool hasSubstituteData() const override;
    bool defersLoading() const override;
    bool isLoadComplete() const override;
    bool pageDismissalEventBeingDispatched() const override;
    bool updateTimingInfoForIFrameNavigation(ResourceTimingInfo*) override;
    void sendImagePing(const KURL&) override;
    void addConsoleMessage(const String&) const override;
    SecurityOrigin* securityOrigin() const override;
    void upgradeInsecureRequest(FetchRequest&) override;
    void addClientHintsIfNecessary(FetchRequest&) override;
    void addCSPHeaderIfNecessary(Resource::Type, FetchRequest&) override;
    bool isLowPriorityIframe() const override;

    void countClientHintsDPR() override;
    void countClientHintsResourceWidth() override;
    void countClientHintsViewportWidth() override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit FrameFetchContext(DocumentLoader*);
    inline DocumentLoader* ensureLoaderForNotifications();

    LocalFrame* frame() const; // Can be null
    void printAccessDeniedMessage(const KURL&) const;

    // FIXME: Oilpan: Ideally this should just be a traced Member but that will
    // currently leak because ComputedStyle and its data are not on the heap.
    // See crbug.com/383860 for details.
    RawPtrWillBeWeakMember<Document> m_document;
    RawPtrWillBeMember<DocumentLoader> m_documentLoader;
};

}

#endif
