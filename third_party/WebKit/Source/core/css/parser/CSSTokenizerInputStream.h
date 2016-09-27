// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSTokenizerInputStream_h
#define CSSTokenizerInputStream_h

#include "wtf/text/WTFString.h"

namespace blink {

struct CSSParserString;

class CSSTokenizerInputStream {
    WTF_MAKE_NONCOPYABLE(CSSTokenizerInputStream);
    WTF_MAKE_FAST_ALLOCATED(CSSTokenizerInputStream);
public:
    CSSTokenizerInputStream(String input);

    UChar peek(unsigned);
    inline UChar nextInputChar()
    {
        return peek(0);
    }

    // For fast-path code, don't replace nulls with replacement characters
    UChar peekWithoutReplacement(unsigned lookaheadOffset)
    {
        ASSERT((m_offset + lookaheadOffset) <= m_stringLength);
        if ((m_offset + lookaheadOffset) == m_stringLength)
            return '\0';
        return (*m_string)[m_offset + lookaheadOffset];
    }

    void advance(unsigned offset = 1) { m_offset += offset; }
    void pushBack(UChar);

    double getDouble(unsigned start, unsigned end);

    template<bool characterPredicate(UChar)>
    unsigned skipWhilePredicate(unsigned offset)
    {
        while ((m_offset + offset) < m_stringLength && characterPredicate((*m_string)[m_offset + offset]))
            ++offset;
        return offset;
    }

    unsigned offset() const { return std::min(m_offset, m_stringLength); }
    CSSParserString rangeAsCSSParserString(unsigned start, unsigned length) const;

private:
    size_t m_offset;
    const size_t m_stringLength;
    const RefPtr<StringImpl> m_string;
};

} // namespace blink

#endif // CSSTokenizerInputStream_h

