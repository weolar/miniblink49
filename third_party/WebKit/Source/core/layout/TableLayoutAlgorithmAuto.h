/*
 * Copyright (C) 2002 Lars Knoll (knoll@kde.org)
 *           (C) 2002 Dirk Mueller (mueller@kde.org)
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

#ifndef TableLayoutAlgorithmAuto_h
#define TableLayoutAlgorithmAuto_h

#include "core/layout/TableLayoutAlgorithm.h"
#include "platform/LayoutUnit.h"
#include "platform/Length.h"
#include "wtf/Vector.h"

namespace blink {

class LayoutTable;
class LayoutTableCell;

enum CellsToProcess {
    AllCells,
    NonEmptyCells,
    EmptyCells
};

enum DistributionMode {
    ExtraWidth,
    InitialWidth,
    LeftoverWidth
};

enum DistributionDirection {
    StartToEnd,
    EndToStart
};

class TableLayoutAlgorithmAuto final : public TableLayoutAlgorithm {
public:
    TableLayoutAlgorithmAuto(LayoutTable*);
    virtual ~TableLayoutAlgorithmAuto();

    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minWidth, LayoutUnit& maxWidth) override;
    virtual void applyPreferredLogicalWidthQuirks(LayoutUnit& minWidth, LayoutUnit& maxWidth) const override;
    virtual void layout() override;
    virtual void willChangeTableLayout() override { }

private:
    void fullRecalc();
    void recalcColumn(unsigned effCol);

    int calcEffectiveLogicalWidth();
    void shrinkColumnWidth(const LengthType&, int& available);
    template<typename Total, LengthType, CellsToProcess, DistributionMode, DistributionDirection> void distributeWidthToColumns(int& available, Total);

    void insertSpanCell(LayoutTableCell*);

    struct Layout {
        Layout()
            : minLogicalWidth(0)
            , maxLogicalWidth(0)
            , effectiveMinLogicalWidth(0)
            , effectiveMaxLogicalWidth(0)
            , computedLogicalWidth(0)
            , emptyCellsOnly(true)
            , columnHasNoCells(true)
        {
        }

        Length logicalWidth;
        Length effectiveLogicalWidth;
        int minLogicalWidth;
        int maxLogicalWidth;
        int effectiveMinLogicalWidth;
        int effectiveMaxLogicalWidth;
        int computedLogicalWidth;
        bool emptyCellsOnly;
        bool columnHasNoCells;
        int clampedEffectiveMaxLogicalWidth() { return std::max<int>(1, effectiveMaxLogicalWidth); }
    };

    Vector<Layout, 4> m_layoutStruct;
    Vector<LayoutTableCell*, 4> m_spanCells;
    bool m_hasPercent : 1;
    mutable bool m_effectiveLogicalWidthDirty : 1;
};

} // namespace blink

#endif // TableLayoutAlgorithmAuto
