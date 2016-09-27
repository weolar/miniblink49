/*
 * Copyright (C) 2010 Google Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"
#include "core/workers/WorkerEventQueue.h"

#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "core/events/Event.h"
#include "core/inspector/InspectorInstrumentation.h"

namespace blink {

PassOwnPtrWillBeRawPtr<WorkerEventQueue> WorkerEventQueue::create(ExecutionContext* context)
{
    return adoptPtrWillBeNoop(new WorkerEventQueue(context));
}

WorkerEventQueue::WorkerEventQueue(ExecutionContext* context)
    : m_executionContext(context)
    , m_isClosed(false)
{
}

WorkerEventQueue::~WorkerEventQueue()
{
    ASSERT(m_eventTaskMap.isEmpty());
}

DEFINE_TRACE(WorkerEventQueue)
{
#if ENABLE(OILPAN)
    visitor->trace(m_executionContext);
    visitor->trace(m_eventTaskMap);
#endif
    EventQueue::trace(visitor);
}

class WorkerEventQueue::EventDispatcherTask : public ExecutionContextTask {
public:
    static PassOwnPtr<EventDispatcherTask> create(PassRefPtrWillBeRawPtr<Event> event, WorkerEventQueue* eventQueue)
    {
        return adoptPtr(new EventDispatcherTask(event, eventQueue));
    }

    virtual ~EventDispatcherTask()
    {
        if (m_event)
            m_eventQueue->removeEvent(m_event.get());
    }

    void dispatchEvent(ExecutionContext*, PassRefPtrWillBeRawPtr<Event> event)
    {
        event->target()->dispatchEvent(event);
    }

    virtual void performTask(ExecutionContext* context)
    {
        if (m_isCancelled)
            return;
        m_eventQueue->removeEvent(m_event.get());
        dispatchEvent(context, m_event);
        m_event.clear();
    }

    void cancel()
    {
        m_isCancelled = true;
        m_event.clear();
    }

private:
    EventDispatcherTask(PassRefPtrWillBeRawPtr<Event> event, WorkerEventQueue* eventQueue)
        : m_event(event)
        , m_eventQueue(eventQueue)
        , m_isCancelled(false)
    {
    }

    RefPtrWillBePersistent<Event> m_event;
    WorkerEventQueue* m_eventQueue;
    bool m_isCancelled;
};

void WorkerEventQueue::removeEvent(Event* event)
{
    InspectorInstrumentation::didRemoveEvent(event->target(), event);
    m_eventTaskMap.remove(event);
}

bool WorkerEventQueue::enqueueEvent(PassRefPtrWillBeRawPtr<Event> prpEvent)
{
    if (m_isClosed)
        return false;
    RefPtrWillBeRawPtr<Event> event = prpEvent;
    InspectorInstrumentation::didEnqueueEvent(event->target(), event.get());
    OwnPtr<EventDispatcherTask> task = EventDispatcherTask::create(event, this);
    m_eventTaskMap.add(event.release(), task.get());
    m_executionContext->postTask(FROM_HERE, task.release());
    return true;
}

bool WorkerEventQueue::cancelEvent(Event* event)
{
    EventDispatcherTask* task = m_eventTaskMap.get(event);
    if (!task)
        return false;
    task->cancel();
    removeEvent(event);
    return true;
}

void WorkerEventQueue::close()
{
    m_isClosed = true;
    for (const auto& entry : m_eventTaskMap) {
        Event* event = entry.key.get();
        EventDispatcherTask* task = entry.value;
        InspectorInstrumentation::didRemoveEvent(event->target(), event);
        task->cancel();
    }
    m_eventTaskMap.clear();
}

}
