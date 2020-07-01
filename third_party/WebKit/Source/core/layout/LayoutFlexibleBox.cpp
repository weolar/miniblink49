/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#include "core/layout/LayoutFlexibleBox.h"

#include "core/frame/UseCounter.h"
#include "core/layout/LayoutState.h"
#include "core/layout/TextAutosizer.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/style/ComputedStyle.h"
#include "platform/LengthFunctions.h"
#include "wtf/MathExtras.h"
#include <limits>

namespace blink {

static bool hasAspectRatio(const LayoutBox& child)
{
    return child.isImage() || child.isCanvas() || child.isVideo();
}

static inline ContentDistributionType resolvedContentAlignmentDistribution(const StyleContentAlignmentData& value, const StyleContentAlignmentData& normalValueBehavior)
{
    return (value.position() == ContentPositionAuto && value.distribution() == ContentDistributionDefault) ? normalValueBehavior.distribution() : value.distribution();
}

static ContentDistributionType resolvedJustifyContentDistribution(const ComputedStyle& style, const StyleContentAlignmentData& normalValueBehavior)
{
    return resolvedContentAlignmentDistribution(style.justifyContent(), normalValueBehavior);
}

static inline ContentPosition resolvedContentAlignmentPosition(const StyleContentAlignmentData& value, const StyleContentAlignmentData& normalValueBehavior)
{
    return (value.position() == ContentPositionAuto && value.distribution() == ContentDistributionDefault) ? normalValueBehavior.position() : value.position();
}

static ContentPosition resolvedJustifyContentPosition(const ComputedStyle& style, const StyleContentAlignmentData& normalValueBehavior)
{
    return resolvedContentAlignmentPosition(style.justifyContent(), normalValueBehavior);
}

static ContentPosition resolvedAlignContentPosition(const ComputedStyle& style, const StyleContentAlignmentData& normalValueBehavior)
{
    return resolvedContentAlignmentPosition(style.alignContent(), normalValueBehavior);
}

static ContentDistributionType resolvedAlignContentDistribution(const ComputedStyle& style, const StyleContentAlignmentData& normalValueBehavior)
{
    return resolvedContentAlignmentDistribution(style.alignContent(), normalValueBehavior);
}

class PaintLayerScrollableArea {
public:
    // If a PreventRelayoutScope object is alive, updateAfterLayout() will not
    // re-run box layout as a result of adding or removing scrollbars.
    // Instead, it will mark the PLSA as needing relayout of its box.
    // When the last PreventRelayoutScope object is popped off the stack,
    // box().setNeedsLayout(), and box().scrollbarsChanged() for LayoutBlock's,
    // will be called as appropriate for all marked PLSA's.
    class PreventRelayoutScope {
        STACK_ALLOCATED();
    public:
        PreventRelayoutScope(SubtreeLayoutScope& layoutScope)
        {
            if (!s_count) {
                ASSERT(!s_layoutScope);
                s_layoutScope = &layoutScope;
            }
            s_count++;
        }
        ~PreventRelayoutScope()
        {
            if (--s_count == 0) {
                s_layoutScope = nullptr;
            }
        }

        static bool relayoutIsPrevented() { return s_count; }
        static bool relayoutNeeded() { return s_count == 0 && s_relayoutNeeded; }
        static void resetRelayoutNeeded()
        {
            ASSERT(s_count == 0);
            s_relayoutNeeded = false;
        }

    private:
        static int s_count;
        static SubtreeLayoutScope* s_layoutScope;
        static bool s_relayoutNeeded;
    };

    // If a FreezeScrollbarScope object is alive, updateAfterLayout() will not
    // recompute the existence of overflow:auto scrollbars.
    class FreezeScrollbarsScope {
        STACK_ALLOCATED();
    public:
        FreezeScrollbarsScope() { s_count++; }
        ~FreezeScrollbarsScope() { s_count--; }

        static bool scrollbarsAreFrozen() { return s_count; }

    private:
        static int s_count;
    };

    class DelayScrollPositionClampScope {
        STACK_ALLOCATED();
    public:
        DelayScrollPositionClampScope()
        {
            s_count++;
        }

        ~DelayScrollPositionClampScope()
        {
            --s_count;
        }

        static bool clampingIsDelayed() { return s_count; }

    private:
        static int s_count;
    };
};

int PaintLayerScrollableArea::PreventRelayoutScope::s_count = 0;
int PaintLayerScrollableArea::DelayScrollPositionClampScope::s_count = 0;
int PaintLayerScrollableArea::FreezeScrollbarsScope::s_count = 0;
SubtreeLayoutScope* PaintLayerScrollableArea::PreventRelayoutScope::s_layoutScope = nullptr;
bool PaintLayerScrollableArea::PreventRelayoutScope::s_relayoutNeeded = false;

struct LayoutFlexibleBox::LineContext {
    LineContext(LayoutUnit crossAxisOffset, LayoutUnit crossAxisExtent, size_t numberOfChildren, LayoutUnit maxAscent)
        : crossAxisOffset(crossAxisOffset)
        , crossAxisExtent(crossAxisExtent)
        , numberOfChildren(numberOfChildren)
        , maxAscent(maxAscent)
    {
    }

    LayoutUnit crossAxisOffset;
    LayoutUnit crossAxisExtent;
    size_t numberOfChildren;
    LayoutUnit maxAscent;
};

struct LayoutFlexibleBox::FlexItem {
    FlexItem(LayoutBox* box, LayoutUnit innerFlexBaseSize, LayoutUnit hypotheticalMainSize)
        : box(box)
        , innerFlexBaseSize(innerFlexBaseSize)
        , hypotheticalMainSize(hypotheticalMainSize)
        , frozen(false)
    {
    }

    // This constructor is used for out-of-flow children
    explicit FlexItem(LayoutBox* box)
        : box(box)
        , innerFlexBaseSize()
        , hypotheticalMainSize()
        , frozen(true)
    {
    }
    LayoutBox* box;
    const LayoutUnit innerFlexBaseSize;
    const LayoutUnit hypotheticalMainSize;
    LayoutUnit flexedContentSize;
    bool frozen;
};

LayoutFlexibleBox::LayoutFlexibleBox(Element* element)
    : LayoutBlock(element)
    , m_orderIterator(this)
    , m_numberOfInFlowChildrenOnFirstLine(-1)
    , m_hasDefiniteHeight(SizeDefiniteness::Unknown)
{
    ASSERT(!childrenInline());
}

LayoutFlexibleBox::~LayoutFlexibleBox()
{
}

LayoutFlexibleBox* LayoutFlexibleBox::createAnonymous(Document* document)
{
    LayoutFlexibleBox* layoutObject = new LayoutFlexibleBox(nullptr);
    layoutObject->setDocumentForAnonymous(document);
    return layoutObject;
}

void LayoutFlexibleBox::computeChildPreferredLogicalWidths(LayoutObject& child, LayoutUnit& minPreferredLogicalWidth, LayoutUnit& maxPreferredLogicalWidth) const
{
    if (child.isBox() && child.isHorizontalWritingMode() != isHorizontalWritingMode()) {
        // If the child is an orthogonal flow, child's height determines the width, but the height is not available until layout.
        // http://dev.w3.org/csswg/css-writing-modes-3/#orthogonal-shrink-to-fit
        if (!child.needsLayout()) {
            minPreferredLogicalWidth = maxPreferredLogicalWidth = toLayoutBox(child).logicalHeight();
            return;
        }
        minPreferredLogicalWidth = maxPreferredLogicalWidth = toLayoutBox(child).computeLogicalHeightWithoutLayout();
        return;
    }
    minPreferredLogicalWidth = child.minPreferredLogicalWidth();
    maxPreferredLogicalWidth = child.maxPreferredLogicalWidth();

    // For non-replaced blocks if the inline size is min|max-content or a definite size the min|max-content contribution
    // is that size plus border, padding and margin https://drafts.csswg.org/css-sizing/#block-intrinsic
    if (child.isLayoutBlock()) {
        const Length& computedInlineSize = child.styleRef().logicalWidth();
        if (computedInlineSize.isMaxContent())
            minPreferredLogicalWidth = maxPreferredLogicalWidth;
        else if (computedInlineSize.isMinContent())
            maxPreferredLogicalWidth = minPreferredLogicalWidth;
    }
}

void LayoutFlexibleBox::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    if (firstChild() && (firstChild()->isText()))
        return;

    // FIXME: We're ignoring flex-basis here and we shouldn't. We can't start honoring it though until
    // the flex shorthand stops setting it to 0.
    // See https://bugs.webkit.org/show_bug.cgi?id=116117 and http://crbug.com/240765.
    float previousMaxContentFlexFraction = -1;
    for (LayoutBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
        if (child->isOutOfFlowPositioned())
            continue;

        LayoutUnit margin = marginIntrinsicLogicalWidthForChild(*child);

        LayoutUnit minPreferredLogicalWidth;
        LayoutUnit maxPreferredLogicalWidth;
        computeChildPreferredLogicalWidths(*child, minPreferredLogicalWidth, maxPreferredLogicalWidth);
        ASSERT(minPreferredLogicalWidth >= 0);
        ASSERT(maxPreferredLogicalWidth >= 0);
        minPreferredLogicalWidth += margin;
        maxPreferredLogicalWidth += margin;
        if (!isColumnFlow()) {
            maxLogicalWidth += maxPreferredLogicalWidth;
            if (isMultiline()) {
                // For multiline, the min preferred width is if you put a break between each item.
                minLogicalWidth = std::max(minLogicalWidth, minPreferredLogicalWidth);
            } else {
                minLogicalWidth += minPreferredLogicalWidth;
            }
        } else {
            minLogicalWidth = std::max(minPreferredLogicalWidth, minLogicalWidth);
            maxLogicalWidth = std::max(maxPreferredLogicalWidth, maxLogicalWidth);
        }

        previousMaxContentFlexFraction = countIntrinsicSizeForAlgorithmChange(maxPreferredLogicalWidth, child, previousMaxContentFlexFraction);
    }

    maxLogicalWidth = std::max(minLogicalWidth, maxLogicalWidth);

    // Due to negative margins, it is possible that we calculated a negative intrinsic width. Make sure that we
    // never return a negative width.
    minLogicalWidth = std::max(LayoutUnit(), minLogicalWidth);
    maxLogicalWidth = std::max(LayoutUnit(), maxLogicalWidth);

    LayoutUnit scrollbarWidth(scrollbarLogicalWidth());
    maxLogicalWidth += scrollbarWidth;
    minLogicalWidth += scrollbarWidth;
}

float LayoutFlexibleBox::countIntrinsicSizeForAlgorithmChange(LayoutUnit maxPreferredLogicalWidth, LayoutBox* child, float previousMaxContentFlexFraction) const
{
    // Determine whether the new version of the intrinsic size algorithm of the flexbox
    // spec would produce a different result than our above algorithm.
    // The algorithm produces a different result iff the max-content flex fraction
    // (as defined in the new algorithm) is not identical for each flex item.
    if (isColumnFlow())
        return previousMaxContentFlexFraction;
    Length flexBasis = child->styleRef().flexBasis();
    float flexGrow = child->styleRef().flexGrow();
    // A flex-basis of auto will lead to a max-content flex fraction of zero, so just like
    // an inflexible item it would compute to a size of max-content, so we ignore it here.
    if (flexBasis.isAuto() || flexGrow == 0)
        return previousMaxContentFlexFraction;
    flexGrow = std::max(1.0f, flexGrow);
    float maxContentFlexFraction = maxPreferredLogicalWidth.toFloat() / flexGrow;
//     if (previousMaxContentFlexFraction != -1 && maxContentFlexFraction != previousMaxContentFlexFraction)
//         UseCounter::count(document(), UseCounter::FlexboxIntrinsicSizeAlgorithmIsDifferent);
    return maxContentFlexFraction;
}

static int synthesizedBaselineFromContentBox(const LayoutBox& box, LineDirectionMode direction)
{
    if (direction == HorizontalLine) {
        return box.size().height() - box.borderBottom() - box.paddingBottom() - box.verticalScrollbarWidth();
    }
    return box.size().width() - box.borderLeft() - box.paddingLeft() - box.horizontalScrollbarHeight();
}

int LayoutFlexibleBox::baselinePosition(FontBaseline, bool, LineDirectionMode direction, LinePositionMode mode) const
{
    ASSERT(mode == PositionOnContainingLine);
    int baseline = firstLineBoxBaseline();
    if (baseline == -1)
        baseline = synthesizedBaselineFromContentBox(*this, direction);

    return beforeMarginInLineDirection(direction) + baseline;
}

static const StyleContentAlignmentData& normalValueBehavior()
{
    // The justify-content property applies along the main axis, but since flexing
    // in the main axis is controlled by flex, stretch behaves as flex-start (ignoring
    // the specified fallback alignment, if any).
    // https://drafts.csswg.org/css-align/#distribution-flex
    static const StyleContentAlignmentData normalBehavior = { ContentPositionAuto, ContentDistributionStretch };
    return normalBehavior;
}

int LayoutFlexibleBox::firstLineBoxBaseline() const
{
    if (isWritingModeRoot() || m_numberOfInFlowChildrenOnFirstLine <= 0)
        return -1;
    LayoutBox* baselineChild = nullptr;
    int childNumber = 0;
    for (LayoutBox* child = m_orderIterator.first(); child; child = m_orderIterator.next()) {
        if (child->isOutOfFlowPositioned())
            continue;
        if (alignmentForChild(*child) == ItemPositionBaseline && !hasAutoMarginsInCrossAxis(*child)) {
            baselineChild = child;
            break;
        }
        if (!baselineChild)
            baselineChild = child;

        ++childNumber;
        if (childNumber == m_numberOfInFlowChildrenOnFirstLine)
            break;
    }

    if (!baselineChild)
        return -1;

    if (!isColumnFlow() && hasOrthogonalFlow(*baselineChild))
        return crossAxisExtentForChild(*baselineChild) + baselineChild->logicalTop();
    if (isColumnFlow() && !hasOrthogonalFlow(*baselineChild))
        return mainAxisExtentForChild(*baselineChild) + baselineChild->logicalTop();

    int baseline = baselineChild->firstLineBoxBaseline();
    if (baseline == -1) {
        // FIXME: We should pass |direction| into firstLineBoxBaseline and stop bailing out if we're a writing mode root.
        // This would also fix some cases where the flexbox is orthogonal to its container.
        LineDirectionMode direction = isHorizontalWritingMode() ? HorizontalLine : VerticalLine;
        return synthesizedBaselineFromContentBox(*baselineChild, direction) + baselineChild->logicalTop();
    }

    return baseline + baselineChild->logicalTop();
}

int LayoutFlexibleBox::inlineBlockBaseline(LineDirectionMode direction) const
{
    int baseline = firstLineBoxBaseline();
    if (baseline != -1)
        return baseline;

    int marginAscent = direction == HorizontalLine ? marginTop() : marginRight();
    return synthesizedBaselineFromContentBox(*this, direction) + marginAscent;
}

IntSize LayoutFlexibleBox::originAdjustmentForScrollbars() const
{
    IntSize size;
    int adjustmentWidth = verticalScrollbarWidth();
    int adjustmentHeight = horizontalScrollbarHeight();
    if (!adjustmentWidth && !adjustmentHeight)
        return size;

    EFlexDirection flexDirection = style()->flexDirection();
    TextDirection textDirection = style()->direction();
    WritingMode writingMode = style()->writingMode();

    if (flexDirection == FlowRow) {
        if (textDirection == RTL) {
            if (writingMode == TopToBottomWritingMode)
                size.expand(adjustmentWidth, 0);
            else
                size.expand(0, adjustmentHeight);
        }
        if (writingMode == RightToLeftWritingMode)
            size.expand(adjustmentWidth, 0);
    } else if (flexDirection == FlowRowReverse) {
        if (textDirection == LTR) {
            if (writingMode == TopToBottomWritingMode)
                size.expand(adjustmentWidth, 0);
            else
                size.expand(0, adjustmentHeight);
        }
        if (writingMode == RightToLeftWritingMode)
            size.expand(adjustmentWidth, 0);
    } else if (flexDirection == FlowColumn) {
        if (writingMode == RightToLeftWritingMode)
            size.expand(adjustmentWidth, 0);
    } else {
        if (writingMode == TopToBottomWritingMode)
            size.expand(0, adjustmentHeight);
        else if (writingMode == LeftToRightWritingMode)
            size.expand(adjustmentWidth, 0);
    }
    return size;
}

bool LayoutFlexibleBox::hasTopOverflow() const
{
    EFlexDirection flexDirection = style()->flexDirection();
    if (isHorizontalWritingMode())
        return flexDirection == FlowColumnReverse;
    return flexDirection == (style()->isLeftToRightDirection() ? FlowRowReverse : FlowRow);
}

bool LayoutFlexibleBox::hasLeftOverflow() const
{
    EFlexDirection flexDirection = style()->flexDirection();
    if (isHorizontalWritingMode())
        return flexDirection == (style()->isLeftToRightDirection() ? FlowRowReverse : FlowRow);
    return flexDirection == FlowColumnReverse;
}

void LayoutFlexibleBox::removeChild(LayoutObject* child)
{
    LayoutBlock::removeChild(child);
    m_intrinsicSizeAlongMainAxis.remove(child);
}

void LayoutFlexibleBox::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBlock::styleDidChange(diff, oldStyle);

    if (oldStyle && oldStyle->alignItemsPosition() == ItemPositionStretch && diff.needsFullLayout()) {
        if (firstChild() && (firstChild()->isText()))
            return;

        // Flex items that were previously stretching need to be relayed out so we can compute new available cross axis space.
        // This is only necessary for stretching since other alignment values don't change the size of the box.
        for (LayoutBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
            ItemPosition previousAlignment = ComputedStyle::resolveAlignment(*oldStyle, child->styleRef(), ItemPositionStretch);
            if (previousAlignment == ItemPositionStretch && previousAlignment != ComputedStyle::resolveAlignment(styleRef(), child->styleRef(), ItemPositionStretch))
                child->setChildNeedsLayout(MarkOnlyThis);
        }
    }
}

void LayoutFlexibleBox::layoutBlock(bool relayoutChildren)
{
    ASSERT(needsLayout());

    if (!relayoutChildren && simplifiedLayout())
        return;

    m_relaidOutChildren.clear();

    if (updateLogicalWidthAndColumnWidth())
        relayoutChildren = true;

    SubtreeLayoutScope layoutScope(*this);
    LayoutUnit previousHeight = logicalHeight();
    setLogicalHeight(borderAndPaddingLogicalHeight() + scrollbarLogicalHeight());

    PaintLayerScrollableArea::DelayScrollPositionClampScope delayClampScope;

    {
        TextAutosizer::LayoutScope textAutosizerLayoutScope(this);
        LayoutState state(*this, locationOffset());

        m_numberOfInFlowChildrenOnFirstLine = -1;

        LayoutBlock::startDelayUpdateScrollInfo(); // weolar TODO
        prepareOrderIteratorAndMargins();

        layoutFlexItems(relayoutChildren, layoutScope);
        if (PaintLayerScrollableArea::PreventRelayoutScope::relayoutNeeded()) {
            PaintLayerScrollableArea::FreezeScrollbarsScope freezeScrollbarsScope;
            prepareOrderIteratorAndMargins();
            layoutFlexItems(true, layoutScope);
            PaintLayerScrollableArea::PreventRelayoutScope::resetRelayoutNeeded();
        }

        LayoutBlock::finishDelayUpdateScrollInfo(); // weolar TODO

        if (logicalHeight() != previousHeight)
            relayoutChildren = true;

        layoutPositionedObjects(relayoutChildren || isDocumentElement());

        // FIXME: css3/flexbox/repaint-rtl-column.html seems to issue paint invalidations for more overflow than it needs to.
        if (!firstChild() || (firstChild() && !(firstChild()->isText())))
            computeOverflow(clientLogicalBottomAfterRepositioning());
        else {
            LayoutObject* child = firstChild();
            child->clearNeedsLayout();
        }
    }

    updateLayerTransformAfterLayout();

    // Update our scroll information if we're overflow:auto/scroll/hidden now that we know if
    // we overflow or not.
    updateScrollInfoAfterLayout(); //  updateAfterLayout();

    clearNeedsLayout();

    // We have to reset this, because changes to our ancestors' style
    // can affect this value.
    m_hasDefiniteHeight = SizeDefiniteness::Unknown;
}

void LayoutFlexibleBox::paintChildren(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    BlockPainter::paintChildrenOfFlexibleBox(*this, paintInfo, paintOffset);
}

void LayoutFlexibleBox::repositionLogicalHeightDependentFlexItems(Vector<LineContext>& lineContexts)
{
    LayoutUnit crossAxisStartEdge = lineContexts.isEmpty() ? LayoutUnit() : lineContexts[0].crossAxisOffset;
    alignFlexLines(lineContexts);

    alignChildren(lineContexts);

    if (style()->flexWrap() == FlexWrapReverse)
        flipForWrapReverse(lineContexts, crossAxisStartEdge);

    // direction:rtl + flex-direction:column means the cross-axis direction is flipped.
    flipForRightToLeftColumn();
}

LayoutUnit LayoutFlexibleBox::clientLogicalBottomAfterRepositioning()
{
    LayoutUnit maxChildLogicalBottom;
    if (firstChild() && (firstChild()->isText()))
        return maxChildLogicalBottom;

    for (LayoutBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
        if (child->isOutOfFlowPositioned())
            continue;
        LayoutUnit childLogicalBottom = logicalTopForChild(*child) + logicalHeightForChild(*child) + marginAfterForChild(*child);
        maxChildLogicalBottom = std::max(maxChildLogicalBottom, childLogicalBottom);
    }
    return std::max(clientLogicalBottom(), maxChildLogicalBottom + paddingAfter());
}

bool LayoutFlexibleBox::hasOrthogonalFlow(const LayoutBox& child) const
{
    return isHorizontalFlow() != child.isHorizontalWritingMode();
}

bool LayoutFlexibleBox::isColumnFlow() const
{
    return style()->isColumnFlexDirection();
}

bool LayoutFlexibleBox::isHorizontalFlow() const
{
    if (isHorizontalWritingMode())
        return !isColumnFlow();
    return isColumnFlow();
}

bool LayoutFlexibleBox::isLeftToRightFlow() const
{
    if (isColumnFlow())
        return style()->writingMode() == TopToBottomWritingMode || style()->writingMode() == LeftToRightWritingMode;
    return style()->isLeftToRightDirection() ^ (style()->flexDirection() == FlowRowReverse);
}

bool LayoutFlexibleBox::isMultiline() const
{
    return style()->flexWrap() != FlexNoWrap;
}

Length LayoutFlexibleBox::flexBasisForChild(const LayoutBox& child) const
{
    Length flexLength = child.style()->flexBasis();
    if (flexLength.isAuto())
        flexLength = isHorizontalFlow() ? child.style()->width() : child.style()->height();
    return flexLength;
}

LayoutUnit LayoutFlexibleBox::crossAxisExtentForChild(const LayoutBox& child) const
{
    return isHorizontalFlow() ? child.size().height() : child.size().width();
}

static inline LayoutUnit constrainedChildIntrinsicContentLogicalHeight(const LayoutBox& child)
{
    LayoutUnit childIntrinsicContentLogicalHeight = child.intrinsicContentLogicalHeight();
    return child.constrainLogicalHeightByMinMax(childIntrinsicContentLogicalHeight + child.borderAndPaddingLogicalHeight(), childIntrinsicContentLogicalHeight);
}

LayoutUnit LayoutFlexibleBox::childIntrinsicHeight(const LayoutBox& child) const
{
    if (child.isHorizontalWritingMode() && needToStretchChildLogicalHeight(child))
        return constrainedChildIntrinsicContentLogicalHeight(child);
    // If our height is auto, make sure that our returned height is unaffected by earlier layouts by
    // returning the max preferred height (=logical width)
    if (!child.isHorizontalWritingMode() && child.styleRef().height().isAuto())
        return child.maxPreferredLogicalWidth();
    return child.size().height();
}

LayoutUnit LayoutFlexibleBox::childIntrinsicWidth(const LayoutBox& child) const
{
    if (!child.isHorizontalWritingMode() && needToStretchChildLogicalHeight(child))
        return constrainedChildIntrinsicContentLogicalHeight(child);
    if (child.isHorizontalWritingMode() && child.styleRef().width().isAuto()) {
        // This value is already clamped by min/max-width
        return child.maxPreferredLogicalWidth();
    }
    return child.size().width();
}

LayoutUnit LayoutFlexibleBox::crossAxisIntrinsicExtentForChild(const LayoutBox& child) const
{
    return isHorizontalFlow() ? childIntrinsicHeight(child) : childIntrinsicWidth(child);
}

LayoutUnit LayoutFlexibleBox::mainAxisExtentForChild(const LayoutBox& child) const
{
    return isHorizontalFlow() ? child.size().width() : child.size().height();
}

LayoutUnit LayoutFlexibleBox::crossAxisExtent() const
{
    return isHorizontalFlow() ? size().height() : size().width();
}

LayoutUnit LayoutFlexibleBox::mainAxisExtent() const
{
    return isHorizontalFlow() ? size().width() : size().height();
}

LayoutUnit LayoutFlexibleBox::crossAxisContentExtent() const
{
    return isHorizontalFlow() ? contentHeight() : contentWidth();
}

LayoutUnit LayoutFlexibleBox::mainAxisContentExtent(LayoutUnit contentLogicalHeight)
{
    if (isColumnFlow()) {
        LogicalExtentComputedValues computedValues;
        LayoutUnit borderPaddingAndScrollbar = borderAndPaddingLogicalHeight() + scrollbarLogicalHeight();
        LayoutUnit borderBoxLogicalHeight = contentLogicalHeight + borderPaddingAndScrollbar;
        computeLogicalHeight(borderBoxLogicalHeight, logicalTop(), computedValues);
        if (computedValues.m_extent == LayoutUnit::max())
            return computedValues.m_extent;
        return std::max(LayoutUnit(), computedValues.m_extent - borderPaddingAndScrollbar);
    }
    return contentLogicalWidth();
}

LayoutUnit LayoutFlexibleBox::computeMainAxisExtentForChild(const LayoutBox& child, SizeType sizeType, const Length& size)
{
    // If we have a horizontal flow, that means the main size is the width.
    // That's the logical width for horizontal writing modes, and the logical height in vertical writing modes.
    // For a vertical flow, main size is the height, so it's the inverse.
    // So we need the logical width if we have a horizontal flow and horizontal writing mode, or vertical flow and vertical writing mode.
    // Otherwise we need the logical height.
    if (isHorizontalFlow() != child.styleRef().isHorizontalWritingMode()) {
        // We don't have to check for "auto" here - computeContentLogicalHeight will just return -1 for that case anyway.
        // It's safe to access scrollbarLogicalHeight here because computeNextFlexLine will have already
        // forced layout on the child.
        // We previously layed out the child if necessary (see computeNextFlexLine and the call to childHasIntrinsicMainAxisSize)
        // so we can be sure that the two height calls here will return up-to-date data.
        return child.computeContentLogicalHeight(sizeType, size, child.intrinsicContentLogicalHeight()) + child.scrollbarLogicalHeight();
    }
    // computeLogicalWidth always re-computes the intrinsic widths. However, when our logical width is auto,
    // we can just use our cached value. So let's do that here. (Compare code in LayoutBlock::computePreferredLogicalWidths)
    LayoutUnit borderAndPadding = child.borderAndPaddingLogicalWidth();
    if (child.styleRef().logicalWidth().isAuto() && !hasAspectRatio(child)) {
        if (size.type() == MinContent)
            return child.minPreferredLogicalWidth() - borderAndPadding;
        if (size.type() == MaxContent)
            return child.maxPreferredLogicalWidth() - borderAndPadding;
    }
    return child.computeLogicalWidthUsing(sizeType, size, contentLogicalWidth(), this) - borderAndPadding;
}

LayoutFlexibleBox::TransformedWritingMode LayoutFlexibleBox::getTransformedWritingMode() const
{
    WritingMode mode = style()->writingMode();
    if (!isColumnFlow()) {
        static_assert(static_cast<TransformedWritingMode>(TopToBottomWritingMode) == TransformedWritingMode::TopToBottomWritingMode
            && static_cast<TransformedWritingMode>(LeftToRightWritingMode) == TransformedWritingMode::LeftToRightWritingMode
            && static_cast<TransformedWritingMode>(RightToLeftWritingMode) == TransformedWritingMode::RightToLeftWritingMode,
            "WritingMode and TransformedWritingMode must match values.");
        return static_cast<TransformedWritingMode>(mode);
    }

    switch (mode) {
    case TopToBottomWritingMode:
        return style()->isLeftToRightDirection() ? TransformedWritingMode::LeftToRightWritingMode : TransformedWritingMode::RightToLeftWritingMode;
    case LeftToRightWritingMode:
    case RightToLeftWritingMode:
        return style()->isLeftToRightDirection() ? TransformedWritingMode::TopToBottomWritingMode : TransformedWritingMode::BottomToTopWritingMode;
    }
    ASSERT_NOT_REACHED();
    return TransformedWritingMode::TopToBottomWritingMode;
}

LayoutUnit LayoutFlexibleBox::flowAwareBorderStart() const
{
    if (isHorizontalFlow())
        return LayoutUnit(isLeftToRightFlow() ? borderLeft() : borderRight());
    return LayoutUnit(isLeftToRightFlow() ? borderTop() : borderBottom());
}

LayoutUnit LayoutFlexibleBox::flowAwareBorderEnd() const
{
    if (isHorizontalFlow())
        return LayoutUnit(isLeftToRightFlow() ? borderRight() : borderLeft());
    return LayoutUnit(isLeftToRightFlow() ? borderBottom() : borderTop());
}

LayoutUnit LayoutFlexibleBox::flowAwareBorderBefore() const
{
    switch (getTransformedWritingMode()) {
    case TransformedWritingMode::TopToBottomWritingMode:
        return LayoutUnit(borderTop());
    case TransformedWritingMode::BottomToTopWritingMode:
        return LayoutUnit(borderBottom());
    case TransformedWritingMode::LeftToRightWritingMode:
        return LayoutUnit(borderLeft());
    case TransformedWritingMode::RightToLeftWritingMode:
        return LayoutUnit(borderRight());
    }
    ASSERT_NOT_REACHED();
    return LayoutUnit(borderTop());
}

LayoutUnit LayoutFlexibleBox::flowAwareBorderAfter() const
{
    switch (getTransformedWritingMode()) {
    case TransformedWritingMode::TopToBottomWritingMode:
        return LayoutUnit(borderBottom());
    case TransformedWritingMode::BottomToTopWritingMode:
        return LayoutUnit(borderTop());
    case TransformedWritingMode::LeftToRightWritingMode:
        return LayoutUnit(borderRight());
    case TransformedWritingMode::RightToLeftWritingMode:
        return LayoutUnit(borderLeft());
    }
    ASSERT_NOT_REACHED();
    return LayoutUnit(borderTop());
}

LayoutUnit LayoutFlexibleBox::flowAwarePaddingStart() const
{
    if (isHorizontalFlow())
        return isLeftToRightFlow() ? paddingLeft() : paddingRight();
    return isLeftToRightFlow() ? paddingTop() : paddingBottom();
}

LayoutUnit LayoutFlexibleBox::flowAwarePaddingEnd() const
{
    if (isHorizontalFlow())
        return isLeftToRightFlow() ? paddingRight() : paddingLeft();
    return isLeftToRightFlow() ? paddingBottom() : paddingTop();
}

LayoutUnit LayoutFlexibleBox::flowAwarePaddingBefore() const
{
    switch (getTransformedWritingMode()) {
    case TransformedWritingMode::TopToBottomWritingMode:
        return paddingTop();
    case TransformedWritingMode::BottomToTopWritingMode:
        return paddingBottom();
    case TransformedWritingMode::LeftToRightWritingMode:
        return paddingLeft();
    case TransformedWritingMode::RightToLeftWritingMode:
        return paddingRight();
    }
    ASSERT_NOT_REACHED();
    return paddingTop();
}

LayoutUnit LayoutFlexibleBox::flowAwarePaddingAfter() const
{
    switch (getTransformedWritingMode()) {
    case TransformedWritingMode::TopToBottomWritingMode:
        return paddingBottom();
    case TransformedWritingMode::BottomToTopWritingMode:
        return paddingTop();
    case TransformedWritingMode::LeftToRightWritingMode:
        return paddingRight();
    case TransformedWritingMode::RightToLeftWritingMode:
        return paddingLeft();
    }
    ASSERT_NOT_REACHED();
    return paddingTop();
}

LayoutUnit LayoutFlexibleBox::flowAwareMarginStartForChild(const LayoutBox& child) const
{
    if (isHorizontalFlow())
        return isLeftToRightFlow() ? child.marginLeft() : child.marginRight();
    return isLeftToRightFlow() ? child.marginTop() : child.marginBottom();
}

LayoutUnit LayoutFlexibleBox::flowAwareMarginEndForChild(const LayoutBox& child) const
{
    if (isHorizontalFlow())
        return isLeftToRightFlow() ? child.marginRight() : child.marginLeft();
    return isLeftToRightFlow() ? child.marginBottom() : child.marginTop();
}

LayoutUnit LayoutFlexibleBox::flowAwareMarginBeforeForChild(const LayoutBox& child) const
{
    switch (getTransformedWritingMode()) {
    case TransformedWritingMode::TopToBottomWritingMode:
        return child.marginTop();
    case TransformedWritingMode::BottomToTopWritingMode:
        return child.marginBottom();
    case TransformedWritingMode::LeftToRightWritingMode:
        return child.marginLeft();
    case TransformedWritingMode::RightToLeftWritingMode:
        return child.marginRight();
    }
    ASSERT_NOT_REACHED();
    return marginTop();
}

LayoutUnit LayoutFlexibleBox::crossAxisMarginExtentForChild(const LayoutBox& child) const
{
    return isHorizontalFlow() ? child.marginHeight() : child.marginWidth();
}

LayoutUnit LayoutFlexibleBox::crossAxisScrollbarExtent() const
{
    return LayoutUnit(isHorizontalFlow() ? horizontalScrollbarHeight() : verticalScrollbarWidth());
}

LayoutUnit LayoutFlexibleBox::crossAxisScrollbarExtentForChild(const LayoutBox& child) const
{
    return LayoutUnit(isHorizontalFlow() ? child.horizontalScrollbarHeight() : child.verticalScrollbarWidth());
}

LayoutPoint LayoutFlexibleBox::flowAwareLocationForChild(const LayoutBox& child) const
{
    return isHorizontalFlow() ? child.location() : child.location().transposedPoint();
}

bool LayoutFlexibleBox::useChildAspectRatio(const LayoutBox& child) const
{
    if (!hasAspectRatio(child))
        return false;
    if (child.intrinsicSize().height() == 0) {
        // We can't compute a ratio in this case.
        return false;
    }
    Length crossSize;
    if (isHorizontalFlow())
        crossSize = child.styleRef().height();
    else
        crossSize = child.styleRef().width();
    return crossAxisLengthIsDefinite(child, crossSize);
}

LayoutUnit LayoutFlexibleBox::computeMainSizeFromAspectRatioUsing(const LayoutBox& child, Length crossSizeLength) const
{
    ASSERT(hasAspectRatio(child));
    ASSERT(child.intrinsicSize().height() != 0);

    LayoutUnit crossSize;
    if (crossSizeLength.isFixed()) {
        crossSize = LayoutUnit(crossSizeLength.value());
    } else {
        ASSERT(crossSizeLength.hasPercent());
        crossSize = hasOrthogonalFlow(child) ?
            adjustBorderBoxLogicalWidthForBoxSizing(valueForLength(crossSizeLength, contentWidth())) :
            child.computePercentageLogicalHeight(crossSizeLength);
    }

    const LayoutSize& childIntrinsicSize = child.intrinsicSize();
    double ratio = childIntrinsicSize.width().toFloat() / childIntrinsicSize.height().toFloat();
    if (isHorizontalFlow())
        return LayoutUnit(crossSize * ratio);
    return LayoutUnit(crossSize / ratio);
}

void LayoutFlexibleBox::setFlowAwareLocationForChild(LayoutBox& child, const LayoutPoint& location)
{
    if (isHorizontalFlow())
        child.setLocationAndUpdateOverflowControlsIfNeeded(location);
    else
        child.setLocationAndUpdateOverflowControlsIfNeeded(location.transposedPoint());
}

LayoutUnit LayoutFlexibleBox::mainAxisBorderAndPaddingExtentForChild(const LayoutBox& child) const
{
    return isHorizontalFlow() ? child.borderAndPaddingWidth() : child.borderAndPaddingHeight();
}

bool LayoutFlexibleBox::mainAxisLengthIsDefinite(const LayoutBox& child, const Length& flexBasis) const
{
    if (flexBasis.isAuto())
        return false;
    if (flexBasis.hasPercent()) {
        if (!isColumnFlow() || m_hasDefiniteHeight == SizeDefiniteness::Definite)
            return true;
        if (m_hasDefiniteHeight == SizeDefiniteness::Indefinite)
            return false;
        bool definite = child.computePercentageLogicalHeight(flexBasis) != -1;
        m_hasDefiniteHeight = definite ? SizeDefiniteness::Definite : SizeDefiniteness::Indefinite;
        return definite;
    }
    return true;
}

bool LayoutFlexibleBox::crossAxisLengthIsDefinite(const LayoutBox& child, const Length& length) const
{
    if (length.isAuto())
        return false;
    if (length.hasPercent()) {
        if (hasOrthogonalFlow(child) || m_hasDefiniteHeight == SizeDefiniteness::Definite)
            return true;
        if (m_hasDefiniteHeight == SizeDefiniteness::Indefinite)
            return false;
        bool definite = child.computePercentageLogicalHeight(length) != -1;
        m_hasDefiniteHeight = definite ? SizeDefiniteness::Definite : SizeDefiniteness::Indefinite;
        return definite;
    }
    // TODO(cbiesinger): Eventually we should support other types of sizes here. Requires updating
    // computeMainSizeFromAspectRatioUsing.
    return length.isFixed();
}

bool LayoutFlexibleBox::childFlexBaseSizeRequiresLayout(const LayoutBox& child) const
{
    return (!mainAxisLengthIsDefinite(child, flexBasisForChild(child))
        && (hasOrthogonalFlow(child)
            || (crossAxisOverflowForChild(child) == OAUTO
                && !PaintLayerScrollableArea::FreezeScrollbarsScope::scrollbarsAreFrozen())));
}

void LayoutFlexibleBox::cacheChildMainSize(const LayoutBox& child)
{
    ASSERT(!child.needsLayout());
    LayoutUnit mainSize;
    if (hasOrthogonalFlow(child)) {
        mainSize = child.logicalHeight();
    } else {
        // The max preferred logical width includes the intrinsic scrollbar logical width, which is only set for
        // overflow: scroll. To handle overflow: auto, we have to take scrollbarLogicalWidth() into account, and then
        // subtract the intrinsic width again so as to not double-count overflow: scroll scrollbars.
        mainSize = child.maxPreferredLogicalWidth() + child.scrollbarLogicalWidth() - child.scrollbarLogicalWidth();
    }
    m_intrinsicSizeAlongMainAxis.set(&child, mainSize);
    m_relaidOutChildren.add(&child);
}

void LayoutFlexibleBox::clearCachedMainSizeForChild(const LayoutBox& child)
{
    m_intrinsicSizeAlongMainAxis.remove(&child);
}

LayoutUnit LayoutFlexibleBox::computeInnerFlexBaseSizeForChild(LayoutBox& child, ChildLayoutType childLayoutType)
{
    child.clearOverrideSize();

    if (child.isImage() || child.isVideo() || child.isCanvas())
        UseCounter::count(document(), UseCounter::AspectRatioFlexItem);

    Length flexBasis = flexBasisForChild(child);
    if (mainAxisLengthIsDefinite(child, flexBasis))
        return std::max(LayoutUnit(), computeMainAxisExtentForChild(child, MainOrPreferredSize, flexBasis));

    LayoutUnit mainAxisExtent;
    if (childFlexBaseSizeRequiresLayout(child)) {
        if (childLayoutType == NeverLayout)
            return LayoutUnit();

        if (child.needsLayout() || childLayoutType == ForceLayout || !m_intrinsicSizeAlongMainAxis.contains(&child)) {
            child.forceChildLayout();
            cacheChildMainSize(child);
        }
        mainAxisExtent = m_intrinsicSizeAlongMainAxis.get(&child);
    } else {
        // We don't need to add scrollbarLogicalWidth here. For overflow: scroll, the preferred width
        // already includes the scrollbar size (via scrollbarLogicalWidth()). For overflow: auto,
        // childFlexBaseSizeRequiresLayout returns true and we handle that via the other branch
        // of this if.
        mainAxisExtent = child.maxPreferredLogicalWidth();
    }
    ASSERT(mainAxisExtent - mainAxisBorderAndPaddingExtentForChild(child) >= 0);
    return mainAxisExtent - mainAxisBorderAndPaddingExtentForChild(child);
}

void LayoutFlexibleBox::layoutFlexItems(bool relayoutChildren, SubtreeLayoutScope& layoutScope)
{
    if (firstChild() && (firstChild()->isText()))
        return;

    Vector<LineContext> lineContexts;
    OrderedFlexItemList orderedChildren;
    LayoutUnit sumFlexBaseSize;
    double totalFlexGrow;
    double totalFlexShrink;
    double totalWeightedFlexShrink;
    LayoutUnit sumHypotheticalMainSize;

    PaintLayerScrollableArea::PreventRelayoutScope preventRelayoutScope(layoutScope);
     
    m_orderIterator.first();
    LayoutUnit crossAxisOffset = flowAwareBorderBefore() + flowAwarePaddingBefore();
    while (computeNextFlexLine(orderedChildren, sumFlexBaseSize, totalFlexGrow, totalFlexShrink, totalWeightedFlexShrink, sumHypotheticalMainSize, relayoutChildren)) {
        LayoutUnit containerMainInnerSize = mainAxisContentExtent(sumHypotheticalMainSize);
        // availableFreeSpace is the initial amount of free space in this flexbox.
        // remainingFreeSpace starts out at the same value but as we place and lay out
        // flex items we subtract from it. Note that both values can be negative.
        LayoutUnit remainingFreeSpace = containerMainInnerSize - sumFlexBaseSize;
        FlexSign flexSign = (sumHypotheticalMainSize < containerMainInnerSize) ? PositiveFlexibility : NegativeFlexibility;
        freezeInflexibleItems(flexSign, orderedChildren, remainingFreeSpace, totalFlexGrow, totalFlexShrink, totalWeightedFlexShrink);
        // The initial free space gets calculated after freezing inflexible items. https://drafts.csswg.org/css-flexbox/#resolve-flexible-lengths step 3
        const LayoutUnit initialFreeSpace = remainingFreeSpace;
        while (!resolveFlexibleLengths(flexSign, orderedChildren, initialFreeSpace, remainingFreeSpace, totalFlexGrow, totalFlexShrink, totalWeightedFlexShrink)) {
            ASSERT(totalFlexGrow >= 0 && totalWeightedFlexShrink >= 0);
        }

        // Recalculate the remaining free space. The adjustment for flex factors between 0..1 means we can't just
        // use remainingFreeSpace here.
        remainingFreeSpace = containerMainInnerSize;
        for (size_t i = 0; i < orderedChildren.size(); ++i) {
            LayoutBox* child = orderedChildren[i].box;
            if (child->isOutOfFlowPositioned())
                continue;
            remainingFreeSpace -= (orderedChildren[i].flexedContentSize + mainAxisBorderAndPaddingExtentForChild(*child)
                + (isHorizontalFlow() ? child->marginWidth() : child->marginHeight()));
        }
        layoutAndPlaceChildren(crossAxisOffset, orderedChildren, remainingFreeSpace, relayoutChildren, layoutScope, lineContexts);
    }
    if (hasLineIfEmpty()) {
        // Even if computeNextFlexLine returns true, the flexbox might not have
        // a line because all our children might be out of flow positioned.
        // Instead of just checking if we have a line, make sure the flexbox
        // has at least a line's worth of height to cover this case.
        LayoutUnit minHeight = minimumLogicalHeightForEmptyLine();
        if (size().height() < minHeight)
            setLogicalHeight(minHeight);
    }

    updateLogicalHeight();
    repositionLogicalHeightDependentFlexItems(lineContexts);
}

LayoutUnit LayoutFlexibleBox::autoMarginOffsetInMainAxis(const OrderedFlexItemList& children, LayoutUnit& availableFreeSpace)
{
    if (availableFreeSpace <= LayoutUnit())
        return LayoutUnit();

    int numberOfAutoMargins = 0;
    bool isHorizontal = isHorizontalFlow();
    for (size_t i = 0; i < children.size(); ++i) {
        LayoutBox* child = children[i].box;
        if (child->isOutOfFlowPositioned())
            continue;
        if (isHorizontal) {
            if (child->style()->marginLeft().isAuto())
                ++numberOfAutoMargins;
            if (child->style()->marginRight().isAuto())
                ++numberOfAutoMargins;
        } else {
            if (child->style()->marginTop().isAuto())
                ++numberOfAutoMargins;
            if (child->style()->marginBottom().isAuto())
                ++numberOfAutoMargins;
        }
    }
    if (!numberOfAutoMargins)
        return LayoutUnit();

    LayoutUnit sizeOfAutoMargin = availableFreeSpace / numberOfAutoMargins;
    availableFreeSpace = LayoutUnit();
    return sizeOfAutoMargin;
}

void LayoutFlexibleBox::updateAutoMarginsInMainAxis(LayoutBox& child, LayoutUnit autoMarginOffset)
{
    ASSERT(autoMarginOffset >= 0);

    if (isHorizontalFlow()) {
        if (child.style()->marginLeft().isAuto())
            child.setMarginLeft(autoMarginOffset);
        if (child.style()->marginRight().isAuto())
            child.setMarginRight(autoMarginOffset);
    } else {
        if (child.style()->marginTop().isAuto())
            child.setMarginTop(autoMarginOffset);
        if (child.style()->marginBottom().isAuto())
            child.setMarginBottom(autoMarginOffset);
    }
}

bool LayoutFlexibleBox::hasAutoMarginsInCrossAxis(const LayoutBox& child) const
{
    if (isHorizontalFlow())
        return child.style()->marginTop().isAuto() || child.style()->marginBottom().isAuto();
    return child.style()->marginLeft().isAuto() || child.style()->marginRight().isAuto();
}

LayoutUnit LayoutFlexibleBox::availableAlignmentSpaceForChild(LayoutUnit lineCrossAxisExtent, const LayoutBox& child)
{
    ASSERT(!child.isOutOfFlowPositioned());
    LayoutUnit childCrossExtent = crossAxisMarginExtentForChild(child) + crossAxisExtentForChild(child);
    return lineCrossAxisExtent - childCrossExtent;
}

LayoutUnit LayoutFlexibleBox::availableAlignmentSpaceForChildBeforeStretching(LayoutUnit lineCrossAxisExtent, const LayoutBox& child)
{
    ASSERT(!child.isOutOfFlowPositioned());
    LayoutUnit childCrossExtent = crossAxisMarginExtentForChild(child) + crossAxisIntrinsicExtentForChild(child);
    return lineCrossAxisExtent - childCrossExtent;
}

bool LayoutFlexibleBox::updateAutoMarginsInCrossAxis(LayoutBox& child, LayoutUnit availableAlignmentSpace)
{
    ASSERT(!child.isOutOfFlowPositioned());
    ASSERT(availableAlignmentSpace >= 0);

    bool isHorizontal = isHorizontalFlow();
    Length topOrLeft = isHorizontal ? child.style()->marginTop() : child.style()->marginLeft();
    Length bottomOrRight = isHorizontal ? child.style()->marginBottom() : child.style()->marginRight();
    if (topOrLeft.isAuto() && bottomOrRight.isAuto()) {
        adjustAlignmentForChild(child, availableAlignmentSpace / 2);
        if (isHorizontal) {
            child.setMarginTop(availableAlignmentSpace / 2);
            child.setMarginBottom(availableAlignmentSpace / 2);
        } else {
            child.setMarginLeft(availableAlignmentSpace / 2);
            child.setMarginRight(availableAlignmentSpace / 2);
        }
        return true;
    }
    bool shouldAdjustTopOrLeft = true;
    if (isColumnFlow() && !child.style()->isLeftToRightDirection()) {
        // For column flows, only make this adjustment if topOrLeft corresponds to the "before" margin,
        // so that flipForRightToLeftColumn will do the right thing.
        shouldAdjustTopOrLeft = false;
    }
    if (!isColumnFlow() && child.style()->isFlippedBlocksWritingMode()) {
        // If we are a flipped writing mode, we need to adjust the opposite side. This is only needed
        // for row flows because this only affects the block-direction axis.
        shouldAdjustTopOrLeft = false;
    }

    if (topOrLeft.isAuto()) {
        if (shouldAdjustTopOrLeft)
            adjustAlignmentForChild(child, availableAlignmentSpace);

        if (isHorizontal)
            child.setMarginTop(availableAlignmentSpace);
        else
            child.setMarginLeft(availableAlignmentSpace);
        return true;
    }
    if (bottomOrRight.isAuto()) {
        if (!shouldAdjustTopOrLeft)
            adjustAlignmentForChild(child, availableAlignmentSpace);

        if (isHorizontal)
            child.setMarginBottom(availableAlignmentSpace);
        else
            child.setMarginRight(availableAlignmentSpace);
        return true;
    }
    return false;
}

LayoutUnit LayoutFlexibleBox::marginBoxAscentForChild(const LayoutBox& child)
{
    LayoutUnit ascent(child.firstLineBoxBaseline());
    if (ascent == -1)
        ascent = crossAxisExtentForChild(child);
    return ascent + flowAwareMarginBeforeForChild(child);
}

LayoutUnit LayoutFlexibleBox::computeChildMarginValue(Length margin)
{
    // When resolving the margins, we use the content size for resolving percent and calc (for percents in calc expressions) margins.
    // Fortunately, percent margins are always computed with respect to the block's width, even for margin-top and margin-bottom.
    LayoutUnit availableSize = contentLogicalWidth();
    return minimumValueForLength(margin, availableSize);
}

void LayoutFlexibleBox::prepareOrderIteratorAndMargins()
{
    if (firstChild() && (firstChild()->isText()))
        return;

    OrderIteratorPopulator populator(m_orderIterator);

    for (LayoutBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
        populator.collectChild(child);

        if (child->isOutOfFlowPositioned())
            continue;

        // Before running the flex algorithm, 'auto' has a margin of 0.
        // Also, if we're not auto sizing, we don't do a layout that computes the start/end margins.
        if (isHorizontalFlow()) {
            child->setMarginLeft(computeChildMarginValue(child->style()->marginLeft()));
            child->setMarginRight(computeChildMarginValue(child->style()->marginRight()));
        } else {
            child->setMarginTop(computeChildMarginValue(child->style()->marginTop()));
            child->setMarginBottom(computeChildMarginValue(child->style()->marginBottom()));
        }
    }
}

LayoutUnit LayoutFlexibleBox::adjustChildSizeForMinAndMax(const LayoutBox& child, LayoutUnit childSize)
{
    Length max = isHorizontalFlow() ? child.style()->maxWidth() : child.style()->maxHeight();
    LayoutUnit maxExtent(-1);
    if (max.isSpecifiedOrIntrinsic()) {
        maxExtent = computeMainAxisExtentForChild(child, MaxSize, max);
        ASSERT(maxExtent >= -1);
        if (maxExtent != -1 && childSize > maxExtent)
            childSize = maxExtent;
    }

    Length min = isHorizontalFlow() ? child.style()->minWidth() : child.style()->minHeight();
    LayoutUnit minExtent;
    if (min.isSpecifiedOrIntrinsic()) {
        minExtent = computeMainAxisExtentForChild(child, MinSize, min);
        // computeMainAxisExtentForChild can return -1 when the child has a percentage
        // min size, but we have an indefinite size in that axis.
        minExtent = std::max(LayoutUnit(), minExtent);
    } else if (min.isAuto() && mainAxisOverflowForChild(child) == OVISIBLE && !(isColumnFlow() && child.isFlexibleBox())) {
        // TODO(cbiesinger): For now, we do not handle min-height: auto for nested column flexboxes. We need
        // to implement https://drafts.csswg.org/css-flexbox/#intrinsic-sizes before that produces
        // reasonable results. Tracking bug: https://crbug.com/581553
        // css-flexbox section 4.5
        LayoutUnit contentSize = computeMainAxisExtentForChild(child, MinSize, Length(MinContent));
        ASSERT(contentSize >= 0);
        if (hasAspectRatio(child) && child.intrinsicSize().height() > 0)
            contentSize = adjustChildSizeForAspectRatioCrossAxisMinAndMax(child, contentSize);
        if (maxExtent != -1 && contentSize > maxExtent)
            contentSize = maxExtent;

        Length mainSize = isHorizontalFlow() ? child.styleRef().width() : child.styleRef().height();
        if (mainAxisLengthIsDefinite(child, mainSize)) {
            LayoutUnit resolvedMainSize = computeMainAxisExtentForChild(child, MainOrPreferredSize, mainSize);
            ASSERT(resolvedMainSize >= 0);
            LayoutUnit specifiedSize = maxExtent != -1 ? std::min(resolvedMainSize, maxExtent) : resolvedMainSize;

            minExtent = std::min(specifiedSize, contentSize);
        } else if (useChildAspectRatio(child)) {
            Length crossSizeLength = isHorizontalFlow() ? child.styleRef().height() : child.styleRef().width();
            LayoutUnit transferredSize = computeMainSizeFromAspectRatioUsing(child, crossSizeLength);
            transferredSize = adjustChildSizeForAspectRatioCrossAxisMinAndMax(child, transferredSize);
            minExtent = std::min(transferredSize, contentSize);
        } else {
            minExtent = contentSize;
        }
    }
    ASSERT(minExtent >= 0);
    return std::max(childSize, minExtent);
}

LayoutUnit LayoutFlexibleBox::crossSizeForPercentageResolution(const LayoutBox& child)
{
    if (alignmentForChild(child) != ItemPositionStretch)
        return LayoutUnit(-1);

    // Here we implement https://drafts.csswg.org/css-flexbox/#algo-stretch
    if (hasOrthogonalFlow(child) && child.hasOverrideLogicalContentWidth())
        return child.overrideLogicalContentWidth();
    if (!hasOrthogonalFlow(child) && child.hasOverrideLogicalContentHeight())
        return child.overrideLogicalContentHeight();

    // We don't currently implement the optimization from https://drafts.csswg.org/css-flexbox/#definite-sizes
    // case 1. While that could speed up a specialized case, it requires determining if we have a definite
    // size, which itself is not cheap. We can consider implementing it at a later time.
    // (The correctness is ensured by redoing layout in applyStretchAlignmentToChild)
    return LayoutUnit(-1);
}

LayoutUnit LayoutFlexibleBox::mainSizeForPercentageResolution(const LayoutBox& child)
{
    // This function implements section 9.8. Definite and Indefinite Sizes, case
    // 2) of the flexbox spec.
    // We need to check for the flexbox to have a definite main size, and for the
    // flex item to have a definite flex basis.
    const Length& flexBasis = flexBasisForChild(child);
    if (!mainAxisLengthIsDefinite(child, flexBasis))
        return LayoutUnit(-1);
    if (!flexBasis.hasPercent()) {
        // If flex basis had a percentage, our size is guaranteed to be definite or the flex item's
        // size could not be definite.
        // Otherwise, we make up a percentage to check whether we have a definite size.
        if (!mainAxisLengthIsDefinite(child, Length(0, Percent)))
            return LayoutUnit(-1);
    }

    if (hasOrthogonalFlow(child))
        return child.hasOverrideLogicalContentHeight() ? child.overrideLogicalContentHeight() : LayoutUnit(-1);
    return child.hasOverrideLogicalContentWidth() ? child.overrideLogicalContentWidth() : LayoutUnit(-1);
}

LayoutUnit LayoutFlexibleBox::childLogicalHeightForPercentageResolution(const LayoutBox& child)
{
    if (!hasOrthogonalFlow(child))
        return crossSizeForPercentageResolution(child);
    return mainSizeForPercentageResolution(child);
}

LayoutUnit LayoutFlexibleBox::adjustChildSizeForAspectRatioCrossAxisMinAndMax(const LayoutBox& child, LayoutUnit childSize)
{
    Length crossMin = isHorizontalFlow() ? child.style()->minHeight() : child.style()->minWidth();
    Length crossMax = isHorizontalFlow() ? child.style()->maxHeight() : child.style()->maxWidth();


    if (crossAxisLengthIsDefinite(child, crossMax)) {
        LayoutUnit maxValue = computeMainSizeFromAspectRatioUsing(child, crossMax);
        childSize = std::min(maxValue, childSize);
    }

    if (crossAxisLengthIsDefinite(child, crossMin)) {
        LayoutUnit minValue = computeMainSizeFromAspectRatioUsing(child, crossMin);
        childSize = std::max(minValue, childSize);
    }

    return childSize;
}

bool LayoutFlexibleBox::computeNextFlexLine(OrderedFlexItemList& orderedChildren, LayoutUnit& sumFlexBaseSize, double& totalFlexGrow, double& totalFlexShrink, double& totalWeightedFlexShrink, LayoutUnit& sumHypotheticalMainSize, bool relayoutChildren)
{
    orderedChildren.clear();
    sumFlexBaseSize = LayoutUnit();
    totalFlexGrow = totalFlexShrink = totalWeightedFlexShrink = 0;
    sumHypotheticalMainSize = LayoutUnit();

    if (!m_orderIterator.currentChild())
        return false;

    LayoutUnit lineBreakLength = mainAxisContentExtent(LayoutUnit::max());

    bool lineHasInFlowItem = false;

    for (LayoutBox* child = m_orderIterator.currentChild(); child; child = m_orderIterator.next()) {
        if (child->isOutOfFlowPositioned()) {
            orderedChildren.append(FlexItem(child));
            continue;
        }

        // If this condition is true, then computeMainAxisExtentForChild will call child.intrinsicContentLogicalHeight()
        // and child.scrollbarLogicalHeight(), so if the child has intrinsic min/max/preferred size,
        // run layout on it now to make sure its logical height and scroll bars are up to date.
        if (childHasIntrinsicMainAxisSize(*child) && child->needsLayout()) {
            child->clearOverrideSize();
            child->layoutIfNeeded();
            cacheChildMainSize(*child);
        }

        LayoutUnit childInnerFlexBaseSize = computeInnerFlexBaseSizeForChild(*child, relayoutChildren ? ForceLayout : LayoutIfNeeded);
        LayoutUnit childMainAxisMarginBorderPadding = mainAxisBorderAndPaddingExtentForChild(*child)
            + (isHorizontalFlow() ? child->marginWidth() : child->marginHeight());
        LayoutUnit childOuterFlexBaseSize = childInnerFlexBaseSize + childMainAxisMarginBorderPadding;

        LayoutUnit childMinMaxAppliedMainAxisExtent = adjustChildSizeForMinAndMax(*child, childInnerFlexBaseSize);
        LayoutUnit childHypotheticalMainSize = childMinMaxAppliedMainAxisExtent + childMainAxisMarginBorderPadding;

        if (isMultiline() && sumHypotheticalMainSize + childHypotheticalMainSize > lineBreakLength && lineHasInFlowItem)
            break;
        orderedChildren.append(FlexItem(child, childInnerFlexBaseSize, childMinMaxAppliedMainAxisExtent));
        lineHasInFlowItem  = true;
        sumFlexBaseSize += childOuterFlexBaseSize;
        totalFlexGrow += child->style()->flexGrow();
        totalFlexShrink += child->style()->flexShrink();
        totalWeightedFlexShrink += child->style()->flexShrink() * childInnerFlexBaseSize;
        sumHypotheticalMainSize += childHypotheticalMainSize;
    }
    return true;
}

void LayoutFlexibleBox::freezeViolations(Vector<FlexItem*>& violations, LayoutUnit& availableFreeSpace, double& totalFlexGrow, double& totalFlexShrink, double& totalWeightedFlexShrink)
{
    for (size_t i = 0; i < violations.size(); ++i) {
        ASSERT(!violations[i]->frozen) /*<< i*/;
        LayoutBox* child = violations[i]->box;
        LayoutUnit childSize = violations[i]->flexedContentSize;
        availableFreeSpace -= childSize - violations[i]->innerFlexBaseSize;
        totalFlexGrow -= child->style()->flexGrow();
        totalFlexShrink -= child->style()->flexShrink();
        totalWeightedFlexShrink -= child->style()->flexShrink() * violations[i]->innerFlexBaseSize;
        // totalWeightedFlexShrink can be negative when we exceed the precision of a double when we initially
        // calcuate totalWeightedFlexShrink. We then subtract each child's weighted flex shrink with full precision,
        // now leading to a negative result. See css3/flexbox/large-flex-shrink-assert.html
        totalWeightedFlexShrink = std::max(totalWeightedFlexShrink, 0.0);
        violations[i]->frozen = true;
    }
}

void LayoutFlexibleBox::freezeInflexibleItems(FlexSign flexSign, OrderedFlexItemList& children, LayoutUnit& remainingFreeSpace, double& totalFlexGrow, double& totalFlexShrink, double& totalWeightedFlexShrink)
{
    // Per https://drafts.csswg.org/css-flexbox/#resolve-flexible-lengths step 2,
    // we freeze all items with a flex factor of 0 as well as this with a min/max size violation.
    Vector<FlexItem*> newInflexibleItems;
    for (size_t i = 0; i < children.size(); ++i) {
        FlexItem& flexItem = children[i];
        LayoutBox* child = flexItem.box;
        if (child->isOutOfFlowPositioned())
            continue;
        ASSERT(!flexItem.frozen) /*<< i*/;
        float flexFactor = (flexSign == PositiveFlexibility) ? child->style()->flexGrow() : child->style()->flexShrink();
        if (flexFactor == 0
            || (flexSign == PositiveFlexibility && flexItem.innerFlexBaseSize > flexItem.hypotheticalMainSize)
            || (flexSign == NegativeFlexibility && flexItem.innerFlexBaseSize < flexItem.hypotheticalMainSize)) {
            flexItem.flexedContentSize = flexItem.hypotheticalMainSize;
            newInflexibleItems.append(&flexItem);
        }
    }
    freezeViolations(newInflexibleItems, remainingFreeSpace, totalFlexGrow, totalFlexShrink, totalWeightedFlexShrink);
}

// Returns true if we successfully ran the algorithm and sized the flex items.
bool LayoutFlexibleBox::resolveFlexibleLengths(FlexSign flexSign, OrderedFlexItemList& children, LayoutUnit initialFreeSpace, LayoutUnit& remainingFreeSpace, double& totalFlexGrow, double& totalFlexShrink, double& totalWeightedFlexShrink)
{
    LayoutUnit totalViolation;
    LayoutUnit usedFreeSpace;
    Vector<FlexItem*> minViolations;
    Vector<FlexItem*> maxViolations;

    double sumFlexFactors = (flexSign == PositiveFlexibility) ? totalFlexGrow : totalFlexShrink;
    if (sumFlexFactors > 0 && sumFlexFactors < 1) {
        LayoutUnit fractional(initialFreeSpace * sumFlexFactors);
        if (fractional.abs() < remainingFreeSpace.abs())
            remainingFreeSpace = fractional;
    }

    for (size_t i = 0; i < children.size(); ++i) {
        FlexItem& flexItem = children[i];
        LayoutBox* child = flexItem.box;
        if (child->isOutOfFlowPositioned())
            continue;

        if (flexItem.frozen)
            continue;

        LayoutUnit childSize = flexItem.innerFlexBaseSize;
        double extraSpace = 0;
        if (remainingFreeSpace > 0 && totalFlexGrow > 0 && flexSign == PositiveFlexibility && std::isfinite(totalFlexGrow)) {
            extraSpace = remainingFreeSpace * child->style()->flexGrow() / totalFlexGrow;
        } else if (remainingFreeSpace < 0 && totalWeightedFlexShrink > 0 && flexSign == NegativeFlexibility && std::isfinite(totalWeightedFlexShrink) && child->style()->flexShrink()) {
            extraSpace = remainingFreeSpace * child->style()->flexShrink() * flexItem.innerFlexBaseSize / totalWeightedFlexShrink;
        }
        if (std::isfinite(extraSpace))
            childSize += LayoutUnit::fromFloatRound(extraSpace);

        LayoutUnit adjustedChildSize = adjustChildSizeForMinAndMax(*child, childSize);
        ASSERT(adjustedChildSize >= 0);
        flexItem.flexedContentSize = adjustedChildSize;
        usedFreeSpace += adjustedChildSize - flexItem.innerFlexBaseSize;

        LayoutUnit violation = adjustedChildSize - childSize;
        if (violation > 0)
            minViolations.append(&flexItem);
        else if (violation < 0)
            maxViolations.append(&flexItem);
        totalViolation += violation;
    }

    if (totalViolation)
        freezeViolations(totalViolation < 0 ? maxViolations : minViolations, remainingFreeSpace, totalFlexGrow, totalFlexShrink, totalWeightedFlexShrink);
    else
        remainingFreeSpace -= usedFreeSpace;

    return !totalViolation;
}

static LayoutUnit initialJustifyContentOffset(LayoutUnit availableFreeSpace, ContentPosition justifyContent, ContentDistributionType justifyContentDistribution, unsigned numberOfChildren)
{
    if (justifyContent == ContentPositionFlexEnd)
        return availableFreeSpace;
    if (justifyContent == ContentPositionCenter)
        return availableFreeSpace / 2;
    if (justifyContentDistribution == ContentDistributionSpaceAround) {
        if (availableFreeSpace > 0 && numberOfChildren)
            return availableFreeSpace / (2 * numberOfChildren);

        return availableFreeSpace / 2;
    }
    return LayoutUnit();
}

static LayoutUnit justifyContentSpaceBetweenChildren(LayoutUnit availableFreeSpace, ContentDistributionType justifyContentDistribution, unsigned numberOfChildren)
{
    if (availableFreeSpace > 0 && numberOfChildren > 1) {
        if (justifyContentDistribution == ContentDistributionSpaceBetween)
            return availableFreeSpace / (numberOfChildren - 1);
        if (justifyContentDistribution == ContentDistributionSpaceAround)
            return availableFreeSpace / numberOfChildren;
    }
    return LayoutUnit();
}

static LayoutUnit alignmentOffset(LayoutUnit availableFreeSpace, ItemPosition position, LayoutUnit ascent, LayoutUnit maxAscent, bool isWrapReverse)
{
    switch (position) {
    case ItemPositionAuto:
        ASSERT_NOT_REACHED();
        break;
    case ItemPositionStretch:
        // Actual stretching must be handled by the caller.
        // Since wrap-reverse flips cross start and cross end, stretch children should be aligned with the cross end.
        // This matters because applyStretchAlignment doesn't always stretch or stretch fully (explicit cross size given,
        // or stretching constrained by max-height/max-width).
        // For flex-start and flex-end this is handled by alignmentForChild().
        if (isWrapReverse)
            return availableFreeSpace;
        break;
    case ItemPositionFlexStart:
        break;
    case ItemPositionFlexEnd:
        return availableFreeSpace;
    case ItemPositionCenter:
        return availableFreeSpace / 2;
    case ItemPositionBaseline:
        // FIXME: If we get here in columns, we want the use the descent, except we currently can't get the ascent/descent of orthogonal children.
        // https://bugs.webkit.org/show_bug.cgi?id=98076
        return maxAscent - ascent;
    case ItemPositionLastBaseline:
    case ItemPositionSelfStart:
    case ItemPositionSelfEnd:
    case ItemPositionStart:
    case ItemPositionEnd:
    case ItemPositionLeft:
    case ItemPositionRight:
        // FIXME: Implement these (https://crbug.com/507690). The extended grammar
        // is not enabled by default so we shouldn't hit this codepath.
        ASSERT_NOT_REACHED();
        break;
    }
    return LayoutUnit();
}

void LayoutFlexibleBox::setOverrideMainAxisSizeForChild(LayoutBox& child, LayoutUnit childPreferredSize)
{
    if (hasOrthogonalFlow(child))
        child.setOverrideLogicalContentHeight(childPreferredSize - child.borderAndPaddingLogicalHeight());
    else
        child.setOverrideLogicalContentWidth(childPreferredSize - child.borderAndPaddingLogicalWidth());
}

LayoutUnit LayoutFlexibleBox::staticMainAxisPositionForPositionedChild(const LayoutBox& child)
{
    const LayoutUnit availableSpace = mainAxisContentExtent(contentLogicalHeight()) - mainAxisExtentForChild(child);

    ContentPosition position = resolvedJustifyContentPosition(styleRef(), normalValueBehavior());
    ContentDistributionType distribution = resolvedJustifyContentDistribution(styleRef(), normalValueBehavior());
    LayoutUnit offset = initialJustifyContentOffset(availableSpace, position, distribution, 1);
    if (styleRef().flexDirection() == FlowRowReverse || styleRef().flexDirection() == FlowColumnReverse)
        offset = availableSpace - offset;
    return offset;
}

LayoutUnit LayoutFlexibleBox::staticCrossAxisPositionForPositionedChild(const LayoutBox& child)
{
    LayoutUnit availableSpace = crossAxisContentExtent() - crossAxisExtentForChild(child);
    return alignmentOffset(availableSpace, alignmentForChild(child), LayoutUnit(), LayoutUnit(), styleRef().flexWrap() == FlexWrapReverse);
}

LayoutUnit LayoutFlexibleBox::staticInlinePositionForPositionedChild(const LayoutBox& child)
{
    return startOffsetForContent() + (isColumnFlow() ?
        staticCrossAxisPositionForPositionedChild(child) :
        staticMainAxisPositionForPositionedChild(child));
}

LayoutUnit LayoutFlexibleBox::staticBlockPositionForPositionedChild(const LayoutBox& child)
{
    return borderAndPaddingBefore() + (isColumnFlow() ?
        staticMainAxisPositionForPositionedChild(child) :
        staticCrossAxisPositionForPositionedChild(child));
}

bool LayoutFlexibleBox::setStaticPositionForPositionedLayout(LayoutBox& child)
{
    bool positionChanged = false;
    DeprecatedPaintLayer* childLayer = child.layer();
    if (child.styleRef().hasStaticInlinePosition(styleRef().isHorizontalWritingMode())) {
        LayoutUnit inlinePosition = staticInlinePositionForPositionedChild(child);
        if (childLayer->staticInlinePosition() != inlinePosition) {
            childLayer->setStaticInlinePosition(inlinePosition);
            positionChanged = true;
        }
    }
    if (child.styleRef().hasStaticBlockPosition(styleRef().isHorizontalWritingMode())) {
        LayoutUnit blockPosition = staticBlockPositionForPositionedChild(child);
        if (childLayer->staticBlockPosition() != blockPosition) {
            childLayer->setStaticBlockPosition(blockPosition);
            positionChanged = true;
        }
    }
    return positionChanged;
}

void LayoutFlexibleBox::prepareChildForPositionedLayout(LayoutBox& child)
{
    ASSERT(child.isOutOfFlowPositioned());
    child.containingBlock()->insertPositionedObject(&child);
    DeprecatedPaintLayer* childLayer = child.layer();
    LayoutUnit staticInlinePosition = flowAwareBorderStart() + flowAwarePaddingStart();
    if (childLayer->staticInlinePosition() != staticInlinePosition) {
        childLayer->setStaticInlinePosition(staticInlinePosition);
        if (child.style()->hasStaticInlinePosition(style()->isHorizontalWritingMode()))
            child.setChildNeedsLayout(MarkOnlyThis);
    }

    LayoutUnit staticBlockPosition = flowAwareBorderBefore() + flowAwarePaddingBefore();
    if (childLayer->staticBlockPosition() != staticBlockPosition) {
        childLayer->setStaticBlockPosition(staticBlockPosition);
        if (child.style()->hasStaticBlockPosition(style()->isHorizontalWritingMode()))
            child.setChildNeedsLayout(MarkOnlyThis);
    }
}

ItemPosition LayoutFlexibleBox::alignmentForChild(const LayoutBox& child) const
{
    ItemPosition align = ComputedStyle::resolveAlignment(styleRef(), child.styleRef(), ItemPositionStretch);

    if (align == ItemPositionBaseline && hasOrthogonalFlow(child))
        align = ItemPositionFlexStart;

    if (style()->flexWrap() == FlexWrapReverse) {
        if (align == ItemPositionFlexStart)
            align = ItemPositionFlexEnd;
        else if (align == ItemPositionFlexEnd)
            align = ItemPositionFlexStart;
    }

    return align;
}

size_t LayoutFlexibleBox::numberOfInFlowPositionedChildren(const OrderedFlexItemList& children) const
{
    size_t count = 0;
    for (size_t i = 0; i < children.size(); ++i) {
        LayoutBox* child = children[i].box;
        if (!child->isOutOfFlowPositioned())
            ++count;
    }
    return count;
}

void LayoutFlexibleBox::resetAutoMarginsAndLogicalTopInCrossAxis(LayoutBox& child)
{
    if (hasAutoMarginsInCrossAxis(child)) {
        child.updateLogicalHeight();
        if (isHorizontalFlow()) {
            if (child.style()->marginTop().isAuto())
                child.setMarginTop(LayoutUnit());
            if (child.style()->marginBottom().isAuto())
                child.setMarginBottom(LayoutUnit());
        } else {
            if (child.style()->marginLeft().isAuto())
                child.setMarginLeft(LayoutUnit());
            if (child.style()->marginRight().isAuto())
                child.setMarginRight(LayoutUnit());
        }
    }
}

bool LayoutFlexibleBox::needToStretchChildLogicalHeight(const LayoutBox& child) const
{
    // This function is a little bit magical. It relies on the fact that blocks intrinsically
    // "stretch" themselves in their inline axis, i.e. a <div> has an implicit width: 100%.
    // So the child will automatically stretch if our cross axis is the child's inline axis. That's the case if:
    // - We are horizontal and the child is in vertical writing mode
    // - We are vertical and the child is in horizontal writing mode
    // Otherwise, we need to stretch if the cross axis size is auto.
    if (alignmentForChild(child) != ItemPositionStretch)
        return false;

    if (isHorizontalFlow() != child.styleRef().isHorizontalWritingMode())
        return false;

    // TODO(cbiesinger): what about indefinite percentage heights?
    return isHorizontalFlow() ? child.styleRef().height().isAuto() : child.styleRef().width().isAuto();
}

bool LayoutFlexibleBox::childHasIntrinsicMainAxisSize(const LayoutBox& child) const
{
    bool result = false;
    if (isHorizontalFlow() != child.styleRef().isHorizontalWritingMode()) {
        Length childFlexBasis = flexBasisForChild(child);
        Length childMinSize = isHorizontalFlow() ? child.style()->minWidth() : child.style()->minHeight();
        Length childMaxSize = isHorizontalFlow() ? child.style()->maxWidth() : child.style()->maxHeight();
        if (childFlexBasis.isIntrinsic() || childMinSize.isIntrinsicOrAuto() || childMaxSize.isIntrinsic())
            result = true;
    }
    return result;
}

EOverflow LayoutFlexibleBox::mainAxisOverflowForChild(const LayoutBox& child) const
{
    if (isHorizontalFlow())
        return child.styleRef().overflowX();
    return child.styleRef().overflowY();
}

EOverflow LayoutFlexibleBox::crossAxisOverflowForChild(const LayoutBox& child) const
{
    if (isHorizontalFlow())
        return child.styleRef().overflowY();
    return child.styleRef().overflowX();
}

static bool shouldPlaceBlockDirectionScrollbarOnLogicalLeft(const LayoutObject& obj)
{
    if (obj.isLayoutView())
        return false;
    return obj.style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft();
}

void LayoutFlexibleBox::layoutAndPlaceChildren(LayoutUnit& crossAxisOffset, const OrderedFlexItemList& children, LayoutUnit availableFreeSpace, bool relayoutChildren, SubtreeLayoutScope& layoutScope, Vector<LineContext>& lineContexts)
{
    ContentPosition position = resolvedJustifyContentPosition(styleRef(), normalValueBehavior());
    ContentDistributionType distribution = resolvedJustifyContentDistribution(styleRef(), normalValueBehavior());

    size_t numberOfChildrenForJustifyContent = numberOfInFlowPositionedChildren(children);
    LayoutUnit autoMarginOffset = autoMarginOffsetInMainAxis(children, availableFreeSpace);
    LayoutUnit mainAxisOffset = flowAwareBorderStart() + flowAwarePaddingStart();
    mainAxisOffset += initialJustifyContentOffset(availableFreeSpace, position, distribution, numberOfChildrenForJustifyContent);
    if (style()->flexDirection() == FlowRowReverse && shouldPlaceBlockDirectionScrollbarOnLogicalLeft(*this))
        mainAxisOffset += isHorizontalFlow() ? verticalScrollbarWidth() : horizontalScrollbarHeight();

    LayoutUnit totalMainExtent = mainAxisExtent();
    if (!shouldPlaceBlockDirectionScrollbarOnLogicalLeft(*this))
        totalMainExtent -= isHorizontalFlow() ? verticalScrollbarWidth() : horizontalScrollbarHeight();
    LayoutUnit maxAscent, maxDescent; // Used when align-items: baseline.
    LayoutUnit maxChildCrossAxisExtent;
    size_t seenInFlowPositionedChildren = 0;
    bool shouldFlipMainAxis = !isColumnFlow() && !isLeftToRightFlow();
    for (size_t i = 0; i < children.size(); ++i) {
        const FlexItem& flexItem = children[i];
        LayoutBox* child = flexItem.box;

        if (child->isOutOfFlowPositioned()) {
            prepareChildForPositionedLayout(*child);
            continue;
        }

        child->setMayNeedPaintInvalidation();

        LayoutUnit childPreferredSize = flexItem.flexedContentSize + mainAxisBorderAndPaddingExtentForChild(*child);
        setOverrideMainAxisSizeForChild(*child, childPreferredSize);
        if (childPreferredSize != mainAxisExtentForChild(*child)) {
            child->setChildNeedsLayout(MarkOnlyThis);
        } else {
            // To avoid double applying margin changes in updateAutoMarginsInCrossAxis, we reset the margins here.
            resetAutoMarginsAndLogicalTopInCrossAxis(*child);
        }
        // We may have already forced relayout for orthogonal flowing children in computeInnerFlexBaseSizeForChild.
        bool forceChildRelayout = relayoutChildren && !m_relaidOutChildren.contains(child);
        if (child->isLayoutBlock() && toLayoutBlock(*child).hasPercentHeightDescendants()) {
            // Have to force another relayout even though the child is sized correctly, because
            // its descendants are not sized correctly yet. Our previous layout of the child was
            // done without an override height set. So, redo it here.
            forceChildRelayout = true;
        }
        updateBlockChildDirtyBitsBeforeLayout(forceChildRelayout, *child);
        if (!child->needsLayout())
            child->markForPaginationRelayoutIfNeeded(layoutScope);
        if (child->needsLayout())
            m_relaidOutChildren.add(child);
        child->layoutIfNeeded();

        updateAutoMarginsInMainAxis(*child, autoMarginOffset);

        LayoutUnit childCrossAxisMarginBoxExtent;
        if (alignmentForChild(*child) == ItemPositionBaseline && !hasAutoMarginsInCrossAxis(*child)) {
            LayoutUnit ascent = marginBoxAscentForChild(*child);
            LayoutUnit descent = (crossAxisMarginExtentForChild(*child) + crossAxisExtentForChild(*child)) - ascent;

            maxAscent = std::max(maxAscent, ascent);
            maxDescent = std::max(maxDescent, descent);

            // TODO(cbiesinger): Take scrollbar into account
            childCrossAxisMarginBoxExtent = maxAscent + maxDescent;
        } else {
            childCrossAxisMarginBoxExtent = crossAxisIntrinsicExtentForChild(*child) + crossAxisMarginExtentForChild(*child) + crossAxisScrollbarExtentForChild(*child);
        }
        if (!isColumnFlow())
            setLogicalHeight(std::max(logicalHeight(), crossAxisOffset + flowAwareBorderAfter() + flowAwarePaddingAfter() + childCrossAxisMarginBoxExtent + crossAxisScrollbarExtent()));
        maxChildCrossAxisExtent = std::max(maxChildCrossAxisExtent, childCrossAxisMarginBoxExtent);

        mainAxisOffset += flowAwareMarginStartForChild(*child);

        LayoutUnit childMainExtent = mainAxisExtentForChild(*child);
        // In an RTL column situation, this will apply the margin-right/margin-end on the left.
        // This will be fixed later in flipForRightToLeftColumn.
        LayoutPoint childLocation(shouldFlipMainAxis ? totalMainExtent - mainAxisOffset - childMainExtent : mainAxisOffset,
            crossAxisOffset + flowAwareMarginBeforeForChild(*child));
        setFlowAwareLocationForChild(*child, childLocation);
        mainAxisOffset += childMainExtent + flowAwareMarginEndForChild(*child);

        ++seenInFlowPositionedChildren;
        if (seenInFlowPositionedChildren < numberOfChildrenForJustifyContent)
            mainAxisOffset += justifyContentSpaceBetweenChildren(availableFreeSpace, distribution, numberOfChildrenForJustifyContent);
    }

    if (isColumnFlow())
        setLogicalHeight(std::max(logicalHeight(), mainAxisOffset + flowAwareBorderEnd() + flowAwarePaddingEnd() + scrollbarLogicalHeight()));

    if (style()->flexDirection() == FlowColumnReverse) {
        // We have to do an extra pass for column-reverse to reposition the flex items since the start depends
        // on the height of the flexbox, which we only know after we've positioned all the flex items.
        updateLogicalHeight();
        layoutColumnReverse(children, crossAxisOffset, availableFreeSpace);
    }

    if (m_numberOfInFlowChildrenOnFirstLine == -1)
        m_numberOfInFlowChildrenOnFirstLine = seenInFlowPositionedChildren;
    lineContexts.append(LineContext(crossAxisOffset, maxChildCrossAxisExtent, children.size(), maxAscent));
    crossAxisOffset += maxChildCrossAxisExtent;
}

void LayoutFlexibleBox::layoutColumnReverse(const OrderedFlexItemList& children, LayoutUnit crossAxisOffset, LayoutUnit availableFreeSpace)
{
    ContentPosition position = resolvedJustifyContentPosition(styleRef(), normalValueBehavior());
    ContentDistributionType distribution = resolvedJustifyContentDistribution(styleRef(), normalValueBehavior());

    // This is similar to the logic in layoutAndPlaceChildren, except we place the children
    // starting from the end of the flexbox. We also don't need to layout anything since we're
    // just moving the children to a new position.
    size_t numberOfChildrenForJustifyContent = numberOfInFlowPositionedChildren(children);
    LayoutUnit mainAxisOffset = logicalHeight() - flowAwareBorderEnd() - flowAwarePaddingEnd();
    mainAxisOffset -= initialJustifyContentOffset(availableFreeSpace, position, distribution, numberOfChildrenForJustifyContent);
    mainAxisOffset -= isHorizontalFlow() ? verticalScrollbarWidth() : horizontalScrollbarHeight();

    size_t seenInFlowPositionedChildren = 0;
    for (size_t i = 0; i < children.size(); ++i) {
        LayoutBox* child = children[i].box;

        if (child->isOutOfFlowPositioned())
            continue;

        mainAxisOffset -= mainAxisExtentForChild(*child) + flowAwareMarginEndForChild(*child);

        setFlowAwareLocationForChild(*child, LayoutPoint(mainAxisOffset, crossAxisOffset + flowAwareMarginBeforeForChild(*child)));

        mainAxisOffset -= flowAwareMarginStartForChild(*child);

        ++seenInFlowPositionedChildren;
        if (seenInFlowPositionedChildren < numberOfChildrenForJustifyContent)
            mainAxisOffset -= justifyContentSpaceBetweenChildren(availableFreeSpace, distribution, numberOfChildrenForJustifyContent);
    }
}

static LayoutUnit initialAlignContentOffset(LayoutUnit availableFreeSpace, ContentPosition alignContent, ContentDistributionType alignContentDistribution, unsigned numberOfLines)
{
    if (numberOfLines <= 1)
        return LayoutUnit();
    if (alignContent == ContentPositionFlexEnd)
        return availableFreeSpace;
    if (alignContent == ContentPositionCenter)
        return availableFreeSpace / 2;
    if (alignContentDistribution == ContentDistributionSpaceAround) {
        if (availableFreeSpace > 0 && numberOfLines)
            return availableFreeSpace / (2 * numberOfLines);
        if (availableFreeSpace < 0)
            return availableFreeSpace / 2;
    }
    return LayoutUnit();
}

static LayoutUnit alignContentSpaceBetweenChildren(LayoutUnit availableFreeSpace, ContentDistributionType alignContentDistribution, unsigned numberOfLines)
{
    if (availableFreeSpace > 0 && numberOfLines > 1) {
        if (alignContentDistribution == ContentDistributionSpaceBetween)
            return availableFreeSpace / (numberOfLines - 1);
        if (alignContentDistribution == ContentDistributionSpaceAround || alignContentDistribution == ContentDistributionStretch)
            return availableFreeSpace / numberOfLines;
    }
    return LayoutUnit();
}

void LayoutFlexibleBox::alignFlexLines(Vector<LineContext>& lineContexts)
{
    ContentPosition position = resolvedAlignContentPosition(styleRef(), normalValueBehavior());
    ContentDistributionType distribution = resolvedAlignContentDistribution(styleRef(), normalValueBehavior());

    // If we have a single line flexbox or a multiline line flexbox with only one flex line,
    // the line height is all the available space.
    // For flex-direction: row, this means we need to use the height, so we do this after calling updateLogicalHeight.
    if (lineContexts.size() == 1) {
        lineContexts[0].crossAxisExtent = crossAxisContentExtent();
        return;
    }

    if (position == ContentPositionFlexStart)
        return;

    LayoutUnit availableCrossAxisSpace = crossAxisContentExtent();
    for (size_t i = 0; i < lineContexts.size(); ++i)
        availableCrossAxisSpace -= lineContexts[i].crossAxisExtent;

    LayoutBox* child = m_orderIterator.first();
    LayoutUnit lineOffset = initialAlignContentOffset(availableCrossAxisSpace, position, distribution, lineContexts.size());
    for (unsigned lineNumber = 0; lineNumber < lineContexts.size(); ++lineNumber) {
        lineContexts[lineNumber].crossAxisOffset += lineOffset;
        for (size_t childNumber = 0; childNumber < lineContexts[lineNumber].numberOfChildren; ++childNumber, child = m_orderIterator.next())
            adjustAlignmentForChild(*child, lineOffset);

        if (distribution == ContentDistributionStretch && availableCrossAxisSpace > 0)
            lineContexts[lineNumber].crossAxisExtent += availableCrossAxisSpace / static_cast<unsigned>(lineContexts.size());

        lineOffset += alignContentSpaceBetweenChildren(availableCrossAxisSpace, distribution, lineContexts.size());
    }
}

void LayoutFlexibleBox::adjustAlignmentForChild(LayoutBox& child, LayoutUnit delta)
{
    if (child.isOutOfFlowPositioned())
        return;

    setFlowAwareLocationForChild(child, flowAwareLocationForChild(child) + LayoutSize(LayoutUnit(), delta));
}

void LayoutFlexibleBox::alignChildren(const Vector<LineContext>& lineContexts)
{
    // Keep track of the space between the baseline edge and the after edge of the box for each line.
    Vector<LayoutUnit> minMarginAfterBaselines;

    LayoutBox* child = m_orderIterator.first();
    for (size_t lineNumber = 0; lineNumber < lineContexts.size(); ++lineNumber) {
        LayoutUnit minMarginAfterBaseline = LayoutUnit::max();
        LayoutUnit lineCrossAxisExtent = lineContexts[lineNumber].crossAxisExtent;
        LayoutUnit maxAscent = lineContexts[lineNumber].maxAscent;

        for (size_t childNumber = 0; childNumber < lineContexts[lineNumber].numberOfChildren; ++childNumber, child = m_orderIterator.next()) {
            ASSERT(child);
            if (child->isOutOfFlowPositioned()) {
                if (style()->flexWrap() == FlexWrapReverse)
                    adjustAlignmentForChild(*child, lineCrossAxisExtent);
                continue;
            }

            if (updateAutoMarginsInCrossAxis(*child, std::max(LayoutUnit(), availableAlignmentSpaceForChild(lineCrossAxisExtent, *child))))
                continue;

            ItemPosition position = alignmentForChild(*child);
            if (position == ItemPositionStretch)
                applyStretchAlignmentToChild(*child, lineCrossAxisExtent);
            LayoutUnit availableSpace = availableAlignmentSpaceForChild(lineCrossAxisExtent, *child);
            LayoutUnit offset = alignmentOffset(availableSpace, position, marginBoxAscentForChild(*child), maxAscent, styleRef().flexWrap() == FlexWrapReverse);
            adjustAlignmentForChild(*child, offset);
            if (position == ItemPositionBaseline && styleRef().flexWrap() == FlexWrapReverse)
                minMarginAfterBaseline = std::min(minMarginAfterBaseline, availableAlignmentSpaceForChild(lineCrossAxisExtent, *child) - offset);
        }
        minMarginAfterBaselines.append(minMarginAfterBaseline);
    }

    if (style()->flexWrap() != FlexWrapReverse)
        return;

    // wrap-reverse flips the cross axis start and end. For baseline alignment, this means we
    // need to align the after edge of baseline elements with the after edge of the flex line.
    child = m_orderIterator.first();
    for (size_t lineNumber = 0; lineNumber < lineContexts.size(); ++lineNumber) {
        LayoutUnit minMarginAfterBaseline = minMarginAfterBaselines[lineNumber];
        for (size_t childNumber = 0; childNumber < lineContexts[lineNumber].numberOfChildren; ++childNumber, child = m_orderIterator.next()) {
            ASSERT(child);
            if (alignmentForChild(*child) == ItemPositionBaseline && !hasAutoMarginsInCrossAxis(*child) && minMarginAfterBaseline)
                adjustAlignmentForChild(*child, minMarginAfterBaseline);
        }
    }
}

static LayoutUnit clampNegativeToZero(const LayoutUnit& self)
{
    if (self.rawValue() > 0)
        return self;
    return LayoutUnit();
}

void LayoutFlexibleBox::applyStretchAlignmentToChild(LayoutBox& child, LayoutUnit lineCrossAxisExtent)
{
    if (!hasOrthogonalFlow(child) && child.style()->logicalHeight().isAuto()) {
        LayoutUnit heightBeforeStretching = needToStretchChildLogicalHeight(child) ? constrainedChildIntrinsicContentLogicalHeight(child) : child.logicalHeight();
        LayoutUnit stretchedLogicalHeight = std::max(child.borderAndPaddingLogicalHeight(), heightBeforeStretching + availableAlignmentSpaceForChildBeforeStretching(lineCrossAxisExtent, child));
        ASSERT(!child.needsLayout());
        LayoutUnit desiredLogicalHeight = child.constrainLogicalHeightByMinMax(stretchedLogicalHeight, heightBeforeStretching - child.borderAndPaddingLogicalHeight());

        // FIXME: Can avoid laying out here in some cases. See https://webkit.org/b/87905.
        bool childNeedsRelayout = desiredLogicalHeight != child.logicalHeight();
        if (child.isLayoutBlock() && toLayoutBlock(child).hasPercentHeightDescendants() && m_relaidOutChildren.contains(&child)) {
            // Have to force another relayout even though the child is sized correctly, because
            // its descendants are not sized correctly yet. Our previous layout of the child was
            // done without an override height set. So, redo it here.
            childNeedsRelayout = true;
        }
        if (childNeedsRelayout || !child.hasOverrideLogicalContentHeight())
            child.setOverrideLogicalContentHeight(desiredLogicalHeight - child.borderAndPaddingLogicalHeight());
        if (childNeedsRelayout) {
            child.setLogicalHeight(LayoutUnit());
            // We cache the child's intrinsic content logical height to avoid it being reset to the stretched height.
            // FIXME: This is fragile. LayoutBoxes should be smart enough to determine their intrinsic content logical
            // height correctly even when there's an overrideHeight.
            LayoutUnit childIntrinsicContentLogicalHeight = child.intrinsicContentLogicalHeight();
            child.forceChildLayout();
            child.setIntrinsicContentLogicalHeight(childIntrinsicContentLogicalHeight);
        }
    } else if (hasOrthogonalFlow(child) && child.style()->logicalWidth().isAuto()) {
        LayoutUnit childWidth = clampNegativeToZero(lineCrossAxisExtent - crossAxisMarginExtentForChild(child));
        childWidth = child.constrainLogicalWidthByMinMax(childWidth, childWidth, this);

        if (childWidth != child.logicalWidth()) {
            child.setOverrideLogicalContentWidth(childWidth - child.borderAndPaddingLogicalWidth());
            child.forceChildLayout();
        }
    }
}

void LayoutFlexibleBox::flipForRightToLeftColumn()
{
    if (style()->isLeftToRightDirection() || !isColumnFlow())
        return;

    LayoutUnit crossExtent = crossAxisExtent();
    for (LayoutBox* child = m_orderIterator.first(); child; child = m_orderIterator.next()) {
        if (child->isOutOfFlowPositioned())
            continue;
        LayoutPoint location = flowAwareLocationForChild(*child);
        // For vertical flows, setFlowAwareLocationForChild will transpose x and y,
        // so using the y axis for a column cross axis extent is correct.
        location.setY(crossExtent - crossAxisExtentForChild(*child) - location.y());
        if (!isHorizontalWritingMode())
            location.move(LayoutSize(0, -horizontalScrollbarHeight()));
        setFlowAwareLocationForChild(*child, location);
    }
}

void LayoutFlexibleBox::flipForWrapReverse(const Vector<LineContext>& lineContexts, LayoutUnit crossAxisStartEdge)
{
    LayoutUnit contentExtent = crossAxisContentExtent();
    LayoutBox* child = m_orderIterator.first();
    for (size_t lineNumber = 0; lineNumber < lineContexts.size(); ++lineNumber) {
        for (size_t childNumber = 0; childNumber < lineContexts[lineNumber].numberOfChildren; ++childNumber, child = m_orderIterator.next()) {
            ASSERT(child);
            LayoutUnit lineCrossAxisExtent = lineContexts[lineNumber].crossAxisExtent;
            LayoutUnit originalOffset = lineContexts[lineNumber].crossAxisOffset - crossAxisStartEdge;
            LayoutUnit newOffset = contentExtent - originalOffset - lineCrossAxisExtent;
            adjustAlignmentForChild(*child, newOffset - originalOffset);
        }
    }
}

} // namespace blink
