#include "common/ThreadCall.h"

#include "common/WinUserMsg.h"
#include "wke.h"
#include "base/thread.h"
#include "libplatform/libplatform.h"

namespace atom {

DWORD ThreadCall::m_blinkThreadId;
DWORD ThreadCall::m_uiThreadId;
DWORD ThreadCall::m_mainThreadId;

uv_loop_t* ThreadCall::m_uiLoop;
uv_loop_t* ThreadCall::m_blinkLoop;

v8::Platform* ThreadCall::m_v8platform = nullptr;

std::list<ThreadCall::TaskItem*>* ThreadCall::m_taskQueue[] = { 0 };
CRITICAL_SECTION ThreadCall::m_taskQueueMutex;

void ThreadCall::init(uv_loop_t* uiLoop) {
    m_uiLoop = uiLoop;
    m_uiThreadId = ::GetCurrentThreadId();

}

void ThreadCall::initTaskQueue() {
    if (!m_taskQueue[0]) {
        for (int i = 0; i < kMaxTaskQueue; ++i)
            m_taskQueue[i] = new std::list<TaskItem*>();
        ::InitializeCriticalSection(&m_taskQueueMutex);
    }
}

void ThreadCall::callUiThreadSync(std::function<void(void)>&& closure) {
    TaskAsyncData* asyncData = cretaeAsyncData(m_uiLoop, m_uiThreadId);
    asyncData->dataEx = &closure;
    asyncData->destroyThreadId = ::GetCurrentThreadId();
    callAsync(asyncData, threadCallbackWrap, asyncData);
    waitForCallThreadAsync(asyncData);
    delete asyncData;
}

void ThreadCall::threadCallbackWrap(void* data) {
    TaskAsyncData* asyncData = (TaskAsyncData*)data;
    std::function<void(void)>* closure = (std::function<void(void)>*)asyncData->dataEx;
    (*closure)();
}

void ThreadCall::asynThreadCallbackWrap(void* data) {
    TaskAsyncData* asyncData = (TaskAsyncData*)data;
    std::function<void(void)>* closure = (std::function<void(void)>*)asyncData->dataEx;
    (*closure)();
    delete closure;
}

void ThreadCall::callBlinkThreadAsync(std::function<void(void)>&& closure) {
    TaskAsyncData* asyncData = cretaeAsyncData(m_blinkLoop, m_blinkThreadId);

    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    asyncData->dataEx = closureDummy;
    callAsync(asyncData, asynThreadCallbackWrap, asyncData);
}

void ThreadCall::callUiThreadAsync(std::function<void(void)>&& closure) {
    TaskAsyncData* asyncData = cretaeAsyncData(m_uiLoop, m_uiThreadId);

    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    asyncData->dataEx = closureDummy;
    callAsync(asyncData, asynThreadCallbackWrap, asyncData);
}

void ThreadCall::callBlinkThreadSync(std::function<void(void)>&& closure) {
    TaskAsyncData* asyncData = cretaeAsyncData(m_blinkLoop, m_blinkThreadId);
    asyncData->dataEx = &closure;
    asyncData->destroyThreadId = ::GetCurrentThreadId();
    callAsync(asyncData, threadCallbackWrap, asyncData);
    waitForCallThreadAsync(asyncData);
    delete asyncData;
}

void ThreadCall::postNodeCoreThreadTask(std::function<void(void)>&& closure) {
    callUiThreadAsync(std::move(closure));
}

void ThreadCall::shutdown() {
}

bool ThreadCall::isBlinkThread() {
    return m_blinkThreadId == ::GetCurrentThreadId();
}

bool ThreadCall::isUiThread() {
    return m_uiThreadId == ::GetCurrentThreadId();
}

void ThreadCall::setMainThread() {
    m_mainThreadId = ::GetCurrentThreadId();
}

void ThreadCall::callbackInOtherThread(TaskAsyncData* asyncData) {
    if (asyncData->call) {
        asyncData->ret = nullptr;
        asyncData->call(asyncData->data);
        //::SetEvent(asyncData->event);
        asyncData->event = TRUE;
    }
}

ThreadCall::TaskQueueType ThreadCall::getWhichTypeByThreadId(DWORD idThread) {
    if (m_blinkThreadId == idThread)
        return kBlinkTaskQueue;
    if (m_uiThreadId == idThread)
        return kUiTaskQueue;
    if (m_mainThreadId == idThread)
        return kMainTaskQueue;
    return kMaxTaskQueue;
}

void ThreadCall::callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data) {
    asyncData->call = call;
    asyncData->data = data;
    postThreadMessage(asyncData->toThreadId, WM_THREAD_CALL, (WPARAM)callbackInOtherThread, (LPARAM)asyncData);
}

void* ThreadCall::waitForCallThreadAsync(TaskAsyncData* asyncData) {
    void* ret = asyncData->ret;
    DWORD waitResult = 0;
    // waitResult = ::WaitForSingleObject(asyncData->event, INFINITE); // INFINITE
    while (!asyncData->event)
        ::Sleep(3);

    return ret;
}

ThreadCall::TaskAsyncData* ThreadCall::cretaeAsyncData(uv_loop_t* loop, DWORD toThreadId) {
    TaskAsyncData* asyncData = new TaskAsyncData();
    asyncData->event = FALSE; // ::CreateEvent(NULL, FALSE, FALSE, NULL);
    asyncData->fromThreadId = ::GetCurrentThreadId();
    asyncData->toThreadId = toThreadId;
    asyncData->destroyThreadId = toThreadId;

    return asyncData;
}

void ThreadCall::exitMessageLoop(DWORD threadId) {
    if (::GetCurrentThreadId() == threadId) {
        postThreadMessage(threadId, WM_QUIT, (WPARAM)::GetCurrentThreadId(), (LPARAM)0);
        return;
    }

    bool hadExit = false;
    postThreadMessage(threadId, WM_QUIT, (WPARAM)::GetCurrentThreadId(), (LPARAM)&hadExit);
    while (!hadExit) {
        ::Sleep(20);
    }
}

void ThreadCall::postThreadMessage(DWORD idThread, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (WM_QUIT == msg) {
        ::PostThreadMessage(idThread, msg, wParam, lParam);
        return;
    }

    TaskQueueType type = getWhichTypeByThreadId(idThread);
    if (kMaxTaskQueue == type)
        return;

    ::EnterCriticalSection(&m_taskQueueMutex);
    m_taskQueue[type]->push_back(new TaskItem(idThread, msg, wParam, lParam));
    ::LeaveCriticalSection(&m_taskQueueMutex);
}

void ThreadCall::doTaskQueue(DWORD threadId) {
    TaskQueueType type = getWhichTypeByThreadId(threadId);
    TaskItem* it = nullptr;
    ::EnterCriticalSection(&m_taskQueueMutex);
    if (m_taskQueue[type] && 0 != m_taskQueue[type]->size()) {
        it = m_taskQueue[type]->front();
        m_taskQueue[type]->pop_front();
    }
    ::LeaveCriticalSection(&m_taskQueueMutex);
    if (!it)
        return;

    runTaskQueue(it->msg, it->wParam, it->lParam);
    delete it;
}

void ThreadCall::runTaskQueue(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (WM_THREAD_CALL != msg)
        return;

    if (wParam != (WPARAM)callbackInOtherThread)
        DebugBreak();
    TaskAsyncData* asyncData = (TaskAsyncData*)lParam;
    callbackInOtherThread(asyncData);

    if (asyncData->destroyThreadId == ::GetCurrentThreadId())
        delete asyncData;
    else if (asyncData->destroyThreadId != asyncData->fromThreadId)
        DebugBreak();
}

void ThreadCall::messageLoop(uv_loop_t* loop, v8::Platform* platform, v8::Isolate* isolate) {
    MSG msg;
    bool more = false;

    DWORD threadId = ::GetCurrentThreadId();
    
    while (true) {
        if (loop)
            more = (0 != uv_run(loop, UV_RUN_NOWAIT));

        if (platform && isolate)
            v8::platform::PumpMessageLoop(platform, isolate);

        doTaskQueue(threadId);
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message) {
                if (0 != msg.lParam)
                    *((bool*)(msg.lParam)) = true;
                else 
                    OutputDebugStringA("ThreadCall::messageLoop\n");
                break;
            }
            
            //runTaskQueue(msg.message, msg.wParam, msg.lParam);
            if (WM_THREAD_CALL == msg.message)
                DebugBreak();
            
            
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        } else {
            ::Sleep(2);
        }
    }

    if (::GetCurrentThreadId() == m_uiThreadId)
        exitMessageLoop(m_mainThreadId);
}

void ThreadCall::blinkThread(void* created) {
    base::SetThreadName("BlinkCore");

    m_blinkThreadId = ::GetCurrentThreadId();
    m_blinkLoop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
    int err = uv_loop_init(m_blinkLoop);
    
    wkeInitialize();
    *(bool*)created = true;

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    messageLoop(m_blinkLoop, m_v8platform, isolate);

    wkeFinalize();

    free(m_blinkLoop);
    m_blinkLoop = nullptr;
}

void ThreadCall::createBlinkThread(v8::Platform* v8platform) {
    m_v8platform = v8platform;
    bool created = false;
    uv_thread_t tid;
    uv_thread_create(&tid, reinterpret_cast<uv_thread_cb>(blinkThread), &created);
    while (!created) { ::Sleep(20); }
}

} // atom