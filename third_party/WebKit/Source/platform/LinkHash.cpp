/*
 * Copyright (c) 2008, 2009, Google Inc. All rights reserved.
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
#include "platform/LinkHash.h"

#include "platform/weborigin/KURL.h"
#include "public/platform/Platform.h"
#include "wtf/text/StringUTF8Adaptor.h"
//#include <url/url_util.h>

namespace blink {
#ifdef MINIBLINK_NOT_IMPLEMENTED
static bool resolveRelative(const KURL& base, const String& relative, url::RawCanonOutput<2048>* buffer)
{

    // We use these low-level GURL functions to avoid converting back and forth from UTF-8 unnecessarily.
    url::Parsed parsed;
    StringUTF8Adaptor baseUTF8(base.string());
    if (relative.is8Bit()) {
        StringUTF8Adaptor relativeUTF8(relative);
        return url::ResolveRelative(baseUTF8.data(), baseUTF8.length(), base.parsed(), relativeUTF8.data(), relativeUTF8.length(), 0, buffer, &parsed);
    }
    return url::ResolveRelative(baseUTF8.data(), baseUTF8.length(), base.parsed(), relative.characters16(), relative.length(), 0, buffer, &parsed);
}
#endif // MINIBLINK_NOT_IMPLEMENTED

LinkHash visitedLinkHash(const KURL& base, const AtomicString& relative)
{
    if (relative.isNull())
        return 0;
#ifdef MINIBLINK_NOT_IMPLEMENTED
    url::RawCanonOutput<2048> buffer;
    if (!resolveRelative(base, relative.string(), &buffer))
        return 0;
    return Platform::current()->visitedLinkHash(buffer.data(), buffer.length());
#else
    String link = base.string();
    link.append(relative.string());
    CString buffer = link.utf8();
    
    return WTF::StringHasher::computeHashAndMaskTop8Bits<LChar>((const LChar*)buffer.data(), (unsigned int)buffer.length());
#endif // MINIBLINK_NOT_IMPLEMENTED
}

} // namespace blink
