/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2007 David Smith (catfish.man@gmail.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
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

#ifndef LayoutBlock_h
#define LayoutBlock_h

#include "core/CoreExport.h"
#include "core/layout/FloatingObjects.h"
#include "core/layout/GapRects.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/line/LineBoxList.h"
#include "core/layout/line/RootInlineBox.h"
#include "core/style/ShapeValue.h"
#include "platform/text/TextBreakIterator.h"
#include "wtf/ListHashSet.h"
#include "wtf/OwnPtr.h"

namespace blink {

class LineLayoutState;
struct PaintInfo;
class LayoutInline;
class WordMeasurement;

typedef WTF::ListHashSet<LayoutBox*, 16> TrackedLayoutBoxListHashSet;
typedef WTF::HashMap<const LayoutBlock*, OwnPtr<TrackedLayoutBoxListHashSet>> TrackedDescendantsMap;
typedef WTF::HashMap<const LayoutBox*, OwnPtr<HashSet<LayoutBlock*>>> TrackedContainerMap;
typedef Vector<WordMeasurement, 64> WordMeasurements;

enum ContainingBlockState { NewContainingBlock, SameContainingBlock };

typedef WTF::HashMap<LayoutBlock*, OwnPtr<ListHashSet<LayoutInline*>>> ContinuationOutlineTableMap;

ContinuationOutlineTableMap* continuationOutlineTable();

class CORE_EXPORT LayoutBlock : public LayoutBox {
public:
    friend class LineLayoutState;

protected:
    explicit LayoutBlock(ContainerNode*);
    virtual ~LayoutBlock();

public:
    LayoutObject* firstChild() const { ASSERT(children() == virtualChildren()); return children()->firstChild(); }
    LayoutObject* lastChild() const { ASSERT(children() == virtualChildren()); return children()->lastChild(); }

    // If you have a LayoutBlock, use firstChild or lastChild instead.
    void slowFirstChild() const = delete;
    void slowLastChild() const = delete;

    const LayoutObjectChildList* children() const { return &m_children; }
    LayoutObjectChildList* children() { return &m_children; }

    bool beingDestroyed() const { return m_beingDestroyed; }

    // These two functions are overridden for inline-block.
    virtual LayoutUnit lineHeight(bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override final;
    virtual int baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override;

    LayoutUnit minLineHeightForReplacedObject(bool isFirstLine, LayoutUnit replacedHeight) const;

    LineBoxList* lineBoxes() { return &m_lineBoxes; }

    virtual const char* name() const override;

protected:
    InlineFlowBox* firstLineBox() const { return m_lineBoxes.firstLineBox(); }
    InlineFlowBox* lastLineBox() const { return m_lineBoxes.lastLineBox(); }

    RootInlineBox* firstRootBox() const { return static_cast<RootInlineBox*>(firstLineBox()); }
    RootInlineBox* lastRootBox() const { return static_cast<RootInlineBox*>(lastLineBox()); }

public:
    // FIXME-BLOCKFLOW: Remove virtualizaion when all callers have moved to LayoutBlockFlow
    virtual void deleteLineBoxTree();

    virtual void addChild(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject*) override;

    virtual void layoutBlock(bool relayoutChildren);

    void insertPositionedObject(LayoutBox*);
    static void removePositionedObject(LayoutBox*);
    void removePositionedObjects(LayoutBlock*, ContainingBlockState = SameContainingBlock);

    TrackedLayoutBoxListHashSet* positionedObjects() const;
    bool hasPositionedObjects() const
    {
        TrackedLayoutBoxListHashSet* objects = positionedObjects();
        return objects && !objects->isEmpty();
    }

    void addPercentHeightDescendant(LayoutBox*);
    static void removePercentHeightDescendant(LayoutBox*);
    static bool hasPercentHeightContainerMap();
    static bool hasPercentHeightDescendant(LayoutBox*);
    static void clearPercentHeightDescendantsFrom(LayoutBox*);
    static void removePercentHeightDescendantIfNeeded(LayoutBox*);

    TrackedLayoutBoxListHashSet* percentHeightDescendants() const;
    bool hasPercentHeightDescendants() const
    {
        TrackedLayoutBoxListHashSet* descendants = percentHeightDescendants();
        return descendants && !descendants->isEmpty();
    }

    void notifyScrollbarThicknessChanged() { m_widthAvailableToChildrenChanged = true; }

    void setHasMarkupTruncation(bool b) { m_hasMarkupTruncation = b; }
    bool hasMarkupTruncation() const { return m_hasMarkupTruncation; }

    void setHasMarginBeforeQuirk(bool b) { m_hasMarginBeforeQuirk = b; }
    void setHasMarginAfterQuirk(bool b) { m_hasMarginAfterQuirk = b; }

    bool hasMarginBeforeQuirk() const { return m_hasMarginBeforeQuirk; }
    bool hasMarginAfterQuirk() const { return m_hasMarginAfterQuirk; }

    bool hasMarginBeforeQuirk(const LayoutBox* child) const;
    bool hasMarginAfterQuirk(const LayoutBox* child) const;

    void markPositionedObjectsForLayout();
    // FIXME: Do we really need this to be virtual? It's just so we can call this on
    // LayoutBoxes without needed to check whether they're LayoutBlocks first.
    virtual void markForPaginationRelayoutIfNeeded(SubtreeLayoutScope&) override final;

    LayoutUnit textIndentOffset() const;

    virtual PositionWithAffinity positionForPoint(const LayoutPoint&) override;

    LayoutUnit blockDirectionOffset(const LayoutSize& offsetFromBlock) const;
    LayoutUnit inlineDirectionOffset(const LayoutSize& offsetFromBlock) const;

    LayoutBlock* blockBeforeWithinSelectionRoot(LayoutSize& offset) const;

    virtual void setSelectionState(SelectionState) override;

    LayoutRect logicalRectToPhysicalRect(const LayoutPoint& physicalPosition, const LayoutRect& logicalRect) const;

    // Helper methods for computing line counts and heights for line counts.
    RootInlineBox* lineAtIndex(int) const;
    int lineCount(const RootInlineBox* = nullptr, bool* = nullptr) const;
    int heightForLineCount(int);
    void clearTruncation();

    void addContinuationWithOutline(LayoutInline*);

    virtual LayoutBoxModelObject* virtualContinuation() const override final { return continuation(); }
    bool isAnonymousBlockContinuation() const { return continuation() && isAnonymousBlock(); }
    LayoutInline* inlineElementContinuation() const;

    using LayoutBoxModelObject::continuation;
    using LayoutBoxModelObject::setContinuation;

    static LayoutBlock* createAnonymousWithParentAndDisplay(const LayoutObject*, EDisplay = BLOCK);
    LayoutBlock* createAnonymousBlock(EDisplay display = BLOCK) const { return createAnonymousWithParentAndDisplay(this, display); }

    virtual LayoutBox* createAnonymousBoxWithSameTypeAs(const LayoutObject* parent) const override;

    int columnGap() const;

    // Accessors for logical width/height and margins in the containing block's block-flow direction.
    LayoutUnit logicalWidthForChild(const LayoutBox& child) const { return isHorizontalWritingMode() ? child.size().width() : child.size().height(); }
    LayoutUnit logicalHeightForChild(const LayoutBox& child) const { return isHorizontalWritingMode() ? child.size().height() : child.size().width(); }
    LayoutSize logicalSizeForChild(const LayoutBox& child) const { return isHorizontalWritingMode() ? child.size() : child.size().transposedSize(); }
    LayoutUnit logicalTopForChild(const LayoutBox& child) const { return isHorizontalWritingMode() ? child.location().y() : child.location().x(); }
    LayoutUnit marginBeforeForChild(const LayoutBoxModelObject& child) const { return child.marginBefore(style()); }
    LayoutUnit marginAfterForChild(const LayoutBoxModelObject& child) const { return child.marginAfter(style()); }
    LayoutUnit marginStartForChild(const LayoutBoxModelObject& child) const { return child.marginStart(style()); }
    LayoutUnit marginEndForChild(const LayoutBoxModelObject& child) const { return child.marginEnd(style()); }
    void setMarginStartForChild(LayoutBox& child, LayoutUnit value) const { child.setMarginStart(value, style()); }
    void setMarginEndForChild(LayoutBox& child, LayoutUnit value) const { child.setMarginEnd(value, style()); }
    void setMarginBeforeForChild(LayoutBox& child, LayoutUnit value) const { child.setMarginBefore(value, style()); }
    void setMarginAfterForChild(LayoutBox& child, LayoutUnit value) const { child.setMarginAfter(value, style()); }
    LayoutUnit collapsedMarginBeforeForChild(const LayoutBox& child) const;
    LayoutUnit collapsedMarginAfterForChild(const LayoutBox& child) const;

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override;

    virtual void scrollbarsChanged(bool /*horizontalScrollbarChanged*/, bool /*verticalScrollbarChanged*/) { }

    LayoutUnit availableLogicalWidthForContent() const { return max<LayoutUnit>(0, logicalRightOffsetForContent() - logicalLeftOffsetForContent()); }
    LayoutUnit logicalLeftOffsetForContent() const { return isHorizontalWritingMode() ? borderLeft() + paddingLeft() : borderTop() + paddingTop(); }
    LayoutUnit logicalRightOffsetForContent() const { return logicalLeftOffsetForContent() + availableLogicalWidth(); }
    LayoutUnit startOffsetForContent() const { return style()->isLeftToRightDirection() ? logicalLeftOffsetForContent() : logicalWidth() - logicalRightOffsetForContent(); }
    LayoutUnit endOffsetForContent() const { return !style()->isLeftToRightDirection() ? logicalLeftOffsetForContent() : logicalWidth() - logicalRightOffsetForContent(); }

    virtual LayoutUnit logicalLeftSelectionOffset(const LayoutBlock* rootBlock, LayoutUnit position) const;
    virtual LayoutUnit logicalRightSelectionOffset(const LayoutBlock* rootBlock, LayoutUnit position) const;

#if ENABLE(ASSERT)
    void checkPositionedObjectsNeedLayout();
    bool paintsContinuationOutline(LayoutInline* flow);
#endif
#ifndef NDEBUG
    void showLineTreeAndMark(const InlineBox* = nullptr, const char* = nullptr, const InlineBox* = nullptr, const char* = nullptr, const LayoutObject* = nullptr) const;
#endif

    bool recalcChildOverflowAfterStyleChange();
    bool recalcOverflowAfterStyleChange();

protected:
    virtual void willBeDestroyed() override;

    void dirtyForLayoutFromPercentageHeightDescendants(SubtreeLayoutScope&);

    virtual void layout() override;
    virtual bool updateImageLoadingPriorities() override final;

    enum PositionedLayoutBehavior {
        DefaultLayout,
        LayoutOnlyFixedPositionedObjects,
        ForcedLayoutAfterContainingBlockMoved
    };

    void layoutPositionedObjects(bool relayoutChildren, PositionedLayoutBehavior = DefaultLayout);
    void markFixedPositionObjectForLayoutIfNeeded(LayoutObject* child, SubtreeLayoutScope&);

    LayoutUnit marginIntrinsicLogicalWidthForChild(LayoutBox& child) const;

    int beforeMarginInLineDirection(LineDirectionMode) const;

    virtual void paint(const PaintInfo&, const LayoutPoint&) override;
public:
    virtual void paintObject(const PaintInfo&, const LayoutPoint&);
    virtual void paintChildren(const PaintInfo&, const LayoutPoint&);

    // FIXME-BLOCKFLOW: Remove virtualizaion when all callers have moved to LayoutBlockFlow
    virtual void paintFloats(const PaintInfo&, const LayoutPoint&, bool) { }
    virtual void paintSelection(const PaintInfo&, const LayoutPoint&) { }

protected:
    virtual void adjustInlineDirectionLineBounds(unsigned /* expansionOpportunityCount */, LayoutUnit& /* logicalLeft */, LayoutUnit& /* logicalWidth */) const { }

    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const override;
    virtual void computePreferredLogicalWidths() override;

    virtual int firstLineBoxBaseline() const override;
    virtual int inlineBlockBaseline(LineDirectionMode) const override;
    int lastLineBoxBaseline(LineDirectionMode) const;

    virtual void updateHitTestResult(HitTestResult&, const LayoutPoint&) override;

    // Delay update scrollbar until finishDelayUpdateScrollInfo() will be
    // called. This function is used when a flexbox is laying out its
    // descendant. If multiple calls are made to startDelayUpdateScrollInfo(),
    // finishDelayUpdateScrollInfo() will do nothing until finishDelayUpdateScrollInfo()
    // is called the same number of times.
    static void startDelayUpdateScrollInfo();
    static void finishDelayUpdateScrollInfo();

    void updateScrollInfoAfterLayout();

    virtual void styleWillChange(StyleDifference, const ComputedStyle& newStyle) override;
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    virtual bool hasLineIfEmpty() const;

    bool simplifiedLayout();
    virtual void simplifiedNormalFlowLayout();

public:
    virtual void computeOverflow(LayoutUnit oldClientAfterEdge, bool = false);
protected:
    virtual void addOverflowFromChildren();
    void addOverflowFromPositionedObjects();
    void addOverflowFromBlockChildren();
    void addVisualOverflowFromTheme();

    virtual void addFocusRingRects(Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const override;

    virtual void computeSelfHitTestRects(Vector<LayoutRect>&, const LayoutPoint& layerOffset) const override;

    void updateBlockChildDirtyBitsBeforeLayout(bool relayoutChildren, LayoutBox&);

    virtual bool isInlineBlockOrInlineTable() const override final { return isInline() && isReplaced(); }

    virtual void invalidatePaintOfSubtreesIfNeeded(PaintInvalidationState& childPaintInvalidationState) override;

private:
    virtual LayoutObjectChildList* virtualChildren() override final { return children(); }
    virtual const LayoutObjectChildList* virtualChildren() const override final { return children(); }

    virtual bool isLayoutBlock() const override final { return true; }

    void makeChildrenNonInline(LayoutObject* insertionPoint = nullptr);

    // Promote all children and make them siblings that come right after this block.
    void promoteAllChildrenAndInsertAfter();

    virtual void removeLeftoverAnonymousBlock(LayoutBlock* child);

    static void collapseAnonymousBlockChild(LayoutBlock* parent, LayoutBlock* child);

    virtual void dirtyLinesFromChangedChild(LayoutObject* child) override final { m_lineBoxes.dirtyLinesFromChangedChild(this, child); }

    virtual void addChildIgnoringContinuation(LayoutObject* newChild, LayoutObject* beforeChild) override;

    virtual bool isSelfCollapsingBlock() const override;

    void removeAnonymousWrappersIfRequired();

    void insertIntoTrackedLayoutBoxMaps(LayoutBox* descendant, TrackedDescendantsMap*&, TrackedContainerMap*&);
    static void removeFromTrackedLayoutBoxMaps(LayoutBox* descendant, TrackedDescendantsMap*&, TrackedContainerMap*&);

    Node* nodeForHitTest() const;

    bool tryLayoutDoingPositionedMovementOnly();

    virtual bool avoidsFloats() const override { return true; }

    bool hitTestContents(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction);
    // FIXME-BLOCKFLOW: Remove virtualizaion when all callers have moved to LayoutBlockFlow
    virtual bool hitTestFloats(HitTestResult&, const HitTestLocation&, const LayoutPoint&) { return false; }

    virtual bool isPointInOverflowControl(HitTestResult&, const LayoutPoint& locationInContainer, const LayoutPoint& accumulatedOffset);

    void computeBlockPreferredLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const;

    // Obtains the nearest enclosing block (including this block) that contributes a first-line style to our inline
    // children.
    virtual LayoutBlock* firstLineBlock() const override;

    virtual LayoutRect rectWithOutlineForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, LayoutUnit outlineWidth, const PaintInvalidationState* = nullptr) const override final;

    virtual LayoutObject* hoverAncestor() const override final;
    virtual void updateDragState(bool dragOn) override final;
    virtual void childBecameNonInline(LayoutObject* child) override final;

    bool isSelectionRoot() const;

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint& accumulatedOffset) const override;
    virtual void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed) const override;

private:
    virtual LayoutRect localCaretRect(InlineBox*, int caretOffset, LayoutUnit* extraWidthToEndOfLine = nullptr) override final;
    bool isInlineBoxWrapperActuallyChild() const;

    void markLinesDirtyInBlockRange(LayoutUnit logicalTop, LayoutUnit logicalBottom, RootInlineBox* highest = nullptr);

    Position positionForBox(InlineBox*, bool start = true) const;
    PositionWithAffinity positionForPointWithInlineChildren(const LayoutPoint&);

    // End helper functions and structs used by layoutBlockChildren.

    void removeFromGlobalMaps();
    bool widthAvailableToChildrenHasChanged();

protected:
    bool isPageLogicalHeightKnown(LayoutUnit logicalOffset) const { return pageLogicalHeightForOffset(logicalOffset); }

    // Returns the logicalOffset at the top of the next page. If the offset passed in is already at the top of the current page,
    // then nextPageLogicalTop with ExcludePageBoundary will still move to the top of the next page. nextPageLogicalTop with
    // IncludePageBoundary set will not.
    //
    // For a page height of 800px, the first rule will return 800 if the value passed in is 0. The second rule will simply return 0.
    enum PageBoundaryRule { ExcludePageBoundary, IncludePageBoundary };
    LayoutUnit nextPageLogicalTop(LayoutUnit logicalOffset, PageBoundaryRule = ExcludePageBoundary) const;

    bool createsNewFormattingContext() const;

public:
    LayoutUnit pageLogicalHeightForOffset(LayoutUnit offset) const;
    LayoutUnit pageRemainingLogicalHeightForOffset(LayoutUnit offset, PageBoundaryRule = IncludePageBoundary) const;

protected:
    // A page break is required at some offset due to space shortage in the current fragmentainer.
    void setPageBreak(LayoutUnit offset, LayoutUnit spaceShortage);

    // Update minimum page height required to avoid fragmentation where it shouldn't occur (inside
    // unbreakable content, between orphans and widows, etc.). This will be used as a hint to the
    // column balancer to help set a good minimum column height.
    void updateMinimumPageHeight(LayoutUnit offset, LayoutUnit minHeight);

    // Adjust from painting offsets to the local coords of this layoutObject
    void offsetForContents(LayoutPoint&) const;

    virtual bool updateLogicalWidthAndColumnWidth();

    virtual bool canCollapseAnonymousBlockChild() const { return true; }

    LayoutObjectChildList m_children;
    LineBoxList m_lineBoxes; // All of the root line boxes created for this block flow.  For example, <div>Hello<br>world.</div> will have two total lines for the <div>.

    unsigned m_hasMarginBeforeQuirk : 1; // Note these quirk values can't be put in LayoutBlockRareData since they are set too frequently.
    unsigned m_hasMarginAfterQuirk : 1;
    unsigned m_beingDestroyed : 1;
    unsigned m_hasMarkupTruncation : 1;
    unsigned m_widthAvailableToChildrenChanged  : 1;
    mutable unsigned m_hasOnlySelfCollapsingChildren : 1;
    mutable unsigned m_descendantsWithFloatsMarkedForLayout : 1;

    // LayoutRubyBase objects need to be able to split and merge, moving their children around
    // (calling moveChildTo, moveAllChildrenTo, and makeChildrenNonInline).
    friend class LayoutRubyBase;
    // FIXME-BLOCKFLOW: Remove this when the line layout stuff has all moved out of LayoutBlock
    friend class LineBreaker;

    // FIXME: This is temporary as we move code that accesses block flow
    // member variables out of LayoutBlock and into LayoutBlockFlow.
    friend class LayoutBlockFlow;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutBlock, isLayoutBlock());

} // namespace blink

#endif // LayoutBlock_h
