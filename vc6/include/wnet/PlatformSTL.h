// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PlatformSTL_h
#define PlatformSTL_h

#include <memory>
#include <type_traits>
#include <algorithmvc6.h>

namespace std {

template <typename T>
struct OwnedPtrDeleter {
    static void deletePtr(T* ptr)
    {
        static_assert(sizeof(T) > 0, "type must be complete");
        delete ptr;
    }
};

template <typename T>
struct OwnedPtrDeleter<T[]> {
    static void deletePtr(T* ptr)
    {
        static_assert(sizeof(T) > 0, "type must be complete");
        delete[] ptr;
    }
};

template <class T, int n>
struct OwnedPtrDeleter<T[n]> {
    static_assert(sizeof(T) < 0, "do not use array with size as type");
};

template <typename T> class unique_ptr {
public:
    typedef typename remove_extent<T>::type ValueType;
    typedef ValueType* PtrType;

    unique_ptr() : m_ptr(nullptr) {}
    unique_ptr(std::nullptr_t) : m_ptr(nullptr) {}
    unique_ptr(unique_ptr&&);
    template <typename U, typename = typename enable_if<is_convertible<U*, T*>::value>::type> unique_ptr(unique_ptr<U>&&);

    ~unique_ptr()
    {
        OwnedPtrDeleter<T>::deletePtr(m_ptr);
        m_ptr = nullptr;
    }

    PtrType get() const { return m_ptr; }

    void reset(PtrType = nullptr);
    PtrType release();

    ValueType& operator*() const
    {
#ifdef DEBUG
        if (!m_ptr)
            DebugBreak();
#endif
        return *m_ptr;
    }
    PtrType operator->() const
    {
#ifdef DEBUG
        if (!m_ptr)
            DebugBreak();
#endif
        return m_ptr;
    }

    ValueType& operator[](std::ptrdiff_t i) const;

    bool operator!() const { return !m_ptr; }
    explicit operator bool() const { return m_ptr; }

    unique_ptr& operator=(std::nullptr_t) { reset(); return *this; }

    bool operator!=(std::nullptr_t) { return !m_ptr; }

    unique_ptr& operator=(unique_ptr&&);
    template <typename U> unique_ptr& operator=(unique_ptr<U>&&);

    void swap(unique_ptr& o) { std::swap(m_ptr, o.m_ptr); }

    static T* hashTableDeletedValue() { return reinterpret_cast<T*>(-1); }

    explicit unique_ptr(PtrType ptr) : m_ptr(ptr) {}

private:

    // We should never have two unique_ptrs for the same underlying object
    // (otherwise we'll get double-destruction), so these equality operators
    // should never be needed.
    template <typename U> bool operator==(const unique_ptr<U>&) const
    {
        static_assert(!sizeof(U*), "unique_ptrs should never be equal");
        return false;
    }
    template <typename U> bool operator!=(const unique_ptr<U>&) const
    {
        static_assert(!sizeof(U*), "unique_ptrs should never be equal");
        return false;
    }

    PtrType m_ptr;
};


template <typename T> inline void unique_ptr<T>::reset(PtrType ptr)
{
    PtrType p = m_ptr;
    m_ptr = ptr;
    OwnedPtrDeleter<T>::deletePtr(p);
}

template <typename T> inline typename unique_ptr<T>::PtrType unique_ptr<T>::release()
{
    PtrType ptr = m_ptr;
    m_ptr = nullptr;
    return ptr;
}

template <typename T> inline typename unique_ptr<T>::ValueType& unique_ptr<T>::operator[](std::ptrdiff_t i) const
{
    static_assert(is_array<T>::value, "elements access is possible for arrays only");
//     DCHECK(m_ptr);
//     DCHECK_GE(i, 0);
#ifdef DEBUG
    if (!m_ptr || i < 0)
        DebugBreak();
#endif
    return m_ptr[i];
}

template <typename T> inline unique_ptr<T>::unique_ptr(unique_ptr<T>&& o)
    : m_ptr(o.release())
{
}

template <typename T>
template <typename U, typename> inline unique_ptr<T>::unique_ptr(unique_ptr<U>&& o)
    : m_ptr(o.release())
{
    static_assert(!is_array<T>::value, "pointers to array must never be converted");
}

template <typename T> inline unique_ptr<T>& unique_ptr<T>::operator=(unique_ptr<T>&& o)
{
    PtrType ptr = m_ptr;
    m_ptr = o.release();
    //DCHECK(!ptr || m_ptr != ptr);
#ifdef DEBUG
    if (ptr && m_ptr == ptr)
        DebugBreak();
#endif
    OwnedPtrDeleter<T>::deletePtr(ptr);

    return *this;
}

template <typename T>
template <typename U> inline unique_ptr<T>& unique_ptr<T>::operator=(unique_ptr<U>&& o)
{
    static_assert(!is_array<T>::value, "pointers to array must never be converted");
    PtrType ptr = m_ptr;
    m_ptr = o.release();
    //DCHECK(!ptr || m_ptr != ptr);
#ifdef DEBUG
    if (ptr && m_ptr == ptr)
        DebugBreak();
#endif
    OwnedPtrDeleter<T>::deletePtr(ptr);

    return *this;
}

template <typename T> inline void swap(unique_ptr<T>& a, unique_ptr<T>& b)
{
    a.swap(b);
}

template <typename T, typename U> inline bool operator==(const unique_ptr<T>& a, U* b)
{
    return a.get() == b;
}

template <typename T, typename U> inline bool operator==(T* a, const unique_ptr<U>& b)
{
    return a == b.get();
}

template <typename T, typename U> inline bool operator!=(const unique_ptr<T>& a, U* b)
{
    return a.get() != b;
}

template <typename T, typename U> inline bool operator!=(T* a, const unique_ptr<U>& b)
{
    return a != b.get();
}

template <typename T> inline bool operator!=(const unique_ptr<T>& a, std::nullptr_t)
{
    return a.get();
}

template <typename T> inline typename unique_ptr<T>::PtrType getPtr(const unique_ptr<T>& p)
{
    return p.get();
}

template <typename T>
unique_ptr<T> move(unique_ptr<T>& ptr)
{
    return unique_ptr<T>(ptr.release());
}

}

template <typename T>
std::unique_ptr<T> wrapUnique(T* ptr)
{
    return std::unique_ptr<T>(ptr);
}

#endif // PlatformSTL_h
