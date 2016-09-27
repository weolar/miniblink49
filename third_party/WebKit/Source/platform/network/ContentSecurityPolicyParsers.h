// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ContentSecurityPolicyParsers_h
#define ContentSecurityPolicyParsers_h

#include "platform/Crypto.h"
#include "platform/PlatformExport.h"
#include "wtf/text/Unicode.h"

namespace WTF {

class StringUTF8Adaptor;

}

namespace blink {

typedef std::pair<unsigned, DigestValue> CSPHashValue;

enum ContentSecurityPolicyHeaderType {
    ContentSecurityPolicyHeaderTypeReport,
    ContentSecurityPolicyHeaderTypeEnforce
};

enum ContentSecurityPolicyHeaderSource {
    ContentSecurityPolicyHeaderSourceHTTP,
    ContentSecurityPolicyHeaderSourceMeta
};

enum ContentSecurityPolicyHashAlgorithm {
    ContentSecurityPolicyHashAlgorithmNone = 0,
    ContentSecurityPolicyHashAlgorithmSha1 = 1 << 1,
    ContentSecurityPolicyHashAlgorithmSha256 = 1 << 2,
    ContentSecurityPolicyHashAlgorithmSha384 = 1 << 3,
    ContentSecurityPolicyHashAlgorithmSha512 = 1 << 4
};

PLATFORM_EXPORT bool isCSPDirectiveNameCharacter(UChar);
PLATFORM_EXPORT bool isCSPDirectiveValueCharacter(UChar);
PLATFORM_EXPORT bool isNonceCharacter(UChar);
PLATFORM_EXPORT bool isSourceCharacter(UChar);
PLATFORM_EXPORT bool isPathComponentCharacter(UChar);
PLATFORM_EXPORT bool isHostCharacter(UChar);
PLATFORM_EXPORT bool isSchemeContinuationCharacter(UChar);
PLATFORM_EXPORT bool isNotASCIISpace(UChar);
PLATFORM_EXPORT bool isNotColonOrSlash(UChar);
PLATFORM_EXPORT bool isMediaTypeCharacter(UChar);

// Only checks for general Base64 encoded chars, not '=' chars since '=' is
// positional and may only appear at the end of a Base64 encoded string.
PLATFORM_EXPORT bool isBase64EncodedCharacter(UChar);

// Normalize script or style source for script hash use.
PLATFORM_EXPORT WTF::StringUTF8Adaptor normalizeSource(const String& source);

} // namespace blink

#endif
