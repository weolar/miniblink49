/*
 * Copyright (C) 2012,2013 Google Inc. All rights reserved.
 * Copyright (C) 2014 Apple Inc. All rights reserved.
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

#ifndef CollectionItemsCache_h
#define CollectionItemsCache_h

#include "core/html/CollectionIndexCache.h"
#include "wtf/Vector.h"
#include <v8.h>

namespace blink {

template <typename Collection, typename NodeType>
class CollectionItemsCache : public CollectionIndexCache<Collection, NodeType> {
    typedef CollectionIndexCache<Collection, NodeType> Base;

public:
    CollectionItemsCache();
    ~CollectionItemsCache();

    unsigned nodeCount(const Collection&);
    NodeType* nodeAt(const Collection&, unsigned index);
    void invalidate();

private:
    ptrdiff_t allocationSize() const { return m_cachedList.capacity() * sizeof(NodeType*); }
    static void reportExtraMemoryCostForCollectionItemsCache(ptrdiff_t diff)
    {
        v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(diff);
    }

    bool m_listValid;
    Vector<NodeType*> m_cachedList;
};

template <typename Collection, typename NodeType>
CollectionItemsCache<Collection, NodeType>::CollectionItemsCache()
    : m_listValid(false)
{
}

template <typename Collection, typename NodeType>
CollectionItemsCache<Collection, NodeType>::~CollectionItemsCache()
{
    if (ptrdiff_t diff = allocationSize())
        reportExtraMemoryCostForCollectionItemsCache(-diff);
}

template <typename Collection, typename NodeType>
void CollectionItemsCache<Collection, NodeType>::invalidate()
{
    Base::invalidate();
    if (m_listValid) {
        m_cachedList.shrink(0);
        m_listValid = false;
    }
}

template <class Collection, class NodeType>
unsigned CollectionItemsCache<Collection, NodeType>::nodeCount(const Collection& collection)
{
    if (this->isCachedNodeCountValid())
        return this->cachedNodeCount();

    NodeType* currentNode = collection.traverseToFirst();
    unsigned currentIndex = 0;
    ptrdiff_t oldCapacity = allocationSize();
    while (currentNode) {
        m_cachedList.append(currentNode);
        currentNode = collection.traverseForwardToOffset(currentIndex + 1, *currentNode, currentIndex);
    }
    if (ptrdiff_t diff = allocationSize() - oldCapacity)
        reportExtraMemoryCostForCollectionItemsCache(diff);

    this->setCachedNodeCount(m_cachedList.size());
    m_listValid = true;
    return this->cachedNodeCount();
}

template <typename Collection, typename NodeType>
inline NodeType* CollectionItemsCache<Collection, NodeType>::nodeAt(const Collection& collection, unsigned index)
{
    if (m_listValid) {
        ASSERT(this->isCachedNodeCountValid());
        return index < this->cachedNodeCount() ? m_cachedList[index] : 0;
    }
    return Base::nodeAt(collection, index);
}

} // namespace blink

#endif // CollectionItemsCache_h
