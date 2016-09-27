// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GridResolvedPosition_h
#define GridResolvedPosition_h

#include "core/style/GridPosition.h"

namespace blink {

struct GridSpan;
class LayoutBox;
class ComputedStyle;

enum GridPositionSide {
    ColumnStartSide,
    ColumnEndSide,
    RowStartSide,
    RowEndSide
};

enum GridTrackSizingDirection {
    ForColumns,
    ForRows
};

// This class represents an index into one of the dimensions of the grid array.
// Wraps a size_t integer just for the purpose of knowing what we manipulate in the grid code.
class GridResolvedPosition {
public:
    static GridResolvedPosition adjustGridPositionForAfterEndSide(size_t resolvedPosition)
    {
        return resolvedPosition ? GridResolvedPosition(resolvedPosition - 1) : GridResolvedPosition(0);
    }

    static GridResolvedPosition adjustGridPositionForSide(size_t resolvedPosition, GridPositionSide side)
    {
        // An item finishing on the N-th line belongs to the N-1-th cell.
        if (side == ColumnEndSide || side == RowEndSide)
            return adjustGridPositionForAfterEndSide(resolvedPosition);

        return GridResolvedPosition(resolvedPosition);
    }

    static void initialAndFinalPositionsFromStyle(const ComputedStyle&, const LayoutBox&, GridTrackSizingDirection, GridPosition &initialPosition, GridPosition &finalPosition);
    static GridSpan resolveGridPositionsFromAutoPlacementPosition(const ComputedStyle&, const LayoutBox&, GridTrackSizingDirection, const GridResolvedPosition&);
    static PassOwnPtr<GridSpan> resolveGridPositionsFromStyle(const ComputedStyle&, const LayoutBox&, GridTrackSizingDirection);
    static GridResolvedPosition resolveNamedGridLinePositionFromStyle(const ComputedStyle&, const GridPosition&, GridPositionSide);
    static GridResolvedPosition resolveGridPositionFromStyle(const ComputedStyle&, const GridPosition&, GridPositionSide);
    static PassOwnPtr<GridSpan> resolveGridPositionAgainstOppositePosition(const ComputedStyle&, const GridResolvedPosition& resolvedOppositePosition, const GridPosition&, GridPositionSide);
    static PassOwnPtr<GridSpan> resolveNamedGridLinePositionAgainstOppositePosition(const ComputedStyle&, const GridResolvedPosition& resolvedOppositePosition, const GridPosition&, GridPositionSide);

    GridResolvedPosition(size_t position)
        : m_integerPosition(position)
    {
    }

    GridResolvedPosition(const GridPosition& position, GridPositionSide side)
    {
        ASSERT(position.integerPosition());
        size_t integerPosition = position.integerPosition() - 1;

        m_integerPosition = adjustGridPositionForSide(integerPosition, side).toInt();
    }

    GridResolvedPosition& operator*()
    {
        return *this;
    }

    GridResolvedPosition& operator++()
    {
        m_integerPosition++;
        return *this;
    }

    bool operator==(const GridResolvedPosition& other) const
    {
        return m_integerPosition == other.m_integerPosition;
    }

    bool operator!=(const GridResolvedPosition& other) const
    {
        return m_integerPosition != other.m_integerPosition;
    }

    bool operator<(const GridResolvedPosition& other) const
    {
        return m_integerPosition < other.m_integerPosition;
    }

    bool operator>(const GridResolvedPosition& other) const
    {
        return m_integerPosition > other.m_integerPosition;
    }

    bool operator<=(const GridResolvedPosition& other) const
    {
        return m_integerPosition <= other.m_integerPosition;
    }

    bool operator>=(const GridResolvedPosition& other) const
    {
        return m_integerPosition >= other.m_integerPosition;
    }

    size_t toInt() const
    {
        return m_integerPosition;
    }

    GridResolvedPosition next() const
    {
        return GridResolvedPosition(m_integerPosition + 1);
    }

    static size_t explicitGridColumnCount(const ComputedStyle&);
    static size_t explicitGridRowCount(const ComputedStyle&);

private:

    static size_t explicitGridSizeForSide(const ComputedStyle&, GridPositionSide);

    size_t m_integerPosition;
};

} // namespace blink

#endif // GridResolvedPosition_h
