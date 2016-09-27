/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
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

#ifndef LiveNodeList_h
#define LiveNodeList_h

#include "core/dom/LiveNodeListBase.h"
#include "core/dom/NodeList.h"
#include "core/html/CollectionItemsCache.h"
#include "core/html/CollectionType.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class Element;

class LiveNodeList : public NodeList, public LiveNodeListBase {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(LiveNodeList);
public:
    LiveNodeList(ContainerNode& ownerNode, CollectionType collectionType, NodeListInvalidationType invalidationType, NodeListRootType rootType = NodeListIsRootedAtNode)
        : LiveNodeListBase(ownerNode, rootType, invalidationType, collectionType) { }

    unsigned length() const final;
    Element* item(unsigned offset) const final;
    virtual bool elementMatches(const Element&) const = 0;

    void invalidateCache(Document* oldDocument = 0) const final;
    void invalidateCacheForAttribute(const QualifiedName*) const;

    // Collection IndexCache API.
    bool canTraverseBackward() const { return true; }
    Element* traverseToFirst() const;
    Element* traverseToLast() const;
    Element* traverseForwardToOffset(unsigned offset, Element& currentNode, unsigned& currentOffset) const;
    Element* traverseBackwardToOffset(unsigned offset, Element& currentNode, unsigned& currentOffset) const;

    DECLARE_VIRTUAL_TRACE();

private:
    Node* virtualOwnerNode() const final;

    mutable CollectionItemsCache<LiveNodeList, Element> m_collectionItemsCache;
};

DEFINE_TYPE_CASTS(LiveNodeList, LiveNodeListBase, list, isLiveNodeListType(list->type()), isLiveNodeListType(list.type()));

inline void LiveNodeList::invalidateCacheForAttribute(const QualifiedName* attrName) const
{
    if (!attrName || shouldInvalidateTypeOnAttributeChange(invalidationType(), *attrName))
        invalidateCache();
}

} // namespace blink

#endif // LiveNodeList_h
