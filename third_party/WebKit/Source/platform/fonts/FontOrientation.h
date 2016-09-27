/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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

#ifndef FontOrientation_h
#define FontOrientation_h

#include "platform/fonts/Character.h"

namespace blink {

enum class FontOrientation {
    // Horizontal; i.e., writing-mode: horizontal-tb
    Horizontal = 0,
    // Baseline is vertical but use rotated horizontal typography; i.e., writing-mode: vertical-*; text-orientation: sideways-*
    VerticalRotated = 1,
    // Vertical with upright CJK and rotated non-CJK; i.e., writing-mode: vertical-*, text-orientation: mixed
    VerticalMixed = 2,
    // Vertical with all upright; i.e., writing-mode: vertical-*, text-orientation: upright
    VerticalUpright = 3,

    BitCount = 2,

    AnyUprightMask = 2,
};

inline bool operator&(FontOrientation value, FontOrientation mask) { return static_cast<unsigned>(value) & static_cast<unsigned>(mask); }
inline bool isVerticalAnyUpright(FontOrientation orientation) { return orientation & FontOrientation::AnyUprightMask; }
inline bool isVerticalNonCJKUpright(FontOrientation orientation) { return orientation == FontOrientation::VerticalUpright; }
inline bool isVerticalUpright(FontOrientation orientation, UChar32 character)
{
    return orientation == FontOrientation::VerticalUpright
        || (orientation == FontOrientation::VerticalMixed && Character::isUprightInMixedVertical(character));
}
inline bool isVerticalBaseline(FontOrientation orientation) { return orientation != FontOrientation::Horizontal; }

} // namespace blink

#endif // FontOrientation_h
