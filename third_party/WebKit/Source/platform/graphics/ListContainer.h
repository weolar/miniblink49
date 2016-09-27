// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ListContainer_h
#define ListContainer_h

#include "platform/PlatformExport.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/Utility.h"
#include "wtf/Vector.h"

#include <iterator>

namespace blink {

// ListContainer is a copy of cc/base/list_container.h and is only here
// temporarily pending the Blink merge.
//
// Use of this class is discouraged.
//
// TODO(pdr): Remove ListContainer in favor of cc/base/list_container.h.

// ListContainer is a container type that handles allocating contiguous memory
// for new elements and traversing through elements with either iterator or
// reverse iterator. Since this container hands out raw pointers of its
// elements, it is very important that this container never reallocate its
// memory so those raw pointer will continue to be valid.  This class is used to
// contain SharedQuadState or DrawQuad. Since the size of each DrawQuad varies,
// to hold DrawQuads, the allocations size of each element in this class is
// LargestDrawQuadSize while BaseElementType is DrawQuad.

// Base class for non-templated logic. All methods are protected, and only
// exposed by ListContainer<BaseElementType>.
// For usage, see comments in ListContainer.
class PLATFORM_EXPORT ListContainerBase {
    WTF_MAKE_NONCOPYABLE(ListContainerBase);
protected:
    explicit ListContainerBase(size_t maxSizeForDerivedClass);
    ListContainerBase(size_t maxSizeForDerivedClass, size_t numElementsToReserveFor);
    ~ListContainerBase();

    // This class deals only with char* and void*. It does allocation and passing
    // out raw pointers, as well as memory deallocation when being destroyed.
    class ListContainerCharAllocator;

    // This class points to a certain position inside memory of
    // ListContainerCharAllocator. It is a base class for ListContainer iterators.
    struct PLATFORM_EXPORT PositionInListContainerCharAllocator {
        ListContainerCharAllocator* ptrToContainer;
        size_t vectorIndex;
        char* itemIterator;

        PositionInListContainerCharAllocator(const PositionInListContainerCharAllocator& other);
        PositionInListContainerCharAllocator(ListContainerCharAllocator*, size_t vectorIndex, char* itemIter);

        bool operator==(const PositionInListContainerCharAllocator& other) const;
        bool operator!=(const PositionInListContainerCharAllocator& other) const;

        PositionInListContainerCharAllocator increment();
        PositionInListContainerCharAllocator reverseIncrement();
    };

    // Iterator classes that can be used to access data.
    /////////////////////////////////////////////////////////////////
    class PLATFORM_EXPORT Iterator : public PositionInListContainerCharAllocator {
    // This class is only defined to forward iterate through ListContainerCharAllocator.
    public:
        Iterator(ListContainerCharAllocator*, size_t vectorIndex, char* itemIter, size_t index);
        ~Iterator();

        size_t index() const;

    protected:
        // This is used to track how many increment has happened since begin(). It
        // is used to avoid double increment at places an index reference is
        // needed. For iterator this means begin() corresponds to index 0 and end()
        // corresponds to index |size|.
        size_t m_index;
    };

    class PLATFORM_EXPORT ConstIterator : public PositionInListContainerCharAllocator {
    // This class is only defined to forward iterate through ListContainerCharAllocator.
    public:
        ConstIterator(ListContainerCharAllocator*, size_t vectorIndex, char* itemIter, size_t index);
        ConstIterator(const Iterator& other);
        ~ConstIterator();

        size_t index() const;

    protected:
        // This is used to track how many increment has happened since begin(). It
        // is used to avoid double increment at places an index reference is
        // needed. For iterator this means begin() corresponds to index 0 and end()
        // corresponds to index |size|.
        size_t m_index;
    };

    class PLATFORM_EXPORT ReverseIterator : public PositionInListContainerCharAllocator {
    // This class is only defined to reverse iterate through ListContainerCharAllocator.
    public:
        ReverseIterator(ListContainerCharAllocator*, size_t vectorIndex, char* itemIter, size_t index);
        ~ReverseIterator();

        size_t index() const;

    protected:
        // This is used to track how many increment has happened since rbegin(). It
        // is used to avoid double increment at places an index reference is
        // needed. For reverse iterator this means rbegin() corresponds to index 0
        // and rend() corresponds to index |size|.
        size_t m_index;
    };

    class PLATFORM_EXPORT ConstReverseIterator : public PositionInListContainerCharAllocator {
    // This class is only defined to reverse iterate through ListContainerCharAllocator.
    public:
        ConstReverseIterator(ListContainerCharAllocator*, size_t vectorIndex, char* itemIter, size_t index);
        ConstReverseIterator(const ReverseIterator& other);
        ~ConstReverseIterator();

        size_t index() const;

    protected:
        // This is used to track how many increment has happened since rbegin(). It
        // is used to avoid double increment at places an index reference is
        // needed. For reverse iterator this means rbegin() corresponds to index 0
        // and rend() corresponds to index |size|.
        size_t m_index;
    };

    // Unlike the ListContainer methods, these do not invoke element destructors.
    void removeLast();
    void eraseAndInvalidateAllPointers(Iterator position);

    ConstReverseIterator crbegin() const;
    ConstReverseIterator crend() const;
    ReverseIterator rbegin();
    ReverseIterator rend();
    ConstIterator cbegin() const;
    ConstIterator cend() const;
    Iterator begin();
    Iterator end();

    Iterator iteratorAt(size_t index);
    ConstIterator iteratorAt(size_t index) const;

    size_t size() const;
    bool empty() const;

    size_t maxSizeForDerivedClass() const;
    size_t getCapacityInBytes() const;

    // Unlike the ListContainer method, this one does not invoke element destructors.
    void clear();

    size_t availableSizeWithoutAnotherAllocationForTesting() const;

    // Hands out memory location for an element at the end of data structure.
    void* allocate(size_t sizeOfActualElementInBytes);

    OwnPtr<ListContainerCharAllocator> m_data;
};

template <class BaseElementType>
class ListContainer : public ListContainerBase {
public:
    // BaseElementType is the type of raw pointers this class hands out; however,
    // its derived classes might require different memory sizes.
    // maxSizeForDerivedClass the largest memory size required for all the
    // derived classes to use for allocation.
    explicit ListContainer(size_t maxSizeForDerivedClass) : ListContainerBase(maxSizeForDerivedClass) {}

    // This constructor omits input variable for maxSizeForDerivedClass. This
    // is used when there is no derived classes from BaseElementType we need to
    // worry about, and allocation size is just sizeof(BaseElementType).
    ListContainer() : ListContainerBase(sizeof(BaseElementType)) {}

    // This constructor reserves the requested memory up front so only single
    // allocation is needed. When numElementsToReserveFor is zero, use the
    // default size.
    ListContainer(size_t maxSizeForDerivedClass, size_t numElementsToReserveFor)
        : ListContainerBase(maxSizeForDerivedClass, numElementsToReserveFor) {}

    ~ListContainer()
    {
        for (Iterator i = begin(); i != end(); ++i)
            i->~BaseElementType();
    }

    class Iterator;
    class ConstIterator;
    class ReverseIterator;
    class ConstReverseIterator;

    // Removes the last element of the list and makes its space available for
    // allocation.
    void removeLast()
    {
        ASSERT(!empty());
        back()->~BaseElementType();
        ListContainerBase::removeLast();
    }

    // When called, all raw pointers that have been handed out are no longer
    // valid. Use with caution.
    // This function does not deallocate memory.
    void eraseAndInvalidateAllPointers(Iterator position)
    {
        BaseElementType* item = *position;
        item->~BaseElementType();
        ListContainerBase::eraseAndInvalidateAllPointers(position);
    }

    ConstReverseIterator crbegin() const { return ConstReverseIterator(ListContainerBase::crbegin()); }
    ConstReverseIterator crend() const { return ConstReverseIterator(ListContainerBase::crend()); }
    ConstReverseIterator rbegin() const { return crbegin(); }
    ConstReverseIterator rend() const { return crend(); }
    ReverseIterator rbegin() { return ReverseIterator(ListContainerBase::rbegin()); }
    ReverseIterator rend() { return ReverseIterator(ListContainerBase::rend()); }
    ConstIterator cbegin() const { return ConstIterator(ListContainerBase::cbegin()); }
    ConstIterator cend() const { return ConstIterator(ListContainerBase::cend()); }
    ConstIterator begin() const { return cbegin(); }
    ConstIterator end() const { return cend(); }
    Iterator begin() { return Iterator(ListContainerBase::begin()); }
    Iterator end() { return Iterator(ListContainerBase::end()); }

    // TODO(weiliangc): front(), back() and elementAt() function should return
    // reference, consistent with container-of-object.
    BaseElementType* front() { return *begin(); }
    BaseElementType* back() { return *rbegin(); }
    const BaseElementType* front() const { return *begin(); }
    const BaseElementType* back() const { return *rbegin(); }

    BaseElementType* elementAt(size_t index)
    {
        return *iteratorAt(index);
    }

    const BaseElementType* elementAt(size_t index) const
    {
        return *iteratorAt(index);
    }

    Iterator iteratorAt(size_t index)
    {
        return Iterator(ListContainerBase::iteratorAt(index));
    }

    ConstIterator iteratorAt(size_t index) const
    {
        return ConstIterator(ListContainerBase::iteratorAt(index));
    }

    // Take in derived element type and construct it at location generated by Allocate().
    template <typename DerivedElementType, typename... Args>
    DerivedElementType* allocateAndConstruct(Args&&... args)
    {
        return new (allocate(sizeof(DerivedElementType))) DerivedElementType(WTF::forward<Args>(args)...);
    }

    // Take in derived element type and copy construct it at location generated by
    // Allocate().
    template <typename DerivedElementType>
    DerivedElementType* allocateAndCopyFrom(const DerivedElementType* source)
    {
        return new (allocate(sizeof(DerivedElementType))) DerivedElementType(*source);
    }

    // Construct a new element on top of an existing one.
    template <typename DerivedElementType>
    DerivedElementType* replaceExistingElement(Iterator at)
    {
        at->~BaseElementType();
        return new (*at) DerivedElementType();
    }

    template <typename DerivedElementType>
    void swap(ListContainer<DerivedElementType>& other)
    {
        m_data.swap(other.m_data);
    }

    // Appends a new item without copying. The original item will not be
    // destructed and will be replaced with a new DerivedElementType. The
    // DerivedElementType does not have to match the moved type as a full block
    // of memory will be moved (up to maxSizeForDerivedClass()). A pointer
    // to the moved element is returned.
    template <typename DerivedElementType>
    DerivedElementType* appendByMoving(DerivedElementType* item)
    {
        size_t maxSize = maxSizeForDerivedClass();
        void* newItem = allocate(maxSize);
        memcpy(newItem, static_cast<void*>(item), maxSize);
        // Construct a new element in-place so it can be destructed safely.
        new (item) DerivedElementType;
        return static_cast<DerivedElementType*>(newItem);
    }

    using ListContainerBase::size;
    using ListContainerBase::empty;
    using ListContainerBase::getCapacityInBytes;

    void clear()
    {
        for (Iterator i = begin(); i != end(); ++i)
            i->~BaseElementType();
        ListContainerBase::clear();
    }

    using ListContainerBase::availableSizeWithoutAnotherAllocationForTesting;

    // Iterator classes that can be used to access data.
    /////////////////////////////////////////////////////////////////
    class Iterator : public ListContainerBase::Iterator {
    // This class is only defined to forward iterate through
    // ListContainerCharAllocator.
    public:
        Iterator(ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter, size_t index)
            : ListContainerBase::Iterator(container, vectorIndex, itemIter, index) {}

        BaseElementType* operator->() const { return reinterpret_cast<BaseElementType*>(itemIterator); }
        BaseElementType* operator*() const { return reinterpret_cast<BaseElementType*>(itemIterator); }
        Iterator operator++(int unused_post_increment)
        {
            Iterator tmp = *this;
            operator++();
            return tmp;
        }
        Iterator& operator++()
        {
            increment();
            ++m_index;
            return *this;
        }

    private:
        explicit Iterator(const ListContainerBase::Iterator& baseIterator)
            : ListContainerBase::Iterator(baseIterator) {}
        friend Iterator ListContainer<BaseElementType>::begin();
        friend Iterator ListContainer<BaseElementType>::end();
        friend BaseElementType* ListContainer<BaseElementType>::elementAt(size_t index);
        friend Iterator ListContainer<BaseElementType>::iteratorAt(size_t index);
    };

    class ConstIterator : public ListContainerBase::ConstIterator {
    // This class is only defined to forward iterate through
    // ListContainerCharAllocator.
    public:
        ConstIterator(ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter, size_t index)
            : ListContainerBase::ConstIterator(container, vectorIndex, itemIter, index) {}
        ConstIterator(const Iterator& other)
            : ListContainerBase::ConstIterator(other) {}

        const BaseElementType* operator->() const
        {
            return reinterpret_cast<const BaseElementType*>(itemIterator);
        }
        const BaseElementType* operator*() const
        {
            return reinterpret_cast<const BaseElementType*>(itemIterator);
        }
        ConstIterator operator++(int)
        {
            ConstIterator tmp = *this;
            operator++();
            return tmp;
        }
        ConstIterator& operator++()
        {
            increment();
            ++m_index;
            return *this;
        }

    private:
        explicit ConstIterator(const ListContainerBase::ConstIterator& baseIterator)
            : ListContainerBase::ConstIterator(baseIterator) {}
        friend ConstIterator ListContainer<BaseElementType>::cbegin() const;
        friend ConstIterator ListContainer<BaseElementType>::cend() const;
        friend const BaseElementType* ListContainer<BaseElementType>::elementAt(size_t index) const;
        friend ConstIterator ListContainer<BaseElementType>::iteratorAt(size_t index) const;
    };

    class ReverseIterator : public ListContainerBase::ReverseIterator {
    // This class is only defined to reverse iterate through
    // ListContainerCharAllocator.
    public:
        ReverseIterator(ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter, size_t index)
            : ListContainerBase::ReverseIterator(container, vectorIndex, itemIter, index) {}

        BaseElementType* operator->() const
        {
            return reinterpret_cast<BaseElementType*>(itemIterator);
        }
        BaseElementType* operator*() const
        {
            return reinterpret_cast<BaseElementType*>(itemIterator);
        }
        ReverseIterator operator++(int)
        {
            ReverseIterator tmp = *this;
            operator++();
            return tmp;
        }
        ReverseIterator& operator++()
        {
            reverseIncrement();
            ++m_index;
            return *this;
        }

    private:
        explicit ReverseIterator(ListContainerBase::ReverseIterator baseIterator)
            : ListContainerBase::ReverseIterator(baseIterator) {}
        friend ReverseIterator ListContainer<BaseElementType>::rbegin();
        friend ReverseIterator ListContainer<BaseElementType>::rend();
    };

    class ConstReverseIterator : public ListContainerBase::ConstReverseIterator {
    // This class is only defined to reverse iterate through
    // ListContainerCharAllocator.
    public:
        ConstReverseIterator(ListContainerCharAllocator* container, size_t vectorIndex, char* itemIter, size_t index)
            : ListContainerBase::ConstReverseIterator(container, vectorIndex, itemIter, index) {}
        ConstReverseIterator(const ReverseIterator& other)
            : ListContainerBase::ConstReverseIterator(other) {}

        const BaseElementType* operator->() const
        {
            return reinterpret_cast<const BaseElementType*>(itemIterator);
        }
        const BaseElementType* operator*() const
        {
            return reinterpret_cast<const BaseElementType*>(itemIterator);
        }
        ConstReverseIterator operator++(int)
        {
            ConstReverseIterator tmp = *this;
            operator++();
            return tmp;
        }
        ConstReverseIterator& operator++()
        {
            reverseIncrement();
            ++m_index;
            return *this;
        }

    private:
        explicit ConstReverseIterator(ListContainerBase::ConstReverseIterator baseIterator)
            : ListContainerBase::ConstReverseIterator(baseIterator) {}
        friend ConstReverseIterator ListContainer<BaseElementType>::crbegin() const;
        friend ConstReverseIterator ListContainer<BaseElementType>::crend() const;
    };
};

} // namespace blink

#endif // ListContainer_h
