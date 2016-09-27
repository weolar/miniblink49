/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2005 Alexey Proskuryakov.
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
#include "core/editing/iterators/SimplifiedBackwardsTextIterator.h"

#include "core/dom/FirstLetterPseudoElement.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLTextFormControlElement.h"
#include "core/layout/LayoutTextFragment.h"

namespace blink {

static int collapsedSpaceLength(LayoutText* layoutText, int textEnd)
{
    const String& text = layoutText->text();
    int length = text.length();
    for (int i = textEnd; i < length; ++i) {
        if (!layoutText->style()->isCollapsibleWhiteSpace(text[i]))
            return i - textEnd;
    }

    return length - textEnd;
}

static int maxOffsetIncludingCollapsedSpaces(Node* node)
{
    int offset = caretMaxOffset(node);

    if (node->layoutObject() && node->layoutObject()->isText())
        offset += collapsedSpaceLength(toLayoutText(node->layoutObject()), offset);

    return offset;
}

template <typename Strategy>
SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::SimplifiedBackwardsTextIteratorAlgorithm(const PositionAlgorithm<Strategy>& start, const PositionAlgorithm<Strategy>& end, TextIteratorBehaviorFlags behavior)
    : m_node(nullptr)
    , m_offset(0)
    , m_handledNode(false)
    , m_handledChildren(false)
    , m_startNode(nullptr)
    , m_startOffset(0)
    , m_endNode(nullptr)
    , m_endOffset(0)
    , m_positionNode(nullptr)
    , m_positionStartOffset(0)
    , m_positionEndOffset(0)
    , m_textOffset(0)
    , m_textLength(0)
    , m_singleCharacterBuffer(0)
    , m_havePassedStartNode(false)
    , m_shouldHandleFirstLetter(false)
    , m_stopsOnFormControls(behavior & TextIteratorStopsOnFormControls)
    , m_shouldStop(false)
    , m_emitsOriginalText(false)
{
    ASSERT(behavior == TextIteratorDefaultBehavior || behavior == TextIteratorStopsOnFormControls);

    Node* startNode = start.deprecatedNode();
    if (!startNode)
        return;
    Node* endNode = end.deprecatedNode();
    int startOffset = start.deprecatedEditingOffset();
    int endOffset = end.deprecatedEditingOffset();

    init(startNode, endNode, startOffset, endOffset);
}

template <typename Strategy>
void SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::init(Node* startNode, Node* endNode, int startOffset, int endOffset)
{
    if (!startNode->offsetInCharacters() && startOffset >= 0) {
        // NodeTraversal::childAt() will return 0 if the offset is out of range. We rely on this behavior
        // instead of calling countChildren() to avoid traversing the children twice.
        if (Node* childAtOffset = NodeTraversal::childAt(*startNode, startOffset)) {
            startNode = childAtOffset;
            startOffset = 0;
        }
    }
    if (!endNode->offsetInCharacters() && endOffset > 0) {
        // NodeTraversal::childAt() will return 0 if the offset is out of range. We rely on this behavior
        // instead of calling countChildren() to avoid traversing the children twice.
        if (Node* childAtOffset = NodeTraversal::childAt(*endNode, endOffset - 1)) {
            endNode = childAtOffset;
            endOffset = lastOffsetInNode(endNode);
        }
    }

    m_node = endNode;
    m_fullyClippedStack.setUpFullyClippedStack(m_node);
    m_offset = endOffset;
    m_handledNode = false;
    m_handledChildren = !endOffset;

    m_startNode = startNode;
    m_startOffset = startOffset;
    m_endNode = endNode;
    m_endOffset = endOffset;

#if ENABLE(ASSERT)
    // Need this just because of the assert.
    m_positionNode = endNode;
#endif

    m_havePassedStartNode = false;

    advance();
}

template <typename Strategy>
void SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::advance()
{
    ASSERT(m_positionNode);

    if (m_shouldStop)
        return;

    if (m_stopsOnFormControls && HTMLFormControlElement::enclosingFormControlElement(m_node)) {
        m_shouldStop = true;
        return;
    }

    m_positionNode = nullptr;
    m_textLength = 0;

    while (m_node && !m_havePassedStartNode) {
        // Don't handle node if we start iterating at [node, 0].
        if (!m_handledNode && !(m_node == m_endNode && !m_endOffset)) {
            LayoutObject* layoutObject = m_node->layoutObject();
            if (layoutObject && layoutObject->isText() && m_node->nodeType() == Node::TEXT_NODE) {
                // FIXME: What about CDATA_SECTION_NODE?
                if (layoutObject->style()->visibility() == VISIBLE && m_offset > 0)
                    m_handledNode = handleTextNode();
            } else if (layoutObject && (layoutObject->isLayoutPart() || TextIterator::supportsAltText(m_node))) {
                if (layoutObject->style()->visibility() == VISIBLE && m_offset > 0)
                    m_handledNode = handleReplacedElement();
            } else {
                m_handledNode = handleNonTextNode();
            }
            if (m_positionNode)
                return;
        }

        if (!m_handledChildren && m_node->hasChildren()) {
            m_node = m_node->lastChild();
            m_fullyClippedStack.pushFullyClippedState(m_node);
        } else {
            // Exit empty containers as we pass over them or containers
            // where [container, 0] is where we started iterating.
            if (!m_handledNode
                && canHaveChildrenForEditing(m_node)
                && m_node->parentNode()
                && (!m_node->lastChild() || (m_node == m_endNode && !m_endOffset))) {
                exitNode();
                if (m_positionNode) {
                    m_handledNode = true;
                    m_handledChildren = true;
                    return;
                }
            }

            // Exit all other containers.
            while (!m_node->previousSibling()) {
                if (!advanceRespectingRange(m_node->parentOrShadowHostNode()))
                    break;
                m_fullyClippedStack.pop();
                exitNode();
                if (m_positionNode) {
                    m_handledNode = true;
                    m_handledChildren = true;
                    return;
                }
            }

            m_fullyClippedStack.pop();
            if (advanceRespectingRange(m_node->previousSibling()))
                m_fullyClippedStack.pushFullyClippedState(m_node);
            else
                m_node = nullptr;
        }

        // For the purpose of word boundary detection,
        // we should iterate all visible text and trailing (collapsed) whitespaces.
        m_offset = m_node ? maxOffsetIncludingCollapsedSpaces(m_node) : 0;
        m_handledNode = false;
        m_handledChildren = false;

        if (m_positionNode)
            return;
    }
}

template <typename Strategy>
bool SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::handleTextNode()
{
    int startOffset;
    int offsetInNode;
    LayoutText* layoutObject = handleFirstLetter(startOffset, offsetInNode);
    if (!layoutObject)
        return true;

    String text = layoutObject->text();
    if (!layoutObject->firstTextBox() && text.length() > 0)
        return true;

    m_positionEndOffset = m_offset;
    m_offset = startOffset + offsetInNode;
    m_positionNode = m_node;
    m_positionStartOffset = m_offset;

    ASSERT(0 <= m_positionStartOffset - offsetInNode && m_positionStartOffset - offsetInNode <= static_cast<int>(text.length()));
    ASSERT(1 <= m_positionEndOffset - offsetInNode && m_positionEndOffset - offsetInNode <= static_cast<int>(text.length()));
    ASSERT(m_positionStartOffset <= m_positionEndOffset);

    m_textLength = m_positionEndOffset - m_positionStartOffset;
    m_textOffset = m_positionStartOffset - offsetInNode;
    m_textContainer = text;
    m_singleCharacterBuffer = 0;
    RELEASE_ASSERT(static_cast<unsigned>(m_textOffset + m_textLength) <= text.length());

    return !m_shouldHandleFirstLetter;
}

template <typename Strategy>
LayoutText* SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::handleFirstLetter(int& startOffset, int& offsetInNode)
{
    LayoutText* layoutObject = toLayoutText(m_node->layoutObject());
    startOffset = (m_node == m_startNode) ? m_startOffset : 0;

    if (!layoutObject->isTextFragment()) {
        offsetInNode = 0;
        return layoutObject;
    }

    LayoutTextFragment* fragment = toLayoutTextFragment(layoutObject);
    int offsetAfterFirstLetter = fragment->start();
    if (startOffset >= offsetAfterFirstLetter) {
        ASSERT(!m_shouldHandleFirstLetter);
        offsetInNode = offsetAfterFirstLetter;
        return layoutObject;
    }

    if (!m_shouldHandleFirstLetter && offsetAfterFirstLetter < m_offset) {
        m_shouldHandleFirstLetter = true;
        offsetInNode = offsetAfterFirstLetter;
        return layoutObject;
    }

    m_shouldHandleFirstLetter = false;
    offsetInNode = 0;

    ASSERT(fragment->isRemainingTextLayoutObject());
    ASSERT(fragment->firstLetterPseudoElement());

    LayoutObject* pseudoElementLayoutObject = fragment->firstLetterPseudoElement()->layoutObject();
    ASSERT(pseudoElementLayoutObject);
    ASSERT(pseudoElementLayoutObject->slowFirstChild());
    LayoutText* firstLetterLayoutObject = toLayoutText(pseudoElementLayoutObject->slowFirstChild());

    m_offset = firstLetterLayoutObject->caretMaxOffset();
    m_offset += collapsedSpaceLength(firstLetterLayoutObject, m_offset);

    return firstLetterLayoutObject;
}

template <typename Strategy>
bool SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::handleReplacedElement()
{
    unsigned index = m_node->nodeIndex();
    // We want replaced elements to behave like punctuation for boundary
    // finding, and to simply take up space for the selection preservation
    // code in moveParagraphs, so we use a comma. Unconditionally emit
    // here because this iterator is only used for boundary finding.
    emitCharacter(',', m_node->parentNode(), index, index + 1);
    return true;
}

template <typename Strategy>
bool SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::handleNonTextNode()
{
    // We can use a linefeed in place of a tab because this simple iterator is only used to
    // find boundaries, not actual content. A linefeed breaks words, sentences, and paragraphs.
    if (TextIterator::shouldEmitNewlineForNode(m_node, m_emitsOriginalText) || TextIterator::shouldEmitNewlineAfterNode(*m_node) || TextIterator::shouldEmitTabBeforeNode(m_node)) {
        unsigned index = m_node->nodeIndex();
        // The start of this emitted range is wrong. Ensuring correctness would require
        // VisiblePositions and so would be slow. previousBoundary expects this.
        emitCharacter('\n', m_node->parentNode(), index + 1, index + 1);
    }
    return true;
}

template <typename Strategy>
void SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::exitNode()
{
    if (TextIterator::shouldEmitNewlineForNode(m_node, m_emitsOriginalText) || TextIterator::shouldEmitNewlineBeforeNode(*m_node) || TextIterator::shouldEmitTabBeforeNode(m_node)) {
        // The start of this emitted range is wrong. Ensuring correctness would require
        // VisiblePositions and so would be slow. previousBoundary expects this.
        emitCharacter('\n', m_node, 0, 0);
    }
}

template <typename Strategy>
void SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::emitCharacter(UChar c, Node* node, int startOffset, int endOffset)
{
    m_singleCharacterBuffer = c;
    m_positionNode = node;
    m_positionStartOffset = startOffset;
    m_positionEndOffset = endOffset;
    m_textOffset = 0;
    m_textLength = 1;
}

template <typename Strategy>
bool SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::advanceRespectingRange(Node* next)
{
    if (!next)
        return false;
    m_havePassedStartNode |= m_node == m_startNode;
    if (m_havePassedStartNode)
        return false;
    m_node = next;
    return true;
}

template <typename Strategy>
Node* SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::startContainer() const
{
    if (m_positionNode)
        return m_positionNode;
    return m_startNode;
}

template <typename Strategy>
int SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::endOffset() const
{
    if (m_positionNode)
        return m_positionEndOffset;
    return m_startOffset;
}

template <typename Strategy>
PositionAlgorithm<Strategy> SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::startPosition() const
{
    if (m_positionNode)
        return PositionAlgorithm<Strategy>::createLegacyEditingPosition(m_positionNode, m_positionStartOffset);
    return PositionAlgorithm<Strategy>::createLegacyEditingPosition(m_startNode, m_startOffset);
}

template <typename Strategy>
PositionAlgorithm<Strategy>SimplifiedBackwardsTextIteratorAlgorithm<Strategy>::endPosition() const
{
    if (m_positionNode)
        return PositionAlgorithm<Strategy>::createLegacyEditingPosition(m_positionNode, m_positionEndOffset);
    return PositionAlgorithm<Strategy>::createLegacyEditingPosition(m_startNode, m_startOffset);
}

template class CORE_TEMPLATE_EXPORT SimplifiedBackwardsTextIteratorAlgorithm<EditingStrategy>;


} // namespace blink
