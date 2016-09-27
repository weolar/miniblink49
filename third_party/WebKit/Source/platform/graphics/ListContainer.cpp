// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/ListContainer.h"

#include "wtf/PassOwnPtr.h"

namespace {
    const size_t kDefaultNumElementTypesToReserve = 32;
} // namespace

namespace blink {

// ListContainerCharAllocator
////////////////////////////////////////////////////
// This class deals only with char* and void*. It does allocation and passing
// out raw pointers, as well as memory deallocation when being destroyed.
class ListContainerBase::ListContainerCharAllocator {
    WTF_MAKE_NONCOPYABLE(ListContainerCharAllocator);
public:
    // ListContainerCharAllocator::InnerList
    /////////////////////////////////////////////
    // This class holds the raw memory chunk, as well as information about its
    // size and availability.
    struct InnerList {
        OwnPtr<char[]> data;
        // The number of elements in total the memory can hold. The difference
        // between capacity and size is the how many more elements this list can
        // hold.
        size_t capacity;
        // The number of elements have been put into this list.
        size_t size;
        // The size of each element is in bytes. This is used to move from between
        // elements' memory locations.
        size_t step;

        InnerList() : capacity(0), size(0), step(0) {}

        void erase(char* position)
        {
            // Confident that destructor is called by caller of this function. Since
            // ListContainerCharAllocator does not handle construction after
            // allocation, it doesn't handle desctrution before deallocation.
            ASSERT(position <= lastElement());
            ASSERT(position >= begin());
            char* start = position + step;
            std::copy(start, end(), position);

            --size;
            // Decrease capacity to avoid creating not full not last InnerList.
            --capacity;
        }

        bool isEmpty() const { return !size; }
        bool isFull() { return capacity == size; }
        size_t numElementsAvailable() const { return capacity - size; }

        void* addElement()
        {
            ASSERT(size < capacity);
            ++size;
            return lastElement();
        }

        void removeLast()
        {
            ASSERT(!isEmpty());
            --size;
        }

        char* begin() const { return data.get(); }
        char* end() const { return data.get() + size * step; }
        char* lastElement() const { return data.get() + (size - 1) * step; }
        char* elementAt(size_t index) const { return data.get() + index * step; }
    private:
        WTF_MAKE_NONCOPYABLE(InnerList);
    };

    explicit ListContainerCharAllocator(size_t elementSize)
        : m_elementSize(elementSize)
        , m_size(0)
        , m_lastListIndex(0)
        , m_lastList(nullptr)
    {
        allocateNewList(kDefaultNumElementTypesToReserve);
        m_lastList = m_storage[m_lastListIndex].get();
    }

    ListContainerCharAllocator(size_t elementSize, size_t element_count)
        : m_elementSize(elementSize)
        , m_size(0)
        , m_lastListIndex(0)
        , m_lastList(nullptr)
    {
        allocateNewList(element_count > 0 ? element_count : kDefaultNumElementTypesToReserve);
        m_lastList = m_storage[m_lastListIndex].get();
    }

    ~ListContainerCharAllocator() {}

    void* allocate()
    {
        if (m_lastList->isFull()) {
            // Only allocate a new list if there isn't a spare one still there from
            // previous usage.
            if (m_lastListIndex + 1 >= m_storage.size())
                allocateNewList(m_lastList->capacity * 2);

            ++m_lastListIndex;
            m_lastList = m_storage[m_lastListIndex].get();
        }

        ++m_size;
        return m_lastList->addElement();
    }

    size_t elementSize() const { return m_elementSize; }
    size_t listCount() const { return m_storage.size(); }
    size_t size() const { return m_size; }
    bool isEmpty() const { return size() == 0; }

    size_t capacity() const
    {
        size_t capacitySum = 0;
        for (const auto& innerList : m_storage)
            capacitySum += innerList->capacity;
        return capacitySum;
    }

    void clear()
    {
        // Remove all except for the first InnerList.
        m_storage.shrink(1);
        m_lastListIndex = 0;
        m_lastList = m_storage[m_lastListIndex].get();
        m_lastList->size = 0;
        m_size = 0;
    }

    void removeLast()
    {
        ASSERT(!isEmpty());
        m_lastList->removeLast();
        if (m_lastList->isEmpty() && m_lastListIndex > 0) {
            --m_lastListIndex;
            m_lastList = m_storage[m_lastListIndex].get();

            // If there are now two empty inner lists, free one of them.
            if (m_lastListIndex + 2 < m_storage.size())
            m_storage.removeLast();
        }
        --m_size;
    }

    void erase(PositionInListContainerCharAllocator position)
    {
        ASSERT(this == position.ptrToContainer);
        m_storage[position.vectorIndex]->erase(position.itemIterator);
        // TODO(weiliangc): Free the InnerList if it is empty.
        --m_size;
    }

    InnerList* innerListById(size_t id) const
    {
        ASSERT(id < m_storage.size());
        return m_storage[id].get();
    }

    size_t firstInnerListId() const
    {
        // |size_| > 0 means that at least one vector in |m_storage| will be
        // non-empty.
        ASSERT(m_size > 0u);
        size_t id = 0;
        while (m_storage[id]->size == 0)
            ++id;
        return id;
    }

    size_t lastInnerListId() const
    {
        // |size_| > 0 means that at least one vector in |m_storage| will be
        // non-empty.
        ASSERT(m_size > 0u);
        size_t id = m_storage.size() - 1;
        while (m_storage[id]->size == 0)
            --id;
        return id;
    }

    size_t numAvailableElementsInLastList() const
    {
        return m_lastList->numElementsAvailable();
    }

private:
    void allocateNewList(size_t listSize)
    {
        OwnPtr<InnerList> newList(adoptPtr(new InnerList));
        newList->capacity = listSize;
        newList->size = 0;
        newList->step = m_elementSize;
        newList->data = adoptArrayPtr(new char[listSize * m_elementSize]);
        m_storage.append(newList.release());
    }

    Vector<OwnPtr<InnerList>> m_storage;
    const size_t m_elementSize;

    // The number of elements in the list.
    size_t m_size;

    // The index of the last list to have had elements added to it, or the only
    // list if the container has not had elements added since being cleared.
    size_t m_lastListIndex;

    // This is equivalent to |m_storage[m_lastListIndex]|.
    InnerList* m_lastList;
};

// PositionInListContainerCharAllocator
//////////////////////////////////////////////////////
ListContainerBase::PositionInListContainerCharAllocator::PositionInListContainerCharAllocator(const ListContainerBase::PositionInListContainerCharAllocator& other)
    : ptrToContainer(other.ptrToContainer)
    , vectorIndex(other.vectorIndex)
    , itemIterator(other.itemIterator)
{
}

ListContainerBase::PositionInListContainerCharAllocator::PositionInListContainerCharAllocator(ListContainerBase::ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter)
    : ptrToContainer(container)
    , vectorIndex(vectorIndex)
    , itemIterator(itemIter)
{
}

bool ListContainerBase::PositionInListContainerCharAllocator::operator==(const ListContainerBase::PositionInListContainerCharAllocator& other) const
{
    ASSERT(ptrToContainer == other.ptrToContainer);
    return vectorIndex == other.vectorIndex && itemIterator == other.itemIterator;
}

bool ListContainerBase::PositionInListContainerCharAllocator::operator!=(const ListContainerBase::PositionInListContainerCharAllocator& other) const
{
    return !(*this == other);
}

ListContainerBase::PositionInListContainerCharAllocator ListContainerBase::PositionInListContainerCharAllocator::increment()
{
    ListContainerCharAllocator::InnerList* list = ptrToContainer->innerListById(vectorIndex);
    if (itemIterator == list->lastElement()) {
        ++vectorIndex;
        while (vectorIndex < ptrToContainer->listCount()) {
            if (ptrToContainer->innerListById(vectorIndex)->size != 0)
                break;
            ++vectorIndex;
        }
        if (vectorIndex < ptrToContainer->listCount())
            itemIterator = ptrToContainer->innerListById(vectorIndex)->begin();
        else
            itemIterator = nullptr;
    } else {
        itemIterator += list->step;
    }
    return *this;
}

ListContainerBase::PositionInListContainerCharAllocator ListContainerBase::PositionInListContainerCharAllocator::reverseIncrement()
{
    ListContainerCharAllocator::InnerList* list = ptrToContainer->innerListById(vectorIndex);
    if (itemIterator == list->begin()) {
        --vectorIndex;
        // Since |vectorIndex| is unsigned, we compare < listCount() instead of
        // comparing >= 0, as the variable will wrap around when it goes out of
        // range (below 0).
        while (vectorIndex < ptrToContainer->listCount()) {
            if (ptrToContainer->innerListById(vectorIndex)->size != 0)
                break;
            --vectorIndex;
        }
        if (vectorIndex < ptrToContainer->listCount()) {
            itemIterator =
            ptrToContainer->innerListById(vectorIndex)->lastElement();
        } else {
            itemIterator = nullptr;
        }
    } else {
        itemIterator -= list->step;
    }
    return *this;
}

// ListContainerBase
////////////////////////////////////////////
ListContainerBase::ListContainerBase(size_t maxSizeForDerivedClass)
    : m_data(adoptPtr(new ListContainerCharAllocator(maxSizeForDerivedClass)))
{
}

ListContainerBase::ListContainerBase(size_t maxSizeForDerivedClass, size_t numElementsToReserveFor)
    : m_data(adoptPtr(new ListContainerCharAllocator(maxSizeForDerivedClass, numElementsToReserveFor)))
{
}

ListContainerBase::~ListContainerBase()
{
}

void ListContainerBase::removeLast()
{
    m_data->removeLast();
}

void ListContainerBase::eraseAndInvalidateAllPointers(ListContainerBase::Iterator position)
{
    m_data->erase(position);
}

ListContainerBase::ConstReverseIterator ListContainerBase::crbegin() const
{
    if (m_data->isEmpty())
        return crend();

    size_t id = m_data->lastInnerListId();
    return ConstReverseIterator(m_data.get(), id, m_data->innerListById(id)->lastElement(), 0);
}

ListContainerBase::ConstReverseIterator ListContainerBase::crend() const
{
    return ConstReverseIterator(m_data.get(), static_cast<size_t>(-1), nullptr, size());
}

ListContainerBase::ReverseIterator ListContainerBase::rbegin()
{
    if (m_data->isEmpty())
        return rend();

    size_t id = m_data->lastInnerListId();
    return ReverseIterator(m_data.get(), id, m_data->innerListById(id)->lastElement(), 0);
}

ListContainerBase::ReverseIterator ListContainerBase::rend()
{
    return ReverseIterator(m_data.get(), static_cast<size_t>(-1), nullptr, size());
}

ListContainerBase::ConstIterator ListContainerBase::cbegin() const
{
    if (m_data->isEmpty())
        return cend();

    size_t id = m_data->firstInnerListId();
    return ConstIterator(m_data.get(), id, m_data->innerListById(id)->begin(), 0);
}

ListContainerBase::ConstIterator ListContainerBase::cend() const
{
    if (m_data->isEmpty())
        return ConstIterator(m_data.get(), 0, nullptr, size());

    size_t id = m_data->listCount();
    return ConstIterator(m_data.get(), id, nullptr, size());
}

ListContainerBase::Iterator ListContainerBase::begin()
{
    if (m_data->isEmpty())
        return end();

    size_t id = m_data->firstInnerListId();
    return Iterator(m_data.get(), id, m_data->innerListById(id)->begin(), 0);
}

ListContainerBase::Iterator ListContainerBase::end()
{
    if (m_data->isEmpty())
        return Iterator(m_data.get(), 0, nullptr, size());

    size_t id = m_data->listCount();
    return Iterator(m_data.get(), id, nullptr, size());
}

ListContainerBase::ConstIterator ListContainerBase::iteratorAt(size_t index) const
{
    ASSERT(index < size());
    size_t originalIndex = index;
    size_t listIndex;
    for (listIndex = 0; listIndex < m_data->listCount(); ++listIndex) {
        size_t currentSize = m_data->innerListById(listIndex)->size;
        if (index < currentSize)
            break;
        index -= currentSize;
    }
    return ConstIterator(m_data.get(), listIndex, m_data->innerListById(listIndex)->elementAt(index), originalIndex);
}

ListContainerBase::Iterator ListContainerBase::iteratorAt(size_t index)
{
    ASSERT(index < size());
    size_t originalIndex = index;
    size_t listIndex;
    for (listIndex = 0; listIndex < m_data->listCount(); ++listIndex) {
        size_t currentSize = m_data->innerListById(listIndex)->size;
        if (index < currentSize)
            break;
        index -= currentSize;
    }
    return Iterator(m_data.get(), listIndex, m_data->innerListById(listIndex)->elementAt(index), originalIndex);
}

void* ListContainerBase::allocate(size_t sizeOfActualElementInBytes)
{
    ASSERT(sizeOfActualElementInBytes <= m_data->elementSize());
    return m_data->allocate();
}

size_t ListContainerBase::size() const
{
    return m_data->size();
}

bool ListContainerBase::empty() const
{
    return m_data->isEmpty();
}

size_t ListContainerBase::maxSizeForDerivedClass() const
{
    return m_data->elementSize();
}

size_t ListContainerBase::getCapacityInBytes() const
{
    return m_data->capacity() * m_data->elementSize();
}

void ListContainerBase::clear()
{
    m_data->clear();
}

size_t ListContainerBase::availableSizeWithoutAnotherAllocationForTesting() const
{
    return m_data->numAvailableElementsInLastList();
}

// ListContainerBase::Iterator
/////////////////////////////////////////////////
ListContainerBase::Iterator::Iterator(ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter, size_t index)
    : PositionInListContainerCharAllocator(container, vectorIndex, itemIter), m_index(index)
{
}

ListContainerBase::Iterator::~Iterator()
{
}

size_t ListContainerBase::Iterator::index() const
{
    return m_index;
}

// ListContainerBase::ConstIterator
/////////////////////////////////////////////////
ListContainerBase::ConstIterator::ConstIterator(const ListContainerBase::Iterator& other)
    : PositionInListContainerCharAllocator(other), m_index(other.index())
{
}

ListContainerBase::ConstIterator::ConstIterator(ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter, size_t index)
    : PositionInListContainerCharAllocator(container, vectorIndex, itemIter), m_index(index)
{
}

ListContainerBase::ConstIterator::~ConstIterator()
{
}

size_t ListContainerBase::ConstIterator::index() const
{
    return m_index;
}

// ListContainerBase::ReverseIterator
/////////////////////////////////////////////////
ListContainerBase::ReverseIterator::ReverseIterator(ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter, size_t index)
    : PositionInListContainerCharAllocator(container, vectorIndex, itemIter), m_index(index)
{
}

ListContainerBase::ReverseIterator::~ReverseIterator()
{
}

size_t ListContainerBase::ReverseIterator::index() const
{
    return m_index;
}

// ListContainerBase::ConstReverseIterator
/////////////////////////////////////////////////
ListContainerBase::ConstReverseIterator::ConstReverseIterator(const ListContainerBase::ReverseIterator& other)
    : PositionInListContainerCharAllocator(other), m_index(other.index())
{
}

ListContainerBase::ConstReverseIterator::ConstReverseIterator(ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter, size_t index)
    : PositionInListContainerCharAllocator(container, vectorIndex, itemIter), m_index(index)
{
}

ListContainerBase::ConstReverseIterator::~ConstReverseIterator()
{
}

size_t ListContainerBase::ConstReverseIterator::index() const
{
    return m_index;
}

} // namespace blink
