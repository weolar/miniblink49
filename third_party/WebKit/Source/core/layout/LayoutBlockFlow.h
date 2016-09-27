/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2007 David Smith (catfish.man@gmail.com)
 * Copyright (C) 2003-2013 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutBlockFlow_h
#define LayoutBlockFlow_h

#include "core/CoreExport.h"
#include "core/layout/FloatingObjects.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/line/TrailingObjects.h"
#include "core/style/ComputedStyleConstants.h"

namespace blink {

class ClipScope;
class MarginInfo;
class LineBreaker;
class LineInfo;
class LineWidth;
class LayoutMultiColumnFlowThread;
class LayoutMultiColumnSpannerPlaceholder;
class LayoutRubyRun;
template <class Run> class BidiRunList;

class CORE_EXPORT LayoutBlockFlow : public LayoutBlock {
public:
    explicit LayoutBlockFlow(ContainerNode*);
    virtual ~LayoutBlockFlow();

    static LayoutBlockFlow* createAnonymous(Document*);

    virtual bool isLayoutBlockFlow() const override final { return true; }

    virtual void layoutBlock(bool relayoutChildren) override;

    virtual void computeOverflow(LayoutUnit oldClientAfterEdge, bool recomputeFloats = false) override;

    virtual void deleteLineBoxTree() override final;

    LayoutUnit availableLogicalWidthForLine(LayoutUnit position, bool shouldIndentText, LayoutUnit logicalHeight = 0) const
    {
        return max<LayoutUnit>(0, logicalRightOffsetForLine(position, shouldIndentText, logicalHeight) - logicalLeftOffsetForLine(position, shouldIndentText, logicalHeight));
    }
    LayoutUnit logicalRightOffsetForLine(LayoutUnit position, bool shouldIndentText, LayoutUnit logicalHeight = 0) const
    {
        return logicalRightOffsetForLine(position, logicalRightOffsetForContent(), shouldIndentText, logicalHeight);
    }
    LayoutUnit logicalLeftOffsetForLine(LayoutUnit position, bool shouldIndentText, LayoutUnit logicalHeight = 0) const
    {
        return logicalLeftOffsetForLine(position, logicalLeftOffsetForContent(), shouldIndentText, logicalHeight);
    }
    LayoutUnit startOffsetForLine(LayoutUnit position, bool shouldIndentText, LayoutUnit logicalHeight = 0) const
    {
        return style()->isLeftToRightDirection() ? logicalLeftOffsetForLine(position, shouldIndentText, logicalHeight)
            : logicalWidth() - logicalRightOffsetForLine(position, shouldIndentText, logicalHeight);
    }
    LayoutUnit endOffsetForLine(LayoutUnit position, bool shouldIndentText, LayoutUnit logicalHeight = 0) const
    {
        return !style()->isLeftToRightDirection() ? logicalLeftOffsetForLine(position, shouldIndentText, logicalHeight)
            : logicalWidth() - logicalRightOffsetForLine(position, shouldIndentText, logicalHeight);
    }

    // FIXME-BLOCKFLOW: Move this into LayoutBlockFlow once there are no calls
    // in LayoutBlock. http://crbug.com/393945, http://crbug.com/302024
    using LayoutBlock::lineBoxes;
    using LayoutBlock::firstLineBox;
    using LayoutBlock::lastLineBox;
    using LayoutBlock::firstRootBox;
    using LayoutBlock::lastRootBox;

    virtual LayoutUnit logicalLeftSelectionOffset(const LayoutBlock* rootBlock, LayoutUnit position) const override;
    virtual LayoutUnit logicalRightSelectionOffset(const LayoutBlock* rootBlock, LayoutUnit position) const override;

    RootInlineBox* createAndAppendRootInlineBox();

    void markAllDescendantsWithFloatsForLayout(LayoutBox* floatToRemove = nullptr, bool inLayout = true);
    void markSiblingsWithFloatsForLayout(LayoutBox* floatToRemove = nullptr);

    bool containsFloats() const { return m_floatingObjects && !m_floatingObjects->set().isEmpty(); }
    bool containsFloat(LayoutBox*) const;

    void removeFloatingObjects();

    virtual void addChild(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) override;

    void moveAllChildrenIncludingFloatsTo(LayoutBlock* toBlock, bool fullRemoveInsert);

    bool generatesLineBoxesForInlineChild(LayoutObject*);

    LayoutUnit logicalTopForFloat(const FloatingObject& floatingObject) const { return isHorizontalWritingMode() ? floatingObject.y() : floatingObject.x(); }
    LayoutUnit logicalBottomForFloat(const FloatingObject& floatingObject) const { return isHorizontalWritingMode() ? floatingObject.maxY() : floatingObject.maxX(); }
    LayoutUnit logicalLeftForFloat(const FloatingObject& floatingObject) const { return isHorizontalWritingMode() ? floatingObject.x() : floatingObject.y(); }
    LayoutUnit logicalRightForFloat(const FloatingObject& floatingObject) const { return isHorizontalWritingMode() ? floatingObject.maxX() : floatingObject.maxY(); }
    LayoutUnit logicalWidthForFloat(const FloatingObject& floatingObject) const { return isHorizontalWritingMode() ? floatingObject.width() : floatingObject.height(); }

    int pixelSnappedLogicalTopForFloat(const FloatingObject& floatingObject) const { return isHorizontalWritingMode() ? floatingObject.frameRect().pixelSnappedY() : floatingObject.frameRect().pixelSnappedX(); }
    int pixelSnappedLogicalBottomForFloat(const FloatingObject& floatingObject) const { return isHorizontalWritingMode() ? floatingObject.frameRect().pixelSnappedMaxY() : floatingObject.frameRect().pixelSnappedMaxX(); }

    void setLogicalTopForFloat(FloatingObject& floatingObject, LayoutUnit logicalTop)
    {
        if (isHorizontalWritingMode())
            floatingObject.setY(logicalTop);
        else
            floatingObject.setX(logicalTop);
    }
    void setLogicalLeftForFloat(FloatingObject& floatingObject, LayoutUnit logicalLeft)
    {
        if (isHorizontalWritingMode())
            floatingObject.setX(logicalLeft);
        else
            floatingObject.setY(logicalLeft);
    }
    void setLogicalHeightForFloat(FloatingObject& floatingObject, LayoutUnit logicalHeight)
    {
        if (isHorizontalWritingMode())
            floatingObject.setHeight(logicalHeight);
        else
            floatingObject.setWidth(logicalHeight);
    }
    void setLogicalWidthForFloat(FloatingObject& floatingObject, LayoutUnit logicalWidth)
    {
        if (isHorizontalWritingMode())
            floatingObject.setWidth(logicalWidth);
        else
            floatingObject.setHeight(logicalWidth);
    }

    LayoutUnit startAlignedOffsetForLine(LayoutUnit position, bool shouldIndentText);

    void setStaticInlinePositionForChild(LayoutBox&, LayoutUnit inlinePosition);
    void updateStaticInlinePositionForChild(LayoutBox&, LayoutUnit logicalTop);

    static bool shouldSkipCreatingRunsForObject(LayoutObject* obj)
    {
        return obj->isFloating() || (obj->isOutOfFlowPositioned() && !obj->style()->isOriginalDisplayInlineType() && !obj->container()->isLayoutInline());
    }

    LayoutMultiColumnFlowThread* multiColumnFlowThread() const { return m_rareData ? m_rareData->m_multiColumnFlowThread : 0; }
    void resetMultiColumnFlowThread()
    {
        if (m_rareData)
            m_rareData->m_multiColumnFlowThread = nullptr;
    }

    void addOverflowFromInlineChildren();

    // FIXME: This should be const to avoid a const_cast, but can modify child dirty bits and LayoutTextCombine
    void computeInlinePreferredLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth);

    virtual bool shouldPaintSelectionGaps() const override final;
    LayoutRect logicalLeftSelectionGap(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
        const LayoutObject* selObj, LayoutUnit logicalLeft, LayoutUnit logicalTop, LayoutUnit logicalHeight, const PaintInfo*) const;
    LayoutRect logicalRightSelectionGap(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
        const LayoutObject* selObj, LayoutUnit logicalRight, LayoutUnit logicalTop, LayoutUnit logicalHeight, const PaintInfo*) const;
    void getSelectionGapInfo(SelectionState, bool& leftGap, bool& rightGap) const;

    virtual LayoutRect selectionRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer) const override final;
    GapRects selectionGapRectsForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer) const;
    GapRects selectionGaps(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
        LayoutUnit& lastLogicalTop, LayoutUnit& lastLogicalLeft, LayoutUnit& lastLogicalRight,
        const PaintInfo* = nullptr, ClipScope* = nullptr) const;
    GapRects inlineSelectionGaps(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
        LayoutUnit& lastLogicalTop, LayoutUnit& lastLogicalLeft, LayoutUnit& lastLogicalRight, const PaintInfo*) const;
    GapRects blockSelectionGaps(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
        LayoutUnit& lastLogicalTop, LayoutUnit& lastLogicalLeft, LayoutUnit& lastLogicalRight, const PaintInfo*) const;
    LayoutRect blockSelectionGap(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
        LayoutUnit lastLogicalTop, LayoutUnit lastLogicalLeft, LayoutUnit lastLogicalRight, LayoutUnit logicalBottom, const PaintInfo*) const;

    LayoutUnit paginationStrut() const { return m_rareData ? m_rareData->m_paginationStrut : LayoutUnit(); }
    void setPaginationStrut(LayoutUnit);

    void positionSpannerDescendant(LayoutMultiColumnSpannerPlaceholder& child);

    virtual bool avoidsFloats() const override;

    using LayoutBoxModelObject::moveChildrenTo;
    virtual void moveChildrenTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* startChild, LayoutObject* endChild, LayoutObject* beforeChild, bool fullRemoveInsert = false) override;

    LayoutUnit xPositionForFloatIncludingMargin(const FloatingObject& child) const
    {
        if (isHorizontalWritingMode())
            return child.x() + child.layoutObject()->marginLeft();

        return child.x() + marginBeforeForChild(*child.layoutObject());
    }

    LayoutUnit yPositionForFloatIncludingMargin(const FloatingObject& child) const
    {
        if (isHorizontalWritingMode())
            return child.y() + marginBeforeForChild(*child.layoutObject());

        return child.y() + child.layoutObject()->marginTop();
    }

    LayoutSize positionForFloatIncludingMargin(const FloatingObject& child) const
    {
        if (isHorizontalWritingMode()) {
            return LayoutSize(child.x() + child.layoutObject()->marginLeft(),
                child.y() + marginBeforeForChild(*child.layoutObject()));
        }

        return LayoutSize(child.x() + marginBeforeForChild(*child.layoutObject()),
            child.y() + child.layoutObject()->marginTop());
    }

    LayoutPoint flipFloatForWritingModeForChild(const FloatingObject&, const LayoutPoint&) const;

    virtual const char* name() const override { return "LayoutBlockFlow"; }

    FloatingObject* insertFloatingObject(LayoutBox&);

    // Called from lineWidth, to position the floats added in the last line.
    // Returns true if and only if it has positioned any floats.
    bool positionNewFloats(LineWidth* = nullptr);

    bool positionNewFloatOnLine(FloatingObject& newFloat, FloatingObject* lastFloatFromPreviousLine, LineInfo&, LineWidth&);

    LayoutUnit nextFloatLogicalBottomBelow(LayoutUnit, ShapeOutsideFloatOffsetMode = ShapeOutsideFloatMarginBoxOffset) const;

    FloatingObject* lastFloatFromPreviousLine() const
    {
        return containsFloats() ? m_floatingObjects->set().last().get() : nullptr;
    }

protected:
    void rebuildFloatsFromIntruding();
    void layoutInlineChildren(bool relayoutChildren, LayoutUnit& paintInvalidationLogicalTop, LayoutUnit& paintInvalidationLogicalBottom, LayoutUnit afterEdge);
    void addLowestFloatFromChildren(LayoutBlockFlow*);

    void createFloatingObjects();

    virtual void styleWillChange(StyleDifference, const ComputedStyle& newStyle) override;
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    void updateBlockChildDirtyBitsBeforeLayout(bool relayoutChildren, LayoutBox&);

    void addOverflowFromFloats();

    LayoutUnit logicalRightOffsetForLine(LayoutUnit logicalTop, LayoutUnit fixedOffset, bool applyTextIndent, LayoutUnit logicalHeight = 0) const
    {
        return adjustLogicalRightOffsetForLine(logicalRightFloatOffsetForLine(logicalTop, fixedOffset, logicalHeight), applyTextIndent);
    }
    LayoutUnit logicalLeftOffsetForLine(LayoutUnit logicalTop, LayoutUnit fixedOffset, bool applyTextIndent, LayoutUnit logicalHeight = 0) const
    {
        return adjustLogicalLeftOffsetForLine(logicalLeftFloatOffsetForLine(logicalTop, fixedOffset, logicalHeight), applyTextIndent);
    }

    virtual LayoutObject* layoutSpecialExcludedChild(bool /*relayoutChildren*/, SubtreeLayoutScope&);
    virtual bool updateLogicalWidthAndColumnWidth() override;

    void setLogicalLeftForChild(LayoutBox& child, LayoutUnit logicalLeft);
    void setLogicalTopForChild(LayoutBox& child, LayoutUnit logicalTop);
    void determineLogicalLeftPositionForChild(LayoutBox& child);

private:
    bool layoutBlockFlow(bool relayoutChildren, LayoutUnit& pageLogicalHeight, SubtreeLayoutScope&);
    void layoutBlockChildren(bool relayoutChildren, SubtreeLayoutScope&, LayoutUnit beforeEdge, LayoutUnit afterEdge);

    void layoutBlockChild(LayoutBox& child, MarginInfo&, LayoutUnit& previousFloatLogicalBottom);
    void adjustPositionedBlock(LayoutBox& child, const MarginInfo&);
    void adjustFloatingBlock(const MarginInfo&);

    LayoutPoint computeLogicalLocationForFloat(const FloatingObject&, LayoutUnit logicalTopOffset) const;

    void removeFloatingObject(LayoutBox*);
    void removeFloatingObjectsBelow(FloatingObject*, int logicalOffset);

    LayoutUnit getClearDelta(LayoutBox* child, LayoutUnit yPos);

    bool hasOverhangingFloats() { return parent() && containsFloats() && lowestFloatLogicalBottom() > logicalHeight(); }
    bool hasOverhangingFloat(LayoutBox*);
    void addIntrudingFloats(LayoutBlockFlow* prev, LayoutUnit xoffset, LayoutUnit yoffset);
    void addOverhangingFloats(LayoutBlockFlow* child, bool makeChildPaintOtherFloats);

    LayoutUnit lowestFloatLogicalBottom(FloatingObject::Type = FloatingObject::FloatLeftRight) const;

    virtual bool hitTestFloats(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset) override final;

    virtual void invalidatePaintForOverhangingFloats(bool paintAllDescendants) override final;
    virtual void invalidatePaintForOverflow() override final;
    virtual void paintFloats(const PaintInfo&, const LayoutPoint&, bool preservePhase = false) override final;
    virtual void paintSelection(const PaintInfo&, const LayoutPoint&) override final;
    virtual void clipOutFloatingObjects(const LayoutBlock*, ClipScope&, const LayoutPoint&, const LayoutSize&) const;
    void clearFloats(EClear);

    LayoutUnit logicalRightFloatOffsetForLine(LayoutUnit logicalTop, LayoutUnit fixedOffset, LayoutUnit logicalHeight) const;
    LayoutUnit logicalLeftFloatOffsetForLine(LayoutUnit logicalTop, LayoutUnit fixedOffset, LayoutUnit logicalHeight) const;

    LayoutUnit logicalRightOffsetForPositioningFloat(LayoutUnit logicalTop, LayoutUnit fixedOffset, bool applyTextIndent, LayoutUnit* heightRemaining) const;
    LayoutUnit logicalLeftOffsetForPositioningFloat(LayoutUnit logicalTop, LayoutUnit fixedOffset, bool applyTextIndent, LayoutUnit* heightRemaining) const;

    LayoutUnit adjustLogicalRightOffsetForLine(LayoutUnit offsetFromFloats, bool applyTextIndent) const;
    LayoutUnit adjustLogicalLeftOffsetForLine(LayoutUnit offsetFromFloats, bool applyTextIndent) const;

    virtual RootInlineBox* createRootInlineBox(); // Subclassed by SVG

    bool isPagedOverflow(const ComputedStyle&);

    enum FlowThreadType {
        NoFlowThread,
        MultiColumnFlowThread,
        PagedFlowThread
    };

    FlowThreadType flowThreadType(const ComputedStyle&);

    LayoutMultiColumnFlowThread* createMultiColumnFlowThread(FlowThreadType);
    void createOrDestroyMultiColumnFlowThreadIfNeeded(const ComputedStyle* oldStyle);

    void updateLogicalWidthForAlignment(const ETextAlign&, const RootInlineBox*, BidiRun* trailingSpaceRun, LayoutUnit& logicalLeft, LayoutUnit& totalLogicalWidth, LayoutUnit& availableLogicalWidth, unsigned expansionOpportunityCount);
    void checkForPaginationLogicalHeightChange(LayoutUnit& pageLogicalHeight, bool& pageLogicalHeightChanged, bool& hasSpecifiedPageLogicalHeight);

    bool shouldBreakAtLineToAvoidWidow() const { return m_rareData && m_rareData->m_lineBreakToAvoidWidow >= 0; }
    void clearShouldBreakAtLineToAvoidWidow() const;
    int lineBreakToAvoidWidow() const { return m_rareData ? m_rareData->m_lineBreakToAvoidWidow : -1; }
    void setBreakAtLineToAvoidWidow(int);
    void clearDidBreakAtLineToAvoidWidow();
    void setDidBreakAtLineToAvoidWidow();
    bool didBreakAtLineToAvoidWidow() const { return m_rareData && m_rareData->m_didBreakAtLineToAvoidWidow; }

public:
    struct FloatWithRect {
        FloatWithRect(LayoutBox* f)
            : object(f)
            , rect(f->frameRect())
            , everHadLayout(f->everHadLayout())
        {
            rect.expand(f->marginBoxOutsets());
        }

        LayoutBox* object;
        LayoutRect rect;
        bool everHadLayout;
    };

    class MarginValues {
    public:
        MarginValues(LayoutUnit beforePos, LayoutUnit beforeNeg, LayoutUnit afterPos, LayoutUnit afterNeg)
            : m_positiveMarginBefore(beforePos)
            , m_negativeMarginBefore(beforeNeg)
            , m_positiveMarginAfter(afterPos)
            , m_negativeMarginAfter(afterNeg)
        { }

        LayoutUnit positiveMarginBefore() const { return m_positiveMarginBefore; }
        LayoutUnit negativeMarginBefore() const { return m_negativeMarginBefore; }
        LayoutUnit positiveMarginAfter() const { return m_positiveMarginAfter; }
        LayoutUnit negativeMarginAfter() const { return m_negativeMarginAfter; }

        void setPositiveMarginBefore(LayoutUnit pos) { m_positiveMarginBefore = pos; }
        void setNegativeMarginBefore(LayoutUnit neg) { m_negativeMarginBefore = neg; }
        void setPositiveMarginAfter(LayoutUnit pos) { m_positiveMarginAfter = pos; }
        void setNegativeMarginAfter(LayoutUnit neg) { m_negativeMarginAfter = neg; }

    private:
        LayoutUnit m_positiveMarginBefore;
        LayoutUnit m_negativeMarginBefore;
        LayoutUnit m_positiveMarginAfter;
        LayoutUnit m_negativeMarginAfter;
    };
    MarginValues marginValuesForChild(LayoutBox& child) const;

    // Allocated only when some of these fields have non-default values
    struct LayoutBlockFlowRareData {
        WTF_MAKE_NONCOPYABLE(LayoutBlockFlowRareData); WTF_MAKE_FAST_ALLOCATED(LayoutBlockFlowRareData);
    public:
        LayoutBlockFlowRareData(const LayoutBlockFlow* block)
            : m_margins(positiveMarginBeforeDefault(block), negativeMarginBeforeDefault(block), positiveMarginAfterDefault(block), negativeMarginAfterDefault(block))
            , m_paginationStrut(0)
            , m_multiColumnFlowThread(nullptr)
            , m_lineBreakToAvoidWidow(-1)
            , m_didBreakAtLineToAvoidWidow(false)
            , m_discardMarginBefore(false)
            , m_discardMarginAfter(false)
        {
        }

        static LayoutUnit positiveMarginBeforeDefault(const LayoutBlockFlow* block)
        {
            return std::max<LayoutUnit>(block->marginBefore(), 0);
        }
        static LayoutUnit negativeMarginBeforeDefault(const LayoutBlockFlow* block)
        {
            return std::max<LayoutUnit>(-block->marginBefore(), 0);
        }
        static LayoutUnit positiveMarginAfterDefault(const LayoutBlockFlow* block)
        {
            return std::max<LayoutUnit>(block->marginAfter(), 0);
        }
        static LayoutUnit negativeMarginAfterDefault(const LayoutBlockFlow* block)
        {
            return std::max<LayoutUnit>(-block->marginAfter(), 0);
        }

        MarginValues m_margins;
        LayoutUnit m_paginationStrut;

        LayoutMultiColumnFlowThread* m_multiColumnFlowThread;

        int m_lineBreakToAvoidWidow;
        bool m_didBreakAtLineToAvoidWidow : 1;
        bool m_discardMarginBefore : 1;
        bool m_discardMarginAfter : 1;
    };

    FloatingObjects* floatingObjects() { return m_floatingObjects.get(); }


protected:
    LayoutUnit maxPositiveMarginBefore() const { return m_rareData ? m_rareData->m_margins.positiveMarginBefore() : LayoutBlockFlowRareData::positiveMarginBeforeDefault(this); }
    LayoutUnit maxNegativeMarginBefore() const { return m_rareData ? m_rareData->m_margins.negativeMarginBefore() : LayoutBlockFlowRareData::negativeMarginBeforeDefault(this); }
    LayoutUnit maxPositiveMarginAfter() const { return m_rareData ? m_rareData->m_margins.positiveMarginAfter() : LayoutBlockFlowRareData::positiveMarginAfterDefault(this); }
    LayoutUnit maxNegativeMarginAfter() const { return m_rareData ? m_rareData->m_margins.negativeMarginAfter() : LayoutBlockFlowRareData::negativeMarginAfterDefault(this); }

    void setMaxMarginBeforeValues(LayoutUnit pos, LayoutUnit neg);
    void setMaxMarginAfterValues(LayoutUnit pos, LayoutUnit neg);

    void setMustDiscardMarginBefore(bool = true);
    void setMustDiscardMarginAfter(bool = true);

    bool mustDiscardMarginBefore() const;
    bool mustDiscardMarginAfter() const;

    bool mustDiscardMarginBeforeForChild(const LayoutBox&) const;
    bool mustDiscardMarginAfterForChild(const LayoutBox&) const;

    bool mustSeparateMarginBeforeForChild(const LayoutBox&) const;
    bool mustSeparateMarginAfterForChild(const LayoutBox&) const;

    void initMaxMarginValues()
    {
        if (m_rareData) {
            m_rareData->m_margins = MarginValues(LayoutBlockFlowRareData::positiveMarginBeforeDefault(this) , LayoutBlockFlowRareData::negativeMarginBeforeDefault(this),
                LayoutBlockFlowRareData::positiveMarginAfterDefault(this), LayoutBlockFlowRareData::negativeMarginAfterDefault(this));

            m_rareData->m_discardMarginBefore = false;
            m_rareData->m_discardMarginAfter = false;
        }
    }

    virtual ETextAlign textAlignmentForLine(bool endsWithSoftBreak) const;
private:
    virtual LayoutUnit collapsedMarginBefore() const override final { return maxPositiveMarginBefore() - maxNegativeMarginBefore(); }
    virtual LayoutUnit collapsedMarginAfter() const override final { return maxPositiveMarginAfter() - maxNegativeMarginAfter(); }

    LayoutUnit collapseMargins(LayoutBox& child, MarginInfo&, bool childIsSelfCollapsing);
    LayoutUnit clearFloatsIfNeeded(LayoutBox& child, MarginInfo&, LayoutUnit oldTopPosMargin, LayoutUnit oldTopNegMargin, LayoutUnit yPos, bool childIsSelfCollapsing);
    LayoutUnit estimateLogicalTopPosition(LayoutBox& child, const MarginInfo&, LayoutUnit& estimateWithoutPagination);
    void marginBeforeEstimateForChild(LayoutBox&, LayoutUnit&, LayoutUnit&, bool&) const;
    void handleAfterSideOfBlock(LayoutBox* lastChild, LayoutUnit top, LayoutUnit bottom, MarginInfo&);
    void setCollapsedBottomMargin(const MarginInfo&);

    LayoutUnit applyBeforeBreak(LayoutBox& child, LayoutUnit logicalOffset); // If the child has a before break, then return a new yPos that shifts to the top of the next page/column.
    LayoutUnit applyAfterBreak(LayoutBox& child, LayoutUnit logicalOffset, MarginInfo&); // If the child has an after break, then return a new offset that shifts to the top of the next page/column.

    LayoutUnit adjustBlockChildForPagination(LayoutUnit logicalTopAfterClear, LayoutUnit estimateWithoutPagination, LayoutBox& child, bool atBeforeSideOfBlock);
    // Computes a deltaOffset value that put a line at the top of the next page if it doesn't fit on the current page.
    void adjustLinePositionForPagination(RootInlineBox&, LayoutUnit& deltaOffset);
    // If the child is unsplittable and can't fit on the current page, return the top of the next page/column.
    LayoutUnit adjustForUnsplittableChild(LayoutBox&, LayoutUnit logicalOffset, bool includeMargins = false);

    // Used to store state between styleWillChange and styleDidChange
    static bool s_canPropagateFloatIntoSibling;

    LayoutBlockFlowRareData& ensureRareData();

    LayoutUnit m_paintInvalidationLogicalTop;
    LayoutUnit m_paintInvalidationLogicalBottom;

    virtual bool isSelfCollapsingBlock() const override;

protected:
    OwnPtr<LayoutBlockFlowRareData> m_rareData;
    OwnPtr<FloatingObjects> m_floatingObjects;

    friend class MarginInfo;
    friend class LineBreaker;
    friend class LineWidth; // needs to know FloatingObject

// FIXME-BLOCKFLOW: These methods have implementations in
// LayoutBlockFlowLine. They should be moved to the proper header once the
// line layout code is separated from LayoutBlock and LayoutBlockFlow.
// START METHODS DEFINED IN LayoutBlockFlowLine
private:
    InlineFlowBox* createLineBoxes(LayoutObject*, const LineInfo&, InlineBox* childBox);
    RootInlineBox* constructLine(BidiRunList<BidiRun>&, const LineInfo&);
    void setMarginsForRubyRun(BidiRun*, LayoutRubyRun*, LayoutObject*, const LineInfo&);
    void computeInlineDirectionPositionsForLine(RootInlineBox*, const LineInfo&, BidiRun* firstRun, BidiRun* trailingSpaceRun, bool reachedEnd, GlyphOverflowAndFallbackFontsMap&, VerticalPositionCache&, WordMeasurements&);
    BidiRun* computeInlineDirectionPositionsForSegment(RootInlineBox*, const LineInfo&, ETextAlign, LayoutUnit& logicalLeft,
        LayoutUnit& availableLogicalWidth, BidiRun* firstRun, BidiRun* trailingSpaceRun, GlyphOverflowAndFallbackFontsMap& textBoxDataMap, VerticalPositionCache&, WordMeasurements&);
    void computeBlockDirectionPositionsForLine(RootInlineBox*, BidiRun*, GlyphOverflowAndFallbackFontsMap&, VerticalPositionCache&);
    void appendFloatingObjectToLastLine(FloatingObject&);
    // Helper function for layoutInlineChildren()
    RootInlineBox* createLineBoxesFromBidiRuns(unsigned bidiLevel, BidiRunList<BidiRun>&, const InlineIterator& end, LineInfo&, VerticalPositionCache&, BidiRun* trailingSpaceRun, WordMeasurements&);
    void layoutRunsAndFloats(LineLayoutState&);
    const InlineIterator& restartLayoutRunsAndFloatsInRange(LayoutUnit oldLogicalHeight, LayoutUnit newLogicalHeight,  FloatingObject* lastFloatFromPreviousLine, InlineBidiResolver&,  const InlineIterator&);
    void layoutRunsAndFloatsInRange(LineLayoutState&, InlineBidiResolver&,
        const InlineIterator& cleanLineStart, const BidiStatus& cleanLineBidiStatus);
    void linkToEndLineIfNeeded(LineLayoutState&);
    static void markDirtyFloatsForPaintInvalidation(Vector<FloatWithRect>& floats);
    void checkFloatsInCleanLine(RootInlineBox*, Vector<FloatWithRect>&, size_t& floatIndex, bool& encounteredNewFloat, bool& dirtiedByFloat);
    RootInlineBox* determineStartPosition(LineLayoutState&, InlineBidiResolver&);
    void determineEndPosition(LineLayoutState&, RootInlineBox* startBox, InlineIterator& cleanLineStart, BidiStatus& cleanLineBidiStatus);
    bool lineBoxHasBRWithClearance(RootInlineBox*);
    bool checkPaginationAndFloatsAtEndLine(LineLayoutState&);
    bool matchedEndLine(LineLayoutState&, const InlineBidiResolver&, const InlineIterator& endLineStart, const BidiStatus& endLineStatus);
    void deleteEllipsisLineBoxes();
    void checkLinesForTextOverflow();
    // Positions new floats and also adjust all floats encountered on the line if any of them
    // have to move to the next page/column.
    void positionDialog();

// END METHODS DEFINED IN LayoutBlockFlowLine

};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutBlockFlow, isLayoutBlockFlow());

} // namespace blink

#endif // LayoutBlockFlow_h
