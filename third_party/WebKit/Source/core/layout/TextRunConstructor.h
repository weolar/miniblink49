/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2007 David Smith (catfish.man@gmail.com)
 * Copyright (C) 2003-2013 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TextRunConstructor_h
#define TextRunConstructor_h

#include "platform/text/TextDirection.h"
#include "platform/text/TextRun.h"
#include "wtf/text/Unicode.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Font;
class LayoutObject;
class ComputedStyle;
class LayoutText;

enum TextRunFlag {
    DefaultTextRunFlags = 0,
    RespectDirection = 1 << 0,
    RespectDirectionOverride = 1 << 1
};

typedef unsigned TextRunFlags;

// Direction resolved from string value.
TextRun constructTextRun(LayoutObject* context, const Font&, const String&, const ComputedStyle&, TextRunFlags = DefaultTextRunFlags);
TextRun constructTextRun(LayoutObject* context, const Font&, const LayoutText*, unsigned offset, unsigned length, const ComputedStyle&);

// Explicit direction.
TextRun constructTextRun(LayoutObject*, const Font&, const String&, const ComputedStyle&, TextDirection, TextRunFlags = DefaultTextRunFlags);
TextRun constructTextRun(LayoutObject*, const Font&, const LayoutText*, const ComputedStyle&, TextDirection);
TextRun constructTextRun(LayoutObject*, const Font&, const LayoutText*, unsigned offset, unsigned length, const ComputedStyle&, TextDirection);
TextRun constructTextRun(LayoutObject*, const Font&, const LChar*, int length, const ComputedStyle&, TextDirection);
TextRun constructTextRun(LayoutObject*, const Font&, const UChar*, int length, const ComputedStyle&, TextDirection);

} // namespace blink

#endif
