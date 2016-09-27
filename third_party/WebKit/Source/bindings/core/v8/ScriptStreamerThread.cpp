// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ScriptStreamerThread.h"

#include "bindings/core/v8/ScriptStreamer.h"
#include "platform/Task.h"
#include "platform/TraceEvent.h"
#include "public/platform/Platform.h"
#include "wtf/MainThread.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

static ScriptStreamerThread* s_sharedThread = 0;
// Guards s_sharedThread. s_sharedThread is initialized and deleted in the main
// thread, but also used by the streamer thread. Races can occur during
// shutdown.
static Mutex* s_mutex = 0;

void ScriptStreamerThread::init()
{
    ASSERT(!s_sharedThread);
    ASSERT(isMainThread());
    // This is called in the main thread before any tasks are created, so no
    // locking is needed.
    s_mutex = new Mutex();
    s_sharedThread = new ScriptStreamerThread();
}

void ScriptStreamerThread::shutdown()
{
    ASSERT(s_sharedThread);
    ScriptStreamerThread* toDelete;
    {
        MutexLocker locker(*s_mutex);
        toDelete = s_sharedThread;
        // The background thread can now safely check s_sharedThread; if it's
        // not 0, we're not shutting down.
        s_sharedThread = 0;
    }
    // This will run the pending tasks into completion. We shouldn't hold the
    // mutex while doing that.
    delete toDelete;
    // Now it's safe to delete s_mutex, since there are no tasks that could
    // access it later.
    delete s_mutex;
}

ScriptStreamerThread* ScriptStreamerThread::shared()
{
    return s_sharedThread;
}

void ScriptStreamerThread::postTask(WebThread::Task* task)
{
    ASSERT(isMainThread());
    MutexLocker locker(m_mutex);
    ASSERT(!m_runningTask);
    m_runningTask = true;
    platformThread().postTask(FROM_HERE, task);
}

void ScriptStreamerThread::taskDone()
{
    MutexLocker locker(m_mutex);
    ASSERT(m_runningTask);
    m_runningTask = false;
}

WebThread& ScriptStreamerThread::platformThread()
{
    if (!isRunning())
        m_thread = adoptPtr(Platform::current()->createThread("ScriptStreamerThread"));
    return *m_thread;
}

void ScriptStreamerThread::runScriptStreamingTask(WTF::PassOwnPtr<v8::ScriptCompiler::ScriptStreamingTask> task, ScriptStreamer* streamer)
{
    TRACE_EVENT0("v8", "v8.parseOnBackground");
    // Running the task can and will block: SourceStream::GetSomeData will get
    // called and it will block and wait for data from the network.
    task->Run();
    streamer->streamingCompleteOnBackgroundThread();
    MutexLocker locker(*s_mutex);
    ScriptStreamerThread* thread = shared();
    if (thread)
        thread->taskDone();
    // If thread is 0, we're shutting down.
}

} // namespace blink
