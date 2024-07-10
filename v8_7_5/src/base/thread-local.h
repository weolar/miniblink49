// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// WARNING: Thread local storage is a bit tricky to get right.  Please make
// sure that this is really the proper solution for what you're trying to
// achieve.  Don't prematurely optimize, most likely you can just use a Lock.
//
// These classes implement a wrapper around the platform's TLS storage
// mechanism.  On construction, they will allocate a TLS slot, and free the
// TLS slot on destruction.  No memory management (creation or destruction) is
// handled.  This means for uses of ThreadLocalPointer, you must correctly
// manage the memory yourself, these classes will not destroy the pointer for
// you.  There are no at-thread-exit actions taken by these classes.
//
// ThreadLocalPointer<Type> wraps a Type*.  It performs no creation or
// destruction, so memory management must be handled elsewhere.  The first call
// to Get() on a thread will return NULL.  You can update the pointer with a
// call to Set().
//
// ThreadLocalBoolean wraps a bool.  It will default to false if it has never
// been set otherwise with Set().
//
// Thread Safety:  An instance of ThreadLocalStorage is completely thread safe
// once it has been created.  If you want to dynamically create an instance,
// you must of course properly deal with safety and race conditions.  This
// means a function-level static initializer is generally inappropiate.
//
// In Android, the system TLS is limited, the implementation is backed with
// ThreadLocalStorage.
//
// Example usage:
//   // My class is logically attached to a single thread.  We cache a pointer
//   // on the thread it was created on, so we can implement current().
//   MyClass::MyClass() {
//     DCHECK(Singleton<ThreadLocalPointer<MyClass> >::get()->Get() == NULL);
//     Singleton<ThreadLocalPointer<MyClass> >::get()->Set(this);
//   }
//
//   MyClass::~MyClass() {
//     DCHECK(Singleton<ThreadLocalPointer<MyClass> >::get()->Get() != NULL);
//     Singleton<ThreadLocalPointer<MyClass> >::get()->Set(NULL);
//   }
//
//   // Return the current MyClass associated with the calling thread, can be
//   // NULL if there isn't a MyClass associated.
//   MyClass* MyClass::current() {
//     return Singleton<ThreadLocalPointer<MyClass> >::get()->Get();
//   }

#ifndef BASE_THREAD_LOCAL_H_
#define BASE_THREAD_LOCAL_H_

#include <windows.h>

namespace v8 {
namespace base {

// Helper functions that abstract the cross-platform APIs.  Do not use directly.
struct ThreadLocalPlatform {
  typedef unsigned long SlotType;

  static void AllocateSlot(SlotType* slot);
  static void FreeSlot(SlotType slot);
  static void* GetValueFromSlot(SlotType slot);
  static void SetValueInSlot(SlotType slot, void* value);
};

template <typename Type>
class ThreadLocalPointer {
public:
  ThreadLocalPointer() : slot_() {
    ThreadLocalPlatform::AllocateSlot(&slot_);
    Set(nullptr);
  }

  ~ThreadLocalPointer() {
    ThreadLocalPlatform::FreeSlot(slot_);
  }

  Type* Get() {
    return static_cast<Type*>(ThreadLocalPlatform::GetValueFromSlot(slot_));
  }

  void Set(Type* ptr) {
    ThreadLocalPlatform::SetValueInSlot(slot_, const_cast<void*>(static_cast<const void*>(ptr)));
  }

private:
  typedef ThreadLocalPlatform::SlotType SlotType;

  SlotType slot_;
};

class ThreadLocalBoolean {
public:
  ThreadLocalBoolean() {}
  ~ThreadLocalBoolean() {}

  bool Get() {
    return tlp_.Get() != NULL;
  }

  void Set(bool val) {
    tlp_.Set(val ? this : NULL);
  }

private:
  ThreadLocalPointer<void> tlp_;
};

// static
inline void ThreadLocalPlatform::AllocateSlot(SlotType* slot) {
  *slot = TlsAlloc();
  //CHECK_NE(*slot, TLS_OUT_OF_INDEXES);
}

// static
inline void ThreadLocalPlatform::FreeSlot(SlotType slot) {
  if (!TlsFree(slot)) {
    //NOTREACHED() << "Failed to deallocate tls slot with TlsFree().";
    DebugBreak();
  }
}

// static
inline void* ThreadLocalPlatform::GetValueFromSlot(SlotType slot) {
  return ::TlsGetValue(slot);
}

// static
inline void ThreadLocalPlatform::SetValueInSlot(SlotType slot, void* value) {
  if (!::TlsSetValue(slot, value)) {
    //LOG(FATAL) << "Failed to TlsSetValue().";
    DebugBreak();
  }
}

} // namespace base
} // namespace v8

#endif // BASE_THREADING_THREAD_LOCAL_H_
