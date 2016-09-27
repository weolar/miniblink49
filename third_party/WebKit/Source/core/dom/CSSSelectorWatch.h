/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef CSSSelectorWatch_h
#define CSSSelectorWatch_h

#include "core/CoreExport.h"
#include "core/css/StyleRule.h"
#include "core/dom/Document.h"
#include "platform/Timer.h"
#include "wtf/HashCountedSet.h"
#include "wtf/HashSet.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class CORE_EXPORT CSSSelectorWatch final : public NoBaseWillBeGarbageCollectedFinalized<CSSSelectorWatch>, public WillBeHeapSupplement<Document> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(CSSSelectorWatch);
public:
    virtual ~CSSSelectorWatch() { }

    static CSSSelectorWatch& from(Document&);
    static CSSSelectorWatch* fromIfExists(Document&);

    void watchCSSSelectors(const Vector<String>& selectors);
    const WillBeHeapVector<RefPtrWillBeMember<StyleRule>>& watchedCallbackSelectors() const { return m_watchedCallbackSelectors; }

    void updateSelectorMatches(const Vector<String>& removedSelectors, const Vector<String>& addedSelectors);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit CSSSelectorWatch(Document&);
    void callbackSelectorChangeTimerFired(Timer<CSSSelectorWatch>*);
    Document& document() const { return *m_document; }

    RawPtrWillBeMember<Document> m_document;

    WillBeHeapVector<RefPtrWillBeMember<StyleRule>> m_watchedCallbackSelectors;

    // Maps a CSS selector string with a -webkit-callback property to the number
    // of matching ComputedStyle objects in this document.
    HashCountedSet<String> m_matchingCallbackSelectors;
    // Selectors are relative to m_matchingCallbackSelectors's contents at
    // the previous call to selectorMatchChanged.
    HashSet<String> m_addedSelectors;
    HashSet<String> m_removedSelectors;

    Timer<CSSSelectorWatch> m_callbackSelectorChangeTimer;

    // When an element is reparented, the new location's style is evaluated after the expriation of the relayout timer.
    // We don't want to send redundant callbacks to the embedder, so this counter lets us wait another time around the event loop.
    int m_timerExpirations;
};

} // namespace blink

#endif // CSSSelectorWatch_h
