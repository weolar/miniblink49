/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All right reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#include "core/dom/AXObjectCache.h"
#include "core/layout/BidiRunForLine.h"
#include "core/layout/LayoutCounter.h"
#include "core/layout/LayoutFlowThread.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutRubyRun.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextRunConstructor.h"
#include "core/layout/VerticalPositionCache.h"
#include "core/layout/api/LineLayoutItem.h"
#include "core/layout/line/BreakingContextInlineHeaders.h"
#include "core/layout/line/GlyphOverflow.h"
#include "core/layout/line/LayoutTextInfo.h"
#include "core/layout/line/LineLayoutState.h"
#include "core/layout/line/LineWidth.h"
#include "core/layout/line/TrailingFloatsRootInlineBox.h"
#include "core/layout/line/WordMeasurement.h"
#include "core/layout/svg/line/SVGRootInlineBox.h"
#include "platform/fonts/Character.h"
#include "platform/text/BidiResolver.h"
#include "wtf/RefCountedLeakCounter.h"
#include "wtf/StdLibExtras.h"
#include "wtf/Vector.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

using namespace WTF::Unicode;

static inline InlineBox* createInlineBoxForLayoutObject(LayoutObject* obj, bool isRootLineBox, bool isOnlyRun = false)
{
    // Callers should handle text themselves.
    ASSERT(!obj->isText());

    if (isRootLineBox)
        return toLayoutBlockFlow(obj)->createAndAppendRootInlineBox();

    if (obj->isBox())
        return toLayoutBox(obj)->createInlineBox();

    return toLayoutInline(obj)->createAndAppendInlineFlowBox();
}

static inline InlineTextBox* createInlineBoxForText(BidiRun& run, bool isOnlyRun)
{
    ASSERT(run.m_object->isText());
    LayoutText* text = toLayoutText(run.m_object);
    InlineTextBox* textBox = text->createInlineTextBox(run.m_start, run.m_stop - run.m_start);
    // We only treat a box as text for a <br> if we are on a line by ourself or in strict mode
    // (Note the use of strict mode.  In "almost strict" mode, we don't treat the box for <br> as text.)
    if (text->isBR())
        textBox->setIsText(isOnlyRun || text->document().inNoQuirksMode());
    textBox->setDirOverride(run.dirOverride(text->style()->rtlOrdering() == VisualOrder));
    if (run.m_hasHyphen)
        textBox->setHasHyphen(true);
    return textBox;
}

static inline void dirtyLineBoxesForObject(LayoutObject* o, bool fullLayout)
{
    if (o->isText()) {
        LayoutText* layoutText = toLayoutText(o);
        layoutText->dirtyOrDeleteLineBoxesIfNeeded(fullLayout);
    } else {
        toLayoutInline(o)->dirtyLineBoxes(fullLayout);
    }
}

static bool parentIsConstructedOrHaveNext(InlineFlowBox* parentBox)
{
    do {
        if (parentBox->isConstructed() || parentBox->nextOnLine())
            return true;
        parentBox = parentBox->parent();
    } while (parentBox);
    return false;
}

InlineFlowBox* LayoutBlockFlow::createLineBoxes(LayoutObject* obj, const LineInfo& lineInfo, InlineBox* childBox)
{
    // See if we have an unconstructed line box for this object that is also
    // the last item on the line.
    unsigned lineDepth = 1;
    InlineFlowBox* parentBox = nullptr;
    InlineFlowBox* result = nullptr;
    bool hasDefaultLineBoxContain = style()->lineBoxContain() == ComputedStyle::initialLineBoxContain();
    do {
        ASSERT_WITH_SECURITY_IMPLICATION(obj->isLayoutInline() || obj == this);

        LayoutInline* inlineFlow = (obj != this) ? toLayoutInline(obj) : 0;

        // Get the last box we made for this layout object.
        parentBox = inlineFlow ? inlineFlow->lastLineBox() : toLayoutBlock(obj)->lastLineBox();

        // If this box or its ancestor is constructed then it is from a previous line, and we need
        // to make a new box for our line.  If this box or its ancestor is unconstructed but it has
        // something following it on the line, then we know we have to make a new box
        // as well.  In this situation our inline has actually been split in two on
        // the same line (this can happen with very fancy language mixtures).
        bool constructedNewBox = false;
        bool allowedToConstructNewBox = !hasDefaultLineBoxContain || !inlineFlow || inlineFlow->alwaysCreateLineBoxes();
        bool canUseExistingParentBox = parentBox && !parentIsConstructedOrHaveNext(parentBox);
        if (allowedToConstructNewBox && !canUseExistingParentBox) {
            // We need to make a new box for this layout object.  Once
            // made, we need to place it at the end of the current line.
            InlineBox* newBox = createInlineBoxForLayoutObject(obj, obj == this);
            ASSERT_WITH_SECURITY_IMPLICATION(newBox->isInlineFlowBox());
            parentBox = toInlineFlowBox(newBox);
            parentBox->setFirstLineStyleBit(lineInfo.isFirstLine());
            parentBox->setIsHorizontal(isHorizontalWritingMode());
            if (!hasDefaultLineBoxContain)
                parentBox->clearDescendantsHaveSameLineHeightAndBaseline();
            constructedNewBox = true;
        }

        if (constructedNewBox || canUseExistingParentBox) {
            if (!result)
                result = parentBox;

            // If we have hit the block itself, then |box| represents the root
            // inline box for the line, and it doesn't have to be appended to any parent
            // inline.
            if (childBox)
                parentBox->addToLine(childBox);

            if (!constructedNewBox || obj == this)
                break;

            childBox = parentBox;
        }

        // If we've exceeded our line depth, then jump straight to the root and skip all the remaining
        // intermediate inline flows.
        obj = (++lineDepth >= cMaxLineDepth) ? this : obj->parent();

    } while (true);

    return result;
}

template <typename CharacterType>
static inline bool endsWithASCIISpaces(const CharacterType* characters, unsigned pos, unsigned end)
{
    while (isASCIISpace(characters[pos])) {
        pos++;
        if (pos >= end)
            return true;
    }
    return false;
}

static bool reachedEndOfTextRun(const BidiRunList<BidiRun>& bidiRuns)
{
    BidiRun* run = bidiRuns.logicallyLastRun();
    if (!run)
        return true;
    unsigned pos = run->stop();
    LayoutObject* r = run->m_object;
    if (!r->isText() || r->isBR())
        return false;
    LayoutText* layoutText = toLayoutText(r);
    unsigned length = layoutText->textLength();
    if (pos >= length)
        return true;

    if (layoutText->is8Bit())
        return endsWithASCIISpaces(layoutText->characters8(), pos, length);
    return endsWithASCIISpaces(layoutText->characters16(), pos, length);
}

RootInlineBox* LayoutBlockFlow::constructLine(BidiRunList<BidiRun>& bidiRuns, const LineInfo& lineInfo)
{
    ASSERT(bidiRuns.firstRun());

    bool rootHasSelectedChildren = false;
    InlineFlowBox* parentBox = nullptr;
    int runCount = bidiRuns.runCount() - lineInfo.runsFromLeadingWhitespace();
    for (BidiRun* r = bidiRuns.firstRun(); r; r = r->next()) {
        // Create a box for our object.
        bool isOnlyRun = (runCount == 1);
        if (runCount == 2 && !r->m_object->isListMarker())
            isOnlyRun = (!style()->isLeftToRightDirection() ? bidiRuns.lastRun() : bidiRuns.firstRun())->m_object->isListMarker();

        if (lineInfo.isEmpty())
            continue;

        InlineBox* box;
        if (r->m_object->isText())
            box = createInlineBoxForText(*r, isOnlyRun);
        else
            box = createInlineBoxForLayoutObject(r->m_object, false, isOnlyRun);
        r->m_box = box;

        ASSERT(box);
        if (!box)
            continue;

        if (!rootHasSelectedChildren && box->layoutObject().selectionState() != LayoutObject::SelectionNone)
            rootHasSelectedChildren = true;

        // If we have no parent box yet, or if the run is not simply a sibling,
        // then we need to construct inline boxes as necessary to properly enclose the
        // run's inline box. Segments can only be siblings at the root level, as
        // they are positioned separately.
        if (!parentBox || parentBox->layoutObject() != r->m_object->parent()) {
            // Create new inline boxes all the way back to the appropriate insertion point.
            parentBox = createLineBoxes(r->m_object->parent(), lineInfo, box);
        } else {
            // Append the inline box to this line.
            parentBox->addToLine(box);
        }

        box->setBidiLevel(r->level());

        if (box->isInlineTextBox()) {
            if (AXObjectCache* cache = document().existingAXObjectCache())
                cache->inlineTextBoxesUpdated(r->m_object);
        }
    }

    // We should have a root inline box.  It should be unconstructed and
    // be the last continuation of our line list.
    ASSERT(lastLineBox() && !lastLineBox()->isConstructed());

    // Set the m_selectedChildren flag on the root inline box if one of the leaf inline box
    // from the bidi runs walk above has a selection state.
    if (rootHasSelectedChildren)
        lastLineBox()->root().setHasSelectedChildren(true);

    // Set bits on our inline flow boxes that indicate which sides should
    // paint borders/margins/padding.  This knowledge will ultimately be used when
    // we determine the horizontal positions and widths of all the inline boxes on
    // the line.
    bool isLogicallyLastRunWrapped = bidiRuns.logicallyLastRun()->m_object && bidiRuns.logicallyLastRun()->m_object->isText() ? !reachedEndOfTextRun(bidiRuns) : true;
    lastLineBox()->determineSpacingForFlowBoxes(lineInfo.isLastLine(), isLogicallyLastRunWrapped, bidiRuns.logicallyLastRun()->m_object);

    // Now mark the line boxes as being constructed.
    lastLineBox()->setConstructed();

    // Return the last line.
    return lastRootBox();
}

ETextAlign LayoutBlockFlow::textAlignmentForLine(bool endsWithSoftBreak) const
{
    ETextAlign alignment = style()->textAlign();
    if (endsWithSoftBreak)
        return alignment;

    if (!RuntimeEnabledFeatures::css3TextEnabled())
        return (alignment == JUSTIFY) ? TASTART : alignment;

    TextAlignLast alignmentLast = style()->textAlignLast();
    switch (alignmentLast) {
    case TextAlignLastStart:
        return TASTART;
    case TextAlignLastEnd:
        return TAEND;
    case TextAlignLastLeft:
        return LEFT;
    case TextAlignLastRight:
        return RIGHT;
    case TextAlignLastCenter:
        return CENTER;
    case TextAlignLastJustify:
        return JUSTIFY;
    case TextAlignLastAuto:
        if (alignment == JUSTIFY)
            return TASTART;
        return alignment;
    }

    return alignment;
}

static void updateLogicalWidthForLeftAlignedBlock(bool isLeftToRightDirection, BidiRun* trailingSpaceRun, LayoutUnit& logicalLeft, LayoutUnit totalLogicalWidth, LayoutUnit availableLogicalWidth)
{
    // The direction of the block should determine what happens with wide lines.
    // In particular with RTL blocks, wide lines should still spill out to the left.
    if (isLeftToRightDirection) {
        if (totalLogicalWidth > availableLogicalWidth && trailingSpaceRun)
            trailingSpaceRun->m_box->setLogicalWidth(std::max(LayoutUnit(), trailingSpaceRun->m_box->logicalWidth() - totalLogicalWidth + availableLogicalWidth));
        return;
    }

    if (trailingSpaceRun)
        trailingSpaceRun->m_box->setLogicalWidth(0);
    else if (totalLogicalWidth > availableLogicalWidth)
        logicalLeft -= (totalLogicalWidth - availableLogicalWidth);
}

static void updateLogicalWidthForRightAlignedBlock(bool isLeftToRightDirection, BidiRun* trailingSpaceRun, LayoutUnit& logicalLeft, LayoutUnit& totalLogicalWidth, LayoutUnit availableLogicalWidth)
{
    // Wide lines spill out of the block based off direction.
    // So even if text-align is right, if direction is LTR, wide lines should overflow out of the right
    // side of the block.
    if (isLeftToRightDirection) {
        if (trailingSpaceRun) {
            totalLogicalWidth -= trailingSpaceRun->m_box->logicalWidth();
            trailingSpaceRun->m_box->setLogicalWidth(0);
        }
        if (totalLogicalWidth < availableLogicalWidth)
            logicalLeft += availableLogicalWidth - totalLogicalWidth;
        return;
    }

    if (totalLogicalWidth > availableLogicalWidth && trailingSpaceRun) {
        trailingSpaceRun->m_box->setLogicalWidth(std::max(LayoutUnit(), trailingSpaceRun->m_box->logicalWidth() - totalLogicalWidth + availableLogicalWidth));
        totalLogicalWidth -= trailingSpaceRun->m_box->logicalWidth();
    } else {
        logicalLeft += availableLogicalWidth - totalLogicalWidth;
    }
}

static void updateLogicalWidthForCenterAlignedBlock(bool isLeftToRightDirection, BidiRun* trailingSpaceRun, LayoutUnit& logicalLeft, LayoutUnit& totalLogicalWidth, LayoutUnit availableLogicalWidth)
{
    LayoutUnit trailingSpaceWidth;
    if (trailingSpaceRun) {
        totalLogicalWidth -= trailingSpaceRun->m_box->logicalWidth();
        trailingSpaceWidth = std::min(trailingSpaceRun->m_box->logicalWidth(), (availableLogicalWidth - totalLogicalWidth + 1) / 2);
        trailingSpaceRun->m_box->setLogicalWidth(std::max(LayoutUnit(), trailingSpaceWidth));
    }
    if (isLeftToRightDirection)
        logicalLeft += std::max((availableLogicalWidth - totalLogicalWidth) / 2, LayoutUnit());
    else
        logicalLeft += totalLogicalWidth > availableLogicalWidth ? (availableLogicalWidth - totalLogicalWidth) : (availableLogicalWidth - totalLogicalWidth) / 2 - trailingSpaceWidth;
}

void LayoutBlockFlow::setMarginsForRubyRun(BidiRun* run, LayoutRubyRun* layoutRubyRun, LayoutObject* previousObject, const LineInfo& lineInfo)
{
    int startOverhang;
    int endOverhang;
    LayoutObject* nextObject = nullptr;
    for (BidiRun* runWithNextObject = run->next(); runWithNextObject; runWithNextObject = runWithNextObject->next()) {
        if (!runWithNextObject->m_object->isOutOfFlowPositioned() && !runWithNextObject->m_box->isLineBreak()) {
            nextObject = runWithNextObject->m_object;
            break;
        }
    }
    layoutRubyRun->getOverhang(lineInfo.isFirstLine(), layoutRubyRun->style()->isLeftToRightDirection() ? previousObject : nextObject, layoutRubyRun->style()->isLeftToRightDirection() ? nextObject : previousObject, startOverhang, endOverhang);
    setMarginStartForChild(*layoutRubyRun, -startOverhang);
    setMarginEndForChild(*layoutRubyRun, -endOverhang);
}

static inline void setLogicalWidthForTextRun(RootInlineBox* lineBox, BidiRun* run, LayoutText* layoutText, LayoutUnit xPos, const LineInfo& lineInfo,
    GlyphOverflowAndFallbackFontsMap& textBoxDataMap, VerticalPositionCache& verticalPositionCache, WordMeasurements& wordMeasurements)
{
    HashSet<const SimpleFontData*> fallbackFonts;
    GlyphOverflow glyphOverflow;

    const Font& font = layoutText->style(lineInfo.isFirstLine())->font();
    // Always compute glyph overflow bounds if the block's line-box-contain value is "glyphs".
    if (lineBox->fitsToGlyphs())
        glyphOverflow.computeBounds = true;

    LayoutUnit hyphenWidth = 0;
    if (toInlineTextBox(run->m_box)->hasHyphen())
        hyphenWidth = layoutText->hyphenWidth(font, run->direction());

    float measuredWidth = 0;
    FloatRect glyphBounds;

    bool kerningIsEnabled = font.fontDescription().typesettingFeatures() & Kerning;

#if OS(MACOSX)
    // FIXME: Having any font feature settings enabled can lead to selection gaps on
    // Chromium-mac. https://bugs.webkit.org/show_bug.cgi?id=113418
    bool canUseSimpleFontCodePath = layoutText->canUseSimpleFontCodePath() && !font.fontDescription().featureSettings();
#else
    bool canUseSimpleFontCodePath = layoutText->canUseSimpleFontCodePath();
#endif

    if (canUseSimpleFontCodePath) {
        int lastEndOffset = run->m_start;
        for (size_t i = 0, size = wordMeasurements.size(); i < size && lastEndOffset < run->m_stop; ++i) {
            const WordMeasurement& wordMeasurement = wordMeasurements[i];
            if (wordMeasurement.startOffset == wordMeasurement.endOffset)
                continue;
            if (wordMeasurement.layoutText != layoutText || wordMeasurement.startOffset != lastEndOffset || wordMeasurement.endOffset > run->m_stop)
                continue;

            lastEndOffset = wordMeasurement.endOffset;
            if (kerningIsEnabled && lastEndOffset == run->m_stop) {
                int wordLength = lastEndOffset - wordMeasurement.startOffset;
                measuredWidth += layoutText->width(wordMeasurement.startOffset, wordLength, xPos, run->direction(), lineInfo.isFirstLine());
                if (i > 0 && wordLength == 1 && layoutText->characterAt(wordMeasurement.startOffset) == ' ')
                    measuredWidth += layoutText->style()->wordSpacing();
            } else {
                FloatRect wordGlyphBounds = wordMeasurement.glyphBounds;
                wordGlyphBounds.move(measuredWidth, 0);
                glyphBounds.unite(wordGlyphBounds);
                measuredWidth += wordMeasurement.width;
            }
            if (!wordMeasurement.fallbackFonts.isEmpty()) {
                HashSet<const SimpleFontData*>::const_iterator end = wordMeasurement.fallbackFonts.end();
                for (HashSet<const SimpleFontData*>::const_iterator it = wordMeasurement.fallbackFonts.begin(); it != end; ++it)
                    fallbackFonts.add(*it);
            }
        }
        if (lastEndOffset != run->m_stop) {
            // If we don't have enough cached data, we'll measure the run again.
            canUseSimpleFontCodePath = false;
            fallbackFonts.clear();
        }
    }

    // Don't put this into 'else' part of the above 'if' because canUseSimpleFontCodePath may be modified in the 'if' block.
    if (!canUseSimpleFontCodePath)
        measuredWidth = layoutText->width(run->m_start, run->m_stop - run->m_start, xPos, run->direction(), lineInfo.isFirstLine(), &fallbackFonts, &glyphBounds);

    // Negative word-spacing and/or letter-spacing may cause some glyphs to overflow the left boundary and result
    // negative measured width. Reset measured width to 0 and adjust glyph bounds accordingly to cover the overflow.
    if (measuredWidth < 0) {
        if (measuredWidth < glyphBounds.x()) {
            glyphBounds.expand(glyphBounds.x() - measuredWidth, 0);
            glyphBounds.setX(measuredWidth);
        }
        measuredWidth = 0;
    }

    glyphOverflow.setFromBounds(glyphBounds, font.fontMetrics().floatAscent(), font.fontMetrics().floatDescent(), measuredWidth);

    run->m_box->setLogicalWidth(measuredWidth + hyphenWidth);
    if (!fallbackFonts.isEmpty()) {
        ASSERT(run->m_box->isText());
        GlyphOverflowAndFallbackFontsMap::ValueType* it = textBoxDataMap.add(toInlineTextBox(run->m_box), std::make_pair(Vector<const SimpleFontData*>(), GlyphOverflow())).storedValue;
        ASSERT(it->value.first.isEmpty());
        copyToVector(fallbackFonts, it->value.first);
        run->m_box->parent()->clearDescendantsHaveSameLineHeightAndBaseline();
    }
    if (!glyphOverflow.isZero()) {
        ASSERT(run->m_box->isText());
        GlyphOverflowAndFallbackFontsMap::ValueType* it = textBoxDataMap.add(toInlineTextBox(run->m_box), std::make_pair(Vector<const SimpleFontData*>(), GlyphOverflow())).storedValue;
        it->value.second = glyphOverflow;
        run->m_box->clearKnownToHaveNoOverflow();
    }
}

static inline void computeExpansionForJustifiedText(BidiRun* firstRun, BidiRun* trailingSpaceRun, Vector<unsigned, 16>& expansionOpportunities, unsigned expansionOpportunityCount, LayoutUnit& totalLogicalWidth, LayoutUnit availableLogicalWidth)
{
    if (!expansionOpportunityCount || availableLogicalWidth <= totalLogicalWidth)
        return;

    size_t i = 0;
    for (BidiRun* r = firstRun; r; r = r->next()) {
        if (!r->m_box || r == trailingSpaceRun)
            continue;

        if (r->m_object->isText()) {
            unsigned opportunitiesInRun = expansionOpportunities[i++];

            ASSERT(opportunitiesInRun <= expansionOpportunityCount);

            // Don't justify for white-space: pre.
            if (r->m_object->style()->whiteSpace() != PRE) {
                InlineTextBox* textBox = toInlineTextBox(r->m_box);
                int expansion = (availableLogicalWidth - totalLogicalWidth) * opportunitiesInRun / expansionOpportunityCount;
                textBox->setExpansion(expansion);
                totalLogicalWidth += expansion;
            }
            expansionOpportunityCount -= opportunitiesInRun;
            if (!expansionOpportunityCount)
                break;
        }
    }
}

void LayoutBlockFlow::updateLogicalWidthForAlignment(const ETextAlign& textAlign, const RootInlineBox* rootInlineBox, BidiRun* trailingSpaceRun, LayoutUnit& logicalLeft, LayoutUnit& totalLogicalWidth, LayoutUnit& availableLogicalWidth, unsigned expansionOpportunityCount)
{
    TextDirection direction;
    if (rootInlineBox && rootInlineBox->layoutObject().style()->unicodeBidi() == Plaintext)
        direction = rootInlineBox->direction();
    else
        direction = style()->direction();

    // Armed with the total width of the line (without justification),
    // we now examine our text-align property in order to determine where to position the
    // objects horizontally. The total width of the line can be increased if we end up
    // justifying text.
    switch (textAlign) {
    case LEFT:
    case WEBKIT_LEFT:
        updateLogicalWidthForLeftAlignedBlock(style()->isLeftToRightDirection(), trailingSpaceRun, logicalLeft, totalLogicalWidth, availableLogicalWidth);
        break;
    case RIGHT:
    case WEBKIT_RIGHT:
        updateLogicalWidthForRightAlignedBlock(style()->isLeftToRightDirection(), trailingSpaceRun, logicalLeft, totalLogicalWidth, availableLogicalWidth);
        break;
    case CENTER:
    case WEBKIT_CENTER:
        updateLogicalWidthForCenterAlignedBlock(style()->isLeftToRightDirection(), trailingSpaceRun, logicalLeft, totalLogicalWidth, availableLogicalWidth);
        break;
    case JUSTIFY:
        adjustInlineDirectionLineBounds(expansionOpportunityCount, logicalLeft, availableLogicalWidth);
        if (expansionOpportunityCount) {
            if (trailingSpaceRun) {
                totalLogicalWidth -= trailingSpaceRun->m_box->logicalWidth();
                trailingSpaceRun->m_box->setLogicalWidth(0);
            }
            break;
        }
        // Fall through
    case TASTART:
        if (direction == LTR)
            updateLogicalWidthForLeftAlignedBlock(style()->isLeftToRightDirection(), trailingSpaceRun, logicalLeft, totalLogicalWidth, availableLogicalWidth);
        else
            updateLogicalWidthForRightAlignedBlock(style()->isLeftToRightDirection(), trailingSpaceRun, logicalLeft, totalLogicalWidth, availableLogicalWidth);
        break;
    case TAEND:
        if (direction == LTR)
            updateLogicalWidthForRightAlignedBlock(style()->isLeftToRightDirection(), trailingSpaceRun, logicalLeft, totalLogicalWidth, availableLogicalWidth);
        else
            updateLogicalWidthForLeftAlignedBlock(style()->isLeftToRightDirection(), trailingSpaceRun, logicalLeft, totalLogicalWidth, availableLogicalWidth);
        break;
    }
    if (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        logicalLeft += verticalScrollbarWidth();
}

static void updateLogicalInlinePositions(LayoutBlockFlow* block, LayoutUnit& lineLogicalLeft, LayoutUnit& lineLogicalRight, LayoutUnit& availableLogicalWidth, bool firstLine, IndentTextOrNot shouldIndentText, LayoutUnit boxLogicalHeight)
{
    LayoutUnit lineLogicalHeight = block->minLineHeightForReplacedObject(firstLine, boxLogicalHeight);
    lineLogicalLeft = block->logicalLeftOffsetForLine(block->logicalHeight(), shouldIndentText == IndentText, lineLogicalHeight);
    lineLogicalRight = block->logicalRightOffsetForLine(block->logicalHeight(), shouldIndentText == IndentText, lineLogicalHeight);
    availableLogicalWidth = lineLogicalRight - lineLogicalLeft;
}

void LayoutBlockFlow::computeInlineDirectionPositionsForLine(RootInlineBox* lineBox, const LineInfo& lineInfo, BidiRun* firstRun, BidiRun* trailingSpaceRun, bool reachedEnd,
    GlyphOverflowAndFallbackFontsMap& textBoxDataMap, VerticalPositionCache& verticalPositionCache, WordMeasurements& wordMeasurements)
{
    ETextAlign textAlign = textAlignmentForLine(!reachedEnd && !lineBox->endsWithBreak());

    // CSS 2.1: "'Text-indent' only affects a line if it is the first formatted line of an element. For example, the first line of an anonymous block
    // box is only affected if it is the first child of its parent element."
    // CSS3 "text-indent", "each-line" affects the first line of the block container as well as each line after a forced line break,
    // but does not affect lines after a soft wrap break.
    bool isFirstLine = lineInfo.isFirstLine() && !(isAnonymousBlock() && parent()->slowFirstChild() != this);
    bool isAfterHardLineBreak = lineBox->prevRootBox() && lineBox->prevRootBox()->endsWithBreak();
    IndentTextOrNot shouldIndentText = requiresIndent(isFirstLine, isAfterHardLineBreak, styleRef());
    LayoutUnit lineLogicalLeft;
    LayoutUnit lineLogicalRight;
    LayoutUnit availableLogicalWidth;
    updateLogicalInlinePositions(this, lineLogicalLeft, lineLogicalRight, availableLogicalWidth, isFirstLine, shouldIndentText, 0);
    bool needsWordSpacing;

    if (firstRun && firstRun->m_object->isReplaced()) {
        LayoutBox* layoutBox = toLayoutBox(firstRun->m_object);
        updateLogicalInlinePositions(this, lineLogicalLeft, lineLogicalRight, availableLogicalWidth, isFirstLine, shouldIndentText, layoutBox->logicalHeight());
    }

    computeInlineDirectionPositionsForSegment(lineBox, lineInfo, textAlign, lineLogicalLeft, availableLogicalWidth, firstRun, trailingSpaceRun, textBoxDataMap, verticalPositionCache, wordMeasurements);
    // The widths of all runs are now known. We can now place every inline box (and
    // compute accurate widths for the inline flow boxes).
    needsWordSpacing = lineBox->isLeftToRightDirection() ? false: true;
    lineBox->placeBoxesInInlineDirection(lineLogicalLeft, needsWordSpacing);
}

BidiRun* LayoutBlockFlow::computeInlineDirectionPositionsForSegment(RootInlineBox* lineBox, const LineInfo& lineInfo, ETextAlign textAlign, LayoutUnit& logicalLeft,
    LayoutUnit& availableLogicalWidth, BidiRun* firstRun, BidiRun* trailingSpaceRun, GlyphOverflowAndFallbackFontsMap& textBoxDataMap, VerticalPositionCache& verticalPositionCache,
    WordMeasurements& wordMeasurements)
{
    bool needsWordSpacing = true;
    LayoutUnit totalLogicalWidth = lineBox->getFlowSpacingLogicalWidth();
    unsigned expansionOpportunityCount = 0;
    bool isAfterExpansion = true;
    Vector<unsigned, 16> expansionOpportunities;
    LayoutObject* previousObject = nullptr;
    TextJustify textJustify = style()->textJustify();

    BidiRun* r = firstRun;
    for (; r; r = r->next()) {
        if (!r->m_box || r->m_object->isOutOfFlowPositioned() || r->m_box->isLineBreak()) {
            continue; // Positioned objects are only participating to figure out their
                // correct static x position.  They have no effect on the width.
                // Similarly, line break boxes have no effect on the width.
        }
        if (r->m_object->isText()) {
            LayoutText* rt = toLayoutText(r->m_object);
            if (textAlign == JUSTIFY && r != trailingSpaceRun && textJustify != TextJustifyNone) {
                if (!isAfterExpansion)
                    toInlineTextBox(r->m_box)->setCanHaveLeadingExpansion(true);
                unsigned opportunitiesInRun;
                if (rt->is8Bit())
                    opportunitiesInRun = Character::expansionOpportunityCount(rt->characters8() + r->m_start, r->m_stop - r->m_start, r->m_box->direction(), isAfterExpansion, textJustify);
                else
                    opportunitiesInRun = Character::expansionOpportunityCount(rt->characters16() + r->m_start, r->m_stop - r->m_start, r->m_box->direction(), isAfterExpansion, textJustify);
                expansionOpportunities.append(opportunitiesInRun);
                expansionOpportunityCount += opportunitiesInRun;
            }

            if (rt->textLength()) {
                if (!r->m_start && needsWordSpacing && isSpaceOrNewline(rt->characterAt(r->m_start)))
                    totalLogicalWidth += rt->style(lineInfo.isFirstLine())->font().fontDescription().wordSpacing();
                needsWordSpacing = !isSpaceOrNewline(rt->characterAt(r->m_stop - 1));
            }

            setLogicalWidthForTextRun(lineBox, r, rt, totalLogicalWidth, lineInfo, textBoxDataMap, verticalPositionCache, wordMeasurements);
        } else {
            isAfterExpansion = false;
            if (!r->m_object->isLayoutInline()) {
                LayoutBox* layoutBox = toLayoutBox(r->m_object);
                if (layoutBox->isRubyRun())
                    setMarginsForRubyRun(r, toLayoutRubyRun(layoutBox), previousObject, lineInfo);
                r->m_box->setLogicalWidth(logicalWidthForChild(*layoutBox));
                totalLogicalWidth += marginStartForChild(*layoutBox) + marginEndForChild(*layoutBox);
                needsWordSpacing = true;
            }
        }

        totalLogicalWidth += r->m_box->logicalWidth();
        previousObject = r->m_object;
    }

    if (isAfterExpansion && !expansionOpportunities.isEmpty()) {
        expansionOpportunities.last()--;
        expansionOpportunityCount--;
    }

    updateLogicalWidthForAlignment(textAlign, lineBox, trailingSpaceRun, logicalLeft, totalLogicalWidth, availableLogicalWidth, expansionOpportunityCount);

    computeExpansionForJustifiedText(firstRun, trailingSpaceRun, expansionOpportunities, expansionOpportunityCount, totalLogicalWidth, availableLogicalWidth);

    return r;
}

void LayoutBlockFlow::computeBlockDirectionPositionsForLine(RootInlineBox* lineBox, BidiRun* firstRun, GlyphOverflowAndFallbackFontsMap& textBoxDataMap,
    VerticalPositionCache& verticalPositionCache)
{
    setLogicalHeight(lineBox->alignBoxesInBlockDirection(logicalHeight(), textBoxDataMap, verticalPositionCache));

    // Now make sure we place replaced layout objects correctly.
    for (BidiRun* r = firstRun; r; r = r->next()) {
        ASSERT(r->m_box);
        if (!r->m_box)
            continue; // Skip runs with no line boxes.

        // Align positioned boxes with the top of the line box.  This is
        // a reasonable approximation of an appropriate y position.
        if (r->m_object->isOutOfFlowPositioned())
            r->m_box->setLogicalTop(logicalHeight());

        // Position is used to properly position both replaced elements and
        // to update the static normal flow x/y of positioned elements.
        if (r->m_object->isText())
            toLayoutText(r->m_object)->positionLineBox(r->m_box);
        else if (r->m_object->isBox())
            toLayoutBox(r->m_object)->positionLineBox(r->m_box);
    }
}

void LayoutBlockFlow::appendFloatingObjectToLastLine(FloatingObject& floatingObject)
{
    ASSERT(!floatingObject.originatingLine());
    floatingObject.setOriginatingLine(lastRootBox());
    lastRootBox()->appendFloat(floatingObject.layoutObject());
}

// This function constructs line boxes for all of the text runs in the resolver and computes their position.
RootInlineBox* LayoutBlockFlow::createLineBoxesFromBidiRuns(unsigned bidiLevel, BidiRunList<BidiRun>& bidiRuns, const InlineIterator& end, LineInfo& lineInfo, VerticalPositionCache& verticalPositionCache, BidiRun* trailingSpaceRun, WordMeasurements& wordMeasurements)
{
    if (!bidiRuns.runCount())
        return nullptr;

    // FIXME: Why is this only done when we had runs?
    lineInfo.setLastLine(!end.object());

    RootInlineBox* lineBox = constructLine(bidiRuns, lineInfo);
    if (!lineBox)
        return nullptr;

    lineBox->setBidiLevel(bidiLevel);
    lineBox->setEndsWithBreak(lineInfo.previousLineBrokeCleanly());

    bool isSVGRootInlineBox = lineBox->isSVGRootInlineBox();

    GlyphOverflowAndFallbackFontsMap textBoxDataMap;

    // Now we position all of our text runs horizontally.
    if (!isSVGRootInlineBox)
        computeInlineDirectionPositionsForLine(lineBox, lineInfo, bidiRuns.firstRun(), trailingSpaceRun, end.atEnd(), textBoxDataMap, verticalPositionCache, wordMeasurements);

    // Now position our text runs vertically.
    computeBlockDirectionPositionsForLine(lineBox, bidiRuns.firstRun(), textBoxDataMap, verticalPositionCache);

    // SVG text layout code computes vertical & horizontal positions on its own.
    // Note that we still need to execute computeVerticalPositionsForLine() as
    // it calls InlineTextBox::positionLineBox(), which tracks whether the box
    // contains reversed text or not. If we wouldn't do that editing and thus
    // text selection in RTL boxes would not work as expected.
    if (isSVGRootInlineBox) {
        ASSERT(isSVGText());
        toSVGRootInlineBox(lineBox)->computePerCharacterLayoutInformation();
    }

    // Compute our overflow now.
    lineBox->computeOverflow(lineBox->lineTop(), lineBox->lineBottom(), textBoxDataMap);

    return lineBox;
}

static void deleteLineRange(LineLayoutState& layoutState, RootInlineBox* startLine, RootInlineBox* stopLine = 0)
{
    RootInlineBox* boxToDelete = startLine;
    while (boxToDelete && boxToDelete != stopLine) {
        layoutState.updatePaintInvalidationRangeFromBox(boxToDelete);
        // Note: deleteLineRange(firstRootBox()) is not identical to deleteLineBoxTree().
        // deleteLineBoxTree uses nextLineBox() instead of nextRootBox() when traversing.
        RootInlineBox* next = boxToDelete->nextRootBox();
        boxToDelete->deleteLine();
        boxToDelete = next;
    }
}

void LayoutBlockFlow::layoutRunsAndFloats(LineLayoutState& layoutState)
{
    // We want to skip ahead to the first dirty line
    InlineBidiResolver resolver;
    RootInlineBox* startLine = determineStartPosition(layoutState, resolver);

    if (containsFloats())
        layoutState.setLastFloat(m_floatingObjects->set().last().get());

    // We also find the first clean line and extract these lines.  We will add them back
    // if we determine that we're able to synchronize after handling all our dirty lines.
    InlineIterator cleanLineStart;
    BidiStatus cleanLineBidiStatus;
    if (!layoutState.isFullLayout() && startLine)
        determineEndPosition(layoutState, startLine, cleanLineStart, cleanLineBidiStatus);

    if (startLine) {
        if (!layoutState.usesPaintInvalidationBounds())
            layoutState.setPaintInvalidationRange(logicalHeight());
        deleteLineRange(layoutState, startLine);
    }

    layoutRunsAndFloatsInRange(layoutState, resolver, cleanLineStart, cleanLineBidiStatus);
    linkToEndLineIfNeeded(layoutState);
    markDirtyFloatsForPaintInvalidation(layoutState.floats());
}

// Before restarting the layout loop with a new logicalHeight, remove all floats that were added and reset the resolver.
inline const InlineIterator& LayoutBlockFlow::restartLayoutRunsAndFloatsInRange(LayoutUnit oldLogicalHeight, LayoutUnit newLogicalHeight,  FloatingObject* lastFloatFromPreviousLine, InlineBidiResolver& resolver,  const InlineIterator& oldEnd)
{
    removeFloatingObjectsBelow(lastFloatFromPreviousLine, oldLogicalHeight);
    setLogicalHeight(newLogicalHeight);
    resolver.setPositionIgnoringNestedIsolates(oldEnd);
    return oldEnd;
}

void LayoutBlockFlow::layoutRunsAndFloatsInRange(LineLayoutState& layoutState,
    InlineBidiResolver& resolver, const InlineIterator& cleanLineStart,
    const BidiStatus& cleanLineBidiStatus)
{
    const ComputedStyle& styleToUse = styleRef();
    bool paginated = view()->layoutState() && view()->layoutState()->isPaginated();
    LineMidpointState& lineMidpointState = resolver.midpointState();
    InlineIterator endOfLine = resolver.position();
    bool checkForEndLineMatch = layoutState.endLine();
    LayoutTextInfo layoutTextInfo;
    VerticalPositionCache verticalPositionCache;

    LineBreaker lineBreaker(LineLayoutBlockFlow(this));

    while (!endOfLine.atEnd()) {
        bool logicalWidthIsAvailable = false;

        // FIXME: Is this check necessary before the first iteration or can it be moved to the end?
        if (checkForEndLineMatch) {
            layoutState.setEndLineMatched(matchedEndLine(layoutState, resolver, cleanLineStart, cleanLineBidiStatus));
            if (layoutState.endLineMatched()) {
                resolver.setPosition(InlineIterator(resolver.position().root(), 0, 0), 0);
                break;
            }
        }

        lineMidpointState.reset();

        layoutState.lineInfo().setEmpty(true);
        layoutState.lineInfo().resetRunsFromLeadingWhitespace();

        const InlineIterator previousEndofLine = endOfLine;
        bool isNewUBAParagraph = layoutState.lineInfo().previousLineBrokeCleanly();
        FloatingObject* lastFloatFromPreviousLine = (containsFloats()) ? m_floatingObjects->set().last().get() : 0;

        WordMeasurements wordMeasurements;
        endOfLine = lineBreaker.nextLineBreak(resolver, layoutState.lineInfo(), layoutTextInfo,
            lastFloatFromPreviousLine, wordMeasurements);
        layoutTextInfo.m_lineBreakIterator.resetPriorContext();
        if (resolver.position().atEnd()) {
            // FIXME: We shouldn't be creating any runs in nextLineBreak to begin with!
            // Once BidiRunList is separated from BidiResolver this will not be needed.
            resolver.runs().deleteRuns();
            resolver.markCurrentRunEmpty(); // FIXME: This can probably be replaced by an ASSERT (or just removed).
            layoutState.setCheckForFloatsFromLastLine(true);
            resolver.setPosition(InlineIterator(resolver.position().root(), 0, 0), 0);
            break;
        }

        ASSERT(endOfLine != resolver.position());

        // This is a short-cut for empty lines.
        if (layoutState.lineInfo().isEmpty()) {
            if (lastRootBox())
                lastRootBox()->setLineBreakInfo(endOfLine.object(), endOfLine.offset(), resolver.status());
        } else {
            VisualDirectionOverride override = (styleToUse.rtlOrdering() == VisualOrder ? (styleToUse.direction() == LTR ? VisualLeftToRightOverride : VisualRightToLeftOverride) : NoVisualOverride);
            if (isNewUBAParagraph && styleToUse.unicodeBidi() == Plaintext && !resolver.context()->parent()) {
                TextDirection direction = determinePlaintextDirectionality(resolver.position().root(), resolver.position().object(), resolver.position().offset());
                resolver.setStatus(BidiStatus(direction, isOverride(styleToUse.unicodeBidi())));
            }
            // FIXME: This ownership is reversed. We should own the BidiRunList and pass it to createBidiRunsForLine.
            BidiRunList<BidiRun>& bidiRuns = resolver.runs();
            constructBidiRunsForLine(resolver, bidiRuns, endOfLine, override, layoutState.lineInfo().previousLineBrokeCleanly(), isNewUBAParagraph);
            ASSERT(resolver.position() == endOfLine);

            BidiRun* trailingSpaceRun = resolver.trailingSpaceRun();

            if (bidiRuns.runCount() && lineBreaker.lineWasHyphenated())
                bidiRuns.logicallyLastRun()->m_hasHyphen = true;

            // Now that the runs have been ordered, we create the line boxes.
            // At the same time we figure out where border/padding/margin should be applied for
            // inline flow boxes.

            LayoutUnit oldLogicalHeight = logicalHeight();
            RootInlineBox* lineBox = createLineBoxesFromBidiRuns(resolver.status().context->level(), bidiRuns, endOfLine, layoutState.lineInfo(), verticalPositionCache, trailingSpaceRun, wordMeasurements);

            bidiRuns.deleteRuns();
            resolver.markCurrentRunEmpty(); // FIXME: This can probably be replaced by an ASSERT (or just removed).

            if (lineBox) {
                lineBox->setLineBreakInfo(endOfLine.object(), endOfLine.offset(), resolver.status());
                if (layoutState.usesPaintInvalidationBounds())
                    layoutState.updatePaintInvalidationRangeFromBox(lineBox);

                if (paginated) {
                    LayoutUnit adjustment = 0;
                    adjustLinePositionForPagination(*lineBox, adjustment);
                    if (adjustment) {
                        LayoutUnit oldLineWidth = availableLogicalWidthForLine(oldLogicalHeight, layoutState.lineInfo().isFirstLine());
                        lineBox->moveInBlockDirection(adjustment);
                        if (layoutState.usesPaintInvalidationBounds())
                            layoutState.updatePaintInvalidationRangeFromBox(lineBox);

                        if (availableLogicalWidthForLine(oldLogicalHeight + adjustment, layoutState.lineInfo().isFirstLine()) != oldLineWidth) {
                            // We have to delete this line, remove all floats that got added, and let line layout re-run.
                            lineBox->deleteLine();
                            endOfLine = restartLayoutRunsAndFloatsInRange(oldLogicalHeight, oldLogicalHeight + adjustment, lastFloatFromPreviousLine, resolver, previousEndofLine);
                            logicalWidthIsAvailable = true;
                        } else {
                            setLogicalHeight(lineBox->lineBottomWithLeading());
                        }
                    }
                }
            }
        }

        if (!logicalWidthIsAvailable) {
            for (size_t i = 0; i < lineBreaker.positionedObjects().size(); ++i)
                setStaticPositions(LineLayoutBlockFlow(this), lineBreaker.positionedObjects()[i]);

            if (!layoutState.lineInfo().isEmpty())
                layoutState.lineInfo().setFirstLine(false);
            clearFloats(lineBreaker.clear());

            if (m_floatingObjects && lastRootBox()) {
                const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
                FloatingObjectSetIterator it = floatingObjectSet.begin();
                FloatingObjectSetIterator end = floatingObjectSet.end();
                if (layoutState.lastFloat()) {
                    FloatingObjectSetIterator lastFloatIterator = floatingObjectSet.find(layoutState.lastFloat());
                    ASSERT(lastFloatIterator != end);
                    ++lastFloatIterator;
                    it = lastFloatIterator;
                }
                for (; it != end; ++it) {
                    FloatingObject& floatingObject = *it->get();
                    appendFloatingObjectToLastLine(floatingObject);
                    ASSERT(floatingObject.layoutObject() == layoutState.floats()[layoutState.floatIndex()].object);
                    // If a float's geometry has changed, give up on syncing with clean lines.
                    if (layoutState.floats()[layoutState.floatIndex()].rect != floatingObject.frameRect())
                        checkForEndLineMatch = false;
                    layoutState.setFloatIndex(layoutState.floatIndex() + 1);
                }
                layoutState.setLastFloat(!floatingObjectSet.isEmpty() ? floatingObjectSet.last().get() : 0);
            }
        }

        lineMidpointState.reset();
        resolver.setPosition(endOfLine, numberOfIsolateAncestors(endOfLine));
    }

    // In case we already adjusted the line positions during this layout to avoid widows
    // then we need to ignore the possibility of having a new widows situation.
    // Otherwise, we risk leaving empty containers which is against the block fragmentation principles.
    if (paginated && !style()->hasAutoWidows() && !didBreakAtLineToAvoidWidow()) {
        // Check the line boxes to make sure we didn't create unacceptable widows.
        // However, we'll prioritize orphans - so nothing we do here should create
        // a new orphan.

        RootInlineBox* lineBox = lastRootBox();

        // Count from the end of the block backwards, to see how many hanging
        // lines we have.
        RootInlineBox* firstLineInBlock = firstRootBox();
        int numLinesHanging = 1;
        while (lineBox && lineBox != firstLineInBlock && !lineBox->isFirstAfterPageBreak()) {
            ++numLinesHanging;
            lineBox = lineBox->prevRootBox();
        }

        // If there were no breaks in the block, we didn't create any widows.
        if (!lineBox || !lineBox->isFirstAfterPageBreak() || lineBox == firstLineInBlock)
            return;

        if (numLinesHanging < style()->widows()) {
            // We have detected a widow. Now we need to work out how many
            // lines there are on the previous page, and how many we need
            // to steal.
            int numLinesNeeded = style()->widows() - numLinesHanging;
            RootInlineBox* currentFirstLineOfNewPage = lineBox;

            // Count the number of lines in the previous page.
            lineBox = lineBox->prevRootBox();
            int numLinesInPreviousPage = 1;
            while (lineBox && lineBox != firstLineInBlock && !lineBox->isFirstAfterPageBreak()) {
                ++numLinesInPreviousPage;
                lineBox = lineBox->prevRootBox();
            }

            // If there was an explicit value for orphans, respect that. If not, we still
            // shouldn't create a situation where we make an orphan bigger than the initial value.
            // This means that setting widows implies we also care about orphans, but given
            // the specification says the initial orphan value is non-zero, this is ok. The
            // author is always free to set orphans explicitly as well.
            int orphans = style()->hasAutoOrphans() ? style()->initialOrphans() : style()->orphans();
            int numLinesAvailable = numLinesInPreviousPage - orphans;
            if (numLinesAvailable <= 0)
                return;

            int numLinesToTake = std::min(numLinesAvailable, numLinesNeeded);
            // Wind back from our first widowed line.
            lineBox = currentFirstLineOfNewPage;
            for (int i = 0; i < numLinesToTake; ++i)
                lineBox = lineBox->prevRootBox();

            // We now want to break at this line. Remember for next layout and trigger relayout.
            setBreakAtLineToAvoidWidow(lineCount(lineBox));
            markLinesDirtyInBlockRange(lastRootBox()->lineBottomWithLeading(), lineBox->lineBottomWithLeading(), lineBox);
        }
    }

    clearDidBreakAtLineToAvoidWidow();
}

void LayoutBlockFlow::linkToEndLineIfNeeded(LineLayoutState& layoutState)
{
    if (layoutState.endLine()) {
        if (layoutState.endLineMatched()) {
            bool paginated = view()->layoutState() && view()->layoutState()->isPaginated();
            // Attach all the remaining lines, and then adjust their y-positions as needed.
            LayoutUnit delta = logicalHeight() - layoutState.endLineLogicalTop();
            for (RootInlineBox* line = layoutState.endLine(); line; line = line->nextRootBox()) {
                line->attachLine();
                if (paginated) {
                    delta -= line->paginationStrut();
                    adjustLinePositionForPagination(*line, delta);
                }
                if (delta) {
                    layoutState.updatePaintInvalidationRangeFromBox(line, delta);
                    line->moveInBlockDirection(delta);
                }
                if (Vector<LayoutBox*>* cleanLineFloats = line->floatsPtr()) {
                    for (auto* box : *cleanLineFloats) {
                        FloatingObject* floatingObject = insertFloatingObject(*box);
                        ASSERT(!floatingObject->originatingLine());
                        floatingObject->setOriginatingLine(line);
                        setLogicalHeight(logicalTopForChild(*box) - marginBeforeForChild(*box) + delta);
                        positionNewFloats();
                    }
                }
            }
            setLogicalHeight(lastRootBox()->lineBottomWithLeading());
        } else {
            // Delete all the remaining lines.
            deleteLineRange(layoutState, layoutState.endLine());
        }
    }

    if (m_floatingObjects && (layoutState.checkForFloatsFromLastLine() || positionNewFloats()) && lastRootBox()) {
        // In case we have a float on the last line, it might not be positioned up to now.
        // This has to be done before adding in the bottom border/padding, or the float will
        // include the padding incorrectly. -dwh
        const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
        FloatingObjectSetIterator it = floatingObjectSet.begin();
        FloatingObjectSetIterator end = floatingObjectSet.end();
        if (layoutState.lastFloat()) {
            FloatingObjectSetIterator lastFloatIterator = floatingObjectSet.find(layoutState.lastFloat());
            ASSERT(lastFloatIterator != end);
            ++lastFloatIterator;
            it = lastFloatIterator;
        }
        layoutState.setLastFloat(!floatingObjectSet.isEmpty() ? floatingObjectSet.last().get() : 0);

        if (it == end)
            return;

        if (layoutState.checkForFloatsFromLastLine()) {
            LayoutUnit bottomVisualOverflow = lastRootBox()->logicalBottomVisualOverflow();
            LayoutUnit bottomLayoutOverflow = lastRootBox()->logicalBottomLayoutOverflow();
            TrailingFloatsRootInlineBox* trailingFloatsLineBox = new TrailingFloatsRootInlineBox(*this);
            m_lineBoxes.appendLineBox(trailingFloatsLineBox);
            trailingFloatsLineBox->setConstructed();
            GlyphOverflowAndFallbackFontsMap textBoxDataMap;
            VerticalPositionCache verticalPositionCache;
            LayoutUnit blockLogicalHeight = logicalHeight();
            trailingFloatsLineBox->alignBoxesInBlockDirection(blockLogicalHeight, textBoxDataMap, verticalPositionCache);
            trailingFloatsLineBox->setLineTopBottomPositions(blockLogicalHeight, blockLogicalHeight, blockLogicalHeight, blockLogicalHeight);
            LayoutRect logicalLayoutOverflow(0, blockLogicalHeight, 1, bottomLayoutOverflow - blockLogicalHeight);
            LayoutRect logicalVisualOverflow(0, blockLogicalHeight, 1, bottomVisualOverflow - blockLogicalHeight);
            trailingFloatsLineBox->setOverflowFromLogicalRects(logicalLayoutOverflow, logicalVisualOverflow, trailingFloatsLineBox->lineTop(), trailingFloatsLineBox->lineBottom());
        }

        for (; it != end; ++it)
            appendFloatingObjectToLastLine(*it->get());
    }
}

void LayoutBlockFlow::markDirtyFloatsForPaintInvalidation(Vector<FloatWithRect>& floats)
{
    size_t floatCount = floats.size();
    // Floats that did not have layout did not paint invalidations when we laid them out. They would have
    // painted by now if they had moved, but if they stayed at (0, 0), they still need to be
    // painted.
    for (size_t i = 0; i < floatCount; ++i) {
        if (!floats[i].everHadLayout) {
            LayoutBox* f = floats[i].object;
            if (!f->location().x() && !f->location().y())
                f->setShouldDoFullPaintInvalidation();
        }
    }
}

struct InlineMinMaxIterator {
/* InlineMinMaxIterator is a class that will iterate over all layout objects that contribute to
   inline min/max width calculations.  Note the following about the way it walks:
   (1) Positioned content is skipped (since it does not contribute to min/max width of a block)
   (2) We do not drill into the children of floats or replaced elements, since you can't break
       in the middle of such an element.
   (3) Inline flows (e.g., <a>, <span>, <i>) are walked twice, since each side can have
       distinct borders/margin/padding that contribute to the min/max width.
*/
    LayoutObject* parent;
    LayoutObject* current;
    bool endOfInline;

    InlineMinMaxIterator(LayoutObject* p, bool end = false)
        : parent(p), current(p), endOfInline(end)
    {

    }

    LayoutObject* next();
};

LayoutObject* InlineMinMaxIterator::next()
{
    LayoutObject* result = nullptr;
    bool oldEndOfInline = endOfInline;
    endOfInline = false;
    while (current || current == parent) {
        if (!oldEndOfInline && (current == parent || (!current->isFloating() && !current->isReplaced() && !current->isOutOfFlowPositioned())))
            result = current->slowFirstChild();

        if (!result) {
            // We hit the end of our inline. (It was empty, e.g., <span></span>.)
            if (!oldEndOfInline && current->isLayoutInline()) {
                result = current;
                endOfInline = true;
                break;
            }

            while (current && current != parent) {
                result = current->nextSibling();
                if (result)
                    break;
                current = current->parent();
                if (current && current != parent && current->isLayoutInline()) {
                    result = current;
                    endOfInline = true;
                    break;
                }
            }
        }

        if (!result)
            break;

        if (!result->isOutOfFlowPositioned() && (result->isText() || result->isFloating() || result->isReplaced() || result->isLayoutInline()))
            break;

        current = result;
        result = nullptr;
    }

    // Update our position.
    current = result;
    return current;
}

static LayoutUnit getBPMWidth(LayoutUnit childValue, Length cssUnit)
{
    if (cssUnit.type() != Auto)
        return (cssUnit.isFixed() ? static_cast<LayoutUnit>(cssUnit.value()) : childValue);
    return LayoutUnit();
}

static LayoutUnit getBorderPaddingMargin(const LayoutBoxModelObject& child, bool endOfInline)
{
    const ComputedStyle& childStyle = child.styleRef();
    if (endOfInline) {
        return getBPMWidth(child.marginEnd(), childStyle.marginEnd()) +
            getBPMWidth(child.paddingEnd(), childStyle.paddingEnd()) +
            child.borderEnd();
    }
    return getBPMWidth(child.marginStart(), childStyle.marginStart()) +
        getBPMWidth(child.paddingStart(), childStyle.paddingStart()) +
        child.borderStart();
}

static inline void stripTrailingSpace(LayoutUnit& inlineMax, LayoutUnit& inlineMin,
    LayoutObject* trailingSpaceChild)
{
    if (trailingSpaceChild && trailingSpaceChild->isText()) {
        // Collapse away the trailing space at the end of a block by finding
        // the first white-space character and subtracting its width. Subsequent
        // white-space characters have been collapsed into the first one (which
        // can be either a space or a tab character).
        LayoutText* text = toLayoutText(trailingSpaceChild);
        UChar trailingWhitespaceChar = ' ';
        for (unsigned i = text->textLength(); i > 0; i--) {
            UChar c = text->characterAt(i - 1);
            if (!Character::treatAsSpace(c))
                break;
            trailingWhitespaceChar = c;
        }

        // FIXME: This ignores first-line.
        const Font& font = text->style()->font();
        TextRun run = constructTextRun(text, font, &trailingWhitespaceChar, 1,
            text->styleRef(), text->style()->direction());
        run.setCodePath(text->canUseSimpleFontCodePath()
            ? TextRun::ForceSimple
            : TextRun::ForceComplex);
        float spaceWidth = font.width(run);
        inlineMax -= LayoutUnit::fromFloatCeil(spaceWidth + font.fontDescription().wordSpacing());
        if (inlineMin > inlineMax)
            inlineMin = inlineMax;
    }
}

// When converting between floating point and LayoutUnits we risk losing precision
// with each conversion. When this occurs while accumulating our preferred widths,
// we can wind up with a line width that's larger than our maxPreferredWidth due to
// pure float accumulation.
static inline LayoutUnit adjustFloatForSubPixelLayout(float value)
{
    return LayoutUnit::fromFloatCeil(value);
}

// FIXME: This function should be broken into something less monolithic.
// FIXME: The main loop here is very similar to LineBreaker::nextSegmentBreak. They can probably reuse code.
void LayoutBlockFlow::computeInlinePreferredLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth)
{
    LayoutUnit inlineMax;
    LayoutUnit inlineMin;

    const ComputedStyle& styleToUse = styleRef();
    LayoutBlock* containingBlock = this->containingBlock();
    LayoutUnit cw = containingBlock ? containingBlock->contentLogicalWidth() : LayoutUnit();

    // If we are at the start of a line, we want to ignore all white-space.
    // Also strip spaces if we previously had text that ended in a trailing space.
    bool stripFrontSpaces = true;
    LayoutObject* trailingSpaceChild = nullptr;

    // Firefox and Opera will allow a table cell to grow to fit an image inside it under
    // very specific cirucumstances (in order to match common WinIE layouts).
    // Not supporting the quirk has caused us to mis-layout some real sites. (See Bugzilla 10517.)
    bool allowImagesToBreak = !document().inQuirksMode() || !isTableCell() || !styleToUse.logicalWidth().isIntrinsicOrAuto();

    bool autoWrap, oldAutoWrap;
    autoWrap = oldAutoWrap = styleToUse.autoWrap();

    InlineMinMaxIterator childIterator(this);

    // Only gets added to the max preffered width once.
    bool addedTextIndent = false;
    // Signals the text indent was more negative than the min preferred width
    bool hasRemainingNegativeTextIndent = false;

    LayoutUnit textIndent = minimumValueForLength(styleToUse.textIndent(), cw);
    LayoutObject* prevFloat = nullptr;
    bool isPrevChildInlineFlow = false;
    bool shouldBreakLineAfterText = false;
    while (LayoutObject* child = childIterator.next()) {
        autoWrap = child->isReplaced() ? child->parent()->style()->autoWrap() :
            child->style()->autoWrap();

        if (!child->isBR()) {
            // Step One: determine whether or not we need to go ahead and
            // terminate our current line. Each discrete chunk can become
            // the new min-width, if it is the widest chunk seen so far, and
            // it can also become the max-width.

            // Children fall into three categories:
            // (1) An inline flow object. These objects always have a min/max of 0,
            // and are included in the iteration solely so that their margins can
            // be added in.
            //
            // (2) An inline non-text non-flow object, e.g., an inline replaced element.
            // These objects can always be on a line by themselves, so in this situation
            // we need to go ahead and break the current line, and then add in our own
            // margins and min/max width on its own line, and then terminate the line.
            //
            // (3) A text object. Text runs can have breakable characters at the start,
            // the middle or the end. They may also lose whitespace off the front if
            // we're already ignoring whitespace. In order to compute accurate min-width
            // information, we need three pieces of information.
            // (a) the min-width of the first non-breakable run. Should be 0 if the text string
            // starts with whitespace.
            // (b) the min-width of the last non-breakable run. Should be 0 if the text string
            // ends with whitespace.
            // (c) the min/max width of the string (trimmed for whitespace).
            //
            // If the text string starts with whitespace, then we need to go ahead and
            // terminate our current line (unless we're already in a whitespace stripping
            // mode.
            //
            // If the text string has a breakable character in the middle, but didn't start
            // with whitespace, then we add the width of the first non-breakable run and
            // then end the current line. We then need to use the intermediate min/max width
            // values (if any of them are larger than our current min/max). We then look at
            // the width of the last non-breakable run and use that to start a new line
            // (unless we end in whitespace).
            const ComputedStyle& childStyle = child->styleRef();
            LayoutUnit childMin;
            LayoutUnit childMax;

            if (!child->isText()) {
                // Case (1) and (2). Inline replaced and inline flow elements.
                if (child->isLayoutInline()) {
                    // Add in padding/border/margin from the appropriate side of
                    // the element.
                    LayoutUnit bpm = getBorderPaddingMargin(toLayoutInline(*child), childIterator.endOfInline);
                    childMin += bpm;
                    childMax += bpm;

                    inlineMin += childMin;
                    inlineMax += childMax;

                    child->clearPreferredLogicalWidthsDirty();
                } else {
                    // Inline replaced elts add in their margins to their min/max values.
                    LayoutUnit margins;
                    Length startMargin = childStyle.marginStart();
                    Length endMargin = childStyle.marginEnd();
                    if (startMargin.isFixed())
                        margins += adjustFloatForSubPixelLayout(startMargin.value());
                    if (endMargin.isFixed())
                        margins += adjustFloatForSubPixelLayout(endMargin.value());
                    childMin += margins;
                    childMax += margins;
                }
            }

            if (!child->isLayoutInline() && !child->isText()) {
                // Case (2). Inline replaced elements and floats.
                // Go ahead and terminate the current line as far as
                // minwidth is concerned.
                LayoutUnit childMinPreferredLogicalWidth, childMaxPreferredLogicalWidth;
                if (child->isBox() && child->isHorizontalWritingMode() != isHorizontalWritingMode()) {
                    LayoutBox* childBox = toLayoutBox(child);
                    LogicalExtentComputedValues computedValues;
                    childBox->computeLogicalHeight(childBox->borderAndPaddingLogicalHeight(), 0, computedValues);
                    childMinPreferredLogicalWidth = childMaxPreferredLogicalWidth = computedValues.m_extent;
                } else {
                    childMinPreferredLogicalWidth = child->minPreferredLogicalWidth();
                    childMaxPreferredLogicalWidth = child->maxPreferredLogicalWidth();
                }
                childMin += childMinPreferredLogicalWidth;
                childMax += childMaxPreferredLogicalWidth;

                bool clearPreviousFloat;
                if (child->isFloating()) {
                    clearPreviousFloat = (prevFloat
                        && ((prevFloat->styleRef().floating() == LeftFloat && (childStyle.clear() & CLEFT))
                            || (prevFloat->styleRef().floating() == RightFloat && (childStyle.clear() & CRIGHT))));
                    prevFloat = child;
                } else {
                    clearPreviousFloat = false;
                }

                bool canBreakReplacedElement = !child->isImage() || allowImagesToBreak;
                if ((canBreakReplacedElement && (autoWrap || oldAutoWrap) && (!isPrevChildInlineFlow || shouldBreakLineAfterText)) || clearPreviousFloat) {
                    minLogicalWidth = std::max(minLogicalWidth, inlineMin);
                    inlineMin = LayoutUnit();
                }

                // If we're supposed to clear the previous float, then terminate maxwidth as well.
                if (clearPreviousFloat) {
                    maxLogicalWidth = std::max(maxLogicalWidth, inlineMax);
                    inlineMax = LayoutUnit();
                }

                // Add in text-indent. This is added in only once.
                if (!addedTextIndent && !child->isFloating()) {
                    childMin += textIndent;
                    childMax += textIndent;

                    if (childMin < LayoutUnit())
                        textIndent = childMin;
                    else
                        addedTextIndent = true;
                }

                // Add our width to the max.
                inlineMax += std::max(LayoutUnit(), childMax);

                if (!autoWrap || !canBreakReplacedElement || (isPrevChildInlineFlow && !shouldBreakLineAfterText)) {
                    if (child->isFloating())
                        minLogicalWidth = std::max(minLogicalWidth, childMin);
                    else
                        inlineMin += childMin;
                } else {
                    // Now check our line.
                    minLogicalWidth = std::max(minLogicalWidth, childMin);

                    // Now start a new line.
                    inlineMin = LayoutUnit();
                }

                if (autoWrap && canBreakReplacedElement && isPrevChildInlineFlow) {
                    minLogicalWidth = std::max(minLogicalWidth, inlineMin);
                    inlineMin = LayoutUnit();
                }

                // We are no longer stripping whitespace at the start of
                // a line.
                if (!child->isFloating()) {
                    stripFrontSpaces = false;
                    trailingSpaceChild = nullptr;
                }
            } else if (child->isText()) {
                // Case (3). Text.
                LayoutText* t = toLayoutText(child);

                if (t->isWordBreak()) {
                    minLogicalWidth = std::max(minLogicalWidth, inlineMin);
                    inlineMin = LayoutUnit();
                    continue;
                }

                // Determine if we have a breakable character. Pass in
                // whether or not we should ignore any spaces at the front
                // of the string. If those are going to be stripped out,
                // then they shouldn't be considered in the breakable char
                // check.
                bool hasBreakableChar, hasBreak;
                LayoutUnit firstLineMinWidth, lastLineMinWidth;
                bool hasBreakableStart, hasBreakableEnd;
                LayoutUnit firstLineMaxWidth, lastLineMaxWidth;
                t->trimmedPrefWidths(inlineMax,
                    firstLineMinWidth, hasBreakableStart, lastLineMinWidth, hasBreakableEnd,
                    hasBreakableChar, hasBreak, firstLineMaxWidth, lastLineMaxWidth,
                    childMin, childMax, stripFrontSpaces, styleToUse.direction());

                // This text object will not be layed out, but it may still provide a breaking opportunity.
                if (!hasBreak && !childMax) {
                    if (autoWrap && (hasBreakableStart || hasBreakableEnd)) {
                        minLogicalWidth = std::max(minLogicalWidth, inlineMin);
                        inlineMin = LayoutUnit();
                    }
                    continue;
                }

                if (stripFrontSpaces)
                    trailingSpaceChild = child;
                else
                    trailingSpaceChild = nullptr;

                // Add in text-indent. This is added in only once.
                LayoutUnit ti;
                if (!addedTextIndent || hasRemainingNegativeTextIndent) {
                    ti = textIndent;
                    childMin += ti;
                    firstLineMinWidth += ti;

                    // It the text indent negative and larger than the child minimum, we re-use the remainder
                    // in future minimum calculations, but using the negative value again on the maximum
                    // will lead to under-counting the max pref width.
                    if (!addedTextIndent) {
                        childMax += ti;
                        firstLineMaxWidth += ti;
                        addedTextIndent = true;
                    }

                    if (childMin < LayoutUnit()) {
                        textIndent = childMin;
                        hasRemainingNegativeTextIndent = true;
                    }
                }

                // If we have no breakable characters at all,
                // then this is the easy case. We add ourselves to the current
                // min and max and continue.
                if (!hasBreakableChar) {
                    inlineMin += childMin;
                } else {
                    if (hasBreakableStart) {
                        minLogicalWidth = std::max(minLogicalWidth, inlineMin);
                    } else {
                        inlineMin += firstLineMinWidth;
                        minLogicalWidth = std::max(minLogicalWidth, inlineMin);
                        childMin -= ti;
                    }

                    inlineMin = childMin;

                    if (hasBreakableEnd) {
                        minLogicalWidth = std::max(minLogicalWidth, inlineMin);
                        inlineMin = LayoutUnit();
                        shouldBreakLineAfterText = false;
                    } else {
                        minLogicalWidth = std::max(minLogicalWidth, inlineMin);
                        inlineMin = lastLineMinWidth;
                        shouldBreakLineAfterText = true;
                    }
                }

                if (hasBreak) {
                    inlineMax += firstLineMaxWidth;
                    maxLogicalWidth = std::max(maxLogicalWidth, inlineMax);
                    maxLogicalWidth = std::max(maxLogicalWidth, childMax);
                    inlineMax = lastLineMaxWidth;
                    addedTextIndent = true;
                } else {
                    inlineMax += std::max(LayoutUnit(), childMax);
                }
            }

            // Ignore spaces after a list marker.
            if (child->isListMarker())
                stripFrontSpaces = true;
        } else {
            minLogicalWidth = std::max(minLogicalWidth, inlineMin);
            maxLogicalWidth = std::max(maxLogicalWidth, inlineMax);
            inlineMin = inlineMax = LayoutUnit();
            stripFrontSpaces = true;
            trailingSpaceChild = nullptr;
            addedTextIndent = true;
        }

        if (!child->isText() && child->isLayoutInline())
            isPrevChildInlineFlow = true;
        else
            isPrevChildInlineFlow = false;

        oldAutoWrap = autoWrap;
    }

    if (styleToUse.collapseWhiteSpace())
        stripTrailingSpace(inlineMax, inlineMin, trailingSpaceChild);

    minLogicalWidth = std::max(minLogicalWidth, inlineMin);
    maxLogicalWidth = std::max(maxLogicalWidth, inlineMax);
}

void LayoutBlockFlow::layoutInlineChildren(bool relayoutChildren, LayoutUnit& paintInvalidationLogicalTop, LayoutUnit& paintInvalidationLogicalBottom, LayoutUnit afterEdge)
{
    LayoutFlowThread* flowThread = flowThreadContainingBlock();
    bool clearLinesForPagination = firstLineBox() && flowThread && !flowThread->hasColumnSets();

    // Figure out if we should clear out our line boxes.
    // FIXME: Handle resize eventually!
    bool isFullLayout = !firstLineBox() || selfNeedsLayout() || relayoutChildren || clearLinesForPagination;
    LineLayoutState layoutState(isFullLayout, paintInvalidationLogicalTop, paintInvalidationLogicalBottom, flowThread);

    if (isFullLayout) {
        // Ensure the old line boxes will be erased.
        if (firstLineBox())
            setShouldDoFullPaintInvalidation();
        lineBoxes()->deleteLineBoxes();
    }

    // Text truncation kicks in in two cases:
    //     1) If your overflow isn't visible and your text-overflow-mode isn't clip.
    //     2) If you're an anonymous block with a block parent that satisfies #1 that was created
    //        to accomodate a block that has inline and block children. This excludes parents where
    //        canCollapseAnonymousBlockChild is false, notabley flex items and grid items.
    // FIXME: CSS3 says that descendants that are clipped must also know how to truncate.  This is insanely
    // difficult to figure out in general (especially in the middle of doing layout), so we only handle the
    // simple case of an anonymous block truncating when it's parent is clipped.
    bool hasTextOverflow = (style()->textOverflow() && hasOverflowClip())
        || (isAnonymousBlock() && parent() && parent()->isLayoutBlock() && toLayoutBlock(parent())->canCollapseAnonymousBlockChild()
            && parent()->style()->textOverflow() && parent()->hasOverflowClip());

    // Walk all the lines and delete our ellipsis line boxes if they exist.
    if (hasTextOverflow)
        deleteEllipsisLineBoxes();

    if (firstChild()) {
        // In full layout mode, clear the line boxes of children upfront. Otherwise,
        // siblings can run into stale root lineboxes during layout. Then layout
        // the replaced elements later. In partial layout mode, line boxes are not
        // deleted and only dirtied. In that case, we can layout the replaced
        // elements at the same time.
        Vector<LayoutBox*> replacedChildren;
        for (InlineWalker walker(this); !walker.atEnd(); walker.advance()) {
            LayoutObject* o = walker.current();

            if (!layoutState.hasInlineChild() && o->isInline())
                layoutState.setHasInlineChild(true);

            if (o->isReplaced() || o->isFloating() || o->isOutOfFlowPositioned()) {
                LayoutBox* box = toLayoutBox(o);

                updateBlockChildDirtyBitsBeforeLayout(relayoutChildren, *box);

                if (o->isOutOfFlowPositioned()) {
                    o->containingBlock()->insertPositionedObject(box);
                } else if (o->isFloating()) {
                    layoutState.floats().append(FloatWithRect(box));
                } else if (isFullLayout || o->needsLayout()) {
                    // Replaced element.
                    box->dirtyLineBoxes(isFullLayout);
                    if (isFullLayout)
                        replacedChildren.append(box);
                    else
                        o->layoutIfNeeded();
                }
            } else if (o->isText() || (o->isLayoutInline() && !walker.atEndOfInline())) {
                if (!o->isText())
                    toLayoutInline(o)->updateAlwaysCreateLineBoxes(layoutState.isFullLayout());
                if (layoutState.isFullLayout() || o->selfNeedsLayout())
                    dirtyLineBoxesForObject(o, layoutState.isFullLayout());
                o->clearNeedsLayout();
            }
        }

        for (size_t i = 0; i < replacedChildren.size(); i++)
            replacedChildren[i]->layoutIfNeeded();

        layoutRunsAndFloats(layoutState);
    }

    // Expand the last line to accommodate Ruby and emphasis marks.
    int lastLineAnnotationsAdjustment = 0;
    if (lastRootBox()) {
        LayoutUnit lowestAllowedPosition = std::max(lastRootBox()->lineBottom(), logicalHeight() + paddingAfter());
        if (!style()->isFlippedLinesWritingMode())
            lastLineAnnotationsAdjustment = lastRootBox()->computeUnderAnnotationAdjustment(lowestAllowedPosition);
        else
            lastLineAnnotationsAdjustment = lastRootBox()->computeOverAnnotationAdjustment(lowestAllowedPosition);
    }

    // Now add in the bottom border/padding.
    setLogicalHeight(logicalHeight() + lastLineAnnotationsAdjustment + afterEdge);

    if (!firstLineBox() && hasLineIfEmpty())
        setLogicalHeight(logicalHeight() + lineHeight(true, isHorizontalWritingMode() ? HorizontalLine : VerticalLine, PositionOfInteriorLineBoxes));

    // See if we have any lines that spill out of our block.  If we do, then we will possibly need to
    // truncate text.
    if (hasTextOverflow)
        checkLinesForTextOverflow();

    // Ensure the new line boxes will be painted.
    if (isFullLayout && firstLineBox())
        setShouldDoFullPaintInvalidation();
}

void LayoutBlockFlow::checkFloatsInCleanLine(RootInlineBox* line, Vector<FloatWithRect>& floats, size_t& floatIndex, bool& encounteredNewFloat, bool& dirtiedByFloat)
{
    Vector<LayoutBox*>* cleanLineFloats = line->floatsPtr();
    if (!cleanLineFloats)
        return;

    for (auto* floatingBox : *cleanLineFloats) {
        floatingBox->layoutIfNeeded();
        LayoutSize newSize = floatingBox->size() +
            LayoutSize(floatingBox->marginWidth(), floatingBox->marginHeight());
        if (floats[floatIndex].object != floatingBox) {
            encounteredNewFloat = true;
            return;
        }

        if (floats[floatIndex].rect.size() != newSize) {
            LayoutUnit floatTop = isHorizontalWritingMode() ? floats[floatIndex].rect.y() : floats[floatIndex].rect.x();
            LayoutUnit floatHeight = isHorizontalWritingMode() ? std::max(floats[floatIndex].rect.height(), newSize.height())
                : std::max(floats[floatIndex].rect.width(), newSize.width());
            floatHeight = std::min(floatHeight, LayoutUnit::max() - floatTop);
            line->markDirty();
            markLinesDirtyInBlockRange(line->lineBottomWithLeading(), floatTop + floatHeight, line);
            floats[floatIndex].rect.setSize(newSize);
            dirtiedByFloat = true;
        }
        floatIndex++;
    }
}

RootInlineBox* LayoutBlockFlow::determineStartPosition(LineLayoutState& layoutState, InlineBidiResolver& resolver)
{
    RootInlineBox* curr = nullptr;
    RootInlineBox* last = nullptr;
    RootInlineBox* firstLineBoxWithBreakAndClearance = 0;

    // FIXME: This entire float-checking block needs to be broken into a new function.
    bool dirtiedByFloat = false;
    if (!layoutState.isFullLayout()) {
        // Paginate all of the clean lines.
        bool paginated = view()->layoutState() && view()->layoutState()->isPaginated();
        LayoutUnit paginationDelta = 0;
        size_t floatIndex = 0;
        for (curr = firstRootBox(); curr && !curr->isDirty(); curr = curr->nextRootBox()) {
            if (paginated) {
                paginationDelta -= curr->paginationStrut();
                adjustLinePositionForPagination(*curr, paginationDelta);
                if (paginationDelta) {
                    if (containsFloats() || !layoutState.floats().isEmpty()) {
                        // FIXME: Do better eventually.  For now if we ever shift because of pagination and floats are present just go to a full layout.
                        layoutState.markForFullLayout();
                        break;
                    }

                    layoutState.updatePaintInvalidationRangeFromBox(curr, paginationDelta);
                    curr->moveInBlockDirection(paginationDelta);
                }
            }

            // If the linebox breaks cleanly and with clearance then dirty from at least this point onwards so that we can clear the correct floats without difficulty.
            if (!firstLineBoxWithBreakAndClearance && lineBoxHasBRWithClearance(curr))
                firstLineBoxWithBreakAndClearance = curr;

            // If a new float has been inserted before this line or before its last known float, just do a full layout.
            bool encounteredNewFloat = false;
            checkFloatsInCleanLine(curr, layoutState.floats(), floatIndex, encounteredNewFloat, dirtiedByFloat);
            if (encounteredNewFloat)
                layoutState.markForFullLayout();

            if (dirtiedByFloat || layoutState.isFullLayout())
                break;
        }
        // Check if a new float has been inserted after the last known float.
        if (!curr && floatIndex < layoutState.floats().size())
            layoutState.markForFullLayout();
    }

    if (layoutState.isFullLayout()) {
        // If we encountered a new float and have inline children, mark ourself to force us to issue paint invalidations.
        if (layoutState.hasInlineChild() && !selfNeedsLayout()) {
            setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::FloatDescendantChanged, MarkOnlyThis);
            setShouldDoFullPaintInvalidation();
        }

        deleteLineBoxTree();
        curr = nullptr;
        ASSERT(!firstLineBox() && !lastLineBox());
    } else {
        if (firstLineBoxWithBreakAndClearance)
            curr = firstLineBoxWithBreakAndClearance;
        if (curr) {
            // We have a dirty line.
            if (RootInlineBox* prevRootBox = curr->prevRootBox()) {
                // We have a previous line.
                if (!dirtiedByFloat && (!prevRootBox->endsWithBreak() || !prevRootBox->lineBreakObj() || (prevRootBox->lineBreakObj()->isText() && prevRootBox->lineBreakPos() >= toLayoutText(prevRootBox->lineBreakObj())->textLength()))) {
                    // The previous line didn't break cleanly or broke at a newline
                    // that has been deleted, so treat it as dirty too.
                    curr = prevRootBox;
                }
            }
        } else {
            // No dirty lines were found.
            // If the last line didn't break cleanly, treat it as dirty.
            if (lastRootBox() && !lastRootBox()->endsWithBreak())
                curr = lastRootBox();
        }

        // If we have no dirty lines, then last is just the last root box.
        last = curr ? curr->prevRootBox() : lastRootBox();
    }

    unsigned numCleanFloats = 0;
    if (!layoutState.floats().isEmpty()) {
        LayoutUnit savedLogicalHeight = logicalHeight();
        // Restore floats from clean lines.
        RootInlineBox* line = firstRootBox();
        while (line != curr) {
            if (Vector<LayoutBox*>* cleanLineFloats = line->floatsPtr()) {
                for (auto* box : *cleanLineFloats) {
                    FloatingObject* floatingObject = insertFloatingObject(*box);
                    ASSERT(!floatingObject->originatingLine());
                    floatingObject->setOriginatingLine(line);
                    setLogicalHeight(logicalTopForChild(*box) - marginBeforeForChild(*box));
                    positionNewFloats();
                    ASSERT(layoutState.floats()[numCleanFloats].object == box);
                    numCleanFloats++;
                }
            }
            line = line->nextRootBox();
        }
        setLogicalHeight(savedLogicalHeight);
    }
    layoutState.setFloatIndex(numCleanFloats);

    layoutState.lineInfo().setFirstLine(!last);
    layoutState.lineInfo().setPreviousLineBrokeCleanly(!last || last->endsWithBreak());

    if (last) {
        setLogicalHeight(last->lineBottomWithLeading());
        InlineIterator iter = InlineIterator(LineLayoutBlockFlow(this), LineLayoutItem(last->lineBreakObj()), last->lineBreakPos());
        resolver.setPosition(iter, numberOfIsolateAncestors(iter));
        resolver.setStatus(last->lineBreakBidiStatus());
    } else {
        TextDirection direction = style()->direction();
        if (style()->unicodeBidi() == Plaintext)
            direction = determinePlaintextDirectionality(this);
        resolver.setStatus(BidiStatus(direction, isOverride(style()->unicodeBidi())));
        InlineIterator iter = InlineIterator(LineLayoutBlockFlow(this), bidiFirstSkippingEmptyInlines(LineLayoutBlockFlow(this), resolver.runs(), &resolver), 0);
        resolver.setPosition(iter, numberOfIsolateAncestors(iter));
    }
    return curr;
}


bool LayoutBlockFlow::lineBoxHasBRWithClearance(RootInlineBox* curr)
{
    // If the linebox breaks cleanly and with clearance then dirty from at least this point onwards so that we can clear the correct floats without difficulty.
    if (!curr->endsWithBreak())
        return false;
    InlineBox* lastBox = style()->isLeftToRightDirection() ? curr->lastLeafChild() : curr->firstLeafChild();
    return lastBox && lastBox->layoutObject().isBR() && lastBox->layoutObject().style()->clear() != CNONE;
}

void LayoutBlockFlow::determineEndPosition(LineLayoutState& layoutState, RootInlineBox* startLine, InlineIterator& cleanLineStart, BidiStatus& cleanLineBidiStatus)
{
    ASSERT(!layoutState.endLine());
    size_t floatIndex = layoutState.floatIndex();
    RootInlineBox* last = nullptr;
    for (RootInlineBox* curr = startLine->nextRootBox(); curr; curr = curr->nextRootBox()) {
        if (!curr->isDirty()) {
            bool encounteredNewFloat = false;
            bool dirtiedByFloat = false;
            checkFloatsInCleanLine(curr, layoutState.floats(), floatIndex, encounteredNewFloat, dirtiedByFloat);
            if (encounteredNewFloat)
                return;
            if (lineBoxHasBRWithClearance(curr))
                return;
        }
        if (curr->isDirty())
            last = nullptr;
        else if (!last)
            last = curr;
    }

    if (!last)
        return;

    // At this point, |last| is the first line in a run of clean lines that ends with the last line
    // in the block.

    RootInlineBox* prev = last->prevRootBox();
    cleanLineStart = InlineIterator(LineLayoutItem(this), LineLayoutItem(prev->lineBreakObj()), prev->lineBreakPos());
    cleanLineBidiStatus = prev->lineBreakBidiStatus();
    layoutState.setEndLineLogicalTop(prev->lineBottomWithLeading());

    for (RootInlineBox* line = last; line; line = line->nextRootBox())
        line->extractLine(); // Disconnect all line boxes from their layout objects while preserving
            // their connections to one another.

    layoutState.setEndLine(last);
}

bool LayoutBlockFlow::checkPaginationAndFloatsAtEndLine(LineLayoutState& layoutState)
{
    LayoutUnit lineDelta = logicalHeight() - layoutState.endLineLogicalTop();

    bool paginated = view()->layoutState() && view()->layoutState()->isPaginated();
    if (paginated && layoutState.flowThread()) {
        // Check all lines from here to the end, and see if the hypothetical new position for the lines will result
        // in a different available line width.
        for (RootInlineBox* lineBox = layoutState.endLine(); lineBox; lineBox = lineBox->nextRootBox()) {
            if (paginated) {
                // This isn't the real move we're going to do, so don't update the line box's pagination
                // strut yet.
                LayoutUnit oldPaginationStrut = lineBox->paginationStrut();
                lineDelta -= oldPaginationStrut;
                adjustLinePositionForPagination(*lineBox, lineDelta);
                lineBox->setPaginationStrut(oldPaginationStrut);
            }
        }
    }

    if (!lineDelta || !m_floatingObjects)
        return true;

    // See if any floats end in the range along which we want to shift the lines vertically.
    LayoutUnit logicalTop = std::min(logicalHeight(), layoutState.endLineLogicalTop());

    RootInlineBox* lastLine = layoutState.endLine();
    while (RootInlineBox* nextLine = lastLine->nextRootBox())
        lastLine = nextLine;

    LayoutUnit logicalBottom = lastLine->lineBottomWithLeading() + absoluteValue(lineDelta);

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    FloatingObjectSetIterator end = floatingObjectSet.end();
    for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
        const FloatingObject& floatingObject = *it->get();
        if (logicalBottomForFloat(floatingObject) >= logicalTop && logicalBottomForFloat(floatingObject) < logicalBottom)
            return false;
    }

    return true;
}

bool LayoutBlockFlow::matchedEndLine(LineLayoutState& layoutState, const InlineBidiResolver& resolver, const InlineIterator& endLineStart, const BidiStatus& endLineStatus)
{
    if (resolver.position() == endLineStart) {
        if (resolver.status() != endLineStatus)
            return false;

        return checkPaginationAndFloatsAtEndLine(layoutState);
    }

    // The first clean line doesn't match, but we can check a handful of following lines to try
    // to match back up.
    static int numLines = 8; // The # of lines we're willing to match against.
    RootInlineBox* originalEndLine = layoutState.endLine();
    RootInlineBox* line = originalEndLine;
    for (int i = 0; i < numLines && line; i++, line = line->nextRootBox()) {
        if (line->lineBreakObj() == resolver.position().object() && line->lineBreakPos() == resolver.position().offset()) {
            // We have a match.
            if (line->lineBreakBidiStatus() != resolver.status())
                return false; // ...but the bidi state doesn't match.

            bool matched = false;
            RootInlineBox* result = line->nextRootBox();
            layoutState.setEndLine(result);
            if (result) {
                layoutState.setEndLineLogicalTop(line->lineBottomWithLeading());
                matched = checkPaginationAndFloatsAtEndLine(layoutState);
            }

            // Now delete the lines that we failed to sync.
            deleteLineRange(layoutState, originalEndLine, result);
            return matched;
        }
    }

    return false;
}

bool LayoutBlockFlow::generatesLineBoxesForInlineChild(LayoutObject* inlineObj)

{
    ASSERT(inlineObj->parent() == this);

    InlineIterator it(LineLayoutBlockFlow(this), LineLayoutItem(inlineObj), 0);
    // FIXME: We should pass correct value for WhitespacePosition.
    while (!it.atEnd() && !requiresLineBox(it))
        it.increment();

    return !it.atEnd();
}


void LayoutBlockFlow::addOverflowFromInlineChildren()
{
    LayoutUnit endPadding = hasOverflowClip() ? paddingEnd() : LayoutUnit();
    // FIXME: Need to find another way to do this, since scrollbars could show when we don't want them to.
    if (hasOverflowClip() && !endPadding && node() && node()->isRootEditableElement() && style()->isLeftToRightDirection())
        endPadding = 1;
    for (RootInlineBox* curr = firstRootBox(); curr; curr = curr->nextRootBox()) {
        addLayoutOverflow(curr->paddedLayoutOverflowRect(endPadding));
        LayoutRect visualOverflow = curr->visualOverflowRect(curr->lineTop(), curr->lineBottom());
        addContentsVisualOverflow(visualOverflow);
    }
}

void LayoutBlockFlow::deleteEllipsisLineBoxes()
{
    ETextAlign textAlign = style()->textAlign();
    bool firstLine = true;
    for (RootInlineBox* curr = firstRootBox(); curr; curr = curr->nextRootBox()) {
        if (curr->hasEllipsisBox()) {
            curr->clearTruncation();

            // Shift the line back where it belongs if we cannot accomodate an ellipsis.
            LayoutUnit logicalLeft = logicalLeftOffsetForLine(curr->lineTop(), firstLine);
            LayoutUnit availableLogicalWidth = logicalRightOffsetForLine(curr->lineTop(), false) - logicalLeft;
            LayoutUnit totalLogicalWidth = curr->logicalWidth();
            updateLogicalWidthForAlignment(textAlign, curr, 0, logicalLeft, totalLogicalWidth, availableLogicalWidth, 0);

            curr->moveInInlineDirection(logicalLeft - curr->logicalLeft());
        }
        firstLine = false;
    }
}

void LayoutBlockFlow::checkLinesForTextOverflow()
{
    // Determine the width of the ellipsis using the current font.
    const Font& font = style()->font();

    const size_t fullStopStringLength = 3;
    const UChar fullStopString[] = {fullstopCharacter, fullstopCharacter, fullstopCharacter};
    DEFINE_STATIC_LOCAL(AtomicString, fullstopCharacterStr, (fullStopString, fullStopStringLength));
    DEFINE_STATIC_LOCAL(AtomicString, ellipsisStr, (&horizontalEllipsisCharacter, 1));
    AtomicString& selectedEllipsisStr = ellipsisStr;

    const Font& firstLineFont = firstLineStyle()->font();
    // FIXME: We should probably not hard-code the direction here. https://crbug.com/333004
    TextDirection ellipsisDirection = LTR;
    float firstLineEllipsisWidth = 0;
    float ellipsisWidth = 0;

    // As per CSS3 http://www.w3.org/TR/2003/CR-css3-text-20030514/ sequence of three
    // Full Stops (002E) can be used.
    ASSERT(firstLineFont.primaryFont());
    if (firstLineFont.primaryFont()->glyphForCharacter(horizontalEllipsisCharacter)) {
        firstLineEllipsisWidth = firstLineFont.width(constructTextRun(this, firstLineFont, &horizontalEllipsisCharacter, 1, *firstLineStyle(), ellipsisDirection));
    } else {
        selectedEllipsisStr = fullstopCharacterStr;
        firstLineEllipsisWidth = firstLineFont.width(constructTextRun(this, firstLineFont, fullStopString, fullStopStringLength, *firstLineStyle(), ellipsisDirection));
    }
    ellipsisWidth = (font == firstLineFont) ? firstLineEllipsisWidth : 0;

    if (!ellipsisWidth) {
        ASSERT(font.primaryFont());
        if (font.primaryFont()->glyphForCharacter(horizontalEllipsisCharacter)) {
            selectedEllipsisStr = ellipsisStr;
            ellipsisWidth = font.width(constructTextRun(this, font, &horizontalEllipsisCharacter, 1, styleRef(), ellipsisDirection));
        } else {
            selectedEllipsisStr = fullstopCharacterStr;
            ellipsisWidth = font.width(constructTextRun(this, font, fullStopString, fullStopStringLength, styleRef(), ellipsisDirection));
        }
    }

    // For LTR text truncation, we want to get the right edge of our padding box, and then we want to see
    // if the right edge of a line box exceeds that.  For RTL, we use the left edge of the padding box and
    // check the left edge of the line box to see if it is less
    // Include the scrollbar for overflow blocks, which means we want to use "contentWidth()"
    bool ltr = style()->isLeftToRightDirection();
    ETextAlign textAlign = style()->textAlign();
    bool firstLine = true;
    for (RootInlineBox* curr = firstRootBox(); curr; curr = curr->nextRootBox()) {
        LayoutUnit currLogicalLeft = curr->logicalLeft();
        LayoutUnit blockRightEdge = logicalRightOffsetForLine(curr->lineTop(), firstLine);
        LayoutUnit blockLeftEdge = logicalLeftOffsetForLine(curr->lineTop(), firstLine);
        LayoutUnit lineBoxEdge = ltr ? currLogicalLeft + curr->logicalWidth() : currLogicalLeft;
        if ((ltr && lineBoxEdge > blockRightEdge) || (!ltr && lineBoxEdge < blockLeftEdge)) {
            // This line spills out of our box in the appropriate direction.  Now we need to see if the line
            // can be truncated.  In order for truncation to be possible, the line must have sufficient space to
            // accommodate our truncation string, and no replaced elements (images, tables) can overlap the ellipsis
            // space.

            LayoutUnit width = firstLine ? firstLineEllipsisWidth : ellipsisWidth;
            LayoutUnit blockEdge = ltr ? blockRightEdge : blockLeftEdge;
            if (curr->lineCanAccommodateEllipsis(ltr, blockEdge, lineBoxEdge, width)) {
                LayoutUnit totalLogicalWidth = curr->placeEllipsis(selectedEllipsisStr, ltr, blockLeftEdge, blockRightEdge, width);
                LayoutUnit logicalLeft; // We are only interested in the delta from the base position.
                LayoutUnit availableLogicalWidth = blockRightEdge - blockLeftEdge;
                updateLogicalWidthForAlignment(textAlign, curr, 0, logicalLeft, totalLogicalWidth, availableLogicalWidth, 0);
                if (ltr)
                    curr->moveInInlineDirection(logicalLeft);
                else
                    curr->moveInInlineDirection(logicalLeft - (availableLogicalWidth - totalLogicalWidth));
            }
        }
        firstLine = false;
    }
}

bool LayoutBlockFlow::positionNewFloatOnLine(FloatingObject& newFloat, FloatingObject* lastFloatFromPreviousLine, LineInfo& lineInfo, LineWidth& width)
{
    if (!positionNewFloats(&width))
        return false;

    // We only connect floats to lines for pagination purposes if the floats occur at the start of
    // the line and the previous line had a hard break (so this line is either the first in the block
    // or follows a <br>).
    if (!newFloat.paginationStrut() || !lineInfo.previousLineBrokeCleanly() || !lineInfo.isEmpty())
        return true;

    const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
    ASSERT(floatingObjectSet.last() == &newFloat);

    LayoutUnit floatLogicalTop = logicalTopForFloat(newFloat);
    int paginationStrut = newFloat.paginationStrut();

    if (floatLogicalTop - paginationStrut != logicalHeight() + lineInfo.floatPaginationStrut())
        return true;

    FloatingObjectSetIterator it = floatingObjectSet.end();
    --it; // Last float is newFloat, skip that one.
    FloatingObjectSetIterator begin = floatingObjectSet.begin();
    while (it != begin) {
        --it;
        FloatingObject& floatingObject = *it->get();
        if (&floatingObject == lastFloatFromPreviousLine)
            break;
        if (logicalTopForFloat(floatingObject) == logicalHeight() + lineInfo.floatPaginationStrut()) {
            floatingObject.setPaginationStrut(paginationStrut + floatingObject.paginationStrut());
            LayoutBox* floatBox = floatingObject.layoutObject();
            setLogicalTopForChild(*floatBox, logicalTopForChild(*floatBox) + marginBeforeForChild(*floatBox) + paginationStrut);
            if (floatBox->isLayoutBlock())
                floatBox->forceChildLayout();
            else
                floatBox->layoutIfNeeded();
            // Save the old logical top before calling removePlacedObject which will set
            // isPlaced to false. Otherwise it will trigger an assert in logicalTopForFloat.
            LayoutUnit oldLogicalTop = logicalTopForFloat(floatingObject);
            m_floatingObjects->removePlacedObject(floatingObject);
            setLogicalTopForFloat(floatingObject, oldLogicalTop + paginationStrut);
            m_floatingObjects->addPlacedObject(floatingObject);
        }
    }

    // Just update the line info's pagination strut without altering our logical height yet. If the line ends up containing
    // no content, then we don't want to improperly grow the height of the block.
    lineInfo.setFloatPaginationStrut(lineInfo.floatPaginationStrut() + paginationStrut);
    return true;
}

LayoutUnit LayoutBlockFlow::startAlignedOffsetForLine(LayoutUnit position, bool firstLine)
{
    ETextAlign textAlign = style()->textAlign();

    if (textAlign == TASTART) // FIXME: Handle TAEND here
        return startOffsetForLine(position, firstLine);

    // updateLogicalWidthForAlignment() handles the direction of the block so no need to consider it here
    LayoutUnit totalLogicalWidth;
    LayoutUnit logicalLeft = logicalLeftOffsetForLine(logicalHeight(), false);
    LayoutUnit availableLogicalWidth = logicalRightOffsetForLine(logicalHeight(), false) - logicalLeft;
    updateLogicalWidthForAlignment(textAlign, 0, 0, logicalLeft, totalLogicalWidth, availableLogicalWidth, 0);

    if (!style()->isLeftToRightDirection())
        return logicalWidth() - logicalLeft;
    return logicalLeft;
}

}
