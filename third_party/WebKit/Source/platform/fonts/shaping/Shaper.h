/*
 * Copyright (C) 2015 Google Inc. All rights reserved.
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

#ifndef Shaper_h
#define Shaper_h

#include "platform/PlatformExport.h"
#include "platform/fonts/Font.h"
#include "wtf/HashSet.h"

namespace blink {

class FloatRect;
class GlyphBuffer;
class SimpleFontData;
class TextRun;

struct GlyphData;

class PLATFORM_EXPORT Shaper {
protected:
    Shaper(const Font*, const TextRun&, const GlyphData* emphasisData = nullptr,
        HashSet<const SimpleFontData*>* fallbackFonts = nullptr, FloatRect* = nullptr);

    void trackNonPrimaryFallbackFont(const SimpleFontData*);

    bool forTextEmphasis() const { return m_emphasisSubstitutionData; }
    void addEmphasisMark(GlyphBuffer*, float midGlyphOffset) const;

    const Font* m_font;
    const TextRun& m_textRun;
    HashSet<const SimpleFontData*>* m_fallbackFonts;
    FloatRect* m_glyphBoundingBox;

    float m_expansion; // Pixels to be distributed over the line at word breaks.
    float m_expansionPerOpportunity; // Pixels to be added to each expansion opportunity.
    bool m_isAfterExpansion;

private:
    // Emphasis substitution info. If specified, this will be used to replace all glyphs which
    // can receive text emphasis with center-aligned emphasis glyphs.
    const GlyphData* m_emphasisSubstitutionData;
    FloatPoint m_emphasisGlyphCenter;
};

inline void Shaper::trackNonPrimaryFallbackFont(const SimpleFontData* fontData)
{
    ASSERT(m_fallbackFonts);

    if (fontData == m_font->primaryFont())
        return;

    m_fallbackFonts->add(fontData);
}

} // namespace blink

#endif // Shaper_h
