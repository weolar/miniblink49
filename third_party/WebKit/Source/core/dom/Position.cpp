/*
 * Copyright (C) 2004, 2005, 2006, 2009 Apple Inc. All rights reserved.
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
#include "core/dom/Position.h"

#include "core/HTMLNames.h"
#include "core/css/CSSComputedStyleDeclaration.h"
#include "core/dom/PositionIterator.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/editing/VisiblePosition.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLTableElement.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutText.h"
#include "core/layout/line/InlineIterator.h"
#include "core/layout/line/InlineTextBox.h"
#include "platform/Logging.h"
#include "wtf/text/CString.h"
#include "wtf/text/CharacterNames.h"
#include <stdio.h>

namespace blink {

using namespace HTMLNames;

static Node* nextRenderedEditable(Node* node)
{
    for (node = nextAtomicLeafNode(*node); node; node = nextAtomicLeafNode(*node)) {
        LayoutObject* layoutObject = node->layoutObject();
        if (!layoutObject)
            continue;
        if (!node->hasEditableStyle())
            continue;
        if ((layoutObject->isBox() && toLayoutBox(layoutObject)->inlineBoxWrapper()) || (layoutObject->isText() && toLayoutText(layoutObject)->firstTextBox()))
            return node;
    }
    return 0;
}

static Node* previousRenderedEditable(Node* node)
{
    for (node = previousAtomicLeafNode(*node); node; node = previousAtomicLeafNode(*node)) {
        LayoutObject* layoutObject = node->layoutObject();
        if (!layoutObject)
            continue;
        if (!node->hasEditableStyle())
            continue;
        if ((layoutObject->isBox() && toLayoutBox(layoutObject)->inlineBoxWrapper()) || (layoutObject->isText() && toLayoutText(layoutObject)->firstTextBox()))
            return node;
    }
    return 0;
}

template <typename Strategy>
const TreeScope* PositionAlgorithm<Strategy>::commonAncestorTreeScope(const PositionAlgorithm<Strategy>& a, const PositionAlgorithm<Strategy>& b)
{
    if (!a.containerNode() || !b.containerNode())
        return nullptr;
    return a.containerNode()->treeScope().commonAncestorTreeScope(b.containerNode()->treeScope());
}

template <typename Strategy>
PositionAlgorithm<Strategy>::PositionAlgorithm(PassRefPtrWillBeRawPtr<Node> anchorNode, LegacyEditingOffset offset)
    : m_anchorNode(anchorNode)
    , m_offset(offset.value())
    , m_anchorType(anchorTypeForLegacyEditingPosition(m_anchorNode.get(), m_offset))
    , m_isLegacyEditingPosition(true)
{
    ASSERT(!m_anchorNode || !m_anchorNode->isPseudoElement() || m_anchorNode->isFirstLetterPseudoElement());
}

template <typename Strategy>
PositionAlgorithm<Strategy>::PositionAlgorithm(PassRefPtrWillBeRawPtr<Node> anchorNode, PositionAnchorType anchorType)
    : m_anchorNode(anchorNode)
    , m_offset(0)
    , m_anchorType(anchorType)
    , m_isLegacyEditingPosition(false)
{
    ASSERT(!m_anchorNode || !m_anchorNode->isPseudoElement() || m_anchorNode->isFirstLetterPseudoElement());
    ASSERT(m_anchorType != PositionAnchorType::OffsetInAnchor);
}

template <typename Strategy>
PositionAlgorithm<Strategy>::PositionAlgorithm(PassRefPtrWillBeRawPtr<Node> anchorNode, int offset)
    : m_anchorNode(anchorNode)
    , m_offset(offset)
    , m_anchorType(PositionAnchorType::OffsetInAnchor)
    , m_isLegacyEditingPosition(false)
{
    ASSERT(!m_anchorNode || !m_anchorNode->isPseudoElement() || m_anchorNode->isFirstLetterPseudoElement());
}

template <typename Strategy>
PositionAlgorithm<Strategy>::PositionAlgorithm(const PositionAlgorithm& other)
    : m_anchorNode(other.m_anchorNode)
    , m_offset(other.m_offset)
    , m_anchorType(other.m_anchorType)
    , m_isLegacyEditingPosition(other.m_isLegacyEditingPosition)
{
}

// --

template <typename Strategy>
void PositionAlgorithm<Strategy>::moveToPosition(PassRefPtrWillBeRawPtr<Node> node, int offset)
{
    ASSERT(!Strategy::editingIgnoresContent(node.get()));
    ASSERT(anchorType() == PositionAnchorType::OffsetInAnchor || m_isLegacyEditingPosition);
    m_anchorNode = node;
    m_offset = offset;
    if (m_isLegacyEditingPosition)
        m_anchorType = anchorTypeForLegacyEditingPosition(m_anchorNode.get(), m_offset);
}
template <typename Strategy>
void PositionAlgorithm<Strategy>::moveToOffset(int offset)
{
    ASSERT(anchorType() == PositionAnchorType::OffsetInAnchor || m_isLegacyEditingPosition);
    m_offset = offset;
    if (m_isLegacyEditingPosition)
        m_anchorType = anchorTypeForLegacyEditingPosition(m_anchorNode.get(), m_offset);
}

template <typename Strategy>
Node* PositionAlgorithm<Strategy>::containerNode() const
{
    if (!m_anchorNode)
        return 0;

    switch (anchorType()) {
    case PositionAnchorType::BeforeChildren:
    case PositionAnchorType::AfterChildren:
    case PositionAnchorType::OffsetInAnchor:
        return m_anchorNode.get();
    case PositionAnchorType::BeforeAnchor:
    case PositionAnchorType::AfterAnchor:
        return Strategy::parent(*m_anchorNode);
    }
    ASSERT_NOT_REACHED();
    return 0;
}

template <typename Strategy>
Text* PositionAlgorithm<Strategy>::containerText() const
{
    switch (anchorType()) {
    case PositionAnchorType::OffsetInAnchor:
        return m_anchorNode && m_anchorNode->isTextNode() ? toText(m_anchorNode) : 0;
    case PositionAnchorType::BeforeAnchor:
    case PositionAnchorType::AfterAnchor:
        return 0;
    case PositionAnchorType::BeforeChildren:
    case PositionAnchorType::AfterChildren:
        ASSERT(!m_anchorNode || !m_anchorNode->isTextNode());
        return 0;
    }
    ASSERT_NOT_REACHED();
    return 0;
}

template <typename Strategy>
int PositionAlgorithm<Strategy>::computeOffsetInContainerNode() const
{
    if (!m_anchorNode)
        return 0;

    switch (anchorType()) {
    case PositionAnchorType::BeforeChildren:
        return 0;
    case PositionAnchorType::AfterChildren:
        return lastOffsetInNode(m_anchorNode.get());
    case PositionAnchorType::OffsetInAnchor:
        return minOffsetForNode(m_anchorNode.get(), m_offset);
    case PositionAnchorType::BeforeAnchor:
        return Strategy::index(*m_anchorNode);
    case PositionAnchorType::AfterAnchor:
        return Strategy::index(*m_anchorNode) + 1;
    }
    ASSERT_NOT_REACHED();
    return 0;
}

template <typename Strategy>
int PositionAlgorithm<Strategy>::offsetForPositionAfterAnchor() const
{
    ASSERT(m_anchorType == PositionAnchorType::AfterAnchor || m_anchorType == PositionAnchorType::AfterChildren);
    ASSERT(!m_isLegacyEditingPosition);
    return Strategy::lastOffsetForEditing(m_anchorNode.get());
}

// Neighbor-anchored positions are invalid DOM positions, so they need to be
// fixed up before handing them off to the Range object.
template <typename Strategy>
PositionAlgorithm<Strategy> PositionAlgorithm<Strategy>::parentAnchoredEquivalent() const
{
    if (!m_anchorNode)
        return PositionAlgorithm<Strategy>();

    // FIXME: This should only be necessary for legacy positions, but is also needed for positions before and after Tables
    if (m_offset <= 0 && (m_anchorType != PositionAnchorType::AfterAnchor && m_anchorType != PositionAnchorType::AfterChildren)) {
        if (Strategy::parent(*m_anchorNode) && (Strategy::editingIgnoresContent(m_anchorNode.get()) || isRenderedHTMLTableElement(m_anchorNode.get())))
            return inParentBeforeNode(*m_anchorNode);
        return PositionAlgorithm<Strategy>(m_anchorNode.get(), 0);
    }
    if (!m_anchorNode->offsetInCharacters()
        && (m_anchorType == PositionAnchorType::AfterAnchor || m_anchorType == PositionAnchorType::AfterChildren || static_cast<unsigned>(m_offset) == m_anchorNode->countChildren())
        && (Strategy::editingIgnoresContent(m_anchorNode.get()) || isRenderedHTMLTableElement(m_anchorNode.get()))
        && containerNode()) {
        return inParentAfterNode(*m_anchorNode);
    }

    return PositionAlgorithm<Strategy>(containerNode(), computeOffsetInContainerNode());
}

template <typename Strategy>
PositionAlgorithm<Strategy> PositionAlgorithm<Strategy>::toOffsetInAnchor() const
{
    if (isNull())
        return PositionAlgorithm<Strategy>();

    return PositionAlgorithm<Strategy>(containerNode(), computeOffsetInContainerNode());
}

template <typename Strategy>
Node* PositionAlgorithm<Strategy>::computeNodeBeforePosition() const
{
    if (!m_anchorNode)
        return 0;
    switch (anchorType()) {
    case PositionAnchorType::BeforeChildren:
        return 0;
    case PositionAnchorType::AfterChildren:
        return Strategy::lastChild(*m_anchorNode);
    case PositionAnchorType::OffsetInAnchor:
        return m_offset ? Strategy::childAt(*m_anchorNode, m_offset - 1) : 0;
    case PositionAnchorType::BeforeAnchor:
        return Strategy::previousSibling(*m_anchorNode);
    case PositionAnchorType::AfterAnchor:
        return m_anchorNode.get();
    }
    ASSERT_NOT_REACHED();
    return 0;
}

template <typename Strategy>
Node* PositionAlgorithm<Strategy>::computeNodeAfterPosition() const
{
    if (!m_anchorNode)
        return 0;

    switch (anchorType()) {
    case PositionAnchorType::BeforeChildren:
        return Strategy::firstChild(*m_anchorNode);
    case PositionAnchorType::AfterChildren:
        return 0;
    case PositionAnchorType::OffsetInAnchor:
        return Strategy::childAt(*m_anchorNode, m_offset);
    case PositionAnchorType::BeforeAnchor:
        return m_anchorNode.get();
    case PositionAnchorType::AfterAnchor:
        return Strategy::nextSibling(*m_anchorNode);
    }
    ASSERT_NOT_REACHED();
    return 0;
}

// An implementation of |Range::firstNode()|.
template <typename Strategy>
Node* PositionAlgorithm<Strategy>::nodeAsRangeFirstNode() const
{
    if (!m_anchorNode)
        return nullptr;
    if (m_anchorType != PositionAnchorType::OffsetInAnchor)
        return toOffsetInAnchor().nodeAsRangeFirstNode();
    if (m_anchorNode->offsetInCharacters())
        return m_anchorNode.get();
    if (Node* child = Strategy::childAt(*m_anchorNode, m_offset))
        return child;
    if (!m_offset)
        return m_anchorNode.get();
    return Strategy::nextSkippingChildren(*m_anchorNode);
}

template <typename Strategy>
Node* PositionAlgorithm<Strategy>::nodeAsRangeLastNode() const
{
    if (isNull())
        return nullptr;
    if (Node* pastLastNode = nodeAsRangePastLastNode())
        return Strategy::previous(*pastLastNode);
    return &Strategy::lastWithinOrSelf(*containerNode());
}

// An implementation of |Range::pastLastNode()|.
template <typename Strategy>
Node* PositionAlgorithm<Strategy>::nodeAsRangePastLastNode() const
{
    if (!m_anchorNode)
        return nullptr;
    if (m_anchorType != PositionAnchorType::OffsetInAnchor)
        return toOffsetInAnchor().nodeAsRangePastLastNode();
    if (m_anchorNode->offsetInCharacters())
        return Strategy::nextSkippingChildren(*m_anchorNode);
    if (Node* child = Strategy::childAt(*m_anchorNode, m_offset))
        return child;
    return Strategy::nextSkippingChildren(*m_anchorNode);
}

template <typename Strategy>
Node* PositionAlgorithm<Strategy>::commonAncestorContainer(const PositionAlgorithm<Strategy>& other) const
{
    return Strategy::commonAncestor(*containerNode(), *other.containerNode());
}

template <typename Strategy>
PositionAnchorType PositionAlgorithm<Strategy>::anchorTypeForLegacyEditingPosition(Node* anchorNode, int offset)
{
    if (anchorNode && Strategy::editingIgnoresContent(anchorNode)) {
        if (offset == 0)
            return PositionAnchorType::BeforeAnchor;
        return PositionAnchorType::AfterAnchor;
    }
    return PositionAnchorType::OffsetInAnchor;
}

// FIXME: This method is confusing (does it return anchorNode() or containerNode()?) and should be renamed or removed
template <typename Strategy>
Element* PositionAlgorithm<Strategy>::element() const
{
    Node* node = anchorNode();
    if (!node || node->isElementNode())
        return toElement(node);
    ContainerNode* parent = Strategy::parent(*node);
    return parent && parent->isElementNode() ? toElement(parent) : nullptr;
}

template <typename Strategy>
PassRefPtrWillBeRawPtr<CSSComputedStyleDeclaration> PositionAlgorithm<Strategy>::ensureComputedStyle() const
{
    Element* elem = element();
    if (!elem)
        return nullptr;
    return CSSComputedStyleDeclaration::create(elem);
}

int comparePositions(const PositionInComposedTree& positionA, const PositionInComposedTree& positionB)
{
    ASSERT(positionA.isNotNull());
    ASSERT(positionB.isNotNull());

    Node* containerA = positionA.containerNode();
    Node* containerB = positionB.containerNode();
    int offsetA = positionA.computeOffsetInContainerNode();
    int offsetB = positionB.computeOffsetInContainerNode();
    return comparePositionsInComposedTree(containerA, offsetA, containerB, offsetB);
}

template <typename Strategy>
int PositionAlgorithm<Strategy>::compareTo(const PositionAlgorithm<Strategy>& other) const
{
    return comparePositions(*this, other);
}

template <typename Strategy>
PositionAlgorithm<Strategy> PositionAlgorithm<Strategy>::previous(PositionMoveType moveType) const
{
    Node* node = deprecatedNode();
    if (!node)
        return PositionAlgorithm<Strategy>(*this);

    int offset = deprecatedEditingOffset();
    // FIXME: Negative offsets shouldn't be allowed. We should catch this earlier.
    ASSERT(offset >= 0);

    if (offset > 0) {
        if (Node* child = Strategy::childAt(*node, offset - 1))
            return lastPositionInOrAfterNode(child);

        // There are two reasons child might be 0:
        //   1) The node is node like a text node that is not an element, and therefore has no children.
        //      Going backward one character at a time is correct.
        //   2) The old offset was a bogus offset like (<br>, 1), and there is no child.
        //      Going from 1 to 0 is correct.
        switch (moveType) {
        case CodePoint:
            return createLegacyEditingPosition(node, offset - 1);
        case Character:
            return createLegacyEditingPosition(node, uncheckedPreviousOffset(node, offset));
        case BackwardDeletion:
            return createLegacyEditingPosition(node, uncheckedPreviousOffsetForBackwardDeletion(node, offset));
        }
    }

    if (ContainerNode* parent = Strategy::parent(*node))
        return createLegacyEditingPosition(parent, node->nodeIndex());
    return PositionAlgorithm<Strategy>(*this);
}

template <typename Strategy>
PositionAlgorithm<Strategy> PositionAlgorithm<Strategy>::next(PositionMoveType moveType) const
{
    ASSERT(moveType != BackwardDeletion);

    Node* node = deprecatedNode();
    if (!node)
        return PositionAlgorithm<Strategy>(*this);

    int offset = deprecatedEditingOffset();
    // FIXME: Negative offsets shouldn't be allowed. We should catch this earlier.
    ASSERT(offset >= 0);

    if (Node* child = Strategy::childAt(*node, offset))
        return firstPositionInOrBeforeNode(child);

    if (!Strategy::hasChildren(*node) && offset < lastOffsetForEditing(node)) {
        // There are two reasons child might be 0:
        //   1) The node is node like a text node that is not an element, and therefore has no children.
        //      Going forward one character at a time is correct.
        //   2) The new offset is a bogus offset like (<br>, 1), and there is no child.
        //      Going from 0 to 1 is correct.
        return createLegacyEditingPosition(node, (moveType == Character) ? uncheckedNextOffset(node, offset) : offset + 1);
    }

    if (ContainerNode* parent = Strategy::parent(*node))
        return createLegacyEditingPosition(parent, node->nodeIndex() + 1);
    return PositionAlgorithm<Strategy>(*this);
}

template <typename Strategy>
int PositionAlgorithm<Strategy>::uncheckedPreviousOffset(const Node* n, int current)
{
    return n->layoutObject() ? n->layoutObject()->previousOffset(current) : current - 1;
}

template <typename Strategy>
int PositionAlgorithm<Strategy>::uncheckedPreviousOffsetForBackwardDeletion(const Node* n, int current)
{
    return n->layoutObject() ? n->layoutObject()->previousOffsetForBackwardDeletion(current) : current - 1;
}

template <typename Strategy>
int PositionAlgorithm<Strategy>::uncheckedNextOffset(const Node* n, int current)
{
    return n->layoutObject() ? n->layoutObject()->nextOffset(current) : current + 1;
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::atFirstEditingPositionForNode() const
{
    if (isNull())
        return true;
    // FIXME: Position before anchor shouldn't be considered as at the first editing position for node
    // since that position resides outside of the node.
    switch (m_anchorType) {
    case PositionAnchorType::OffsetInAnchor:
        return m_offset <= 0;
    case PositionAnchorType::BeforeChildren:
    case PositionAnchorType::BeforeAnchor:
        return true;
    case PositionAnchorType::AfterChildren:
    case PositionAnchorType::AfterAnchor:
        return !lastOffsetForEditing(deprecatedNode());
    }
    ASSERT_NOT_REACHED();
    return false;
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::atLastEditingPositionForNode() const
{
    if (isNull())
        return true;
    // FIXME: Position after anchor shouldn't be considered as at the first editing position for node
    // since that position resides outside of the node.
    return m_anchorType == PositionAnchorType::AfterAnchor || m_anchorType == PositionAnchorType::AfterChildren || m_offset >= lastOffsetForEditing(deprecatedNode());
}

// A position is considered at editing boundary if one of the following is true:
// 1. It is the first position in the node and the next visually equivalent position
//    is non editable.
// 2. It is the last position in the node and the previous visually equivalent position
//    is non editable.
// 3. It is an editable position and both the next and previous visually equivalent
//    positions are both non editable.
template <typename Strategy>
bool PositionAlgorithm<Strategy>::atEditingBoundary() const
{
    PositionAlgorithm<Strategy> nextPosition = downstream(CanCrossEditingBoundary);
    if (atFirstEditingPositionForNode() && nextPosition.isNotNull() && !nextPosition.deprecatedNode()->hasEditableStyle())
        return true;

    PositionAlgorithm<Strategy> prevPosition = upstream(CanCrossEditingBoundary);
    if (atLastEditingPositionForNode() && prevPosition.isNotNull() && !prevPosition.deprecatedNode()->hasEditableStyle())
        return true;

    return nextPosition.isNotNull() && !nextPosition.deprecatedNode()->hasEditableStyle()
        && prevPosition.isNotNull() && !prevPosition.deprecatedNode()->hasEditableStyle();
}

template <typename Strategy>
static ContainerNode* nonShadowBoundaryParentNode(Node* node)
{
    ContainerNode* parent = Strategy::parent(*node);
    return parent && !parent->isShadowRoot() ? parent : nullptr;
}

template <typename Strategy>
Node* PositionAlgorithm<Strategy>::parentEditingBoundary() const
{
    if (!m_anchorNode)
        return 0;

    Node* documentElement = m_anchorNode->document().documentElement();
    if (!documentElement)
        return 0;

    Node* boundary = containerNode();
    while (boundary != documentElement && nonShadowBoundaryParentNode<Strategy>(boundary) && m_anchorNode->hasEditableStyle() == Strategy::parent(*boundary)->hasEditableStyle())
        boundary = nonShadowBoundaryParentNode<Strategy>(boundary);

    return boundary;
}


template <typename Strategy>
bool PositionAlgorithm<Strategy>::atStartOfTree() const
{
    if (isNull())
        return true;
    return !Strategy::parent(*deprecatedNode()) && m_offset <= 0;
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::atEndOfTree() const
{
    if (isNull())
        return true;
    return !Strategy::parent(*deprecatedNode()) && m_offset >= lastOffsetForEditing(deprecatedNode());
}

template <typename Strategy>
int PositionAlgorithm<Strategy>::renderedOffset() const
{
    if (!deprecatedNode()->isTextNode())
        return m_offset;

    if (!deprecatedNode()->layoutObject())
        return m_offset;

    int result = 0;
    LayoutText* textLayoutObject = toLayoutText(deprecatedNode()->layoutObject());
    for (InlineTextBox *box = textLayoutObject->firstTextBox(); box; box = box->nextTextBox()) {
        int start = box->start();
        int end = box->start() + box->len();
        if (m_offset < start)
            return result;
        if (m_offset <= end) {
            result += m_offset - start;
            return result;
        }
        result += box->len();
    }
    return result;
}

// Whether or not [node, 0] and [node, lastOffsetForEditing(node)] are their own VisiblePositions.
// If true, adjacent candidates are visually distinct.
// FIXME: Disregard nodes with layoutObjects that have no height, as we do in isCandidate.
// FIXME: Share code with isCandidate, if possible.
static bool endsOfNodeAreVisuallyDistinctPositions(Node* node)
{
    if (!node || !node->layoutObject())
        return false;

    if (!node->layoutObject()->isInline())
        return true;

    // Don't include inline tables.
    if (isHTMLTableElement(*node))
        return false;

    // A Marquee elements are moving so we should assume their ends are always
    // visibily distinct.
    if (isHTMLMarqueeElement(*node))
        return true;

    // There is a VisiblePosition inside an empty inline-block container.
    return node->layoutObject()->isReplaced() && canHaveChildrenForEditing(node) && toLayoutBox(node->layoutObject())->size().height() != 0 && !node->hasChildren();
}

template <typename Strategy>
static Node* enclosingVisualBoundary(Node* node)
{
    while (node && !endsOfNodeAreVisuallyDistinctPositions(node))
        node = Strategy::parent(*node);

    return node;
}

// upstream() and downstream() want to return positions that are either in a
// text node or at just before a non-text node.  This method checks for that.
template <typename Strategy>
static bool isStreamer(const PositionIteratorAlgorithm<Strategy>& pos)
{
    if (!pos.node())
        return true;

    if (isAtomicNode(pos.node()))
        return true;

    return pos.atStartOfNode();
}

// This function and downstream() are used for moving back and forth between visually equivalent candidates.
// For example, for the text node "foo     bar" where whitespace is collapsible, there are two candidates
// that map to the VisiblePosition between 'b' and the space.  This function will return the left candidate
// and downstream() will return the right one.
// Also, upstream() will return [boundary, 0] for any of the positions from [boundary, 0] to the first candidate
// in boundary, where endsOfNodeAreVisuallyDistinctPositions(boundary) is true.
template <typename Strategy>
PositionAlgorithm<Strategy> PositionAlgorithm<Strategy>::upstream(EditingBoundaryCrossingRule rule) const
{
    Node* startNode = deprecatedNode();
    if (!startNode)
        return PositionAlgorithm<Strategy>();

    // iterate backward from there, looking for a qualified position
    Node* boundary = enclosingVisualBoundary<Strategy>(startNode);
    // FIXME: PositionIterator should respect Before and After positions.
    PositionIteratorAlgorithm<Strategy> lastVisible(m_anchorType == PositionAnchorType::AfterAnchor ? createLegacyEditingPosition(m_anchorNode.get(), caretMaxOffset(m_anchorNode.get())) : PositionAlgorithm<Strategy>(*this));
    PositionIteratorAlgorithm<Strategy> currentPos = lastVisible;
    bool startEditable = startNode->hasEditableStyle();
    Node* lastNode = startNode;
    bool boundaryCrossed = false;
    for (; !currentPos.atStart(); currentPos.decrement()) {
        Node* currentNode = currentPos.node();
        // Don't check for an editability change if we haven't moved to a different node,
        // to avoid the expense of computing hasEditableStyle().
        if (currentNode != lastNode) {
            // Don't change editability.
            bool currentEditable = currentNode->hasEditableStyle();
            if (startEditable != currentEditable) {
                if (rule == CannotCrossEditingBoundary)
                    break;
                boundaryCrossed = true;
            }
            lastNode = currentNode;
        }

        // If we've moved to a position that is visually distinct, return the last saved position. There
        // is code below that terminates early if we're *about* to move to a visually distinct position.
        if (endsOfNodeAreVisuallyDistinctPositions(currentNode) && currentNode != boundary)
            return lastVisible;

        // skip position in non-laid out or invisible node
        LayoutObject* layoutObject = currentNode->layoutObject();
        if (!layoutObject || layoutObject->style()->visibility() != VISIBLE)
            continue;

        if (rule == CanCrossEditingBoundary && boundaryCrossed) {
            lastVisible = currentPos;
            break;
        }

        // track last visible streamer position
        if (isStreamer<Strategy>(currentPos))
            lastVisible = currentPos;

        // Don't move past a position that is visually distinct.  We could rely on code above to terminate and
        // return lastVisible on the next iteration, but we terminate early to avoid doing a nodeIndex() call.
        if (endsOfNodeAreVisuallyDistinctPositions(currentNode) && currentPos.atStartOfNode())
            return lastVisible;

        // Return position after tables and nodes which have content that can be ignored.
        if (Strategy::editingIgnoresContent(currentNode) || isRenderedHTMLTableElement(currentNode)) {
            if (currentPos.atEndOfNode())
                return afterNode(currentNode);
            continue;
        }

        // return current position if it is in laid out text
        if (layoutObject->isText() && toLayoutText(layoutObject)->firstTextBox()) {
            if (currentNode != startNode) {
                // This assertion fires in layout tests in the case-transform.html test because
                // of a mix-up between offsets in the text in the DOM tree with text in the
                // layout tree which can have a different length due to case transformation.
                // Until we resolve that, disable this so we can run the layout tests!
                // ASSERT(currentOffset >= layoutObject->caretMaxOffset());
                return createLegacyEditingPosition(currentNode, layoutObject->caretMaxOffset());
            }

            unsigned textOffset = currentPos.offsetInLeafNode();
            LayoutText* textLayoutObject = toLayoutText(layoutObject);
            InlineTextBox* lastTextBox = textLayoutObject->lastTextBox();
            for (InlineTextBox* box = textLayoutObject->firstTextBox(); box; box = box->nextTextBox()) {
                if (textOffset <= box->start() + box->len()) {
                    if (textOffset > box->start())
                        return currentPos;
                    continue;
                }

                if (box == lastTextBox || textOffset != box->start() + box->len() + 1)
                    continue;

                // The text continues on the next line only if the last text box is not on this line and
                // none of the boxes on this line have a larger start offset.

                bool continuesOnNextLine = true;
                InlineBox* otherBox = box;
                while (continuesOnNextLine) {
                    otherBox = otherBox->nextLeafChild();
                    if (!otherBox)
                        break;
                    if (otherBox == lastTextBox || (otherBox->layoutObject() == textLayoutObject && toInlineTextBox(otherBox)->start() > textOffset))
                        continuesOnNextLine = false;
                }

                otherBox = box;
                while (continuesOnNextLine) {
                    otherBox = otherBox->prevLeafChild();
                    if (!otherBox)
                        break;
                    if (otherBox == lastTextBox || (otherBox->layoutObject() == textLayoutObject && toInlineTextBox(otherBox)->start() > textOffset))
                        continuesOnNextLine = false;
                }

                if (continuesOnNextLine)
                    return currentPos;
            }
        }
    }
    return lastVisible;
}

// This function and upstream() are used for moving back and forth between visually equivalent candidates.
// For example, for the text node "foo     bar" where whitespace is collapsible, there are two candidates
// that map to the VisiblePosition between 'b' and the space.  This function will return the right candidate
// and upstream() will return the left one.
// Also, downstream() will return the last position in the last atomic node in boundary for all of the positions
// in boundary after the last candidate, where endsOfNodeAreVisuallyDistinctPositions(boundary).
// FIXME: This function should never be called when the line box tree is dirty. See https://bugs.webkit.org/show_bug.cgi?id=97264
template <typename Strategy>
PositionAlgorithm<Strategy> PositionAlgorithm<Strategy>::downstream(EditingBoundaryCrossingRule rule) const
{
    Node* startNode = deprecatedNode();
    if (!startNode)
        return PositionAlgorithm<Strategy>();

    // iterate forward from there, looking for a qualified position
    Node* boundary = enclosingVisualBoundary<Strategy>(startNode);
    // FIXME: PositionIterator should respect Before and After positions.
    PositionIteratorAlgorithm<Strategy> lastVisible(m_anchorType == PositionAnchorType::AfterAnchor ? createLegacyEditingPosition(m_anchorNode.get(), caretMaxOffset(m_anchorNode.get())) : PositionAlgorithm<Strategy>(*this));
    PositionIteratorAlgorithm<Strategy> currentPos = lastVisible;
    bool startEditable = startNode->hasEditableStyle();
    Node* lastNode = startNode;
    bool boundaryCrossed = false;
    for (; !currentPos.atEnd(); currentPos.increment()) {
        Node* currentNode = currentPos.node();
        // Don't check for an editability change if we haven't moved to a different node,
        // to avoid the expense of computing hasEditableStyle().
        if (currentNode != lastNode) {
            // Don't change editability.
            bool currentEditable = currentNode->hasEditableStyle();
            if (startEditable != currentEditable) {
                if (rule == CannotCrossEditingBoundary)
                    break;
                boundaryCrossed = true;
            }

            lastNode = currentNode;
        }

        // stop before going above the body, up into the head
        // return the last visible streamer position
        if (isHTMLBodyElement(*currentNode) && currentPos.atEndOfNode())
            break;

        // Do not move to a visually distinct position.
        if (endsOfNodeAreVisuallyDistinctPositions(currentNode) && currentNode != boundary)
            return lastVisible;
        // Do not move past a visually disinct position.
        // Note: The first position after the last in a node whose ends are visually distinct
        // positions will be [boundary->parentNode(), originalBlock->nodeIndex() + 1].
        if (boundary && Strategy::parent(*boundary) == currentNode)
            return lastVisible;

        // skip position in non-laid out or invisible node
        LayoutObject* layoutObject = currentNode->layoutObject();
        if (!layoutObject || layoutObject->style()->visibility() != VISIBLE)
            continue;

        if (rule == CanCrossEditingBoundary && boundaryCrossed) {
            lastVisible = currentPos;
            break;
        }

        // track last visible streamer position
        if (isStreamer<Strategy>(currentPos))
            lastVisible = currentPos;

        // Return position before tables and nodes which have content that can be ignored.
        if (Strategy::editingIgnoresContent(currentNode) || isRenderedHTMLTableElement(currentNode)) {
            if (currentPos.offsetInLeafNode() <= layoutObject->caretMinOffset())
                return createLegacyEditingPosition(currentNode, layoutObject->caretMinOffset());
            continue;
        }

        // return current position if it is in laid out text
        if (layoutObject->isText() && toLayoutText(layoutObject)->firstTextBox()) {
            if (currentNode != startNode) {
                ASSERT(currentPos.atStartOfNode());
                return createLegacyEditingPosition(currentNode, layoutObject->caretMinOffset());
            }

            unsigned textOffset = currentPos.offsetInLeafNode();
            LayoutText* textLayoutObject = toLayoutText(layoutObject);
            InlineTextBox* lastTextBox = textLayoutObject->lastTextBox();
            for (InlineTextBox* box = textLayoutObject->firstTextBox(); box; box = box->nextTextBox()) {
                if (textOffset <= box->end()) {
                    if (textOffset >= box->start())
                        return currentPos;
                    continue;
                }

                if (box == lastTextBox || textOffset != box->start() + box->len())
                    continue;

                // The text continues on the next line only if the last text box is not on this line and
                // none of the boxes on this line have a larger start offset.

                bool continuesOnNextLine = true;
                InlineBox* otherBox = box;
                while (continuesOnNextLine) {
                    otherBox = otherBox->nextLeafChild();
                    if (!otherBox)
                        break;
                    if (otherBox == lastTextBox || (otherBox->layoutObject() == textLayoutObject && toInlineTextBox(otherBox)->start() >= textOffset))
                        continuesOnNextLine = false;
                }

                otherBox = box;
                while (continuesOnNextLine) {
                    otherBox = otherBox->prevLeafChild();
                    if (!otherBox)
                        break;
                    if (otherBox == lastTextBox || (otherBox->layoutObject() == textLayoutObject && toInlineTextBox(otherBox)->start() >= textOffset))
                        continuesOnNextLine = false;
                }

                if (continuesOnNextLine)
                    return currentPos;
            }
        }
    }

    return lastVisible;
}

static int boundingBoxLogicalHeight(LayoutObject *o, const IntRect &rect)
{
    return o->style()->isHorizontalWritingMode() ? rect.height() : rect.width();
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::hasRenderedNonAnonymousDescendantsWithHeight(LayoutObject* layoutObject)
{
    LayoutObject* stop = layoutObject->nextInPreOrderAfterChildren();
    for (LayoutObject *o = layoutObject->slowFirstChild(); o && o != stop; o = o->nextInPreOrder()) {
        if (o->nonPseudoNode()) {
            if ((o->isText() && boundingBoxLogicalHeight(o, toLayoutText(o)->linesBoundingBox()))
                || (o->isBox() && toLayoutBox(o)->pixelSnappedLogicalHeight())
                || (o->isLayoutInline() && isEmptyInline(LineLayoutItem(o)) && boundingBoxLogicalHeight(o, toLayoutInline(o)->linesBoundingBox())))
                return true;
        }
    }
    return false;
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::nodeIsUserSelectNone(Node* node)
{
    return node && node->layoutObject() && !node->layoutObject()->isSelectable();
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::nodeIsUserSelectAll(const Node* node)
{
    return RuntimeEnabledFeatures::userSelectAllEnabled() && node && node->layoutObject() && node->layoutObject()->style()->userSelect() == SELECT_ALL;
}

template <typename Strategy>
Node* PositionAlgorithm<Strategy>::rootUserSelectAllForNode(Node* node)
{
    if (!node || !nodeIsUserSelectAll(node))
        return 0;
    Node* parent = Strategy::parent(*node);
    if (!parent)
        return node;

    Node* candidateRoot = node;
    while (parent) {
        if (!parent->layoutObject()) {
            parent = Strategy::parent(*parent);
            continue;
        }
        if (!nodeIsUserSelectAll(parent))
            break;
        candidateRoot = parent;
        parent = Strategy::parent(*candidateRoot);
    }
    return candidateRoot;
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::isCandidate() const
{
    if (isNull())
        return false;

    LayoutObject* layoutObject = deprecatedNode()->layoutObject();
    if (!layoutObject)
        return false;

    if (layoutObject->style()->visibility() != VISIBLE)
        return false;

    if (layoutObject->isBR()) {
        // TODO(leviw) The condition should be
        // m_anchorType == PositionAnchorType::BeforeAnchor, but for now we
        // still need to support legacy positions.
        return !m_offset && m_anchorType != PositionAnchorType::AfterAnchor && !nodeIsUserSelectNone(Strategy::parent(*deprecatedNode()));
    }

    if (layoutObject->isText())
        return !nodeIsUserSelectNone(deprecatedNode()) && inRenderedText();

    if (layoutObject->isSVG()) {
        // We don't consider SVG elements are contenteditable except for
        // associated layoutObject returns isText() true, e.g. LayoutSVGInlineText.
        return false;
    }

    if (isRenderedHTMLTableElement(deprecatedNode()) || Strategy::editingIgnoresContent(deprecatedNode()))
        return (atFirstEditingPositionForNode() || atLastEditingPositionForNode()) && !nodeIsUserSelectNone(Strategy::parent(*deprecatedNode()));

    if (isHTMLHtmlElement(*m_anchorNode))
        return false;

    if (layoutObject->isLayoutBlockFlow() || layoutObject->isFlexibleBox() || layoutObject->isLayoutGrid()) {
        if (toLayoutBlock(layoutObject)->logicalHeight() || isHTMLBodyElement(*m_anchorNode)) {
            if (!hasRenderedNonAnonymousDescendantsWithHeight(layoutObject))
                return atFirstEditingPositionForNode() && !nodeIsUserSelectNone(deprecatedNode());
            return m_anchorNode->hasEditableStyle() && !nodeIsUserSelectNone(deprecatedNode()) && atEditingBoundary();
        }
    } else {
        LocalFrame* frame = m_anchorNode->document().frame();
        bool caretBrowsing = frame->settings() && frame->settings()->caretBrowsingEnabled();
        return (caretBrowsing || m_anchorNode->hasEditableStyle()) && !nodeIsUserSelectNone(deprecatedNode()) && atEditingBoundary();
    }

    return false;
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::inRenderedText() const
{
    if (isNull() || !deprecatedNode()->isTextNode())
        return false;

    LayoutObject* layoutObject = deprecatedNode()->layoutObject();
    if (!layoutObject)
        return false;

    LayoutText* textLayoutObject = toLayoutText(layoutObject);
    for (InlineTextBox *box = textLayoutObject->firstTextBox(); box; box = box->nextTextBox()) {
        if (m_offset < static_cast<int>(box->start()) && !textLayoutObject->containsReversedText()) {
            // The offset we're looking for is before this node
            // this means the offset must be in content that is
            // not laid out. Return false.
            return false;
        }
        if (box->containsCaretOffset(m_offset)) {
            // Return false for offsets inside composed characters.
            return m_offset == 0 || m_offset == textLayoutObject->nextOffset(textLayoutObject->previousOffset(m_offset));
        }
    }

    return false;
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::isRenderedCharacter() const
{
    if (isNull() || !deprecatedNode()->isTextNode())
        return false;

    LayoutObject* layoutObject = deprecatedNode()->layoutObject();
    if (!layoutObject)
        return false;

    LayoutText* textLayoutObject = toLayoutText(layoutObject);
    for (InlineTextBox* box = textLayoutObject->firstTextBox(); box; box = box->nextTextBox()) {
        if (m_offset < static_cast<int>(box->start()) && !textLayoutObject->containsReversedText()) {
            // The offset we're looking for is before this node
            // this means the offset must be in content that is
            // not laid out. Return false.
            return false;
        }
        if (m_offset >= static_cast<int>(box->start()) && m_offset < static_cast<int>(box->start() + box->len()))
            return true;
    }

    return false;
}

template <typename Strategy>
bool PositionAlgorithm<Strategy>::rendersInDifferentPosition(const PositionAlgorithm<Strategy> &pos) const
{
    if (isNull() || pos.isNull())
        return false;

    LayoutObject* layoutObject = deprecatedNode()->layoutObject();
    if (!layoutObject)
        return false;

    LayoutObject* posLayoutObject = pos.deprecatedNode()->layoutObject();
    if (!posLayoutObject)
        return false;

    if (layoutObject->style()->visibility() != VISIBLE
        || posLayoutObject->style()->visibility() != VISIBLE)
        return false;

    if (deprecatedNode() == pos.deprecatedNode()) {
        if (isHTMLBRElement(*deprecatedNode()))
            return false;

        if (m_offset == pos.deprecatedEditingOffset())
            return false;

        if (!deprecatedNode()->isTextNode() && !pos.deprecatedNode()->isTextNode()) {
            if (m_offset != pos.deprecatedEditingOffset())
                return true;
        }
    }

    if (isHTMLBRElement(*deprecatedNode()) && pos.isCandidate())
        return true;

    if (isHTMLBRElement(*pos.deprecatedNode()) && isCandidate())
        return true;

    if (!inSameContainingBlockFlowElement(deprecatedNode(), pos.deprecatedNode()))
        return true;

    if (deprecatedNode()->isTextNode() && !inRenderedText())
        return false;

    if (pos.deprecatedNode()->isTextNode() && !pos.inRenderedText())
        return false;

    int thisRenderedOffset = renderedOffset();
    int posRenderedOffset = pos.renderedOffset();

    if (layoutObject == posLayoutObject && thisRenderedOffset == posRenderedOffset)
        return false;

    InlineBoxPosition boxPosition1 = computeInlineBoxPosition(DOWNSTREAM);
    InlineBoxPosition boxPosition2 = pos.computeInlineBoxPosition(DOWNSTREAM);

    WTF_LOG(Editing, "layoutObject:           %p [%p]\n", layoutObject, boxPosition1.inlineBox);
    WTF_LOG(Editing, "thisRenderedOffset:     %d\n", thisRenderedOffset);
    WTF_LOG(Editing, "posLayoutObject:        %p [%p]\n", posLayoutObject, boxPosition2.inlineBox);
    WTF_LOG(Editing, "posRenderedOffset:      %d\n", posRenderedOffset);
    WTF_LOG(Editing, "node min/max:           %d:%d\n", caretMinOffset(deprecatedNode()), caretMaxOffset(deprecatedNode()));
    WTF_LOG(Editing, "pos node min/max:       %d:%d\n", caretMinOffset(pos.deprecatedNode()), caretMaxOffset(pos.deprecatedNode()));
    WTF_LOG(Editing, "----------------------------------------------------------------------\n");

    if (!boxPosition1.inlineBox || !boxPosition2.inlineBox) {
        return false;
    }

    if (boxPosition1.inlineBox->root() != boxPosition2.inlineBox->root()) {
        return true;
    }

    if (nextRenderedEditable(deprecatedNode()) == pos.deprecatedNode()
        && thisRenderedOffset == caretMaxOffset(deprecatedNode()) && !posRenderedOffset) {
        return false;
    }

    if (previousRenderedEditable(deprecatedNode()) == pos.deprecatedNode()
        && !thisRenderedOffset && posRenderedOffset == caretMaxOffset(pos.deprecatedNode())) {
        return false;
    }

    return true;
}

template <typename Strategy>
InlineBoxPosition PositionAlgorithm<Strategy>::computeInlineBoxPosition(EAffinity affinity) const
{
    return computeInlineBoxPosition(affinity, primaryDirection());
}

static bool isNonTextLeafChild(LayoutObject* object)
{
    if (object->slowFirstChild())
        return false;
    if (object->isText())
        return false;
    return true;
}

static InlineTextBox* searchAheadForBetterMatch(LayoutObject* layoutObject)
{
    LayoutBlock* container = layoutObject->containingBlock();
    for (LayoutObject* next = layoutObject->nextInPreOrder(container); next; next = next->nextInPreOrder(container)) {
        if (next->isLayoutBlock())
            return 0;
        if (next->isBR())
            return 0;
        if (isNonTextLeafChild(next))
            return 0;
        if (next->isText()) {
            InlineTextBox* match = 0;
            int minOffset = INT_MAX;
            for (InlineTextBox* box = toLayoutText(next)->firstTextBox(); box; box = box->nextTextBox()) {
                int caretMinOffset = box->caretMinOffset();
                if (caretMinOffset < minOffset) {
                    match = box;
                    minOffset = caretMinOffset;
                }
            }
            if (match)
                return match;
        }
    }
    return 0;
}

template <typename Strategy>
PositionAlgorithm<Strategy> downstreamIgnoringEditingBoundaries(PositionAlgorithm<Strategy> position)
{
    PositionAlgorithm<Strategy> lastPosition;
    while (position != lastPosition) {
        lastPosition = position;
        position = position.downstream(CanCrossEditingBoundary);
    }
    return position;
}

template <typename Strategy>
PositionAlgorithm<Strategy> upstreamIgnoringEditingBoundaries(PositionAlgorithm<Strategy> position)
{
    PositionAlgorithm<Strategy> lastPosition;
    while (position != lastPosition) {
        lastPosition = position;
        position = position.upstream(CanCrossEditingBoundary);
    }
    return position;
}

template <typename Strategy>
InlineBoxPosition PositionAlgorithm<Strategy>::computeInlineBoxPosition(EAffinity affinity, TextDirection primaryDirection) const
{
    InlineBox* inlineBox = nullptr;
    int caretOffset = deprecatedEditingOffset();
    LayoutObject* layoutObject = m_anchorNode->isShadowRoot() ? toShadowRoot(m_anchorNode)->host()->layoutObject() : m_anchorNode->layoutObject();

    if (!layoutObject->isText()) {
        inlineBox = 0;
        if (canHaveChildrenForEditing(deprecatedNode()) && layoutObject->isLayoutBlockFlow() && hasRenderedNonAnonymousDescendantsWithHeight(layoutObject)) {
            // Try a visually equivalent position with possibly opposite editability. This helps in case |this| is in
            // an editable block but surrounded by non-editable positions. It acts to negate the logic at the beginning
            // of LayoutObject::createVisiblePosition().
            PositionAlgorithm<Strategy> thisPosition = PositionAlgorithm<Strategy>(*this);
            PositionAlgorithm<Strategy> equivalent = downstreamIgnoringEditingBoundaries(thisPosition);
            if (equivalent == thisPosition) {
                equivalent = upstreamIgnoringEditingBoundaries(thisPosition);
                if (equivalent == thisPosition || downstreamIgnoringEditingBoundaries(equivalent) == thisPosition)
                    return InlineBoxPosition(inlineBox, caretOffset);
            }

            return equivalent.computeInlineBoxPosition(UPSTREAM, primaryDirection);
        }
        if (layoutObject->isBox()) {
            inlineBox = toLayoutBox(layoutObject)->inlineBoxWrapper();
            if (!inlineBox || (caretOffset > inlineBox->caretMinOffset() && caretOffset < inlineBox->caretMaxOffset()))
                return InlineBoxPosition(inlineBox, caretOffset);
        }
    } else {
        LayoutText* textLayoutObject = toLayoutText(layoutObject);

        InlineTextBox* box;
        InlineTextBox* candidate = 0;

        for (box = textLayoutObject->firstTextBox(); box; box = box->nextTextBox()) {
            int caretMinOffset = box->caretMinOffset();
            int caretMaxOffset = box->caretMaxOffset();

            if (caretOffset < caretMinOffset || caretOffset > caretMaxOffset || (caretOffset == caretMaxOffset && box->isLineBreak()))
                continue;

            if (caretOffset > caretMinOffset && caretOffset < caretMaxOffset)
                return InlineBoxPosition(box, caretOffset);

            if (((caretOffset == caretMaxOffset) ^ (affinity == DOWNSTREAM))
                || ((caretOffset == caretMinOffset) ^ (affinity == UPSTREAM))
                || (caretOffset == caretMaxOffset && box->nextLeafChild() && box->nextLeafChild()->isLineBreak()))
                break;

            candidate = box;
        }
        if (candidate && candidate == textLayoutObject->lastTextBox() && affinity == DOWNSTREAM) {
            box = searchAheadForBetterMatch(textLayoutObject);
            if (box)
                caretOffset = box->caretMinOffset();
        }
        inlineBox = box ? box : candidate;
    }

    if (!inlineBox)
        return InlineBoxPosition(inlineBox, caretOffset);

    unsigned char level = inlineBox->bidiLevel();

    if (inlineBox->direction() == primaryDirection) {
        if (caretOffset == inlineBox->caretRightmostOffset()) {
            InlineBox* nextBox = inlineBox->nextLeafChild();
            if (!nextBox || nextBox->bidiLevel() >= level)
                return InlineBoxPosition(inlineBox, caretOffset);

            level = nextBox->bidiLevel();
            InlineBox* prevBox = inlineBox;
            do {
                prevBox = prevBox->prevLeafChild();
            } while (prevBox && prevBox->bidiLevel() > level);

            if (prevBox && prevBox->bidiLevel() == level) // For example, abc FED 123 ^ CBA
                return InlineBoxPosition(inlineBox, caretOffset);

            // For example, abc 123 ^ CBA
            while (InlineBox* nextBox = inlineBox->nextLeafChild()) {
                if (nextBox->bidiLevel() < level)
                    break;
                inlineBox = nextBox;
            }
            caretOffset = inlineBox->caretRightmostOffset();
        } else {
            InlineBox* prevBox = inlineBox->prevLeafChild();
            if (!prevBox || prevBox->bidiLevel() >= level)
                return InlineBoxPosition(inlineBox, caretOffset);

            level = prevBox->bidiLevel();
            InlineBox* nextBox = inlineBox;
            do {
                nextBox = nextBox->nextLeafChild();
            } while (nextBox && nextBox->bidiLevel() > level);

            if (nextBox && nextBox->bidiLevel() == level)
                return InlineBoxPosition(inlineBox, caretOffset);

            while (InlineBox* prevBox = inlineBox->prevLeafChild()) {
                if (prevBox->bidiLevel() < level)
                    break;
                inlineBox = prevBox;
            }
            caretOffset = inlineBox->caretLeftmostOffset();
        }
        return InlineBoxPosition(inlineBox, caretOffset);
    }

    if (caretOffset == inlineBox->caretLeftmostOffset()) {
        InlineBox* prevBox = inlineBox->prevLeafChildIgnoringLineBreak();
        if (!prevBox || prevBox->bidiLevel() < level) {
            // Left edge of a secondary run. Set to the right edge of the entire run.
            while (InlineBox* nextBox = inlineBox->nextLeafChildIgnoringLineBreak()) {
                if (nextBox->bidiLevel() < level)
                    break;
                inlineBox = nextBox;
            }
            caretOffset = inlineBox->caretRightmostOffset();
        } else if (prevBox->bidiLevel() > level) {
            // Right edge of a "tertiary" run. Set to the left edge of that run.
            while (InlineBox* tertiaryBox = inlineBox->prevLeafChildIgnoringLineBreak()) {
                if (tertiaryBox->bidiLevel() <= level)
                    break;
                inlineBox = tertiaryBox;
            }
            caretOffset = inlineBox->caretLeftmostOffset();
        }
    } else if (layoutObject && layoutObject->style()->unicodeBidi() == Plaintext) {
        if (inlineBox->bidiLevel() < level)
            caretOffset = inlineBox->caretLeftmostOffset();
        else
            caretOffset = inlineBox->caretRightmostOffset();
    } else {
        InlineBox* nextBox = inlineBox->nextLeafChildIgnoringLineBreak();
        if (!nextBox || nextBox->bidiLevel() < level) {
            // Right edge of a secondary run. Set to the left edge of the entire run.
            while (InlineBox* prevBox = inlineBox->prevLeafChildIgnoringLineBreak()) {
                if (prevBox->bidiLevel() < level)
                    break;
                inlineBox = prevBox;
            }
            caretOffset = inlineBox->caretLeftmostOffset();
        } else if (nextBox->bidiLevel() > level) {
            // Left edge of a "tertiary" run. Set to the right edge of that run.
            while (InlineBox* tertiaryBox = inlineBox->nextLeafChildIgnoringLineBreak()) {
                if (tertiaryBox->bidiLevel() <= level)
                    break;
                inlineBox = tertiaryBox;
            }
            caretOffset = inlineBox->caretRightmostOffset();
        }
    }
    return InlineBoxPosition(inlineBox, caretOffset);
}

template <typename Strategy>
TextDirection PositionAlgorithm<Strategy>::primaryDirection() const
{
    TextDirection primaryDirection = LTR;
    for (const LayoutObject* r = m_anchorNode->layoutObject(); r; r = r->parent()) {
        if (r->isLayoutBlockFlow()) {
            primaryDirection = r->style()->direction();
            break;
        }
    }

    return primaryDirection;
}

template <typename Strategy>
void PositionAlgorithm<Strategy>::debugPosition(const char* msg) const
{
    static const char* const anchorTypes[] = {
        "OffsetInAnchor",
        "BeforeAnchor",
        "AfterAnchor",
        "BeforeChildren",
        "AfterChildren",
        "Invalid",
    };

    if (isNull()) {
        fprintf(stderr, "Position [%s]: null\n", msg);
        return;
    }

    const char* anchorType = anchorTypes[std::min(static_cast<size_t>(m_anchorType), WTF_ARRAY_LENGTH(anchorTypes) - 1)];
    if (m_anchorNode->isTextNode()) {
        fprintf(stderr, "Position [%s]: %s%s [%p] %s, (%s) at %d\n", msg, m_isLegacyEditingPosition ? "LEGACY, " : "", deprecatedNode()->nodeName().utf8().data(), deprecatedNode(), anchorType, m_anchorNode->nodeValue().utf8().data(), m_offset);
        return;
    }

    fprintf(stderr, "Position [%s]: %s%s [%p] %s at %d\n", msg, m_isLegacyEditingPosition ? "LEGACY, " : "", deprecatedNode()->nodeName().utf8().data(), deprecatedNode(), anchorType, m_offset);
}

PositionInComposedTree toPositionInComposedTree(const Position& pos)
{
    if (pos.isNull())
        return PositionInComposedTree();

    PositionInComposedTree position;
    if (pos.anchorType() == PositionAnchorType::OffsetInAnchor) {
        Node* anchor = pos.anchorNode();
        if (anchor->offsetInCharacters())
            return PositionInComposedTree(anchor, pos.computeOffsetInContainerNode());
        ASSERT(!isActiveInsertionPoint(*anchor));
        int offset = pos.computeOffsetInContainerNode();
        Node* child = NodeTraversal::childAt(*anchor, offset);
        if (!child) {
            if (anchor->isShadowRoot())
                return PositionInComposedTree(anchor->shadowHost(), PositionAnchorType::AfterChildren);
            return PositionInComposedTree(anchor, PositionAnchorType::AfterChildren);
        }
        child->updateDistribution();
        if (isActiveInsertionPoint(*child)) {
            if (anchor->isShadowRoot())
                return PositionInComposedTree(anchor->shadowHost(), offset);
            return PositionInComposedTree(anchor, offset);
        }
        return PositionInComposedTree(ComposedTreeTraversal::parent(*child), ComposedTreeTraversal::index(*child));
    }

    return PositionInComposedTree(pos.anchorNode(), pos.anchorType());
}

Position toPositionInDOMTree(const Position& position)
{
    return position;
}

Position toPositionInDOMTree(const PositionInComposedTree& position)
{
    if (position.isNull())
        return Position();

    Node* anchorNode = position.anchorNode();

    switch (position.anchorType()) {
    case PositionAnchorType::AfterChildren:
        // FIXME: When anchorNode is <img>, assertion fails in the constructor.
        return Position(anchorNode, PositionAnchorType::AfterChildren);
    case PositionAnchorType::AfterAnchor:
        return positionAfterNode(anchorNode);
    case PositionAnchorType::BeforeChildren:
        return Position(anchorNode, PositionAnchorType::BeforeChildren);
    case PositionAnchorType::BeforeAnchor:
        return positionBeforeNode(anchorNode);
    case PositionAnchorType::OffsetInAnchor: {
        int offset = position.offsetInContainerNode();
        if (anchorNode->offsetInCharacters())
            return Position(anchorNode, offset);
        Node* child = ComposedTreeTraversal::childAt(*anchorNode, offset);
        if (child)
            return Position(child->parentNode(), child->nodeIndex());
        if (!position.offsetInContainerNode())
            return Position(anchorNode, PositionAnchorType::BeforeChildren);

        // |child| is null when the position is at the end of the children.
        // <div>foo|</div>
        return Position(anchorNode, PositionAnchorType::AfterChildren);
    }
    default:
        ASSERT_NOT_REACHED();
        return Position();
    }
}

#ifndef NDEBUG

template <typename Strategy>
void PositionAlgorithm<Strategy>::formatForDebugger(char* buffer, unsigned length) const
{
    StringBuilder result;

    if (isNull()) {
        result.appendLiteral("<null>");
    } else {
        char s[1024];
        result.appendLiteral("offset ");
        result.appendNumber(m_offset);
        result.appendLiteral(" of ");
        deprecatedNode()->formatForDebugger(s, sizeof(s));
        result.append(s);
    }

    strncpy(buffer, result.toString().utf8().data(), length - 1);
}

template <typename Strategy>
void PositionAlgorithm<Strategy>::showAnchorTypeAndOffset() const
{
    if (m_isLegacyEditingPosition)
        fputs("legacy, ", stderr);
    switch (anchorType()) {
    case PositionAnchorType::OffsetInAnchor:
        fputs("offset", stderr);
        break;
    case PositionAnchorType::BeforeChildren:
        fputs("beforeChildren", stderr);
        break;
    case PositionAnchorType::AfterChildren:
        fputs("afterChildren", stderr);
        break;
    case PositionAnchorType::BeforeAnchor:
        fputs("before", stderr);
        break;
    case PositionAnchorType::AfterAnchor:
        fputs("after", stderr);
        break;
    }
    fprintf(stderr, ", offset:%d\n", m_offset);
}

template <typename Strategy>
void PositionAlgorithm<Strategy>::showTreeForThis() const
{
    if (!anchorNode())
        return;
    anchorNode()->showTreeForThis();
    showAnchorTypeAndOffset();
}

template <typename Strategy>
void PositionAlgorithm<Strategy>::showTreeForThisInComposedTree() const
{
    if (!anchorNode())
        return;
    anchorNode()->showTreeForThisInComposedTree();
    showAnchorTypeAndOffset();
}

#endif

template class CORE_TEMPLATE_EXPORT PositionAlgorithm<EditingStrategy>;
template class CORE_TEMPLATE_EXPORT PositionAlgorithm<EditingInComposedTreeStrategy>;

} // namespace blink

#ifndef NDEBUG

void showTree(const blink::Position& pos)
{
    pos.showTreeForThis();
}

void showTree(const blink::Position* pos)
{
    if (pos)
        pos->showTreeForThis();
    else
        fprintf(stderr, "Cannot showTree for (nil)\n");
}

#endif
