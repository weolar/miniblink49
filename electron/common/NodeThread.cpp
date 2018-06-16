#include "NodeThread.h"

#include "node/nodeblink.h"
#include "gin/v8_initializer.h"
#include "libplatform/libplatform.h"
#include "base/thread.h"
#include "common/ThreadCall.h"
#include "common/NodeBinding.h"
#include "third_party/zlib/unzip.h"
#include <string.h>
#include <Windows.h>
#include <process.h>
#include <objbase.h>

namespace atom {

static void childSignalCallback(uv_async_t* signal) {

}

class ElectronFsHooks : public node::Environment::FileSystemHooks {
    virtual bool internalModuleStat(const char* path, int *rc) override {
        *rc = 1;
        return false;
    }

    void open() {

    }
};

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    ArrayBufferAllocator() { }

    virtual void* Allocate(size_t size) {
        void* p = AllocateUninitialized(size);
        memset(p, 0, size);
        return p;
    }

    virtual void* AllocateUninitialized(size_t size) {
        if (!ThreadCall::isUiThread())
            DebugBreak();
        return nodeBlinkAllocateUninitialized(size);
    }

    virtual void Free(void* data, size_t size) { nodeBlinkFree(data, size); }
};

static void gcTimerCallBack(uv_timer_t* handle) {
    return;

    v8::Isolate *isolate = (v8::Isolate*)(handle->data);
    if (isolate)
        isolate->LowMemoryNotification();
}

void messageLoop(NodeArgc* n) {
    atom::ThreadCall::messageLoop(n->childLoop, n->v8platform, v8::Isolate::GetCurrent());
}

void initThread(NodeArgc* n) {
//     uv_timer_t gcTimer;
//     gcTimer.data = n->childEnv->isolate();
//     uv_timer_init(n->childLoop, &gcTimer);
//     uv_timer_start(&gcTimer, gcTimerCallBack, 1000 * 10, 1);

    uv_loop_t* loop = n->childLoop;
    atom::ThreadCall::init(loop);
}

static v8::Isolate* initNodeEnv(NodeArgc* nodeArgc) {
    v8::Isolate::CreateParams params;
    params.array_buffer_allocator = new ArrayBufferAllocator();
    v8::Isolate *isolate = v8::Isolate::New(params);

    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    v8::Context::Scope contextScope(context);
    nodeArgc->childEnv = nodeArgc->m_nodeBinding->createEnvironment(context);

    initThread(nodeArgc);

    ElectronFsHooks fsHooks;
    nodeArgc->childEnv->file_system_hooks(&fsHooks);

    NodeBindings::initNodeEnv();

    nodeArgc->m_nodeBinding->loadEnvironment();

    messageLoop(nodeArgc);

    return isolate;
}

static void workerRun(NodeArgc* nodeArgc) {
    int err = uv_loop_init(nodeArgc->childLoop);
    if (err != 0)
        goto loop_init_failed;

    uv_default_loop();

    ::OleInitialize(nullptr);

    // Interruption signal handler
    err = uv_async_init(nodeArgc->childLoop, &nodeArgc->async, childSignalCallback);
    if (err != 0)
        goto async_init_failed;
    //uv_unref(reinterpret_cast<uv_handle_t*>(&nodeArgc->async)); //zero 不屏蔽此句导致loop循环退出
    nodeArgc->initType = true;
    ::SetEvent(nodeArgc->initEvent);

    nodeArgc->v8platform = (v8::Platform*)nodeCreateDefaultPlatform();

    base::SetThreadName("NodeCore");
    ThreadCall::createBlinkThread(nodeArgc->v8platform);

    v8::Isolate* isolate = initNodeEnv(nodeArgc);

    // Clean-up all running handles
    nodeArgc->childEnv->CleanupHandles();

    nodeArgc->childEnv->Dispose();
    nodeArgc->childEnv = nullptr;

    isolate->Dispose();
    
    return;

async_init_failed:
    err = uv_loop_close(nodeArgc->childLoop);
    CHECK_EQ(err, 0);

loop_init_failed:
    free(nodeArgc);
    nodeArgc->initType = false;
    ::SetEvent(nodeArgc->initEvent);
}

NodeArgc* runNodeThread() {
    NodeArgc* nodeArgc = (NodeArgc *)malloc(sizeof(NodeArgc));
    memset(nodeArgc, 0, sizeof(NodeArgc));
    nodeArgc->childLoop = (uv_loop_t *)malloc(sizeof(uv_loop_t));

    nodeArgc->m_nodeBinding = new NodeBindings(true, nodeArgc->childLoop);

    nodeArgc->initEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL); // 创建一个对象,用来等待node环境基础环境创建成功
    int err = uv_thread_create(&nodeArgc->thread, reinterpret_cast<uv_thread_cb>(workerRun), nodeArgc);
    if (err != 0)
        goto thread_create_failed;
    ::WaitForSingleObject(nodeArgc->initEvent, INFINITE);
    ::CloseHandle(nodeArgc->initEvent);
    nodeArgc->initEvent = NULL;
    if (!nodeArgc->initType)
        goto thread_init_failed;
    return nodeArgc;

thread_init_failed:

thread_create_failed:
    free(nodeArgc);
    return nullptr;
}

node::Environment* nodeGetEnvironment(NodeArgc* nodeArgc) {
    if (nodeArgc)
        return nodeArgc->childEnv;
    return nullptr;
}

} // atom

#include "node/src/debug-agent.h"

namespace node {
namespace debugger {

Agent::~Agent(void) {
    Stop();

    uv_sem_destroy(&start_sem_);

    while (AgentMessage* msg = messages_.PopFront())
        delete msg;
}


void Agent::Stop() {
    if (state_ != kRunning)
        return;

    DebugBreak();
    state_ = kNone;
}

}
}