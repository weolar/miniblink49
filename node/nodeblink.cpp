#include "include/nodeblink.h"

#include <string.h>
#include <Windows.h>
#include <process.h>

#pragma comment(lib,"openssl.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Psapi.lib")

namespace node {

static void childSignalCallback(uv_async_t* signal) {

}

static void workerRun(NodeArgc* nodeArgc) {
    v8::Isolate::CreateParams params;
    node::ArrayBufferAllocator array_buffer_allocator;
    params.array_buffer_allocator = &array_buffer_allocator;
    v8::Isolate* isolate = v8::Isolate::New(params);
    
    v8::Isolate::Scope isolate_scope(isolate);

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate);

    v8::Context::Scope context_scope(context);
    node::Environment* env = node::CreateEnvironment(isolate, nodeArgc->childLoop, context, nodeArgc->argc, nodeArgc->argv, nodeArgc->argc, nodeArgc->argv);

    nodeArgc->childEnv = env;

    // Expose API
    LoadEnvironment(env);

    if (nodeArgc->initcall)
        nodeArgc->initcall(nodeArgc);
    CHECK_EQ(nodeArgc->childLoop, env->event_loop());
    uv_run(nodeArgc->childLoop, UV_RUN_DEFAULT);

    // Clean-up all running handles
    env->CleanupHandles();

    env->Dispose();
    env = nullptr;
    
    isolate->Dispose();
}

extern "C" NODE_EXTERN NodeArgc* runNodeThread(int argc, wchar_t *wargv[], NodeInitCallBack initcall) {
    NodeArgc* nodeArgc = (NodeArgc *)malloc(sizeof(NodeArgc));
    memset(nodeArgc, 0, sizeof(NodeArgc));
    nodeArgc->initcall = initcall;
    nodeArgc->childLoop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
    nodeArgc->argv = new char*[argc + 1];
    for (int i = 0; i < argc; i++) {
        // Compute the size of the required buffer
        DWORD size = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
        if (size == 0)
            ::DebugBreak();
        
        // Do the actual conversion
        nodeArgc->argv[i] = new char[size];
        DWORD result = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nodeArgc->argv[i], size, nullptr, nullptr);
        if (result == 0)
            ::DebugBreak();
    }
    nodeArgc->argv[argc] = nullptr;
    nodeArgc->argc = argc;

    uv_async_t childSignal;
    uv_thread_t thread;
    int err = uv_loop_init(nodeArgc->childLoop);
    if (err != 0)
        goto loop_init_failed;

    // Interruption signal handler
    err = uv_async_init(nodeArgc->childLoop, &childSignal, childSignalCallback);
    if (err != 0)
        goto async_init_failed;
    uv_unref(reinterpret_cast<uv_handle_t*>(&childSignal));

    err = uv_thread_create(&thread, reinterpret_cast<uv_thread_cb>(workerRun), nodeArgc);
    if (err != 0)
        goto thread_create_failed;

    return nodeArgc;

thread_create_failed:
    uv_close(reinterpret_cast<uv_handle_t*>(&childSignal), nullptr);

async_init_failed:
    err = uv_loop_close(nodeArgc->childLoop);
    CHECK_EQ(err, 0);

loop_init_failed:
    free(nodeArgc);
    return nullptr;
}

extern "C" NODE_EXTERN Environment* nodeGetEnvironment(NodeArgc* nodeArgc) {
    if (nodeArgc)
        return nodeArgc->childEnv;
    return nullptr;
}

} // node