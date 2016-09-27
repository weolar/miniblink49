// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/layout/MultiColumnFragmentainerGroup.h"

#include "core/layout/LayoutMultiColumnSet.h"

namespace blink {

MultiColumnFragmentainerGroup::MultiColumnFragmentainerGroup(LayoutMultiColumnSet& columnSet)
    : m_columnSet(columnSet)
{
}

bool MultiColumnFragmentainerGroup::isLastGroup() const
{
    return &m_columnSet.lastFragmentainerGroup() == this;
}

LayoutSize MultiColumnFragmentainerGroup::offsetFromColumnSet() const
{
    LayoutSize offset(LayoutUnit(), logicalTop());
    if (!m_columnSet.flowThread()->isHorizontalWritingMode())
        return offset.transposedSize();
    return offset;
}

bool MultiColumnFragmentainerGroup::heightIsAuto() const
{
    // Only the last row may have auto height, and thus be balanced. There are no good reasons to
    // balance the preceding rows, and that could potentially lead to an insane number of layout
    // passes as well.
    return isLastGroup() && m_columnSet.heightIsAuto();
}

void MultiColumnFragmentainerGroup::resetColumnHeight()
{
    // Nuke previously stored minimum column height. Contents may have changed for all we know.
    m_minimumColumnHeight = 0;

    m_maxColumnHeight = calculateMaxColumnHeight();

    LayoutUnit oldColumnHeight = m_columnHeight;

    if (heightIsAuto())
        m_columnHeight = LayoutUnit();
    else
        setAndConstrainColumnHeight(heightAdjustedForRowOffset(m_columnSet.multiColumnFlowThread()->columnHeightAvailable()));

    if (m_columnHeight != oldColumnHeight)
        m_columnSet.setChildNeedsLayout(MarkOnlyThis);

    // Content runs are only needed in the initial layout pass, in order to find an initial column
    // height, and should have been deleted afterwards. We're about to rebuild the content runs, so
    // the list needs to be empty.
    ASSERT(m_contentRuns.isEmpty());
}

void MultiColumnFragmentainerGroup::addContentRun(LayoutUnit endOffsetInFlowThread)
{
    if (!m_contentRuns.isEmpty() && endOffsetInFlowThread <= m_contentRuns.last().breakOffset())
        return;
    // Append another item as long as we haven't exceeded used column count. What ends up in the
    // overflow area shouldn't affect column balancing.
    if (m_contentRuns.size() < m_columnSet.usedColumnCount())
        m_contentRuns.append(ContentRun(endOffsetInFlowThread));
}

void MultiColumnFragmentainerGroup::recordSpaceShortage(LayoutUnit spaceShortage)
{
    if (spaceShortage >= m_minSpaceShortage)
        return;

    // The space shortage is what we use as our stretch amount. We need a positive number here in
    // order to get anywhere.
    ASSERT(spaceShortage > 0);

    m_minSpaceShortage = spaceShortage;
}

bool MultiColumnFragmentainerGroup::recalculateColumnHeight(BalancedColumnHeightCalculation calculationMode)
{
    LayoutUnit oldColumnHeight = m_columnHeight;

    m_maxColumnHeight = calculateMaxColumnHeight();

    if (heightIsAuto()) {
        if (calculationMode == GuessFromFlowThreadPortion) {
            // Post-process the content runs and find out where the implicit breaks will occur.
            distributeImplicitBreaks();
        }
        LayoutUnit newColumnHeight = calculateColumnHeight(calculationMode);
        setAndConstrainColumnHeight(newColumnHeight);
        // After having calculated an initial column height, the multicol container typically needs at
        // least one more layout pass with a new column height, but if a height was specified, we only
        // need to do this if we think that we need less space than specified. Conversely, if we
        // determined that the columns need to be as tall as the specified height of the container, we
        // have already laid it out correctly, and there's no need for another pass.
    } else {
        // The position of the column set may have changed, in which case height available for
        // columns may have changed as well.
        setAndConstrainColumnHeight(m_columnHeight);
    }

    // We can get rid of the content runs now, if we haven't already done so. They are only needed
    // to calculate the initial balanced column height. In fact, we have to get rid of them before
    // the next layout pass, since each pass will rebuild this.
    m_contentRuns.clear();

    if (m_columnHeight == oldColumnHeight)
        return false; // No change. We're done.

    m_minSpaceShortage = LayoutUnit::max();
    return true; // Need another pass.
}

void MultiColumnFragmentainerGroup::expandToEncompassFlowThreadOverflow()
{
    ASSERT(isLastGroup());
    // Get the offset within the flow thread in its block progression direction. Then get the
    // flow thread's remaining logical height including its overflow and expand our rect
    // to encompass that remaining height and overflow. The idea is that we will generate
    // additional columns and pages to hold that overflow, since people do write bad
    // content like <body style="height:0px"> in multi-column layouts.
    LayoutMultiColumnFlowThread* flowThread = m_columnSet.multiColumnFlowThread();
    LayoutRect layoutRect = flowThread->layoutOverflowRect();
    m_logicalBottomInFlowThread = flowThread->isHorizontalWritingMode() ? layoutRect.maxY() : layoutRect.maxX();
}

LayoutSize MultiColumnFragmentainerGroup::flowThreadTranslationAtOffset(LayoutUnit offsetInFlowThread) const
{
    LayoutFlowThread* flowThread = m_columnSet.flowThread();
    unsigned columnIndex = columnIndexAtOffset(offsetInFlowThread);
    LayoutRect portionRect(flowThreadPortionRectAt(columnIndex));
    flowThread->flipForWritingMode(portionRect);
    LayoutRect columnRect(columnRectAt(columnIndex));
    m_columnSet.flipForWritingMode(columnRect);
    LayoutSize translationRelativeToGroup = columnRect.location() - portionRect.location();
    return translationRelativeToGroup + offsetFromColumnSet() + m_columnSet.topLeftLocationOffset() - flowThread->topLeftLocationOffset();
}

LayoutUnit MultiColumnFragmentainerGroup::columnLogicalTopForOffset(LayoutUnit offsetInFlowThread) const
{
    unsigned columnIndex = columnIndexAtOffset(offsetInFlowThread, AssumeNewColumns);
    return logicalTopInFlowThreadAt(columnIndex);
}

LayoutPoint MultiColumnFragmentainerGroup::visualPointToFlowThreadPoint(const LayoutPoint& visualPoint) const
{
    unsigned columnIndex = columnIndexAtVisualPoint(visualPoint);
    LayoutRect columnRect = columnRectAt(columnIndex);
    LayoutPoint localPoint(visualPoint);
    localPoint.moveBy(-columnRect.location());
    // Before converting to a flow thread position, if the block direction coordinate is outside the
    // column, snap to the bounds of the column, and reset the inline direction coordinate to the
    // start position in the column. The effect of this is that if the block position is before the
    // column rectangle, we'll get to the beginning of this column, while if the block position is
    // after the column rectangle, we'll get to the beginning of the next column.
    if (!m_columnSet.isHorizontalWritingMode()) {
        LayoutUnit columnStart = m_columnSet.style()->isLeftToRightDirection() ? LayoutUnit() : columnRect.height();
        if (localPoint.x() < 0)
            localPoint = LayoutPoint(LayoutUnit(), columnStart);
        else if (localPoint.x() > logicalHeight())
            localPoint = LayoutPoint(logicalHeight(), columnStart);
        return LayoutPoint(localPoint.x() + logicalTopInFlowThreadAt(columnIndex), localPoint.y());
    }
    LayoutUnit columnStart = m_columnSet.style()->isLeftToRightDirection() ? LayoutUnit() : columnRect.width();
    if (localPoint.y() < 0)
        localPoint = LayoutPoint(columnStart, LayoutUnit());
    else if (localPoint.y() > logicalHeight())
        localPoint = LayoutPoint(columnStart, logicalHeight());
    return LayoutPoint(localPoint.x(), localPoint.y() + logicalTopInFlowThreadAt(columnIndex));
}

void MultiColumnFragmentainerGroup::collectLayerFragments(DeprecatedPaintLayerFragments& fragments, const LayoutRect& layerBoundingBox, const LayoutRect& dirtyRect) const
{
    // |layerBoundingBox| is in the flow thread coordinate space, relative to the top/left edge of
    // the flow thread, but note that it has been converted with respect to writing mode (so that
    // it's visual/physical in that sense).
    //
    // |dirtyRect| is visual, relative to the multicol container.
    //
    // Then there's the output from this method - the stuff we put into the list of fragments. The
    // fragment.paginationOffset point is the actual visual translation required to get from a
    // location in the flow thread to a location in a given column. The fragment.paginationClip
    // rectangle, on the other hand, is in flow thread coordinates, but otherwise completely
    // physical in terms of writing mode.
    //
    // All other rectangles in this method are sized physically, and the inline direction coordinate
    // is physical too, but the block direction coordinate is "logical top". This is the same as
    // e.g. LayoutBox::frameRect(). These rectangles also pretend that there's only one long column,
    // i.e. they are for the flow thread.

    LayoutMultiColumnFlowThread* flowThread = m_columnSet.multiColumnFlowThread();
    bool isHorizontalWritingMode = m_columnSet.isHorizontalWritingMode();

    // Put the layer bounds into flow thread-local coordinates by flipping it first. Since we're in
    // a layoutObject, most rectangles are represented this way.
    LayoutRect layerBoundsInFlowThread(layerBoundingBox);
    flowThread->flipForWritingMode(layerBoundsInFlowThread);

    // Now we can compare with the flow thread portions owned by each column. First let's
    // see if the rect intersects our flow thread portion at all.
    LayoutRect clippedRect(layerBoundsInFlowThread);
    clippedRect.intersect(m_columnSet.flowThreadPortionOverflowRect());
    if (clippedRect.isEmpty())
        return;

    // Now we know we intersect at least one column. Let's figure out the logical top and logical
    // bottom of the area we're checking.
    LayoutUnit layerLogicalTop = isHorizontalWritingMode ? layerBoundsInFlowThread.y() : layerBoundsInFlowThread.x();
    LayoutUnit layerLogicalBottom = (isHorizontalWritingMode ? layerBoundsInFlowThread.maxY() : layerBoundsInFlowThread.maxX()) - 1;

    // Figure out the start and end columns and only check within that range so that we don't walk the
    // entire column row.
    unsigned startColumn = columnIndexAtOffset(layerLogicalTop);
    unsigned endColumn = columnIndexAtOffset(layerLogicalBottom);

    LayoutUnit colLogicalWidth = m_columnSet.pageLogicalWidth();
    LayoutUnit colGap = m_columnSet.columnGap();
    unsigned colCount = actualColumnCount();

    bool progressionIsInline = flowThread->progressionIsInline();
    bool leftToRight = m_columnSet.style()->isLeftToRightDirection();

    LayoutUnit initialBlockOffset = m_columnSet.logicalTop() + logicalTop() - flowThread->logicalTop();

    for (unsigned i = startColumn; i <= endColumn; i++) {
        // Get the portion of the flow thread that corresponds to this column.
        LayoutRect flowThreadPortion = flowThreadPortionRectAt(i);

        // Now get the overflow rect that corresponds to the column.
        LayoutRect flowThreadOverflowPortion = flowThreadPortionOverflowRect(flowThreadPortion, i, colCount, colGap);

        // In order to create a fragment we must intersect the portion painted by this column.
        LayoutRect clippedRect(layerBoundsInFlowThread);
        clippedRect.intersect(flowThreadOverflowPortion);
        if (clippedRect.isEmpty())
            continue;

        // We also need to intersect the dirty rect. We have to apply a translation and shift based off
        // our column index.
        LayoutPoint translationOffset;
        LayoutUnit inlineOffset = progressionIsInline ? i * (colLogicalWidth + colGap) : LayoutUnit();
        if (!leftToRight)
            inlineOffset = -inlineOffset;
        translationOffset.setX(inlineOffset);
        LayoutUnit blockOffset;
        if (progressionIsInline) {
            blockOffset = initialBlockOffset + (isHorizontalWritingMode ? -flowThreadPortion.y() : -flowThreadPortion.x());
        } else {
            // Column gap can apply in the block direction for page fragmentainers.
            // There is currently no spec which calls for column-gap to apply
            // for page fragmentainers at all, but it's applied here for compatibility
            // with the old multicolumn implementation.
            blockOffset = i * colGap;
        }
        if (isFlippedBlocksWritingMode(m_columnSet.style()->writingMode()))
            blockOffset = -blockOffset;
        translationOffset.setY(blockOffset);
        if (!isHorizontalWritingMode)
            translationOffset = translationOffset.transposedPoint();

        // Shift the dirty rect to be in flow thread coordinates with this translation applied.
        LayoutRect translatedDirtyRect(dirtyRect);
        translatedDirtyRect.moveBy(-translationOffset);

        // See if we intersect the dirty rect.
        clippedRect = layerBoundingBox;
        clippedRect.intersect(translatedDirtyRect);
        if (clippedRect.isEmpty())
            continue;

        // Something does need to paint in this column. Make a fragment now and supply the physical translation
        // offset and the clip rect for the column with that offset applied.
        DeprecatedPaintLayerFragment fragment;
        fragment.paginationOffset = translationOffset;

        LayoutRect flippedFlowThreadOverflowPortion(flowThreadOverflowPortion);
        // Flip it into more a physical (DeprecatedPaintLayer-style) rectangle.
        flowThread->flipForWritingMode(flippedFlowThreadOverflowPortion);
        fragment.paginationClip = flippedFlowThreadOverflowPortion;
        fragments.append(fragment);
    }
}

LayoutRect MultiColumnFragmentainerGroup::calculateOverflow() const
{
    unsigned columnCount = actualColumnCount();
    if (!columnCount)
        return LayoutRect();
    return columnRectAt(columnCount - 1);
}

unsigned MultiColumnFragmentainerGroup::actualColumnCount() const
{
    // We must always return a value of 1 or greater. Column count = 0 is a meaningless situation,
    // and will confuse and cause problems in other parts of the code.
    if (!m_columnHeight)
        return 1;

    // Our flow thread portion determines our column count. We have as many columns as needed to fit
    // all the content.
    LayoutUnit flowThreadPortionHeight = logicalHeightInFlowThread();
    if (!flowThreadPortionHeight)
        return 1;

    unsigned count = ceil(flowThreadPortionHeight.toFloat() / m_columnHeight.toFloat());
    ASSERT(count >= 1);
    return count;
}

LayoutUnit MultiColumnFragmentainerGroup::heightAdjustedForRowOffset(LayoutUnit height) const
{
    // Adjust for the top offset within the content box of the multicol container (containing
    // block), unless we're in the first set. We know that the top offset for the first set will be
    // zero, but if the multicol container has non-zero top border or padding, the set's top offset
    // (initially being 0 and relative to the border box) will be negative until it has been laid
    // out. Had we used this bogus offset, we would calculate the wrong height, and risk performing
    // a wasted layout iteration. Of course all other sets (if any) have this problem in the first
    // layout pass too, but there's really nothing we can do there until the flow thread has been
    // laid out anyway.
    if (m_columnSet.previousSiblingMultiColumnSet()) {
        LayoutBlockFlow* multicolBlock = m_columnSet.multiColumnBlockFlow();
        LayoutUnit contentLogicalTop = m_columnSet.logicalTop() - multicolBlock->borderAndPaddingBefore();
        height -= contentLogicalTop;
    }
    height -= logicalTop();
    return max(height, LayoutUnit(1)); // Let's avoid zero height, as that would probably cause an infinite amount of columns to be created.
}

LayoutUnit MultiColumnFragmentainerGroup::calculateMaxColumnHeight() const
{
    LayoutBlockFlow* multicolBlock = m_columnSet.multiColumnBlockFlow();
    const ComputedStyle& multicolStyle = multicolBlock->styleRef();
    LayoutUnit availableHeight = m_columnSet.multiColumnFlowThread()->columnHeightAvailable();
    LayoutUnit maxColumnHeight = availableHeight ? availableHeight : LayoutUnit::max();
    if (!multicolStyle.logicalMaxHeight().isMaxSizeNone()) {
        LayoutUnit logicalMaxHeight = multicolBlock->computeContentLogicalHeight(MaxSize, multicolStyle.logicalMaxHeight(), -1);
        if (logicalMaxHeight != -1 && maxColumnHeight > logicalMaxHeight)
            maxColumnHeight = logicalMaxHeight;
    }
    return heightAdjustedForRowOffset(maxColumnHeight);
}

void MultiColumnFragmentainerGroup::setAndConstrainColumnHeight(LayoutUnit newHeight)
{
    m_columnHeight = newHeight;
    if (m_columnHeight > m_maxColumnHeight)
        m_columnHeight = m_maxColumnHeight;
    // FIXME: the height may also be affected by the enclosing pagination context, if any.
}

unsigned MultiColumnFragmentainerGroup::findRunWithTallestColumns() const
{
    unsigned indexWithLargestHeight = 0;
    LayoutUnit largestHeight;
    LayoutUnit previousOffset = m_logicalTopInFlowThread;
    size_t runCount = m_contentRuns.size();
    ASSERT(runCount);
    for (size_t i = 0; i < runCount; i++) {
        const ContentRun& run = m_contentRuns[i];
        LayoutUnit height = run.columnLogicalHeight(previousOffset);
        if (largestHeight < height) {
            largestHeight = height;
            indexWithLargestHeight = i;
        }
        previousOffset = run.breakOffset();
    }
    return indexWithLargestHeight;
}

void MultiColumnFragmentainerGroup::distributeImplicitBreaks()
{
#if ENABLE(ASSERT)
    // There should be no implicit breaks assumed at this point.
    for (unsigned i = 0; i < m_contentRuns.size(); i++)
        ASSERT(!m_contentRuns[i].assumedImplicitBreaks());
#endif // ENABLE(ASSERT)

    // Insert a final content run to encompass all content. This will include overflow if this is
    // the last set.
    addContentRun(m_logicalBottomInFlowThread);
    unsigned columnCount = m_contentRuns.size();

    // If there is room for more breaks (to reach the used value of column-count), imagine that we
    // insert implicit breaks at suitable locations. At any given time, the content run with the
    // currently tallest columns will get another implicit break "inserted", which will increase its
    // column count by one and shrink its columns' height. Repeat until we have the desired total
    // number of breaks. The largest column height among the runs will then be the initial column
    // height for the balancer to use.
    while (columnCount < m_columnSet.usedColumnCount()) {
        unsigned index = findRunWithTallestColumns();
        m_contentRuns[index].assumeAnotherImplicitBreak();
        columnCount++;
    }
}

LayoutUnit MultiColumnFragmentainerGroup::calculateColumnHeight(BalancedColumnHeightCalculation calculationMode) const
{
    if (calculationMode == GuessFromFlowThreadPortion) {
        // Initial balancing. Start with the lowest imaginable column height. We use the tallest
        // content run (after having "inserted" implicit breaks), and find its start offset (by
        // looking at the previous run's end offset, or, if there's no previous run, the set's start
        // offset in the flow thread).
        unsigned index = findRunWithTallestColumns();
        LayoutUnit startOffset = index > 0 ? m_contentRuns[index - 1].breakOffset() : m_logicalTopInFlowThread;
        return std::max<LayoutUnit>(m_contentRuns[index].columnLogicalHeight(startOffset), m_minimumColumnHeight);
    }

    if (actualColumnCount() <= m_columnSet.usedColumnCount()) {
        // With the current column height, the content fits without creating overflowing columns. We're done.
        return m_columnHeight;
    }

    if (m_contentRuns.size() >= m_columnSet.usedColumnCount()) {
        // Too many forced breaks to allow any implicit breaks. Initial balancing should already
        // have set a good height. There's nothing more we should do.
        return m_columnHeight;
    }

    if (m_columnHeight >= m_maxColumnHeight) {
        // We cannot stretch any further. We'll just have to live with the overflowing columns. This
        // typically happens if the max column height is less than the height of the tallest piece
        // of unbreakable content (e.g. lines).
        return m_columnHeight;
    }

    // If the initial guessed column height wasn't enough, stretch it now. Stretch by the lowest
    // amount of space shortage found during layout.

    ASSERT(m_minSpaceShortage > 0); // We should never _shrink_ the height!
    ASSERT(m_minSpaceShortage != LayoutUnit::max()); // If this happens, we probably have a bug.
    if (m_minSpaceShortage == LayoutUnit::max())
        return m_columnHeight; // So bail out rather than looping infinitely.

    return m_columnHeight + m_minSpaceShortage;
}

LayoutRect MultiColumnFragmentainerGroup::columnRectAt(unsigned columnIndex) const
{
    LayoutUnit columnLogicalWidth = m_columnSet.pageLogicalWidth();
    LayoutUnit columnLogicalHeight = m_columnHeight;
    LayoutUnit columnLogicalTop;
    LayoutUnit columnLogicalLeft;
    LayoutUnit columnGap = m_columnSet.columnGap();
    LayoutUnit portionOutsideFlowThread = logicalTopInFlowThread() + (columnIndex + 1) * columnLogicalHeight - logicalBottomInFlowThread();
    if (portionOutsideFlowThread > 0) {
        // The last column may not be using all available space.
        ASSERT(columnIndex + 1 == actualColumnCount());
        columnLogicalHeight -= portionOutsideFlowThread;
        ASSERT(columnLogicalHeight >= 0);
    }

    if (m_columnSet.multiColumnFlowThread()->progressionIsInline()) {
        if (m_columnSet.style()->isLeftToRightDirection())
            columnLogicalLeft += columnIndex * (columnLogicalWidth + columnGap);
        else
            columnLogicalLeft += m_columnSet.contentLogicalWidth() - columnLogicalWidth - columnIndex * (columnLogicalWidth + columnGap);
    } else {
        columnLogicalTop += columnIndex * (m_columnHeight + columnGap);
    }

    LayoutRect columnRect(columnLogicalLeft, columnLogicalTop, columnLogicalWidth, columnLogicalHeight);
    if (!m_columnSet.isHorizontalWritingMode())
        return columnRect.transposedRect();
    return columnRect;
}

LayoutRect MultiColumnFragmentainerGroup::flowThreadPortionRectAt(unsigned columnIndex) const
{
    LayoutUnit logicalTop = logicalTopInFlowThreadAt(columnIndex);
    LayoutUnit logicalBottom = logicalTop + m_columnHeight;
    if (logicalBottom > logicalBottomInFlowThread()) {
        // The last column may not be using all available space.
        ASSERT(columnIndex + 1 == actualColumnCount());
        logicalBottom = logicalBottomInFlowThread();
        ASSERT(logicalBottom >= logicalTop);
    }
    LayoutUnit portionLogicalHeight = logicalBottom - logicalTop;
    if (m_columnSet.isHorizontalWritingMode())
        return LayoutRect(LayoutUnit(), logicalTop, m_columnSet.pageLogicalWidth(), portionLogicalHeight);
    return LayoutRect(logicalTop, LayoutUnit(), portionLogicalHeight, m_columnSet.pageLogicalWidth());
}

LayoutRect MultiColumnFragmentainerGroup::flowThreadPortionOverflowRect(const LayoutRect& portionRect, unsigned columnIndex, unsigned columnCount, LayoutUnit columnGap) const
{
    // This function determines the portion of the flow thread that paints for the column. Along the inline axis, columns are
    // unclipped at outside edges (i.e., the first and last column in the set), and they clip to half the column
    // gap along interior edges.
    //
    // In the block direction, we will not clip overflow out of the top of the first column, or out of the bottom of
    // the last column. This applies only to the true first column and last column across all column sets.
    //
    // FIXME: Eventually we will know overflow on a per-column basis, but we can't do this until we have a painting
    // mode that understands not to paint contents from a previous column in the overflow area of a following column.
    bool isFirstColumn = !columnIndex;
    bool isLastColumn = columnIndex == columnCount - 1;
    bool isLTR = m_columnSet.style()->isLeftToRightDirection();
    bool isLeftmostColumn = isLTR ? isFirstColumn : isLastColumn;
    bool isRightmostColumn = isLTR ? isLastColumn : isFirstColumn;

    // Calculate the overflow rectangle, based on the flow thread's, clipped at column logical
    // top/bottom unless it's the first/last column.
    LayoutRect overflowRect = m_columnSet.overflowRectForFlowThreadPortion(portionRect, isFirstColumn && !m_columnSet.previousSiblingMultiColumnSet(), isLastColumn && !m_columnSet.nextSiblingMultiColumnSet());

    // Avoid overflowing into neighboring columns, by clipping in the middle of adjacent column
    // gaps. Also make sure that we avoid rounding errors.
    if (m_columnSet.isHorizontalWritingMode()) {
        if (!isLeftmostColumn)
            overflowRect.shiftXEdgeTo(portionRect.x() - columnGap / 2);
        if (!isRightmostColumn)
            overflowRect.shiftMaxXEdgeTo(portionRect.maxX() + columnGap - columnGap / 2);
    } else {
        if (!isLeftmostColumn)
            overflowRect.shiftYEdgeTo(portionRect.y() - columnGap / 2);
        if (!isRightmostColumn)
            overflowRect.shiftMaxYEdgeTo(portionRect.maxY() + columnGap - columnGap / 2);
    }
    return overflowRect;
}

unsigned MultiColumnFragmentainerGroup::columnIndexAtOffset(LayoutUnit offsetInFlowThread, ColumnIndexCalculationMode mode) const
{
    // Handle the offset being out of range.
    if (offsetInFlowThread < m_logicalTopInFlowThread)
        return 0;
    // If we're laying out right now, we cannot constrain against some logical bottom, since it
    // isn't known yet. Otherwise, just return the last column if we're past the logical bottom.
    if (mode == ClampToExistingColumns) {
        if (offsetInFlowThread >= m_logicalBottomInFlowThread)
            return actualColumnCount() - 1;
    }

    if (m_columnHeight)
        return ((offsetInFlowThread - m_logicalTopInFlowThread) / m_columnHeight).floor();
    return 0;
}

unsigned MultiColumnFragmentainerGroup::columnIndexAtVisualPoint(const LayoutPoint& visualPoint) const
{
    bool isColumnProgressionInline = m_columnSet.multiColumnFlowThread()->progressionIsInline();
    bool isHorizontalWritingMode = m_columnSet.isHorizontalWritingMode();
    LayoutUnit columnLengthInColumnProgressionDirection = isColumnProgressionInline ? m_columnSet.pageLogicalWidth() : m_columnSet.pageLogicalHeight();
    LayoutUnit offsetInColumnProgressionDirection = isHorizontalWritingMode == isColumnProgressionInline ? visualPoint.x() : visualPoint.y();
    if (!m_columnSet.style()->isLeftToRightDirection() && isColumnProgressionInline)
        offsetInColumnProgressionDirection = m_columnSet.logicalWidth() - offsetInColumnProgressionDirection;
    LayoutUnit columnGap = m_columnSet.columnGap();
    if (columnLengthInColumnProgressionDirection + columnGap <= 0)
        return 0;
    // Column boundaries are in the middle of the column gap.
    int index = (offsetInColumnProgressionDirection + columnGap / 2) / (columnLengthInColumnProgressionDirection + columnGap);
    if (index < 0)
        return 0;
    return std::min(unsigned(index), actualColumnCount() - 1);
}

MultiColumnFragmentainerGroupList::MultiColumnFragmentainerGroupList(LayoutMultiColumnSet& columnSet)
    : m_columnSet(columnSet)
{
    append(MultiColumnFragmentainerGroup(m_columnSet));
}

// An explicit empty destructor of MultiColumnFragmentainerGroupList should be in
// MultiColumnFragmentainerGroup.cpp, because if an implicit destructor is used,
// msvc 2015 tries to generate its destructor (because the class is dll-exported class)
// and causes a compile error because of lack of MultiColumnFragmentainerGroup::operator=.
// Since MultiColumnFragmentainerGroup is non-copyable, we cannot define the operator=.
MultiColumnFragmentainerGroupList::~MultiColumnFragmentainerGroupList()
{
}

MultiColumnFragmentainerGroup& MultiColumnFragmentainerGroupList::addExtraGroup()
{
    append(MultiColumnFragmentainerGroup(m_columnSet));
    return last();
}

void MultiColumnFragmentainerGroupList::deleteExtraGroups()
{
    shrink(1);
}

} // namespace blink
