// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/InspectorTaskRunner.h"

#include "wtf/Deque.h"
#include "wtf/ThreadingPrimitives.h"
#include <v8.h>

namespace blink {

InspectorTaskRunner::IgnoreInterruptsScope::IgnoreInterruptsScope(InspectorTaskRunner* taskRunner)
    : m_wasIgnoring(taskRunner->m_ignoreInterrupts)
    , m_taskRunner(taskRunner)
{
    // There may be nested scopes e.g. when tasks are being executed on XHR breakpoint.
    m_taskRunner->m_ignoreInterrupts = true;
}

InspectorTaskRunner::IgnoreInterruptsScope::~IgnoreInterruptsScope()
{
    m_taskRunner->m_ignoreInterrupts = m_wasIgnoring;
}

class InspectorTaskRunner::ThreadSafeTaskQueue {
    WTF_MAKE_NONCOPYABLE(ThreadSafeTaskQueue);
public:
    ThreadSafeTaskQueue() { }
    PassOwnPtr<Task> tryTake()
    {
        MutexLocker lock(m_mutex);
        if (m_queue.isEmpty())
            return nullptr;
        return m_queue.takeFirst();
    }
    void append(PassOwnPtr<Task> task)
    {
        MutexLocker lock(m_mutex);
        m_queue.append(task);
    }
private:
    Mutex m_mutex;
    Deque<OwnPtr<Task>> m_queue;
};


InspectorTaskRunner::InspectorTaskRunner(v8::Isolate* isolate)
    : m_isolate(isolate)
    , m_taskQueue(adoptPtr(new ThreadSafeTaskQueue))
    , m_ignoreInterrupts(false)
{
}

InspectorTaskRunner::~InspectorTaskRunner()
{
}

void InspectorTaskRunner::interruptAndRun(PassOwnPtr<Task> task)
{
    m_taskQueue->append(task);
    m_isolate->RequestInterrupt(&v8InterruptCallback, this);
}

void InspectorTaskRunner::runPendingTasks()
{
    while (true) {
        OwnPtr<Task> task = m_taskQueue->tryTake();
        if (!task)
            return;
        task->run();
    }
}

void InspectorTaskRunner::v8InterruptCallback(v8::Isolate*, void* data)
{
    InspectorTaskRunner* runner = static_cast<InspectorTaskRunner*>(data);
    if (runner->m_ignoreInterrupts)
        return;
    runner->runPendingTasks();
}

}
