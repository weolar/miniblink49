/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All right reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2013 Adobe Systems Incorporated.
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

#ifndef BreakingContextInlineHeaders_h
#define BreakingContextInlineHeaders_h

#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutRubyRun.h"
#include "core/layout/LayoutTextCombine.h"
#include "core/layout/TextRunConstructor.h"
#include "core/layout/api/LineLayoutBox.h"
#include "core/layout/line/InlineIterator.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/layout/line/LayoutTextInfo.h"
#include "core/layout/line/LineBreaker.h"
#include "core/layout/line/LineInfo.h"
#include "core/layout/line/LineWidth.h"
#include "core/layout/line/TrailingObjects.h"
#include "core/layout/line/WordMeasurement.h"
#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/text/TextBreakIterator.h"
#include "wtf/Vector.h"

namespace blink {

// We don't let our line box tree for a single line get any deeper than this.
const unsigned cMaxLineDepth = 200;

class BreakingContext {
public:
    BreakingContext(InlineBidiResolver& resolver, LineInfo& inLineInfo, LineWidth& lineWidth, LayoutTextInfo& inLayoutTextInfo, FloatingObject* inLastFloatFromPreviousLine, bool appliedStartWidth, LineLayoutBlockFlow block)
        : m_resolver(resolver)
        , m_current(resolver.position())
        , m_lineBreak(resolver.position())
        , m_block(block)
        , m_lastObject(m_current.object())
        , m_nextObject(nullptr)
        , m_currentStyle(nullptr)
        , m_blockStyle(block->style())
        , m_lineInfo(inLineInfo)
        , m_layoutTextInfo(inLayoutTextInfo)
        , m_lastFloatFromPreviousLine(inLastFloatFromPreviousLine)
        , m_width(lineWidth)
        , m_currWS(NORMAL)
        , m_lastWS(NORMAL)
        , m_preservesNewline(false)
        , m_atStart(true)
        , m_ignoringSpaces(false)
        , m_currentCharacterIsSpace(false)
        , m_currentCharacterShouldCollapseIfPreWap(false)
        , m_appliedStartWidth(appliedStartWidth)
        , m_includeEndWidth(true)
        , m_autoWrap(false)
        , m_autoWrapWasEverTrueOnLine(false)
        , m_floatsFitOnLine(true)
        , m_collapseWhiteSpace(false)
        , m_startingNewParagraph(m_lineInfo.previousLineBrokeCleanly())
        , m_allowImagesToBreak(!block->document().inQuirksMode() || !block->isTableCell() || !m_blockStyle->logicalWidth().isIntrinsicOrAuto())
        , m_atEnd(false)
        , m_lineMidpointState(resolver.midpointState())
    {
        m_lineInfo.setPreviousLineBrokeCleanly(false);
    }

    LayoutObject* currentObject() { return m_current.object(); }
    InlineIterator lineBreak() { return m_lineBreak; }
    bool atEnd() { return m_atEnd; }

    void initializeForCurrentObject();

    void increment();

    void handleBR(EClear&);
    void handleOutOfFlowPositioned(Vector<LayoutBox*>& positionedObjects);
    void handleFloat();
    void handleEmptyInline();
    void handleReplaced();
    bool handleText(WordMeasurements&, bool& hyphenated);
    void commitAndUpdateLineBreakIfNeeded();
    InlineIterator handleEndOfLine();

    void clearLineBreakIfFitsOnLine()
    {
        if (m_width.fitsOnLine() || m_lastWS == NOWRAP)
            m_lineBreak.clear();
    }

private:
    void skipTrailingWhitespace(InlineIterator&, const LineInfo&);

    InlineBidiResolver& m_resolver;

    InlineIterator m_current;
    InlineIterator m_lineBreak;
    InlineIterator m_startOfIgnoredSpaces;

    LineLayoutBlockFlow m_block;
    LayoutObject* m_lastObject;
    LayoutObject* m_nextObject;

    const ComputedStyle* m_currentStyle;
    const ComputedStyle* m_blockStyle;

    LineInfo& m_lineInfo;

    LayoutTextInfo& m_layoutTextInfo;

    FloatingObject* m_lastFloatFromPreviousLine;

    LineWidth m_width;

    EWhiteSpace m_currWS;
    EWhiteSpace m_lastWS;

    bool m_preservesNewline;
    bool m_atStart;
    bool m_ignoringSpaces;
    bool m_currentCharacterIsSpace;
    bool m_currentCharacterShouldCollapseIfPreWap;
    bool m_appliedStartWidth;
    bool m_includeEndWidth;
    bool m_autoWrap;
    bool m_autoWrapWasEverTrueOnLine;
    bool m_floatsFitOnLine;
    bool m_collapseWhiteSpace;
    bool m_startingNewParagraph;
    bool m_allowImagesToBreak;
    bool m_atEnd;

    LineMidpointState& m_lineMidpointState;

    TrailingObjects m_trailingObjects;
};

// When ignoring spaces, this needs to be called for objects that need line boxes such as LayoutInlines or
// hard line breaks to ensure that they're not ignored.
inline void ensureLineBoxInsideIgnoredSpaces(LineMidpointState* midpointState, LayoutObject* renderer)
{
    InlineIterator midpoint(0, LineLayoutItem(renderer), 0);
    midpointState->stopIgnoringSpaces(midpoint);
    midpointState->startIgnoringSpaces(midpoint);
}

inline bool shouldCollapseWhiteSpace(const ComputedStyle& style, const LineInfo& lineInfo, WhitespacePosition whitespacePosition)
{
    // CSS2 16.6.1
    // If a space (U+0020) at the beginning of a line has 'white-space' set to 'normal', 'nowrap', or 'pre-line', it is removed.
    // If a space (U+0020) at the end of a line has 'white-space' set to 'normal', 'nowrap', or 'pre-line', it is also removed.
    // If spaces (U+0020) or tabs (U+0009) at the end of a line have 'white-space' set to 'pre-wrap', UAs may visually collapse them.
    return style.collapseWhiteSpace()
        || (whitespacePosition == TrailingWhitespace && style.whiteSpace() == PRE_WRAP && (!lineInfo.isEmpty() || !lineInfo.previousLineBrokeCleanly()));
}

inline bool requiresLineBoxForContent(LayoutInline* flow, const LineInfo& lineInfo)
{
    LayoutObject* parent = flow->parent();
    if (flow->document().inNoQuirksMode()
        && (flow->style(lineInfo.isFirstLine())->lineHeight() != parent->style(lineInfo.isFirstLine())->lineHeight()
        || flow->style()->verticalAlign() != parent->style()->verticalAlign()
        || !parent->style()->font().fontMetrics().hasIdenticalAscentDescentAndLineGap(flow->style()->font().fontMetrics())))
        return true;
    return false;
}

inline bool alwaysRequiresLineBox(LayoutObject* flow)
{
    // FIXME: Right now, we only allow line boxes for inlines that are truly empty.
    // We need to fix this, though, because at the very least, inlines containing only
    // ignorable whitespace should should also have line boxes.
    return isEmptyInline(LineLayoutItem(flow)) && toLayoutInline(flow)->hasInlineDirectionBordersPaddingOrMargin();
}

inline bool requiresLineBox(const InlineIterator& it, const LineInfo& lineInfo = LineInfo(), WhitespacePosition whitespacePosition = LeadingWhitespace)
{
    if (it.object()->isFloatingOrOutOfFlowPositioned())
        return false;

    if (it.object()->isLayoutInline() && !alwaysRequiresLineBox(it.object()) && !requiresLineBoxForContent(toLayoutInline(it.object()), lineInfo))
        return false;

    if (!shouldCollapseWhiteSpace(it.object()->styleRef(), lineInfo, whitespacePosition) || it.object()->isBR())
        return true;

    UChar current = it.current();
    bool notJustWhitespace = current != spaceCharacter && current != tabulationCharacter && current != softHyphenCharacter && (current != newlineCharacter || it.object()->preservesNewline());
    return notJustWhitespace || isEmptyInline(LineLayoutItem(it.object()));
}

inline void setStaticPositions(LineLayoutBlockFlow block, LayoutBox* child)
{
    ASSERT(child->isOutOfFlowPositioned());
    // FIXME: The math here is actually not really right. It's a best-guess approximation that
    // will work for the common cases
    LayoutObject* containerBlock = child->container();
    LayoutUnit blockHeight = block.logicalHeight();
    if (containerBlock->isLayoutInline()) {
        // A relative positioned inline encloses us. In this case, we also have to determine our
        // position as though we were an inline. Set |staticInlinePosition| and |staticBlockPosition| on the relative positioned
        // inline so that we can obtain the value later.
        toLayoutInline(containerBlock)->layer()->setStaticInlinePosition(block.startAlignedOffsetForLine(blockHeight, false));
        toLayoutInline(containerBlock)->layer()->setStaticBlockPosition(blockHeight);

        // If |child| is a leading or trailing positioned object this is its only opportunity to ensure it moves with an inline
        // container changing width.
        child->moveWithEdgeOfInlineContainerIfNecessary(child->isHorizontalWritingMode());
    }
    block.updateStaticInlinePositionForChild(*child, blockHeight);
    child->layer()->setStaticBlockPosition(blockHeight);
}

// FIXME: The entire concept of the skipTrailingWhitespace function is flawed, since we really need to be building
// line boxes even for containers that may ultimately collapse away. Otherwise we'll never get positioned
// elements quite right. In other words, we need to build this function's work into the normal line
// object iteration process.
// NB. this function will insert any floating elements that would otherwise
// be skipped but it will not position them.
inline void BreakingContext::skipTrailingWhitespace(InlineIterator& iterator, const LineInfo& lineInfo)
{
    while (!iterator.atEnd() && !requiresLineBox(iterator, lineInfo, TrailingWhitespace)) {
        LayoutObject* object = iterator.object();
        if (object->isOutOfFlowPositioned())
            setStaticPositions(m_block, toLayoutBox(object));
        else if (object->isFloating())
            m_block.insertFloatingObject(*toLayoutBox(object));
        iterator.increment();
    }
}

inline void BreakingContext::initializeForCurrentObject()
{
    m_currentStyle = m_current.object()->style();
    m_nextObject = bidiNextSkippingEmptyInlines(m_block, LineLayoutItem(m_current.object()));
    if (m_nextObject && m_nextObject->parent() && !m_nextObject->parent()->isDescendantOf(m_current.object()->parent()))
        m_includeEndWidth = true;

    m_currWS = m_current.object()->isReplaced() ? m_current.object()->parent()->style()->whiteSpace() : m_currentStyle->whiteSpace();
    m_lastWS = m_lastObject->isReplaced() ? m_lastObject->parent()->style()->whiteSpace() : m_lastObject->style()->whiteSpace();

    bool isSVGText = m_current.object()->isSVGInlineText();
    m_autoWrap = !isSVGText && ComputedStyle::autoWrap(m_currWS);
    m_autoWrapWasEverTrueOnLine = m_autoWrapWasEverTrueOnLine || m_autoWrap;

    m_preservesNewline = !isSVGText && ComputedStyle::preserveNewline(m_currWS);

    m_collapseWhiteSpace = ComputedStyle::collapseWhiteSpace(m_currWS);
}

inline void BreakingContext::increment()
{
    // Clear out our character space bool, since inline <pre>s don't collapse whitespace
    // with adjacent inline normal/nowrap spans.
    if (!m_collapseWhiteSpace)
        m_currentCharacterIsSpace = false;

    m_current.moveToStartOf(LineLayoutItem(m_nextObject));
    m_atStart = false;
}

inline void BreakingContext::handleBR(EClear& clear)
{
    if (m_width.fitsOnLine()) {
        LayoutObject* br = m_current.object();
        m_lineBreak.moveToStartOf(LineLayoutItem(br));
        m_lineBreak.increment();

        // A <br> always breaks a line, so don't let the line be collapsed
        // away. Also, the space at the end of a line with a <br> does not
        // get collapsed away. It only does this if the previous line broke
        // cleanly. Otherwise the <br> has no effect on whether the line is
        // empty or not.
        if (m_startingNewParagraph)
            m_lineInfo.setEmpty(false, m_block, &m_width);
        m_trailingObjects.clear();
        m_lineInfo.setPreviousLineBrokeCleanly(true);

        // A <br> with clearance always needs a linebox in case the lines below it get dirtied later and
        // need to check for floats to clear - so if we're ignoring spaces, stop ignoring them and add a
        // run for this object.
        if (m_ignoringSpaces && m_currentStyle->clear() != CNONE)
            ensureLineBoxInsideIgnoredSpaces(&m_lineMidpointState, br);

        if (!m_lineInfo.isEmpty())
            clear = m_currentStyle->clear();
    }
    m_atEnd = true;
}

inline LayoutUnit borderPaddingMarginStart(LayoutInline* child)
{
    return child->marginStart() + child->paddingStart() + child->borderStart();
}

inline LayoutUnit borderPaddingMarginEnd(LayoutInline* child)
{
    return child->marginEnd() + child->paddingEnd() + child->borderEnd();
}

inline bool shouldAddBorderPaddingMargin(LayoutObject* child, bool &checkSide)
{
    if (!child || (child->isText() && !toLayoutText(child)->textLength()))
        return true;
    checkSide = false;
    return checkSide;
}

inline LayoutUnit inlineLogicalWidth(LayoutObject* child, bool start = true, bool end = true)
{
    unsigned lineDepth = 1;
    LayoutUnit extraWidth = 0;
    LayoutObject* parent = child->parent();
    while (parent->isLayoutInline() && lineDepth++ < cMaxLineDepth) {
        LayoutInline* parentAsLayoutInline = toLayoutInline(parent);
        if (!isEmptyInline(LineLayoutItem(parentAsLayoutInline))) {
            if (start && shouldAddBorderPaddingMargin(child->previousSibling(), start))
                extraWidth += borderPaddingMarginStart(parentAsLayoutInline);
            if (end && shouldAddBorderPaddingMargin(child->nextSibling(), end))
                extraWidth += borderPaddingMarginEnd(parentAsLayoutInline);
            if (!start && !end)
                return extraWidth;
        }
        child = parent;
        parent = child->parent();
    }
    return extraWidth;
}

inline void BreakingContext::handleOutOfFlowPositioned(Vector<LayoutBox*>& positionedObjects)
{
    // If our original display wasn't an inline type, then we can
    // go ahead and determine our static inline position now.
    LayoutBox* box = toLayoutBox(m_current.object());
    bool isInlineType = box->style()->isOriginalDisplayInlineType();
    if (!isInlineType) {
        m_block.setStaticInlinePositionForChild(*box, m_block.startOffsetForContent());
    } else {
        // If our original display was an INLINE type, then we can go ahead
        // and determine our static y position now.
        box->layer()->setStaticBlockPosition(m_block.logicalHeight());
    }

    // If we're ignoring spaces, we have to stop and include this object and
    // then start ignoring spaces again.
    if (isInlineType || box->container()->isLayoutInline()) {
        if (m_ignoringSpaces)
            ensureLineBoxInsideIgnoredSpaces(&m_lineMidpointState, box);
        m_trailingObjects.appendObjectIfNeeded(box);
    } else {
        positionedObjects.append(box);
    }
    m_width.addUncommittedWidth(inlineLogicalWidth(box).toFloat());
    // Reset prior line break context characters.
    m_layoutTextInfo.m_lineBreakIterator.resetPriorContext();
}

inline void BreakingContext::handleFloat()
{
    LayoutBox* floatBox = toLayoutBox(m_current.object());
    FloatingObject* floatingObject = m_block.insertFloatingObject(*floatBox);
    // check if it fits in the current line.
    // If it does, position it now, otherwise, position
    // it after moving to next line (in newLine() func)
    // FIXME: Bug 110372: Properly position multiple stacked floats with non-rectangular shape outside.
    if (m_floatsFitOnLine && m_width.fitsOnLine(m_block.logicalWidthForFloat(*floatingObject).toFloat(), ExcludeWhitespace)) {
        m_block.positionNewFloatOnLine(*floatingObject, m_lastFloatFromPreviousLine, m_lineInfo, m_width);
        if (m_lineBreak.object() == m_current.object()) {
            ASSERT(!m_lineBreak.offset());
            m_lineBreak.increment();
        }
    } else {
        m_floatsFitOnLine = false;
    }
    // Update prior line break context characters, using U+FFFD (OBJECT REPLACEMENT CHARACTER) for floating element.
    m_layoutTextInfo.m_lineBreakIterator.updatePriorContext(replacementCharacter);
}

// This is currently just used for list markers and inline flows that have line boxes. Neither should
// have an effect on whitespace at the start of the line.
inline bool shouldSkipWhitespaceAfterStartObject(LineLayoutBlockFlow block, LayoutObject* o, LineMidpointState& lineMidpointState)
{
    LayoutObject* next = bidiNextSkippingEmptyInlines(block, LineLayoutItem(o));
    while (next && next->isFloatingOrOutOfFlowPositioned())
        next = bidiNextSkippingEmptyInlines(block, LineLayoutItem(next));

    if (next && !next->isBR() && next->isText() && toLayoutText(next)->textLength() > 0) {
        LayoutText* nextText = toLayoutText(next);
        UChar nextChar = nextText->characterAt(0);
        if (nextText->style()->isCollapsibleWhiteSpace(nextChar)) {
            lineMidpointState.startIgnoringSpaces(InlineIterator(0, LineLayoutItem(o), 0));
            return true;
        }
    }

    return false;
}

inline void BreakingContext::handleEmptyInline()
{
    // This should only end up being called on empty inlines
    ASSERT(isEmptyInline(LineLayoutItem(m_current.object())));

    LayoutInline* flowBox = toLayoutInline(m_current.object());

    bool requiresLineBox = alwaysRequiresLineBox(m_current.object());
    if (requiresLineBox || requiresLineBoxForContent(flowBox, m_lineInfo)) {
        // An empty inline that only has line-height, vertical-align or font-metrics will
        // not force linebox creation (and thus affect the height of the line) if the rest of the line is empty.
        if (requiresLineBox)
            m_lineInfo.setEmpty(false, m_block, &m_width);
        if (m_ignoringSpaces) {
            // If we are in a run of ignored spaces then ensure we get a linebox if lineboxes are eventually
            // created for the line...
            m_trailingObjects.clear();
            ensureLineBoxInsideIgnoredSpaces(&m_lineMidpointState, m_current.object());
        } else if (m_blockStyle->collapseWhiteSpace() && m_resolver.position().object() == m_current.object()
            && shouldSkipWhitespaceAfterStartObject(m_block, m_current.object(), m_lineMidpointState)) {
            // If this object is at the start of the line, we need to behave like list markers and
            // start ignoring spaces.
            m_currentCharacterShouldCollapseIfPreWap = m_currentCharacterIsSpace = true;
            m_ignoringSpaces = true;
        } else {
            // If we are after a trailing space but aren't ignoring spaces yet then ensure we get a linebox
            // if we encounter collapsible whitepace.
            m_trailingObjects.appendObjectIfNeeded(m_current.object());
        }
    }

    m_width.addUncommittedWidth((inlineLogicalWidth(m_current.object()) + borderPaddingMarginStart(flowBox) + borderPaddingMarginEnd(flowBox)).toFloat());
}

inline void BreakingContext::handleReplaced()
{
    LayoutBox* replacedBox = toLayoutBox(m_current.object());

    if (m_atStart)
        m_width.updateAvailableWidth(replacedBox->logicalHeight());

    // Break on replaced elements if either has normal white-space.
    if ((m_autoWrap || ComputedStyle::autoWrap(m_lastWS)) && (!m_current.object()->isImage() || m_allowImagesToBreak)) {
        m_width.commit();
        m_lineBreak.moveToStartOf(m_current.object());
    }

    if (m_ignoringSpaces)
        m_lineMidpointState.stopIgnoringSpaces(InlineIterator(0, m_current.object(), 0));

    m_lineInfo.setEmpty(false, m_block, &m_width);
    m_ignoringSpaces = false;
    m_currentCharacterShouldCollapseIfPreWap = m_currentCharacterIsSpace = false;
    m_trailingObjects.clear();

    // Optimize for a common case. If we can't find whitespace after the list
    // item, then this is all moot.
    LayoutUnit replacedLogicalWidth = m_block.logicalWidthForChild(*replacedBox) + m_block.marginStartForChild(*replacedBox) + m_block.marginEndForChild(*replacedBox) + inlineLogicalWidth(m_current.object());
    if (m_current.object()->isListMarker()) {
        if (m_blockStyle->collapseWhiteSpace() && shouldSkipWhitespaceAfterStartObject(m_block, m_current.object(), m_lineMidpointState)) {
            // Like with inline flows, we start ignoring spaces to make sure that any
            // additional spaces we see will be discarded.
            m_currentCharacterShouldCollapseIfPreWap = m_currentCharacterIsSpace = true;
            m_ignoringSpaces = true;
        }
        if (toLayoutListMarker(m_current.object())->isInside())
            m_width.addUncommittedWidth(replacedLogicalWidth.toFloat());
    } else {
        m_width.addUncommittedWidth(replacedLogicalWidth.toFloat());
    }
    if (m_current.object()->isRubyRun())
        m_width.applyOverhang(toLayoutRubyRun(m_current.object()), m_lastObject, m_nextObject);
    // Update prior line break context characters, using U+FFFD (OBJECT REPLACEMENT CHARACTER) for replaced element.
    m_layoutTextInfo.m_lineBreakIterator.updatePriorContext(replacementCharacter);
}

inline void nextCharacter(UChar& currentCharacter, UChar& lastCharacter, UChar& secondToLastCharacter)
{
    secondToLastCharacter = lastCharacter;
    lastCharacter = currentCharacter;
}

inline float firstPositiveWidth(const WordMeasurements& wordMeasurements)
{
    for (size_t i = 0; i < wordMeasurements.size(); ++i) {
        if (wordMeasurements[i].width > 0)
            return wordMeasurements[i].width;
    }
    return 0;
}

ALWAYS_INLINE TextDirection textDirectionFromUnicode(WTF::Unicode::Direction direction)
{
    return direction == WTF::Unicode::RightToLeft
        || direction == WTF::Unicode::RightToLeftArabic ? RTL : LTR;
}

ALWAYS_INLINE float textWidth(LayoutText* text, unsigned from, unsigned len, const Font& font, float xPos, bool collapseWhiteSpace, HashSet<const SimpleFontData*>* fallbackFonts = nullptr, FloatRect* glyphBounds = nullptr)
{
    if ((!from && len == text->textLength()) || text->style()->hasTextCombine())
        return text->width(from, len, font, xPos, text->style()->direction(), fallbackFonts, glyphBounds);

    TextRun run = constructTextRun(text, font, text, from, len, text->styleRef());
    run.setCodePath(text->canUseSimpleFontCodePath() ? TextRun::ForceSimple : TextRun::ForceComplex);
    run.setTabSize(!collapseWhiteSpace, text->style()->tabSize());
    run.setXPos(xPos);
    return font.width(run, fallbackFonts, glyphBounds);
}

inline bool BreakingContext::handleText(WordMeasurements& wordMeasurements, bool& hyphenated)
{
    if (!m_current.offset())
        m_appliedStartWidth = false;

    LayoutText* layoutText = toLayoutText(m_current.object());

    bool isSVGText = layoutText->isSVGInlineText();

    // If we have left a no-wrap inline and entered an autowrap inline while ignoring spaces
    // then we need to mark the start of the autowrap inline as a potential linebreak now.
    if (m_autoWrap && !ComputedStyle::autoWrap(m_lastWS) && m_ignoringSpaces) {
        m_width.commit();
        m_lineBreak.moveToStartOf(m_current.object());
    }

    const ComputedStyle& style = layoutText->styleRef(m_lineInfo.isFirstLine());
    const Font& font = style.font();

    unsigned lastSpace = m_current.offset();
    float wordSpacing = m_currentStyle->wordSpacing();
    float lastSpaceWordSpacing = 0;
    float wordSpacingForWordMeasurement = 0;

    float wrapW = m_width.uncommittedWidth() + inlineLogicalWidth(m_current.object(), !m_appliedStartWidth, true);
    float charWidth = 0;
    // Auto-wrapping text should wrap in the middle of a word only if it could not wrap before the word,
    // which is only possible if the word is the first thing on the line, that is, if |w| is zero.
    bool breakWords = m_currentStyle->breakWords() && ((m_autoWrap && !m_width.committedWidth()) || m_currWS == PRE);
    bool midWordBreak = false;
    bool breakAll = m_currentStyle->wordBreak() == BreakAllWordBreak && m_autoWrap;
    bool keepAll = m_currentStyle->wordBreak() == KeepAllWordBreak && m_autoWrap;
    bool prohibitBreakInside = m_currentStyle->hasTextCombine() && layoutText->isCombineText() && toLayoutTextCombine(layoutText)->isCombined();
    float hyphenWidth = 0;

    if (isSVGText) {
        breakWords = false;
        breakAll = false;
        keepAll = false;
    }

    if (layoutText->isWordBreak()) {
        m_width.commit();
        m_lineBreak.moveToStartOf(m_current.object());
        ASSERT(m_current.offset() == layoutText->textLength());
    }

    if (m_layoutTextInfo.m_text != layoutText) {
        m_layoutTextInfo.m_text = layoutText;
        m_layoutTextInfo.m_font = &font;
        m_layoutTextInfo.m_lineBreakIterator.resetStringAndReleaseIterator(layoutText->text(), style.locale());
    } else if (m_layoutTextInfo.m_font != &font) {
        m_layoutTextInfo.m_font = &font;
    }

    // Non-zero only when kerning is enabled, in which case we measure
    // words with their trailing space, then subtract its width.
    float wordTrailingSpaceWidth = (font.fontDescription().typesettingFeatures() & Kerning) ?
        font.width(constructTextRun(layoutText, font, &spaceCharacter, 1, style, style.direction())) + wordSpacing
        : 0;

    UChar lastCharacter = m_layoutTextInfo.m_lineBreakIterator.lastCharacter();
    UChar secondToLastCharacter = m_layoutTextInfo.m_lineBreakIterator.secondToLastCharacter();
    for (; m_current.offset() < layoutText->textLength(); m_current.fastIncrementInTextNode()) {
        bool previousCharacterIsSpace = m_currentCharacterIsSpace;
        bool previousCharacterShouldCollapseIfPreWap = m_currentCharacterShouldCollapseIfPreWap;
        UChar c = m_current.current();
        m_currentCharacterShouldCollapseIfPreWap = m_currentCharacterIsSpace = c == spaceCharacter || c == tabulationCharacter || (!m_preservesNewline && (c == newlineCharacter));

        if (!m_collapseWhiteSpace || !m_currentCharacterIsSpace)
            m_lineInfo.setEmpty(false, m_block, &m_width);

        if (c == softHyphenCharacter && m_autoWrap && !hyphenWidth) {
            hyphenWidth = layoutText->hyphenWidth(font, textDirectionFromUnicode(m_resolver.position().direction()));
            m_width.addUncommittedWidth(hyphenWidth);
        }

        bool applyWordSpacing = false;

        if (breakWords && !midWordBreak) {
            wrapW += charWidth;
            bool midWordBreakIsBeforeSurrogatePair = U16_IS_LEAD(c) && m_current.offset() + 1 < layoutText->textLength() && U16_IS_TRAIL((*layoutText)[m_current.offset() + 1]);
            charWidth = textWidth(layoutText, m_current.offset(), midWordBreakIsBeforeSurrogatePair ? 2 : 1, font, m_width.committedWidth() + wrapW, m_collapseWhiteSpace);
            midWordBreak = m_width.committedWidth() + wrapW + charWidth > m_width.availableWidth();
        }

        int nextBreakablePosition = m_current.nextBreakablePosition();
        bool betweenWords = c == newlineCharacter || (m_currWS != PRE && !m_atStart && m_layoutTextInfo.m_lineBreakIterator.isBreakable(m_current.offset(), nextBreakablePosition, breakAll ? LineBreakType::BreakAll : keepAll ? LineBreakType::KeepAll : LineBreakType::Normal));
        m_current.setNextBreakablePosition(nextBreakablePosition);

        if (betweenWords || midWordBreak) {
            bool stoppedIgnoringSpaces = false;
            if (m_ignoringSpaces) {
                lastSpaceWordSpacing = 0;
                if (!m_currentCharacterIsSpace) {
                    // Stop ignoring spaces and begin at this
                    // new point.
                    m_ignoringSpaces = false;
                    wordSpacingForWordMeasurement = 0;
                    lastSpace = m_current.offset(); // e.g., "Foo    goo", don't add in any of the ignored spaces.
                    m_lineMidpointState.stopIgnoringSpaces(InlineIterator(0, m_current.object(), m_current.offset()));
                    stoppedIgnoringSpaces = true;
                } else {
                    // Just keep ignoring these spaces.
                    nextCharacter(c, lastCharacter, secondToLastCharacter);
                    continue;
                }
            }

            wordMeasurements.grow(wordMeasurements.size() + 1);
            WordMeasurement& wordMeasurement = wordMeasurements.last();

            wordMeasurement.layoutText = layoutText;
            wordMeasurement.endOffset = m_current.offset();
            wordMeasurement.startOffset = lastSpace;

            float additionalTempWidth;
            if (wordTrailingSpaceWidth && c == spaceCharacter)
                additionalTempWidth = textWidth(layoutText, lastSpace, m_current.offset() + 1 - lastSpace, font, m_width.currentWidth(), m_collapseWhiteSpace, &wordMeasurement.fallbackFonts, &wordMeasurement.glyphBounds) - wordTrailingSpaceWidth;
            else
                additionalTempWidth = textWidth(layoutText, lastSpace, m_current.offset() - lastSpace, font, m_width.currentWidth(), m_collapseWhiteSpace, &wordMeasurement.fallbackFonts, &wordMeasurement.glyphBounds);

            wordMeasurement.width = additionalTempWidth + wordSpacingForWordMeasurement;
            wordMeasurement.glyphBounds.move(wordSpacingForWordMeasurement, 0);
            additionalTempWidth += lastSpaceWordSpacing;
            m_width.addUncommittedWidth(additionalTempWidth);

            if (m_collapseWhiteSpace && previousCharacterIsSpace && m_currentCharacterIsSpace && additionalTempWidth)
                m_width.setTrailingWhitespaceWidth(additionalTempWidth);

            if (!m_appliedStartWidth) {
                m_width.addUncommittedWidth(inlineLogicalWidth(m_current.object(), true, false).toFloat());
                m_appliedStartWidth = true;
            }

            applyWordSpacing = wordSpacing && m_currentCharacterIsSpace;

            if (!m_width.committedWidth() && m_autoWrap && !m_width.fitsOnLine())
                m_width.fitBelowFloats(m_lineInfo.isFirstLine());

            if (m_autoWrap || breakWords) {
                // If we break only after white-space, consider the current character
                // as candidate width for this line.
                bool lineWasTooWide = false;
                if (m_width.fitsOnLine() && m_currentCharacterIsSpace && m_currentStyle->breakOnlyAfterWhiteSpace() && !midWordBreak) {
                    float charWidth = textWidth(layoutText, m_current.offset(), 1, font, m_width.currentWidth(), m_collapseWhiteSpace, &wordMeasurement.fallbackFonts, &wordMeasurement.glyphBounds) + (applyWordSpacing ? wordSpacing : 0);
                    // Check if line is too big even without the extra space
                    // at the end of the line. If it is not, do nothing.
                    // If the line needs the extra whitespace to be too long,
                    // then move the line break to the space and skip all
                    // additional whitespace.
                    if (!m_width.fitsOnLine(charWidth)) {
                        lineWasTooWide = true;
                        m_lineBreak.moveTo(m_current.object(), m_current.offset(), m_current.nextBreakablePosition());
                        skipTrailingWhitespace(m_lineBreak, m_lineInfo);
                    }
                }
                if (lineWasTooWide || !m_width.fitsOnLine()) {
                    if (m_lineBreak.atTextParagraphSeparator()) {
                        if (!stoppedIgnoringSpaces && m_current.offset() > 0)
                            m_lineMidpointState.ensureCharacterGetsLineBox(m_current);
                        m_lineBreak.increment();
                        m_lineInfo.setPreviousLineBrokeCleanly(true);
                        wordMeasurement.endOffset = m_lineBreak.offset();
                    }
                    if (m_lineBreak.object() && m_lineBreak.offset() && m_lineBreak.object()->isText() && toLayoutText(m_lineBreak.object())->textLength() && toLayoutText(m_lineBreak.object())->characterAt(m_lineBreak.offset() - 1) == softHyphenCharacter)
                        hyphenated = true;
                    if (m_lineBreak.offset() && m_lineBreak.offset() != (unsigned)wordMeasurement.endOffset && !wordMeasurement.width) {
                        if (charWidth) {
                            wordMeasurement.endOffset = m_lineBreak.offset();
                            wordMeasurement.width = charWidth;
                        }
                    }
                    // Didn't fit. Jump to the end unless there's still an opportunity to collapse whitespace.
                    if (m_ignoringSpaces || !m_collapseWhiteSpace || !m_currentCharacterIsSpace || !previousCharacterIsSpace) {
                        m_atEnd = true;
                        return false;
                    }
                } else {
                    if (!betweenWords || (midWordBreak && !m_autoWrap))
                        m_width.addUncommittedWidth(-additionalTempWidth);
                    if (hyphenWidth) {
                        // Subtract the width of the soft hyphen out since we fit on a line.
                        m_width.addUncommittedWidth(-hyphenWidth);
                        hyphenWidth = 0;
                    }
                }
            }

            if (c == newlineCharacter && m_preservesNewline) {
                if (!stoppedIgnoringSpaces && m_current.offset())
                    m_lineMidpointState.ensureCharacterGetsLineBox(m_current);
                m_lineBreak.moveTo(m_current.object(), m_current.offset(), m_current.nextBreakablePosition());
                m_lineBreak.increment();
                m_lineInfo.setPreviousLineBrokeCleanly(true);
                return true;
            }

            if (m_autoWrap && betweenWords) {
                m_width.commit();
                wrapW = 0;
                m_lineBreak.moveTo(m_current.object(), m_current.offset(), m_current.nextBreakablePosition());
                // Auto-wrapping text should not wrap in the middle of a word once it has had an
                // opportunity to break after a word.
                breakWords = false;
            }

            if (midWordBreak && !U16_IS_TRAIL(c) && !(WTF::Unicode::category(c) & (WTF::Unicode::Mark_NonSpacing | WTF::Unicode::Mark_Enclosing | WTF::Unicode::Mark_SpacingCombining))) {
                // Remember this as a breakable position in case
                // adding the end width forces a break.
                m_lineBreak.moveTo(m_current.object(), m_current.offset(), m_current.nextBreakablePosition());
                midWordBreak &= (breakWords || breakAll);
            }

            if (betweenWords) {
                lastSpaceWordSpacing = applyWordSpacing ? wordSpacing : 0;
                wordSpacingForWordMeasurement = (applyWordSpacing && wordMeasurement.width) ? wordSpacing : 0;
                lastSpace = m_current.offset();
            }

            if (!m_ignoringSpaces && m_currentStyle->collapseWhiteSpace()) {
                // If we encounter a newline, or if we encounter a
                // second space, we need to go ahead and break up this
                // run and enter a mode where we start collapsing spaces.
                if (m_currentCharacterIsSpace && previousCharacterIsSpace) {
                    m_ignoringSpaces = true;

                    // We just entered a mode where we are ignoring
                    // spaces. Create a midpoint to terminate the run
                    // before the second space.
                    m_lineMidpointState.startIgnoringSpaces(m_startOfIgnoredSpaces);
                    m_trailingObjects.updateMidpointsForTrailingObjects(m_lineMidpointState, InlineIterator(), TrailingObjects::DoNotCollapseFirstSpace);
                }
            }
        } else if (m_ignoringSpaces) {
            // Stop ignoring spaces and begin at this
            // new point.
            m_ignoringSpaces = false;
            lastSpaceWordSpacing = applyWordSpacing ? wordSpacing : 0;
            wordSpacingForWordMeasurement = (applyWordSpacing && wordMeasurements.last().width) ? wordSpacing : 0;
            lastSpace = m_current.offset(); // e.g., "Foo    goo", don't add in any of the ignored spaces.
            m_lineMidpointState.stopIgnoringSpaces(InlineIterator(0, m_current.object(), m_current.offset()));
        }

        if (isSVGText && m_current.offset()) {
            // Force creation of new InlineBoxes for each absolute positioned character (those that start new text chunks).
            if (toLayoutSVGInlineText(layoutText)->characterStartsNewTextChunk(m_current.offset()))
                m_lineMidpointState.ensureCharacterGetsLineBox(m_current);
        }

        if (prohibitBreakInside) {
            m_current.setNextBreakablePosition(layoutText->textLength());
            prohibitBreakInside = false;
        }

        if (m_currentCharacterIsSpace && !previousCharacterIsSpace) {
            m_startOfIgnoredSpaces.setObject(m_current.object());
            m_startOfIgnoredSpaces.setOffset(m_current.offset());
        }

        if (!m_currentCharacterIsSpace && previousCharacterShouldCollapseIfPreWap) {
            if (m_autoWrap && m_currentStyle->breakOnlyAfterWhiteSpace())
                m_lineBreak.moveTo(m_current.object(), m_current.offset(), m_current.nextBreakablePosition());
        }

        if (m_collapseWhiteSpace && m_currentCharacterIsSpace && !m_ignoringSpaces)
            m_trailingObjects.setTrailingWhitespace(toLayoutText(m_current.object()));
        else if (!m_currentStyle->collapseWhiteSpace() || !m_currentCharacterIsSpace)
            m_trailingObjects.clear();

        m_atStart = false;
        nextCharacter(c, lastCharacter, secondToLastCharacter);
    }

    m_layoutTextInfo.m_lineBreakIterator.setPriorContext(lastCharacter, secondToLastCharacter);

    wordMeasurements.grow(wordMeasurements.size() + 1);
    WordMeasurement& wordMeasurement = wordMeasurements.last();
    wordMeasurement.layoutText = layoutText;

    // IMPORTANT: current.m_pos is > length here!
    float additionalTempWidth = 0;
    wordMeasurement.startOffset = lastSpace;
    wordMeasurement.endOffset = m_current.offset();
    if (!m_ignoringSpaces) {
        additionalTempWidth = textWidth(layoutText, lastSpace, m_current.offset() - lastSpace, font, m_width.currentWidth(), m_collapseWhiteSpace, &wordMeasurement.fallbackFonts, &wordMeasurement.glyphBounds);
        wordMeasurement.width = additionalTempWidth + wordSpacingForWordMeasurement;
        wordMeasurement.glyphBounds.move(wordSpacingForWordMeasurement, 0);
    }
    additionalTempWidth += lastSpaceWordSpacing;

    LayoutUnit inlineLogicalTempWidth = inlineLogicalWidth(m_current.object(), !m_appliedStartWidth, m_includeEndWidth);
    m_width.addUncommittedWidth(additionalTempWidth + inlineLogicalTempWidth);

    if (m_collapseWhiteSpace && m_currentCharacterIsSpace && additionalTempWidth)
        m_width.setTrailingWhitespaceWidth(additionalTempWidth + inlineLogicalTempWidth);

    m_includeEndWidth = false;

    if (!m_width.fitsOnLine()) {
        if (!hyphenated && m_lineBreak.previousInSameNode() == softHyphenCharacter) {
            hyphenated = true;
            m_atEnd = true;
        }
    }
    return false;
}

inline void BreakingContext::commitAndUpdateLineBreakIfNeeded()
{
    bool checkForBreak = m_autoWrap;
    if (m_width.committedWidth() && !m_width.fitsOnLine() && m_lineBreak.object() && m_currWS == NOWRAP) {
        checkForBreak = true;
    } else if (m_nextObject && m_current.object()->isText() && m_nextObject->isText() && !m_nextObject->isBR() && (m_autoWrap || m_nextObject->style()->autoWrap())) {
        if (m_autoWrap && m_currentCharacterIsSpace) {
            checkForBreak = true;
        } else {
            LayoutText* nextText = toLayoutText(m_nextObject);
            if (nextText->textLength()) {
                UChar c = nextText->characterAt(0);
                // If the next item on the line is text, and if we did not end with
                // a space, then the next text run continues our word (and so it needs to
                // keep adding to the uncommitted width. Just update and continue.
                checkForBreak = !m_currentCharacterIsSpace && (c == spaceCharacter || c == tabulationCharacter || (c == newlineCharacter && !m_nextObject->preservesNewline()));
            } else if (nextText->isWordBreak()) {
                checkForBreak = true;
            }

            if (!m_width.fitsOnLine() && !m_width.committedWidth())
                m_width.fitBelowFloats(m_lineInfo.isFirstLine());

            bool canPlaceOnLine = m_width.fitsOnLine() || !m_autoWrapWasEverTrueOnLine;
            if (canPlaceOnLine && checkForBreak) {
                m_width.commit();
                m_lineBreak.moveToStartOf(LineLayoutItem(m_nextObject));
            }
        }
    }

    ASSERT_WITH_SECURITY_IMPLICATION(m_currentStyle->refCount() > 0);
    if (checkForBreak && !m_width.fitsOnLine()) {
        // if we have floats, try to get below them.
        if (m_currentCharacterIsSpace && !m_ignoringSpaces && m_currentStyle->collapseWhiteSpace())
            m_trailingObjects.clear();

        if (m_width.committedWidth()) {
            m_atEnd = true;
            return;
        }

        m_width.fitBelowFloats(m_lineInfo.isFirstLine());

        // |width| may have been adjusted because we got shoved down past a float (thus
        // giving us more room), so we need to retest, and only jump to
        // the end label if we still don't fit on the line. -dwh
        if (!m_width.fitsOnLine()) {
            m_atEnd = true;
            return;
        }
    } else if (m_blockStyle->autoWrap() && !m_width.fitsOnLine() && !m_width.committedWidth()) {
        // If the container autowraps but the current child does not then we still need to ensure that it
        // wraps and moves below any floats.
        m_width.fitBelowFloats(m_lineInfo.isFirstLine());
    }

    if (!m_current.object()->isFloatingOrOutOfFlowPositioned()) {
        m_lastObject = m_current.object();
        if (m_lastObject->isReplaced() && m_autoWrap && (!m_lastObject->isImage() || m_allowImagesToBreak) && (!m_lastObject->isListMarker() || toLayoutListMarker(m_lastObject)->isInside())) {
            m_width.commit();
            m_lineBreak.moveToStartOf(LineLayoutItem(m_nextObject));
        }
    }
}

inline IndentTextOrNot requiresIndent(bool isFirstLine, bool isAfterHardLineBreak, const ComputedStyle& style)
{
    IndentTextOrNot shouldIndentText = DoNotIndentText;
    if (isFirstLine || (isAfterHardLineBreak && style.textIndentLine()) == TextIndentEachLine)
        shouldIndentText = IndentText;

    if (style.textIndentType() == TextIndentHanging)
        shouldIndentText = shouldIndentText == IndentText ? DoNotIndentText : IndentText;

    return shouldIndentText;
}

}

#endif // BreakingContextInlineHeaders_h
