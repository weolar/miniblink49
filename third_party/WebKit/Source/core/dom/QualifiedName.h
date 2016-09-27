/*
 * Copyright (C) 2005, 2006, 2009 Apple Inc. All rights reserved.
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

#ifndef QualifiedName_h
#define QualifiedName_h

#include "core/CoreExport.h"
#include "wtf/HashTableDeletedValueType.h"
#include "wtf/HashTraits.h"
#include "wtf/RefCounted.h"
#include "wtf/text/AtomicString.h"

namespace blink {

struct QualifiedNameComponents {
    StringImpl* m_prefix;
    StringImpl* m_localName;
    StringImpl* m_namespace;
};

// This struct is used to pass data between QualifiedName and the QNameTranslator.
// For hashing and equality only the QualifiedNameComponents fields are used.
struct QualifiedNameData {
    QualifiedNameComponents m_components;
    bool m_isStatic;
};

class CORE_EXPORT QualifiedName {
    WTF_MAKE_FAST_ALLOCATED(QualifiedName);
public:
    class QualifiedNameImpl : public RefCounted<QualifiedNameImpl> {
    public:
        static PassRefPtr<QualifiedNameImpl> create(const AtomicString& prefix, const AtomicString& localName, const AtomicString& namespaceURI, bool isStatic)
        {
            return adoptRef(new QualifiedNameImpl(prefix, localName, namespaceURI, isStatic));
        }

        ~QualifiedNameImpl();

        unsigned computeHash() const;

        void ref()
        {
            if (m_isStatic)
                return;
            RefCounted<QualifiedNameImpl>::ref();
        }

        void deref()
        {
            if (m_isStatic)
                return;
            RefCounted<QualifiedNameImpl>::deref();
        }

        // We rely on StringHasher's hashMemory clearing out the top 8 bits when
        // doing hashing and use one of the bits for the m_isStatic value.
        mutable unsigned m_existingHash : 24;
        unsigned m_isStatic : 1;
        const AtomicString m_prefix;
        const AtomicString m_localName;
        const AtomicString m_namespace;
        mutable AtomicString m_localNameUpper;

    private:
        QualifiedNameImpl(const AtomicString& prefix, const AtomicString& localName, const AtomicString& namespaceURI, bool isStatic)
            : m_existingHash(0)
            , m_isStatic(isStatic)
            , m_prefix(prefix)
            , m_localName(localName)
            , m_namespace(namespaceURI)

        {
            ASSERT(!namespaceURI.isEmpty() || namespaceURI.isNull());
        }
    };

    QualifiedName(const AtomicString& prefix, const AtomicString& localName, const AtomicString& namespaceURI);
    ~QualifiedName();

    QualifiedName(const QualifiedName& other) : m_impl(other.m_impl) { }
    const QualifiedName& operator=(const QualifiedName& other) { m_impl = other.m_impl; return *this; }

    // Hash table deleted values, which are only constructed and never copied or destroyed.
    QualifiedName(WTF::HashTableDeletedValueType) : m_impl(WTF::HashTableDeletedValue) { }
    bool isHashTableDeletedValue() const { return m_impl.isHashTableDeletedValue(); }

    bool operator==(const QualifiedName& other) const { return m_impl == other.m_impl; }
    bool operator!=(const QualifiedName& other) const { return !(*this == other); }

    bool matches(const QualifiedName& other) const { return m_impl == other.m_impl || (localName() == other.localName() && namespaceURI() == other.namespaceURI()); }

    bool matchesPossiblyIgnoringCase(const QualifiedName& other, bool shouldIgnoreCase) const { return m_impl == other.m_impl || (equalPossiblyIgnoringCase(localName(), other.localName(), shouldIgnoreCase) && namespaceURI() == other.namespaceURI()); }

    bool hasPrefix() const { return m_impl->m_prefix != nullAtom; }
    void setPrefix(const AtomicString& prefix) { *this = QualifiedName(prefix, localName(), namespaceURI()); }

    const AtomicString& prefix() const { return m_impl->m_prefix; }
    const AtomicString& localName() const { return m_impl->m_localName; }
    const AtomicString& namespaceURI() const { return m_impl->m_namespace; }

    // Uppercased localName, cached for efficiency
    const AtomicString& localNameUpper() const;

    String toString() const;

    QualifiedNameImpl* impl() const { return m_impl.get(); }

    // Init routine for globals
    static void init();

    static const QualifiedName& null();

    // The below methods are only for creating static global QNames that need no ref counting.
    static void createStatic(void* targetAddress, StringImpl* name);
    static void createStatic(void* targetAddress, StringImpl* name, const AtomicString& nameNamespace);

private:
    // This constructor is used only to create global/static QNames that don't require any ref counting.
    QualifiedName(const AtomicString& prefix, const AtomicString& localName, const AtomicString& namespaceURI, bool isStatic);

    RefPtr<QualifiedNameImpl> m_impl;
};

extern const QualifiedName& anyName;
inline const QualifiedName& anyQName() { return anyName; }

inline bool operator==(const AtomicString& a, const QualifiedName& q) { return a == q.localName(); }
inline bool operator!=(const AtomicString& a, const QualifiedName& q) { return a != q.localName(); }
inline bool operator==(const QualifiedName& q, const AtomicString& a) { return a == q.localName(); }
inline bool operator!=(const QualifiedName& q, const AtomicString& a) { return a != q.localName(); }

inline unsigned hashComponents(const QualifiedNameComponents& buf)
{
    return StringHasher::hashMemory<sizeof(QualifiedNameComponents)>(&buf);
}

struct QualifiedNameHash {
    static unsigned hash(const QualifiedName& name) { return hash(name.impl()); }

    static unsigned hash(const QualifiedName::QualifiedNameImpl* name)
    {
        if (!name->m_existingHash)
            name->m_existingHash = name->computeHash();
        return name->m_existingHash;
    }

    static bool equal(const QualifiedName& a, const QualifiedName& b) { return a == b; }
    static bool equal(const QualifiedName::QualifiedNameImpl* a, const QualifiedName::QualifiedNameImpl* b) { return a == b; }

    static const bool safeToCompareToEmptyOrDeleted = false;
};

} // namespace blink

namespace WTF {

template<> struct DefaultHash<blink::QualifiedName> {
    typedef blink::QualifiedNameHash Hash;
};

template<> struct HashTraits<blink::QualifiedName> : SimpleClassHashTraits<blink::QualifiedName> {
    static const bool emptyValueIsZero = false;
    static blink::QualifiedName emptyValue() { return blink::QualifiedName::null(); }
};
}

#endif
