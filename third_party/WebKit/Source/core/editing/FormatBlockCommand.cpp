/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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
#include "core/editing/FormatBlockCommand.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/dom/Element.h"
#include "core/dom/Range.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/html/HTMLBRElement.h"
#include "core/html/HTMLElement.h"

namespace blink {

using namespace HTMLNames;

static Node* enclosingBlockToSplitTreeTo(Node* startNode);
static bool isElementForFormatBlock(const QualifiedName& tagName);
static inline bool isElementForFormatBlock(Node* node)
{
    return node->isElementNode() && isElementForFormatBlock(toElement(node)->tagQName());
}

FormatBlockCommand::FormatBlockCommand(Document& document, const QualifiedName& tagName)
    : ApplyBlockElementCommand(document, tagName)
    , m_didApply(false)
{
}

void FormatBlockCommand::formatSelection(const VisiblePosition& startOfSelection, const VisiblePosition& endOfSelection)
{
    if (!isElementForFormatBlock(tagName()))
        return;
    ApplyBlockElementCommand::formatSelection(startOfSelection, endOfSelection);
    m_didApply = true;
}

void FormatBlockCommand::formatRange(const Position& start, const Position& end, const Position& endOfSelection, RefPtrWillBeRawPtr<HTMLElement>& blockElement)
{
    Element* refElement = enclosingBlockFlowElement(VisiblePosition(end));
    Element* root = editableRootForPosition(start);
    // Root is null for elements with contenteditable=false.
    if (!root || !refElement)
        return;

    Node* nodeToSplitTo = enclosingBlockToSplitTreeTo(start.deprecatedNode());
    RefPtrWillBeRawPtr<Node> outerBlock = (start.deprecatedNode() == nodeToSplitTo) ? start.deprecatedNode() : splitTreeToNode(start.deprecatedNode(), nodeToSplitTo).get();
    RefPtrWillBeRawPtr<Node> nodeAfterInsertionPosition = outerBlock;
    RefPtrWillBeRawPtr<Range> range = Range::create(document(), start, endOfSelection);

    if (isElementForFormatBlock(refElement->tagQName()) && VisiblePosition(start) == startOfBlock(VisiblePosition(start))
        && (VisiblePosition(end) == endOfBlock(VisiblePosition(end)) || isNodeVisiblyContainedWithin(*refElement, *range))
        && refElement != root && !root->isDescendantOf(refElement)) {
        // Already in a block element that only contains the current paragraph
        if (refElement->hasTagName(tagName()))
            return;
        nodeAfterInsertionPosition = refElement;
    }

    if (!blockElement) {
        // Create a new blockquote and insert it as a child of the root editable element. We accomplish
        // this by splitting all parents of the current paragraph up to that point.
        blockElement = createBlockElement();
        insertNodeBefore(blockElement, nodeAfterInsertionPosition);
    }

    Position lastParagraphInBlockNode = blockElement->lastChild() ? positionAfterNode(blockElement->lastChild()) : Position();
    bool wasEndOfParagraph = isEndOfParagraph(VisiblePosition(lastParagraphInBlockNode));

    moveParagraphWithClones(VisiblePosition(start), VisiblePosition(end), blockElement.get(), outerBlock.get());

    // Copy the inline style of the original block element to the newly created block-style element.
    if (outerBlock.get() != nodeAfterInsertionPosition.get() && toHTMLElement(nodeAfterInsertionPosition.get())->hasAttribute(styleAttr))
        blockElement->setAttribute(styleAttr, toHTMLElement(nodeAfterInsertionPosition.get())->getAttribute(styleAttr));

    if (wasEndOfParagraph && !isEndOfParagraph(VisiblePosition(lastParagraphInBlockNode)) && !isStartOfParagraph(VisiblePosition(lastParagraphInBlockNode)))
        insertBlockPlaceholder(lastParagraphInBlockNode);
}

Element* FormatBlockCommand::elementForFormatBlockCommand(Range* range)
{
    if (!range)
        return 0;

    Node* commonAncestor = range->commonAncestorContainer();
    while (commonAncestor && !isElementForFormatBlock(commonAncestor))
        commonAncestor = commonAncestor->parentNode();

    if (!commonAncestor)
        return 0;

    Element* rootEditableElement = range->startContainer()->rootEditableElement();
    if (!rootEditableElement || commonAncestor->contains(rootEditableElement))
        return 0;

    return commonAncestor->isElementNode() ? toElement(commonAncestor) : 0;
}

bool isElementForFormatBlock(const QualifiedName& tagName)
{
    DEFINE_STATIC_LOCAL(HashSet<QualifiedName>, blockTags, ());
    if (blockTags.isEmpty()) {
        blockTags.add(addressTag);
        blockTags.add(articleTag);
        blockTags.add(asideTag);
        blockTags.add(blockquoteTag);
        blockTags.add(ddTag);
        blockTags.add(divTag);
        blockTags.add(dlTag);
        blockTags.add(dtTag);
        blockTags.add(footerTag);
        blockTags.add(h1Tag);
        blockTags.add(h2Tag);
        blockTags.add(h3Tag);
        blockTags.add(h4Tag);
        blockTags.add(h5Tag);
        blockTags.add(h6Tag);
        blockTags.add(headerTag);
        blockTags.add(hgroupTag);
        blockTags.add(mainTag);
        blockTags.add(navTag);
        blockTags.add(pTag);
        blockTags.add(preTag);
        blockTags.add(sectionTag);
    }
    return blockTags.contains(tagName);
}

Node* enclosingBlockToSplitTreeTo(Node* startNode)
{
    Node* lastBlock = startNode;
    for (Node* n = startNode; n; n = n->parentNode()) {
        if (!n->hasEditableStyle())
            return lastBlock;
        if (isTableCell(n) || isHTMLBodyElement(*n) || !n->parentNode() || !n->parentNode()->hasEditableStyle() || isElementForFormatBlock(n))
            return n;
        if (isBlock(n))
            lastBlock = n;
        if (isHTMLListElement(n))
            return n->parentNode()->hasEditableStyle() ? n->parentNode() : n;
    }
    return lastBlock;
}

}
