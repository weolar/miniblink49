#include "common/ThreadCall.h"

#include "common/WinUserMsg.h"
#include "common/mbexport.h"
#include "wke.h"
#include "base/thread.h"
#include "node/nodeblink.h"
#include "libplatform/libplatform.h"

namespace atom {

DWORD ThreadCall::m_blinkThreadId;
DWORD ThreadCall::m_uiThreadId;
DWORD ThreadCall::m_mainThreadId;

uv_loop_t* ThreadCall::m_uiLoop;
uv_loop_t* ThreadCall::m_blinkLoop;

v8::Platform* ThreadCall::m_v8platformOfBlink = nullptr;
v8::Platform* ThreadCall::m_v8platformOfUi = nullptr;

const int WM_QUIT_REENTER = (WM_USER + 0x5326);

std::list<ThreadCall::TaskItem*>* ThreadCall::m_taskQueue[] = { 0 };
CRITICAL_SECTION ThreadCall::m_taskQueueMutex;

void ThreadCall::init(uv_loop_t* uiLoop)
{
    m_uiLoop = uiLoop;
    m_uiThreadId = ::GetCurrentThreadId();
}

static void MB_CALL_TYPE threadCallStub(void* param1, void* param2)
{
    std::function<void(void)>* closure = (std::function<void(void)>*)param1;
    (*closure)();
}

void ThreadCall::callBlinkThreadAsync(std::function<void(void)>&& closure)
{
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    mbCallBlinkThreadAsync(threadCallStub, closureDummy, nullptr);
}

void ThreadCall::callUiThreadAsync(std::function<void(void)>&& closure)
{
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    mbCallUiThreadAsync(threadCallStub, closureDummy, nullptr);
}

void ThreadCall::callUiThreadSync(std::function<void(void)>&& closure)
{
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    mbCallUiThreadSync(threadCallStub, closureDummy, nullptr);
}

void ThreadCall::callBlinkThreadSync(std::function<void(void)>&& closure)
{
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    mbCallBlinkThreadSync(threadCallStub, closureDummy, nullptr);
}

void ThreadCall::postNodeCoreThreadTask(std::function<void(void)>&& closure)
{
    callUiThreadAsync(std::move(closure));
}

void ThreadCall::shutdown()
{
}

bool ThreadCall::isBlinkThread()
{
    return m_blinkThreadId == ::GetCurrentThreadId();
}

bool ThreadCall::isUiThread()
{
    return m_uiThreadId == ::GetCurrentThreadId();
}

void ThreadCall::setMainThread()
{
    m_mainThreadId = ::GetCurrentThreadId();
}

void ThreadCall::exitMessageLoop(DWORD threadId)
{
//     if (::GetCurrentThreadId() == threadId) {
//         postThreadMessage(threadId, WM_QUIT, (WPARAM)::GetCurrentThreadId(), (LPARAM)0);
//         return;
//     }
// 
//     bool hadExit = false;
//     postThreadMessage(threadId, WM_QUIT, (WPARAM)::GetCurrentThreadId(), (LPARAM)&hadExit);
//     while (!hadExit) {
//         ::Sleep(2);
//     }
}

void ThreadCall::exitReEnterMessageLoop(DWORD threadId)
{
    //postThreadMessage(threadId, WM_QUIT_REENTER, (WPARAM)::GetCurrentThreadId(), 0);
}

void ThreadCall::messageLoop(uv_loop_t* loop, v8::Platform* platform, v8::Isolate* isolate)
{
//     MSG msg;
//     bool more = false;
// 
//     DWORD threadId = ::GetCurrentThreadId();
// 
//     while (true) {
//         if (loop) {
//             BlinkMicrotaskSuppressionHandle handle = nullptr;
//             if (isBlinkThread())
//                 handle = nodeBlinkMicrotaskSuppressionEnter(isolate);
//             more = (0 != uv_run(loop, UV_RUN_NOWAIT));
//             if (handle)
//                 nodeBlinkMicrotaskSuppressionLeave(handle);
//         }
// 
//         if (platform && isolate)
//             v8::platform::PumpMessageLoop(platform, isolate);
// 
//         if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//             if (WM_QUIT == msg.message) {
//                 if (0 != msg.lParam)
//                     *((bool*)(msg.lParam)) = true;
//                 else
//                     OutputDebugStringA("ThreadCall::messageLoop\n");
//                 break;
//             }
// 
//             if (WM_THREAD_CALL == msg.message)
//                 DebugBreak();
// 
//             ::TranslateMessage(&msg);
//             ::DispatchMessageW(&msg);
//         } else {
//             ::Sleep(2);
//         }
// 
//         if (doTaskQueue(threadId))
//             return;
//     }
// 
//     if (::GetCurrentThreadId() == m_uiThreadId)
//         exitMessageLoop(m_mainThreadId);
}

void ThreadCall::blinkThread(void* created)
{
//     base::SetThreadName("BlinkCore");
// 
//     m_blinkThreadId = ::GetCurrentThreadId();
//     m_blinkLoop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
//     int err = uv_loop_init(m_blinkLoop);
// 
//     wkeInitialize();
//     *(bool*)created = true;
// 
//     v8::Isolate* isolate = v8::Isolate::GetCurrent();
//     messageLoop(m_blinkLoop, m_v8platform, isolate);
// 
//     wkeFinalize();
// 
//     free(m_blinkLoop);
//     m_blinkLoop = nullptr;
    DebugBreak();
}

void ThreadCall::OnBlinkThreadIdle(ThreadCall* self, v8::Isolate* isolate)
{
    bool more = (0 != uv_run(m_blinkLoop, UV_RUN_NOWAIT));
    v8::platform::PumpMessageLoop(m_v8platformOfBlink, isolate);
}

void ThreadCall::OnUiThreadIdle(ThreadCall* self, v8::Isolate* isolate)
{
    bool more = (0 != uv_run(m_uiLoop, UV_RUN_NOWAIT));
    v8::platform::PumpMessageLoop(m_v8platformOfUi, isolate);
}

void ThreadCall::onBlinkThreadInited(void* param1, void* param2)
{
    base::SetThreadName("BlinkThread");

    m_blinkThreadId = ::GetCurrentThreadId();
    m_blinkLoop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
    int err = uv_loop_init(m_blinkLoop);

    ThreadCall::m_uiThreadId = ::GetCurrentThreadId();
    mbOnThreadIdle((mbThreadCallback)OnBlinkThreadIdle, nullptr, v8::Isolate::GetCurrent());

    BOOL* created = (BOOL*)param1;
    *created = TRUE;
}

void ThreadCall::createBlinkThread(v8::Platform* v8platform)
{
    m_v8platformOfBlink = v8platform;
    BOOL created = FALSE;

    //mbSetMbDllPath(L"node.dll");

    mbSettings* settings = new mbSettings();
    memset(settings, 0, sizeof(mbSettings));

    //settings->mask |= MB_SETTING_PROXY;
    //settings->proxy.type = MB_PROXY_HTTP;
    //strcpy(settings->proxy.hostname, "127.0.0.1");
    //settings->proxy.port = 10809;
    //settings->proxy.username[50];
    //strcpy(settings->proxy.password, "c906d0c8902c75d3910cd46c21dc34e8");

    //settings->mask = MB_ENABLE_NODEJS;
    //settings->mask = MB_ENABLE_DISABLE_CC;
    //settings->mask |= MB_ENABLE_DISABLE_H5VIDEO;
    //settings->mask |= MB_ENABLE_ENABLE_SWIFTSHAER;
    //settings->mask |= MB_ENABLE_ENABLE_EGLGLES2;
    settings->version = kMbVersion;

    mbInit(settings);
    mbFillFuncPtr();
    mbEnableHighDPISupport();

    s_mbPassWebElementValueToOtherContext = (FN_mbPassWebElementValueToOtherContext)mbGetProcAddr("mbPassWebElementValueToOtherContext");
    s_mbGetContextByV8Object = (FN_mbGetContextByV8Object)mbGetProcAddr("mbGetContextByV8Object");
    s_mbGetWorldScriptContextByWebFrame = (FN_mbGetWorldScriptContextByWebFrame)mbGetProcAddr("mbGetWorldScriptContextByWebFrame");

//     uv_thread_t tid;
//     uv_thread_create(&tid, reinterpret_cast<uv_thread_cb>(blinkThread), &created);

    mbOnBlinkThreadInit(onBlinkThreadInited, &created, nullptr);
    while (!created) {
        ::Sleep(1);
    }
}

void ThreadCall::runUiThread(uv_loop_t* loop, v8::Platform* platform, v8::Isolate* isolate)
{
    m_v8platformOfUi = platform;

    mbOnThreadIdle((mbThreadCallback)OnUiThreadIdle, nullptr, v8::Isolate::GetCurrent());
    mbRunMessageLoop();
}

void ThreadCall::runMainThread()
{
    MSG msg;
    if (::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
}

} // atom