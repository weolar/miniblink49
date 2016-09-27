/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef MatchedPropertiesCache_h
#define MatchedPropertiesCache_h

#include "core/css/StylePropertySet.h"
#include "core/css/resolver/MatchResult.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"

namespace blink {

class ComputedStyle;
class StyleResolverState;

class CachedMatchedProperties final : public NoBaseWillBeGarbageCollectedFinalized<CachedMatchedProperties> {

public:
    WillBeHeapVector<MatchedProperties> matchedProperties;
    RefPtr<ComputedStyle> computedStyle;
    RefPtr<ComputedStyle> parentComputedStyle;

    void set(const ComputedStyle&, const ComputedStyle& parentStyle, const MatchResult&);
    void clear();
    DEFINE_INLINE_TRACE()
    {
#if ENABLE(OILPAN)
        visitor->trace(matchedProperties);
#endif
    }
};

// Specialize the HashTraits for CachedMatchedProperties to check for dead
// entries in the MatchedPropertiesCache.
#if ENABLE(OILPAN)
struct CachedMatchedPropertiesHashTraits : HashTraits<Member<CachedMatchedProperties>> {
    static const WTF::WeakHandlingFlag weakHandlingFlag = WTF::WeakHandlingInCollections;

    template<typename VisitorDispatcher>
    static bool traceInCollection(VisitorDispatcher visitor, Member<CachedMatchedProperties>& cachedProperties, WTF::ShouldWeakPointersBeMarkedStrongly strongify)
    {
        // Only honor the cache's weakness semantics if the collection is traced
        // with WeakPointersActWeak. Otherwise just trace the cachedProperties
        // strongly, ie. call trace on it.
        if (cachedProperties && strongify == WTF::WeakPointersActWeak) {
            // A given cache entry is only kept alive if none of the MatchedProperties
            // in the CachedMatchedProperties value contain a dead "properties" field.
            // If there is a dead field the entire cache entry is removed.
            for (const auto& matchedProperties : cachedProperties->matchedProperties) {
                if (!Heap::isHeapObjectAlive(matchedProperties.properties)) {
                    // For now report the cache entry as dead. This might not
                    // be the final result if in a subsequent call for this entry,
                    // the "properties" field has been marked via another path.
                    return true;
                }
            }
        }
        // At this point none of the entries in the matchedProperties vector
        // had a dead "properties" field so trace CachedMatchedProperties strongly.
        // FIXME: traceInCollection is also called from WeakProcessing to check if the entry is dead.
        // Avoid calling trace in that case by only calling trace when cachedProperties is not yet marked.
        if (!Heap::isHeapObjectAlive(cachedProperties))
            visitor->trace(cachedProperties);
        return false;
    }
};
#endif

class MatchedPropertiesCache {
    DISALLOW_ALLOCATION();
    WTF_MAKE_NONCOPYABLE(MatchedPropertiesCache);
public:
    MatchedPropertiesCache();

    const CachedMatchedProperties* find(unsigned hash, const StyleResolverState&, const MatchResult&);
    void add(const ComputedStyle&, const ComputedStyle& parentStyle, unsigned hash, const MatchResult&);

    void clear();
    void clearViewportDependent();

    static bool isCacheable(const Element*, const ComputedStyle&, const ComputedStyle& parentStyle);

    DECLARE_TRACE();

private:
#if ENABLE(OILPAN)
    typedef HeapHashMap<unsigned, Member<CachedMatchedProperties>, DefaultHash<unsigned>::Hash, HashTraits<unsigned>, CachedMatchedPropertiesHashTraits > Cache;
#else
    // Every N additions to the matched declaration cache trigger a sweep where entries holding
    // the last reference to a style declaration are garbage collected.
    void sweep(Timer<MatchedPropertiesCache>*);

    unsigned m_additionsSinceLastSweep;

    typedef HashMap<unsigned, OwnPtr<CachedMatchedProperties>> Cache;
    Timer<MatchedPropertiesCache> m_sweepTimer;
#endif
    Cache m_cache;
};

}

#endif
