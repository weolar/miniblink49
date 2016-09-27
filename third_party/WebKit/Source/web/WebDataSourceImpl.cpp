/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "web/WebDataSourceImpl.h"

#include "core/dom/Document.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLError.h"
#include "public/platform/WebVector.h"

namespace blink {

static OwnPtr<WebPluginLoadObserver>& nextPluginLoadObserver()
{
    DEFINE_STATIC_LOCAL(OwnPtr<WebPluginLoadObserver>, nextPluginLoadObserver, ());
    return nextPluginLoadObserver;
}

PassRefPtrWillBeRawPtr<WebDataSourceImpl> WebDataSourceImpl::create(LocalFrame* frame, const ResourceRequest& request, const SubstituteData& data)
{
    return adoptRefWillBeNoop(new WebDataSourceImpl(frame, request, data));
}

const WebURLRequest& WebDataSourceImpl::originalRequest() const
{
    m_originalRequestWrapper.bind(DocumentLoader::originalRequest());
    return m_originalRequestWrapper;
}

const WebURLRequest& WebDataSourceImpl::request() const
{
    m_requestWrapper.bind(DocumentLoader::request());
    return m_requestWrapper;
}

const WebURLResponse& WebDataSourceImpl::response() const
{
    m_responseWrapper.bind(DocumentLoader::response());
    return m_responseWrapper;
}

bool WebDataSourceImpl::hasUnreachableURL() const
{
    return !DocumentLoader::unreachableURL().isEmpty();
}

WebURL WebDataSourceImpl::unreachableURL() const
{
    return DocumentLoader::unreachableURL();
}

void WebDataSourceImpl::appendRedirect(const WebURL& url)
{
    DocumentLoader::appendRedirect(url);
}

void WebDataSourceImpl::redirectChain(WebVector<WebURL>& result) const
{
    result.assign(m_redirectChain);
}

bool WebDataSourceImpl::isClientRedirect() const
{
    return DocumentLoader::isClientRedirect();
}

bool WebDataSourceImpl::replacesCurrentHistoryItem() const
{
    return DocumentLoader::replacesCurrentHistoryItem();
}

WebNavigationType WebDataSourceImpl::navigationType() const
{
    return toWebNavigationType(DocumentLoader::navigationType());
}

WebDataSource::ExtraData* WebDataSourceImpl::extraData() const
{
    return m_extraData.get();
}

void WebDataSourceImpl::setExtraData(ExtraData* extraData)
{
    // extraData can't be a PassOwnPtr because setExtraData is a WebKit API function.
    m_extraData = adoptPtr(extraData);
}

void WebDataSourceImpl::setNavigationStartTime(double navigationStart)
{
    timing().setNavigationStart(navigationStart);
}

WebNavigationType WebDataSourceImpl::toWebNavigationType(NavigationType type)
{
    switch (type) {
    case NavigationTypeLinkClicked:
        return WebNavigationTypeLinkClicked;
    case NavigationTypeFormSubmitted:
        return WebNavigationTypeFormSubmitted;
    case NavigationTypeBackForward:
        return WebNavigationTypeBackForward;
    case NavigationTypeReload:
        return WebNavigationTypeReload;
    case NavigationTypeFormResubmitted:
        return WebNavigationTypeFormResubmitted;
    case NavigationTypeOther:
    default:
        return WebNavigationTypeOther;
    }
}

void WebDataSourceImpl::setNextPluginLoadObserver(PassOwnPtr<WebPluginLoadObserver> observer)
{
    nextPluginLoadObserver() = observer;
}

WebDataSourceImpl::WebDataSourceImpl(LocalFrame* frame, const ResourceRequest& request, const SubstituteData& data)
    : DocumentLoader(frame, request, data)
{
    if (!nextPluginLoadObserver())
        return;
    // When a new frame is created, it initially gets a data source for an
    // empty document. Then it is navigated to the source URL of the
    // frame, which results in a second data source being created. We want
    // to wait to attach the WebPluginLoadObserver to that data source.
    if (request.url().isEmpty())
        return;

    ASSERT(nextPluginLoadObserver()->url() == WebURL(request.url()));
    m_pluginLoadObserver = nextPluginLoadObserver().release();
}

WebDataSourceImpl::~WebDataSourceImpl()
{
    // Verify that detachFromFrame() has been called.
    ASSERT(!m_extraData);
}

void WebDataSourceImpl::detachFromFrame()
{
    RefPtrWillBeRawPtr<DocumentLoader> protect(this);

    DocumentLoader::detachFromFrame();
    m_extraData.clear();
    m_pluginLoadObserver.clear();
}

DEFINE_TRACE(WebDataSourceImpl)
{
    DocumentLoader::trace(visitor);
}

} // namespace blink
