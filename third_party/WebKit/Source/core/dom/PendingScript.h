/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PendingScript_h
#define PendingScript_h

#include "core/CoreExport.h"
#include "core/fetch/ResourceOwner.h"
#include "core/fetch/ScriptResource.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/TextPosition.h"

namespace blink {

class Element;
class ScriptSourceCode;
class ScriptStreamer;

// A container for an external script which may be loaded and executed.
//
// A ResourcePtr alone does not prevent the underlying Resource
// from purging its data buffer. This class holds a dummy client open for its
// lifetime in order to guarantee that the data buffer will not be purged.
class CORE_EXPORT PendingScript final : public ResourceOwner<ScriptResource> {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    enum Type {
        ParsingBlocking,
        Deferred,
        Async
    };

    PendingScript();
    PendingScript(Element*, ScriptResource*);
    PendingScript(const PendingScript&);
    ~PendingScript();

    PendingScript& operator=(const PendingScript&);

    TextPosition startingPosition() const { return m_startingPosition; }
    void setStartingPosition(const TextPosition& position) { m_startingPosition = position; }

    void watchForLoad(ScriptResourceClient*);
    void stopWatchingForLoad(ScriptResourceClient*);

    Element* element() const { return m_element.get(); }
    void setElement(Element*);
    PassRefPtrWillBeRawPtr<Element> releaseElementAndClear();

    void setScriptResource(ScriptResource*);

    virtual void notifyFinished(Resource*);
    virtual void notifyAppendData(ScriptResource*);

    DECLARE_TRACE();

    ScriptSourceCode getSource(const KURL& documentURL, bool& errorOccurred) const;

    void setStreamer(PassRefPtrWillBeRawPtr<ScriptStreamer>);

    bool isReady() const;

private:
    bool m_watchingForLoad;
    RefPtrWillBeMember<Element> m_element;
    TextPosition m_startingPosition; // Only used for inline script tags.

    RefPtrWillBeMember<ScriptStreamer> m_streamer;
};

} // namespace blink

#endif // PendingScript_h
