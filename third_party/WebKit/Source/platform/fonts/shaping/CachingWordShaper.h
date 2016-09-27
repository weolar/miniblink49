/*
 * Copyright (C) 2015 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CachingWordShaper_h
#define CachingWordShaper_h


#include "platform/geometry/FloatRect.h"
#include "platform/text/TextRun.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class Font;
class GlyphBuffer;
class SimpleFontData;
class ShapeCache;
struct GlyphData;

class PLATFORM_EXPORT CachingWordShaper final {
public:
    CachingWordShaper();
    ~CachingWordShaper();
    void clear();

    float width(const Font*, const TextRun&,
        HashSet<const SimpleFontData*>* fallbackFonts,
        FloatRect* glyphBounds);
    float fillGlyphBuffer(const Font*, const TextRun&,
        HashSet<const SimpleFontData*>*, GlyphBuffer*,
        unsigned from, unsigned to);
    float fillGlyphBufferForTextEmphasis(const Font*, const TextRun&,
        const GlyphData* emphasisData, GlyphBuffer*,
        unsigned from, unsigned to);
    FloatRect selectionRect(const Font*, const TextRun&, const FloatPoint&,
        int height, unsigned from, unsigned to);

private:
    ShapeCache* m_shapeCache;
};

} // namespace blink

#endif // CachingWordShaper_h
