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
    int err = uv_loop_init(nodeArgc->childLoop);
    if (err != 0)
        goto loop_init_failed;

    // Interruption signal handler
    err = uv_async_init(nodeArgc->childLoop, &nodeArgc->async, childSignalCallback);
    if (err != 0)
        goto async_init_failed;
    //uv_unref(reinterpret_cast<uv_handle_t*>(&nodeArgc->async)); //zero 不屏蔽此句导致loop循环退出
    nodeArgc->initType = true;
    ::PulseEvent(nodeArgc->initEvent);

    {
        v8::Isolate::CreateParams params;
        node::ArrayBufferAllocator array_buffer_allocator;
        params.array_buffer_allocator = &array_buffer_allocator;
        v8::Isolate *isolate = v8::Isolate::New(params);
        v8::Isolate::Scope isolate_scope(isolate);
        {
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::Context> context = v8::Context::New(isolate);

            v8::Context::Scope context_scope(context);
            nodeArgc->childEnv = node::CreateEnvironment(isolate, nodeArgc->childLoop, context, nodeArgc->argc, nodeArgc->argv, nodeArgc->argc, nodeArgc->argv);

            // Expose API
            LoadEnvironment(nodeArgc->childEnv);

            if (nodeArgc->initcall)
                nodeArgc->initcall(nodeArgc);
            CHECK_EQ(nodeArgc->childLoop, nodeArgc->childEnv->event_loop());
            uv_run(nodeArgc->childLoop, UV_RUN_DEFAULT);
        }
        // Clean-up all running handles
        nodeArgc->childEnv->CleanupHandles();

        nodeArgc->childEnv->Dispose();
        nodeArgc->childEnv = nullptr;

        isolate->Dispose();
    }
    return;

async_init_failed:
    err = uv_loop_close(nodeArgc->childLoop);
    CHECK_EQ(err, 0);
loop_init_failed:
    free(nodeArgc);
    nodeArgc->initType = false;
    ::PulseEvent(nodeArgc->initEvent);
}

extern "C" NODE_EXTERN NodeArgc* runNodeThread(int argc, wchar_t *wargv[], NodeInitCallBack initcall, void *data) {
    NodeArgc* nodeArgc = (NodeArgc *)malloc(sizeof(NodeArgc));
    memset(nodeArgc, 0, sizeof(NodeArgc));
    nodeArgc->initcall = initcall;
    nodeArgc->data = data;
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

    nodeArgc->initEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);//创建一个对象,用来等待node环境基础环境创建成功
    int err = uv_thread_create(&nodeArgc->thread, reinterpret_cast<uv_thread_cb>(workerRun), nodeArgc);
    if (err != 0)
        goto thread_create_failed;
    ::WaitForSingleObject(nodeArgc->initEvent, INFINITE);
    CloseHandle(nodeArgc->initEvent);
    nodeArgc->initEvent = NULL;
    if (!nodeArgc->initType)
        goto thread_init_failed;
    return nodeArgc;

thread_init_failed:

thread_create_failed:
    free(nodeArgc);
    return nullptr;
}

extern "C" NODE_EXTERN Environment* nodeGetEnvironment(NodeArgc* nodeArgc) {
    if (nodeArgc)
        return nodeArgc->childEnv;
    return nullptr;
}

} // node