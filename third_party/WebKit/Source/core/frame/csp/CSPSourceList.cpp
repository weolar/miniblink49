// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/csp/CSPSourceList.h"

#include "core/frame/csp/CSPSource.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "platform/ParsingUtilities.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/HashSet.h"
#include "wtf/text/Base64.h"
#include "wtf/text/WTFString.h"

namespace blink {

static bool isSourceListNone(const UChar* begin, const UChar* end)
{
    skipWhile<UChar, isASCIISpace>(begin, end);

    const UChar* position = begin;
    skipWhile<UChar, isSourceCharacter>(position, end);
    if (!equalIgnoringCase("'none'", begin, position - begin))
        return false;

    skipWhile<UChar, isASCIISpace>(position, end);
    if (position != end)
        return false;

    return true;
}

CSPSourceList::CSPSourceList(ContentSecurityPolicy* policy, const String& directiveName)
    : m_policy(policy)
    , m_directiveName(directiveName)
    , m_allowSelf(false)
    , m_allowStar(false)
    , m_allowInline(false)
    , m_allowEval(false)
    , m_hashAlgorithmsUsed(0)
{
}

bool CSPSourceList::matches(const KURL& url, ContentSecurityPolicy::RedirectStatus redirectStatus) const
{
    if (m_allowStar)
        return true;

    KURL effectiveURL = m_policy->selfMatchesInnerURL() && SecurityOrigin::shouldUseInnerURL(url) ? SecurityOrigin::extractInnerURL(url) : url;

    if (m_allowSelf && m_policy->urlMatchesSelf(effectiveURL))
        return true;

    for (size_t i = 0; i < m_list.size(); ++i) {
        if (m_list[i].matches(effectiveURL, redirectStatus))
            return true;
    }

    return false;
}

bool CSPSourceList::allowInline() const
{
    return m_allowInline;
}

bool CSPSourceList::allowEval() const
{
    return m_allowEval;
}

bool CSPSourceList::allowNonce(const String& nonce) const
{
    return !nonce.isNull() && m_nonces.contains(nonce);
}

bool CSPSourceList::allowHash(const CSPHashValue& hashValue) const
{
    return m_hashes.contains(hashValue);
}

uint8_t CSPSourceList::hashAlgorithmsUsed() const
{
    return m_hashAlgorithmsUsed;
}

bool CSPSourceList::isHashOrNoncePresent() const
{
    return !m_nonces.isEmpty() || m_hashAlgorithmsUsed != ContentSecurityPolicyHashAlgorithmNone;
}

// source-list       = *WSP [ source *( 1*WSP source ) *WSP ]
//                   / *WSP "'none'" *WSP
//
void CSPSourceList::parse(const UChar* begin, const UChar* end)
{
    // We represent 'none' as an empty m_list.
    if (isSourceListNone(begin, end))
        return;

    const UChar* position = begin;
    while (position < end) {
        skipWhile<UChar, isASCIISpace>(position, end);
        if (position == end)
            return;

        const UChar* beginSource = position;
        skipWhile<UChar, isSourceCharacter>(position, end);

        String scheme, host, path;
        int port = 0;
        CSPSource::WildcardDisposition hostWildcard = CSPSource::NoWildcard;
        CSPSource::WildcardDisposition portWildcard = CSPSource::NoWildcard;

        if (parseSource(beginSource, position, scheme, host, port, path, hostWildcard, portWildcard)) {
            // Wildcard hosts and keyword sources ('self', 'unsafe-inline',
            // etc.) aren't stored in m_list, but as attributes on the source
            // list itself.
            if (scheme.isEmpty() && host.isEmpty())
                continue;
            if (m_policy->isDirectiveName(host))
                m_policy->reportDirectiveAsSourceExpression(m_directiveName, host);
            m_list.append(CSPSource(m_policy, scheme, host, port, path, hostWildcard, portWildcard));
        } else {
            m_policy->reportInvalidSourceExpression(m_directiveName, String(beginSource, position - beginSource));
        }

        ASSERT(position == end || isASCIISpace(*position));
    }
}

// source            = scheme ":"
//                   / ( [ scheme "://" ] host [ port ] [ path ] )
//                   / "'self'"
bool CSPSourceList::parseSource(const UChar* begin, const UChar* end, String& scheme, String& host, int& port, String& path, CSPSource::WildcardDisposition& hostWildcard, CSPSource::WildcardDisposition& portWildcard)
{
    if (begin == end)
        return false;

    if (equalIgnoringCase("'none'", begin, end - begin))
        return false;

    if (end - begin == 1 && *begin == '*') {
        addSourceStar();
        return true;
    }

    if (equalIgnoringCase("'self'", begin, end - begin)) {
        addSourceSelf();
        return true;
    }

    if (equalIgnoringCase("'unsafe-inline'", begin, end - begin)) {
        addSourceUnsafeInline();
        return true;
    }

    if (equalIgnoringCase("'unsafe-eval'", begin, end - begin)) {
        addSourceUnsafeEval();
        return true;
    }

    String nonce;
    if (!parseNonce(begin, end, nonce))
        return false;

    if (!nonce.isNull()) {
        addSourceNonce(nonce);
        return true;
    }

    DigestValue hash;
    ContentSecurityPolicyHashAlgorithm algorithm = ContentSecurityPolicyHashAlgorithmNone;
    if (!parseHash(begin, end, hash, algorithm))
        return false;

    if (hash.size() > 0) {
        addSourceHash(algorithm, hash);
        return true;
    }

    const UChar* position = begin;
    const UChar* beginHost = begin;
    const UChar* beginPath = end;
    const UChar* beginPort = 0;

    skipWhile<UChar, isNotColonOrSlash>(position, end);

    if (position == end) {
        // host
        //     ^
        return parseHost(beginHost, position, host, hostWildcard);
    }

    if (position < end && *position == '/') {
        // host/path || host/ || /
        //     ^            ^    ^
        return parseHost(beginHost, position, host, hostWildcard) && parsePath(position, end, path);
    }

    if (position < end && *position == ':') {
        if (end - position == 1) {
            // scheme:
            //       ^
            return parseScheme(begin, position, scheme);
        }

        if (position[1] == '/') {
            // scheme://host || scheme://
            //       ^                ^
            if (!parseScheme(begin, position, scheme)
                || !skipExactly<UChar>(position, end, ':')
                || !skipExactly<UChar>(position, end, '/')
                || !skipExactly<UChar>(position, end, '/'))
                return false;
            if (position == end)
                return false;
            beginHost = position;
            skipWhile<UChar, isNotColonOrSlash>(position, end);
        }

        if (position < end && *position == ':') {
            // host:port || scheme://host:port
            //     ^                     ^
            beginPort = position;
            skipUntil<UChar>(position, end, '/');
        }
    }

    if (position < end && *position == '/') {
        // scheme://host/path || scheme://host:port/path
        //              ^                          ^
        if (position == beginHost)
            return false;
        beginPath = position;
    }

    if (!parseHost(beginHost, beginPort ? beginPort : beginPath, host, hostWildcard))
        return false;

    if (beginPort) {
        if (!parsePort(beginPort, beginPath, port, portWildcard))
            return false;
    } else {
        port = 0;
    }

    if (beginPath != end) {
        if (!parsePath(beginPath, end, path))
            return false;
    }

    return true;
}

// nonce-source      = "'nonce-" nonce-value "'"
// nonce-value        = 1*( ALPHA / DIGIT / "+" / "/" / "=" )
//
bool CSPSourceList::parseNonce(const UChar* begin, const UChar* end, String& nonce)
{
    size_t nonceLength = end - begin;
    const char* prefix = "'nonce-";

    if (nonceLength <= strlen(prefix) || !equalIgnoringCase(prefix, begin, strlen(prefix)))
        return true;

    const UChar* position = begin + strlen(prefix);
    const UChar* nonceBegin = position;

    ASSERT(position < end);
    skipWhile<UChar, isNonceCharacter>(position, end);
    ASSERT(nonceBegin <= position);

    if (position + 1 != end || *position != '\'' || position == nonceBegin)
        return false;

    nonce = String(nonceBegin, position - nonceBegin);
    return true;
}

// hash-source       = "'" hash-algorithm "-" hash-value "'"
// hash-algorithm    = "sha1" / "sha256" / "sha384" / "sha512"
// hash-value        = 1*( ALPHA / DIGIT / "+" / "/" / "=" )
//
bool CSPSourceList::parseHash(const UChar* begin, const UChar* end, DigestValue& hash, ContentSecurityPolicyHashAlgorithm& hashAlgorithm)
{
    // Any additions or subtractions from this struct should also modify the
    // respective entries in the kAlgorithmMap array in checkDigest().
    static const struct {
        const char* prefix;
        ContentSecurityPolicyHashAlgorithm type;
    } kSupportedPrefixes[] = {
        // FIXME: Drop support for SHA-1. It's not in the spec.
        { "'sha1-", ContentSecurityPolicyHashAlgorithmSha1 },
        { "'sha256-", ContentSecurityPolicyHashAlgorithmSha256 },
        { "'sha384-", ContentSecurityPolicyHashAlgorithmSha384 },
        { "'sha512-", ContentSecurityPolicyHashAlgorithmSha512 },
        { "'sha-256-", ContentSecurityPolicyHashAlgorithmSha256 },
        { "'sha-384-", ContentSecurityPolicyHashAlgorithmSha384 },
        { "'sha-512-", ContentSecurityPolicyHashAlgorithmSha512 }
    };

    String prefix;
    hashAlgorithm = ContentSecurityPolicyHashAlgorithmNone;
    size_t hashLength = end - begin;

    for (const auto& algorithm : kSupportedPrefixes) {
        if (hashLength > strlen(algorithm.prefix) && equalIgnoringCase(algorithm.prefix, begin, strlen(algorithm.prefix))) {
            prefix = algorithm.prefix;
            hashAlgorithm = algorithm.type;
            break;
        }
    }

    if (hashAlgorithm == ContentSecurityPolicyHashAlgorithmNone)
        return true;

    const UChar* position = begin + prefix.length();
    const UChar* hashBegin = position;

    ASSERT(position < end);
    skipWhile<UChar, isBase64EncodedCharacter>(position, end);
    ASSERT(hashBegin <= position);

    // Base64 encodings may end with exactly one or two '=' characters
    if (position < end)
        skipExactly<UChar>(position, position + 1, '=');
    if (position < end)
        skipExactly<UChar>(position, position + 1, '=');

    if (position + 1 != end || *position != '\'' || position == hashBegin)
        return false;

    Vector<char> hashVector;
    // We accept base64url-encoded data here by normalizing it to base64.
    base64Decode(normalizeToBase64(String(hashBegin, position - hashBegin)), hashVector);
    if (hashVector.size() > kMaxDigestSize)
        return false;
    hash.append(reinterpret_cast<uint8_t*>(hashVector.data()), hashVector.size());
    return true;
}

//                     ; <scheme> production from RFC 3986
// scheme      = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
//
bool CSPSourceList::parseScheme(const UChar* begin, const UChar* end, String& scheme)
{
    ASSERT(begin <= end);
    ASSERT(scheme.isEmpty());

    if (begin == end)
        return false;

    const UChar* position = begin;

    if (!skipExactly<UChar, isASCIIAlpha>(position, end))
        return false;

    skipWhile<UChar, isSchemeContinuationCharacter>(position, end);

    if (position != end)
        return false;

    scheme = String(begin, end - begin);
    return true;
}

// host              = [ "*." ] 1*host-char *( "." 1*host-char )
//                   / "*"
// host-char         = ALPHA / DIGIT / "-"
//
bool CSPSourceList::parseHost(const UChar* begin, const UChar* end, String& host, CSPSource::WildcardDisposition& hostWildcard)
{
    ASSERT(begin <= end);
    ASSERT(host.isEmpty());
    ASSERT(hostWildcard == CSPSource::NoWildcard);

    if (begin == end)
        return false;

    const UChar* position = begin;

    if (skipExactly<UChar>(position, end, '*')) {
        hostWildcard = CSPSource::HasWildcard;

        if (position == end)
            return true;

        if (!skipExactly<UChar>(position, end, '.'))
            return false;
    }

    const UChar* hostBegin = position;

    while (position < end) {
        if (!skipExactly<UChar, isHostCharacter>(position, end))
            return false;

        skipWhile<UChar, isHostCharacter>(position, end);

        if (position < end && !skipExactly<UChar>(position, end, '.'))
            return false;
    }

    ASSERT(position == end);
    host = String(hostBegin, end - hostBegin);
    return true;
}

bool CSPSourceList::parsePath(const UChar* begin, const UChar* end, String& path)
{
    ASSERT(begin <= end);
    ASSERT(path.isEmpty());

    const UChar* position = begin;
    skipWhile<UChar, isPathComponentCharacter>(position, end);
    // path/to/file.js?query=string || path/to/file.js#anchor
    //                ^                               ^
    if (position < end)
        m_policy->reportInvalidPathCharacter(m_directiveName, String(begin, end - begin), *position);

    path = decodeURLEscapeSequences(String(begin, position - begin));

    ASSERT(position <= end);
    ASSERT(position == end || (*position == '#' || *position == '?'));
    return true;
}

// port              = ":" ( 1*DIGIT / "*" )
//
bool CSPSourceList::parsePort(const UChar* begin, const UChar* end, int& port, CSPSource::WildcardDisposition& portWildcard)
{
    ASSERT(begin <= end);
    ASSERT(!port);
    ASSERT(portWildcard == CSPSource::NoWildcard);

    if (!skipExactly<UChar>(begin, end, ':'))
        ASSERT_NOT_REACHED();

    if (begin == end)
        return false;

    if (end - begin == 1 && *begin == '*') {
        port = 0;
        portWildcard = CSPSource::HasWildcard;
        return true;
    }

    const UChar* position = begin;
    skipWhile<UChar, isASCIIDigit>(position, end);

    if (position != end)
        return false;

    bool ok;
    port = charactersToIntStrict(begin, end - begin, &ok);
    return ok;
}

void CSPSourceList::addSourceSelf()
{
    m_allowSelf = true;
}

void CSPSourceList::addSourceStar()
{
    m_allowStar = true;
}

void CSPSourceList::addSourceUnsafeInline()
{
    m_allowInline = true;
}

void CSPSourceList::addSourceUnsafeEval()
{
    m_allowEval = true;
}

void CSPSourceList::addSourceNonce(const String& nonce)
{
    m_nonces.add(nonce);
}

void CSPSourceList::addSourceHash(const ContentSecurityPolicyHashAlgorithm& algorithm, const DigestValue& hash)
{
    m_hashes.add(CSPHashValue(algorithm, hash));
    m_hashAlgorithmsUsed |= algorithm;
}


} // namespace blink
