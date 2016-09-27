// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WeakIdentifierMap_h
#define WeakIdentifierMap_h

#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"

namespace blink {

template<typename T> struct IdentifierGenerator;

template<> struct IdentifierGenerator<int> {
    using IdentifierType = int;
    static IdentifierType next()
    {
        static int s_lastId = 0;
        return ++s_lastId;
    }
};

template<typename T> struct WeakIdentifierMapTraits {
    static void removedFromIdentifierMap(T*) { }
    static void addedToIdentifierMap(T*) { }
};

template<typename T,
    typename Generator = IdentifierGenerator<int>,
    typename Traits = WeakIdentifierMapTraits<T>,
    bool isGarbageCollected = IsGarbageCollectedType<T>::value> class WeakIdentifierMap;

template<typename T, typename Generator, typename Traits> class WeakIdentifierMap<T, Generator, Traits, false> {
public:
    using IdentifierType = typename Generator::IdentifierType;
    using ReferenceType = RawPtr<WeakIdentifierMap<T, Generator, Traits, false>>;

    static IdentifierType identifier(T* object)
    {
        IdentifierType result = instance().m_objectToIdentifier.get(object);

        if (WTF::isHashTraitsEmptyValue<HashTraits<IdentifierType>>(result)) {
            result = Generator::next();
            instance().put(object, result);
        }
        return result;
    }

    static T* lookup(IdentifierType identifier)
    {
        return instance().m_identifierToObject.get(identifier);
    }

    static void notifyObjectDestroyed(T* object)
    {
        instance().objectDestroyed(object);
    }

private:
    static WeakIdentifierMap<T, Generator, Traits>& instance();
    WeakIdentifierMap() { }
    ~WeakIdentifierMap();

    void put(T* object, IdentifierType identifier)
    {
        ASSERT(object && !m_objectToIdentifier.contains(object));
        m_objectToIdentifier.set(object, identifier);
        m_identifierToObject.set(identifier, object);
        Traits::addedToIdentifierMap(object);
    }

    void objectDestroyed(T* object)
    {
        IdentifierType identifier = m_objectToIdentifier.take(object);
        if (!WTF::isHashTraitsEmptyValue<HashTraits<IdentifierType>>(identifier))
            m_identifierToObject.remove(identifier);
    }

    using ObjectToIdentifier = HashMap<T*, IdentifierType>;
    using IdentifierToObject = HashMap<IdentifierType, T*>;

    ObjectToIdentifier m_objectToIdentifier;
    IdentifierToObject m_identifierToObject;
};

template<typename T, typename Generator, typename Traits> class WeakIdentifierMap<T, Generator, Traits, true>
    : public GarbageCollected<WeakIdentifierMap<T, Generator, Traits, true>> {
public:
    using IdentifierType = typename Generator::IdentifierType;
    using ReferenceType = Persistent<WeakIdentifierMap<T, Generator, Traits, true>>;

    static IdentifierType identifier(T* object)
    {
        IdentifierType result = instance().m_objectToIdentifier->get(object);

        if (WTF::isHashTraitsEmptyValue<HashTraits<IdentifierType>>(result)) {
            result = Generator::next();
            instance().put(object, result);
        }
        return result;
    }

    static T* lookup(IdentifierType identifier)
    {
        return instance().m_identifierToObject->get(identifier);
    }

    static void notifyObjectDestroyed(T* object) { }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_objectToIdentifier);
        visitor->trace(m_identifierToObject);
    }

private:
    static WeakIdentifierMap<T, Generator, Traits>& instance();

    WeakIdentifierMap()
        : m_objectToIdentifier(new ObjectToIdentifier())
        , m_identifierToObject(new IdentifierToObject())
    {
    }

    void put(T* object, IdentifierType identifier)
    {
        ASSERT(object && !m_objectToIdentifier->contains(object));
        m_objectToIdentifier->set(object, identifier);
        m_identifierToObject->set(identifier, object);
    }

    using ObjectToIdentifier = HeapHashMap<WeakMember<T>, IdentifierType>;
    using IdentifierToObject = HeapHashMap<IdentifierType, WeakMember<T>>;

    Member<ObjectToIdentifier> m_objectToIdentifier;
    Member<IdentifierToObject> m_identifierToObject;
};

#define DECLARE_WEAK_IDENTIFIER_MAP(T, ...) \
    template<> WeakIdentifierMap<T, ##__VA_ARGS__>& WeakIdentifierMap<T, ##__VA_ARGS__>::instance(); \
    extern template class WeakIdentifierMap<T, ##__VA_ARGS__>;

#define DEFINE_WEAK_IDENTIFIER_MAP(T, ...) \
    template class WeakIdentifierMap<T, ##__VA_ARGS__>; \
    template<> WeakIdentifierMap<T, ##__VA_ARGS__>& WeakIdentifierMap<T, ##__VA_ARGS__>::instance() \
    { \
        using RefType = WeakIdentifierMap<T, ##__VA_ARGS__>::ReferenceType; \
        DEFINE_STATIC_LOCAL(RefType, mapInstance, (new WeakIdentifierMap<T, ##__VA_ARGS__>())); \
        return *mapInstance; \
    }
}

#endif // WeakIdentifierMap_h
