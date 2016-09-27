// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/csp/CSPSource.h"

#include "core/frame/csp/ContentSecurityPolicy.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/KnownPorts.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/text/WTFString.h"

namespace blink {

CSPSource::CSPSource(ContentSecurityPolicy* policy, const String& scheme, const String& host, int port, const String& path, WildcardDisposition hostWildcard, WildcardDisposition portWildcard)
    : m_policy(policy)
    , m_scheme(scheme)
    , m_host(host)
    , m_port(port)
    , m_path(path)
    , m_hostWildcard(hostWildcard)
    , m_portWildcard(portWildcard)
{
}

bool CSPSource::matches(const KURL& url, ContentSecurityPolicy::RedirectStatus redirectStatus) const
{
    if (!schemeMatches(url))
        return false;
    if (isSchemeOnly())
        return true;
    bool pathsMatch = (redirectStatus == ContentSecurityPolicy::DidRedirect) || pathMatches(url);
    return hostMatches(url) && portMatches(url) && pathsMatch;
}

bool CSPSource::schemeMatches(const KURL& url) const
{
    if (m_scheme.isEmpty())
        return m_policy->protocolMatchesSelf(url);
    return equalIgnoringCase(url.protocol(), m_scheme);
}

bool CSPSource::hostMatches(const KURL& url) const
{
    const String& host = url.host();
    if (equalIgnoringCase(host, m_host))
        return true;
    return m_hostWildcard == HasWildcard && host.endsWith("." + m_host, TextCaseInsensitive);

}

bool CSPSource::pathMatches(const KURL& url) const
{
    if (m_path.isEmpty())
        return true;

    String path = decodeURLEscapeSequences(url.path());

    if (m_path.endsWith("/"))
        return path.startsWith(m_path, TextCaseInsensitive);

    return path == m_path;
}

bool CSPSource::portMatches(const KURL& url) const
{
    if (m_portWildcard == HasWildcard)
        return true;

    int port = url.port();

    if (port == m_port)
        return true;

    if (!port)
        return isDefaultPortForProtocol(m_port, url.protocol());

    if (!m_port)
        return isDefaultPortForProtocol(port, url.protocol());

    return false;
}

bool CSPSource::isSchemeOnly() const
{
    return m_host.isEmpty();
}

} // namespace
