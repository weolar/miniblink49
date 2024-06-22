/*
 * Copyright (C) 2006, 2007, 2010, 2011 Apple Inc. All rights reserved.
 *           (C) 2007 Graham Dennis (graham.dennis@gmail.com)
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

#include "config.h"
#include "core/fetch/ResourceLoader.h"

#include "core/fetch/CSSStyleSheetResource.h"
#include "core/fetch/Resource.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/fetch/ResourcePtr.h"
#include "platform/Logging.h"
#include "platform/SharedBuffer.h"
#include "platform/ThreadedDataReceiver.h"
#include "platform/exported/WrappedResourceRequest.h"
#include "platform/exported/WrappedResourceResponse.h"
#include "platform/network/ResourceError.h"
#include "public/platform/Platform.h"
#include "public/platform/WebData.h"
#include "public/platform/WebThreadedDataReceiver.h"
#include "public/platform/WebURLError.h"
#include "public/platform/WebURLRequest.h"
#include "public/platform/WebURLResponse.h"
#include "wtf/Assertions.h"
#include "wtf/CurrentTime.h"

namespace blink {

ResourceLoader* ResourceLoader::create(ResourceFetcher* fetcher, Resource* resource, const ResourceRequest& request, const ResourceLoaderOptions& options)
{
    ResourceLoader* loader = new ResourceLoader(fetcher, resource, options);
    loader->init(request);
    return loader;
}

ResourceLoader::ResourceLoader(ResourceFetcher* fetcher, Resource* resource, const ResourceLoaderOptions& options)
    : m_fetcher(fetcher)
    , m_notifiedLoadComplete(false)
    , m_defersLoading(fetcher->defersLoading())
    , m_loadingMultipartContent(false)
    , m_options(options)
    , m_resource(resource)
    , m_state(Initialized)
    , m_connectionState(ConnectionStateNew)
{
}

ResourceLoader::~ResourceLoader()
{
    ASSERT(m_state == Terminated);
}

DEFINE_TRACE(ResourceLoader)
{
    visitor->trace(m_fetcher);
    visitor->trace(m_resource);
}

void ResourceLoader::releaseResources()
{
    ASSERT(m_state != Terminated);
    ASSERT(m_notifiedLoadComplete);
    m_fetcher->didLoadResource();
    if (m_state == Terminated)
        return;
    m_resource->clearLoader();
    m_resource->deleteIfPossible();
    m_resource = nullptr;

    ASSERT(m_state != Terminated);

    m_state = Terminated;
    if (m_loader) {
        m_loader->cancel();
        m_loader.clear();
    }
    m_deferredRequest = ResourceRequest();
    m_fetcher.clear();
}

void ResourceLoader::init(const ResourceRequest& passedRequest)
{
    ASSERT(m_state != Terminated);
    ResourceRequest request(passedRequest);
    m_fetcher->willSendRequest(m_resource->identifier(), request, ResourceResponse(), m_options.initiatorInfo);
    ASSERT(m_state != Terminated);
    ASSERT(!request.isNull());
    m_originalRequest = m_request = applyOptions(request);
    m_resource->updateRequest(request);
    ASSERT(m_state != Terminated);
    m_fetcher->didInitializeResourceLoader(this);
}

void ResourceLoader::start()
{
    ASSERT(!m_loader);
    ASSERT(!m_request.isNull());
    ASSERT(m_deferredRequest.isNull());

    if (responseNeedsAccessControlCheck() && m_fetcher->isControlledByServiceWorker()) {
        m_fallbackRequestForServiceWorker = adoptPtr(new ResourceRequest(m_request));
        m_fallbackRequestForServiceWorker->setSkipServiceWorker(true);
    }

    m_fetcher->willStartLoadingResource(m_resource, m_request);

    if (m_options.synchronousPolicy == RequestSynchronously) {
        requestSynchronously();
        return;
    }

    if (m_defersLoading) {
        m_deferredRequest = m_request;
        return;
    }

    if (m_state == Terminated)
        return;

    RELEASE_ASSERT(m_connectionState == ConnectionStateNew);
    m_connectionState = ConnectionStateStarted;

    m_loader = adoptPtr(Platform::current()->createURLLoader());
    ASSERT(m_loader);
    WrappedResourceRequest wrappedRequest(m_request);
    m_loader->loadAsynchronously(wrappedRequest, this);
}

void ResourceLoader::changeToSynchronous()
{
    ASSERT(m_options.synchronousPolicy == RequestAsynchronously);
    ASSERT(m_loader);
    m_loader->cancel();
    m_loader.clear();
    m_request.setPriority(ResourceLoadPriorityHighest);
    m_connectionState = ConnectionStateNew;
    requestSynchronously();
}

void ResourceLoader::setDefersLoading(bool defers)
{
    m_defersLoading = defers;
    if (m_loader)
        m_loader->setDefersLoading(defers);
    if (!defers && !m_deferredRequest.isNull()) {
        m_request = applyOptions(m_deferredRequest);
        m_deferredRequest = ResourceRequest();
        start();
    }
}

void ResourceLoader::attachThreadedDataReceiver(PassRefPtrWillBeRawPtr<ThreadedDataReceiver> threadedDataReceiver)
{
    if (m_loader) {
        // The implementor of the WebURLLoader assumes ownership of the
        // threaded data receiver if it signals that it got successfully
        // attached.
        WebThreadedDataReceiver* webDataReceiver = new WebThreadedDataReceiver(threadedDataReceiver);
        if (!m_loader->attachThreadedDataReceiver(webDataReceiver))
            delete webDataReceiver;
    }
}

void ResourceLoader::didDownloadData(WebURLLoader*, int length, int encodedDataLength)
{
    ASSERT(m_state != Terminated);
    RELEASE_ASSERT(m_connectionState == ConnectionStateReceivedResponse);
    m_fetcher->didDownloadData(m_resource, length, encodedDataLength);
    if (m_state == Terminated)
        return;
    m_resource->didDownloadData(length);
}

void ResourceLoader::didFinishLoadingOnePart(double finishTime, int64_t encodedDataLength)
{
    // If load has been cancelled after finishing (which could happen with a
    // JavaScript that changes the window location), do nothing.
    if (m_state == Terminated)
        return;

    if (m_notifiedLoadComplete)
        return;
    m_notifiedLoadComplete = true;
    m_fetcher->didFinishLoading(m_resource, finishTime, encodedDataLength);
}

void ResourceLoader::didChangePriority(ResourceLoadPriority loadPriority, int intraPriorityValue)
{
    if (m_loader) {
        m_fetcher->didChangeLoadingPriority(m_resource, loadPriority, intraPriorityValue);
        ASSERT(m_state != Terminated);
        m_loader->didChangePriority(static_cast<WebURLRequest::Priority>(loadPriority), intraPriorityValue);
    }
}

void ResourceLoader::cancelIfNotFinishing()
{
    if (m_state != Initialized)
        return;
    cancel();
}

void ResourceLoader::cancel()
{
    cancel(ResourceError());
}

void ResourceLoader::cancel(const ResourceError& error)
{
    // If the load has already completed - succeeded, failed, or previously cancelled - do nothing.
    if (m_state == Terminated)
        return;
    if (m_state == Finishing) {
        releaseResources();
        return;
    }

    ResourceError nonNullError = error.isNull() ? ResourceError::cancelledError(m_request.url()) : error;

    WTF_LOG(ResourceLoading, "Cancelled load of '%s'.\n", m_resource->url().string().latin1().data());
    if (m_state == Initialized)
        m_state = Finishing;
    m_resource->setResourceError(nonNullError);

    if (m_loader) {
        m_connectionState = ConnectionStateCanceled;
        m_loader->cancel();
        m_loader.clear();
    }

    if (!m_notifiedLoadComplete) {
        m_notifiedLoadComplete = true;
        m_fetcher->didFailLoading(m_resource, nonNullError);
    }

    if (m_state == Finishing)
        m_resource->error(Resource::LoadError);
    if (m_state != Terminated)
        releaseResources();
}

void ResourceLoader::willSendRequest(WebURLLoader*, WebURLRequest& passedNewRequest, const WebURLResponse& passedRedirectResponse)
{
    ASSERT(m_state != Terminated);

    ResourceRequest& newRequest(applyOptions(passedNewRequest.toMutableResourceRequest()));

    ASSERT(!newRequest.isNull());
    const ResourceResponse& redirectResponse(passedRedirectResponse.toResourceResponse());
    ASSERT(!redirectResponse.isNull());
    newRequest.setFollowedRedirect(true);
    if (!m_fetcher->canAccessRedirect(m_resource, newRequest, redirectResponse, m_options)) {
        cancel(ResourceError::cancelledDueToAccessCheckError(newRequest.url()));
        return;
    }
    ASSERT(m_state != Terminated);

    applyOptions(newRequest); // canAccessRedirect() can modify m_options so we should re-apply it.
    m_fetcher->redirectReceived(m_resource, redirectResponse);
    ASSERT(m_state != Terminated);
    m_resource->willFollowRedirect(newRequest, redirectResponse);
    if (newRequest.isNull() || m_state == Terminated)
        return;

    m_fetcher->willSendRequest(m_resource->identifier(), newRequest, redirectResponse, m_options.initiatorInfo);
    ASSERT(m_state != Terminated);
    ASSERT(!newRequest.isNull());
    m_resource->updateRequest(newRequest);
    m_request = newRequest;
}

void ResourceLoader::didReceiveCachedMetadata(WebURLLoader*, const char* data, int length)
{
    RELEASE_ASSERT(m_connectionState == ConnectionStateReceivedResponse || m_connectionState == ConnectionStateReceivingData);
    ASSERT(m_state == Initialized);
    m_resource->setSerializedCachedMetadata(data, length);
}

void ResourceLoader::didSendData(WebURLLoader*, unsigned long long bytesSent, unsigned long long totalBytesToBeSent)
{
    ASSERT(m_state == Initialized);
    m_resource->didSendData(bytesSent, totalBytesToBeSent);
}

bool ResourceLoader::responseNeedsAccessControlCheck() const
{
    // If the fetch was (potentially) CORS enabled, an access control check of the response is required.
    return m_options.corsEnabled == IsCORSEnabled;
}

void ResourceLoader::didReceiveResponse(WebURLLoader*, const WebURLResponse& response, WebDataConsumerHandle* rawHandle)
{
    ASSERT(!response.isNull());
    ASSERT(m_state == Initialized);
    // |rawHandle|'s ownership is transferred to the callee.
    OwnPtr<WebDataConsumerHandle> handle = adoptPtr(rawHandle);

    bool isMultipartPayload = response.isMultipartPayload();
    bool isValidStateTransition = (m_connectionState == ConnectionStateStarted || m_connectionState == ConnectionStateReceivedResponse);
    // In the case of multipart loads, calls to didReceiveData & didReceiveResponse can be interleaved.
    RELEASE_ASSERT(isMultipartPayload || isValidStateTransition);
    m_connectionState = ConnectionStateReceivedResponse;

    const ResourceResponse& resourceResponse = response.toResourceResponse();

    if (responseNeedsAccessControlCheck()) {
        if (response.wasFetchedViaServiceWorker()) {
            if (response.wasFallbackRequiredByServiceWorker()) {
                ASSERT(m_fallbackRequestForServiceWorker);
                m_loader->cancel();
                m_loader.clear();
                m_connectionState = ConnectionStateStarted;
                m_request = *m_fallbackRequestForServiceWorker;
                m_loader = adoptPtr(Platform::current()->createURLLoader());
                ASSERT(m_loader);
                WrappedResourceRequest wrappedRequest(m_request);
                m_loader->loadAsynchronously(wrappedRequest, this);
                return;
            }
        } else {
            // If the response successfully validated a cached resource, perform
            // the access control with respect to it. Need to do this right here
            // before the resource switches clients over to that validated resource.
            Resource* resource = m_resource;
            if (resource->isCacheValidator() && resourceResponse.httpStatusCode() == 304)
                resource = m_resource->resourceToRevalidate();
            else
                m_resource->setResponse(resourceResponse);
            if (!m_fetcher->canAccessResource(resource, m_options.securityOrigin.get(), response.url(), ResourceFetcher::ShouldLogAccessControlErrors)) {
                m_fetcher->didReceiveResponse(m_resource, resourceResponse);
                cancel(ResourceError::cancelledDueToAccessCheckError(KURL(response.url())));
                return;
            }
        }
    }

    m_resource->responseReceived(resourceResponse, handle.release());
    if (m_state == Terminated)
        return;

    m_fetcher->didReceiveResponse(m_resource, resourceResponse);
    if (m_state == Terminated)
        return;

    if (response.toResourceResponse().isMultipart()) {
        // We only support multipart for images, though the image may be loaded
        // as a main resource that we end up displaying through an ImageDocument.
        if (!m_resource->isImage() && m_resource->type() != Resource::MainResource) {
            cancel();
            return;
        }
        m_loadingMultipartContent = true;
    } else if (isMultipartPayload) {
        // Since a subresource loader does not load multipart sections progressively, data was delivered to the loader all at once.
        // After the first multipart section is complete, signal to delegates that this load is "finished"
        m_fetcher->subresourceLoaderFinishedLoadingOnePart(this);
        didFinishLoadingOnePart(0, WebURLLoaderClient::kUnknownEncodedDataLength);
    }
    if (m_state == Terminated)
        return;

    if (m_resource->response().httpStatusCode() < 400 || m_resource->shouldIgnoreHTTPStatusCodeErrors())
        return;
    m_state = Finishing;

    if (!m_notifiedLoadComplete) {
        m_notifiedLoadComplete = true;
        m_fetcher->didFailLoading(m_resource, ResourceError::cancelledError(m_request.url()));
    }

    ASSERT(m_state != Terminated);
    m_resource->error(Resource::LoadError);
    cancel();
}

void ResourceLoader::didReceiveResponse(WebURLLoader* loader, const WebURLResponse& response)
{
    didReceiveResponse(loader, response, nullptr);
}

void ResourceLoader::didReceiveData(WebURLLoader*, const char* data, int length, int encodedDataLength)
{
    ASSERT(m_state != Terminated);
    RELEASE_ASSERT(m_connectionState == ConnectionStateReceivedResponse || m_connectionState == ConnectionStateReceivingData);
    m_connectionState = ConnectionStateReceivingData;

    // It is possible to receive data on uninitialized resources if it had an error status code, and we are running a nested message
    // loop. When this occurs, ignoring the data is the correct action.
    if (m_resource->response().httpStatusCode() >= 400 && !m_resource->shouldIgnoreHTTPStatusCodeErrors())
        return;
    ASSERT(m_state == Initialized);

    // FIXME: If we get a resource with more than 2B bytes, this code won't do the right thing.
    // However, with today's computers and networking speeds, this won't happen in practice.
    // Could be an issue with a giant local file.
    m_fetcher->didReceiveData(m_resource, data, length, encodedDataLength);
    if (m_state == Terminated)
        return;
    RELEASE_ASSERT(length >= 0);
    m_resource->appendData(data, length);

#if 0 // ndef NDEBUG
    if (0 != length && m_dump.size() < 32768) {
        Vector<char> dump;
        dump.resize(length);
        memcpy(dump.data(), data, length);
        m_dump.append(data, length); // weolar
    }
#endif
}

bool saveDumpFile(const String& url, char* buffer, unsigned int size);

void ResourceLoader::didFinishLoading(WebURLLoader*, double finishTime, int64_t encodedDataLength)
{
    RELEASE_ASSERT(m_connectionState == ConnectionStateReceivedResponse || m_connectionState == ConnectionStateReceivingData);
    m_connectionState = ConnectionStateFinishedLoading;
    if (m_state != Initialized)
        return;
    ASSERT(m_state != Terminated);
    WTF_LOG(ResourceLoading, "Received '%s'.", m_resource->url().string().latin1().data());

    ResourcePtr<Resource> protectResource(m_resource);
    m_state = Finishing;
    m_resource->setLoadFinishTime(finishTime);
    didFinishLoadingOnePart(finishTime, encodedDataLength);
    if (m_state == Terminated)
        return;
    m_resource->finish();

    // If the load has been cancelled by a delegate in response to didFinishLoad(), do not release
    // the resources a second time, they have been released by cancel.
    if (m_state == Terminated)
        return;
    releaseResources();
}

void ResourceLoader::didFail(WebURLLoader*, const WebURLError& error)
{
    m_connectionState = ConnectionStateFailed;
    ASSERT(m_state != Terminated);
    WTF_LOG(ResourceLoading, "Failed to load '%s'.\n", m_resource->url().string().latin1().data());

    ResourcePtr<Resource> protectResource(m_resource);
    m_state = Finishing;
    m_resource->setResourceError(error);

    if (!m_notifiedLoadComplete) {
        m_notifiedLoadComplete = true;
        m_fetcher->didFailLoading(m_resource, error);
    }
    if (m_state == Terminated)
        return;

    m_resource->error(Resource::LoadError);

    if (m_state == Terminated)
        return;

    releaseResources();
}

bool ResourceLoader::isLoadedBy(ResourceFetcher* loader) const
{
    return m_fetcher->isLoadedBy(loader);
}

void ResourceLoader::requestSynchronously()
{
    OwnPtr<WebURLLoader> loader = adoptPtr(Platform::current()->createURLLoader());
    ASSERT(loader);

    // downloadToFile is not supported for synchronous requests.
    ASSERT(!m_request.downloadToFile());

    ResourcePtr<Resource> protectResource(m_resource);

    RELEASE_ASSERT(m_connectionState == ConnectionStateNew);
    m_connectionState = ConnectionStateStarted;

    WrappedResourceRequest requestIn(m_request);
    WebURLResponse responseOut;
    responseOut.initialize();
    WebURLError errorOut;
    WebData dataOut;
    loader->loadSynchronously(requestIn, responseOut, errorOut, dataOut);
    if (errorOut.reason) {
        if (m_state == Terminated) {
            // A message dispatched while synchronously fetching the resource
            // can bring about the cancellation of this load.
            ASSERT(!m_resource);
            return;
        }
        didFail(0, errorOut);
        return;
    }
    didReceiveResponse(0, responseOut);
    if (m_state == Terminated)
        return;
    RefPtr<ResourceLoadInfo> resourceLoadInfo = responseOut.toResourceResponse().resourceLoadInfo();
    int64_t encodedDataLength = resourceLoadInfo ? resourceLoadInfo->encodedDataLength : WebURLLoaderClient::kUnknownEncodedDataLength;
    m_fetcher->didReceiveData(m_resource, dataOut.data(), dataOut.size(), encodedDataLength);
    m_resource->setResourceBuffer(dataOut);
    didFinishLoading(0, monotonicallyIncreasingTime(), encodedDataLength);
}

ResourceRequest& ResourceLoader::applyOptions(ResourceRequest& request) const
{
    request.setAllowStoredCredentials(m_options.allowCredentials == AllowStoredCredentials);
    return request;
}

}
