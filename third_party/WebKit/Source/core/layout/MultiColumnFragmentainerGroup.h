// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MultiColumnFragmentainerGroup_h
#define MultiColumnFragmentainerGroup_h

#include "core/layout/LayoutMultiColumnFlowThread.h"

namespace blink {

// A group of columns, that are laid out in the inline progression direction, all with the same
// column height.
//
// When a multicol container is inside another fragmentation context, and said multicol container
// lives in multiple outer fragmentainers (pages / columns), we need to put these inner columns into
// separate groups, with one group per outer fragmentainer. Such a group of columns is what
// comprises a "row of column boxes" in spec lingo.
//
// Column balancing, when enabled, takes place within a column fragmentainer group.
//
// Each fragmentainer group may have its own actual column count (if there are unused columns
// because of forced breaks, for example). If there are multiple fragmentainer groups, the actual
// column count must not exceed the used column count (the one calculated based on column-count and
// column-width from CSS), or they'd overflow the outer fragmentainer in the inline direction. If we
// need more columns than what a group has room for, we'll create another group and put them there
// (and make them appear in the next outer fragmentainer).
class MultiColumnFragmentainerGroup {
public:
    MultiColumnFragmentainerGroup(LayoutMultiColumnSet&);

    bool isLastGroup() const;

    // Position within the LayoutMultiColumnSet.
    LayoutUnit logicalTop() const { return m_logicalTop; }

    LayoutUnit logicalHeight() const { return m_columnHeight; }

    LayoutSize offsetFromColumnSet() const;

    // The top of our flow thread portion
    LayoutUnit logicalTopInFlowThread() const { return m_logicalTopInFlowThread; }
    void setLogicalTopInFlowThread(LayoutUnit logicalTopInFlowThread) { m_logicalTopInFlowThread = logicalTopInFlowThread; }

    // The bottom of our flow thread portion
    LayoutUnit logicalBottomInFlowThread() const { return m_logicalBottomInFlowThread; }
    void setLogicalBottomInFlowThread(LayoutUnit logicalBottomInFlowThread) { m_logicalBottomInFlowThread = logicalBottomInFlowThread; }

    // The height of our flow thread portion
    LayoutUnit logicalHeightInFlowThread() const { return m_logicalBottomInFlowThread - m_logicalTopInFlowThread; }

    bool heightIsAuto() const;
    void resetColumnHeight();
    void addContentRun(LayoutUnit endOffsetInFlowThread);
    void updateMinimumColumnHeight(LayoutUnit height) { m_minimumColumnHeight = std::max(height, m_minimumColumnHeight); }
    void recordSpaceShortage(LayoutUnit);
    bool recalculateColumnHeight(BalancedColumnHeightCalculation calculationMode);

    void expandToEncompassFlowThreadOverflow();

    LayoutSize flowThreadTranslationAtOffset(LayoutUnit offsetInFlowThread) const;
    LayoutUnit columnLogicalTopForOffset(LayoutUnit offsetInFlowThread) const;
    LayoutPoint visualPointToFlowThreadPoint(const LayoutPoint& visualPoint) const;
    void collectLayerFragments(DeprecatedPaintLayerFragments&, const LayoutRect& layerBoundingBox, const LayoutRect& dirtyRect) const;
    LayoutRect calculateOverflow() const;

    // The "CSS actual" value of column-count. This includes overflowing columns, if any.
    unsigned actualColumnCount() const;

private:
    LayoutUnit heightAdjustedForRowOffset(LayoutUnit height) const;
    LayoutUnit calculateMaxColumnHeight() const;
    void setAndConstrainColumnHeight(LayoutUnit);

    // Return the index of the content run with the currently tallest columns, taking all implicit
    // breaks assumed so far into account.
    unsigned findRunWithTallestColumns() const;

    // Given the current list of content runs, make assumptions about where we need to insert
    // implicit breaks (if there's room for any at all; depending on the number of explicit breaks),
    // and store the results. This is needed in order to balance the columns.
    void distributeImplicitBreaks();

    LayoutUnit calculateColumnHeight(BalancedColumnHeightCalculation) const;

    LayoutRect columnRectAt(unsigned columnIndex) const;
    LayoutUnit logicalTopInFlowThreadAt(unsigned columnIndex) const { return m_logicalTopInFlowThread + columnIndex * m_columnHeight; }
    LayoutRect flowThreadPortionRectAt(unsigned columnIndex) const;
    LayoutRect flowThreadPortionOverflowRect(const LayoutRect& flowThreadPortion, unsigned columnIndex, unsigned columnCount, LayoutUnit columnGap) const;

    enum ColumnIndexCalculationMode {
        ClampToExistingColumns, // Stay within the range of already existing columns.
        AssumeNewColumns // Allow column indices outside the range of already existing columns.
    };
    unsigned columnIndexAtOffset(LayoutUnit offsetInFlowThread, ColumnIndexCalculationMode = ClampToExistingColumns) const;

    // Return the column that the specified visual point belongs to. Only the coordinate on the
    // column progression axis is relevant. Every point belongs to a column, even if said point is
    // not inside any of the columns.
    unsigned columnIndexAtVisualPoint(const LayoutPoint& visualPoint) const;

    LayoutMultiColumnSet& m_columnSet;

    LayoutUnit m_logicalTop;
    LayoutUnit m_logicalTopInFlowThread;
    LayoutUnit m_logicalBottomInFlowThread;

    LayoutUnit m_columnHeight;

    // The following variables are used when balancing the column set.
    LayoutUnit m_maxColumnHeight; // Maximum column height allowed.
    LayoutUnit m_minSpaceShortage; // The smallest amout of space shortage that caused a column break.
    LayoutUnit m_minimumColumnHeight;

    // A run of content without explicit (forced) breaks; i.e. a flow thread portion between two
    // explicit breaks, between flow thread start and an explicit break, between an explicit break
    // and flow thread end, or, in cases when there are no explicit breaks at all: between flow
    // thread portion start and flow thread portion end. We need to know where the explicit breaks
    // are, in order to figure out where the implicit breaks will end up, so that we get the columns
    // properly balanced. A content run starts out as representing one single column, and will
    // represent one additional column for each implicit break "inserted" there.
    class ContentRun {
    public:
        ContentRun(LayoutUnit breakOffset)
            : m_breakOffset(breakOffset)
            , m_assumedImplicitBreaks(0) { }

        unsigned assumedImplicitBreaks() const { return m_assumedImplicitBreaks; }
        void assumeAnotherImplicitBreak() { m_assumedImplicitBreaks++; }
        LayoutUnit breakOffset() const { return m_breakOffset; }

        // Return the column height that this content run would require, considering the implicit
        // breaks assumed so far.
        LayoutUnit columnLogicalHeight(LayoutUnit startOffset) const { return ceilf((m_breakOffset - startOffset).toFloat() / float(m_assumedImplicitBreaks + 1)); }

    private:
        LayoutUnit m_breakOffset; // Flow thread offset where this run ends.
        unsigned m_assumedImplicitBreaks; // Number of implicit breaks in this run assumed so far.
    };
    Vector<ContentRun, 1> m_contentRuns;
};

// List of all fragmentainer groups within a column set. There will always be at least one
// group. Deleting the one group is not allowed (or possible). There will be more than one group if
// the owning column set lives in multiple outer fragmentainers (e.g. multicol inside paged media).
class CORE_EXPORT MultiColumnFragmentainerGroupList {
public:
    MultiColumnFragmentainerGroupList(LayoutMultiColumnSet&);
    ~MultiColumnFragmentainerGroupList();

    // Add an additional fragmentainer group to the end of the list, and return it.
    MultiColumnFragmentainerGroup& addExtraGroup();

    // Remove all fragmentainer groups but the first one.
    void deleteExtraGroups();

    MultiColumnFragmentainerGroup& first() { return m_groups.first(); }
    const MultiColumnFragmentainerGroup& first() const { return m_groups.first(); }
    MultiColumnFragmentainerGroup& last() { return m_groups.last(); }
    const MultiColumnFragmentainerGroup& last() const { return m_groups.last(); }

    typedef Vector<MultiColumnFragmentainerGroup, 1>::iterator iterator;
    typedef Vector<MultiColumnFragmentainerGroup, 1>::const_iterator const_iterator;

    iterator begin() { return m_groups.begin(); }
    const_iterator begin() const { return m_groups.begin(); }
    iterator end() { return m_groups.end(); }
    const_iterator end() const { return m_groups.end(); }

    void append(const MultiColumnFragmentainerGroup& group) { m_groups.append(group); }

    void shrink(size_t size) { m_groups.shrink(size); }

private:
    LayoutMultiColumnSet& m_columnSet;

    Vector<MultiColumnFragmentainerGroup, 1> m_groups;
};

} // namespace blink

#endif // MultiColumnFragmentainerGroup_h
