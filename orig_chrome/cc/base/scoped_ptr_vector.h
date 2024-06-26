// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_SCOPED_PTR_VECTOR_H_
#define CC_BASE_SCOPED_PTR_VECTOR_H_

#include <algorithm>
#include <vector>

#include "base/basictypes.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/stl_util.h"

namespace cc {

// This type acts like a vector<scoped_ptr> based on top of std::vector. The
// ScopedPtrVector has ownership of all elements in the vector.
template <typename T>
class ScopedPtrVector {
public:
    typedef typename std::vector<T*>::const_iterator const_iterator;
    typedef typename std::vector<T*>::reverse_iterator reverse_iterator;
    typedef typename std::vector<T*>::const_reverse_iterator
        const_reverse_iterator;

#if defined(OS_ANDROID) || USING_VC6RT == 1
    // On Android the iterator is not a class, so we can't block assignment.
    typedef typename std::vector<T*>::iterator iterator;
#else
    // Ban setting values on the iterator directly. New pointers must be passed
    // to methods on the ScopedPtrVector class to appear in the vector.
    class iterator : public std::vector<T*>::iterator {
    public:
        iterator(const typename std::vector<T*>::iterator& other) // NOLINT
            : std::vector<T*>::iterator(other)
        {
        }
        T* const& operator*() { return std::vector<T*>::iterator::operator*(); }
    };
#endif

    ScopedPtrVector()
    {
    }

    ~ScopedPtrVector() { clear(); }

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

    scoped_ptr<T> take(iterator position)
    {
        if (position == end())
            return nullptr;
        DCHECK(position < end());

        typename std::vector<T*>::iterator writable_position = position;
        scoped_ptr<T> ret(*writable_position);
        *writable_position = nullptr;
        return ret.Pass();
    }

    scoped_ptr<T> take_back()
    {
        DCHECK(!empty());
        if (empty())
            return nullptr;
        return take(end() - 1);
    }

    void erase(iterator position)
    {
        if (position == end())
            return;
        typename std::vector<T*>::iterator writable_position = position;
        delete *writable_position;
        data_.erase(position);
    }

    void erase(iterator first, iterator last)
    {
        DCHECK(first <= last);
        for (iterator it = first; it != last; ++it) {
            DCHECK(it < end());

            typename std::vector<T*>::iterator writable_it = it;
            delete *writable_it;
        }
        data_.erase(first, last);
    }

    void reserve(size_t size)
    {
        data_.reserve(size);
    }

    void clear()
    {
        STLDeleteElements(&data_);
    }

    void push_back(scoped_ptr<T> item)
    {
        data_.push_back(item.release());
    }

    void pop_back()
    {
        delete data_.back();
        data_.pop_back();
    }

    void insert(iterator position, scoped_ptr<T> item)
    {
        DCHECK(position <= end());
        data_.insert(position, item.release());
    }

    void insert_and_take(iterator position, ScopedPtrVector<T>* other)
    {
        std::vector<T*> tmp_data;
        for (ScopedPtrVector<T>::iterator it = other->begin(); it != other->end();
             ++it) {
            tmp_data.push_back(other->take(it).release());
        }
        data_.insert(position, tmp_data.begin(), tmp_data.end());
    }

    template <typename Predicate>
    iterator partition(Predicate predicate)
    {
        typename std::vector<T*>::iterator first = begin();
        typename std::vector<T*>::iterator last = end();
        return static_cast<iterator>(std::partition(first, last, predicate));
    }

    void swap(ScopedPtrVector<T>& other)
    {
        data_.swap(other.data_);
    }

    void swap(iterator a, iterator b)
    {
        DCHECK(a < end());
        DCHECK(b < end());
        if (a == end() || b == end() || a == b)
            return;
        typename std::vector<T*>::iterator writable_a = a;
        typename std::vector<T*>::iterator writable_b = b;
        std::swap(*writable_a, *writable_b);
    }

    // This acts like std::remove_if but with one key difference. The values to be
    // removed to will each appear exactly once at or after the returned iterator,
    // so that erase(foo.remove_if(P), foo.end()) will not leak or double-free the
    // pointers in the vector.
    template <typename Predicate>
    iterator remove_if(Predicate predicate)
    {
        typename std::vector<T*>::iterator it = std::find_if(data_.begin(), data_.end(), predicate);
        typename std::vector<T*>::iterator end = data_.end();
        if (it == end)
            return it;
        typename std::vector<T*>::iterator result = it;
        ++it;
        for (; it != end; ++it) {
            if (!static_cast<bool>(predicate(*it))) {
                // Swap here instead of just assign to |result| so that all the
                // pointers are preserved to be deleted afterward.
                std::swap(*result, *it);
                ++result;
            }
        }
        return result;
    }

    template <class Compare>
    inline void sort(Compare comp)
    {
        std::sort(data_.begin(), data_.end(), comp);
    }

    template <class Compare>
    inline void make_heap(Compare comp)
    {
        std::make_heap(data_.begin(), data_.end(), comp);
    }

    template <class Compare>
    inline void push_heap(Compare comp)
    {
        std::push_heap(data_.begin(), data_.end(), comp);
    }

    template <class Compare>
    inline void pop_heap(Compare comp)
    {
        std::pop_heap(data_.begin(), data_.end(), comp);
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
    std::vector<T*> data_;

    DISALLOW_COPY_AND_ASSIGN(ScopedPtrVector);
};

} // namespace cc

#endif // CC_BASE_SCOPED_PTR_VECTOR_H_
