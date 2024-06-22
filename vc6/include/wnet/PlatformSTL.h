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

template<class _Ty>
struct default_delete {	// default deleter for unique_ptr
    default_delete() = default;

    template<class _Ty2, class = typename enable_if<is_convertible<_Ty2 *, _Ty *>::value, void>::type>
        default_delete(const default_delete<_Ty2>&) // construct from another default_delete
    {
    }

    void operator()(_Ty *_Ptr) const
    {	// delete a pointer
        static_assert(0 < sizeof(_Ty), "can't delete an incomplete type");
        delete _Ptr;
    }
};

template<class _Ty>
struct default_delete<_Ty[]> { // default deleter for unique_ptr to array of unknown size
    default_delete() = default;

    template<class _Uty, class = typename enable_if<is_convertible<_Uty(*)[], _Ty(*)[]>::value, void>::type>
        default_delete(const default_delete<_Uty[]>&)
    {	// construct from another default_delete
    }

    template<class _Uty,
    class = typename enable_if<is_convertible<_Uty(*)[], _Ty(*)[]>::value,
        void>::type>
        void operator()(_Uty *_Ptr) const // delete a pointer
    {	
        static_assert(0 < sizeof(_Uty), "can't delete an incomplete type");
        delete[] _Ptr;
    }
};

template <class T, int n>
struct OwnedPtrDeleter<T[n]> {
    static_assert(sizeof(T) < 0, "do not use array with size as type");
};

template <typename T, typename TDeleteor = default_delete<T> > class unique_ptr {
public:
    typedef typename remove_extent<T>::type ValueType;
    typedef ValueType* PtrType;

    unique_ptr() : m_ptr(nullptr) {}
    unique_ptr(std::nullptr_t) : m_ptr(nullptr) {}
    unique_ptr(unique_ptr&&);
    template <typename U, typename = typename enable_if<is_convertible<U*, T*>::value>::type> unique_ptr(unique_ptr<U>&&);

    ~unique_ptr()
    {
        //OwnedPtrDeleter<T>::deletePtr(m_ptr);
        TDeleteor()(m_ptr);
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
    explicit operator bool() const { return !!m_ptr; }

    unique_ptr& operator=(std::nullptr_t) { reset(); return *this; }

    bool operator!=(std::nullptr_t) { return !!m_ptr; }

    unique_ptr& operator=(unique_ptr&&);
    //template <typename U, typename TDeleteor = default_delete<T>> unique_ptr& operator=(unique_ptr<U, TDeleteor>&&);

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

template <typename T, typename TDeleteor = default_delete<T>> inline void unique_ptr<T, TDeleteor>::reset(PtrType ptr)
{
    PtrType p = m_ptr;
    m_ptr = ptr;
    //OwnedPtrDeleter<T>::deletePtr(p);
    TDeleteor()(p);
}

template <typename T, typename TDeleteor = default_delete<T> > inline typename unique_ptr<T, TDeleteor>::PtrType unique_ptr<T, TDeleteor>::release()
{
    PtrType ptr = m_ptr;
    m_ptr = nullptr;
    return ptr;
}

template <typename T, typename TDeleteor = default_delete<T> > inline typename unique_ptr<T, TDeleteor>::ValueType& unique_ptr<T, TDeleteor>::operator[](std::ptrdiff_t i) const
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

template <typename T, typename TDeleteor = default_delete<T> > inline unique_ptr<T, TDeleteor>::unique_ptr(unique_ptr<T, TDeleteor>&& o)
    : m_ptr(o.release())
{
}

// template <typename T, typename TDeleteor = default_delete<T> >
// template <typename U, typename TDeleteor = default_delete<T> > 
// inline unique_ptr<T, TDeleteor>::unique_ptr(unique_ptr<U, TDeleteor>&& o)
//     : m_ptr(o.release())
// {
//     static_assert(!is_array<T>::value, "pointers to array must never be converted");
// }

template <typename T, typename TDeleteor = default_delete<T>> inline unique_ptr<T, TDeleteor>& unique_ptr<T, TDeleteor>::operator=(unique_ptr<T, TDeleteor>&& o)
{
    PtrType ptr = m_ptr;
    m_ptr = o.release();
    //DCHECK(!ptr || m_ptr != ptr);
#ifdef DEBUG
    if (ptr && m_ptr == ptr)
        DebugBreak();
#endif
    //OwnedPtrDeleter<T>::deletePtr(ptr);
    TDeleteor()(ptr);

    return *this;
}

// template <typename T, typename TDeleteor >
// template <typename U, typename TDeleteor > 
// inline unique_ptr<T, TDeleteor>& unique_ptr<T, TDeleteor>::operator=(unique_ptr<U, TDeleteor>&& o)
// {
//     static_assert(!is_array<T>::value, "pointers to array must never be converted");
//     PtrType ptr = m_ptr;
//     m_ptr = o.release();
//     //DCHECK(!ptr || m_ptr != ptr);
// #ifdef DEBUG
//     if (ptr && m_ptr == ptr)
//         DebugBreak();
// #endif
//     //OwnedPtrDeleter<T>::deletePtr(ptr);
//     TDeleteor()(ptr);
// 
//     return *this;
// }

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
