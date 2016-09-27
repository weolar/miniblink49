/*
 * Copyright (C) 2002 Lars Knoll (knoll@kde.org)
 *           (C) 2002 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2006, 2008, 2010 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License.
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
#include "core/layout/TableLayoutAlgorithmAuto.h"

#include "core/layout/LayoutTable.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutTableCol.h"
#include "core/layout/LayoutTableSection.h"
#include "core/layout/TextAutosizer.h"

namespace blink {

TableLayoutAlgorithmAuto::TableLayoutAlgorithmAuto(LayoutTable* table)
    : TableLayoutAlgorithm(table)
    , m_hasPercent(false)
    , m_effectiveLogicalWidthDirty(true)
{
}

TableLayoutAlgorithmAuto::~TableLayoutAlgorithmAuto()
{
}

void TableLayoutAlgorithmAuto::recalcColumn(unsigned effCol)
{
    Layout& columnLayout = m_layoutStruct[effCol];

    LayoutTableCell* fixedContributor = nullptr;
    LayoutTableCell* maxContributor = nullptr;

    for (LayoutObject* child = m_table->children()->firstChild(); child; child = child->nextSibling()) {
        if (child->isLayoutTableCol()) {
            // LayoutTableCols don't have the concept of preferred logical width, but we need to clear their dirty bits
            // so that if we call setPreferredWidthsDirty(true) on a col or one of its descendants, we'll mark it's
            // ancestors as dirty.
            toLayoutTableCol(child)->clearPreferredLogicalWidthsDirtyBits();
        } else if (child->isTableSection()) {
            LayoutTableSection* section = toLayoutTableSection(child);
            unsigned numRows = section->numRows();
            for (unsigned i = 0; i < numRows; i++) {
                LayoutTableSection::CellStruct current = section->cellAt(i, effCol);
                LayoutTableCell* cell = current.primaryCell();

                if (current.inColSpan || !cell)
                    continue;
                columnLayout.columnHasNoCells = false;

                if (cell->maxPreferredLogicalWidth())
                    columnLayout.emptyCellsOnly = false;

                if (cell->colSpan() == 1) {
                    columnLayout.minLogicalWidth = std::max<int>(cell->minPreferredLogicalWidth(), columnLayout.minLogicalWidth);
                    if (cell->maxPreferredLogicalWidth() > columnLayout.maxLogicalWidth) {
                        columnLayout.maxLogicalWidth = cell->maxPreferredLogicalWidth();
                        maxContributor = cell;
                    }

                    // All browsers implement a size limit on the cell's max width.
                    // Our limit is based on KHTML's representation that used 16 bits widths.
                    // FIXME: Other browsers have a lower limit for the cell's max width.
                    const int cCellMaxWidth = 32760;
                    Length cellLogicalWidth = cell->styleOrColLogicalWidth();
                    // FIXME: calc() on tables should be handled consistently with other lengths. See bug: https://crbug.com/382725
                    if (cellLogicalWidth.isCalculated())
                        cellLogicalWidth = Length(); // Make it Auto
                    if (cellLogicalWidth.value() > cCellMaxWidth)
                        cellLogicalWidth.setValue(cCellMaxWidth);
                    if (cellLogicalWidth.isNegative())
                        cellLogicalWidth.setValue(0);
                    switch (cellLogicalWidth.type()) {
                    case Fixed:
                        // ignore width=0
                        if (cellLogicalWidth.isPositive() && !columnLayout.logicalWidth.hasPercent()) {
                            int logicalWidth = cell->adjustBorderBoxLogicalWidthForBoxSizing(cellLogicalWidth.value());
                            if (columnLayout.logicalWidth.isFixed()) {
                                // Nav/IE weirdness
                                if ((logicalWidth > columnLayout.logicalWidth.value())
                                    || ((columnLayout.logicalWidth.value() == logicalWidth) && (maxContributor == cell))) {
                                    columnLayout.logicalWidth.setValue(Fixed, logicalWidth);
                                    fixedContributor = cell;
                                }
                            } else {
                                columnLayout.logicalWidth.setValue(Fixed, logicalWidth);
                                fixedContributor = cell;
                            }
                        }
                        break;
                    case Percent:
                        m_hasPercent = true;
                        // TODO(alancutter): Make this work correctly for calc lengths.
                        if (cellLogicalWidth.isPositive() && (!columnLayout.logicalWidth.hasPercent() || cellLogicalWidth.value() > columnLayout.logicalWidth.value()))
                            columnLayout.logicalWidth = cellLogicalWidth;
                        break;
                    default:
                        break;
                    }
                } else if (!effCol || section->primaryCellAt(i, effCol - 1) != cell) {
                    // If a cell originates in this spanning column ensure we have a min/max width of at least 1px for it.
                    columnLayout.minLogicalWidth = std::max<int>(columnLayout.minLogicalWidth, cell->maxPreferredLogicalWidth() ? 1 : 0);

                    // This spanning cell originates in this column. Insert the cell into spanning cells list.
                    insertSpanCell(cell);
                }
            }
        }
    }

    // Nav/IE weirdness
    if (columnLayout.logicalWidth.isFixed()) {
        if (m_table->document().inQuirksMode() && columnLayout.maxLogicalWidth > columnLayout.logicalWidth.value() && fixedContributor != maxContributor) {
            columnLayout.logicalWidth = Length();
            fixedContributor = nullptr;
        }
    }

    columnLayout.maxLogicalWidth = std::max(columnLayout.maxLogicalWidth, columnLayout.minLogicalWidth);
}

void TableLayoutAlgorithmAuto::fullRecalc()
{
    m_hasPercent = false;
    m_effectiveLogicalWidthDirty = true;

    unsigned nEffCols = m_table->numEffCols();
    m_layoutStruct.resize(nEffCols);
    m_layoutStruct.fill(Layout());
    m_spanCells.fill(0);

    Length groupLogicalWidth;
    unsigned currentColumn = 0;
    for (LayoutTableCol* column = m_table->firstColumn(); column; column = column->nextColumn()) {
        if (column->isTableColumnGroupWithColumnChildren()) {
            groupLogicalWidth = column->style()->logicalWidth();
        } else {
            Length colLogicalWidth = column->style()->logicalWidth();
            // FIXME: calc() on tables should be handled consistently with other lengths. See bug: https://crbug.com/382725
            if (colLogicalWidth.isCalculated() || colLogicalWidth.isAuto())
                colLogicalWidth = groupLogicalWidth;
            // TODO(alancutter): Make this work correctly for calc lengths.
            if ((colLogicalWidth.isFixed() || colLogicalWidth.hasPercent()) && colLogicalWidth.isZero())
                colLogicalWidth = Length();
            unsigned effCol = m_table->colToEffCol(currentColumn);
            unsigned span = column->span();
            if (!colLogicalWidth.isAuto() && span == 1 && effCol < nEffCols && m_table->spanOfEffCol(effCol) == 1) {
                m_layoutStruct[effCol].logicalWidth = colLogicalWidth;
                if (colLogicalWidth.isFixed() && m_layoutStruct[effCol].maxLogicalWidth < colLogicalWidth.value())
                    m_layoutStruct[effCol].maxLogicalWidth = colLogicalWidth.value();
            }
            currentColumn += span;
        }

        // For the last column in a column-group, we invalidate our group logical width.
        if (column->isTableColumn() && !column->nextSibling())
            groupLogicalWidth = Length();
    }

    for (unsigned i = 0; i < nEffCols; i++)
        recalcColumn(i);
}

// FIXME: This needs to be adapted for vertical writing modes.
static bool shouldScaleColumns(LayoutTable* table)
{
    // A special case.  If this table is not fixed width and contained inside
    // a cell, then don't bloat the maxwidth by examining percentage growth.
    bool scale = true;
    while (table) {
        Length tw = table->style()->width();
        if ((tw.isAuto() || tw.hasPercent()) && !table->isOutOfFlowPositioned()) {
            LayoutBlock* cb = table->containingBlock();
            while (cb && !cb->isLayoutView() && !cb->isTableCell()
                && cb->style()->width().isAuto() && !cb->isOutOfFlowPositioned())
                cb = cb->containingBlock();

            table = 0;
            if (cb && cb->isTableCell()
                && (cb->style()->width().isAuto() || cb->style()->width().hasPercent())) {
                LayoutTableCell* cell = toLayoutTableCell(cb);
                if (cell->colSpan() > 1 || cell->table()->style()->width().isAuto())
                    scale = false;
                else
                    table = cell->table();
            }
        } else {
            table = 0;
        }
    }
    return scale;
}

void TableLayoutAlgorithmAuto::computeIntrinsicLogicalWidths(LayoutUnit& minWidth, LayoutUnit& maxWidth)
{
    TextAutosizer::TableLayoutScope textAutosizerTableLayoutScope(m_table);

    fullRecalc();

    int spanMaxLogicalWidth = calcEffectiveLogicalWidth();
    minWidth = 0;
    maxWidth = 0;
    float maxPercent = 0;
    float maxNonPercent = 0;
    bool scaleColumns = shouldScaleColumns(m_table);

    // We substitute 0 percent by (epsilon / percentScaleFactor) percent in two places below to avoid division by zero.
    // FIXME: Handle the 0% cases properly.
    const float epsilon = 1 / 128.0f;

    float remainingPercent = 100;
    for (size_t i = 0; i < m_layoutStruct.size(); ++i) {
        minWidth += m_layoutStruct[i].effectiveMinLogicalWidth;
        maxWidth += m_layoutStruct[i].effectiveMaxLogicalWidth;
        if (scaleColumns) {
            if (m_layoutStruct[i].effectiveLogicalWidth.hasPercent()) {
                float percent = std::min(static_cast<float>(m_layoutStruct[i].effectiveLogicalWidth.percent()), remainingPercent);
                float logicalWidth = static_cast<float>(m_layoutStruct[i].effectiveMaxLogicalWidth) * 100 / std::max(percent, epsilon);
                maxPercent = std::max(logicalWidth,  maxPercent);
                remainingPercent -= percent;
            } else {
                maxNonPercent += m_layoutStruct[i].effectiveMaxLogicalWidth;
            }
        }
    }

    if (scaleColumns) {
        maxNonPercent = maxNonPercent * 100 / std::max(remainingPercent, epsilon);
        maxWidth = std::max<int>(maxWidth, static_cast<int>(std::min(maxNonPercent, static_cast<float>(tableMaxWidth))));
        maxWidth = std::max<int>(maxWidth, static_cast<int>(std::min(maxPercent, static_cast<float>(tableMaxWidth))));
    }

    maxWidth = std::max<int>(maxWidth, spanMaxLogicalWidth);
}

void TableLayoutAlgorithmAuto::applyPreferredLogicalWidthQuirks(LayoutUnit& minWidth, LayoutUnit& maxWidth) const
{
    Length tableLogicalWidth = m_table->style()->logicalWidth();
    if (tableLogicalWidth.isFixed() && tableLogicalWidth.isPositive()) {
        // |minWidth| is the result of measuring the intrinsic content's size. Keep it to
        // make sure we are *never* smaller than the actual content.
        LayoutUnit minContentWidth = minWidth;
        // FIXME: This line looks REALLY suspicious as it could allow the minimum
        // preferred logical width to be smaller than the table content. This has
        // to be cross-checked against other browsers.
        minWidth = maxWidth = std::max<int>(minWidth, tableLogicalWidth.value());

        const Length& styleMaxLogicalWidth = m_table->style()->logicalMaxWidth();
        if (styleMaxLogicalWidth.isFixed() && !styleMaxLogicalWidth.isNegative()) {
            minWidth = std::min<int>(minWidth, styleMaxLogicalWidth.value());
            minWidth = std::max(minWidth, minContentWidth);
            maxWidth = minWidth;
        }
    }
}

/*
  This method takes care of colspans.
  effWidth is the same as width for cells without colspans. If we have colspans, they get modified.
 */
int TableLayoutAlgorithmAuto::calcEffectiveLogicalWidth()
{
    int maxLogicalWidth = 0;

    size_t nEffCols = m_layoutStruct.size();
    int spacingInRowDirection = m_table->hBorderSpacing();

    for (size_t i = 0; i < nEffCols; ++i) {
        m_layoutStruct[i].effectiveLogicalWidth = m_layoutStruct[i].logicalWidth;
        m_layoutStruct[i].effectiveMinLogicalWidth = m_layoutStruct[i].minLogicalWidth;
        m_layoutStruct[i].effectiveMaxLogicalWidth = m_layoutStruct[i].maxLogicalWidth;
    }

    for (size_t i = 0; i < m_spanCells.size(); ++i) {
        LayoutTableCell* cell = m_spanCells[i];
        if (!cell)
            break;

        unsigned span = cell->colSpan();

        Length cellLogicalWidth = cell->styleOrColLogicalWidth();
        // FIXME: calc() on tables should be handled consistently with other lengths. See bug: https://crbug.com/382725
        if (cellLogicalWidth.isZero() || cellLogicalWidth.isCalculated())
            cellLogicalWidth = Length(); // Make it Auto

        unsigned effCol = m_table->colToEffCol(cell->col());
        size_t lastCol = effCol;
        int cellMinLogicalWidth = cell->minPreferredLogicalWidth() + spacingInRowDirection;
        int cellMaxLogicalWidth = cell->maxPreferredLogicalWidth() + spacingInRowDirection;
        float totalPercent = 0;
        int spanMinLogicalWidth = 0;
        int spanMaxLogicalWidth = 0;
        bool allColsArePercent = true;
        bool allColsAreFixed = true;
        bool haveAuto = false;
        bool spanHasEmptyCellsOnly = true;
        int fixedWidth = 0;
        while (lastCol < nEffCols && span > 0) {
            Layout& columnLayout = m_layoutStruct[lastCol];
            switch (columnLayout.logicalWidth.type()) {
            case Percent:
                totalPercent += columnLayout.logicalWidth.percent();
                allColsAreFixed = false;
                break;
            case Fixed:
                if (columnLayout.logicalWidth.value() > 0) {
                    fixedWidth += columnLayout.logicalWidth.value();
                    allColsArePercent = false;
                    // IE resets effWidth to Auto here, but this breaks the konqueror about page and seems to be some bad
                    // legacy behaviour anyway. mozilla doesn't do this so I decided we don't neither.
                    break;
                }
                // fall through
            case Auto:
                haveAuto = true;
                // fall through
            default:
                // If the column is a percentage width, do not let the spanning cell overwrite the
                // width value.  This caused a mis-layout on amazon.com.
                // Sample snippet:
                // <table border=2 width=100%><
                //   <tr><td>1</td><td colspan=2>2-3</tr>
                //   <tr><td>1</td><td colspan=2 width=100%>2-3</td></tr>
                // </table>
                // TODO(alancutter): Make this work correctly for calc lengths.
                if (!columnLayout.effectiveLogicalWidth.hasPercent()) {
                    columnLayout.effectiveLogicalWidth = Length();
                    allColsArePercent = false;
                } else {
                    totalPercent += columnLayout.effectiveLogicalWidth.percent();
                }
                allColsAreFixed = false;
            }
            if (!columnLayout.emptyCellsOnly)
                spanHasEmptyCellsOnly = false;
            span -= m_table->spanOfEffCol(lastCol);
            spanMinLogicalWidth += columnLayout.effectiveMinLogicalWidth;
            spanMaxLogicalWidth += columnLayout.effectiveMaxLogicalWidth;
            lastCol++;
            cellMinLogicalWidth -= spacingInRowDirection;
            cellMaxLogicalWidth -= spacingInRowDirection;
        }

        // adjust table max width if needed
        if (cellLogicalWidth.hasPercent()) {
            if (totalPercent > cellLogicalWidth.percent() || allColsArePercent) {
                // can't satify this condition, treat as variable
                cellLogicalWidth = Length();
            } else {
                maxLogicalWidth = std::max(maxLogicalWidth, static_cast<int>(std::max(spanMaxLogicalWidth, cellMaxLogicalWidth) * 100  / cellLogicalWidth.percent()));

                // all non percent columns in the span get percent values to sum up correctly.
                float percentMissing = cellLogicalWidth.percent() - totalPercent;
                int totalWidth = 0;
                for (unsigned pos = effCol; pos < lastCol; ++pos) {
                    if (!m_layoutStruct[pos].effectiveLogicalWidth.hasPercent())
                        totalWidth += m_layoutStruct[pos].clampedEffectiveMaxLogicalWidth();
                }

                for (unsigned pos = effCol; pos < lastCol && totalWidth > 0; ++pos) {
                    if (!m_layoutStruct[pos].effectiveLogicalWidth.hasPercent()) {
                        float percent = percentMissing * static_cast<float>(m_layoutStruct[pos].effectiveMaxLogicalWidth) / totalWidth;
                        totalWidth -= m_layoutStruct[pos].clampedEffectiveMaxLogicalWidth();
                        percentMissing -= percent;
                        if (percent > 0)
                            m_layoutStruct[pos].effectiveLogicalWidth.setValue(Percent, percent);
                        else
                            m_layoutStruct[pos].effectiveLogicalWidth = Length();
                    }
                }
            }
        }

        // make sure minWidth and maxWidth of the spanning cell are honoured
        if (cellMinLogicalWidth > spanMinLogicalWidth) {
            if (allColsAreFixed) {
                for (unsigned pos = effCol; fixedWidth > 0 && pos < lastCol; ++pos) {
                    int cellLogicalWidth = std::max(m_layoutStruct[pos].effectiveMinLogicalWidth, static_cast<int>(cellMinLogicalWidth * m_layoutStruct[pos].logicalWidth.value() / fixedWidth));
                    fixedWidth -= m_layoutStruct[pos].logicalWidth.value();
                    cellMinLogicalWidth -= cellLogicalWidth;
                    m_layoutStruct[pos].effectiveMinLogicalWidth = cellLogicalWidth;
                }
            } else if (allColsArePercent) {
                // In this case, we just split the colspan's min amd max widths following the percentage.
                int allocatedMinLogicalWidth = 0;
                int allocatedMaxLogicalWidth = 0;
                for (unsigned pos = effCol; pos < lastCol; ++pos) {
                    // TODO(alancutter): Make this work correctly for calc lengths.
                    ASSERT(m_layoutStruct[pos].logicalWidth.hasPercent() || m_layoutStruct[pos].effectiveLogicalWidth.hasPercent());
                    // |allColsArePercent| means that either the logicalWidth *or* the effectiveLogicalWidth are percents, handle both of them here.
                    float percent = m_layoutStruct[pos].logicalWidth.hasPercent() ? m_layoutStruct[pos].logicalWidth.percent() : m_layoutStruct[pos].effectiveLogicalWidth.percent();
                    int columnMinLogicalWidth = static_cast<int>(percent * cellMinLogicalWidth / totalPercent);
                    int columnMaxLogicalWidth = static_cast<int>(percent * cellMaxLogicalWidth / totalPercent);
                    m_layoutStruct[pos].effectiveMinLogicalWidth = std::max(m_layoutStruct[pos].effectiveMinLogicalWidth, columnMinLogicalWidth);
                    m_layoutStruct[pos].effectiveMaxLogicalWidth = columnMaxLogicalWidth;
                    allocatedMinLogicalWidth += columnMinLogicalWidth;
                    allocatedMaxLogicalWidth += columnMaxLogicalWidth;
                }
                ASSERT(allocatedMinLogicalWidth <= cellMinLogicalWidth);
                ASSERT(allocatedMaxLogicalWidth <= cellMaxLogicalWidth);
                cellMinLogicalWidth -= allocatedMinLogicalWidth;
                cellMaxLogicalWidth -= allocatedMaxLogicalWidth;
            } else {
                int remainingMaxLogicalWidth = spanMaxLogicalWidth;
                int remainingMinLogicalWidth = spanMinLogicalWidth;

                // Give min to variable first, to fixed second, and to others third.
                for (unsigned pos = effCol; remainingMaxLogicalWidth >= 0 && pos < lastCol; ++pos) {
                    if (m_layoutStruct[pos].logicalWidth.isFixed() && haveAuto && fixedWidth <= cellMinLogicalWidth) {
                        int colMinLogicalWidth = std::max<int>(m_layoutStruct[pos].effectiveMinLogicalWidth, m_layoutStruct[pos].logicalWidth.value());
                        fixedWidth -= m_layoutStruct[pos].logicalWidth.value();
                        remainingMinLogicalWidth -= m_layoutStruct[pos].effectiveMinLogicalWidth;
                        remainingMaxLogicalWidth -= m_layoutStruct[pos].effectiveMaxLogicalWidth;
                        cellMinLogicalWidth -= colMinLogicalWidth;
                        m_layoutStruct[pos].effectiveMinLogicalWidth = colMinLogicalWidth;
                    }
                }

                for (unsigned pos = effCol; remainingMaxLogicalWidth >= 0 && pos < lastCol && remainingMinLogicalWidth < cellMinLogicalWidth; ++pos) {
                    if (!(m_layoutStruct[pos].logicalWidth.isFixed() && haveAuto && fixedWidth <= cellMinLogicalWidth)) {
                        int colMinLogicalWidth = std::max<int>(m_layoutStruct[pos].effectiveMinLogicalWidth, static_cast<int>(remainingMaxLogicalWidth ? cellMinLogicalWidth * static_cast<float>(m_layoutStruct[pos].effectiveMaxLogicalWidth) / remainingMaxLogicalWidth : cellMinLogicalWidth));
                        colMinLogicalWidth = std::min<int>(m_layoutStruct[pos].effectiveMinLogicalWidth + (cellMinLogicalWidth - remainingMinLogicalWidth), colMinLogicalWidth);
                        remainingMaxLogicalWidth -= m_layoutStruct[pos].effectiveMaxLogicalWidth;
                        remainingMinLogicalWidth -= m_layoutStruct[pos].effectiveMinLogicalWidth;
                        cellMinLogicalWidth -= colMinLogicalWidth;
                        m_layoutStruct[pos].effectiveMinLogicalWidth = colMinLogicalWidth;
                    }
                }
            }
        }
        if (!cellLogicalWidth.hasPercent()) {
            if (cellMaxLogicalWidth > spanMaxLogicalWidth) {
                for (unsigned pos = effCol; spanMaxLogicalWidth >= 0 && pos < lastCol; ++pos) {
                    int colMaxLogicalWidth = std::max(m_layoutStruct[pos].effectiveMaxLogicalWidth, static_cast<int>(spanMaxLogicalWidth ? cellMaxLogicalWidth * static_cast<float>(m_layoutStruct[pos].effectiveMaxLogicalWidth) / spanMaxLogicalWidth : cellMaxLogicalWidth));
                    spanMaxLogicalWidth -= m_layoutStruct[pos].effectiveMaxLogicalWidth;
                    cellMaxLogicalWidth -= colMaxLogicalWidth;
                    m_layoutStruct[pos].effectiveMaxLogicalWidth = colMaxLogicalWidth;
                }
            }
        } else {
            for (unsigned pos = effCol; pos < lastCol; ++pos)
                m_layoutStruct[pos].maxLogicalWidth = std::max(m_layoutStruct[pos].maxLogicalWidth, m_layoutStruct[pos].minLogicalWidth);
        }
        // treat span ranges consisting of empty cells only as if they had content
        if (spanHasEmptyCellsOnly) {
            for (unsigned pos = effCol; pos < lastCol; ++pos)
                m_layoutStruct[pos].emptyCellsOnly = false;
        }
    }
    m_effectiveLogicalWidthDirty = false;

    return std::min(maxLogicalWidth, INT_MAX / 2);
}

/* gets all cells that originate in a column and have a cellspan > 1
   Sorts them by increasing cellspan
*/
void TableLayoutAlgorithmAuto::insertSpanCell(LayoutTableCell *cell)
{
    ASSERT_ARG(cell, cell && cell->colSpan() != 1);
    if (!cell || cell->colSpan() == 1)
        return;

    unsigned size = m_spanCells.size();
    if (!size || m_spanCells[size-1] != 0) {
        m_spanCells.grow(size + 10);
        for (unsigned i = 0; i < 10; i++)
            m_spanCells[size+i] = 0;
        size += 10;
    }

    // add them in sort. This is a slow algorithm, and a binary search or a fast sorting after collection would be better
    unsigned pos = 0;
    unsigned span = cell->colSpan();
    while (pos < m_spanCells.size() && m_spanCells[pos] && span > m_spanCells[pos]->colSpan())
        pos++;
    memmove(m_spanCells.data()+pos+1, m_spanCells.data()+pos, (size-pos-1)*sizeof(LayoutTableCell *));
    m_spanCells[pos] = cell;
}


void TableLayoutAlgorithmAuto::layout()
{
    // table layout based on the values collected in the layout structure.
    int tableLogicalWidth = m_table->logicalWidth() - m_table->bordersPaddingAndSpacingInRowDirection();
    int available = tableLogicalWidth;
    size_t nEffCols = m_table->numEffCols();

    // FIXME: It is possible to be called without having properly updated our internal representation.
    // This means that our preferred logical widths were not recomputed as expected.
    if (nEffCols != m_layoutStruct.size()) {
        fullRecalc();
        // FIXME: Table layout shouldn't modify our table structure (but does due to columns and column-groups).
        nEffCols = m_table->numEffCols();
    }

    if (m_effectiveLogicalWidthDirty)
        calcEffectiveLogicalWidth();

    bool havePercent = false;
    int numAuto = 0;
    int numFixed = 0;
    float totalAuto = 0;
    float totalFixed = 0;
    float totalPercent = 0;
    int allocAuto = 0;
    unsigned numAutoEmptyCellsOnly = 0;

    // fill up every cell with its minWidth
    for (size_t i = 0; i < nEffCols; ++i) {
        int cellLogicalWidth = m_layoutStruct[i].effectiveMinLogicalWidth;
        m_layoutStruct[i].computedLogicalWidth = cellLogicalWidth;
        available -= cellLogicalWidth;
        Length& logicalWidth = m_layoutStruct[i].effectiveLogicalWidth;
        switch (logicalWidth.type()) {
        case Percent:
            havePercent = true;
            totalPercent += logicalWidth.percent();
            break;
        case Fixed:
            numFixed++;
            totalFixed += m_layoutStruct[i].clampedEffectiveMaxLogicalWidth();
            // fall through
            break;
        case Auto:
            if (m_layoutStruct[i].emptyCellsOnly) {
                numAutoEmptyCellsOnly++;
            } else {
                numAuto++;
                totalAuto += m_layoutStruct[i].clampedEffectiveMaxLogicalWidth();
                allocAuto += cellLogicalWidth;
            }
            break;
        default:
            break;
        }
    }

    // allocate width to percent cols
    if (available > 0 && havePercent) {
        for (size_t i = 0; i < nEffCols; ++i) {
            Length& logicalWidth = m_layoutStruct[i].effectiveLogicalWidth;
            if (logicalWidth.hasPercent()) {
                int cellLogicalWidth = std::max<int>(m_layoutStruct[i].effectiveMinLogicalWidth, minimumValueForLength(logicalWidth, tableLogicalWidth));
                available += m_layoutStruct[i].computedLogicalWidth - cellLogicalWidth;
                m_layoutStruct[i].computedLogicalWidth = cellLogicalWidth;
            }
        }
        if (totalPercent > 100) {
            // remove overallocated space from the last columns
            int excess = tableLogicalWidth * (totalPercent - 100) / 100;
            for (unsigned i = nEffCols; i; ) {
                --i;
                if (m_layoutStruct[i].effectiveLogicalWidth.hasPercent()) {
                    int cellLogicalWidth = m_layoutStruct[i].computedLogicalWidth;
                    int reduction = std::min(cellLogicalWidth,  excess);
                    // the lines below might look inconsistent, but that's the way it's handled in mozilla
                    excess -= reduction;
                    int newLogicalWidth = std::max<int>(m_layoutStruct[i].effectiveMinLogicalWidth, cellLogicalWidth - reduction);
                    available += cellLogicalWidth - newLogicalWidth;
                    m_layoutStruct[i].computedLogicalWidth = newLogicalWidth;
                }
            }
        }
    }

    // then allocate width to fixed cols
    if (available > 0) {
        for (size_t i = 0; i < nEffCols; ++i) {
            Length& logicalWidth = m_layoutStruct[i].effectiveLogicalWidth;
            if (logicalWidth.isFixed() && logicalWidth.value() > m_layoutStruct[i].computedLogicalWidth) {
                available += m_layoutStruct[i].computedLogicalWidth - logicalWidth.value();
                m_layoutStruct[i].computedLogicalWidth = logicalWidth.value();
            }
        }
    }

    // Give each auto width column its share of the available width, non-empty columns then empty columns.
    if (available > 0 && numAuto) {
        available += allocAuto;
        distributeWidthToColumns<float, Auto, NonEmptyCells, InitialWidth, StartToEnd>(available, totalAuto);
    }
    if (available > 0 && numAutoEmptyCellsOnly) {
        unsigned total = numAutoEmptyCellsOnly;
        distributeWidthToColumns<unsigned, Auto, EmptyCells, InitialWidth, StartToEnd>(available, total);
    }

    // Any remaining available width expands fixed width, percent width, and non-empty auto width columns, in that order.
    if (available > 0 && numFixed)
        distributeWidthToColumns<float, Fixed, AllCells, ExtraWidth, StartToEnd>(available, totalFixed);

    if (available > 0 && m_hasPercent && totalPercent < 100)
        distributeWidthToColumns<float, Percent, AllCells, ExtraWidth, StartToEnd>(available, totalPercent);

    if (available > 0 && nEffCols > numAutoEmptyCellsOnly) {
        unsigned total = nEffCols - numAutoEmptyCellsOnly;
        // Starting from the last cell is for compatibility with FF/IE - it isn't specified anywhere.
        distributeWidthToColumns<unsigned, Auto, NonEmptyCells, LeftoverWidth, EndToStart>(available, total);
    }

    // If we have overallocated, reduce every cell according to the difference between desired width and minwidth
    // this seems to produce to the pixel exact results with IE. Wonder is some of this also holds for width distributing.
    // This is basically the reverse of how we grew the cells.
    if (available < 0)
        shrinkColumnWidth(Auto, available);
    if (available < 0)
        shrinkColumnWidth(Fixed, available);
    if (available < 0)
        shrinkColumnWidth(Percent, available);

    int pos = 0;
    for (size_t i = 0; i < nEffCols; ++i) {
        m_table->setColumnPosition(i, pos);
        pos += m_layoutStruct[i].computedLogicalWidth + m_table->hBorderSpacing();
    }
    m_table->setColumnPosition(m_table->columnPositions().size() - 1, pos);
}

template<typename Total, LengthType lengthType, CellsToProcess cellsToProcess, DistributionMode distributionMode, DistributionDirection distributionDirection>
void TableLayoutAlgorithmAuto::distributeWidthToColumns(int& available, Total total)
{
    // TODO(alancutter): Make this work correctly for calc lengths.
    int nEffCols = static_cast<int>(m_table->numEffCols());
    bool startToEnd = distributionDirection == StartToEnd;
    for (int i = startToEnd ? 0 : nEffCols - 1; startToEnd ? i < nEffCols : i > -1; startToEnd ? ++i : --i) {
        const Length& logicalWidth = m_layoutStruct[i].effectiveLogicalWidth;
        if (cellsToProcess == NonEmptyCells && logicalWidth.isAuto() && m_layoutStruct[i].emptyCellsOnly)
            continue;
        // When allocating width to columns with nothing but empty cells we avoid
        // columns that exist only to flesh out a colspan and have no actual cells.
        if (cellsToProcess == EmptyCells && logicalWidth.isAuto() && (!m_layoutStruct[i].emptyCellsOnly || m_layoutStruct[i].columnHasNoCells))
            continue;
        if (distributionMode != LeftoverWidth && logicalWidth.type() != lengthType)
            continue;

        float factor = 1;
        if (distributionMode != LeftoverWidth) {
            if (lengthType == Percent)
                factor = logicalWidth.percent();
            else if (lengthType == Auto || lengthType == Fixed)
                factor = m_layoutStruct[i].clampedEffectiveMaxLogicalWidth();
        }

        int newWidth = available * factor / total;
        int cellLogicalWidth = (distributionMode == InitialWidth) ? max<int>(m_layoutStruct[i].computedLogicalWidth, newWidth) : newWidth;
        available -= cellLogicalWidth;
        total -= factor;
        m_layoutStruct[i].computedLogicalWidth = (distributionMode == InitialWidth) ? cellLogicalWidth : m_layoutStruct[i].computedLogicalWidth + cellLogicalWidth;

        // If we have run out of width to allocate we're done.
        // TODO(rhogan): Extend this to Fixed as well.
        if (lengthType == Percent && (!available || !total))
            return;
        if (lengthType == Auto && !total)
            return;
    }
}

void TableLayoutAlgorithmAuto::shrinkColumnWidth(const LengthType& lengthType, int& available)
{
    size_t nEffCols = m_table->numEffCols();
    int logicalWidthBeyondMin = 0;
    for (unsigned i = nEffCols; i; ) {
        --i;
        Length& logicalWidth = m_layoutStruct[i].effectiveLogicalWidth;
        if (logicalWidth.type() == lengthType)
            logicalWidthBeyondMin += m_layoutStruct[i].computedLogicalWidth - m_layoutStruct[i].effectiveMinLogicalWidth;
    }

    for (unsigned i = nEffCols; i && logicalWidthBeyondMin > 0; ) {
        --i;
        Length& logicalWidth = m_layoutStruct[i].effectiveLogicalWidth;
        if (logicalWidth.type() == lengthType) {
            int minMaxDiff = m_layoutStruct[i].computedLogicalWidth - m_layoutStruct[i].effectiveMinLogicalWidth;
            int reduce = available * minMaxDiff / logicalWidthBeyondMin;
            m_layoutStruct[i].computedLogicalWidth += reduce;
            available -= reduce;
            logicalWidthBeyondMin -= minMaxDiff;
            if (available >= 0)
                break;
        }
    }
}
}
