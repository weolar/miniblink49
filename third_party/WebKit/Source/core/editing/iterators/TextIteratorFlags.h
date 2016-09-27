/*
 * Copyright (C) 2004, 2006, 2009 Apple Inc. All rights reserved.
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

#ifndef TextIteratorFlags_h
#define TextIteratorFlags_h

namespace blink {

enum TextIteratorBehavior {
    TextIteratorDefaultBehavior = 0,
    TextIteratorEmitsCharactersBetweenAllVisiblePositions = 1 << 0,
    TextIteratorEntersTextControls = 1 << 1,
    TextIteratorIgnoresStyleVisibility = 1 << 2,
    TextIteratorEmitsOriginalText = 1 << 3,
    TextIteratorStopsOnFormControls = 1 << 4,
    TextIteratorEmitsImageAltText = 1 << 5,
    TextIteratorEntersOpenShadowRoots = 1 << 6,
    TextIteratorEmitsObjectReplacementCharacter = 1 << 7,
    TextIteratorDoesNotBreakAtReplacedElement = 1 << 8,
    TextIteratorForInnerText = 1 << 9,
    TextIteratorForSelectionToString = 1 << 10,
    TextIteratorForWindowFind = 1 << 11,
};
typedef unsigned TextIteratorBehaviorFlags;

} // namespace blink

#endif // TextIteratorFlags_h
