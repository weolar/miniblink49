/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#ifndef FontSize_h
#define FontSize_h

#include "core/CSSValueKeywords.h"

namespace blink {

class Document;

enum ESmartMinimumForFontSize { DoNotUseSmartMinimumForFontSize, UseSmartMinimumForFontFize };

class FontSize {
private:
    FontSize()
    {
    }

public:
    static float getComputedSizeFromSpecifiedSize(const Document*, float zoomFactor, bool isAbsoluteSize, float specifiedSize, ESmartMinimumForFontSize = UseSmartMinimumForFontFize);

    // Given a CSS keyword in the range (xx-small to -webkit-xxx-large), this function returns
    // values from '1' to '8'.
    static unsigned keywordSize(CSSValueID valueID)
    {
        ASSERT(valueID >= CSSValueXxSmall && valueID <= CSSValueWebkitXxxLarge);
        return valueID - CSSValueXxSmall + 1;
    }

    static unsigned initialKeywordSize() { return 4; } // CSSValueMedium

    // Given a keyword size in the range (1 to 8), this function will return
    // the correct font size scaled relative to the user's default (4).
    static float fontSizeForKeyword(const Document*, unsigned keyword, bool isMonospace);

    // Given a font size in pixel, this function will return legacy font size between 1 and 7.
    static int legacyFontSize(const Document*, int pixelFontSize, bool isMonospace);
};

} // namespace blink

#endif // FontSize_h
