// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/list_container.h"

#include <algorithm>
#include <vector>

#include "cc/base/scoped_ptr_vector.h"
#include "third_party/WebKit/Source/platform/graphics/ListContainer.h"

namespace {
const size_t kDefaultNumElementTypesToReserve = 32;
} // namespace

namespace cc {

// ListContainerCharAllocator
////////////////////////////////////////////////////
// This class deals only with char* and void*. It does allocation and passing
// out raw pointers, as well as memory deallocation when being destroyed.
class ListContainerBase::ListContainerCharAllocator {
public:
    // ListContainerCharAllocator::InnerList
    /////////////////////////////////////////////
    // This class holds the raw memory chunk, as well as information about its
    // size and availability.
    struct InnerList {
        scoped_ptr<char[]> data;
        // The number of elements in total the memory can hold. The difference
        // between capacity and size is the how many more elements this list can
        // hold.
        size_t capacity;
        // The number of elements have been put into this list.
        size_t size;
        // The size of each element is in bytes. This is used to move from between
        // elements' memory locations.
        size_t step;

        InnerList()
            : capacity(0)
            , size(0)
            , step(0)
        {
        }

        void Erase(char* position)
        {
            // Confident that destructor is called by caller of this function. Since
            // ListContainerCharAllocator does not handle construction after
            // allocation, it doesn't handle desctrution before deallocation.
            DCHECK_LE(position, LastElement());
            DCHECK_GE(position, Begin());
            char* start = position + step;
            std::copy(start, End(), position);

            --size;
            // Decrease capacity to avoid creating not full not last InnerList.
            --capacity;
        }

        bool IsEmpty() const { return !size; }
        bool IsFull() { return capacity == size; }
        size_t NumElementsAvailable() const { return capacity - size; }

        void* AddElement()
        {
            DCHECK_LT(size, capacity);
            ++size;
            return LastElement();
        }

        void RemoveLast()
        {
            DCHECK(!IsEmpty());
            --size;
        }

        char* Begin() const { return data.get(); }
        char* End() const { return data.get() + size * step; }
        char* LastElement() const { return data.get() + (size - 1) * step; }
        char* ElementAt(size_t index) const { return data.get() + index * step; }

    private:
        DISALLOW_COPY_AND_ASSIGN(InnerList);
    };

    explicit ListContainerCharAllocator(size_t element_size)
        : element_size_(element_size)
        , size_(0)
        , last_list_index_(0)
        , last_list_(NULL)
    {
        AllocateNewList(kDefaultNumElementTypesToReserve);
        last_list_ = storage_[last_list_index_];
    }

    ListContainerCharAllocator(size_t element_size, size_t element_count)
        : element_size_(element_size)
        , size_(0)
        , last_list_index_(0)
        , last_list_(NULL)
    {
        AllocateNewList(element_count > 0 ? element_count
                                          : kDefaultNumElementTypesToReserve);
        last_list_ = storage_[last_list_index_];
    }

    ~ListContainerCharAllocator() { }

    void* Allocate()
    {
        if (last_list_->IsFull()) {
            // Only allocate a new list if there isn't a spare one still there from
            // previous usage.
            if (last_list_index_ + 1 >= storage_.size())
                AllocateNewList(last_list_->capacity * 2);

            ++last_list_index_;
            last_list_ = storage_[last_list_index_];
        }

        ++size_;
        return last_list_->AddElement();
    }

    size_t element_size() const { return element_size_; }
    size_t list_count() const { return storage_.size(); }
    size_t size() const { return size_; }
    bool IsEmpty() const { return size() == 0; }

    size_t Capacity() const
    {
        size_t capacity_sum = 0;
        for (const auto& inner_list : storage_)
            capacity_sum += inner_list->capacity;
        return capacity_sum;
    }

    void Clear()
    {
        // Remove all except for the first InnerList.
        DCHECK(!storage_.empty());
        storage_.erase(storage_.begin() + 1, storage_.end());
        last_list_index_ = 0;
        last_list_ = storage_[0];
        last_list_->size = 0;
        size_ = 0;
    }

    void RemoveLast()
    {
        DCHECK(!IsEmpty());
        last_list_->RemoveLast();
        if (last_list_->IsEmpty() && last_list_index_ > 0) {
            --last_list_index_;
            last_list_ = storage_[last_list_index_];

            // If there are now two empty inner lists, free one of them.
            if (last_list_index_ + 2 < storage_.size())
                storage_.pop_back();
        }
        --size_;
    }

    void Erase(PositionInListContainerCharAllocator position)
    {
        DCHECK_EQ(this, position.ptr_to_container);
        storage_[position.vector_index]->Erase(position.item_iterator);
        // TODO(weiliangc): Free the InnerList if it is empty.
        --size_;
    }

    InnerList* InnerListById(size_t id) const
    {
        DCHECK_LT(id, storage_.size());
        return storage_[id];
    }

    size_t FirstInnerListId() const
    {
        // |size_| > 0 means that at least one vector in |storage_| will be
        // non-empty.
        DCHECK_GT(size_, 0u);
        size_t id = 0;
        while (storage_[id]->size == 0)
            ++id;
        return id;
    }

    size_t LastInnerListId() const
    {
        // |size_| > 0 means that at least one vector in |storage_| will be
        // non-empty.
        DCHECK_GT(size_, 0u);
        size_t id = storage_.size() - 1;
        while (storage_[id]->size == 0)
            --id;
        return id;
    }

    size_t NumAvailableElementsInLastList() const
    {
        return last_list_->NumElementsAvailable();
    }

private:
    void AllocateNewList(size_t list_size)
    {
        scoped_ptr<InnerList> new_list(new InnerList);
        new_list->capacity = list_size;
        new_list->size = 0;
        new_list->step = element_size_;
        new_list->data.reset(new char[list_size * element_size_]);
        storage_.push_back(new_list.Pass());
    }

    ScopedPtrVector<InnerList> storage_;
    const size_t element_size_;

    // The number of elements in the list.
    size_t size_;

    // The index of the last list to have had elements added to it, or the only
    // list if the container has not had elements added since being cleared.
    size_t last_list_index_;

    // This is equivalent to |storage_[last_list_index_]|.
    InnerList* last_list_;

    DISALLOW_COPY_AND_ASSIGN(ListContainerCharAllocator);
};

// PositionInListContainerCharAllocator
//////////////////////////////////////////////////////
ListContainerBase::PositionInListContainerCharAllocator::
    PositionInListContainerCharAllocator(
        const ListContainerBase::PositionInListContainerCharAllocator& other)
    : ptr_to_container(other.ptr_to_container)
    , vector_index(other.vector_index)
    , item_iterator(other.item_iterator)
{
}

ListContainerBase::PositionInListContainerCharAllocator::
    PositionInListContainerCharAllocator(
        ListContainerBase::ListContainerCharAllocator* container,
        size_t vector_ind,
        char* item_iter)
    : ptr_to_container(container)
    , vector_index(vector_ind)
    , item_iterator(item_iter)
{
}

bool ListContainerBase::PositionInListContainerCharAllocator::operator==(
    const ListContainerBase::PositionInListContainerCharAllocator& other)
    const
{
    DCHECK_EQ(ptr_to_container, other.ptr_to_container);
    return vector_index == other.vector_index && item_iterator == other.item_iterator;
}

bool ListContainerBase::PositionInListContainerCharAllocator::operator!=(
    const ListContainerBase::PositionInListContainerCharAllocator& other)
    const
{
    return !(*this == other);
}

ListContainerBase::PositionInListContainerCharAllocator
ListContainerBase::PositionInListContainerCharAllocator::Increment()
{
    ListContainerCharAllocator::InnerList* list = ptr_to_container->InnerListById(vector_index);
    if (item_iterator == list->LastElement()) {
        ++vector_index;
        while (vector_index < ptr_to_container->list_count()) {
            if (ptr_to_container->InnerListById(vector_index)->size != 0)
                break;
            ++vector_index;
        }
        if (vector_index < ptr_to_container->list_count())
            item_iterator = ptr_to_container->InnerListById(vector_index)->Begin();
        else
            item_iterator = NULL;
    } else {
        item_iterator += list->step;
    }
    return *this;
}

ListContainerBase::PositionInListContainerCharAllocator
ListContainerBase::PositionInListContainerCharAllocator::ReverseIncrement()
{
    ListContainerCharAllocator::InnerList* list = ptr_to_container->InnerListById(vector_index);
    if (item_iterator == list->Begin()) {
        --vector_index;
        // Since |vector_index| is unsigned, we compare < list_count() instead of
        // comparing >= 0, as the variable will wrap around when it goes out of
        // range (below 0).
        while (vector_index < ptr_to_container->list_count()) {
            if (ptr_to_container->InnerListById(vector_index)->size != 0)
                break;
            --vector_index;
        }
        if (vector_index < ptr_to_container->list_count()) {
            item_iterator = ptr_to_container->InnerListById(vector_index)->LastElement();
        } else {
            item_iterator = NULL;
        }
    } else {
        item_iterator -= list->step;
    }
    return *this;
}

// ListContainerBase
////////////////////////////////////////////
ListContainerBase::ListContainerBase(size_t max_size_for_derived_class)
    : data_(new ListContainerCharAllocator(max_size_for_derived_class))
{
}

ListContainerBase::ListContainerBase(size_t max_size_for_derived_class,
    size_t num_of_elements_to_reserve_for)
    : data_(new ListContainerCharAllocator(max_size_for_derived_class,
        num_of_elements_to_reserve_for))
{
}

ListContainerBase::~ListContainerBase()
{
}

void ListContainerBase::RemoveLast()
{
    data_->RemoveLast();
}

void ListContainerBase::EraseAndInvalidateAllPointers(
    ListContainerBase::Iterator position)
{
    data_->Erase(position);
}

ListContainerBase::ConstReverseIterator ListContainerBase::crbegin() const
{
    if (data_->IsEmpty())
        return crend();

    size_t id = data_->LastInnerListId();
    return ConstReverseIterator(data_.get(), id,
        data_->InnerListById(id)->LastElement(), 0);
}

ListContainerBase::ConstReverseIterator ListContainerBase::crend() const
{
    return ConstReverseIterator(data_.get(), static_cast<size_t>(-1), NULL,
        size());
}

ListContainerBase::ReverseIterator ListContainerBase::rbegin()
{
    if (data_->IsEmpty())
        return rend();

    size_t id = data_->LastInnerListId();
    return ReverseIterator(data_.get(), id,
        data_->InnerListById(id)->LastElement(), 0);
}

ListContainerBase::ReverseIterator ListContainerBase::rend()
{
    return ReverseIterator(data_.get(), static_cast<size_t>(-1), NULL, size());
}

ListContainerBase::ConstIterator ListContainerBase::cbegin() const
{
    if (data_->IsEmpty())
        return cend();

    size_t id = data_->FirstInnerListId();
    return ConstIterator(data_.get(), id, data_->InnerListById(id)->Begin(), 0);
}

ListContainerBase::ConstIterator ListContainerBase::cend() const
{
    if (data_->IsEmpty())
        return ConstIterator(data_.get(), 0, NULL, size());

    size_t id = data_->list_count();
    return ConstIterator(data_.get(), id, NULL, size());
}

ListContainerBase::Iterator ListContainerBase::begin()
{
    if (data_->IsEmpty())
        return end();

    size_t id = data_->FirstInnerListId();
    return Iterator(data_.get(), id, data_->InnerListById(id)->Begin(), 0);
}

ListContainerBase::Iterator ListContainerBase::end()
{
    if (data_->IsEmpty())
        return Iterator(data_.get(), 0, NULL, size());

    size_t id = data_->list_count();
    return Iterator(data_.get(), id, NULL, size());
}

ListContainerBase::ConstIterator ListContainerBase::IteratorAt(
    size_t index) const
{
    DCHECK_LT(index, size());
    size_t original_index = index;
    size_t list_index;
    for (list_index = 0; list_index < data_->list_count(); ++list_index) {
        size_t current_size = data_->InnerListById(list_index)->size;
        if (index < current_size)
            break;
        index -= current_size;
    }
    return ConstIterator(data_.get(), list_index,
        data_->InnerListById(list_index)->ElementAt(index),
        original_index);
}

ListContainerBase::Iterator ListContainerBase::IteratorAt(size_t index)
{
    DCHECK_LT(index, size());
    size_t original_index = index;
    size_t list_index;
    for (list_index = 0; list_index < data_->list_count(); ++list_index) {
        size_t current_size = data_->InnerListById(list_index)->size;
        if (index < current_size)
            break;
        index -= current_size;
    }
    return Iterator(data_.get(), list_index,
        data_->InnerListById(list_index)->ElementAt(index),
        original_index);
}

void* ListContainerBase::Allocate(size_t size_of_actual_element_in_bytes)
{
    DCHECK_LE(size_of_actual_element_in_bytes, data_->element_size());
    return data_->Allocate();
}

size_t ListContainerBase::size() const
{
    return data_->size();
}

bool ListContainerBase::empty() const
{
    return data_->IsEmpty();
}

size_t ListContainerBase::MaxSizeForDerivedClass() const
{
    return data_->element_size();
}

size_t ListContainerBase::GetCapacityInBytes() const
{
    return data_->Capacity() * data_->element_size();
}

void ListContainerBase::clear()
{
    data_->Clear();
}

size_t ListContainerBase::AvailableSizeWithoutAnotherAllocationForTesting()
    const
{
    return data_->NumAvailableElementsInLastList();
}

// ListContainerBase::Iterator
/////////////////////////////////////////////////
ListContainerBase::Iterator::Iterator(ListContainerCharAllocator* container,
    size_t vector_ind,
    char* item_iter,
    size_t index)
    : PositionInListContainerCharAllocator(container, vector_ind, item_iter)
    , index_(index)
{
}

ListContainerBase::Iterator::~Iterator()
{
}

size_t ListContainerBase::Iterator::index() const
{
    return index_;
}

// ListContainerBase::ConstIterator
/////////////////////////////////////////////////
ListContainerBase::ConstIterator::ConstIterator(
    const ListContainerBase::Iterator& other)
    : PositionInListContainerCharAllocator(other)
    , index_(other.index())
{
}

ListContainerBase::ConstIterator::ConstIterator(
    ListContainerCharAllocator* container,
    size_t vector_ind,
    char* item_iter,
    size_t index)
    : PositionInListContainerCharAllocator(container, vector_ind, item_iter)
    , index_(index)
{
}

ListContainerBase::ConstIterator::~ConstIterator()
{
}

size_t ListContainerBase::ConstIterator::index() const
{
    return index_;
}

// ListContainerBase::ReverseIterator
/////////////////////////////////////////////////
ListContainerBase::ReverseIterator::ReverseIterator(
    ListContainerCharAllocator* container,
    size_t vector_ind,
    char* item_iter,
    size_t index)
    : PositionInListContainerCharAllocator(container, vector_ind, item_iter)
    , index_(index)
{
}

ListContainerBase::ReverseIterator::~ReverseIterator()
{
}

size_t ListContainerBase::ReverseIterator::index() const
{
    return index_;
}

// ListContainerBase::ConstReverseIterator
/////////////////////////////////////////////////
ListContainerBase::ConstReverseIterator::ConstReverseIterator(
    const ListContainerBase::ReverseIterator& other)
    : PositionInListContainerCharAllocator(other)
    , index_(other.index())
{
}

ListContainerBase::ConstReverseIterator::ConstReverseIterator(
    ListContainerCharAllocator* container,
    size_t vector_ind,
    char* item_iter,
    size_t index)
    : PositionInListContainerCharAllocator(container, vector_ind, item_iter)
    , index_(index)
{
}

ListContainerBase::ConstReverseIterator::~ConstReverseIterator()
{
}

size_t ListContainerBase::ConstReverseIterator::index() const
{
    return index_;
}

} // namespace cc
