/*
 * Copyright (C) 2004, 2006, 2009 Apple Inc. All rights reserved.
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

#ifndef CharacterIterator_h
#define CharacterIterator_h

#include "core/CoreExport.h"
#include "core/dom/Range.h"
#include "core/editing/EphemeralRange.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/editing/iterators/TextIteratorFlags.h"
#include "platform/heap/Heap.h"

namespace blink {

// Builds on the text iterator, adding a character position so we can walk one
// character at a time, or faster, as needed. Useful for searching.
template <typename Strategy>
class CORE_EXPORT CharacterIteratorAlgorithm {
    STACK_ALLOCATED();
public:
    CharacterIteratorAlgorithm(const PositionAlgorithm<Strategy>& start, const PositionAlgorithm<Strategy>& end, TextIteratorBehaviorFlags = TextIteratorDefaultBehavior);
    explicit CharacterIteratorAlgorithm(const EphemeralRangeTemplate<Strategy>&, TextIteratorBehaviorFlags = TextIteratorDefaultBehavior);

    void advance(int numCharacters);

    bool atBreak() const { return m_atBreak; }
    bool atEnd() const { return m_textIterator.atEnd(); }

    int length() const { return m_textIterator.length() - m_runOffset; }
    UChar characterAt(unsigned index) const { return m_textIterator.text().characterAt(m_runOffset + index); }

    template<typename BufferType>
    void appendTextTo(BufferType& output) { m_textIterator.text().appendTextTo(output, m_runOffset); }

    int characterOffset() const { return m_offset; }
    EphemeralRangeTemplate<Strategy> range() const;

    Document* ownerDocument() const;
    Node* currentContainer() const;
    int startOffset() const;
    int endOffset() const;
    PositionAlgorithm<Strategy> startPosition() const;
    PositionAlgorithm<Strategy> endPosition() const;

    EphemeralRangeTemplate<Strategy> calculateCharacterSubrange(int offset, int length);

private:
    void initialize();

    int m_offset;
    int m_runOffset;
    bool m_atBreak;

    TextIteratorAlgorithm<Strategy> m_textIterator;
};

extern template class CORE_EXTERN_TEMPLATE_EXPORT CharacterIteratorAlgorithm<EditingStrategy>;
using CharacterIterator = CharacterIteratorAlgorithm<EditingStrategy>;

CORE_EXPORT EphemeralRange findPlainText(const EphemeralRange& inputRange, const String&, FindOptions);

} // namespace blink

#endif // CharacterIterator_h
