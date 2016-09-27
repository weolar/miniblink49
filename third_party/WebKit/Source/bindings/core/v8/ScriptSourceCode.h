/*
 * Copyright (C) 2008, 2009 Google Inc. All rights reserved.
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

#ifndef ScriptSourceCode_h
#define ScriptSourceCode_h

#include "bindings/core/v8/ScriptStreamer.h"
#include "core/CoreExport.h"
#include "core/fetch/ResourcePtr.h"
#include "core/fetch/ScriptResource.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/TextPosition.h"
#include "wtf/text/WTFString.h"

namespace blink {

template <class R> class ResourcePtr;
class ScriptResource;

class CORE_EXPORT ScriptSourceCode final {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    ScriptSourceCode();
    // We lose the encoding information from ScriptResource.
    // Not sure if that matters.
    explicit ScriptSourceCode(ScriptResource*);
    ScriptSourceCode(const String&, const KURL& = KURL(), const TextPosition& startPosition = TextPosition::minimumPosition());
    ScriptSourceCode(PassRefPtrWillBeRawPtr<ScriptStreamer>, ScriptResource*);

    ~ScriptSourceCode();
    DECLARE_TRACE();

    bool isEmpty() const { return m_source.isEmpty(); }

    // The null value represents a missing script, created by the nullary
    // constructor, and differs from the empty script.
    bool isNull() const { return m_source.isNull(); }

    const String& source() const { return m_source; }
    ScriptResource* resource() const { return m_resource.get(); }
    const KURL& url() const;
    int startLine() const { return m_startPosition.m_line.oneBasedInt(); }
    const TextPosition& startPosition() const { return m_startPosition; }
    String sourceMapUrl() const;

    ScriptStreamer* streamer() const { return m_streamer.get(); }

private:
    void treatNullSourceAsEmpty();

    String m_source;
    ResourcePtr<ScriptResource> m_resource;
    RefPtrWillBeMember<ScriptStreamer> m_streamer;
    mutable KURL m_url;
    TextPosition m_startPosition;
};

} // namespace blink

WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::ScriptSourceCode);

#endif // ScriptSourceCode_h
