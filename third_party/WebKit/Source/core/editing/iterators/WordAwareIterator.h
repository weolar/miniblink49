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

#ifndef WordAwareIterator_h
#define WordAwareIterator_h

#include "core/editing/iterators/TextIterator.h"
#include "platform/heap/Heap.h"
#include "wtf/Vector.h"

namespace blink {

// Very similar to the TextIterator, except that the chunks of text returned are "well behaved",
// meaning they never end split up a word.  This is useful for spellcheck or (perhaps one day) searching.
class WordAwareIterator {
    STACK_ALLOCATED();
public:
    explicit WordAwareIterator(const Position& start, const Position& end);
    ~WordAwareIterator();

    bool atEnd() const { return !m_didLookAhead && m_textIterator.atEnd(); }
    void advance();

    String substring(unsigned position, unsigned length) const;
    UChar characterAt(unsigned index) const;
    int length() const;

private:
    Vector<UChar> m_buffer;
    // Did we have to look ahead in the textIterator to confirm the current chunk?
    bool m_didLookAhead;
    TextIterator m_textIterator;
};

} // namespace blink

#endif // WordAwareIterator_h
