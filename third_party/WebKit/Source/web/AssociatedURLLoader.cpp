/*
 * Copyright (C) 2010, 2011, 2012 Google Inc. All rights reserved.
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
#include "web/AssociatedURLLoader.h"

#include "core/fetch/CrossOriginAccessControl.h"
#include "core/fetch/FetchUtils.h"
#include "core/loader/DocumentThreadableLoader.h"
#include "core/loader/DocumentThreadableLoaderClient.h"
#include "platform/Timer.h"
#include "platform/exported/WrappedResourceRequest.h"
#include "platform/exported/WrappedResourceResponse.h"
#include "platform/network/HTTPParsers.h"
#include "platform/network/ResourceError.h"
#include "public/platform/WebHTTPHeaderVisitor.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURLError.h"
#include "public/platform/WebURLLoaderClient.h"
#include "public/platform/WebURLRequest.h"
#include "public/web/WebDataSource.h"
#include "web/WebLocalFrameImpl.h"
#include "wtf/HashSet.h"
#include "wtf/text/WTFString.h"
#include <limits.h>

namespace blink {

namespace {

class HTTPRequestHeaderValidator : public WebHTTPHeaderVisitor {
    WTF_MAKE_NONCOPYABLE(HTTPRequestHeaderValidator);
public:
    HTTPRequestHeaderValidator() : m_isSafe(true) { }

    void visitHeader(const WebString& name, const WebString& value);
    bool isSafe() const { return m_isSafe; }

private:
    bool m_isSafe;
};

void HTTPRequestHeaderValidator::visitHeader(const WebString& name, const WebString& value)
{
    m_isSafe = m_isSafe && isValidHTTPToken(name) && !FetchUtils::isForbiddenHeaderName(name) && isValidHTTPHeaderValue(value);
}

// FIXME: Remove this and use WebCore code that does the same thing.
class HTTPResponseHeaderValidator : public WebHTTPHeaderVisitor {
    WTF_MAKE_NONCOPYABLE(HTTPResponseHeaderValidator);
public:
    HTTPResponseHeaderValidator(bool usingAccessControl) : m_usingAccessControl(usingAccessControl) { }

    void visitHeader(const WebString& name, const WebString& value);
    const HTTPHeaderSet& blockedHeaders();

private:
    HTTPHeaderSet m_exposedHeaders;
    HTTPHeaderSet m_blockedHeaders;
    bool m_usingAccessControl;
};

void HTTPResponseHeaderValidator::visitHeader(const WebString& name, const WebString& value)
{
    String headerName(name);
    if (m_usingAccessControl) {
        if (equalIgnoringCase(headerName, "access-control-expose-headers"))
            parseAccessControlExposeHeadersAllowList(value, m_exposedHeaders);
        else if (!isOnAccessControlResponseHeaderWhitelist(headerName))
            m_blockedHeaders.add(name);
    }
}

const HTTPHeaderSet& HTTPResponseHeaderValidator::blockedHeaders()
{
    // Remove exposed headers from the blocked set.
    if (!m_exposedHeaders.isEmpty()) {
        // Don't allow Set-Cookie headers to be exposed.
        m_exposedHeaders.remove("set-cookie");
        m_exposedHeaders.remove("set-cookie2");
        // Block Access-Control-Expose-Header itself. It could be exposed later.
        m_blockedHeaders.add("access-control-expose-headers");
        m_blockedHeaders.removeAll(m_exposedHeaders);
    }

    return m_blockedHeaders;
}

}

// This class bridges the interface differences between WebCore and WebKit loader clients.
// It forwards its ThreadableLoaderClient notifications to a WebURLLoaderClient.
class AssociatedURLLoader::ClientAdapter final : public DocumentThreadableLoaderClient {
    WTF_MAKE_NONCOPYABLE(ClientAdapter);
public:
    static PassOwnPtr<ClientAdapter> create(AssociatedURLLoader*, WebURLLoaderClient*, const WebURLLoaderOptions&);

    // ThreadableLoaderClient
    void didSendData(unsigned long long /*bytesSent*/, unsigned long long /*totalBytesToBeSent*/) override;
    void didReceiveResponse(unsigned long, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>) override;
    void didDownloadData(int /*dataLength*/) override;
    void didReceiveData(const char*, unsigned /*dataLength*/) override;
    void didReceiveCachedMetadata(const char*, int /*dataLength*/) override;
    void didFinishLoading(unsigned long /*identifier*/, double /*finishTime*/) override;
    void didFail(const ResourceError&) override;
    void didFailRedirectCheck() override;
    // DocumentThreadableLoaderClient
    void willFollowRedirect(ResourceRequest& /*newRequest*/, const ResourceResponse& /*redirectResponse*/) override;

    // Sets an error to be reported back to the client, asychronously.
    void setDelayedError(const ResourceError&);

    // Enables forwarding of error notifications to the WebURLLoaderClient. These must be
    // deferred until after the call to AssociatedURLLoader::loadAsynchronously() completes.
    void enableErrorNotifications();

    // Stops loading and releases the DocumentThreadableLoader as early as possible.
    void clearClient() { m_client = 0; }

private:
    ClientAdapter(AssociatedURLLoader*, WebURLLoaderClient*, const WebURLLoaderOptions&);

    void notifyError(Timer<ClientAdapter>*);

    AssociatedURLLoader* m_loader;
    WebURLLoaderClient* m_client;
    WebURLLoaderOptions m_options;
    WebURLError m_error;

    Timer<ClientAdapter> m_errorTimer;
    bool m_enableErrorNotifications;
    bool m_didFail;
};

PassOwnPtr<AssociatedURLLoader::ClientAdapter> AssociatedURLLoader::ClientAdapter::create(AssociatedURLLoader* loader, WebURLLoaderClient* client, const WebURLLoaderOptions& options)
{
    return adoptPtr(new ClientAdapter(loader, client, options));
}

AssociatedURLLoader::ClientAdapter::ClientAdapter(AssociatedURLLoader* loader, WebURLLoaderClient* client, const WebURLLoaderOptions& options)
    : m_loader(loader)
    , m_client(client)
    , m_options(options)
    , m_errorTimer(this, &ClientAdapter::notifyError)
    , m_enableErrorNotifications(false)
    , m_didFail(false)
{
    ASSERT(m_loader);
    ASSERT(m_client);
}

void AssociatedURLLoader::ClientAdapter::willFollowRedirect(ResourceRequest& newRequest, const ResourceResponse& redirectResponse)
{
    if (!m_client)
        return;

    WrappedResourceRequest wrappedNewRequest(newRequest);
    WrappedResourceResponse wrappedRedirectResponse(redirectResponse);
    m_client->willSendRequest(m_loader, wrappedNewRequest, wrappedRedirectResponse);
}

void AssociatedURLLoader::ClientAdapter::didSendData(unsigned long long bytesSent, unsigned long long totalBytesToBeSent)
{
    if (!m_client)
        return;

    m_client->didSendData(m_loader, bytesSent, totalBytesToBeSent);
}

void AssociatedURLLoader::ClientAdapter::didReceiveResponse(unsigned long, const ResourceResponse& response, PassOwnPtr<WebDataConsumerHandle> handle)
{
    ASSERT_UNUSED(handle, !handle);
    if (!m_client)
        return;

    // Try to use the original ResourceResponse if possible.
    WebURLResponse validatedResponse = WrappedResourceResponse(response);
    HTTPResponseHeaderValidator validator(m_options.crossOriginRequestPolicy == WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl);
    if (!m_options.exposeAllResponseHeaders)
        validatedResponse.visitHTTPHeaderFields(&validator);

    // If there are blocked headers, copy the response so we can remove them.
    const HTTPHeaderSet& blockedHeaders = validator.blockedHeaders();
    if (!blockedHeaders.isEmpty()) {
        validatedResponse = WebURLResponse(validatedResponse);
        HTTPHeaderSet::const_iterator end = blockedHeaders.end();
        for (HTTPHeaderSet::const_iterator it = blockedHeaders.begin(); it != end; ++it)
            validatedResponse.clearHTTPHeaderField(*it);
    }
    m_client->didReceiveResponse(m_loader, validatedResponse);
}

void AssociatedURLLoader::ClientAdapter::didDownloadData(int dataLength)
{
    if (!m_client)
        return;

    m_client->didDownloadData(m_loader, dataLength, -1);
}

void AssociatedURLLoader::ClientAdapter::didReceiveData(const char* data, unsigned dataLength)
{
    if (!m_client)
        return;

    RELEASE_ASSERT(dataLength <= static_cast<unsigned>(std::numeric_limits<int>::max()));

    m_client->didReceiveData(m_loader, data, dataLength, -1);
}

void AssociatedURLLoader::ClientAdapter::didReceiveCachedMetadata(const char* data, int dataLength)
{
    if (!m_client)
        return;

    m_client->didReceiveCachedMetadata(m_loader, data, dataLength);
}

void AssociatedURLLoader::ClientAdapter::didFinishLoading(unsigned long identifier, double finishTime)
{
    if (!m_client)
        return;

    m_client->didFinishLoading(m_loader, finishTime, WebURLLoaderClient::kUnknownEncodedDataLength);
}

void AssociatedURLLoader::ClientAdapter::didFail(const ResourceError& error)
{
    if (!m_client)
        return;

    m_didFail = true;
    m_error = WebURLError(error);
    if (m_enableErrorNotifications)
        notifyError(&m_errorTimer);
}

void AssociatedURLLoader::ClientAdapter::didFailRedirectCheck()
{
    didFail(ResourceError());
}

void AssociatedURLLoader::ClientAdapter::setDelayedError(const ResourceError& error)
{
    didFail(error);
}

void AssociatedURLLoader::ClientAdapter::enableErrorNotifications()
{
    m_enableErrorNotifications = true;
    // If an error has already been received, start a timer to report it to the client
    // after AssociatedURLLoader::loadAsynchronously has returned to the caller.
    if (m_didFail)
        m_errorTimer.startOneShot(0, FROM_HERE);
}

void AssociatedURLLoader::ClientAdapter::notifyError(Timer<ClientAdapter>* timer)
{
    ASSERT_UNUSED(timer, timer == &m_errorTimer);

    m_client->didFail(m_loader, m_error);
}

AssociatedURLLoader::AssociatedURLLoader(PassRefPtrWillBeRawPtr<WebLocalFrameImpl> frameImpl, const WebURLLoaderOptions& options)
    : m_frameImpl(frameImpl)
    , m_options(options)
    , m_client(0)
{
    ASSERT(m_frameImpl);
}

AssociatedURLLoader::~AssociatedURLLoader()
{
    cancel();
}

#define STATIC_ASSERT_MATCHING_ENUM(webkit_name, webcore_name) \
    static_assert(static_cast<int>(webkit_name) == static_cast<int>(webcore_name), "mismatching enum values")

STATIC_ASSERT_MATCHING_ENUM(WebURLLoaderOptions::CrossOriginRequestPolicyDeny, DenyCrossOriginRequests);
STATIC_ASSERT_MATCHING_ENUM(WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl, UseAccessControl);
STATIC_ASSERT_MATCHING_ENUM(WebURLLoaderOptions::CrossOriginRequestPolicyAllow, AllowCrossOriginRequests);

STATIC_ASSERT_MATCHING_ENUM(WebURLLoaderOptions::ConsiderPreflight, ConsiderPreflight);
STATIC_ASSERT_MATCHING_ENUM(WebURLLoaderOptions::ForcePreflight, ForcePreflight);
STATIC_ASSERT_MATCHING_ENUM(WebURLLoaderOptions::PreventPreflight, PreventPreflight);

void AssociatedURLLoader::loadSynchronously(const WebURLRequest& request, WebURLResponse& response, WebURLError& error, WebData& data)
{
    ASSERT(0); // Synchronous loading is not supported.
}

void AssociatedURLLoader::loadAsynchronously(const WebURLRequest& request, WebURLLoaderClient* client)
{
    ASSERT(!m_loader);
    ASSERT(!m_client);

    m_client = client;
    ASSERT(m_client);

    bool allowLoad = true;
    WebURLRequest newRequest(request);
    if (m_options.untrustedHTTP) {
        WebString method = newRequest.httpMethod();
        allowLoad = isValidHTTPToken(method) && FetchUtils::isUsefulMethod(method);
        if (allowLoad) {
            newRequest.setHTTPMethod(FetchUtils::normalizeMethod(method));
            HTTPRequestHeaderValidator validator;
            newRequest.visitHTTPHeaderFields(&validator);
            allowLoad = validator.isSafe();
        }
    }

    m_clientAdapter = ClientAdapter::create(this, m_client, m_options);

    if (allowLoad) {
        ThreadableLoaderOptions options;
        options.preflightPolicy = static_cast<PreflightPolicy>(m_options.preflightPolicy);
        options.crossOriginRequestPolicy = static_cast<CrossOriginRequestPolicy>(m_options.crossOriginRequestPolicy);

        ResourceLoaderOptions resourceLoaderOptions;
        resourceLoaderOptions.allowCredentials = m_options.allowCredentials ? AllowStoredCredentials : DoNotAllowStoredCredentials;
        resourceLoaderOptions.dataBufferingPolicy = DoNotBufferData;

        const ResourceRequest& webcoreRequest = newRequest.toResourceRequest();
        if (webcoreRequest.requestContext() == WebURLRequest::RequestContextUnspecified) {
            // FIXME: We load URLs without setting a TargetType (and therefore a request context) in several
            // places in content/ (P2PPortAllocatorSession::AllocateLegacyRelaySession, for example). Remove
            // this once those places are patched up.
            newRequest.setRequestContext(WebURLRequest::RequestContextInternal);
        }

        Document* webcoreDocument = m_frameImpl->frame()->document();
        ASSERT(webcoreDocument);
        m_loader = DocumentThreadableLoader::create(*webcoreDocument, m_clientAdapter.get(), webcoreRequest, options, resourceLoaderOptions);
    }

    if (!m_loader) {
        // FIXME: return meaningful error codes.
        m_clientAdapter->setDelayedError(ResourceError());
    }
    m_clientAdapter->enableErrorNotifications();
}

void AssociatedURLLoader::cancel()
{
    if (m_clientAdapter)
        m_clientAdapter->clearClient();
    if (m_loader)
        m_loader->cancel();
}

void AssociatedURLLoader::setDefersLoading(bool defersLoading)
{
    if (m_loader)
        m_loader->setDefersLoading(defersLoading);
}

} // namespace blink
