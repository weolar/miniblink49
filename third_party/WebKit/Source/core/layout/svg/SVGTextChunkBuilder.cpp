/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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
#include "core/layout/svg/SVGTextChunkBuilder.h"

#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/layout/svg/line/SVGInlineTextBox.h"
#include "core/svg/SVGLengthContext.h"
#include "core/svg/SVGTextContentElement.h"
#include "platform/transforms/AffineTransform.h"

namespace blink {

namespace {

float calculateTextAnchorShift(const ComputedStyle& style, float length)
{
    bool isLTR = style.isLeftToRightDirection();
    switch (style.svgStyle().textAnchor()) {
    default:
        ASSERT_NOT_REACHED();
    case TA_START:
        return isLTR ? 0 : -length;
    case TA_MIDDLE:
        return -length / 2;
    case TA_END:
        return isLTR ? -length : 0;
    }
}

bool needsTextAnchorAdjustment(const ComputedStyle& style)
{
    bool isLTR = style.isLeftToRightDirection();
    switch (style.svgStyle().textAnchor()) {
    default:
        ASSERT_NOT_REACHED();
    case TA_START:
        return !isLTR;
    case TA_MIDDLE:
        return true;
    case TA_END:
        return isLTR;
    }
}

class ChunkLengthAccumulator {
public:
    ChunkLengthAccumulator(bool isVertical)
        : m_numCharacters(0)
        , m_length(0)
        , m_isVertical(isVertical)
    {
    }

    typedef Vector<SVGInlineTextBox*>::const_iterator BoxListConstIterator;

    void processRange(BoxListConstIterator boxStart, BoxListConstIterator boxEnd);
    void reset()
    {
        m_numCharacters = 0;
        m_length = 0;
    }

    float length() const { return m_length; }
    unsigned numCharacters() const { return m_numCharacters; }

private:
    unsigned m_numCharacters;
    float m_length;
    const bool m_isVertical;
};

void ChunkLengthAccumulator::processRange(BoxListConstIterator boxStart, BoxListConstIterator boxEnd)
{
    SVGTextFragment* lastFragment = nullptr;
    for (auto boxIter = boxStart; boxIter != boxEnd; ++boxIter) {
        for (SVGTextFragment& fragment : (*boxIter)->textFragments()) {
            m_numCharacters += fragment.length;

            if (m_isVertical)
                m_length += fragment.height;
            else
                m_length += fragment.width;

            if (!lastFragment) {
                lastFragment = &fragment;
                continue;
            }

            // Respect gap between chunks.
            if (m_isVertical)
                m_length += fragment.y - (lastFragment->y + lastFragment->height);
            else
                m_length += fragment.x - (lastFragment->x + lastFragment->width);

            lastFragment = &fragment;
        }
    }
}

}

SVGTextChunkBuilder::SVGTextChunkBuilder()
{
}

void SVGTextChunkBuilder::processTextChunks(const Vector<SVGInlineTextBox*>& lineLayoutBoxes)
{
    if (lineLayoutBoxes.isEmpty())
        return;

    bool foundStart = false;
    auto boxIter = lineLayoutBoxes.begin();
    auto endBox = lineLayoutBoxes.end();
    auto chunkStartBox = boxIter;
    for (; boxIter != endBox; ++boxIter) {
        if (!(*boxIter)->startsNewTextChunk())
            continue;

        if (!foundStart) {
            foundStart = true;
        } else {
            ASSERT(boxIter != chunkStartBox);
            handleTextChunk(chunkStartBox, boxIter);
        }
        chunkStartBox = boxIter;
    }

    if (!foundStart)
        return;

    if (boxIter != chunkStartBox)
        handleTextChunk(chunkStartBox, boxIter);
}

SVGTextPathChunkBuilder::SVGTextPathChunkBuilder()
    : SVGTextChunkBuilder()
    , m_totalLength(0)
    , m_totalCharacters(0)
    , m_totalTextAnchorShift(0)
{
}

void SVGTextPathChunkBuilder::handleTextChunk(BoxListConstIterator boxStart, BoxListConstIterator boxEnd)
{
    const ComputedStyle& style = (*boxStart)->layoutObject().styleRef();

    ChunkLengthAccumulator lengthAccumulator(style.svgStyle().isVerticalWritingMode());
    lengthAccumulator.processRange(boxStart, boxEnd);

    // Handle text-anchor as additional start offset for text paths.
    m_totalTextAnchorShift += calculateTextAnchorShift(style, lengthAccumulator.length());

    m_totalLength += lengthAccumulator.length();
    m_totalCharacters += lengthAccumulator.numCharacters();
}

static void buildSpacingAndGlyphsTransform(bool isVerticalText, float scale, const SVGTextFragment& fragment, AffineTransform& spacingAndGlyphsTransform)
{
    spacingAndGlyphsTransform.translate(fragment.x, fragment.y);

    if (isVerticalText)
        spacingAndGlyphsTransform.scaleNonUniform(1, scale);
    else
        spacingAndGlyphsTransform.scaleNonUniform(scale, 1);

    spacingAndGlyphsTransform.translate(-fragment.x, -fragment.y);
}

void SVGTextChunkBuilder::handleTextChunk(BoxListConstIterator boxStart, BoxListConstIterator boxEnd)
{
    ASSERT(*boxStart);

    const LayoutSVGInlineText& textLayoutObject = toLayoutSVGInlineText((*boxStart)->layoutObject());
    const ComputedStyle& style = textLayoutObject.styleRef();

    // Handle 'lengthAdjust' property.
    float desiredTextLength = 0;
    SVGLengthAdjustType lengthAdjust = SVGLengthAdjustUnknown;
    if (SVGTextContentElement* textContentElement = SVGTextContentElement::elementFromLayoutObject(textLayoutObject.parent())) {
        lengthAdjust = textContentElement->lengthAdjust()->currentValue()->enumValue();

        SVGLengthContext lengthContext(textContentElement);
        if (textContentElement->textLengthIsSpecifiedByUser())
            desiredTextLength = textContentElement->textLength()->currentValue()->value(lengthContext);
        else
            desiredTextLength = 0;
    }

    bool processTextLength = desiredTextLength > 0;
    bool processTextAnchor = needsTextAnchorAdjustment(style);
    if (!processTextAnchor && !processTextLength)
        return;

    bool isVerticalText = style.svgStyle().isVerticalWritingMode();

    // Calculate absolute length of whole text chunk (starting from text box 'start', spanning 'length' text boxes).
    ChunkLengthAccumulator lengthAccumulator(isVerticalText);
    lengthAccumulator.processRange(boxStart, boxEnd);

    if (processTextLength) {
        float chunkLength = lengthAccumulator.length();
        if (lengthAdjust == SVGLengthAdjustSpacing) {
            float textLengthShift = (desiredTextLength - chunkLength) / lengthAccumulator.numCharacters();
            unsigned atCharacter = 0;
            for (auto boxIter = boxStart; boxIter != boxEnd; ++boxIter) {
                Vector<SVGTextFragment>& fragments = (*boxIter)->textFragments();
                if (fragments.isEmpty())
                    continue;
                processTextLengthSpacingCorrection(isVerticalText, textLengthShift, fragments, atCharacter);
            }

            // Fragments have been adjusted, we have to recalculate the chunk
            // length, to be able to apply the text-anchor shift.
            if (processTextAnchor) {
                lengthAccumulator.reset();
                lengthAccumulator.processRange(boxStart, boxEnd);
            }
        } else {
            ASSERT(lengthAdjust == SVGLengthAdjustSpacingAndGlyphs);
            float textLengthScale = desiredTextLength / chunkLength;
            AffineTransform spacingAndGlyphsTransform;

            bool foundFirstFragment = false;
            for (auto boxIter = boxStart; boxIter != boxEnd; ++boxIter) {
                SVGInlineTextBox* textBox = *boxIter;
                Vector<SVGTextFragment>& fragments = textBox->textFragments();
                if (fragments.isEmpty())
                    continue;

                if (!foundFirstFragment) {
                    foundFirstFragment = true;
                    buildSpacingAndGlyphsTransform(isVerticalText, textLengthScale, fragments.first(), spacingAndGlyphsTransform);
                }

                applyTextLengthScaleAdjustment(spacingAndGlyphsTransform, fragments);
            }
        }
    }

    if (!processTextAnchor)
        return;

    float textAnchorShift = calculateTextAnchorShift(style, lengthAccumulator.length());
    for (auto boxIter = boxStart; boxIter != boxEnd; ++boxIter) {
        Vector<SVGTextFragment>& fragments = (*boxIter)->textFragments();
        if (fragments.isEmpty())
            continue;
        processTextAnchorCorrection(isVerticalText, textAnchorShift, fragments);
    }
}

void SVGTextChunkBuilder::processTextLengthSpacingCorrection(bool isVerticalText, float textLengthShift, Vector<SVGTextFragment>& fragments, unsigned& atCharacter)
{
    for (SVGTextFragment& fragment : fragments) {
        if (isVerticalText)
            fragment.y += textLengthShift * atCharacter;
        else
            fragment.x += textLengthShift * atCharacter;

        atCharacter += fragment.length;
    }
}

void SVGTextChunkBuilder::applyTextLengthScaleAdjustment(const AffineTransform& spacingAndGlyphsTransform, Vector<SVGTextFragment>& fragments)
{
    for (SVGTextFragment& fragment : fragments) {
        ASSERT(fragment.lengthAdjustTransform.isIdentity());
        fragment.lengthAdjustTransform = spacingAndGlyphsTransform;
    }
}

void SVGTextChunkBuilder::processTextAnchorCorrection(bool isVerticalText, float textAnchorShift, Vector<SVGTextFragment>& fragments)
{
    for (SVGTextFragment& fragment : fragments) {
        if (isVerticalText)
            fragment.y += textAnchorShift;
        else
            fragment.x += textAnchorShift;
    }
}

}
