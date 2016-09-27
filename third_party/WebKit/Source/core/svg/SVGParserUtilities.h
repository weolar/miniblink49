/*
 * Copyright (C) 2002, 2003 The Karbon Developers
 * Copyright (C) 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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

#ifndef SVGParserUtilities_h
#define SVGParserUtilities_h

#include "core/html/parser/HTMLParserIdioms.h"
#include "core/svg/SVGTransform.h"
#include "platform/text/ParserUtilities.h"
#include "wtf/HashSet.h"

typedef pair<unsigned, unsigned> UnicodeRange;
typedef Vector<UnicodeRange> UnicodeRanges;

namespace blink {

class FloatPoint;

enum WhitespaceMode {
    DisallowWhitespace = 0,
    AllowLeadingWhitespace = 0x1,
    AllowTrailingWhitespace = 0x2,
    AllowLeadingAndTrailingWhitespace = AllowLeadingWhitespace | AllowTrailingWhitespace
};

bool parseNumber(const LChar*& ptr, const LChar* end, float& number, WhitespaceMode = AllowLeadingAndTrailingWhitespace);
bool parseNumber(const UChar*& ptr, const UChar* end, float& number, WhitespaceMode = AllowLeadingAndTrailingWhitespace);
bool parseNumberOptionalNumber(const String& s, float& h, float& v);
bool parseNumberOrPercentage(const String& s, float& number);
bool parseArcFlag(const LChar*& ptr, const LChar* end, bool& flag);
bool parseArcFlag(const UChar*& ptr, const UChar* end, bool& flag);

template <typename CharType>
inline bool skipOptionalSVGSpaces(const CharType*& ptr, const CharType* end)
{
    while (ptr < end && isHTMLSpace<CharType>(*ptr))
        ptr++;
    return ptr < end;
}

template <typename CharType>
inline bool skipOptionalSVGSpacesOrDelimiter(const CharType*& ptr, const CharType* end, char delimiter = ',')
{
    if (ptr < end && !isHTMLSpace<CharType>(*ptr) && *ptr != delimiter)
        return false;
    if (skipOptionalSVGSpaces(ptr, end)) {
        if (ptr < end && *ptr == delimiter) {
            ptr++;
            skipOptionalSVGSpaces(ptr, end);
        }
    }
    return ptr < end;
}

template<typename CharType>
bool parseAndSkipTransformType(const CharType*& ptr, const CharType* end, SVGTransformType&);
SVGTransformType parseTransformType(const String&);

} // namespace blink

#endif // SVGParserUtilities_h
