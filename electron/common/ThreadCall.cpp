
#include "ThreadCall.h"

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

void ThreadCall::init(uv_loop_t* uiLoop) {
    m_uiLoop = uiLoop;
    m_uiThreadId = ::GetCurrentThreadId();
}

void ThreadCall::callUiThreadSync(std::function<void(void)>&& closure) {
//     if (isUiThread()) {
//         closure();
//         return;
//     }

    TaskAsyncData* asyncData = cretaeAsyncData(m_uiLoop, m_uiThreadId);
    asyncData->dataEx = &closure;
    callAsync(asyncData, threadCallbackWrap, asyncData);
    waitForCallThreadAsync(asyncData);
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
//     if (isBlinkThread()) {
//         closure();
//         return;
//     }

    TaskAsyncData* asyncData = cretaeAsyncData(m_blinkLoop, m_blinkThreadId);
    asyncData->dataEx = &closure;
    callAsync(asyncData, threadCallbackWrap, asyncData);
    waitForCallThreadAsync(asyncData);
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
        ::SetEvent(asyncData->event);
    }
}

void ThreadCall::callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data) {
    asyncData->call = call;
    asyncData->data = data;
    ::PostThreadMessage(asyncData->toThreadId, WM_THREAD_CALL, (WPARAM)callbackInOtherThread, (LPARAM)asyncData);
}

void* ThreadCall::waitForCallThreadAsync(TaskAsyncData* asyncData) {
    void* ret = asyncData->ret;
    DWORD waitResult = 0;
    //do {
        waitResult = ::WaitForSingleObject(asyncData->event, INFINITE); // INFINITE
    //} while (WAIT_TIMEOUT == waitResult || WAIT_FAILED == waitResult);

    return ret;
}

ThreadCall::TaskAsyncData* ThreadCall::cretaeAsyncData(uv_loop_t* loop, DWORD toThreadId) {
    TaskAsyncData* asyncData = new TaskAsyncData();
    asyncData->event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    asyncData->fromThreadId = ::GetCurrentThreadId();
    asyncData->toThreadId = toThreadId;

    return asyncData;
}

void ThreadCall::exitMessageLoop(DWORD threadId) {
    if (::GetCurrentThreadId() == threadId) {
        ::PostThreadMessage(threadId, WM_QUIT, (WPARAM)::GetCurrentThreadId(), (LPARAM)0);
        return;
    }

    bool hadExit = false;
    ::PostThreadMessage(threadId, WM_QUIT, (WPARAM)::GetCurrentThreadId(), (LPARAM)&hadExit);
    while (!hadExit) {
        ::Sleep(20);
    }
}

void ThreadCall::messageLoop(uv_loop_t* loop, v8::Platform* platform, v8::Isolate* isolate) {
    MSG msg;
    bool more;
    
    while (true) {
        more = (0 != uv_run(loop, UV_RUN_NOWAIT));
        if (platform && isolate)
            v8::platform::PumpMessageLoop(platform, isolate);
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message) {
                if (0 != msg.lParam)
                    *((bool*)(msg.lParam)) = true;
                else 
                    OutputDebugStringA("ThreadCall::messageLoop\n");
                break;
            }
            
            if (WM_THREAD_CALL == msg.message) {
                if (msg.wParam != (WPARAM)callbackInOtherThread)
                    DebugBreak();
                TaskAsyncData* data = (TaskAsyncData*)msg.lParam;
                callbackInOtherThread(data);
                delete data;
            }
            
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        } else {
            ::Sleep(20);
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