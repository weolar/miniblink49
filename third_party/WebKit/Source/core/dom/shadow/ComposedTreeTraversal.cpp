/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
#include "core/dom/shadow/ComposedTreeTraversal.h"

#include "core/dom/Element.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/html/HTMLShadowElement.h"

namespace blink {

static inline ElementShadow* shadowFor(const Node& node)
{
    return node.isElementNode() ? toElement(node).shadow() : 0;
}

Node* ComposedTreeTraversal::traverseChild(const Node& node, TraversalDirection direction)
{
    ElementShadow* shadow = shadowFor(node);
    return shadow ? traverseLightChildren(*shadow->youngestShadowRoot(), direction)
        : traverseLightChildren(node, direction);
}

Node* ComposedTreeTraversal::traverseLightChildren(const Node& node, TraversalDirection direction)
{
    return traverseSiblings(direction == TraversalDirectionForward ? node.firstChild() : node.lastChild(), direction);
}

Node* ComposedTreeTraversal::traverseSiblings(const Node* node, TraversalDirection direction)
{
    for (const Node* sibling = node; sibling; sibling = (direction == TraversalDirectionForward ? sibling->nextSibling() : sibling->previousSibling())) {
        if (Node* found = traverseNode(*sibling, direction))
            return found;
    }
    return 0;
}

Node* ComposedTreeTraversal::traverseNode(const Node& node, TraversalDirection direction)
{
    if (!isActiveInsertionPoint(node))
        return const_cast<Node*>(&node);
    const InsertionPoint& insertionPoint = toInsertionPoint(node);
    if (Node* found = traverseDistributedNodes(direction == TraversalDirectionForward ? insertionPoint.firstDistributedNode() : insertionPoint.lastDistributedNode(), insertionPoint, direction))
        return found;
    ASSERT(isHTMLShadowElement(node) || (isHTMLContentElement(node) && !node.hasChildren()));
    return 0;
}

Node* ComposedTreeTraversal::traverseDistributedNodes(const Node* node, const InsertionPoint& insertionPoint, TraversalDirection direction)
{
    for (const Node* next = node; next; next = (direction == TraversalDirectionForward ? insertionPoint.distributedNodeNextTo(next) : insertionPoint.distributedNodePreviousTo(next))) {
        if (Node* found = traverseNode(*next, direction))
            return found;
    }
    return 0;
}

Node* ComposedTreeTraversal::traverseSiblingOrBackToInsertionPoint(const Node& node, TraversalDirection direction)
{
    if (!shadowWhereNodeCanBeDistributed(node))
        return traverseSiblingInCurrentTree(node, direction);

    const InsertionPoint* insertionPoint = resolveReprojection(&node);
    if (!insertionPoint)
        return traverseSiblingInCurrentTree(node, direction);

    if (Node* found = traverseDistributedNodes(direction == TraversalDirectionForward ? insertionPoint->distributedNodeNextTo(&node) : insertionPoint->distributedNodePreviousTo(&node), *insertionPoint, direction))
        return found;
    return traverseSiblingOrBackToInsertionPoint(*insertionPoint, direction);
}

Node* ComposedTreeTraversal::traverseSiblingInCurrentTree(const Node& node, TraversalDirection direction)
{
    if (Node* found = traverseSiblings(direction == TraversalDirectionForward ? node.nextSibling() : node.previousSibling(), direction))
        return found;
    if (Node* next = traverseBackToYoungerShadowRoot(node, direction))
        return next;
    return 0;
}

Node* ComposedTreeTraversal::traverseBackToYoungerShadowRoot(const Node& node, TraversalDirection direction)
{
    if (node.parentNode() && node.parentNode()->isShadowRoot()) {
        ShadowRoot* parentShadowRoot = toShadowRoot(node.parentNode());
        if (!parentShadowRoot->isYoungest()) {
            HTMLShadowElement* assignedInsertionPoint = parentShadowRoot->shadowInsertionPointOfYoungerShadowRoot();
            ASSERT(assignedInsertionPoint);
            return traverseSiblingInCurrentTree(*assignedInsertionPoint, direction);
        }
    }
    return 0;
}

// FIXME: Use an iterative algorithm so that it can be inlined.
// https://bugs.webkit.org/show_bug.cgi?id=90415
ContainerNode* ComposedTreeTraversal::traverseParent(const Node& node, ParentTraversalDetails* details)
{
    // TODO(hayato): Stop this hack for a pseudo element because a pseudo element is not a child of its parentOrShadowHostNode() in a composed tree.
    if (node.isPseudoElement())
        return node.parentOrShadowHostNode();

    if (shadowWhereNodeCanBeDistributed(node)) {
        if (const InsertionPoint* insertionPoint = resolveReprojection(&node)) {
            if (details)
                details->didTraverseInsertionPoint(insertionPoint);
            // The node is distributed. But the distribution was stopped at this insertion point.
            if (shadowWhereNodeCanBeDistributed(*insertionPoint))
                return nullptr;
            return traverseParentOrHost(*insertionPoint);
        }
        return nullptr;
    }
    return traverseParentOrHost(node);
}

inline ContainerNode* ComposedTreeTraversal::traverseParentOrHost(const Node& node)
{
    ContainerNode* parent = node.parentNode();
    if (!parent)
        return nullptr;
    if (!parent->isShadowRoot())
        return parent;
    ShadowRoot* shadowRoot = toShadowRoot(parent);
    ASSERT(!shadowRoot->shadowInsertionPointOfYoungerShadowRoot());
    if (!shadowRoot->isYoungest())
        return nullptr;
    Element* host = shadowRoot->host();
    if (isActiveInsertionPoint(*host))
        return nullptr;
    return host;
}

Node* ComposedTreeTraversal::childAt(const Node& node, unsigned index)
{
    assertPrecondition(node);
    Node* child = traverseFirstChild(node);
    while (child && index--)
        child = nextSibling(*child);
    assertPostcondition(child);
    return child;
}

Node* ComposedTreeTraversal::nextSkippingChildren(const Node& node)
{
    if (Node* nextSibling = traverseNextSibling(node))
        return nextSibling;
    return traverseNextAncestorSibling(node);
}

bool ComposedTreeTraversal::containsIncludingPseudoElement(const ContainerNode& container, const Node& node)
{
    assertPrecondition(container);
    assertPrecondition(node);
    // This can be slower than ComposedTreeTraversal::contains() because we
    // can't early exit even when container doesn't have children.
    for (const Node* current = &node; current; current = traverseParent(*current)) {
        if (current == &container)
            return true;
    }
    return false;
}

Node* ComposedTreeTraversal::previousSkippingChildren(const Node& node)
{
    if (Node* previousSibling = traversePreviousSibling(node))
        return previousSibling;
    return traversePreviousAncestorSibling(node);
}

bool ComposedTreeTraversal::isDescendantOf(const Node& node, const Node& other)
{
    assertPrecondition(node);
    assertPrecondition(other);
    if (!hasChildren(other) || node.inDocument() != other.inDocument())
        return false;
    for (const ContainerNode* n = traverseParent(node); n; n = traverseParent(*n)) {
        if (n == other)
            return true;
    }
    return false;
}

Node* ComposedTreeTraversal::commonAncestor(const Node& nodeA, const Node& nodeB)
{
    assertPrecondition(nodeA);
    assertPrecondition(nodeB);
    Node* result = nodeA.commonAncestor(nodeB,
        [](const Node& node)
        {
            return ComposedTreeTraversal::parent(node);
        });
    assertPostcondition(result);
    return result;
}

Node* ComposedTreeTraversal::traverseNextAncestorSibling(const Node& node)
{
    ASSERT(!traverseNextSibling(node));
    for (Node* parent = traverseParent(node); parent; parent = traverseParent(*parent)) {
        if (Node* nextSibling = traverseNextSibling(*parent))
            return nextSibling;
    }
    return nullptr;
}

Node* ComposedTreeTraversal::traversePreviousAncestorSibling(const Node& node)
{
    ASSERT(!traversePreviousSibling(node));
    for (Node* parent = traverseParent(node); parent; parent = traverseParent(*parent)) {
        if (Node* previousSibling = traversePreviousSibling(*parent))
            return previousSibling;
    }
    return nullptr;
}

unsigned ComposedTreeTraversal::index(const Node& node)
{
    assertPrecondition(node);
    unsigned count = 0;
    for (Node* runner = traversePreviousSibling(node); runner; runner = previousSibling(*runner))
        ++count;
    return count;
}

unsigned ComposedTreeTraversal::countChildren(const Node& node)
{
    assertPrecondition(node);
    unsigned count = 0;
    for (Node* runner = traverseFirstChild(node); runner; runner = traverseNextSibling(*runner))
        ++count;
    return count;
}

Node* ComposedTreeTraversal::lastWithin(const Node& node)
{
    assertPrecondition(node);
    Node* descendant = traverseLastChild(node);
    for (Node* child = descendant; child; child = lastChild(*child))
        descendant = child;
    assertPostcondition(descendant);
    return descendant;
}

Node& ComposedTreeTraversal::lastWithinOrSelf(const Node& node)
{
    assertPrecondition(node);
    Node* lastDescendant = lastWithin(node);
    Node& result = lastDescendant ? *lastDescendant : const_cast<Node&>(node);
    assertPostcondition(&result);
    return result;
}

} // namespace
