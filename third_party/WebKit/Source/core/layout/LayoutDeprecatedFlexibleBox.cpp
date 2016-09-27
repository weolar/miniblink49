/*
 * This file is part of the layout object implementation for KHTML.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003 Apple Computer, Inc.
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
 *
 */

#include "config.h"
#include "core/layout/LayoutDeprecatedFlexibleBox.h"

#include "core/frame/UseCounter.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextAutosizer.h"
#include "core/layout/TextRunConstructor.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/fonts/Font.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

class FlexBoxIterator {
public:
    FlexBoxIterator(LayoutDeprecatedFlexibleBox* parent)
        : m_box(parent)
        , m_largestOrdinal(1)
    {
        if (m_box->style()->boxOrient() == HORIZONTAL && !m_box->style()->isLeftToRightDirection())
            m_forward = m_box->style()->boxDirection() != BNORMAL;
        else
            m_forward = m_box->style()->boxDirection() == BNORMAL;
        if (!m_forward) {
            // No choice, since we're going backwards, we have to find out the highest ordinal up front.
            LayoutBox* child = m_box->firstChildBox();
            while (child) {
                if (child->style()->boxOrdinalGroup() > m_largestOrdinal)
                    m_largestOrdinal = child->style()->boxOrdinalGroup();
                child = child->nextSiblingBox();
            }
        }

        reset();
    }

    void reset()
    {
        m_currentChild = 0;
        m_ordinalIteration = -1;
    }

    LayoutBox* first()
    {
        reset();
        return next();
    }

    LayoutBox* next()
    {
        do {
            if (!m_currentChild) {
                ++m_ordinalIteration;

                if (!m_ordinalIteration) {
                    m_currentOrdinal = m_forward ? 1 : m_largestOrdinal;
                } else {
                    if (static_cast<size_t>(m_ordinalIteration) >= m_ordinalValues.size() + 1)
                        return nullptr;

                    // Only copy+sort the values once per layout even if the iterator is reset.
                    if (m_ordinalValues.size() != m_sortedOrdinalValues.size()) {
                        copyToVector(m_ordinalValues, m_sortedOrdinalValues);
                        std::sort(m_sortedOrdinalValues.begin(), m_sortedOrdinalValues.end());
                    }
                    m_currentOrdinal = m_forward ? m_sortedOrdinalValues[m_ordinalIteration - 1] : m_sortedOrdinalValues[m_sortedOrdinalValues.size() - m_ordinalIteration];
                }

                m_currentChild = m_forward ? m_box->firstChildBox() : m_box->lastChildBox();
            } else {
                m_currentChild = m_forward ? m_currentChild->nextSiblingBox() : m_currentChild->previousSiblingBox();
            }

            if (m_currentChild && notFirstOrdinalValue())
                m_ordinalValues.add(m_currentChild->style()->boxOrdinalGroup());
        } while (!m_currentChild || (!m_currentChild->isAnonymous()
            && m_currentChild->style()->boxOrdinalGroup() != m_currentOrdinal));
        return m_currentChild;
    }

private:
    bool notFirstOrdinalValue()
    {
        unsigned firstOrdinalValue = m_forward ? 1 : m_largestOrdinal;
        return m_currentOrdinal == firstOrdinalValue && m_currentChild->style()->boxOrdinalGroup() != firstOrdinalValue;
    }

    LayoutDeprecatedFlexibleBox* m_box;
    LayoutBox* m_currentChild;
    bool m_forward;
    unsigned m_currentOrdinal;
    unsigned m_largestOrdinal;
    HashSet<unsigned> m_ordinalValues;
    Vector<unsigned> m_sortedOrdinalValues;
    int m_ordinalIteration;
};

LayoutDeprecatedFlexibleBox::LayoutDeprecatedFlexibleBox(Element& element)
    : LayoutBlock(&element)
{
    ASSERT(!childrenInline());
    m_stretchingChildren = false;
    if (!isAnonymous()) {
        const KURL& url = document().url();
        if (url.protocolIs("chrome"))
            UseCounter::count(document(), UseCounter::DeprecatedFlexboxChrome);
        else if (url.protocolIs("chrome-extension"))
            UseCounter::count(document(), UseCounter::DeprecatedFlexboxChromeExtension);
        else
            UseCounter::count(document(), UseCounter::DeprecatedFlexboxWebContent);
    }
}

LayoutDeprecatedFlexibleBox::~LayoutDeprecatedFlexibleBox()
{
}

static LayoutUnit marginWidthForChild(LayoutBox* child)
{
    // A margin basically has three types: fixed, percentage, and auto (variable).
    // Auto and percentage margins simply become 0 when computing min/max width.
    // Fixed margins can be added in as is.
    Length marginLeft = child->style()->marginLeft();
    Length marginRight = child->style()->marginRight();
    LayoutUnit margin = 0;
    if (marginLeft.isFixed())
        margin += marginLeft.value();
    if (marginRight.isFixed())
        margin += marginRight.value();
    return margin;
}

static bool childDoesNotAffectWidthOrFlexing(LayoutObject* child)
{
    // Positioned children and collapsed children don't affect the min/max width.
    return child->isOutOfFlowPositioned() || child->style()->visibility() == COLLAPSE;
}

static LayoutUnit contentWidthForChild(LayoutBox* child)
{
    if (child->hasOverrideLogicalContentWidth())
        return child->overrideLogicalContentWidth();
    return child->logicalWidth() - child->borderAndPaddingLogicalWidth();
}

static LayoutUnit contentHeightForChild(LayoutBox* child)
{
    if (child->hasOverrideLogicalContentHeight())
        return child->overrideLogicalContentHeight();
    return child->logicalHeight() - child->borderAndPaddingLogicalHeight();
}

void LayoutDeprecatedFlexibleBox::styleWillChange(StyleDifference diff, const ComputedStyle& newStyle)
{
    const ComputedStyle* oldStyle = style();
    if (oldStyle && !oldStyle->lineClamp().isNone() && newStyle.lineClamp().isNone())
        clearLineClamp();

    LayoutBlock::styleWillChange(diff, newStyle);
}

void LayoutDeprecatedFlexibleBox::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    if (hasMultipleLines() || isVertical()) {
        for (LayoutBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
            if (childDoesNotAffectWidthOrFlexing(child))
                continue;

            LayoutUnit margin = marginWidthForChild(child);
            LayoutUnit width = child->minPreferredLogicalWidth() + margin;
            minLogicalWidth = std::max(width, minLogicalWidth);

            width = child->maxPreferredLogicalWidth() + margin;
            maxLogicalWidth = std::max(width, maxLogicalWidth);
        }
    } else {
        for (LayoutBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
            if (childDoesNotAffectWidthOrFlexing(child))
                continue;

            LayoutUnit margin = marginWidthForChild(child);
            minLogicalWidth += child->minPreferredLogicalWidth() + margin;
            maxLogicalWidth += child->maxPreferredLogicalWidth() + margin;
        }
    }

    maxLogicalWidth = std::max(minLogicalWidth, maxLogicalWidth);

    LayoutUnit scrollbarWidth = intrinsicScrollbarLogicalWidth();
    maxLogicalWidth += scrollbarWidth;
    minLogicalWidth += scrollbarWidth;
}

void LayoutDeprecatedFlexibleBox::layoutBlock(bool relayoutChildren)
{
    ASSERT(needsLayout());

    if (!relayoutChildren && simplifiedLayout())
        return;

    {
        // LayoutState needs this deliberate scope to pop before paint invalidation.
        LayoutState state(*this, locationOffset());

        LayoutSize previousSize = size();

        updateLogicalWidth();
        updateLogicalHeight();

        TextAutosizer::LayoutScope textAutosizerLayoutScope(this);

        if (previousSize != size()
            || (parent()->isDeprecatedFlexibleBox() && parent()->style()->boxOrient() == HORIZONTAL
            && parent()->style()->boxAlign() == BSTRETCH))
            relayoutChildren = true;

        setHeight(0);

        m_stretchingChildren = false;

        if (isHorizontal())
            layoutHorizontalBox(relayoutChildren);
        else
            layoutVerticalBox(relayoutChildren);

        LayoutUnit oldClientAfterEdge = clientLogicalBottom();
        updateLogicalHeight();

        if (previousSize.height() != size().height())
            relayoutChildren = true;

        layoutPositionedObjects(relayoutChildren || isDocumentElement());

        computeOverflow(oldClientAfterEdge);
    }

    updateLayerTransformAfterLayout();

    if (view()->layoutState()->pageLogicalHeight())
        setPageLogicalOffset(view()->layoutState()->pageLogicalOffset(*this, logicalTop()));

    // Update our scrollbars if we're overflow:auto/scroll/hidden now that we know if
    // we overflow or not.
    if (hasOverflowClip())
        layer()->scrollableArea()->updateAfterLayout();

    clearNeedsLayout();
}

// The first walk over our kids is to find out if we have any flexible children.
static void gatherFlexChildrenInfo(FlexBoxIterator& iterator, bool relayoutChildren, unsigned& highestFlexGroup, unsigned& lowestFlexGroup, bool& haveFlex)
{
    for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
        // Check to see if this child flexes.
        if (!childDoesNotAffectWidthOrFlexing(child) && child->style()->boxFlex() > 0.0f) {
            // We always have to lay out flexible objects again, since the flex distribution
            // may have changed, and we need to reallocate space.
            child->clearOverrideSize();
            if (!relayoutChildren)
                child->setChildNeedsLayout(MarkOnlyThis);
            haveFlex = true;
            unsigned flexGroup = child->style()->boxFlexGroup();
            if (lowestFlexGroup == 0)
                lowestFlexGroup = flexGroup;
            if (flexGroup < lowestFlexGroup)
                lowestFlexGroup = flexGroup;
            if (flexGroup > highestFlexGroup)
                highestFlexGroup = flexGroup;
        }
    }
}

void LayoutDeprecatedFlexibleBox::layoutHorizontalBox(bool relayoutChildren)
{
    LayoutUnit toAdd = borderBottom() + paddingBottom() + horizontalScrollbarHeight();
    LayoutUnit yPos = borderTop() + paddingTop();
    LayoutUnit xPos = borderLeft() + paddingLeft();
    bool heightSpecified = false;
    LayoutUnit oldHeight = 0;

    LayoutUnit remainingSpace = 0;


    FlexBoxIterator iterator(this);
    unsigned highestFlexGroup = 0;
    unsigned lowestFlexGroup = 0;
    bool haveFlex = false, flexingChildren = false;
    gatherFlexChildrenInfo(iterator, relayoutChildren, highestFlexGroup, lowestFlexGroup, haveFlex);

    LayoutBlock::startDelayUpdateScrollInfo();

    // We do 2 passes.  The first pass is simply to lay everyone out at
    // their preferred widths.  The second pass handles flexing the children.
    do {
        // Reset our height.
        setHeight(yPos);

        xPos = borderLeft() + paddingLeft();

        // Our first pass is done without flexing.  We simply lay the children
        // out within the box.  We have to do a layout first in order to determine
        // our box's intrinsic height.
        LayoutUnit maxAscent = 0, maxDescent = 0;
        for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
            if (child->isOutOfFlowPositioned())
                continue;

            SubtreeLayoutScope layoutScope(*child);
            if (relayoutChildren || (child->isReplaced() && (child->style()->width().hasPercent() || child->style()->height().hasPercent())))
                layoutScope.setChildNeedsLayout(child);

            // Compute the child's vertical margins.
            child->computeAndSetBlockDirectionMargins(this);

            if (!child->needsLayout())
                child->markForPaginationRelayoutIfNeeded(layoutScope);

            // Now do the layout.
            child->layoutIfNeeded();

            // Update our height and overflow height.
            if (style()->boxAlign() == BBASELINE) {
                LayoutUnit ascent = child->firstLineBoxBaseline();
                if (ascent == -1)
                    ascent = child->size().height() + child->marginBottom();
                ascent += child->marginTop();
                LayoutUnit descent = (child->size().height() + child->marginHeight()) - ascent;

                // Update our maximum ascent.
                maxAscent = std::max(maxAscent, ascent);

                // Update our maximum descent.
                maxDescent = std::max(maxDescent, descent);

                // Now update our height.
                setHeight(std::max(yPos + maxAscent + maxDescent, size().height()));
            } else {
                setHeight(std::max(size().height(), yPos + child->size().height() + child->marginHeight()));
            }
        }

        if (!iterator.first() && hasLineIfEmpty())
            setHeight(size().height() + lineHeight(true, style()->isHorizontalWritingMode() ? HorizontalLine : VerticalLine, PositionOfInteriorLineBoxes));

        setHeight(size().height() + toAdd);

        oldHeight = size().height();
        updateLogicalHeight();

        relayoutChildren = false;
        if (oldHeight != size().height())
            heightSpecified = true;

        // Now that our height is actually known, we can place our boxes.
        m_stretchingChildren = (style()->boxAlign() == BSTRETCH);
        for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
            if (child->isOutOfFlowPositioned()) {
                child->containingBlock()->insertPositionedObject(child);
                DeprecatedPaintLayer* childLayer = child->layer();
                childLayer->setStaticInlinePosition(xPos);
                if (childLayer->staticBlockPosition() != yPos) {
                    childLayer->setStaticBlockPosition(yPos);
                    if (child->style()->hasStaticBlockPosition(style()->isHorizontalWritingMode()))
                        child->setChildNeedsLayout(MarkOnlyThis);
                }
                continue;
            }

            if (child->style()->visibility() == COLLAPSE) {
                // visibility: collapsed children do not participate in our positioning.
                // But we need to lay them down.
                child->layoutIfNeeded();
                continue;
            }

            SubtreeLayoutScope layoutScope(*child);

            // We need to see if this child's height will change, since we make block elements fill
            // the height of a containing box by default. We cannot actually *set* the new height
            // here, though. Need to do that from within layout, or we won't be able to detect the
            // change and duly notify any positioned descendants that are affected by it.
            LayoutUnit oldChildHeight = child->logicalHeight();
            LogicalExtentComputedValues computedValues;
            child->computeLogicalHeight(child->logicalHeight(), child->logicalTop(), computedValues);
            LayoutUnit newChildHeight = computedValues.m_extent;
            if (oldChildHeight != newChildHeight)
                layoutScope.setChildNeedsLayout(child);

            if (!child->needsLayout())
                child->markForPaginationRelayoutIfNeeded(layoutScope);

            child->layoutIfNeeded();

            // We can place the child now, using our value of box-align.
            xPos += child->marginLeft();
            LayoutUnit childY = yPos;
            switch (style()->boxAlign()) {
            case BCENTER:
                childY += child->marginTop() + std::max<LayoutUnit>(0, (contentHeight() - (child->size().height() + child->marginHeight())) / 2);
                break;
            case BBASELINE: {
                LayoutUnit ascent = child->firstLineBoxBaseline();
                if (ascent == -1)
                    ascent = child->size().height() + child->marginBottom();
                ascent += child->marginTop();
                childY += child->marginTop() + (maxAscent - ascent);
                break;
            }
            case BEND:
                childY += contentHeight() - child->marginBottom() - child->size().height();
                break;
            default: // BSTART
                childY += child->marginTop();
                break;
            }

            placeChild(child, LayoutPoint(xPos, childY));

            xPos += child->size().width() + child->marginRight();
        }

        remainingSpace = size().width() - borderRight() - paddingRight() - verticalScrollbarWidth() - xPos;

        m_stretchingChildren = false;
        if (flexingChildren) {
            haveFlex = false; // We're done.
        } else if (haveFlex) {
            // We have some flexible objects.  See if we need to grow/shrink them at all.
            if (!remainingSpace)
                break;

            // Allocate the remaining space among the flexible objects.  If we are trying to
            // grow, then we go from the lowest flex group to the highest flex group.  For shrinking,
            // we go from the highest flex group to the lowest group.
            bool expanding = remainingSpace > 0;
            unsigned start = expanding ? lowestFlexGroup : highestFlexGroup;
            unsigned end = expanding? highestFlexGroup : lowestFlexGroup;
            for (unsigned i = start; i <= end && remainingSpace; i++) {
                // Always start off by assuming the group can get all the remaining space.
                LayoutUnit groupRemainingSpace = remainingSpace;
                do {
                    // Flexing consists of multiple passes, since we have to change ratios every time an object hits its max/min-width
                    // For a given pass, we always start off by computing the totalFlex of all objects that can grow/shrink at all, and
                    // computing the allowed growth before an object hits its min/max width (and thus
                    // forces a totalFlex recomputation).
                    LayoutUnit groupRemainingSpaceAtBeginning = groupRemainingSpace;
                    float totalFlex = 0.0f;
                    for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                        if (allowedChildFlex(child, expanding, i))
                            totalFlex += child->style()->boxFlex();
                    }
                    LayoutUnit spaceAvailableThisPass = groupRemainingSpace;
                    for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                        LayoutUnit allowedFlex = allowedChildFlex(child, expanding, i);
                        if (allowedFlex) {
                            LayoutUnit projectedFlex = (allowedFlex == LayoutUnit::max()) ? allowedFlex : LayoutUnit(allowedFlex * (totalFlex / child->style()->boxFlex()));
                            spaceAvailableThisPass = expanding ? std::min(spaceAvailableThisPass, projectedFlex) : std::max(spaceAvailableThisPass, projectedFlex);
                        }
                    }

                    // The flex groups may not have any flexible objects this time around.
                    if (!spaceAvailableThisPass || totalFlex == 0.0f) {
                        // If we just couldn't grow/shrink any more, then it's time to transition to the next flex group.
                        groupRemainingSpace = 0;
                        continue;
                    }

                    // Now distribute the space to objects.
                    for (LayoutBox* child = iterator.first(); child && spaceAvailableThisPass && totalFlex; child = iterator.next()) {
                        if (child->style()->visibility() == COLLAPSE)
                            continue;

                        if (allowedChildFlex(child, expanding, i)) {
                            LayoutUnit spaceAdd = LayoutUnit(spaceAvailableThisPass * (child->style()->boxFlex() / totalFlex));
                            if (spaceAdd) {
                                child->setOverrideLogicalContentWidth(contentWidthForChild(child) + spaceAdd);
                                flexingChildren = true;
                                relayoutChildren = true;
                            }

                            spaceAvailableThisPass -= spaceAdd;
                            remainingSpace -= spaceAdd;
                            groupRemainingSpace -= spaceAdd;

                            totalFlex -= child->style()->boxFlex();
                        }
                    }
                    if (groupRemainingSpace == groupRemainingSpaceAtBeginning) {
                        // This is not advancing, avoid getting stuck by distributing the remaining pixels.
                        LayoutUnit spaceAdd = groupRemainingSpace > 0 ? 1 : -1;
                        for (LayoutBox* child = iterator.first(); child && groupRemainingSpace; child = iterator.next()) {
                            if (allowedChildFlex(child, expanding, i)) {
                                child->setOverrideLogicalContentWidth(contentWidthForChild(child) + spaceAdd);
                                flexingChildren = true;
                                relayoutChildren = true;
                                remainingSpace -= spaceAdd;
                                groupRemainingSpace -= spaceAdd;
                            }
                        }
                    }
                } while (absoluteValue(groupRemainingSpace) >= 1);
            }

            // We didn't find any children that could grow.
            if (haveFlex && !flexingChildren)
                haveFlex = false;
        }
    } while (haveFlex);

    LayoutBlock::finishDelayUpdateScrollInfo();

    if (remainingSpace > 0 && ((style()->isLeftToRightDirection() && style()->boxPack() != Start)
        || (!style()->isLeftToRightDirection() && style()->boxPack() != End))) {
        // Children must be repositioned.
        LayoutUnit offset = 0;
        if (style()->boxPack() == Justify) {
            // Determine the total number of children.
            int totalChildren = 0;
            for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                if (childDoesNotAffectWidthOrFlexing(child))
                    continue;
                ++totalChildren;
            }

            // Iterate over the children and space them out according to the
            // justification level.
            if (totalChildren > 1) {
                --totalChildren;
                bool firstChild = true;
                for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                    if (childDoesNotAffectWidthOrFlexing(child))
                        continue;

                    if (firstChild) {
                        firstChild = false;
                        continue;
                    }

                    offset += remainingSpace / totalChildren;
                    remainingSpace -= (remainingSpace / totalChildren);
                    --totalChildren;

                    placeChild(child, child->location() + LayoutSize(offset, 0));
                }
            }
        } else {
            if (style()->boxPack() == Center)
                offset += remainingSpace / 2;
            else // END for LTR, START for RTL
                offset += remainingSpace;
            for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                if (childDoesNotAffectWidthOrFlexing(child))
                    continue;

                placeChild(child, child->location() + LayoutSize(offset, 0));
            }
        }
    }

    // So that the computeLogicalHeight in layoutBlock() knows to relayout positioned objects because of
    // a height change, we revert our height back to the intrinsic height before returning.
    if (heightSpecified)
        setHeight(oldHeight);
}

void LayoutDeprecatedFlexibleBox::layoutVerticalBox(bool relayoutChildren)
{
    LayoutUnit yPos = borderTop() + paddingTop();
    LayoutUnit toAdd = borderBottom() + paddingBottom() + horizontalScrollbarHeight();
    bool heightSpecified = false;
    LayoutUnit oldHeight = 0;

    LayoutUnit remainingSpace = 0;

    FlexBoxIterator iterator(this);
    unsigned highestFlexGroup = 0;
    unsigned lowestFlexGroup = 0;
    bool haveFlex = false, flexingChildren = false;
    gatherFlexChildrenInfo(iterator, relayoutChildren, highestFlexGroup, lowestFlexGroup, haveFlex);

    // We confine the line clamp ugliness to vertical flexible boxes (thus keeping it out of
    // mainstream block layout); this is not really part of the XUL box model.
    bool haveLineClamp = !style()->lineClamp().isNone();
    if (haveLineClamp)
        applyLineClamp(iterator, relayoutChildren);

    LayoutBlock::startDelayUpdateScrollInfo();

    // We do 2 passes.  The first pass is simply to lay everyone out at
    // their preferred widths.  The second pass handles flexing the children.
    // Our first pass is done without flexing.  We simply lay the children
    // out within the box.
    do {
        setHeight(borderTop() + paddingTop());
        LayoutUnit minHeight = size().height() + toAdd;

        for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
            if (child->isOutOfFlowPositioned()) {
                child->containingBlock()->insertPositionedObject(child);
                DeprecatedPaintLayer* childLayer = child->layer();
                childLayer->setStaticInlinePosition(borderStart() + paddingStart());
                if (childLayer->staticBlockPosition() != size().height()) {
                    childLayer->setStaticBlockPosition(size().height());
                    if (child->style()->hasStaticBlockPosition(style()->isHorizontalWritingMode()))
                        child->setChildNeedsLayout(MarkOnlyThis);
                }
                continue;
            }

            SubtreeLayoutScope layoutScope(*child);
            if (!haveLineClamp && (relayoutChildren || (child->isReplaced() && (child->style()->width().hasPercent() || child->style()->height().hasPercent()))))
                layoutScope.setChildNeedsLayout(child);

            if (child->style()->visibility() == COLLAPSE) {
                // visibility: collapsed children do not participate in our positioning.
                // But we need to lay them down.
                child->layoutIfNeeded();
                continue;
            }

            // Compute the child's vertical margins.
            child->computeAndSetBlockDirectionMargins(this);

            // Add in the child's marginTop to our height.
            setHeight(size().height() + child->marginTop());

            if (!child->needsLayout())
                child->markForPaginationRelayoutIfNeeded(layoutScope);

            // Now do a layout.
            child->layoutIfNeeded();

            // We can place the child now, using our value of box-align.
            LayoutUnit childX = borderLeft() + paddingLeft();
            switch (style()->boxAlign()) {
            case BCENTER:
            case BBASELINE: // Baseline just maps to center for vertical boxes
                childX += child->marginLeft() + std::max<LayoutUnit>(0, (contentWidth() - (child->size().width() + child->marginWidth())) / 2);
                break;
            case BEND:
                if (!style()->isLeftToRightDirection())
                    childX += child->marginLeft();
                else
                    childX += contentWidth() - child->marginRight() - child->size().width();
                break;
            default: // BSTART/BSTRETCH
                if (style()->isLeftToRightDirection())
                    childX += child->marginLeft();
                else
                    childX += contentWidth() - child->marginRight() - child->size().width();
                break;
            }

            // Place the child.
            placeChild(child, LayoutPoint(childX, size().height()));
            setHeight(size().height() + child->size().height() + child->marginBottom());
        }

        yPos = size().height();

        if (!iterator.first() && hasLineIfEmpty())
            setHeight(size().height() + lineHeight(true, style()->isHorizontalWritingMode() ? HorizontalLine : VerticalLine, PositionOfInteriorLineBoxes));

        setHeight(size().height() + toAdd);

        // Negative margins can cause our height to shrink below our minimal height (border/padding).
        // If this happens, ensure that the computed height is increased to the minimal height.
        if (size().height() < minHeight)
            setHeight(minHeight);

        // Now we have to calc our height, so we know how much space we have remaining.
        oldHeight = size().height();
        updateLogicalHeight();
        if (oldHeight != size().height())
            heightSpecified = true;

        remainingSpace = size().height() - borderBottom() - paddingBottom() - horizontalScrollbarHeight() - yPos;

        if (flexingChildren) {
            haveFlex = false; // We're done.
        } else if (haveFlex) {
            // We have some flexible objects.  See if we need to grow/shrink them at all.
            if (!remainingSpace)
                break;

            // Allocate the remaining space among the flexible objects.  If we are trying to
            // grow, then we go from the lowest flex group to the highest flex group.  For shrinking,
            // we go from the highest flex group to the lowest group.
            bool expanding = remainingSpace > 0;
            unsigned start = expanding ? lowestFlexGroup : highestFlexGroup;
            unsigned end = expanding? highestFlexGroup : lowestFlexGroup;
            for (unsigned i = start; i <= end && remainingSpace; i++) {
                // Always start off by assuming the group can get all the remaining space.
                LayoutUnit groupRemainingSpace = remainingSpace;
                do {
                    // Flexing consists of multiple passes, since we have to change ratios every time an object hits its max/min-width
                    // For a given pass, we always start off by computing the totalFlex of all objects that can grow/shrink at all, and
                    // computing the allowed growth before an object hits its min/max width (and thus
                    // forces a totalFlex recomputation).
                    LayoutUnit groupRemainingSpaceAtBeginning = groupRemainingSpace;
                    float totalFlex = 0.0f;
                    for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                        if (allowedChildFlex(child, expanding, i))
                            totalFlex += child->style()->boxFlex();
                    }
                    LayoutUnit spaceAvailableThisPass = groupRemainingSpace;
                    for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                        LayoutUnit allowedFlex = allowedChildFlex(child, expanding, i);
                        if (allowedFlex) {
                            LayoutUnit projectedFlex = (allowedFlex == LayoutUnit::max()) ? allowedFlex : static_cast<LayoutUnit>(allowedFlex * (totalFlex / child->style()->boxFlex()));
                            spaceAvailableThisPass = expanding ? std::min(spaceAvailableThisPass, projectedFlex) : std::max(spaceAvailableThisPass, projectedFlex);
                        }
                    }

                    // The flex groups may not have any flexible objects this time around.
                    if (!spaceAvailableThisPass || totalFlex == 0.0f) {
                        // If we just couldn't grow/shrink any more, then it's time to transition to the next flex group.
                        groupRemainingSpace = 0;
                        continue;
                    }

                    // Now distribute the space to objects.
                    for (LayoutBox* child = iterator.first(); child && spaceAvailableThisPass && totalFlex; child = iterator.next()) {
                        if (allowedChildFlex(child, expanding, i)) {
                            LayoutUnit spaceAdd = static_cast<LayoutUnit>(spaceAvailableThisPass * (child->style()->boxFlex() / totalFlex));
                            if (spaceAdd) {
                                child->setOverrideLogicalContentHeight(contentHeightForChild(child) + spaceAdd);
                                flexingChildren = true;
                                relayoutChildren = true;
                            }

                            spaceAvailableThisPass -= spaceAdd;
                            remainingSpace -= spaceAdd;
                            groupRemainingSpace -= spaceAdd;

                            totalFlex -= child->style()->boxFlex();
                        }
                    }
                    if (groupRemainingSpace == groupRemainingSpaceAtBeginning) {
                        // This is not advancing, avoid getting stuck by distributing the remaining pixels.
                        LayoutUnit spaceAdd = groupRemainingSpace > 0 ? 1 : -1;
                        for (LayoutBox* child = iterator.first(); child && groupRemainingSpace; child = iterator.next()) {
                            if (allowedChildFlex(child, expanding, i)) {
                                child->setOverrideLogicalContentHeight(contentHeightForChild(child) + spaceAdd);
                                flexingChildren = true;
                                relayoutChildren = true;
                                remainingSpace -= spaceAdd;
                                groupRemainingSpace -= spaceAdd;
                            }
                        }
                    }
                } while (absoluteValue(groupRemainingSpace) >= 1);
            }

            // We didn't find any children that could grow.
            if (haveFlex && !flexingChildren)
                haveFlex = false;
        }
    } while (haveFlex);

    LayoutBlock::finishDelayUpdateScrollInfo();

    if (style()->boxPack() != Start && remainingSpace > 0) {
        // Children must be repositioned.
        LayoutUnit offset = 0;
        if (style()->boxPack() == Justify) {
            // Determine the total number of children.
            int totalChildren = 0;
            for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                if (childDoesNotAffectWidthOrFlexing(child))
                    continue;

                ++totalChildren;
            }

            // Iterate over the children and space them out according to the
            // justification level.
            if (totalChildren > 1) {
                --totalChildren;
                bool firstChild = true;
                for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                    if (childDoesNotAffectWidthOrFlexing(child))
                        continue;

                    if (firstChild) {
                        firstChild = false;
                        continue;
                    }

                    offset += remainingSpace / totalChildren;
                    remainingSpace -= (remainingSpace / totalChildren);
                    --totalChildren;
                    placeChild(child, child->location() + LayoutSize(0, offset));
                }
            }
        } else {
            if (style()->boxPack() == Center)
                offset += remainingSpace / 2;
            else // END
                offset += remainingSpace;
            for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
                if (childDoesNotAffectWidthOrFlexing(child))
                    continue;
                placeChild(child, child->location() + LayoutSize(0, offset));
            }
        }
    }

    // So that the computeLogicalHeight in layoutBlock() knows to relayout positioned objects because of
    // a height change, we revert our height back to the intrinsic height before returning.
    if (heightSpecified)
        setHeight(oldHeight);
}

void LayoutDeprecatedFlexibleBox::applyLineClamp(FlexBoxIterator& iterator, bool relayoutChildren)
{
    UseCounter::count(document(), UseCounter::LineClamp);

    int maxLineCount = 0;
    for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
        if (childDoesNotAffectWidthOrFlexing(child))
            continue;

        child->clearOverrideSize();
        if (relayoutChildren || (child->isReplaced() && (child->style()->width().hasPercent() || child->style()->height().hasPercent()))
            || (child->style()->height().isAuto() && child->isLayoutBlock())) {
            child->setChildNeedsLayout(MarkOnlyThis);

            // Dirty all the positioned objects.
            if (child->isLayoutBlock()) {
                toLayoutBlock(child)->markPositionedObjectsForLayout();
                toLayoutBlock(child)->clearTruncation();
            }
        }
        child->layoutIfNeeded();
        if (child->style()->height().isAuto() && child->isLayoutBlock())
            maxLineCount = std::max(maxLineCount, toLayoutBlock(child)->lineCount());
    }

    // Get the number of lines and then alter all block flow children with auto height to use the
    // specified height. We always try to leave room for at least one line.
    LineClampValue lineClamp = style()->lineClamp();
    int numVisibleLines = lineClamp.isPercentage() ? std::max(1, (maxLineCount + 1) * lineClamp.value() / 100) : lineClamp.value();
    if (numVisibleLines >= maxLineCount)
        return;

    for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
        if (childDoesNotAffectWidthOrFlexing(child) || !child->style()->height().isAuto() || !child->isLayoutBlock())
            continue;

        LayoutBlock* blockChild = toLayoutBlock(child);
        int lineCount = blockChild->lineCount();
        if (lineCount <= numVisibleLines)
            continue;

        LayoutUnit newHeight = blockChild->heightForLineCount(numVisibleLines);
        if (newHeight == child->size().height())
            continue;

        child->setOverrideLogicalContentHeight(newHeight - child->borderAndPaddingHeight());
        child->forceChildLayout();

        // FIXME: For now don't support RTL.
        if (style()->direction() != LTR)
            continue;

        // Get the last line
        RootInlineBox* lastLine = blockChild->lineAtIndex(lineCount - 1);
        if (!lastLine)
            continue;

        RootInlineBox* lastVisibleLine = blockChild->lineAtIndex(numVisibleLines - 1);
        if (!lastVisibleLine)
            continue;

        DEFINE_STATIC_LOCAL(AtomicString, ellipsisStr, (&horizontalEllipsisCharacter, 1));
        const Font& font = style(numVisibleLines == 1)->font();
        float totalWidth = font.width(constructTextRun(this, font, &horizontalEllipsisCharacter, 1, styleRef(), style()->direction()));

        // See if this width can be accommodated on the last visible line
        LayoutBlockFlow& destBlock = lastVisibleLine->block();
        LayoutBlockFlow& srcBlock = lastLine->block();

        // FIXME: Directions of src/destBlock could be different from our direction and from one another.
        if (!srcBlock.style()->isLeftToRightDirection())
            continue;

        bool leftToRight = destBlock.style()->isLeftToRightDirection();
        if (!leftToRight)
            continue;

        LayoutUnit blockRightEdge = destBlock.logicalRightOffsetForLine(lastVisibleLine->y(), false);
        if (!lastVisibleLine->lineCanAccommodateEllipsis(leftToRight, blockRightEdge, lastVisibleLine->x() + lastVisibleLine->logicalWidth(), totalWidth))
            continue;

        // Let the truncation code kick in.
        // FIXME: the text alignment should be recomputed after the width changes due to truncation.
        LayoutUnit blockLeftEdge = destBlock.logicalLeftOffsetForLine(lastVisibleLine->y(), false);
        lastVisibleLine->placeEllipsis(ellipsisStr, leftToRight, blockLeftEdge, blockRightEdge, totalWidth);
        destBlock.setHasMarkupTruncation(true);
    }
}

void LayoutDeprecatedFlexibleBox::clearLineClamp()
{
    FlexBoxIterator iterator(this);
    for (LayoutBox* child = iterator.first(); child; child = iterator.next()) {
        if (childDoesNotAffectWidthOrFlexing(child))
            continue;

        child->clearOverrideSize();
        if ((child->isReplaced() && (child->style()->width().hasPercent() || child->style()->height().hasPercent()))
            || (child->style()->height().isAuto() && child->isLayoutBlock())) {
            child->setChildNeedsLayout();

            if (child->isLayoutBlock()) {
                toLayoutBlock(child)->markPositionedObjectsForLayout();
                toLayoutBlock(child)->clearTruncation();
            }
        }
    }
}

void LayoutDeprecatedFlexibleBox::placeChild(LayoutBox* child, const LayoutPoint& location)
{
    // FIXME Investigate if this can be removed based on other flags. crbug.com/370010
    child->setMayNeedPaintInvalidation();

    // Place the child.
    child->setLocation(location);
}

LayoutUnit LayoutDeprecatedFlexibleBox::allowedChildFlex(LayoutBox* child, bool expanding, unsigned group)
{
    if (childDoesNotAffectWidthOrFlexing(child) || child->style()->boxFlex() == 0.0f || child->style()->boxFlexGroup() != group)
        return 0;

    if (expanding) {
        if (isHorizontal()) {
            // FIXME: For now just handle fixed values.
            LayoutUnit maxWidth = LayoutUnit::max();
            LayoutUnit width = contentWidthForChild(child);
            if (child->style()->maxWidth().isFixed())
                maxWidth = child->style()->maxWidth().value();
            else if (child->style()->maxWidth().type() == Intrinsic)
                maxWidth = child->maxPreferredLogicalWidth();
            else if (child->style()->maxWidth().type() == MinIntrinsic)
                maxWidth = child->minPreferredLogicalWidth();
            if (maxWidth == LayoutUnit::max())
                return maxWidth;
            return std::max<LayoutUnit>(0, maxWidth - width);
        }
        // FIXME: For now just handle fixed values.
        LayoutUnit maxHeight = LayoutUnit::max();
        LayoutUnit height = contentHeightForChild(child);
        if (child->style()->maxHeight().isFixed())
            maxHeight = child->style()->maxHeight().value();
        if (maxHeight == LayoutUnit::max())
            return maxHeight;
        return std::max<LayoutUnit>(0, maxHeight - height);
    }

    // FIXME: For now just handle fixed values.
    if (isHorizontal()) {
        LayoutUnit minWidth = child->minPreferredLogicalWidth();
        LayoutUnit width = contentWidthForChild(child);
        if (child->style()->minWidth().isFixed())
            minWidth = child->style()->minWidth().value();
        else if (child->style()->minWidth().type() == Intrinsic)
            minWidth = child->maxPreferredLogicalWidth();
        else if (child->style()->minWidth().type() == MinIntrinsic)
            minWidth = child->minPreferredLogicalWidth();
        else if (child->style()->minWidth().type() == Auto)
            minWidth = 0;

        LayoutUnit allowedShrinkage = std::min<LayoutUnit>(0, minWidth - width);
        return allowedShrinkage;
    }
    Length minHeight = child->style()->minHeight();
    if (minHeight.isFixed() || minHeight.isAuto()) {
        LayoutUnit minHeight = child->style()->minHeight().value();
        LayoutUnit height = contentHeightForChild(child);
        LayoutUnit allowedShrinkage = std::min<LayoutUnit>(0, minHeight - height);
        return allowedShrinkage;
    }
    return 0;
}

} // namespace blink
