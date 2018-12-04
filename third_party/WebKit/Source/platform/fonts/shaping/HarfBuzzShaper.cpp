/*
 * Copyright (c) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013 BlackBerry Limited. All rights reserved.
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

#ifndef MINIBLINK_NO_HARFBUZZ
#include "config.h"
#include "platform/fonts/shaping/HarfBuzzShaper.h"

#include "third_party/harfbuzz-ng/src/hb.h"
#include "platform/LayoutUnit.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/fonts/Character.h"
#include "platform/fonts/Font.h"
#include "platform/fonts/GlyphBuffer.h"
#include "platform/fonts/UTF16TextIterator.h"
#include "platform/fonts/shaping/HarfBuzzFace.h"
#include "platform/text/TextBreakIterator.h"
#include "wtf/Compiler.h"
#include "wtf/MathExtras.h"
#include "wtf/text/Unicode.h"
#include "wtf/ASCIICType.h"

#include <list>
#include <map>
#include <string>
// #include <unicode/normlzr.h>
// #include <unicode/uchar.h>
// #include <unicode/uscript.h>

namespace blink {

struct HarfBuzzRunGlyphData {
    uint16_t glyph;
    uint16_t characterIndex;
    float advance;
    FloatSize offset;
};

struct ShapeResult::RunInfo {
    RunInfo(const SimpleFontData* font, hb_direction_t dir, hb_script_t script,
        unsigned startIndex, unsigned numGlyphs, unsigned numCharacters)
        : m_fontData(font), m_direction(dir), m_script(script)
        , m_startIndex(startIndex), m_numCharacters(numCharacters)
        , m_numGlyphs(numGlyphs)
    {
        m_glyphData.resize(m_numGlyphs);
    }

    bool rtl() const { return HB_DIRECTION_IS_BACKWARD(m_direction); }
    float xPositionForVisualOffset(unsigned offset) const;
    float xPositionForOffset(unsigned) const;
    int characterIndexForXPosition(float) const;
    void setGlyphAndPositions(unsigned index, uint16_t glyphId, float advance,
        float offsetX, float offsetY);

    void addAdvance(unsigned index, float advance)
    {
        m_glyphData[index].advance += advance;
    }

    size_t glyphToCharacterIndex(size_t i) const
    {
        return m_startIndex + m_glyphData[i].characterIndex;
    }

    const SimpleFontData* m_fontData;
    hb_direction_t m_direction;
    hb_script_t m_script;
    Vector<HarfBuzzRunGlyphData> m_glyphData;
    unsigned m_startIndex;
    unsigned m_numCharacters;
    unsigned m_numGlyphs;
    float m_width;
};

float ShapeResult::RunInfo::xPositionForVisualOffset(unsigned offset) const
{
    ASSERT(offset < m_numCharacters);
    if (rtl())
        offset = m_numCharacters - offset - 1;
    return xPositionForOffset(offset);
}

float ShapeResult::RunInfo::xPositionForOffset(unsigned offset) const
{
    ASSERT(offset < m_numCharacters);
    unsigned glyphIndex = 0;
    float position = 0;
    if (m_direction == HB_DIRECTION_RTL) {
        while (glyphIndex < m_numGlyphs && m_glyphData[glyphIndex].characterIndex > offset) {
            position += m_glyphData[glyphIndex].advance;
            ++glyphIndex;
        }
        // For RTL, we need to return the right side boundary of the character.
        // Add advance of glyphs which are part of the character.
        while (glyphIndex < m_numGlyphs - 1 && m_glyphData[glyphIndex].characterIndex == m_glyphData[glyphIndex + 1].characterIndex) {
            position += m_glyphData[glyphIndex].advance;
            ++glyphIndex;
        }
        position += m_glyphData[glyphIndex].advance;
    } else {
        while (glyphIndex < m_numGlyphs && m_glyphData[glyphIndex].characterIndex < offset) {
            position += m_glyphData[glyphIndex].advance;
            ++glyphIndex;
        }
    }
    return position;
}

int ShapeResult::RunInfo::characterIndexForXPosition(float targetX) const
{
    ASSERT(targetX <= m_width);
    float currentX = 0;
    float currentAdvance = m_glyphData[0].advance;
    unsigned glyphIndex = 0;

    // Sum up advances that belong to the first character.
    while (glyphIndex < m_numGlyphs - 1 && m_glyphData[glyphIndex].characterIndex == m_glyphData[glyphIndex + 1].characterIndex)
        currentAdvance += m_glyphData[++glyphIndex].advance;
    currentAdvance = currentAdvance / 2.0;
    if (targetX <= currentAdvance)
        return m_direction == HB_DIRECTION_RTL ? m_numCharacters : 0;

    currentX = currentAdvance;
    ++glyphIndex;
    while (glyphIndex < m_numGlyphs) {
        unsigned prevCharacterIndex = m_glyphData[glyphIndex - 1].characterIndex;
        float prevAdvance = currentAdvance;
        currentAdvance = m_glyphData[glyphIndex].advance;
        while (glyphIndex < m_numGlyphs - 1 && m_glyphData[glyphIndex].characterIndex == m_glyphData[glyphIndex + 1].characterIndex)
            currentAdvance += m_glyphData[++glyphIndex].advance;
        currentAdvance = currentAdvance / 2.0;
        float nextX = currentX + prevAdvance + currentAdvance;
        if (currentX <= targetX && targetX <= nextX)
            return m_direction == HB_DIRECTION_RTL ? prevCharacterIndex : m_glyphData[glyphIndex].characterIndex;
        currentX = nextX;
        ++glyphIndex;
    }

    return m_direction == HB_DIRECTION_RTL ? 0 : m_numCharacters;
}

void ShapeResult::RunInfo::setGlyphAndPositions(unsigned index,
    uint16_t glyphId, float advance, float offsetX, float offsetY)
{
    HarfBuzzRunGlyphData& data = m_glyphData[index];
    data.glyph = glyphId;
    data.advance = advance;
    data.offset = FloatSize(offsetX, offsetY);
}

ShapeResult::~ShapeResult()
{
    unsigned destroyed = 0;
    for (unsigned i = 0; i < m_runs.size(); i++) {
        if (m_runs[i]) {
            delete m_runs[i];
            destroyed++;
        }
    }
}

static inline void addGlyphToBuffer(GlyphBuffer* glyphBuffer, float advance,
    hb_direction_t direction, const SimpleFontData* fontData,
    const HarfBuzzRunGlyphData& glyphData)
{
    FloatPoint startOffset = HB_DIRECTION_IS_HORIZONTAL(direction)
        ? FloatPoint(advance, 0)
        : FloatPoint(0, advance);
    glyphBuffer->add(glyphData.glyph, fontData, startOffset + glyphData.offset);
}

template<TextDirection direction>
float ShapeResult::fillGlyphBufferForRun(GlyphBuffer* glyphBuffer,
    const RunInfo* run, float initialAdvance, unsigned from, unsigned to,
    unsigned runOffset)
{
    if (!run)
        return 0;
    float advanceSoFar = initialAdvance;
    unsigned numGlyphs = run->m_numGlyphs;
    for (unsigned i = 0; i < numGlyphs; ++i) {
        const HarfBuzzRunGlyphData& glyphData = run->m_glyphData[i];
        uint16_t currentCharacterIndex = run->m_startIndex +
            glyphData.characterIndex + runOffset;
        if ((direction == RTL && currentCharacterIndex >= to)
            || (direction == LTR && currentCharacterIndex < from)) {
            advanceSoFar += glyphData.advance;
        } else if ((direction == RTL && currentCharacterIndex >= from)
            || (direction == LTR && currentCharacterIndex < to)) {
            addGlyphToBuffer(glyphBuffer, advanceSoFar, run->m_direction,
                run->m_fontData, glyphData);
            advanceSoFar += glyphData.advance;
        }
    }
    return advanceSoFar - initialAdvance;
}

static inline unsigned countGraphemesInCluster(const UChar* str,
    unsigned strLength, uint16_t startIndex, uint16_t endIndex)
{
    if (startIndex > endIndex) {
        uint16_t tempIndex = startIndex;
        startIndex = endIndex;
        endIndex = tempIndex;
    }
    uint16_t length = endIndex - startIndex;
    ASSERT(static_cast<unsigned>(startIndex + length) <= strLength);
    TextBreakIterator* cursorPosIterator = cursorMovementIterator(&str[startIndex], length);

    int cursorPos = cursorPosIterator->current();
    int numGraphemes = -1;
    while (0 <= cursorPos) {
        cursorPos = cursorPosIterator->next();
        numGraphemes++;
    }
    return numGraphemes < 0 ? 0 : numGraphemes;
}

static inline void addEmphasisMark(GlyphBuffer* buffer,
    const GlyphData* emphasisData, FloatPoint glyphCenter,
    float midGlyphOffset)
{
    ASSERT(buffer);
    ASSERT(emphasisData);

    const SimpleFontData* emphasisFontData = emphasisData->fontData;
    ASSERT(emphasisFontData);

    bool isVertical = emphasisFontData->platformData().isVerticalAnyUpright()
        && emphasisFontData->verticalData();

    if (!isVertical) {
        buffer->add(emphasisData->glyph, emphasisFontData,
            midGlyphOffset - glyphCenter.x());
    } else {
        buffer->add(emphasisData->glyph, emphasisFontData,
            FloatPoint(-glyphCenter.x(), midGlyphOffset - glyphCenter.y()));
    }
}

float ShapeResult::fillGlyphBufferForTextEmphasisRun(GlyphBuffer* glyphBuffer,
    const RunInfo* run, const TextRun& textRun, const GlyphData* emphasisData,
    float initialAdvance, unsigned from, unsigned to, unsigned runOffset)
{
    if (!run)
        return 0;

    unsigned graphemesInCluster = 1;
    float clusterAdvance = 0;

    FloatPoint glyphCenter = emphasisData->fontData->
        boundsForGlyph(emphasisData->glyph).center();

    TextDirection direction = textRun.direction();

    // A "cluster" in this context means a cluster as it is used by HarfBuzz:
    // The minimal group of characters and corresponding glyphs, that cannot be broken
    // down further from a text shaping point of view.
    // A cluster can contain multiple glyphs and grapheme clusters, with mutually
    // overlapping boundaries. Below we count grapheme clusters per HarfBuzz clusters,
    // then linearly split the sum of corresponding glyph advances by the number of
    // grapheme clusters in order to find positions for emphasis mark drawing.
    uint16_t clusterStart = direction == RTL
        ? (uint16_t)(run->m_startIndex + run->m_numCharacters + runOffset)
        : (uint16_t)(run->glyphToCharacterIndex(0) + runOffset);

    float advanceSoFar = initialAdvance;
    unsigned numGlyphs = run->m_numGlyphs;
    for (unsigned i = 0; i < numGlyphs; ++i) {
        const HarfBuzzRunGlyphData& glyphData = run->m_glyphData[i];
        uint16_t currentCharacterIndex = run->m_startIndex + glyphData.characterIndex + runOffset;
        bool isRunEnd = (i + 1 == numGlyphs);
        bool isClusterEnd =  isRunEnd || (run->glyphToCharacterIndex(i + 1) + runOffset != currentCharacterIndex);

        if ((direction == RTL && currentCharacterIndex >= to) || (direction != RTL && currentCharacterIndex < from)) {
            advanceSoFar += glyphData.advance;
            direction == RTL ? --clusterStart : ++clusterStart;
            continue;
        }

        clusterAdvance += glyphData.advance;

        if (textRun.is8Bit()) {
            float glyphAdvanceX = glyphData.advance;
            if (Character::canReceiveTextEmphasis(textRun[currentCharacterIndex])) {
                addEmphasisMark(glyphBuffer, emphasisData, glyphCenter, advanceSoFar + glyphAdvanceX / 2);
            }
            advanceSoFar += glyphAdvanceX;
        } else if (isClusterEnd) {
            uint16_t clusterEnd;
            if (direction == RTL)
                clusterEnd = currentCharacterIndex;
            else
                clusterEnd = (uint16_t)(isRunEnd ? run->m_startIndex + run->m_numCharacters + runOffset : run->glyphToCharacterIndex(i + 1) + runOffset);

            graphemesInCluster = countGraphemesInCluster(textRun.characters16(), textRun.charactersLength(), clusterStart, clusterEnd);
            if (!graphemesInCluster || !clusterAdvance)
                continue;

            float glyphAdvanceX = clusterAdvance / graphemesInCluster;
            for (unsigned j = 0; j < graphemesInCluster; ++j) {
                // Do not put emphasis marks on space, separator, and control characters.
                if (Character::canReceiveTextEmphasis(textRun[currentCharacterIndex]))
                    addEmphasisMark(glyphBuffer, emphasisData, glyphCenter, advanceSoFar + glyphAdvanceX / 2);
                advanceSoFar += glyphAdvanceX;
            }
            clusterStart = clusterEnd;
            clusterAdvance = 0;
        }
    }
    return advanceSoFar - initialAdvance;
}

float ShapeResult::fillGlyphBuffer(Vector<RefPtr<ShapeResult>>& results,
    GlyphBuffer* glyphBuffer, const TextRun& textRun,
    unsigned from, unsigned to)
{
    float advance = 0;
    if (textRun.rtl()) {
        unsigned wordOffset = textRun.length();
        for (unsigned j = 0; j < results.size(); j++) {
            unsigned resolvedIndex = results.size() - 1 - j;
            RefPtr<ShapeResult>& wordResult = results[resolvedIndex];
            for (unsigned i = 0; i < wordResult->m_runs.size(); i++) {
                advance += wordResult->fillGlyphBufferForRun<RTL>(glyphBuffer,
                    wordResult->m_runs[i], advance, from, to,
                    wordOffset - wordResult->numCharacters());
            }
            wordOffset -= wordResult->numCharacters();
        }
    } else {
        unsigned wordOffset = 0;
        for (unsigned j = 0; j < results.size(); j++) {
            RefPtr<ShapeResult>& wordResult = results[j];
            for (unsigned i = 0; i < wordResult->m_runs.size(); i++) {
                advance += wordResult->fillGlyphBufferForRun<LTR>(glyphBuffer,
                    wordResult->m_runs[i], advance, from, to, wordOffset);
            }
            wordOffset += wordResult->numCharacters();
        }
    }

    return advance;
}

float ShapeResult::fillGlyphBufferForTextEmphasis(
    Vector<RefPtr<ShapeResult>>& results, GlyphBuffer* glyphBuffer,
    const TextRun& textRun, const GlyphData* emphasisData,
    unsigned from, unsigned to)
{
    float advance = 0;
    unsigned wordOffset = textRun.rtl() ? textRun.length() : 0;
    for (unsigned j = 0; j < results.size(); j++) {
        unsigned resolvedIndex = textRun.rtl() ? results.size() - 1 - j : j;
        RefPtr<ShapeResult>& wordResult = results[resolvedIndex];
        for (unsigned i = 0; i < wordResult->m_runs.size(); i++) {
            unsigned resolvedOffset = wordOffset -
                (textRun.rtl() ? wordResult->numCharacters() : 0);
            advance += wordResult->fillGlyphBufferForTextEmphasisRun(
                glyphBuffer, wordResult->m_runs[i], textRun, emphasisData,
                advance, from, to, resolvedOffset);
        }
        wordOffset += wordResult->numCharacters() * (textRun.rtl() ? -1 : 1);
    }

    return advance;
}

FloatRect ShapeResult::selectionRect(Vector<RefPtr<ShapeResult>>& results,
    TextDirection direction, float totalWidth, const FloatPoint& point,
    int height, unsigned absoluteFrom, unsigned absoluteTo)
{
    float currentX = 0;
    float fromX = 0;
    float toX = 0;
    bool foundFromX = false;
    bool foundToX = false;

    if (direction == RTL)
        currentX = totalWidth;

    // The absoluteFrom and absoluteTo arguments represent the start/end offset
    // for the entire run, from/to are continuously updated to be relative to
    // the current word (ShapeResult instance).
    int from = absoluteFrom;
    int to = absoluteTo;

    unsigned totalNumCharacters = 0;
    for (unsigned j = 0; j < results.size(); j++) {
        RefPtr<ShapeResult> result = results[j];
        if (direction == RTL) {
            // Convert logical offsets to visual offsets, because results are in
            // logical order while runs are in visual order.
            if (!foundFromX && from >= 0 && static_cast<unsigned>(from) < result->numCharacters())
                from = result->numCharacters() - from - 1;
            if (!foundToX && to >= 0 && static_cast<unsigned>(to) < result->numCharacters())
                to = result->numCharacters() - to - 1;
            currentX -= result->width();
        }
        for (unsigned i = 0; i < result->m_runs.size(); i++) {
            if (!result->m_runs[i])
                continue;
            ASSERT((direction == RTL) == result->m_runs[i]->rtl());
            int numCharacters = result->m_runs[i]->m_numCharacters;
            if (!foundFromX && from >= 0 && from < numCharacters) {
                fromX = result->m_runs[i]->xPositionForVisualOffset(from) + currentX;
                foundFromX = true;
            } else {
                from -= numCharacters;
            }

            if (!foundToX && to >= 0 && to < numCharacters) {
                toX = result->m_runs[i]->xPositionForVisualOffset(to) + currentX;
                foundToX = true;
            } else {
                to -= numCharacters;
            }

            if (foundFromX && foundToX)
                break;
            currentX += result->m_runs[i]->m_width;
        }
        if (direction == RTL)
            currentX -= result->width();
        totalNumCharacters += result->numCharacters();
    }

    // The position in question might be just after the text.
    if (!foundFromX && absoluteFrom == totalNumCharacters) {
        fromX = direction == RTL ? 0 : totalWidth;
        foundFromX = true;
    }
    if (!foundToX && absoluteTo == totalNumCharacters) {
        toX = direction == RTL ? 0 : totalWidth;
        foundToX = true;
    }
    if (!foundFromX)
        fromX = 0;
    if (!foundToX)
        toX = direction == RTL ? 0 : totalWidth;

    // None of our runs is part of the selection, possibly invalid arguments.
    if (!foundToX && !foundFromX)
        fromX = toX = 0;
    if (fromX < toX)
        return FloatRect(point.x() + fromX, point.y(), toX - fromX, height);
    return FloatRect(point.x() + toX, point.y(), fromX - toX, height);
}

int ShapeResult::offsetForPosition(float targetX)
{
    int charactersSoFar = 0;
    float currentX = 0;

    if (m_direction == RTL) {
        charactersSoFar = m_numCharacters;
        for (unsigned i = 0; i < m_runs.size(); ++i) {
            if (!m_runs[i])
                continue;
            charactersSoFar -= m_runs[i]->m_numCharacters;
            float nextX = currentX + m_runs[i]->m_width;
            float offsetForRun = targetX - currentX;
            if (offsetForRun >= 0 && offsetForRun <= m_runs[i]->m_width) {
                // The x value in question is within this script run.
                const unsigned index = m_runs[i]->characterIndexForXPosition(offsetForRun);
                return charactersSoFar + index;
            }
            currentX = nextX;
        }
    } else {
        for (unsigned i = 0; i < m_runs.size(); ++i) {
            if (!m_runs[i])
                continue;
            float nextX = currentX + m_runs[i]->m_width;
            float offsetForRun = targetX - currentX;
            if (offsetForRun >= 0 && offsetForRun <= m_runs[i]->m_width) {
                const unsigned index = m_runs[i]->characterIndexForXPosition(offsetForRun);
                return charactersSoFar + index;
            }
            charactersSoFar += m_runs[i]->m_numCharacters;
            currentX = nextX;
        }
    }

    return charactersSoFar;
}

unsigned ShapeResult::numberOfRunsForTesting() const
{
    return m_runs.size();
}

bool ShapeResult::runInfoForTesting(unsigned runIndex, unsigned& startIndex,
    unsigned& numGlyphs, hb_script_t& script)
{
    if (runIndex < m_runs.size() && m_runs[runIndex]) {
        startIndex = m_runs[runIndex]->m_startIndex;
        numGlyphs = m_runs[runIndex]->m_numGlyphs;
        script = m_runs[runIndex]->m_script;
        return true;
    }
    return false;
}

uint16_t ShapeResult::glyphForTesting(unsigned runIndex, size_t glyphIndex)
{
    return m_runs[runIndex]->m_glyphData[glyphIndex].glyph;
}

float ShapeResult::advanceForTesting(unsigned runIndex, size_t glyphIndex)
{
    return m_runs[runIndex]->m_glyphData[glyphIndex].advance;
}

template<typename T>
class HarfBuzzScopedPtr {
public:
    typedef void (*DestroyFunction)(T*);

    HarfBuzzScopedPtr(T* ptr, DestroyFunction destroy)
        : m_ptr(ptr)
        , m_destroy(destroy)
    {
        ASSERT(m_destroy);
    }
    ~HarfBuzzScopedPtr()
    {
        if (m_ptr)
            (*m_destroy)(m_ptr);
    }

    T* get() { return m_ptr; }
    void set(T* ptr) { m_ptr = ptr; }
private:
    T* m_ptr;
    DestroyFunction m_destroy;
};

static inline float harfBuzzPositionToFloat(hb_position_t value)
{
    return static_cast<float>(value) / (1 << 16);
}

inline HarfBuzzShaper::HarfBuzzRun::HarfBuzzRun(const SimpleFontData* fontData, unsigned startIndex, unsigned numCharacters, hb_direction_t direction, hb_script_t script)
    : m_fontData(fontData)
    , m_startIndex(startIndex)
    , m_numCharacters(numCharacters)
    , m_direction(direction)
    , m_script(script)
{
}

inline HarfBuzzShaper::HarfBuzzRun::HarfBuzzRun(const HarfBuzzRun& rhs)
    : m_fontData(rhs.m_fontData)
    , m_startIndex(rhs.m_startIndex)
    , m_numCharacters(rhs.m_numCharacters)
    , m_direction(rhs.m_direction)
    , m_script(rhs.m_script)
{
}

HarfBuzzShaper::HarfBuzzRun::~HarfBuzzRun()
{
}

#define U16_APPEND(s, i, capacity, c, isError) { \
    if((uint32_t)(c)<=0xffff) { \
        (s)[(i)++]=(uint16_t)(c); \
    } else if((uint32_t)(c)<=0x10ffff && (i)+1<(capacity)) { \
        (s)[(i)++]=(uint16_t)(((c)>>10)+0xd7c0); \
        (s)[(i)++]=(uint16_t)(((c)&0x3ff)|0xdc00); \
    } else /* c>0x10ffff or not enough space */ { \
        (isError)=TRUE; \
    } \
}

static void normalizeCharacters(const TextRun& run, unsigned length, UChar* destination, unsigned* destinationLength)
{
    unsigned position = 0;
    bool error = false;
    const UChar* source;
    String stringFor8BitRun;
    if (run.is8Bit()) {
        stringFor8BitRun = String::make16BitFrom8BitSource(run.characters8(), run.length());
        source = stringFor8BitRun.characters16();
    } else {
        source = run.characters16();
    }

    *destinationLength = 0;
    while (position < length) {
        UChar32 character;
        U16_NEXT(source, position, length, character);
        // Don't normalize tabs as they are not treated as spaces for word-end.
        if (run.normalizeSpace() && Character::isNormalizedCanvasSpaceCharacter(character))
            character = spaceCharacter;
        else if (Character::treatAsSpace(character) && character != tabulationCharacter)
            character = spaceCharacter;
        else if (Character::treatAsZeroWidthSpaceInComplexScript(character))
            character = zeroWidthSpaceCharacter;

        U16_APPEND(destination, *destinationLength, length, character, error);
        ASSERT_UNUSED(error, !error);
    }
}

HarfBuzzShaper::HarfBuzzShaper(const Font* font, const TextRun& run,
    HashSet<const SimpleFontData*>* fallbackFonts)
    : Shaper(font, run, nullptr, fallbackFonts)
    , m_normalizedBufferLength(0)
    , m_wordSpacingAdjustment(font->fontDescription().wordSpacing())
    , m_letterSpacing(font->fontDescription().letterSpacing())
    , m_expansionOpportunityCount(0)
{
    m_normalizedBuffer = adoptArrayPtr(new UChar[m_textRun.length() + 1]);
    normalizeCharacters(m_textRun, m_textRun.length(), m_normalizedBuffer.get(), &m_normalizedBufferLength);
    setExpansion(m_textRun.expansion());
    setFontFeatures();
}

float HarfBuzzShaper::nextExpansionPerOpportunity()
{
    if (!m_expansionOpportunityCount) {
        ASSERT_NOT_REACHED(); // failures indicate that the logic in HarfBuzzShaper does not match to the one in expansionOpportunityCount()
        return 0;
    }
    if (!--m_expansionOpportunityCount) {
        float remaining = m_expansion;
        m_expansion = 0;
        return remaining;
    }
    m_expansion -= m_expansionPerOpportunity;
    return m_expansionPerOpportunity;
}

// setPadding sets a number of pixels to be distributed across the TextRun.
// WebKit uses this to justify text.
void HarfBuzzShaper::setExpansion(float padding)
{
    m_expansion = padding;
    if (!m_expansion)
        return;

    // If we have padding to distribute, then we try to give an equal
    // amount to each expansion opportunity.
    bool isAfterExpansion = m_isAfterExpansion;
    m_expansionOpportunityCount = Character::expansionOpportunityCount(m_normalizedBuffer.get(), m_normalizedBufferLength, m_textRun.direction(), isAfterExpansion, m_textRun.textJustify());
    if (isAfterExpansion && !m_textRun.allowsTrailingExpansion()) {
        ASSERT(m_expansionOpportunityCount > 0);
        --m_expansionOpportunityCount;
    }

    if (m_expansionOpportunityCount)
        m_expansionPerOpportunity = m_expansion / m_expansionOpportunityCount;
    else
        m_expansionPerOpportunity = 0;
}

void HarfBuzzShaper::setFontFeatures()
{
    const FontDescription& description = m_font->fontDescription();

    static hb_feature_t noKern = { HB_TAG('k', 'e', 'r', 'n'), 0, 0, static_cast<unsigned>(-1) };
    static hb_feature_t noVkrn = { HB_TAG('v', 'k', 'r', 'n'), 0, 0, static_cast<unsigned>(-1) };
    switch (description.kerning()) {
    case FontDescription::NormalKerning:
        // kern/vkrn are enabled by default
        break;
    case FontDescription::NoneKerning:
        m_features.append(description.isVerticalAnyUpright() ? noVkrn : noKern);
        break;
    case FontDescription::AutoKerning:
        break;
    }

    static hb_feature_t noClig = { HB_TAG('c', 'l', 'i', 'g'), 0, 0, static_cast<unsigned>(-1) };
    static hb_feature_t noLiga = { HB_TAG('l', 'i', 'g', 'a'), 0, 0, static_cast<unsigned>(-1) };
    switch (description.commonLigaturesState()) {
    case FontDescription::DisabledLigaturesState:
        m_features.append(noLiga);
        m_features.append(noClig);
        break;
    case FontDescription::EnabledLigaturesState:
        // liga and clig are on by default
        break;
    case FontDescription::NormalLigaturesState:
        break;
    }
    static hb_feature_t dlig = { HB_TAG('d', 'l', 'i', 'g'), 1, 0, static_cast<unsigned>(-1) };
    switch (description.discretionaryLigaturesState()) {
    case FontDescription::DisabledLigaturesState:
        // dlig is off by default
        break;
    case FontDescription::EnabledLigaturesState:
        m_features.append(dlig);
        break;
    case FontDescription::NormalLigaturesState:
        break;
    }
    static hb_feature_t hlig = { HB_TAG('h', 'l', 'i', 'g'), 1, 0, static_cast<unsigned>(-1) };
    switch (description.historicalLigaturesState()) {
    case FontDescription::DisabledLigaturesState:
        // hlig is off by default
        break;
    case FontDescription::EnabledLigaturesState:
        m_features.append(hlig);
        break;
    case FontDescription::NormalLigaturesState:
        break;
    }
    static hb_feature_t noCalt = { HB_TAG('c', 'a', 'l', 't'), 0, 0, static_cast<unsigned>(-1) };
    switch (description.contextualLigaturesState()) {
    case FontDescription::DisabledLigaturesState:
        m_features.append(noCalt);
        break;
    case FontDescription::EnabledLigaturesState:
        // calt is on by default
        break;
    case FontDescription::NormalLigaturesState:
        break;
    }

    static hb_feature_t hwid = { HB_TAG('h', 'w', 'i', 'd'), 1, 0, static_cast<unsigned>(-1) };
    static hb_feature_t twid = { HB_TAG('t', 'w', 'i', 'd'), 1, 0, static_cast<unsigned>(-1) };
    static hb_feature_t qwid = { HB_TAG('q', 'w', 'i', 'd'), 1, 0, static_cast<unsigned>(-1) };
    switch (description.widthVariant()) {
    case HalfWidth:
        m_features.append(hwid);
        break;
    case ThirdWidth:
        m_features.append(twid);
        break;
    case QuarterWidth:
        m_features.append(qwid);
        break;
    case RegularWidth:
        break;
    }

    FontFeatureSettings* settings = description.featureSettings();
    if (!settings)
        return;

    unsigned numFeatures = settings->size();
    for (unsigned i = 0; i < numFeatures; ++i) {
        hb_feature_t feature;
        const AtomicString& tag = settings->at(i).tag();
        feature.tag = HB_TAG(tag[0], tag[1], tag[2], tag[3]);
        feature.value = settings->at(i).value();
        feature.start = 0;
        feature.end = static_cast<unsigned>(-1);
        m_features.append(feature);
    }
}

PassRefPtr<ShapeResult> HarfBuzzShaper::shapeResult()
{
    if (!createHarfBuzzRuns())
        return nullptr;

    ShapeResult* result = new ShapeResult();
    result->m_numCharacters = m_normalizedBufferLength;
    result->m_direction = m_textRun.direction();

    if (!shapeHarfBuzzRuns(result)) {
        delete result;
        return nullptr;
    }

    return adoptRef(result);
}

struct CandidateRun {
    UChar32 character;
    unsigned start;
    unsigned end;
    const SimpleFontData* fontData;
    UScriptCode script;
};

static const int32_t countPropsVectors = 5520;
static const int32_t propsVectorsColumns = 3;

#define UPROPS_SCRIPT_MASK 0x000000ff
#define UPROPS_SCRIPT_X_MASK 0x00c000ff
#define UPROPS_SCRIPT_X_WITH_COMMON 0x400000
#define UPROPS_SCRIPT_X_WITH_INHERITED 0x800000
#define UPROPS_SCRIPT_X_WITH_OTHER 0xc00000
#define U_FAILURE(x) ((x)>U_ZERO_ERROR)

UScriptCode uscript_getScript(UChar32 c, UErrorCode *pErrorCode)
{
    if (pErrorCode == NULL || U_FAILURE(*pErrorCode))
        return USCRIPT_INVALID_CODE;
    
    if ((uint32_t)c > 0x10ffff) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        return USCRIPT_INVALID_CODE;
    }

    return USCRIPT_COMMON;
}

UBool uscript_hasScript(UChar32 c, UScriptCode sc)
{
    return false;
}

static inline bool collectCandidateRuns(const UChar* normalizedBuffer,
    size_t bufferLength, const Font* font, Vector<CandidateRun>* runs, bool isSpaceNormalize)
{
    UTF16TextIterator iterator(normalizedBuffer, bufferLength);
    UChar32 character;
    unsigned startIndexOfCurrentRun = 0;

    if (!iterator.consume(character))
        return false;

    const SimpleFontData* nextFontData = font->glyphDataForCharacter(character, false, isSpaceNormalize).fontData;
    UErrorCode errorCode = U_ZERO_ERROR;
    UScriptCode nextScript = uscript_getScript(character, &errorCode);
    if (U_FAILURE(errorCode))
        return false;

    do {
        const UChar* currentCharacterPosition = iterator.characters();
        const SimpleFontData* currentFontData = nextFontData;
        UScriptCode currentScript = nextScript;

        UChar32 lastCharacter = character;
        for (iterator.advance(); iterator.consume(character); iterator.advance()) {
            if (Character::treatAsZeroWidthSpace(character))
                continue;
            if ((U_GET_GC_MASK(character) & U_GC_M_MASK)
                && (Character::isUnicodeVariationSelector(character)
                    || currentFontData->canRenderCombiningCharacterSequence(
                        currentCharacterPosition,
                        iterator.glyphEnd() - currentCharacterPosition)))
                continue;

            nextFontData = font->glyphDataForCharacter(character, false, isSpaceNormalize).fontData;
            nextScript = uscript_getScript(character, &errorCode);
            if (U_FAILURE(errorCode))
                return false;
            if (lastCharacter == zeroWidthJoinerCharacter)
                currentFontData = nextFontData;
            if ((nextFontData != currentFontData) || ((currentScript != nextScript) && (nextScript != USCRIPT_INHERITED) && (!uscript_hasScript(character, currentScript))))
                break;
            currentCharacterPosition = iterator.characters();
            lastCharacter = character;
        }

        CandidateRun run = { lastCharacter, startIndexOfCurrentRun, static_cast<unsigned>(iterator.offset()), currentFontData, currentScript };
        runs->append(run);

        startIndexOfCurrentRun = iterator.offset();
    } while (iterator.consume(character));

    return true;
}

static inline bool matchesAdjacentRun(UScriptCode* scriptExtensions, int length,
    CandidateRun& adjacentRun)
{
    for (int i = 0; i < length; i++) {
        if (scriptExtensions[i] == adjacentRun.script)
            return true;
    }
    return false;
}

static inline void resolveRunBasedOnScriptExtensions(Vector<CandidateRun>& runs,
    CandidateRun& run, size_t i, size_t length, UScriptCode* scriptExtensions,
    int extensionsLength, size_t& nextResolvedRun)
{
    // If uscript_getScriptExtensions returns 1 it only contains the script value,
    // we only care about ScriptExtensions which is indicated by a value >= 2.
    if (extensionsLength <= 1)
        return;

    if (i > 0 && matchesAdjacentRun(scriptExtensions, extensionsLength, runs[i - 1])) {
        run.script = runs[i - 1].script;
        return;
    }

    for (size_t j = i + 1; j < length; j++) {
        if (runs[j].script != USCRIPT_COMMON
            && runs[j].script != USCRIPT_INHERITED
            && matchesAdjacentRun(scriptExtensions, extensionsLength, runs[j])) {
            nextResolvedRun = j;
            break;
        }
    }
}

static inline void resolveRunBasedOnScriptValue(Vector<CandidateRun>& runs,
    CandidateRun& run, size_t i, size_t length, size_t& nextResolvedRun)
{
    if (run.script != USCRIPT_COMMON)
        return;

    if (i > 0 && runs[i - 1].script != USCRIPT_COMMON) {
        run.script = runs[i - 1].script;
        return;
    }

    for (size_t j = i + 1; j < length; j++) {
        if (runs[j].script != USCRIPT_COMMON
            && runs[j].script != USCRIPT_INHERITED) {
            nextResolvedRun = j;
            break;
        }
    }
}

int32_t uscript_getScriptExtensions(UChar32 c, UScriptCode *scripts, int32_t capacity, UErrorCode *pErrorCode)
{
    uint32_t scriptX = 25;
    if (pErrorCode == NULL || U_FAILURE(*pErrorCode))
        return 0;
    
    if (capacity < 0 || (capacity>0 && scripts == NULL)) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }

    if (capacity == 0) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
    } else {
        scripts[0] = (UScriptCode)scriptX;
    }
    return 1;
}

static inline bool resolveCandidateRuns(Vector<CandidateRun>& runs)
{
    UScriptCode scriptExtensions[USCRIPT_CODE_LIMIT];
    UErrorCode errorCode = U_ZERO_ERROR;
    size_t length = runs.size();
    size_t nextResolvedRun = 0;
    for (size_t i = 0; i < length; i++) {
        CandidateRun& run = runs[i];
        nextResolvedRun = 0;

        if (run.script == USCRIPT_INHERITED)
            run.script = i > 0 ? runs[i - 1].script : USCRIPT_COMMON;

        int extensionsLength = uscript_getScriptExtensions(run.character, scriptExtensions, sizeof(scriptExtensions) / sizeof(scriptExtensions[0]), &errorCode);
        if (U_FAILURE(errorCode))
            return false;

        resolveRunBasedOnScriptExtensions(runs, run, i, length, scriptExtensions, extensionsLength, nextResolvedRun);
        resolveRunBasedOnScriptValue(runs, run, i, length, nextResolvedRun);
        for (size_t j = i; j < nextResolvedRun; j++)
            runs[j].script = runs[nextResolvedRun].script;

        i = std::max(i, nextResolvedRun);
    }
    return true;
}

// For ideographic (CJK) documents, 90-95% of calls from width() are one character length
// because most characters have break opportunities both before and after.
bool HarfBuzzShaper::createHarfBuzzRunsForSingleCharacter()
{
    ASSERT(m_normalizedBufferLength == 1);
    UChar32 character = m_normalizedBuffer[0];
    if (!U16_IS_SINGLE(character))
        return false;
    const SimpleFontData* fontData = m_font->glyphDataForCharacter(character, false, m_textRun.normalizeSpace()).fontData;
    UErrorCode errorCode = U_ZERO_ERROR;
    UScriptCode script = uscript_getScript(character, &errorCode);
    if (U_FAILURE(errorCode))
        return false;
    addHarfBuzzRun(0, 1, fontData, script);

    return true;
}

bool HarfBuzzShaper::createHarfBuzzRuns()
{
    if (m_normalizedBufferLength == 1)
        return createHarfBuzzRunsForSingleCharacter();

    Vector<CandidateRun> candidateRuns;
    if (!collectCandidateRuns(m_normalizedBuffer.get(), m_normalizedBufferLength, m_font, &candidateRuns, m_textRun.normalizeSpace()))
        return false;

    if (!resolveCandidateRuns(candidateRuns))
        return false;

    size_t length = candidateRuns.size();
    for (size_t i = 0; i < length; ) {
        CandidateRun& run = candidateRuns[i];
        CandidateRun lastMatchingRun = run;
        for (i++; i < length; i++) {
            if (candidateRuns[i].script != run.script
                || candidateRuns[i].fontData != run.fontData)
                break;
            lastMatchingRun = candidateRuns[i];
        }
        addHarfBuzzRun(run.start, lastMatchingRun.end, run.fontData, run.script);
    }
    return !m_harfBuzzRuns.isEmpty();
}

const char* uscript_getShortName(UScriptCode scriptCode)
{
    if (USCRIPT_LATIN == scriptCode)
        return "Latn";
    return "Zyyy";
}

// A port of hb_icu_script_to_script because harfbuzz on CrOS is built
// without hb-icu. See http://crbug.com/356929
static inline hb_script_t ICUScriptToHBScript(UScriptCode script)
{
    if (UNLIKELY(script == USCRIPT_INVALID_CODE))
        return HB_SCRIPT_INVALID;

    return hb_script_from_string(uscript_getShortName(script), -1);
    return HB_SCRIPT_INVALID;
}

static inline hb_direction_t TextDirectionToHBDirection(TextDirection dir, FontOrientation orientation, const SimpleFontData* fontData)
{
    hb_direction_t harfBuzzDirection = isVerticalAnyUpright(orientation) && !fontData->isTextOrientationFallback() ? HB_DIRECTION_TTB : HB_DIRECTION_LTR;
    return dir == RTL ? HB_DIRECTION_REVERSE(harfBuzzDirection) : harfBuzzDirection;
}

void HarfBuzzShaper::addHarfBuzzRun(unsigned startCharacter,
    unsigned endCharacter, const SimpleFontData* fontData,
    UScriptCode script)
{
    ASSERT(endCharacter > startCharacter);
    ASSERT(script != USCRIPT_INVALID_CODE);
    if (m_fallbackFonts)
        trackNonPrimaryFallbackFont(fontData);
    return m_harfBuzzRuns.append(HarfBuzzRun::create(fontData,
        startCharacter, endCharacter - startCharacter,
        TextDirectionToHBDirection(m_textRun.direction(), m_font->fontDescription().orientation(), fontData),
        ICUScriptToHBScript(script)));
}

static const uint16_t* toUint16(const UChar* src)
{
    // FIXME: This relies on undefined behavior however it works on the
    // current versions of all compilers we care about and avoids making
    // a copy of the string.
    static_assert(sizeof(UChar) == sizeof(uint16_t), "UChar should be the same size as uint16_t");
    return reinterpret_cast<const uint16_t*>(src);
}

static inline void addToHarfBuzzBufferInternal(hb_buffer_t* buffer,
    const FontDescription& fontDescription, const UChar* normalizedBuffer,
    unsigned startIndex, unsigned numCharacters)
{
    if (fontDescription.variant() == FontVariantSmallCaps &&  isASCIILower(normalizedBuffer[startIndex])) {
        String upperText = String(normalizedBuffer + startIndex, numCharacters).upper();
        // TextRun is 16 bit, therefore upperText is 16 bit, even after we call makeUpper().
        ASSERT(!upperText.is8Bit());
        hb_buffer_add_utf16(buffer, toUint16(upperText.characters16()),
            numCharacters, 0, numCharacters);
    } else {
        hb_buffer_add_utf16(buffer, toUint16(normalizedBuffer + startIndex),
            numCharacters, 0, numCharacters);
    }
}

bool HarfBuzzShaper::shapeHarfBuzzRuns(ShapeResult* result)
{
    HarfBuzzScopedPtr<hb_buffer_t> harfBuzzBuffer(hb_buffer_create(), hb_buffer_destroy);

    const FontDescription& fontDescription = m_font->fontDescription();
    const String& localeString = fontDescription.locale();
    CString locale = localeString.latin1();
    const hb_language_t language = hb_language_from_string(locale.data(), locale.length());

    result->m_runs.resize(m_harfBuzzRuns.size());
    for (unsigned i = 0; i < m_harfBuzzRuns.size(); ++i) {
        unsigned runIndex = m_textRun.rtl() ? m_harfBuzzRuns.size() - i - 1 : i;
        HarfBuzzRun* currentRun = m_harfBuzzRuns[runIndex].get();

        const SimpleFontData* currentFontData = currentRun->fontData();
        FontPlatformData* platformData = const_cast<FontPlatformData*>(&currentFontData->platformData());
        HarfBuzzFace* face = platformData->harfBuzzFace();
        if (!face)
            return false;

        hb_buffer_set_language(harfBuzzBuffer.get(), language);
        hb_buffer_set_script(harfBuzzBuffer.get(), currentRun->script());
        hb_buffer_set_direction(harfBuzzBuffer.get(), currentRun->direction());

        // Add a space as pre-context to the buffer. This prevents showing dotted-circle
        // for combining marks at the beginning of runs.
        static const uint16_t preContext = spaceCharacter;
        hb_buffer_add_utf16(harfBuzzBuffer.get(), &preContext, 1, 1, 0);

        addToHarfBuzzBufferInternal(harfBuzzBuffer.get(),
            fontDescription, m_normalizedBuffer.get(), currentRun->startIndex(),
            currentRun->numCharacters());

        if (fontDescription.isVerticalAnyUpright())
            face->setScriptForVerticalGlyphSubstitution(harfBuzzBuffer.get());

        HarfBuzzScopedPtr<hb_font_t> harfBuzzFont(face->createFont(), hb_font_destroy);
        hb_shape(harfBuzzFont.get(), harfBuzzBuffer.get(), m_features.isEmpty() ? 0 : m_features.data(), m_features.size());
        shapeResult(result, i, currentRun, harfBuzzBuffer.get());

        hb_buffer_reset(harfBuzzBuffer.get());
    }

    // We should have consumed all expansion opportunities.
    // Failures here means that our logic does not match to the one in expansionOpportunityCount().
    // FIXME: Ideally, we should ASSERT(!m_expansionOpportunityCount) here to ensure that,
    // or unify the two logics (and the one in SimplePath too,) but there are some cases where our impl
    // does not support justification very well yet such as U+3099, and it'll cause the ASSERT to fail.
    // It's to be fixed because they're very rarely used, and a broken justification is still somewhat readable.

    return true;
}

void HarfBuzzShaper::shapeResult(ShapeResult* result, unsigned index,
    HarfBuzzRun* currentRun, hb_buffer_t* harfBuzzBuffer)
{
    unsigned numGlyphs = hb_buffer_get_length(harfBuzzBuffer);
    if (!numGlyphs) {
        result->m_runs[index] = nullptr;
        return;
    }

    ShapeResult::RunInfo* run = new ShapeResult::RunInfo(currentRun->fontData(),
        currentRun->direction(), currentRun->script(), currentRun->startIndex(),
        numGlyphs, currentRun->numCharacters());
    result->m_runs[index] = run;
    result->m_numGlyphs += numGlyphs;

    const SimpleFontData* currentFontData = currentRun->fontData();
    hb_glyph_info_t* glyphInfos = hb_buffer_get_glyph_infos(harfBuzzBuffer, 0);
    hb_glyph_position_t* glyphPositions = hb_buffer_get_glyph_positions(harfBuzzBuffer, 0);

    float totalAdvance = 0;
    FloatPoint glyphOrigin;
    float offsetX, offsetY;
    float* directionOffset = m_font->fontDescription().isVerticalAnyUpright() ? &offsetY : &offsetX;

    // HarfBuzz returns result in visual order, no need to flip for RTL.
    for (size_t i = 0; i < numGlyphs; ++i) {
        bool runEnd = i + 1 == numGlyphs;
        uint16_t glyph = glyphInfos[i].codepoint;
        offsetX = harfBuzzPositionToFloat(glyphPositions[i].x_offset);
        offsetY = -harfBuzzPositionToFloat(glyphPositions[i].y_offset);

        // One out of x_advance and y_advance is zero, depending on
        // whether the buffer direction is horizontal or vertical.
        float advance = harfBuzzPositionToFloat(glyphPositions[i].x_advance - glyphPositions[i].y_advance);
        unsigned currentCharacterIndex = currentRun->startIndex() + glyphInfos[i].cluster;
        RELEASE_ASSERT(m_normalizedBufferLength > currentCharacterIndex);
        bool isClusterEnd = runEnd || glyphInfos[i].cluster != glyphInfos[i + 1].cluster;
        float spacing = 0;

        run->m_glyphData[i].characterIndex = glyphInfos[i].cluster;

        if (isClusterEnd)
            spacing += adjustSpacing(run, i, currentCharacterIndex, *directionOffset, totalAdvance);

        if (currentFontData->isZeroWidthSpaceGlyph(glyph)) {
            run->setGlyphAndPositions(i, glyph, 0, 0, 0);
            continue;
        }

        advance += spacing;
        if (m_textRun.rtl()) {
            // In RTL, spacing should be added to left side of glyphs.
            *directionOffset += spacing;
            if (!isClusterEnd)
                *directionOffset += m_letterSpacing;
        }

        run->setGlyphAndPositions(i, glyph, advance, offsetX, offsetY);
        totalAdvance += advance;

        FloatRect glyphBounds = currentFontData->boundsForGlyph(glyph);
        glyphBounds.move(glyphOrigin.x(), glyphOrigin.y());
        result->m_glyphBoundingBox.unite(glyphBounds);
        glyphOrigin += FloatSize(advance + offsetX, offsetY);
    }

    run->m_width = totalAdvance > 0.0 ? totalAdvance : 0.0;
    result->m_width += run->m_width;
}

float HarfBuzzShaper::adjustSpacing(ShapeResult::RunInfo* run, size_t glyphIndex, unsigned currentCharacterIndex, float& offset, float& totalAdvance)
{
    float spacing = 0;
    UChar32 character = m_normalizedBuffer[currentCharacterIndex];
    if (m_letterSpacing && !Character::treatAsZeroWidthSpace(character))
        spacing += m_letterSpacing;

    bool treatAsSpace = Character::treatAsSpace(character);
    if (treatAsSpace && currentCharacterIndex && (character != '\t' || !m_textRun.allowTabs()))
        spacing += m_wordSpacingAdjustment;

    if (!m_expansionOpportunityCount)
        return spacing;

    if (treatAsSpace) {
        spacing += nextExpansionPerOpportunity();
        m_isAfterExpansion = true;
        return spacing;
    }

    if (m_textRun.textJustify() != TextJustify::TextJustifyAuto) {
        m_isAfterExpansion = false;
        return spacing;
    }

    // isCJKIdeographOrSymbol() has expansion opportunities both before and after each character.
    // http://www.w3.org/TR/jlreq/#line_adjustment
    if (U16_IS_LEAD(character) && currentCharacterIndex + 1 < m_normalizedBufferLength && U16_IS_TRAIL(m_normalizedBuffer[currentCharacterIndex + 1]))
        character = U16_GET_SUPPLEMENTARY(character, m_normalizedBuffer[currentCharacterIndex + 1]);
    if (!Character::isCJKIdeographOrSymbol(character)) {
        m_isAfterExpansion = false;
        return spacing;
    }

    if (!m_isAfterExpansion) {
        // Take the expansion opportunity before this ideograph.
        float expandBefore = nextExpansionPerOpportunity();
        if (expandBefore) {
            if (glyphIndex > 0) {
                run->addAdvance(glyphIndex - 1, expandBefore);
                totalAdvance += expandBefore;
            } else {
                offset += expandBefore;
                spacing += expandBefore;
            }
        }
        if (!m_expansionOpportunityCount)
            return spacing;
    }

    // Don't need to check m_textRun.allowsTrailingExpansion() since it's covered by !m_expansionOpportunityCount above
    spacing += nextExpansionPerOpportunity();
    m_isAfterExpansion = true;
    return spacing;
}

} // namespace blink

#endif