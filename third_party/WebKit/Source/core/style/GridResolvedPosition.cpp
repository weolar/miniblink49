// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/style/GridResolvedPosition.h"

#include "core/layout/LayoutBox.h"
#include "core/style/GridCoordinate.h"

namespace blink {

static const NamedGridLinesMap& gridLinesForSide(const ComputedStyle& style, GridPositionSide side)
{
    return (side == ColumnStartSide || side == ColumnEndSide) ? style.namedGridColumnLines() : style.namedGridRowLines();
}

static inline String implicitNamedGridLineForSide(const String& lineName, GridPositionSide side)
{
    return lineName + ((side == ColumnStartSide || side == RowStartSide) ? "-start" : "-end");
}

static bool isValidNamedLineOrArea(const String& lineName, const ComputedStyle& style, GridPositionSide side)
{
    const NamedGridLinesMap& gridLineNames = gridLinesForSide(style, side);

    return gridLineNames.contains(implicitNamedGridLineForSide(lineName, side)) || gridLineNames.contains(lineName);
}

static GridPositionSide calculateInitialPositionSide(GridTrackSizingDirection direction)
{
    return (direction == ForColumns) ? ColumnStartSide : RowStartSide;
}

static GridPositionSide calculateFinalPositionSide(GridTrackSizingDirection direction)
{
    return (direction == ForColumns) ? ColumnEndSide : RowEndSide;
}

void GridResolvedPosition::initialAndFinalPositionsFromStyle(const ComputedStyle& gridContainerStyle, const LayoutBox& gridItem, GridTrackSizingDirection direction, GridPosition& initialPosition, GridPosition& finalPosition)
{
    initialPosition = (direction == ForColumns) ? gridItem.style()->gridColumnStart() : gridItem.style()->gridRowStart();
    finalPosition = (direction == ForColumns) ? gridItem.style()->gridColumnEnd() : gridItem.style()->gridRowEnd();
    GridPositionSide initialPositionSide = calculateInitialPositionSide(direction);
    GridPositionSide finalPositionSide = calculateFinalPositionSide(direction);

    // We must handle the placement error handling code here instead of in the StyleAdjuster because we don't want to
    // overwrite the specified values.
    if (initialPosition.isSpan() && finalPosition.isSpan())
        finalPosition.setAutoPosition();

    // Try to early detect the case of non existing named grid lines. This way we could assume later that
    // GridResolvedPosition::resolveGrisPositionFromStyle() always return a valid resolved position.
    if (initialPosition.isNamedGridArea() && !isValidNamedLineOrArea(initialPosition.namedGridLine(), gridContainerStyle, initialPositionSide))
        initialPosition.setAutoPosition();

    if (finalPosition.isNamedGridArea() && !isValidNamedLineOrArea(finalPosition.namedGridLine(), gridContainerStyle, finalPositionSide))
        finalPosition.setAutoPosition();

    // If the grid item has an automatic position and a grid span for a named line in a given dimension, instead treat the grid span as one.
    if (initialPosition.isAuto() && finalPosition.isSpan() && !finalPosition.namedGridLine().isNull())
        finalPosition.setSpanPosition(1, String());
    if (finalPosition.isAuto() && initialPosition.isSpan() && !initialPosition.namedGridLine().isNull())
        initialPosition.setSpanPosition(1, String());
}

GridSpan GridResolvedPosition::resolveGridPositionsFromAutoPlacementPosition(const ComputedStyle& gridContainerStyle, const LayoutBox& gridItem, GridTrackSizingDirection direction, const GridResolvedPosition& resolvedInitialPosition)
{
    GridPosition initialPosition, finalPosition;
    initialAndFinalPositionsFromStyle(gridContainerStyle, gridItem, direction, initialPosition, finalPosition);

    GridPositionSide finalPositionSide = calculateFinalPositionSide(direction);

    // This method will only be used when both positions need to be resolved against the opposite one.
    ASSERT(initialPosition.shouldBeResolvedAgainstOppositePosition() && finalPosition.shouldBeResolvedAgainstOppositePosition());

    GridResolvedPosition resolvedFinalPosition = resolvedInitialPosition;

    if (initialPosition.isSpan())
        return *resolveGridPositionAgainstOppositePosition(gridContainerStyle, resolvedInitialPosition, initialPosition, finalPositionSide);
    if (finalPosition.isSpan())
        return *resolveGridPositionAgainstOppositePosition(gridContainerStyle, resolvedInitialPosition, finalPosition, finalPositionSide);

    return GridSpan(resolvedInitialPosition, resolvedFinalPosition);
}

PassOwnPtr<GridSpan> GridResolvedPosition::resolveGridPositionsFromStyle(const ComputedStyle& gridContainerStyle, const LayoutBox& gridItem, GridTrackSizingDirection direction)
{
    GridPosition initialPosition, finalPosition;
    initialAndFinalPositionsFromStyle(gridContainerStyle, gridItem, direction, initialPosition, finalPosition);

    GridPositionSide initialPositionSide = calculateInitialPositionSide(direction);
    GridPositionSide finalPositionSide = calculateFinalPositionSide(direction);

    if (initialPosition.shouldBeResolvedAgainstOppositePosition() && finalPosition.shouldBeResolvedAgainstOppositePosition()) {
        // We can't get our grid positions without running the auto placement algorithm.
        return nullptr;
    }

    if (initialPosition.shouldBeResolvedAgainstOppositePosition()) {
        // Infer the position from the final position ('auto / 1' or 'span 2 / 3' case).
        GridResolvedPosition finalResolvedPosition = resolveGridPositionFromStyle(gridContainerStyle, finalPosition, finalPositionSide);
        return resolveGridPositionAgainstOppositePosition(gridContainerStyle, finalResolvedPosition, initialPosition, initialPositionSide);
    }

    if (finalPosition.shouldBeResolvedAgainstOppositePosition()) {
        // Infer our position from the initial position ('1 / auto' or '3 / span 2' case).
        GridResolvedPosition initialResolvedPosition = resolveGridPositionFromStyle(gridContainerStyle, initialPosition, initialPositionSide);
        return resolveGridPositionAgainstOppositePosition(gridContainerStyle, initialResolvedPosition, finalPosition, finalPositionSide);
    }

    GridResolvedPosition resolvedInitialPosition = resolveGridPositionFromStyle(gridContainerStyle, initialPosition, initialPositionSide);
    GridResolvedPosition resolvedFinalPosition = resolveGridPositionFromStyle(gridContainerStyle, finalPosition, finalPositionSide);

    // If 'grid-after' specifies a line at or before that specified by 'grid-before', it computes to 'span 1'.
    if (resolvedFinalPosition < resolvedInitialPosition)
        resolvedFinalPosition = resolvedInitialPosition;

    return adoptPtr(new GridSpan(resolvedInitialPosition, resolvedFinalPosition));
}

size_t GridResolvedPosition::explicitGridColumnCount(const ComputedStyle& gridContainerStyle)
{
    return std::min(gridContainerStyle.gridTemplateColumns().size(), kGridMaxTracks);
}

size_t GridResolvedPosition::explicitGridRowCount(const ComputedStyle& gridContainerStyle)
{
    return std::min(gridContainerStyle.gridTemplateRows().size(), kGridMaxTracks);
}

size_t GridResolvedPosition::explicitGridSizeForSide(const ComputedStyle& gridContainerStyle, GridPositionSide side)
{
    return (side == ColumnStartSide || side == ColumnEndSide) ? explicitGridColumnCount(gridContainerStyle) : explicitGridRowCount(gridContainerStyle);
}

GridResolvedPosition GridResolvedPosition::resolveNamedGridLinePositionFromStyle(const ComputedStyle& gridContainerStyle, const GridPosition& position, GridPositionSide side)
{
    ASSERT(!position.namedGridLine().isNull());

    const NamedGridLinesMap& gridLinesNames = gridLinesForSide(gridContainerStyle, side);
    NamedGridLinesMap::const_iterator it = gridLinesNames.find(position.namedGridLine());
    if (it == gridLinesNames.end()) {
        if (position.isPositive())
            return GridResolvedPosition(0);
        const size_t lastLine = explicitGridSizeForSide(gridContainerStyle, side);
        return adjustGridPositionForSide(lastLine, side);
    }

    size_t namedGridLineIndex;
    if (position.isPositive())
        namedGridLineIndex = std::min<size_t>(position.integerPosition(), it->value.size()) - 1;
    else
        namedGridLineIndex = std::max<int>(it->value.size() - abs(position.integerPosition()), 0);
    return adjustGridPositionForSide(it->value[namedGridLineIndex], side);
}

GridResolvedPosition GridResolvedPosition::resolveGridPositionFromStyle(const ComputedStyle& gridContainerStyle, const GridPosition& position, GridPositionSide side)
{
    switch (position.type()) {
    case ExplicitPosition: {
        ASSERT(position.integerPosition());

        if (!position.namedGridLine().isNull())
            return resolveNamedGridLinePositionFromStyle(gridContainerStyle, position, side);

        // Handle <integer> explicit position.
        if (position.isPositive())
            return adjustGridPositionForSide(position.integerPosition() - 1, side);

        size_t resolvedPosition = abs(position.integerPosition()) - 1;
        const size_t endOfTrack = explicitGridSizeForSide(gridContainerStyle, side);

        // Per http://lists.w3.org/Archives/Public/www-style/2013Mar/0589.html, we clamp negative value to the first line.
        if (endOfTrack < resolvedPosition)
            return GridResolvedPosition(0);

        return adjustGridPositionForSide(endOfTrack - resolvedPosition, side);
    }
    case NamedGridAreaPosition:
    {
        // First attempt to match the grid area's edge to a named grid area: if there is a named line with the name
        // ''<custom-ident>-start (for grid-*-start) / <custom-ident>-end'' (for grid-*-end), contributes the first such
        // line to the grid item's placement.
        String namedGridLine = position.namedGridLine();
        ASSERT(isValidNamedLineOrArea(namedGridLine, gridContainerStyle, side));

        const NamedGridLinesMap& gridLineNames = gridLinesForSide(gridContainerStyle, side);
        NamedGridLinesMap::const_iterator implicitLineIter = gridLineNames.find(implicitNamedGridLineForSide(namedGridLine, side));
        if (implicitLineIter != gridLineNames.end())
            return adjustGridPositionForSide(implicitLineIter->value[0], side);

        // Otherwise, if there is a named line with the specified name, contributes the first such line to the grid
        // item's placement.
        NamedGridLinesMap::const_iterator explicitLineIter = gridLineNames.find(namedGridLine);
        if (explicitLineIter != gridLineNames.end())
            return adjustGridPositionForSide(explicitLineIter->value[0], side);

        // If none of the above works specs mandate us to treat it as auto BUT we should have detected it before calling
        // this function in GridResolvedPosition::resolveGridPositionsFromStyle(). We should be also covered by the
        // ASSERT at the beginning of this block.
        ASSERT_NOT_REACHED();
        return GridResolvedPosition(0);
    }
    case AutoPosition:
    case SpanPosition:
        // 'auto' and span depend on the opposite position for resolution (e.g. grid-row: auto / 1 or grid-column: span 3 / "myHeader").
        ASSERT_NOT_REACHED();
        return GridResolvedPosition(0);
    }
    ASSERT_NOT_REACHED();
    return GridResolvedPosition(0);
}

PassOwnPtr<GridSpan> GridResolvedPosition::resolveGridPositionAgainstOppositePosition(const ComputedStyle& gridContainerStyle, const GridResolvedPosition& resolvedOppositePosition, const GridPosition& position, GridPositionSide side)
{
    if (position.isAuto())
        return GridSpan::create(resolvedOppositePosition, resolvedOppositePosition);

    ASSERT(position.isSpan());
    ASSERT(position.spanPosition() > 0);

    if (!position.namedGridLine().isNull()) {
        // span 2 'c' -> we need to find the appropriate grid line before / after our opposite position.
        return resolveNamedGridLinePositionAgainstOppositePosition(gridContainerStyle, resolvedOppositePosition, position, side);
    }

    return GridSpan::createWithSpanAgainstOpposite(resolvedOppositePosition, position, side);
}

PassOwnPtr<GridSpan> GridResolvedPosition::resolveNamedGridLinePositionAgainstOppositePosition(const ComputedStyle& gridContainerStyle, const GridResolvedPosition& resolvedOppositePosition, const GridPosition& position, GridPositionSide side)
{
    ASSERT(position.isSpan());
    ASSERT(!position.namedGridLine().isNull());
    // Negative positions are not allowed per the specification and should have been handled during parsing.
    ASSERT(position.spanPosition() > 0);

    const NamedGridLinesMap& gridLinesNames = gridLinesForSide(gridContainerStyle, side);
    NamedGridLinesMap::const_iterator it = gridLinesNames.find(position.namedGridLine());

    // If there is no named grid line of that name, we resolve the position to 'auto' (which is equivalent to 'span 1' in this case).
    // See http://lists.w3.org/Archives/Public/www-style/2013Jun/0394.html.
    if (it == gridLinesNames.end())
        return GridSpan::create(resolvedOppositePosition, resolvedOppositePosition);

    return GridSpan::createWithNamedSpanAgainstOpposite(resolvedOppositePosition, position, side, it->value);
}

} // namespace blink
