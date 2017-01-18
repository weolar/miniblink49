
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

    static void callUiThreadAsync(CoreMainTask call, void* data);
    static void* waitForCallUiThreadAsync();

    static void* callUiThreadSync(CoreMainTask call, void* data);
    static bool callUiThreadSync(v8::FunctionCallback call, const v8::FunctionCallbackInfo<v8::Value>& args);

    static void callBlinkThreadSync(std::function<void(void)> closure);

    static void shutdown();

    static void createBlinkThread();

private:
    struct TaskAsyncData {
        uv_async_t async;
        CoreMainTask call;
        void* data;
        HANDLE event;
        void* ret;
        uv_mutex_t mutex;
        uv_thread_t main_thread_id;
    };
    static TaskAsyncData* m_uiThreadAsyncData;
    static TaskAsyncData* m_blinkThreadAsyncData;

    static void callbackInThread(uv_async_t* handle);

    static void callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data);

    static void* waitForCallThreadAsync(TaskAsyncData* asyncData);

    static void initData(TaskAsyncData** asyncData, uv_loop_t* loop);

    static void blinkThread(void* created);
};

} // atom