// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/MultiColumnSetPainter.h"

#include "core/layout/LayoutMultiColumnSet.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/geometry/LayoutPoint.h"

namespace blink {

void MultiColumnSetPainter::paintObject(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (m_layoutMultiColumnSet.style()->visibility() != VISIBLE)
        return;

    BlockPainter(m_layoutMultiColumnSet).paintObject(paintInfo, paintOffset);

    // FIXME: Right now we're only painting in the foreground phase.
    // Columns should technically respect phases and allow for background/float/foreground overlap etc., just like
    // LayoutBlocks do. Note this is a pretty minor issue, since the old column implementation clipped columns
    // anyway, thus making it impossible for them to overlap one another. It's also really unlikely that the columns
    // would overlap another block.
    if (!m_layoutMultiColumnSet.flowThread() || (paintInfo.phase != PaintPhaseForeground && paintInfo.phase != PaintPhaseSelection))
        return;

    paintColumnRules(paintInfo, paintOffset);
}

void MultiColumnSetPainter::paintColumnRules(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (m_layoutMultiColumnSet.flowThread()->isLayoutPagedFlowThread())
        return;

    const ComputedStyle& blockStyle = m_layoutMultiColumnSet.multiColumnBlockFlow()->styleRef();
    const Color& ruleColor = m_layoutMultiColumnSet.resolveColor(blockStyle, CSSPropertyWebkitColumnRuleColor);
    bool ruleTransparent = blockStyle.columnRuleIsTransparent();
    EBorderStyle ruleStyle = blockStyle.columnRuleStyle();
    LayoutUnit ruleThickness = blockStyle.columnRuleWidth();
    LayoutUnit colGap = m_layoutMultiColumnSet.columnGap();
    bool renderRule = ruleStyle > BHIDDEN && !ruleTransparent;
    if (!renderRule)
        return;

    unsigned colCount = m_layoutMultiColumnSet.actualColumnCount();
    if (colCount <= 1)
        return;

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutMultiColumnSet, DisplayItem::ColumnRules))
        return;

    LayoutRect paintRect = m_layoutMultiColumnSet.visualOverflowRect();
    paintRect.moveBy(paintOffset);
    LayoutObjectDrawingRecorder drawingRecorder(*paintInfo.context, m_layoutMultiColumnSet, DisplayItem::ColumnRules, paintRect);

    bool antialias = BoxPainter::shouldAntialiasLines(paintInfo.context);
    bool leftToRight = m_layoutMultiColumnSet.style()->isLeftToRightDirection();
    LayoutUnit currLogicalLeftOffset = leftToRight ? LayoutUnit() : m_layoutMultiColumnSet.contentLogicalWidth();
    LayoutUnit ruleAdd = m_layoutMultiColumnSet.borderAndPaddingLogicalLeft();
    LayoutUnit ruleLogicalLeft = leftToRight ? LayoutUnit() : m_layoutMultiColumnSet.contentLogicalWidth();
    LayoutUnit inlineDirectionSize = m_layoutMultiColumnSet.pageLogicalWidth();
    BoxSide boxSide = m_layoutMultiColumnSet.isHorizontalWritingMode()
        ? leftToRight ? BSLeft : BSRight
        : leftToRight ? BSTop : BSBottom;

    for (unsigned i = 0; i < colCount; i++) {
        // Move to the next position.
        if (leftToRight) {
            ruleLogicalLeft += inlineDirectionSize + colGap / 2;
            currLogicalLeftOffset += inlineDirectionSize + colGap;
        } else {
            ruleLogicalLeft -= (inlineDirectionSize + colGap / 2);
            currLogicalLeftOffset -= (inlineDirectionSize + colGap);
        }

        // Now paint the column rule.
        if (i < colCount - 1) {
            LayoutUnit ruleLeft = m_layoutMultiColumnSet.isHorizontalWritingMode() ? paintOffset.x() + ruleLogicalLeft - ruleThickness / 2 + ruleAdd : paintOffset.x() + m_layoutMultiColumnSet.borderLeft() + m_layoutMultiColumnSet.paddingLeft();
            LayoutUnit ruleRight = m_layoutMultiColumnSet.isHorizontalWritingMode() ? ruleLeft + ruleThickness : ruleLeft + m_layoutMultiColumnSet.contentWidth();
            LayoutUnit ruleTop = m_layoutMultiColumnSet.isHorizontalWritingMode() ? paintOffset.y() + m_layoutMultiColumnSet.borderTop() + m_layoutMultiColumnSet.paddingTop() : paintOffset.y() + ruleLogicalLeft - ruleThickness / 2 + ruleAdd;
            LayoutUnit ruleBottom = m_layoutMultiColumnSet.isHorizontalWritingMode() ? ruleTop + m_layoutMultiColumnSet.contentHeight() : ruleTop + ruleThickness;
            IntRect pixelSnappedRuleRect = pixelSnappedIntRectFromEdges(ruleLeft, ruleTop, ruleRight, ruleBottom);
            ObjectPainter::drawLineForBoxSide(paintInfo.context, pixelSnappedRuleRect.x(), pixelSnappedRuleRect.y(), pixelSnappedRuleRect.maxX(), pixelSnappedRuleRect.maxY(), boxSide, ruleColor, ruleStyle, 0, 0, antialias);
        }

        ruleLogicalLeft = currLogicalLeftOffset;
    }
}

} // namespace blink
