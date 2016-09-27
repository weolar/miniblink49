// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NthIndexCache_h
#define NthIndexCache_h

#include "core/CoreExport.h"
#include "core/dom/Element.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class Document;

class CORE_EXPORT NthIndexData final : public NoBaseWillBeGarbageCollected<NthIndexData> {
    WTF_MAKE_NONCOPYABLE(NthIndexData);
    DECLARE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(NthIndexData);
public:
    NthIndexData() { }

    unsigned nthIndex(Element&);
    unsigned nthIndexOfType(Element&, const QualifiedName&);
    unsigned nthLastIndex(Element&);
    unsigned nthLastIndexOfType(Element&, const QualifiedName&);

private:
    unsigned cacheNthIndices(Element&);
    unsigned cacheNthIndicesOfType(Element&, const QualifiedName&);

    WillBeHeapHashMap<RawPtrWillBeMember<Element>, unsigned> m_elementIndexMap;
    unsigned m_count = 0;

    DECLARE_TRACE();
};

class CORE_EXPORT NthIndexCache final {
    STACK_ALLOCATED();
    WTF_MAKE_NONCOPYABLE(NthIndexCache);
public:
    explicit NthIndexCache(Document&);
    ~NthIndexCache();

    unsigned nthChildIndex(Element& element)
    {
        ASSERT(element.parentNode());
        return ensureNthIndexDataFor(*element.parentNode()).nthIndex(element);
    }

    unsigned nthChildIndexOfType(Element& element, const QualifiedName& type)
    {
        ASSERT(element.parentNode());
        return nthIndexDataWithTagName(element).nthIndexOfType(element, type);
    }

    unsigned nthLastChildIndex(Element& element)
    {
        ASSERT(element.parentNode());
        return ensureNthIndexDataFor(*element.parentNode()).nthLastIndex(element);
    }

    unsigned nthLastChildIndexOfType(Element& element, const QualifiedName& type)
    {
        ASSERT(element.parentNode());
        return nthIndexDataWithTagName(element).nthLastIndexOfType(element, type);
    }

private:
    using IndexByType = WillBeHeapHashMap<String, OwnPtrWillBeMember<NthIndexData>>;
    using ParentMap = WillBeHeapHashMap<RefPtrWillBeMember<Node>, OwnPtrWillBeMember<NthIndexData>>;
    using ParentMapForType = WillBeHeapHashMap<RefPtrWillBeMember<Node>, OwnPtrWillBeMember<IndexByType>>;

    NthIndexData& ensureNthIndexDataFor(Node&);
    IndexByType& ensureTypeIndexMap(Node&);
    NthIndexData& nthIndexDataWithTagName(Element&);

    RawPtrWillBeMember<Document> m_document;
    OwnPtrWillBeMember<ParentMap> m_parentMap;
    OwnPtrWillBeMember<ParentMapForType> m_parentMapForType;

#if ENABLE(ASSERT)
    uint64_t m_domTreeVersion;
#endif
};

} // namespace blink

#endif // NthIndexCache_h
