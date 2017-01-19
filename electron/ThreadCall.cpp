
#include "ThreadCall.h"

#include "wke.h"
#include "base/thread.h"

namespace atom {

DWORD ThreadCall::m_blinkThreadId;
DWORD ThreadCall::m_uiThreadId;

uv_loop_t* ThreadCall::m_uiLoop;
uv_loop_t* ThreadCall::m_blinkLoop;

#define WM_THREAD_CALL (WM_USER + 0x5325)

void ThreadCall::init(uv_loop_t* uiLoop) {
    m_uiLoop = uiLoop;
    m_uiThreadId = ::GetCurrentThreadId();
    createBlinkThread();
}

void* ThreadCall::callUiThreadSync(CoreMainTask call, void* data) {
    if (::GetCurrentThreadId() == m_uiThreadId) {
        return call(data);
    }

    TaskAsyncData* asyncData = initAsyncData(m_uiLoop, m_uiThreadId);
    callAsync(asyncData, call, data);
    void* ret = waitForCallThreadAsync(asyncData);
    delete asyncData;

    return ret;
}

void ThreadCall::callUiThreadSync(std::function<void(void)> closure) {
    if (::GetCurrentThreadId() == m_uiThreadId) {
        closure();
        return;
    }

    TaskAsyncData* asyncData = initAsyncData(m_uiLoop, m_uiThreadId);
    asyncData->dataEx = &closure;
    callAsync(asyncData, threadCallbackWrap, asyncData);
    waitForCallThreadAsync(asyncData);
    delete asyncData;
}

void* ThreadCall::threadCallbackWrap(void* data) {
    TaskAsyncData* asyncData = (TaskAsyncData*)data;
    std::function<void(void)>* closure = (std::function<void(void)>*)asyncData->dataEx;
    (*closure)();
    return nullptr;
}

void ThreadCall::callBlinkThreadSync(std::function<void(void)> closure) {
    if (::GetCurrentThreadId() == m_blinkThreadId) {
        closure();
        return;
    }

    TaskAsyncData* asyncData = initAsyncData(m_blinkLoop, m_blinkThreadId);
    asyncData->dataEx = &closure;
    callAsync(asyncData, threadCallbackWrap, asyncData);
    waitForCallThreadAsync(asyncData);
    delete asyncData;
}

void ThreadCall::shutdown() {
}

void ThreadCall::callbackInThread(uv_async_t* handle) {
    TaskAsyncData* asyncData = (TaskAsyncData*)handle;
    if (asyncData->call) {
        asyncData->ret = asyncData->call(asyncData->data);
        ::SetEvent(asyncData->event);
    }
}

void ThreadCall::callbackInOtherThread(TaskAsyncData* asyncData) {
    if (asyncData->call) {
        asyncData->ret = asyncData->call(asyncData->data);
        ::SetEvent(asyncData->event);
    }
}

void ThreadCall::callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data) {
    asyncData->call = call;
    asyncData->data = data;
}

void* ThreadCall::waitForCallThreadAsync(TaskAsyncData* asyncData) {
    void* ret = asyncData->ret;
    DWORD waitResult = 0;
    //do {
        ::PostThreadMessage(asyncData->toThreadId, WM_THREAD_CALL, (WPARAM)callbackInOtherThread, (LPARAM)asyncData);
        waitResult = ::WaitForSingleObject(asyncData->event, INFINITE); // INFINITE
    //} while (WAIT_TIMEOUT == waitResult || WAIT_FAILED == waitResult);

    return ret;
}

ThreadCall::TaskAsyncData* ThreadCall::initAsyncData(uv_loop_t* loop, DWORD toThreadId) {
    TaskAsyncData* asyncData = new TaskAsyncData();
    asyncData->event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    asyncData->fromThreadId = ::GetCurrentThreadId();
    asyncData->toThreadId = toThreadId;

    return asyncData;
}

void ThreadCall::messageLoop(uv_loop_t* loop) {
    MSG msg;
    bool more;
    while (true) {
        more = (0 != uv_run(loop, UV_RUN_NOWAIT));
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message)
                break;
            if (WM_THREAD_CALL == msg.message) {
                if (msg.wParam != (WPARAM)callbackInOtherThread)
                    DebugBreak();
                callbackInOtherThread((TaskAsyncData*)msg.lParam);
            }
            
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        } else {
            ::Sleep(20);
        }
    }
}

void ThreadCall::blinkThread(void* created) {
    base::SetThreadName("BlinkCore");

    m_blinkThreadId = ::GetCurrentThreadId();
    m_blinkLoop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
    int err = uv_loop_init(m_blinkLoop);
    
    wkeInitialize();
    *(bool*)created = true;

    messageLoop(m_blinkLoop);

    free(m_blinkLoop);
    m_blinkLoop = nullptr;
}

void ThreadCall::createBlinkThread() {
    bool created = false;
    uv_thread_t tid;
    uv_thread_create(&tid, reinterpret_cast<uv_thread_cb>(blinkThread), &created);
    while (!created) { ::Sleep(20); }
}

} // atom