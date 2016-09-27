// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/TableCellPainter.h"

#include "core/layout/LayoutTableCell.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/graphics/GraphicsContextStateSaver.h"

namespace blink {

inline const CollapsedBorderValue& TableCellPainter::cachedCollapsedLeftBorder(const ComputedStyle& styleForCellFlow) const
{
    if (styleForCellFlow.isHorizontalWritingMode()) {
        return styleForCellFlow.isLeftToRightDirection() ?  m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSStart)
            : m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSEnd);
    }
    return styleForCellFlow.isFlippedBlocksWritingMode() ?  m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSAfter)
        : m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSBefore);
}

inline const CollapsedBorderValue& TableCellPainter::cachedCollapsedRightBorder(const ComputedStyle& styleForCellFlow) const
{
    if (styleForCellFlow.isHorizontalWritingMode()) {
        return styleForCellFlow.isLeftToRightDirection() ?  m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSEnd)
            : m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSStart);
    }
    return styleForCellFlow.isFlippedBlocksWritingMode() ?  m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSBefore)
        : m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSAfter);
}

inline const CollapsedBorderValue& TableCellPainter::cachedCollapsedTopBorder(const ComputedStyle& styleForCellFlow) const
{
    if (styleForCellFlow.isHorizontalWritingMode())
        return styleForCellFlow.isFlippedBlocksWritingMode() ?  m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSAfter) : m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSBefore);
    return styleForCellFlow.isLeftToRightDirection() ?  m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSStart) : m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSEnd);
}

inline const CollapsedBorderValue& TableCellPainter::cachedCollapsedBottomBorder(const ComputedStyle& styleForCellFlow) const
{
    if (styleForCellFlow.isHorizontalWritingMode()) {
        return styleForCellFlow.isFlippedBlocksWritingMode() ?  m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSBefore)
            : m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSAfter);
    }
    return styleForCellFlow.isLeftToRightDirection() ?  m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSEnd)
        : m_layoutTableCell.section()->cachedCollapsedBorder(&m_layoutTableCell, CBSStart);
}

void TableCellPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(paintInfo.phase != PaintPhaseCollapsedTableBorders);
    BlockPainter(m_layoutTableCell).paint(paintInfo, paintOffset);
}

static EBorderStyle collapsedBorderStyle(EBorderStyle style)
{
    if (style == OUTSET)
        return GROOVE;
    if (style == INSET)
        return RIDGE;
    return style;
}

void TableCellPainter::paintCollapsedBorders(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(paintInfo.phase == PaintPhaseCollapsedTableBorders);

    if (!paintInfo.shouldPaintWithinRoot(&m_layoutTableCell) || m_layoutTableCell.style()->visibility() != VISIBLE)
        return;

    const CollapsedBorderValue* tableCurrentBorderValue = m_layoutTableCell.table()->currentBorderValue();
    if (!tableCurrentBorderValue)
        return;

    const ComputedStyle& styleForCellFlow = m_layoutTableCell.styleForCellFlow();
    const CollapsedBorderValue& leftBorderValue = cachedCollapsedLeftBorder(styleForCellFlow);
    const CollapsedBorderValue& rightBorderValue = cachedCollapsedRightBorder(styleForCellFlow);
    const CollapsedBorderValue& topBorderValue = cachedCollapsedTopBorder(styleForCellFlow);
    const CollapsedBorderValue& bottomBorderValue = cachedCollapsedBottomBorder(styleForCellFlow);

    bool shouldPaintTop = topBorderValue.shouldPaint(*tableCurrentBorderValue);
    bool shouldPaintBottom = bottomBorderValue.shouldPaint(*tableCurrentBorderValue);
    bool shouldPaintLeft = leftBorderValue.shouldPaint(*tableCurrentBorderValue);
    bool shouldPaintRight = rightBorderValue.shouldPaint(*tableCurrentBorderValue);

    if (!shouldPaintTop && !shouldPaintBottom && !shouldPaintLeft && !shouldPaintRight)
        return;

    // Adjust our x/y/width/height so that we paint the collapsed borders at the correct location.
    int topWidth = topBorderValue.width();
    int bottomWidth = bottomBorderValue.width();
    int leftWidth = leftBorderValue.width();
    int rightWidth = rightBorderValue.width();

    LayoutRect paintRect = paintBounds(paintOffset, AddOffsetFromParent);
    IntRect borderRect = pixelSnappedIntRect(paintRect.x() - leftWidth / 2,
        paintRect.y() - topWidth / 2,
        paintRect.width() + leftWidth / 2 + (rightWidth + 1) / 2,
        paintRect.height() + topWidth / 2 + (bottomWidth + 1) / 2);

    if (!borderRect.intersects(paintInfo.rect))
        return;

    GraphicsContext* graphicsContext = paintInfo.context;
    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*graphicsContext, m_layoutTableCell, paintInfo.phase))
        return;

    LayoutObjectDrawingRecorder recorder(*graphicsContext, m_layoutTableCell, paintInfo.phase, borderRect);
    Color cellColor = m_layoutTableCell.resolveColor(CSSPropertyColor);
    bool antialias = BoxPainter::shouldAntialiasLines(graphicsContext);

    // We never paint diagonals at the joins.  We simply let the border with the highest
    // precedence paint on top of borders with lower precedence.
    if (shouldPaintTop) {
        ObjectPainter::drawLineForBoxSide(graphicsContext, borderRect.x(), borderRect.y(), borderRect.maxX(), borderRect.y() + topWidth, BSTop,
            topBorderValue.color().resolve(cellColor), collapsedBorderStyle(topBorderValue.style()), 0, 0, antialias);
    }
    if (shouldPaintBottom) {
        ObjectPainter::drawLineForBoxSide(graphicsContext, borderRect.x(), borderRect.maxY() - bottomWidth, borderRect.maxX(), borderRect.maxY(), BSBottom,
            bottomBorderValue.color().resolve(cellColor), collapsedBorderStyle(bottomBorderValue.style()), 0, 0, antialias);
    }
    if (shouldPaintLeft) {
        ObjectPainter::drawLineForBoxSide(graphicsContext, borderRect.x(), borderRect.y(), borderRect.x() + leftWidth, borderRect.maxY(), BSLeft,
            leftBorderValue.color().resolve(cellColor), collapsedBorderStyle(leftBorderValue.style()), 0, 0, antialias);
    }
    if (shouldPaintRight) {
        ObjectPainter::drawLineForBoxSide(graphicsContext, borderRect.maxX() - rightWidth, borderRect.y(), borderRect.maxX(), borderRect.maxY(), BSRight,
            rightBorderValue.color().resolve(cellColor), collapsedBorderStyle(rightBorderValue.style()), 0, 0, antialias);
    }
}

void TableCellPainter::paintBackgroundsBehindCell(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, LayoutObject* backgroundObject)
{
    if (!paintInfo.shouldPaintWithinRoot(&m_layoutTableCell))
        return;

    if (!backgroundObject)
        return;

    if (m_layoutTableCell.style()->visibility() != VISIBLE)
        return;

    LayoutTable* tableElt = m_layoutTableCell.table();
    if (!tableElt->collapseBorders() && m_layoutTableCell.style()->emptyCells() == HIDE && !m_layoutTableCell.firstChild())
        return;

    Color c = backgroundObject->resolveColor(CSSPropertyBackgroundColor);
    const FillLayer& bgLayer = backgroundObject->style()->backgroundLayers();

    LayoutRect paintRect = paintBounds(paintOffset, backgroundObject != &m_layoutTableCell ? AddOffsetFromParent : DoNotAddOffsetFromParent);

    if (bgLayer.hasImage() || c.alpha()) {
        // We have to clip here because the background would paint
        // on top of the borders otherwise.  This only matters for cells and rows.
        bool shouldClip = backgroundObject->hasLayer() && (backgroundObject == &m_layoutTableCell || backgroundObject == m_layoutTableCell.parent()) && tableElt->collapseBorders();
        GraphicsContextStateSaver stateSaver(*paintInfo.context, shouldClip);
        if (shouldClip) {
            LayoutRect clipRect(paintRect.location(), m_layoutTableCell.size());
            clipRect.expand(m_layoutTableCell.borderInsets());
            paintInfo.context->clip(clipRect);
        }
        BoxPainter(m_layoutTableCell).paintFillLayers(paintInfo, c, bgLayer, paintRect, BackgroundBleedNone, SkXfermode::kSrcOver_Mode, backgroundObject);
    }
}

void TableCellPainter::paintBoxDecorationBackground(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (!paintInfo.shouldPaintWithinRoot(&m_layoutTableCell))
        return;

    LayoutTable* table = m_layoutTableCell.table();
    if (!table->collapseBorders() && m_layoutTableCell.style()->emptyCells() == HIDE && !m_layoutTableCell.firstChild())
        return;

    bool needsToPaintBorder = m_layoutTableCell.styleRef().hasBorderDecoration() && !table->collapseBorders();
    if (!m_layoutTableCell.hasBackground() && !m_layoutTableCell.styleRef().boxShadow() && !needsToPaintBorder)
        return;

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutTableCell, DisplayItem::BoxDecorationBackground))
        return;

    LayoutRect visualOverflowRect = m_layoutTableCell.visualOverflowRect();
    visualOverflowRect.moveBy(paintOffset);
    LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutTableCell, DisplayItem::BoxDecorationBackground, pixelSnappedIntRect(visualOverflowRect));

    LayoutRect paintRect = paintBounds(paintOffset, DoNotAddOffsetFromParent);

    BoxPainter::paintBoxShadow(paintInfo, paintRect, m_layoutTableCell.styleRef(), Normal);

    // Paint our cell background.
    paintBackgroundsBehindCell(paintInfo, paintOffset, &m_layoutTableCell);

    BoxPainter::paintBoxShadow(paintInfo, paintRect, m_layoutTableCell.styleRef(), Inset);

    if (!needsToPaintBorder)
        return;

    BoxPainter::paintBorder(m_layoutTableCell, paintInfo, paintRect, m_layoutTableCell.styleRef());
}

void TableCellPainter::paintMask(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (m_layoutTableCell.style()->visibility() != VISIBLE || paintInfo.phase != PaintPhaseMask)
        return;

    LayoutTable* tableElt = m_layoutTableCell.table();
    if (!tableElt->collapseBorders() && m_layoutTableCell.style()->emptyCells() == HIDE && !m_layoutTableCell.firstChild())
        return;

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutTableCell, paintInfo.phase))
        return;

    LayoutRect paintRect = paintBounds(paintOffset, DoNotAddOffsetFromParent);
    LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutTableCell, paintInfo.phase, paintRect);
    BoxPainter(m_layoutTableCell).paintMaskImages(paintInfo, paintRect);
}

LayoutRect TableCellPainter::paintBounds(const LayoutPoint& paintOffset, PaintBoundOffsetBehavior paintBoundOffsetBehavior)
{
    LayoutPoint adjustedPaintOffset = paintOffset;
    if (paintBoundOffsetBehavior == AddOffsetFromParent)
        adjustedPaintOffset.moveBy(m_layoutTableCell.location());
    return LayoutRect(adjustedPaintOffset, LayoutSize(m_layoutTableCell.pixelSnappedSize()));
}

} // namespace blink

