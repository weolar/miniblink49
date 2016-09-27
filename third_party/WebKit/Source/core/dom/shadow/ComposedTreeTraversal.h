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

#ifndef ComposedTreeTraversal_h
#define ComposedTreeTraversal_h

#include "core/CoreExport.h"
#include "core/dom/Document.h"
#include "core/dom/LayoutTreeBuilderTraversal.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/dom/shadow/ShadowRoot.h"

namespace blink {

class ContainerNode;
class Node;

// Composed tree version of |NodeTraversal|.
//
// None of member functions takes a |ShadowRoot| or an active insertion point,
// e.g. roughly speaking <content> and <shadow> in the shadow tree, see
// |InsertionPoint::isActive()| for details of active insertion points, since
// they aren't appeared in the composed tree. |assertPrecondition()| and
// |assertPostCondition()| check this condition.
//
// FIXME: Make some functions inline to optimise the performance.
// https://bugs.webkit.org/show_bug.cgi?id=82702
class CORE_EXPORT ComposedTreeTraversal {
public:
    typedef LayoutTreeBuilderTraversal::ParentDetails ParentTraversalDetails;

    static Node* next(const Node&);
    static Node* next(const Node&, const Node* stayWithin);
    static Node* previous(const Node&);

    static Node* firstChild(const Node&);
    static Node* lastChild(const Node&);
    static bool hasChildren(const Node&);

    static ContainerNode* parent(const Node&, ParentTraversalDetails* = 0);
    static Element* parentElement(const Node&);

    static Node* nextSibling(const Node&);
    static Node* previousSibling(const Node&);

    // Returns a child node at |index|. If |index| is greater than or equal to
    // the children, this function returns |nullptr|.
    static Node* childAt(const Node&, unsigned index);

    // Composed tree version of |NodeTraversal::nextSkippingChildren()|. This
    // function is similar to |next()| but skips child nodes of a specified
    // node.
    static Node* nextSkippingChildren(const Node&);
    static Node* nextSkippingChildren(const Node&, const Node* stayWithin);

    // Composed tree version of |NodeTraversal::previousSkippingChildren()|
    // similar to |previous()| but skipping child nodes of the specified node.
    static Node* previousSkippingChildren(const Node&);

    // Composed tree version of |Node::isDescendantOf(other)|. This function
    // returns true if |other| contains |node|, otherwise returns
    // false. If |other| is |node|, this function returns false.
    static bool isDescendantOf(const Node& /*node*/, const Node& other);

    static bool contains(const ContainerNode& container, const Node& node)
    {
        assertPrecondition(container);
        assertPrecondition(node);
        return container == node || isDescendantOf(node, container);
    }

    static bool containsIncludingPseudoElement(const ContainerNode&, const Node&);

    // Returns a common ancestor of |nodeA| and |nodeB| if exists, otherwise
    // returns |nullptr|.
    static Node* commonAncestor(const Node& nodeA, const Node& nodeB);

    // Composed tree version of |Node::nodeIndex()|. This function returns a
    // zero base position number of the specified node in child nodes list, or
    // zero if the specified node has no parent.
    static unsigned index(const Node&);

    // Composed tree version of |ContainerNode::countChildren()|. This function
    // returns the number of the child nodes of the specified node in the
    // composed tree.
    static unsigned countChildren(const Node&);

    static Node* lastWithin(const Node&);
    static Node& lastWithinOrSelf(const Node&);

private:
    enum TraversalDirection {
        TraversalDirectionForward,
        TraversalDirectionBackward
    };

    static void assertPrecondition(const Node& node)
    {
#if ENABLE(ASSERT)
        ASSERT(!node.needsDistributionRecalc());
        ASSERT(node.canParticipateInComposedTree());
#endif
    }

    static void assertPostcondition(const Node* node)
    {
#if ENABLE(ASSERT)
        if (node)
            assertPrecondition(*node);
#endif
    }

    static Node* traverseNode(const Node&, TraversalDirection);
    static Node* traverseLightChildren(const Node&, TraversalDirection);

    static Node* traverseNext(const Node&);
    static Node* traverseNext(const Node&, const Node* stayWithin);
    static Node* traverseNextSkippingChildren(const Node&, const Node* stayWithin);
    static Node* traversePrevious(const Node&);

    static Node* traverseFirstChild(const Node&);
    static Node* traverseLastChild(const Node&);
    static Node* traverseChild(const Node&, TraversalDirection);

    static ContainerNode* traverseParent(const Node&, ParentTraversalDetails* = 0);

    static Node* traverseNextSibling(const Node&);
    static Node* traversePreviousSibling(const Node&);

    static Node* traverseSiblingOrBackToInsertionPoint(const Node&, TraversalDirection);
    static Node* traverseSiblingInCurrentTree(const Node&, TraversalDirection);

    static Node* traverseSiblings(const Node*, TraversalDirection);
    static Node* traverseDistributedNodes(const Node*, const InsertionPoint&, TraversalDirection);

    static Node* traverseBackToYoungerShadowRoot(const Node&, TraversalDirection);

    static ContainerNode* traverseParentOrHost(const Node&);
    static Node* traverseNextAncestorSibling(const Node&);
    static Node* traversePreviousAncestorSibling(const Node&);
};

inline ContainerNode* ComposedTreeTraversal::parent(const Node& node, ParentTraversalDetails* details)
{
    assertPrecondition(node);
    ContainerNode* result = traverseParent(node, details);
    assertPostcondition(result);
    return result;
}

inline Element* ComposedTreeTraversal::parentElement(const Node& node)
{
    ContainerNode* parent = ComposedTreeTraversal::parent(node);
    return parent && parent->isElementNode() ? toElement(parent) : nullptr;
}

inline Node* ComposedTreeTraversal::nextSibling(const Node& node)
{
    assertPrecondition(node);
    Node* result = traverseSiblingOrBackToInsertionPoint(node, TraversalDirectionForward);
    assertPostcondition(result);
    return result;
}

inline Node* ComposedTreeTraversal::previousSibling(const Node& node)
{
    assertPrecondition(node);
    Node* result = traverseSiblingOrBackToInsertionPoint(node, TraversalDirectionBackward);
    assertPostcondition(result);
    return result;
}

inline Node* ComposedTreeTraversal::next(const Node& node)
{
    assertPrecondition(node);
    Node* result = traverseNext(node);
    assertPostcondition(result);
    return result;
}

inline Node* ComposedTreeTraversal::next(const Node& node, const Node* stayWithin)
{
    assertPrecondition(node);
    Node* result = traverseNext(node, stayWithin);
    assertPostcondition(result);
    return result;
}

inline Node* ComposedTreeTraversal::nextSkippingChildren(const Node& node, const Node* stayWithin)
{
    assertPrecondition(node);
    Node* result = traverseNextSkippingChildren(node, stayWithin);
    assertPostcondition(result);
    return result;
}

inline Node* ComposedTreeTraversal::traverseNext(const Node& node)
{
    if (Node* next = traverseFirstChild(node))
        return next;
    for (const Node* next = &node; next; next = traverseParent(*next)) {
        if (Node* sibling = traverseNextSibling(*next))
            return sibling;
    }
    return 0;
}

inline Node* ComposedTreeTraversal::traverseNext(const Node& node, const Node* stayWithin)
{
    if (Node* next = traverseFirstChild(node))
        return next;
    return traverseNextSkippingChildren(node, stayWithin);
}

inline Node* ComposedTreeTraversal::traverseNextSkippingChildren(const Node& node, const Node* stayWithin)
{
    for (const Node* next = &node; next; next = traverseParent(*next)) {
        if (next == stayWithin)
            return nullptr;
        if (Node* sibling = traverseNextSibling(*next))
            return sibling;
    }
    return nullptr;
}

inline Node* ComposedTreeTraversal::previous(const Node& node)
{
    assertPrecondition(node);
    Node* result = traversePrevious(node);
    assertPostcondition(result);
    return result;
}

inline Node* ComposedTreeTraversal::traversePrevious(const Node& node)
{
    if (Node* previous = traversePreviousSibling(node)) {
        while (Node* child = traverseLastChild(*previous))
            previous = child;
        return previous;
    }
    return traverseParent(node);
}

inline Node* ComposedTreeTraversal::firstChild(const Node& node)
{
    assertPrecondition(node);
    Node* result = traverseChild(node, TraversalDirectionForward);
    assertPostcondition(result);
    return result;
}

inline Node* ComposedTreeTraversal::lastChild(const Node& node)
{
    assertPrecondition(node);
    Node* result = traverseLastChild(node);
    assertPostcondition(result);
    return result;
}

inline bool ComposedTreeTraversal::hasChildren(const Node& node)
{
    return firstChild(node);
}

inline Node* ComposedTreeTraversal::traverseNextSibling(const Node& node)
{
    return traverseSiblingOrBackToInsertionPoint(node, TraversalDirectionForward);
}

inline Node* ComposedTreeTraversal::traversePreviousSibling(const Node& node)
{
    return traverseSiblingOrBackToInsertionPoint(node, TraversalDirectionBackward);
}

inline Node* ComposedTreeTraversal::traverseFirstChild(const Node& node)
{
    return traverseChild(node, TraversalDirectionForward);
}

inline Node* ComposedTreeTraversal::traverseLastChild(const Node& node)
{
    return traverseChild(node, TraversalDirectionBackward);
}

} // namespace

#endif
