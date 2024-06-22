/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
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

#include "config.h"
#include "core/editing/VisibleUnits.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/Position.h"
#include "core/dom/Text.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/RenderedPosition.h"
#include "core/editing/VisiblePosition.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/BackwardsCharacterIterator.h"
#include "core/editing/iterators/CharacterIterator.h"
#include "core/editing/iterators/SimplifiedBackwardsTextIterator.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/html/HTMLBRElement.h"
#include "core/layout/HitTestRequest.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutView.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/heap/Handle.h"
#include "platform/text/TextBoundaries.h"

namespace blink {

using namespace HTMLNames;
using namespace WTF::Unicode;

static Node* previousLeafWithSameEditability(Node* node, EditableType editableType)
{
    bool editable = node->hasEditableStyle(editableType);
    node = previousAtomicLeafNode(*node);
    while (node) {
        if (editable == node->hasEditableStyle(editableType))
            return node;
        node = previousAtomicLeafNode(*node);
    }
    return 0;
}

static Node* nextLeafWithSameEditability(Node* node, EditableType editableType = ContentIsEditable)
{
    if (!node)
        return 0;

    bool editable = node->hasEditableStyle(editableType);
    node = nextAtomicLeafNode(*node);
    while (node) {
        if (editable == node->hasEditableStyle(editableType))
            return node;
        node = nextAtomicLeafNode(*node);
    }
    return 0;
}

// FIXME: consolidate with code in previousLinePosition.
static Position previousRootInlineBoxCandidatePosition(Node* node, const VisiblePosition& visiblePosition, EditableType editableType)
{
    ContainerNode* highestRoot = highestEditableRoot(visiblePosition.deepEquivalent(), editableType);
    Node* previousNode = previousLeafWithSameEditability(node, editableType);

    while (previousNode && (!previousNode->layoutObject() || inSameLine(VisiblePosition(firstPositionInOrBeforeNode(previousNode)), visiblePosition)))
        previousNode = previousLeafWithSameEditability(previousNode, editableType);

    while (previousNode && !previousNode->isShadowRoot()) {
        if (highestEditableRoot(firstPositionInOrBeforeNode(previousNode), editableType) != highestRoot)
            break;

        Position pos = isHTMLBRElement(*previousNode) ? positionBeforeNode(previousNode) :
            createLegacyEditingPosition(previousNode, caretMaxOffset(previousNode));

        if (pos.isCandidate())
            return pos;

        previousNode = previousLeafWithSameEditability(previousNode, editableType);
    }
    return Position();
}

static Position nextRootInlineBoxCandidatePosition(Node* node, const VisiblePosition& visiblePosition, EditableType editableType)
{
    ContainerNode* highestRoot = highestEditableRoot(visiblePosition.deepEquivalent(), editableType);
    Node* nextNode = nextLeafWithSameEditability(node, editableType);
    while (nextNode && (!nextNode->layoutObject() || inSameLine(VisiblePosition(firstPositionInOrBeforeNode(nextNode)), visiblePosition)))
        nextNode = nextLeafWithSameEditability(nextNode, ContentIsEditable);

    while (nextNode && !nextNode->isShadowRoot()) {
        if (highestEditableRoot(firstPositionInOrBeforeNode(nextNode), editableType) != highestRoot)
            break;

        Position pos;
        pos = createLegacyEditingPosition(nextNode, caretMinOffset(nextNode));

        if (pos.isCandidate())
            return pos;

        nextNode = nextLeafWithSameEditability(nextNode, editableType);
    }
    return Position();
}

class CachedLogicallyOrderedLeafBoxes {
public:
    CachedLogicallyOrderedLeafBoxes();

    const InlineTextBox* previousTextBox(const RootInlineBox*, const InlineTextBox*);
    const InlineTextBox* nextTextBox(const RootInlineBox*, const InlineTextBox*);

    size_t size() const { return m_leafBoxes.size(); }
    const InlineBox* firstBox() const { return m_leafBoxes[0]; }

private:
    const Vector<InlineBox*>& collectBoxes(const RootInlineBox*);
    int boxIndexInLeaves(const InlineTextBox*) const;

    const RootInlineBox* m_rootInlineBox;
    Vector<InlineBox*> m_leafBoxes;
};

CachedLogicallyOrderedLeafBoxes::CachedLogicallyOrderedLeafBoxes() : m_rootInlineBox(0) { }

const InlineTextBox* CachedLogicallyOrderedLeafBoxes::previousTextBox(const RootInlineBox* root, const InlineTextBox* box)
{
    if (!root)
        return 0;

    collectBoxes(root);

    // If box is null, root is box's previous RootInlineBox, and previousBox is the last logical box in root.
    int boxIndex = m_leafBoxes.size() - 1;
    if (box)
        boxIndex = boxIndexInLeaves(box) - 1;

    for (int i = boxIndex; i >= 0; --i) {
        if (m_leafBoxes[i]->isInlineTextBox())
            return toInlineTextBox(m_leafBoxes[i]);
    }

    return 0;
}

const InlineTextBox* CachedLogicallyOrderedLeafBoxes::nextTextBox(const RootInlineBox* root, const InlineTextBox* box)
{
    if (!root)
        return 0;

    collectBoxes(root);

    // If box is null, root is box's next RootInlineBox, and nextBox is the first logical box in root.
    // Otherwise, root is box's RootInlineBox, and nextBox is the next logical box in the same line.
    size_t nextBoxIndex = 0;
    if (box)
        nextBoxIndex = boxIndexInLeaves(box) + 1;

    for (size_t i = nextBoxIndex; i < m_leafBoxes.size(); ++i) {
        if (m_leafBoxes[i]->isInlineTextBox())
            return toInlineTextBox(m_leafBoxes[i]);
    }

    return 0;
}

const Vector<InlineBox*>& CachedLogicallyOrderedLeafBoxes::collectBoxes(const RootInlineBox* root)
{
    if (m_rootInlineBox != root) {
        m_rootInlineBox = root;
        m_leafBoxes.clear();
        root->collectLeafBoxesInLogicalOrder(m_leafBoxes);
    }
    return m_leafBoxes;
}

int CachedLogicallyOrderedLeafBoxes::boxIndexInLeaves(const InlineTextBox* box) const
{
    for (size_t i = 0; i < m_leafBoxes.size(); ++i) {
        if (box == m_leafBoxes[i])
            return i;
    }
    return 0;
}

static const InlineTextBox* logicallyPreviousBox(const VisiblePosition& visiblePosition, const InlineTextBox* textBox,
    bool& previousBoxInDifferentBlock, CachedLogicallyOrderedLeafBoxes& leafBoxes)
{
    const InlineBox* startBox = textBox;

    const InlineTextBox* previousBox = leafBoxes.previousTextBox(&startBox->root(), textBox);
    if (previousBox)
        return previousBox;

    previousBox = leafBoxes.previousTextBox(startBox->root().prevRootBox(), 0);
    if (previousBox)
        return previousBox;

    while (1) {
        Node* startNode = startBox->layoutObject().nonPseudoNode();
        if (!startNode)
            break;

        Position position = previousRootInlineBoxCandidatePosition(startNode, visiblePosition, ContentIsEditable);
        if (position.isNull())
            break;

        RenderedPosition renderedPosition(position, DOWNSTREAM);
        RootInlineBox* previousRoot = renderedPosition.rootBox();
        if (!previousRoot)
            break;

        previousBox = leafBoxes.previousTextBox(previousRoot, 0);
        if (previousBox) {
            previousBoxInDifferentBlock = true;
            return previousBox;
        }

        if (!leafBoxes.size())
            break;
        startBox = leafBoxes.firstBox();
    }
    return 0;
}


static const InlineTextBox* logicallyNextBox(const VisiblePosition& visiblePosition, const InlineTextBox* textBox,
    bool& nextBoxInDifferentBlock, CachedLogicallyOrderedLeafBoxes& leafBoxes)
{
    const InlineBox* startBox = textBox;

    const InlineTextBox* nextBox = leafBoxes.nextTextBox(&startBox->root(), textBox);
    if (nextBox)
        return nextBox;

    nextBox = leafBoxes.nextTextBox(startBox->root().nextRootBox(), 0);
    if (nextBox)
        return nextBox;

    while (1) {
        Node* startNode =startBox->layoutObject().nonPseudoNode();
        if (!startNode)
            break;

        Position position = nextRootInlineBoxCandidatePosition(startNode, visiblePosition, ContentIsEditable);
        if (position.isNull())
            break;

        RenderedPosition renderedPosition(position, DOWNSTREAM);
        RootInlineBox* nextRoot = renderedPosition.rootBox();
        if (!nextRoot)
            break;

        nextBox = leafBoxes.nextTextBox(nextRoot, 0);
        if (nextBox) {
            nextBoxInDifferentBlock = true;
            return nextBox;
        }

        if (!leafBoxes.size())
            break;
        startBox = leafBoxes.firstBox();
    }
    return 0;
}

static TextBreakIterator* wordBreakIteratorForMinOffsetBoundary(const VisiblePosition& visiblePosition, const InlineTextBox* textBox,
    int& previousBoxLength, bool& previousBoxInDifferentBlock, Vector<UChar, 1024>& string, CachedLogicallyOrderedLeafBoxes& leafBoxes)
{
    previousBoxInDifferentBlock = false;

    // FIXME: Handle the case when we don't have an inline text box.
    const InlineTextBox* previousBox = logicallyPreviousBox(visiblePosition, textBox, previousBoxInDifferentBlock, leafBoxes);

    int len = 0;
    string.clear();
    if (previousBox) {
        previousBoxLength = previousBox->len();
        previousBox->layoutObject().text().appendTo(string, previousBox->start(), previousBoxLength);
        len += previousBoxLength;
    }
    textBox->layoutObject().text().appendTo(string, textBox->start(), textBox->len());
    len += textBox->len();

    return wordBreakIterator(string.data(), len);
}

static TextBreakIterator* wordBreakIteratorForMaxOffsetBoundary(const VisiblePosition& visiblePosition, const InlineTextBox* textBox,
    bool& nextBoxInDifferentBlock, Vector<UChar, 1024>& string, CachedLogicallyOrderedLeafBoxes& leafBoxes)
{
    nextBoxInDifferentBlock = false;

    // FIXME: Handle the case when we don't have an inline text box.
    const InlineTextBox* nextBox = logicallyNextBox(visiblePosition, textBox, nextBoxInDifferentBlock, leafBoxes);

    int len = 0;
    string.clear();
    textBox->layoutObject().text().appendTo(string, textBox->start(), textBox->len());
    len += textBox->len();
    if (nextBox) {
        nextBox->layoutObject().text().appendTo(string, nextBox->start(), nextBox->len());
        len += nextBox->len();
    }

    return wordBreakIterator(string.data(), len);
}

static bool isLogicalStartOfWord(TextBreakIterator* iter, int position, bool hardLineBreak)
{
    bool boundary = hardLineBreak ? true : iter->isBoundary(position);
    if (!boundary)
        return false;

    iter->following(position);
    // isWordTextBreak returns true after moving across a word and false after moving across a punctuation/space.
    return isWordTextBreak(iter);
}

static bool islogicalEndOfWord(TextBreakIterator* iter, int position, bool hardLineBreak)
{
    bool boundary = iter->isBoundary(position);
    return (hardLineBreak || boundary) && isWordTextBreak(iter);
}

enum CursorMovementDirection { MoveLeft, MoveRight };

static VisiblePosition visualWordPosition(const VisiblePosition& visiblePosition, CursorMovementDirection direction,
    bool skipsSpaceWhenMovingRight)
{
    if (visiblePosition.isNull())
        return VisiblePosition();

    TextDirection blockDirection = directionOfEnclosingBlock(visiblePosition.deepEquivalent());
    InlineBox* previouslyVisitedBox = 0;
    VisiblePosition current = visiblePosition;
    TextBreakIterator* iter = 0;

    CachedLogicallyOrderedLeafBoxes leafBoxes;
    Vector<UChar, 1024> string;

    while (1) {
        VisiblePosition adjacentCharacterPosition = direction == MoveRight ? current.right(true) : current.left(true);
        if (adjacentCharacterPosition == current || adjacentCharacterPosition.isNull())
            return VisiblePosition();

        InlineBoxPosition boxPosition = adjacentCharacterPosition.deepEquivalent().computeInlineBoxPosition(UPSTREAM);
        InlineBox* box = boxPosition.inlineBox;
        int offsetInBox = boxPosition.offsetInBox;

        if (!box)
            break;
        if (!box->isInlineTextBox()) {
            current = adjacentCharacterPosition;
            continue;
        }

        InlineTextBox* textBox = toInlineTextBox(box);
        int previousBoxLength = 0;
        bool previousBoxInDifferentBlock = false;
        bool nextBoxInDifferentBlock = false;
        bool movingIntoNewBox = previouslyVisitedBox != box;

        if (offsetInBox == box->caretMinOffset()) {
            iter = wordBreakIteratorForMinOffsetBoundary(visiblePosition, textBox, previousBoxLength, previousBoxInDifferentBlock, string, leafBoxes);
        } else if (offsetInBox == box->caretMaxOffset()) {
            iter = wordBreakIteratorForMaxOffsetBoundary(visiblePosition, textBox, nextBoxInDifferentBlock, string, leafBoxes);
        } else if (movingIntoNewBox) {
            iter = wordBreakIterator(textBox->layoutObject().text(), textBox->start(), textBox->len());
            previouslyVisitedBox = box;
        }

        if (!iter)
            break;

        iter->first();
        int offsetInIterator = offsetInBox - textBox->start() + previousBoxLength;

        bool isWordBreak;
        bool boxHasSameDirectionalityAsBlock = box->direction() == blockDirection;
        bool movingBackward = (direction == MoveLeft && box->direction() == LTR) || (direction == MoveRight && box->direction() == RTL);
        if ((skipsSpaceWhenMovingRight && boxHasSameDirectionalityAsBlock)
            || (!skipsSpaceWhenMovingRight && movingBackward)) {
            bool logicalStartInLayoutObject = offsetInBox == static_cast<int>(textBox->start()) && previousBoxInDifferentBlock;
            isWordBreak = isLogicalStartOfWord(iter, offsetInIterator, logicalStartInLayoutObject);
        } else {
            bool logicalEndInLayoutObject = offsetInBox == static_cast<int>(textBox->start() + textBox->len()) && nextBoxInDifferentBlock;
            isWordBreak = islogicalEndOfWord(iter, offsetInIterator, logicalEndInLayoutObject);
        }

        if (isWordBreak)
            return adjacentCharacterPosition;

        current = adjacentCharacterPosition;
    }

    return VisiblePosition();
}

VisiblePosition leftWordPosition(const VisiblePosition& visiblePosition, bool skipsSpaceWhenMovingRight)
{
    VisiblePosition leftWordBreak = visualWordPosition(visiblePosition, MoveLeft, skipsSpaceWhenMovingRight);
    leftWordBreak = visiblePosition.honorEditingBoundaryAtOrBefore(leftWordBreak);

    // FIXME: How should we handle a non-editable position?
    if (leftWordBreak.isNull() && isEditablePosition(visiblePosition.deepEquivalent())) {
        TextDirection blockDirection = directionOfEnclosingBlock(visiblePosition.deepEquivalent());
        leftWordBreak = blockDirection == LTR ? startOfEditableContent(visiblePosition) : endOfEditableContent(visiblePosition);
    }
    return leftWordBreak;
}

VisiblePosition rightWordPosition(const VisiblePosition& visiblePosition, bool skipsSpaceWhenMovingRight)
{
    VisiblePosition rightWordBreak = visualWordPosition(visiblePosition, MoveRight, skipsSpaceWhenMovingRight);
    rightWordBreak = visiblePosition.honorEditingBoundaryAtOrBefore(rightWordBreak);

    // FIXME: How should we handle a non-editable position?
    if (rightWordBreak.isNull() && isEditablePosition(visiblePosition.deepEquivalent())) {
        TextDirection blockDirection = directionOfEnclosingBlock(visiblePosition.deepEquivalent());
        rightWordBreak = blockDirection == LTR ? endOfEditableContent(visiblePosition) : startOfEditableContent(visiblePosition);
    }
    return rightWordBreak;
}


enum BoundarySearchContextAvailability { DontHaveMoreContext, MayHaveMoreContext };

typedef unsigned (*BoundarySearchFunction)(const UChar*, unsigned length, unsigned offset, BoundarySearchContextAvailability, bool& needMoreContext);

static VisiblePosition previousBoundary(const VisiblePosition& c, BoundarySearchFunction searchFunction)
{
    Position pos = c.deepEquivalent();
    Node* boundary = pos.parentEditingBoundary();
    if (!boundary)
        return VisiblePosition();

    Document& d = boundary->document();
    Position start = createLegacyEditingPosition(boundary, 0).parentAnchoredEquivalent();
    Position end = pos.parentAnchoredEquivalent();

    Vector<UChar, 1024> string;
    unsigned suffixLength = 0;

    TrackExceptionState exceptionState;
    if (requiresContextForWordBoundary(c.characterBefore())) {
        RefPtrWillBeRawPtr<Range> forwardsScanRange(d.createRange());
        forwardsScanRange->setEndAfter(boundary, exceptionState);
        forwardsScanRange->setStart(end.deprecatedNode(), end.deprecatedEditingOffset(), exceptionState);
        TextIterator forwardsIterator(forwardsScanRange->startPosition(), forwardsScanRange->endPosition());
        while (!forwardsIterator.atEnd()) {
            Vector<UChar, 1024> characters;
            forwardsIterator.text().appendTextTo(characters);
            int i = endOfFirstWordBoundaryContext(characters.data(), characters.size());
            string.append(characters.data(), i);
            suffixLength += i;
            if (static_cast<unsigned>(i) < characters.size())
                break;
            forwardsIterator.advance();
        }
    }

    ASSERT(!exceptionState.hadException());
    if (exceptionState.hadException())
        return VisiblePosition();

    SimplifiedBackwardsTextIterator it(start, end);
    unsigned next = 0;
    bool needMoreContext = false;
    while (!it.atEnd()) {
        bool inTextSecurityMode = it.node() && it.node()->layoutObject() && it.node()->layoutObject()->style()->textSecurity() != TSNONE;
        // iterate to get chunks until the searchFunction returns a non-zero value.
        if (!inTextSecurityMode) {
            it.prependTextTo(string);
        } else {
            // Treat bullets used in the text security mode as regular characters when looking for boundaries
            Vector<UChar, 1024> iteratorString;
            iteratorString.fill('x', it.length());
            string.prepend(iteratorString.data(), iteratorString.size());
        }
        next = searchFunction(string.data(), string.size(), string.size() - suffixLength, MayHaveMoreContext, needMoreContext);
        if (next)
            break;
        it.advance();
    }
    if (needMoreContext) {
        // The last search returned the beginning of the buffer and asked for more context,
        // but there is no earlier text. Force a search with what's available.
        next = searchFunction(string.data(), string.size(), string.size() - suffixLength, DontHaveMoreContext, needMoreContext);
        ASSERT(!needMoreContext);
    }

    if (!next)
        return VisiblePosition(it.atEnd() ? it.startPosition() : pos, DOWNSTREAM);

    Node* node = it.startContainer();
    if ((node->isTextNode() && static_cast<int>(next) <= node->maxCharacterOffset()) || (node->layoutObject() && node->layoutObject()->isBR() && !next)) {
        // The next variable contains a usable index into a text node
        return VisiblePosition(createLegacyEditingPosition(node, next), DOWNSTREAM);
    }

    // Use the character iterator to translate the next value into a DOM position.
    BackwardsCharacterIterator charIt(start, end);
    charIt.advance(string.size() - suffixLength - next);
    // FIXME: charIt can get out of shadow host.
    return VisiblePosition(charIt.endPosition(), DOWNSTREAM);
}

static VisiblePosition nextBoundary(const VisiblePosition& c, BoundarySearchFunction searchFunction)
{
    Position pos = c.deepEquivalent();
    Node* boundary = pos.parentEditingBoundary();
    if (!boundary)
        return VisiblePosition();

    Document& d = boundary->document();
    Position start(pos.parentAnchoredEquivalent());

    Vector<UChar, 1024> string;
    unsigned prefixLength = 0;

    if (requiresContextForWordBoundary(c.characterAfter())) {
        SimplifiedBackwardsTextIterator backwardsIterator(Position::firstPositionInNode(&d), start);
        while (!backwardsIterator.atEnd()) {
            Vector<UChar, 1024> characters;
            backwardsIterator.prependTextTo(characters);
            int length = characters.size();
            int i = startOfLastWordBoundaryContext(characters.data(), length);
            string.prepend(characters.data() + i, length - i);
            prefixLength += length - i;
            if (i > 0)
                break;
            backwardsIterator.advance();
        }
    }

    Position searchStart = createLegacyEditingPosition(start.deprecatedNode(), start.deprecatedEditingOffset());
    RangeBoundaryPoint searchEndPoint(boundary);
    searchEndPoint.setToEndOfNode(*boundary);
    Position searchEnd = searchEndPoint.toPosition();
    TextIterator it(searchStart, searchEnd, TextIteratorEmitsCharactersBetweenAllVisiblePositions);
    const unsigned invalidOffset = static_cast<unsigned>(-1);
    unsigned next = invalidOffset;
    bool needMoreContext = false;
    while (!it.atEnd()) {
        // Keep asking the iterator for chunks until the search function
        // returns an end value not equal to the length of the string passed to it.
        bool inTextSecurityMode = it.node() && it.node()->layoutObject() && it.node()->layoutObject()->style()->textSecurity() != TSNONE;
        if (!inTextSecurityMode) {
            it.text().appendTextTo(string);
        } else {
            // Treat bullets used in the text security mode as regular characters when looking for boundaries
            Vector<UChar, 1024> iteratorString;
            iteratorString.fill('x', it.length());
            string.append(iteratorString.data(), iteratorString.size());
        }
        next = searchFunction(string.data(), string.size(), prefixLength, MayHaveMoreContext, needMoreContext);
        if (next != string.size())
            break;
        it.advance();
    }
    if (needMoreContext) {
        // The last search returned the end of the buffer and asked for more context,
        // but there is no further text. Force a search with what's available.
        next = searchFunction(string.data(), string.size(), prefixLength, DontHaveMoreContext, needMoreContext);
        ASSERT(!needMoreContext);
    }

    if (it.atEnd() && next == string.size()) {
        pos = it.startPositionInCurrentContainer();
    } else if (next != invalidOffset && next != prefixLength) {
        // Use the character iterator to translate the next value into a DOM position.
        CharacterIterator charIt(searchStart, searchEnd, TextIteratorEmitsCharactersBetweenAllVisiblePositions);
        charIt.advance(next - prefixLength - 1);
        pos = charIt.endPosition();

        if (charIt.characterAt(0) == '\n') {
            // FIXME: workaround for collapsed range (where only start position is correct) emitted for some emitted newlines (see rdar://5192593)
            VisiblePosition visPos = VisiblePosition(pos);
            if (visPos == VisiblePosition(charIt.startPosition())) {
                charIt.advance(1);
                pos = charIt.startPosition();
            }
        }
    }

    // generate VisiblePosition, use UPSTREAM affinity if possible
    return VisiblePosition(pos, VP_UPSTREAM_IF_POSSIBLE);
}

// ---------

static unsigned startWordBoundary(const UChar* characters, unsigned length, unsigned offset, BoundarySearchContextAvailability mayHaveMoreContext, bool& needMoreContext)
{
    ASSERT(offset);
    if (mayHaveMoreContext && !startOfLastWordBoundaryContext(characters, offset)) {
        needMoreContext = true;
        return 0;
    }
    needMoreContext = false;
    int start, end;
    U16_BACK_1(characters, 0, offset);
    findWordBoundary(characters, length, offset, &start, &end);
    return start;
}

VisiblePosition startOfWord(const VisiblePosition &c, EWordSide side)
{
    // FIXME: This returns a null VP for c at the start of the document
    // and side == LeftWordIfOnBoundary
    VisiblePosition p = c;
    if (side == RightWordIfOnBoundary) {
        // at paragraph end, the startofWord is the current position
        if (isEndOfParagraph(c))
            return c;

        p = c.next();
        if (p.isNull())
            return c;
    }
    return previousBoundary(p, startWordBoundary);
}

static unsigned endWordBoundary(const UChar* characters, unsigned length, unsigned offset, BoundarySearchContextAvailability mayHaveMoreContext, bool& needMoreContext)
{
    ASSERT(offset <= length);
    if (mayHaveMoreContext && endOfFirstWordBoundaryContext(characters + offset, length - offset) == static_cast<int>(length - offset)) {
        needMoreContext = true;
        return length;
    }
    needMoreContext = false;
    return findWordEndBoundary(characters, length, offset);
}

VisiblePosition endOfWord(const VisiblePosition &c, EWordSide side)
{
    VisiblePosition p = c;
    if (side == LeftWordIfOnBoundary) {
        if (isStartOfParagraph(c))
            return c;

        p = c.previous();
        if (p.isNull())
            return c;
    } else if (isEndOfParagraph(c)) {
        return c;
    }

    return nextBoundary(p, endWordBoundary);
}

static unsigned previousWordPositionBoundary(const UChar* characters, unsigned length, unsigned offset, BoundarySearchContextAvailability mayHaveMoreContext, bool& needMoreContext)
{
    if (mayHaveMoreContext && !startOfLastWordBoundaryContext(characters, offset)) {
        needMoreContext = true;
        return 0;
    }
    needMoreContext = false;
    return findNextWordFromIndex(characters, length, offset, false);
}

VisiblePosition previousWordPosition(const VisiblePosition &c)
{
    VisiblePosition prev = previousBoundary(c, previousWordPositionBoundary);
    return c.honorEditingBoundaryAtOrBefore(prev);
}

static unsigned nextWordPositionBoundary(const UChar* characters, unsigned length, unsigned offset, BoundarySearchContextAvailability mayHaveMoreContext, bool& needMoreContext)
{
    if (mayHaveMoreContext && endOfFirstWordBoundaryContext(characters + offset, length - offset) == static_cast<int>(length - offset)) {
        needMoreContext = true;
        return length;
    }
    needMoreContext = false;
    return findNextWordFromIndex(characters, length, offset, true);
}

VisiblePosition nextWordPosition(const VisiblePosition &c)
{
    VisiblePosition next = nextBoundary(c, nextWordPositionBoundary);
    return c.honorEditingBoundaryAtOrAfter(next);
}

// ---------

enum LineEndpointComputationMode { UseLogicalOrdering, UseInlineBoxOrdering };
template <typename PositionWithAffinityType>
static PositionWithAffinityType startPositionForLine(const PositionWithAffinityType& c, LineEndpointComputationMode mode)
{
    using PositionType = typename PositionWithAffinityType::PositionType;

    if (c.isNull())
        return PositionWithAffinityType();

    RootInlineBox* rootBox = RenderedPosition(c.position(), c.affinity()).rootBox();
    if (!rootBox) {
        // There are VisiblePositions at offset 0 in blocks without
        // RootInlineBoxes, like empty editable blocks and bordered blocks.
        PositionType p = c.position();
        if (p.deprecatedNode()->layoutObject() && p.deprecatedNode()->layoutObject()->isLayoutBlock() && !p.deprecatedEditingOffset())
            return c;

        return PositionWithAffinityType();
    }

    Node* startNode;
    InlineBox* startBox;
    if (mode == UseLogicalOrdering) {
        startNode = rootBox->getLogicalStartBoxWithNode(startBox);
        if (!startNode)
            return PositionWithAffinityType();
    } else {
        // Generated content (e.g. list markers and CSS :before and :after pseudoelements) have no corresponding DOM element,
        // and so cannot be represented by a VisiblePosition. Use whatever follows instead.
        startBox = rootBox->firstLeafChild();
        while (true) {
            if (!startBox)
                return PositionWithAffinityType();

            startNode = startBox->layoutObject().nonPseudoNode();
            if (startNode)
                break;

            startBox = startBox->nextLeafChild();
        }
    }

    return PositionWithAffinityType(startNode->isTextNode() ? PositionType(toText(startNode), toInlineTextBox(startBox)->start()) : PositionType::beforeNode(startNode));
}

template <typename PositionWithAffinityType>
static PositionWithAffinityType startOfLine(const PositionWithAffinityType& c, LineEndpointComputationMode mode)
{
    using PositionType = typename PositionWithAffinityType::PositionType;
    // TODO: this is the current behavior that might need to be fixed.
    // Please refer to https://bugs.webkit.org/show_bug.cgi?id=49107 for detail.
    PositionWithAffinityType visPos = startPositionForLine(c, mode);

    if (mode == UseLogicalOrdering) {
        if (ContainerNode* editableRoot = highestEditableRoot(c.position())) {
            if (!editableRoot->contains(visPos.position().containerNode()))
                return PositionWithAffinityType(PositionType::firstPositionInNode(editableRoot));
        }
    }

    return honorEditingBoundaryAtOrBeforeOf(visPos, c.position());
}

static PositionWithAffinity startOfLine(const PositionWithAffinity& currentPosition)
{
    return startOfLine(currentPosition, UseInlineBoxOrdering);
}

static PositionInComposedTreeWithAffinity startOfLine(const PositionInComposedTreeWithAffinity& currentPosition)
{
    return startOfLine(currentPosition, UseInlineBoxOrdering);
}

// FIXME: Rename this function to reflect the fact it ignores bidi levels.
VisiblePosition startOfLine(const VisiblePosition& currentPosition)
{
    return VisiblePosition(startOfLine(PositionWithAffinity(currentPosition.deepEquivalent(), currentPosition.affinity()), UseInlineBoxOrdering));
}

VisiblePosition logicalStartOfLine(const VisiblePosition& currentPosition)
{
    return VisiblePosition(startOfLine(PositionWithAffinity(currentPosition.deepEquivalent(), currentPosition.affinity()), UseLogicalOrdering));
}

static VisiblePosition endPositionForLine(const VisiblePosition& c, LineEndpointComputationMode mode)
{
    if (c.isNull())
        return VisiblePosition();

    RootInlineBox* rootBox = RenderedPosition(c).rootBox();
    if (!rootBox) {
        // There are VisiblePositions at offset 0 in blocks without
        // RootInlineBoxes, like empty editable blocks and bordered blocks.
        Position p = c.deepEquivalent();
        if (p.deprecatedNode()->layoutObject() && p.deprecatedNode()->layoutObject()->isLayoutBlock() && !p.deprecatedEditingOffset())
            return c;
        return VisiblePosition();
    }

    Node* endNode;
    InlineBox* endBox;
    if (mode == UseLogicalOrdering) {
        endNode = rootBox->getLogicalEndBoxWithNode(endBox);
        if (!endNode)
            return VisiblePosition();
    } else {
        // Generated content (e.g. list markers and CSS :before and :after pseudoelements) have no corresponding DOM element,
        // and so cannot be represented by a VisiblePosition. Use whatever precedes instead.
        endBox = rootBox->lastLeafChild();
        while (true) {
            if (!endBox)
                return VisiblePosition();

            endNode = endBox->layoutObject().nonPseudoNode();
            if (endNode)
                break;

            endBox = endBox->prevLeafChild();
        }
    }

    Position pos;
    if (isHTMLBRElement(*endNode)) {
        pos = positionBeforeNode(endNode);
    } else if (endBox->isInlineTextBox() && endNode->isTextNode()) {
        InlineTextBox* endTextBox = toInlineTextBox(endBox);
        int endOffset = endTextBox->start();
        if (!endTextBox->isLineBreak())
            endOffset += endTextBox->len();
        pos = Position(toText(endNode), endOffset);
    } else {
        pos = positionAfterNode(endNode);
    }

    return VisiblePosition(pos, VP_UPSTREAM_IF_POSSIBLE);
}

static bool inSameLogicalLine(const VisiblePosition& a, const VisiblePosition& b)
{
    return a.isNotNull() && logicalStartOfLine(a) == logicalStartOfLine(b);
}

static VisiblePosition endOfLine(const VisiblePosition& c, LineEndpointComputationMode mode)
{
    // TODO: this is the current behavior that might need to be fixed.
    // Please refer to https://bugs.webkit.org/show_bug.cgi?id=49107 for detail.
    VisiblePosition visPos = endPositionForLine(c, mode);

    if (mode == UseLogicalOrdering) {
        // Make sure the end of line is at the same line as the given input position. For a wrapping line, the logical end
        // position for the not-last-2-lines might incorrectly hand back the logical beginning of the next line.
        // For example, <div contenteditable dir="rtl" style="line-break:before-white-space">abcdefg abcdefg abcdefg
        // a abcdefg abcdefg abcdefg abcdefg abcdefg abcdefg abcdefg abcdefg abcdefg abcdefg </div>
        // In this case, use the previous position of the computed logical end position.
        if (!inSameLogicalLine(c, visPos))
            visPos = visPos.previous();

        if (ContainerNode* editableRoot = highestEditableRoot(c.deepEquivalent())) {
            if (!editableRoot->contains(visPos.deepEquivalent().containerNode()))
                return VisiblePosition(lastPositionInNode(editableRoot));
        }

        return c.honorEditingBoundaryAtOrAfter(visPos);
    }

    // Make sure the end of line is at the same line as the given input position. Else use the previous position to
    // obtain end of line. This condition happens when the input position is before the space character at the end
    // of a soft-wrapped non-editable line. In this scenario, endPositionForLine would incorrectly hand back a position
    // in the next line instead. This fix is to account for the discrepancy between lines with webkit-line-break:after-white-space style
    // versus lines without that style, which would break before a space by default.
    if (!inSameLine(c, visPos)) {
        visPos = c.previous();
        if (visPos.isNull())
            return VisiblePosition();
        visPos = endPositionForLine(visPos, UseInlineBoxOrdering);
    }

    return c.honorEditingBoundaryAtOrAfter(visPos);
}

// FIXME: Rename this function to reflect the fact it ignores bidi levels.
VisiblePosition endOfLine(const VisiblePosition& currentPosition)
{
    return endOfLine(currentPosition, UseInlineBoxOrdering);
}

VisiblePosition logicalEndOfLine(const VisiblePosition& currentPosition)
{
    return endOfLine(currentPosition, UseLogicalOrdering);
}

template <typename PositionWithAffinityType>
bool inSameLineAlgorithm(const PositionWithAffinityType& position1, const PositionWithAffinityType& position2)
{
    if (position1.isNull() || position2.isNull())
        return false;
    PositionWithAffinityType startOfLine1 = startOfLine(position1);
    PositionWithAffinityType startOfLine2 = startOfLine(position2);
    if (startOfLine1 == startOfLine2)
        return true;
    typename PositionWithAffinityType::PositionType canonicalized1 = canonicalPositionOf(startOfLine1.position());
    if (canonicalized1 == startOfLine2.position())
        return true;
    return canonicalized1 == canonicalPositionOf(startOfLine2.position());
}

bool inSameLine(const PositionWithAffinity& a, const PositionWithAffinity& b)
{
    return inSameLineAlgorithm(a, b);
}

bool inSameLine(const PositionInComposedTreeWithAffinity& position1, const PositionInComposedTreeWithAffinity& position2)
{
    return inSameLineAlgorithm(position1, position2);
}

bool inSameLine(const VisiblePosition &a, const VisiblePosition &b)
{
    return inSameLine(PositionWithAffinity(a.deepEquivalent(), a.affinity()), PositionWithAffinity(b.deepEquivalent(), b.affinity()));
}

bool isStartOfLine(const VisiblePosition &p)
{
    return p.isNotNull() && p == startOfLine(p);
}

bool isEndOfLine(const VisiblePosition &p)
{
    return p.isNotNull() && p == endOfLine(p);
}

bool isLogicalEndOfLine(const VisiblePosition &p)
{
    return p.isNotNull() && p == logicalEndOfLine(p);
}

static inline LayoutPoint absoluteLineDirectionPointToLocalPointInBlock(RootInlineBox* root, LayoutUnit lineDirectionPoint)
{
    ASSERT(root);
    LayoutBlockFlow& containingBlock = root->block();
    FloatPoint absoluteBlockPoint = containingBlock.localToAbsolute(FloatPoint());
    if (containingBlock.hasOverflowClip())
        absoluteBlockPoint -= containingBlock.scrolledContentOffset();

    if (root->block().isHorizontalWritingMode())
        return LayoutPoint(lineDirectionPoint - absoluteBlockPoint.x(), root->blockDirectionPointInLine());

    return LayoutPoint(root->blockDirectionPointInLine(), lineDirectionPoint - absoluteBlockPoint.y());
}

VisiblePosition previousLinePosition(const VisiblePosition &visiblePosition, LayoutUnit lineDirectionPoint, EditableType editableType)
{
    Position p = visiblePosition.deepEquivalent();
    Node* node = p.deprecatedNode();

    if (!node)
        return VisiblePosition();

    node->document().updateLayoutIgnorePendingStylesheets();

    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject)
        return VisiblePosition();

    RootInlineBox* root = 0;
    InlineBox* box = visiblePosition.computeInlineBoxPosition().inlineBox;
    if (box) {
        root = box->root().prevRootBox();
        // We want to skip zero height boxes.
        // This could happen in case it is a TrailingFloatsRootInlineBox.
        if (!root || !root->logicalHeight() || !root->firstLeafChild())
            root = 0;
    }

    if (!root) {
        Position position = previousRootInlineBoxCandidatePosition(node, visiblePosition, editableType);
        if (position.isNotNull()) {
            RenderedPosition renderedPosition((VisiblePosition(position)));
            root = renderedPosition.rootBox();
            if (!root)
                return VisiblePosition(position);
        }
    }

    if (root) {
        // FIXME: Can be wrong for multi-column layout and with transforms.
        LayoutPoint pointInLine = absoluteLineDirectionPointToLocalPointInBlock(root, lineDirectionPoint);
        LayoutObject& layoutObject = root->closestLeafChildForPoint(pointInLine, isEditablePosition(p))->layoutObject();
        Node* node = layoutObject.node();
        if (node && editingIgnoresContent(node))
            return VisiblePosition(positionInParentBeforeNode(*node));
        return VisiblePosition(layoutObject.positionForPoint(pointInLine));
    }

    // Could not find a previous line. This means we must already be on the first line.
    // Move to the start of the content in this block, which effectively moves us
    // to the start of the line we're on.
    Element* rootElement = node->hasEditableStyle(editableType) ? node->rootEditableElement(editableType) : node->document().documentElement();
    if (!rootElement)
        return VisiblePosition();
    return VisiblePosition(firstPositionInNode(rootElement), DOWNSTREAM);
}

VisiblePosition nextLinePosition(const VisiblePosition &visiblePosition, LayoutUnit lineDirectionPoint, EditableType editableType)
{
    Position p = visiblePosition.deepEquivalent();
    Node* node = p.deprecatedNode();

    if (!node)
        return VisiblePosition();

    node->document().updateLayoutIgnorePendingStylesheets();

    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject)
        return VisiblePosition();

    RootInlineBox* root = 0;
    InlineBox* box = visiblePosition.computeInlineBoxPosition().inlineBox;
    if (box) {
        root = box->root().nextRootBox();
        // We want to skip zero height boxes.
        // This could happen in case it is a TrailingFloatsRootInlineBox.
        if (!root || !root->logicalHeight() || !root->firstLeafChild())
            root = 0;
    }

    if (!root) {
        // FIXME: We need do the same in previousLinePosition.
        Node* child = NodeTraversal::childAt(*node, p.deprecatedEditingOffset());
        node = child ? child : &NodeTraversal::lastWithinOrSelf(*node);
        Position position = nextRootInlineBoxCandidatePosition(node, visiblePosition, editableType);
        if (position.isNotNull()) {
            RenderedPosition renderedPosition((VisiblePosition(position)));
            root = renderedPosition.rootBox();
            if (!root)
                return VisiblePosition(position);
        }
    }

    if (root) {
        // FIXME: Can be wrong for multi-column layout and with transforms.
        LayoutPoint pointInLine = absoluteLineDirectionPointToLocalPointInBlock(root, lineDirectionPoint);
        LayoutObject& layoutObject = root->closestLeafChildForPoint(pointInLine, isEditablePosition(p))->layoutObject();
        Node* node = layoutObject.node();
        if (node && editingIgnoresContent(node))
            return VisiblePosition(positionInParentBeforeNode(*node));
        return VisiblePosition(layoutObject.positionForPoint(pointInLine));
    }

    // Could not find a next line. This means we must already be on the last line.
    // Move to the end of the content in this block, which effectively moves us
    // to the end of the line we're on.
    Element* rootElement = node->hasEditableStyle(editableType) ? node->rootEditableElement(editableType) : node->document().documentElement();
    if (!rootElement)
        return VisiblePosition();
    return VisiblePosition(lastPositionInNode(rootElement), DOWNSTREAM);
}

// ---------

static unsigned startSentenceBoundary(const UChar* characters, unsigned length, unsigned, BoundarySearchContextAvailability, bool&)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    TextBreakIterator* iterator = sentenceBreakIterator(characters, length);
    // FIXME: The following function can return -1; we don't handle that.
    return iterator->preceding(length);
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return 0;
}

VisiblePosition startOfSentence(const VisiblePosition &c)
{
    return previousBoundary(c, startSentenceBoundary);
}

static unsigned endSentenceBoundary(const UChar* characters, unsigned length, unsigned, BoundarySearchContextAvailability, bool&)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    TextBreakIterator* iterator = sentenceBreakIterator(characters, length);
    return iterator->next();
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return 0;
}

// FIXME: This includes the space after the punctuation that marks the end of the sentence.
VisiblePosition endOfSentence(const VisiblePosition &c)
{
    return nextBoundary(c, endSentenceBoundary);
}

static unsigned previousSentencePositionBoundary(const UChar* characters, unsigned length, unsigned, BoundarySearchContextAvailability, bool&)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    // FIXME: This is identical to startSentenceBoundary. I'm pretty sure that's not right.
    TextBreakIterator* iterator = sentenceBreakIterator(characters, length);
    // FIXME: The following function can return -1; we don't handle that.
    return iterator->preceding(length);
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return 0;
}

VisiblePosition previousSentencePosition(const VisiblePosition &c)
{
    VisiblePosition prev = previousBoundary(c, previousSentencePositionBoundary);
    return c.honorEditingBoundaryAtOrBefore(prev);
}

static unsigned nextSentencePositionBoundary(const UChar* characters, unsigned length, unsigned, BoundarySearchContextAvailability, bool&)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    // FIXME: This is identical to endSentenceBoundary. This isn't right, it needs to
    // move to the equivlant position in the following sentence.
    TextBreakIterator* iterator = sentenceBreakIterator(characters, length);
    return iterator->following(0);
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return 0;
}

VisiblePosition nextSentencePosition(const VisiblePosition &c)
{
    VisiblePosition next = nextBoundary(c, nextSentencePositionBoundary);
    return c.honorEditingBoundaryAtOrAfter(next);
}

VisiblePosition startOfParagraph(const VisiblePosition& c, EditingBoundaryCrossingRule boundaryCrossingRule)
{
    Position p = c.deepEquivalent();
    Node* startNode = p.deprecatedNode();

    if (!startNode)
        return VisiblePosition();

    if (isRenderedAsNonInlineTableImageOrHR(startNode))
        return VisiblePosition(positionBeforeNode(startNode));

    Element* startBlock = enclosingBlock(startNode);

    Node* node = startNode;
    ContainerNode* highestRoot = highestEditableRoot(p);
    int offset = p.deprecatedEditingOffset();
    PositionAnchorType type = p.anchorType();

    Node* n = startNode;
    bool startNodeIsEditable = startNode->hasEditableStyle();
    while (n) {
        if (boundaryCrossingRule == CannotCrossEditingBoundary && !Position::nodeIsUserSelectAll(n) && n->hasEditableStyle() != startNodeIsEditable)
            break;
        if (boundaryCrossingRule == CanSkipOverEditingBoundary) {
            while (n && n->hasEditableStyle() != startNodeIsEditable)
                n = NodeTraversal::previousPostOrder(*n, startBlock);
            if (!n || !n->isDescendantOf(highestRoot))
                break;
        }
        LayoutObject* r = n->layoutObject();
        if (!r) {
            n = NodeTraversal::previousPostOrder(*n, startBlock);
            continue;
        }
        const ComputedStyle& style = r->styleRef();
        if (style.visibility() != VISIBLE) {
            n = NodeTraversal::previousPostOrder(*n, startBlock);
            continue;
        }

        if (r->isBR() || isBlock(n))
            break;

        if (r->isText() && toLayoutText(r)->resolvedTextLength()) {
            ASSERT_WITH_SECURITY_IMPLICATION(n->isTextNode());
            type = PositionAnchorType::OffsetInAnchor;
            if (style.preserveNewline()) {
                LayoutText* text = toLayoutText(r);
                int i = text->textLength();
                int o = offset;
                if (n == startNode && o < i)
                    i = max(0, o);
                while (--i >= 0) {
                    if ((*text)[i] == '\n')
                        return VisiblePosition(Position(toText(n), i + 1), DOWNSTREAM);
                }
            }
            node = n;
            offset = 0;
            n = NodeTraversal::previousPostOrder(*n, startBlock);
        } else if (editingIgnoresContent(n) || isRenderedTableElement(n)) {
            node = n;
            type = PositionAnchorType::BeforeAnchor;
            n = n->previousSibling() ? n->previousSibling() : NodeTraversal::previousPostOrder(*n, startBlock);
        } else {
            n = NodeTraversal::previousPostOrder(*n, startBlock);
        }
    }

    if (type == PositionAnchorType::OffsetInAnchor)
        return VisiblePosition(Position(node, offset), DOWNSTREAM);

    return VisiblePosition(Position(node, type), DOWNSTREAM);
}

VisiblePosition endOfParagraph(const VisiblePosition &c, EditingBoundaryCrossingRule boundaryCrossingRule)
{
    if (c.isNull())
        return VisiblePosition();

    Position p = c.deepEquivalent();
    Node* startNode = p.deprecatedNode();

    if (isRenderedAsNonInlineTableImageOrHR(startNode))
        return VisiblePosition(positionAfterNode(startNode));

    Element* startBlock = enclosingBlock(startNode);
    Element* stayInsideBlock = startBlock;

    Node* node = startNode;
    ContainerNode* highestRoot = highestEditableRoot(p);
    int offset = p.deprecatedEditingOffset();
    PositionAnchorType type = p.anchorType();

    Node* n = startNode;
    bool startNodeIsEditable = startNode->hasEditableStyle();
    while (n) {
        if (boundaryCrossingRule == CannotCrossEditingBoundary && !Position::nodeIsUserSelectAll(n) && n->hasEditableStyle() != startNodeIsEditable)
            break;
        if (boundaryCrossingRule == CanSkipOverEditingBoundary) {
            while (n && n->hasEditableStyle() != startNodeIsEditable)
                n = NodeTraversal::next(*n, stayInsideBlock);
            if (!n || !n->isDescendantOf(highestRoot))
                break;
        }

        LayoutObject* r = n->layoutObject();
        if (!r) {
            n = NodeTraversal::next(*n, stayInsideBlock);
            continue;
        }
        const ComputedStyle& style = r->styleRef();
        if (style.visibility() != VISIBLE) {
            n = NodeTraversal::next(*n, stayInsideBlock);
            continue;
        }

        if (r->isBR() || isBlock(n))
            break;

        // FIXME: We avoid returning a position where the layoutObject can't accept the caret.
        if (r->isText() && toLayoutText(r)->resolvedTextLength()) {
            ASSERT_WITH_SECURITY_IMPLICATION(n->isTextNode());
            int length = toLayoutText(r)->textLength();
            type = PositionAnchorType::OffsetInAnchor;
            if (style.preserveNewline()) {
                LayoutText* text = toLayoutText(r);
                int o = n == startNode ? offset : 0;
                for (int i = o; i < length; ++i) {
                    if ((*text)[i] == '\n')
                        return VisiblePosition(Position(toText(n), i), DOWNSTREAM);
                }
            }
            node = n;
            offset = r->caretMaxOffset();
            n = NodeTraversal::next(*n, stayInsideBlock);
        } else if (editingIgnoresContent(n) || isRenderedTableElement(n)) {
            node = n;
            type = PositionAnchorType::AfterAnchor;
            n = NodeTraversal::nextSkippingChildren(*n, stayInsideBlock);
        } else {
            n = NodeTraversal::next(*n, stayInsideBlock);
        }
    }

    if (type == PositionAnchorType::OffsetInAnchor)
        return VisiblePosition(Position(node, offset), DOWNSTREAM);

    return VisiblePosition(Position(node, type), DOWNSTREAM);
}

// FIXME: isStartOfParagraph(startOfNextParagraph(pos)) is not always true
VisiblePosition startOfNextParagraph(const VisiblePosition& visiblePosition)
{
    VisiblePosition paragraphEnd(endOfParagraph(visiblePosition, CanSkipOverEditingBoundary));
    VisiblePosition afterParagraphEnd(paragraphEnd.next(CannotCrossEditingBoundary));
    // The position after the last position in the last cell of a table
    // is not the start of the next paragraph.
    if (isFirstPositionAfterTable(afterParagraphEnd))
        return afterParagraphEnd.next(CannotCrossEditingBoundary);
    return afterParagraphEnd;
}

bool inSameParagraph(const VisiblePosition &a, const VisiblePosition &b, EditingBoundaryCrossingRule boundaryCrossingRule)
{
    return a.isNotNull() && startOfParagraph(a, boundaryCrossingRule) == startOfParagraph(b, boundaryCrossingRule);
}

bool isStartOfParagraph(const VisiblePosition &pos, EditingBoundaryCrossingRule boundaryCrossingRule)
{
    return pos.isNotNull() && pos == startOfParagraph(pos, boundaryCrossingRule);
}

bool isEndOfParagraph(const VisiblePosition &pos, EditingBoundaryCrossingRule boundaryCrossingRule)
{
    return pos.isNotNull() && pos == endOfParagraph(pos, boundaryCrossingRule);
}

VisiblePosition previousParagraphPosition(const VisiblePosition& p, LayoutUnit x)
{
    VisiblePosition pos = p;
    do {
        VisiblePosition n = previousLinePosition(pos, x);
        if (n.isNull() || n == pos)
            break;
        pos = n;
    } while (inSameParagraph(p, pos));
    return pos;
}

VisiblePosition nextParagraphPosition(const VisiblePosition& p, LayoutUnit x)
{
    VisiblePosition pos = p;
    do {
        VisiblePosition n = nextLinePosition(pos, x);
        if (n.isNull() || n == pos)
            break;
        pos = n;
    } while (inSameParagraph(p, pos));
    return pos;
}

// ---------

VisiblePosition startOfBlock(const VisiblePosition& visiblePosition, EditingBoundaryCrossingRule rule)
{
    Position position = visiblePosition.deepEquivalent();
    Element* startBlock = position.containerNode() ? enclosingBlock(position.containerNode(), rule) : 0;
    return startBlock ? VisiblePosition(firstPositionInNode(startBlock)) : VisiblePosition();
}

VisiblePosition endOfBlock(const VisiblePosition& visiblePosition, EditingBoundaryCrossingRule rule)
{
    Position position = visiblePosition.deepEquivalent();
    Element* endBlock = position.containerNode() ? enclosingBlock(position.containerNode(), rule) : 0;
    return endBlock ? VisiblePosition(lastPositionInNode(endBlock)) : VisiblePosition();
}

bool inSameBlock(const VisiblePosition &a, const VisiblePosition &b)
{
    return !a.isNull() && enclosingBlock(a.deepEquivalent().containerNode()) == enclosingBlock(b.deepEquivalent().containerNode());
}

bool isStartOfBlock(const VisiblePosition &pos)
{
    return pos.isNotNull() && pos == startOfBlock(pos, CanCrossEditingBoundary);
}

bool isEndOfBlock(const VisiblePosition &pos)
{
    return pos.isNotNull() && pos == endOfBlock(pos, CanCrossEditingBoundary);
}

// ---------

VisiblePosition startOfDocument(const Node* node)
{
    if (!node || !node->document().documentElement())
        return VisiblePosition();

    return VisiblePosition(firstPositionInNode(node->document().documentElement()), DOWNSTREAM);
}

VisiblePosition startOfDocument(const VisiblePosition &c)
{
    return startOfDocument(c.deepEquivalent().deprecatedNode());
}

VisiblePosition endOfDocument(const Node* node)
{
    if (!node || !node->document().documentElement())
        return VisiblePosition();

    Element* doc = node->document().documentElement();
    return VisiblePosition(lastPositionInNode(doc), DOWNSTREAM);
}

VisiblePosition endOfDocument(const VisiblePosition &c)
{
    return endOfDocument(c.deepEquivalent().deprecatedNode());
}

bool isStartOfDocument(const VisiblePosition &p)
{
    return p.isNotNull() && p.previous(CanCrossEditingBoundary).isNull();
}

bool isEndOfDocument(const VisiblePosition &p)
{
    return p.isNotNull() && p.next(CanCrossEditingBoundary).isNull();
}

// ---------

VisiblePosition startOfEditableContent(const VisiblePosition& visiblePosition)
{
    ContainerNode* highestRoot = highestEditableRoot(visiblePosition.deepEquivalent());
    if (!highestRoot)
        return VisiblePosition();

    return VisiblePosition(firstPositionInNode(highestRoot));
}

VisiblePosition endOfEditableContent(const VisiblePosition& visiblePosition)
{
    ContainerNode* highestRoot = highestEditableRoot(visiblePosition.deepEquivalent());
    if (!highestRoot)
        return VisiblePosition();

    return VisiblePosition(lastPositionInNode(highestRoot));
}

bool isEndOfEditableOrNonEditableContent(const VisiblePosition &p)
{
    return p.isNotNull() && p.next().isNull();
}

VisiblePosition leftBoundaryOfLine(const VisiblePosition& c, TextDirection direction)
{
    return direction == LTR ? logicalStartOfLine(c) : logicalEndOfLine(c);
}

VisiblePosition rightBoundaryOfLine(const VisiblePosition& c, TextDirection direction)
{
    return direction == LTR ? logicalEndOfLine(c) : logicalStartOfLine(c);
}

LayoutRect localCaretRectOfPosition(const PositionWithAffinity& position, LayoutObject*& layoutObject)
{
    if (position.position().isNull()) {
        layoutObject = nullptr;
        return LayoutRect();
    }
    Node* node = position.position().anchorNode();

    layoutObject = node->layoutObject();
    if (!layoutObject)
        return LayoutRect();

    InlineBoxPosition boxPosition = position.position().computeInlineBoxPosition(position.affinity());

    if (boxPosition.inlineBox)
        layoutObject = &boxPosition.inlineBox->layoutObject();

    return layoutObject->localCaretRect(boxPosition.inlineBox, boxPosition.offsetInBox);
}

VisiblePosition visiblePositionForContentsPoint(const IntPoint& contentsPoint, LocalFrame* frame)
{
    HitTestRequest request = HitTestRequest::Move | HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::IgnoreClipping;
    HitTestResult result(request, contentsPoint);
    frame->document()->layoutView()->hitTest(result);

    if (Node* node = result.innerNode())
        return frame->selection().selection().visiblePositionRespectingEditingBoundary(result.localPoint(), node);
    return VisiblePosition();
}

}
