// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/public/v8_platform.h"
#if V8_MAJOR_VERSION >= 7
#include "v8_7_5/src/libplatform/default_platform_wrap.h"
#endif
#include <windows.h>
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8PerIsolateData.h"
#include "gin/v8_task_runner.h"

namespace gin {

namespace {

static V8Platform* g_v8_platform = nullptr;

}  // namespace

#if V8_MAJOR_VERSION >= 7

// TracingController implementation.
const uint8_t* TracingControllerImpl::GetCategoryGroupEnabled(const char* name)
{
    return (const uint8_t*)(name);
}

uint64_t TracingControllerImpl::AddTraceEvent(
    char phase,
    const uint8_t* category_enabled_flag,
    const char* name,
    const char* scope,
    uint64_t id,
    uint64_t bind_id,
    int32_t num_args,
    const char** arg_names,
    const uint8_t* arg_types,
    const uint64_t* arg_values,
    std::unique_ptr<v8::ConvertableToTraceFormat>* arg_convertables,
    unsigned int flags)
{
    //         base::trace_event::TraceArguments args(
    //             num_args, arg_names, arg_types,
    //             reinterpret_cast<const unsigned long long*>(arg_values),
    //             arg_convertables);
    //         DCHECK_LE(num_args, 2);
    //         base::trace_event::TraceEventHandle handle =
    //             TRACE_EVENT_API_ADD_TRACE_EVENT_WITH_BIND_ID(
    //                 phase, category_enabled_flag, name, scope, id, bind_id, &args,
    //                 flags);
    //         uint64_t result;
    //         memcpy(&result, &handle, sizeof(result));
    //         return result;
    return 0;
}

uint64_t TracingControllerImpl::AddTraceEventWithTimestamp(
    char phase,
    const uint8_t* category_enabled_flag,
    const char* name,
    const char* scope,
    uint64_t id,
    uint64_t bind_id,
    int32_t num_args,
    const char** arg_names,
    const uint8_t* arg_types,
    const uint64_t* arg_values,
    std::unique_ptr<v8::ConvertableToTraceFormat>* arg_convertables,
    unsigned int flags,
    int64_t timestampMicroseconds)
{
    //         base::trace_event::TraceArguments args(
    //             num_args, arg_names, arg_types,
    //             reinterpret_cast<const unsigned long long*>(arg_values),
    //             arg_convertables);
    //         DCHECK_LE(num_args, 2);
    //         base::TimeTicks timestamp =
    //             base::TimeTicks() +
    //             base::TimeDelta::FromMicroseconds(timestampMicroseconds);
    //         base::trace_event::TraceEventHandle handle =
    //             TRACE_EVENT_API_ADD_TRACE_EVENT_WITH_THREAD_ID_AND_TIMESTAMP(
    //                 phase, category_enabled_flag, name, scope, id, bind_id,
    //                 TRACE_EVENT_API_CURRENT_THREAD_ID, timestamp, &args, flags);
    //         uint64_t result;
    //         memcpy(&result, &handle, sizeof(result));
    //         return result;
    return 0;
}

void TracingControllerImpl::UpdateTraceEventDuration(const uint8_t* category_enabled_flag, const char* name, uint64_t handle)
{
    //         base::trace_event::TraceEventHandle traceEventHandle;
    //         memcpy(&traceEventHandle, &handle, sizeof(handle));
    //         TRACE_EVENT_API_UPDATE_TRACE_EVENT_DURATION(category_enabled_flag, name, traceEventHandle);
}

void TracingControllerImpl::AddTraceStateObserver(TraceStateObserver* observer)
{
    //g_trace_state_dispatcher.Get().AddObserver(observer);
}

void TracingControllerImpl::RemoveTraceStateObserver(TraceStateObserver* observer)
{
    //g_trace_state_dispatcher.Get().RemoveObserver(observer);
}

#endif

// static
V8Platform* V8Platform::Get() {
    if (!g_v8_platform)
        g_v8_platform = new V8Platform();
    return g_v8_platform;
}

struct WorkThreadItem {
    WorkThreadItem()
    {
        task = nullptr;
        next = nullptr;
    }
    ~WorkThreadItem()
    {
        if (task)
            delete task;
    }
    v8::Task* task;
    WorkThreadItem* next;
};

struct WorkThreadInfo {
    WorkThreadInfo()
    {
        ::InitializeCriticalSection(&lock);
        head = nullptr;
    }
    CRITICAL_SECTION lock;
    WorkThreadItem* head;
};
WorkThreadInfo* g_workThreadInfo = nullptr;

static DWORD NTAPI v8WorkThreadProc(void* param)
{
    WorkThreadInfo* info = (WorkThreadInfo*)param;
    g_workThreadInfo = info;

    while (true) {
        ::EnterCriticalSection(&info->lock);
        WorkThreadItem* item = info->head;
        if (info->head)
            info->head = info->head->next;
        ::LeaveCriticalSection(&info->lock);

        if (item)
            item->task->Run();
        delete item;
        
        ::Sleep(16);
    }
}

V8Platform::V8Platform()
{
#if V8_MAJOR_VERSION >= 7
    m_tracingControllerImpl = new TracingControllerImpl();
    m_defaultPlatformWrap = new DefaultPlatformWrap();
    //m_foregroundTaskRunner = std::make_shared<V8ForegroundTaskRunner>();
#else
    DWORD threadIdentifier = 0;
    HANDLE handle = ::CreateThread(0, 0, v8WorkThreadProc, new WorkThreadInfo(), 0, &threadIdentifier);
    while (!g_workThreadInfo)
        ::Sleep(1);
    ::CloseHandle(handle);
#endif
}

V8Platform::~V8Platform()
{
#if V8_MAJOR_VERSION >= 7
    delete m_tracingControllerImpl;
    delete m_defaultPlatformWrap;
#endif
}

#if V8_MAJOR_VERSION < 7

void V8Platform::CallOnBackgroundThread(v8::Task* task, v8::Platform::ExpectedRuntime expected_runtime)
{
    ULONG flags = 0;
    if (expected_runtime == v8::Platform::kLongRunningTask)
        flags |= WT_EXECUTELONGFUNCTION;

    ::EnterCriticalSection(&g_workThreadInfo->lock);
    WorkThreadItem* item = g_workThreadInfo->head;
    do {
        if (!item) {
            g_workThreadInfo->head = new WorkThreadItem();
            g_workThreadInfo->head->task = task;
            break;
        }

        if (!item->next) {
            item->next = new WorkThreadItem();
            item->next->task = task;
            break;
        }
        item = item->next;
    } while (item);
    ::LeaveCriticalSection(&g_workThreadInfo->lock);
}

#endif

void V8Platform::CallOnForegroundThread(v8::Isolate* isolate, v8::Task* task)
{
    blink::V8PerIsolateData* data = blink::V8PerIsolateData::from(isolate);
    data->getThread()->postTask(FROM_HERE, new V8TaskToWebThreadTask(task));
}

void V8Platform::CallDelayedOnForegroundThread(v8::Isolate* isolate, v8::Task* task, double delay_in_seconds)
{
    blink::V8PerIsolateData* data = blink::V8PerIsolateData::from(isolate);
    data->getThread()->postDelayedTask(FROM_HERE, new V8TaskToWebThreadTask(task), (long long)(delay_in_seconds * 1000));
}

double V8Platform::MonotonicallyIncreasingTime()
{
    return blink::Platform::current()->monotonicallyIncreasingTime();
}

#if V8_MAJOR_VERSION >= 7

void V8Platform::CallOnWorkerThread(std::unique_ptr<v8::Task> task)
{
    m_defaultPlatformWrap->CallOnWorkerThread(std::move(task));
}

void V8Platform::CallDelayedOnWorkerThread(std::unique_ptr<v8::Task> task, double delay_in_seconds)
{
    //return blink::Platform::current()->currentThread()->postTask(FROM_HERE, new V8TaskToWebThreadTask(task.release()));
  return m_defaultPlatformWrap->CallDelayedOnWorkerThread(std::move(task), delay_in_seconds);
}

double V8Platform::CurrentClockTimeMillis()
{
    //return blink::Platform::current()->monotonicallyIncreasingTime();
    return m_defaultPlatformWrap->CurrentClockTimeMillis();
}

int V8Platform::NumberOfWorkerThreads()
{
    return m_defaultPlatformWrap->NumberOfWorkerThreads();
}

std::shared_ptr<v8::TaskRunner> V8Platform::GetForegroundTaskRunner(v8::Isolate* isolate)
{
    //return m_defaultPlatformWrap->GetForegroundTaskRunner(isolate);
    //return m_foregroundTaskRunner;
    return blink::V8PerIsolateData::getThreadRunner(isolate);
}

#endif

}  // namespace gin
