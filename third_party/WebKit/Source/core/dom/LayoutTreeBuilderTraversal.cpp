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
#include "core/dom/LayoutTreeBuilderTraversal.h"

#include "core/HTMLNames.h"
#include "core/dom/PseudoElement.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/layout/LayoutObject.h"

namespace blink {

namespace LayoutTreeBuilderTraversal {

static bool isLayoutObjectReparented(const LayoutObject* layoutObject)
{
    if (!layoutObject->node()->isElementNode())
        return false;
    if (toElement(layoutObject->node())->isInTopLayer())
        return true;
    return false;
}

void ParentDetails::didTraverseInsertionPoint(const InsertionPoint* insertionPoint)
{
    if (!m_insertionPoint) {
        m_insertionPoint = insertionPoint;
    }
}

ContainerNode* parent(const Node& node, ParentDetails* details)
{
    // TODO(hayato): Uncomment this once we can be sure LayoutTreeBuilderTraversal::parent() is used only for a node in a document.
    // ASSERT(node.inDocument());
    return ComposedTreeTraversal::parent(node, details);
}

Node* nextSibling(const Node& node)
{
    if (node.isBeforePseudoElement()) {
        if (Node* next = ComposedTreeTraversal::firstChild(*ComposedTreeTraversal::parent(node)))
            return next;
    } else {
        if (Node* next = ComposedTreeTraversal::nextSibling(node))
            return next;
        if (node.isAfterPseudoElement())
            return 0;
    }

    Node* parent = ComposedTreeTraversal::parent(node);
    if (parent && parent->isElementNode())
        return toElement(parent)->pseudoElement(AFTER);

    return 0;
}

Node* previousSibling(const Node& node)
{
    if (node.isAfterPseudoElement()) {
        if (Node* previous = ComposedTreeTraversal::lastChild(*ComposedTreeTraversal::parent(node)))
            return previous;
    } else {
        if (Node* previous = ComposedTreeTraversal::previousSibling(node))
            return previous;
        if (node.isBeforePseudoElement())
            return 0;
    }

    Node* parent = ComposedTreeTraversal::parent(node);
    if (parent && parent->isElementNode())
        return toElement(parent)->pseudoElement(BEFORE);

    return 0;
}

static Node* lastChild(const Node& node)
{
    return ComposedTreeTraversal::lastChild(node);
}

static Node* pseudoAwarePreviousSibling(const Node& node)
{
    Node* previousNode = previousSibling(node);
    Node* parentNode = parent(node);

    if (parentNode && parentNode->isElementNode() && !previousNode) {
        if (node.isAfterPseudoElement()) {
            if (Node* child = lastChild(*parentNode))
                return child;
        }
        if (!node.isBeforePseudoElement())
            return toElement(parentNode)->pseudoElement(BEFORE);
    }
    return previousNode;
}

static Node* pseudoAwareLastChild(const Node& node)
{
    if (node.isElementNode()) {
        const Element& currentElement = toElement(node);
        Node* last = currentElement.pseudoElement(AFTER);
        if (last)
            return last;

        last = lastChild(currentElement);
        if (!last)
            last = currentElement.pseudoElement(BEFORE);
        return last;
    }

    return lastChild(node);
}

Node* previous(const Node& node, const Node* stayWithin)
{
    if (node == stayWithin)
        return 0;

    if (Node* previousNode = pseudoAwarePreviousSibling(node)) {
        while (Node* previousLastChild = pseudoAwareLastChild(*previousNode))
            previousNode = previousLastChild;
        return previousNode;
    }
    return parent(node);
}

Node* firstChild(const Node& node)
{
    return ComposedTreeTraversal::firstChild(node);
}

static Node* pseudoAwareNextSibling(const Node& node)
{
    Node* parentNode = parent(node);
    Node* nextNode = nextSibling(node);

    if (parentNode && parentNode->isElementNode() && !nextNode) {
        if (node.isBeforePseudoElement()) {
            if (Node* child = firstChild(*parentNode))
                return child;
        }
        if (!node.isAfterPseudoElement())
            return toElement(parentNode)->pseudoElement(AFTER);
    }
    return nextNode;
}

static Node* pseudoAwareFirstChild(const Node& node)
{
    if (node.isElementNode()) {
        const Element& currentElement = toElement(node);
        Node* first = currentElement.pseudoElement(BEFORE);
        if (first)
            return first;
        first = firstChild(currentElement);
        if (!first)
            first = currentElement.pseudoElement(AFTER);
        return first;
    }

    return firstChild(node);
}

static Node* nextAncestorSibling(const Node& node, const Node* stayWithin)
{
    ASSERT(!pseudoAwareNextSibling(node));
    ASSERT(node != stayWithin);
    for (Node* parentNode = parent(node); parentNode; parentNode = parent(*parentNode)) {
        if (parentNode == stayWithin)
            return 0;
        if (Node* nextNode = pseudoAwareNextSibling(*parentNode))
            return nextNode;
    }
    return 0;
}

Node* nextSkippingChildren(const Node& node, const Node* stayWithin)
{
    if (node == stayWithin)
        return 0;
    if (Node* nextNode = pseudoAwareNextSibling(node))
        return nextNode;
    return nextAncestorSibling(node, stayWithin);
}

Node* next(const Node& node, const Node* stayWithin)
{
    if (Node* child = pseudoAwareFirstChild(node))
        return child;
    return nextSkippingChildren(node, stayWithin);
}

LayoutObject* nextSiblingLayoutObject(const Node& node)
{
    for (Node* sibling = LayoutTreeBuilderTraversal::nextSibling(node); sibling; sibling = LayoutTreeBuilderTraversal::nextSibling(*sibling)) {
        LayoutObject* layoutObject = sibling->layoutObject();
        if (layoutObject && !isLayoutObjectReparented(layoutObject))
            return layoutObject;
    }
    return 0;
}

LayoutObject* previousSiblingLayoutObject(const Node& node)
{
    for (Node* sibling = LayoutTreeBuilderTraversal::previousSibling(node); sibling; sibling = LayoutTreeBuilderTraversal::previousSibling(*sibling)) {
        LayoutObject* layoutObject = sibling->layoutObject();
        if (layoutObject && !isLayoutObjectReparented(layoutObject))
            return layoutObject;
    }
    return 0;
}

LayoutObject* nextInTopLayer(const Element& element)
{
    if (!element.isInTopLayer())
        return 0;
    const WillBeHeapVector<RefPtrWillBeMember<Element>>& topLayerElements = element.document().topLayerElements();
    size_t position = topLayerElements.find(&element);
    ASSERT(position != kNotFound);
    for (size_t i = position + 1; i < topLayerElements.size(); ++i) {
        if (LayoutObject* layoutObject = topLayerElements[i]->layoutObject())
            return layoutObject;
    }
    return 0;
}

}

} // namespace
