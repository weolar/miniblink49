/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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

#ifndef TagCollection_h
#define TagCollection_h

#include "core/html/HTMLCollection.h"
#include "wtf/text/AtomicString.h"

namespace blink {

// Collection that limits to a particular tag.
class TagCollection : public HTMLCollection {
public:
    static PassRefPtrWillBeRawPtr<TagCollection> create(ContainerNode& rootNode, const AtomicString& namespaceURI, const AtomicString& localName)
    {
        ASSERT(namespaceURI != starAtom);
        return adoptRefWillBeNoop(new TagCollection(rootNode, TagCollectionType, namespaceURI, localName));
    }

    static PassRefPtrWillBeRawPtr<TagCollection> create(ContainerNode& rootNode, CollectionType type, const AtomicString& localName)
    {
        ASSERT_UNUSED(type, type == TagCollectionType);
        return adoptRefWillBeNoop(new TagCollection(rootNode, TagCollectionType, starAtom, localName));
    }

    ~TagCollection() override;

    bool elementMatches(const Element&) const;

protected:
    TagCollection(ContainerNode& rootNode, CollectionType, const AtomicString& namespaceURI, const AtomicString& localName);

    AtomicString m_namespaceURI;
    AtomicString m_localName;
};

DEFINE_TYPE_CASTS(TagCollection, LiveNodeListBase, collection, collection->type() == TagCollectionType, collection.type() == TagCollectionType);

} // namespace blink

#endif // TagCollection_h
