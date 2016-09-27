/*
 * Copyright (C) Research In Motion Limited 2010-2012. All rights reserved.
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
 */

#include "config.h"

#include "core/layout/svg/SVGTextMetricsBuilder.h"

#include "core/layout/svg/LayoutSVGInline.h"
#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/layout/svg/LayoutSVGText.h"
#include "core/layout/svg/SVGTextMetrics.h"
#include "platform/fonts/GlyphBuffer.h"
#include "platform/fonts/shaping/SimpleShaper.h"
#include "platform/text/BidiCharacterRun.h"
#include "platform/text/BidiResolver.h"
#include "platform/text/TextDirection.h"
#include "platform/text/TextPath.h"
#include "platform/text/TextRun.h"
#include "platform/text/TextRunIterator.h"
#include "wtf/Vector.h"

namespace blink {

class SVGTextMetricsCalculator {
public:
    SVGTextMetricsCalculator(LayoutSVGInlineText*);
    ~SVGTextMetricsCalculator();

    SVGTextMetrics computeMetricsForCharacter(unsigned textPosition);
    unsigned textLength() const { return static_cast<unsigned>(m_run.charactersLength()); }

    bool characterStartsSurrogatePair(unsigned textPosition) const
    {
        return U16_IS_LEAD(m_run[textPosition]) && textPosition + 1 < textLength() && U16_IS_TRAIL(m_run[textPosition + 1]);
    }
    bool characterIsWhiteSpace(unsigned textPosition) const
    {
        return m_run[textPosition] == ' ';
    }

private:
    void setupBidiRuns();
    SVGTextMetrics computeMetricsForCharacterSimple(unsigned textPosition);
    SVGTextMetrics computeMetricsForCharacterComplex(unsigned textPosition);

    LayoutSVGInlineText* m_text;
    BidiCharacterRun* m_bidiRun;
    TextRun m_run;
    BidiResolver<TextRunIterator, BidiCharacterRun> m_bidiResolver;
    bool m_isComplexText;
    float m_totalWidth;
    TextDirection m_textDirection;

    // Simple text only.
    OwnPtr<SimpleShaper> m_simpleShaper;
};

SVGTextMetricsCalculator::SVGTextMetricsCalculator(LayoutSVGInlineText* text)
    : m_text(text)
    , m_bidiRun(nullptr)
    , m_run(SVGTextMetrics::constructTextRun(text, 0, text->textLength(), text->styleRef().direction()))
    , m_isComplexText(false)
    , m_totalWidth(0)
{
    const Font& scaledFont = text->scaledFont();
    CodePath codePath = scaledFont.codePath(TextRunPaintInfo(m_run));
    m_isComplexText = codePath == ComplexPath;
    m_run.setCodePath(m_isComplexText ? TextRun::ForceComplex : TextRun::ForceSimple);

    if (!m_isComplexText)
        m_simpleShaper = adoptPtr(new SimpleShaper(&scaledFont, m_run));
    else
        setupBidiRuns();
}

SVGTextMetricsCalculator::~SVGTextMetricsCalculator()
{
    if (m_bidiRun)
        m_bidiResolver.runs().deleteRuns();
}

void SVGTextMetricsCalculator::setupBidiRuns()
{
    const ComputedStyle& style = m_text->styleRef();
    m_textDirection = style.direction();
    if (isOverride(style.unicodeBidi()))
        return;

    BidiStatus status(LTR, false);
    status.last = status.lastStrong = WTF::Unicode::OtherNeutral;
    m_bidiResolver.setStatus(status);
    m_bidiResolver.setPositionIgnoringNestedIsolates(TextRunIterator(&m_run, 0));
    const bool hardLineBreak = false;
    const bool reorderRuns = false;
    m_bidiResolver.createBidiRunsForLine(TextRunIterator(&m_run, m_run.length()), NoVisualOverride, hardLineBreak, reorderRuns);
    BidiRunList<BidiCharacterRun>& bidiRuns = m_bidiResolver.runs();
    m_bidiRun = bidiRuns.firstRun();
}

SVGTextMetrics SVGTextMetricsCalculator::computeMetricsForCharacterSimple(unsigned textPosition)
{
    GlyphBuffer glyphBuffer;
    unsigned metricsLength = m_simpleShaper->advance(textPosition + 1, &glyphBuffer);
    if (!metricsLength)
        return SVGTextMetrics();

    float currentWidth = m_simpleShaper->runWidthSoFar() - m_totalWidth;
    m_totalWidth = m_simpleShaper->runWidthSoFar();

    return SVGTextMetrics(m_text, metricsLength, currentWidth);
}

SVGTextMetrics SVGTextMetricsCalculator::computeMetricsForCharacterComplex(unsigned textPosition)
{
    unsigned metricsLength = characterStartsSurrogatePair(textPosition) ? 2 : 1;
    SVGTextMetrics metrics = SVGTextMetrics::measureCharacterRange(m_text, textPosition, metricsLength, m_textDirection);
    ASSERT(metrics.length() == metricsLength);

    unsigned startPosition = m_bidiRun ? m_bidiRun->start() : 0;
    ASSERT(startPosition <= textPosition);
    SVGTextMetrics complexStartToCurrentMetrics = SVGTextMetrics::measureCharacterRange(m_text, startPosition, textPosition - startPosition + metricsLength, m_textDirection);
    // Frequent case for Arabic text: when measuring a single character the arabic isolated form is taken
    // when laying out the glyph "in context" (with it's surrounding characters) it changes due to shaping.
    // So whenever currentWidth != currentMetrics.width(), we are processing a text run whose length is
    // not equal to the sum of the individual lengths of the glyphs, when measuring them isolated.
    float currentWidth = complexStartToCurrentMetrics.width() - m_totalWidth;
    if (currentWidth != metrics.width())
        metrics.setWidth(currentWidth);

    m_totalWidth = complexStartToCurrentMetrics.width();
    return metrics;
}

SVGTextMetrics SVGTextMetricsCalculator::computeMetricsForCharacter(unsigned textPosition)
{
    if (m_bidiRun) {
        if (textPosition >= static_cast<unsigned>(m_bidiRun->stop())) {
            m_bidiRun = m_bidiRun->next();
            // New BiDi run means new reference position for measurements, so reset |m_totalWidth|.
            m_totalWidth = 0;
        }
        ASSERT(m_bidiRun);
        ASSERT(static_cast<int>(textPosition) < m_bidiRun->stop());
        m_textDirection = m_bidiRun->direction();
    }

    if (m_isComplexText)
        return computeMetricsForCharacterComplex(textPosition);

    return computeMetricsForCharacterSimple(textPosition);
}

struct MeasureTextData {
    MeasureTextData(SVGCharacterDataMap* characterDataMap)
        : allCharactersMap(characterDataMap)
        , lastCharacterWasWhiteSpace(true)
        , valueListPosition(0)
    {
    }

    SVGCharacterDataMap* allCharactersMap;
    bool lastCharacterWasWhiteSpace;
    unsigned valueListPosition;
};

static void measureTextLayoutObject(LayoutSVGInlineText* text, MeasureTextData* data, bool processLayoutObject)
{
    ASSERT(text);

    SVGTextLayoutAttributes* attributes = text->layoutAttributes();
    Vector<SVGTextMetrics>* textMetricsValues = &attributes->textMetricsValues();
    if (processLayoutObject) {
        if (data->allCharactersMap)
            attributes->clear();
        else
            textMetricsValues->clear();
    }

    SVGTextMetricsCalculator calculator(text);
    bool preserveWhiteSpace = text->style()->whiteSpace() == PRE;
    unsigned surrogatePairCharacters = 0;
    unsigned skippedCharacters = 0;
    unsigned textPosition = 0;
    unsigned textLength = calculator.textLength();

    SVGTextMetrics currentMetrics;
    for (; textPosition < textLength; textPosition += currentMetrics.length()) {
        currentMetrics = calculator.computeMetricsForCharacter(textPosition);
        if (!currentMetrics.length())
            break;

        bool characterIsWhiteSpace = calculator.characterIsWhiteSpace(textPosition);
        if (characterIsWhiteSpace && !preserveWhiteSpace && data->lastCharacterWasWhiteSpace) {
            if (processLayoutObject)
                textMetricsValues->append(SVGTextMetrics(SVGTextMetrics::SkippedSpaceMetrics));
            if (data->allCharactersMap)
                skippedCharacters += currentMetrics.length();
            continue;
        }

        if (processLayoutObject) {
            if (data->allCharactersMap) {
                const SVGCharacterDataMap::const_iterator it = data->allCharactersMap->find(data->valueListPosition + textPosition - skippedCharacters - surrogatePairCharacters + 1);
                if (it != data->allCharactersMap->end())
                    attributes->characterDataMap().set(textPosition + 1, it->value);
            }
            textMetricsValues->append(currentMetrics);
        }

        if (data->allCharactersMap && calculator.characterStartsSurrogatePair(textPosition))
            surrogatePairCharacters++;

        data->lastCharacterWasWhiteSpace = characterIsWhiteSpace;
    }

    if (!data->allCharactersMap)
        return;

    data->valueListPosition += textPosition - skippedCharacters;
}

static void walkTree(LayoutSVGText* start, LayoutSVGInlineText* stopAtLeaf, MeasureTextData* data)
{
    LayoutObject* child = start->firstChild();
    while (child) {
        if (child->isSVGInlineText()) {
            LayoutSVGInlineText* text = toLayoutSVGInlineText(child);
            measureTextLayoutObject(text, data, !stopAtLeaf || stopAtLeaf == text);
            if (stopAtLeaf && stopAtLeaf == text)
                return;
        } else if (child->isSVGInline()) {
            // Visit children of text content elements.
            if (LayoutObject* inlineChild = toLayoutSVGInline(child)->firstChild()) {
                child = inlineChild;
                continue;
            }
        }
        child = child->nextInPreOrderAfterChildren(start);
    }
}

void SVGTextMetricsBuilder::measureTextLayoutObject(LayoutSVGInlineText* text)
{
    ASSERT(text);

    LayoutSVGText* textRoot = LayoutSVGText::locateLayoutSVGTextAncestor(text);
    if (!textRoot)
        return;

    MeasureTextData data(0);
    walkTree(textRoot, text, &data);
}

void SVGTextMetricsBuilder::buildMetricsAndLayoutAttributes(LayoutSVGText* textRoot, LayoutSVGInlineText* stopAtLeaf, SVGCharacterDataMap& allCharactersMap)
{
    ASSERT(textRoot);
    MeasureTextData data(&allCharactersMap);
    walkTree(textRoot, stopAtLeaf, &data);
}

}
