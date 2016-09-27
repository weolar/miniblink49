// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MediaListDirective_h
#define MediaListDirective_h

#include "core/frame/csp/CSPDirective.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "wtf/HashSet.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ContentSecurityPolicy;

class MediaListDirective final : public CSPDirective {
    WTF_MAKE_NONCOPYABLE(MediaListDirective);
public:
    MediaListDirective(const String& name, const String& value, ContentSecurityPolicy*);
    bool allows(const String& type);

private:
    void parse(const UChar* begin, const UChar* end);

    HashSet<String> m_pluginTypes;
};

} // namespace blink

#endif
