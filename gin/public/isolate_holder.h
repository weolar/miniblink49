// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_PUBLIC_ISOLATE_HOLDER_H_
#define GIN_PUBLIC_ISOLATE_HOLDER_H_

#include "base/basictypes.h"
//#include "base/memory/scoped_ptr.h"
#include "gin/gin_export.h"
//#include "v8/include/v8.h"
#include "v8.h"

namespace gin {

class PerIsolateData;
class RunMicrotasksObserver;
class V8IsolateMemoryDumpProvider;

// To embed Gin, first initialize gin using IsolateHolder::Initialize and then
// create an instance of IsolateHolder to hold the v8::Isolate in which you
// will execute JavaScript. You might wish to subclass IsolateHolder if you
// want to tie more state to the lifetime of the isolate.
class GIN_EXPORT IsolateHolder {
 public:
  // Controls whether or not V8 should only accept strict mode scripts.
  enum ScriptMode {
    kNonStrictMode,
    kStrictMode
  };

  // Stores whether the client uses v8::Locker to access the isolate.
  enum AccessMode {
    kSingleThread,
    kUseLocker
  };

  IsolateHolder();
  explicit IsolateHolder(AccessMode access_mode);
  ~IsolateHolder();

  // Should be invoked once before creating IsolateHolder instances to
  // initialize V8 and Gin. In case V8_USE_EXTERNAL_STARTUP_DATA is
  // defined, V8's initial natives should be loaded (by calling
  // V8Initializer::LoadV8NativesFromFD or
  // V8Initializer::LoadV8Natives) before calling this method.  If the
  // snapshot file is available, it should also be loaded (by calling
  // V8Initializer::LoadV8SnapshotFromFD or
  // V8Initializer::LoadV8Snapshot) before calling this method.
  static void Initialize(ScriptMode mode,
                         v8::ArrayBuffer::Allocator* allocator);

  v8::Isolate* isolate() { return isolate_; }

  // The implementations of Object.observe() and Promise enqueue v8 Microtasks
  // that should be executed just before control is returned to the message
  // loop. This method adds a MessageLoop TaskObserver which runs any pending
  // Microtasks each time a Task is completed. This method should be called
  // once, when a MessageLoop is created and it should be called on the
  // MessageLoop's thread.
  void AddRunMicrotasksObserver();

  // This method should also only be called once, and on the MessageLoop's
  // thread.
  void RemoveRunMicrotasksObserver();

  // This method returns if v8::Locker is needed to access isolate.
  AccessMode access_mode() const { return access_mode_; }

  // This method returns V8IsolateMemoryDumpProvider of this isolate, used for
  // testing.
#ifdef MINIBLINK_NOT_IMPLEMENTED
  V8IsolateMemoryDumpProvider* isolate_memory_dump_provider_for_testing()
      const {
    return isolate_memory_dump_provider_.get();
  }
#endif // MINIBLINK_NOT_IMPLEMENTED

  struct MemoryHead {
      size_t magicNum;
      size_t size;
  };
  static const size_t magicNum0 = 0x1122dd44;
  static const size_t magicNum1 = 0x11227788;

  static MemoryHead* GetPointerHead(void* pointer);
  static size_t GetPointerMemSize(void* pointer);
  static void* GetHeadToMemBegin(MemoryHead* head);

  static v8::ArrayBuffer::Allocator* get_allocator();

 private:
  v8::Isolate* isolate_;
#ifdef MINIBLINK_NOT_IMPLEMENTED
  scoped_ptr<PerIsolateData> isolate_data_;
  scoped_ptr<RunMicrotasksObserver> task_observer_;
  scoped_ptr<V8IsolateMemoryDumpProvider> isolate_memory_dump_provider_;
#endif // MINIBLINK_NOT_IMPLEMENTED
  AccessMode access_mode_;

  DISALLOW_COPY_AND_ASSIGN(IsolateHolder);
};

}  // namespace gin

#endif  // GIN_PUBLIC_ISOLATE_HOLDER_H_
