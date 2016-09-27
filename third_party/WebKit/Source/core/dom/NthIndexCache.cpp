// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/NthIndexCache.h"

#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"

namespace blink {

NthIndexCache::NthIndexCache(Document& document)
    : m_document(&document)
#if ENABLE(ASSERT)
    , m_domTreeVersion(document.domTreeVersion())
#endif
{
    document.setNthIndexCache(this);
}

NthIndexCache::~NthIndexCache()
{
    ASSERT(m_domTreeVersion == m_document->domTreeVersion());
    m_document->setNthIndexCache(nullptr);
}

NthIndexData& NthIndexCache::ensureNthIndexDataFor(Node& parent)
{
    if (!m_parentMap)
        m_parentMap = adoptPtrWillBeNoop(new ParentMap());

    ParentMap::AddResult addResult = m_parentMap->add(&parent, nullptr);
    if (addResult.isNewEntry)
        addResult.storedValue->value = adoptPtrWillBeNoop(new NthIndexData());

    ASSERT(addResult.storedValue->value);
    return *addResult.storedValue->value;
}

NthIndexCache::IndexByType& NthIndexCache::ensureTypeIndexMap(Node& parent)
{
    if (!m_parentMapForType)
        m_parentMapForType = adoptPtrWillBeNoop(new ParentMapForType());

    ParentMapForType::AddResult addResult = m_parentMapForType->add(&parent, nullptr);
    if (addResult.isNewEntry)
        addResult.storedValue->value = adoptPtrWillBeNoop(new IndexByType());

    ASSERT(addResult.storedValue->value);
    return *addResult.storedValue->value;
}

NthIndexData& NthIndexCache::nthIndexDataWithTagName(Element& element)
{
    IndexByType::AddResult addResult = ensureTypeIndexMap(*element.parentNode()).add(element.tagName(), nullptr);
    if (addResult.isNewEntry)
        addResult.storedValue->value = adoptPtrWillBeNoop(new NthIndexData());
    return *addResult.storedValue->value;
}

unsigned NthIndexData::nthIndex(Element& element)
{
    if (element.isPseudoElement())
        return 1;
    if (!m_count)
        return cacheNthIndices(element);

    unsigned index = 0;
    for (Element* sibling = &element; sibling; sibling = ElementTraversal::previousSibling(*sibling), index++) {
        auto it = m_elementIndexMap.find(sibling);
        if (it != m_elementIndexMap.end())
            return it->value + index;
    }
    return index;
}

unsigned NthIndexData::nthIndexOfType(Element& element, const QualifiedName& type)
{
    if (element.isPseudoElement())
        return 1;
    if (!m_count)
        return cacheNthIndicesOfType(element, type);
    unsigned index = 0;
    for (Element* sibling = &element; sibling; sibling = ElementTraversal::previousSibling(*sibling, HasTagName(type)), index++) {
        auto it = m_elementIndexMap.find(sibling);
        if (it != m_elementIndexMap.end())
            return it->value + index;
    }
    return index;
}

unsigned NthIndexData::nthLastIndex(Element& element)
{
    if (element.isPseudoElement())
        return 1;
    unsigned index = nthIndex(element);
    return m_count - index + 1;
}

unsigned NthIndexData::nthLastIndexOfType(Element& element, const QualifiedName& type)
{
    if (element.isPseudoElement())
        return 1;
    unsigned index = nthIndexOfType(element, type);
    return m_count - index + 1;
}

unsigned NthIndexData::cacheNthIndices(Element& element)
{
    ASSERT(!element.isPseudoElement());
    ASSERT(m_elementIndexMap.isEmpty());
    unsigned index = 0;
    // The frequency at which we cache the nth-index for a set of siblings.
    // A spread value of 3 means every third Element will have its nth-index cached.
    // Using a spread value > 1 is done to save memory. Looking up the nth-index will
    // still be done in constant time in terms of sibling count, at most 'spread'
    // elements will be traversed.
    const unsigned spread = 3;
    unsigned count = 0;
    for (Element* sibling = ElementTraversal::firstChild(*element.parentNode()); sibling; sibling = ElementTraversal::nextSibling(*sibling)) {
        if (!(++count % spread))
            m_elementIndexMap.add(sibling, count);
        if (sibling == &element)
            index = count;
    }
    ASSERT(count && index);
    m_count = count;
    return index;
}

unsigned NthIndexData::cacheNthIndicesOfType(Element& element, const QualifiedName& type)
{
    ASSERT(!element.isPseudoElement());
    ASSERT(m_elementIndexMap.isEmpty());
    unsigned index = 0;
    // The frequency at which we cache the nth-index of type for a set of siblings.
    // A spread value of 3 means every third Element of its type will have its nth-index cached.
    // Using a spread value > 1 is done to save memory. Looking up the nth-index of its type will
    // still be done in less time, as most number of elements traversed
    // will be equal to find 'spread' elements in the sibling set.
    const unsigned spread = 3;
    unsigned count = 0;
    for (Element* sibling = ElementTraversal::firstChild(*element.parentNode(), HasTagName(type)); sibling; sibling = ElementTraversal::nextSibling(*sibling, HasTagName(type))) {
        if (!(++count % spread))
            m_elementIndexMap.add(sibling, count);
        if (sibling == &element)
            index = count;
    }
    ASSERT(count && index);
    m_count = count;
    return index;
}

DEFINE_TRACE(NthIndexData)
{
#if ENABLE(OILPAN)
    visitor->trace(m_elementIndexMap);
#endif
}

#if !ENABLE(OILPAN)
NthIndexData::~NthIndexData()
{
}
#endif

} // namespace blink
