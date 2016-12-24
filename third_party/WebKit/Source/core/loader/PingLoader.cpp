/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
 *
 */

#include "config.h"
#include "core/loader/PingLoader.h"

#include "core/dom/Document.h"
#include "core/fetch/FetchContext.h"
#include "core/fetch/FetchInitiatorTypeNames.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/fetch/UniqueIdentifier.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/MixedContentChecker.h"
#include "core/page/Page.h"
#include "platform/exported/WrappedResourceRequest.h"
#include "platform/network/ResourceError.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "platform/weborigin/SecurityPolicy.h"
#include "public/platform/Platform.h"
#include "public/platform/WebURLLoader.h"
#include "public/platform/WebURLRequest.h"
#include "public/platform/WebURLResponse.h"
#include "wtf/OwnPtr.h"

namespace blink {

void PingLoader::loadImage(LocalFrame* frame, const KURL& url)
{
    if (!frame->document()->securityOrigin()->canDisplay(url)) {
        FrameLoader::reportLocalLoadFailed(frame, url.string());
        return;
    }

    ResourceRequest request(url);
    request.setRequestContext(WebURLRequest::RequestContextPing);
    request.setHTTPHeaderField("Cache-Control", "max-age=0");
    frame->document()->fetcher()->context().addAdditionalRequestHeaders(request, FetchSubresource);
    frame->document()->fetcher()->context().setFirstPartyForCookies(request);

    FetchInitiatorInfo initiatorInfo;
    initiatorInfo.name = FetchInitiatorTypeNames::ping;
    PingLoader::start(frame, request, initiatorInfo);
}

// http://www.whatwg.org/specs/web-apps/current-work/multipage/links.html#hyperlink-auditing
void PingLoader::sendLinkAuditPing(LocalFrame* frame, const KURL& pingURL, const KURL& destinationURL)
{
    ResourceRequest request(pingURL);
    request.setRequestContext(WebURLRequest::RequestContextPing);
    request.setHTTPMethod("POST");
    request.setHTTPContentType("text/ping");
    request.setHTTPBody(FormData::create("PING"));
    request.setHTTPHeaderField("Cache-Control", "max-age=0");
    frame->document()->fetcher()->context().addAdditionalRequestHeaders(request, FetchSubresource);
    frame->document()->fetcher()->context().setFirstPartyForCookies(request);

    RefPtr<SecurityOrigin> pingOrigin = SecurityOrigin::create(pingURL);
    // addAdditionalRequestHeaders() will have added a referrer for same origin requests,
    // but the spec omits the referrer for same origin.
    if (frame->document()->securityOrigin()->isSameSchemeHostPort(pingOrigin.get()))
        request.clearHTTPReferrer();

    request.setHTTPHeaderField("Ping-To", AtomicString(destinationURL.string()));

    // Ping-From follows the same rules as the default referrer beahavior for subresource requests.
    // FIXME: Should Ping-From obey ReferrerPolicy?
    if (!SecurityPolicy::shouldHideReferrer(pingURL, frame->document()->url().string()))
        request.setHTTPHeaderField("Ping-From", AtomicString(frame->document()->url().string()));

    FetchInitiatorInfo initiatorInfo;
    initiatorInfo.name = FetchInitiatorTypeNames::ping;
    PingLoader::start(frame, request, initiatorInfo);
}

void PingLoader::sendViolationReport(LocalFrame* frame, const KURL& reportURL, PassRefPtr<FormData> report, ViolationReportType type)
{
    ResourceRequest request(reportURL);
    request.setRequestContext(WebURLRequest::RequestContextPing);
    request.setHTTPMethod("POST");
    request.setHTTPContentType(type == ContentSecurityPolicyViolationReport ? "application/csp-report" : "application/json");
    request.setHTTPBody(report);
    frame->document()->fetcher()->context().addAdditionalRequestHeaders(request, FetchSubresource);
    frame->document()->fetcher()->context().setFirstPartyForCookies(request);

    FetchInitiatorInfo initiatorInfo;
    initiatorInfo.name = FetchInitiatorTypeNames::violationreport;
    PingLoader::start(frame, request, initiatorInfo, SecurityOrigin::create(reportURL)->isSameSchemeHostPort(frame->document()->securityOrigin()) ? AllowStoredCredentials : DoNotAllowStoredCredentials);
}

void PingLoader::start(LocalFrame* frame, ResourceRequest& request, const FetchInitiatorInfo& initiatorInfo, StoredCredentials credentialsAllowed)
{
    if (MixedContentChecker::shouldBlockFetch(frame, request, request.url()))
        return;

    // Leak the ping loader, since it will kill itself as soon as it receives a response.
    RefPtrWillBeRawPtr<PingLoader> loader = adoptRefWillBeNoop(new PingLoader(frame, request, initiatorInfo, credentialsAllowed));
    loader->ref();
}

PingLoader::PingLoader(LocalFrame* frame, ResourceRequest& request, const FetchInitiatorInfo& initiatorInfo, StoredCredentials credentialsAllowed)
    : PageLifecycleObserver(frame->page())
    , m_timeout(this, &PingLoader::timeout)
    , m_url(request.url())
    , m_identifier(createUniqueIdentifier())
{
    frame->loader().client()->didDispatchPingLoader(request.url());
    frame->loader().client()->dispatchWillSendRequest(frame->loader().documentLoader(), m_identifier, request, ResourceResponse()); // Add by weolar

    TRACE_EVENT_INSTANT1("devtools.timeline", "ResourceSendRequest", TRACE_EVENT_SCOPE_THREAD, "data", InspectorSendRequestEvent::data(m_identifier, frame, request));
    InspectorInstrumentation::willSendRequest(frame, m_identifier, frame->loader().documentLoader(), request, ResourceResponse(), initiatorInfo);

    m_loader = adoptPtr(Platform::current()->createURLLoader());
    ASSERT(m_loader);
    WrappedResourceRequest wrappedRequest(request);
    wrappedRequest.setAllowStoredCredentials(credentialsAllowed == AllowStoredCredentials);
    m_loader->loadAsynchronously(wrappedRequest, this);

    // If the server never responds, FrameLoader won't be able to cancel this load and
    // we'll sit here waiting forever. Set a very generous timeout, just in case.
    m_timeout.startOneShot(60000, FROM_HERE);
}

PingLoader::~PingLoader()
{
    if (m_loader)
        m_loader->cancel();
}

void PingLoader::dispose()
{
    m_timeout.stop(); // weolar add.
    if (m_loader) {
        m_loader->cancel();
        m_loader = nullptr;
    }
    deref();
}

void PingLoader::didReceiveResponse(WebURLLoader*, const WebURLResponse& response)
{
    if (Page* page = this->page()) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "ResourceFinish", TRACE_EVENT_SCOPE_THREAD, "data", InspectorResourceFinishEvent::data(m_identifier, 0, true));
        const ResourceResponse& resourceResponse = response.toResourceResponse();
        InspectorInstrumentation::didReceiveResourceResponse(page->deprecatedLocalMainFrame(), m_identifier, 0, resourceResponse, 0);
        didFailLoading(page);
    }
    dispose();
}

void PingLoader::didReceiveData(WebURLLoader*, const char*, int, int)
{
    if (Page* page = this->page()) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "ResourceFinish", TRACE_EVENT_SCOPE_THREAD, "data", InspectorResourceFinishEvent::data(m_identifier, 0, true));
        didFailLoading(page);
    }
    dispose();
}

void PingLoader::didFinishLoading(WebURLLoader*, double, int64_t)
{
    if (Page* page = this->page()) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "ResourceFinish", TRACE_EVENT_SCOPE_THREAD, "data", InspectorResourceFinishEvent::data(m_identifier, 0, true));
        didFailLoading(page);
    }
    dispose();
}

void PingLoader::didFail(WebURLLoader*, const WebURLError& resourceError)
{
    if (Page* page = this->page()) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "ResourceFinish", TRACE_EVENT_SCOPE_THREAD, "data", InspectorResourceFinishEvent::data(m_identifier, 0, true));
        didFailLoading(page);
    }
    dispose();
}

void PingLoader::timeout(Timer<PingLoader>*)
{
    if (Page* page = this->page()) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "ResourceFinish", TRACE_EVENT_SCOPE_THREAD, "data", InspectorResourceFinishEvent::data(m_identifier, 0, true));
        didFailLoading(page);
    }
    dispose();
}

void PingLoader::didFailLoading(Page* page)
{
    LocalFrame* frame = page->deprecatedLocalMainFrame();
    if (!frame) // weolar
        return;
    InspectorInstrumentation::didFailLoading(frame, m_identifier, ResourceError::cancelledError(m_url));
    frame->console().didFailLoading(m_identifier, ResourceError::cancelledError(m_url));
}

DEFINE_TRACE(PingLoader)
{
    PageLifecycleObserver::trace(visitor);
}

} // namespace blink
