// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/network/ContentSecurityPolicyParsers.h"

#include "wtf/ASCIICType.h"
#include "wtf/text/StringUTF8Adaptor.h"

namespace blink {

bool isCSPDirectiveNameCharacter(UChar c)
{
    return isASCIIAlphanumeric(c) || c == '-';
}

bool isCSPDirectiveValueCharacter(UChar c)
{
    return isASCIISpace(c) || (c >= 0x21 && c <= 0x7e); // Whitespace + VCHAR
}

// Only checks for general Base64(url) encoded chars, not '=' chars since '=' is
// positional and may only appear at the end of a Base64 encoded string.
bool isBase64EncodedCharacter(UChar c)
{
    return isASCIIAlphanumeric(c) || c == '+' || c == '/' || c == '-' || c == '_';
}

bool isNonceCharacter(UChar c)
{
    return isBase64EncodedCharacter(c) || c == '=';
}

bool isSourceCharacter(UChar c)
{
    return !isASCIISpace(c);
}

bool isPathComponentCharacter(UChar c)
{
    return c != '?' && c != '#';
}

bool isHostCharacter(UChar c)
{
    return isASCIIAlphanumeric(c) || c == '-';
}

bool isSchemeContinuationCharacter(UChar c)
{
    return isASCIIAlphanumeric(c) || c == '+' || c == '-' || c == '.';
}

bool isNotASCIISpace(UChar c)
{
    return !isASCIISpace(c);
}

bool isNotColonOrSlash(UChar c)
{
    return c != ':' && c != '/';
}

bool isMediaTypeCharacter(UChar c)
{
    return !isASCIISpace(c) && c != '/';
}

WTF::StringUTF8Adaptor normalizeSource(const String& source)
{
    return WTF::StringUTF8Adaptor(source, WTF::StringUTF8Adaptor::Normalize, WTF::EntitiesForUnencodables);
}

} // namespace
