// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/csp/SourceListDirective.h"

#include "core/frame/csp/CSPSourceList.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/WTFString.h"

namespace blink {

SourceListDirective::SourceListDirective(const String& name, const String& value, ContentSecurityPolicy* policy)
    : CSPDirective(name, value, policy)
    , m_sourceList(policy, name)
{
    Vector<UChar> characters;
    value.appendTo(characters);

    m_sourceList.parse(characters.data(), characters.data() + characters.size());
}

bool SourceListDirective::allows(const KURL& url, ContentSecurityPolicy::RedirectStatus redirectStatus) const
{
    return m_sourceList.matches(url.isEmpty() ? policy()->url() : url, redirectStatus);
}

bool SourceListDirective::allowInline() const
{
    return m_sourceList.allowInline();
}

bool SourceListDirective::allowEval() const
{
    return m_sourceList.allowEval();
}

bool SourceListDirective::allowNonce(const String& nonce) const
{
    return m_sourceList.allowNonce(nonce.stripWhiteSpace());
}

bool SourceListDirective::allowHash(const CSPHashValue& hashValue) const
{
    return m_sourceList.allowHash(hashValue);
}

bool SourceListDirective::isHashOrNoncePresent() const
{
    return m_sourceList.isHashOrNoncePresent();
}

uint8_t SourceListDirective::hashAlgorithmsUsed() const
{
    return m_sourceList.hashAlgorithmsUsed();
}

} // namespace blink
