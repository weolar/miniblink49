
#include "uv.h"
#include "v8.h"
#include <functional>
#if USING_VC6RT == 1
#include <functionalvc6.h>
#endif

namespace atom {

class ThreadCall {
public:
    static void init(uv_loop_t* loop);

    typedef void *(*CoreMainTask)(void *data);

private:
    struct TaskAsyncData {
        CoreMainTask call;
        void* data;
        void* dataEx;
        HANDLE event;
        void* ret;
        DWORD fromThreadId;
        DWORD toThreadId;
    };

public:
    static void callBlinkThreadSync(std::function<void(void)> closure);
    static void callUiThreadSync(std::function<void(void)> closure);

    static void shutdown();

    static void messageLoop(uv_loop_t* loop);

private:
    static void* callUiThreadSync(CoreMainTask call, void* data);

    static void* threadCallbackWrap(void* data);

    static void createBlinkThread();

    static DWORD m_blinkThreadId;
    static DWORD m_uiThreadId;

    static uv_loop_t* m_uiLoop;
    static uv_loop_t* m_blinkLoop;

    static void callbackInThread(uv_async_t* handle);
    static void callbackInOtherThread(TaskAsyncData* asyncData);

    static void callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data);

    static void* waitForCallThreadAsync(TaskAsyncData* asyncData);

    static TaskAsyncData* initAsyncData(uv_loop_t* loop, DWORD toThreadId);

    static void blinkThread(void* created);
};

} // atom