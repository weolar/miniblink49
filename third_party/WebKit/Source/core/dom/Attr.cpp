/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Peter Kelly (pmk@post.com)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2009, 2010, 2012 Apple Inc. All rights reserved.
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
#include "core/dom/Attr.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/Text.h"
#include "core/events/ScopedEventQueue.h"
#include "core/frame/UseCounter.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

Attr::Attr(Element& element, const QualifiedName& name)
    : Node(&element.document(), CreateOther)
    , m_element(&element)
    , m_name(name)
{
}

Attr::Attr(Document& document, const QualifiedName& name, const AtomicString& standaloneValue)
    : Node(&document, CreateOther)
    , m_element(nullptr)
    , m_name(name)
    , m_standaloneValueOrAttachedLocalName(standaloneValue)
{
}

PassRefPtrWillBeRawPtr<Attr> Attr::create(Element& element, const QualifiedName& name)
{
    return adoptRefWillBeNoop(new Attr(element, name));
}

PassRefPtrWillBeRawPtr<Attr> Attr::create(Document& document, const QualifiedName& name, const AtomicString& value)
{
    return adoptRefWillBeNoop(new Attr(document, name, value));
}

Attr::~Attr()
{
}

const QualifiedName Attr::qualifiedName() const
{
    if (m_element && !m_standaloneValueOrAttachedLocalName.isNull()) {
        // In the unlikely case the Element attribute has a local name
        // that differs by case, construct the qualified name based on
        // it. This is the qualified name that must be used when
        // looking up the attribute on the element.
        return QualifiedName(m_name.prefix(), m_standaloneValueOrAttachedLocalName, m_name.namespaceURI());
    }

    return m_name;
}

const AtomicString& Attr::value() const
{
    if (m_element)
        return m_element->getAttribute(qualifiedName());
    return m_standaloneValueOrAttachedLocalName;
}

void Attr::setValue(const AtomicString& value)
{
    if (m_element)
        m_element->setAttribute(qualifiedName(), value);
    else
        m_standaloneValueOrAttachedLocalName = value;
}

const AtomicString& Attr::valueForBindings() const
{
    UseCounter::count(document(), UseCounter::AttrGetValue);
    return value();
}

void Attr::setValueForBindings(const AtomicString& value)
{
    UseCounter::count(document(), UseCounter::AttrSetValue);
    if (m_element)
        UseCounter::count(document(), UseCounter::AttrSetValueWithElement);
    setValue(value);
}

void Attr::setNodeValue(const String& v)
{
    // Attr uses AtomicString type for its value to save memory as there
    // is duplication among Elements' attributes values.
    setValue(AtomicString(v));
}

PassRefPtrWillBeRawPtr<Node> Attr::cloneNode(bool /*deep*/)
{
    return adoptRefWillBeNoop(new Attr(document(), m_name, value()));
}

void Attr::detachFromElementWithValue(const AtomicString& value)
{
    ASSERT(m_element);
    m_standaloneValueOrAttachedLocalName = value;
    m_element = nullptr;
}

void Attr::attachToElement(Element* element, const AtomicString& attachedLocalName)
{
    ASSERT(!m_element);
    m_element = element;
    m_standaloneValueOrAttachedLocalName = attachedLocalName;
}

DEFINE_TRACE(Attr)
{
    visitor->trace(m_element);
    Node::trace(visitor);
}

}
