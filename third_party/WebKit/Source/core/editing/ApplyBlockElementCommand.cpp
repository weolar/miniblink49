/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "core/editing/ApplyBlockElementCommand.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/HTMLNames.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/Text.h"
#include "core/editing/VisiblePosition.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/html/HTMLBRElement.h"
#include "core/html/HTMLElement.h"
#include "core/layout/LayoutObject.h"
#include "core/style/ComputedStyle.h"

namespace blink {

using namespace HTMLNames;

ApplyBlockElementCommand::ApplyBlockElementCommand(Document& document, const QualifiedName& tagName, const AtomicString& inlineStyle)
    : CompositeEditCommand(document)
    , m_tagName(tagName)
    , m_inlineStyle(inlineStyle)
{
}

ApplyBlockElementCommand::ApplyBlockElementCommand(Document& document, const QualifiedName& tagName)
    : CompositeEditCommand(document)
    , m_tagName(tagName)
{
}

void ApplyBlockElementCommand::doApply()
{
    if (!endingSelection().rootEditableElement())
        return;

    VisiblePosition visibleEnd = endingSelection().visibleEnd();
    VisiblePosition visibleStart = endingSelection().visibleStart();
    if (visibleStart.isNull() || visibleStart.isOrphan() || visibleEnd.isNull() || visibleEnd.isOrphan())
        return;

    // When a selection ends at the start of a paragraph, we rarely paint
    // the selection gap before that paragraph, because there often is no gap.
    // In a case like this, it's not obvious to the user that the selection
    // ends "inside" that paragraph, so it would be confusing if Indent/Outdent
    // operated on that paragraph.
    // FIXME: We paint the gap before some paragraphs that are indented with left
    // margin/padding, but not others.  We should make the gap painting more consistent and
    // then use a left margin/padding rule here.
    if (visibleEnd != visibleStart && isStartOfParagraph(visibleEnd)) {
        VisibleSelection newSelection(visibleStart, visibleEnd.previous(CannotCrossEditingBoundary), endingSelection().isDirectional());
        if (newSelection.isNone())
            return;
        setEndingSelection(newSelection);
    }

    VisibleSelection selection = selectionForParagraphIteration(endingSelection());
    VisiblePosition startOfSelection = selection.visibleStart();
    VisiblePosition endOfSelection = selection.visibleEnd();
    ASSERT(!startOfSelection.isNull());
    ASSERT(!endOfSelection.isNull());
    RefPtrWillBeRawPtr<ContainerNode> startScope = nullptr;
    int startIndex = indexForVisiblePosition(startOfSelection, startScope);
    RefPtrWillBeRawPtr<ContainerNode> endScope = nullptr;
    int endIndex = indexForVisiblePosition(endOfSelection, endScope);

    formatSelection(startOfSelection, endOfSelection);

    document().updateLayoutIgnorePendingStylesheets();

    ASSERT(startScope == endScope);
    ASSERT(startIndex >= 0);
    ASSERT(startIndex <= endIndex);
    if (startScope == endScope && startIndex >= 0 && startIndex <= endIndex) {
        VisiblePosition start(visiblePositionForIndex(startIndex, startScope.get()));
        VisiblePosition end(visiblePositionForIndex(endIndex, endScope.get()));
        if (start.isNotNull() && end.isNotNull())
            setEndingSelection(VisibleSelection(start, end, endingSelection().isDirectional()));
    }
}

void ApplyBlockElementCommand::formatSelection(const VisiblePosition& startOfSelection, const VisiblePosition& endOfSelection)
{
    // Special case empty unsplittable elements because there's nothing to split
    // and there's nothing to move.
    Position start = startOfSelection.deepEquivalent().downstream();
    if (isAtUnsplittableElement(start)) {
        RefPtrWillBeRawPtr<HTMLElement> blockquote = createBlockElement();
        insertNodeAt(blockquote, start);
        RefPtrWillBeRawPtr<HTMLBRElement> placeholder = createBreakElement(document());
        appendNode(placeholder, blockquote);
        setEndingSelection(VisibleSelection(positionBeforeNode(placeholder.get()), DOWNSTREAM, endingSelection().isDirectional()));
        return;
    }

    RefPtrWillBeRawPtr<HTMLElement> blockquoteForNextIndent = nullptr;
    VisiblePosition endOfCurrentParagraph = endOfParagraph(startOfSelection);
    VisiblePosition endOfLastParagraph = endOfParagraph(endOfSelection);
    VisiblePosition endAfterSelection = endOfParagraph(endOfLastParagraph.next());
    m_endOfLastParagraph = endOfLastParagraph.deepEquivalent();

    bool atEnd = false;
    Position end;
    while (endOfCurrentParagraph != endAfterSelection && !atEnd) {
        if (endOfCurrentParagraph.deepEquivalent() == m_endOfLastParagraph)
            atEnd = true;

        rangeForParagraphSplittingTextNodesIfNeeded(endOfCurrentParagraph, start, end);
        endOfCurrentParagraph = VisiblePosition(end);

        Node* enclosingCell = enclosingNodeOfType(start, &isTableCell);
        VisiblePosition endOfNextParagraph = endOfNextParagrahSplittingTextNodesIfNeeded(endOfCurrentParagraph, start, end);

        formatRange(start, end, m_endOfLastParagraph, blockquoteForNextIndent);

        // Don't put the next paragraph in the blockquote we just created for this paragraph unless
        // the next paragraph is in the same cell.
        if (enclosingCell && enclosingCell != enclosingNodeOfType(endOfNextParagraph.deepEquivalent(), &isTableCell))
            blockquoteForNextIndent = nullptr;

        // indentIntoBlockquote could move more than one paragraph if the paragraph
        // is in a list item or a table. As a result, endAfterSelection could refer to a position
        // no longer in the document.
        if (endAfterSelection.isNotNull() && !endAfterSelection.deepEquivalent().inDocument())
            break;
        // Sanity check: Make sure our moveParagraph calls didn't remove endOfNextParagraph.deepEquivalent().deprecatedNode()
        // If somehow, e.g. mutation event handler, we did, return to prevent crashes.
        if (endOfNextParagraph.isNotNull() && !endOfNextParagraph.deepEquivalent().inDocument())
            return;
        endOfCurrentParagraph = endOfNextParagraph;
    }
}

static bool isNewLineAtPosition(const Position& position)
{
    Node* textNode = position.containerNode();
    int offset = position.offsetInContainerNode();
    if (!textNode || !textNode->isTextNode() || offset < 0 || offset >= textNode->maxCharacterOffset())
        return false;

    TrackExceptionState exceptionState;
    String textAtPosition = toText(textNode)->substringData(offset, 1, exceptionState);
    if (exceptionState.hadException())
        return false;

    return textAtPosition[0] == '\n';
}

static const ComputedStyle* computedStyleOfEnclosingTextNode(const Position& position)
{
    if (position.anchorType() != PositionAnchorType::OffsetInAnchor || !position.containerNode() || !position.containerNode()->isTextNode())
        return 0;
    return position.containerNode()->computedStyle();
}

void ApplyBlockElementCommand::rangeForParagraphSplittingTextNodesIfNeeded(const VisiblePosition& endOfCurrentParagraph, Position& start, Position& end)
{
    start = startOfParagraph(endOfCurrentParagraph).deepEquivalent();
    end = endOfCurrentParagraph.deepEquivalent();

    document().updateLayoutTreeIfNeeded();

    bool isStartAndEndOnSameNode = false;
    if (const ComputedStyle* startStyle = computedStyleOfEnclosingTextNode(start)) {
        isStartAndEndOnSameNode = computedStyleOfEnclosingTextNode(end) && start.containerNode() == end.containerNode();
        bool isStartAndEndOfLastParagraphOnSameNode = computedStyleOfEnclosingTextNode(m_endOfLastParagraph) && start.containerNode() == m_endOfLastParagraph.containerNode();

        // Avoid obtanining the start of next paragraph for start
        if (startStyle->preserveNewline() && isNewLineAtPosition(start) && !isNewLineAtPosition(start.previous()) && start.offsetInContainerNode() > 0)
            start = startOfParagraph(VisiblePosition(end.previous())).deepEquivalent();

        // If start is in the middle of a text node, split.
        if (!startStyle->collapseWhiteSpace() && start.offsetInContainerNode() > 0) {
            int startOffset = start.offsetInContainerNode();
            Text* startText = start.containerText();
            splitTextNode(startText, startOffset);
            start = firstPositionInNode(startText);
            if (isStartAndEndOnSameNode) {
                ASSERT(end.offsetInContainerNode() >= startOffset);
                end = Position(startText, end.offsetInContainerNode() - startOffset);
            }
            if (isStartAndEndOfLastParagraphOnSameNode) {
                ASSERT(m_endOfLastParagraph.offsetInContainerNode() >= startOffset);
                m_endOfLastParagraph = Position(startText, m_endOfLastParagraph.offsetInContainerNode() - startOffset);
            }
        }
    }

    document().updateLayoutTreeIfNeeded();

    if (const ComputedStyle* endStyle = computedStyleOfEnclosingTextNode(end)) {
        bool isEndAndEndOfLastParagraphOnSameNode = computedStyleOfEnclosingTextNode(m_endOfLastParagraph) && end.deprecatedNode() == m_endOfLastParagraph.deprecatedNode();
        // Include \n at the end of line if we're at an empty paragraph
        if (endStyle->preserveNewline() && start == end && end.offsetInContainerNode() < end.containerNode()->maxCharacterOffset()) {
            int endOffset = end.offsetInContainerNode();
            if (!isNewLineAtPosition(end.previous()) && isNewLineAtPosition(end))
                end = Position(end.containerText(), endOffset + 1);
            if (isEndAndEndOfLastParagraphOnSameNode && end.offsetInContainerNode() >= m_endOfLastParagraph.offsetInContainerNode())
                m_endOfLastParagraph = end;
        }

        // If end is in the middle of a text node, split.
        if (!endStyle->collapseWhiteSpace() && end.offsetInContainerNode() && end.offsetInContainerNode() < end.containerNode()->maxCharacterOffset()) {
            RefPtrWillBeRawPtr<Text> endContainer = end.containerText();
            splitTextNode(endContainer, end.offsetInContainerNode());
            if (isStartAndEndOnSameNode)
                start = firstPositionInOrBeforeNode(endContainer->previousSibling());
            if (isEndAndEndOfLastParagraphOnSameNode) {
                if (m_endOfLastParagraph.offsetInContainerNode() == end.offsetInContainerNode())
                    m_endOfLastParagraph = lastPositionInOrAfterNode(endContainer->previousSibling());
                else
                    m_endOfLastParagraph = Position(endContainer, m_endOfLastParagraph.offsetInContainerNode() - end.offsetInContainerNode());
            }
            end = lastPositionInNode(endContainer->previousSibling());
        }
    }
}

VisiblePosition ApplyBlockElementCommand::endOfNextParagrahSplittingTextNodesIfNeeded(VisiblePosition& endOfCurrentParagraph, Position& start, Position& end)
{
    VisiblePosition endOfNextParagraph = endOfParagraph(endOfCurrentParagraph.next());
    Position position = endOfNextParagraph.deepEquivalent();
    const ComputedStyle* style = computedStyleOfEnclosingTextNode(position);
    if (!style)
        return endOfNextParagraph;

    RefPtrWillBeRawPtr<Text> text = position.containerText();
    if (!style->preserveNewline() || !position.offsetInContainerNode() || !isNewLineAtPosition(firstPositionInNode(text.get())))
        return endOfNextParagraph;

    // \n at the beginning of the text node immediately following the current paragraph is trimmed by moveParagraphWithClones.
    // If endOfNextParagraph was pointing at this same text node, endOfNextParagraph will be shifted by one paragraph.
    // Avoid this by splitting "\n"
    splitTextNode(text, 1);

    if (text == start.containerNode() && text->previousSibling() && text->previousSibling()->isTextNode()) {
        ASSERT(start.offsetInContainerNode() < position.offsetInContainerNode());
        start = Position(toText(text->previousSibling()), start.offsetInContainerNode());
    }
    if (text == end.containerNode() && text->previousSibling() && text->previousSibling()->isTextNode()) {
        ASSERT(end.offsetInContainerNode() < position.offsetInContainerNode());
        end = Position(toText(text->previousSibling()), end.offsetInContainerNode());
    }
    if (text == m_endOfLastParagraph.containerNode()) {
        if (m_endOfLastParagraph.offsetInContainerNode() < position.offsetInContainerNode()) {
            // We can only fix endOfLastParagraph if the previous node was still text and hasn't been modified by script.
            if (text->previousSibling()->isTextNode()
                && static_cast<unsigned>(m_endOfLastParagraph.offsetInContainerNode()) <= toText(text->previousSibling())->length())
                m_endOfLastParagraph = Position(toText(text->previousSibling()), m_endOfLastParagraph.offsetInContainerNode());
        } else
            m_endOfLastParagraph = Position(text.get(), m_endOfLastParagraph.offsetInContainerNode() - 1);
    }

    return VisiblePosition(Position(text.get(), position.offsetInContainerNode() - 1));
}

PassRefPtrWillBeRawPtr<HTMLElement> ApplyBlockElementCommand::createBlockElement() const
{
    RefPtrWillBeRawPtr<HTMLElement> element = createHTMLElement(document(), m_tagName);
    if (m_inlineStyle.length())
        element->setAttribute(styleAttr, m_inlineStyle);
    return element.release();
}

DEFINE_TRACE(ApplyBlockElementCommand)
{
    visitor->trace(m_endOfLastParagraph);
    CompositeEditCommand::trace(visitor);
}

}
