// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/common/CefTaskRunnerImpl.h"

#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"

// CefTaskRunner

// static
CefRefPtr<CefTaskRunner> CefTaskRunner::GetForCurrentThread() {
    blink::WebThread* taskRunner = CefTaskRunnerImpl::GetCurrentTaskRunner();
    if (taskRunner)
        return new CefTaskRunnerImpl(taskRunner);
    return nullptr;
}

// static
CefRefPtr<CefTaskRunner> CefTaskRunner::GetForThread(CefThreadId threadId) {
    blink::WebThread* taskRunner = CefTaskRunnerImpl::GetTaskRunner(threadId);
    if (taskRunner)
        return new CefTaskRunnerImpl(taskRunner);

    //LOG(WARNING) << "Invalid thread id " << threadId;
    return nullptr;
}


// CefTaskRunnerImpl

CefTaskRunnerImpl::CefTaskRunnerImpl(blink::WebThread* taskRunner)
    : m_taskRunner(taskRunner) {
    ASSERT(m_taskRunner);
}

// static
blink::WebThread* CefTaskRunnerImpl::GetTaskRunner(CefThreadId threadId) {
	content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
    blink::WebThread* webThread = nullptr;

    // Render process.
    if (threadId == TID_RENDERER) {
        webThread = platform->mainThread();
        return webThread;
    }

    // Browser process.
    int id = -1;
    switch (threadId) {
    case TID_UI:
        webThread = platform->mainThread();
        break;
    case TID_DB:
        DebugBreak();
        break;
    case TID_FILE:
        DebugBreak();
        break;
    case TID_FILE_USER_BLOCKING:
        DebugBreak();
        break;
    case TID_PROCESS_LAUNCHER:
        DebugBreak();
        break;
    case TID_CACHE:
        DebugBreak();
        break;
    case TID_IO:
        webThread = platform->ioThread(); // TODO test
        break;
    default:
        break;
    };

    return webThread;
}

// static
blink::WebThread* CefTaskRunnerImpl::GetCurrentTaskRunner() {
    blink::WebThread* taskRunner;

//     // Check for a MessageLoopProxy. This covers all of the named browser and
//     // render process threads, plus a few extra.
//     taskRunner = base::ThreadTaskRunnerHandle::Get();
// 
//     if (!taskRunner.get()) {
//         // Check for a WebWorker thread.
//         CefContentRendererClient* client = CefContentRendererClient::Get();
//         if (client)
//             taskRunner = client->GetCurrentTaskRunner();
//     }

    taskRunner = blink::Platform::current()->currentThread();
    return taskRunner;
}

bool CefTaskRunnerImpl::IsSame(CefRefPtr<CefTaskRunner> that) {
    CefTaskRunnerImpl* impl = static_cast<CefTaskRunnerImpl*>(that.get());
    return (impl && m_taskRunner == impl->m_taskRunner);
}

bool CefTaskRunnerImpl::BelongsToCurrentThread() {
    return m_taskRunner->isCurrentThread();
}

bool CefTaskRunnerImpl::BelongsToThread(CefThreadId threadId) {
    blink::WebThread* taskRunner = GetTaskRunner(threadId);
    return (m_taskRunner == taskRunner);
}

bool CefTaskRunnerImpl::PostTask(CefRefPtr<CefTask> task) {
    m_taskRunner->postTask(FROM_HERE, WTF::bind(&CefTask::Execute, task.get()));
    return true;
}

bool CefTaskRunnerImpl::PostDelayedTask(CefRefPtr<CefTask> task, int64 delay_ms) {
    m_taskRunner->postDelayedTask(FROM_HERE, WTF::bind(&CefTask::Execute, task.get()), (delay_ms));
    return true;
}
#endif
