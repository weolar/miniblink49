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

#ifndef FetchContext_h
#define FetchContext_h

#include "core/CoreExport.h"
#include "core/fetch/CachePolicy.h"
#include "core/fetch/FetchInitiatorInfo.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/Resource.h"
#include "platform/heap/Handle.h"
#include "platform/network/ResourceLoadPriority.h"
#include "wtf/Noncopyable.h"

namespace blink {

class KURL;
class ResourceError;
class ResourceLoader;
class ResourceResponse;
class ResourceRequest;
class ResourceTimingInfo;

enum FetchResourceType {
    FetchMainResource,
    FetchSubresource
};

class CORE_EXPORT FetchContext : public GarbageCollectedFinalized<FetchContext> {
    WTF_MAKE_NONCOPYABLE(FetchContext);
public:
    static FetchContext& nullInstance();

    virtual ~FetchContext() { }
    DEFINE_INLINE_VIRTUAL_TRACE() { }

    virtual bool isLiveContext() { return false; }
    virtual void countClientHintsDPR() { }
    virtual void countClientHintsResourceWidth() { }
    virtual void countClientHintsViewportWidth() { }

    virtual void addAdditionalRequestHeaders(ResourceRequest&, FetchResourceType);
    virtual void setFirstPartyForCookies(ResourceRequest&);
    virtual CachePolicy cachePolicy() const;
    virtual ResourceRequestCachePolicy resourceRequestCachePolicy(const ResourceRequest&, Resource::Type) const;

    virtual void dispatchDidChangeResourcePriority(unsigned long identifier, ResourceLoadPriority, int intraPriorityValue);
    virtual void dispatchWillSendRequest(unsigned long identifier, ResourceRequest&, const ResourceResponse& redirectResponse, const FetchInitiatorInfo& = FetchInitiatorInfo());
    virtual void dispatchDidLoadResourceFromMemoryCache(const ResourceRequest&, const ResourceResponse&);
    virtual void dispatchDidReceiveResponse(unsigned long identifier, const ResourceResponse&, ResourceLoader* = 0);
    virtual void dispatchDidReceiveData(unsigned long identifier, const char* data, int dataLength, int encodedDataLength);
    virtual void dispatchDidDownloadData(unsigned long identifier, int dataLength, int encodedDataLength);
    virtual void dispatchDidFinishLoading(unsigned long identifier, double finishTime, int64_t encodedDataLength);
    virtual void dispatchDidFail(unsigned long identifier, const ResourceError&, bool isInternalRequest);
    virtual void sendRemainingDelegateMessages(unsigned long identifier, const ResourceResponse&, int dataLength);

    virtual bool shouldLoadNewResource(Resource::Type) const { return false; }
    virtual void dispatchWillRequestResource(FetchRequest*);
    virtual void willStartLoadingResource(ResourceRequest&);
    virtual void didLoadResource();

    virtual void addResourceTiming(const ResourceTimingInfo&);
    virtual bool allowImage(bool, const KURL&) const { return false; }
    virtual bool canRequest(Resource::Type, const ResourceRequest&, const KURL&, const ResourceLoaderOptions&, bool forPreload, FetchRequest::OriginRestriction) const { return false; }

    virtual bool isControlledByServiceWorker() const { return false; }
    virtual int64_t serviceWorkerID() const { return -1; }

    virtual bool isMainFrame() const { return true; }
    virtual bool hasSubstituteData() const { return false; }
    virtual bool defersLoading() const { return false; }
    virtual bool isLoadComplete() const { return false; }
    virtual bool pageDismissalEventBeingDispatched() const { return false; }
    virtual bool updateTimingInfoForIFrameNavigation(ResourceTimingInfo*) { return false; }
    virtual void sendImagePing(const KURL&);
    virtual void addConsoleMessage(const String&) const;
    virtual SecurityOrigin* securityOrigin() const { return nullptr; }
    virtual void upgradeInsecureRequest(FetchRequest&);
    virtual void addClientHintsIfNecessary(FetchRequest&);
    virtual void addCSPHeaderIfNecessary(Resource::Type, FetchRequest&);
    virtual bool isLowPriorityIframe() const { return false; }

protected:
    FetchContext() { }
};

}

#endif
