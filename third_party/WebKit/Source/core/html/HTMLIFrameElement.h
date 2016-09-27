/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2004, 2006, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef HTMLIFrameElement_h
#define HTMLIFrameElement_h

#include "core/dom/DOMSettableTokenList.h"
#include "core/html/HTMLFrameElementBase.h"

namespace blink {

class HTMLIFrameElement final : public HTMLFrameElementBase, public DOMSettableTokenListObserver {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(HTMLIFrameElement);
public:
    DECLARE_NODE_FACTORY(HTMLIFrameElement);
    DECLARE_VIRTUAL_TRACE();
    ~HTMLIFrameElement() override;
    DOMSettableTokenList* sandbox() const;

private:
    explicit HTMLIFrameElement(Document&);

    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    void attributeWillChange(const QualifiedName&, const AtomicString& oldValue, const AtomicString& newValue) override;
    bool isPresentationAttribute(const QualifiedName&) const override;
    void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) override;

    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) override;

    bool layoutObjectIsNeeded(const ComputedStyle&) override;
    LayoutObject* createLayoutObject(const ComputedStyle&) override;

    bool loadedNonEmptyDocument() const override { return m_didLoadNonEmptyDocument; }
    void didLoadNonEmptyDocument() override { m_didLoadNonEmptyDocument = true; }
    bool isInteractiveContent() const override;

    void valueChanged() override;

    AtomicString m_name;
    bool m_didLoadNonEmptyDocument;
    RefPtrWillBeMember<DOMSettableTokenList> m_sandbox;
};

} // namespace blink

#endif // HTMLIFrameElement_h
