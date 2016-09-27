/*
 * Copyright (C) 2002, 2003 The Karbon Developers
 * Copyright (C) 2006 Alexander Kellett <lypanov@kde.org>
 * Copyright (C) 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007, 2009, 2013 Apple Inc. All rights reserved.
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
#include "core/svg/SVGParserUtilities.h"

#include "core/svg/SVGPointList.h"
#include "platform/geometry/FloatRect.h"
#include "platform/transforms/AffineTransform.h"
#include "wtf/ASCIICType.h"
#include "wtf/text/StringHash.h"
#include <limits>

namespace blink {

template <typename FloatType>
static inline bool isValidRange(const FloatType& x)
{
    static const FloatType max = std::numeric_limits<FloatType>::max();
    return x >= -max && x <= max;
}

// We use this generic parseNumber function to allow the Path parsing code to work
// at a higher precision internally, without any unnecessary runtime cost or code
// complexity.
template <typename CharType, typename FloatType>
static bool genericParseNumber(const CharType*& ptr, const CharType* end, FloatType& number, WhitespaceMode mode)
{
    FloatType integer, decimal, frac, exponent;
    int sign, expsign;
    const CharType* start = ptr;

    exponent = 0;
    integer = 0;
    frac = 1;
    decimal = 0;
    sign = 1;
    expsign = 1;

    if (mode & AllowLeadingWhitespace)
        skipOptionalSVGSpaces(ptr, end);

    // read the sign
    if (ptr < end && *ptr == '+')
        ptr++;
    else if (ptr < end && *ptr == '-') {
        ptr++;
        sign = -1;
    }

    if (ptr == end || ((*ptr < '0' || *ptr > '9') && *ptr != '.'))
        // The first character of a number must be one of [0-9+-.]
        return false;

    // read the integer part, build right-to-left
    const CharType* ptrStartIntPart = ptr;
    while (ptr < end && *ptr >= '0' && *ptr <= '9')
        ++ptr; // Advance to first non-digit.

    if (ptr != ptrStartIntPart) {
        const CharType* ptrScanIntPart = ptr - 1;
        FloatType multiplier = 1;
        while (ptrScanIntPart >= ptrStartIntPart) {
            integer += multiplier * static_cast<FloatType>(*(ptrScanIntPart--) - '0');
            multiplier *= 10;
        }
        // Bail out early if this overflows.
        if (!isValidRange(integer))
            return false;
    }

    if (ptr < end && *ptr == '.') { // read the decimals
        ptr++;

        // There must be a least one digit following the .
        if (ptr >= end || *ptr < '0' || *ptr > '9')
            return false;

        while (ptr < end && *ptr >= '0' && *ptr <= '9')
            decimal += (*(ptr++) - '0') * (frac *= static_cast<FloatType>(0.1));
    }

    // read the exponent part
    if (ptr != start && ptr + 1 < end && (*ptr == 'e' || *ptr == 'E')
        && (ptr[1] != 'x' && ptr[1] != 'm')) {
        ptr++;

        // read the sign of the exponent
        if (*ptr == '+')
            ptr++;
        else if (*ptr == '-') {
            ptr++;
            expsign = -1;
        }

        // There must be an exponent
        if (ptr >= end || *ptr < '0' || *ptr > '9')
            return false;

        while (ptr < end && *ptr >= '0' && *ptr <= '9') {
            exponent *= static_cast<FloatType>(10);
            exponent += *ptr - '0';
            ptr++;
        }
        // Make sure exponent is valid.
        if (!isValidRange(exponent) || exponent > std::numeric_limits<FloatType>::max_exponent)
            return false;
    }

    number = integer + decimal;
    number *= sign;

    if (exponent)
        number *= static_cast<FloatType>(pow(10.0, expsign * static_cast<int>(exponent)));

    // Don't return Infinity() or NaN().
    if (!isValidRange(number))
        return false;

    if (start == ptr)
        return false;

    if (mode & AllowTrailingWhitespace)
        skipOptionalSVGSpacesOrDelimiter(ptr, end);

    return true;
}

bool parseNumber(const LChar*& ptr, const LChar* end, float& number, WhitespaceMode mode)
{
    return genericParseNumber(ptr, end, number, mode);
}

bool parseNumber(const UChar*& ptr, const UChar* end, float& number, WhitespaceMode mode)
{
    return genericParseNumber(ptr, end, number, mode);
}

// only used to parse largeArcFlag and sweepFlag which must be a "0" or "1"
// and might not have any whitespace/comma after it
template <typename CharType>
bool genericParseArcFlag(const CharType*& ptr, const CharType* end, bool& flag)
{
    if (ptr >= end)
        return false;
    const CharType flagChar = *ptr++;
    if (flagChar == '0')
        flag = false;
    else if (flagChar == '1')
        flag = true;
    else
        return false;

    skipOptionalSVGSpacesOrDelimiter(ptr, end);

    return true;
}

bool parseArcFlag(const LChar*& ptr, const LChar* end, bool& flag)
{
    return genericParseArcFlag(ptr, end, flag);
}

bool parseArcFlag(const UChar*& ptr, const UChar* end, bool& flag)
{
    return genericParseArcFlag(ptr, end, flag);
}

template<typename CharType>
static bool genericParseNumberOptionalNumber(const CharType*& ptr, const CharType* end, float& x, float& y)
{
    if (!parseNumber(ptr, end, x))
        return false;

    if (ptr == end)
        y = x;
    else if (!parseNumber(ptr, end, y, AllowLeadingAndTrailingWhitespace))
        return false;

    return ptr == end;
}

bool parseNumberOptionalNumber(const String& string, float& x, float& y)
{
    if (string.isEmpty())
        return false;

    if (string.is8Bit()) {
        const LChar* ptr = string.characters8();
        const LChar* end = ptr + string.length();
        return genericParseNumberOptionalNumber(ptr, end, x, y);
    }
    const UChar* ptr = string.characters16();
    const UChar* end = ptr + string.length();
    return genericParseNumberOptionalNumber(ptr, end, x, y);
}

template<typename CharType>
bool genericParseNumberOrPercentage(const CharType*& ptr, const CharType* end, float& number)
{
    if (genericParseNumber(ptr, end, number, AllowLeadingWhitespace)) {
        if (ptr == end)
            return true;

        bool isPercentage = (*ptr == '%');
        if (isPercentage)
            ptr++;

        skipOptionalSVGSpaces(ptr, end);

        if (isPercentage)
            number /= 100.f;

        return ptr == end;
    }

    return false;
}

bool parseNumberOrPercentage(const String& string, float& number)
{
    if (string.isEmpty())
        return false;

    if (string.is8Bit()) {
        const LChar* ptr = string.characters8();
        const LChar* end = ptr + string.length();
        return genericParseNumberOrPercentage(ptr, end, number);
    }
    const UChar* ptr = string.characters16();
    const UChar* end = ptr + string.length();
    return genericParseNumberOrPercentage(ptr, end, number);
}

static const LChar skewXDesc[] =  {'s', 'k', 'e', 'w', 'X'};
static const LChar skewYDesc[] =  {'s', 'k', 'e', 'w', 'Y'};
static const LChar scaleDesc[] =  {'s', 'c', 'a', 'l', 'e'};
static const LChar translateDesc[] =  {'t', 'r', 'a', 'n', 's', 'l', 'a', 't', 'e'};
static const LChar rotateDesc[] =  {'r', 'o', 't', 'a', 't', 'e'};
static const LChar matrixDesc[] =  {'m', 'a', 't', 'r', 'i', 'x'};

template<typename CharType>
bool parseAndSkipTransformType(const CharType*& ptr, const CharType* end, SVGTransformType& type)
{
    if (ptr >= end)
        return false;

    if (*ptr == 's') {
        if (skipString(ptr, end, skewXDesc, WTF_ARRAY_LENGTH(skewXDesc)))
            type = SVG_TRANSFORM_SKEWX;
        else if (skipString(ptr, end, skewYDesc, WTF_ARRAY_LENGTH(skewYDesc)))
            type = SVG_TRANSFORM_SKEWY;
        else if (skipString(ptr, end, scaleDesc, WTF_ARRAY_LENGTH(scaleDesc)))
            type = SVG_TRANSFORM_SCALE;
        else
            return false;
    } else if (skipString(ptr, end, translateDesc, WTF_ARRAY_LENGTH(translateDesc)))
        type = SVG_TRANSFORM_TRANSLATE;
    else if (skipString(ptr, end, rotateDesc, WTF_ARRAY_LENGTH(rotateDesc)))
        type = SVG_TRANSFORM_ROTATE;
    else if (skipString(ptr, end, matrixDesc, WTF_ARRAY_LENGTH(matrixDesc)))
        type = SVG_TRANSFORM_MATRIX;
    else
        return false;

    return true;
}

template bool parseAndSkipTransformType(const UChar*& current, const UChar* end, SVGTransformType&);
template bool parseAndSkipTransformType(const LChar*& current, const LChar* end, SVGTransformType&);

SVGTransformType parseTransformType(const String& string)
{
    if (string.isEmpty())
        return SVG_TRANSFORM_UNKNOWN;
    SVGTransformType type = SVG_TRANSFORM_UNKNOWN;
    if (string.is8Bit()) {
        const LChar* ptr = string.characters8();
        const LChar* end = ptr + string.length();
        parseAndSkipTransformType(ptr, end, type);
    } else {
        const UChar* ptr = string.characters16();
        const UChar* end = ptr + string.length();
        parseAndSkipTransformType(ptr, end, type);
    }
    return type;
}

}
