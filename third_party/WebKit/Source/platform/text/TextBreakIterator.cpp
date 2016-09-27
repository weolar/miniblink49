/*
 * (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2007-2009 Torch Mobile, Inc.
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
 */

#include "config.h"
#include "platform/text/TextBreakIterator.h"

#include "wtf/ASCIICType.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/CharacterNames.h"
#include "third_party/icu/source/common/unicode/uchar.h"

namespace blink {

unsigned numGraphemeClusters(const String& string)
{
    unsigned stringLength = string.length();

    if (!stringLength)
        return 0;

    // The only Latin-1 Extended Grapheme Cluster is CR LF
    if (string.is8Bit() && !string.contains('\r'))
        return stringLength;

    NonSharedCharacterBreakIterator it(string);
    if (!it)
        return stringLength;

    unsigned num = 0;
    while (it.next() != TextBreakDone)
        ++num;
    return num;
}


static inline bool isBreakableSpace(UChar ch)
{
    switch (ch) {
    case ' ':
    case '\n':
    case '\t':
        return true;
    default:
        return false;
    }
}

static const UChar asciiLineBreakTableFirstChar = '!';
static const UChar asciiLineBreakTableLastChar = 127;

// Pack 8 bits into one byte
#define B(a, b, c, d, e, f, g, h) \
    ((a) | ((b) << 1) | ((c) << 2) | ((d) << 3) | ((e) << 4) | ((f) << 5) | ((g) << 6) | ((h) << 7))

// Line breaking table row for each digit (0-9)
#define DI { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

// Line breaking table row for ascii letters (a-z A-Z)
#define AL { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define F 0xFF

// Line breaking table for printable ASCII characters. Line breaking opportunities in this table are as below:
// - before opening punctuations such as '(', '<', '[', '{' after certain characters (compatible with Firefox 3.6);
// - after '-' and '?' (backward-compatible, and compatible with Internet Explorer).
// Please refer to <https://bugs.webkit.org/show_bug.cgi?id=37698> for line breaking matrixes of different browsers
// and the ICU standard.
static const unsigned char asciiLineBreakTable[][(asciiLineBreakTableLastChar - asciiLineBreakTableFirstChar) / 8 + 1] = {
    //  !  "  #  $  %  &  '  (     )  *  +  ,  -  .  /  0  1-8   9  :  ;  <  =  >  ?  @     A-X      Y  Z  [  \  ]  ^  _  `     a-x      y  z  {  |  }  ~  DEL
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // !
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // "
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // #
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // $
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // %
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // &
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // '
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // (
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // )
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // *
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // +
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // ,
    { B(1, 1, 1, 1, 1, 1, 1, 1), B(1, 1, 1, 1, 1, 0, 1, 0), 0, B(0, 1, 1, 1, 1, 1, 1, 1), F, F, F, B(1, 1, 1, 1, 1, 1, 1, 1), F, F, F, B(1, 1, 1, 1, 1, 1, 1, 1) }, // - Note: breaking before '0'-'9' is handled hard-coded in shouldBreakAfter().
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // .
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // /
    DI,  DI,  DI,  DI,  DI,  DI,  DI,  DI,  DI,  DI, // 0-9
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // :
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // ;
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // <
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // =
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // >
    { B(0, 0, 1, 1, 1, 1, 0, 1), B(0, 1, 1, 0, 1, 0, 0, 1), F, B(1, 0, 0, 1, 1, 1, 0, 1), F, F, F, B(1, 1, 1, 1, 0, 1, 1, 1), F, F, F, B(1, 1, 1, 1, 0, 1, 1, 0) }, // ?
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // @
    AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL, // A-Z
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // [
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // '\'
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // ]
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // ^
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // _
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // `
    AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL,  AL, // a-z
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // {
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // |
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // }
    { B(0, 0, 0, 0, 0, 0, 0, 1), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 1, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 1, 0, 0, 0, 0, 0) }, // ~
    { B(0, 0, 0, 0, 0, 0, 0, 0), B(0, 0, 0, 0, 0, 0, 0, 0), 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0), 0, 0, 0, B(0, 0, 0, 0, 0, 0, 0, 0) }, // DEL
};

#undef B
#undef F
#undef DI
#undef AL

static_assert(WTF_ARRAY_LENGTH(asciiLineBreakTable) == asciiLineBreakTableLastChar - asciiLineBreakTableFirstChar + 1, "asciiLineBreakTable should be consistent");

static inline bool shouldBreakAfter(UChar lastCh, UChar ch, UChar nextCh)
{
    // Don't allow line breaking between '-' and a digit if the '-' may mean a minus sign in the context,
    // while allow breaking in 'ABCD-1234' and '1234-5678' which may be in long URLs.
    if (ch == '-' && isASCIIDigit(nextCh))
        return isASCIIAlphanumeric(lastCh);

    // If both ch and nextCh are ASCII characters, use a lookup table for enhanced speed and for compatibility
    // with other browsers (see comments for asciiLineBreakTable for details).
    if (ch >= asciiLineBreakTableFirstChar && ch <= asciiLineBreakTableLastChar
        && nextCh >= asciiLineBreakTableFirstChar && nextCh <= asciiLineBreakTableLastChar) {
        const unsigned char* tableRow = asciiLineBreakTable[ch - asciiLineBreakTableFirstChar];
        int nextChIndex = nextCh - asciiLineBreakTableFirstChar;
        return tableRow[nextChIndex / 8] & (1 << (nextChIndex % 8));
    }
    // Otherwise defer to the Unicode algorithm by returning false.
    return false;
}

inline bool needsLineBreakIterator(UChar ch)
{
    return ch > asciiLineBreakTableLastChar && ch != noBreakSpaceCharacter;
}

template<typename CharacterType>
static inline int nextBreakablePosition(LazyLineBreakIterator& lazyBreakIterator, const CharacterType* str, unsigned length, int pos)
{
    int len = static_cast<int>(length);
    int nextBreak = -1;

    CharacterType lastLastCh = pos > 1 ? str[pos - 2] : static_cast<CharacterType>(lazyBreakIterator.secondToLastCharacter());
    CharacterType lastCh = pos > 0 ? str[pos - 1] : static_cast<CharacterType>(lazyBreakIterator.lastCharacter());
    unsigned priorContextLength = lazyBreakIterator.priorContextLength();
    for (int i = pos; i < len; i++) {
        CharacterType ch = str[i];

        if (isBreakableSpace(ch) || shouldBreakAfter(lastLastCh, lastCh, ch))
            return i;

        if (needsLineBreakIterator(ch) || needsLineBreakIterator(lastCh)) {
            if (nextBreak < i) {
                // Don't break if positioned at start of primary context and there is no prior context.
                if (i || priorContextLength) {
                    TextBreakIterator* breakIterator = lazyBreakIterator.get(priorContextLength);
                    if (breakIterator) {
                        nextBreak = breakIterator->following(i - 1 + priorContextLength);
                        if (nextBreak >= 0) {
                            nextBreak -= priorContextLength;
                        }
                    }
                }
            }
            if (i == nextBreak && !isBreakableSpace(lastCh))
                return i;
        }

        lastLastCh = lastCh;
        lastCh = ch;
    }

    return len;
}

static inline bool isUnicodeCategoryLetterOrNumber(UChar lastCh, UChar ch)
{
    UChar32 ch32 = U16_IS_LEAD(lastCh) && U16_IS_TRAIL(ch) ? U16_GET_SUPPLEMENTARY(lastCh, ch) : ch;
    return (U_MASK(u_charType(ch32)) & (U_GC_L_MASK | U_GC_N_MASK));
}

template<typename CharacterType>
static inline int nextBreakablePositionBreakAllInternal(LazyLineBreakIterator& lazyBreakIterator, const CharacterType* str, unsigned length, int pos)
{
    int len = static_cast<int>(length);
    CharacterType lastLastCh = pos > 1 ? str[pos - 2] : static_cast<CharacterType>(lazyBreakIterator.secondToLastCharacter());
    CharacterType lastCh = pos > 0 ? str[pos - 1] : static_cast<CharacterType>(lazyBreakIterator.lastCharacter());
    bool lastIsLetterOrNumber = isUnicodeCategoryLetterOrNumber(lastLastCh, lastCh);
    for (int i = pos; i < len; ++i) {
        CharacterType ch = str[i];

        if (isBreakableSpace(ch) || shouldBreakAfter(lastLastCh, lastCh, ch))
            return i;

        if (!U16_IS_LEAD(ch)) {
            bool isLetterOrNumber = isUnicodeCategoryLetterOrNumber(lastCh, ch);
            if (isLetterOrNumber && lastIsLetterOrNumber)
                return i > pos && U16_IS_TRAIL(ch) ? i - 1 : i;
            lastIsLetterOrNumber = isLetterOrNumber;
        }

        lastLastCh = lastCh;
        lastCh = ch;
    }
    return len;
}

static inline bool shouldKeepAfter(UChar lastCh, UChar ch, UChar nextCh)
{
    UChar preCh = U_MASK(u_charType(ch)) & U_GC_M_MASK ? lastCh : ch;
    return U_MASK(u_charType(preCh)) & (U_GC_L_MASK | U_GC_N_MASK)
        && !WTF::Unicode::hasLineBreakingPropertyComplexContext(preCh)
        && U_MASK(u_charType(nextCh)) & (U_GC_L_MASK | U_GC_N_MASK)
        && !WTF::Unicode::hasLineBreakingPropertyComplexContext(nextCh);
}

static inline int nextBreakablePositionKeepAllInternal(LazyLineBreakIterator& lazyBreakIterator, const UChar* str, unsigned length, int pos)
{
    int len = static_cast<int>(length);
    int nextBreak = -1;

    UChar lastLastCh = pos > 1 ? str[pos - 2] : static_cast<UChar>(lazyBreakIterator.secondToLastCharacter());
    UChar lastCh = pos > 0 ? str[pos - 1] : static_cast<UChar>(lazyBreakIterator.lastCharacter());
    unsigned priorContextLength = lazyBreakIterator.priorContextLength();
    for (int i = pos; i < len; i++) {
        UChar ch = str[i];

        if (isBreakableSpace(ch) || shouldBreakAfter(lastLastCh, lastCh, ch))
            return i;

        if (!shouldKeepAfter(lastLastCh, lastCh, ch) && (needsLineBreakIterator(ch) || needsLineBreakIterator(lastCh))) {
            if (nextBreak < i) {
                // Don't break if positioned at start of primary context and there is no prior context.
                if (i || priorContextLength) {
                    TextBreakIterator* breakIterator = lazyBreakIterator.get(priorContextLength);
                    if (breakIterator) {
                        nextBreak = breakIterator->following(i - 1 + priorContextLength);
                        if (nextBreak >= 0) {
                            nextBreak -= priorContextLength;
                        }
                    }
                }
            }
            if (i == nextBreak && !isBreakableSpace(lastCh))
                return i;
        }

        lastLastCh = lastCh;
        lastCh = ch;
    }

    return len;
}

int LazyLineBreakIterator::nextBreakablePositionIgnoringNBSP(int pos)
{
    if (m_string.is8Bit())
        return nextBreakablePosition<LChar>(*this, m_string.characters8(), m_string.length(), pos);
    return nextBreakablePosition<UChar>(*this, m_string.characters16(), m_string.length(), pos);
}

int LazyLineBreakIterator::nextBreakablePositionBreakAll(int pos)
{
    if (m_string.is8Bit())
        return nextBreakablePositionBreakAllInternal<LChar>(*this, m_string.characters8(), m_string.length(), pos);
    return nextBreakablePositionBreakAllInternal<UChar>(*this, m_string.characters16(), m_string.length(), pos);
}

int LazyLineBreakIterator::nextBreakablePositionKeepAll(int pos)
{
    if (m_string.is8Bit())
        return nextBreakablePosition<LChar>(*this, m_string.characters8(), m_string.length(), pos);
    return nextBreakablePositionKeepAllInternal(*this, m_string.characters16(), m_string.length(), pos);
}

} // namespace blink
