#ifndef WebThreadImpl_h
#define WebThreadImpl_h

#include <vector>

#include "base/compiler_specific.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"

namespace blink {

class WebTraceLocation;
class Task;

} // blink

namespace WTF {
class Mutex;
} // WTF

namespace content {

class WebSchedulerImpl;
class WebTimerBase;

class WebThreadImpl : public blink::WebThread {
public:
    explicit WebThreadImpl(const char* name);
    virtual ~WebThreadImpl();

    virtual void postTask(const blink::WebTraceLocation&, blink::WebThread::Task*) OVERRIDE;
    virtual void postDelayedTask(const blink::WebTraceLocation&, blink::WebThread::Task*, long long delayMs) OVERRIDE;

    virtual bool isCurrentThread() const OVERRIDE;
    virtual blink::PlatformThreadId threadId() const OVERRIDE;

    virtual void addTaskObserver(TaskObserver*) OVERRIDE;
    virtual void removeTaskObserver(TaskObserver*) OVERRIDE;

    // Returns the scheduler associated with the thread.
    virtual blink::WebScheduler* scheduler() const OVERRIDE;

	static void setThreadName(DWORD dwThreadID, LPCSTR szThreadName);

    void willExit();

    void startTriggerTasks();
    void schedulerTasks();

    void fire();
    
    void suspendTimerQueue();
    void resumeTimerQueue();

    std::vector<WebTimerBase*>& timerHeap();
    void updateSharedTimer() {}
    void appendUnusedTimerToDelete(WebTimerBase* timer) { m_unusedTimersToDelete.push_back(timer); }

    bool threadClosed() { return m_threadClosed; }

    void shutdown();

private:
    //void shutdown();
    void fireOnExit();
    void waitForExit();
	void fireTimeOnExit();

    struct TaskPair {
        TaskPair(const blink::WebTraceLocation& location, blink::WebThread::Task* task, long long delayMs);

        blink::WebTraceLocation location;
        blink::WebThread::Task* task;
        long long delayMs;
    };
    void postDelayedTaskImpl(const blink::WebTraceLocation& location, blink::WebThread::Task* task, long long delayMs);
    
    static unsigned __stdcall WebThreadImplThreadEntryPoint(void* param);
    void threadEntryPoint();
    void deleteUnusedTimers();
    void deleteTimersOnExit();
    void deleteTaskPairsToPostOnExit();

    void willProcessTasks();
    void didProcessTasks();

    HANDLE m_hEvent;
    blink::PlatformThreadId m_threadId;
    bool m_willExit;
    bool m_threadClosed;

    bool m_firingTimers; // Reentrancy guard.
    WebSchedulerImpl* m_webSchedulerImpl;

    // 不能用wtf的函数，否则退出后wtf被关闭了，就不能访问了
    std::vector<WebTimerBase*> m_timerHeap;
    std::vector<WebTimerBase*> m_unusedTimersToDelete;
    std::vector<TaskPair*> m_taskPairsToPost;
    std::vector<TaskObserver*> m_observers;
    const char* m_name;

    CRITICAL_SECTION m_taskPairsMutex; // weolar
    bool m_suspendTimerQueue;

    bool m_hadThreadInit;
    HANDLE m_threadHandle;
};

} // namespace content

#endif // WebThreadImpl_h