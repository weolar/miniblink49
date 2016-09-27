/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef HTMLDocument_h
#define HTMLDocument_h

#include "core/dom/Document.h"
#include "core/fetch/ResourceClient.h"
#include "wtf/HashCountedSet.h"

namespace blink {

class HTMLBodyElement;

class CORE_EXPORT HTMLDocument : public Document, public ResourceClient {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<HTMLDocument> create(const DocumentInit& initializer = DocumentInit())
    {
        return adoptRefWillBeNoop(new HTMLDocument(initializer));
    }
    ~HTMLDocument() override;

    const AtomicString& bgColor() const;
    void setBgColor(const AtomicString&);
    const AtomicString& fgColor() const;
    void setFgColor(const AtomicString&);
    const AtomicString& alinkColor() const;
    void setAlinkColor(const AtomicString&);
    const AtomicString& linkColor() const;
    void setLinkColor(const AtomicString&);
    const AtomicString& vlinkColor() const;
    void setVlinkColor(const AtomicString&);

    void clear() { }

    void captureEvents() { }
    void releaseEvents() { }

    void addNamedItem(const AtomicString& name);
    void removeNamedItem(const AtomicString& name);
    bool hasNamedItem(const AtomicString& name);

    void addExtraNamedItem(const AtomicString& name);
    void removeExtraNamedItem(const AtomicString& name);
    bool hasExtraNamedItem(const AtomicString& name);

    static bool isCaseSensitiveAttribute(const QualifiedName&);

    PassRefPtrWillBeRawPtr<Document> cloneDocumentWithoutChildren() final;

protected:
    HTMLDocument(const DocumentInit&, DocumentClassFlags extendedDocumentClasses = DefaultDocumentClass);

private:
    HTMLBodyElement* htmlBodyElement() const;

    const AtomicString& bodyAttributeValue(const QualifiedName&) const;
    void setBodyAttribute(const QualifiedName&, const AtomicString&);

    void addItemToMap(HashCountedSet<AtomicString>&, const AtomicString&);
    void removeItemFromMap(HashCountedSet<AtomicString>&, const AtomicString&);

    HashCountedSet<AtomicString> m_namedItemCounts;
    HashCountedSet<AtomicString> m_extraNamedItemCounts;
};

inline bool HTMLDocument::hasNamedItem(const AtomicString& name)
{
    return m_namedItemCounts.contains(name);
}

inline bool HTMLDocument::hasExtraNamedItem(const AtomicString& name)
{
    return m_extraNamedItemCounts.contains(name);
}

DEFINE_DOCUMENT_TYPE_CASTS(HTMLDocument);

} // namespace blink

#endif // HTMLDocument_h
