
#include "ThreadCall.h"

#include "wke.h"
#include "base/thread.h"

namespace atom {

DWORD ThreadCall::m_blinkThreadId;
DWORD ThreadCall::m_uiThreadId;

uv_loop_t* ThreadCall::m_uiLoop;
uv_loop_t* ThreadCall::m_blinkLoop;

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

// bool ThreadCall::callUiThreadSync(v8::FunctionCallback call, const v8::FunctionCallbackInfo<v8::Value>& args) {
//     if (::GetCurrentThreadId() == m_uiThreadId) {
//         call(args);
//         return true;
//     }
// 
//     TaskAsyncData* asyncData = initAsyncData(m_uiLoop, m_uiThreadId);
//     callAsync(asyncData, (CoreMainTask)call, (void *)&args);
//     waitForCallThreadAsync(asyncData);
//     delete asyncData;
// 
//     return true;
// }

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
//     delete m_uiThreadAsyncData;
//     delete m_blinkThreadAsyncData;
}

void ThreadCall::callbackInThread(uv_async_t* handle) {
    TaskAsyncData* asyncData = (TaskAsyncData*)handle;
    if (asyncData->call) {
        asyncData->ret = asyncData->call(asyncData->data);
        ::PulseEvent(asyncData->event);
    }
}

void ThreadCall::callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data) {
    //uv_mutex_lock(&asyncData->mutex);
    asyncData->call = call;
    asyncData->data = data;
    uv_async_send((uv_async_t*)asyncData);
}

void* ThreadCall::waitForCallThreadAsync(TaskAsyncData* asyncData) {
    void* ret = asyncData->ret;
    DWORD waitResult = 0;
    do {
        ::PostThreadMessage(asyncData->toThreadId, WM_NULL, 0, 0);
        waitResult = ::WaitForSingleObject(asyncData->event, 10); // INFINITE
    } while (WAIT_TIMEOUT == waitResult || WAIT_FAILED == waitResult);

    //uv_mutex_unlock(&asyncData->mutex);
    return ret;
}

ThreadCall::TaskAsyncData* ThreadCall::initAsyncData(uv_loop_t* loop, DWORD toThreadId) {
    TaskAsyncData* asyncData = new TaskAsyncData();
    asyncData->event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    uv_async_init(loop, (uv_async_t*)asyncData, callbackInThread);
    uv_mutex_init(&(asyncData->mutex));
    asyncData->fromThreadId = ::GetCurrentThreadId();
    asyncData->toThreadId = toThreadId;

    return asyncData;
}

void ThreadCall::messageLoop(uv_loop_t* loop) {
    MSG msg;
    bool more;
    while (true) {
        more = (0 != uv_run(loop, UV_RUN_NOWAIT));
        //if (::GetMessageW(&msg, NULL, 0, 0)) {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message)
                break;
            
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        } else {
            ::Sleep(20);
            //break;
        }
    }
}

void ThreadCall::blinkThread(void* created) {
    base::SetThreadName("BlinkCore");

    uv_loop_t* loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
    int err = uv_loop_init(loop);

    m_blinkThreadId = ::GetCurrentThreadId();
    m_blinkLoop = loop;
    
    wkeInitialize();
    *(bool*)created = true;

    messageLoop(loop);

    free(loop);
}

void ThreadCall::createBlinkThread() {
    bool created = false;
    uv_thread_t tid;
    uv_thread_create(&tid, reinterpret_cast<uv_thread_cb>(blinkThread), &created);
    while (!created) { ::Sleep(20); }
}

} // atom