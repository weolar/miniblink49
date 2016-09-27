/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "core/html/LinkRelAttribute.h"

#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

LinkRelAttribute::LinkRelAttribute(const String& rel)
    : m_iconType(InvalidIcon)
    , m_isStyleSheet(false)
    , m_isAlternate(false)
    , m_isDNSPrefetch(false)
    , m_isPreconnect(false)
    , m_isLinkPrefetch(false)
    , m_isLinkSubresource(false)
    , m_isLinkPreload(false)
    , m_isLinkPrerender(false)
    , m_isLinkNext(false)
    , m_isImport(false)
    , m_isManifest(false)
    , m_isDefaultPresentation(false)
{
    if (rel.isEmpty())
        return;
    String relCopy = rel;
    relCopy.replace('\n', ' ');
    Vector<String> list;
    relCopy.split(' ', list);
    for (const String& linkType : list) {
        if (equalIgnoringCase(linkType, "stylesheet")) {
            if (!m_isImport)
                m_isStyleSheet = true;
        } else if (equalIgnoringCase(linkType, "import")) {
            if (!m_isStyleSheet)
                m_isImport = true;
        } else if (equalIgnoringCase(linkType, "alternate")) {
            m_isAlternate = true;
        } else if (equalIgnoringCase(linkType, "icon")) {
            // This also allows "shortcut icon" since we just ignore the non-standard "shortcut" token.
            // FIXME: This doesn't really follow the spec that requires "shortcut icon" to be the
            // entire string http://www.whatwg.org/specs/web-apps/current-work/multipage/links.html#rel-icon
            m_iconType = Favicon;
        } else if (equalIgnoringCase(linkType, "prefetch")) {
            m_isLinkPrefetch = true;
        } else if (equalIgnoringCase(linkType, "dns-prefetch")) {
            m_isDNSPrefetch = true;
        } else if (equalIgnoringCase(linkType, "preconnect")) {
            if (RuntimeEnabledFeatures::linkPreconnectEnabled())
                m_isPreconnect = true;
        } else if (equalIgnoringCase(linkType, "subresource")) {
            m_isLinkSubresource = true;
        } else if (equalIgnoringCase(linkType, "preload")) {
            if (RuntimeEnabledFeatures::linkPreloadEnabled())
                m_isLinkPreload = true;
        } else if (equalIgnoringCase(linkType, "prerender")) {
            m_isLinkPrerender = true;
        } else if (equalIgnoringCase(linkType, "next")) {
            m_isLinkNext = true;
        } else if (equalIgnoringCase(linkType, "apple-touch-icon")) {
            if (RuntimeEnabledFeatures::touchIconLoadingEnabled())
                m_iconType = TouchIcon;
        } else if (equalIgnoringCase(linkType, "apple-touch-icon-precomposed")) {
            if (RuntimeEnabledFeatures::touchIconLoadingEnabled())
                m_iconType = TouchPrecomposedIcon;
        } else if (equalIgnoringCase(linkType, "manifest")) {
            m_isManifest = true;
        } else if (equalIgnoringCase(linkType, "default-presentation")) {
            if (RuntimeEnabledFeatures::presentationEnabled())
                m_isDefaultPresentation = true;
        }
    }
}

}
