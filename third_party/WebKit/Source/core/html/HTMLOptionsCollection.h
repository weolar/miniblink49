/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
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

#ifndef HTMLOptionsCollection_h
#define HTMLOptionsCollection_h

#include "core/html/HTMLCollection.h"
#include "core/html/HTMLOptionElement.h"

namespace blink {

class ExceptionState;
class HTMLOptionElementOrHTMLOptGroupElement;
class HTMLElementOrLong;
class NodeListOrElement;

class HTMLOptionsCollection final : public HTMLCollection {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<HTMLOptionsCollection> create(ContainerNode&, CollectionType);

    HTMLOptionElement* item(unsigned offset) const { return toHTMLOptionElement(HTMLCollection::item(offset)); }

    void add(const HTMLOptionElementOrHTMLOptGroupElement&, const HTMLElementOrLong&, ExceptionState&);
    void remove(int index);

    int selectedIndex() const;
    void setSelectedIndex(int);

    void setLength(unsigned, ExceptionState&);
    void namedGetter(const AtomicString& name, NodeListOrElement&);
    bool anonymousIndexedSetter(unsigned, PassRefPtrWillBeRawPtr<HTMLOptionElement>, ExceptionState&);

    bool elementMatches(const HTMLElement&) const;

private:
    explicit HTMLOptionsCollection(ContainerNode&);

    void supportedPropertyNames(Vector<String>& names) override;
};

DEFINE_TYPE_CASTS(HTMLOptionsCollection, LiveNodeListBase, collection, collection->type() == SelectOptions, collection.type() == SelectOptions);

inline bool HTMLOptionsCollection::elementMatches(const HTMLElement& element) const
{
    return isHTMLOptionElement(element);
}

} // namespace blink

#endif // HTMLOptionsCollection_h
