/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Peter Kelly (pmk@post.com)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2013 Apple Inc. All rights reserved.
 *           (C) 2007 Eric Seidel (eric@webkit.org)
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
#include "core/dom/NamedNodeMap.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Attr.h"
#include "core/dom/Element.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

using namespace HTMLNames;

#if !ENABLE(OILPAN)
void NamedNodeMap::ref()
{
    m_element->ref();
}

void NamedNodeMap::deref()
{
    m_element->deref();
}
#endif

PassRefPtrWillBeRawPtr<Attr> NamedNodeMap::getNamedItem(const AtomicString& name) const
{
    return m_element->getAttributeNode(name);
}

PassRefPtrWillBeRawPtr<Attr> NamedNodeMap::getNamedItemNS(const AtomicString& namespaceURI, const AtomicString& localName) const
{
    return m_element->getAttributeNodeNS(namespaceURI, localName);
}

PassRefPtrWillBeRawPtr<Attr> NamedNodeMap::removeNamedItem(const AtomicString& name, ExceptionState& exceptionState)
{
    size_t index = m_element->attributes().findIndex(name, m_element->shouldIgnoreAttributeCase());
    if (index == kNotFound) {
        exceptionState.throwDOMException(NotFoundError, "No item with name '" + name + "' was found.");
        return nullptr;
    }
    return m_element->detachAttribute(index);
}

PassRefPtrWillBeRawPtr<Attr> NamedNodeMap::removeNamedItemNS(const AtomicString& namespaceURI, const AtomicString& localName, ExceptionState& exceptionState)
{
    size_t index = m_element->attributes().findIndex(QualifiedName(nullAtom, localName, namespaceURI));
    if (index == kNotFound) {
        exceptionState.throwDOMException(NotFoundError, "No item with name '" + namespaceURI + "::" + localName + "' was found.");
        return nullptr;
    }
    return m_element->detachAttribute(index);
}

PassRefPtrWillBeRawPtr<Attr> NamedNodeMap::setNamedItem(Attr* attr, ExceptionState& exceptionState)
{
    ASSERT(attr);
    return m_element->setAttributeNode(attr, exceptionState);
}

PassRefPtrWillBeRawPtr<Attr> NamedNodeMap::setNamedItemNS(Attr* attr, ExceptionState& exceptionState)
{
    ASSERT(attr);
    return m_element->setAttributeNodeNS(attr, exceptionState);
}

PassRefPtrWillBeRawPtr<Attr> NamedNodeMap::item(unsigned index) const
{
    AttributeCollection attributes = m_element->attributes();
    if (index >= attributes.size())
        return nullptr;
    return m_element->ensureAttr(attributes[index].name());
}

size_t NamedNodeMap::length() const
{
    return m_element->attributes().size();
}

DEFINE_TRACE(NamedNodeMap)
{
    visitor->trace(m_element);
}

} // namespace blink
