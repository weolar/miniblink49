/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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

#include "config.h"
#include "core/layout/line/InlineFlowBox.h"

#include "core/CSSPropertyNames.h"
#include "core/dom/Document.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutObjectInlines.h"
#include "core/layout/LayoutRubyBase.h"
#include "core/layout/LayoutRubyRun.h"
#include "core/layout/LayoutRubyText.h"
#include "core/layout/LayoutView.h"
#include "core/layout/line/GlyphOverflow.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/layout/line/RootInlineBox.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/InlineFlowBoxPainter.h"
#include "core/style/ShadowList.h"
#include "platform/fonts/Font.h"

#include <math.h>

namespace blink {

struct SameSizeAsInlineFlowBox : public InlineBox {
    void* pointers[5];
    uint32_t bitfields : 23;
};

static_assert(sizeof(InlineFlowBox) == sizeof(SameSizeAsInlineFlowBox), "InlineFlowBox should stay small");

#if ENABLE(ASSERT)

InlineFlowBox::~InlineFlowBox()
{
    if (!m_hasBadChildList)
        for (InlineBox* child = firstChild(); child; child = child->nextOnLine())
            child->setHasBadParent();
}

#endif

LayoutUnit InlineFlowBox::getFlowSpacingLogicalWidth()
{
    LayoutUnit totWidth = marginBorderPaddingLogicalLeft() + marginBorderPaddingLogicalRight();
    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        if (curr->isInlineFlowBox())
            totWidth += toInlineFlowBox(curr)->getFlowSpacingLogicalWidth();
    }
    return totWidth;
}

IntRect InlineFlowBox::roundedFrameRect() const
{
    // Begin by snapping the x and y coordinates to the nearest pixel.
    int snappedX = x().round();
    int snappedY = y().round();

    int snappedMaxX = (x() + width()).round();
    int snappedMaxY = (y() + height()).round();

    return IntRect(snappedX, snappedY, snappedMaxX - snappedX, snappedMaxY - snappedY);
}

static void setHasTextDescendantsOnAncestors(InlineFlowBox* box)
{
    while (box && !box->hasTextDescendants()) {
        box->setHasTextDescendants();
        box = box->parent();
    }
}

void InlineFlowBox::addToLine(InlineBox* child)
{
    ASSERT(!child->parent());
    ASSERT(!child->nextOnLine());
    ASSERT(!child->prevOnLine());
    checkConsistency();

    child->setParent(this);
    if (!m_firstChild) {
        m_firstChild = child;
        m_lastChild = child;
    } else {
        m_lastChild->setNextOnLine(child);
        child->setPrevOnLine(m_lastChild);
        m_lastChild = child;
    }
    child->setFirstLineStyleBit(isFirstLineStyle());
    child->setIsHorizontal(isHorizontal());
    if (child->isText()) {
        if (child->layoutObject().parent() == layoutObject())
            m_hasTextChildren = true;
        setHasTextDescendantsOnAncestors(this);
    } else if (child->isInlineFlowBox()) {
        if (toInlineFlowBox(child)->hasTextDescendants())
            setHasTextDescendantsOnAncestors(this);
    }

    if (descendantsHaveSameLineHeightAndBaseline() && !child->layoutObject().isOutOfFlowPositioned()) {
        const ComputedStyle& parentStyle = layoutObject().styleRef(isFirstLineStyle());
        const ComputedStyle& childStyle = child->layoutObject().styleRef(isFirstLineStyle());
        bool shouldClearDescendantsHaveSameLineHeightAndBaseline = false;
        if (child->layoutObject().isReplaced()) {
            shouldClearDescendantsHaveSameLineHeightAndBaseline = true;
        } else if (child->isText()) {
            if (child->layoutObject().isBR() || child->layoutObject().parent() != layoutObject()) {
                if (!parentStyle.font().fontMetrics().hasIdenticalAscentDescentAndLineGap(childStyle.font().fontMetrics())
                    || parentStyle.lineHeight() != childStyle.lineHeight()
                    || (parentStyle.verticalAlign() != BASELINE && !isRootInlineBox()) || childStyle.verticalAlign() != BASELINE)
                    shouldClearDescendantsHaveSameLineHeightAndBaseline = true;
            }
            if (childStyle.hasTextCombine() || childStyle.textEmphasisMark() != TextEmphasisMarkNone)
                shouldClearDescendantsHaveSameLineHeightAndBaseline = true;
        } else {
            if (child->layoutObject().isBR()) {
                // FIXME: This is dumb. We only turn off because current layout test results expect the <br> to be 0-height on the baseline.
                // Other than making a zillion tests have to regenerate results, there's no reason to ditch the optimization here.
                shouldClearDescendantsHaveSameLineHeightAndBaseline = true;
            } else {
                ASSERT(isInlineFlowBox());
                InlineFlowBox* childFlowBox = toInlineFlowBox(child);
                // Check the child's bit, and then also check for differences in font, line-height, vertical-align
                if (!childFlowBox->descendantsHaveSameLineHeightAndBaseline()
                    || !parentStyle.font().fontMetrics().hasIdenticalAscentDescentAndLineGap(childStyle.font().fontMetrics())
                    || parentStyle.lineHeight() != childStyle.lineHeight()
                    || (parentStyle.verticalAlign() != BASELINE && !isRootInlineBox()) || childStyle.verticalAlign() != BASELINE
                    || childStyle.hasBorder() || childStyle.hasPadding() || childStyle.hasTextCombine())
                    shouldClearDescendantsHaveSameLineHeightAndBaseline = true;
            }
        }

        if (shouldClearDescendantsHaveSameLineHeightAndBaseline)
            clearDescendantsHaveSameLineHeightAndBaseline();
    }

    if (!child->layoutObject().isOutOfFlowPositioned()) {
        if (child->isText()) {
            const ComputedStyle& childStyle = child->layoutObject().styleRef(isFirstLineStyle());
            if (childStyle.letterSpacing() < 0 || childStyle.textShadow() || childStyle.textEmphasisMark() != TextEmphasisMarkNone || childStyle.textStrokeWidth())
                child->clearKnownToHaveNoOverflow();
        } else if (child->layoutObject().isReplaced()) {
            LayoutBox& box = toLayoutBox(child->layoutObject());
            if (box.hasOverflowModel() || box.hasSelfPaintingLayer())
                child->clearKnownToHaveNoOverflow();
        } else if (!child->layoutObject().isBR() && (child->layoutObject().style(isFirstLineStyle())->boxShadow() || child->boxModelObject()->hasSelfPaintingLayer()
            || (child->layoutObject().isListMarker() && !toLayoutListMarker(child->layoutObject()).isInside())
            || child->layoutObject().style(isFirstLineStyle())->hasBorderImageOutsets()
            || child->layoutObject().style(isFirstLineStyle())->hasOutline())) {
            child->clearKnownToHaveNoOverflow();
        }

        if (knownToHaveNoOverflow() && child->isInlineFlowBox() && !toInlineFlowBox(child)->knownToHaveNoOverflow())
            clearKnownToHaveNoOverflow();
    }

    checkConsistency();
}

void InlineFlowBox::removeChild(InlineBox* child, MarkLineBoxes markDirty)
{
    checkConsistency();

    if (markDirty == MarkLineBoxesDirty && !isDirty())
        dirtyLineBoxes();

    root().childRemoved(child);

    if (child == m_firstChild)
        m_firstChild = child->nextOnLine();
    if (child == m_lastChild)
        m_lastChild = child->prevOnLine();
    if (child->nextOnLine())
        child->nextOnLine()->setPrevOnLine(child->prevOnLine());
    if (child->prevOnLine())
        child->prevOnLine()->setNextOnLine(child->nextOnLine());

    child->setParent(nullptr);

    checkConsistency();
}

void InlineFlowBox::deleteLine()
{
    InlineBox* child = firstChild();
    InlineBox* next = nullptr;
    while (child) {
        ASSERT(this == child->parent());
        next = child->nextOnLine();
#if ENABLE(ASSERT)
        child->setParent(nullptr);
#endif
        child->deleteLine();
        child = next;
    }
#if ENABLE(ASSERT)
    m_firstChild = nullptr;
    m_lastChild = nullptr;
#endif

    removeLineBoxFromLayoutObject();
    destroy();
}

void InlineFlowBox::removeLineBoxFromLayoutObject()
{
    lineBoxes()->removeLineBox(this);
}

void InlineFlowBox::extractLine()
{
    if (!extracted())
        extractLineBoxFromLayoutObject();
    for (InlineBox* child = firstChild(); child; child = child->nextOnLine())
        child->extractLine();
}

void InlineFlowBox::extractLineBoxFromLayoutObject()
{
    lineBoxes()->extractLineBox(this);
}

void InlineFlowBox::attachLine()
{
    if (extracted())
        attachLineBoxToLayoutObject();
    for (InlineBox* child = firstChild(); child; child = child->nextOnLine())
        child->attachLine();
}

void InlineFlowBox::attachLineBoxToLayoutObject()
{
    lineBoxes()->attachLineBox(this);
}

void InlineFlowBox::move(const LayoutSize& delta)
{
    InlineBox::move(delta);
    for (InlineBox* child = firstChild(); child; child = child->nextOnLine()) {
        if (child->layoutObject().isOutOfFlowPositioned())
            continue;
        child->move(delta);
    }
    if (m_overflow)
        m_overflow->move(delta.width(), delta.height()); // FIXME: Rounding error here since overflow was pixel snapped, but nobody other than list markers passes non-integral values here.
}

LineBoxList* InlineFlowBox::lineBoxes() const
{
    return toLayoutInline(layoutObject()).lineBoxes();
}

static inline bool isLastChildForLayoutObject(LayoutObject* ancestor, LayoutObject* child)
{
    if (!child)
        return false;

    if (child == ancestor)
        return true;

    LayoutObject* curr = child;
    LayoutObject* parent = curr->parent();
    while (parent && (!parent->isLayoutBlock() || parent->isInline())) {
        if (parent->slowLastChild() != curr)
            return false;
        if (parent == ancestor)
            return true;

        curr = parent;
        parent = curr->parent();
    }

    return true;
}

static bool isAncestorAndWithinBlock(LayoutObject* ancestor, LayoutObject* child)
{
    LayoutObject* object = child;
    while (object && (!object->isLayoutBlock() || object->isInline())) {
        if (object == ancestor)
            return true;
        object = object->parent();
    }
    return false;
}

void InlineFlowBox::determineSpacingForFlowBoxes(bool lastLine, bool isLogicallyLastRunWrapped, LayoutObject* logicallyLastRunLayoutObject)
{
    // All boxes start off open.  They will not apply any margins/border/padding on
    // any side.
    bool includeLeftEdge = false;
    bool includeRightEdge = false;

    // The root inline box never has borders/margins/padding.
    if (parent()) {
        bool ltr = layoutObject().style()->isLeftToRightDirection();

        // Check to see if all initial lines are unconstructed.  If so, then
        // we know the inline began on this line (unless we are a continuation).
        LineBoxList* lineBoxList = lineBoxes();
        if (!lineBoxList->firstLineBox()->isConstructed() && !layoutObject().isInlineElementContinuation()) {
            if (layoutObject().style()->boxDecorationBreak() == DCLONE)
                includeLeftEdge = includeRightEdge = true;
            else if (ltr && lineBoxList->firstLineBox() == this)
                includeLeftEdge = true;
            else if (!ltr && lineBoxList->lastLineBox() == this)
                includeRightEdge = true;
        }

        if (!lineBoxList->lastLineBox()->isConstructed()) {
            LayoutInline& inlineFlow = toLayoutInline(layoutObject());
            bool isLastObjectOnLine = !isAncestorAndWithinBlock(&layoutObject(), logicallyLastRunLayoutObject) || (isLastChildForLayoutObject(&layoutObject(), logicallyLastRunLayoutObject) && !isLogicallyLastRunWrapped);

            // We include the border under these conditions:
            // (1) The next line was not created, or it is constructed. We check the previous line for rtl.
            // (2) The logicallyLastRun is not a descendant of this layout object.
            // (3) The logicallyLastRun is a descendant of this layout object, but it is the last child of this layout object and it does not wrap to the next line.
            // (4) The decoration break is set to clone therefore there will be borders on every sides.
            if (layoutObject().style()->boxDecorationBreak() == DCLONE) {
                includeLeftEdge = includeRightEdge = true;
            } else if (ltr) {
                if (!nextLineBox()
                    && ((lastLine || isLastObjectOnLine) && !inlineFlow.continuation()))
                    includeRightEdge = true;
            } else {
                if ((!prevLineBox() || prevLineBox()->isConstructed())
                    && ((lastLine || isLastObjectOnLine) && !inlineFlow.continuation()))
                    includeLeftEdge = true;
            }
        }
    }

    setEdges(includeLeftEdge, includeRightEdge);

    // Recur into our children.
    for (InlineBox* currChild = firstChild(); currChild; currChild = currChild->nextOnLine()) {
        if (currChild->isInlineFlowBox()) {
            InlineFlowBox* currFlow = toInlineFlowBox(currChild);
            currFlow->determineSpacingForFlowBoxes(lastLine, isLogicallyLastRunWrapped, logicallyLastRunLayoutObject);
        }
    }
}

LayoutUnit InlineFlowBox::placeBoxesInInlineDirection(LayoutUnit logicalLeft, bool& needsWordSpacing)
{
    // Set our x position.
    beginPlacingBoxRangesInInlineDirection(logicalLeft);

    LayoutUnit startLogicalLeft = logicalLeft;
    logicalLeft += borderLogicalLeft() + paddingLogicalLeft();

    LayoutUnit minLogicalLeft = startLogicalLeft;
    LayoutUnit maxLogicalRight = logicalLeft;

    placeBoxRangeInInlineDirection(firstChild(), 0, logicalLeft, minLogicalLeft, maxLogicalRight, needsWordSpacing);

    logicalLeft += borderLogicalRight() + paddingLogicalRight();
    endPlacingBoxRangesInInlineDirection(startLogicalLeft, logicalLeft, minLogicalLeft, maxLogicalRight);
    return logicalLeft;
}

LayoutUnit InlineFlowBox::placeBoxRangeInInlineDirection(InlineBox* firstChild, InlineBox* lastChild,
    LayoutUnit& logicalLeft, LayoutUnit& minLogicalLeft, LayoutUnit& maxLogicalRight, bool& needsWordSpacing)
{
    for (InlineBox* curr = firstChild; curr && curr != lastChild; curr = curr->nextOnLine()) {
        if (curr->layoutObject().isText()) {
            InlineTextBox* text = toInlineTextBox(curr);
            LayoutText& rt = text->layoutObject();
            LayoutUnit space;
            if (rt.textLength()) {
                if (needsWordSpacing && isSpaceOrNewline(rt.characterAt(text->start())))
                    space = rt.style(isFirstLineStyle())->font().fontDescription().wordSpacing();
                needsWordSpacing = !isSpaceOrNewline(rt.characterAt(text->end()));
            }
            if (isLeftToRightDirection()) {
                logicalLeft += space;
                text->setLogicalLeft(logicalLeft);
            } else {
                text->setLogicalLeft(logicalLeft);
                logicalLeft += space;
            }
            if (knownToHaveNoOverflow())
                minLogicalLeft = std::min(logicalLeft, minLogicalLeft);
            logicalLeft += text->logicalWidth();
            if (knownToHaveNoOverflow())
                maxLogicalRight = std::max(logicalLeft, maxLogicalRight);
        } else {
            if (curr->layoutObject().isOutOfFlowPositioned()) {
                if (curr->layoutObject().parent()->style()->isLeftToRightDirection()) {
                    curr->setLogicalLeft(logicalLeft);
                } else {
                    // Our offset that we cache needs to be from the edge of the right border box and
                    // not the left border box.  We have to subtract |x| from the width of the block
                    // (which can be obtained from the root line box).
                    curr->setLogicalLeft(root().block().logicalWidth() - logicalLeft);
                }
                continue; // The positioned object has no effect on the width.
            }
            if (curr->layoutObject().isLayoutInline()) {
                InlineFlowBox* flow = toInlineFlowBox(curr);
                logicalLeft += flow->marginLogicalLeft();
                if (knownToHaveNoOverflow())
                    minLogicalLeft = std::min(logicalLeft, minLogicalLeft);
                logicalLeft = flow->placeBoxesInInlineDirection(logicalLeft, needsWordSpacing);
                if (knownToHaveNoOverflow())
                    maxLogicalRight = std::max(logicalLeft, maxLogicalRight);
                logicalLeft += flow->marginLogicalRight();
            } else if (!curr->layoutObject().isListMarker() || toLayoutListMarker(curr->layoutObject()).isInside()) {
                // The box can have a different writing-mode than the overall line, so this is a bit complicated.
                // Just get all the physical margin and overflow values by hand based off |isHorizontal|.
                LayoutUnit logicalLeftMargin = isHorizontal() ? curr->boxModelObject()->marginLeft() : curr->boxModelObject()->marginTop();
                LayoutUnit logicalRightMargin = isHorizontal() ? curr->boxModelObject()->marginRight() : curr->boxModelObject()->marginBottom();

                logicalLeft += logicalLeftMargin;
                curr->setLogicalLeft(logicalLeft);
                if (knownToHaveNoOverflow())
                    minLogicalLeft = std::min(logicalLeft, minLogicalLeft);
                logicalLeft += curr->logicalWidth();
                if (knownToHaveNoOverflow())
                    maxLogicalRight = std::max(logicalLeft, maxLogicalRight);
                logicalLeft += logicalRightMargin;
                // If we encounter any space after this inline block then ensure it is treated as the space between two words.
                needsWordSpacing = true;
            }
        }
    }
    return logicalLeft;
}

FontBaseline InlineFlowBox::dominantBaseline() const
{
    // Use "central" (Ideographic) baseline if writing-mode is vertical-* and text-orientation is not sideways-*.
    // http://dev.w3.org/csswg/css-writing-modes-3/#text-baselines
    if (!isHorizontal() && layoutObject().style(isFirstLineStyle())->fontDescription().isVerticalAnyUpright())
        return IdeographicBaseline;
    return AlphabeticBaseline;
}

void InlineFlowBox::adjustMaxAscentAndDescent(int& maxAscent, int& maxDescent, int maxPositionTop, int maxPositionBottom)
{
    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        // The computed lineheight needs to be extended for the
        // positioned elements
        if (curr->layoutObject().isOutOfFlowPositioned())
            continue; // Positioned placeholders don't affect calculations.
        if (curr->verticalAlign() == TOP || curr->verticalAlign() == BOTTOM) {
            int lineHeight = curr->lineHeight();
            if (curr->verticalAlign() == TOP) {
                if (maxAscent + maxDescent < lineHeight)
                    maxDescent = lineHeight - maxAscent;
            } else {
                if (maxAscent + maxDescent < lineHeight)
                    maxAscent = lineHeight - maxDescent;
            }

            if (maxAscent + maxDescent >= std::max(maxPositionTop, maxPositionBottom))
                break;
        }

        if (curr->isInlineFlowBox())
            toInlineFlowBox(curr)->adjustMaxAscentAndDescent(maxAscent, maxDescent, maxPositionTop, maxPositionBottom);
    }
}

void InlineFlowBox::computeLogicalBoxHeights(RootInlineBox* rootBox, LayoutUnit& maxPositionTop, LayoutUnit& maxPositionBottom, int& maxAscent, int& maxDescent, bool& setMaxAscent, bool& setMaxDescent, bool strictMode, GlyphOverflowAndFallbackFontsMap& textBoxDataMap, FontBaseline baselineType, VerticalPositionCache& verticalPositionCache)
{
    // The primary purpose of this function is to compute the maximal ascent and descent values for
    // a line. These values are computed based off the block's line-box-contain property, which indicates
    // what parts of descendant boxes have to fit within the line.
    //
    // The maxAscent value represents the distance of the highest point of any box (typically including line-height) from
    // the root box's baseline. The maxDescent value represents the distance of the lowest point of any box
    // (also typically including line-height) from the root box baseline. These values can be negative.
    //
    // A secondary purpose of this function is to store the offset of every box's baseline from the root box's
    // baseline. This information is cached in the logicalTop() of every box. We're effectively just using
    // the logicalTop() as scratch space.
    //
    // Because a box can be positioned such that it ends up fully above or fully below the
    // root line box, we only consider it to affect the maxAscent and maxDescent values if some
    // part of the box (EXCLUDING leading) is above (for ascent) or below (for descent) the root box's baseline.
    bool affectsAscent = false;
    bool affectsDescent = false;
    bool checkChildren = !descendantsHaveSameLineHeightAndBaseline();

    if (isRootInlineBox()) {
        // Examine our root box.
        int ascent = 0;
        int descent = 0;
        rootBox->ascentAndDescentForBox(rootBox, textBoxDataMap, ascent, descent, affectsAscent, affectsDescent);
        if (strictMode || hasTextChildren() || (!checkChildren && hasTextDescendants())) {
            if (maxAscent < ascent || !setMaxAscent) {
                maxAscent = ascent;
                setMaxAscent = true;
            }
            if (maxDescent < descent || !setMaxDescent) {
                maxDescent = descent;
                setMaxDescent = true;
            }
        }
    }

    if (!checkChildren)
        return;

    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        if (curr->layoutObject().isOutOfFlowPositioned())
            continue; // Positioned placeholders don't affect calculations.

        InlineFlowBox* inlineFlowBox = curr->isInlineFlowBox() ? toInlineFlowBox(curr) : nullptr;

        bool affectsAscent = false;
        bool affectsDescent = false;

        // The verticalPositionForBox function returns the distance between the child box's baseline
        // and the root box's baseline.  The value is negative if the child box's baseline is above the
        // root box's baseline, and it is positive if the child box's baseline is below the root box's baseline.
        curr->setLogicalTop(rootBox->verticalPositionForBox(curr, verticalPositionCache));

        int ascent = 0;
        int descent = 0;
        rootBox->ascentAndDescentForBox(curr, textBoxDataMap, ascent, descent, affectsAscent, affectsDescent);

        LayoutUnit boxHeight = ascent + descent;
        if (curr->verticalAlign() == TOP) {
            if (maxPositionTop < boxHeight)
                maxPositionTop = boxHeight;
        } else if (curr->verticalAlign() == BOTTOM) {
            if (maxPositionBottom < boxHeight)
                maxPositionBottom = boxHeight;
        } else if (!inlineFlowBox || strictMode || inlineFlowBox->hasTextChildren() || (inlineFlowBox->descendantsHaveSameLineHeightAndBaseline() && inlineFlowBox->hasTextDescendants()) || inlineFlowBox->boxModelObject()->hasInlineDirectionBordersOrPadding()) {
            // Note that these values can be negative.  Even though we only affect the maxAscent and maxDescent values
            // if our box (excluding line-height) was above (for ascent) or below (for descent) the root baseline, once you factor in line-height
            // the final box can end up being fully above or fully below the root box's baseline!  This is ok, but what it
            // means is that ascent and descent (including leading), can end up being negative.  The setMaxAscent and
            // setMaxDescent booleans are used to ensure that we're willing to initially set maxAscent/Descent to negative
            // values.
            ascent -= curr->logicalTop().round();
            descent += curr->logicalTop().round();
            if (affectsAscent && (maxAscent < ascent || !setMaxAscent)) {
                maxAscent = ascent;
                setMaxAscent = true;
            }

            if (affectsDescent && (maxDescent < descent || !setMaxDescent)) {
                maxDescent = descent;
                setMaxDescent = true;
            }
        }

        if (inlineFlowBox)
            inlineFlowBox->computeLogicalBoxHeights(rootBox, maxPositionTop, maxPositionBottom, maxAscent, maxDescent, setMaxAscent, setMaxDescent, strictMode, textBoxDataMap, baselineType, verticalPositionCache);
    }
}

void InlineFlowBox::placeBoxesInBlockDirection(LayoutUnit top, LayoutUnit maxHeight, int maxAscent, bool strictMode, LayoutUnit& lineTop, LayoutUnit& lineBottom, LayoutUnit& selectionBottom, bool& setLineTop, LayoutUnit& lineTopIncludingMargins, LayoutUnit& lineBottomIncludingMargins, bool& hasAnnotationsBefore, bool& hasAnnotationsAfter, FontBaseline baselineType)
{
    bool isRootBox = isRootInlineBox();
    if (isRootBox) {
        const FontMetrics& fontMetrics = layoutObject().style(isFirstLineStyle())->fontMetrics();
        // RootInlineBoxes are always placed at pixel boundaries in their logical y direction. Not doing
        // so results in incorrect layout of text decorations, most notably underlines.
        setLogicalTop(roundToInt(top + maxAscent - fontMetrics.ascent(baselineType)));
    }

    LayoutUnit adjustmentForChildrenWithSameLineHeightAndBaseline = 0;
    if (descendantsHaveSameLineHeightAndBaseline()) {
        adjustmentForChildrenWithSameLineHeightAndBaseline = logicalTop();
        if (parent())
            adjustmentForChildrenWithSameLineHeightAndBaseline += (boxModelObject()->borderBefore() + boxModelObject()->paddingBefore());
    }

    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        if (curr->layoutObject().isOutOfFlowPositioned())
            continue; // Positioned placeholders don't affect calculations.

        if (descendantsHaveSameLineHeightAndBaseline()) {
            curr->moveInBlockDirection(adjustmentForChildrenWithSameLineHeightAndBaseline);
            continue;
        }

        InlineFlowBox* inlineFlowBox = curr->isInlineFlowBox() ? toInlineFlowBox(curr) : nullptr;
        bool childAffectsTopBottomPos = true;
        if (curr->verticalAlign() == TOP) {
            curr->setLogicalTop(top);
        } else if (curr->verticalAlign() == BOTTOM) {
            curr->setLogicalTop((top + maxHeight - curr->lineHeight()));
        } else {
            if (!strictMode && inlineFlowBox && !inlineFlowBox->hasTextChildren() && !curr->boxModelObject()->hasInlineDirectionBordersOrPadding()
                && !(inlineFlowBox->descendantsHaveSameLineHeightAndBaseline() && inlineFlowBox->hasTextDescendants()))
                childAffectsTopBottomPos = false;
            LayoutUnit posAdjust = maxAscent - curr->baselinePosition(baselineType);
            curr->setLogicalTop(curr->logicalTop() + top + posAdjust);
        }

        LayoutUnit newLogicalTop = curr->logicalTop();
        LayoutUnit newLogicalTopIncludingMargins = newLogicalTop;
        LayoutUnit boxHeight = curr->logicalHeight();
        LayoutUnit boxHeightIncludingMargins = boxHeight;
        LayoutUnit borderPaddingHeight = 0;
        if (curr->isText() || curr->isInlineFlowBox()) {
            const FontMetrics& fontMetrics = curr->layoutObject().style(isFirstLineStyle())->fontMetrics();
            newLogicalTop += curr->baselinePosition(baselineType) - fontMetrics.ascent(baselineType);
            if (curr->isInlineFlowBox()) {
                LayoutBoxModelObject& boxObject = toLayoutBoxModelObject(curr->layoutObject());
                newLogicalTop -= boxObject.style(isFirstLineStyle())->isHorizontalWritingMode() ? boxObject.borderTop() + boxObject.paddingTop() :
                    boxObject.borderRight() + boxObject.paddingRight();
                borderPaddingHeight = boxObject.borderAndPaddingLogicalHeight();
            }
            newLogicalTopIncludingMargins = newLogicalTop;
        } else if (!curr->layoutObject().isBR()) {
            LayoutBox& box = toLayoutBox(curr->layoutObject());
            newLogicalTopIncludingMargins = newLogicalTop;
            LayoutUnit overSideMargin = curr->isHorizontal() ? box.marginTop() : box.marginRight();
            LayoutUnit underSideMargin = curr->isHorizontal() ? box.marginBottom() : box.marginLeft();
            newLogicalTop += overSideMargin;
            boxHeightIncludingMargins += overSideMargin + underSideMargin;
        }

        curr->setLogicalTop(newLogicalTop);

        if (childAffectsTopBottomPos) {
            if (curr->layoutObject().isRubyRun()) {
                // Treat the leading on the first and last lines of ruby runs as not being part of the overall lineTop/lineBottom.
                // Really this is a workaround hack for the fact that ruby should have been done as line layout and not done using
                // inline-block.
                if (layoutObject().style()->isFlippedLinesWritingMode() == (curr->layoutObject().style()->rubyPosition() == RubyPositionAfter))
                    hasAnnotationsBefore = true;
                else
                    hasAnnotationsAfter = true;

                LayoutRubyRun& rubyRun = toLayoutRubyRun(curr->layoutObject());
                if (LayoutRubyBase* rubyBase = rubyRun.rubyBase()) {
                    LayoutUnit bottomRubyBaseLeading = (curr->logicalHeight() - rubyBase->logicalBottom()) + rubyBase->logicalHeight() - (rubyBase->lastRootBox() ? rubyBase->lastRootBox()->lineBottom() : LayoutUnit());
                    LayoutUnit topRubyBaseLeading = rubyBase->logicalTop() + (rubyBase->firstRootBox() ? rubyBase->firstRootBox()->lineTop() : LayoutUnit());
                    newLogicalTop += !layoutObject().style()->isFlippedLinesWritingMode() ? topRubyBaseLeading : bottomRubyBaseLeading;
                    boxHeight -= (topRubyBaseLeading + bottomRubyBaseLeading);
                }
            }
            if (curr->isInlineTextBox()) {
                TextEmphasisPosition emphasisMarkPosition;
                if (toInlineTextBox(curr)->getEmphasisMarkPosition(curr->layoutObject().styleRef(isFirstLineStyle()), emphasisMarkPosition)) {
                    bool emphasisMarkIsOver = emphasisMarkPosition == TextEmphasisPositionOver;
                    if (emphasisMarkIsOver != curr->layoutObject().style(isFirstLineStyle())->isFlippedLinesWritingMode())
                        hasAnnotationsBefore = true;
                    else
                        hasAnnotationsAfter = true;
                }
            }

            if (!setLineTop) {
                setLineTop = true;
                lineTop = newLogicalTop;
                lineTopIncludingMargins = std::min(lineTop, newLogicalTopIncludingMargins);
            } else {
                lineTop = std::min(lineTop, newLogicalTop);
                lineTopIncludingMargins = std::min(lineTop, std::min(lineTopIncludingMargins, newLogicalTopIncludingMargins));
            }
            selectionBottom = std::max(selectionBottom, newLogicalTop + boxHeight - borderPaddingHeight);
            lineBottom = std::max(lineBottom, newLogicalTop + boxHeight);
            lineBottomIncludingMargins = std::max(lineBottom, std::max(lineBottomIncludingMargins, newLogicalTopIncludingMargins + boxHeightIncludingMargins));
        }

        // Adjust boxes to use their real box y/height and not the logical height (as dictated by
        // line-height).
        if (inlineFlowBox)
            inlineFlowBox->placeBoxesInBlockDirection(top, maxHeight, maxAscent, strictMode, lineTop, lineBottom, selectionBottom, setLineTop, lineTopIncludingMargins, lineBottomIncludingMargins, hasAnnotationsBefore, hasAnnotationsAfter, baselineType);
    }

    if (isRootBox) {
        if (strictMode || hasTextChildren() || (descendantsHaveSameLineHeightAndBaseline() && hasTextDescendants())) {
            if (!setLineTop) {
                setLineTop = true;
                lineTop = pixelSnappedLogicalTop();
                lineTopIncludingMargins = lineTop;
            } else {
                lineTop = std::min<LayoutUnit>(lineTop, pixelSnappedLogicalTop());
                lineTopIncludingMargins = std::min(lineTop, lineTopIncludingMargins);
            }
            selectionBottom = std::max<LayoutUnit>(selectionBottom, pixelSnappedLogicalBottom());
            lineBottom = std::max<LayoutUnit>(lineBottom, pixelSnappedLogicalBottom());
            lineBottomIncludingMargins = std::max(lineBottom, lineBottomIncludingMargins);
        }

        if (layoutObject().style()->isFlippedLinesWritingMode())
            flipLinesInBlockDirection(lineTopIncludingMargins, lineBottomIncludingMargins);
    }
}

void InlineFlowBox::computeMaxLogicalTop(LayoutUnit& maxLogicalTop) const
{
    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        if (curr->layoutObject().isOutOfFlowPositioned())
            continue; // Positioned placeholders don't affect calculations.

        if (descendantsHaveSameLineHeightAndBaseline())
            continue;

        maxLogicalTop = std::max<LayoutUnit>(maxLogicalTop, curr->y());
        LayoutUnit localMaxLogicalTop;
        if (curr->isInlineFlowBox())
            toInlineFlowBox(curr)->computeMaxLogicalTop(localMaxLogicalTop);
        maxLogicalTop = std::max<LayoutUnit>(maxLogicalTop, localMaxLogicalTop);
    }
}

void InlineFlowBox::flipLinesInBlockDirection(LayoutUnit lineTop, LayoutUnit lineBottom)
{
    // Flip the box on the line such that the top is now relative to the lineBottom instead of the lineTop.
    setLogicalTop(lineBottom - (logicalTop() - lineTop) - logicalHeight());

    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        if (curr->layoutObject().isOutOfFlowPositioned())
            continue; // Positioned placeholders aren't affected here.

        if (curr->isInlineFlowBox())
            toInlineFlowBox(curr)->flipLinesInBlockDirection(lineTop, lineBottom);
        else
            curr->setLogicalTop(lineBottom - (curr->logicalTop() - lineTop) - curr->logicalHeight());
    }
}

inline void InlineFlowBox::addBoxShadowVisualOverflow(LayoutRect& logicalVisualOverflow)
{
    const ComputedStyle& style = layoutObject().styleRef(isFirstLineStyle());

    // box-shadow on the block element applies to the block and not to the lines,
    // unless it is modified by :first-line pseudo element.
    if (!parent() && (!isFirstLineStyle() || &style == layoutObject().style()))
        return;

    WritingMode writingMode = style.writingMode();
    ShadowList* boxShadow = style.boxShadow();
    if (!boxShadow)
        return;

    LayoutRectOutsets outsets(boxShadow->rectOutsetsIncludingOriginal());
    // Similar to how glyph overflow works, if our lines are flipped, then it's actually the opposite shadow that applies, since
    // the line is "upside down" in terms of block coordinates.
    LayoutRectOutsets logicalOutsets(outsets.logicalOutsetsWithFlippedLines(writingMode));

    LayoutRect shadowBounds(logicalFrameRect());
    shadowBounds.expand(logicalOutsets);
    logicalVisualOverflow.unite(shadowBounds);
}

inline void InlineFlowBox::addBorderOutsetVisualOverflow(LayoutRect& logicalVisualOverflow)
{
    const ComputedStyle& style = layoutObject().styleRef(isFirstLineStyle());

    // border-image-outset on the block element applies to the block and not to the lines,
    // unless it is modified by :first-line pseudo element.
    if (!parent() && (!isFirstLineStyle() || &style == layoutObject().style()))
        return;

    if (!style.hasBorderImageOutsets())
        return;

    // Similar to how glyph overflow works, if our lines are flipped, then it's actually the opposite border that applies, since
    // the line is "upside down" in terms of block coordinates. vertical-rl and horizontal-bt are the flipped line modes.
    LayoutRectOutsets logicalOutsets = style.borderImageOutsets().logicalOutsetsWithFlippedLines(style.writingMode());

    if (!includeLogicalLeftEdge())
        logicalOutsets.setLeft(LayoutUnit());
    if (!includeLogicalRightEdge())
        logicalOutsets.setRight(LayoutUnit());

    LayoutRect borderOutsetBounds(logicalFrameRect());
    borderOutsetBounds.expand(logicalOutsets);
    logicalVisualOverflow.unite(borderOutsetBounds);
}

inline void InlineFlowBox::addOutlineVisualOverflow(LayoutRect& logicalVisualOverflow)
{
    // Outline on root line boxes is applied to the block and not to the lines.
    if (!parent())
        return;

    const ComputedStyle& style = layoutObject().styleRef(isFirstLineStyle());
    if (!style.hasOutline())
        return;

    logicalVisualOverflow.inflate(style.outlineSize());
}

inline void InlineFlowBox::addTextBoxVisualOverflow(InlineTextBox* textBox, GlyphOverflowAndFallbackFontsMap& textBoxDataMap, LayoutRect& logicalVisualOverflow)
{
    if (textBox->knownToHaveNoOverflow())
        return;

    const ComputedStyle& style = textBox->layoutObject().styleRef(isFirstLineStyle());

    GlyphOverflowAndFallbackFontsMap::iterator it = textBoxDataMap.find(textBox);
    GlyphOverflow* glyphOverflow = it == textBoxDataMap.end() ? nullptr : &it->value.second;
    bool isFlippedLine = style.isFlippedLinesWritingMode();

    int topGlyphEdge = glyphOverflow ? (isFlippedLine ? glyphOverflow->bottom : glyphOverflow->top) : 0;
    int bottomGlyphEdge = glyphOverflow ? (isFlippedLine ? glyphOverflow->top : glyphOverflow->bottom) : 0;
    int leftGlyphEdge = glyphOverflow ? glyphOverflow->left : 0;
    int rightGlyphEdge = glyphOverflow ? glyphOverflow->right : 0;

    int strokeOverflow = static_cast<int>(ceilf(style.textStrokeWidth() / 2.0f));
    int topGlyphOverflow = -strokeOverflow - topGlyphEdge;
    int bottomGlyphOverflow = strokeOverflow + bottomGlyphEdge;
    int leftGlyphOverflow = -strokeOverflow - leftGlyphEdge;
    int rightGlyphOverflow = strokeOverflow + rightGlyphEdge;

    TextEmphasisPosition emphasisMarkPosition;
    if (style.textEmphasisMark() != TextEmphasisMarkNone && textBox->getEmphasisMarkPosition(style, emphasisMarkPosition)) {
        int emphasisMarkHeight = style.font().emphasisMarkHeight(style.textEmphasisMarkString());
        if ((emphasisMarkPosition == TextEmphasisPositionOver) == (!style.isFlippedLinesWritingMode()))
            topGlyphOverflow = std::min(topGlyphOverflow, -emphasisMarkHeight);
        else
            bottomGlyphOverflow = std::max(bottomGlyphOverflow, emphasisMarkHeight);
    }

    // If letter-spacing is negative, we should factor that into right layout overflow. Even in RTL, letter-spacing is
    // applied to the right, so this is not an issue with left overflow.
    rightGlyphOverflow -= std::min(0, (int)style.font().fontDescription().letterSpacing());

    LayoutRectOutsets textShadowLogicalOutsets;
    if (ShadowList* textShadow = style.textShadow())
        textShadowLogicalOutsets = LayoutRectOutsets(textShadow->rectOutsetsIncludingOriginal()).logicalOutsets(style.writingMode());

    // FIXME: This code currently uses negative values for expansion of the top
    // and left edges. This should be cleaned up.
    LayoutUnit textShadowLogicalTop = -textShadowLogicalOutsets.top();
    LayoutUnit textShadowLogicalBottom = textShadowLogicalOutsets.bottom();
    LayoutUnit textShadowLogicalLeft = -textShadowLogicalOutsets.left();
    LayoutUnit textShadowLogicalRight = textShadowLogicalOutsets.right();

    LayoutUnit childOverflowLogicalTop = std::min<LayoutUnit>(textShadowLogicalTop + topGlyphOverflow, topGlyphOverflow);
    LayoutUnit childOverflowLogicalBottom = std::max<LayoutUnit>(textShadowLogicalBottom + bottomGlyphOverflow, bottomGlyphOverflow);
    LayoutUnit childOverflowLogicalLeft = std::min<LayoutUnit>(textShadowLogicalLeft + leftGlyphOverflow, leftGlyphOverflow);
    LayoutUnit childOverflowLogicalRight = std::max<LayoutUnit>(textShadowLogicalRight + rightGlyphOverflow, rightGlyphOverflow);

    LayoutUnit logicalTopVisualOverflow = std::min(textBox->pixelSnappedLogicalTop() + childOverflowLogicalTop, logicalVisualOverflow.y());
    LayoutUnit logicalBottomVisualOverflow = std::max(textBox->pixelSnappedLogicalBottom() + childOverflowLogicalBottom, logicalVisualOverflow.maxY());
    LayoutUnit logicalLeftVisualOverflow = std::min(textBox->pixelSnappedLogicalLeft() + childOverflowLogicalLeft, logicalVisualOverflow.x());
    LayoutUnit logicalRightVisualOverflow = std::max(textBox->pixelSnappedLogicalRight() + childOverflowLogicalRight, logicalVisualOverflow.maxX());

    logicalVisualOverflow = LayoutRect(logicalLeftVisualOverflow, logicalTopVisualOverflow, logicalRightVisualOverflow - logicalLeftVisualOverflow, logicalBottomVisualOverflow - logicalTopVisualOverflow);

    textBox->setLogicalOverflowRect(logicalVisualOverflow);
}

inline void InlineFlowBox::addReplacedChildOverflow(const InlineBox* inlineBox, LayoutRect& logicalLayoutOverflow, LayoutRect& logicalVisualOverflow)
{
    LayoutBox& box = toLayoutBox(inlineBox->layoutObject());

    // Visual overflow only propagates if the box doesn't have a self-painting layer.  This rectangle does not include
    // transforms or relative positioning (since those objects always have self-painting layers), but it does need to be adjusted
    // for writing-mode differences.
    if (!box.hasSelfPaintingLayer()) {
        LayoutRect childLogicalVisualOverflow = box.logicalVisualOverflowRectForPropagation(layoutObject().styleRef());
        childLogicalVisualOverflow.move(inlineBox->logicalLeft(), inlineBox->logicalTop());
        logicalVisualOverflow.unite(childLogicalVisualOverflow);
    }

    // Layout overflow internal to the child box only propagates if the child box doesn't have overflow clip set.
    // Otherwise the child border box propagates as layout overflow.  This rectangle must include transforms and relative positioning
    // and be adjusted for writing-mode differences.
    LayoutRect childLogicalLayoutOverflow = box.logicalLayoutOverflowRectForPropagation(layoutObject().styleRef());
    childLogicalLayoutOverflow.move(inlineBox->logicalLeft(), inlineBox->logicalTop());
    logicalLayoutOverflow.unite(childLogicalLayoutOverflow);
}

void InlineFlowBox::computeOverflow(LayoutUnit lineTop, LayoutUnit lineBottom, GlyphOverflowAndFallbackFontsMap& textBoxDataMap)
{
    // If we know we have no overflow, we can just bail.
    if (knownToHaveNoOverflow()) {
        ASSERT(!m_overflow);
        return;
    }

    if (m_overflow)
        m_overflow.clear();

    // Visual overflow just includes overflow for stuff we need to issues paint invalidations for ourselves. Self-painting layers are ignored.
    // Layout overflow is used to determine scrolling extent, so it still includes child layers and also factors in
    // transforms, relative positioning, etc.
    LayoutRect logicalLayoutOverflow(logicalFrameRectIncludingLineHeight(lineTop, lineBottom));
    LayoutRect logicalVisualOverflow(logicalLayoutOverflow);

    addBoxShadowVisualOverflow(logicalVisualOverflow);
    addBorderOutsetVisualOverflow(logicalVisualOverflow);
    addOutlineVisualOverflow(logicalVisualOverflow);

    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        if (curr->layoutObject().isOutOfFlowPositioned())
            continue; // Positioned placeholders don't affect calculations.

        if (curr->layoutObject().isText()) {
            InlineTextBox* text = toInlineTextBox(curr);
            LayoutText& rt = text->layoutObject();
            if (rt.isBR())
                continue;
            LayoutRect textBoxOverflow(text->logicalFrameRect());
            addTextBoxVisualOverflow(text, textBoxDataMap, textBoxOverflow);
            logicalVisualOverflow.unite(textBoxOverflow);
        } else if (curr->layoutObject().isLayoutInline()) {
            InlineFlowBox* flow = toInlineFlowBox(curr);
            flow->computeOverflow(lineTop, lineBottom, textBoxDataMap);
            if (!flow->boxModelObject()->hasSelfPaintingLayer())
                logicalVisualOverflow.unite(flow->logicalVisualOverflowRect(lineTop, lineBottom));
            LayoutRect childLayoutOverflow = flow->logicalLayoutOverflowRect(lineTop, lineBottom);
            childLayoutOverflow.move(flow->boxModelObject()->relativePositionLogicalOffset());
            logicalLayoutOverflow.unite(childLayoutOverflow);
        } else {
            addReplacedChildOverflow(curr, logicalLayoutOverflow, logicalVisualOverflow);
        }
    }

    setOverflowFromLogicalRects(logicalLayoutOverflow, logicalVisualOverflow, lineTop, lineBottom);
}

void InlineFlowBox::setLayoutOverflow(const LayoutRect& rect, const LayoutRect& frameBox)
{
    if (frameBox.contains(rect) || rect.isEmpty())
        return;

    if (!m_overflow)
        m_overflow = adoptPtr(new OverflowModel(frameBox, frameBox));

    m_overflow->setLayoutOverflow(rect);
}

void InlineFlowBox::setVisualOverflow(const LayoutRect& rect, const LayoutRect& frameBox)
{
    if (frameBox.contains(rect) || rect.isEmpty())
        return;

    if (!m_overflow)
        m_overflow = adoptPtr(new OverflowModel(frameBox, frameBox));

    m_overflow->setVisualOverflow(rect);
}

void InlineFlowBox::setOverflowFromLogicalRects(const LayoutRect& logicalLayoutOverflow, const LayoutRect& logicalVisualOverflow, LayoutUnit lineTop, LayoutUnit lineBottom)
{
    LayoutRect frameBox = frameRectIncludingLineHeight(lineTop, lineBottom);

    LayoutRect layoutOverflow(isHorizontal() ? logicalLayoutOverflow : logicalLayoutOverflow.transposedRect());
    setLayoutOverflow(layoutOverflow, frameBox);

    LayoutRect visualOverflow(isHorizontal() ? logicalVisualOverflow : logicalVisualOverflow.transposedRect());
    setVisualOverflow(visualOverflow, frameBox);
}

bool InlineFlowBox::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, LayoutUnit lineTop, LayoutUnit lineBottom)
{
    LayoutRect overflowRect(visualOverflowRect(lineTop, lineBottom));
    flipForWritingMode(overflowRect);
    overflowRect.moveBy(accumulatedOffset);
    if (!locationInContainer.intersects(overflowRect))
        return false;

    // We need to hit test both our inline children (InlineBoxes) and culled inlines
    // (LayoutObjects). We check our inlines in the same order as line layout but
    // for each inline we additionally need to hit test its culled inline parents.
    // While hit testing culled inline parents, we can stop once we reach
    // a non-inline parent or a culled inline associated with a different inline box.
    InlineBox* prev;
    for (InlineBox* curr = lastChild(); curr; curr = prev) {
        prev = curr->prevOnLine();

        // Layers will handle hit testing themselves.
        if (curr->boxModelObject() && curr->boxModelObject()->hasSelfPaintingLayer())
            continue;

        if (curr->nodeAtPoint(result, locationInContainer, accumulatedOffset, lineTop, lineBottom)) {
            layoutObject().updateHitTestResult(result, locationInContainer.point() - toLayoutSize(accumulatedOffset));
            return true;
        }

        // If the current inlinebox's layout object and the previous inlinebox's layout object are same,
        // we should yield the hit-test to the previous inlinebox.
        if (prev && curr->layoutObject() == prev->layoutObject())
            continue;

        LayoutObject* culledParent = &curr->layoutObject();
        while (true) {
            LayoutObject* sibling = culledParent->style()->isLeftToRightDirection() ? culledParent->previousSibling() : culledParent->nextSibling();
            culledParent = culledParent->parent();
            ASSERT(culledParent);

            if (culledParent == layoutObject() || (sibling && prev && prev->layoutObject().isDescendantOf(culledParent)))
                break;

            if (culledParent->isLayoutInline() && toLayoutInline(culledParent)->hitTestCulledInline(result, locationInContainer, accumulatedOffset))
                return true;
        }
    }

    if (layoutObject().style()->hasBorderRadius()) {
        LayoutRect borderRect = logicalFrameRect();
        borderRect.moveBy(accumulatedOffset);
        FloatRoundedRect border = layoutObject().style()->getRoundedBorderFor(borderRect, includeLogicalLeftEdge(), includeLogicalRightEdge());
        if (!locationInContainer.intersects(border))
            return false;
    }

    // Now check ourselves. Pixel snap hit testing.
    LayoutRect frameRect(roundedFrameRect());
    LayoutUnit minX = frameRect.x();
    LayoutUnit minY = frameRect.y();
    LayoutUnit width = frameRect.width();
    LayoutUnit height = frameRect.height();

    // Constrain our hit testing to the line top and bottom if necessary.
    bool noQuirksMode = layoutObject().document().inNoQuirksMode();
    if (!noQuirksMode && !hasTextChildren() && !(descendantsHaveSameLineHeightAndBaseline() && hasTextDescendants())) {
        RootInlineBox& rootBox = root();
        LayoutUnit& top = isHorizontal() ? minY : minX;
        LayoutUnit& logicalHeight = isHorizontal() ? height : width;
        LayoutUnit bottom = std::min(rootBox.lineBottom(), top + logicalHeight);
        top = std::max(rootBox.lineTop(), top);
        logicalHeight = bottom - top;
    }

    // Move x/y to our coordinates.
    LayoutRect rect(minX, minY, width, height);
    flipForWritingMode(rect);
    rect.moveBy(accumulatedOffset);

    if (visibleToHitTestRequest(result.hitTestRequest()) && locationInContainer.intersects(rect)) {
        layoutObject().updateHitTestResult(result, flipForWritingMode(locationInContainer.point() - toLayoutSize(accumulatedOffset))); // Don't add in m_topLeft here, we want coords in the containing block's space.
        if (!result.addNodeToListBasedTestResult(layoutObject().node(), locationInContainer, rect))
            return true;
    }

    return false;
}

void InlineFlowBox::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, LayoutUnit lineTop, LayoutUnit lineBottom)
{
    InlineFlowBoxPainter(*this).paint(paintInfo, paintOffset, lineTop, lineBottom);
}

bool InlineFlowBox::boxShadowCanBeAppliedToBackground(const FillLayer& lastBackgroundLayer) const
{
    // The checks here match how paintFillLayer() decides whether to clip (if it does, the shadow
    // would be clipped out, so it has to be drawn separately).
    StyleImage* image = lastBackgroundLayer.image();
    bool hasFillImage = image && image->canRender(layoutObject(), layoutObject().style()->effectiveZoom());
    return (!hasFillImage && !layoutObject().style()->hasBorderRadius()) || (!prevLineBox() && !nextLineBox()) || !parent();
}

InlineBox* InlineFlowBox::firstLeafChild() const
{
    InlineBox* leaf = nullptr;
    for (InlineBox* child = firstChild(); child && !leaf; child = child->nextOnLine())
        leaf = child->isLeaf() ? child : toInlineFlowBox(child)->firstLeafChild();
    return leaf;
}

InlineBox* InlineFlowBox::lastLeafChild() const
{
    InlineBox* leaf = nullptr;
    for (InlineBox* child = lastChild(); child && !leaf; child = child->prevOnLine())
        leaf = child->isLeaf() ? child : toInlineFlowBox(child)->lastLeafChild();
    return leaf;
}

LayoutObject::SelectionState InlineFlowBox::selectionState() const
{
    return LayoutObject::SelectionNone;
}

bool InlineFlowBox::canAccommodateEllipsis(bool ltr, int blockEdge, int ellipsisWidth) const
{
    for (InlineBox* box = firstChild(); box; box = box->nextOnLine()) {
        if (!box->canAccommodateEllipsis(ltr, blockEdge, ellipsisWidth))
            return false;
    }
    return true;
}

LayoutUnit InlineFlowBox::placeEllipsisBox(bool ltr, LayoutUnit blockLeftEdge, LayoutUnit blockRightEdge, LayoutUnit ellipsisWidth, LayoutUnit &truncatedWidth, bool& foundBox)
{
    LayoutUnit result = -1;
    // We iterate over all children, the foundBox variable tells us when we've found the
    // box containing the ellipsis.  All boxes after that one in the flow are hidden.
    // If our flow is ltr then iterate over the boxes from left to right, otherwise iterate
    // from right to left. Varying the order allows us to correctly hide the boxes following the ellipsis.
    InlineBox* box = ltr ? firstChild() : lastChild();

    // NOTE: these will cross after foundBox = true.
    int visibleLeftEdge = blockLeftEdge;
    int visibleRightEdge = blockRightEdge;

    while (box) {
        int currResult = box->placeEllipsisBox(ltr, visibleLeftEdge, visibleRightEdge, ellipsisWidth, truncatedWidth, foundBox);
        if (currResult != -1 && result == -1)
            result = currResult;

        if (ltr) {
            visibleLeftEdge += box->logicalWidth().round();
            box = box->nextOnLine();
        } else {
            visibleRightEdge -= box->logicalWidth().round();
            box = box->prevOnLine();
        }
    }
    return result;
}

void InlineFlowBox::clearTruncation()
{
    for (InlineBox* box = firstChild(); box; box = box->nextOnLine())
        box->clearTruncation();
}

LayoutUnit InlineFlowBox::computeOverAnnotationAdjustment(LayoutUnit allowedPosition) const
{
    LayoutUnit result = 0;
    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        if (curr->layoutObject().isOutOfFlowPositioned())
            continue; // Positioned placeholders don't affect calculations.

        if (curr->isInlineFlowBox())
            result = std::max(result, toInlineFlowBox(curr)->computeOverAnnotationAdjustment(allowedPosition));

        if (curr->layoutObject().isReplaced() && curr->layoutObject().isRubyRun() && curr->layoutObject().style()->rubyPosition() == RubyPositionBefore) {
            LayoutRubyRun& rubyRun = toLayoutRubyRun(curr->layoutObject());
            LayoutRubyText* rubyText = rubyRun.rubyText();
            if (!rubyText)
                continue;

            if (!rubyRun.style()->isFlippedLinesWritingMode()) {
                LayoutUnit topOfFirstRubyTextLine = rubyText->logicalTop() + (rubyText->firstRootBox() ? rubyText->firstRootBox()->lineTop() : LayoutUnit());
                if (topOfFirstRubyTextLine >= 0)
                    continue;
                topOfFirstRubyTextLine += curr->logicalTop();
                result = std::max(result, allowedPosition - topOfFirstRubyTextLine);
            } else {
                LayoutUnit bottomOfLastRubyTextLine = rubyText->logicalTop() + (rubyText->lastRootBox() ? rubyText->lastRootBox()->lineBottom() : rubyText->logicalHeight());
                if (bottomOfLastRubyTextLine <= curr->logicalHeight())
                    continue;
                bottomOfLastRubyTextLine += curr->logicalTop();
                result = std::max(result, bottomOfLastRubyTextLine - allowedPosition);
            }
        }

        if (curr->isInlineTextBox()) {
            const ComputedStyle& style = curr->layoutObject().styleRef(isFirstLineStyle());
            TextEmphasisPosition emphasisMarkPosition;
            if (style.textEmphasisMark() != TextEmphasisMarkNone && toInlineTextBox(curr)->getEmphasisMarkPosition(style, emphasisMarkPosition) && emphasisMarkPosition == TextEmphasisPositionOver) {
                if (!style.isFlippedLinesWritingMode()) {
                    int topOfEmphasisMark = curr->logicalTop() - style.font().emphasisMarkHeight(style.textEmphasisMarkString());
                    result = std::max(result, allowedPosition - topOfEmphasisMark);
                } else {
                    int bottomOfEmphasisMark = curr->logicalBottom() + style.font().emphasisMarkHeight(style.textEmphasisMarkString());
                    result = std::max(result, bottomOfEmphasisMark - allowedPosition);
                }
            }
        }
    }
    return result;
}

LayoutUnit InlineFlowBox::computeUnderAnnotationAdjustment(LayoutUnit allowedPosition) const
{
    LayoutUnit result = 0;
    for (InlineBox* curr = firstChild(); curr; curr = curr->nextOnLine()) {
        if (curr->layoutObject().isOutOfFlowPositioned())
            continue; // Positioned placeholders don't affect calculations.

        if (curr->isInlineFlowBox())
            result = std::max(result, toInlineFlowBox(curr)->computeUnderAnnotationAdjustment(allowedPosition));

        if (curr->layoutObject().isReplaced() && curr->layoutObject().isRubyRun() && curr->layoutObject().style()->rubyPosition() == RubyPositionAfter) {
            LayoutRubyRun& rubyRun = toLayoutRubyRun(curr->layoutObject());
            LayoutRubyText* rubyText = rubyRun.rubyText();
            if (!rubyText)
                continue;

            if (rubyRun.style()->isFlippedLinesWritingMode()) {
                LayoutUnit topOfFirstRubyTextLine = rubyText->logicalTop() + (rubyText->firstRootBox() ? rubyText->firstRootBox()->lineTop() : LayoutUnit());
                if (topOfFirstRubyTextLine >= 0)
                    continue;
                topOfFirstRubyTextLine += curr->logicalTop();
                result = std::max(result, allowedPosition - topOfFirstRubyTextLine);
            } else {
                LayoutUnit bottomOfLastRubyTextLine = rubyText->logicalTop() + (rubyText->lastRootBox() ? rubyText->lastRootBox()->lineBottom() : rubyText->logicalHeight());
                if (bottomOfLastRubyTextLine <= curr->logicalHeight())
                    continue;
                bottomOfLastRubyTextLine += curr->logicalTop();
                result = std::max(result, bottomOfLastRubyTextLine - allowedPosition);
            }
        }

        if (curr->isInlineTextBox()) {
            const ComputedStyle& style = curr->layoutObject().styleRef(isFirstLineStyle());
            if (style.textEmphasisMark() != TextEmphasisMarkNone && style.textEmphasisPosition() == TextEmphasisPositionUnder) {
                if (!style.isFlippedLinesWritingMode()) {
                    LayoutUnit bottomOfEmphasisMark = curr->logicalBottom() + style.font().emphasisMarkHeight(style.textEmphasisMarkString());
                    result = std::max(result, bottomOfEmphasisMark - allowedPosition);
                } else {
                    LayoutUnit topOfEmphasisMark = curr->logicalTop() - style.font().emphasisMarkHeight(style.textEmphasisMarkString());
                    result = std::max(result, allowedPosition - topOfEmphasisMark);
                }
            }
        }
    }
    return result;
}

void InlineFlowBox::collectLeafBoxesInLogicalOrder(Vector<InlineBox*>& leafBoxesInLogicalOrder, CustomInlineBoxRangeReverse customReverseImplementation, void* userData) const
{
    InlineBox* leaf = firstLeafChild();

    // FIXME: The reordering code is a copy of parts from BidiResolver::createBidiRunsForLine, operating directly on InlineBoxes, instead of BidiRuns.
    // Investigate on how this code could possibly be shared.
    unsigned char minLevel = 128;
    unsigned char maxLevel = 0;

    // First find highest and lowest levels, and initialize leafBoxesInLogicalOrder with the leaf boxes in visual order.
    for (; leaf; leaf = leaf->nextLeafChild()) {
        minLevel = std::min(minLevel, leaf->bidiLevel());
        maxLevel = std::max(maxLevel, leaf->bidiLevel());
        leafBoxesInLogicalOrder.append(leaf);
    }

    if (layoutObject().style()->rtlOrdering() == VisualOrder)
        return;

    // Reverse of reordering of the line (L2 according to Bidi spec):
    // L2. From the highest level found in the text to the lowest odd level on each line,
    // reverse any contiguous sequence of characters that are at that level or higher.

    // Reversing the reordering of the line is only done up to the lowest odd level.
    if (!(minLevel % 2))
        ++minLevel;

    Vector<InlineBox*>::iterator end = leafBoxesInLogicalOrder.end();
    while (minLevel <= maxLevel) {
        Vector<InlineBox*>::iterator it = leafBoxesInLogicalOrder.begin();
        while (it != end) {
            while (it != end) {
                if ((*it)->bidiLevel() >= minLevel)
                    break;
                ++it;
            }
            Vector<InlineBox*>::iterator first = it;
            while (it != end) {
                if ((*it)->bidiLevel() < minLevel)
                    break;
                ++it;
            }
            Vector<InlineBox*>::iterator last = it;
            if (customReverseImplementation) {
                ASSERT(userData);
                (*customReverseImplementation)(userData, first, last);
            } else {
                std::reverse(first, last);
            }
        }
        ++minLevel;
    }
}

const char* InlineFlowBox::boxName() const
{
    return "InlineFlowBox";
}

#ifndef NDEBUG

void InlineFlowBox::showLineTreeAndMark(const InlineBox* markedBox1, const char* markedLabel1, const InlineBox* markedBox2, const char* markedLabel2, const LayoutObject* obj, int depth) const
{
    InlineBox::showLineTreeAndMark(markedBox1, markedLabel1, markedBox2, markedLabel2, obj, depth);
    for (const InlineBox* box = firstChild(); box; box = box->nextOnLine())
        box->showLineTreeAndMark(markedBox1, markedLabel1, markedBox2, markedLabel2, obj, depth + 1);
}

#endif

#if ENABLE(ASSERT)
void InlineFlowBox::checkConsistency() const
{
#ifdef CHECK_CONSISTENCY
    ASSERT(!m_hasBadChildList);
    const InlineBox* prev = nullptr;
    for (const InlineBox* child = m_firstChild; child; child = child->nextOnLine()) {
        ASSERT(child->parent() == this);
        ASSERT(child->prevOnLine() == prev);
        prev = child;
    }
    ASSERT(prev == m_lastChild);
#endif
}

#endif

} // namespace blink
