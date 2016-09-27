/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TextTrackCueList_h
#define TextTrackCueList_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/html/track/TextTrackCue.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/Vector.h"

namespace blink {

class TextTrackCueList final : public RefCountedWillBeGarbageCollected<TextTrackCueList>, public ScriptWrappable {
    DECLARE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(TextTrackCueList);
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<TextTrackCueList> create()
    {
        return adoptRefWillBeNoop(new TextTrackCueList);
    }

    unsigned long length() const;

    TextTrackCue* item(unsigned index) const;
    TextTrackCue* getCueById(const AtomicString&) const;

    bool add(PassRefPtrWillBeRawPtr<TextTrackCue>);
    bool remove(TextTrackCue*);

    void collectActiveCues(TextTrackCueList&) const;
    void updateCueIndex(TextTrackCue*);
    bool isCueIndexValid(unsigned probeIndex) const { return probeIndex < m_firstInvalidIndex; }
    void validateCueIndexes();

    DECLARE_TRACE();

private:
    TextTrackCueList();
    size_t findInsertionIndex(const TextTrackCue*) const;
    void invalidateCueIndex(size_t index);
    void clear();

    WillBeHeapVector<RefPtrWillBeMember<TextTrackCue>> m_list;
    size_t m_firstInvalidIndex;
};

} // namespace blink

#endif // TextTrackCueList_h
