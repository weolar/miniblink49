// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebThreadSupportingGC_h
#define WebThreadSupportingGC_h

#include "platform/heap/glue/MessageLoopInterruptor.h"
#include "platform/heap/glue/PendingGCRunner.h"
#include "public/platform/Platform.h"
#include "public/platform/WebThread.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

// WebThreadSupportingGC wraps a WebThread and adds support for attaching
// to and detaching from the Blink GC infrastructure. The initialize method
// must be called during initialization on the WebThread and before the
// thread allocates any objects managed by the Blink GC. The shutdown
// method must be called on the WebThread during shutdown when the thread
// no longer needs to access objects managed by the Blink GC.
class PLATFORM_EXPORT WebThreadSupportingGC final {
    WTF_MAKE_NONCOPYABLE(WebThreadSupportingGC);
public:
    static PassOwnPtr<WebThreadSupportingGC> create(const char*);
    ~WebThreadSupportingGC();

    void postTask(const WebTraceLocation& location, WebThread::Task* task)
    {
        m_thread->postTask(location, task);
    }

    void postDelayedTask(const WebTraceLocation& location, WebThread::Task* task, long long delayMs)
    {
        m_thread->postDelayedTask(location, task, delayMs);
    }

    bool isCurrentThread() const
    {
        return m_thread->isCurrentThread();
    }

    void addTaskObserver(WebThread::TaskObserver* observer)
    {
        m_thread->addTaskObserver(observer);
    }

    void removeTaskObserver(WebThread::TaskObserver* observer)
    {
        m_thread->removeTaskObserver(observer);
    }

    void initialize();
    void shutdown();

    WebThread& platformThread() const
    {
        ASSERT(m_thread);
        return *m_thread;
    }

private:
    explicit WebThreadSupportingGC(const char*);

    OwnPtr<PendingGCRunner> m_pendingGCRunner;
    OwnPtr<MessageLoopInterruptor> m_messageLoopInterruptor;

    // FIXME: This has to be last because of crbug.com/401397.
    // A WorkerThread might get deleted before it had a chance to properly
    // shut down. By deleting the WebThread first, we can guarantee that
    // no pending tasks on the thread might want to access any of the other
    // members during the WorkerThread's destruction.
    OwnPtr<WebThread> m_thread;
};

}

#endif
