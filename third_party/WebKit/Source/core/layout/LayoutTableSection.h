/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2009, 2013 Apple Inc. All rights reserved.
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

#ifndef LayoutTableSection_h
#define LayoutTableSection_h

#include "core/CoreExport.h"
#include "core/layout/LayoutTable.h"
#include "wtf/Vector.h"

namespace blink {

// This variable is used to balance the memory consumption vs the paint invalidation time on big tables.
const float gMaxAllowedOverflowingCellRatioForFastPaintPath = 0.1f;

enum CollapsedBorderSide {
    CBSBefore,
    CBSAfter,
    CBSStart,
    CBSEnd
};

// Helper class for paintObject.
class CellSpan {
public:
    CellSpan(unsigned start, unsigned end)
        : m_start(start)
        , m_end(end)
    {
    }

    unsigned start() const { return m_start; }
    unsigned end() const { return m_end; }

    void decreaseStart() { --m_start; }
    void increaseEnd() { ++m_end; }

    void ensureConsistency(const unsigned);

private:
    unsigned m_start;
    unsigned m_end;
};

class LayoutTableCell;
class LayoutTableRow;

class CORE_EXPORT LayoutTableSection final : public LayoutBox {
public:
    LayoutTableSection(Element*);
    virtual ~LayoutTableSection();

    LayoutTableRow* firstRow() const;
    LayoutTableRow* lastRow() const;

    const LayoutObjectChildList* children() const { return &m_children; }
    LayoutObjectChildList* children() { return &m_children; }

    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override;

    virtual int firstLineBoxBaseline() const override;

    void addCell(LayoutTableCell*, LayoutTableRow*);

    int calcRowLogicalHeight();
    void layoutRows();
    void computeOverflowFromCells();

    LayoutTable* table() const { return toLayoutTable(parent()); }

    typedef Vector<LayoutTableCell*, 2> SpanningLayoutTableCells;

    struct CellStruct {
    public:
        Vector<LayoutTableCell*, 1> cells;
        bool inColSpan; // true for columns after the first in a colspan

        CellStruct()
            : inColSpan(false)
        {
        }

        LayoutTableCell* primaryCell()
        {
            return hasCells() ? cells[cells.size() - 1] : 0;
        }

        const LayoutTableCell* primaryCell() const
        {
            return hasCells() ? cells[cells.size() - 1] : 0;
        }

        bool hasCells() const { return cells.size() > 0; }
    };

    typedef Vector<CellStruct> Row;

    struct RowStruct {
    public:
        RowStruct()
            : rowLayoutObject(nullptr)
            , baseline(-1)
        {
        }

        Row row;
        LayoutTableRow* rowLayoutObject;
        LayoutUnit baseline;
        Length logicalHeight;
    };

    struct SpanningRowsHeight {
        WTF_MAKE_NONCOPYABLE(SpanningRowsHeight);

    public:
        SpanningRowsHeight()
            : totalRowsHeight(0)
            , spanningCellHeightIgnoringBorderSpacing(0)
            , isAnyRowWithOnlySpanningCells(false)
        {
        }

        Vector<int> rowHeight;
        int totalRowsHeight;
        int spanningCellHeightIgnoringBorderSpacing;
        bool isAnyRowWithOnlySpanningCells;
    };

    const BorderValue& borderAdjoiningTableStart() const
    {
        if (hasSameDirectionAs(table()))
            return style()->borderStart();

        return style()->borderEnd();
    }

    const BorderValue& borderAdjoiningTableEnd() const
    {
        if (hasSameDirectionAs(table()))
            return style()->borderEnd();

        return style()->borderStart();
    }

    const BorderValue& borderAdjoiningStartCell(const LayoutTableCell*) const;
    const BorderValue& borderAdjoiningEndCell(const LayoutTableCell*) const;

    const LayoutTableCell* firstRowCellAdjoiningTableStart() const;
    const LayoutTableCell* firstRowCellAdjoiningTableEnd() const;

    CellStruct& cellAt(unsigned row,  unsigned col) { return m_grid[row].row[col]; }
    const CellStruct& cellAt(unsigned row, unsigned col) const { return m_grid[row].row[col]; }
    LayoutTableCell* primaryCellAt(unsigned row, unsigned col)
    {
        CellStruct& c = m_grid[row].row[col];
        return c.primaryCell();
    }

    LayoutTableRow* rowLayoutObjectAt(unsigned row) const { return m_grid[row].rowLayoutObject; }

    void appendColumn(unsigned pos);
    void splitColumn(unsigned pos, unsigned first);

    enum BlockBorderSide { BorderBefore, BorderAfter };
    int calcBlockDirectionOuterBorder(BlockBorderSide) const;
    enum InlineBorderSide { BorderStart, BorderEnd };
    int calcInlineDirectionOuterBorder(InlineBorderSide) const;
    void recalcOuterBorder();

    int outerBorderBefore() const { return m_outerBorderBefore; }
    int outerBorderAfter() const { return m_outerBorderAfter; }
    int outerBorderStart() const { return m_outerBorderStart; }
    int outerBorderEnd() const { return m_outerBorderEnd; }

    unsigned numRows() const { return m_grid.size(); }
    unsigned numColumns() const;
    void recalcCells();
    void recalcCellsIfNeeded()
    {
        if (m_needsCellRecalc)
            recalcCells();
    }

    bool needsCellRecalc() const { return m_needsCellRecalc; }
    void setNeedsCellRecalc();

    LayoutUnit rowBaseline(unsigned row) { return m_grid[row].baseline; }

    void rowLogicalHeightChanged(LayoutTableRow*);

    void removeCachedCollapsedBorders(const LayoutTableCell*);
    bool setCachedCollapsedBorder(const LayoutTableCell*, CollapsedBorderSide, const CollapsedBorderValue&);
    const CollapsedBorderValue& cachedCollapsedBorder(const LayoutTableCell*, CollapsedBorderSide) const;

    // distributeExtraLogicalHeightToRows methods return the *consumed* extra logical height.
    // FIXME: We may want to introduce a structure holding the in-flux layout information.
    int distributeExtraLogicalHeightToRows(int extraLogicalHeight);

    static LayoutTableSection* createAnonymousWithParent(const LayoutObject*);
    virtual LayoutBox* createAnonymousBoxWithSameTypeAs(const LayoutObject* parent) const override
    {
        return createAnonymousWithParent(parent);
    }

    virtual void paint(const PaintInfo&, const LayoutPoint&) override;

    // Flip the rect so it aligns with the coordinates used by the rowPos and columnPos vectors.
    LayoutRect logicalRectForWritingModeAndDirection(const LayoutRect&) const;

    CellSpan dirtiedRows(const LayoutRect& paintInvalidationRect) const;
    CellSpan dirtiedColumns(const LayoutRect& paintInvalidationRect) const;
    HashSet<LayoutTableCell*>& overflowingCells() { return m_overflowingCells; }
    bool hasMultipleCellLevels() { return m_hasMultipleCellLevels; }

    virtual const char* name() const override { return "LayoutTableSection"; }

protected:
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override;

private:
    virtual LayoutObjectChildList* virtualChildren() override { return children(); }
    virtual const LayoutObjectChildList* virtualChildren() const override { return children(); }

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectTableSection || LayoutBox::isOfType(type); }

    virtual void willBeRemovedFromTree() override;

    virtual void layout() override;

    virtual void imageChanged(WrappedImagePtr, const IntRect* = nullptr) override;

    int borderSpacingForRow(unsigned row) const { return m_grid[row].rowLayoutObject ? table()->vBorderSpacing() : 0; }

    void ensureRows(unsigned);

    bool rowHasOnlySpanningCells(unsigned);
    unsigned calcRowHeightHavingOnlySpanningCells(unsigned, int&, unsigned, unsigned&, Vector<int>&);
    void updateRowsHeightHavingOnlySpanningCells(LayoutTableCell*, struct SpanningRowsHeight&, unsigned&, Vector<int>&);

    void populateSpanningRowsHeightFromCell(LayoutTableCell*, struct SpanningRowsHeight&);
    void distributeExtraRowSpanHeightToPercentRows(LayoutTableCell*, int, int&, Vector<int>&);
    void distributeWholeExtraRowSpanHeightToPercentRows(LayoutTableCell*, float, int&, Vector<int>&);
    void distributeExtraRowSpanHeightToAutoRows(LayoutTableCell*, int, int&, Vector<int>&);
    void distributeExtraRowSpanHeightToRemainingRows(LayoutTableCell*, int, int&, Vector<int>&);
    void distributeRowSpanHeightToRows(SpanningLayoutTableCells& rowSpanCells);

    void distributeExtraLogicalHeightToPercentRows(int& extraLogicalHeight, int totalPercent);
    void distributeExtraLogicalHeightToAutoRows(int& extraLogicalHeight, unsigned autoRowsCount);
    void distributeRemainingExtraLogicalHeight(int& extraLogicalHeight);

    void updateBaselineForCell(LayoutTableCell*, unsigned row, LayoutUnit& baselineDescent);

    bool hasOverflowingCell() const { return m_overflowingCells.size() || m_forceSlowPaintPathWithOverflowingCell; }

    void computeOverflowFromCells(unsigned totalRows, unsigned nEffCols);

    CellSpan fullTableRowSpan() const { return CellSpan(0, m_grid.size()); }
    CellSpan fullTableColumnSpan() const { return CellSpan(0, table()->columns().size()); }

    // These two functions take a rectangle as input that has been flipped by logicalRectForWritingModeAndDirection.
    // The returned span of rows or columns is end-exclusive, and empty if start==end.
    CellSpan spannedRows(const LayoutRect& flippedRect) const;
    CellSpan spannedColumns(const LayoutRect& flippedRect) const;

    void setLogicalPositionForCell(LayoutTableCell*, unsigned effectiveColumn) const;

    LayoutObjectChildList m_children;

    Vector<RowStruct> m_grid;
    Vector<int> m_rowPos;

    // the current insertion position
    unsigned m_cCol;
    unsigned m_cRow;

    int m_outerBorderStart;
    int m_outerBorderEnd;
    int m_outerBorderBefore;
    int m_outerBorderAfter;

    bool m_needsCellRecalc;

    // This HashSet holds the overflowing cells for faster painting.
    // If we have more than gMaxAllowedOverflowingCellRatio * total cells, it will be empty
    // and m_forceSlowPaintPathWithOverflowingCell will be set to save memory.
    HashSet<LayoutTableCell*> m_overflowingCells;
    bool m_forceSlowPaintPathWithOverflowingCell;

    bool m_hasMultipleCellLevels;

    // This map holds the collapsed border values for cells with collapsed borders.
    // It is held at LayoutTableSection level to spare memory consumption by table cells.
    using CellsCollapsedBordersMap = HashMap<pair<const LayoutTableCell*, int>, CollapsedBorderValue>;
    CellsCollapsedBordersMap m_cellsCollapsedBorders;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutTableSection, isTableSection());

} // namespace blink

#endif // LayoutTableSection_h
