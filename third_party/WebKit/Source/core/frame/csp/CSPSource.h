// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSPSource_h
#define CSPSource_h

#include "core/CoreExport.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ContentSecurityPolicy;
class KURL;

class CORE_EXPORT CSPSource {
public:
    enum WildcardDisposition {
        HasWildcard,
        NoWildcard
    };

    CSPSource(ContentSecurityPolicy*, const String& scheme, const String& host, int port, const String& path, WildcardDisposition hostWildcard, WildcardDisposition portWildcard);
    bool matches(const KURL&, ContentSecurityPolicy::RedirectStatus = ContentSecurityPolicy::DidNotRedirect) const;

private:
    bool schemeMatches(const KURL&) const;
    bool hostMatches(const KURL&) const;
    bool pathMatches(const KURL&) const;
    bool portMatches(const KURL&) const;
    bool isSchemeOnly() const;

    ContentSecurityPolicy* m_policy;
    String m_scheme;
    String m_host;
    int m_port;
    String m_path;

    WildcardDisposition m_hostWildcard;
    WildcardDisposition m_portWildcard;
};

} // namespace

#endif
