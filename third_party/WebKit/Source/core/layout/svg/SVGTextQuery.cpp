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
#include "core/layout/svg/SVGTextQuery.h"

#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/line/InlineFlowBox.h"
#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/layout/svg/SVGTextFragment.h"
#include "core/layout/svg/SVGTextMetrics.h"
#include "core/layout/svg/line/SVGInlineTextBox.h"
#include "platform/FloatConversion.h"
#include "wtf/MathExtras.h"
#include "wtf/Vector.h"

namespace blink {

// Base structure for callback user data
struct QueryData {
    QueryData()
        : isVerticalText(false)
        , currentOffset(0)
        , textLayoutObject(nullptr)
        , textBox(nullptr)
    {
    }

    bool isVerticalText;
    unsigned currentOffset;
    LayoutSVGInlineText* textLayoutObject;
    const SVGInlineTextBox* textBox;
};

static inline InlineFlowBox* flowBoxForLayoutObject(LayoutObject* layoutObject)
{
    if (!layoutObject)
        return nullptr;

    if (layoutObject->isLayoutBlock()) {
        // If we're given a block element, it has to be a LayoutSVGText.
        ASSERT(layoutObject->isSVGText());
        LayoutBlockFlow* layoutBlockFlow = toLayoutBlockFlow(layoutObject);

        // LayoutSVGText only ever contains a single line box.
        InlineFlowBox* flowBox = layoutBlockFlow->firstLineBox();
        ASSERT(flowBox == layoutBlockFlow->lastLineBox());
        return flowBox;
    }

    if (layoutObject->isLayoutInline()) {
        // We're given a LayoutSVGInline or objects that derive from it (LayoutSVGTSpan / LayoutSVGTextPath)
        LayoutInline* layoutInline = toLayoutInline(layoutObject);

        // LayoutSVGInline only ever contains a single line box.
        InlineFlowBox* flowBox = layoutInline->firstLineBox();
        ASSERT(flowBox == layoutInline->lastLineBox());
        return flowBox;
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

static void collectTextBoxesInFlowBox(InlineFlowBox* flowBox, Vector<SVGInlineTextBox*>& textBoxes)
{
    if (!flowBox)
        return;

    for (InlineBox* child = flowBox->firstChild(); child; child = child->nextOnLine()) {
        if (child->isInlineFlowBox()) {
            // Skip generated content.
            if (!child->layoutObject().node())
                continue;

            collectTextBoxesInFlowBox(toInlineFlowBox(child), textBoxes);
            continue;
        }

        if (child->isSVGInlineTextBox())
            textBoxes.append(toSVGInlineTextBox(child));
    }
}

typedef bool ProcessTextFragmentCallback(QueryData*, const SVGTextFragment&);

static bool queryTextBox(QueryData* queryData, const SVGInlineTextBox* textBox, ProcessTextFragmentCallback fragmentCallback)
{
    queryData->textBox = textBox;
    queryData->textLayoutObject = &toLayoutSVGInlineText(textBox->layoutObject());

    queryData->isVerticalText = textBox->layoutObject().style()->svgStyle().isVerticalWritingMode();

    // Loop over all text fragments in this text box, firing a callback for each.
    for (const SVGTextFragment& fragment : textBox->textFragments()) {
        if (fragmentCallback(queryData, fragment))
            return true;
    }
    return false;
}

// Execute a query in "spatial" order starting at |queryRoot|. This means
// walking the lines boxes in the order they would get painted.
static void spatialQuery(LayoutObject* queryRoot, QueryData* queryData, ProcessTextFragmentCallback fragmentCallback)
{
    Vector<SVGInlineTextBox*> textBoxes;
    collectTextBoxesInFlowBox(flowBoxForLayoutObject(queryRoot), textBoxes);

    // Loop over all text boxes
    for (const SVGInlineTextBox* textBox : textBoxes) {
        if (queryTextBox(queryData, textBox, fragmentCallback))
            return;
    }
}

static void collectTextBoxesInLogicalOrder(const LayoutSVGInlineText& textLayoutObject, Vector<SVGInlineTextBox*>& textBoxes)
{
    textBoxes.shrink(0);
    for (InlineTextBox* textBox = textLayoutObject.firstTextBox(); textBox; textBox = textBox->nextTextBox())
        textBoxes.append(toSVGInlineTextBox(textBox));
    std::sort(textBoxes.begin(), textBoxes.end(), InlineTextBox::compareByStart);
}

// Execute a query in "logical" order starting at |queryRoot|. This means
// walking the lines boxes for each layout object in layout tree (pre)order.
static void logicalQuery(LayoutObject* queryRoot, QueryData* queryData, ProcessTextFragmentCallback fragmentCallback)
{
    if (!queryRoot)
        return;

    // Walk the layout tree in pre-order, starting at the specified root, and
    // run the query for each text node.
    Vector<SVGInlineTextBox*> textBoxes;
    for (LayoutObject* layoutObject = queryRoot->slowFirstChild(); layoutObject; layoutObject = layoutObject->nextInPreOrder(queryRoot)) {
        if (!layoutObject->isSVGInlineText())
            continue;

        LayoutSVGInlineText& textLayoutObject = toLayoutSVGInlineText(*layoutObject);
        ASSERT(textLayoutObject.style());

        // TODO(fs): Allow filtering the search earlier, since we should be
        // able to trivially reject (prune) at least some of the queries.
        collectTextBoxesInLogicalOrder(textLayoutObject, textBoxes);

        for (const SVGInlineTextBox* textBox : textBoxes) {
            if (queryTextBox(queryData, textBox, fragmentCallback))
                return;
            queryData->currentOffset += textBox->len();
        }
    }
}

static void modifyStartEndPositionsRespectingLigatures(const QueryData* queryData, const SVGTextFragment& fragment, int& startPosition, int& endPosition)
{
    const Vector<SVGTextMetrics>& textMetricsValues = queryData->textLayoutObject->layoutAttributes()->textMetricsValues();

    unsigned textMetricsOffset = fragment.metricsListOffset;
    int fragmentOffset = 0;
    int fragmentEnd = static_cast<int>(fragment.length);

    // Find the text metrics cell that start at or contain the character startPosition.
    while (fragmentOffset < fragmentEnd) {
        const SVGTextMetrics& metrics = textMetricsValues[textMetricsOffset];
        int glyphEnd = fragmentOffset + metrics.length();
        if (startPosition < glyphEnd)
            break;
        fragmentOffset = glyphEnd;
        textMetricsOffset++;
    }

    startPosition = fragmentOffset;

    // Find the text metrics cell that contain or ends at the character endPosition.
    while (fragmentOffset < fragmentEnd) {
        const SVGTextMetrics& metrics = textMetricsValues[textMetricsOffset];
        fragmentOffset += metrics.length();
        if (fragmentOffset >= endPosition)
            break;
        textMetricsOffset++;
    }

    endPosition = fragmentOffset;
}

static bool mapStartEndPositionsIntoFragmentCoordinates(const QueryData* queryData, const SVGTextFragment& fragment, int& startPosition, int& endPosition)
{
    unsigned boxStart = queryData->currentOffset;

    // Make <startPosition, endPosition> offsets relative to the current text box.
    startPosition -= boxStart;
    endPosition -= boxStart;

    // Reuse the same logic used for text selection & painting, to map our
    // query start/length into start/endPositions of the current text fragment.
    if (!queryData->textBox->mapStartEndPositionsIntoFragmentCoordinates(fragment, startPosition, endPosition))
        return false;

    modifyStartEndPositionsRespectingLigatures(queryData, fragment, startPosition, endPosition);
    ASSERT(startPosition < endPosition);
    return true;
}

// numberOfCharacters() implementation
static bool numberOfCharactersCallback(QueryData*, const SVGTextFragment&)
{
    // no-op
    return false;
}

unsigned SVGTextQuery::numberOfCharacters() const
{
    QueryData data;
    logicalQuery(m_queryRootLayoutObject, &data, numberOfCharactersCallback);
    return data.currentOffset;
}

// textLength() implementation
struct TextLengthData : QueryData {
    TextLengthData()
        : textLength(0)
    {
    }

    float textLength;
};

static bool textLengthCallback(QueryData* queryData, const SVGTextFragment& fragment)
{
    TextLengthData* data = static_cast<TextLengthData*>(queryData);
    data->textLength += queryData->isVerticalText ? fragment.height : fragment.width;
    return false;
}

float SVGTextQuery::textLength() const
{
    TextLengthData data;
    logicalQuery(m_queryRootLayoutObject, &data, textLengthCallback);
    return data.textLength;
}

// subStringLength() implementation
struct SubStringLengthData : QueryData {
    SubStringLengthData(unsigned queryStartPosition, unsigned queryLength)
        : startPosition(queryStartPosition)
        , length(queryLength)
        , subStringLength(0)
    {
    }

    unsigned startPosition;
    unsigned length;

    float subStringLength;
};

static bool subStringLengthCallback(QueryData* queryData, const SVGTextFragment& fragment)
{
    SubStringLengthData* data = static_cast<SubStringLengthData*>(queryData);

    int startPosition = data->startPosition;
    int endPosition = startPosition + data->length;
    if (!mapStartEndPositionsIntoFragmentCoordinates(queryData, fragment, startPosition, endPosition))
        return false;

    SVGTextMetrics metrics = SVGTextMetrics::measureCharacterRange(queryData->textLayoutObject, fragment.characterOffset + startPosition, endPosition - startPosition, queryData->textBox->direction());
    data->subStringLength += queryData->isVerticalText ? metrics.height() : metrics.width();
    return false;
}

float SVGTextQuery::subStringLength(unsigned startPosition, unsigned length) const
{
    SubStringLengthData data(startPosition, length);
    logicalQuery(m_queryRootLayoutObject, &data, subStringLengthCallback);
    return data.subStringLength;
}

// startPositionOfCharacter() implementation
struct StartPositionOfCharacterData : QueryData {
    StartPositionOfCharacterData(unsigned queryPosition)
        : position(queryPosition)
    {
    }

    unsigned position;
    FloatPoint startPosition;
};

static FloatPoint calculateGlyphPositionWithoutTransform(const QueryData* queryData, const SVGTextFragment& fragment, int offsetInFragment)
{
    float glyphOffsetInDirection = 0;
    if (offsetInFragment) {
        SVGTextMetrics metrics = SVGTextMetrics::measureCharacterRange(queryData->textLayoutObject, fragment.characterOffset, offsetInFragment, queryData->textBox->direction());
        if (queryData->isVerticalText)
            glyphOffsetInDirection = metrics.height();
        else
            glyphOffsetInDirection = metrics.width();
    }

    if (!queryData->textBox->isLeftToRightDirection()) {
        float fragmentExtent = queryData->isVerticalText ? fragment.height : fragment.width;
        glyphOffsetInDirection = fragmentExtent - glyphOffsetInDirection;
    }

    FloatPoint glyphPosition(fragment.x, fragment.y);
    if (queryData->isVerticalText)
        glyphPosition.move(0, glyphOffsetInDirection);
    else
        glyphPosition.move(glyphOffsetInDirection, 0);

    return glyphPosition;
}

static FloatPoint calculateGlyphPosition(const QueryData* queryData, const SVGTextFragment& fragment, int offsetInFragment)
{
    FloatPoint glyphPosition = calculateGlyphPositionWithoutTransform(queryData, fragment, offsetInFragment);
    AffineTransform fragmentTransform;
    fragment.buildFragmentTransform(fragmentTransform, SVGTextFragment::TransformIgnoringTextLength);
    if (!fragmentTransform.isIdentity())
        glyphPosition = fragmentTransform.mapPoint(glyphPosition);

    return glyphPosition;
}

static bool startPositionOfCharacterCallback(QueryData* queryData, const SVGTextFragment& fragment)
{
    StartPositionOfCharacterData* data = static_cast<StartPositionOfCharacterData*>(queryData);

    int startPosition = data->position;
    int endPosition = startPosition + 1;
    if (!mapStartEndPositionsIntoFragmentCoordinates(queryData, fragment, startPosition, endPosition))
        return false;

    data->startPosition = calculateGlyphPosition(queryData, fragment, startPosition);
    return true;
}

FloatPoint SVGTextQuery::startPositionOfCharacter(unsigned position) const
{
    StartPositionOfCharacterData data(position);
    logicalQuery(m_queryRootLayoutObject, &data, startPositionOfCharacterCallback);
    return data.startPosition;
}

// endPositionOfCharacter() implementation
struct EndPositionOfCharacterData : QueryData {
    EndPositionOfCharacterData(unsigned queryPosition)
        : position(queryPosition)
    {
    }

    unsigned position;
    FloatPoint endPosition;
};

static bool endPositionOfCharacterCallback(QueryData* queryData, const SVGTextFragment& fragment)
{
    EndPositionOfCharacterData* data = static_cast<EndPositionOfCharacterData*>(queryData);

    int startPosition = data->position;
    int endPosition = startPosition + 1;
    if (!mapStartEndPositionsIntoFragmentCoordinates(queryData, fragment, startPosition, endPosition))
        return false;

    // TODO(fs): mapStartEndPositionsIntoFragmentCoordinates(...) above applies
    // some heuristics for ligatures, so why not just use endPosition here?
    // (rather than startPosition+1)
    data->endPosition = calculateGlyphPosition(queryData, fragment, startPosition + 1);
    return true;
}

FloatPoint SVGTextQuery::endPositionOfCharacter(unsigned position) const
{
    EndPositionOfCharacterData data(position);
    logicalQuery(m_queryRootLayoutObject, &data, endPositionOfCharacterCallback);
    return data.endPosition;
}

// rotationOfCharacter() implementation
struct RotationOfCharacterData : QueryData {
    RotationOfCharacterData(unsigned queryPosition)
        : position(queryPosition)
        , rotation(0)
    {
    }

    unsigned position;
    float rotation;
};

static bool rotationOfCharacterCallback(QueryData* queryData, const SVGTextFragment& fragment)
{
    RotationOfCharacterData* data = static_cast<RotationOfCharacterData*>(queryData);

    int startPosition = data->position;
    int endPosition = startPosition + 1;
    if (!mapStartEndPositionsIntoFragmentCoordinates(queryData, fragment, startPosition, endPosition))
        return false;

    AffineTransform fragmentTransform;
    fragment.buildFragmentTransform(fragmentTransform, SVGTextFragment::TransformIgnoringTextLength);
    if (fragmentTransform.isIdentity()) {
        data->rotation = 0;
    } else {
        fragmentTransform.scale(1 / fragmentTransform.xScale(), 1 / fragmentTransform.yScale());
        data->rotation = narrowPrecisionToFloat(rad2deg(atan2(fragmentTransform.b(), fragmentTransform.a())));
    }

    return true;
}

float SVGTextQuery::rotationOfCharacter(unsigned position) const
{
    RotationOfCharacterData data(position);
    logicalQuery(m_queryRootLayoutObject, &data, rotationOfCharacterCallback);
    return data.rotation;
}

// extentOfCharacter() implementation
struct ExtentOfCharacterData : QueryData {
    ExtentOfCharacterData(unsigned queryPosition)
        : position(queryPosition)
    {
    }

    unsigned position;
    FloatRect extent;
};

const SVGTextMetrics& findMetricsForCharacter(const Vector<SVGTextMetrics>& textMetricsValues, const SVGTextFragment& fragment, unsigned startInFragment)
{
    // Find the text metrics cell that start at or contain the character at |startInFragment|.
    unsigned textMetricsOffset = fragment.metricsListOffset;
    unsigned fragmentOffset = 0;
    while (fragmentOffset < fragment.length) {
        const SVGTextMetrics& metrics = textMetricsValues[textMetricsOffset++];
        unsigned glyphEnd = fragmentOffset + metrics.length();
        if (startInFragment < glyphEnd)
            break;
        fragmentOffset = glyphEnd;
    }
    return textMetricsValues[textMetricsOffset - 1];
}

static inline void calculateGlyphBoundaries(const QueryData* queryData, const SVGTextFragment& fragment, int startPosition, FloatRect& extent)
{
    float scalingFactor = queryData->textLayoutObject->scalingFactor();
    ASSERT(scalingFactor);

    FloatPoint glyphPosition = calculateGlyphPositionWithoutTransform(queryData, fragment, startPosition);
    glyphPosition.move(0, -queryData->textLayoutObject->scaledFont().fontMetrics().floatAscent() / scalingFactor);
    extent.setLocation(glyphPosition);

    // Use the SVGTextMetrics computed by SVGTextMetricsBuilder (which spends
    // time attempting to compute more correct glyph bounds already, handling
    // cursive scripts to some degree.)
    const Vector<SVGTextMetrics>& textMetricsValues = queryData->textLayoutObject->layoutAttributes()->textMetricsValues();
    const SVGTextMetrics& metrics = findMetricsForCharacter(textMetricsValues, fragment, startPosition);

    // TODO(fs): Negative glyph extents seems kind of weird to have, but
    // presently it can occur in some cases (like Arabic.)
    FloatSize glyphSize(std::max<float>(metrics.width(), 0), std::max<float>(metrics.height(), 0));
    extent.setSize(glyphSize);

    // If RTL, adjust the starting point to align with the LHS of the glyph bounding box.
    if (!queryData->textBox->isLeftToRightDirection()) {
        if (queryData->isVerticalText)
            extent.move(0, -glyphSize.height());
        else
            extent.move(-glyphSize.width(), 0);
    }

    AffineTransform fragmentTransform;
    fragment.buildFragmentTransform(fragmentTransform, SVGTextFragment::TransformIgnoringTextLength);

    extent = fragmentTransform.mapRect(extent);
}

static inline FloatRect calculateFragmentBoundaries(const LayoutSVGInlineText& textLayoutObject, const SVGTextFragment& fragment)
{
    float scalingFactor = textLayoutObject.scalingFactor();
    ASSERT(scalingFactor);
    float baseline = textLayoutObject.scaledFont().fontMetrics().floatAscent() / scalingFactor;

    AffineTransform fragmentTransform;
    FloatRect fragmentRect(fragment.x, fragment.y - baseline, fragment.width, fragment.height);
    fragment.buildFragmentTransform(fragmentTransform);
    return fragmentTransform.mapRect(fragmentRect);
}

static bool extentOfCharacterCallback(QueryData* queryData, const SVGTextFragment& fragment)
{
    ExtentOfCharacterData* data = static_cast<ExtentOfCharacterData*>(queryData);

    int startPosition = data->position;
    int endPosition = startPosition + 1;
    if (!mapStartEndPositionsIntoFragmentCoordinates(queryData, fragment, startPosition, endPosition))
        return false;

    calculateGlyphBoundaries(queryData, fragment, startPosition, data->extent);
    return true;
}

FloatRect SVGTextQuery::extentOfCharacter(unsigned position) const
{
    ExtentOfCharacterData data(position);
    logicalQuery(m_queryRootLayoutObject, &data, extentOfCharacterCallback);
    return data.extent;
}

// characterNumberAtPosition() implementation
struct CharacterNumberAtPositionData : QueryData {
    CharacterNumberAtPositionData(const FloatPoint& queryPosition)
        : position(queryPosition)
        , hitLayoutObject(nullptr)
        , offsetInTextNode(0)
    {
    }

    int characterNumberWithin(const LayoutObject* queryRoot) const;

    FloatPoint position;
    LayoutObject* hitLayoutObject;
    int offsetInTextNode;
};

int CharacterNumberAtPositionData::characterNumberWithin(const LayoutObject* queryRoot) const
{
    // http://www.w3.org/TR/SVG/single-page.html#text-__svg__SVGTextContentElement__getCharNumAtPosition
    // "If no such character exists, a value of -1 is returned."
    if (!hitLayoutObject)
        return -1;
    ASSERT(queryRoot);
    int characterNumber = offsetInTextNode;

    // Accumulate the lengths of all the text nodes preceding the target layout
    // object within the queried root, to get the complete character number.
    for (const LayoutObject* layoutObject = hitLayoutObject->previousInPreOrder(queryRoot);
        layoutObject; layoutObject = layoutObject->previousInPreOrder(queryRoot)) {
        if (!layoutObject->isSVGInlineText())
            continue;
        characterNumber += toLayoutSVGInlineText(layoutObject)->resolvedTextLength();
    }
    return characterNumber;
}

static unsigned logicalOffsetInTextNode(const LayoutSVGInlineText& textLayoutObject, const SVGInlineTextBox* startTextBox, unsigned fragmentOffset)
{
    Vector<SVGInlineTextBox*> textBoxes;
    collectTextBoxesInLogicalOrder(textLayoutObject, textBoxes);

    ASSERT(startTextBox);
    size_t index = textBoxes.find(startTextBox);
    ASSERT(index != kNotFound);

    unsigned offset = fragmentOffset;
    while (index) {
        --index;
        offset += textBoxes[index]->len();
    }
    return offset;
}

static bool characterNumberAtPositionCallback(QueryData* queryData, const SVGTextFragment& fragment)
{
    CharacterNumberAtPositionData* data = static_cast<CharacterNumberAtPositionData*>(queryData);

    // Test the query point against the bounds of the entire fragment first.
    FloatRect fragmentExtents = calculateFragmentBoundaries(*queryData->textLayoutObject, fragment);
    if (!fragmentExtents.contains(data->position))
        return false;

    // Iterate through the glyphs in this fragment, and check if their extents
    // contain the query point.
    FloatRect extent;
    const Vector<SVGTextMetrics>& textMetrics = queryData->textLayoutObject->layoutAttributes()->textMetricsValues();
    unsigned textMetricsOffset = fragment.metricsListOffset;
    unsigned fragmentOffset = 0;
    while (fragmentOffset < fragment.length) {
        calculateGlyphBoundaries(queryData, fragment, fragmentOffset, extent);
        if (extent.contains(data->position)) {
            // Compute the character offset of the glyph within the text node.
            unsigned offsetInBox = fragment.characterOffset - queryData->textBox->start() + fragmentOffset;
            data->offsetInTextNode = logicalOffsetInTextNode(*queryData->textLayoutObject, queryData->textBox, offsetInBox);
            data->hitLayoutObject = data->textLayoutObject;
            return true;
        }
        fragmentOffset += textMetrics[textMetricsOffset].length();
        textMetricsOffset++;
    }
    return false;
}

int SVGTextQuery::characterNumberAtPosition(const FloatPoint& position) const
{
    CharacterNumberAtPositionData data(position);
    spatialQuery(m_queryRootLayoutObject, &data, characterNumberAtPositionCallback);
    return data.characterNumberWithin(m_queryRootLayoutObject);
}

}
