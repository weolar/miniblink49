/*
 * Copyright (c) 2015 Google Inc. All rights reserved.
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

#include "config.h"
#include "platform/fonts/shaping/Shaper.h"

#include "platform/fonts/GlyphBuffer.h"
#include "platform/fonts/GlyphPage.h"
#include "platform/text/TextRun.h"

namespace blink {

Shaper::Shaper(const Font* font, const TextRun& run, const GlyphData* emphasisData,
    HashSet<const SimpleFontData*>* fallbackFonts, FloatRect* bounds)
    : m_font(font)
    , m_textRun(run)
    , m_fallbackFonts(fallbackFonts)
    , m_glyphBoundingBox(bounds)
    , m_expansion(0)
    , m_expansionPerOpportunity(0)
    , m_isAfterExpansion(!run.allowsLeadingExpansion())
    , m_emphasisSubstitutionData(emphasisData)
{
    if (emphasisData) {
        ASSERT(emphasisData->fontData);
        m_emphasisGlyphCenter = emphasisData->fontData->boundsForGlyph(emphasisData->glyph).center();
    }
}

void Shaper::addEmphasisMark(GlyphBuffer* buffer, float midGlyphOffset) const
{
    ASSERT(buffer);
    ASSERT(m_emphasisSubstitutionData);

    const SimpleFontData* emphasisFontData = m_emphasisSubstitutionData->fontData;
    ASSERT(emphasisFontData);

    bool isVertical = emphasisFontData->platformData().isVerticalAnyUpright()
        && emphasisFontData->verticalData();

    if (!isVertical) {
        buffer->add(m_emphasisSubstitutionData->glyph, emphasisFontData,
            midGlyphOffset - m_emphasisGlyphCenter.x());
    } else {
        buffer->add(m_emphasisSubstitutionData->glyph, emphasisFontData,
            FloatPoint(-m_emphasisGlyphCenter.x(), midGlyphOffset - m_emphasisGlyphCenter.y()));
    }
}

} // namespace blink
