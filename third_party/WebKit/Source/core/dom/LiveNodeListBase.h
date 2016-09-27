/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
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

#ifndef LiveNodeListBase_h
#define LiveNodeListBase_h

#include "core/CoreExport.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/html/CollectionType.h"
#include "platform/heap/Handle.h"

namespace blink {

enum NodeListRootType {
    NodeListIsRootedAtNode,
    NodeListIsRootedAtDocument
};

class CORE_EXPORT LiveNodeListBase : public WillBeGarbageCollectedMixin {
public:
    LiveNodeListBase(ContainerNode& ownerNode, NodeListRootType rootType, NodeListInvalidationType invalidationType,
        CollectionType collectionType)
        : m_ownerNode(ownerNode)
        , m_rootType(rootType)
        , m_invalidationType(invalidationType)
        , m_collectionType(collectionType)
    {
        ASSERT(m_rootType == static_cast<unsigned>(rootType));
        ASSERT(m_invalidationType == static_cast<unsigned>(invalidationType));
        ASSERT(m_collectionType == static_cast<unsigned>(collectionType));

        document().registerNodeList(this);
    }

    virtual ~LiveNodeListBase()
    {
#if !ENABLE(OILPAN)
        document().unregisterNodeList(this);
#endif
    }

    ContainerNode& rootNode() const;

    void didMoveToDocument(Document& oldDocument, Document& newDocument);
    ALWAYS_INLINE bool isRootedAtDocument() const { return m_rootType == NodeListIsRootedAtDocument; }
    ALWAYS_INLINE NodeListInvalidationType invalidationType() const { return static_cast<NodeListInvalidationType>(m_invalidationType); }
    ALWAYS_INLINE CollectionType type() const { return static_cast<CollectionType>(m_collectionType); }
    ContainerNode& ownerNode() const { return *m_ownerNode; }

    virtual void invalidateCache(Document* oldDocument = 0) const = 0;
    void invalidateCacheForAttribute(const QualifiedName*) const;

    static bool shouldInvalidateTypeOnAttributeChange(NodeListInvalidationType, const QualifiedName&);

protected:
    Document& document() const { return m_ownerNode->document(); }

    ALWAYS_INLINE NodeListRootType rootType() const { return static_cast<NodeListRootType>(m_rootType); }

    template <typename MatchFunc>
    static Element* traverseMatchingElementsForwardToOffset(Element& currentElement, const ContainerNode* stayWithin, unsigned offset, unsigned& currentOffset, MatchFunc);
    template <typename MatchFunc>
    static Element* traverseMatchingElementsBackwardToOffset(Element& currentElement, const ContainerNode* stayWithin, unsigned offset, unsigned& currentOffset, MatchFunc);

    DEFINE_INLINE_VIRTUAL_TRACE() { visitor->trace(m_ownerNode); }

private:
    RefPtrWillBeMember<ContainerNode> m_ownerNode; // Cannot be null.
    const unsigned m_rootType : 1;
    const unsigned m_invalidationType : 4;
    const unsigned m_collectionType : 5;
};

ALWAYS_INLINE bool LiveNodeListBase::shouldInvalidateTypeOnAttributeChange(NodeListInvalidationType type, const QualifiedName& attrName)
{
    switch (type) {
    case InvalidateOnClassAttrChange:
        return attrName == HTMLNames::classAttr;
    case InvalidateOnNameAttrChange:
        return attrName == HTMLNames::nameAttr;
    case InvalidateOnIdNameAttrChange:
        return attrName == HTMLNames::idAttr || attrName == HTMLNames::nameAttr;
    case InvalidateOnForAttrChange:
        return attrName == HTMLNames::forAttr;
    case InvalidateForFormControls:
        return attrName == HTMLNames::nameAttr || attrName == HTMLNames::idAttr || attrName == HTMLNames::forAttr
            || attrName == HTMLNames::formAttr || attrName == HTMLNames::typeAttr;
    case InvalidateOnHRefAttrChange:
        return attrName == HTMLNames::hrefAttr;
    case DoNotInvalidateOnAttributeChanges:
        return false;
    case InvalidateOnAnyAttrChange:
        return true;
    }
    return false;
}

template <typename MatchFunc>
Element* LiveNodeListBase::traverseMatchingElementsForwardToOffset(Element& currentElement, const ContainerNode* stayWithin, unsigned offset, unsigned& currentOffset, MatchFunc isMatch)
{
    ASSERT(currentOffset < offset);
    for (Element* next = ElementTraversal::next(currentElement, stayWithin, isMatch); next; next = ElementTraversal::next(*next, stayWithin, isMatch)) {
        if (++currentOffset == offset)
            return next;
    }
    return 0;
}

template <typename MatchFunc>
Element* LiveNodeListBase::traverseMatchingElementsBackwardToOffset(Element& currentElement, const ContainerNode* stayWithin, unsigned offset, unsigned& currentOffset, MatchFunc isMatch)
{
    ASSERT(currentOffset > offset);
    for (Element* previous = ElementTraversal::previous(currentElement, stayWithin, isMatch); previous; previous = ElementTraversal::previous(*previous, stayWithin, isMatch)) {
        if (--currentOffset == offset)
            return previous;
    }
    return 0;
}

} // namespace blink

#endif // LiveNodeListBase_h
