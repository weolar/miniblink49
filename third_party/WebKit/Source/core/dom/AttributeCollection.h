/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AttributeCollection_h
#define AttributeCollection_h

#include "core/dom/Attr.h"
#include "core/dom/Attribute.h"
#include "wtf/Vector.h"

namespace blink {

template <typename Container, typename ContainerMemberType = Container>
class AttributeCollectionGeneric {
public:
    using ValueType = typename Container::ValueType;
    using iterator = ValueType*;

    AttributeCollectionGeneric(Container& attributes)
        : m_attributes(attributes)
    { }

    ValueType& operator[](unsigned index) const { return at(index); }
    ValueType& at(unsigned index) const
    {
        RELEASE_ASSERT(index < size());
        return begin()[index];
    }

    iterator begin() const { return m_attributes.data(); }
    iterator end() const { return begin() + size(); }

    unsigned size() const { return m_attributes.size(); }
    bool isEmpty() const { return !size(); }

    iterator find(const QualifiedName&) const;
    iterator find(const AtomicString& name, bool shouldIgnoreCase) const;
    size_t findIndex(const QualifiedName&, bool shouldIgnoreCase = false) const;
    size_t findIndex(const AtomicString& name, bool shouldIgnoreCase) const;
    size_t findIndex(Attr*) const;

protected:
    size_t findSlowCase(const AtomicString& name, bool shouldIgnoreAttributeCase) const;

    ContainerMemberType m_attributes;
};

class AttributeArray {
public:
    using ValueType = const Attribute;

    AttributeArray(const Attribute* array, unsigned size)
        : m_array(array)
        , m_size(size)
    { }

    const Attribute* data() const { return m_array; }
    unsigned size() const { return m_size; }

private:
    const Attribute* m_array;
    unsigned m_size;
};

class AttributeCollection : public AttributeCollectionGeneric<const AttributeArray> {
public:
    AttributeCollection()
        : AttributeCollectionGeneric<const AttributeArray>(AttributeArray(nullptr, 0))
    { }

    AttributeCollection(const Attribute* array, unsigned size)
        : AttributeCollectionGeneric<const AttributeArray>(AttributeArray(array, size))
    { }
};

using AttributeVector = Vector<Attribute, 4>;
class MutableAttributeCollection : public AttributeCollectionGeneric<AttributeVector, AttributeVector&> {
public:
    explicit MutableAttributeCollection(AttributeVector& attributes)
        : AttributeCollectionGeneric<AttributeVector, AttributeVector&>(attributes)
    { }

    // These functions do no error/duplicate checking.
    void append(const QualifiedName&, const AtomicString& value);
    void remove(unsigned index);
};

inline void MutableAttributeCollection::append(const QualifiedName& name, const AtomicString& value)
{
    m_attributes.append(Attribute(name, value));
}

inline void MutableAttributeCollection::remove(unsigned index)
{
    m_attributes.remove(index);
}

template <typename Container, typename ContainerMemberType>
inline typename AttributeCollectionGeneric<Container, ContainerMemberType>::iterator AttributeCollectionGeneric<Container, ContainerMemberType>::find(const AtomicString& name, bool shouldIgnoreCase) const
{
    size_t index = findIndex(name, shouldIgnoreCase);
    return index != kNotFound ? &at(index) : nullptr;
}

template <typename Container, typename ContainerMemberType>
inline size_t AttributeCollectionGeneric<Container, ContainerMemberType>::findIndex(const QualifiedName& name, bool shouldIgnoreCase) const
{
    iterator end = this->end();
    unsigned index = 0;
    for (iterator it = begin(); it != end; ++it, ++index) {
        if (it->name().matchesPossiblyIgnoringCase(name, shouldIgnoreCase))
            return index;
    }
    return kNotFound;
}

// We use a boolean parameter instead of calling shouldIgnoreAttributeCase so that the caller
// can tune the behavior (hasAttribute is case sensitive whereas getAttribute is not).
template <typename Container, typename ContainerMemberType>
inline size_t AttributeCollectionGeneric<Container, ContainerMemberType>::findIndex(const AtomicString& name, bool shouldIgnoreCase) const
{
    bool doSlowCheck = shouldIgnoreCase;

    // Optimize for the case where the attribute exists and its name exactly matches.
    iterator end = this->end();
    unsigned index = 0;
    for (iterator it = begin(); it != end; ++it, ++index) {
        // FIXME: Why check the prefix? Namespaces should be all that matter.
        // Most attributes (all of HTML and CSS) have no namespace.
        if (!it->name().hasPrefix()) {
            if (name == it->localName())
                return index;
        } else {
            doSlowCheck = true;
        }
    }

    if (doSlowCheck)
        return findSlowCase(name, shouldIgnoreCase);
    return kNotFound;
}

template <typename Container, typename ContainerMemberType>
inline typename AttributeCollectionGeneric<Container, ContainerMemberType>::iterator AttributeCollectionGeneric<Container, ContainerMemberType>::find(const QualifiedName& name) const
{
    iterator end = this->end();
    for (iterator it = begin(); it != end; ++it) {
        if (it->name().matches(name))
            return it;
    }
    return nullptr;
}

template <typename Container, typename ContainerMemberType>
size_t AttributeCollectionGeneric<Container, ContainerMemberType>::findIndex(Attr* attr) const
{
    // This relies on the fact that Attr's QualifiedName == the Attribute's name.
    iterator end = this->end();
    unsigned index = 0;
    for (iterator it = begin(); it != end; ++it, ++index) {
        if (it->name() == attr->qualifiedName())
            return index;
    }
    return kNotFound;
}

template <typename Container, typename ContainerMemberType>
size_t AttributeCollectionGeneric<Container, ContainerMemberType>::findSlowCase(const AtomicString& name, bool shouldIgnoreAttributeCase) const
{
    // Continue to checking case-insensitively and/or full namespaced names if necessary:
    iterator end = this->end();
    unsigned index = 0;
    for (iterator it = begin(); it != end; ++it, ++index) {
        // FIXME: Why check the prefix? Namespace is all that should matter
        // and all HTML/SVG attributes have a null namespace!
        if (!it->name().hasPrefix()) {
            if (shouldIgnoreAttributeCase && equalIgnoringCase(name, it->localName()))
                return index;
        } else {
            // FIXME: Would be faster to do this comparison without calling toString, which
            // generates a temporary string by concatenation. But this branch is only reached
            // if the attribute name has a prefix, which is rare in HTML.
            if (equalPossiblyIgnoringCase(name, it->name().toString(), shouldIgnoreAttributeCase))
                return index;
        }
    }
    return kNotFound;
}

} // namespace blink

#endif // AttributeCollection_h
