/*
 * Copyright (C) 2006, 2010 Apple Inc. All rights reserved.
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
#include "core/editing/InsertListCommand.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/ElementTraversal.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/html/HTMLBRElement.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLLIElement.h"
#include "core/html/HTMLUListElement.h"

namespace blink {

using namespace HTMLNames;

static Node* enclosingListChild(Node* node, Node* listNode)
{
    Node* listChild = enclosingListChild(node);
    while (listChild && enclosingList(listChild) != listNode)
        listChild = enclosingListChild(listChild->parentNode());
    return listChild;
}

HTMLUListElement* InsertListCommand::fixOrphanedListChild(Node* node)
{
    RefPtrWillBeRawPtr<HTMLUListElement> listElement = createUnorderedListElement(document());
    insertNodeBefore(listElement, node);
    removeNode(node);
    appendNode(node, listElement);
    m_listElement = listElement;
    return listElement.get();
}

PassRefPtrWillBeRawPtr<HTMLElement> InsertListCommand::mergeWithNeighboringLists(PassRefPtrWillBeRawPtr<HTMLElement> passedList)
{
    RefPtrWillBeRawPtr<HTMLElement> list = passedList;
    Element* previousList = ElementTraversal::previousSibling(*list);
    if (canMergeLists(previousList, list.get()))
        mergeIdenticalElements(previousList, list);

    if (!list)
        return nullptr;

    Element* nextSibling = ElementTraversal::nextSibling(*list);
    if (!nextSibling || !nextSibling->isHTMLElement())
        return list.release();

    RefPtrWillBeRawPtr<HTMLElement> nextList = toHTMLElement(nextSibling);
    if (canMergeLists(list.get(), nextList.get())) {
        mergeIdenticalElements(list, nextList);
        return nextList.release();
    }
    return list.release();
}

bool InsertListCommand::selectionHasListOfType(const VisibleSelection& selection, const HTMLQualifiedName& listTag)
{
    VisiblePosition start = selection.visibleStart();

    if (!enclosingList(start.deepEquivalent().deprecatedNode()))
        return false;

    VisiblePosition end = startOfParagraph(selection.visibleEnd());
    while (start.isNotNull() && start != end) {
        HTMLElement* listElement = enclosingList(start.deepEquivalent().deprecatedNode());
        if (!listElement || !listElement->hasTagName(listTag))
            return false;
        start = startOfNextParagraph(start);
    }

    return true;
}

InsertListCommand::InsertListCommand(Document& document, Type type)
    : CompositeEditCommand(document), m_type(type)
{
}

void InsertListCommand::doApply()
{
    if (!endingSelection().isNonOrphanedCaretOrRange())
        return;

    if (!endingSelection().rootEditableElement())
        return;

    VisiblePosition visibleEnd = endingSelection().visibleEnd();
    VisiblePosition visibleStart = endingSelection().visibleStart();
    // When a selection ends at the start of a paragraph, we rarely paint
    // the selection gap before that paragraph, because there often is no gap.
    // In a case like this, it's not obvious to the user that the selection
    // ends "inside" that paragraph, so it would be confusing if InsertUn{Ordered}List
    // operated on that paragraph.
    // FIXME: We paint the gap before some paragraphs that are indented with left
    // margin/padding, but not others.  We should make the gap painting more consistent and
    // then use a left margin/padding rule here.
    if (visibleEnd != visibleStart && isStartOfParagraph(visibleEnd, CanSkipOverEditingBoundary)) {
        setEndingSelection(VisibleSelection(visibleStart, visibleEnd.previous(CannotCrossEditingBoundary), endingSelection().isDirectional()));
        if (!endingSelection().rootEditableElement())
            return;
    }

    const HTMLQualifiedName& listTag = (m_type == OrderedList) ? olTag : ulTag;
    if (endingSelection().isRange()) {
        bool forceListCreation = false;
        VisibleSelection selection = selectionForParagraphIteration(endingSelection());
        ASSERT(selection.isRange());
        VisiblePosition startOfSelection = selection.visibleStart();
        VisiblePosition endOfSelection = selection.visibleEnd();
        VisiblePosition startOfLastParagraph = startOfParagraph(endOfSelection, CanSkipOverEditingBoundary);

        RefPtrWillBeRawPtr<Range> currentSelection = endingSelection().firstRange();
        RefPtrWillBeRawPtr<ContainerNode> scopeForStartOfSelection = nullptr;
        RefPtrWillBeRawPtr<ContainerNode> scopeForEndOfSelection = nullptr;
        // FIXME: This is an inefficient way to keep selection alive because
        // indexForVisiblePosition walks from the beginning of the document to the
        // endOfSelection everytime this code is executed. But not using index is hard
        // because there are so many ways we can los eselection inside doApplyForSingleParagraph.
        int indexForStartOfSelection = indexForVisiblePosition(startOfSelection, scopeForStartOfSelection);
        int indexForEndOfSelection = indexForVisiblePosition(endOfSelection, scopeForEndOfSelection);

        if (startOfParagraph(startOfSelection, CanSkipOverEditingBoundary) != startOfLastParagraph) {
            forceListCreation = !selectionHasListOfType(selection, listTag);

            VisiblePosition startOfCurrentParagraph = startOfSelection;
            while (startOfCurrentParagraph.isNotNull() && !inSameParagraph(startOfCurrentParagraph, startOfLastParagraph, CanCrossEditingBoundary)) {
                // doApply() may operate on and remove the last paragraph of the selection from the document
                // if it's in the same list item as startOfCurrentParagraph.  Return early to avoid an
                // infinite loop and because there is no more work to be done.
                // FIXME(<rdar://problem/5983974>): The endingSelection() may be incorrect here.  Compute
                // the new location of endOfSelection and use it as the end of the new selection.
                if (!startOfLastParagraph.deepEquivalent().inDocument())
                    return;
                setEndingSelection(startOfCurrentParagraph);

                // Save and restore endOfSelection and startOfLastParagraph when necessary
                // since moveParagraph and movePragraphWithClones can remove nodes.
                doApplyForSingleParagraph(forceListCreation, listTag, *currentSelection);
                if (endOfSelection.isNull() || endOfSelection.isOrphan() || startOfLastParagraph.isNull() || startOfLastParagraph.isOrphan()) {
                    endOfSelection = visiblePositionForIndex(indexForEndOfSelection, scopeForEndOfSelection.get());
                    // If endOfSelection is null, then some contents have been deleted from the document.
                    // This should never happen and if it did, exit early immediately because we've lost the loop invariant.
                    ASSERT(endOfSelection.isNotNull());
                    if (endOfSelection.isNull() || !endOfSelection.rootEditableElement())
                        return;
                    startOfLastParagraph = startOfParagraph(endOfSelection, CanSkipOverEditingBoundary);
                }

                startOfCurrentParagraph = startOfNextParagraph(endingSelection().visibleStart());
            }
            setEndingSelection(endOfSelection);
        }
        doApplyForSingleParagraph(forceListCreation, listTag, *currentSelection);
        // Fetch the end of the selection, for the reason mentioned above.
        if (endOfSelection.isNull() || endOfSelection.isOrphan()) {
            endOfSelection = visiblePositionForIndex(indexForEndOfSelection, scopeForEndOfSelection.get());
            if (endOfSelection.isNull())
                return;
        }
        if (startOfSelection.isNull() || startOfSelection.isOrphan()) {
            startOfSelection = visiblePositionForIndex(indexForStartOfSelection, scopeForStartOfSelection.get());
            if (startOfSelection.isNull())
                return;
        }
        setEndingSelection(VisibleSelection(startOfSelection, endOfSelection, endingSelection().isDirectional()));
        return;
    }

    ASSERT(endingSelection().firstRange());
    doApplyForSingleParagraph(false, listTag, *endingSelection().firstRange());
}

void InsertListCommand::doApplyForSingleParagraph(bool forceCreateList, const HTMLQualifiedName& listTag, Range& currentSelection)
{
    // FIXME: This will produce unexpected results for a selection that starts just before a
    // table and ends inside the first cell, selectionForParagraphIteration should probably
    // be renamed and deployed inside setEndingSelection().
    Node* selectionNode = endingSelection().start().deprecatedNode();
    Node* listChildNode = enclosingListChild(selectionNode);
    bool switchListType = false;
    if (listChildNode) {
        // Remove the list chlild.
        RefPtrWillBeRawPtr<HTMLElement> listElement = enclosingList(listChildNode);
        if (!listElement) {
            listElement = fixOrphanedListChild(listChildNode);
            listElement = mergeWithNeighboringLists(listElement);
        }
        if (!listElement->hasTagName(listTag))
            // listChildNode will be removed from the list and a list of type m_type will be created.
            switchListType = true;

        // If the list is of the desired type, and we are not removing the list, then exit early.
        if (!switchListType && forceCreateList)
            return;

        // If the entire list is selected, then convert the whole list.
        if (switchListType && isNodeVisiblyContainedWithin(*listElement, currentSelection)) {
            bool rangeStartIsInList = visiblePositionBeforeNode(*listElement) == VisiblePosition(currentSelection.startPosition());
            bool rangeEndIsInList = visiblePositionAfterNode(*listElement) == VisiblePosition(currentSelection.endPosition());

            RefPtrWillBeRawPtr<HTMLElement> newList = createHTMLElement(document(), listTag);
            insertNodeBefore(newList, listElement);

            Node* firstChildInList = enclosingListChild(VisiblePosition(firstPositionInNode(listElement.get())).deepEquivalent().deprecatedNode(), listElement.get());
            Element* outerBlock = firstChildInList && isBlockFlowElement(*firstChildInList) ? toElement(firstChildInList) : listElement.get();

            moveParagraphWithClones(VisiblePosition(firstPositionInNode(listElement.get())), VisiblePosition(lastPositionInNode(listElement.get())), newList.get(), outerBlock);

            // Manually remove listNode because moveParagraphWithClones sometimes leaves it behind in the document.
            // See the bug 33668 and editing/execCommand/insert-list-orphaned-item-with-nested-lists.html.
            // FIXME: This might be a bug in moveParagraphWithClones or deleteSelection.
            if (listElement && listElement->inDocument())
                removeNode(listElement);

            newList = mergeWithNeighboringLists(newList);

            // Restore the start and the end of current selection if they started inside listNode
            // because moveParagraphWithClones could have removed them.
            if (rangeStartIsInList && newList)
                currentSelection.setStart(newList, 0, IGNORE_EXCEPTION);
            if (rangeEndIsInList && newList)
                currentSelection.setEnd(newList, lastOffsetInNode(newList.get()), IGNORE_EXCEPTION);

            setEndingSelection(VisiblePosition(firstPositionInNode(newList.get())));

            return;
        }

        unlistifyParagraph(endingSelection().visibleStart(), listElement.get(), listChildNode);
    }

    if (!listChildNode || switchListType || forceCreateList)
        m_listElement = listifyParagraph(endingSelection().visibleStart(), listTag);
}

void InsertListCommand::unlistifyParagraph(const VisiblePosition& originalStart, HTMLElement* listElement, Node* listChildNode)
{
    Node* nextListChild;
    Node* previousListChild;
    VisiblePosition start;
    VisiblePosition end;
    ASSERT(listChildNode);
    if (isHTMLLIElement(*listChildNode)) {
        start = VisiblePosition(firstPositionInNode(listChildNode));
        end = VisiblePosition(lastPositionInNode(listChildNode));
        nextListChild = listChildNode->nextSibling();
        previousListChild = listChildNode->previousSibling();
    } else {
        // A paragraph is visually a list item minus a list marker.  The paragraph will be moved.
        start = startOfParagraph(originalStart, CanSkipOverEditingBoundary);
        end = endOfParagraph(start, CanSkipOverEditingBoundary);
        nextListChild = enclosingListChild(end.next().deepEquivalent().deprecatedNode(), listElement);
        ASSERT(nextListChild != listChildNode);
        previousListChild = enclosingListChild(start.previous().deepEquivalent().deprecatedNode(), listElement);
        ASSERT(previousListChild != listChildNode);
    }
    // When removing a list, we must always create a placeholder to act as a point of insertion
    // for the list content being removed.
    RefPtrWillBeRawPtr<HTMLBRElement> placeholder = createBreakElement(document());
    RefPtrWillBeRawPtr<HTMLElement> elementToInsert = placeholder;
    // If the content of the list item will be moved into another list, put it in a list item
    // so that we don't create an orphaned list child.
    if (enclosingList(listElement)) {
        elementToInsert = createListItemElement(document());
        appendNode(placeholder, elementToInsert);
    }

    if (nextListChild && previousListChild) {
        // We want to pull listChildNode out of listNode, and place it before nextListChild
        // and after previousListChild, so we split listNode and insert it between the two lists.
        // But to split listNode, we must first split ancestors of listChildNode between it and listNode,
        // if any exist.
        // FIXME: We appear to split at nextListChild as opposed to listChildNode so that when we remove
        // listChildNode below in moveParagraphs, previousListChild will be removed along with it if it is
        // unrendered. But we ought to remove nextListChild too, if it is unrendered.
        splitElement(listElement, splitTreeToNode(nextListChild, listElement));
        insertNodeBefore(elementToInsert, listElement);
    } else if (nextListChild || listChildNode->parentNode() != listElement) {
        // Just because listChildNode has no previousListChild doesn't mean there isn't any content
        // in listNode that comes before listChildNode, as listChildNode could have ancestors
        // between it and listNode. So, we split up to listNode before inserting the placeholder
        // where we're about to move listChildNode to.
        if (listChildNode->parentNode() != listElement)
            splitElement(listElement, splitTreeToNode(listChildNode, listElement).get());
        insertNodeBefore(elementToInsert, listElement);
    } else {
        insertNodeAfter(elementToInsert, listElement);
    }

    VisiblePosition insertionPoint = VisiblePosition(positionBeforeNode(placeholder.get()));
    moveParagraphs(start, end, insertionPoint, /* preserveSelection */ true, /* preserveStyle */ true, listChildNode);
}

static HTMLElement* adjacentEnclosingList(const VisiblePosition& pos, const VisiblePosition& adjacentPos, const HTMLQualifiedName& listTag)
{
    HTMLElement* listElement = outermostEnclosingList(adjacentPos.deepEquivalent().deprecatedNode());

    if (!listElement)
        return 0;

    Element* previousCell = enclosingTableCell(pos.deepEquivalent());
    Element* currentCell = enclosingTableCell(adjacentPos.deepEquivalent());

    if (!listElement->hasTagName(listTag)
        || listElement->contains(pos.deepEquivalent().deprecatedNode())
        || previousCell != currentCell
        || enclosingList(listElement) != enclosingList(pos.deepEquivalent().deprecatedNode()))
        return 0;

    return listElement;
}

PassRefPtrWillBeRawPtr<HTMLElement> InsertListCommand::listifyParagraph(const VisiblePosition& originalStart, const HTMLQualifiedName& listTag)
{
    VisiblePosition start = startOfParagraph(originalStart, CanSkipOverEditingBoundary);
    VisiblePosition end = endOfParagraph(start, CanSkipOverEditingBoundary);

    if (start.isNull() || end.isNull())
        return nullptr;

    // Check for adjoining lists.
    RefPtrWillBeRawPtr<HTMLElement> listItemElement = createListItemElement(document());
    RefPtrWillBeRawPtr<HTMLBRElement> placeholder = createBreakElement(document());
    appendNode(placeholder, listItemElement);

    // Place list item into adjoining lists.
    HTMLElement* previousList = adjacentEnclosingList(start, start.previous(CannotCrossEditingBoundary), listTag);
    HTMLElement* nextList = adjacentEnclosingList(start, end.next(CannotCrossEditingBoundary), listTag);
    RefPtrWillBeRawPtr<HTMLElement> listElement = nullptr;
    if (previousList)
        appendNode(listItemElement, previousList);
    else if (nextList)
        insertNodeAt(listItemElement, positionBeforeNode(nextList));
    else {
        // Create the list.
        listElement = createHTMLElement(document(), listTag);
        appendNode(listItemElement, listElement);

        if (start == end && isBlock(start.deepEquivalent().deprecatedNode())) {
            // Inserting the list into an empty paragraph that isn't held open
            // by a br or a '\n', will invalidate start and end.  Insert
            // a placeholder and then recompute start and end.
            RefPtrWillBeRawPtr<HTMLBRElement> placeholder = insertBlockPlaceholder(start.deepEquivalent());
            start = VisiblePosition(positionBeforeNode(placeholder.get()));
            end = start;
        }

        // Insert the list at a position visually equivalent to start of the
        // paragraph that is being moved into the list.
        // Try to avoid inserting it somewhere where it will be surrounded by
        // inline ancestors of start, since it is easier for editing to produce
        // clean markup when inline elements are pushed down as far as possible.
        Position insertionPos(start.deepEquivalent().upstream());
        // Also avoid the containing list item.
        Node* listChild = enclosingListChild(insertionPos.deprecatedNode());
        if (isHTMLLIElement(listChild))
            insertionPos = positionInParentBeforeNode(*listChild);

        insertNodeAt(listElement, insertionPos);

        // We inserted the list at the start of the content we're about to move
        // Update the start of content, so we don't try to move the list into itself.  bug 19066
        // Layout is necessary since start's node's inline layoutObjects may have been destroyed by the insertion
        // The end of the content may have changed after the insertion and layout so update it as well.
        if (insertionPos == start.deepEquivalent())
            start = originalStart;
    }

    // Inserting list element and list item list may change start of pargraph
    // to move. We calculate start of paragraph again.
    document().updateLayoutIgnorePendingStylesheets();
    start = startOfParagraph(start, CanSkipOverEditingBoundary);
    end = endOfParagraph(start, CanSkipOverEditingBoundary);
    moveParagraph(start, end, VisiblePosition(positionBeforeNode(placeholder.get())), true);

    if (listElement)
        return mergeWithNeighboringLists(listElement);

    if (canMergeLists(previousList, nextList))
        mergeIdenticalElements(previousList, nextList);

    return listElement;
}

DEFINE_TRACE(InsertListCommand)
{
    visitor->trace(m_listElement);
    CompositeEditCommand::trace(visitor);
}

}
