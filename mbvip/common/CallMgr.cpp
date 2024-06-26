
#include "CallMgr.h"
#include "E:/mycode/miniblink49/trunk/wke/wkedefine.h"
#include <process.h>
#include <Shlwapi.h>
#include <xstring>
#include <vector>

#define WM_THREAD_CALL_OTHER_THREAD (WM_USER + 0x5328)
std::string utf16ToUtf8(LPCWSTR lpszSrc);
extern wkeWebView g_wkeWebview;

namespace MtRender {

DWORD CallMgr::m_blinkThreadId;
DWORD CallMgr::m_uiThreadId;
DWORD CallMgr::m_mainThreadId;

void CallMgr::init(const wkeSettings* settings)
{
    m_uiThreadId = ::GetCurrentThreadId();
    createBlinkThread(settings);
}

void CallMgr::callUiThreadSync(std::function<void(void)>&& closure)
{
    TaskAsyncData* asyncData = cretaeAsyncData(m_uiThreadId);
    asyncData->dataEx = &closure;

    asyncData->call = threadCallbackWrap;
    asyncData->data = asyncData;
    ::PostThreadMessage(asyncData->toThreadId, WM_THREAD_CALL_OTHER_THREAD, (WPARAM)callbackInOtherThread, (LPARAM)asyncData);

    waitForCallThreadAsync(asyncData);
}

void CallMgr::threadCallbackWrap(void* data)
{
    TaskAsyncData* asyncData = (TaskAsyncData*)data;
    std::function<void(void)>* closure = (std::function<void(void)>*)asyncData->dataEx;
    (*closure)();
}

void CallMgr::asynThreadCallbackWrap(void* data)
{
    TaskAsyncData* asyncData = (TaskAsyncData*)data;
    std::function<void(void)>* closure = (std::function<void(void)>*)asyncData->dataEx;
    (*closure)();
    delete closure;
}

void CallMgr::callBlinkThreadAsync(std::function<void(void)>&& closure)
{
    TaskAsyncData* asyncData = cretaeAsyncData(m_blinkThreadId);

    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    asyncData->dataEx = closureDummy;
    callAsync(asyncData, asynThreadCallbackWrap, asyncData);
}

void CallMgr::callUiThreadAsync(std::function<void(void)>&& closure)
{
    TaskAsyncData* asyncData = cretaeAsyncData(m_uiThreadId);

    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    asyncData->dataEx = closureDummy;
    callAsync(asyncData, asynThreadCallbackWrap, asyncData);
}

void CallMgr::callBlinkThreadSync(std::function<void(void)>&& closure)
{
    TaskAsyncData* asyncData = cretaeAsyncData(m_blinkThreadId);
    asyncData->dataEx = &closure;
    callAsync(asyncData, threadCallbackWrap, asyncData);
    waitForCallThreadAsync(asyncData);
}

void CallMgr::postNodeCoreThreadTask(std::function<void(void)>&& closure)
{
    callUiThreadAsync(std::move(closure));
}

void CallMgr::shutdown()
{
}

bool CallMgr::isBlinkThread()
{
    return m_blinkThreadId == ::GetCurrentThreadId();
}

bool CallMgr::isUiThread()
{
    return m_uiThreadId == ::GetCurrentThreadId();
}

void CallMgr::setMainThread()
{
    m_mainThreadId = ::GetCurrentThreadId();
}

void CallMgr::callbackInOtherThread(TaskAsyncData* asyncData)
{
    if (asyncData->call) {
        asyncData->ret = nullptr;
        asyncData->call(asyncData->data);
        ::SetEvent(asyncData->event);
    }
}

void CallMgr::callAsync(TaskAsyncData* asyncData, CoreMainTask call, void* data)
{
    asyncData->call = call;
    asyncData->data = data;
    ::PostThreadMessage(asyncData->toThreadId, WM_THREAD_CALL, (WPARAM)callbackInOtherThread, (LPARAM)asyncData);
}

void* CallMgr::waitForCallThreadAsync(TaskAsyncData* asyncData)
{
    void* ret = asyncData->ret;
    DWORD waitResult = 0;

    waitResult = ::WaitForSingleObject(asyncData->event, INFINITE);
    return ret;
}

CallMgr::TaskAsyncData* CallMgr::cretaeAsyncData(DWORD toThreadId)
{
    TaskAsyncData* asyncData = new TaskAsyncData();
    asyncData->event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    asyncData->fromThreadId = ::GetCurrentThreadId();
    asyncData->toThreadId = toThreadId;

    return asyncData;
}

void CallMgr::exitMessageLoop(DWORD threadId)
{
    if (::GetCurrentThreadId() == threadId) {
        ::PostThreadMessage(threadId, WM_QUIT, (WPARAM)::GetCurrentThreadId(), (LPARAM)0);
        return;
    }

    bool hadExit = false;
    ::PostThreadMessage(threadId, WM_QUIT, (WPARAM)::GetCurrentThreadId(), (LPARAM)&hadExit);
    while (!hadExit) {
        ::Sleep(20);
    }
}

void onShowDevtools()
{
    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path[0], 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(nullptr, &path[0], MAX_PATH);
    ::PathRemoveFileSpecW(&path[0]);

    std::vector<wchar_t> pathXX = path;
    ::PathAppendW(&path[0], L"front_end\\inspector.html");

    std::string pathA = "E:/mycode/miniblink49/trunk/third_party/WebKit/Source/devtools/front_end/inspector.html";
    if (::PathFileExistsA(pathA.c_str())) {
        wkeSetDebugConfig(g_wkeWebview, "showDevTools", pathA.c_str());
        return;
    }

    if (::PathFileExistsW(&path[0])) {
        pathA = utf16ToUtf8(&path[0]);
        wkeSetDebugConfig(g_wkeWebview, "showDevTools", pathA.c_str());
    }
}

void CallMgr::messageLoop()
{
    MSG msg;
    while (true) {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message) {
                if (0 != msg.lParam)
                    *((bool*)(msg.lParam)) = true;
                else
                    OutputDebugStringA("CallMgr::messageLoop\n");
                break;
            }

            if (msg.message == WM_KEYDOWN && msg.wParam == 113) {
                callBlinkThreadAsync([] { onShowDevtools(); });
            }

            if (WM_THREAD_CALL == msg.message) {
                if (msg.wParam != (WPARAM)callbackInOtherThread)
                    DebugBreak();
                TaskAsyncData* data = (TaskAsyncData*)msg.lParam;
                callbackInOtherThread(data);
                delete data;
            }

            if (WM_THREAD_CALL_OTHER_THREAD == msg.message) {
                if (msg.wParam != (WPARAM)callbackInOtherThread)
                    DebugBreak();
                TaskAsyncData* data = (TaskAsyncData*)msg.lParam;
                callbackInOtherThread(data);
                delete data;
            }

            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        } else {
            ::Sleep(20);
        }
    }

    if (0 != m_mainThreadId && ::GetCurrentThreadId() == m_uiThreadId)
        exitMessageLoop(m_mainThreadId);
}

static int uiThreadPostTaskCallback(HWND hWnd, wkeUiThreadRunCallback callback, void* param)
{
    CallMgr::callUiThreadAsync([hWnd, callback, param] {
        callback(hWnd, param);
    });
    return 1;
}

struct InitBlinkInfo {
    bool created;
    const wkeSettings* settings;
};

unsigned CallMgr::blinkThread(void* param)
{
    m_blinkThreadId = ::GetCurrentThreadId();
    InitBlinkInfo* initBlinkInfo = (InitBlinkInfo*)param;

    wkeInitializeEx(initBlinkInfo->settings);

    initBlinkInfo->created = true;

    messageLoop();

    wkeFinalize();

    return 0;
}

void CallMgr::createBlinkThread(const wkeSettings* settings)
{
    InitBlinkInfo initBlinkInfo;
    initBlinkInfo.created = false;
    initBlinkInfo.settings = settings;

    unsigned threadIdentifier = 0;
    HANDLE threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, blinkThread, &initBlinkInfo, 0, &threadIdentifier));

    while (!initBlinkInfo.created) { ::Sleep(20); }
}

} // MtRender