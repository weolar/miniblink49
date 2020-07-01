/*
 * Copyright (C) 2011 Adobe Systems Incorporated. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"

#include "core/layout/LayoutFlowThread.h"

#include "core/layout/LayoutMultiColumnSet.h"
#include "core/layout/LayoutView.h"

namespace blink {

LayoutFlowThread::LayoutFlowThread()
    : LayoutBlockFlow(nullptr)
    , m_columnSetsInvalidated(false)
    , m_pageLogicalSizeChanged(false)
{
}

void LayoutFlowThread::removeColumnSetFromThread(LayoutMultiColumnSet* columnSet)
{
    ASSERT(columnSet);
    m_multiColumnSetList.remove(columnSet);
    invalidateColumnSets();
    // Clear the interval tree right away, instead of leaving it around with dead objects. Not that
    // anyone _should_ try to access the interval tree when the column sets are marked as invalid,
    // but this is actually possible if other parts of the engine has bugs that cause us to not lay
    // out everything that was marked for layout, so that LayoutObject::assertLaidOut() (and a LOT
    // of other assertions) fails.
    m_multiColumnSetIntervalTree.clear();
}

void LayoutFlowThread::invalidateColumnSets()
{
    if (m_columnSetsInvalidated) {
        ASSERT(selfNeedsLayout());
        return;
    }

    setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::ColumnsChanged);

    m_columnSetsInvalidated = true;
}

void LayoutFlowThread::validateColumnSets()
{
    m_columnSetsInvalidated = false;
    updateLogicalWidth(); // Called to get the maximum logical width for the columnSet.
    generateColumnSetIntervalTree();
}

void LayoutFlowThread::mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, const PaintInvalidationState* paintInvalidationState) const
{
    ASSERT(paintInvalidationContainer != this); // A flow thread should never be an invalidation container.
    // |rect| is a layout rectangle, where the block direction coordinate is flipped for writing
    // mode. fragmentsBoundingBox(), on the other hand, works on physical rectangles, so we need to
    // flip the rectangle before and after calling it.
    flipForWritingMode(rect);
    rect = fragmentsBoundingBox(rect);
    flipForWritingMode(rect);
    LayoutBlockFlow::mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
}

void LayoutFlowThread::layout()
{
    m_pageLogicalSizeChanged = m_columnSetsInvalidated && everHadLayout();
    LayoutBlockFlow::layout();
    m_pageLogicalSizeChanged = false;
}

void LayoutFlowThread::computeLogicalHeight(LayoutUnit, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    computedValues.m_position = logicalTop;
    computedValues.m_extent = 0;

    for (LayoutMultiColumnSetList::const_iterator iter = m_multiColumnSetList.begin(); iter != m_multiColumnSetList.end(); ++iter) {
        LayoutMultiColumnSet* columnSet = *iter;
        computedValues.m_extent += columnSet->logicalHeightInFlowThread();
    }
}

bool LayoutFlowThread::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction)
{
    if (hitTestAction == HitTestBlockBackground)
        return false;
    return LayoutBlockFlow::nodeAtPoint(result, locationInContainer, accumulatedOffset, hitTestAction);
}

LayoutUnit LayoutFlowThread::pageLogicalHeightForOffset(LayoutUnit offset)
{
    LayoutMultiColumnSet* columnSet = columnSetAtBlockOffset(offset);
    if (!columnSet)
        return 0;

    return columnSet->pageLogicalHeight();
}

LayoutUnit LayoutFlowThread::pageRemainingLogicalHeightForOffset(LayoutUnit offset, PageBoundaryRule pageBoundaryRule)
{
    LayoutMultiColumnSet* columnSet = columnSetAtBlockOffset(offset);
    if (!columnSet)
        return 0;

    LayoutUnit pageLogicalTop = columnSet->pageLogicalTopForOffset(offset);
    LayoutUnit pageLogicalHeight = columnSet->pageLogicalHeight();
    LayoutUnit pageLogicalBottom = pageLogicalTop + pageLogicalHeight;
    LayoutUnit remainingHeight = pageLogicalBottom - offset;
    if (pageBoundaryRule == IncludePageBoundary) {
        // If IncludePageBoundary is set, the line exactly on the top edge of a
        // columnSet will act as being part of the previous columnSet.
        remainingHeight = intMod(remainingHeight, pageLogicalHeight);
    }
    return remainingHeight;
}

void LayoutFlowThread::generateColumnSetIntervalTree()
{
    // FIXME: Optimize not to clear the interval all the time. This implies manually managing the tree nodes lifecycle.
    m_multiColumnSetIntervalTree.clear();
    m_multiColumnSetIntervalTree.initIfNeeded();
    for (auto columnSet : m_multiColumnSetList)
        m_multiColumnSetIntervalTree.add(MultiColumnSetIntervalTree::createInterval(columnSet->logicalTopInFlowThread(), columnSet->logicalBottomInFlowThread(), columnSet));
}

void LayoutFlowThread::collectLayerFragments(DeprecatedPaintLayerFragments& layerFragments, const LayoutRect& layerBoundingBox, const LayoutRect& dirtyRect)
{
    ASSERT(!m_columnSetsInvalidated);

    for (LayoutMultiColumnSetList::const_iterator iter = m_multiColumnSetList.begin(); iter != m_multiColumnSetList.end(); ++iter) {
        LayoutMultiColumnSet* columnSet = *iter;
        columnSet->collectLayerFragments(layerFragments, layerBoundingBox, dirtyRect);
    }
}

LayoutRect LayoutFlowThread::fragmentsBoundingBox(const LayoutRect& layerBoundingBox) const
{
    ASSERT(!m_columnSetsInvalidated);

    LayoutRect result;
    for (auto* columnSet : m_multiColumnSetList) {
        DeprecatedPaintLayerFragments fragments;
        columnSet->collectLayerFragments(fragments, layerBoundingBox, LayoutRect(LayoutRect::infiniteIntRect()));
        for (const auto& fragment : fragments) {
            LayoutRect fragmentRect(layerBoundingBox);
            fragmentRect.intersect(fragment.paginationClip);
            fragmentRect.moveBy(fragment.paginationOffset);
            result.unite(fragmentRect);
        }
    }

    return result;
}

void LayoutFlowThread::MultiColumnSetSearchAdapter::collectIfNeeded(const MultiColumnSetInterval& interval)
{
    if (m_result)
        return;
    if (interval.low() <= m_offset && interval.high() > m_offset)
        m_result = interval.data();
}

void LayoutFlowThread::flowThreadToContainingCoordinateSpace(LayoutUnit& blockPosition, LayoutUnit& inlinePosition) const
{
    LayoutPoint position(inlinePosition, blockPosition);
    // First we have to make |position| physical, because that's what offsetLeft()
    // expects and returns.
    if (!isHorizontalWritingMode())
        position = position.transposedPoint();
    position = flipForWritingMode(position);

    position.move(columnOffset(position));

    // Make |position| logical again, and read out the values.
    position = flipForWritingMode(position);
    if (!isHorizontalWritingMode())
        position = position.transposedPoint();
    blockPosition = position.y();
    inlinePosition = position.x();
}


} // namespace blink
