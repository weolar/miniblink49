// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/TableSectionPainter.h"

#include "core/layout/LayoutTable.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutTableCol.h"
#include "core/layout/LayoutTableRow.h"
#include "core/paint/BoxClipper.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/ObjectPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/TableCellPainter.h"
#include "core/paint/TableRowPainter.h"

namespace blink {

void TableSectionPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(!m_layoutTableSection.needsLayout());
    // avoid crashing on bugs that cause us to paint with dirty layout
    if (m_layoutTableSection.needsLayout())
        return;

    unsigned totalRows = m_layoutTableSection.numRows();
    unsigned totalCols = m_layoutTableSection.table()->columns().size();

    if (!totalRows || !totalCols)
        return;

    LayoutPoint adjustedPaintOffset = paintOffset + m_layoutTableSection.location();
    {
        BoxClipper boxClipper(m_layoutTableSection, paintInfo, adjustedPaintOffset, ForceContentsClip);
        paintObject(paintInfo, adjustedPaintOffset);
    }

    if ((paintInfo.phase == PaintPhaseOutline || paintInfo.phase == PaintPhaseSelfOutline) && m_layoutTableSection.style()->visibility() == VISIBLE) {
        LayoutRect visualOverflowRect(m_layoutTableSection.visualOverflowRect());
        visualOverflowRect.moveBy(adjustedPaintOffset);
        ObjectPainter(m_layoutTableSection).paintOutline(paintInfo, LayoutRect(adjustedPaintOffset, m_layoutTableSection.size()), visualOverflowRect);
    }
}

static inline bool compareCellPositions(LayoutTableCell* elem1, LayoutTableCell* elem2)
{
    return elem1->rowIndex() < elem2->rowIndex();
}

// This comparison is used only when we have overflowing cells as we have an unsorted array to sort. We thus need
// to sort both on rows and columns to properly issue paint invalidations.
static inline bool compareCellPositionsWithOverflowingCells(LayoutTableCell* elem1, LayoutTableCell* elem2)
{
    if (elem1->rowIndex() != elem2->rowIndex())
        return elem1->rowIndex() < elem2->rowIndex();

    return elem1->col() < elem2->col();
}

void TableSectionPainter::paintObject(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    LayoutRect localPaintInvalidationRect = LayoutRect(paintInfo.rect);
    localPaintInvalidationRect.moveBy(-paintOffset);

    LayoutRect tableAlignedRect = m_layoutTableSection.logicalRectForWritingModeAndDirection(localPaintInvalidationRect);

    CellSpan dirtiedRows = m_layoutTableSection.dirtiedRows(tableAlignedRect);
    CellSpan dirtiedColumns = m_layoutTableSection.dirtiedColumns(tableAlignedRect);

    HashSet<LayoutTableCell*> overflowingCells = m_layoutTableSection.overflowingCells();
    if (dirtiedColumns.start() < dirtiedColumns.end()) {
        if (!m_layoutTableSection.hasMultipleCellLevels() && !overflowingCells.size()) {
            if (paintInfo.phase == PaintPhaseCollapsedTableBorders) {
                // Collapsed borders are painted from the bottom right to the top left so that precedence
                // due to cell position is respected.
                for (unsigned r = dirtiedRows.end(); r > dirtiedRows.start(); r--) {
                    unsigned row = r - 1;
                    for (unsigned c = dirtiedColumns.end(); c > dirtiedColumns.start(); c--) {
                        unsigned col = c - 1;
                        LayoutTableSection::CellStruct& current = m_layoutTableSection.cellAt(row, col);
                        LayoutTableCell* cell = current.primaryCell();
                        if (!cell || (row > dirtiedRows.start() && m_layoutTableSection.primaryCellAt(row - 1, col) == cell) || (col > dirtiedColumns.start() && m_layoutTableSection.primaryCellAt(row, col - 1) == cell))
                            continue;
                        LayoutPoint cellPoint = m_layoutTableSection.flipForWritingModeForChild(cell, paintOffset);
                        TableCellPainter(*cell).paintCollapsedBorders(paintInfo, cellPoint);
                    }
                }
            } else {
                // Draw the dirty cells in the order that they appear.
                for (unsigned r = dirtiedRows.start(); r < dirtiedRows.end(); r++) {
                    LayoutTableRow* row = m_layoutTableSection.rowLayoutObjectAt(r);
                    if (row && !row->hasSelfPaintingLayer())
                        TableRowPainter(*row).paintOutlineForRowIfNeeded(paintInfo, paintOffset);
                    for (unsigned c = dirtiedColumns.start(); c < dirtiedColumns.end(); c++) {
                        LayoutTableSection::CellStruct& current = m_layoutTableSection.cellAt(r, c);
                        LayoutTableCell* cell = current.primaryCell();
                        if (!cell || (r > dirtiedRows.start() && m_layoutTableSection.primaryCellAt(r - 1, c) == cell) || (c > dirtiedColumns.start() && m_layoutTableSection.primaryCellAt(r, c - 1) == cell))
                            continue;
                        paintCell(cell, paintInfo, paintOffset);
                    }
                }
            }
        } else {
            // The overflowing cells should be scarce to avoid adding a lot of cells to the HashSet.
#if ENABLE(ASSERT)
            unsigned totalRows = m_layoutTableSection.numRows();
            unsigned totalCols = m_layoutTableSection.table()->columns().size();
            ASSERT(overflowingCells.size() < totalRows * totalCols * gMaxAllowedOverflowingCellRatioForFastPaintPath);
#endif

            // To make sure we properly paint invalidate the section, we paint invalidated all the overflowing cells that we collected.
            Vector<LayoutTableCell*> cells;
            copyToVector(overflowingCells, cells);

            HashSet<LayoutTableCell*> spanningCells;

            for (unsigned r = dirtiedRows.start(); r < dirtiedRows.end(); r++) {
                LayoutTableRow* row = m_layoutTableSection.rowLayoutObjectAt(r);
                if (row && !row->hasSelfPaintingLayer())
                    TableRowPainter(*row).paintOutlineForRowIfNeeded(paintInfo, paintOffset);
                for (unsigned c = dirtiedColumns.start(); c < dirtiedColumns.end(); c++) {
                    LayoutTableSection::CellStruct& current = m_layoutTableSection.cellAt(r, c);
                    if (!current.hasCells())
                        continue;
                    for (unsigned i = 0; i < current.cells.size(); ++i) {
                        if (overflowingCells.contains(current.cells[i]))
                            continue;

                        if (current.cells[i]->rowSpan() > 1 || current.cells[i]->colSpan() > 1) {
                            if (!spanningCells.add(current.cells[i]).isNewEntry)
                                continue;
                        }

                        cells.append(current.cells[i]);
                    }
                }
            }

            // Sort the dirty cells by paint order.
            if (!overflowingCells.size())
                std::stable_sort(cells.begin(), cells.end(), compareCellPositions);
            else
                std::sort(cells.begin(), cells.end(), compareCellPositionsWithOverflowingCells);

            if (paintInfo.phase == PaintPhaseCollapsedTableBorders) {
                for (unsigned i = cells.size(); i > 0; --i) {
                    LayoutPoint cellPoint = m_layoutTableSection.flipForWritingModeForChild(cells[i - 1], paintOffset);
                    TableCellPainter(*cells[i - 1]).paintCollapsedBorders(paintInfo, cellPoint);
                }
            } else {
                for (unsigned i = 0; i < cells.size(); ++i)
                    paintCell(cells[i], paintInfo, paintOffset);
            }
        }
    }
}

void TableSectionPainter::paintCell(LayoutTableCell* cell, const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    LayoutPoint cellPoint = m_layoutTableSection.flipForWritingModeForChild(cell, paintOffset);
    PaintPhase paintPhase = paintInfo.phase;
    LayoutTableRow* row = toLayoutTableRow(cell->parent());

    if (paintPhase == PaintPhaseBlockBackground || paintPhase == PaintPhaseChildBlockBackground) {
        // We need to handle painting a stack of backgrounds. This stack (from bottom to top) consists of
        // the column group, column, row group, row, and then the cell.
        LayoutTableCol* column = m_layoutTableSection.table()->colElement(cell->col());
        LayoutTableCol* columnGroup = column ? column->enclosingColumnGroup() : 0;

        bool columnHasBackground = column && column->hasBackground();
        bool columnGroupHasBackground = columnGroup && columnGroup->hasBackground();
        bool sectionHasBackground = m_layoutTableSection.hasBackground();
        bool rowHasBackground = row->hasBackground();

        if (columnHasBackground || columnGroupHasBackground || sectionHasBackground || rowHasBackground) {
            TableCellPainter tableCellPainter(*cell);
            if (!LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, *cell, paintPhase)) {
                LayoutObjectDrawingRecorder recorder(*paintInfo.context, *cell, paintPhase, tableCellPainter.paintBounds(cellPoint, TableCellPainter::AddOffsetFromParent));
                // Column groups and columns first.
                // FIXME: Columns and column groups do not currently support opacity, and they are being painted "too late" in
                // the stack, since we have already opened a transparency layer (potentially) for the table row group.
                // Note that we deliberately ignore whether or not the cell has a layer, since these backgrounds paint "behind" the
                // cell.
                if (columnGroupHasBackground)
                    tableCellPainter.paintBackgroundsBehindCell(paintInfo, cellPoint, columnGroup);
                if (columnHasBackground)
                    tableCellPainter.paintBackgroundsBehindCell(paintInfo, cellPoint, column);

                // Paint the row group next.
                if (sectionHasBackground)
                    tableCellPainter.paintBackgroundsBehindCell(paintInfo, cellPoint, &m_layoutTableSection);

                // Paint the row next, but only if it doesn't have a layer. If a row has a layer, it will be responsible for
                // painting the row background for the cell.
                if (rowHasBackground && !row->hasSelfPaintingLayer())
                    tableCellPainter.paintBackgroundsBehindCell(paintInfo, cellPoint, row);
            }
        }
    }
    if ((!cell->hasSelfPaintingLayer() && !row->hasSelfPaintingLayer()))
        cell->paint(paintInfo, cellPoint);
}

} // namespace blink
