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
#include "platform/weborigin/OriginAccessEntry.h"

#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "public/platform/WebPublicSuffixList.h"
// #include <url/third_party/mozilla/url_parse.h>
// #include <url/url_canon.h>

namespace blink {

namespace {

// TODO(mkwst): This basically replicates GURL::HostIsIPAddress. If/when
// we re-evaluate everything after merging the Blink and Chromium
// repositories, perhaps we can just use that directly.
bool HostIsIPAddress(const String& host)
{
    if (host.isEmpty())
        return false;

#ifdef MINIBLINK_NOT_IMPLEMENTED
    String protocol("https://");
    KURL url(KURL(), protocol + host + "/");
    if (!url.isValid())
        return false;

    url::RawCanonOutputT<char, 128> ignoredOutput;
    url::CanonHostInfo hostInfo;
    url::Component hostComponent(0, static_cast<int>(url.host().utf8().length()));
    url::CanonicalizeIPAddress(url.host().utf8().data(), hostComponent, &ignoredOutput, &hostInfo);
	return hostInfo.IsIPAddress();
#else
    Vector<String> result;
    host.split(L'.', result);
    for (size_t i = 0; i < result.size(); ++i) {
        bool ok = 0;
        int ip = result[i].toInt(&ok);
        if (!ok || ip < 0 || ip > 255)
            return false;
    }

	return true;
#endif // MINIBLINK_NOT_IMPLEMENTED
}

bool IsSubdomainOfHost(const String& subdomain, const String& host)
{
    if (subdomain.length() <= host.length())
        return false;

    if (subdomain[subdomain.length() - host.length() - 1] != '.')
        return false;

    if (!subdomain.endsWith(host))
        return false;

    return true;
}
}

OriginAccessEntry::OriginAccessEntry(const String& protocol, const String& host, SubdomainSetting subdomainSetting)
    : m_protocol(protocol.lower())
    , m_host(host.lower())
    , m_subdomainSettings(subdomainSetting)
    , m_hostIsPublicSuffix(false)
{
    ASSERT(subdomainSetting >= AllowSubdomains || subdomainSetting <= DisallowSubdomains);

    m_hostIsIPAddress = HostIsIPAddress(host);

    // Look for top-level domains, either with or without an additional dot.
    if (!m_hostIsIPAddress) {
        WebPublicSuffixList* suffixList = Platform::current()->publicSuffixList();
        if (!suffixList)
            return;

        size_t publicSuffixLength = suffixList->getPublicSuffixLength(m_host);
        if (m_host.length() <= publicSuffixLength + 1) {
            m_hostIsPublicSuffix = true;
        } else if (subdomainSetting == AllowRegisterableDomains && publicSuffixLength) {
            // The "2" in the next line is 1 for the '.', plus a 1-char minimum label length.
            const size_t dot = m_host.reverseFind('.', m_host.length() - publicSuffixLength - 2);
            if (dot == kNotFound)
                m_registerableDomain = host;
            else
                m_registerableDomain = host.substring(dot + 1);
        }
    }
}

OriginAccessEntry::MatchResult OriginAccessEntry::matchesOrigin(const SecurityOrigin& origin) const
{
    ASSERT(origin.host() == origin.host().lower());
    ASSERT(origin.protocol() == origin.protocol().lower());

    if (m_protocol != origin.protocol())
        return DoesNotMatchOrigin;

    // Special case: Include subdomains and empty host means "all hosts, including ip addresses".
    if (m_subdomainSettings != DisallowSubdomains && m_host.isEmpty())
        return MatchesOrigin;

    // Exact match.
    if (m_host == origin.host())
        return MatchesOrigin;

    // Don't try to do subdomain matching on IP addresses.
    if (m_hostIsIPAddress)
        return DoesNotMatchOrigin;

    // Match subdomains.
    switch (m_subdomainSettings) {
    case DisallowSubdomains:
        return DoesNotMatchOrigin;

    case AllowSubdomains:
        if (!IsSubdomainOfHost(origin.host(), m_host))
            return DoesNotMatchOrigin;
        break;

    case AllowRegisterableDomains:
        // Fall back to a simple subdomain check if no registerable domain could be found:
        if (m_registerableDomain.isEmpty()) {
            if (!IsSubdomainOfHost(origin.host(), m_host))
                return DoesNotMatchOrigin;
        } else if (m_registerableDomain != origin.host() && !IsSubdomainOfHost(origin.host(), m_registerableDomain)) {
            return DoesNotMatchOrigin;
        }
        break;
    };

    if (m_hostIsPublicSuffix)
        return MatchesOriginButIsPublicSuffix;

    return MatchesOrigin;
}

} // namespace blink
