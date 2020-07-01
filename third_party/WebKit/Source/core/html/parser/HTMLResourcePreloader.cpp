/*
 * Copyright (C) 2013 Google Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
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
#include "core/html/parser/HTMLResourcePreloader.h"

#include "core/dom/Document.h"
#include "core/fetch/FetchInitiatorInfo.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/loader/DocumentLoader.h"
#include "platform/network/NetworkHints.h"
#include "public/platform/Platform.h"

namespace blink {

inline HTMLResourcePreloader::HTMLResourcePreloader(Document& document)
    : m_document(document)
{
}

PassOwnPtrWillBeRawPtr<HTMLResourcePreloader> HTMLResourcePreloader::create(Document& document)
{
    return adoptPtrWillBeNoop(new HTMLResourcePreloader(document));
}

DEFINE_TRACE(HTMLResourcePreloader)
{
    visitor->trace(m_document);
}

static void preconnectHost(PreloadRequest* request)
{
    ASSERT(request);
    ASSERT(request->isPreconnect());
    KURL host(request->baseURL(), request->resourceURL());
    if (!host.isValid() || !host.protocolIsInHTTPFamily())
        return;
    CrossOriginAttributeValue crossOrigin = CrossOriginAttributeNotSet;
    if (request->isCORS()) {
        if (request->isAllowCredentials())
            crossOrigin = CrossOriginAttributeUseCredentials;
        else
            crossOrigin = CrossOriginAttributeAnonymous;
    }
    preconnect(host, crossOrigin);
}

// some ali url is use GBK by mistake
// TODO ScriptLoader::prepareScript
bool isBlacklistWebsiteToUseUtf8(const String& url)
{
    if (WTF::kNotFound != url.find("g.alicdn.com/kissy/k/1.4.4/seed.js") ||
        WTF::kNotFound != url.find("g.alicdn.com/kg/??select") ||
        WTF::kNotFound != url.find("g.alicdn.com/kg/??slide")
        )
        return true;
    return false;
}

void HTMLResourcePreloader::preload(PassOwnPtr<PreloadRequest> preload)
{
    if (preload->isPreconnect()) {
        preconnectHost(preload.get());
        return;
    }
    // TODO(yoichio): Should preload if document is imported.
    if (!m_document->loader())
        return;
    FetchRequest request = preload->resourceRequest(m_document);
    // TODO(dgozman): This check should go to HTMLPreloadScanner, but this requires
    // making Document::completeURLWithOverride logic to be statically accessible.
    if (request.url().protocolIsData())
        return;
    if (preload->resourceType() == Resource::Script || preload->resourceType() == Resource::CSSStyleSheet || preload->resourceType() == Resource::ImportResource) {
        String charset = preload->charset().isEmpty() ? m_document->charset().string() : preload->charset();
#ifndef MINIBLINK_NO_CHANGE
        if (preload->resourceType() == Resource::Script && isBlacklistWebsiteToUseUtf8(preload->resourceURL()))
            charset = "UTF-8";
#endif
        request.setCharset(charset);
    }
    request.setForPreload(true);
    Platform::current()->histogramCustomCounts("WebCore.PreloadDelayMs", static_cast<int>(1000 * (monotonicallyIncreasingTime() - preload->discoveryTime())), 0, 2000, 20);
    m_document->loader()->startPreload(preload->resourceType(), request);
}

} // namespace blink
