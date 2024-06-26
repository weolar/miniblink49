
#ifndef common_ThreadCall_h
#define common_ThreadCall_h

#include "wke/wkedefine.h"
#include "../core/MbWebView.h"
#include <functional>
#include <list>
#include <windows.h>

typedef struct uv_loop_s uv_loop_t;

namespace v8 {
class Platform;
class Isolate;
}

namespace mb {
class MbWebView;
}

typedef struct _wkeSettings  wkeSettings;
typedef intptr_t mbWebView;

#define MB_FROM_HERE ::common::TraceLocation(__FUNCTION__, __FILE__)

namespace common {

class TraceLocation {
public:
    // These char*s are not copied and must live for the duration of the program.
    TraceLocation(const char* func, const char* file)
        : m_functionName(func)
        , m_fileName(file)
    { }
    TraceLocation()
        : m_functionName("unknown")
        , m_fileName("unknown")
    { }

    const char* functionName() const { return m_functionName; }
    const char* fileName() const { return m_fileName; }

private:
    const char* m_functionName;
    const char* m_fileName;
};

struct ThreadCallballInfo {
    mbThreadCallback cb = nullptr;
    void* param1 = nullptr;
    void* param2 = nullptr;
};

class ThreadCall {
private:
    typedef void (*CoreMainTask)(void *data);
    struct TaskAsyncData {
        CoreMainTask call;
        void* data;
        void* dataEx;
        BOOL event;
        void* ret;
        DWORD fromThreadId;
        DWORD toThreadId;
        DWORD destroyThreadId;
        TraceLocation caller;
    };
    
public:
    static void init(const mbSettings* settings);
    static void initTaskQueue();

    static void callBlinkThreadAsyncWithValid(const TraceLocation& caller, mbWebView webView, std::function<void(mb::MbWebView* webview)>&& closure);
    static void callBlinkThreadAsync(const TraceLocation& caller, std::function<void(void)>&& closure);
    static void callBlinkThreadSync(const TraceLocation& caller, std::function<void(void)>&& closure);
    static void callUiThreadSync(const TraceLocation& caller, std::function<void(void)>&& closure);
    static void callUiThreadAsync(const TraceLocation& caller, std::function<void(void)>&& closure);
    static void callMediaThreadAsync(const TraceLocation& caller, std::function<void(void)>&& closure);

    static void postNodeCoreThreadTask(const TraceLocation& caller, std::function<void(void)>&& closure);

    static void shutdown();

    static void exitBlinkMessageLoop();
    static void blinkMessageLoop(uv_loop_t* loop, v8::Platform* platform, v8::Isolate* isolate);
    static void uiMessageLoop();

    static DWORD getUiThreadId() { return m_uiThreadId; }
    static DWORD getBlinkThreadId() { return m_blinkThreadId; }

    static uv_loop_t* getBlinkLoop() { return m_blinkLoop; }

    static bool isBlinkThread();
    static bool isUiThread();

    static void wake();
    static bool runNoWait(DWORD threadId);

    static void setThreadName(const char* szThreadName);

    static void setThreadIdle(mbThreadCallback callback, void* param1, void* param2);
    static void setBlinkThreadInited(mbThreadCallback callback, void* param1, void* param2);

private:
    static void initializeOffScreenTimerWindow();
    static LRESULT CALLBACK timerWindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    static void createBlinkThread(const mbSettings* settings);
    static void createMediaThread();
    static void callSyncAndWait(TaskAsyncData* asyncData);
    static void threadCallbackWrap(void* data);
    static void asynThreadCallbackWrap(void* data);

    static DWORD m_blinkThreadId;
    static DWORD m_uiThreadId;
    static DWORD m_mediaThreadId;

    enum CompositorTpye {
        kCompositorTpyeMC,
        kCompositorTpyeSoftware,
        kCompositorTpyeEGLGLES2,
        kCompositorTpyeSwiftShader,
    };
    static CompositorTpye m_compositorTpye;

    static void callbackInOtherThread(TaskAsyncData* asyncData);
    static void callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data);
    static void* waitForCallThreadAsync(TaskAsyncData* asyncData);
    static TaskAsyncData* cretaeAsyncData(const TraceLocation& caller, DWORD toThreadId, void* dataEx, DWORD destroyThreadId);
    static void postThreadMessage(DWORD idThread, UINT Msg, TaskAsyncData* asyncData);

    static unsigned MB_CALL_TYPE blinkThread(void* created);
    static unsigned MB_CALL_TYPE mediaThread(void* param);

    struct TaskItem {
        TaskItem(DWORD idThread, UINT msg, TaskAsyncData* asyncData) {
            this->idThread = idThread;
            this->msg = msg;
            this->asyncData = asyncData;
        }
        DWORD idThread;
        UINT msg;
        TaskAsyncData* asyncData;
    };
    enum TaskQueueType {
        kBlinkTaskQueue = 0,
        kUiTaskQueue,
        kMediaTaskQueue,

        kMaxTaskQueue,
    };
    
    static TaskQueueType getWhichTypeByThreadId(DWORD idThread);
    static bool runUntilIdle(DWORD threadId);
    static void runV8(DWORD threadId);
    static bool runTaskQueue(UINT msg, TaskAsyncData* asyncData);
    static std::list<TaskItem*>* m_taskQueue[kMaxTaskQueue];
    static CRITICAL_SECTION m_taskQueueMutex;

    friend void MB_CALL_TYPE mbOnThreadIdle(mbThreadCallback callback, void* param1, void* param2);
    friend void MB_CALL_TYPE mbOnBlinkThreadInit(mbThreadCallback callback, void* param1, void* param2);
    static ThreadCallballInfo s_blinkThreadInitedInfo;
    static ThreadCallballInfo s_blinkThreadIdleInfo;
    static ThreadCallballInfo s_uiThreadIdleInfo;

    static uv_loop_t* m_blinkLoop;
    static v8::Platform* m_v8platform;
};

} // atom

void MyOutputDebugStringA(const char* lpcszOutputString, ...);

#endif // common_ThreadCall_h