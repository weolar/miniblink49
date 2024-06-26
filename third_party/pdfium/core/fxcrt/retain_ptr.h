// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXCRT_RETAIN_PTR_H_
#define CORE_FXCRT_RETAIN_PTR_H_

#include <functional>
#include <memory>
#include <utility>

#include "core/fxcrt/fx_system.h"

namespace fxcrt {

// Used with std::unique_ptr to Release() objects that can't be deleted.
template <class T>
struct ReleaseDeleter {
  inline void operator()(T* ptr) const { ptr->Release(); }
};

// Analogous to base's scoped_refptr.
template <class T>
class RetainPtr {
 public:
  explicit RetainPtr(T* pObj) : m_pObj(pObj) {
    if (m_pObj)
      m_pObj->Retain();
  }

  RetainPtr() = default;
  RetainPtr(const RetainPtr& that) : RetainPtr(that.Get()) {}
  RetainPtr(RetainPtr&& that) noexcept { Swap(that); }

  // Deliberately implicit to allow returning nullptrs.
  // NOLINTNEXTLINE(runtime/explicit)
  RetainPtr(std::nullptr_t ptr) {}

  template <class U>
  RetainPtr(const RetainPtr<U>& that) : RetainPtr(that.Get()) {}

  template <class U>
  RetainPtr<U> As() const {
    return RetainPtr<U>(static_cast<U*>(Get()));
  }

  void Reset(T* obj = nullptr) {
    if (obj)
      obj->Retain();
    m_pObj.reset(obj);
  }

  T* Get() const { return m_pObj.get(); }
  void Swap(RetainPtr& that) { m_pObj.swap(that.m_pObj); }

  // Useful for passing notion of object ownership across a C API.
  T* Leak() { return m_pObj.release(); }
  void Unleak(T* ptr) { m_pObj.reset(ptr); }

  RetainPtr& operator=(const RetainPtr& that) {
    if (*this != that)
      Reset(that.Get());
    return *this;
  }

  RetainPtr& operator=(RetainPtr&& that) {
    m_pObj.reset(that.Leak());
    return *this;
  }

  bool operator==(const RetainPtr& that) const { return Get() == that.Get(); }
  bool operator!=(const RetainPtr& that) const { return !(*this == that); }

  bool operator<(const RetainPtr& that) const {
    return std::less<T*>()(Get(), that.Get());
  }

  explicit operator bool() const { return !!m_pObj; }
  T& operator*() const { return *m_pObj.get(); }
  T* operator->() const { return m_pObj.get(); }

 private:
  std::unique_ptr<T, ReleaseDeleter<T>> m_pObj;
};

// Trivial implementation - internal ref count with virtual destructor.
class Retainable {
 public:
  Retainable() = default;

  bool HasOneRef() const { return m_nRefCount == 1; }

 protected:
  virtual ~Retainable() = default;

 private:
  template <typename U>
  friend struct ReleaseDeleter;

  template <typename U>
  friend class RetainPtr;

  Retainable(const Retainable& that) = delete;
  Retainable& operator=(const Retainable& that) = delete;

  void Retain() { ++m_nRefCount; }
  void Release() {
    ASSERT(m_nRefCount > 0);
    if (--m_nRefCount == 0)
      delete this;
  }

  intptr_t m_nRefCount = 0;
};

}  // namespace fxcrt

using fxcrt::ReleaseDeleter;
using fxcrt::RetainPtr;
using fxcrt::Retainable;

namespace pdfium {

// Helper to make a RetainPtr along the lines of std::make_unique<>(),
// or pdfium::MakeUnique<>(). Arguments are forwarded to T's constructor.
// Classes managed by RetainPtr should have protected (or private)
// constructors, and should friend this function.
template <typename T, typename... Args>
RetainPtr<T> MakeRetain(Args&&... args) {
  return RetainPtr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace pdfium

#endif  // CORE_FXCRT_RETAIN_PTR_H_
