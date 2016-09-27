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

#include "config.h"
#include "core/loader/CookieJar.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/FrameLoaderClient.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCookieJar.h"
#include "public/platform/WebURL.h"

namespace blink {

static WebCookieJar* toCookieJar(const Document* document)
{
    if (!document || !document->frame())
        return 0;
    return document->frame()->loader().client()->cookieJar();
}

String cookies(const Document* document, const KURL& url)
{
    WebCookieJar* cookieJar = toCookieJar(document);
    if (!cookieJar)
        return String();
    return cookieJar->cookies(url, document->firstPartyForCookies());
}

void setCookies(Document* document, const KURL& url, const String& cookieString)
{
    WebCookieJar* cookieJar = toCookieJar(document);
    if (!cookieJar)
        return;
    cookieJar->setCookie(url, document->firstPartyForCookies(), cookieString);
}

bool cookiesEnabled(const Document* document)
{
    WebCookieJar* cookieJar = toCookieJar(document);
    if (!cookieJar)
        return false;
    return cookieJar->cookiesEnabled(document->cookieURL(), document->firstPartyForCookies());
}

String cookieRequestHeaderFieldValue(const Document* document, const KURL& url)
{
    WebCookieJar* cookieJar = toCookieJar(document);
    if (!cookieJar)
        return String();
    return cookieJar->cookieRequestHeaderFieldValue(url, document->firstPartyForCookies());
}

} // namespace blink
