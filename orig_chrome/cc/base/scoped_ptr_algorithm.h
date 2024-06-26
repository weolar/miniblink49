// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_SCOPED_PTR_ALGORITHM_H_
#define CC_BASE_SCOPED_PTR_ALGORITHM_H_

namespace cc {

// ScopedContainers need to implement a swap() method since they do not allow
// assignment to their iterators.
template <class ForwardIterator, class Predicate, class ScopedContainer>
ForwardIterator remove_if(
    ScopedContainer* container,
    ForwardIterator first,
    ForwardIterator last,
    Predicate predicate)
{
    ForwardIterator result = first;
    for (; first != last; ++first) {
        if (!predicate(*first)) {
            container->swap(first, result);
            ++result;
        }
    }
    return result;
}

} // namespace cc

#endif // CC_BASE_SCOPED_PTR_ALGORITHM_H_
