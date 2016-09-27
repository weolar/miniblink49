#include "config.h"
#include "base/compiler_specific.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebScheduler.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "WebSchedulerImpl.h"
#include "WebThreadImpl.h"

namespace content {

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

bool WebSchedulerImpl::canExceedIdleDeadlineIfRequired() { notImplemented(); return false; }

void WebSchedulerImpl::postIdleTask(const blink::WebTraceLocation&, blink::WebThread::IdleTask*) { notImplemented(); }

void WebSchedulerImpl::postNonNestableIdleTask(const blink::WebTraceLocation&, blink::WebThread::IdleTask*) { notImplemented(); }

void WebSchedulerImpl::postIdleTaskAfterWakeup(const blink::WebTraceLocation&, blink::WebThread::IdleTask*) { notImplemented(); }

void WebSchedulerImpl::postLoadingTask(const blink::WebTraceLocation& location, blink::WebThread::Task* task) { m_thread->postDelayedTask(location, task, 0); }

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