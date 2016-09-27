/*
 * Copyright (C) 2007, 2009, 2010 Apple Inc. All rights reserved.
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

#ifndef CharacterNames_h
#define CharacterNames_h

#include "wtf/text/Unicode.h"

namespace WTF {
namespace Unicode {

// Names here are taken from the Unicode standard.

// Most of these are UChar constants, not UChar32, which makes them
// more convenient for WebCore code that mostly uses UTF-16.

const UChar activateArabicFormShapingCharacter = 0x206D;
const UChar activateSymmetricSwappingCharacter = 0x206B;
const UChar32 aegeanWordSeparatorLineCharacter = 0x10100;
const UChar32 aegeanWordSeparatorDotCharacter = 0x10101;
const UChar arabicLetterMarkCharacter = 0x061C;
const UChar blackCircleCharacter = 0x25CF;
const UChar blackSquareCharacter = 0x25A0;
const UChar blackUpPointingTriangleCharacter = 0x25B2;
const UChar bulletCharacter = 0x2022;
const UChar bullseyeCharacter = 0x25CE;
const UChar carriageReturnCharacter = 0x000D;
const UChar tabulationCharacter = 0x0009;
const UChar ethiopicPrefaceColonCharacter = 0x1366;
const UChar ethiopicWordspaceCharacter = 0x1361;
const UChar firstStrongIsolateCharacter = 0x2068;
const UChar fisheyeCharacter = 0x25C9;
const UChar fullstopCharacter = 0x002E;
const UChar hebrewPunctuationGereshCharacter = 0x05F3;
const UChar hebrewPunctuationGershayimCharacter = 0x05F4;
const UChar hiraganaLetterSmallACharacter = 0x3041;
const UChar horizontalEllipsisCharacter = 0x2026;
const UChar hyphenCharacter = 0x2010;
const UChar hyphenMinusCharacter = 0x002D;
const UChar ideographicCommaCharacter = 0x3001;
const UChar ideographicFullStopCharacter = 0x3002;
#if defined(USING_SYSTEM_ICU)
const UChar ideographicSpaceCharacter = 0x3000;
#endif
const UChar inhibitArabicFormShapingCharacter = 0x206C;
const UChar inhibitSymmetricSwappingCharacter = 0x206A;
const UChar latinCapitalLetterIWithDotAbove = 0x0130;
const UChar latinSmallLetterDotlessI = 0x0131;
const UChar leftDoubleQuotationMarkCharacter = 0x201C;
const UChar leftSingleQuotationMarkCharacter = 0x2018;
const UChar leftToRightEmbedCharacter = 0x202A;
const UChar leftToRightIsolateCharacter = 0x2066;
const UChar leftToRightMarkCharacter = 0x200E;
const UChar leftToRightOverrideCharacter = 0x202D;
const UChar lineSeparator = 0x2028;
const UChar minusSignCharacter = 0x2212;
const UChar newlineCharacter = 0x000A;
const UChar nationalDigitShapesCharacter = 0x206E;
const UChar nominalDigitShapesCharacter = 0x206F;
const UChar noBreakSpaceCharacter = 0x00A0;
const UChar objectReplacementCharacter = 0xFFFC;
const UChar paragraphSeparator = 0x2029;
const UChar popDirectionalFormattingCharacter = 0x202C;
const UChar popDirectionalIsolateCharacter = 0x2069;
const UChar replacementCharacter = 0xFFFD;
const UChar rightDoubleQuotationMarkCharacter = 0x201D;
const UChar rightSingleQuotationMarkCharacter = 0x2019;
const UChar rightToLeftEmbedCharacter = 0x202B;
const UChar rightToLeftIsolateCharacter = 0x2067;
const UChar rightToLeftMarkCharacter = 0x200F;
const UChar rightToLeftOverrideCharacter = 0x202E;
const UChar sesameDotCharacter = 0xFE45;
const UChar smallLetterSharpSCharacter = 0x00DF;
const UChar softHyphenCharacter = 0x00AD;
const UChar spaceCharacter = 0x0020;
const UChar tibetanMarkIntersyllabicTshegCharacter = 0x0F0B;
const UChar tibetanMarkDelimiterTshegBstarCharacter = 0x0F0C;
const UChar32 ugariticWordDividerCharacter = 0x1039F;
const UChar whiteBulletCharacter = 0x25E6;
const UChar whiteCircleCharacter = 0x25CB;
const UChar whiteSesameDotCharacter = 0xFE46;
const UChar whiteUpPointingTriangleCharacter = 0x25B3;
const UChar yenSignCharacter = 0x00A5;
const UChar zeroWidthJoinerCharacter = 0x200D;
const UChar zeroWidthNonJoinerCharacter = 0x200C;
const UChar zeroWidthSpaceCharacter = 0x200B;
const UChar zeroWidthNoBreakSpaceCharacter = 0xFEFF;

} // namespace Unicode
} // namespace WTF

using WTF::Unicode::aegeanWordSeparatorLineCharacter;
using WTF::Unicode::aegeanWordSeparatorDotCharacter;
using WTF::Unicode::activateArabicFormShapingCharacter;
using WTF::Unicode::activateSymmetricSwappingCharacter;
using WTF::Unicode::arabicLetterMarkCharacter;
using WTF::Unicode::blackCircleCharacter;
using WTF::Unicode::blackSquareCharacter;
using WTF::Unicode::blackUpPointingTriangleCharacter;
using WTF::Unicode::bulletCharacter;
using WTF::Unicode::bullseyeCharacter;
using WTF::Unicode::carriageReturnCharacter;
using WTF::Unicode::tabulationCharacter;
using WTF::Unicode::ethiopicPrefaceColonCharacter;
using WTF::Unicode::ethiopicWordspaceCharacter;
using WTF::Unicode::firstStrongIsolateCharacter;
using WTF::Unicode::fisheyeCharacter;
using WTF::Unicode::fullstopCharacter;
using WTF::Unicode::hebrewPunctuationGereshCharacter;
using WTF::Unicode::hebrewPunctuationGershayimCharacter;
using WTF::Unicode::hiraganaLetterSmallACharacter;
using WTF::Unicode::horizontalEllipsisCharacter;
using WTF::Unicode::hyphenCharacter;
using WTF::Unicode::hyphenMinusCharacter;
using WTF::Unicode::ideographicCommaCharacter;
using WTF::Unicode::ideographicFullStopCharacter;
#if defined(USING_SYSTEM_ICU)
using WTF::Unicode::ideographicSpaceCharacter;
#endif
using WTF::Unicode::inhibitArabicFormShapingCharacter;
using WTF::Unicode::inhibitSymmetricSwappingCharacter;
using WTF::Unicode::latinCapitalLetterIWithDotAbove;
using WTF::Unicode::latinSmallLetterDotlessI;
using WTF::Unicode::leftDoubleQuotationMarkCharacter;
using WTF::Unicode::leftSingleQuotationMarkCharacter;
using WTF::Unicode::leftToRightEmbedCharacter;
using WTF::Unicode::leftToRightIsolateCharacter;
using WTF::Unicode::leftToRightMarkCharacter;
using WTF::Unicode::leftToRightOverrideCharacter;
using WTF::Unicode::lineSeparator;
using WTF::Unicode::minusSignCharacter;
using WTF::Unicode::newlineCharacter;
using WTF::Unicode::nationalDigitShapesCharacter;
using WTF::Unicode::nominalDigitShapesCharacter;
using WTF::Unicode::noBreakSpaceCharacter;
using WTF::Unicode::objectReplacementCharacter;
using WTF::Unicode::paragraphSeparator;
using WTF::Unicode::popDirectionalFormattingCharacter;
using WTF::Unicode::popDirectionalIsolateCharacter;
using WTF::Unicode::replacementCharacter;
using WTF::Unicode::rightDoubleQuotationMarkCharacter;
using WTF::Unicode::rightSingleQuotationMarkCharacter;
using WTF::Unicode::rightToLeftEmbedCharacter;
using WTF::Unicode::rightToLeftIsolateCharacter;
using WTF::Unicode::rightToLeftMarkCharacter;
using WTF::Unicode::rightToLeftOverrideCharacter;
using WTF::Unicode::sesameDotCharacter;
using WTF::Unicode::smallLetterSharpSCharacter;
using WTF::Unicode::softHyphenCharacter;
using WTF::Unicode::spaceCharacter;
using WTF::Unicode::tibetanMarkIntersyllabicTshegCharacter;
using WTF::Unicode::tibetanMarkDelimiterTshegBstarCharacter;
using WTF::Unicode::ugariticWordDividerCharacter;
using WTF::Unicode::whiteBulletCharacter;
using WTF::Unicode::whiteCircleCharacter;
using WTF::Unicode::whiteSesameDotCharacter;
using WTF::Unicode::whiteUpPointingTriangleCharacter;
using WTF::Unicode::yenSignCharacter;
using WTF::Unicode::zeroWidthJoinerCharacter;
using WTF::Unicode::zeroWidthNonJoinerCharacter;
using WTF::Unicode::zeroWidthSpaceCharacter;
using WTF::Unicode::zeroWidthNoBreakSpaceCharacter;

#endif // CharacterNames_h
