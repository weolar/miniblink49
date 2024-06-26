// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_LIST_CONTAINER_HELPER_H_
#define CC_BASE_LIST_CONTAINER_HELPER_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"

namespace cc {

// Helper class for ListContainer non-templated logic. All methods are private,
// and only exposed to friend classes.
// For usage, see comments in ListContainer (list_container.h).
class CC_EXPORT ListContainerHelper final {
private:
    template <typename T>
    friend class ListContainer;

    template <typename T>
    friend class RandomAccessListContainer;

    explicit ListContainerHelper(size_t max_size_for_derived_class);
    ListContainerHelper(size_t max_size_for_derived_class,
        size_t num_of_elements_to_reserve_for);
    ~ListContainerHelper();

    // This class deals only with char* and void*. It does allocation and passing
    // out raw pointers, as well as memory deallocation when being destroyed.
    class CharAllocator;

    // This class points to a certain position inside memory of
    // CharAllocator. It is a base class for ListContainer iterators.
    struct CC_EXPORT PositionInCharAllocator {
        CharAllocator* ptr_to_container;
        size_t vector_index;
        char* item_iterator;

        PositionInCharAllocator(const PositionInCharAllocator& other);

        PositionInCharAllocator(CharAllocator* container,
            size_t vector_ind,
            char* item_iter);

        bool operator==(const PositionInCharAllocator& other) const;
        bool operator!=(const PositionInCharAllocator& other) const;

        PositionInCharAllocator Increment();
        PositionInCharAllocator ReverseIncrement();
    };

    // Iterator classes that can be used to access data.
    /////////////////////////////////////////////////////////////////
    class CC_EXPORT Iterator : public PositionInCharAllocator {
        // This class is only defined to forward iterate through
        // CharAllocator.
    public:
        Iterator(CharAllocator* container,
            size_t vector_ind,
            char* item_iter,
            size_t index);
        ~Iterator();

        size_t index() const;

    protected:
        // This is used to track how many increment has happened since begin(). It
        // is used to avoid double increment at places an index reference is
        // needed. For iterator this means begin() corresponds to index 0 and end()
        // corresponds to index |size|.
        size_t index_;
    };

    class CC_EXPORT ConstIterator : public PositionInCharAllocator {
        // This class is only defined to forward iterate through
        // CharAllocator.
    public:
        ConstIterator(CharAllocator* container,
            size_t vector_ind,
            char* item_iter,
            size_t index);
        ConstIterator(const Iterator& other); // NOLINT
        ~ConstIterator();

        size_t index() const;

    protected:
        // This is used to track how many increment has happened since begin(). It
        // is used to avoid double increment at places an index reference is
        // needed. For iterator this means begin() corresponds to index 0 and end()
        // corresponds to index |size|.
        size_t index_;
    };

    class CC_EXPORT ReverseIterator : public PositionInCharAllocator {
        // This class is only defined to reverse iterate through
        // CharAllocator.
    public:
        ReverseIterator(CharAllocator* container,
            size_t vector_ind,
            char* item_iter,
            size_t index);
        ~ReverseIterator();

        size_t index() const;

    protected:
        // This is used to track how many increment has happened since rbegin(). It
        // is used to avoid double increment at places an index reference is
        // needed. For reverse iterator this means rbegin() corresponds to index 0
        // and rend() corresponds to index |size|.
        size_t index_;
    };

    class CC_EXPORT ConstReverseIterator : public PositionInCharAllocator {
        // This class is only defined to reverse iterate through
        // CharAllocator.
    public:
        ConstReverseIterator(CharAllocator* container,
            size_t vector_ind,
            char* item_iter,
            size_t index);
        ConstReverseIterator(const ReverseIterator& other); // NOLINT
        ~ConstReverseIterator();

        size_t index() const;

    protected:
        // This is used to track how many increment has happened since rbegin(). It
        // is used to avoid double increment at places an index reference is
        // needed. For reverse iterator this means rbegin() corresponds to index 0
        // and rend() corresponds to index |size|.
        size_t index_;
    };

    // Unlike the ListContainer methods, these do not invoke element destructors.
    void RemoveLast();
    void EraseAndInvalidateAllPointers(Iterator* position);
    void InsertBeforeAndInvalidateAllPointers(Iterator* position,
        size_t number_of_elements);

    ConstReverseIterator crbegin() const;
    ConstReverseIterator crend() const;
    ReverseIterator rbegin();
    ReverseIterator rend();
    ConstIterator cbegin() const;
    ConstIterator cend() const;
    Iterator begin();
    Iterator end();

    Iterator IteratorAt(size_t index);
    ConstIterator IteratorAt(size_t index) const;

    size_t size() const;
    bool empty() const;

    size_t MaxSizeForDerivedClass() const;

    size_t GetCapacityInBytes() const;

    // Unlike the ListContainer method, this one does not invoke element
    // destructors.
    void clear();

    size_t AvailableSizeWithoutAnotherAllocationForTesting() const;

    // Hands out memory location for an element at the end of data structure.
    void* Allocate(size_t size_of_actual_element_in_bytes);

    scoped_ptr<CharAllocator> data_;

    DISALLOW_COPY_AND_ASSIGN(ListContainerHelper);
};

} // namespace cc

#endif // CC_BASE_LIST_CONTAINER_HELPER_H_
