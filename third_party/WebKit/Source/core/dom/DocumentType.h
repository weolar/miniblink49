/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef DocumentType_h
#define DocumentType_h

#include "core/dom/Node.h"

namespace blink {

class DocumentType final : public Node {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<DocumentType> create(Document* document, const String& name, const String& publicId, const String& systemId)
    {
        return adoptRefWillBeNoop(new DocumentType(document, name, publicId, systemId));
    }

    const String& name() const { return m_name; }
    const String& publicId() const { return m_publicId; }
    const String& systemId() const { return m_systemId; }

private:
    DocumentType(Document*, const String& name, const String& publicId, const String& systemId);

    KURL baseURI() const override;
    String nodeName() const override;
    NodeType nodeType() const override;
    PassRefPtrWillBeRawPtr<Node> cloneNode(bool deep) override;

    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) override;

    String m_name;
    String m_publicId;
    String m_systemId;
};

DEFINE_NODE_TYPE_CASTS(DocumentType, isDocumentTypeNode());

} // namespace blink

#endif // DocumentType_h
