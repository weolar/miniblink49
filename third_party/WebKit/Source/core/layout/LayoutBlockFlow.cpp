/*
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

#include "config.h"
#include "core/layout/LayoutBlockFlow.h"

#include "core/dom/AXObjectCache.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLDialogElement.h"
#include "core/layout/HitTestLocation.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutFlowThread.h"
#include "core/layout/LayoutMultiColumnFlowThread.h"
#include "core/layout/LayoutMultiColumnSpannerPlaceholder.h"
#include "core/layout/LayoutPagedFlowThread.h"
#include "core/layout/LayoutText.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextAutosizer.h"
#include "core/layout/line/LineBreaker.h"
#include "core/layout/line/LineWidth.h"
#include "core/layout/shapes/ShapeOutsideInfo.h"
#include "core/paint/BlockFlowPainter.h"
#include "core/paint/ClipScope.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/geometry/TransformState.h"
#include "platform/text/BidiTextRun.h"

namespace blink {

bool LayoutBlockFlow::s_canPropagateFloatIntoSibling = false;

struct SameSizeAsMarginInfo {
    uint16_t bitfields;
    LayoutUnit margins[2];
};

static_assert(sizeof(LayoutBlockFlow::MarginValues) == sizeof(LayoutUnit[4]), "MarginValues should stay small");

class MarginInfo {
    // Collapsing flags for whether we can collapse our margins with our children's margins.
    bool m_canCollapseWithChildren : 1;
    bool m_canCollapseMarginBeforeWithChildren : 1;
    bool m_canCollapseMarginAfterWithChildren : 1;
    bool m_canCollapseMarginAfterWithLastChild: 1;

    // Whether or not we are a quirky container, i.e., do we collapse away top and bottom
    // margins in our container. Table cells and the body are the common examples. We
    // also have a custom style property for Safari RSS to deal with TypePad blog articles.
    bool m_quirkContainer : 1;

    // This flag tracks whether we are still looking at child margins that can all collapse together at the beginning of a block.
    // They may or may not collapse with the top margin of the block (|m_canCollapseTopWithChildren| tells us that), but they will
    // always be collapsing with one another. This variable can remain set to true through multiple iterations
    // as long as we keep encountering self-collapsing blocks.
    bool m_atBeforeSideOfBlock : 1;

    // This flag is set when we know we're examining bottom margins and we know we're at the bottom of the block.
    bool m_atAfterSideOfBlock : 1;

    // These variables are used to detect quirky margins that we need to collapse away (in table cells
    // and in the body element).
    bool m_hasMarginBeforeQuirk : 1;
    bool m_hasMarginAfterQuirk : 1;
    bool m_determinedMarginBeforeQuirk : 1;

    bool m_discardMargin : 1;
    bool m_lastChildIsSelfCollapsingBlockWithClearance : 1;

    // These flags track the previous maximal positive and negative margins.
    LayoutUnit m_positiveMargin;
    LayoutUnit m_negativeMargin;

public:
    MarginInfo(LayoutBlockFlow*, LayoutUnit beforeBorderPadding, LayoutUnit afterBorderPadding);

    void setAtBeforeSideOfBlock(bool b) { m_atBeforeSideOfBlock = b; }
    void setAtAfterSideOfBlock(bool b) { m_atAfterSideOfBlock = b; }
    void clearMargin()
    {
        m_positiveMargin = 0;
        m_negativeMargin = 0;
    }
    void setHasMarginBeforeQuirk(bool b) { m_hasMarginBeforeQuirk = b; }
    void setHasMarginAfterQuirk(bool b) { m_hasMarginAfterQuirk = b; }
    void setDeterminedMarginBeforeQuirk(bool b) { m_determinedMarginBeforeQuirk = b; }
    void setPositiveMargin(LayoutUnit p) { ASSERT(!m_discardMargin); m_positiveMargin = p; }
    void setNegativeMargin(LayoutUnit n) { ASSERT(!m_discardMargin); m_negativeMargin = n; }
    void setPositiveMarginIfLarger(LayoutUnit p)
    {
        ASSERT(!m_discardMargin);
        if (p > m_positiveMargin)
            m_positiveMargin = p;
    }
    void setNegativeMarginIfLarger(LayoutUnit n)
    {
        ASSERT(!m_discardMargin);
        if (n > m_negativeMargin)
            m_negativeMargin = n;
    }

    void setMargin(LayoutUnit p, LayoutUnit n) { ASSERT(!m_discardMargin); m_positiveMargin = p; m_negativeMargin = n; }
    void setCanCollapseMarginAfterWithChildren(bool collapse) { m_canCollapseMarginAfterWithChildren = collapse; }
    void setCanCollapseMarginAfterWithLastChild(bool collapse) { m_canCollapseMarginAfterWithLastChild = collapse; }
    void setDiscardMargin(bool value) { m_discardMargin = value; }

    bool atBeforeSideOfBlock() const { return m_atBeforeSideOfBlock; }
    bool canCollapseWithMarginBefore() const { return m_atBeforeSideOfBlock && m_canCollapseMarginBeforeWithChildren; }
    bool canCollapseWithMarginAfter() const { return m_atAfterSideOfBlock && m_canCollapseMarginAfterWithChildren; }
    bool canCollapseMarginBeforeWithChildren() const { return m_canCollapseMarginBeforeWithChildren; }
    bool canCollapseMarginAfterWithChildren() const { return m_canCollapseMarginAfterWithChildren; }
    bool canCollapseMarginAfterWithLastChild() const { return m_canCollapseMarginAfterWithLastChild; }
    bool quirkContainer() const { return m_quirkContainer; }
    bool determinedMarginBeforeQuirk() const { return m_determinedMarginBeforeQuirk; }
    bool hasMarginBeforeQuirk() const { return m_hasMarginBeforeQuirk; }
    bool hasMarginAfterQuirk() const { return m_hasMarginAfterQuirk; }
    LayoutUnit positiveMargin() const { return m_positiveMargin; }
    LayoutUnit negativeMargin() const { return m_negativeMargin; }
    bool discardMargin() const { return m_discardMargin; }
    LayoutUnit margin() const { return m_positiveMargin - m_negativeMargin; }
    void setLastChildIsSelfCollapsingBlockWithClearance(bool value) { m_lastChildIsSelfCollapsingBlockWithClearance = value; }
    bool lastChildIsSelfCollapsingBlockWithClearance() const { return m_lastChildIsSelfCollapsingBlockWithClearance; }
};
static bool inNormalFlow(LayoutBox* child)
{
    LayoutBlock* curr = child->containingBlock();
    LayoutView* layoutView = child->view();
    while (curr && curr != layoutView) {
        if (curr->isLayoutFlowThread())
            return true;
        if (curr->isFloatingOrOutOfFlowPositioned())
            return false;
        curr = curr->containingBlock();
    }
    return true;
}

LayoutBlockFlow::LayoutBlockFlow(ContainerNode* node)
    : LayoutBlock(node)
{
    static_assert(sizeof(MarginInfo) == sizeof(SameSizeAsMarginInfo), "MarginInfo should stay small");
    setChildrenInline(true);
}

LayoutBlockFlow::~LayoutBlockFlow()
{
}

LayoutBlockFlow* LayoutBlockFlow::createAnonymous(Document* document)
{
    LayoutBlockFlow* layoutBlockFlow = new LayoutBlockFlow(nullptr);
    layoutBlockFlow->setDocumentForAnonymous(document);
    return layoutBlockFlow;
}

LayoutObject* LayoutBlockFlow::layoutSpecialExcludedChild(bool relayoutChildren, SubtreeLayoutScope& layoutScope)
{
    LayoutMultiColumnFlowThread* flowThread = multiColumnFlowThread();
    if (!flowThread)
        return nullptr;
    setLogicalTopForChild(*flowThread, borderBefore() + paddingBefore());
    flowThread->layoutColumns(relayoutChildren, layoutScope);
    determineLogicalLeftPositionForChild(*flowThread);
    return flowThread;
}

bool LayoutBlockFlow::updateLogicalWidthAndColumnWidth()
{
    bool relayoutChildren = LayoutBlock::updateLogicalWidthAndColumnWidth();
    if (LayoutMultiColumnFlowThread* flowThread = multiColumnFlowThread()) {
        if (flowThread->needsNewWidth())
            return true;
    }
    return relayoutChildren;
}

void LayoutBlockFlow::checkForPaginationLogicalHeightChange(LayoutUnit& pageLogicalHeight, bool& pageLogicalHeightChanged, bool& hasSpecifiedPageLogicalHeight)
{
    if (LayoutMultiColumnFlowThread* flowThread = multiColumnFlowThread()) {
        LogicalExtentComputedValues computedValues;
        computeLogicalHeight(LayoutUnit(), logicalTop(), computedValues);
        LayoutUnit columnHeight = computedValues.m_extent - borderAndPaddingLogicalHeight() - scrollbarLogicalHeight();
        pageLogicalHeightChanged = columnHeight != flowThread->columnHeightAvailable();
        flowThread->setColumnHeightAvailable(std::max<LayoutUnit>(columnHeight, 0));
    } else if (isLayoutFlowThread()) {
        LayoutFlowThread* flowThread = toLayoutFlowThread(this);

        // FIXME: This is a hack to always make sure we have a page logical height, if said height
        // is known. The page logical height thing in LayoutState is meaningless for flow
        // thread-based pagination (page height isn't necessarily uniform throughout the flow
        // thread), but as long as it is used universally as a means to determine whether page
        // height is known or not, we need this. Page height is unknown when column balancing is
        // enabled and flow thread height is still unknown (i.e. during the first layout pass). When
        // it's unknown, we need to prevent the pagination code from assuming page breaks everywhere
        // and thereby eating every top margin. It should be trivial to clean up and get rid of this
        // hack once the old multicol implementation is gone.
        pageLogicalHeight = flowThread->isPageLogicalHeightKnown() ? LayoutUnit(1) : LayoutUnit();

        pageLogicalHeightChanged = flowThread->pageLogicalSizeChanged();
    }
}

void LayoutBlockFlow::setBreakAtLineToAvoidWidow(int lineToBreak)
{
    ASSERT(lineToBreak >= 0);
    ensureRareData();
    ASSERT(!m_rareData->m_didBreakAtLineToAvoidWidow);
    m_rareData->m_lineBreakToAvoidWidow = lineToBreak;
}

void LayoutBlockFlow::setDidBreakAtLineToAvoidWidow()
{
    ASSERT(!shouldBreakAtLineToAvoidWidow());

    // This function should be called only after a break was applied to avoid widows
    // so assert |m_rareData| exists.
    ASSERT(m_rareData);

    m_rareData->m_didBreakAtLineToAvoidWidow = true;
}

void LayoutBlockFlow::clearDidBreakAtLineToAvoidWidow()
{
    if (!m_rareData)
        return;

    m_rareData->m_didBreakAtLineToAvoidWidow = false;
}

void LayoutBlockFlow::clearShouldBreakAtLineToAvoidWidow() const
{
    ASSERT(shouldBreakAtLineToAvoidWidow());
    if (!m_rareData)
        return;

    m_rareData->m_lineBreakToAvoidWidow = -1;
}

bool LayoutBlockFlow::isSelfCollapsingBlock() const
{
    m_hasOnlySelfCollapsingChildren = LayoutBlock::isSelfCollapsingBlock();
    return m_hasOnlySelfCollapsingChildren;
}

void LayoutBlockFlow::layoutBlock(bool relayoutChildren)
{
    ASSERT(needsLayout());
    ASSERT(isInlineBlockOrInlineTable() || !isInline());

    // If we are self-collapsing with self-collapsing descendants this will get set to save us burrowing through our
    // descendants every time in |isSelfCollapsingBlock|. We reset it here so that |isSelfCollapsingBlock| attempts to burrow
    // at least once and so that it always gives a reliable result reflecting the latest layout.
    m_hasOnlySelfCollapsingChildren = false;

    if (!relayoutChildren && simplifiedLayout())
        return;

    LayoutAnalyzer::BlockScope analyzer(*this);
    SubtreeLayoutScope layoutScope(*this);

    LayoutUnit previousHeight = logicalHeight();
    LayoutUnit oldLeft = logicalLeft();
    
    // Multiple passes might be required for column based layout.
    // The number of passes could be as high as the number of columns.
    bool done = false;
    LayoutUnit pageLogicalHeight = 0;
    while (!done)
        done = layoutBlockFlow(relayoutChildren, pageLogicalHeight, layoutScope);

    LayoutView* layoutView = view();
    if (layoutView->layoutState()->pageLogicalHeight())
        setPageLogicalOffset(layoutView->layoutState()->pageLogicalOffset(*this, logicalTop()));

    if (logicalHeight() != previousHeight || isDocumentElement())
        relayoutChildren = true;

    PositionedLayoutBehavior behavior = DefaultLayout;
    if (oldLeft != logicalLeft())
        behavior = ForcedLayoutAfterContainingBlockMoved;
    layoutPositionedObjects(relayoutChildren, behavior);

    updateLayerTransformAfterLayout();

    // Update our scroll information if we're overflow:auto/scroll/hidden now that we know if
    // we overflow or not.
    updateScrollInfoAfterLayout();

    if (m_paintInvalidationLogicalTop != m_paintInvalidationLogicalBottom) {
        bool hasVisibleContent = style()->visibility() == VISIBLE;
        if (!hasVisibleContent) {
            DeprecatedPaintLayer* layer = enclosingLayer();
            layer->updateDescendantDependentFlags();
            hasVisibleContent = layer->hasVisibleContent();
        }
        if (hasVisibleContent)
            setShouldInvalidateOverflowForPaint(true);
    }

    if (isHTMLDialogElement(node()) && isOutOfFlowPositioned())
        positionDialog();

    clearNeedsLayout();
}

inline bool LayoutBlockFlow::layoutBlockFlow(bool relayoutChildren, LayoutUnit &pageLogicalHeight, SubtreeLayoutScope& layoutScope)
{
    LayoutUnit oldLeft = logicalLeft();
    bool logicalWidthChanged = updateLogicalWidthAndColumnWidth();
    relayoutChildren |= logicalWidthChanged;

    rebuildFloatsFromIntruding();

    bool pageLogicalHeightChanged = false;
    bool hasSpecifiedPageLogicalHeight = false;
    checkForPaginationLogicalHeightChange(pageLogicalHeight, pageLogicalHeightChanged, hasSpecifiedPageLogicalHeight);
    if (pageLogicalHeightChanged)
        relayoutChildren = true;

    LayoutState state(*this, locationOffset(), pageLogicalHeight, pageLogicalHeightChanged, logicalWidthChanged);

    // We use four values, maxTopPos, maxTopNeg, maxBottomPos, and maxBottomNeg, to track
    // our current maximal positive and negative margins. These values are used when we
    // are collapsed with adjacent blocks, so for example, if you have block A and B
    // collapsing together, then you'd take the maximal positive margin from both A and B
    // and subtract it from the maximal negative margin from both A and B to get the
    // true collapsed margin. This algorithm is recursive, so when we finish layout()
    // our block knows its current maximal positive/negative values.
    //
    // Start out by setting our margin values to our current margins. Table cells have
    // no margins, so we don't fill in the values for table cells.
    if (!isTableCell()) {
        initMaxMarginValues();
        setHasMarginBeforeQuirk(style()->hasMarginBeforeQuirk());
        setHasMarginAfterQuirk(style()->hasMarginAfterQuirk());
        setPaginationStrut(0);
    }

    LayoutUnit beforeEdge = borderBefore() + paddingBefore();
    LayoutUnit afterEdge = borderAfter() + paddingAfter() + scrollbarLogicalHeight();
    LayoutUnit previousHeight = logicalHeight();
    setLogicalHeight(beforeEdge);

    m_paintInvalidationLogicalTop = 0;
    m_paintInvalidationLogicalBottom = 0;
    if (!firstChild() && !isAnonymousBlock())
        setChildrenInline(true);

    TextAutosizer::LayoutScope textAutosizerLayoutScope(this);

    if (childrenInline())
        layoutInlineChildren(relayoutChildren, m_paintInvalidationLogicalTop, m_paintInvalidationLogicalBottom, afterEdge);
    else
        layoutBlockChildren(relayoutChildren, layoutScope, beforeEdge, afterEdge);

    // Expand our intrinsic height to encompass floats.
    if (lowestFloatLogicalBottom() > (logicalHeight() - afterEdge) && createsNewFormattingContext())
        setLogicalHeight(lowestFloatLogicalBottom() + afterEdge);

    if (LayoutMultiColumnFlowThread* flowThread = multiColumnFlowThread()) {
        if (flowThread->recalculateColumnHeights()) {
            setChildNeedsLayout(MarkOnlyThis);
            return false;
        }
    }

    if (shouldBreakAtLineToAvoidWidow()) {
        setEverHadLayout(true);
        return false;
    }

    // Calculate our new height.
    LayoutUnit oldHeight = logicalHeight();
    LayoutUnit oldClientAfterEdge = clientLogicalBottom();

    updateLogicalHeight();
    LayoutUnit newHeight = logicalHeight();
    if (!childrenInline()) {
        LayoutBlockFlow* lowestBlock = nullptr;
        bool addedOverhangingFloats = false;
        // One of our children's floats may have become an overhanging float for us.
        for (LayoutObject* child = lastChild(); child; child = child->previousSibling()) {
            // TODO(robhogan): We should exclude blocks that create formatting contexts, not just out of flow or floating blocks.
            if (child->isLayoutBlockFlow() && !child->isFloatingOrOutOfFlowPositioned()) {
                LayoutBlockFlow* block = toLayoutBlockFlow(child);
                lowestBlock = block;
                if (oldHeight <= newHeight || block->lowestFloatLogicalBottom() + block->logicalTop() <= newHeight)
                    break;
                addOverhangingFloats(block, false);
                addedOverhangingFloats = true;
            }
        }
        if (!addedOverhangingFloats)
            addLowestFloatFromChildren(lowestBlock);
    }

    bool heightChanged = (previousHeight != newHeight);
    if (heightChanged)
        relayoutChildren = true;

    layoutPositionedObjects(relayoutChildren || isDocumentElement(), oldLeft != logicalLeft() ? ForcedLayoutAfterContainingBlockMoved : DefaultLayout);

    // Add overflow from children (unless we're multi-column, since in that case all our child overflow is clipped anyway).
    computeOverflow(oldClientAfterEdge);

    m_descendantsWithFloatsMarkedForLayout = false;
    return true;
}

void LayoutBlockFlow::addLowestFloatFromChildren(LayoutBlockFlow* block)
{
    // TODO(robhogan): Make createsNewFormattingContext an ASSERT.
    if (!block || !block->containsFloats() || block->createsNewFormattingContext())
        return;

    FloatingObject* floatingObject = block->m_floatingObjects->lowestFloatingObject();
    if (!floatingObject || containsFloat(floatingObject->layoutObject()))
        return;

    LayoutSize offset(-block->logicalLeft(), -block->logicalTop());
    if (!isHorizontalWritingMode())
        offset = offset.transposedSize();

    if (!m_floatingObjects)
        createFloatingObjects();
    FloatingObject* newFloatingObject = m_floatingObjects->add(floatingObject->copyToNewContainer(offset, false, true));
    newFloatingObject->setIsLowestNonOverhangingFloatInChild(true);
}

void LayoutBlockFlow::determineLogicalLeftPositionForChild(LayoutBox& child)
{
    LayoutUnit startPosition = borderStart() + paddingStart();
    LayoutUnit initialStartPosition = startPosition;
    if (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        startPosition -= verticalScrollbarWidth();
    LayoutUnit totalAvailableLogicalWidth = borderAndPaddingLogicalWidth() + availableLogicalWidth();

    LayoutUnit childMarginStart = marginStartForChild(child);
    LayoutUnit newPosition = startPosition + childMarginStart;

    LayoutUnit positionToAvoidFloats;
    if (child.avoidsFloats() && containsFloats() && !flowThreadContainingBlock())
        positionToAvoidFloats = startOffsetForLine(logicalTopForChild(child), false, logicalHeightForChild(child));

    // If the child has an offset from the content edge to avoid floats then use that, otherwise let any negative
    // margin pull it back over the content edge or any positive margin push it out.
    // If the child is being centred then the margin calculated to do that has factored in any offset required to
    // avoid floats, so use it if necessary.
    if (style()->textAlign() == WEBKIT_CENTER || child.style()->marginStartUsing(style()).isAuto())
        newPosition = std::max(newPosition, positionToAvoidFloats + childMarginStart);
    else if (positionToAvoidFloats > initialStartPosition)
        newPosition = std::max(newPosition, positionToAvoidFloats);

    setLogicalLeftForChild(child, style()->isLeftToRightDirection() ? newPosition : totalAvailableLogicalWidth - newPosition - logicalWidthForChild(child));
}

void LayoutBlockFlow::setLogicalLeftForChild(LayoutBox& child, LayoutUnit logicalLeft)
{
    if (isHorizontalWritingMode()) {
        child.setX(logicalLeft);
    } else {
        child.setY(logicalLeft);
    }
}

void LayoutBlockFlow::setLogicalTopForChild(LayoutBox& child, LayoutUnit logicalTop)
{
    if (isHorizontalWritingMode()) {
        child.setY(logicalTop);
    } else {
        child.setX(logicalTop);
    }
}

void LayoutBlockFlow::layoutBlockChild(LayoutBox& child, MarginInfo& marginInfo, LayoutUnit& previousFloatLogicalBottom)
{
    LayoutUnit oldPosMarginBefore = maxPositiveMarginBefore();
    LayoutUnit oldNegMarginBefore = maxNegativeMarginBefore();

    // The child is a normal flow object. Compute the margins we will use for collapsing now.
    child.computeAndSetBlockDirectionMargins(this);

    // Try to guess our correct logical top position. In most cases this guess will
    // be correct. Only if we're wrong (when we compute the real logical top position)
    // will we have to potentially relayout.
    LayoutUnit estimateWithoutPagination;
    LayoutUnit logicalTopEstimate = estimateLogicalTopPosition(child, marginInfo, estimateWithoutPagination);

    // Cache our old rect so that we can dirty the proper paint invalidation rects if the child moves.
    LayoutRect oldRect = child.frameRect();
    LayoutUnit oldLogicalTop = logicalTopForChild(child);

    // Go ahead and position the child as though it didn't collapse with the top.
    setLogicalTopForChild(child, logicalTopEstimate);

    LayoutBlockFlow* childLayoutBlockFlow = child.isLayoutBlockFlow() ? toLayoutBlockFlow(&child) : 0;
    bool markDescendantsWithFloats = false;
    if (logicalTopEstimate != oldLogicalTop && childLayoutBlockFlow && !childLayoutBlockFlow->avoidsFloats() && childLayoutBlockFlow->containsFloats()) {
        markDescendantsWithFloats = true;
    } else if (UNLIKELY(logicalTopEstimate.mightBeSaturated())) {
        // logicalTopEstimate, returned by estimateLogicalTopPosition, might be saturated for
        // very large elements. If it does the comparison with oldLogicalTop might yield a
        // false negative as adding and removing margins, borders etc from a saturated number
        // might yield incorrect results. If this is the case always mark for layout.
        markDescendantsWithFloats = true;
    } else if (!child.avoidsFloats() || child.shrinkToAvoidFloats()) {
        // If an element might be affected by the presence of floats, then always mark it for
        // layout.
        LayoutUnit fb = std::max(previousFloatLogicalBottom, lowestFloatLogicalBottom());
        if (fb > logicalTopEstimate)
            markDescendantsWithFloats = true;
    }

    if (childLayoutBlockFlow) {
        if (markDescendantsWithFloats)
            childLayoutBlockFlow->markAllDescendantsWithFloatsForLayout();
        if (!child.isWritingModeRoot())
            previousFloatLogicalBottom = std::max(previousFloatLogicalBottom, oldLogicalTop + childLayoutBlockFlow->lowestFloatLogicalBottom());
    }

    SubtreeLayoutScope layoutScope(child);
    if (!child.needsLayout())
        child.markForPaginationRelayoutIfNeeded(layoutScope);

    bool childNeededLayout = child.needsLayout();
    if (childNeededLayout)
        child.layout();

    // Cache if we are at the top of the block right now.
    bool atBeforeSideOfBlock = marginInfo.atBeforeSideOfBlock();
    bool childIsSelfCollapsing = child.isSelfCollapsingBlock();

    // Now determine the correct ypos based off examination of collapsing margin
    // values.
    LayoutUnit logicalTopBeforeClear = collapseMargins(child, marginInfo, childIsSelfCollapsing);

    // Now check for clear.
    LayoutUnit logicalTopAfterClear = clearFloatsIfNeeded(child, marginInfo, oldPosMarginBefore, oldNegMarginBefore, logicalTopBeforeClear, childIsSelfCollapsing);

    bool paginated = view()->layoutState()->isPaginated();
    if (paginated) {
        logicalTopAfterClear = adjustBlockChildForPagination(logicalTopAfterClear, estimateWithoutPagination, child,
            atBeforeSideOfBlock && logicalTopBeforeClear == logicalTopAfterClear);
    }

    setLogicalTopForChild(child, logicalTopAfterClear);

    // Now we have a final top position. See if it really does end up being different from our estimate.
    // clearFloatsIfNeeded can also mark the child as needing a layout even though we didn't move. This happens
    // when collapseMargins dynamically adds overhanging floats because of a child with negative margins.
    if (logicalTopAfterClear != logicalTopEstimate || child.needsLayout() || (paginated && childLayoutBlockFlow && childLayoutBlockFlow->shouldBreakAtLineToAvoidWidow())) {
        SubtreeLayoutScope layoutScope(child);
        if (child.shrinkToAvoidFloats()) {
            // The child's width depends on the line width.
            // When the child shifts to clear an item, its width can
            // change (because it has more available line width).
            // So go ahead and mark the item as dirty.
            layoutScope.setChildNeedsLayout(&child);
        }

        if (childLayoutBlockFlow && !childLayoutBlockFlow->avoidsFloats() && childLayoutBlockFlow->containsFloats())
            childLayoutBlockFlow->markAllDescendantsWithFloatsForLayout();

        if (!child.needsLayout())
            child.markForPaginationRelayoutIfNeeded(layoutScope);

        // Our guess was wrong. Make the child lay itself out again.
        child.layoutIfNeeded();
    }

    // If we previously encountered a self-collapsing sibling of this child that had clearance then
    // we set this bit to ensure we would not collapse the child's margins, and those of any subsequent
    // self-collapsing siblings, with our parent. If this child is not self-collapsing then it can
    // collapse its margins with the parent so reset the bit.
    if (!marginInfo.canCollapseMarginAfterWithLastChild() && !childIsSelfCollapsing)
        marginInfo.setCanCollapseMarginAfterWithLastChild(true);

    // We are no longer at the top of the block if we encounter a non-empty child.
    // This has to be done after checking for clear, so that margins can be reset if a clear occurred.
    if (marginInfo.atBeforeSideOfBlock() && !childIsSelfCollapsing)
        marginInfo.setAtBeforeSideOfBlock(false);

    // Now place the child in the correct left position
    determineLogicalLeftPositionForChild(child);

    LayoutSize childOffset = child.location() - oldRect.location();

    // Update our height now that the child has been placed in the correct position.
    setLogicalHeight(logicalHeight() + logicalHeightForChild(child));
    if (mustSeparateMarginAfterForChild(child)) {
        setLogicalHeight(logicalHeight() + marginAfterForChild(child));
        marginInfo.clearMargin();
    }
    // If the child has overhanging floats that intrude into following siblings (or possibly out
    // of this block), then the parent gets notified of the floats now.
    if (childLayoutBlockFlow)
        addOverhangingFloats(childLayoutBlockFlow, !childNeededLayout);

    // If the child moved, we have to invalidate its paint as well as any floating/positioned
    // descendants. An exception is if we need a layout. In this case, we know we're going to
    // invalidate our paint (and the child) anyway.
    if (!selfNeedsLayout() && (childOffset.width() || childOffset.height()))
        child.invalidatePaintForOverhangingFloats(true);

    if (paginated) {
        // Check for an after page/column break.
        LayoutUnit newHeight = applyAfterBreak(child, logicalHeight(), marginInfo);
        if (newHeight != size().height())
            setLogicalHeight(newHeight);
    }

    if (child.isLayoutMultiColumnSpannerPlaceholder()) {
        // The actual column-span:all element is positioned by this placeholder child.
        positionSpannerDescendant(toLayoutMultiColumnSpannerPlaceholder(child));
    }
}

LayoutUnit LayoutBlockFlow::adjustBlockChildForPagination(LayoutUnit logicalTopAfterClear, LayoutUnit estimateWithoutPagination, LayoutBox& child, bool atBeforeSideOfBlock)
{
    LayoutBlockFlow* childBlockFlow = child.isLayoutBlockFlow() ? toLayoutBlockFlow(&child) : 0;

    if (estimateWithoutPagination != logicalTopAfterClear) {
        // Our guess prior to pagination movement was wrong. Before we attempt to paginate, let's try again at the new
        // position.
        setLogicalHeight(logicalTopAfterClear);
        setLogicalTopForChild(child, logicalTopAfterClear);

        if (child.shrinkToAvoidFloats()) {
            // The child's width depends on the line width.
            // When the child shifts to clear an item, its width can
            // change (because it has more available line width).
            // So go ahead and mark the item as dirty.
            child.setChildNeedsLayout(MarkOnlyThis);
        }

        SubtreeLayoutScope layoutScope(child);

        if (childBlockFlow) {
            if (!childBlockFlow->avoidsFloats() && childBlockFlow->containsFloats())
                childBlockFlow->markAllDescendantsWithFloatsForLayout();
            if (!child.needsLayout())
                child.markForPaginationRelayoutIfNeeded(layoutScope);
        }

        // Our guess was wrong. Make the child lay itself out again.
        child.layoutIfNeeded();
    }

    LayoutUnit oldTop = logicalTopAfterClear;

    // If the object has a page or column break value of "before", then we should shift to the top of the next page.
    LayoutUnit result = applyBeforeBreak(child, logicalTopAfterClear);

    // For replaced elements and scrolled elements, we want to shift them to the next page if they don't fit on the current one.
    LayoutUnit logicalTopBeforeUnsplittableAdjustment = result;
    LayoutUnit logicalTopAfterUnsplittableAdjustment = adjustForUnsplittableChild(child, result);

    LayoutUnit paginationStrut = 0;
    LayoutUnit unsplittableAdjustmentDelta = logicalTopAfterUnsplittableAdjustment - logicalTopBeforeUnsplittableAdjustment;
    LayoutUnit childLogicalHeight = child.logicalHeight();
    if (unsplittableAdjustmentDelta) {
        setPageBreak(result, childLogicalHeight - unsplittableAdjustmentDelta);
        paginationStrut = unsplittableAdjustmentDelta;
    } else if (childBlockFlow && childBlockFlow->paginationStrut()) {
        paginationStrut = childBlockFlow->paginationStrut();
    }

    if (paginationStrut) {
        // We are willing to propagate out to our parent block as long as we were at the top of the block prior
        // to collapsing our margins, and as long as we didn't clear or move as a result of other pagination.
        if (atBeforeSideOfBlock && oldTop == result && !isOutOfFlowPositioned() && !isTableCell()) {
            // FIXME: Should really check if we're exceeding the page height before propagating the strut, but we don't
            // have all the information to do so (the strut only has the remaining amount to push). Gecko gets this wrong too
            // and pushes to the next page anyway, so not too concerned about it.
            setPaginationStrut(result + paginationStrut);
            if (childBlockFlow)
                childBlockFlow->setPaginationStrut(0);
        } else {
            result += paginationStrut;
        }
    }

    if (!unsplittableAdjustmentDelta) {
        if (LayoutUnit pageLogicalHeight = pageLogicalHeightForOffset(result)) {
            LayoutUnit remainingLogicalHeight = pageRemainingLogicalHeightForOffset(result, ExcludePageBoundary);
            LayoutUnit spaceShortage = childLogicalHeight - remainingLogicalHeight;
            if (spaceShortage > 0) {
                // If the child crosses a column boundary, report a break, in case nothing inside it
                // has already done so. The column balancer needs to know how much it has to stretch
                // the columns to make more content fit. If no breaks are reported (but do occur),
                // the balancer will have no clue. Only measure the space after the last column
                // boundary, in case it crosses more than one.
                LayoutUnit spaceShortageInLastColumn = intMod(spaceShortage, pageLogicalHeight);
                setPageBreak(result, spaceShortageInLastColumn ? spaceShortageInLastColumn : spaceShortage);
            } else if (remainingLogicalHeight == pageLogicalHeight && offsetFromLogicalTopOfFirstPage() + child.logicalTop()) {
                // We're at the very top of a page or column, and it's not the first one. This child
                // may turn out to be the smallest piece of content that causes a page break, so we
                // need to report it.
                setPageBreak(result, childLogicalHeight);
            }
        }
    }

    // Similar to how we apply clearance. Go ahead and boost height() to be the place where we're going to position the child.
    setLogicalHeight(logicalHeight() + (result - oldTop));

    // Return the final adjusted logical top.
    return result;
}

static inline LayoutUnit calculateMinimumPageHeight(const ComputedStyle& style, const RootInlineBox& lastLine)
{
    // We may require a certain minimum number of lines per page in order to satisfy
    // orphans and widows, and that may affect the minimum page height.
    unsigned lineCount = std::max<unsigned>(style.hasAutoOrphans() ? 1 : style.orphans(), style.widows());
    const RootInlineBox* firstLine = &lastLine;
    for (unsigned i = 1; i < lineCount && firstLine->prevRootBox(); i++)
        firstLine = firstLine->prevRootBox();
    return lastLine.lineBottomWithLeading() - firstLine->lineTopWithLeading();
}

void LayoutBlockFlow::adjustLinePositionForPagination(RootInlineBox& lineBox, LayoutUnit& delta)
{
    // TODO(mstensho): Pay attention to line overflow. It should be painted in the same column as
    // the rest of the line, possibly overflowing the column. We currently only allow overflow above
    // the first column. We clip at all other column boundaries, and that's how it has to be for
    // now. The paint we have to do when a column has overflow has to be special. We need to exclude
    // content that paints in a previous column (and content that paints in the following column).
    //
    // FIXME: Another problem with simply moving lines is that the available line width may change (because of floats).
    // Technically if the location we move the line to has a different line width than our old position, then we need to dirty the
    // line and all following lines.
    LayoutUnit logicalOffset = lineBox.lineTopWithLeading();
    LayoutUnit lineHeight = lineBox.lineBottomWithLeading() - logicalOffset;
    updateMinimumPageHeight(logicalOffset, calculateMinimumPageHeight(styleRef(), lineBox));
    logicalOffset += delta;
    lineBox.setPaginationStrut(LayoutUnit());
    lineBox.setIsFirstAfterPageBreak(false);
    LayoutUnit pageLogicalHeight = pageLogicalHeightForOffset(logicalOffset);
    if (!pageLogicalHeight)
        return;
    if (lineHeight > pageLogicalHeight) {
        // Too tall to fit in one page / column. Give up. Don't push to the next page / column.
        // TODO(mstensho): Get rid of this. This is just utter weirdness, but the other browsers
        // also do something slightly similar, although in much more specific cases than we do here,
        // and printing Google Docs depends on it.
        return;
    }
    LayoutUnit remainingLogicalHeight = pageRemainingLogicalHeightForOffset(logicalOffset, ExcludePageBoundary);

    int lineIndex = lineCount(&lineBox);
    if (remainingLogicalHeight < lineHeight || (shouldBreakAtLineToAvoidWidow() && lineBreakToAvoidWidow() == lineIndex)) {
        if (shouldBreakAtLineToAvoidWidow() && lineBreakToAvoidWidow() == lineIndex) {
            clearShouldBreakAtLineToAvoidWidow();
            setDidBreakAtLineToAvoidWidow();
        }
        LayoutUnit totalLogicalHeight = lineHeight + std::max<LayoutUnit>(0, logicalOffset);
        LayoutUnit pageLogicalHeightAtNewOffset = pageLogicalHeightForOffset(logicalOffset + remainingLogicalHeight);
        setPageBreak(logicalOffset, lineHeight - remainingLogicalHeight);
        if (((lineBox == firstRootBox() && totalLogicalHeight < pageLogicalHeightAtNewOffset) || (!style()->hasAutoOrphans() && style()->orphans() >= lineIndex))
            && !isOutOfFlowPositioned() && !isTableCell()) {
            setPaginationStrut(remainingLogicalHeight + std::max<LayoutUnit>(0, logicalOffset));
        } else {
            delta += remainingLogicalHeight;
            lineBox.setPaginationStrut(remainingLogicalHeight);
            lineBox.setIsFirstAfterPageBreak(true);
        }
    } else if (remainingLogicalHeight == pageLogicalHeight) {
        // We're at the very top of a page or column.
        if (lineBox != firstRootBox())
            lineBox.setIsFirstAfterPageBreak(true);
        if (lineBox != firstRootBox() || offsetFromLogicalTopOfFirstPage())
            setPageBreak(logicalOffset, lineHeight);
    }
}

LayoutUnit LayoutBlockFlow::adjustForUnsplittableChild(LayoutBox& child, LayoutUnit logicalOffset, bool includeMargins)
{
    bool checkColumnBreaks = flowThreadContainingBlock();
    bool checkPageBreaks = !checkColumnBreaks && view()->layoutState()->pageLogicalHeight();
    bool isUnsplittable = child.isUnsplittableForPagination() || (checkColumnBreaks && child.style()->columnBreakInside() == PBAVOID)
        || (checkPageBreaks && child.style()->pageBreakInside() == PBAVOID);
    if (!isUnsplittable)
        return logicalOffset;
    LayoutUnit childLogicalHeight = logicalHeightForChild(child) + (includeMargins ? marginBeforeForChild(child) + marginAfterForChild(child) : LayoutUnit());
    LayoutUnit pageLogicalHeight = pageLogicalHeightForOffset(logicalOffset);
    updateMinimumPageHeight(logicalOffset, childLogicalHeight);
    if (!pageLogicalHeight)
        return logicalOffset;
    LayoutUnit remainingLogicalHeight = pageRemainingLogicalHeightForOffset(logicalOffset, ExcludePageBoundary);
    if (remainingLogicalHeight < childLogicalHeight)
        return logicalOffset + remainingLogicalHeight;
    return logicalOffset;
}

void LayoutBlockFlow::rebuildFloatsFromIntruding()
{
    if (m_floatingObjects)
        m_floatingObjects->setHorizontalWritingMode(isHorizontalWritingMode());

    HashSet<LayoutBox*> oldIntrudingFloatSet;
    if (!childrenInline() && m_floatingObjects) {
        const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
        FloatingObjectSetIterator end = floatingObjectSet.end();
        for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
            const FloatingObject& floatingObject = *it->get();
            if (!floatingObject.isDescendant())
                oldIntrudingFloatSet.add(floatingObject.layoutObject());
        }
    }

    // Inline blocks are covered by the isReplaced() check in the avoidFloats method.
    if (avoidsFloats() || isDocumentElement() || isLayoutView() || isFloatingOrOutOfFlowPositioned() || isTableCell()) {
        if (m_floatingObjects) {
            m_floatingObjects->clear();
        }
        if (!oldIntrudingFloatSet.isEmpty())
            markAllDescendantsWithFloatsForLayout();
        return;
    }

    LayoutBoxToFloatInfoMap floatMap;

    if (m_floatingObjects) {
        if (childrenInline())
            m_floatingObjects->moveAllToFloatInfoMap(floatMap);
        else
            m_floatingObjects->clear();
    }

    // We should not process floats if the parent node is not a LayoutBlockFlow. Otherwise, we will add
    // floats in an invalid context. This will cause a crash arising from a bad cast on the parent.
    // See <rdar://problem/8049753>, where float property is applied on a text node in a SVG.
    if (!parent() || !parent()->isLayoutBlockFlow())
        return;

    // Attempt to locate a previous sibling with overhanging floats. We skip any elements that
    // may have shifted to avoid floats, and any objects whose floats cannot interact with objects
    // outside it (i.e. objects that create a new block formatting context).
    LayoutBlockFlow* parentBlockFlow = toLayoutBlockFlow(parent());
    bool parentHasFloats = false;
    LayoutObject* prev = previousSibling();
    while (prev && (!prev->isBox() || !prev->isLayoutBlock() || toLayoutBlock(prev)->avoidsFloats() || toLayoutBlock(prev)->createsNewFormattingContext())) {
        if (prev->isFloating())
            parentHasFloats = true;
        prev = prev->previousSibling();
    }

    // First add in floats from the parent. Self-collapsing blocks let their parent track any floats that intrude into
    // them (as opposed to floats they contain themselves) so check for those here too.
    LayoutUnit logicalTopOffset = logicalTop();
    bool parentHasIntrudingFloats = !parentHasFloats && (!prev || toLayoutBlockFlow(prev)->isSelfCollapsingBlock()) && parentBlockFlow->lowestFloatLogicalBottom() > logicalTopOffset;
    if (parentHasFloats || parentHasIntrudingFloats)
        addIntrudingFloats(parentBlockFlow, parentBlockFlow->logicalLeftOffsetForContent(), logicalTopOffset);

    // Add overhanging floats from the previous LayoutBlockFlow, but only if it has a float that intrudes into our space.
    if (prev) {
        LayoutBlockFlow* blockFlow = toLayoutBlockFlow(prev);
        logicalTopOffset -= blockFlow->logicalTop();
        if (blockFlow->lowestFloatLogicalBottom() > logicalTopOffset)
            addIntrudingFloats(blockFlow, 0, logicalTopOffset);
    }

    if (childrenInline()) {
        LayoutUnit changeLogicalTop = LayoutUnit::max();
        LayoutUnit changeLogicalBottom = LayoutUnit::min();
        if (m_floatingObjects) {
            const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
            FloatingObjectSetIterator end = floatingObjectSet.end();
            for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
                const FloatingObject& floatingObject = *it->get();
                FloatingObject* oldFloatingObject = floatMap.get(floatingObject.layoutObject());
                LayoutUnit logicalBottom = logicalBottomForFloat(floatingObject);
                if (oldFloatingObject) {
                    LayoutUnit oldLogicalBottom = logicalBottomForFloat(*oldFloatingObject);
                    if (logicalWidthForFloat(floatingObject) != logicalWidthForFloat(*oldFloatingObject) || logicalLeftForFloat(floatingObject) != logicalLeftForFloat(*oldFloatingObject)) {
                        changeLogicalTop = 0;
                        changeLogicalBottom = std::max(changeLogicalBottom, std::max(logicalBottom, oldLogicalBottom));
                    } else {
                        if (logicalBottom != oldLogicalBottom) {
                            changeLogicalTop = std::min(changeLogicalTop, std::min(logicalBottom, oldLogicalBottom));
                            changeLogicalBottom = std::max(changeLogicalBottom, std::max(logicalBottom, oldLogicalBottom));
                        }
                        LayoutUnit logicalTop = logicalTopForFloat(floatingObject);
                        LayoutUnit oldLogicalTop = logicalTopForFloat(*oldFloatingObject);
                        if (logicalTop != oldLogicalTop) {
                            changeLogicalTop = std::min(changeLogicalTop, std::min(logicalTop, oldLogicalTop));
                            changeLogicalBottom = std::max(changeLogicalBottom, std::max(logicalTop, oldLogicalTop));
                        }
                    }

                    if (oldFloatingObject->originatingLine() && !selfNeedsLayout()) {
                        ASSERT(oldFloatingObject->originatingLine()->layoutObject() == this);
                        oldFloatingObject->originatingLine()->markDirty();
                    }

                    floatMap.remove(floatingObject.layoutObject());
                } else {
                    changeLogicalTop = 0;
                    changeLogicalBottom = std::max(changeLogicalBottom, logicalBottom);
                }
            }
        }

        LayoutBoxToFloatInfoMap::iterator end = floatMap.end();
        for (LayoutBoxToFloatInfoMap::iterator it = floatMap.begin(); it != end; ++it) {
            OwnPtr<FloatingObject>& floatingObject = it->value;
            if (!floatingObject->isDescendant()) {
                changeLogicalTop = 0;
                changeLogicalBottom = std::max(changeLogicalBottom, logicalBottomForFloat(*floatingObject));
            }
        }

        markLinesDirtyInBlockRange(changeLogicalTop, changeLogicalBottom);
    } else if (!oldIntrudingFloatSet.isEmpty()) {
        // If there are previously intruding floats that no longer intrude, then children with floats
        // should also get layout because they might need their floating object lists cleared.
        if (m_floatingObjects->set().size() < oldIntrudingFloatSet.size()) {
            markAllDescendantsWithFloatsForLayout();
        } else {
            const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
            FloatingObjectSetIterator end = floatingObjectSet.end();
            for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end && !oldIntrudingFloatSet.isEmpty(); ++it)
                oldIntrudingFloatSet.remove((*it)->layoutObject());
            if (!oldIntrudingFloatSet.isEmpty())
                markAllDescendantsWithFloatsForLayout();
        }
    }
}

void LayoutBlockFlow::layoutBlockChildren(bool relayoutChildren, SubtreeLayoutScope& layoutScope, LayoutUnit beforeEdge, LayoutUnit afterEdge)
{
    dirtyForLayoutFromPercentageHeightDescendants(layoutScope);

    // The margin struct caches all our current margin collapsing state. The compact struct caches state when we encounter compacts,
    MarginInfo marginInfo(this, beforeEdge, afterEdge);

    // Fieldsets need to find their legend and position it inside the border of the object.
    // The legend then gets skipped during normal layout. The same is true for ruby text.
    // It doesn't get included in the normal layout process but is instead skipped.
    LayoutObject* childToExclude = layoutSpecialExcludedChild(relayoutChildren, layoutScope);

    LayoutUnit previousFloatLogicalBottom = 0;

    LayoutBox* next = firstChildBox();
    LayoutBox* lastNormalFlowChild = nullptr;

    while (next) {
        LayoutBox* child = next;
        next = child->nextSiblingBox();

        child->setMayNeedPaintInvalidation();

        if (childToExclude == child)
            continue; // Skip this child, since it will be positioned by the specialized subclass (fieldsets and ruby runs).

        updateBlockChildDirtyBitsBeforeLayout(relayoutChildren, *child);

        if (child->isOutOfFlowPositioned()) {
            child->containingBlock()->insertPositionedObject(child);
            adjustPositionedBlock(*child, marginInfo);
            continue;
        }
        if (child->isFloating()) {
            insertFloatingObject(*child);
            adjustFloatingBlock(marginInfo);
            continue;
        }
        if (child->isColumnSpanAll()) {
            // This is not the containing block of the spanner. The spanner's placeholder will lay
            // it out in due course. For now we just need to consult our flow thread, so that the
            // columns (if any) preceding and following the spanner are laid out correctly. But
            // first we apply the pending margin, so that it's taken into consideration and doesn't
            // end up on the other side of the spanner.
            setLogicalHeight(logicalHeight() + marginInfo.margin());
            marginInfo.clearMargin();

            flowThreadContainingBlock()->skipColumnSpanner(child, offsetFromLogicalTopOfFirstPage() + logicalHeight());
            continue;
        }

        // Lay out the child.
        layoutBlockChild(*child, marginInfo, previousFloatLogicalBottom);
        lastNormalFlowChild = child;
    }

    // Now do the handling of the bottom of the block, adding in our bottom border/padding and
    // determining the correct collapsed bottom margin information.
    handleAfterSideOfBlock(lastNormalFlowChild, beforeEdge, afterEdge, marginInfo);
}

// Our MarginInfo state used when laying out block children.
MarginInfo::MarginInfo(LayoutBlockFlow* blockFlow, LayoutUnit beforeBorderPadding, LayoutUnit afterBorderPadding)
    : m_canCollapseMarginAfterWithLastChild(true)
    , m_atBeforeSideOfBlock(true)
    , m_atAfterSideOfBlock(false)
    , m_hasMarginBeforeQuirk(false)
    , m_hasMarginAfterQuirk(false)
    , m_determinedMarginBeforeQuirk(false)
    , m_discardMargin(false)
    , m_lastChildIsSelfCollapsingBlockWithClearance(false)
{
    const ComputedStyle& blockStyle = blockFlow->styleRef();
    ASSERT(blockFlow->isLayoutView() || blockFlow->parent());
    m_canCollapseWithChildren = !blockFlow->createsNewFormattingContext() && !blockFlow->isLayoutFlowThread() && !blockFlow->isLayoutView();

    m_canCollapseMarginBeforeWithChildren = m_canCollapseWithChildren && !beforeBorderPadding && blockStyle.marginBeforeCollapse() != MSEPARATE;

    // If any height other than auto is specified in CSS, then we don't collapse our bottom
    // margins with our children's margins. To do otherwise would be to risk odd visual
    // effects when the children overflow out of the parent block and yet still collapse
    // with it. We also don't collapse if we have any bottom border/padding.
    m_canCollapseMarginAfterWithChildren = m_canCollapseWithChildren && !afterBorderPadding
        && (blockStyle.logicalHeight().isAuto() && !blockStyle.logicalHeight().value()) && blockStyle.marginAfterCollapse() != MSEPARATE;

    m_quirkContainer = blockFlow->isTableCell() || blockFlow->isBody();

    m_discardMargin = m_canCollapseMarginBeforeWithChildren && blockFlow->mustDiscardMarginBefore();

    m_positiveMargin = (m_canCollapseMarginBeforeWithChildren && !blockFlow->mustDiscardMarginBefore()) ? blockFlow->maxPositiveMarginBefore() : LayoutUnit();
    m_negativeMargin = (m_canCollapseMarginBeforeWithChildren && !blockFlow->mustDiscardMarginBefore()) ? blockFlow->maxNegativeMarginBefore() : LayoutUnit();
}

LayoutBlockFlow::MarginValues LayoutBlockFlow::marginValuesForChild(LayoutBox& child) const
{
    LayoutUnit childBeforePositive = 0;
    LayoutUnit childBeforeNegative = 0;
    LayoutUnit childAfterPositive = 0;
    LayoutUnit childAfterNegative = 0;

    LayoutUnit beforeMargin = 0;
    LayoutUnit afterMargin = 0;

    LayoutBlockFlow* childLayoutBlockFlow = child.isLayoutBlockFlow() ? toLayoutBlockFlow(&child) : 0;

    // If the child has the same directionality as we do, then we can just return its
    // margins in the same direction.
    if (!child.isWritingModeRoot()) {
        if (childLayoutBlockFlow) {
            childBeforePositive = childLayoutBlockFlow->maxPositiveMarginBefore();
            childBeforeNegative = childLayoutBlockFlow->maxNegativeMarginBefore();
            childAfterPositive = childLayoutBlockFlow->maxPositiveMarginAfter();
            childAfterNegative = childLayoutBlockFlow->maxNegativeMarginAfter();
        } else {
            beforeMargin = child.marginBefore();
            afterMargin = child.marginAfter();
        }
    } else if (child.isHorizontalWritingMode() == isHorizontalWritingMode()) {
        // The child has a different directionality. If the child is parallel, then it's just
        // flipped relative to us. We can use the margins for the opposite edges.
        if (childLayoutBlockFlow) {
            childBeforePositive = childLayoutBlockFlow->maxPositiveMarginAfter();
            childBeforeNegative = childLayoutBlockFlow->maxNegativeMarginAfter();
            childAfterPositive = childLayoutBlockFlow->maxPositiveMarginBefore();
            childAfterNegative = childLayoutBlockFlow->maxNegativeMarginBefore();
        } else {
            beforeMargin = child.marginAfter();
            afterMargin = child.marginBefore();
        }
    } else {
        // The child is perpendicular to us, which means its margins don't collapse but are on the
        // "logical left/right" sides of the child box. We can just return the raw margin in this case.
        beforeMargin = marginBeforeForChild(child);
        afterMargin = marginAfterForChild(child);
    }

    // Resolve uncollapsing margins into their positive/negative buckets.
    if (beforeMargin) {
        if (beforeMargin > 0)
            childBeforePositive = beforeMargin;
        else
            childBeforeNegative = -beforeMargin;
    }
    if (afterMargin) {
        if (afterMargin > 0)
            childAfterPositive = afterMargin;
        else
            childAfterNegative = -afterMargin;
    }

    return LayoutBlockFlow::MarginValues(childBeforePositive, childBeforeNegative, childAfterPositive, childAfterNegative);
}

LayoutUnit LayoutBlockFlow::collapseMargins(LayoutBox& child, MarginInfo& marginInfo, bool childIsSelfCollapsing)
{
    bool childDiscardMarginBefore = mustDiscardMarginBeforeForChild(child);
    bool childDiscardMarginAfter = mustDiscardMarginAfterForChild(child);

    // The child discards the before margin when the the after margin has discard in the case of a self collapsing block.
    childDiscardMarginBefore = childDiscardMarginBefore || (childDiscardMarginAfter && childIsSelfCollapsing);

    // Get the four margin values for the child and cache them.
    const LayoutBlockFlow::MarginValues childMargins = marginValuesForChild(child);

    // Get our max pos and neg top margins.
    LayoutUnit posTop = childMargins.positiveMarginBefore();
    LayoutUnit negTop = childMargins.negativeMarginBefore();

    // For self-collapsing blocks, collapse our bottom margins into our
    // top to get new posTop and negTop values.
    if (childIsSelfCollapsing) {
        posTop = std::max(posTop, childMargins.positiveMarginAfter());
        negTop = std::max(negTop, childMargins.negativeMarginAfter());
    }

    // See if the top margin is quirky. We only care if this child has
    // margins that will collapse with us.
    bool topQuirk = hasMarginBeforeQuirk(&child);

    if (marginInfo.canCollapseWithMarginBefore()) {
        if (!childDiscardMarginBefore && !marginInfo.discardMargin()) {
            // This child is collapsing with the top of the
            // block. If it has larger margin values, then we need to update
            // our own maximal values.
            if (!document().inQuirksMode() || !marginInfo.quirkContainer() || !topQuirk)
                setMaxMarginBeforeValues(std::max(posTop, maxPositiveMarginBefore()), std::max(negTop, maxNegativeMarginBefore()));

            // The minute any of the margins involved isn't a quirk, don't
            // collapse it away, even if the margin is smaller (www.webreference.com
            // has an example of this, a <dt> with 0.8em author-specified inside
            // a <dl> inside a <td>.
            if (!marginInfo.determinedMarginBeforeQuirk() && !topQuirk && (posTop - negTop)) {
                setHasMarginBeforeQuirk(false);
                marginInfo.setDeterminedMarginBeforeQuirk(true);
            }

            if (!marginInfo.determinedMarginBeforeQuirk() && topQuirk && !marginBefore()) {
                // We have no top margin and our top child has a quirky margin.
                // We will pick up this quirky margin and pass it through.
                // This deals with the <td><div><p> case.
                // Don't do this for a block that split two inlines though. You do
                // still apply margins in this case.
                setHasMarginBeforeQuirk(true);
            }
        } else {
            // The before margin of the container will also discard all the margins it is collapsing with.
            setMustDiscardMarginBefore();
        }
    }

    // Once we find a child with discardMarginBefore all the margins collapsing with us must also discard.
    if (childDiscardMarginBefore) {
        marginInfo.setDiscardMargin(true);
        marginInfo.clearMargin();
    }

    if (marginInfo.quirkContainer() && marginInfo.atBeforeSideOfBlock() && (posTop - negTop))
        marginInfo.setHasMarginBeforeQuirk(topQuirk);

    LayoutUnit beforeCollapseLogicalTop = logicalHeight();
    LayoutUnit logicalTop = beforeCollapseLogicalTop;

    LayoutUnit clearanceForSelfCollapsingBlock;
    LayoutObject* prev = child.previousSibling();
    LayoutBlockFlow* previousBlockFlow =  prev && prev->isLayoutBlockFlow() && !prev->isFloatingOrOutOfFlowPositioned() ? toLayoutBlockFlow(prev) : 0;
    // If the child's previous sibling is a self-collapsing block that cleared a float then its top border edge has been set at the bottom border edge
    // of the float. Since we want to collapse the child's top margin with the self-collapsing block's top and bottom margins we need to adjust our parent's height to match the
    // margin top of the self-collapsing block. If the resulting collapsed margin leaves the child still intruding into the float then we will want to clear it.
    if (!marginInfo.canCollapseWithMarginBefore() && previousBlockFlow && marginInfo.lastChildIsSelfCollapsingBlockWithClearance()) {
        clearanceForSelfCollapsingBlock = marginValuesForChild(*previousBlockFlow).positiveMarginBefore();
        setLogicalHeight(logicalHeight() - clearanceForSelfCollapsingBlock);
    }

    if (childIsSelfCollapsing) {
        // For a self collapsing block both the before and after margins get discarded. The block doesn't contribute anything to the height of the block.
        // Also, the child's top position equals the logical height of the container.
        if (!childDiscardMarginBefore && !marginInfo.discardMargin()) {
            // This child has no height. We need to compute our
            // position before we collapse the child's margins together,
            // so that we can get an accurate position for the zero-height block.
            LayoutUnit collapsedBeforePos = std::max(marginInfo.positiveMargin(), childMargins.positiveMarginBefore());
            LayoutUnit collapsedBeforeNeg = std::max(marginInfo.negativeMargin(), childMargins.negativeMarginBefore());
            marginInfo.setMargin(collapsedBeforePos, collapsedBeforeNeg);

            // Now collapse the child's margins together, which means examining our
            // bottom margin values as well.
            marginInfo.setPositiveMarginIfLarger(childMargins.positiveMarginAfter());
            marginInfo.setNegativeMarginIfLarger(childMargins.negativeMarginAfter());

            if (!marginInfo.canCollapseWithMarginBefore()) {
                // We need to make sure that the position of the self-collapsing block
                // is correct, since it could have overflowing content
                // that needs to be positioned correctly (e.g., a block that
                // had a specified height of 0 but that actually had subcontent).
                logicalTop = logicalHeight() + collapsedBeforePos - collapsedBeforeNeg;
            }
        }
    } else {
        if (mustSeparateMarginBeforeForChild(child)) {
            ASSERT(!marginInfo.discardMargin() || (marginInfo.discardMargin() && !marginInfo.margin()));
            // If we are at the before side of the block and we collapse, ignore the computed margin
            // and just add the child margin to the container height. This will correctly position
            // the child inside the container.
            LayoutUnit separateMargin = !marginInfo.canCollapseWithMarginBefore() ? marginInfo.margin() : LayoutUnit();
            setLogicalHeight(logicalHeight() + separateMargin + marginBeforeForChild(child));
            logicalTop = logicalHeight();
        } else if (!marginInfo.discardMargin() && (!marginInfo.atBeforeSideOfBlock()
            || (!marginInfo.canCollapseMarginBeforeWithChildren()
            && (!document().inQuirksMode() || !marginInfo.quirkContainer() || !marginInfo.hasMarginBeforeQuirk())))) {
            // We're collapsing with a previous sibling's margins and not
            // with the top of the block.
            setLogicalHeight(logicalHeight() + std::max(marginInfo.positiveMargin(), posTop) - std::max(marginInfo.negativeMargin(), negTop));
            logicalTop = logicalHeight();
        }

        marginInfo.setDiscardMargin(childDiscardMarginAfter);

        if (!marginInfo.discardMargin()) {
            marginInfo.setPositiveMargin(childMargins.positiveMarginAfter());
            marginInfo.setNegativeMargin(childMargins.negativeMarginAfter());
        } else {
            marginInfo.clearMargin();
        }

        if (marginInfo.margin())
            marginInfo.setHasMarginAfterQuirk(hasMarginAfterQuirk(&child));
    }

    // If margins would pull us past the top of the next page, then we need to pull back and pretend like the margins
    // collapsed into the page edge.
    LayoutState* layoutState = view()->layoutState();
    if (layoutState->isPaginated() && isPageLogicalHeightKnown(beforeCollapseLogicalTop) && logicalTop > beforeCollapseLogicalTop) {
        LayoutUnit oldLogicalTop = logicalTop;
        logicalTop = std::min(logicalTop, nextPageLogicalTop(beforeCollapseLogicalTop));
        setLogicalHeight(logicalHeight() + (logicalTop - oldLogicalTop));
    }

    if (previousBlockFlow) {
        // If |child| is a self-collapsing block it may have collapsed into a previous sibling and although it hasn't reduced the height of the parent yet
        // any floats from the parent will now overhang.
        LayoutUnit oldLogicalHeight = logicalHeight();
        setLogicalHeight(logicalTop);
        if (!previousBlockFlow->avoidsFloats() && (previousBlockFlow->logicalTop() + previousBlockFlow->lowestFloatLogicalBottom()) > logicalTop)
            addOverhangingFloats(previousBlockFlow, false);
        setLogicalHeight(oldLogicalHeight);

        // If |child|'s previous sibling is a self-collapsing block that cleared a float and margin collapsing resulted in |child| moving up
        // into the margin area of the self-collapsing block then the float it clears is now intruding into |child|. Layout again so that we can look for
        // floats in the parent that overhang |child|'s new logical top.
        bool logicalTopIntrudesIntoFloat = clearanceForSelfCollapsingBlock > 0 && logicalTop < beforeCollapseLogicalTop;
        if (logicalTopIntrudesIntoFloat && containsFloats() && !child.avoidsFloats() && lowestFloatLogicalBottom() > logicalTop)
            child.setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::AncestorMarginCollapsing);
    }

    return logicalTop;
}

void LayoutBlockFlow::adjustPositionedBlock(LayoutBox& child, const MarginInfo& marginInfo)
{
    LayoutUnit logicalTop = logicalHeight();
    updateStaticInlinePositionForChild(child, logicalTop);

    if (!marginInfo.canCollapseWithMarginBefore()) {
        // Positioned blocks don't collapse margins, so add the margin provided by
        // the container now. The child's own margin is added later when calculating its logical top.
        LayoutUnit collapsedBeforePos = marginInfo.positiveMargin();
        LayoutUnit collapsedBeforeNeg = marginInfo.negativeMargin();
        logicalTop += collapsedBeforePos - collapsedBeforeNeg;
    }

    DeprecatedPaintLayer* childLayer = child.layer();
    if (childLayer->staticBlockPosition() != logicalTop)
        childLayer->setStaticBlockPosition(logicalTop);
}

LayoutUnit LayoutBlockFlow::clearFloatsIfNeeded(LayoutBox& child, MarginInfo& marginInfo, LayoutUnit oldTopPosMargin, LayoutUnit oldTopNegMargin, LayoutUnit yPos, bool childIsSelfCollapsing)
{
    LayoutUnit heightIncrease = getClearDelta(&child, yPos);
    marginInfo.setLastChildIsSelfCollapsingBlockWithClearance(false);

    if (!heightIncrease)
        return yPos;

    if (childIsSelfCollapsing) {
        marginInfo.setLastChildIsSelfCollapsingBlockWithClearance(true);
        bool childDiscardMargin = mustDiscardMarginBeforeForChild(child) || mustDiscardMarginAfterForChild(child);
        marginInfo.setDiscardMargin(childDiscardMargin);

        // For self-collapsing blocks that clear, they can still collapse their
        // margins with following siblings. Reset the current margins to represent
        // the self-collapsing block's margins only.
        // If DISCARD is specified for -webkit-margin-collapse, reset the margin values.
        LayoutBlockFlow::MarginValues childMargins = marginValuesForChild(child);
        if (!childDiscardMargin) {
            marginInfo.setPositiveMargin(std::max(childMargins.positiveMarginBefore(), childMargins.positiveMarginAfter()));
            marginInfo.setNegativeMargin(std::max(childMargins.negativeMarginBefore(), childMargins.negativeMarginAfter()));
        } else {
            marginInfo.clearMargin();
        }

        // CSS2.1 states:
        // "If the top and bottom margins of an element with clearance are adjoining, its margins collapse with
        // the adjoining margins of following siblings but that resulting margin does not collapse with the bottom margin of the parent block."
        // So the parent's bottom margin cannot collapse through this block or any subsequent self-collapsing blocks. Set a bit to ensure
        // this happens; it will get reset if we encounter an in-flow sibling that is not self-collapsing.
        marginInfo.setCanCollapseMarginAfterWithLastChild(false);

        // For now set the border-top of |child| flush with the bottom border-edge of the float so it can layout any floating or positioned children of
        // its own at the correct vertical position. If subsequent siblings attempt to collapse with |child|'s margins in |collapseMargins| we will
        // adjust the height of the parent to |child|'s margin top (which if it is positive sits up 'inside' the float it's clearing) so that all three
        // margins can collapse at the correct vertical position.
        // Per CSS2.1 we need to ensure that any negative margin-top clears |child| beyond the bottom border-edge of the float so that the top border edge of the child
        // (i.e. its clearance)  is at a position that satisfies the equation: "the amount of clearance is set so that clearance + margin-top = [height of float],
        // i.e., clearance = [height of float] - margin-top".
        setLogicalHeight(child.logicalTop() + childMargins.negativeMarginBefore());
    } else {
        // Increase our height by the amount we had to clear.
        setLogicalHeight(logicalHeight() + heightIncrease);
    }

    if (marginInfo.canCollapseWithMarginBefore()) {
        // We can no longer collapse with the top of the block since a clear
        // occurred. The empty blocks collapse into the cleared block.
        setMaxMarginBeforeValues(oldTopPosMargin, oldTopNegMargin);
        marginInfo.setAtBeforeSideOfBlock(false);

        // In case the child discarded the before margin of the block we need to reset the mustDiscardMarginBefore flag to the initial value.
        setMustDiscardMarginBefore(style()->marginBeforeCollapse() == MDISCARD);
    }

    return yPos + heightIncrease;
}

void LayoutBlockFlow::setCollapsedBottomMargin(const MarginInfo& marginInfo)
{
    if (marginInfo.canCollapseWithMarginAfter() && !marginInfo.canCollapseWithMarginBefore()) {
        // Update the after side margin of the container to discard if the after margin of the last child also discards and we collapse with it.
        // Don't update the max margin values because we won't need them anyway.
        if (marginInfo.discardMargin()) {
            setMustDiscardMarginAfter();
            return;
        }

        // Update our max pos/neg bottom margins, since we collapsed our bottom margins
        // with our children.
        setMaxMarginAfterValues(std::max(maxPositiveMarginAfter(), marginInfo.positiveMargin()), std::max(maxNegativeMarginAfter(), marginInfo.negativeMargin()));

        if (!marginInfo.hasMarginAfterQuirk())
            setHasMarginAfterQuirk(false);

        if (marginInfo.hasMarginAfterQuirk() && !marginAfter()) {
            // We have no bottom margin and our last child has a quirky margin.
            // We will pick up this quirky margin and pass it through.
            // This deals with the <td><div><p> case.
            setHasMarginAfterQuirk(true);
        }
    }
}

void LayoutBlockFlow::marginBeforeEstimateForChild(LayoutBox& child, LayoutUnit& positiveMarginBefore, LayoutUnit& negativeMarginBefore, bool& discardMarginBefore) const
{
    // Give up if in quirks mode and we're a body/table cell and the top margin of the child box is quirky.
    // Give up if the child specified -webkit-margin-collapse: separate that prevents collapsing.
    // FIXME: Use writing mode independent accessor for marginBeforeCollapse.
    if ((document().inQuirksMode() && hasMarginBeforeQuirk(&child) && (isTableCell() || isBody())) || child.style()->marginBeforeCollapse() == MSEPARATE)
        return;

    // The margins are discarded by a child that specified -webkit-margin-collapse: discard.
    // FIXME: Use writing mode independent accessor for marginBeforeCollapse.
    if (child.style()->marginBeforeCollapse() == MDISCARD) {
        positiveMarginBefore = 0;
        negativeMarginBefore = 0;
        discardMarginBefore = true;
        return;
    }

    LayoutUnit beforeChildMargin = marginBeforeForChild(child);
    positiveMarginBefore = std::max(positiveMarginBefore, beforeChildMargin);
    negativeMarginBefore = std::max(negativeMarginBefore, -beforeChildMargin);

    if (!child.isLayoutBlockFlow())
        return;

    LayoutBlockFlow* childBlockFlow = toLayoutBlockFlow(&child);
    if (childBlockFlow->childrenInline() || childBlockFlow->isWritingModeRoot())
        return;

    MarginInfo childMarginInfo(childBlockFlow, childBlockFlow->borderBefore() + childBlockFlow->paddingBefore(), childBlockFlow->borderAfter() + childBlockFlow->paddingAfter());
    if (!childMarginInfo.canCollapseMarginBeforeWithChildren())
        return;

    LayoutBox* grandchildBox = childBlockFlow->firstChildBox();
    for ( ; grandchildBox; grandchildBox = grandchildBox->nextSiblingBox()) {
        if (!grandchildBox->isFloatingOrOutOfFlowPositioned() && !grandchildBox->isColumnSpanAll())
            break;
    }

    if (!grandchildBox)
        return;

    // Make sure to update the block margins now for the grandchild box so that we're looking at current values.
    if (grandchildBox->needsLayout()) {
        grandchildBox->computeAndSetBlockDirectionMargins(this);
        if (grandchildBox->isLayoutBlock()) {
            LayoutBlock* grandchildBlock = toLayoutBlock(grandchildBox);
            grandchildBlock->setHasMarginBeforeQuirk(grandchildBox->style()->hasMarginBeforeQuirk());
            grandchildBlock->setHasMarginAfterQuirk(grandchildBox->style()->hasMarginAfterQuirk());
        }
    }

    // If we have a 'clear' value but also have a margin we may not actually require clearance to move past any floats.
    // If that's the case we want to be sure we estimate the correct position including margins after any floats rather
    // than use 'clearance' later which could give us the wrong position.
    if (grandchildBox->style()->clear() != CNONE && childBlockFlow->marginBeforeForChild(*grandchildBox) == 0)
        return;

    // Collapse the margin of the grandchild box with our own to produce an estimate.
    childBlockFlow->marginBeforeEstimateForChild(*grandchildBox, positiveMarginBefore, negativeMarginBefore, discardMarginBefore);
}

LayoutUnit LayoutBlockFlow::estimateLogicalTopPosition(LayoutBox& child, const MarginInfo& marginInfo, LayoutUnit& estimateWithoutPagination)
{
    // FIXME: We need to eliminate the estimation of vertical position, because when it's wrong we sometimes trigger a pathological
    // relayout if there are intruding floats.
    LayoutUnit logicalTopEstimate = logicalHeight();
    if (!marginInfo.canCollapseWithMarginBefore()) {
        LayoutUnit positiveMarginBefore = 0;
        LayoutUnit negativeMarginBefore = 0;
        bool discardMarginBefore = false;
        if (child.selfNeedsLayout()) {
            // Try to do a basic estimation of how the collapse is going to go.
            marginBeforeEstimateForChild(child, positiveMarginBefore, negativeMarginBefore, discardMarginBefore);
        } else {
            // Use the cached collapsed margin values from a previous layout. Most of the time they
            // will be right.
            LayoutBlockFlow::MarginValues marginValues = marginValuesForChild(child);
            positiveMarginBefore = std::max(positiveMarginBefore, marginValues.positiveMarginBefore());
            negativeMarginBefore = std::max(negativeMarginBefore, marginValues.negativeMarginBefore());
            discardMarginBefore = mustDiscardMarginBeforeForChild(child);
        }

        // Collapse the result with our current margins.
        if (!discardMarginBefore)
            logicalTopEstimate += std::max(marginInfo.positiveMargin(), positiveMarginBefore) - std::max(marginInfo.negativeMargin(), negativeMarginBefore);
    }

    // Adjust logicalTopEstimate down to the next page if the margins are so large that we don't fit on the current
    // page.
    LayoutState* layoutState = view()->layoutState();
    if (layoutState->isPaginated() && isPageLogicalHeightKnown(logicalHeight()) && logicalTopEstimate > logicalHeight())
        logicalTopEstimate = std::min(logicalTopEstimate, nextPageLogicalTop(logicalHeight()));

    logicalTopEstimate += getClearDelta(&child, logicalTopEstimate);

    estimateWithoutPagination = logicalTopEstimate;

    if (layoutState->isPaginated()) {
        // If the object has a page or column break value of "before", then we should shift to the top of the next page.
        logicalTopEstimate = applyBeforeBreak(child, logicalTopEstimate);

        // For replaced elements and scrolled elements, we want to shift them to the next page if they don't fit on the current one.
        logicalTopEstimate = adjustForUnsplittableChild(child, logicalTopEstimate);

        if (!child.selfNeedsLayout() && child.isLayoutBlockFlow())
            logicalTopEstimate += toLayoutBlockFlow(&child)->paginationStrut();
    }

    return logicalTopEstimate;
}

void LayoutBlockFlow::adjustFloatingBlock(const MarginInfo& marginInfo)
{
    // The float should be positioned taking into account the bottom margin
    // of the previous flow. We add that margin into the height, get the
    // float positioned properly, and then subtract the margin out of the
    // height again. In the case of self-collapsing blocks, we always just
    // use the top margins, since the self-collapsing block collapsed its
    // own bottom margin into its top margin.
    //
    // Note also that the previous flow may collapse its margin into the top of
    // our block. If this is the case, then we do not add the margin in to our
    // height when computing the position of the float. This condition can be tested
    // for by simply calling canCollapseWithMarginBefore. See
    // http://www.hixie.ch/tests/adhoc/css/box/block/margin-collapse/046.html for
    // an example of this scenario.
    LayoutUnit marginOffset = marginInfo.canCollapseWithMarginBefore() ? LayoutUnit() : marginInfo.margin();
    setLogicalHeight(logicalHeight() + marginOffset);
    positionNewFloats();
    setLogicalHeight(logicalHeight() - marginOffset);
}

void LayoutBlockFlow::handleAfterSideOfBlock(LayoutBox* lastChild, LayoutUnit beforeSide, LayoutUnit afterSide, MarginInfo& marginInfo)
{
    marginInfo.setAtAfterSideOfBlock(true);

    // If our last child was a self-collapsing block with clearance then our logical height is flush with the
    // bottom edge of the float that the child clears. The correct vertical position for the margin-collapsing we want
    // to perform now is at the child's margin-top - so adjust our height to that position.
    if (marginInfo.lastChildIsSelfCollapsingBlockWithClearance()) {
        ASSERT(lastChild);
        setLogicalHeight(logicalHeight() - marginValuesForChild(*lastChild).positiveMarginBefore());
    }

    if (marginInfo.canCollapseMarginAfterWithChildren() && !marginInfo.canCollapseMarginAfterWithLastChild())
        marginInfo.setCanCollapseMarginAfterWithChildren(false);

    // If we can't collapse with children then go ahead and add in the bottom margin.
    if (!marginInfo.discardMargin() && (!marginInfo.canCollapseWithMarginAfter() && !marginInfo.canCollapseWithMarginBefore()
        && (!document().inQuirksMode() || !marginInfo.quirkContainer() || !marginInfo.hasMarginAfterQuirk())))
        setLogicalHeight(logicalHeight() + marginInfo.margin());

    // Now add in our bottom border/padding.
    setLogicalHeight(logicalHeight() + afterSide);

    // Negative margins can cause our height to shrink below our minimal height (border/padding).
    // If this happens, ensure that the computed height is increased to the minimal height.
    setLogicalHeight(std::max(logicalHeight(), beforeSide + afterSide));

    // Update our bottom collapsed margin info.
    setCollapsedBottomMargin(marginInfo);
}

void LayoutBlockFlow::setMustDiscardMarginBefore(bool value)
{
    if (style()->marginBeforeCollapse() == MDISCARD) {
        ASSERT(value);
        return;
    }

    if (!m_rareData && !value)
        return;

    if (!m_rareData)
        m_rareData = adoptPtr(new LayoutBlockFlowRareData(this));

    m_rareData->m_discardMarginBefore = value;
}

void LayoutBlockFlow::setMustDiscardMarginAfter(bool value)
{
    if (style()->marginAfterCollapse() == MDISCARD) {
        ASSERT(value);
        return;
    }

    if (!m_rareData && !value)
        return;

    if (!m_rareData)
        m_rareData = adoptPtr(new LayoutBlockFlowRareData(this));

    m_rareData->m_discardMarginAfter = value;
}

bool LayoutBlockFlow::mustDiscardMarginBefore() const
{
    return style()->marginBeforeCollapse() == MDISCARD || (m_rareData && m_rareData->m_discardMarginBefore);
}

bool LayoutBlockFlow::mustDiscardMarginAfter() const
{
    return style()->marginAfterCollapse() == MDISCARD || (m_rareData && m_rareData->m_discardMarginAfter);
}

bool LayoutBlockFlow::mustDiscardMarginBeforeForChild(const LayoutBox& child) const
{
    ASSERT(!child.selfNeedsLayout());
    if (!child.isWritingModeRoot())
        return child.isLayoutBlockFlow() ? toLayoutBlockFlow(&child)->mustDiscardMarginBefore() : (child.style()->marginBeforeCollapse() == MDISCARD);
    if (child.isHorizontalWritingMode() == isHorizontalWritingMode())
        return child.isLayoutBlockFlow() ? toLayoutBlockFlow(&child)->mustDiscardMarginAfter() : (child.style()->marginAfterCollapse() == MDISCARD);

    // FIXME: We return false here because the implementation is not geometrically complete. We have values only for before/after, not start/end.
    // In case the boxes are perpendicular we assume the property is not specified.
    return false;
}

bool LayoutBlockFlow::mustDiscardMarginAfterForChild(const LayoutBox& child) const
{
    ASSERT(!child.selfNeedsLayout());
    if (!child.isWritingModeRoot())
        return child.isLayoutBlockFlow() ? toLayoutBlockFlow(&child)->mustDiscardMarginAfter() : (child.style()->marginAfterCollapse() == MDISCARD);
    if (child.isHorizontalWritingMode() == isHorizontalWritingMode())
        return child.isLayoutBlockFlow() ? toLayoutBlockFlow(&child)->mustDiscardMarginBefore() : (child.style()->marginBeforeCollapse() == MDISCARD);

    // FIXME: See |mustDiscardMarginBeforeForChild| above.
    return false;
}

void LayoutBlockFlow::setMaxMarginBeforeValues(LayoutUnit pos, LayoutUnit neg)
{
    if (!m_rareData) {
        if (pos == LayoutBlockFlowRareData::positiveMarginBeforeDefault(this) && neg == LayoutBlockFlowRareData::negativeMarginBeforeDefault(this))
            return;
        m_rareData = adoptPtr(new LayoutBlockFlowRareData(this));
    }
    m_rareData->m_margins.setPositiveMarginBefore(pos);
    m_rareData->m_margins.setNegativeMarginBefore(neg);
}

void LayoutBlockFlow::setMaxMarginAfterValues(LayoutUnit pos, LayoutUnit neg)
{
    if (!m_rareData) {
        if (pos == LayoutBlockFlowRareData::positiveMarginAfterDefault(this) && neg == LayoutBlockFlowRareData::negativeMarginAfterDefault(this))
            return;
        m_rareData = adoptPtr(new LayoutBlockFlowRareData(this));
    }
    m_rareData->m_margins.setPositiveMarginAfter(pos);
    m_rareData->m_margins.setNegativeMarginAfter(neg);
}

bool LayoutBlockFlow::mustSeparateMarginBeforeForChild(const LayoutBox& child) const
{
    ASSERT(!child.selfNeedsLayout());
    const ComputedStyle& childStyle = child.styleRef();
    if (!child.isWritingModeRoot())
        return childStyle.marginBeforeCollapse() == MSEPARATE;
    if (child.isHorizontalWritingMode() == isHorizontalWritingMode())
        return childStyle.marginAfterCollapse() == MSEPARATE;

    // FIXME: See |mustDiscardMarginBeforeForChild| above.
    return false;
}

bool LayoutBlockFlow::mustSeparateMarginAfterForChild(const LayoutBox& child) const
{
    ASSERT(!child.selfNeedsLayout());
    const ComputedStyle& childStyle = child.styleRef();
    if (!child.isWritingModeRoot())
        return childStyle.marginAfterCollapse() == MSEPARATE;
    if (child.isHorizontalWritingMode() == isHorizontalWritingMode())
        return childStyle.marginBeforeCollapse() == MSEPARATE;

    // FIXME: See |mustDiscardMarginBeforeForChild| above.
    return false;
}

LayoutUnit LayoutBlockFlow::applyBeforeBreak(LayoutBox& child, LayoutUnit logicalOffset)
{
    // FIXME: Add page break checking here when we support printing.
    LayoutFlowThread* flowThread = flowThreadContainingBlock();
    bool checkColumnBreaks = flowThread;
    bool checkPageBreaks = !checkColumnBreaks && view()->layoutState()->pageLogicalHeight(); // FIXME: Once columns can print we have to check this.
    bool checkBeforeAlways = (checkColumnBreaks && child.style()->columnBreakBefore() == PBALWAYS)
        || (checkPageBreaks && child.style()->pageBreakBefore() == PBALWAYS);
    if (checkBeforeAlways && inNormalFlow(&child)) {
        if (checkColumnBreaks) {
            LayoutUnit offsetBreakAdjustment = 0;
            if (flowThread->addForcedColumnBreak(offsetFromLogicalTopOfFirstPage() + logicalOffset, &child, true, &offsetBreakAdjustment))
                return logicalOffset + offsetBreakAdjustment;
        }
        return nextPageLogicalTop(logicalOffset, IncludePageBoundary);
    }
    return logicalOffset;
}

LayoutUnit LayoutBlockFlow::applyAfterBreak(LayoutBox& child, LayoutUnit logicalOffset, MarginInfo& marginInfo)
{
    // FIXME: Add page break checking here when we support printing.
    LayoutFlowThread* flowThread = flowThreadContainingBlock();
    bool checkColumnBreaks = flowThread;
    bool checkPageBreaks = !checkColumnBreaks && view()->layoutState()->pageLogicalHeight(); // FIXME: Once columns can print we have to check this.
    bool checkAfterAlways = (checkColumnBreaks && child.style()->columnBreakAfter() == PBALWAYS)
        || (checkPageBreaks && child.style()->pageBreakAfter() == PBALWAYS);
    if (checkAfterAlways && inNormalFlow(&child)) {
        // So our margin doesn't participate in the next collapsing steps.
        marginInfo.clearMargin();

        if (checkColumnBreaks) {
            LayoutUnit offsetBreakAdjustment = 0;
            if (flowThread->addForcedColumnBreak(offsetFromLogicalTopOfFirstPage() + logicalOffset, &child, false, &offsetBreakAdjustment))
                return logicalOffset + offsetBreakAdjustment;
        }
        return nextPageLogicalTop(logicalOffset, IncludePageBoundary);
    }
    return logicalOffset;
}

void LayoutBlockFlow::addOverflowFromFloats()
{
    if (!m_floatingObjects)
        return;

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObjectSetIterator end = floatingObjectSet.end();
    for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
        const FloatingObject& floatingObject = *it->get();
        if (floatingObject.isDescendant())
            addOverflowFromChild(floatingObject.layoutObject(), LayoutSize(xPositionForFloatIncludingMargin(floatingObject), yPositionForFloatIncludingMargin(floatingObject)));
    }
}

void LayoutBlockFlow::computeOverflow(LayoutUnit oldClientAfterEdge, bool recomputeFloats)
{
    LayoutBlock::computeOverflow(oldClientAfterEdge, recomputeFloats);
    if (recomputeFloats || createsNewFormattingContext() || hasSelfPaintingLayer())
        addOverflowFromFloats();
}

RootInlineBox* LayoutBlockFlow::createAndAppendRootInlineBox()
{
    RootInlineBox* rootBox = createRootInlineBox();
    m_lineBoxes.appendLineBox(rootBox);

    return rootBox;
}

void LayoutBlockFlow::deleteLineBoxTree()
{
    if (containsFloats())
        m_floatingObjects->clearLineBoxTreePointers();

    m_lineBoxes.deleteLineBoxTree();
}

void LayoutBlockFlow::markAllDescendantsWithFloatsForLayout(LayoutBox* floatToRemove, bool inLayout)
{
    if (!everHadLayout() && !containsFloats())
        return;

    if (m_descendantsWithFloatsMarkedForLayout && !floatToRemove)
        return;
    m_descendantsWithFloatsMarkedForLayout |= !floatToRemove;

    MarkingBehavior markParents = inLayout ? MarkOnlyThis : MarkContainerChain;
    setChildNeedsLayout(markParents);

    if (floatToRemove)
        removeFloatingObject(floatToRemove);

    // Iterate over our children and mark them as needed.
    if (!childrenInline() || floatToRemove) {
        for (LayoutObject* child = firstChild(); child; child = child->nextSibling()) {
            if ((!floatToRemove && child->isFloatingOrOutOfFlowPositioned()) || !child->isLayoutBlock())
                continue;
            if (!child->isLayoutBlockFlow()) {
                LayoutBlock* childBlock = toLayoutBlock(child);
                if (childBlock->shrinkToAvoidFloats() && childBlock->everHadLayout())
                    childBlock->setChildNeedsLayout(markParents);
                continue;
            }
            LayoutBlockFlow* childBlockFlow = toLayoutBlockFlow(child);
            if ((floatToRemove ? childBlockFlow->containsFloat(floatToRemove) : childBlockFlow->containsFloats()) || childBlockFlow->shrinkToAvoidFloats())
                childBlockFlow->markAllDescendantsWithFloatsForLayout(floatToRemove, inLayout);
        }
    }
}

void LayoutBlockFlow::markSiblingsWithFloatsForLayout(LayoutBox* floatToRemove)
{
    if (!m_floatingObjects)
        return;

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObjectSetIterator end = floatingObjectSet.end();

    for (LayoutObject* next = nextSibling(); next; next = next->nextSibling()) {
        if (!next->isLayoutBlockFlow() || (!floatToRemove && (next->isFloatingOrOutOfFlowPositioned() || toLayoutBlockFlow(next)->avoidsFloats())))
            continue;

        LayoutBlockFlow* nextBlock = toLayoutBlockFlow(next);
        for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
            LayoutBox* floatingBox = (*it)->layoutObject();
            if (floatToRemove && floatingBox != floatToRemove)
                continue;
            if (nextBlock->containsFloat(floatingBox))
                nextBlock->markAllDescendantsWithFloatsForLayout(floatingBox);
        }
    }
}

LayoutUnit LayoutBlockFlow::getClearDelta(LayoutBox* child, LayoutUnit logicalTop)
{
    // There is no need to compute clearance if we have no floats.
    if (!containsFloats())
        return LayoutUnit();

    // At least one float is present. We need to perform the clearance computation.
    bool clearSet = child->style()->clear() != CNONE;
    LayoutUnit logicalBottom = 0;
    switch (child->style()->clear()) {
    case CNONE:
        break;
    case CLEFT:
        logicalBottom = lowestFloatLogicalBottom(FloatingObject::FloatLeft);
        break;
    case CRIGHT:
        logicalBottom = lowestFloatLogicalBottom(FloatingObject::FloatRight);
        break;
    case CBOTH:
        logicalBottom = lowestFloatLogicalBottom();
        break;
    }

    // We also clear floats if we are too big to sit on the same line as a float (and wish to avoid floats by default).
    LayoutUnit result = clearSet ? std::max<LayoutUnit>(0, logicalBottom - logicalTop) : LayoutUnit();
    if (!result && child->avoidsFloats()) {
        LayoutUnit newLogicalTop = logicalTop;
        LayoutRect borderBox = child->borderBoxRect();
        LayoutUnit childLogicalWidthAtOldLogicalTopOffset = isHorizontalWritingMode() ? borderBox.width() : borderBox.height();
        while (true) {
            LayoutUnit availableLogicalWidthAtNewLogicalTopOffset = availableLogicalWidthForLine(newLogicalTop, false, logicalHeightForChild(*child));
            if (availableLogicalWidthAtNewLogicalTopOffset == availableLogicalWidthForContent())
                return newLogicalTop - logicalTop;

            LogicalExtentComputedValues computedValues;
            child->logicalExtentAfterUpdatingLogicalWidth(newLogicalTop, computedValues);
            LayoutUnit childLogicalWidthAtNewLogicalTopOffset = computedValues.m_extent;

            if (childLogicalWidthAtNewLogicalTopOffset <= availableLogicalWidthAtNewLogicalTopOffset) {
                // Even though we may not be moving, if the logical width did shrink because of the presence of new floats, then
                // we need to force a relayout as though we shifted. This happens because of the dynamic addition of overhanging floats
                // from previous siblings when negative margins exist on a child (see the addOverhangingFloats call at the end of collapseMargins).
                if (childLogicalWidthAtOldLogicalTopOffset != childLogicalWidthAtNewLogicalTopOffset)
                    child->setChildNeedsLayout(MarkOnlyThis);
                return newLogicalTop - logicalTop;
            }

            newLogicalTop = nextFloatLogicalBottomBelow(newLogicalTop);
            ASSERT(newLogicalTop >= logicalTop);
            if (newLogicalTop < logicalTop)
                break;
        }
        ASSERT_NOT_REACHED();
    }
    return result;
}

void LayoutBlockFlow::createFloatingObjects()
{
    m_floatingObjects = adoptPtr(new FloatingObjects(this, isHorizontalWritingMode()));
}

void LayoutBlockFlow::styleWillChange(StyleDifference diff, const ComputedStyle& newStyle)
{
    const ComputedStyle* oldStyle = style();
    s_canPropagateFloatIntoSibling = oldStyle ? !isFloatingOrOutOfFlowPositioned() && !avoidsFloats() : false;
    if (oldStyle && parent() && diff.needsFullLayout() && oldStyle->position() != newStyle.position()
        && containsFloats() && !isFloating() && !isOutOfFlowPositioned() && newStyle.hasOutOfFlowPosition())
            markAllDescendantsWithFloatsForLayout();

    LayoutBlock::styleWillChange(diff, newStyle);
}

void LayoutBlockFlow::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBlock::styleDidChange(diff, oldStyle);

    // After our style changed, if we lose our ability to propagate floats into next sibling
    // blocks, then we need to find the top most parent containing that overhanging float and
    // then mark its descendants with floats for layout and clear all floats from its next
    // sibling blocks that exist in our floating objects list. See bug 56299 and 62875.
    bool canPropagateFloatIntoSibling = !isFloatingOrOutOfFlowPositioned() && !avoidsFloats();
    if (diff.needsFullLayout() && s_canPropagateFloatIntoSibling && !canPropagateFloatIntoSibling && hasOverhangingFloats()) {
        LayoutBlockFlow* parentBlockFlow = this;
        const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
        FloatingObjectSetIterator end = floatingObjectSet.end();

        for (LayoutObject* curr = parent(); curr && !curr->isLayoutView(); curr = curr->parent()) {
            if (curr->isLayoutBlockFlow()) {
                LayoutBlockFlow* currBlock = toLayoutBlockFlow(curr);

                if (currBlock->hasOverhangingFloats()) {
                    for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
                        LayoutBox* layoutBox = (*it)->layoutObject();
                        if (currBlock->hasOverhangingFloat(layoutBox)) {
                            parentBlockFlow = currBlock;
                            break;
                        }
                    }
                }
            }
        }

        parentBlockFlow->markAllDescendantsWithFloatsForLayout();
        parentBlockFlow->markSiblingsWithFloatsForLayout();
    }

    if (diff.needsFullLayout() || !oldStyle)
        createOrDestroyMultiColumnFlowThreadIfNeeded(oldStyle);
    if (oldStyle) {
        if (LayoutMultiColumnFlowThread* flowThread = multiColumnFlowThread()) {
            if (!style()->columnRuleEquivalent(oldStyle)) {
                // Column rules are painted by anonymous column set children of the multicol
                // container. We need to notify them.
                flowThread->columnRuleStyleDidChange();
            }
        }
    }
}

void LayoutBlockFlow::updateBlockChildDirtyBitsBeforeLayout(bool relayoutChildren, LayoutBox& child)
{
    if (child.isLayoutMultiColumnSpannerPlaceholder())
        toLayoutMultiColumnSpannerPlaceholder(child).markForLayoutIfObjectInFlowThreadNeedsLayout();
    LayoutBlock::updateBlockChildDirtyBitsBeforeLayout(relayoutChildren, child);
}

void LayoutBlockFlow::updateStaticInlinePositionForChild(LayoutBox& child, LayoutUnit logicalTop)
{
    if (child.style()->isOriginalDisplayInlineType())
        setStaticInlinePositionForChild(child, startAlignedOffsetForLine(logicalTop, false));
    else
        setStaticInlinePositionForChild(child, startOffsetForContent());
}

void LayoutBlockFlow::setStaticInlinePositionForChild(LayoutBox& child, LayoutUnit inlinePosition)
{
    child.layer()->setStaticInlinePosition(inlinePosition);
}

void LayoutBlockFlow::addChild(LayoutObject* newChild, LayoutObject* beforeChild)
{
    if (LayoutMultiColumnFlowThread* flowThread = multiColumnFlowThread()) {
        if (beforeChild == flowThread)
            beforeChild = flowThread->firstChild();
        ASSERT(!beforeChild || beforeChild->isDescendantOf(flowThread));
        flowThread->addChild(newChild, beforeChild);
        return;
    }
    LayoutBlock::addChild(newChild, beforeChild);
}

void LayoutBlockFlow::moveAllChildrenIncludingFloatsTo(LayoutBlock* toBlock, bool fullRemoveInsert)
{
    LayoutBlockFlow* toBlockFlow = toLayoutBlockFlow(toBlock);
    moveAllChildrenTo(toBlockFlow, fullRemoveInsert);

    // When a portion of the layout tree is being detached, anonymous blocks
    // will be combined as their children are deleted. In this process, the
    // anonymous block later in the tree is merged into the one preceeding it.
    // It can happen that the later block (this) contains floats that the
    // previous block (toBlockFlow) did not contain, and thus are not in the
    // floating objects list for toBlockFlow. This can result in toBlockFlow containing
    // floats that are not in it's floating objects list, but are in the
    // floating objects lists of siblings and parents. This can cause problems
    // when the float itself is deleted, since the deletion code assumes that
    // if a float is not in it's containing block's floating objects list, it
    // isn't in any floating objects list. In order to preserve this condition
    // (removing it has serious performance implications), we need to copy the
    // floating objects from the old block (this) to the new block (toBlockFlow).
    // The float's metrics will likely all be wrong, but since toBlockFlow is
    // already marked for layout, this will get fixed before anything gets
    // displayed.
    // See bug https://code.google.com/p/chromium/issues/detail?id=230907
    if (m_floatingObjects) {
        if (!toBlockFlow->m_floatingObjects)
            toBlockFlow->createFloatingObjects();

        const FloatingObjectSet& fromFloatingObjectSet = m_floatingObjects->set();
        FloatingObjectSetIterator end = fromFloatingObjectSet.end();

        for (FloatingObjectSetIterator it = fromFloatingObjectSet.begin(); it != end; ++it) {
            const FloatingObject& floatingObject = *it->get();

            // Don't insert the object again if it's already in the list
            if (toBlockFlow->containsFloat(floatingObject.layoutObject()))
                continue;

            toBlockFlow->m_floatingObjects->add(floatingObject.unsafeClone());
        }
    }

}

void LayoutBlockFlow::invalidatePaintForOverhangingFloats(bool paintAllDescendants)
{
    // Invalidate paint of any overhanging floats (if we know we're the one to paint them).
    // Otherwise, bail out.
    if (!hasOverhangingFloats())
        return;

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObjectSetIterator end = floatingObjectSet.end();
    for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
        const FloatingObject& floatingObject = *it->get();
        // Only issue paint invaldiations for the object if it is overhanging, is not in its own layer, and
        // is our responsibility to paint (m_shouldPaint is set). When paintAllDescendants is true, the latter
        // condition is replaced with being a descendant of us.
        if (logicalBottomForFloat(floatingObject) > logicalHeight()
            && !floatingObject.layoutObject()->hasSelfPaintingLayer()
            && (floatingObject.shouldPaint() || (paintAllDescendants && floatingObject.layoutObject()->isDescendantOf(this)))) {

            LayoutBox* floatingLayoutBox = floatingObject.layoutObject();
            floatingLayoutBox->setShouldDoFullPaintInvalidation();
            floatingLayoutBox->invalidatePaintForOverhangingFloats(false);
        }
    }
}

void LayoutBlockFlow::invalidatePaintForOverflow()
{
    // FIXME: We could tighten up the left and right invalidation points if we let layoutInlineChildren fill them in based off the particular lines
    // it had to lay out. We wouldn't need the hasOverflowClip() hack in that case either.
    LayoutUnit paintInvalidationLogicalLeft = logicalLeftVisualOverflow();
    LayoutUnit paintInvalidationLogicalRight = logicalRightVisualOverflow();
    if (hasOverflowClip()) {
        // If we have clipped overflow, we should use layout overflow as well, since visual overflow from lines didn't propagate to our block's overflow.
        // Note the old code did this as well but even for overflow:visible. The addition of hasOverflowClip() at least tightens up the hack a bit.
        // layoutInlineChildren should be patched to compute the entire paint invalidation rect.
        paintInvalidationLogicalLeft = std::min(paintInvalidationLogicalLeft, logicalLeftLayoutOverflow());
        paintInvalidationLogicalRight = std::max(paintInvalidationLogicalRight, logicalRightLayoutOverflow());
    }

    LayoutRect paintInvalidationRect;
    if (isHorizontalWritingMode())
        paintInvalidationRect = LayoutRect(paintInvalidationLogicalLeft, m_paintInvalidationLogicalTop, paintInvalidationLogicalRight - paintInvalidationLogicalLeft, m_paintInvalidationLogicalBottom - m_paintInvalidationLogicalTop);
    else
        paintInvalidationRect = LayoutRect(m_paintInvalidationLogicalTop, paintInvalidationLogicalLeft, m_paintInvalidationLogicalBottom - m_paintInvalidationLogicalTop, paintInvalidationLogicalRight - paintInvalidationLogicalLeft);

    if (hasOverflowClip()) {
        // Adjust the paint invalidation rect for scroll offset
        paintInvalidationRect.move(-scrolledContentOffset());

        // Don't allow this rect to spill out of our overflow box.
        paintInvalidationRect.intersect(LayoutRect(LayoutPoint(), size()));
    }

    // Make sure the rect is still non-empty after intersecting for overflow above
    if (!paintInvalidationRect.isEmpty()) {
        // Hits in media/event-attributes.html
        DisableCompositingQueryAsserts disabler;

        invalidatePaintRectangle(paintInvalidationRect); // We need to do a partial paint invalidation of our content.
        if (hasReflection())
            invalidatePaintRectangle(reflectedRect(paintInvalidationRect));
    }

    m_paintInvalidationLogicalTop = 0;
    m_paintInvalidationLogicalBottom = 0;
}

void LayoutBlockFlow::paintFloats(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, bool preservePhase)
{
    BlockFlowPainter(*this).paintFloats(paintInfo, paintOffset, preservePhase);
}

void LayoutBlockFlow::paintSelection(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    BlockFlowPainter(*this).paintSelection(paintInfo, paintOffset);
}

void LayoutBlockFlow::clipOutFloatingObjects(const LayoutBlock* rootBlock, ClipScope& clipScope,
    const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock) const
{
    if (!m_floatingObjects)
        return;

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObjectSetIterator end = floatingObjectSet.end();
    for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
        const FloatingObject& floatingObject = *it->get();
        LayoutRect floatBox(LayoutPoint(offsetFromRootBlock), floatingObject.layoutObject()->size());
        floatBox.move(positionForFloatIncludingMargin(floatingObject));
        rootBlock->flipForWritingMode(floatBox);
        floatBox.move(rootBlockPhysicalPosition.x(), rootBlockPhysicalPosition.y());

        clipScope.clip(floatBox, SkRegion::kDifference_Op);
    }
}

void LayoutBlockFlow::clearFloats(EClear clear)
{
    positionNewFloats();
    // set y position
    LayoutUnit newY = 0;
    switch (clear) {
    case CLEFT:
        newY = lowestFloatLogicalBottom(FloatingObject::FloatLeft);
        break;
    case CRIGHT:
        newY = lowestFloatLogicalBottom(FloatingObject::FloatRight);
        break;
    case CBOTH:
        newY = lowestFloatLogicalBottom();
    default:
        break;
    }
    if (size().height() < newY)
        setLogicalHeight(newY);
}

bool LayoutBlockFlow::containsFloat(LayoutBox* layoutBox) const
{
    return m_floatingObjects && m_floatingObjects->set().contains<FloatingObjectHashTranslator>(layoutBox);
}

void LayoutBlockFlow::removeFloatingObjects()
{
    if (!m_floatingObjects)
        return;

    markSiblingsWithFloatsForLayout();

    m_floatingObjects->clear();
}

LayoutPoint LayoutBlockFlow::flipFloatForWritingModeForChild(const FloatingObject& child, const LayoutPoint& point) const
{
    if (!style()->isFlippedBlocksWritingMode())
        return point;

    // This is similar to LayoutBox::flipForWritingModeForChild. We have to subtract out our left/top offsets twice, since
    // it's going to get added back in. We hide this complication here so that the calling code looks normal for the unflipped
    // case.
    if (isHorizontalWritingMode())
        return LayoutPoint(point.x(), point.y() + size().height() - child.layoutObject()->size().height() - 2 * yPositionForFloatIncludingMargin(child));
    return LayoutPoint(point.x() + size().width() - child.layoutObject()->size().width() - 2 * xPositionForFloatIncludingMargin(child), point.y());
}

LayoutUnit LayoutBlockFlow::logicalLeftOffsetForPositioningFloat(LayoutUnit logicalTop, LayoutUnit fixedOffset, bool applyTextIndent, LayoutUnit* heightRemaining) const
{
    LayoutUnit offset = fixedOffset;
    if (m_floatingObjects && m_floatingObjects->hasLeftObjects())
        offset = m_floatingObjects->logicalLeftOffsetForPositioningFloat(fixedOffset, logicalTop, heightRemaining);
    return adjustLogicalLeftOffsetForLine(offset, applyTextIndent);
}

LayoutUnit LayoutBlockFlow::logicalRightOffsetForPositioningFloat(LayoutUnit logicalTop, LayoutUnit fixedOffset, bool applyTextIndent, LayoutUnit* heightRemaining) const
{
    LayoutUnit offset = fixedOffset;
    if (m_floatingObjects && m_floatingObjects->hasRightObjects())
        offset = m_floatingObjects->logicalRightOffsetForPositioningFloat(fixedOffset, logicalTop, heightRemaining);
    return adjustLogicalRightOffsetForLine(offset, applyTextIndent);
}

LayoutUnit LayoutBlockFlow::adjustLogicalLeftOffsetForLine(LayoutUnit offsetFromFloats, bool applyTextIndent) const
{
    LayoutUnit left = offsetFromFloats;

    if (applyTextIndent && style()->isLeftToRightDirection())
        left += textIndentOffset();

    return left;
}

LayoutUnit LayoutBlockFlow::adjustLogicalRightOffsetForLine(LayoutUnit offsetFromFloats, bool applyTextIndent) const
{
    LayoutUnit right = offsetFromFloats;

    if (applyTextIndent && !style()->isLeftToRightDirection())
        right -= textIndentOffset();

    return right;
}

LayoutPoint LayoutBlockFlow::computeLogicalLocationForFloat(const FloatingObject& floatingObject, LayoutUnit logicalTopOffset) const
{
    LayoutBox* childBox = floatingObject.layoutObject();
    LayoutUnit logicalLeftOffset = logicalLeftOffsetForContent(); // Constant part of left offset.
    LayoutUnit logicalRightOffset; // Constant part of right offset.
    logicalRightOffset = logicalRightOffsetForContent();

    LayoutUnit floatLogicalWidth = std::min(logicalWidthForFloat(floatingObject), logicalRightOffset - logicalLeftOffset); // The width we look for.

    LayoutUnit floatLogicalLeft;

    bool insideFlowThread = flowThreadContainingBlock();

    if (childBox->style()->floating() == LeftFloat) {
        LayoutUnit heightRemainingLeft = 1;
        LayoutUnit heightRemainingRight = 1;
        floatLogicalLeft = logicalLeftOffsetForPositioningFloat(logicalTopOffset, logicalLeftOffset, false, &heightRemainingLeft);
        while (logicalRightOffsetForPositioningFloat(logicalTopOffset, logicalRightOffset, false, &heightRemainingRight) - floatLogicalLeft < floatLogicalWidth) {
            logicalTopOffset += std::min(heightRemainingLeft, heightRemainingRight);
            floatLogicalLeft = logicalLeftOffsetForPositioningFloat(logicalTopOffset, logicalLeftOffset, false, &heightRemainingLeft);
            if (insideFlowThread) {
                // Have to re-evaluate all of our offsets, since they may have changed.
                logicalRightOffset = logicalRightOffsetForContent(); // Constant part of right offset.
                logicalLeftOffset = logicalLeftOffsetForContent(); // Constant part of left offset.
                floatLogicalWidth = std::min(logicalWidthForFloat(floatingObject), logicalRightOffset - logicalLeftOffset);
            }
        }
        floatLogicalLeft = std::max(logicalLeftOffset - borderAndPaddingLogicalLeft(), floatLogicalLeft);
    } else {
        LayoutUnit heightRemainingLeft = 1;
        LayoutUnit heightRemainingRight = 1;
        floatLogicalLeft = logicalRightOffsetForPositioningFloat(logicalTopOffset, logicalRightOffset, false, &heightRemainingRight);
        while (floatLogicalLeft - logicalLeftOffsetForPositioningFloat(logicalTopOffset, logicalLeftOffset, false, &heightRemainingLeft) < floatLogicalWidth) {
            logicalTopOffset += std::min(heightRemainingLeft, heightRemainingRight);
            floatLogicalLeft = logicalRightOffsetForPositioningFloat(logicalTopOffset, logicalRightOffset, false, &heightRemainingRight);
            if (insideFlowThread) {
                // Have to re-evaluate all of our offsets, since they may have changed.
                logicalRightOffset = logicalRightOffsetForContent(); // Constant part of right offset.
                logicalLeftOffset = logicalLeftOffsetForContent(); // Constant part of left offset.
                floatLogicalWidth = std::min(logicalWidthForFloat(floatingObject), logicalRightOffset - logicalLeftOffset);
            }
        }
        // Use the original width of the float here, since the local variable
        // |floatLogicalWidth| was capped to the available line width. See
        // fast/block/float/clamped-right-float.html.
        floatLogicalLeft -= logicalWidthForFloat(floatingObject);
    }

    return LayoutPoint(floatLogicalLeft, logicalTopOffset);
}

FloatingObject* LayoutBlockFlow::insertFloatingObject(LayoutBox& floatBox)
{
    ASSERT(floatBox.isFloating());

    // Create the list of special objects if we don't aleady have one
    if (!m_floatingObjects) {
        createFloatingObjects();
    } else {
        // Don't insert the object again if it's already in the list
        const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
        FloatingObjectSetIterator it = floatingObjectSet.find<FloatingObjectHashTranslator>(&floatBox);
        if (it != floatingObjectSet.end())
            return it->get();
    }

    // Create the special object entry & append it to the list

    OwnPtr<FloatingObject> newObj = FloatingObject::create(&floatBox);

    // Our location is irrelevant if we're unsplittable or no pagination is in effect.
    // Just go ahead and lay out the float.
    bool isChildLayoutBlock = floatBox.isLayoutBlock();
    if (isChildLayoutBlock && !floatBox.needsLayout() && view()->layoutState()->pageLogicalHeightChanged())
        floatBox.setChildNeedsLayout(MarkOnlyThis);

    bool needsBlockDirectionLocationSetBeforeLayout = isChildLayoutBlock && view()->layoutState()->needsBlockDirectionLocationSetBeforeLayout();
    if (!needsBlockDirectionLocationSetBeforeLayout || isWritingModeRoot()) { // We are unsplittable if we're a block flow root.
        floatBox.layoutIfNeeded();
    } else {
        floatBox.updateLogicalWidth();
        floatBox.computeAndSetBlockDirectionMargins(this);
    }

    setLogicalWidthForFloat(*newObj, logicalWidthForChild(floatBox) + marginStartForChild(floatBox) + marginEndForChild(floatBox));

    return m_floatingObjects->add(newObj.release());
}

void LayoutBlockFlow::removeFloatingObject(LayoutBox* floatBox)
{
    if (m_floatingObjects) {
        const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
        FloatingObjectSetIterator it = floatingObjectSet.find<FloatingObjectHashTranslator>(floatBox);
        if (it != floatingObjectSet.end()) {
            FloatingObject& floatingObject = *it->get();
            if (childrenInline()) {
                LayoutUnit logicalTop = logicalTopForFloat(floatingObject);
                LayoutUnit logicalBottom = logicalBottomForFloat(floatingObject);

                // Fix for https://bugs.webkit.org/show_bug.cgi?id=54995.
                if (logicalBottom < 0 || logicalBottom < logicalTop || logicalTop == LayoutUnit::max()) {
                    logicalBottom = LayoutUnit::max();
                } else {
                    // Special-case zero- and less-than-zero-height floats: those don't touch
                    // the line that they're on, but it still needs to be dirtied. This is
                    // accomplished by pretending they have a height of 1.
                    logicalBottom = std::max(logicalBottom, logicalTop + 1);
                }
                if (floatingObject.originatingLine()) {
                    if (!selfNeedsLayout()) {
                        ASSERT(floatingObject.originatingLine()->layoutObject() == this);
                        floatingObject.originatingLine()->markDirty();
                    }
#if ENABLE(ASSERT)
                    floatingObject.setOriginatingLine(nullptr);
#endif
                }
                markLinesDirtyInBlockRange(0, logicalBottom);
            }
            m_floatingObjects->remove(&floatingObject);
        }
    }
}

void LayoutBlockFlow::removeFloatingObjectsBelow(FloatingObject* lastFloat, int logicalOffset)
{
    if (!containsFloats())
        return;

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObject* curr = floatingObjectSet.last().get();
    while (curr != lastFloat && (!curr->isPlaced() || logicalTopForFloat(*curr) >= logicalOffset)) {
        m_floatingObjects->remove(curr);
        if (floatingObjectSet.isEmpty())
            break;
        curr = floatingObjectSet.last().get();
    }
}

bool LayoutBlockFlow::positionNewFloats(LineWidth* width)
{
    if (!m_floatingObjects)
        return false;

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    if (floatingObjectSet.isEmpty())
        return false;

    // If all floats have already been positioned, then we have no work to do.
    if (floatingObjectSet.last()->isPlaced())
        return false;

    // Move backwards through our floating object list until we find a float that has
    // already been positioned. Then we'll be able to move forward, positioning all of
    // the new floats that need it.
    FloatingObjectSetIterator it = floatingObjectSet.end();
    --it; // Go to last item.
    FloatingObjectSetIterator begin = floatingObjectSet.begin();
    FloatingObject* lastPlacedFloatingObject = nullptr;
    while (it != begin) {
        --it;
        if ((*it)->isPlaced()) {
            lastPlacedFloatingObject = it->get();
            ++it;
            break;
        }
    }

    LayoutUnit logicalTop = logicalHeight();

    // The float cannot start above the top position of the last positioned float.
    if (lastPlacedFloatingObject)
        logicalTop = std::max(logicalTopForFloat(*lastPlacedFloatingObject), logicalTop);

    FloatingObjectSetIterator end = floatingObjectSet.end();
    // Now walk through the set of unpositioned floats and place them.
    for (; it != end; ++it) {
        FloatingObject& floatingObject = *it->get();
        // The containing block is responsible for positioning floats, so if we have floats in our
        // list that come from somewhere else, do not attempt to position them.
        if (floatingObject.layoutObject()->containingBlock() != this)
            continue;

        LayoutBox* childBox = floatingObject.layoutObject();

        // FIXME Investigate if this can be removed. crbug.com/370006
        childBox->setMayNeedPaintInvalidation();

        LayoutUnit childLogicalLeftMargin = style()->isLeftToRightDirection() ? marginStartForChild(*childBox) : marginEndForChild(*childBox);
        if (childBox->style()->clear() & CLEFT)
            logicalTop = std::max(lowestFloatLogicalBottom(FloatingObject::FloatLeft), logicalTop);
        if (childBox->style()->clear() & CRIGHT)
            logicalTop = std::max(lowestFloatLogicalBottom(FloatingObject::FloatRight), logicalTop);

        LayoutPoint floatLogicalLocation = computeLogicalLocationForFloat(floatingObject, logicalTop);

        setLogicalLeftForFloat(floatingObject, floatLogicalLocation.x());

        setLogicalLeftForChild(*childBox, floatLogicalLocation.x() + childLogicalLeftMargin);
        setLogicalTopForChild(*childBox, floatLogicalLocation.y() + marginBeforeForChild(*childBox));

        SubtreeLayoutScope layoutScope(*childBox);
        LayoutState* layoutState = view()->layoutState();
        bool isPaginated = layoutState->isPaginated();
        if (isPaginated && !childBox->needsLayout())
            childBox->markForPaginationRelayoutIfNeeded(layoutScope);

        childBox->layoutIfNeeded();

        if (isPaginated) {
            // If we are unsplittable and don't fit, then we need to move down.
            // We include our margins as part of the unsplittable area.
            LayoutUnit newLogicalTop = adjustForUnsplittableChild(*childBox, floatLogicalLocation.y(), true);

            // See if we have a pagination strut that is making us move down further.
            // Note that an unsplittable child can't also have a pagination strut, so this is
            // exclusive with the case above.
            LayoutBlockFlow* childBlockFlow = childBox->isLayoutBlockFlow() ? toLayoutBlockFlow(childBox) : 0;
            if (childBlockFlow && childBlockFlow->paginationStrut()) {
                newLogicalTop += childBlockFlow->paginationStrut();
                childBlockFlow->setPaginationStrut(0);
            }

            if (newLogicalTop != floatLogicalLocation.y()) {
                floatingObject.setPaginationStrut(newLogicalTop - floatLogicalLocation.y());

                floatLogicalLocation = computeLogicalLocationForFloat(floatingObject, newLogicalTop);
                setLogicalLeftForFloat(floatingObject, floatLogicalLocation.x());

                setLogicalLeftForChild(*childBox, floatLogicalLocation.x() + childLogicalLeftMargin);
                setLogicalTopForChild(*childBox, floatLogicalLocation.y() + marginBeforeForChild(*childBox));

                if (childBox->isLayoutBlock())
                    childBox->setChildNeedsLayout(MarkOnlyThis);
                childBox->layoutIfNeeded();
            }
        }

        setLogicalTopForFloat(floatingObject, floatLogicalLocation.y());

        setLogicalHeightForFloat(floatingObject, logicalHeightForChild(*childBox) + marginBeforeForChild(*childBox) + marginAfterForChild(*childBox));

        m_floatingObjects->addPlacedObject(floatingObject);

        if (ShapeOutsideInfo* shapeOutside = childBox->shapeOutsideInfo())
            shapeOutside->setReferenceBoxLogicalSize(logicalSizeForChild(*childBox));

        if (width)
            width->shrinkAvailableWidthForNewFloatIfNeeded(floatingObject);
    }
    return true;
}

bool LayoutBlockFlow::hasOverhangingFloat(LayoutBox* layoutBox)
{
    if (!m_floatingObjects || !parent())
        return false;

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObjectSetIterator it = floatingObjectSet.find<FloatingObjectHashTranslator>(layoutBox);
    if (it == floatingObjectSet.end())
        return false;

    return logicalBottomForFloat(*it->get()) > logicalHeight();
}

void LayoutBlockFlow::addIntrudingFloats(LayoutBlockFlow* prev, LayoutUnit logicalLeftOffset, LayoutUnit logicalTopOffset)
{
    ASSERT(!avoidsFloats());

    // If we create our own block formatting context then our contents don't interact with floats outside it, even those from our parent.
    if (createsNewFormattingContext())
        return;

    // If the parent or previous sibling doesn't have any floats to add, don't bother.
    if (!prev->m_floatingObjects)
        return;

    logicalLeftOffset += marginLogicalLeft();

    const FloatingObjectSet& prevSet = prev->m_floatingObjects->set();
    FloatingObjectSetIterator prevEnd = prevSet.end();
    for (FloatingObjectSetIterator prevIt = prevSet.begin(); prevIt != prevEnd; ++prevIt) {
        FloatingObject& floatingObject = *prevIt->get();
        if (logicalBottomForFloat(floatingObject) > logicalTopOffset) {
            if (!m_floatingObjects || !m_floatingObjects->set().contains(&floatingObject)) {
                // We create the floating object list lazily.
                if (!m_floatingObjects)
                    createFloatingObjects();

                // Applying the child's margin makes no sense in the case where the child was passed in.
                // since this margin was added already through the modification of the |logicalLeftOffset| variable
                // above. |logicalLeftOffset| will equal the margin in this case, so it's already been taken
                // into account. Only apply this code if prev is the parent, since otherwise the left margin
                // will get applied twice.
                LayoutSize offset = isHorizontalWritingMode()
                    ? LayoutSize(logicalLeftOffset - (prev != parent() ? prev->marginLeft() : LayoutUnit()), logicalTopOffset)
                    : LayoutSize(logicalTopOffset, logicalLeftOffset - (prev != parent() ? prev->marginTop() : LayoutUnit()));

                m_floatingObjects->add(floatingObject.copyToNewContainer(offset));
            }
        }
    }
}

void LayoutBlockFlow::addOverhangingFloats(LayoutBlockFlow* child, bool makeChildPaintOtherFloats)
{
    // Prevent floats from being added to the canvas by the root element, e.g., <html>.
    if (!child->containsFloats() || child->createsNewFormattingContext())
        return;

    LayoutUnit childLogicalTop = child->logicalTop();
    LayoutUnit childLogicalLeft = child->logicalLeft();

    // Floats that will remain the child's responsibility to paint should factor into its
    // overflow.
    FloatingObjectSetIterator childEnd = child->m_floatingObjects->set().end();
    for (FloatingObjectSetIterator childIt = child->m_floatingObjects->set().begin(); childIt != childEnd; ++childIt) {
        FloatingObject& floatingObject = *childIt->get();
        LayoutUnit logicalBottomForFloat = std::min(this->logicalBottomForFloat(floatingObject), LayoutUnit::max() - childLogicalTop);
        LayoutUnit logicalBottom = childLogicalTop + logicalBottomForFloat;

        if (logicalBottom > logicalHeight()) {
            // If the object is not in the list, we add it now.
            if (!containsFloat(floatingObject.layoutObject())) {
                LayoutSize offset = isHorizontalWritingMode() ? LayoutSize(-childLogicalLeft, -childLogicalTop) : LayoutSize(-childLogicalTop, -childLogicalLeft);
                bool shouldPaint = false;

                // The nearest enclosing layer always paints the float (so that zindex and stacking
                // behaves properly). We always want to propagate the desire to paint the float as
                // far out as we can, to the outermost block that overlaps the float, stopping only
                // if we hit a self-painting layer boundary.
                if (floatingObject.layoutObject()->enclosingFloatPaintingLayer() == enclosingFloatPaintingLayer() && !floatingObject.isLowestNonOverhangingFloatInChild()) {
                    floatingObject.setShouldPaint(false);
                    shouldPaint = true;
                }
                // We create the floating object list lazily.
                if (!m_floatingObjects)
                    createFloatingObjects();

                m_floatingObjects->add(floatingObject.copyToNewContainer(offset, shouldPaint, true));
            }
        } else {
            if (makeChildPaintOtherFloats && !floatingObject.shouldPaint() && !floatingObject.layoutObject()->hasSelfPaintingLayer() && !floatingObject.isLowestNonOverhangingFloatInChild()
                && floatingObject.layoutObject()->isDescendantOf(child) && floatingObject.layoutObject()->enclosingFloatPaintingLayer() == child->enclosingFloatPaintingLayer()) {
                // The float is not overhanging from this block, so if it is a descendant of the child, the child should
                // paint it (the other case is that it is intruding into the child), unless it has its own layer or enclosing
                // layer.
                // If makeChildPaintOtherFloats is false, it means that the child must already know about all the floats
                // it should paint.
                floatingObject.setShouldPaint(true);
            }

            // Since the float doesn't overhang, it didn't get put into our list. We need to go ahead and add its overflow in to the
            // child now.
            if (floatingObject.isDescendant())
                child->addOverflowFromChild(floatingObject.layoutObject(), LayoutSize(xPositionForFloatIncludingMargin(floatingObject), yPositionForFloatIncludingMargin(floatingObject)));
        }
    }
}

LayoutUnit LayoutBlockFlow::lowestFloatLogicalBottom(FloatingObject::Type floatType) const
{
    if (!m_floatingObjects)
        return LayoutUnit();

    return m_floatingObjects->lowestFloatLogicalBottom(floatType);
}

LayoutUnit LayoutBlockFlow::nextFloatLogicalBottomBelow(LayoutUnit logicalHeight, ShapeOutsideFloatOffsetMode offsetMode) const
{
    if (!m_floatingObjects)
        return logicalHeight;

    LayoutUnit logicalBottom;
    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObjectSetIterator end = floatingObjectSet.end();
    for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
        const FloatingObject& floatingObject = *it->get();
        LayoutUnit floatLogicalBottom = logicalBottomForFloat(floatingObject);
        ShapeOutsideInfo* shapeOutside = floatingObject.layoutObject()->shapeOutsideInfo();
        if (shapeOutside && (offsetMode == ShapeOutsideFloatShapeOffset)) {
            LayoutUnit shapeLogicalBottom = logicalTopForFloat(floatingObject) + marginBeforeForChild(*floatingObject.layoutObject()) + shapeOutside->shapeLogicalBottom();
            // Use the shapeLogicalBottom unless it extends outside of the margin box, in which case it is clipped.
            if (shapeLogicalBottom < floatLogicalBottom)
                floatLogicalBottom = shapeLogicalBottom;
        }
        if (floatLogicalBottom > logicalHeight)
            logicalBottom = logicalBottom ? std::min(floatLogicalBottom, logicalBottom) : floatLogicalBottom;
    }

    return logicalBottom;
}

bool LayoutBlockFlow::hitTestFloats(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset)
{
    if (!m_floatingObjects)
        return false;

    LayoutPoint adjustedLocation = accumulatedOffset;
    if (isLayoutView()) {
        DoublePoint position = toLayoutView(this)->frameView()->scrollPositionDouble();
        adjustedLocation.move(position.x(), position.y());
    }

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObjectSetIterator begin = floatingObjectSet.begin();
    for (FloatingObjectSetIterator it = floatingObjectSet.end(); it != begin;) {
        --it;
        const FloatingObject& floatingObject = *it->get();
        if (floatingObject.shouldPaint() && !floatingObject.layoutObject()->hasSelfPaintingLayer()) {
            LayoutUnit xOffset = xPositionForFloatIncludingMargin(floatingObject) - floatingObject.layoutObject()->location().x();
            LayoutUnit yOffset = yPositionForFloatIncludingMargin(floatingObject) - floatingObject.layoutObject()->location().y();
            LayoutPoint childPoint = flipFloatForWritingModeForChild(floatingObject, adjustedLocation + LayoutSize(xOffset, yOffset));
            if (floatingObject.layoutObject()->hitTest(result, locationInContainer, childPoint)) {
                updateHitTestResult(result, locationInContainer.point() - toLayoutSize(childPoint));
                return true;
            }
        }
    }

    return false;
}

LayoutUnit LayoutBlockFlow::logicalLeftFloatOffsetForLine(LayoutUnit logicalTop, LayoutUnit fixedOffset, LayoutUnit logicalHeight) const
{
    if (m_floatingObjects && m_floatingObjects->hasLeftObjects())
        return m_floatingObjects->logicalLeftOffset(fixedOffset, logicalTop, logicalHeight);

    return fixedOffset;
}

LayoutUnit LayoutBlockFlow::logicalRightFloatOffsetForLine(LayoutUnit logicalTop, LayoutUnit fixedOffset, LayoutUnit logicalHeight) const
{
    if (m_floatingObjects && m_floatingObjects->hasRightObjects())
        return m_floatingObjects->logicalRightOffset(fixedOffset, logicalTop, logicalHeight);

    return fixedOffset;
}

LayoutRect LayoutBlockFlow::selectionRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer) const
{
    LayoutRect rect = selectionGapRectsForPaintInvalidation(paintInvalidationContainer);
    // FIXME: groupedMapping() leaks the squashing abstraction.
    if (paintInvalidationContainer->layer()->groupedMapping())
        DeprecatedPaintLayer::mapRectToPaintBackingCoordinates(paintInvalidationContainer, rect);
    return rect;
}

GapRects LayoutBlockFlow::selectionGapRectsForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer) const
{
    ASSERT(!needsLayout());

    if (!shouldPaintSelectionGaps())
        return GapRects();

    TransformState transformState(TransformState::ApplyTransformDirection, FloatPoint());
    mapLocalToContainer(paintInvalidationContainer, transformState, ApplyContainerFlip | UseTransforms);
    LayoutPoint offsetFromPaintInvalidationContainer = roundedLayoutPoint(transformState.mappedPoint());

    if (hasOverflowClip())
        offsetFromPaintInvalidationContainer -= scrolledContentOffset();

    LayoutUnit lastTop = 0;
    LayoutUnit lastLeft = logicalLeftSelectionOffset(this, lastTop);
    LayoutUnit lastRight = logicalRightSelectionOffset(this, lastTop);

    return selectionGaps(this, offsetFromPaintInvalidationContainer, LayoutSize(), lastTop, lastLeft, lastRight);
}

static void clipOutPositionedObjects(ClipScope& clipScope, const LayoutPoint& offset, TrackedLayoutBoxListHashSet* positionedObjects)
{
    if (!positionedObjects)
        return;

    TrackedLayoutBoxListHashSet::const_iterator end = positionedObjects->end();
    for (TrackedLayoutBoxListHashSet::const_iterator it = positionedObjects->begin(); it != end; ++it) {
        LayoutBox* r = *it;
        clipScope.clip(LayoutRect(flooredIntPoint(r->location() + offset), flooredIntSize(r->size())), SkRegion::kDifference_Op);
    }
}

GapRects LayoutBlockFlow::selectionGaps(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition,
    const LayoutSize& offsetFromRootBlock, LayoutUnit& lastLogicalTop, LayoutUnit& lastLogicalLeft, LayoutUnit& lastLogicalRight,
    const PaintInfo* paintInfo, ClipScope* clipScope) const
{
    // IMPORTANT: Callers of this method that intend for painting to happen need to do a save/restore.
    if (clipScope) {
        // Note that we don't clip out overflow for positioned objects.  We just stick to the border box.
        LayoutRect flippedBlockRect(LayoutPoint(offsetFromRootBlock), size());
        rootBlock->flipForWritingMode(flippedBlockRect);
        flippedBlockRect.moveBy(rootBlockPhysicalPosition);
        clipOutPositionedObjects(*clipScope, flippedBlockRect.location(), positionedObjects());
        if (isBody() || isDocumentElement()) // The <body> must make sure to examine its containingBlock's positioned objects.
            for (LayoutBlock* cb = containingBlock(); cb && !cb->isLayoutView(); cb = cb->containingBlock())
                clipOutPositionedObjects(*clipScope, cb->location(), cb->positionedObjects()); // FIXME: Not right for flipped writing modes.
        clipOutFloatingObjects(rootBlock, *clipScope, rootBlockPhysicalPosition, offsetFromRootBlock);
    }

    GapRects result;

    if (hasTransformRelatedProperty() || style()->columnSpan())
        return result;

    if (childrenInline())
        result = inlineSelectionGaps(rootBlock, rootBlockPhysicalPosition, offsetFromRootBlock, lastLogicalTop, lastLogicalLeft, lastLogicalRight, paintInfo);
    else
        result = blockSelectionGaps(rootBlock, rootBlockPhysicalPosition, offsetFromRootBlock, lastLogicalTop, lastLogicalLeft, lastLogicalRight, paintInfo);

    // Go ahead and fill the vertical gap all the way to the bottom of our block if the selection extends past our block.
    if (rootBlock == this && (selectionState() != SelectionBoth && selectionState() != SelectionEnd)) {
        result.uniteCenter(blockSelectionGap(rootBlock, rootBlockPhysicalPosition, offsetFromRootBlock,
            lastLogicalTop, lastLogicalLeft, lastLogicalRight, logicalHeight(), paintInfo));
    }
    return result;
}


GapRects LayoutBlockFlow::inlineSelectionGaps(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
    LayoutUnit& lastLogicalTop, LayoutUnit& lastLogicalLeft, LayoutUnit& lastLogicalRight, const PaintInfo* paintInfo) const
{
    GapRects result;

    bool containsStart = selectionState() == SelectionStart || selectionState() == SelectionBoth;

    if (!firstLineBox()) {
        if (containsStart) {
            // Go ahead and update our lastLogicalTop to be the bottom of the block.  <hr>s or empty blocks with height can trip this
            // case.
            lastLogicalTop = rootBlock->blockDirectionOffset(offsetFromRootBlock) + logicalHeight();
            lastLogicalLeft = logicalLeftSelectionOffset(rootBlock, logicalHeight());
            lastLogicalRight = logicalRightSelectionOffset(rootBlock, logicalHeight());
        }
        return result;
    }

    RootInlineBox* lastSelectedLine = nullptr;
    RootInlineBox* curr;
    for (curr = firstRootBox(); curr && !curr->hasSelectedChildren(); curr = curr->nextRootBox()) { }

    // Now paint the gaps for the lines.
    for (; curr && curr->hasSelectedChildren(); curr = curr->nextRootBox()) {
        LayoutUnit selTop =  curr->selectionTopAdjustedForPrecedingBlock();
        LayoutUnit selHeight = curr->selectionHeightAdjustedForPrecedingBlock();

        if (!containsStart && !lastSelectedLine && selectionState() != SelectionStart && selectionState() != SelectionBoth) {
            result.uniteCenter(blockSelectionGap(rootBlock, rootBlockPhysicalPosition, offsetFromRootBlock, lastLogicalTop,
                lastLogicalLeft, lastLogicalRight, selTop, paintInfo));
        }

        LayoutRect logicalRect(curr->logicalLeft(), selTop, curr->logicalWidth(), selTop + selHeight);
        logicalRect.move(isHorizontalWritingMode() ? offsetFromRootBlock : offsetFromRootBlock.transposedSize());
        LayoutRect physicalRect = rootBlock->logicalRectToPhysicalRect(rootBlockPhysicalPosition, logicalRect);
        if (!paintInfo || (isHorizontalWritingMode() && physicalRect.y() < paintInfo->rect.maxY() && physicalRect.maxY() > paintInfo->rect.y())
            || (!isHorizontalWritingMode() && physicalRect.x() < paintInfo->rect.maxX() && physicalRect.maxX() > paintInfo->rect.x()))
            result.unite(curr->lineSelectionGap(rootBlock, rootBlockPhysicalPosition, offsetFromRootBlock, selTop, selHeight, paintInfo));

        lastSelectedLine = curr;
    }

    if (containsStart && !lastSelectedLine) {
        // VisibleSelection must start just after our last line.
        lastSelectedLine = lastRootBox();
    }

    if (lastSelectedLine && selectionState() != SelectionEnd && selectionState() != SelectionBoth) {
        // Go ahead and update our lastY to be the bottom of the last selected line.
        lastLogicalTop = rootBlock->blockDirectionOffset(offsetFromRootBlock) + lastSelectedLine->selectionBottom();
        lastLogicalLeft = logicalLeftSelectionOffset(rootBlock, lastSelectedLine->selectionBottom());
        lastLogicalRight = logicalRightSelectionOffset(rootBlock, lastSelectedLine->selectionBottom());
    }
    return result;
}

IntRect alignSelectionRectToDevicePixels(LayoutRect& rect)
{
    LayoutUnit roundedX = rect.x().round();
    return IntRect(roundedX, rect.y().round(),
        (rect.maxX() - roundedX).round(),
        snapSizeToPixel(rect.height(), rect.y()));
}

bool LayoutBlockFlow::shouldPaintSelectionGaps() const
{
    return selectionState() != SelectionNone && style()->visibility() == VISIBLE && isSelectionRoot();
}

LayoutRect LayoutBlockFlow::blockSelectionGap(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
    LayoutUnit lastLogicalTop, LayoutUnit lastLogicalLeft, LayoutUnit lastLogicalRight, LayoutUnit logicalBottom, const PaintInfo* paintInfo) const
{
    LayoutUnit logicalTop = lastLogicalTop;
    LayoutUnit logicalHeight = rootBlock->blockDirectionOffset(offsetFromRootBlock) + logicalBottom - logicalTop;
    if (logicalHeight <= 0)
        return LayoutRect();

    // Get the selection offsets for the bottom of the gap
    LayoutUnit logicalLeft = std::max(lastLogicalLeft, logicalLeftSelectionOffset(rootBlock, logicalBottom));
    LayoutUnit logicalRight = std::min(lastLogicalRight, logicalRightSelectionOffset(rootBlock, logicalBottom));
    LayoutUnit logicalWidth = logicalRight - logicalLeft;
    if (logicalWidth <= 0)
        return LayoutRect();

    LayoutRect gapRect = rootBlock->logicalRectToPhysicalRect(rootBlockPhysicalPosition, LayoutRect(logicalLeft, logicalTop, logicalWidth, logicalHeight));
    if (paintInfo) {
        IntRect selectionGapRect = alignSelectionRectToDevicePixels(gapRect);
        paintInfo->context->fillRect(selectionGapRect, selectionBackgroundColor());
    }
    return gapRect;
}

GapRects LayoutBlockFlow::blockSelectionGaps(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
    LayoutUnit& lastLogicalTop, LayoutUnit& lastLogicalLeft, LayoutUnit& lastLogicalRight, const PaintInfo* paintInfo) const
{
    GapRects result;

    // Go ahead and jump right to the first block child that contains some selected objects.
    LayoutBox* curr;
    for (curr = firstChildBox(); curr && curr->selectionState() == SelectionNone; curr = curr->nextSiblingBox()) { }

    for (bool sawSelectionEnd = false; curr && !sawSelectionEnd; curr = curr->nextSiblingBox()) {
        SelectionState childState = curr->selectionState();
        if (childState == SelectionBoth || childState == SelectionEnd)
            sawSelectionEnd = true;

        if (curr->isFloatingOrOutOfFlowPositioned())
            continue; // We must be a normal flow object in order to even be considered.

        if (curr->isRelPositioned() && curr->hasLayer()) {
            // If the relposition offset is anything other than 0, then treat this just like an absolute positioned element.
            // Just disregard it completely.
            LayoutSize relOffset = curr->layer()->offsetForInFlowPosition();
            if (relOffset.width() || relOffset.height())
                continue;
        }

        bool paintsOwnSelection = curr->shouldPaintSelectionGaps() || curr->isTable(); // FIXME: Eventually we won't special-case table like this.
        bool fillBlockGaps = paintsOwnSelection || (curr->canBeSelectionLeaf() && childState != SelectionNone);
        if (fillBlockGaps) {
            // We need to fill the vertical gap above this object.
            if (childState == SelectionEnd || childState == SelectionInside) {
                // Fill the gap above the object.
                result.uniteCenter(blockSelectionGap(rootBlock, rootBlockPhysicalPosition, offsetFromRootBlock, lastLogicalTop, lastLogicalLeft, lastLogicalRight,
                    curr->logicalTop(), paintInfo));
            }

            // Only fill side gaps for objects that paint their own selection if we know for sure the selection is going to extend all the way *past*
            // our object.  We know this if the selection did not end inside our object.
            if (paintsOwnSelection && (childState == SelectionStart || sawSelectionEnd))
                childState = SelectionNone;

            // Fill side gaps on this object based off its state.
            bool leftGap, rightGap;
            getSelectionGapInfo(childState, leftGap, rightGap);

            if (leftGap)
                result.uniteLeft(logicalLeftSelectionGap(rootBlock, rootBlockPhysicalPosition, offsetFromRootBlock, this, curr->logicalLeft(), curr->logicalTop(), curr->logicalHeight(), paintInfo));
            if (rightGap)
                result.uniteRight(logicalRightSelectionGap(rootBlock, rootBlockPhysicalPosition, offsetFromRootBlock, this, curr->logicalRight(), curr->logicalTop(), curr->logicalHeight(), paintInfo));

            // Update lastLogicalTop to be just underneath the object.  lastLogicalLeft and lastLogicalRight extend as far as
            // they can without bumping into floating or positioned objects.  Ideally they will go right up
            // to the border of the root selection block.
            lastLogicalTop = rootBlock->blockDirectionOffset(offsetFromRootBlock) + curr->logicalBottom();
            lastLogicalLeft = logicalLeftSelectionOffset(rootBlock, curr->logicalBottom());
            lastLogicalRight = logicalRightSelectionOffset(rootBlock, curr->logicalBottom());
        } else if (childState != SelectionNone && curr->isLayoutBlockFlow()) {
            // We must be a block that has some selected object inside it.  Go ahead and recur.
            result.unite(toLayoutBlockFlow(curr)->selectionGaps(rootBlock, rootBlockPhysicalPosition, LayoutSize(offsetFromRootBlock.width() + curr->location().x(), offsetFromRootBlock.height() + curr->location().y()),
                lastLogicalTop, lastLogicalLeft, lastLogicalRight, paintInfo));
        }
    }
    return result;
}

LayoutRect LayoutBlockFlow::logicalLeftSelectionGap(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
    const LayoutObject* selObj, LayoutUnit logicalLeft, LayoutUnit logicalTop, LayoutUnit logicalHeight, const PaintInfo* paintInfo) const
{
    LayoutUnit rootBlockLogicalTop = rootBlock->blockDirectionOffset(offsetFromRootBlock) + logicalTop;
    LayoutUnit rootBlockLogicalLeft = std::max(logicalLeftSelectionOffset(rootBlock, logicalTop), logicalLeftSelectionOffset(rootBlock, logicalTop + logicalHeight));
    LayoutUnit rootBlockLogicalRight = std::min(rootBlock->inlineDirectionOffset(offsetFromRootBlock) + logicalLeft, std::min(logicalRightSelectionOffset(rootBlock, logicalTop), logicalRightSelectionOffset(rootBlock, logicalTop + logicalHeight)));
    LayoutUnit rootBlockLogicalWidth = rootBlockLogicalRight - rootBlockLogicalLeft;
    if (rootBlockLogicalWidth <= 0)
        return LayoutRect();

    LayoutRect gapRect = rootBlock->logicalRectToPhysicalRect(rootBlockPhysicalPosition, LayoutRect(rootBlockLogicalLeft, rootBlockLogicalTop, rootBlockLogicalWidth, logicalHeight));
    if (paintInfo) {
        IntRect selectionGapRect = alignSelectionRectToDevicePixels(gapRect);
        paintInfo->context->fillRect(selectionGapRect, selObj->selectionBackgroundColor());
    }
    return gapRect;
}

LayoutRect LayoutBlockFlow::logicalRightSelectionGap(const LayoutBlock* rootBlock, const LayoutPoint& rootBlockPhysicalPosition, const LayoutSize& offsetFromRootBlock,
    const LayoutObject* selObj, LayoutUnit logicalRight, LayoutUnit logicalTop, LayoutUnit logicalHeight, const PaintInfo* paintInfo) const
{
    LayoutUnit rootBlockLogicalTop = rootBlock->blockDirectionOffset(offsetFromRootBlock) + logicalTop;
    LayoutUnit rootBlockLogicalLeft = std::max(rootBlock->inlineDirectionOffset(offsetFromRootBlock) + logicalRight, max(logicalLeftSelectionOffset(rootBlock, logicalTop), logicalLeftSelectionOffset(rootBlock, logicalTop + logicalHeight)));
    LayoutUnit rootBlockLogicalRight = std::min(logicalRightSelectionOffset(rootBlock, logicalTop), logicalRightSelectionOffset(rootBlock, logicalTop + logicalHeight));
    LayoutUnit rootBlockLogicalWidth = rootBlockLogicalRight - rootBlockLogicalLeft;
    if (rootBlockLogicalWidth <= 0)
        return LayoutRect();

    LayoutRect gapRect = rootBlock->logicalRectToPhysicalRect(rootBlockPhysicalPosition, LayoutRect(rootBlockLogicalLeft, rootBlockLogicalTop, rootBlockLogicalWidth, logicalHeight));
    if (paintInfo) {
        IntRect selectionGapRect = alignSelectionRectToDevicePixels(gapRect);
        paintInfo->context->fillRect(selectionGapRect, selObj->selectionBackgroundColor());
    }
    return gapRect;
}

void LayoutBlockFlow::getSelectionGapInfo(SelectionState state, bool& leftGap, bool& rightGap) const
{
    bool ltr = style()->isLeftToRightDirection();
    leftGap = (state == LayoutObject::SelectionInside)
        || (state == LayoutObject::SelectionEnd && ltr)
        || (state == LayoutObject::SelectionStart && !ltr);
    rightGap = (state == LayoutObject::SelectionInside)
        || (state == LayoutObject::SelectionStart && ltr)
        || (state == LayoutObject::SelectionEnd && !ltr);
}

void LayoutBlockFlow::setPaginationStrut(LayoutUnit strut)
{
    if (!m_rareData) {
        if (!strut)
            return;
        m_rareData = adoptPtr(new LayoutBlockFlowRareData(this));
    }
    m_rareData->m_paginationStrut = strut;
}

void LayoutBlockFlow::positionSpannerDescendant(LayoutMultiColumnSpannerPlaceholder& child)
{
    LayoutBox& spanner = *child.layoutObjectInFlowThread();
    // FIXME: |spanner| is a descendant, but never a direct child, so the names here are bad, if
    // nothing else.
    setLogicalTopForChild(spanner, child.logicalTop());
    determineLogicalLeftPositionForChild(spanner);
}

bool LayoutBlockFlow::avoidsFloats() const
{
    // Floats can't intrude into our box if we have a non-auto column count or width.
    // Note: we need to use LayoutBox::avoidsFloats here since LayoutBlock::avoidsFloats is always true.
    return LayoutBox::avoidsFloats() || !style()->hasAutoColumnCount() || !style()->hasAutoColumnWidth();
}

void LayoutBlockFlow::moveChildrenTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* startChild, LayoutObject* endChild, LayoutObject* beforeChild, bool fullRemoveInsert)
{
    if (childrenInline())
        deleteLineBoxTree();
    LayoutBoxModelObject::moveChildrenTo(toBoxModelObject, startChild, endChild, beforeChild, fullRemoveInsert);
}

LayoutUnit LayoutBlockFlow::logicalLeftSelectionOffset(const LayoutBlock* rootBlock, LayoutUnit position) const
{
    LayoutUnit logicalLeft = logicalLeftOffsetForLine(position, false);
    if (logicalLeft == logicalLeftOffsetForContent())
        return LayoutBlock::logicalLeftSelectionOffset(rootBlock, position);

    const LayoutBlock* cb = this;
    while (cb != rootBlock) {
        logicalLeft += cb->logicalLeft();
        cb = cb->containingBlock();
    }
    return logicalLeft;
}

LayoutUnit LayoutBlockFlow::logicalRightSelectionOffset(const LayoutBlock* rootBlock, LayoutUnit position) const
{
    LayoutUnit logicalRight = logicalRightOffsetForLine(position, false);
    if (logicalRight == logicalRightOffsetForContent())
        return LayoutBlock::logicalRightSelectionOffset(rootBlock, position);

    const LayoutBlock* cb = this;
    while (cb != rootBlock) {
        logicalRight += cb->logicalLeft();
        cb = cb->containingBlock();
    }
    return logicalRight;
}

RootInlineBox* LayoutBlockFlow::createRootInlineBox()
{
    return new RootInlineBox(*this);
}

bool LayoutBlockFlow::isPagedOverflow(const ComputedStyle& style)
{
    return style.isOverflowPaged() && node() != document().viewportDefiningElement();
}

LayoutBlockFlow::FlowThreadType LayoutBlockFlow::flowThreadType(const ComputedStyle& style)
{
    if (isPagedOverflow(style))
        return PagedFlowThread;
    if (style.specifiesColumns())
        return MultiColumnFlowThread;
    return NoFlowThread;
}

LayoutMultiColumnFlowThread* LayoutBlockFlow::createMultiColumnFlowThread(FlowThreadType type)
{
    switch (type) {
    case MultiColumnFlowThread:
        return LayoutMultiColumnFlowThread::createAnonymous(document(), styleRef());
    case PagedFlowThread:
        // Paged overflow is currently done using the multicol implementation.
        return LayoutPagedFlowThread::createAnonymous(document(), styleRef());
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

void LayoutBlockFlow::createOrDestroyMultiColumnFlowThreadIfNeeded(const ComputedStyle* oldStyle)
{
    // Paged overflow trumps multicol in this implementation. Ideally, it should be possible to have
    // both paged overflow and multicol on the same element, but then we need two flow
    // threads. Anyway, this is nothing to worry about until we can actually nest multicol properly
    // inside other fragmentation contexts.
    FlowThreadType type = flowThreadType(styleRef());

    if (multiColumnFlowThread()) {
        ASSERT(oldStyle);
        if (type != flowThreadType(*oldStyle)) {
            // If we're no longer to be multicol/paged, destroy the flow thread. Also destroy it
            // when switching between multicol and paged, since that affects the column set
            // structure (multicol containers may have spanners, paged containers may not).
            multiColumnFlowThread()->evacuateAndDestroy();
            ASSERT(!multiColumnFlowThread());
        }
    }

    if (type == NoFlowThread || multiColumnFlowThread())
        return;

    // Ruby elements manage child insertion in a special way, and would mess up insertion of the
    // flow thread. The flow thread needs to be a direct child of the multicol block (|this|).
    if (isRuby())
        return;

    // Fieldsets look for a legend special child (layoutSpecialExcludedChild()). We currently only
    // support one special child per layout object, and the flow thread would make for a second one.
    if (isFieldset())
        return;

    // Form controls are replaced content, and are therefore not supposed to support multicol.
    if (isFileUploadControl() || isTextControl() || isListBox())
        return;

    LayoutMultiColumnFlowThread* flowThread = createMultiColumnFlowThread(type);
    addChild(flowThread);

    // Check that addChild() put the flow thread as a direct child, and didn't do fancy things.
    ASSERT(flowThread->parent() == this);

    flowThread->populate();
    LayoutBlockFlowRareData& rareData = ensureRareData();
    ASSERT(!rareData.m_multiColumnFlowThread);
    rareData.m_multiColumnFlowThread = flowThread;
}

LayoutBlockFlow::LayoutBlockFlowRareData& LayoutBlockFlow::ensureRareData()
{
    if (m_rareData)
        return *m_rareData;

    m_rareData = adoptPtr(new LayoutBlockFlowRareData(this));
    return *m_rareData;
}

void LayoutBlockFlow::positionDialog()
{
    HTMLDialogElement* dialog = toHTMLDialogElement(node());
    if (dialog->centeringMode() == HTMLDialogElement::NotCentered)
        return;

    bool canCenterDialog = (style()->position() == AbsolutePosition || style()->position() == FixedPosition)
        && style()->hasAutoTopAndBottom();

    if (dialog->centeringMode() == HTMLDialogElement::Centered) {
        if (canCenterDialog)
            setY(dialog->centeredPosition());
        return;
    }

    ASSERT(dialog->centeringMode() == HTMLDialogElement::NeedsCentering);
    if (!canCenterDialog) {
        dialog->setNotCentered();
        return;
    }

    FrameView* frameView = document().view();
    LayoutUnit top = (style()->position() == FixedPosition) ? 0 : frameView->scrollOffset().height();
    int visibleHeight = frameView->visibleContentRect(IncludeScrollbars).height();
    if (size().height() < visibleHeight)
        top += (visibleHeight - size().height()) / 2;
    setY(top);
    dialog->setCentered(top);
}

} // namespace blink
