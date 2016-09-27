/*
 * Copyright (C) 2011 Google Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/fetch/RawResource.h"

#include "core/fetch/FetchRequest.h"
#include "core/fetch/MemoryCache.h"
#include "core/fetch/ResourceClientWalker.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/fetch/ResourceLoader.h"
#include "core/fetch/SubstituteData.h"
#include "platform/SharedBuffer.h"

namespace blink {

void RawResource::preCacheSubstituteDataForMainResource(const FetchRequest& request, ResourceFetcher* fetcher, const SubstituteData& substituteData)
{
    const String cacheIdentifier = fetcher->getCacheIdentifier();
    const KURL& url = request.url();
    if (Resource* oldResource = memoryCache()->resourceForURL(url, cacheIdentifier))
        memoryCache()->remove(oldResource);

    ResourceResponse response(url, substituteData.mimeType(), substituteData.content()->size(), substituteData.textEncoding(), emptyString());
    ResourcePtr<Resource> resource = new RawResource(request.resourceRequest(), Resource::MainResource);
    resource->setNeedsSynchronousCacheHit(substituteData.forceSynchronousLoad());
    resource->setOptions(request.options());
    resource->setDataBufferingPolicy(BufferData);
    resource->responseReceived(response, nullptr);
    if (substituteData.content()->size())
        resource->setResourceBuffer(substituteData.content());
    resource->setCacheIdentifier(cacheIdentifier);
    resource->finish();
    memoryCache()->add(resource.get());
}

ResourcePtr<Resource> RawResource::fetchSynchronously(FetchRequest& request, ResourceFetcher* fetcher)
{
    request.mutableResourceRequest().setTimeoutInterval(10);
    ResourceLoaderOptions options(request.options());
    options.synchronousPolicy = RequestSynchronously;
    request.setOptions(options);
    return fetcher->requestResource(request, RawResourceFactory(Resource::Raw));
}

ResourcePtr<RawResource> RawResource::fetchImport(FetchRequest& request, ResourceFetcher* fetcher)
{
    ASSERT(request.resourceRequest().frameType() == WebURLRequest::FrameTypeNone);
    request.mutableResourceRequest().setRequestContext(WebURLRequest::RequestContextImport);
    RawResourceFactory factory(Resource::ImportResource);
    return toRawResource(fetcher->requestResource(request, RawResourceFactory(Resource::ImportResource)));
}

ResourcePtr<RawResource> RawResource::fetch(FetchRequest& request, ResourceFetcher* fetcher)
{
    ASSERT(request.resourceRequest().frameType() == WebURLRequest::FrameTypeNone);
    ASSERT(request.resourceRequest().requestContext() != WebURLRequest::RequestContextUnspecified);
    return toRawResource(fetcher->requestResource(request, RawResourceFactory(Resource::Raw)));
}

ResourcePtr<RawResource> RawResource::fetchMainResource(FetchRequest& request, ResourceFetcher* fetcher, const SubstituteData& substituteData)
{
    ASSERT(request.resourceRequest().frameType() != WebURLRequest::FrameTypeNone);
    ASSERT(request.resourceRequest().requestContext() == WebURLRequest::RequestContextForm || request.resourceRequest().requestContext() == WebURLRequest::RequestContextFrame || request.resourceRequest().requestContext() == WebURLRequest::RequestContextHyperlink || request.resourceRequest().requestContext() == WebURLRequest::RequestContextIframe || request.resourceRequest().requestContext() == WebURLRequest::RequestContextInternal || request.resourceRequest().requestContext() == WebURLRequest::RequestContextLocation);

    if (substituteData.isValid())
        preCacheSubstituteDataForMainResource(request, fetcher, substituteData);
    return toRawResource(fetcher->requestResource(request, RawResourceFactory(Resource::MainResource)));
}

ResourcePtr<RawResource> RawResource::fetchMedia(FetchRequest& request, ResourceFetcher* fetcher)
{
    ASSERT(request.resourceRequest().frameType() == WebURLRequest::FrameTypeNone);
    ASSERT(request.resourceRequest().requestContext() == WebURLRequest::RequestContextAudio || request.resourceRequest().requestContext() == WebURLRequest::RequestContextVideo);
    return toRawResource(fetcher->requestResource(request, RawResourceFactory(Resource::Media)));
}

ResourcePtr<RawResource> RawResource::fetchTextTrack(FetchRequest& request, ResourceFetcher* fetcher)
{
    ASSERT(request.resourceRequest().frameType() == WebURLRequest::FrameTypeNone);
    request.mutableResourceRequest().setRequestContext(WebURLRequest::RequestContextTrack);
    return toRawResource(fetcher->requestResource(request, RawResourceFactory(Resource::TextTrack)));
}

RawResource::RawResource(const ResourceRequest& resourceRequest, Type type)
    : Resource(resourceRequest, type)
{
}

void RawResource::appendData(const char* data, unsigned length)
{
    Resource::appendData(data, length);

    ResourcePtr<RawResource> protect(this);
    ResourceClientWalker<RawResourceClient> w(m_clients);
    while (RawResourceClient* c = w.next())
        c->dataReceived(this, data, length);
}

void RawResource::didAddClient(ResourceClient* c)
{
    if (!hasClient(c))
        return;
    // The calls to the client can result in events running, potentially causing
    // this resource to be evicted from the cache and all clients to be removed,
    // so a protector is necessary.
    ResourcePtr<RawResource> protect(this);
    ASSERT(c->resourceClientType() == RawResourceClient::expectedType());
    RawResourceClient* client = static_cast<RawResourceClient*>(c);
    for (const auto& redirect : redirectChain()) {
        ResourceRequest request(redirect.m_request);
        client->redirectReceived(this, request, redirect.m_redirectResponse);
        if (!hasClient(c))
            return;
    }

    if (!m_response.isNull())
        client->responseReceived(this, m_response, nullptr);
    if (!hasClient(c))
        return;
    if (m_data)
        client->dataReceived(this, m_data->data(), m_data->size());
    if (!hasClient(c))
        return;
    Resource::didAddClient(client);
}

void RawResource::willFollowRedirect(ResourceRequest& newRequest, const ResourceResponse& redirectResponse)
{
    ResourcePtr<RawResource> protect(this);
    if (!redirectResponse.isNull()) {
        ResourceClientWalker<RawResourceClient> w(m_clients);
        while (RawResourceClient* c = w.next())
            c->redirectReceived(this, newRequest, redirectResponse);
    }
    Resource::willFollowRedirect(newRequest, redirectResponse);
}

void RawResource::updateRequest(const ResourceRequest& request)
{
    ResourcePtr<RawResource> protect(this);
    ResourceClientWalker<RawResourceClient> w(m_clients);
    while (RawResourceClient* c = w.next())
        c->updateRequest(this, request);
}

void RawResource::responseReceived(const ResourceResponse& response, PassOwnPtr<WebDataConsumerHandle> handle)
{
    InternalResourcePtr protect(this);
    Resource::responseReceived(response, nullptr);
    ResourceClientWalker<RawResourceClient> w(m_clients);
    ASSERT(count() <= 1 || !handle);
    while (RawResourceClient* c = w.next()) {
        // |handle| is cleared when passed, but it's not a problem because
        // |handle| is null when there are two or more clients, as asserted.
        c->responseReceived(this, m_response, handle);
    }
}

void RawResource::setSerializedCachedMetadata(const char* data, size_t size)
{
    ResourcePtr<RawResource> protect(this);
    Resource::setSerializedCachedMetadata(data, size);
    ResourceClientWalker<RawResourceClient> w(m_clients);
    while (RawResourceClient* c = w.next())
        c->setSerializedCachedMetadata(this, data, size);
}

void RawResource::didSendData(unsigned long long bytesSent, unsigned long long totalBytesToBeSent)
{
    ResourceClientWalker<RawResourceClient> w(m_clients);
    while (RawResourceClient* c = w.next())
        c->dataSent(this, bytesSent, totalBytesToBeSent);
}

void RawResource::didDownloadData(int dataLength)
{
    ResourceClientWalker<RawResourceClient> w(m_clients);
    while (RawResourceClient* c = w.next())
        c->dataDownloaded(this, dataLength);
}

void RawResource::reportResourceTimingToClients(const ResourceTimingInfo& info)
{
    ResourceClientWalker<RawResourceClient> w(m_clients);
    while (RawResourceClient* c = w.next())
        c->didReceiveResourceTiming(this, info);
}

void RawResource::setDefersLoading(bool defers)
{
    if (m_loader)
        m_loader->setDefersLoading(defers);
}

static bool shouldIgnoreHeaderForCacheReuse(AtomicString headerName)
{
    // FIXME: This list of headers that don't affect cache policy almost certainly isn't complete.
    DEFINE_STATIC_LOCAL(HashSet<AtomicString>, m_headers, ());
    if (m_headers.isEmpty()) {
        m_headers.add("Cache-Control");
        m_headers.add("If-Modified-Since");
        m_headers.add("If-None-Match");
        m_headers.add("Origin");
        m_headers.add("Pragma");
        m_headers.add("Purpose");
        m_headers.add("Referer");
        m_headers.add("User-Agent");
    }
    return m_headers.contains(headerName);
}

static bool isCacheableHTTPMethod(const AtomicString& method)
{
    // Per http://www.w3.org/Protocols/rfc2616/rfc2616-sec13.html#sec13.10,
    // these methods always invalidate the cache entry.
    return method != "POST" && method != "PUT" && method != "DELETE";
}

bool RawResource::canReuse(const ResourceRequest& newRequest) const
{
    if (m_options.dataBufferingPolicy == DoNotBufferData)
        return false;

    if (!isCacheableHTTPMethod(m_resourceRequest.httpMethod()))
        return false;
    if (m_resourceRequest.httpMethod() != newRequest.httpMethod())
        return false;

    if (m_resourceRequest.httpBody() != newRequest.httpBody())
        return false;

    if (m_resourceRequest.allowStoredCredentials() != newRequest.allowStoredCredentials())
        return false;

    // Ensure most headers match the existing headers before continuing.
    // Note that the list of ignored headers includes some headers explicitly related to caching.
    // A more detailed check of caching policy will be performed later, this is simply a list of
    // headers that we might permit to be different and still reuse the existing Resource.
    const HTTPHeaderMap& newHeaders = newRequest.httpHeaderFields();
    const HTTPHeaderMap& oldHeaders = m_resourceRequest.httpHeaderFields();

    for (const auto& header : newHeaders) {
        AtomicString headerName = header.key;
        if (!shouldIgnoreHeaderForCacheReuse(headerName) && header.value != oldHeaders.get(headerName))
            return false;
    }

    for (const auto& header : oldHeaders) {
        AtomicString headerName = header.key;
        if (!shouldIgnoreHeaderForCacheReuse(headerName) && header.value != newHeaders.get(headerName))
            return false;
    }

    return true;
}

} // namespace blink
