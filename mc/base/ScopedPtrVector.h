// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_base_ScopedPtrVector_h
#define mc_base_ScopedPtrVector_h

#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include <algorithm>
#include <vector>

namespace mc {

// This type acts like a vector<scoped_ptr> based on top of std::vector. The
// ScopedPtrVector has ownership of all elements in the vector.
template <typename T>
class ScopedPtrVector {
public:
    typedef typename std::vector<T*>::const_iterator const_iterator;
    typedef typename std::vector<T*>::reverse_iterator reverse_iterator;
    typedef typename std::vector<T*>::const_reverse_iterator const_reverse_iterator;

#if defined(OS_ANDROID) || USING_VC6RT==1
    // On Android the iterator is not a class, so we can't block assignment.
    typedef typename std::vector<T*>::iterator iterator;
#else
    // Ban setting values on the iterator directly. New pointers must be passed
    // to methods on the ScopedPtrVector class to appear in the vector.
    class iterator : public std::vector<T*>::iterator {
    public:
        iterator(const typename std::vector<T*>::iterator& other) // NOLINT
            : std::vector<T*>::iterator(other) {}
        T* const& operator*() 
        {
            return std::vector<T*>::iterator::operator*();
        }
    };
#endif

    ScopedPtrVector() {}

    ~ScopedPtrVector() 
    {
        clear();
    }

    size_t size() const 
    {
        return m_data.size();
    }

    T* at(size_t index) const 
    {
        ASSERT(index < size());
        return m_data[index];
    }

    T* operator[](size_t index) const
    {
        return at(index);
    }

    T* front() const
    {
        ASSERT(!empty());
        return at(0);
    }

    T* back() const
    {
        ASSERT(!empty());
        return at(size() - 1);
    }

    bool empty() const
    {
        return m_data.empty();
    }

    WTF::PassOwnPtr<T> take(iterator position) 
    {
        if (position == end())
            return nullptr;
        ASSERT(position < end());

        typename std::vector<T*>::iterator writable_position = position;
        T* ret = *writable_position;
        *writable_position = nullptr;
        return adoptPtr(ret);
    }

    WTF::PassOwnPtr<T> take_back()
    {
        ASSERT(!empty());
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
        m_data.erase(position);
    }

    void erase(iterator first, iterator last) 
    {
        ASSERT(first <= last);
        for (iterator it = first; it != last; ++it) {
            ASSERT(it < end());

            typename std::vector<T*>::iterator writable_it = it;
            delete *writable_it;
        }
        m_data.erase(first, last);
    }

    void reserve(size_t size)
    {
        m_data.reserve(size);
    }

    void clear() 
    {
        for (size_t i = 0; i < m_data.size(); ++i) {
            T* it = m_data[i];
            delete it;
        }

        m_data.clear();
    }

    void push_back(T* item) 
    {
        m_data.push_back(item);
    }

    void pop_back()
    {
        delete m_data.back();
        m_data.pop_back();
    }

    void insert(iterator position, T* item) 
    {
        ASSERT(position <= end());
        m_data.insert(position, item);
    }

    void insert_and_take(iterator position, ScopedPtrVector<T>* other) 
    {
        std::vector<T*> tmp_data;
        for (ScopedPtrVector<T>::iterator it = other->begin(); it != other->end(); ++it) {
            tmp_data.push_back(other->take(it).release());
        }
        m_data.insert(position, tmp_data.begin(), tmp_data.end());
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
        m_data.swap(other.m_data);
    }

    void swap(iterator a, iterator b)
    {
        ASSERT(a < end());
        ASSERT(b < end());
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
        typename std::vector<T*>::iterator it =
            std::find_if(m_data.begin(), m_data.end(), predicate);
        typename std::vector<T*>::iterator end = m_data.end();
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

    template<class Compare>
    inline void sort(Compare comp) 
    {
        std::sort(m_data.begin(), m_data.end(), comp);
    }

    template <class Compare>
    inline void make_heap(Compare comp)
    {
        std::make_heap(m_data.begin(), m_data.end(), comp);
    }

    template <class Compare>
    inline void push_heap(Compare comp)
    {
        std::push_heap(m_data.begin(), m_data.end(), comp);
    }

    template <class Compare>
    inline void pop_heap(Compare comp) 
    {
        std::pop_heap(m_data.begin(), m_data.end(), comp);
    }

    iterator begin()
    {
        return static_cast<iterator>(m_data.begin());
    }
    const_iterator begin() const
    {
        return m_data.begin();
    }
    iterator end() 
    {
        return static_cast<iterator>(m_data.end());
    }
    const_iterator end() const 
    {
        return m_data.end();
    }

    reverse_iterator rbegin()
    {
        return m_data.rbegin();
    }
    const_reverse_iterator rbegin() const 
    {
        return m_data.rbegin();
    }
    reverse_iterator rend()
    {
        return m_data.rend();
    }
    const_reverse_iterator rend() const 
    {
        return m_data.rend();
    }

private:
    std::vector<T*> m_data;
};

}  // namespace mc

#endif  // mc_base_ScopedPtrVector_h
