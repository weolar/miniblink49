// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libcef/common/CefTaskImpl.h"
#include "libcef/common/CefTaskRunnerImpl.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, cefThreadTaskCounter, ("cefThreadTaskCounter"));
#endif

class CefThreadTask : public blink::WebThread::Task {
public:
    CefThreadTask(CefRefPtr<CefTask> task)
        : m_task(task)
    {
#ifndef NDEBUG
        cefThreadTaskCounter.increment();
#endif
    }

    virtual ~CefThreadTask() OVERRIDE
    {
#ifndef NDEBUG
        cefThreadTaskCounter.decrement();
#endif
    }

    virtual void run() OVERRIDE
    {
        m_task->Execute();
    }

private:
    CefRefPtr<CefTask> m_task;
};

bool CefPostTask(CefThreadId threadId, CefRefPtr<CefTask> task) {
    blink::WebThread* taskRunner = CefTaskRunnerImpl::GetTaskRunner(threadId);
    if (taskRunner) {
        taskRunner->postTask(FROM_HERE, new CefThreadTask(task));
        return true;
    }
    return false;
}

bool CefPostDelayedTask(CefThreadId threadId, CefRefPtr<CefTask> task, int64 delay_ms) {
    blink::WebThread* taskRunner = CefTaskRunnerImpl::GetTaskRunner(threadId);
    if (taskRunner) {
        taskRunner->postDelayedTask(FROM_HERE, new CefThreadTask(task), delay_ms);
        return true;
    }
    return false;
}

bool CefPostBlinkTask(const blink::WebTraceLocation& location, CefThreadId threadId, WTF::PassOwnPtr<WTF::Function<void()>> task) {
	blink::WebThread* taskRunner = CefTaskRunnerImpl::GetTaskRunner(threadId);
	if (taskRunner) {
		taskRunner->postTask(location, (task));
		return true;
	}
	return false;
}

bool CefPostBlinkDelayedTask(const blink::WebTraceLocation& location, CefThreadId threadId, WTF::PassOwnPtr<WTF::Function<void()>> task, int64 delayMs) {
	blink::WebThread* taskRunner = CefTaskRunnerImpl::GetTaskRunner(threadId);
	if (taskRunner) {
		taskRunner->postDelayedTask(location, (task), delayMs);
		return true;
	}
	return false;
}
