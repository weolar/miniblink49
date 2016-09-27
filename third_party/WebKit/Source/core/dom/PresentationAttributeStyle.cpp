/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#include "config.h"
#include "core/dom/PresentationAttributeStyle.h"

#include "core/css/StylePropertySet.h"
#include "core/dom/Attribute.h"
#include "core/dom/Element.h"
#include "core/html/HTMLInputElement.h"
#include "platform/Timer.h"
#include "wtf/HashFunctions.h"
#include "wtf/HashMap.h"
#include "wtf/text/CString.h"

namespace blink {

using namespace HTMLNames;

struct PresentationAttributeCacheKey {
    PresentationAttributeCacheKey() : tagName(nullptr) { }
    StringImpl* tagName;
    Vector<std::pair<StringImpl*, AtomicString>, 3> attributesAndValues;
};

static bool operator!=(const PresentationAttributeCacheKey& a, const PresentationAttributeCacheKey& b)
{
    if (a.tagName != b.tagName)
        return true;
    return a.attributesAndValues != b.attributesAndValues;
}

struct PresentationAttributeCacheEntry final : public NoBaseWillBeGarbageCollectedFinalized<PresentationAttributeCacheEntry> {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(PresentationAttributeCacheEntry);
public:
    DEFINE_INLINE_TRACE() { visitor->trace(value); }

    PresentationAttributeCacheKey key;
    RefPtrWillBeMember<StylePropertySet> value;
};

using PresentationAttributeCache = WillBeHeapHashMap<unsigned, OwnPtrWillBeMember<PresentationAttributeCacheEntry>, AlreadyHashed>;
static PresentationAttributeCache& presentationAttributeCache()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<PresentationAttributeCache>, cache, (adoptPtrWillBeNoop(new PresentationAttributeCache())));
    return *cache;
}

class PresentationAttributeCacheCleaner {
    WTF_MAKE_NONCOPYABLE(PresentationAttributeCacheCleaner); WTF_MAKE_FAST_ALLOCATED(PresentationAttributeCacheCleaner);
public:
    PresentationAttributeCacheCleaner()
        : m_hitCount(0)
        , m_cleanTimer(this, &PresentationAttributeCacheCleaner::cleanCache)
    {
    }

    void didHitPresentationAttributeCache()
    {
        if (presentationAttributeCache().size() < minimumPresentationAttributeCacheSizeForCleaning)
            return;

        m_hitCount++;

        if (!m_cleanTimer.isActive())
            m_cleanTimer.startOneShot(presentationAttributeCacheCleanTimeInSeconds, FROM_HERE);
    }

private:
    static const unsigned presentationAttributeCacheCleanTimeInSeconds = 60;
    static const unsigned minimumPresentationAttributeCacheSizeForCleaning = 100;
    static const unsigned minimumPresentationAttributeCacheHitCountPerMinute = (100 * presentationAttributeCacheCleanTimeInSeconds) / 60;

    void cleanCache(Timer<PresentationAttributeCacheCleaner>* timer)
    {
        ASSERT_UNUSED(timer, timer == &m_cleanTimer);
        unsigned hitCount = m_hitCount;
        m_hitCount = 0;
        if (hitCount > minimumPresentationAttributeCacheHitCountPerMinute)
            return;
        presentationAttributeCache().clear();
    }

    unsigned m_hitCount;
    Timer<PresentationAttributeCacheCleaner> m_cleanTimer;
};

static bool attributeNameSort(const pair<StringImpl*, AtomicString>& p1, const pair<StringImpl*, AtomicString>& p2)
{
    // Sort based on the attribute name pointers. It doesn't matter what the order is as long as it is always the same.
    return p1.first < p2.first;
}

static void makePresentationAttributeCacheKey(Element& element, PresentationAttributeCacheKey& result)
{
    // FIXME: Enable for SVG.
    if (!element.isHTMLElement())
        return;
    // Interpretation of the size attributes on <input> depends on the type attribute.
    if (isHTMLInputElement(element))
        return;
    AttributeCollection attributes = element.attributesWithoutUpdate();
    for (const Attribute& attr : attributes) {
        if (!element.isPresentationAttribute(attr.name()))
            continue;
        if (!attr.namespaceURI().isNull())
            return;
        // FIXME: Background URL may depend on the base URL and can't be shared. Disallow caching.
        if (attr.name() == backgroundAttr)
            return;
        result.attributesAndValues.append(std::make_pair(attr.localName().impl(), attr.value()));
    }
    if (result.attributesAndValues.isEmpty())
        return;
    // Attribute order doesn't matter. Sort for easy equality comparison.
    std::sort(result.attributesAndValues.begin(), result.attributesAndValues.end(), attributeNameSort);
    // The cache key is non-null when the tagName is set.
    result.tagName = element.localName().impl();
}

static unsigned computePresentationAttributeCacheHash(const PresentationAttributeCacheKey& key)
{
    if (!key.tagName)
        return 0;
    ASSERT(key.attributesAndValues.size());
    unsigned attributeHash = StringHasher::hashMemory(key.attributesAndValues.data(), key.attributesAndValues.size() * sizeof(key.attributesAndValues[0]));
    return WTF::pairIntHash(key.tagName->existingHash(), attributeHash);
}

PassRefPtrWillBeRawPtr<StylePropertySet> computePresentationAttributeStyle(Element& element)
{
    DEFINE_STATIC_LOCAL(PresentationAttributeCacheCleaner, cacheCleaner, ());

    ASSERT(element.isStyledElement());

    PresentationAttributeCacheKey cacheKey;
    makePresentationAttributeCacheKey(element, cacheKey);

    unsigned cacheHash = computePresentationAttributeCacheHash(cacheKey);

    PresentationAttributeCache::ValueType* cacheValue;
    if (cacheHash) {
        cacheValue = presentationAttributeCache().add(cacheHash, nullptr).storedValue;
        if (cacheValue->value && cacheValue->value->key != cacheKey)
            cacheHash = 0;
    } else {
        cacheValue = nullptr;
    }

    RefPtrWillBeRawPtr<StylePropertySet> style = nullptr;
    if (cacheHash && cacheValue->value) {
        style = cacheValue->value->value;
        cacheCleaner.didHitPresentationAttributeCache();
    } else {
        style = MutableStylePropertySet::create(element.isSVGElement() ? SVGAttributeMode : HTMLAttributeMode);
        AttributeCollection attributes = element.attributesWithoutUpdate();
        for (const Attribute& attr : attributes)
            element.collectStyleForPresentationAttribute(attr.name(), attr.value(), toMutableStylePropertySet(style));
    }

    if (!cacheHash || cacheValue->value)
        return style.release();

    OwnPtrWillBeRawPtr<PresentationAttributeCacheEntry> newEntry = adoptPtrWillBeNoop(new PresentationAttributeCacheEntry);
    newEntry->key = cacheKey;
    newEntry->value = style;

    static const unsigned presentationAttributeCacheMaximumSize = 4096;
    if (presentationAttributeCache().size() > presentationAttributeCacheMaximumSize) {
        // FIXME: Discarding the entire cache when it gets too big is probably bad
        // since it creates a perf "cliff". Perhaps we should use an LRU?
        presentationAttributeCache().clear();
        presentationAttributeCache().set(cacheHash, newEntry.release());
    } else {
        cacheValue->value = newEntry.release();
    }

    return style.release();
}

} // namespace blink
