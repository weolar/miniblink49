
#include "config.h"

#include "content/web_impl_win/WebThreadImpl.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebTimerBase.h"
#include "content/web_impl_win/WebSchedulerImpl.h"
#include "content/web_impl_win/ActivatingTimerCheck.h"
#include "content/browser/SharedTimerWin.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/browser/CefContext.h"
#endif
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#include "wke/wkeJsBindFreeTempObject.h"
#endif

#include "base/compiler_specific.h"
#include "base/thread.h"

#include <windows.h>
#include <process.h>
#include <mmsystem.h>

namespace content {

// 100ms is about a perceptable delay in UI, so use a half of that as a threshold.
// This is to prevent UI freeze when there are too many timers or machine performance is low.
static const double maxDurationOfFiringTimers = 0.050;

unsigned WebThreadImpl::m_currentHeapInsertionOrder = 0;

unsigned __stdcall WebThreadImpl::WebThreadImplThreadEntryPoint(void* param)
{
    WebThreadImpl* impl = (WebThreadImpl*)param;
    impl->threadEntryPoint();
    return 0;
}

unsigned WebThreadImpl::getNewCurrentHeapInsertionOrder()
{
    return atomicIncrement((volatile int *)&m_currentHeapInsertionOrder);
}

#ifdef _DEBUG
ActivatingTimerCheck* gActivatingTimerCheck = nullptr;
#endif

WebThreadImpl::WebThreadImpl(const char* name)
    : m_hEvent(NULL)
    , m_threadId(-1)
    , m_firingTimers(false)
    , m_webSchedulerImpl(new WebSchedulerImpl(this))
    , m_name(name)
    , m_suspendTimerQueue(false)
    , m_hadThreadInit(false)
    , m_willExit(false)
    , m_threadClosed(false)
    , m_threadHandle(nullptr)
    , m_currentFrameCreateTime(0)
{
    m_name = name;
    ::InitializeCriticalSection(&m_taskPairsMutex);

    m_isMainThread = (0 == strcmp("MainThread", name));
    if (m_isMainThread) {
#ifdef _DEBUG
        if (!gActivatingTimerCheck)
            gActivatingTimerCheck = new ActivatingTimerCheck();
#endif
        m_hadThreadInit = true;
        m_threadId = WTF::currentThread();
        updateSharedTimer();
        return;
    }

    m_hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    unsigned threadIdentifier = 0;
    m_threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, WebThreadImplThreadEntryPoint, this, 0, &threadIdentifier));
 
    while (!m_hadThreadInit) {
        Sleep(20);
    };
}

WebThreadImpl::~WebThreadImpl()
{
    //if (0 != strcmp("MainThread", m_name))
        shutdown();

    if (m_threadHandle) {
        ::WaitForSingleObject(m_threadHandle, INFINITE);
        ::CloseHandle(m_threadHandle);
        m_threadHandle = nullptr;
    }

    delete m_webSchedulerImpl;
    
    ::DeleteCriticalSection(&m_taskPairsMutex);

    BlinkPlatformImpl* platform = (BlinkPlatformImpl*)blink::Platform::current();
    platform->onThreadExit(this);
}

void WebThreadImpl::shutdown()
{
    if (m_isMainThread)
        stopSharedTimer();
    willExit();
    waitForExit();
}

void WebThreadImpl::waitForExit()
{
    if (m_threadId == WTF::currentThread())
        return;

    while (!m_threadClosed) {
        Sleep(20);
    }
}

void WebThreadImpl::willExit()
{
    m_willExit = true;
    if (m_hEvent)
        ::SetEvent(m_hEvent);

//     if (0 == strcmp("MainThread", m_name))
//         return;

    if (m_threadId == WTF::currentThread())
        fireOnExit();
}

void WebThreadImpl::threadEntryPoint()
{
    base::SetThreadName(m_name);

    m_threadId = WTF::currentThread();
    BlinkPlatformImpl::onCurrentThreadWhenWebThreadImplCreated(this);
    m_hadThreadInit = true;

    while (!m_willExit) {
        DWORD dReturn = ::WaitForSingleObject(m_hEvent, INFINITE);

        startTriggerTasks();

        while (!m_timerHeap.empty()) {
            schedulerTasks();
            ::Sleep(10);
        }
    }

    fireOnExit();
    m_threadClosed = true;
}

void WebThreadImpl::postTask(const blink::WebTraceLocation& location, blink::WebThread::Task* task)
{
    postDelayedTask(location, task, 0);
}

void WebThreadImpl::postDelayedTaskImpl(
    const blink::WebTraceLocation& location, blink::WebThread::Task* task, 
    long long delayMs, double* createTimeOnOtherThread, int priority, unsigned* heapInsertionOrder)
{
    // delete by self
    WebTimerBase* timer = WebTimerBase::create(this, location, task, priority);
//     timer->startOneShot((double)delayMs / 1000.0);
    timer->startFromOtherThread((double)delayMs / 1000.0, createTimeOnOtherThread, heapInsertionOrder);
}

void WebThreadImpl::postDelayedTaskWithPriorityCrossThread(
    const blink::WebTraceLocation& location, 
    blink::WebThread::Task* task,
    long long delayMs,
    int priority)
{
    if (m_willExit) {
        if (m_hEvent)
            ::SetEvent(m_hEvent);
        delete task;
        return;
    }

    if (isCurrentThread()) {
        postDelayedTaskImpl(location, task, delayMs, &m_currentFrameCreateTime, priority, nullptr);
        return;
    }

    ::EnterCriticalSection(&m_taskPairsMutex);
    m_taskPairsToPost.push_back(new TaskPair(location, task, delayMs, priority));

    if (m_hEvent)
        ::SetEvent(m_hEvent);
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (CefContext::Get())
        CefContext::Get()->SetNeedHeartbeat();
#endif
    ::LeaveCriticalSection(&m_taskPairsMutex);
}

void WebThreadImpl::postDelayedTask(const blink::WebTraceLocation& location, blink::WebThread::Task* task, long long delayMs)
{
    postDelayedTaskWithPriorityCrossThread(location, task, delayMs, kLoadingPriority);
}

WebThreadImpl::TaskPair::TaskPair(const blink::WebTraceLocation& location, blink::WebThread::Task* task, long long delayMs, int priority)
{
    this->location = location;
    this->task = task;
    this->delayMs = delayMs;
    this->priority = priority;
    this->createTime = WTF::currentTime();
    this->heapInsertionOrder = WebThreadImpl::getNewCurrentHeapInsertionOrder();
}

void WebThreadImpl::TaskPair::sortByPriority(std::vector<WebThreadImpl::TaskPair*>* tasks)
{
    for (size_t i = 0; i < tasks->size(); ++i) {
        for (size_t j = i + 1; j < tasks->size(); ++j) {
            int a = tasks->at(i)->priority;
            int b = tasks->at(j)->priority;
            if (!(a < b))
                continue;

            WebThreadImpl::TaskPair* ptr = tasks->at(i);
            *(&tasks->at(i)) = tasks->at(j);
            *(&tasks->at(j)) = ptr;
        }
    }
}

void WebThreadImpl::startTriggerTasks()
{
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::freeV8TempObejctOnOneFrameBefore();
#endif
    m_currentFrameCreateTime = WTF::currentTime();
    
    while (true) {
        ::EnterCriticalSection(&m_taskPairsMutex);
        if (0 == m_taskPairsToPost.size()) {
            LeaveCriticalSection(&m_taskPairsMutex);
            break;
        }
        
        std::vector<TaskPair*> taskPairsToPostCopy = m_taskPairsToPost;
        m_taskPairsToPost.clear();
        ::LeaveCriticalSection(&m_taskPairsMutex);

        //TaskPair::sortByPriority(&taskPairsToPostCopy);

        for (size_t i = 0; i < taskPairsToPostCopy.size(); ++i) {
            TaskPair* taskPair = taskPairsToPostCopy[i];
//             if (0 == taskPair->delayMs) {
//                 willProcessTasks();
// 
//                 taskPair->task->run();
//                 delete taskPair->task;
// 
//                 didProcessTasks();
//             } else
                postDelayedTaskImpl(taskPair->location, taskPair->task, taskPair->delayMs, &taskPair->createTime, taskPair->priority, &taskPair->heapInsertionOrder);

            delete taskPair;
        }
    } 
}

bool WebThreadImpl::isCurrentThread() const 
{
    return threadId() == WTF::currentThread();
}

blink::PlatformThreadId WebThreadImpl::threadId() const
{
    return m_threadId;
}

static std::vector<WebThreadImpl::TaskObserver*>::iterator findObserver(std::vector<WebThreadImpl::TaskObserver*>& observers, WebThreadImpl::TaskObserver* observer)
{
    for (std::vector<WebThreadImpl::TaskObserver*>::iterator it = observers.begin(); it != observers.end(); ++it) {
        if (*it == observer)
            return it;
    }

    return observers.end();
}

class EmptyTask : public blink::WebThread::Task {
public:
    virtual ~EmptyTask() override {}
    virtual void run() override {};
};

void WebThreadImpl::addTaskObserver(TaskObserver* observer)
{
    if (m_observers.end() != findObserver(m_observers, observer))
        return;

    m_observers.push_back(observer);
    postTask(FROM_HERE, new EmptyTask());
}

void WebThreadImpl::removeTaskObserver(TaskObserver* observer)
{
    std::vector<WebThreadImpl::TaskObserver*>::iterator pos = findObserver(m_observers, observer);
    if (m_observers.end() != pos)
        m_observers.erase(pos);
}

void WebThreadImpl::willProcessTasks()
{
    // 有些回调，比如Microtask::enqueueMicrotask，会在退出的时候append进来，需要在最后执行，否则一些ImageLoad没法释放
    for (size_t i = 0; i < m_observers.size(); ++i)
        m_observers[i]->willProcessTask();
}

void WebThreadImpl::didProcessTasks()
{
    for (size_t i = 0; i < m_observers.size(); ++i)
        m_observers[i]->didProcessTask();
}

blink::WebScheduler* WebThreadImpl::scheduler() const
{
    return m_webSchedulerImpl;
}

void WebThreadImpl::deleteUnusedTimers()
{
    for (size_t i = 0; i < m_unusedTimersToDelete.size(); ++i) {
        WebTimerBase* timer = m_unusedTimersToDelete[i];

#if 0
        if (1 == timer->refCount()) {
            for (size_t j = 0; j < m_timerHeap.size(); ++j) {
                WebTimerBase* timerOther = m_timerHeap[j];
                if (timerOther == timer)
                    DebugBreak();
            }
        }
#endif
        timer->deref();
    }
    m_unusedTimersToDelete.clear();
}

void WebThreadImpl::deleteTimersOnExit()
{
    for (size_t i = 0; i < m_timerHeap.size(); ++i) {
        WebTimerBase* timer = m_timerHeap[i];
        timer->deref();
    }
    m_timerHeap.clear();
}

void WebThreadImpl::deleteTaskPairsToPostOnExit()
{
    for (size_t i = 0; i < m_taskPairsToPost.size(); ++i) {
        TaskPair* taskPair = m_taskPairsToPost[i];
        delete taskPair;
    }
    m_taskPairsToPost.clear();
}

void WebThreadImpl::suspendTimerQueue()
{
    m_suspendTimerQueue = true;
}

void WebThreadImpl::resumeTimerQueue()
{
    m_suspendTimerQueue = false;
}

void WebThreadImpl::fire()
{
    startTriggerTasks();
    schedulerTasks();
}

void WebThreadImpl::fireTimeOnExit()
{
    while (!m_timerHeap.empty()) {
        WebTimerBase* timer = m_timerHeap[0];
        timer->m_nextFireTime = 0;
        timer->heapDeleteMin();

        willProcessTasks();
        timer->fired(); // 可能会append m_timerHeap
        didProcessTasks();
    }
}

void WebThreadImpl::fireOnExit()
{
    startTriggerTasks();
    fireTimeOnExit();
    ASSERT(0 == m_timerHeap.size() && 0 == m_taskPairsToPost.size());

    deleteUnusedTimers();
    deleteTimersOnExit();
    deleteTaskPairsToPostOnExit();
}

void WebThreadImpl::schedulerTasks()
{
    // Do a re-entrancy check.
    if (m_firingTimers /*|| m_suspendTimerQueue*/) 
        return;
    m_firingTimers = true;

    ASSERT(m_threadId == WTF::currentThread());
    
    deleteUnusedTimers();

    double fireTime = currentTime();
    double timeToQuit = fireTime + maxDurationOfFiringTimers;

#ifdef _DEBUG
    std::vector<WebTimerBase*> dumpTimerHeap = m_timerHeap;
    for (size_t i = 0; i < dumpTimerHeap.size(); ++i) {
        if (!gActivatingTimerCheck->isActivating(dumpTimerHeap[i]))
            DebugBreak();
    }
#endif

    startTriggerTasks(); // 如果不加这句，且下面的循环在本线程不停添加定时器，则startTriggerTasks里的就没机会执行了。

    while (!m_timerHeap.empty() && (m_timerHeap[0]->m_nextFireTime <= fireTime || m_willExit)) {
        WebTimerBase* timer = m_timerHeap[0];
        timer->m_nextFireTime = 0;
        timer->heapDeleteMin();

        double interval = timer->repeatInterval();
        timer->setNextFireTime(interval ? fireTime + interval : 0, nullptr);
#ifdef _DEBUG
        size_t count = gActivatingTimerCheck->count();
        WebTimerBase* timerDump = timer;
        if (!gActivatingTimerCheck->isActivating(timer))
            DebugBreak();
#endif
        // Once the timer has been fired, it may be deleted, so do nothing else with it after this point.
        willProcessTasks();
        timer->fired();
        didProcessTasks();

        startTriggerTasks();

        // Catch the case where the timer asked timers to fire in a nested event loop, or we are over time limit.
        if (!m_firingTimers || timeToQuit < currentTime())
            break;
    }

    m_firingTimers = false;

    updateSharedTimer();
}

void WebThreadImpl::updateSharedTimer()
{
    if (m_isMainThread)
        setSharedTimerFireInterval(0.016);
}

std::vector<WebTimerBase*>& WebThreadImpl::timerHeap()
{
    return m_timerHeap;
}

} // namespace content