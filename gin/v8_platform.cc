// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/public/v8_platform.h"
#ifdef MINIBLINK_NOT_IMPLEMENTED
#include "base/bind.h"
#include "base/location.h"
#include "base/threading/worker_pool.h"
#include "gin/per_isolate_data.h"
#else
#include "windows.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#endif // MINIBLINK_NOT_IMPLEMENTED


namespace gin {

namespace {

#ifdef MINIBLINK_NOT_IMPLEMENTED
base::LazyInstance<V8Platform>::Leaky g_v8_platform = LAZY_INSTANCE_INITIALIZER;
#else
static V8Platform* g_v8_platform = nullptr;
#endif // MINIBLINK_NOT_IMPLEMENTED


}  // namespace

// static
#ifdef MINIBLINK_NOT_IMPLEMENTED

V8Platform* V8Platform::Get() { return g_v8_platform.Pointer(); }

#else

V8Platform* V8Platform::Get() {
  if (!g_v8_platform)
    g_v8_platform = new V8Platform();
  return g_v8_platform;
}

class V8TaskToWebThreadTask : public blink::WebThread::Task {
public:
    V8TaskToWebThreadTask(v8::Task* task)
        : m_task(task) { }

    virtual ~V8TaskToWebThreadTask() OVERRIDE { delete m_task; }
    virtual void run() OVERRIDE { m_task->Run(); }

private:
    v8::Task* m_task;
};

#endif // MINIBLINK_NOT_IMPLEMENTED

V8Platform::V8Platform() {}

V8Platform::~V8Platform() {}

DWORD CALLBACK WorkItemCallback(void* param)
{
    v8::Task* task = (v8::Task*)param;
    task->Run();
    delete task;
    return 0;
}

void V8Platform::CallOnBackgroundThread(
    v8::Task* task,
    v8::Platform::ExpectedRuntime expected_runtime) {
#ifdef MINIBLINK_NOT_IMPLEMENTED
  base::WorkerPool::PostTask(
      FROM_HERE,
      base::Bind(&v8::Task::Run, base::Owned(task)),
      expected_runtime == v8::Platform::kLongRunningTask);
#endif // MINIBLINK_NOT_IMPLEMENTED
  ULONG flags = 0;
  if (expected_runtime == v8::Platform::kLongRunningTask)
      flags |= WT_EXECUTELONGFUNCTION;

  if (!QueueUserWorkItem(WorkItemCallback, task, flags)) {
      delete task;
      return;
  }
}

void V8Platform::CallOnForegroundThread(v8::Isolate* isolate, v8::Task* task) {
#ifdef MINIBLINK_NOT_IMPLEMENTED
  PerIsolateData::From(isolate)->task_runner()->PostTask(
      FROM_HERE, base::Bind(&v8::Task::Run, base::Owned(task)));
#else
    return blink::Platform::current()->currentThread()->postTask(FROM_HERE, new V8TaskToWebThreadTask(task));
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void V8Platform::CallDelayedOnForegroundThread(v8::Isolate* isolate,
                                               v8::Task* task,
                                               double delay_in_seconds) {
#ifdef MINIBLINK_NOT_IMPLEMENTED
  PerIsolateData::From(isolate)->task_runner()->PostDelayedTask(
      FROM_HERE, base::Bind(&v8::Task::Run, base::Owned(task)),
      base::TimeDelta::FromSecondsD(delay_in_seconds));
#else
    return blink::Platform::current()->currentThread()->postDelayedTask(FROM_HERE, new V8TaskToWebThreadTask(task), (long long)(delay_in_seconds * 1000));
#endif // MINIBLINK_NOT_IMPLEMENTED
}

double V8Platform::MonotonicallyIncreasingTime() {
#ifdef MINIBLINK_NOT_IMPLEMENTED
  return base::TimeTicks::Now().ToInternalValue() /
      static_cast<double>(base::Time::kMicrosecondsPerSecond);
#else
    return blink::Platform::current()->monotonicallyIncreasingTime();
#endif // MINIBLINK_NOT_IMPLEMENTED
}

}  // namespace gin
