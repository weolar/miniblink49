/*
 * Copyright (C) 2003, 2006, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Holger Hans Peter Freyther
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#include "config.h"
#include "platform/fonts/shaping/SimpleShaper.h"

#include "platform/fonts/Character.h"
#include "platform/fonts/Font.h"
#include "platform/fonts/GlyphBuffer.h"
#include "platform/fonts/Latin1TextIterator.h"
#include "platform/fonts/SimpleFontData.h"
#include "platform/fonts/UTF16TextIterator.h"
#include "wtf/MathExtras.h"
#include "wtf/text/CharacterNames.h"

using namespace WTF;
using namespace Unicode;

namespace blink {

SimpleShaper::SimpleShaper(const Font* font, const TextRun& run, const GlyphData* emphasisData,
    HashSet<const SimpleFontData*>* fallbackFonts, FloatRect* bounds)
    : Shaper(font, run, emphasisData, fallbackFonts, bounds)
    , m_currentCharacter(0)
    , m_runWidthSoFar(0)
{
    // If the padding is non-zero, count the number of spaces in the run
    // and divide that by the padding for per space addition.
    m_expansion = m_textRun.expansion();
    if (!m_expansion) {
        m_expansionPerOpportunity = 0;
    } else {
        bool isAfterExpansion = m_isAfterExpansion;
        unsigned expansionOpportunityCount = m_textRun.is8Bit() ? Character::expansionOpportunityCount(m_textRun.characters8(), m_textRun.length(), m_textRun.direction(), isAfterExpansion, m_textRun.textJustify()) : Character::expansionOpportunityCount(m_textRun.characters16(), m_textRun.length(), m_textRun.direction(), isAfterExpansion, m_textRun.textJustify());
        if (isAfterExpansion && !m_textRun.allowsTrailingExpansion())
            expansionOpportunityCount--;

        if (!expansionOpportunityCount)
            m_expansionPerOpportunity = 0;
        else
            m_expansionPerOpportunity = m_expansion / expansionOpportunityCount;
    }
}

GlyphData SimpleShaper::glyphDataForCharacter(CharacterData& charData, bool normalizeSpace)
{
    ASSERT(m_font);
    return m_font->glyphDataForCharacter(charData.character, m_textRun.rtl(), normalizeSpace);
}

float SimpleShaper::characterWidth(UChar32 character, const GlyphData& glyphData) const
{
    const SimpleFontData* fontData = glyphData.fontData;
    ASSERT(fontData);

    if (UNLIKELY(character == tabulationCharacter && m_textRun.allowTabs()))
        return m_font->tabWidth(*fontData, m_textRun.tabSize(), m_textRun.xPos() + m_runWidthSoFar);

    float width = fontData->widthForGlyph(glyphData.glyph);

    // SVG uses horizontalGlyphStretch(), when textLength is used to stretch/squeeze text.
    if (UNLIKELY(m_textRun.horizontalGlyphStretch() != 1))
        width *= m_textRun.horizontalGlyphStretch();

    return width;
}

float SimpleShaper::adjustSpacing(float width, const CharacterData& charData)
{
    // Account for letter-spacing.
    if (width)
        width += m_font->fontDescription().letterSpacing();

    bool isExpansionOpportunity = Character::treatAsSpace(charData.character) || (m_textRun.textJustify() == TextJustifyDistribute);
    if (isExpansionOpportunity || (m_textRun.textJustify() == TextJustifyAuto && Character::isCJKIdeographOrSymbol(charData.character))) {
        // Distribute the run's total expansion evenly over all expansion opportunities in the run.
        if (m_expansion) {
            if (!isExpansionOpportunity && !m_isAfterExpansion) {
                // Take the expansion opportunity before this ideograph.
                m_expansion -= m_expansionPerOpportunity;
                m_runWidthSoFar += m_expansionPerOpportunity;
            }
            if (m_textRun.allowsTrailingExpansion()
                || (m_textRun.ltr() && charData.characterOffset + charData.clusterLength < static_cast<size_t>(m_textRun.length()))
                || (m_textRun.rtl() && charData.characterOffset)) {
                m_expansion -= m_expansionPerOpportunity;
                width += m_expansionPerOpportunity;
                m_isAfterExpansion = true;
            }
        } else {
            m_isAfterExpansion = false;
        }

        // Account for word spacing.
        // We apply additional space between "words" by adding width to the space character.
        if (isExpansionOpportunity && (charData.character != tabulationCharacter || !m_textRun.allowTabs())
            && (charData.characterOffset || charData.character == noBreakSpaceCharacter)
            && m_font->fontDescription().wordSpacing()) {
            width += m_font->fontDescription().wordSpacing();
        }
    } else {
        m_isAfterExpansion = false;
    }

    return width;
}

template <typename TextIterator>
unsigned SimpleShaper::advanceInternal(TextIterator& textIterator, GlyphBuffer* glyphBuffer)
{
    bool hasExtraSpacing = (m_font->fontDescription().letterSpacing() || m_font->fontDescription().wordSpacing() || m_expansion)
        && !m_textRun.spacingDisabled();

    const SimpleFontData* lastFontData = m_font->primaryFont();
    bool normalizeSpace = m_textRun.normalizeSpace();
    const float initialRunWidth = m_runWidthSoFar;

    CharacterData charData;
    while (textIterator.consume(charData.character)) {
        charData.characterOffset = textIterator.offset();
        charData.clusterLength = textIterator.glyphLength();
        GlyphData glyphData = glyphDataForCharacter(charData, normalizeSpace);

        // Some fonts do not have a glyph for zero-width-space,
        // in that case use the space character and override the width.
        float width;
        bool spaceUsedAsZeroWidthSpace = false;
        if (!glyphData.glyph && Character::treatAsZeroWidthSpace(charData.character)) {
            charData.character = spaceCharacter;
            glyphData = glyphDataForCharacter(charData);
            width = 0;
            spaceUsedAsZeroWidthSpace = true;
        } else {
            width = characterWidth(charData.character, glyphData);
        }

        Glyph glyph = glyphData.glyph;
        const SimpleFontData* fontData = glyphData.fontData;
        ASSERT(fontData);

        if (m_fallbackFonts && lastFontData != fontData && width) {
            lastFontData = fontData;
            trackNonPrimaryFallbackFont(fontData);
        }

        if (hasExtraSpacing && !spaceUsedAsZeroWidthSpace)
            width = adjustSpacing(width, charData);

        if (m_glyphBoundingBox) {
            ASSERT(glyphData.fontData);
            FloatRect glyphBounds = glyphData.fontData->boundsForGlyph(glyphData.glyph);
            // We are handling simple text run here, so Y-Offset will be zero.
            // FIXME: Computing bounds relative to the initial advance seems odd. Are we adjusting
            // these someplace else? If not, we'll end up with different bounds depending on how
            // we segment our advance() calls.
            glyphBounds.move(m_runWidthSoFar - initialRunWidth, 0);
            m_glyphBoundingBox->unite(glyphBounds);
        }

        if (glyphBuffer) {
            if (!forTextEmphasis()) {
                glyphBuffer->add(glyph, fontData, m_runWidthSoFar);
            } else if (Character::canReceiveTextEmphasis(charData.character)) {
                addEmphasisMark(glyphBuffer, m_runWidthSoFar + width / 2);
            }
        }

        // Advance past the character we just dealt with.
        textIterator.advance();
        m_runWidthSoFar += width;
    }

    unsigned consumedCharacters = textIterator.offset() - m_currentCharacter;
    m_currentCharacter = textIterator.offset();

    return consumedCharacters;
}

unsigned SimpleShaper::advance(unsigned offset, GlyphBuffer* glyphBuffer)
{
    unsigned length = m_textRun.length();

    if (offset > length)
        offset = length;

    if (m_currentCharacter >= offset)
        return 0;

    if (m_textRun.is8Bit()) {
        Latin1TextIterator textIterator(m_textRun.data8(m_currentCharacter), m_currentCharacter, offset);
        return advanceInternal(textIterator, glyphBuffer);
    }

    UTF16TextIterator textIterator(m_textRun.data16(m_currentCharacter), m_currentCharacter, offset, length);
    return advanceInternal(textIterator, glyphBuffer);
}

bool SimpleShaper::advanceOneCharacter(float& width)
{
    float initialWidth = m_runWidthSoFar;

    if (!advance(m_currentCharacter + 1))
        return false;

    width = m_runWidthSoFar - initialWidth;
    return true;
}

} // namespace blink
