
#include "uv.h"
#include "v8.h"
#include <functional>
#if USING_VC6RT == 1
#include <functionalvc6.h>
#endif

namespace atom {

class ThreadCall {
private:
    typedef void (*CoreMainTask)(void *data);
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
    static void init(uv_loop_t* loop);
    static void createBlinkThread(v8::Platform* v8platform); // Blink线程要先创建，才能调用init

    static void callBlinkThreadAsync(std::function<void(void)>&& closure);
    static void callBlinkThreadSync(std::function<void(void)>&& closure);
    static void callUiThreadSync(std::function<void(void)>&& closure);
    static void callUiThreadAsync(std::function<void(void)>&& closure);

    static void postNodeCoreThreadTask(std::function<void(void)>&& closure);

    static void shutdown();

    static void exitMessageLoop(DWORD threadId);
    static void messageLoop(uv_loop_t* loop, v8::Platform* platform, v8::Isolate* isolate);

    static uv_loop_t* getUiLoop() { return m_uiLoop; }
    static uv_loop_t* getBlinkLoop() { return m_blinkLoop; }

    static DWORD getUiThreadId() { return m_uiThreadId; }
    static DWORD getBlinkThreadId() { return m_blinkThreadId; }

    static bool isBlinkThread();
    static bool isUiThread();

    static void setMainThread();

private:
    static void callThreadAsync(std::function<void(void)> closure);
    static void threadCallbackWrap(void* data);
    static void asynThreadCallbackWrap(void* data);

    static DWORD m_blinkThreadId;
    static DWORD m_uiThreadId;
    static DWORD m_mainThreadId;

    static uv_loop_t* m_uiLoop;
    static uv_loop_t* m_blinkLoop;

    static void callbackInOtherThread(TaskAsyncData* asyncData);

    static void callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data);

    static void* waitForCallThreadAsync(TaskAsyncData* asyncData);

    static TaskAsyncData* cretaeAsyncData(uv_loop_t* loop, DWORD toThreadId);

    static void blinkThread(void* created);

    static v8::Platform* m_v8platform;
};

} // atom