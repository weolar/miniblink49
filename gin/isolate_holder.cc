// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/public/isolate_holder.h"

#include <stdlib.h>
#include <string.h>

#if defined(OS_WIN)
#include "windows.h"
#endif

#include "base/logging.h"
#include "gin/debug_impl.h"
#include "gin/v8_initializer.h"
//#include "v8/src/base/sys-info.h"
#include "sys-info.h"

#ifdef MINIBLINK_NOT_IMPLEMENTED
#include "base/message_loop/message_loop.h"
#include "gin/function_template.h"
#include "gin/per_isolate_data.h"
#include "gin/run_microtasks_observer.h"
#include "gin/v8_isolate_memory_dump_provider.h"
#endif // MINIBLINK_NOT_IMPLEMENTED

// namespace v8 {
//     namespace base {
// 
//         class SysInfo final {
//         public:
//             // Returns the number of logical processors/core on the current machine.
//             static int NumberOfProcessors();
// 
//             // Returns the number of bytes of physical memory on the current machine.
//             static int64_t AmountOfPhysicalMemory();
// 
//             // Returns the number of bytes of virtual memory of this process. A return
//             // value of zero means that there is no limit on the available virtual memory.
//             static int64_t AmountOfVirtualMemory();
//         };
// 
//     }  // namespace base
// }  // namespace v8

namespace gin {

namespace {
v8::ArrayBuffer::Allocator* g_array_buffer_allocator = nullptr;
}  // namespace

IsolateHolder::IsolateHolder() : IsolateHolder(AccessMode::kSingleThread) {
}

IsolateHolder::IsolateHolder(AccessMode access_mode)
    : access_mode_(access_mode) {

  v8::ArrayBuffer::Allocator* allocator = g_array_buffer_allocator;
  CHECK(allocator); // << "You need to invoke gin::IsolateHolder::Initialize first";
  v8::Isolate::CreateParams params;
  params.entry_hook = DebugImpl::GetFunctionEntryHook();
  params.code_event_handler = DebugImpl::GetJitCodeEventHandler();
  params.constraints.ConfigureDefaults(v8::base::SysInfo::AmountOfPhysicalMemory(),
    v8::base::SysInfo::AmountOfVirtualMemory());
  params.array_buffer_allocator = allocator;
  isolate_ = v8::Isolate::New(params);
#ifdef MINIBLINK_NOT_IMPLEMENTED
  isolate_data_.reset(new PerIsolateData(isolate_, allocator));
  isolate_memory_dump_provider_.reset(new V8IsolateMemoryDumpProvider(this));
#endif // MINIBLINK_NOT_IMPLEMENTED
#if defined(OS_WIN)
  {
    void* code_range;
    size_t size;
    isolate_->GetCodeRange(&code_range, &size);
    Debug::CodeRangeCreatedCallback callback =
        DebugImpl::GetCodeRangeCreatedCallback();
    if (code_range && size && callback)
      callback(code_range, size);
  }
#endif
}

IsolateHolder::~IsolateHolder() {
#ifdef MINIBLINK_NOT_IMPLEMENTED
  if (task_observer_.get())
    base::MessageLoop::current()->RemoveTaskObserver(task_observer_.get());
#endif // MINIBLINK_NOT_IMPLEMENTED

#if defined(OS_WIN)
  {
    void* code_range;
    size_t size;
    isolate_->GetCodeRange(&code_range, &size);
    Debug::CodeRangeDeletedCallback callback = DebugImpl::GetCodeRangeDeletedCallback();
    if (code_range && callback)
      callback(code_range);
  }
#endif

#ifdef MINIBLINK_NOT_IMPLEMENTED
  isolate_memory_dump_provider_.reset();
  isolate_data_.reset();
#endif // MINIBLINK_NOT_IMPLEMENTED

  isolate_->Dispose();
  isolate_ = NULL;
}

// static
void IsolateHolder::Initialize(ScriptMode mode,
                               v8::ArrayBuffer::Allocator* allocator) {
  CHECK(allocator);
  V8Initializer::Initialize(mode);
  g_array_buffer_allocator = allocator;
}

IsolateHolder::MemoryHead* IsolateHolder::GetPointerHead(void* pointer) {
    return ((MemoryHead*)pointer) - 1;
}

size_t IsolateHolder::GetPointerMemSize(void* pointer) {
    return GetPointerHead(pointer)->size;
}

void* IsolateHolder::GetHeadToMemBegin(IsolateHolder::MemoryHead* head) {
    return head + 1;
}

v8::ArrayBuffer::Allocator* IsolateHolder::get_allocator() {
    return g_array_buffer_allocator;
}

void IsolateHolder::AddRunMicrotasksObserver() {
  DCHECK(!task_observer_.get());
#ifdef MINIBLINK_NOT_IMPLEMENTED
  task_observer_.reset(new RunMicrotasksObserver(isolate_));
  base::MessageLoop::current()->AddTaskObserver(task_observer_.get());
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void IsolateHolder::RemoveRunMicrotasksObserver() {
#ifdef MINIBLINK_NOT_IMPLEMENTED
  DCHECK(task_observer_.get());
  base::MessageLoop::current()->RemoveTaskObserver(task_observer_.get());
  task_observer_.reset();
#endif // MINIBLINK_NOT_IMPLEMENTED
}

}  // namespace gin
