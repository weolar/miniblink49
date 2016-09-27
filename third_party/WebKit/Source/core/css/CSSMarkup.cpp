/*
 * Copyright (C) 2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2005 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Nicholas Shanks <webkit@nickshanks.com>
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2012 Adobe Systems Incorporated. All rights reserved.
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
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
#include "core/css/CSSMarkup.h"

#include "wtf/HexNumber.h"
#include "wtf/text/StringBuffer.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

template <typename CharacterType>
static inline bool isCSSTokenizerIdentifier(const CharacterType* characters, unsigned length)
{
    const CharacterType* end = characters + length;

    // -?
    if (characters != end && characters[0] == '-')
        ++characters;

    // {nmstart}
    if (characters == end || !(characters[0] == '_' || characters[0] >= 128 || isASCIIAlpha(characters[0])))
        return false;
    ++characters;

    // {nmchar}*
    for (; characters != end; ++characters) {
        if (!(characters[0] == '_' || characters[0] == '-' || characters[0] >= 128 || isASCIIAlphanumeric(characters[0])))
            return false;
    }

    return true;
}

// "ident" from the CSS tokenizer, minus backslash-escape sequences
static bool isCSSTokenizerIdentifier(const String& string)
{
    unsigned length = string.length();

    if (!length)
        return false;

    if (string.is8Bit())
        return isCSSTokenizerIdentifier(string.characters8(), length);
    return isCSSTokenizerIdentifier(string.characters16(), length);
}

template <typename CharacterType>
static inline bool isCSSTokenizerURL(const CharacterType* characters, unsigned length)
{
    const CharacterType* end = characters + length;

    for (; characters != end; ++characters) {
        CharacterType c = characters[0];
        switch (c) {
        case '!':
        case '#':
        case '$':
        case '%':
        case '&':
            break;
        default:
            if (c < '*')
                return false;
            if (c <= '~')
                break;
            if (c < 128)
                return false;
        }
    }

    return true;
}

// "url" from the CSS tokenizer, minus backslash-escape sequences
static bool isCSSTokenizerURL(const String& string)
{
    unsigned length = string.length();

    if (!length)
        return true;

    if (string.is8Bit())
        return isCSSTokenizerURL(string.characters8(), length);
    return isCSSTokenizerURL(string.characters16(), length);
}

template <typename CharacterType>
static inline String quoteCSSStringInternal(const CharacterType* characters, unsigned length)
{
    // For efficiency, we first pre-calculate the length of the quoted string, then we build the actual one.
    // Please see below for the actual logic.
    unsigned quotedStringSize = 2; // Two quotes surrounding the entire string.
    bool afterEscape = false;
    for (unsigned i = 0; i < length; ++i) {
        CharacterType ch = characters[i];
        if (ch == '\\' || ch == '\'') {
            quotedStringSize += 2;
            afterEscape = false;
        } else if (ch < 0x20 || ch == 0x7F) {
            quotedStringSize += 2 + (ch >= 0x10);
            afterEscape = true;
        } else {
            quotedStringSize += 1 + (afterEscape && (isASCIIHexDigit(ch) || ch == ' '));
            afterEscape = false;
        }
    }

    StringBuffer<CharacterType> buffer(quotedStringSize);
    unsigned index = 0;
    buffer[index++] = '\'';
    afterEscape = false;
    for (unsigned i = 0; i < length; ++i) {
        CharacterType ch = characters[i];
        if (ch == '\\' || ch == '\'') {
            buffer[index++] = '\\';
            buffer[index++] = ch;
            afterEscape = false;
        } else if (ch < 0x20 || ch == 0x7F) { // Control characters.
            buffer[index++] = '\\';
            placeByteAsHexCompressIfPossible(ch, buffer, index, Lowercase);
            afterEscape = true;
        } else {
            // Space character may be required to separate backslash-escape sequence and normal characters.
            if (afterEscape && (isASCIIHexDigit(ch) || ch == ' '))
                buffer[index++] = ' ';
            buffer[index++] = ch;
            afterEscape = false;
        }
    }
    buffer[index++] = '\'';

    ASSERT(quotedStringSize == index);
    return String::adopt(buffer);
}

// We use single quotes for now because markup.cpp uses double quotes.
static String quoteCSSString(const String& string)
{
    // This function expands each character to at most 3 characters ('\u0010' -> '\' '1' '0') as well as adds
    // 2 quote characters (before and after). Make sure the resulting size (3 * length + 2) will not overflow unsigned.

    unsigned length = string.length();

    if (!length)
        return String("\'\'");

    if (length > std::numeric_limits<unsigned>::max() / 3 - 2)
        return emptyString();

    if (string.is8Bit())
        return quoteCSSStringInternal(string.characters8(), length);
    return quoteCSSStringInternal(string.characters16(), length);
}

String quoteCSSStringIfNeeded(const String& string)
{
    return isCSSTokenizerIdentifier(string) ? string : quoteCSSString(string);
}

String quoteCSSURLIfNeeded(const String& string)
{
    return isCSSTokenizerURL(string) ? string : quoteCSSString(string);
}

static void serializeCharacter(UChar32 c, StringBuilder& appendTo)
{
    appendTo.append('\\');
    appendTo.append(c);
}

static void serializeCharacterAsCodePoint(UChar32 c, StringBuilder& appendTo)
{
    appendTo.append('\\');
    appendUnsignedAsHex(c, appendTo, Lowercase);
    appendTo.append(' ');
}

void serializeIdentifier(const String& identifier, StringBuilder& appendTo)
{
    bool isFirst = true;
    bool isSecond = false;
    bool isFirstCharHyphen = false;
    unsigned index = 0;
    while (index < identifier.length()) {
        UChar32 c = identifier.characterStartingAt(index);
        index += U16_LENGTH(c);

        if (c <= 0x1f || (0x30 <= c && c <= 0x39 && (isFirst || (isSecond && isFirstCharHyphen))))
            serializeCharacterAsCodePoint(c, appendTo);
        else if (c == 0x2d && isSecond && isFirstCharHyphen)
            serializeCharacter(c, appendTo);
        else if (0x80 <= c || c == 0x2d || c == 0x5f || (0x30 <= c && c <= 0x39) || (0x41 <= c && c <= 0x5a) || (0x61 <= c && c <= 0x7a))
            appendTo.append(c);
        else
            serializeCharacter(c, appendTo);

        if (isFirst) {
            isFirst = false;
            isSecond = true;
            isFirstCharHyphen = (c == 0x2d);
        } else if (isSecond) {
            isSecond = false;
        }
    }
}

void serializeString(const String& string, StringBuilder& appendTo)
{
    appendTo.append('\"');

    unsigned index = 0;
    while (index < string.length()) {
        UChar32 c = string.characterStartingAt(index);
        index += U16_LENGTH(c);
        if (c <= 0x1f)
            serializeCharacterAsCodePoint(c, appendTo);
        else if (c == 0x22 || c == 0x5c)
            serializeCharacter(c, appendTo);
        else
            appendTo.append(c);
    }

    appendTo.append('\"');
}

String serializeString(const String& string)
{
    StringBuilder builder;
    serializeString(string, builder);
    return builder.toString();
}

} // namespace blink
