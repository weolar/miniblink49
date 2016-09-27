/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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

#ifndef CollectionIndexCache_h
#define CollectionIndexCache_h

namespace blink {

template <typename Collection, typename NodeType>
class CollectionIndexCache {
    DISALLOW_ALLOCATION();
public:
    CollectionIndexCache();

    bool isEmpty(const Collection& collection)
    {
        if (isCachedNodeCountValid())
            return !cachedNodeCount();
        if (cachedNode())
            return false;
        return !nodeAt(collection, 0);
    }
    bool hasExactlyOneNode(const Collection& collection)
    {
        if (isCachedNodeCountValid())
            return cachedNodeCount() == 1;
        if (cachedNode())
            return !cachedNodeIndex() && !nodeAt(collection, 1);
        return nodeAt(collection, 0) && !nodeAt(collection, 1);
    }

    unsigned nodeCount(const Collection&);
    NodeType* nodeAt(const Collection&, unsigned index);

    void invalidate();

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_currentNode);
    }

protected:
    ALWAYS_INLINE NodeType* cachedNode() const { return m_currentNode; }
    ALWAYS_INLINE unsigned cachedNodeIndex() const { ASSERT(cachedNode()); return m_cachedNodeIndex; }
    ALWAYS_INLINE void setCachedNode(NodeType* node, unsigned index)
    {
        ASSERT(node);
        m_currentNode = node;
        m_cachedNodeIndex = index;
    }

    ALWAYS_INLINE bool isCachedNodeCountValid() const { return m_isLengthCacheValid; }
    ALWAYS_INLINE unsigned cachedNodeCount() const { return m_cachedNodeCount; }
    ALWAYS_INLINE void setCachedNodeCount(unsigned length)
    {
        m_cachedNodeCount = length;
        m_isLengthCacheValid = true;
    }

private:
    NodeType* nodeBeforeCachedNode(const Collection&, unsigned index);
    NodeType* nodeAfterCachedNode(const Collection&, unsigned index);

    RawPtrWillBeMember<NodeType> m_currentNode;
    unsigned m_cachedNodeCount;
    unsigned m_cachedNodeIndex : 31;
    unsigned m_isLengthCacheValid : 1;
};

template <typename Collection, typename NodeType>
CollectionIndexCache<Collection, NodeType>::CollectionIndexCache()
    : m_currentNode(nullptr)
    , m_cachedNodeCount(0)
    , m_cachedNodeIndex(0)
    , m_isLengthCacheValid(false)
{
}

template <typename Collection, typename NodeType>
void CollectionIndexCache<Collection, NodeType>::invalidate()
{
    m_currentNode = nullptr;
    m_isLengthCacheValid = false;
}

template <typename Collection, typename NodeType>
inline unsigned CollectionIndexCache<Collection, NodeType>::nodeCount(const Collection& collection)
{
    if (isCachedNodeCountValid())
        return cachedNodeCount();

    nodeAt(collection, UINT_MAX);
    ASSERT(isCachedNodeCountValid());

    return cachedNodeCount();
}

template <typename Collection, typename NodeType>
inline NodeType* CollectionIndexCache<Collection, NodeType>::nodeAt(const Collection& collection, unsigned index)
{
    if (isCachedNodeCountValid() && index >= cachedNodeCount())
        return nullptr;

    if (cachedNode()) {
        if (index > cachedNodeIndex())
            return nodeAfterCachedNode(collection, index);
        if (index < cachedNodeIndex())
            return nodeBeforeCachedNode(collection, index);
        return cachedNode();
    }

    // No valid cache yet, let's find the first matching element.
    ASSERT(!isCachedNodeCountValid());
    NodeType* firstNode = collection.traverseToFirst();
    if (!firstNode) {
        // The collection is empty.
        setCachedNodeCount(0);
        return nullptr;
    }
    setCachedNode(firstNode, 0);
    return index ? nodeAfterCachedNode(collection, index) : firstNode;
}

template <typename Collection, typename NodeType>
inline NodeType* CollectionIndexCache<Collection, NodeType>::nodeBeforeCachedNode(const Collection& collection, unsigned index)
{
    ASSERT(cachedNode()); // Cache should be valid.
    unsigned currentIndex = cachedNodeIndex();
    ASSERT(currentIndex > index);

    // Determine if we should traverse from the beginning of the collection instead of the cached node.
    bool firstIsCloser = index < currentIndex - index;
    if (firstIsCloser || !collection.canTraverseBackward()) {
        NodeType* firstNode = collection.traverseToFirst();
        ASSERT(firstNode);
        setCachedNode(firstNode, 0);
        return index ? nodeAfterCachedNode(collection, index) : firstNode;
    }

    // Backward traversal from the cached node to the requested index.
    ASSERT(collection.canTraverseBackward());
    NodeType* currentNode = collection.traverseBackwardToOffset(index, *cachedNode(), currentIndex);
    ASSERT(currentNode);
    setCachedNode(currentNode, currentIndex);
    return currentNode;
}

template <typename Collection, typename NodeType>
inline NodeType* CollectionIndexCache<Collection, NodeType>::nodeAfterCachedNode(const Collection& collection, unsigned index)
{
    ASSERT(cachedNode()); // Cache should be valid.
    unsigned currentIndex = cachedNodeIndex();
    ASSERT(currentIndex < index);

    // Determine if we should traverse from the end of the collection instead of the cached node.
    bool lastIsCloser = isCachedNodeCountValid() && cachedNodeCount() - index < index - currentIndex;
    if (lastIsCloser && collection.canTraverseBackward()) {
        NodeType* lastItem = collection.traverseToLast();
        ASSERT(lastItem);
        setCachedNode(lastItem, cachedNodeCount() - 1);
        if (index < cachedNodeCount() - 1)
            return nodeBeforeCachedNode(collection, index);
        return lastItem;
    }

    // Forward traversal from the cached node to the requested index.
    NodeType* currentNode = collection.traverseForwardToOffset(index, *cachedNode(), currentIndex);
    if (!currentNode) {
        // Did not find the node. On plus side, we now know the length.
        setCachedNodeCount(currentIndex + 1);
        return nullptr;
    }
    setCachedNode(currentNode, currentIndex);
    return currentNode;
}

} // namespace blink

#endif // CollectionIndexCache_h
