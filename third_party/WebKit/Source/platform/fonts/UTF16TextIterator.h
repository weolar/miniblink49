/*
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef UTF16TextIterator_h
#define UTF16TextIterator_h

#include "platform/PlatformExport.h"
#include "wtf/text/CharacterNames.h"
#include "wtf/text/WTFString.h"
#include <unicode/uchar.h>

namespace blink {

class PLATFORM_EXPORT UTF16TextIterator {
public:
    // The passed in UChar pointer starts at 'offset'. The iterator operates on the range [offset, endOffset].
    // 'length' denotes the maximum length of the UChar array, which might exceed 'endOffset'.
    UTF16TextIterator(const UChar*, int length);

    // FIXME: The offset/endOffset fields are only used by the SimpleShaper,
    // remove once HarfBuzz is used for all text.
    UTF16TextIterator(const UChar*, int offset, int endOffset, int length);

    inline bool consume(UChar32& character)
    {
        if (m_offset >= m_endOffset)
            return false;

        character = *m_characters;
        m_currentGlyphLength = 1;

        if (character < hiraganaLetterSmallACharacter || consumeSlowCase(character)) {
//#ifdef MINIBLINK_NOT_IMPLEMENTED
            if (U_GET_GC_MASK(character) & U_GC_M_MASK)
                consumeMultipleUChar();
//#endif // MINIBLINK_NOT_IMPLEMENTED
            return true;
        }

        return false;
    }

    void advance()
    {
        m_characters += m_currentGlyphLength;
        m_offset += m_currentGlyphLength;
    }

    int offset() const { return m_offset; }
    const UChar* characters() const { return m_characters; }
    const UChar* glyphEnd() const { return m_characters + m_currentGlyphLength; }
    // FIXME: Only used by SimpleShaper, should be removed once the SimpleShaper
    // is removed.
    unsigned glyphLength() const { return m_currentGlyphLength; }

private:
    bool consumeSlowCase(UChar32&);
    void consumeMultipleUChar();
    UChar32 normalizeVoicingMarks();

    const UChar* m_characters;
    const UChar* m_charactersEnd;
    int m_offset;
    int m_endOffset;
    unsigned m_currentGlyphLength;
};

}

#endif
