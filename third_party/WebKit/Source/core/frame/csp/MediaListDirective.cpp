// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/csp/MediaListDirective.h"

#include "core/frame/csp/ContentSecurityPolicy.h"
#include "platform/ParsingUtilities.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "wtf/HashSet.h"
#include "wtf/text/WTFString.h"

namespace blink {

MediaListDirective::MediaListDirective(const String& name, const String& value, ContentSecurityPolicy* policy)
    : CSPDirective(name, value, policy)
{
    Vector<UChar> characters;
    value.appendTo(characters);
    parse(characters.data(), characters.data() + characters.size());
}

bool MediaListDirective::allows(const String& type)
{
    return m_pluginTypes.contains(type);
}

void MediaListDirective::parse(const UChar* begin, const UChar* end)
{
    const UChar* position = begin;

    // 'plugin-types ____;' OR 'plugin-types;'
    if (position == end) {
        policy()->reportInvalidPluginTypes(String());
        return;
    }

    while (position < end) {
        // _____ OR _____mime1/mime1
        // ^        ^
        skipWhile<UChar, isASCIISpace>(position, end);
        if (position == end)
            return;

        // mime1/mime1 mime2/mime2
        // ^
        begin = position;
        if (!skipExactly<UChar, isMediaTypeCharacter>(position, end)) {
            skipWhile<UChar, isNotASCIISpace>(position, end);
            policy()->reportInvalidPluginTypes(String(begin, position - begin));
            continue;
        }
        skipWhile<UChar, isMediaTypeCharacter>(position, end);

        // mime1/mime1 mime2/mime2
        //      ^
        if (!skipExactly<UChar>(position, end, '/')) {
            skipWhile<UChar, isNotASCIISpace>(position, end);
            policy()->reportInvalidPluginTypes(String(begin, position - begin));
            continue;
        }

        // mime1/mime1 mime2/mime2
        //       ^
        if (!skipExactly<UChar, isMediaTypeCharacter>(position, end)) {
            skipWhile<UChar, isNotASCIISpace>(position, end);
            policy()->reportInvalidPluginTypes(String(begin, position - begin));
            continue;
        }
        skipWhile<UChar, isMediaTypeCharacter>(position, end);

        // mime1/mime1 mime2/mime2 OR mime1/mime1  OR mime1/mime1/error
        //            ^                          ^               ^
        if (position < end && isNotASCIISpace(*position)) {
            skipWhile<UChar, isNotASCIISpace>(position, end);
            policy()->reportInvalidPluginTypes(String(begin, position - begin));
            continue;
        }
        m_pluginTypes.add(String(begin, position - begin));

        ASSERT(position == end || isASCIISpace(*position));
    }
}

} // namespace blink
