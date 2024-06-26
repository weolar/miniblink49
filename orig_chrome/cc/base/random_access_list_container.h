// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_RANDOM_ACCESS_LIST_CONTAINER_H_
#define CC_BASE_RANDOM_ACCESS_LIST_CONTAINER_H_

#include <vector>

#include "base/logging.h"
#include "cc/base/list_container_helper.h"

namespace cc {

// RandomAccessListContainer is a container similar to ListContainer (see
// list_container.h), but it allows random access into its elements via
// operator[]. In order to have efficient support for random access, some
// functionality is not available for RandomAccessListContainers, such as
// insert/deletes in the middle of the list.
template <class BaseElementType>
class RandomAccessListContainer {
public:
    // BaseElementType is the type of raw pointers this class hands out; however,
    // its derived classes might require different memory sizes.
    // max_size_for_derived_class the largest memory size required for all the
    // derived classes to use for allocation.
    explicit RandomAccessListContainer(size_t max_size_for_derived_class)
        : helper_(max_size_for_derived_class)
    {
    }

    // This constructor reserves the requested memory up front so only a single
    // allocation is needed. When num_of_elements_to_reserve_for is zero, use the
    // default size.
    RandomAccessListContainer(size_t max_size_for_derived_class,
        size_t num_of_elements_to_reserve_for)
        : helper_(max_size_for_derived_class, num_of_elements_to_reserve_for)
    {
        items_.reserve(num_of_elements_to_reserve_for);
    }

    ~RandomAccessListContainer()
    {
        for (BaseElementType* item : items_)
            item->~BaseElementType();
    }

    void clear()
    {
        for (BaseElementType* item : items_)
            item->~BaseElementType();
        helper_.clear();
        items_.clear();
    }

    bool empty() const { return helper_.empty(); }
    size_t size() const { return helper_.size(); }
    size_t GetCapacityInBytes() const { return helper_.GetCapacityInBytes(); }

    template <typename DerivedElementType>
    DerivedElementType* AllocateAndConstruct()
    {
        auto* value = new (helper_.Allocate(sizeof(DerivedElementType))) DerivedElementType;
        items_.push_back(value);
        return value;
    }

    void RemoveLast()
    {
        items_.back()->~BaseElementType();
        items_.pop_back();
        helper_.RemoveLast();
    }

    const BaseElementType* operator[](size_t index) const
    {
        DCHECK_GE(index, 0u);
        DCHECK_LT(index, items_.size());
        return items_[index];
    }

    BaseElementType* operator[](size_t index)
    {
        DCHECK_GE(index, 0u);
        DCHECK_LT(index, items_.size());
        return items_[index];
    }

    // Note that although BaseElementType objects can change, the pointer itself
    // (in the vector) cannot. So this class only supports a const iterator.
    using ConstIterator = typename std::vector<BaseElementType*>::const_iterator;
    ConstIterator begin() const { return items_.begin(); }
    ConstIterator end() const { return items_.end(); }

private:
    ListContainerHelper helper_;
    std::vector<BaseElementType*> items_;
};

} // namespace cc

#endif // CC_BASE_RANDOM_ACCESS_LIST_CONTAINER_H_
