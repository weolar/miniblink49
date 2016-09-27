/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2005 Alexey Proskuryakov.
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

#include "config.h"
#include "core/editing/iterators/WordAwareIterator.h"

namespace blink {

WordAwareIterator::WordAwareIterator(const Position& start, const Position& end)
    : m_didLookAhead(true) // So we consider the first chunk from the text iterator.
    , m_textIterator(start, end)
{
    advance(); // Get in position over the first chunk of text.
}

WordAwareIterator::~WordAwareIterator()
{
}

// FIXME: Performance could be bad for huge spans next to each other that don't fall on word boundaries.

void WordAwareIterator::advance()
{
    m_buffer.clear();

    // If last time we did a look-ahead, start with that looked-ahead chunk now
    if (!m_didLookAhead) {
        ASSERT(!m_textIterator.atEnd());
        m_textIterator.advance();
    }
    m_didLookAhead = false;

    // Go to next non-empty chunk.
    while (!m_textIterator.atEnd() && !m_textIterator.length())
        m_textIterator.advance();

    if (m_textIterator.atEnd())
        return;

    while (1) {
        // If this chunk ends in whitespace we can just use it as our chunk.
        if (isSpaceOrNewline(m_textIterator.text().characterAt(m_textIterator.length() - 1)))
            return;

        // If this is the first chunk that failed, save it in m_buffer before look ahead.
        if (m_buffer.isEmpty())
            m_textIterator.text().appendTextTo(m_buffer);

        // Look ahead to next chunk. If it is whitespace or a break, we can use the previous stuff
        m_textIterator.advance();
        if (m_textIterator.atEnd() || !m_textIterator.length() || isSpaceOrNewline(m_textIterator.text().characterAt(0))) {
            m_didLookAhead = true;
            return;
        }

        // Start gobbling chunks until we get to a suitable stopping point
        m_textIterator.text().appendTextTo(m_buffer);
    }
}

int WordAwareIterator::length() const
{
    if (!m_buffer.isEmpty())
        return m_buffer.size();
    return m_textIterator.length();
}

String WordAwareIterator::substring(unsigned position, unsigned length) const
{
    if (!m_buffer.isEmpty())
        return String(m_buffer.data() + position, length);
    return m_textIterator.text().substring(position, length);
}

UChar WordAwareIterator::characterAt(unsigned index) const
{
    if (!m_buffer.isEmpty())
        return m_buffer[index];
    return m_textIterator.text().characterAt(index);
}

} // namespace blink
