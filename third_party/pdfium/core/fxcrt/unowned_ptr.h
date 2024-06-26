// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXCRT_UNOWNED_PTR_H_
#define CORE_FXCRT_UNOWNED_PTR_H_

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

// UnownedPtr is a smart pointer class that behaves very much like a
// standard C-style pointer. The advantages of using it over raw
// pointers are:
//
// 1. It documents the nature of the pointer with no need to add a comment
//    explaining that is it // Not owned. Additionally, an attempt to delete
//    an unowned ptr will fail to compile rather than silently succeeding,
//    since it is a class and not a raw pointer.
//
// 2. When built for a memory tool like ASAN, the class provides a destructor
//    which checks that the object being pointed to is still alive.
//
// Hence, when using UnownedPtr, no dangling pointers are ever permitted,
// even if they are not de-referenced after becoming dangling. The style of
// programming required is that the lifetime an object containing an
// UnownedPtr must be strictly less than the object to which it points.
//
// The same checks are also performed at assignment time to prove that the
// old value was not a dangling pointer, either.
//
// The array indexing operation [] is not supported on an unowned ptr,
// because an unowned ptr expresses a one to one relationship with some
// other heap object. Use pdfium::span<> for the cases where indexing
// into an unowned array is desired, which performs the same checks.

namespace pdfium {

template <typename T>
class span;

}  // namespace pdfium

namespace fxcrt {

template <class T>
class UnownedPtr {
 public:
  constexpr UnownedPtr() noexcept = default;
  constexpr UnownedPtr(const UnownedPtr& that) noexcept = default;

  template <typename U>
  explicit constexpr UnownedPtr(U* pObj) noexcept : m_pObj(pObj) {}

  // Deliberately implicit to allow returning nullptrs.
  // NOLINTNEXTLINE(runtime/explicit)
  constexpr UnownedPtr(std::nullptr_t ptr) noexcept {}

  ~UnownedPtr() { ProbeForLowSeverityLifetimeIssue(); }

  UnownedPtr& operator=(T* that) noexcept {
    ProbeForLowSeverityLifetimeIssue();
    m_pObj = that;
    return *this;
  }

  UnownedPtr& operator=(const UnownedPtr& that) noexcept {
    ProbeForLowSeverityLifetimeIssue();
    if (*this != that)
      m_pObj = that.Get();
    return *this;
  }

  bool operator==(const UnownedPtr& that) const { return Get() == that.Get(); }
  bool operator!=(const UnownedPtr& that) const { return !(*this == that); }
  bool operator<(const UnownedPtr& that) const {
    return std::less<T*>()(Get(), that.Get());
  }

  template <typename U>
  bool operator==(const U* that) const {
    return Get() == that;
  }

  template <typename U>
  bool operator!=(const U* that) const {
    return !(*this == that);
  }

  T* Get() const noexcept { return m_pObj; }

  T* Release() {
    ProbeForLowSeverityLifetimeIssue();
    T* pTemp = nullptr;
    std::swap(pTemp, m_pObj);
    return pTemp;
  }

  explicit operator bool() const { return !!m_pObj; }
  T& operator*() const { return *m_pObj; }
  T* operator->() const { return m_pObj; }

 private:
  friend class pdfium::span<T>;

  inline void ProbeForLowSeverityLifetimeIssue() {
#if defined(MEMORY_TOOL_REPLACES_ALLOCATOR)
    if (m_pObj)
      reinterpret_cast<const volatile uint8_t*>(m_pObj)[0];
#endif
  }

  inline void ReleaseBadPointer() {
#if defined(MEMORY_TOOL_REPLACES_ALLOCATOR)
    m_pObj = nullptr;
#endif
  }

  T* m_pObj = nullptr;
};

template <typename T, typename U>
inline bool operator==(const U* lhs, const UnownedPtr<T>& rhs) {
  return rhs == lhs;
}

template <typename T, typename U>
inline bool operator!=(const U* lhs, const UnownedPtr<T>& rhs) {
  return rhs != lhs;
}

}  // namespace fxcrt

using fxcrt::UnownedPtr;

#endif  // CORE_FXCRT_UNOWNED_PTR_H_
