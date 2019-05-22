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

#ifndef LayoutFlowThread_h
#define LayoutFlowThread_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/paint/DeprecatedPaintLayerFragment.h"
#include "wtf/ListHashSet.h"

namespace blink {

class LayoutMultiColumnSet;

typedef ListHashSet<LayoutMultiColumnSet*> LayoutMultiColumnSetList;

// LayoutFlowThread is used to collect all the layout objects that participate in a flow thread. It
// will also help in doing the layout. However, it will not layout directly to screen. Instead,
// LayoutMultiColumnSet objects will redirect their paint and nodeAtPoint methods to this
// object. Each LayoutMultiColumnSet will actually be a viewPort of the LayoutFlowThread.
class CORE_EXPORT LayoutFlowThread: public LayoutBlockFlow {
public:
    LayoutFlowThread();
    virtual ~LayoutFlowThread() { }

    virtual bool isLayoutFlowThread() const override final { return true; }
    virtual bool isLayoutMultiColumnFlowThread() const { return false; }
    virtual bool isLayoutPagedFlowThread() const { return false; }

    virtual bool supportsPaintInvalidationStateCachedOffsets() const override { return false; }

    virtual void layout() override;

    // Always create a Layer for the LayoutFlowThread so that we
    // can easily avoid drawing the children directly.
    virtual DeprecatedPaintLayerType layerTypeRequired() const override final { return NormalDeprecatedPaintLayer; }

    // Skip past a column spanner during flow thread layout. Spanners are not laid out inside the
    // flow thread, since the flow thread is not in a spanner's containing block chain (since the
    // containing block is the multicol container).
    virtual void skipColumnSpanner(LayoutBox*, LayoutUnit logicalTopInFlowThread) { }

    virtual void flowThreadDescendantWasInserted(LayoutObject*) { }
    virtual void flowThreadDescendantWillBeRemoved(LayoutObject*) { }
    virtual void flowThreadDescendantStyleWillChange(LayoutObject*, StyleDifference, const ComputedStyle& newStyle) { }
    virtual void flowThreadDescendantStyleDidChange(LayoutObject*, StyleDifference, const ComputedStyle& oldStyle) { }

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override final;

    virtual void addColumnSetToThread(LayoutMultiColumnSet*) = 0;
    virtual void removeColumnSetFromThread(LayoutMultiColumnSet*);

    virtual void computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues&) const override;

    bool hasColumnSets() const { return m_multiColumnSetList.size(); }

    void validateColumnSets();
    void invalidateColumnSets();
    bool hasValidColumnSetInfo() const { return !m_columnSetsInvalidated && !m_multiColumnSetList.isEmpty(); }

    virtual void mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&, const PaintInvalidationState*) const override;

    LayoutUnit pageLogicalHeightForOffset(LayoutUnit);
    LayoutUnit pageRemainingLogicalHeightForOffset(LayoutUnit, PageBoundaryRule = IncludePageBoundary);

    virtual void setPageBreak(LayoutUnit /*offset*/, LayoutUnit /*spaceShortage*/) { }
    virtual void updateMinimumPageHeight(LayoutUnit /*offset*/, LayoutUnit /*minHeight*/) { }

    virtual bool addForcedColumnBreak(LayoutUnit, LayoutObject* breakChild, bool isBefore, LayoutUnit* offsetBreakAdjustment = nullptr) { return false; }

    virtual bool isPageLogicalHeightKnown() const { return true; }
    bool pageLogicalSizeChanged() const { return m_pageLogicalSizeChanged; }

    void collectLayerFragments(DeprecatedPaintLayerFragments&, const LayoutRect& layerBoundingBox, const LayoutRect& dirtyRect);

    // Return the visual bounding box based on the supplied flow-thread bounding box. Both
    // rectangles are completely physical in terms of writing mode.
    LayoutRect fragmentsBoundingBox(const LayoutRect& layerBoundingBox) const;

    // Convert a logical position in the flow thread coordinate space to a logical
    // position in the containing coordinate space.
    void flowThreadToContainingCoordinateSpace(LayoutUnit& blockPosition, LayoutUnit& inlinePosition) const;

    LayoutPoint flowThreadPointToVisualPoint(const LayoutPoint& flowThreadPoint) const
    {
        return flowThreadPoint + columnOffset(flowThreadPoint);
    }

    virtual LayoutPoint visualPointToFlowThreadPoint(const LayoutPoint& visualPoint) const = 0;

    virtual LayoutMultiColumnSet* columnSetAtBlockOffset(LayoutUnit) const = 0;

    virtual const char* name() const = 0;

protected:
    void generateColumnSetIntervalTree();

    LayoutMultiColumnSetList m_multiColumnSetList;

    typedef PODInterval<LayoutUnit, LayoutMultiColumnSet*> MultiColumnSetInterval;
    typedef PODIntervalTree<LayoutUnit, LayoutMultiColumnSet*> MultiColumnSetIntervalTree;

    class MultiColumnSetSearchAdapter {
    public:
        MultiColumnSetSearchAdapter(LayoutUnit offset)
            : m_offset(offset)
            , m_result(nullptr)
        {
        }

        const LayoutUnit& lowValue() const { return m_offset; }
        const LayoutUnit& highValue() const { return m_offset; }
        void collectIfNeeded(const MultiColumnSetInterval&);

        LayoutMultiColumnSet* result() const { return m_result; }

    private:
        LayoutUnit m_offset;
        LayoutMultiColumnSet* m_result;
    };

    MultiColumnSetIntervalTree m_multiColumnSetIntervalTree;

    bool m_columnSetsInvalidated : 1;
    bool m_pageLogicalSizeChanged : 1;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutFlowThread, isLayoutFlowThread());

// These structures are used by PODIntervalTree for debugging.
#ifndef NDEBUG
template <> struct ValueToString<LayoutUnit> {
    static String string(const LayoutUnit value) { return String::number(value.toFloat()); }
};

template <> struct ValueToString<LayoutMultiColumnSet*> {
    static String string(const LayoutMultiColumnSet* value) { return String::format("%p", value); }
};
#endif

} // namespace blink

#endif // LayoutFlowThread_h
