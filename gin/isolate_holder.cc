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
//#include "src/base/sys-info.h"
//#include "sys-info.h"
#include "v8.h"

namespace v8 {
namespace base {

class SysInfo final {
public:
    // Returns the number of logical processors/core on the current machine.
    static int V8CALL NumberOfProcessors();

    // Returns the number of bytes of physical memory on the current machine.
    static int64_t V8CALL AmountOfPhysicalMemory();

    // Returns the number of bytes of virtual memory of this process. A return
    // value of zero means that there is no limit on the available virtual memory.
    static int64_t V8CALL AmountOfVirtualMemory();
};

}  // namespace base
}  // namespace v8

// void readFile(const wchar_t* path, std::vector<char>* buffer)
// {
//     HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//     if (INVALID_HANDLE_VALUE == hFile) {
//         DebugBreak();
//         return;
//     }
// 
//     DWORD fileSizeHigh;
//     const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);
// 
//     DWORD numberOfBytesRead = 0;
//     buffer->resize(bufferSize);
//     BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
//     ::CloseHandle(hFile);
// }
// 
// //一个函数，实现在js中调用C++函数
// void testAlert(const v8::FunctionCallbackInfo<v8::Value>& args)
// {
//     //     v8::String::Utf8Value str(args[0]);
//     //     printf("day--：%s\n", *str);
//     v8::Local<v8::Value> str = args[0];
//     v8::Local<v8::String> info = str->ToString(args.GetIsolate());
// 
//     std::vector<char> buffer(1000);
//     info->WriteUtf8(args.GetIsolate(), buffer.data());
//     buffer.push_back('\n');
//     buffer.push_back('\0');
// 
//     std::string temp = buffer.data();
//     temp += "\n";
//     OutputDebugStringA(temp.c_str());
//     OutputDebugStringA("");
// }
// 
// int testQueenMain(v8::Isolate* isolate)
// {
//     std::vector<char> buffer;
//     readFile(L"G:\\test\\web_test\\zzz_test\\queen8.js", &buffer);
//     buffer.push_back('\0');
// 
//     // Initialize V8.
//     //     v8::V8::InitializeICUDefaultLocation(argv[0]);
//     //     v8::V8::InitializeExternalStartupData(argv[0]);
//     //     std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
//     //     v8::V8::InitializePlatform(platform.get());
//     //     v8::V8::Initialize();
// 
//     // Create a new Isolate and make it the current one.
//     //     v8::Isolate::CreateParams create_params;
//     //     create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
//     //     v8::Isolate* isolate = v8::Isolate::New(create_params);
// 
//     {
//         //v8::Isolate::Scope isolate_scope(isolate);
// 
//         // Create a stack-allocated handle scope.
//         v8::HandleScope handle_scope(isolate);
// 
//         v8::Local<v8::ObjectTemplate> global_templ = v8::ObjectTemplate::New(isolate);
//         global_templ->Set(v8::String::NewFromUtf8(isolate, "alert"), v8::FunctionTemplate::New(isolate, testAlert));
// 
//         // Create a new context.
//         v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global_templ);
//         // Enter the context for compiling and running the hello world script.
//         v8::Context::Scope context_scope(context);
//         // Create a string containing the JavaScript source code.
//         v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, buffer.data(), v8::NewStringType::kNormal).ToLocalChecked();
// 
//         // Compile the source code.
//         v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
//         // Run the script to get the result.
//         //v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
//         script->Run(context);
// 
//         // Convert the result to an UTF8 string and print it.
//         //v8::String::Utf8Value utf8(isolate, result);
//         //printf("%s\n", *utf8);
//     }
// 
//     return 0;
// }

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
#if V8_MAJOR_VERSION != 7
  params.entry_hook = DebugImpl::GetFunctionEntryHook();
#endif
  params.code_event_handler = DebugImpl::GetJitCodeEventHandler();
  params.constraints.ConfigureDefaults(v8::base::SysInfo::AmountOfPhysicalMemory(), v8::base::SysInfo::AmountOfVirtualMemory());
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
  //testQueenMain(isolate_);
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
    MemoryHead* head = ((MemoryHead*)pointer) - 1;
    if (head->magicNum != gin::IsolateHolder::magicNum0)
        DebugBreak();
    return head;
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
