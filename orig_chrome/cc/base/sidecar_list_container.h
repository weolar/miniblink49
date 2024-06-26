// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_SIDECAR_LIST_CONTAINER_H_
#define CC_BASE_SIDECAR_LIST_CONTAINER_H_

#include "base/logging.h"
#include "cc/base/list_container.h"

namespace cc {

// This is a container, based on ListContainer, which allocates space in a
// contiguous block for objects subclassing BaseElementType, as well as an
// additional "sidecar" of opaque type.
//
// It takes a pointer to a function for tearing down sidecar objects, which must
// free any resources held by it as its memory will be deallocated by the
// container afterwards. When an element is constructed, callers are expected to
// immediately construct the sidecar as well (such that the sidecar destroyer
// will run safely and successfully).
//
// TODO(jbroman): It would be nice to be clear about the memory alignment
// constraints here, but that probably requires closer inspection of
// ListContainer first.
template <class BaseElementType>
class SidecarListContainer {
public:
    using SidecarDestroyer = void (*)(void* sidecar);
    using Iterator = typename ListContainer<BaseElementType>::Iterator;
    using ConstIterator = typename ListContainer<BaseElementType>::ConstIterator;
    using ReverseIterator =
        typename ListContainer<BaseElementType>::ReverseIterator;
    using ConstReverseIterator =
        typename ListContainer<BaseElementType>::ConstReverseIterator;

    explicit SidecarListContainer(size_t max_size_for_derived_class,
        size_t max_size_for_sidecar,
        size_t num_of_elements_to_reserve_for,
        SidecarDestroyer destroyer)
        : list_(max_size_for_derived_class + max_size_for_sidecar,
            num_of_elements_to_reserve_for)
        , destroyer_(destroyer)
        , sidecar_offset_(max_size_for_derived_class)
    {
    }
    ~SidecarListContainer() { DestroyAllSidecars(); }

    // Forward most of the reading logic to ListContainer.
    bool empty() const { return list_.empty(); }
    size_t size() const { return list_.size(); }
    size_t GetCapacityInBytes() const { return list_.GetCapacityInBytes(); }
    ConstIterator begin() const { return list_.begin(); }
    ConstIterator end() const { return list_.end(); }

    template <typename DerivedElementType>
    DerivedElementType* AllocateAndConstruct()
    {
        return list_.template AllocateAndConstruct<DerivedElementType>();
    }
    template <typename DerivedElementType>
    DerivedElementType* AllocateAndCopyFrom(const DerivedElementType* source)
    {
        return list_.template AllocateAndCopyFrom<DerivedElementType>(source);
    }

    void clear()
    {
        DestroyAllSidecars();
        list_.clear();
    }

    void RemoveLast()
    {
        destroyer_(GetSidecar(*list_.rbegin()));
        list_.RemoveLast();
    }

    // This permits a client to exchange a pointer to an element to a pointer to
    // its corresponding sidecar.
    void* GetSidecar(BaseElementType* element)
    {
        DCHECK_GT(sidecar_offset_, 0u);
        return reinterpret_cast<char*>(element) + sidecar_offset_;
    }
    const void* GetSidecar(const BaseElementType* element)
    {
        DCHECK_GT(sidecar_offset_, 0u);
        return reinterpret_cast<const char*>(element) + sidecar_offset_;
    }

private:
    void DestroyAllSidecars()
    {
        for (auto* element : list_)
            destroyer_(GetSidecar(element));
    }

    ListContainer<BaseElementType> list_;
    SidecarDestroyer destroyer_;
    size_t sidecar_offset_;
};

} // namespace cc

#endif // CC_BASE_SIDECAR_LIST_CONTAINER_H_
