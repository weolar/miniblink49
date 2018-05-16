/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
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
#include "platform/weborigin/SecurityOrigin.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/KnownPorts.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "platform/weborigin/SecurityOriginCache.h"
#include "platform/weborigin/SecurityPolicy.h"
//#include "url/url_canon_ip.h"
#include "wtf/HexNumber.h"
#include "wtf/MainThread.h"
#include "wtf/NotFound.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

const int InvalidPort = 0;
const int MaxAllowedPort = 65535;

static SecurityOriginCache* s_originCache = 0;

static bool schemeRequiresAuthority(const KURL& url)
{
    // We expect URLs with these schemes to have authority components. If the
    // URL lacks an authority component, we get concerned and mark the origin
    // as unique.
    return url.protocolIsInHTTPFamily() || url.protocolIs("ftp");
}

static SecurityOrigin* cachedOrigin(const KURL& url)
{
    if (s_originCache)
        return s_originCache->cachedOrigin(url);
    return 0;
}

bool SecurityOrigin::shouldUseInnerURL(const KURL& url)
{
    // FIXME: Blob URLs don't have inner URLs. Their form is "blob:<inner-origin>/<UUID>", so treating the part after "blob:" as a URL is incorrect.
    if (url.protocolIs("blob"))
        return true;
    if (url.protocolIs("filesystem"))
        return true;
    return false;
}

// In general, extracting the inner URL varies by scheme. It just so happens
// that all the URL schemes we currently support that use inner URLs for their
// security origin can be parsed using this algorithm.
KURL SecurityOrigin::extractInnerURL(const KURL& url)
{
    if (url.innerURL())
        return *url.innerURL();
    // FIXME: Update this callsite to use the innerURL member function when
    // we finish implementing it.
    return KURL(ParsedURLString, decodeURLEscapeSequences(url.path()));
}

void SecurityOrigin::setCache(SecurityOriginCache* originCache)
{
    s_originCache = originCache;
}

static bool shouldTreatAsUniqueOrigin(const KURL& url)
{
    if (!url.isValid())
        return true;

    // FIXME: Do we need to unwrap the URL further?
    KURL innerURL = SecurityOrigin::shouldUseInnerURL(url) ? SecurityOrigin::extractInnerURL(url) : url;

    // FIXME: Check whether innerURL is valid.

    // For edge case URLs that were probably misparsed, make sure that the origin is unique.
    // FIXME: Do we really need to do this? This looks to be a hack around a
    // security bug in CFNetwork that might have been fixed.
    if (schemeRequiresAuthority(innerURL) && innerURL.host().isEmpty())
        return true;

    // SchemeRegistry needs a lower case protocol because it uses HashMaps
    // that assume the scheme has already been canonicalized.
    String protocol = innerURL.protocol().lower();

    if (SchemeRegistry::shouldTreatURLSchemeAsNoAccess(protocol))
        return true;

    // This is the common case.
    return false;
}

SecurityOrigin::SecurityOrigin(const KURL& url)
    : m_protocol(url.protocol().isNull() ? "" : url.protocol().lower())
    , m_host(url.host().isNull() ? "" : url.host().lower())
    , m_port(url.port())
    , m_isUnique(false)
    , m_universalAccess(false)
    , m_domainWasSetInDOM(false)
    , m_enforceFilePathSeparation(false)
    , m_needsDatabaseIdentifierQuirkForFiles(false)
{
    // Suborigins are serialized into the host, so extract it if necessary.
    String suboriginName;
    if (deserializeSuboriginAndHost(m_host, suboriginName, m_host))
        addSuborigin(suboriginName);

    // document.domain starts as m_host, but can be set by the DOM.
    m_domain = m_host;

    if (isDefaultPortForProtocol(m_port, m_protocol))
        m_port = InvalidPort;

    // By default, only local SecurityOrigins can load local resources.
    m_canLoadLocalResources = isLocal();
}

SecurityOrigin::SecurityOrigin()
    : m_protocol("")
    , m_host("")
    , m_domain("")
    , m_suboriginName(WTF::String())
    , m_port(InvalidPort)
    , m_isUnique(true)
    , m_universalAccess(false)
    , m_domainWasSetInDOM(false)
    , m_canLoadLocalResources(false)
    , m_enforceFilePathSeparation(false)
    , m_needsDatabaseIdentifierQuirkForFiles(false)
{
}

SecurityOrigin::SecurityOrigin(const SecurityOrigin* other)
    : m_protocol(other->m_protocol.isolatedCopy())
    , m_host(other->m_host.isolatedCopy())
    , m_domain(other->m_domain.isolatedCopy())
    , m_suboriginName(other->m_suboriginName)
    , m_port(other->m_port)
    , m_isUnique(other->m_isUnique)
    , m_universalAccess(other->m_universalAccess)
    , m_domainWasSetInDOM(other->m_domainWasSetInDOM)
    , m_canLoadLocalResources(other->m_canLoadLocalResources)
    , m_enforceFilePathSeparation(other->m_enforceFilePathSeparation)
    , m_needsDatabaseIdentifierQuirkForFiles(other->m_needsDatabaseIdentifierQuirkForFiles)
{
}

PassRefPtr<SecurityOrigin> SecurityOrigin::create(const KURL& url)
{
    if (RefPtr<SecurityOrigin> origin = cachedOrigin(url))
        return origin.release();

    if (shouldTreatAsUniqueOrigin(url)) {
        RefPtr<SecurityOrigin> origin = adoptRef(new SecurityOrigin());

        if (url.protocolIs("file")) {
            // Unfortunately, we can't represent all unique origins exactly
            // the same way because we need to produce a quirky database
            // identifier for file URLs due to persistent storage in some
            // embedders of WebKit.
            origin->m_needsDatabaseIdentifierQuirkForFiles = true;
        }

        return origin.release();
    }

    if (shouldUseInnerURL(url))
        return adoptRef(new SecurityOrigin(extractInnerURL(url)));

    return adoptRef(new SecurityOrigin(url));
}

PassRefPtr<SecurityOrigin> SecurityOrigin::createUnique()
{
    RefPtr<SecurityOrigin> origin = adoptRef(new SecurityOrigin());
    ASSERT(origin->isUnique());
    return origin.release();
}

void SecurityOrigin::addSuborigin(const String& suborigin)
{
    ASSERT(RuntimeEnabledFeatures::suboriginsEnabled());
    // Changing suborigins midstream is bad. Very bad. It should not happen.
    // This is, in fact, one of the very basic invariants that makes suborigins
    // an effective security tool.
    RELEASE_ASSERT(m_suboriginName.isNull() || m_suboriginName == suborigin);
    m_suboriginName = suborigin;
}

PassRefPtr<SecurityOrigin> SecurityOrigin::isolatedCopy() const
{
    return adoptRef(new SecurityOrigin(this));
}

void SecurityOrigin::setDomainFromDOM(const String& newDomain)
{
    m_domainWasSetInDOM = true;
    m_domain = newDomain.lower();
}

bool SecurityOrigin::isSecure(const KURL& url)
{
    if (SchemeRegistry::shouldTreatURLSchemeAsSecure(url.protocol()))
        return true;

    // URLs that wrap inner URLs are secure if those inner URLs are secure.
    if (shouldUseInnerURL(url) && SchemeRegistry::shouldTreatURLSchemeAsSecure(extractInnerURL(url).protocol()))
        return true;

    return false;
}

bool SecurityOrigin::canAccess(const SecurityOrigin* other) const
{
    if (!RuntimeEnabledFeatures::cspCheckEnabled())
        return true;

    if (m_universalAccess)
        return true;

    if (this == other)
        return true;

    if (isUnique() || other->isUnique())
        return false;

    // document.domain handling, as per https://html.spec.whatwg.org/multipage/browsers.html#dom-document-domain:
    //
    // 1) Neither document has set document.domain. In this case, we insist
    //    that the scheme, host, and port of the URLs match.
    //
    // 2) Both documents have set document.domain. In this case, we insist
    //    that the documents have set document.domain to the same value and
    //    that the scheme of the URLs match. Ports do not need to match.
    bool canAccess = false;
    if (m_protocol == other->m_protocol) {
        if (!m_domainWasSetInDOM && !other->m_domainWasSetInDOM) {
            if (m_host == other->m_host && m_port == other->m_port)
                canAccess = true;
        } else if (m_domainWasSetInDOM && other->m_domainWasSetInDOM) {
            if (m_domain == other->m_domain)
                canAccess = true;
        }
    }

    if (canAccess && isLocal())
        canAccess = passesFileCheck(other);

    return canAccess;
}

bool SecurityOrigin::canAccessCheckSuborigins(const SecurityOrigin* other) const
{
    if (!RuntimeEnabledFeatures::cspCheckEnabled())
        return true;

    if (hasSuborigin() != other->hasSuborigin())
        return false;

    if (hasSuborigin() && suboriginName() != other->suboriginName())
        return false;

    return canAccess(other);
}

bool SecurityOrigin::passesFileCheck(const SecurityOrigin* other) const
{
    ASSERT(isLocal() && other->isLocal());

    return !m_enforceFilePathSeparation && !other->m_enforceFilePathSeparation;
}

bool SecurityOrigin::canRequest(const KURL& url) const
{
    if (!RuntimeEnabledFeatures::cspCheckEnabled())
        return true;

    if (m_universalAccess)
        return true;

    if (cachedOrigin(url) == this)
        return true;

    if (isUnique())
        return false;

    RefPtr<SecurityOrigin> targetOrigin = SecurityOrigin::create(url);

    if (targetOrigin->isUnique())
        return false;

    // We call isSameSchemeHostPort here instead of canAccess because we want
    // to ignore document.domain effects.
    if (isSameSchemeHostPort(targetOrigin.get()))
        return true;

    if (SecurityPolicy::isAccessWhiteListed(this, targetOrigin.get()))
        return true;

    return false;
}

bool SecurityOrigin::canRequestNoSuborigin(const KURL& url) const
{
    if (!RuntimeEnabledFeatures::cspCheckEnabled())
        return true;

    return !hasSuborigin() && canRequest(url);
}

bool SecurityOrigin::taintsCanvas(const KURL& url) const
{
    if (canRequest(url))
        return false;

    // This function exists because we treat data URLs as having a unique origin,
    // contrary to the current (9/19/2009) draft of the HTML5 specification.
    // We still want to let folks paint data URLs onto untainted canvases, so
    // we special case data URLs below. If we change to match HTML5 w.r.t.
    // data URL security, then we can remove this function in favor of
    // !canRequest.
    if (url.protocolIsData())
        return false;

    return true;
}

bool SecurityOrigin::canDisplay(const KURL& url) const
{
    if (!RuntimeEnabledFeatures::cspCheckEnabled())
        return true;

    if (m_universalAccess)
        return true;

    String protocol = url.protocol().lower();

    if (SchemeRegistry::canDisplayOnlyIfCanRequest(protocol))
        return canRequest(url);

    if (SchemeRegistry::shouldTreatURLSchemeAsDisplayIsolated(protocol))
        return m_protocol == protocol || SecurityPolicy::isAccessToURLWhiteListed(this, url);

    if (SchemeRegistry::shouldTreatURLSchemeAsLocal(protocol))
        return canLoadLocalResources() || SecurityPolicy::isAccessToURLWhiteListed(this, url);

    return true;
}

bool SecurityOrigin::isPotentiallyTrustworthy(String& errorMessage) const
{
    ASSERT(m_protocol != "data");
    if (SchemeRegistry::shouldTreatURLSchemeAsSecure(m_protocol) || isLocal() || isLocalhost())
        return true;

    if (SecurityPolicy::isOriginWhiteListedTrustworthy(*this))
        return true;

    errorMessage = "Only secure origins are allowed (see: https://goo.gl/Y0ZkNV).";
    return false;
}

void SecurityOrigin::grantLoadLocalResources()
{
    // Granting privileges to some, but not all, documents in a SecurityOrigin
    // is a security hazard because the documents without the privilege can
    // obtain the privilege by injecting script into the documents that have
    // been granted the privilege.
    m_canLoadLocalResources = true;
}

void SecurityOrigin::grantUniversalAccess()
{
    m_universalAccess = true;
}

void SecurityOrigin::enforceFilePathSeparation()
{
    ASSERT(isLocal());
    m_enforceFilePathSeparation = true;
}

bool SecurityOrigin::isLocal() const
{
    return SchemeRegistry::shouldTreatURLSchemeAsLocal(m_protocol);
}

bool SecurityOrigin::isLocalhost() const
{
    if (m_host == "localhost")
        return true;

    if (m_host == "[::1]")
        return true;

    // Test if m_host matches 127.0.0.1/8
    ASSERT(m_host.containsOnlyASCII());
    CString hostAscii = m_host.ascii();
    Vector<uint8, 4> ipNumber;
    ipNumber.resize(4);

#ifdef MINIBLINK_NOT_IMPLEMENTED
    int numComponents;
    url::Component hostComponent(0, hostAscii.length());
    url::CanonHostInfo::Family family = url::IPv4AddressToNumber(
        hostAscii.data(), hostComponent, &(ipNumber)[0], &numComponents);
    if (family != url::CanonHostInfo::IPV4)
        return false;
    return ipNumber[0] == 127;
#else
    size_t pos0 = 0;
    size_t pos1 = 0;
    for (; pos0 < hostAscii.length(); pos0 = pos1 + 1) {
        for (pos1 = pos0 + 1; pos1 < hostAscii.length(); ++pos1) {
            char c = hostAscii.data()[pos1];
            if (isASCIIAlphanumeric(c))
                continue;

            if ('.' == hostAscii.data()[pos1]) {
                unsigned ip = 0;
                String temp(hostAscii.data() + pos0, pos1 - pos0);

                bool ok = false;
                ip = temp.toUIntStrict(&ok);
                if (!ok)
                    return false;
                if (ip > 255)
                    return false;
                ipNumber.append(ip);
                break;
            } else
                return false;
        }
    }

    return ipNumber.size() == 4 && ipNumber[0] == 127;
#endif // MINIBLINK_NOT_IMPLEMENTED
}

String SecurityOrigin::toString() const
{
    if (isUnique())
        return "null";
    if (m_protocol == "file" && m_enforceFilePathSeparation)
        return "null";
    return toRawString();
}

AtomicString SecurityOrigin::toAtomicString() const
{
    if (isUnique())
        return AtomicString("null", AtomicString::ConstructFromLiteral);
    if (m_protocol == "file" && m_enforceFilePathSeparation)
        return AtomicString("null", AtomicString::ConstructFromLiteral);
    return toRawAtomicString();
}

String SecurityOrigin::toRawString() const
{
    if (m_protocol == "file")
        return "file://";

    StringBuilder result;
    buildRawString(result);
    return result.toString();
}

// Returns true if and only if a suborigin component was found. If false, no
// guarantees about the return value |suboriginName| are made.
bool SecurityOrigin::deserializeSuboriginAndHost(const String& oldHost, String& suboriginName, String& newHost)
{
    if (!RuntimeEnabledFeatures::suboriginsEnabled())
        return false;

    size_t suboriginEnd = oldHost.find('_');
    // Suborigins cannot be empty
    if (suboriginEnd == 0 || suboriginEnd == WTF::kNotFound)
        return false;

    suboriginName = oldHost.substring(0, suboriginEnd);
    newHost = oldHost.substring(suboriginEnd + 1);

    return true;
}


AtomicString SecurityOrigin::toRawAtomicString() const
{
    if (m_protocol == "file")
        return AtomicString("file://", AtomicString::ConstructFromLiteral);

    StringBuilder result;
    buildRawString(result);
    return result.toAtomicString();
}

void SecurityOrigin::buildRawString(StringBuilder& builder) const
{
    builder.append(m_protocol);
    builder.appendLiteral("://");
    if (hasSuborigin()) {
        builder.append(m_suboriginName);
        builder.appendLiteral("_");
    }
    builder.append(m_host);

    if (m_port) {
        builder.append(':');
        builder.appendNumber(m_port);
    }
}

PassRefPtr<SecurityOrigin> SecurityOrigin::createFromString(const String& originString)
{
    return SecurityOrigin::create(KURL(KURL(), originString));
}

PassRefPtr<SecurityOrigin> SecurityOrigin::create(const String& protocol, const String& host, int port)
{
    if (port < 0 || port > MaxAllowedPort)
        return createUnique();
    String decodedHost = decodeURLEscapeSequences(host);
    return create(KURL(KURL(), protocol + "://" + host + ":" + String::number(port) + "/"));
}

bool SecurityOrigin::isSameSchemeHostPort(const SecurityOrigin* other) const
{
    if (m_host != other->m_host)
        return false;

    if (m_protocol != other->m_protocol)
        return false;

    if (m_port != other->m_port)
        return false;

    if (isLocal() && !passesFileCheck(other))
        return false;

    return true;
}

bool SecurityOrigin::isSameSchemeHostPortAndSuborigin(const SecurityOrigin* other) const
{
    return isSameSchemeHostPort(other) && (!hasSuborigin() || suboriginName() == other->suboriginName());
}

const KURL& SecurityOrigin::urlWithUniqueSecurityOrigin()
{
    ASSERT(isMainThread());
    DEFINE_STATIC_LOCAL(const KURL, uniqueSecurityOriginURL, (ParsedURLString, "data:,"));
    return uniqueSecurityOriginURL;
}

void SecurityOrigin::transferPrivilegesFrom(const SecurityOrigin& origin)
{
    m_universalAccess = origin.m_universalAccess;
    m_canLoadLocalResources = origin.m_canLoadLocalResources;
    m_enforceFilePathSeparation = origin.m_enforceFilePathSeparation;
}

} // namespace blink
