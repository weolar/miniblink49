// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_SCOPED_PTR_DEQUE_H_
#define CC_BASE_SCOPED_PTR_DEQUE_H_

#include <algorithm>
#include <deque>

#include "base/basictypes.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/stl_util.h"

namespace cc {

// This type acts like a deque<scoped_ptr> based on top of std::deque. The
// ScopedPtrDeque has ownership of all elements in the deque.
template <typename T>
class ScopedPtrDeque {
public:
    typedef typename std::deque<T*>::const_iterator const_iterator;
    typedef typename std::deque<T*>::reverse_iterator reverse_iterator;
    typedef typename std::deque<T*>::const_reverse_iterator
        const_reverse_iterator;

#if defined(OS_ANDROID)
    // On Android the iterator is not a class, so we can't block assignment.
    typedef typename std::deque<T*>::iterator iterator;
#else
    // Ban setting values on the iterator directly. New pointers must be passed
    // to methods on the ScopedPtrDeque class to appear in the deque.
    class iterator : public std::deque<T*>::iterator {
    public:
        explicit iterator(const typename std::deque<T*>::iterator& other)
            : std::deque<T*>::iterator(other)
        {
        }
        T* const& operator*() { return std::deque<T*>::iterator::operator*(); }
    };
#endif

    ScopedPtrDeque()
    {
    }

    ~ScopedPtrDeque() { clear(); }

    size_t size() const
    {
        return data_.size();
    }

    T* at(size_t index) const
    {
        DCHECK(index < size());
        return data_[index];
    }

    T* operator[](size_t index) const
    {
        return at(index);
    }

    T* front() const
    {
        DCHECK(!empty());
        return at(0);
    }

    T* back() const
    {
        DCHECK(!empty());
        return at(size() - 1);
    }

    bool empty() const
    {
        return data_.empty();
    }

    scoped_ptr<T> take_front()
    {
        scoped_ptr<T> ret(front());
        data_.pop_front();
        return ret.Pass();
    }

    scoped_ptr<T> take_back()
    {
        scoped_ptr<T> ret(back());
        data_.pop_back();
        return ret.Pass();
    }

    void clear()
    {
        STLDeleteElements(&data_);
    }

    void push_front(scoped_ptr<T> item)
    {
        data_.push_front(item.release());
    }

    void push_back(scoped_ptr<T> item)
    {
        data_.push_back(item.release());
    }

    void insert(iterator position, scoped_ptr<T> item)
    {
        DCHECK(position <= end());
        data_.insert(position, item.release());
    }

    scoped_ptr<T> take(iterator position)
    {
        DCHECK(position < end());
        scoped_ptr<T> ret(*position);
        data_.erase(position);
        return ret.Pass();
    }

    void swap(iterator a, iterator b)
    {
        DCHECK(a < end());
        DCHECK(b < end());
        if (a == end() || b == end() || a == b)
            return;
        typename std::deque<T*>::iterator writable_a = a;
        typename std::deque<T*>::iterator writable_b = b;
        std::swap(*writable_a, *writable_b);
    }

    iterator begin() { return static_cast<iterator>(data_.begin()); }
    const_iterator begin() const { return data_.begin(); }
    iterator end() { return static_cast<iterator>(data_.end()); }
    const_iterator end() const { return data_.end(); }

    reverse_iterator rbegin() { return data_.rbegin(); }
    const_reverse_iterator rbegin() const { return data_.rbegin(); }
    reverse_iterator rend() { return data_.rend(); }
    const_reverse_iterator rend() const { return data_.rend(); }

private:
    std::deque<T*> data_;

    DISALLOW_COPY_AND_ASSIGN(ScopedPtrDeque);
};

} // namespace cc

#endif // CC_BASE_SCOPED_PTR_DEQUE_H_
