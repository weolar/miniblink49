
#include "ThreadCall.h"

#include "wke.h"

namespace atom {

ThreadCall::TaskAsyncData* ThreadCall::m_uiThreadAsyncData;
ThreadCall::TaskAsyncData* ThreadCall::m_blinkThreadAsyncData;

void ThreadCall::init(uv_loop_t* loop) {
    initData(&m_uiThreadAsyncData, loop);
    initData(&m_blinkThreadAsyncData, loop);

    createBlinkThread();
}

void ThreadCall::callUiThreadAsync(CoreMainTask call, void* data) {
    callAsync(m_uiThreadAsyncData, call, data);
}

void* ThreadCall::waitForCallUiThreadAsync() {
    return waitForCallThreadAsync(m_uiThreadAsyncData);
}

void* ThreadCall::callUiThreadSync(CoreMainTask call, void* data) {
    callUiThreadAsync(call, data);
    void* ret = waitForCallUiThreadAsync();
    return ret;
}

bool ThreadCall::callUiThreadSync(v8::FunctionCallback call, const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (uv_thread_self() == m_uiThreadAsyncData->main_thread_id)
        return false;
    callUiThreadAsync((CoreMainTask)call, (void *)&args);
    waitForCallUiThreadAsync();
    return true;
}

static void* blinkThreadCallbackWrap(void* data) {
    std::function<void(void)>* closure = (std::function<void(void)>*)data;
    (*closure)();
    return nullptr;
}

void ThreadCall::callBlinkThreadSync(std::function<void(void)> closure) {
    if (uv_thread_self() == m_blinkThreadAsyncData->main_thread_id) {
        closure();
        return;
    }

    callAsync(m_blinkThreadAsyncData, blinkThreadCallbackWrap, &closure);
    waitForCallThreadAsync(m_blinkThreadAsyncData);
}

void ThreadCall::shutdown() {
    delete m_uiThreadAsyncData;
    delete m_blinkThreadAsyncData;
}

void ThreadCall::callbackInThread(uv_async_t* handle) {
    TaskAsyncData* asyncData = (TaskAsyncData*)handle;
    if (asyncData->call) {
        asyncData->ret = asyncData->call(asyncData->data);
        ::PulseEvent(asyncData->event);
    }
}

void ThreadCall::callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data) {
    uv_mutex_lock(&asyncData->mutex);
    asyncData->call = call;
    asyncData->data = data;
    uv_async_send((uv_async_t*)asyncData);
}

void* ThreadCall::waitForCallThreadAsync(TaskAsyncData* asyncData) {
    void* ret = asyncData->ret;
    ::WaitForSingleObject(asyncData->event, INFINITE);
    uv_mutex_unlock(&asyncData->mutex);
    return ret;
}

void ThreadCall::initData(TaskAsyncData** asyncData, uv_loop_t* loop) {
    (*asyncData) = new TaskAsyncData();
    (*asyncData)->event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    uv_async_init(loop, (uv_async_t*)(*asyncData), callbackInThread);
    uv_mutex_init(&((*asyncData)->mutex));
    (*asyncData)->main_thread_id = uv_thread_self();
}

void ThreadCall::blinkThread(void* created) {
    uv_loop_t* childLoop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
    int err = uv_loop_init(childLoop);

    wkeInitialize();

    *(bool*)created = true;

    uv_run(childLoop, UV_RUN_DEFAULT);
}

void ThreadCall::createBlinkThread() {
    bool created = false;
    uv_thread_t tid;
    uv_thread_create(&tid, reinterpret_cast<uv_thread_cb>(blinkThread), &created);
    while (!created) { ::Sleep(20); }
}

} // atom