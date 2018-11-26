
#include "CallMgr.h"
#include "wke.h"
#include <process.h>

#define WM_THREAD_CALL_OTHER_THREAD (WM_USER + 0x5328)

namespace MtRender {

DWORD CallMgr::m_blinkThreadId;
DWORD CallMgr::m_uiThreadId;
DWORD CallMgr::m_mainThreadId;

void CallMgr::init()
{
    m_uiThreadId = ::GetCurrentThreadId();
    createBlinkThread();
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

unsigned CallMgr::blinkThread(void* created)
{
    //base::SetThreadName("BlinkCore");

    m_blinkThreadId = ::GetCurrentThreadId();

    wkeInitialize();
    *(bool*)created = true;

    messageLoop();

    wkeFinalize();

    return 0;
}

void CallMgr::createBlinkThread()
{
    bool created = false;

    unsigned threadIdentifier = 0;
    HANDLE threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, blinkThread, &created, 0, &threadIdentifier));

    while (!created) { ::Sleep(20); }
}

} // MtRender