/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
 */

#ifndef HTMLTagCollection_h
#define HTMLTagCollection_h

#include "core/dom/Element.h"
#include "core/dom/TagCollection.h"

namespace blink {

// Collection that limits to a particular tag and whose rootNode is in an HTMLDocument.
class HTMLTagCollection final : public TagCollection {
public:
    static PassRefPtrWillBeRawPtr<HTMLTagCollection> create(ContainerNode& rootNode, CollectionType type, const AtomicString& localName)
    {
        ASSERT_UNUSED(type, type == HTMLTagCollectionType);
        return adoptRefWillBeNoop(new HTMLTagCollection(rootNode, localName));
    }

    bool elementMatches(const Element&) const;

private:
    HTMLTagCollection(ContainerNode& rootNode, const AtomicString& localName);

    AtomicString m_loweredLocalName;
};

DEFINE_TYPE_CASTS(HTMLTagCollection, LiveNodeListBase, collection, collection->type() == HTMLTagCollectionType, collection.type() == HTMLTagCollectionType);

inline bool HTMLTagCollection::elementMatches(const Element& testElement) const
{
    // Implements http://dvcs.w3.org/hg/domcore/raw-file/tip/Overview.html#concept-getelementsbytagname
    if (m_localName != starAtom) {
        const AtomicString& localName = testElement.isHTMLElement() ? m_loweredLocalName : m_localName;
        if (localName != testElement.localName())
            return false;
    }
    ASSERT(m_namespaceURI == starAtom);
    return true;
}

} // namespace blink

#endif // HTMLTagCollection_h
