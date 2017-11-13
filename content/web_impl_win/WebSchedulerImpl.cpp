#include "config.h"
#include "base/compiler_specific.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebScheduler.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "WebSchedulerImpl.h"
#include "WebThreadImpl.h"

namespace content {

class IdleTaskToTask : public blink::WebThread::Task {
public:
    IdleTaskToTask(blink::WebThread::IdleTask* task) { m_task = task; }

    virtual ~IdleTaskToTask() override { delete m_task; }

    virtual void run() override { m_task->run(0); }
private:
    blink::WebThread::IdleTask* m_task;
};

WebSchedulerImpl::WebSchedulerImpl(WebThreadImpl* thread)
    : m_thread(thread)
{

}

WebSchedulerImpl::~WebSchedulerImpl() { }

void WebSchedulerImpl::shutdown() 
{
}

bool WebSchedulerImpl::shouldYieldForHighPriorityWork()
{
    return false; 
}

bool WebSchedulerImpl::canExceedIdleDeadlineIfRequired() { return true; }

void WebSchedulerImpl::postIdleTask(const blink::WebTraceLocation& location, blink::WebThread::IdleTask* task)
{
    m_thread->postDelayedTask(location, (new IdleTaskToTask(task)), 0);
}

void WebSchedulerImpl::postNonNestableIdleTask(const blink::WebTraceLocation& location, blink::WebThread::IdleTask* task)
{
    m_thread->postDelayedTask(location, (new IdleTaskToTask(task)), 0);
}

void WebSchedulerImpl::postIdleTaskAfterWakeup(const blink::WebTraceLocation& location, blink::WebThread::IdleTask* task)
{ 
    m_thread->postDelayedTask(location, (new IdleTaskToTask(task)), 0);
}

void WebSchedulerImpl::postLoadingTask(const blink::WebTraceLocation& location, blink::WebThread::Task* task) 
{
    m_thread->postDelayedTaskWithPriorityCrossThread(location, task, 0, WebThreadImpl::kDefaultPriority);
}

void WebSchedulerImpl::postTimerTask(const blink::WebTraceLocation& location, blink::WebThread::Task* task, long long delayMs)
{
    m_thread->postDelayedTask(location, task, delayMs);
}

void WebSchedulerImpl::postTimerTaskAt(const blink::WebTraceLocation&, blink::WebThread::Task*, double monotonicTime) { notImplemented(); }

void WebSchedulerImpl::suspendTimerQueue() 
{
    m_thread->suspendTimerQueue();
}

void WebSchedulerImpl::resumeTimerQueue() 
{
    m_thread->resumeTimerQueue();
}

void WebSchedulerImpl::postIdleTask(const blink::WebTraceLocation&, PassOwnPtr<IdleTask>) { notImplemented(); }
void WebSchedulerImpl::postNonNestableIdleTask(const blink::WebTraceLocation&, PassOwnPtr<IdleTask>) { notImplemented(); }
void WebSchedulerImpl::postIdleTaskAfterWakeup(const blink::WebTraceLocation&, PassOwnPtr<IdleTask>) { notImplemented(); }
void WebSchedulerImpl::postLoadingTask(const blink::WebTraceLocation&, PassOwnPtr<Task>) { notImplemented(); }

} // content