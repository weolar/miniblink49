/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
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
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (IndentOutdentCommandINCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/editing/IndentOutdentCommand.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/editing/InsertListCommand.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/html/HTMLBRElement.h"
#include "core/html/HTMLElement.h"
#include "core/layout/LayoutObject.h"

namespace blink {

using namespace HTMLNames;

static bool isHTMLListOrBlockquoteElement(const Node* node)
{
    if (!node || !node->isHTMLElement())
        return false;
    const HTMLElement& element = toHTMLElement(*node);
    return isHTMLUListElement(element) || isHTMLOListElement(element) || element.hasTagName(blockquoteTag);
}

IndentOutdentCommand::IndentOutdentCommand(Document& document, EIndentType typeOfAction)
    : ApplyBlockElementCommand(document, blockquoteTag, "margin: 0 0 0 40px; border: none; padding: 0px;")
    , m_typeOfAction(typeOfAction)
{
}

bool IndentOutdentCommand::tryIndentingAsListItem(const Position& start, const Position& end)
{
    // If our selection is not inside a list, bail out.
    RefPtrWillBeRawPtr<Node> lastNodeInSelectedParagraph = start.deprecatedNode();
    RefPtrWillBeRawPtr<HTMLElement> listElement = enclosingList(lastNodeInSelectedParagraph.get());
    if (!listElement)
        return false;

    // Find the block that we want to indent.  If it's not a list item (e.g., a div inside a list item), we bail out.
    RefPtrWillBeRawPtr<Element> selectedListItem = enclosingBlock(lastNodeInSelectedParagraph.get());

    // FIXME: we need to deal with the case where there is no li (malformed HTML)
    if (!isHTMLLIElement(selectedListItem))
        return false;

    // FIXME: previousElementSibling does not ignore non-rendered content like <span></span>.  Should we?
    RefPtrWillBeRawPtr<Element> previousList = ElementTraversal::previousSibling(*selectedListItem);
    RefPtrWillBeRawPtr<Element> nextList = ElementTraversal::nextSibling(*selectedListItem);

    // We should calculate visible range in list item because inserting new
    // list element will change visibility of list item, e.g. :first-child
    // CSS selector.
    RefPtrWillBeRawPtr<HTMLElement> newList = toHTMLElement(document().createElement(listElement->tagQName(), false).get());
    insertNodeBefore(newList, selectedListItem.get());

    // We should clone all the children of the list item for indenting purposes. However, in case the current
    // selection does not encompass all its children, we need to explicitally handle the same. The original
    // list item too would require proper deletion in that case.
    if (end.anchorNode() == selectedListItem.get() || end.anchorNode()->isDescendantOf(selectedListItem->lastChild())) {
        moveParagraphWithClones(VisiblePosition(start), VisiblePosition(end), newList.get(), selectedListItem.get());
    } else {
        moveParagraphWithClones(VisiblePosition(start), VisiblePosition(positionAfterNode(selectedListItem->lastChild())), newList.get(), selectedListItem.get());
        removeNode(selectedListItem.get());
    }

    if (canMergeLists(previousList.get(), newList.get()))
        mergeIdenticalElements(previousList.get(), newList.get());
    if (canMergeLists(newList.get(), nextList.get()))
        mergeIdenticalElements(newList.get(), nextList.get());

    return true;
}

void IndentOutdentCommand::indentIntoBlockquote(const Position& start, const Position& end, RefPtrWillBeRawPtr<HTMLElement>& targetBlockquote)
{
    Element* enclosingCell = toElement(enclosingNodeOfType(start, &isTableCell));
    Element* elementToSplitTo;
    if (enclosingCell)
        elementToSplitTo = enclosingCell;
    else if (enclosingList(start.containerNode()))
        elementToSplitTo = enclosingBlock(start.containerNode());
    else
        elementToSplitTo = editableRootForPosition(start);

    if (!elementToSplitTo)
        return;

    RefPtrWillBeRawPtr<Node> outerBlock = (start.containerNode() == elementToSplitTo) ? start.containerNode() : splitTreeToNode(start.containerNode(), elementToSplitTo).get();

    VisiblePosition startOfContents(start);
    if (!targetBlockquote) {
        // Create a new blockquote and insert it as a child of the root editable element. We accomplish
        // this by splitting all parents of the current paragraph up to that point.
        targetBlockquote = createBlockElement();
        if (outerBlock == start.containerNode())
            insertNodeAt(targetBlockquote, start);
        else
            insertNodeBefore(targetBlockquote, outerBlock);
        startOfContents = VisiblePosition(positionInParentAfterNode(*targetBlockquote));
    }

    VisiblePosition endOfContents(end);
    if (startOfContents.isNull() || endOfContents.isNull())
        return;
    moveParagraphWithClones(startOfContents, endOfContents, targetBlockquote.get(), outerBlock.get());
}

void IndentOutdentCommand::outdentParagraph()
{
    VisiblePosition visibleStartOfParagraph = startOfParagraph(endingSelection().visibleStart());
    VisiblePosition visibleEndOfParagraph = endOfParagraph(visibleStartOfParagraph);

    HTMLElement* enclosingElement = toHTMLElement(enclosingNodeOfType(visibleStartOfParagraph.deepEquivalent(), &isHTMLListOrBlockquoteElement));
    if (!enclosingElement || !enclosingElement->parentNode()->hasEditableStyle()) // We can't outdent if there is no place to go!
        return;

    // Use InsertListCommand to remove the selection from the list
    if (isHTMLOListElement(*enclosingElement)) {
        applyCommandToComposite(InsertListCommand::create(document(), InsertListCommand::OrderedList));
        return;
    }
    if (isHTMLUListElement(*enclosingElement)) {
        applyCommandToComposite(InsertListCommand::create(document(), InsertListCommand::UnorderedList));
        return;
    }

    // The selection is inside a blockquote i.e. enclosingNode is a blockquote
    VisiblePosition positionInEnclosingBlock = VisiblePosition(firstPositionInNode(enclosingElement));
    // If the blockquote is inline, the start of the enclosing block coincides with
    // positionInEnclosingBlock.
    VisiblePosition startOfEnclosingBlock = (enclosingElement->layoutObject() && enclosingElement->layoutObject()->isInline()) ? positionInEnclosingBlock : startOfBlock(positionInEnclosingBlock);
    VisiblePosition lastPositionInEnclosingBlock = VisiblePosition(lastPositionInNode(enclosingElement));
    VisiblePosition endOfEnclosingBlock = endOfBlock(lastPositionInEnclosingBlock);
    if (visibleStartOfParagraph == startOfEnclosingBlock &&
        visibleEndOfParagraph == endOfEnclosingBlock) {
        // The blockquote doesn't contain anything outside the paragraph, so it can be totally removed.
        Node* splitPoint = enclosingElement->nextSibling();
        removeNodePreservingChildren(enclosingElement);
        // outdentRegion() assumes it is operating on the first paragraph of an enclosing blockquote, but if there are multiply nested blockquotes and we've
        // just removed one, then this assumption isn't true. By splitting the next containing blockquote after this node, we keep this assumption true
        if (splitPoint) {
            if (Element* splitPointParent = splitPoint->parentElement()) {
                if (splitPointParent->hasTagName(blockquoteTag)
                    && !splitPoint->hasTagName(blockquoteTag)
                    && splitPointParent->parentNode()->hasEditableStyle()) // We can't outdent if there is no place to go!
                    splitElement(splitPointParent, splitPoint);
            }
        }

        document().updateLayoutIgnorePendingStylesheets();
        visibleStartOfParagraph = VisiblePosition(visibleStartOfParagraph.deepEquivalent());
        visibleEndOfParagraph = VisiblePosition(visibleEndOfParagraph.deepEquivalent());
        if (visibleStartOfParagraph.isNotNull() && !isStartOfParagraph(visibleStartOfParagraph))
            insertNodeAt(createBreakElement(document()), visibleStartOfParagraph.deepEquivalent());
        if (visibleEndOfParagraph.isNotNull() && !isEndOfParagraph(visibleEndOfParagraph))
            insertNodeAt(createBreakElement(document()), visibleEndOfParagraph.deepEquivalent());

        return;
    }
    RefPtrWillBeRawPtr<Node> splitBlockquoteNode = enclosingElement;
    if (Element* enclosingBlockFlow = enclosingBlock(visibleStartOfParagraph.deepEquivalent().deprecatedNode())) {
        if (enclosingBlockFlow != enclosingElement) {
            splitBlockquoteNode = splitTreeToNode(enclosingBlockFlow, enclosingElement, true);
        } else {
            // We split the blockquote at where we start outdenting.
            Node* highestInlineNode = highestEnclosingNodeOfType(visibleStartOfParagraph.deepEquivalent(), isInline, CannotCrossEditingBoundary, enclosingBlockFlow);
            splitElement(enclosingElement, highestInlineNode ? highestInlineNode : visibleStartOfParagraph.deepEquivalent().deprecatedNode());
        }
    }
    VisiblePosition startOfParagraphToMove(startOfParagraph(visibleStartOfParagraph));
    VisiblePosition endOfParagraphToMove(endOfParagraph(visibleEndOfParagraph));
    if (startOfParagraphToMove.isNull() || endOfParagraphToMove.isNull())
        return;
    RefPtrWillBeRawPtr<HTMLBRElement> placeholder = createBreakElement(document());
    insertNodeBefore(placeholder, splitBlockquoteNode);
    moveParagraph(startOfParagraphToMove, endOfParagraphToMove, VisiblePosition(positionBeforeNode(placeholder.get())), true);
}

// FIXME: We should merge this function with ApplyBlockElementCommand::formatSelection
void IndentOutdentCommand::outdentRegion(const VisiblePosition& startOfSelection, const VisiblePosition& endOfSelection)
{
    VisiblePosition endOfCurrentParagraph = endOfParagraph(startOfSelection);
    VisiblePosition endOfLastParagraph = endOfParagraph(endOfSelection);

    if (endOfCurrentParagraph == endOfLastParagraph) {
        outdentParagraph();
        return;
    }

    Position originalSelectionEnd = endingSelection().end();
    VisiblePosition endAfterSelection = endOfParagraph(endOfLastParagraph.next());

    while (endOfCurrentParagraph != endAfterSelection) {
        VisiblePosition endOfNextParagraph = endOfParagraph(endOfCurrentParagraph.next());
        if (endOfCurrentParagraph == endOfLastParagraph)
            setEndingSelection(VisibleSelection(originalSelectionEnd, DOWNSTREAM));
        else
            setEndingSelection(endOfCurrentParagraph);

        outdentParagraph();

        // outdentParagraph could move more than one paragraph if the paragraph
        // is in a list item. As a result, endAfterSelection and endOfNextParagraph
        // could refer to positions no longer in the document.
        if (endAfterSelection.isNotNull() && !endAfterSelection.deepEquivalent().inDocument())
            break;

        if (endOfNextParagraph.isNotNull() && !endOfNextParagraph.deepEquivalent().inDocument()) {
            endOfCurrentParagraph = VisiblePosition(endingSelection().end());
            endOfNextParagraph = endOfParagraph(endOfCurrentParagraph.next());
        }
        endOfCurrentParagraph = endOfNextParagraph;
    }
}

void IndentOutdentCommand::formatSelection(const VisiblePosition& startOfSelection, const VisiblePosition& endOfSelection)
{
    if (m_typeOfAction == Indent)
        ApplyBlockElementCommand::formatSelection(startOfSelection, endOfSelection);
    else
        outdentRegion(startOfSelection, endOfSelection);
}

void IndentOutdentCommand::formatRange(const Position& start, const Position& end, const Position&, RefPtrWillBeRawPtr<HTMLElement>& blockquoteForNextIndent)
{
    if (tryIndentingAsListItem(start, end))
        blockquoteForNextIndent = nullptr;
    else
        indentIntoBlockquote(start, end, blockquoteForNextIndent);
}

}
