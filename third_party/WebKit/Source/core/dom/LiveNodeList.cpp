/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
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
 */

#include "config.h"
#include "core/dom/LiveNodeList.h"

namespace blink {

namespace {

class IsMatch {
public:
    IsMatch(const LiveNodeList& list)
        : m_list(list)
    { }

    bool operator() (const Element& element) const
    {
        return m_list.elementMatches(element);
    }

private:
    const LiveNodeList& m_list;
};

} // namespace

Node* LiveNodeList::virtualOwnerNode() const
{
    return &ownerNode();
}

void LiveNodeList::invalidateCache(Document*) const
{
    m_collectionItemsCache.invalidate();
}

unsigned LiveNodeList::length() const
{
    return m_collectionItemsCache.nodeCount(*this);
}

Element* LiveNodeList::item(unsigned offset) const
{
    return m_collectionItemsCache.nodeAt(*this, offset);
}

Element* LiveNodeList::traverseToFirst() const
{
    return ElementTraversal::firstWithin(rootNode(), IsMatch(*this));
}

Element* LiveNodeList::traverseToLast() const
{
    return ElementTraversal::lastWithin(rootNode(), IsMatch(*this));
}

Element* LiveNodeList::traverseForwardToOffset(unsigned offset, Element& currentElement, unsigned& currentOffset) const
{
    return traverseMatchingElementsForwardToOffset(currentElement, &rootNode(), offset, currentOffset, IsMatch(*this));
}

Element* LiveNodeList::traverseBackwardToOffset(unsigned offset, Element& currentElement, unsigned& currentOffset) const
{
    return traverseMatchingElementsBackwardToOffset(currentElement, &rootNode(), offset, currentOffset, IsMatch(*this));
}

DEFINE_TRACE(LiveNodeList)
{
    visitor->trace(m_collectionItemsCache);
    LiveNodeListBase::trace(visitor);
    NodeList::trace(visitor);
}

} // namespace blink
