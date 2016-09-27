/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef NodeTraversal_h
#define NodeTraversal_h

#include "core/CoreExport.h"
#include "core/dom/ContainerNode.h"
#include "core/dom/Node.h"

namespace blink {

template <class Iterator> class TraversalRange;
template <class TraversalNext> class TraversalChildrenIterator;
template <class TraversalNext> class TraversalDescendantIterator;
template <class TraversalNext> class TraversalInclusiveDescendantIterator;
template <class TraversalNext> class TraversalNextIterator;

class NodeTraversal {
public:
    using TraversalNodeType = Node;

    // Does a pre-order traversal of the tree to find the next node after this one.
    // This uses the same order that tags appear in the source file. If the stayWithin
    // argument is non-null, the traversal will stop once the specified node is reached.
    // This can be used to restrict traversal to a particular sub-tree.
    static Node* next(const Node& current) { return traverseNextTemplate(current); }
    static Node* next(const ContainerNode& current) { return traverseNextTemplate(current); }
    static Node* next(const Node& current, const Node* stayWithin) { return traverseNextTemplate(current, stayWithin); }
    static Node* next(const ContainerNode& current, const Node* stayWithin) { return traverseNextTemplate(current, stayWithin); }

    // Like next, but skips children and starts with the next sibling.
    static Node* nextSkippingChildren(const Node&);
    static Node* nextSkippingChildren(const Node&, const Node* stayWithin);

    static Node* firstWithin(const Node& current) { return current.firstChild(); }

    static Node* lastWithin(const ContainerNode&);
    static Node& lastWithinOrSelf(Node&);

    // Does a reverse pre-order traversal to find the node that comes before the current one in document order
    static Node* previous(const Node&, const Node* stayWithin = 0);

    // Like previous, but skips children and starts with the next sibling.
    static Node* previousSkippingChildren(const Node&, const Node* stayWithin = 0);

    // Like next, but visits parents after their children.
    static Node* nextPostOrder(const Node&, const Node* stayWithin = 0);

    // Like previous, but visits parents before their children.
    static Node* previousPostOrder(const Node&, const Node* stayWithin = 0);

    // Pre-order traversal including the pseudo-elements.
    static Node* previousIncludingPseudo(const Node&, const Node* stayWithin = 0);
    static Node* nextIncludingPseudo(const Node&, const Node* stayWithin = 0);
    static Node* nextIncludingPseudoSkippingChildren(const Node&, const Node* stayWithin = 0);

    CORE_EXPORT static Node* nextAncestorSibling(const Node&);
    CORE_EXPORT static Node* nextAncestorSibling(const Node&, const Node* stayWithin);
    static Node& highestAncestorOrSelf(Node&);

    // Children traversal.
    static Node* childAt(const Node& parent, unsigned index) { return childAtTemplate(parent, index); }
    static Node* childAt(const ContainerNode& parent, unsigned index) { return childAtTemplate(parent, index); }

    // These functions are provided for matching with |ComposedTreeTraversal|.
    static bool hasChildren(const Node& parent) { return firstChild(parent); }
    static bool isDescendantOf(const Node& node, const Node& other) { return node.isDescendantOf(&other); }
    static Node* firstChild(const Node& parent) { return parent.firstChild(); }
    static Node* lastChild(const Node& parent) { return parent.lastChild(); }
    static Node* nextSibling(const Node& node) { return node.nextSibling(); }
    static Node* previousSibling(const Node& node) { return node.previousSibling(); }
    static ContainerNode* parent(const Node& node) { return node.parentNode(); }
    static Node* commonAncestor(const Node& nodeA, const Node& nodeB);
    static unsigned index(const Node& node) { return node.nodeIndex(); }
    static unsigned countChildren(const Node& parent) { return parent.countChildren(); }
    static ContainerNode* parentOrShadowHostNode(const Node& node) { return node.parentOrShadowHostNode(); }

    static TraversalRange<TraversalChildrenIterator<NodeTraversal>> childrenOf(const Node&);
    static TraversalRange<TraversalDescendantIterator<NodeTraversal>> descendantsOf(const Node&);
    static TraversalRange<TraversalInclusiveDescendantIterator<NodeTraversal>> inclusiveDescendantsOf(const Node&);
    static TraversalRange<TraversalNextIterator<NodeTraversal>> startsAt(const Node*);
    static TraversalRange<TraversalNextIterator<NodeTraversal>> startsAfter(const Node&);

private:
    template <class NodeType>
    static Node* traverseNextTemplate(NodeType&);
    template <class NodeType>
    static Node* traverseNextTemplate(NodeType&, const Node* stayWithin);
    template <class NodeType>
    static Node* childAtTemplate(NodeType&, unsigned);
};

template <class Iterator>
class TraversalRange {
    STACK_ALLOCATED();
public:
    using StartNodeType = typename Iterator::StartNodeType;
    explicit TraversalRange(const StartNodeType* start) : m_start(start) { }
    Iterator begin() { return Iterator(m_start); }
    Iterator end() { return Iterator::end(); }
private:
    RawPtrWillBeMember<const StartNodeType> m_start;
};

template <class TraversalNext>
class TraversalIteratorBase {
    STACK_ALLOCATED();
public:
    using NodeType = typename TraversalNext::TraversalNodeType;
    NodeType& operator*() { return *m_current; }
    bool operator!=(const TraversalIteratorBase& rval) const { return m_current != rval.m_current ; }
protected:
    explicit TraversalIteratorBase(NodeType* current) : m_current(current) { }

    RawPtrWillBeMember<NodeType> m_current;
};

template <class TraversalNext>
class TraversalChildrenIterator : public TraversalIteratorBase<TraversalNext> {
    STACK_ALLOCATED();
public:
    using StartNodeType = Node;
    using TraversalIteratorBase<TraversalNext>::m_current;
    explicit TraversalChildrenIterator(const StartNodeType* start) : TraversalIteratorBase<TraversalNext>(TraversalNext::firstWithin(*start)) { }
    void operator++() { m_current = TraversalNext::nextSibling(*m_current); }
    static TraversalChildrenIterator end() { return TraversalChildrenIterator(); }
private:
    TraversalChildrenIterator() : TraversalIteratorBase<TraversalNext>(nullptr) { }
};

template <class TraversalNext>
class TraversalNextIterator : public TraversalIteratorBase<TraversalNext> {
    STACK_ALLOCATED();
public:
    using StartNodeType = typename TraversalNext::TraversalNodeType;
    using TraversalIteratorBase<TraversalNext>::m_current;
    explicit TraversalNextIterator(const StartNodeType* start) : TraversalIteratorBase<TraversalNext>(const_cast<StartNodeType*>(start)) { }
    void operator++() { m_current = TraversalNext::next(*m_current); }
    static TraversalNextIterator end() { return TraversalNextIterator(nullptr); }
};

template <class TraversalNext>
class TraversalDescendantIterator : public TraversalIteratorBase<TraversalNext> {
    STACK_ALLOCATED();
public:
    using StartNodeType = Node;
    using TraversalIteratorBase<TraversalNext>::m_current;
    explicit TraversalDescendantIterator(const StartNodeType* start) : TraversalIteratorBase<TraversalNext>(TraversalNext::firstWithin(*start)), m_root(start) { }
    void operator++() { m_current = TraversalNext::next(*m_current, m_root); }
    static TraversalDescendantIterator end() { return TraversalDescendantIterator(); }
private:
    TraversalDescendantIterator() : TraversalIteratorBase<TraversalNext>(nullptr), m_root(nullptr) { }
    RawPtrWillBeMember<const Node> m_root;
};

template <class TraversalNext>
class TraversalInclusiveDescendantIterator : public TraversalIteratorBase<TraversalNext> {
    STACK_ALLOCATED();
public:
    using StartNodeType = typename TraversalNext::TraversalNodeType;
    using NodeType = typename TraversalNext::TraversalNodeType;
    using TraversalIteratorBase<TraversalNext>::m_current;
    explicit TraversalInclusiveDescendantIterator(const StartNodeType* start) : TraversalIteratorBase<TraversalNext>(const_cast<NodeType*>(start)), m_root(start) { }
    void operator++() { m_current = TraversalNext::next(*m_current, m_root); }
    static TraversalInclusiveDescendantIterator end() { return TraversalInclusiveDescendantIterator(nullptr); }
private:
    RawPtrWillBeMember<const StartNodeType> m_root;
};

inline TraversalRange<TraversalChildrenIterator<NodeTraversal>> NodeTraversal::childrenOf(const Node& parent)
{
    return TraversalRange<TraversalChildrenIterator<NodeTraversal>>(&parent);
}

inline TraversalRange<TraversalDescendantIterator<NodeTraversal>> NodeTraversal::descendantsOf(const Node& root)
{
    return TraversalRange<TraversalDescendantIterator<NodeTraversal>>(&root);
}

inline TraversalRange<TraversalInclusiveDescendantIterator<NodeTraversal>> NodeTraversal::inclusiveDescendantsOf(const Node& root)
{
    return TraversalRange<TraversalInclusiveDescendantIterator<NodeTraversal>>(&root);
}

inline TraversalRange<TraversalNextIterator<NodeTraversal>> NodeTraversal::startsAt(const Node* start)
{
    return TraversalRange<TraversalNextIterator<NodeTraversal>>(start);
};

inline TraversalRange<TraversalNextIterator<NodeTraversal>> NodeTraversal::startsAfter(const Node& start)
{
    return startsAt(NodeTraversal::next(start));
};

template <class NodeType>
inline Node* NodeTraversal::traverseNextTemplate(NodeType& current)
{
    if (current.hasChildren())
        return current.firstChild();
    if (current.nextSibling())
        return current.nextSibling();
    return nextAncestorSibling(current);
}

template <class NodeType>
inline Node* NodeTraversal::traverseNextTemplate(NodeType& current, const Node* stayWithin)
{
    if (current.hasChildren())
        return current.firstChild();
    if (current == stayWithin)
        return 0;
    if (current.nextSibling())
        return current.nextSibling();
    return nextAncestorSibling(current, stayWithin);
}

inline Node* NodeTraversal::nextSkippingChildren(const Node& current)
{
    if (current.nextSibling())
        return current.nextSibling();
    return nextAncestorSibling(current);
}

inline Node* NodeTraversal::nextSkippingChildren(const Node& current, const Node* stayWithin)
{
    if (current == stayWithin)
        return 0;
    if (current.nextSibling())
        return current.nextSibling();
    return nextAncestorSibling(current, stayWithin);
}

inline Node& NodeTraversal::highestAncestorOrSelf(Node& current)
{
    Node* highest = &current;
    while (highest->parentNode())
        highest = highest->parentNode();
    return *highest;
}

template <class NodeType>
inline Node* NodeTraversal::childAtTemplate(NodeType& parent, unsigned index)
{
    Node* child = parent.firstChild();
    while (child && index--)
        child = child->nextSibling();
    return child;
}

} // namespace blink

#endif
