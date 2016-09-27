/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Peter Kelly (pmk@post.com)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2008, 2012 Apple Inc. All rights reserved.
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

#ifndef Attribute_h
#define Attribute_h

#include "core/dom/QualifiedName.h"

namespace blink {

// This is the internal representation of an attribute, consisting of a name and
// value. It is distinct from the web-exposed Attr, which also knows of the
// element to which it attached, if any.
class Attribute {
public:
    Attribute(const QualifiedName& name, const AtomicString& value)
        : m_name(name)
        , m_value(value)
    {
    }

    // NOTE: The references returned by these functions are only valid for as long
    // as the Attribute stays in place. For example, calling a function that mutates
    // an Element's internal attribute storage may invalidate them.
    const AtomicString& value() const { return m_value; }
    const AtomicString& prefix() const { return m_name.prefix(); }
    const AtomicString& localName() const { return m_name.localName(); }
    const AtomicString& namespaceURI() const { return m_name.namespaceURI(); }

    const QualifiedName& name() const { return m_name; }

    bool isEmpty() const { return m_value.isEmpty(); }
    bool matches(const QualifiedName&) const;

    void setValue(const AtomicString& value) { m_value = value; }

    // Note: This API is only for HTMLTreeBuilder.  It is not safe to change the
    // name of an attribute once parseAttribute has been called as DOM
    // elements may have placed the Attribute in a hash by name.
    void parserSetName(const QualifiedName& name) { m_name = name; }

#if COMPILER(MSVC)
    // NOTE: This constructor is not actually implemented, it's just defined so MSVC
    // will let us use a zero-length array of Attributes.
    Attribute();
#endif

private:
    QualifiedName m_name;
    AtomicString m_value;
};

inline bool Attribute::matches(const QualifiedName& qualifiedName) const
{
    if (qualifiedName.localName() != localName())
        return false;
    return qualifiedName.prefix() == starAtom || qualifiedName.namespaceURI() == namespaceURI();
}

} // namespace blink

#endif // Attribute_h
