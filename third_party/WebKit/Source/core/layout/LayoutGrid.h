/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutGrid_h
#define LayoutGrid_h

#include "core/layout/LayoutBlock.h"
#include "core/layout/OrderIterator.h"
#include "core/style/GridResolvedPosition.h"

namespace blink {

struct ContentAlignmentData;
struct GridCoordinate;
struct GridSpan;
class GridTrack;

enum TrackSizeComputationPhase {
    ResolveIntrinsicMinimums,
    ResolveContentBasedMinimums,
    ResolveMaxContentMinimums,
    ResolveIntrinsicMaximums,
    ResolveMaxContentMaximums,
    MaximizeTracks,
};
enum GridAxisPosition {GridAxisStart, GridAxisEnd, GridAxisCenter};

class LayoutGrid final : public LayoutBlock {
public:
    explicit LayoutGrid(Element*);
    virtual ~LayoutGrid();

    virtual const char* name() const override { return "LayoutGrid"; }

    virtual void layoutBlock(bool relayoutChildren) override;

    virtual bool canCollapseAnonymousBlockChild() const override { return false; }

    void dirtyGrid();

    const Vector<LayoutUnit>& columnPositions() const
    {
        ASSERT(!m_gridIsDirty);
        return m_columnPositions;
    }

    const Vector<LayoutUnit>& rowPositions() const
    {
        ASSERT(!m_gridIsDirty);
        return m_rowPositions;
    }

    typedef Vector<LayoutBox*, 1> GridCell;
    const GridCell& gridCell(int row, int column)
    {
        ASSERT_WITH_SECURITY_IMPLICATION(!m_gridIsDirty);
        return m_grid[row][column];
    }

    const Vector<LayoutBox*>& itemsOverflowingGridArea()
    {
        ASSERT_WITH_SECURITY_IMPLICATION(!m_gridIsDirty);
        return m_gridItemsOverflowingGridArea;
    }

    int paintIndexForGridItem(const LayoutBox* layoutBox)
    {
        ASSERT_WITH_SECURITY_IMPLICATION(!m_gridIsDirty);
        return m_gridItemsIndexesMap.get(layoutBox);
    }

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectLayoutGrid || LayoutBlock::isOfType(type); }
    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const override;
    virtual void computePreferredLogicalWidths() override;

    virtual void addChild(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject*) override;

    virtual void styleDidChange(StyleDifference, const ComputedStyle*) override;

    bool explicitGridDidResize(const ComputedStyle&) const;
    bool namedGridLinesDefinitionDidChange(const ComputedStyle&) const;

    class GridIterator;
    struct GridSizingData;
    bool gridElementIsShrinkToFit();
    void computeUsedBreadthOfGridTracks(GridTrackSizingDirection, GridSizingData&, LayoutUnit& availableLogicalSpace);
    LayoutUnit computeUsedBreadthOfMinLength(GridTrackSizingDirection, const GridLength&) const;
    LayoutUnit computeUsedBreadthOfMaxLength(GridTrackSizingDirection, const GridLength&, LayoutUnit usedBreadth) const;
    LayoutUnit computeUsedBreadthOfSpecifiedLength(GridTrackSizingDirection, const Length&) const;
    void resolveContentBasedTrackSizingFunctions(GridTrackSizingDirection, GridSizingData&);

    void ensureGridSize(size_t maximumRowIndex, size_t maximumColumnIndex);
    void insertItemIntoGrid(LayoutBox&, const GridCoordinate&);
    void placeItemsOnGrid();
    void populateExplicitGridAndOrderIterator();
    PassOwnPtr<GridCoordinate> createEmptyGridAreaAtSpecifiedPositionsOutsideGrid(const LayoutBox&, GridTrackSizingDirection, const GridSpan& specifiedPositions) const;
    void placeSpecifiedMajorAxisItemsOnGrid(const Vector<LayoutBox*>&);
    void placeAutoMajorAxisItemsOnGrid(const Vector<LayoutBox*>&);
    void placeAutoMajorAxisItemOnGrid(LayoutBox&, std::pair<size_t, size_t>& autoPlacementCursor);
    GridTrackSizingDirection autoPlacementMajorAxisDirection() const;
    GridTrackSizingDirection autoPlacementMinorAxisDirection() const;

    void layoutGridItems();
    void layoutPositionedObjects(bool relayoutChildren, PositionedLayoutBehavior = DefaultLayout);
    void offsetAndBreadthForPositionedChild(const LayoutBox&, GridTrackSizingDirection, bool startIsAuto, bool endIsAuto, LayoutUnit& offset, LayoutUnit& breadth);
    void populateGridPositions(GridSizingData&, LayoutUnit availableSpaceForColumns, LayoutUnit availableSpaceForRows);

    typedef struct GridItemsSpanGroupRange GridItemsSpanGroupRange;
    LayoutUnit currentItemSizeForTrackSizeComputationPhase(TrackSizeComputationPhase, LayoutBox&, GridTrackSizingDirection, Vector<GridTrack>& columnTracks);
    void resolveContentBasedTrackSizingFunctionsForNonSpanningItems(GridTrackSizingDirection, const GridCoordinate&, LayoutBox& gridItem, GridTrack&, Vector<GridTrack>& columnTracks);
    template <TrackSizeComputationPhase> void resolveContentBasedTrackSizingFunctionsForItems(GridTrackSizingDirection, GridSizingData&, const GridItemsSpanGroupRange&);
    template <TrackSizeComputationPhase> void distributeSpaceToTracks(Vector<GridTrack*>&, const Vector<GridTrack*>* growBeyondGrowthLimitsTracks, GridSizingData&, LayoutUnit& availableLogicalSpace);

    double computeNormalizedFractionBreadth(Vector<GridTrack>&, const GridSpan& tracksSpan, GridTrackSizingDirection, LayoutUnit availableLogicalSpace) const;

    GridTrackSize gridTrackSize(GridTrackSizingDirection, size_t) const;

    LayoutUnit logicalHeightForChild(LayoutBox&, Vector<GridTrack>&);
    LayoutUnit minSizeForChild(LayoutBox&, GridTrackSizingDirection, Vector<GridTrack>& columnTracks);
    LayoutUnit minContentForChild(LayoutBox&, GridTrackSizingDirection, Vector<GridTrack>& columnTracks);
    LayoutUnit maxContentForChild(LayoutBox&, GridTrackSizingDirection, Vector<GridTrack>& columnTracks);
    GridAxisPosition columnAxisPositionForChild(const LayoutBox&) const;
    GridAxisPosition rowAxisPositionForChild(const LayoutBox&) const;
    LayoutUnit rowAxisOffsetForChild(const LayoutBox&) const;
    LayoutUnit columnAxisOffsetForChild(const LayoutBox&) const;
    ContentAlignmentData computeContentPositionAndDistributionOffset(GridTrackSizingDirection, LayoutUnit availableFreeSpace, unsigned numberOfGridTracks) const;
    LayoutPoint findChildLogicalPosition(const LayoutBox&, GridSizingData&) const;
    GridCoordinate cachedGridCoordinate(const LayoutBox&) const;

    LayoutUnit gridAreaBreadthForChild(const LayoutBox& child, GridTrackSizingDirection, const Vector<GridTrack>&) const;
    LayoutUnit gridAreaBreadthForChildIncludingAlignmentOffsets(const LayoutBox&, GridTrackSizingDirection, const GridSizingData&) const;

    void applyStretchAlignmentToTracksIfNeeded(GridTrackSizingDirection, GridSizingData&, LayoutUnit availableSpace);

    virtual void paintChildren(const PaintInfo&, const LayoutPoint&) override;
    bool allowedToStretchLogicalHeightForChild(const LayoutBox& child) const;
    bool needToStretchChildLogicalHeight(const LayoutBox&) const;
    LayoutUnit childIntrinsicHeight(const LayoutBox&) const;
    LayoutUnit childIntrinsicWidth(const LayoutBox&) const;
    LayoutUnit intrinsicLogicalHeightForChild(const LayoutBox&) const;
    LayoutUnit marginLogicalHeightForChild(const LayoutBox&) const;
    LayoutUnit computeMarginLogicalHeightForChild(const LayoutBox&) const;
    LayoutUnit availableAlignmentSpaceForChildBeforeStretching(LayoutUnit gridAreaBreadthForChild, const LayoutBox&) const;
    void applyStretchAlignmentToChildIfNeeded(LayoutBox&, LayoutUnit gridAreaBreadthForChild);

#if ENABLE(ASSERT)
    bool tracksAreWiderThanMinTrackBreadth(GridTrackSizingDirection, const Vector<GridTrack>&);
#endif

    size_t gridItemSpan(const LayoutBox&, GridTrackSizingDirection);
    bool spanningItemCrossesFlexibleSizedTracks(const GridCoordinate&, GridTrackSizingDirection) const;

    size_t gridColumnCount() const
    {
        ASSERT(!m_gridIsDirty);
        return m_grid[0].size();
    }
    size_t gridRowCount() const
    {
        ASSERT(!m_gridIsDirty);
        return m_grid.size();
    }

    bool hasDefiniteLogicalSize(GridTrackSizingDirection) const;

    typedef Vector<Vector<GridCell>> GridRepresentation;
    GridRepresentation m_grid;
    bool m_gridIsDirty;
    Vector<LayoutUnit> m_rowPositions;
    Vector<LayoutUnit> m_columnPositions;
    HashMap<const LayoutBox*, GridCoordinate> m_gridItemCoordinate;
    OrderIterator m_orderIterator;
    Vector<LayoutBox*> m_gridItemsOverflowingGridArea;
    HashMap<const LayoutBox*, size_t> m_gridItemsIndexesMap;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutGrid, isLayoutGrid());

} // namespace blink

#endif // LayoutGrid_h
