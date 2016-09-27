/*
 * Copyright (C) 2006, 2011, 2012 Apple Computer, Inc.
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

#include "config.h"
#include "core/html/HTMLOptionsCollection.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/StaticNodeList.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLSelectElement.h"

namespace blink {

HTMLOptionsCollection::HTMLOptionsCollection(ContainerNode& select)
    : HTMLCollection(select, SelectOptions, DoesNotOverrideItemAfter)
{
    ASSERT(isHTMLSelectElement(select));
}

void HTMLOptionsCollection::supportedPropertyNames(Vector<String>& names)
{
    // As per http://www.whatwg.org/specs/web-apps/current-work/multipage/common-dom-interfaces.html#htmloptionscollection:
    // The supported property names consist of the non-empty values of all the id and name attributes of all the elements
    // represented by the collection, in tree order, ignoring later duplicates, with the id of an element preceding its
    // name if it contributes both, they differ from each other, and neither is the duplicate of an earlier entry.
    HashSet<AtomicString> existingNames;
    unsigned length = this->length();
    for (unsigned i = 0; i < length; ++i) {
        Element* element = item(i);
        ASSERT(element);
        const AtomicString& idAttribute = element->getIdAttribute();
        if (!idAttribute.isEmpty()) {
            HashSet<AtomicString>::AddResult addResult = existingNames.add(idAttribute);
            if (addResult.isNewEntry)
                names.append(idAttribute);
        }
        const AtomicString& nameAttribute = element->getNameAttribute();
        if (!nameAttribute.isEmpty()) {
            HashSet<AtomicString>::AddResult addResult = existingNames.add(nameAttribute);
            if (addResult.isNewEntry)
                names.append(nameAttribute);
        }
    }
}

PassRefPtrWillBeRawPtr<HTMLOptionsCollection> HTMLOptionsCollection::create(ContainerNode& select, CollectionType)
{
    return adoptRefWillBeNoop(new HTMLOptionsCollection(select));
}

void HTMLOptionsCollection::add(const HTMLOptionElementOrHTMLOptGroupElement& element, const HTMLElementOrLong& before, ExceptionState& exceptionState)
{
    toHTMLSelectElement(ownerNode()).add(element, before, exceptionState);
}

void HTMLOptionsCollection::remove(int index)
{
    toHTMLSelectElement(ownerNode()).remove(index);
}

int HTMLOptionsCollection::selectedIndex() const
{
    return toHTMLSelectElement(ownerNode()).selectedIndex();
}

void HTMLOptionsCollection::setSelectedIndex(int index)
{
    toHTMLSelectElement(ownerNode()).setSelectedIndex(index);
}

void HTMLOptionsCollection::setLength(unsigned length, ExceptionState& exceptionState)
{
    toHTMLSelectElement(ownerNode()).setLength(length, exceptionState);
}

void HTMLOptionsCollection::namedGetter(const AtomicString& name, NodeListOrElement& returnValue)
{
    WillBeHeapVector<RefPtrWillBeMember<Element>> namedItems;
    this->namedItems(name, namedItems);

    if (!namedItems.size())
        return;

    if (namedItems.size() == 1) {
        returnValue.setElement(namedItems.at(0));
        return;
    }

    // FIXME: The spec and Firefox do not return a NodeList. They always return the first matching Element.
    returnValue.setNodeList(StaticElementList::adopt(namedItems));
}

bool HTMLOptionsCollection::anonymousIndexedSetter(unsigned index, PassRefPtrWillBeRawPtr<HTMLOptionElement> value, ExceptionState& exceptionState)
{
    HTMLSelectElement& base = toHTMLSelectElement(ownerNode());
    if (!value) { // undefined or null
        base.remove(index);
        return true;
    }
    base.setOption(index, value.get(), exceptionState);
    return true;
}

} // namespace

