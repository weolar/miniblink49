/*
 * Copyright (C) 2012 Apple Inc.  All rights reserved.
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


#ifndef LayoutMultiColumnFlowThread_h
#define LayoutMultiColumnFlowThread_h

#include "core/CoreExport.h"
#include "core/layout/LayoutFlowThread.h"

namespace blink {

class LayoutMultiColumnSet;
class LayoutMultiColumnSpannerPlaceholder;

enum BalancedColumnHeightCalculation { GuessFromFlowThreadPortion, StretchBySpaceShortage };

// Flow thread implementation for CSS multicol. This will be inserted as an anonymous child block of
// the actual multicol container (i.e. the LayoutBlockFlow whose style computes to non-auto
// column-count and/or column-width). LayoutMultiColumnFlowThread is the heart of the multicol
// implementation, and there is only one instance per multicol container. Child content of the
// multicol container is parented into the flow thread at the time of layoutObject insertion.
//
// Apart from this flow thread child, the multicol container will also have LayoutMultiColumnSet
// children, which are used to position the columns visually. The flow thread is in charge
// of layout, and, after having calculated the column width, it lays out content as if everything
// were in one tall single column, except that there will typically be some amount of blank space
// (also known as pagination struts) at the offsets where the actual column boundaries are. This
// way, content that needs to be preceded by a break will appear at the top of the next
// column. Content needs to be preceded by a break when there's a forced break or when the content
// is unbreakable and cannot fully fit in the same column as the preceding piece of
// content. Although a LayoutMultiColumnFlowThread is laid out, it does not take up any space in its
// container. It's the LayoutMultiColumnSet objects that take up the necessary amount of space, and
// make sure that the columns are painted and hit-tested correctly.
//
// If there is any column content inside the multicol container, we create a
// LayoutMultiColumnSet. We only need to create multiple sets if there are spanners
// (column-span:all) in the multicol container. When a spanner is inserted, content preceding it
// gets its own set, and content succeeding it will get another set. The spanner itself will also
// get its own placeholder between the sets (LayoutMultiColumnSpannerPlaceholder), so that it gets
// positioned and sized correctly. The column-span:all element is inside the flow thread, but its
// containing block is the multicol container.
//
// Some invariants for the layout tree structure for multicol:
// - A multicol container is always a LayoutBlockFlow
// - Every multicol container has one and only one LayoutMultiColumnFlowThread
// - All multicol DOM children and pseudo-elements associated with the multicol container are
//   reparented into the flow thread
// - The LayoutMultiColumnFlowThread is the first child of the multicol container
// - A multicol container may only have LayoutMultiColumnFlowThread, LayoutMultiColumnSet and
//   LayoutMultiColumnSpannerPlaceholder children
// - A LayoutMultiColumnSet may not be adjacent to another LayoutMultiColumnSet; there are no
//   use-cases for it, and there are also implementation limitations behind this requirement.
// - The flow thread is not in the containing block chain for children that are not to be laid out
//   in columns. This means column spanners and absolutely positioned children whose containing
//   block is outside column content
// - Each spanner (column-span:all) establishes a LayoutMultiColumnSpannerPlaceholder
//
// The width of the flow thread is the same as the column width. The width of a column set is the
// same as the content box width of the multicol container; in other words exactly enough to hold
// the number of columns to be used, stacked horizontally, plus column gaps between them.
//
// Since it's the first child of the multicol container, the flow thread is laid out first, albeit
// in a slightly special way, since it's not to take up any space in its ancestors. Afterwards, the
// column sets are laid out. Column sets get their height from the columns that they hold. In single
// column-row constrained height non-balancing cases without spanners this will simply be the same
// as the content height of the multicol container itself. In most other cases we'll have to
// calculate optimal column heights ourselves, though. This process is referred to as column
// balancing, and then we infer the column set height from the height of the flow thread portion
// occupied by each set.
//
// More on column balancing: the columns' height is unknown in the first layout pass when
// balancing. This means that we cannot insert any implicit (soft / unforced) breaks (and pagination
// struts) when laying out the contents of the flow thread. We'll just lay out everything in tall
// single strip. After the initial flow thread layout pass we can determine a tentative / minimal /
// initial column height. This is calculated by simply dividing the flow thread's height by the
// number of specified columns. In the layout pass that follows, we can insert breaks (and
// pagination struts) at column boundaries, since we now have a column height. It may very easily
// turn out that the calculated height wasn't enough, though. We'll notice this at end of layout. If
// we end up with too many columns (i.e. columns overflowing the multicol container), it wasn't
// enough. In this case we need to increase the column heights. We'll increase them by the lowest
// amount of space that could possibly affect where the breaks occur (see
// LayoutMultiColumnSet::recordSpaceShortage()). We'll relayout (to find new break points and the
// new lowest amount of space increase that could affect where they occur, in case we need another
// round) until we've reached an acceptable height (where everything fits perfectly in the number of
// columns that we have specified). The rule of thumb is that we shouldn't have to perform more of
// such iterations than the number of columns that we have.
//
// For each layout iteration done for column balancing, the flow thread will need a deep layout if
// column heights changed in the previous pass, since column height changes may affect break points
// and pagination struts anywhere in the tree, and currently no way exists to do this in a more
// optimized manner.
//
// There's also some documentation online:
// https://sites.google.com/a/chromium.org/dev/developers/design-documents/multi-column-layout
class CORE_EXPORT LayoutMultiColumnFlowThread : public LayoutFlowThread {
public:
    virtual ~LayoutMultiColumnFlowThread();

    static LayoutMultiColumnFlowThread* createAnonymous(Document&, const ComputedStyle& parentStyle);

    virtual bool isLayoutMultiColumnFlowThread() const override final { return true; }

    LayoutBlockFlow* multiColumnBlockFlow() const { return toLayoutBlockFlow(parent()); }

    LayoutMultiColumnSet* firstMultiColumnSet() const;
    LayoutMultiColumnSet* lastMultiColumnSet() const;

    // Return the first column set or spanner placeholder.
    LayoutBox* firstMultiColumnBox() const
    {
        return nextSiblingBox();
    }
    // Return the last column set or spanner placeholder.
    LayoutBox* lastMultiColumnBox() const
    {
        LayoutBox* lastSiblingBox = multiColumnBlockFlow()->lastChildBox();
        // The flow thread is the first child of the multicol container. If the flow thread is also
        // the last child, it means that there are no siblings; i.e. we have no column boxes.
        return lastSiblingBox != this ? lastSiblingBox : 0;
    }

    // Find the first set inside which the specified layoutObject (which is a flowthread descendant) would be rendered.
    LayoutMultiColumnSet* mapDescendantToColumnSet(LayoutObject*) const;

    // Return the spanner placeholder that belongs to the spanner in the containing block chain, if
    // any. This includes the layoutObject for the element that actually establishes the spanner too.
    LayoutMultiColumnSpannerPlaceholder* containingColumnSpannerPlaceholder(const LayoutObject* descendant) const;

    // Populate the flow thread with what's currently its siblings. Called when a regular block
    // becomes a multicol container.
    void populate();

    // Empty the flow thread by moving everything to the parent. Remove all multicol specific
    // layoutObjects. Then destroy the flow thread. Called when a multicol container becomes a regular
    // block.
    void evacuateAndDestroy();

    unsigned columnCount() const { return m_columnCount; }
    LayoutUnit columnHeightAvailable() const { return m_columnHeightAvailable; }
    void setColumnHeightAvailable(LayoutUnit available) { m_columnHeightAvailable = available; }
    bool progressionIsInline() const { return m_progressionIsInline; }

    virtual LayoutSize columnOffset(const LayoutPoint&) const override final;

    // Do we need to set a new width and lay out?
    virtual bool needsNewWidth() const;

    virtual LayoutPoint visualPointToFlowThreadPoint(const LayoutPoint& visualPoint) const override;

    virtual LayoutMultiColumnSet* columnSetAtBlockOffset(LayoutUnit) const override final;

    void layoutColumns(bool relayoutChildren, SubtreeLayoutScope&);

    bool recalculateColumnHeights();

    void columnRuleStyleDidChange();

    // Remove the spanner placeholder and return true if the specified object is no longer a valid spanner.
    bool removeSpannerPlaceholderIfNoLongerValid(LayoutBox* spannerObjectInFlowThread);

    virtual const char* name() const override { return "LayoutMultiColumnFlowThread"; }

protected:
    LayoutMultiColumnFlowThread();
    void setProgressionIsInline(bool isInline) { m_progressionIsInline = isInline; }

    virtual void layout() override;

private:
    void calculateColumnCountAndWidth(LayoutUnit& width, unsigned& count) const;
    void createAndInsertMultiColumnSet(LayoutBox* insertBefore = nullptr);
    void createAndInsertSpannerPlaceholder(LayoutBox* spannerObjectInFlowThread, LayoutObject* insertedBeforeInFlowThread);
    void destroySpannerPlaceholder(LayoutMultiColumnSpannerPlaceholder*);
    virtual bool descendantIsValidColumnSpanner(LayoutObject* descendant) const;

    virtual void addColumnSetToThread(LayoutMultiColumnSet*) override;
    virtual void willBeRemovedFromTree() override;
    virtual void skipColumnSpanner(LayoutBox*, LayoutUnit logicalTopInFlowThread) override;
    virtual void flowThreadDescendantWasInserted(LayoutObject*) final;
    virtual void flowThreadDescendantWillBeRemoved(LayoutObject*) final;
    virtual void flowThreadDescendantStyleWillChange(LayoutObject*, StyleDifference, const ComputedStyle& newStyle) override;
    virtual void flowThreadDescendantStyleDidChange(LayoutObject*, StyleDifference, const ComputedStyle& oldStyle) override;
    virtual void computePreferredLogicalWidths() override;
    virtual void computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues&) const override;
    virtual void updateLogicalWidth() override;
    virtual void setPageBreak(LayoutUnit offset, LayoutUnit spaceShortage) override;
    virtual void updateMinimumPageHeight(LayoutUnit offset, LayoutUnit minHeight) override;
    virtual bool addForcedColumnBreak(LayoutUnit, LayoutObject* breakChild, bool isBefore, LayoutUnit* offsetBreakAdjustment = nullptr) override;
    virtual bool isPageLogicalHeightKnown() const override;

    // The last set we worked on. It's not to be used as the "current set". The concept of a
    // "current set" is difficult, since layout may jump back and forth in the tree, due to wrong
    // top location estimates (due to e.g. margin collapsing), and possibly for other reasons.
    LayoutMultiColumnSet* m_lastSetWorkedOn;

    unsigned m_columnCount; // The used value of column-count
    LayoutUnit m_columnHeightAvailable; // Total height available to columns, or 0 if auto.
    bool m_inBalancingPass; // Set when relayouting for column balancing.
    bool m_needsColumnHeightsRecalculation; // Set when we need to recalculate the column set heights after layout.
    bool m_progressionIsInline; // Always true for regular multicol. False for paged-y overflow.
    bool m_isBeingEvacuated;
};

} // namespace blink

#endif // LayoutMultiColumnFlowThread_h
