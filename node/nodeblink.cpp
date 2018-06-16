
#define USING_UV_SHARED 1

#include "nodeblink.h"

#include "gin/v8_initializer.h"
#include "libplatform/libplatform.h"

#include "third_party/WebKit/Source/config.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8RecursionScope.h"

#include <string.h>
#include <Windows.h>
#include <process.h>

#pragma comment(lib,"openssl.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Psapi.lib")

namespace node {

// static void childSignalCallback(uv_async_t* signal) {
// 
// }
// 
// static void workerRun(NodeArgc* nodeArgc) {
//     int err = uv_loop_init(nodeArgc->childLoop);
//     if (err != 0)
//         goto loop_init_failed;
// 
//     // Interruption signal handler
//     err = uv_async_init(nodeArgc->childLoop, &nodeArgc->async, childSignalCallback);
//     if (err != 0)
//         goto async_init_failed;
// 
//     //uv_unref(reinterpret_cast<uv_handle_t*>(&nodeArgc->async)); //zero 不屏蔽此句导致loop循环退出
//     nodeArgc->initType = true;
//     ::SetEvent(nodeArgc->initEvent);
// 
//     {
//         if (nodeArgc->preInitcall)
//             nodeArgc->preInitcall(nodeArgc);
// 
//         v8::Isolate::CreateParams params;
//         node::ArrayBufferAllocator array_buffer_allocator;
//         params.array_buffer_allocator = &array_buffer_allocator;
//         v8::Isolate *isolate = v8::Isolate::New(params);
//         v8::Isolate::Scope isolate_scope(isolate);
//         {
//             v8::HandleScope handle_scope(isolate);
//             v8::Local<v8::Context> context = v8::Context::New(isolate);
// 
//             v8::Context::Scope context_scope(context);
//             nodeArgc->childEnv = node::CreateEnvironment(isolate, nodeArgc->childLoop, context, nodeArgc->argc, nodeArgc->argv, nodeArgc->argc, nodeArgc->argv);
// 
//             // Expose API
//             LoadEnvironment(nodeArgc->childEnv);
// 
//             if (nodeArgc->initcall)
//                 nodeArgc->initcall(nodeArgc);
//             CHECK_EQ(nodeArgc->childLoop, nodeArgc->childEnv->event_loop());
//             //uv_run(nodeArgc->childLoop, UV_RUN_DEFAULT); // 由nodeArgc->initcall里负责消息循环
//         }
//         // Clean-up all running handles
//         nodeArgc->childEnv->CleanupHandles();
// 
//         nodeArgc->childEnv->Dispose();
//         nodeArgc->childEnv = nullptr;
// 
//         isolate->Dispose();
// 
//         delete nodeArgc->v8platform;
//         nodeArgc->v8platform = nullptr;
//     }
//     return;
// 
// async_init_failed:
//     err = uv_loop_close(nodeArgc->childLoop);
//     CHECK_EQ(err, 0);
// loop_init_failed:
//     free(nodeArgc);
//     nodeArgc->initType = false;
//     ::SetEvent(nodeArgc->initEvent);
// }
// 
// extern "C" NODE_EXTERN NodeArgc* nodeRunThread(int argc, const wchar_t *wargv[], NodeInitCallBack initcall, NodeInitCallBack preInitcall, void *data) {
//     NodeArgc* nodeArgc = (NodeArgc *)malloc(sizeof(NodeArgc));
//     memset(nodeArgc, 0, sizeof(NodeArgc));
//     nodeArgc->initcall = initcall;
//     nodeArgc->preInitcall = preInitcall;
//     nodeArgc->data = data;
//     nodeArgc->childLoop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
//     nodeArgc->argv = new char*[argc + 1];
//     for (int i = 0; i < argc; i++) {
//         // Compute the size of the required buffer
//         DWORD size = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
//         if (size == 0)
//             ::DebugBreak();
//         
//         // Do the actual conversion
//         nodeArgc->argv[i] = new char[size];
//         DWORD result = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nodeArgc->argv[i], size, nullptr, nullptr);
//         if (result == 0)
//             ::DebugBreak();
//     }
//     nodeArgc->argv[argc] = nullptr;
//     nodeArgc->argc = argc;
// 
//     nodeArgc->v8platform = v8::platform::CreateDefaultPlatform(4);
//     gin::V8Initializer::SetV8Platform(nodeArgc->v8platform);
// 
//     nodeArgc->initEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL); // 创建一个对象,用来等待node环境基础环境创建成功
//     int err = uv_thread_create(&nodeArgc->thread, reinterpret_cast<uv_thread_cb>(workerRun), nodeArgc);
//     if (err != 0)
//         goto thread_create_failed;
//     ::WaitForSingleObject(nodeArgc->initEvent, INFINITE);
//     CloseHandle(nodeArgc->initEvent);
//     nodeArgc->initEvent = NULL;
//     if (!nodeArgc->initType)
//         goto thread_init_failed;
//     return nodeArgc;
// 
// thread_init_failed:
// 
// thread_create_failed:
//     free(nodeArgc);
//     return nullptr;
// }
// 
// extern "C" NODE_EXTERN Environment* nodeGetEnvironment(NodeArgc* nodeArgc) {
//     if (nodeArgc)
//         return nodeArgc->childEnv;
//     return nullptr;
// }

} // node

extern "C" NODE_EXTERN void* nodeCreateDefaultPlatform() {
    v8::Platform* v8platform = v8::platform::CreateDefaultPlatform(4);
    gin::V8Initializer::SetV8Platform(v8platform);
    return v8platform;
}

static void waitForEnvironmentHandleWrapQueue(node::Environment* env) {
    while (true) {
        uv_run(env->event_loop(), UV_RUN_NOWAIT);

        int emptyCount = 0;
        int allCount = 0;
        node::Environment::HandleWrapQueue::Iterator it = env->handle_wrap_queue()->begin();
        for (; it != env->handle_wrap_queue()->end(); ++it, ++allCount) {
            v8::Local<v8::Object> obj = (*it)->object();
            bool b = obj.IsEmpty();
            if (b)
                ++emptyCount;
        }
        if (emptyCount == allCount)
            break;
    }
}

static void handleCloseCb(uv_handle_t* handle) {
    int* closingHandleCount = (int*)handle->data;
    ++(*closingHandleCount);
}

static void closeWalkCB(uv_handle_t* handle, void* arg) {
    int* closeHandleCount = (int*)arg;

    if (!uv_is_closing(handle)) {
        ++(*closeHandleCount);
        handle->data = closeHandleCount + 1;
        uv_close(handle, handleCloseCb);
    }
}

static void waitForAllHandleWrapQueue(node::Environment* env) {
    int closeHandleCount[2] = { 0, 0 };
    uv_walk(env->event_loop(), closeWalkCB, &closeHandleCount);

    while (closeHandleCount[0] != closeHandleCount[1])
        uv_run(env->event_loop(), UV_RUN_NOWAIT);
}

extern "C" NODE_EXTERN void nodeDeleteNodeEnvironment(node::Environment* env) {
    env->CleanupHandles();
    waitForEnvironmentHandleWrapQueue(env);
    waitForAllHandleWrapQueue(env);

    env->Dispose();
}

extern "C" NODE_EXTERN BlinkMicrotaskSuppressionHandle nodeBlinkMicrotaskSuppressionEnter(v8::Isolate* isolate) {
    return new blink::V8RecursionScope::MicrotaskSuppression(isolate);
}

extern "C" NODE_EXTERN void nodeBlinkMicrotaskSuppressionLeave(BlinkMicrotaskSuppressionHandle handle) {
    blink::V8RecursionScope::MicrotaskSuppression* suppression = (blink::V8RecursionScope::MicrotaskSuppression*)handle;
    delete suppression;
}

extern "C" NODE_EXTERN void* nodeBlinkAllocateUninitialized(size_t length) {
    return node::Malloc(length);
}

extern "C" NODE_EXTERN void nodeBlinkFree(void* data, size_t length) {
    node::Realloc(data, 0);
}

