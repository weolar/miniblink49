/*
 * Copyright (C) 2013 Adobe Systems Incorporated. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/line/LineWidth.h"
#include "core/layout/shapes/ShapeOutsideInfo.h"

#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutRubyRun.h"

namespace blink {

LineWidth::LineWidth(LineLayoutBlockFlow block, bool isFirstLine, IndentTextOrNot shouldIndentText)
    : m_block(block)
    , m_uncommittedWidth(0)
    , m_committedWidth(0)
    , m_overhangWidth(0)
    , m_trailingWhitespaceWidth(0)
    , m_left(0)
    , m_right(0)
    , m_availableWidth(0)
    , m_isFirstLine(isFirstLine)
    , m_shouldIndentText(shouldIndentText)
{
    updateAvailableWidth();
}

void LineWidth::updateAvailableWidth(LayoutUnit replacedHeight)
{
    LayoutUnit height = m_block.logicalHeight();
    LayoutUnit logicalHeight = m_block.minLineHeightForReplacedObject(m_isFirstLine, replacedHeight);
    m_left = m_block.logicalLeftOffsetForLine(height, shouldIndentText(), logicalHeight).toFloat();
    m_right = m_block.logicalRightOffsetForLine(height, shouldIndentText(), logicalHeight).toFloat();

    computeAvailableWidthFromLeftAndRight();
}

void LineWidth::shrinkAvailableWidthForNewFloatIfNeeded(const FloatingObject& newFloat)
{
    LayoutUnit height = m_block.logicalHeight();
    if (height < m_block.logicalTopForFloat(newFloat) || height >= m_block.logicalBottomForFloat(newFloat))
        return;

    ShapeOutsideDeltas shapeDeltas;
    if (ShapeOutsideInfo* shapeOutsideInfo = newFloat.layoutObject()->shapeOutsideInfo()) {
        LayoutUnit lineHeight = m_block.lineHeight(m_isFirstLine, m_block.isHorizontalWritingMode() ? HorizontalLine : VerticalLine, PositionOfInteriorLineBoxes);
        shapeDeltas = shapeOutsideInfo->computeDeltasForContainingBlockLine(m_block, newFloat, m_block.logicalHeight(), lineHeight);
    }

    if (newFloat.type() == FloatingObject::FloatLeft) {
        float newLeft = m_block.logicalRightForFloat(newFloat).toFloat();
        if (shapeDeltas.isValid()) {
            if (shapeDeltas.lineOverlapsShape())
                newLeft += shapeDeltas.rightMarginBoxDelta();
            else // Per the CSS Shapes spec, If the line doesn't overlap the shape, then ignore this shape for this line.
                newLeft = m_left;
        }
        if (shouldIndentText() && m_block.style()->isLeftToRightDirection())
            newLeft += floorToInt(m_block.textIndentOffset());
        m_left = std::max<float>(m_left, newLeft);
    } else {
        float newRight = m_block.logicalLeftForFloat(newFloat).toFloat();
        if (shapeDeltas.isValid()) {
            if (shapeDeltas.lineOverlapsShape())
                newRight += shapeDeltas.leftMarginBoxDelta();
            else // Per the CSS Shapes spec, If the line doesn't overlap the shape, then ignore this shape for this line.
                newRight = m_right;
        }
        if (shouldIndentText() && !m_block.style()->isLeftToRightDirection())
            newRight -= floorToInt(m_block.textIndentOffset());
        m_right = std::min<float>(m_right, newRight);
    }

    computeAvailableWidthFromLeftAndRight();
}

void LineWidth::commit()
{
    m_committedWidth += m_uncommittedWidth;
    m_uncommittedWidth = 0;
}

void LineWidth::applyOverhang(LayoutRubyRun* rubyRun, LayoutObject* startLayoutObject, LayoutObject* endLayoutObject)
{
    int startOverhang;
    int endOverhang;
    rubyRun->getOverhang(m_isFirstLine, startLayoutObject, endLayoutObject, startOverhang, endOverhang);

    startOverhang = std::min<int>(startOverhang, m_committedWidth);
    m_availableWidth += startOverhang;

    endOverhang = std::max(std::min<int>(endOverhang, m_availableWidth - currentWidth()), 0);
    m_availableWidth += endOverhang;
    m_overhangWidth += startOverhang + endOverhang;
}

inline static float availableWidthAtOffset(LineLayoutBlockFlow block, const LayoutUnit& offset, bool shouldIndentText, float& newLineLeft,
    float& newLineRight, const LayoutUnit& lineHeight = 0)
{
    newLineLeft = block.logicalLeftOffsetForLine(offset, shouldIndentText, lineHeight).toFloat();
    newLineRight = block.logicalRightOffsetForLine(offset, shouldIndentText, lineHeight).toFloat();
    return std::max(0.0f, newLineRight - newLineLeft);
}

void LineWidth::updateLineDimension(LayoutUnit newLineTop, LayoutUnit newLineWidth, const float& newLineLeft, const float& newLineRight)
{
    if (newLineWidth <= m_availableWidth)
        return;

    m_block.setLogicalHeight(newLineTop);
    m_availableWidth = newLineWidth + m_overhangWidth;
    m_left = newLineLeft;
    m_right = newLineRight;
}

void LineWidth::wrapNextToShapeOutside(bool isFirstLine)
{
    LayoutUnit lineHeight = m_block.lineHeight(isFirstLine, m_block.isHorizontalWritingMode() ? HorizontalLine : VerticalLine, PositionOfInteriorLineBoxes);
    LayoutUnit lineLogicalTop = m_block.logicalHeight();
    LayoutUnit newLineTop = lineLogicalTop;
    LayoutUnit floatLogicalBottom = m_block.nextFloatLogicalBottomBelow(lineLogicalTop);

    float newLineWidth;
    float newLineLeft = m_left;
    float newLineRight = m_right;
    while (true) {
        newLineWidth = availableWidthAtOffset(m_block, newLineTop, shouldIndentText(), newLineLeft, newLineRight, lineHeight);
        if (newLineWidth >= m_uncommittedWidth)
            break;

        if (newLineTop >= floatLogicalBottom)
            break;

        newLineTop++;
    }
    updateLineDimension(newLineTop, newLineWidth, newLineLeft, newLineRight);
}

void LineWidth::fitBelowFloats(bool isFirstLine)
{
    ASSERT(!m_committedWidth);
    ASSERT(!fitsOnLine());
    m_block.positionNewFloats(this);

    LayoutUnit floatLogicalBottom;
    LayoutUnit lastFloatLogicalBottom = m_block.logicalHeight();
    float newLineWidth = m_availableWidth;
    float newLineLeft = m_left;
    float newLineRight = m_right;

    FloatingObject* lastFloatFromPreviousLine = m_block.lastFloatFromPreviousLine();
        if (lastFloatFromPreviousLine && lastFloatFromPreviousLine->layoutObject()->shapeOutsideInfo())
            return wrapNextToShapeOutside(isFirstLine);

    while (true) {
        floatLogicalBottom = m_block.nextFloatLogicalBottomBelow(lastFloatLogicalBottom, ShapeOutsideFloatShapeOffset);
        if (floatLogicalBottom <= lastFloatLogicalBottom)
            break;

        newLineWidth = availableWidthAtOffset(m_block, floatLogicalBottom, shouldIndentText(), newLineLeft, newLineRight);
        lastFloatLogicalBottom = floatLogicalBottom;

        if (newLineWidth >= m_uncommittedWidth)
            break;
    }
    updateLineDimension(lastFloatLogicalBottom, newLineWidth, newLineLeft, newLineRight);
}

void LineWidth::computeAvailableWidthFromLeftAndRight()
{
    m_availableWidth = max(0.0f, m_right - m_left) + m_overhangWidth;
}

}
