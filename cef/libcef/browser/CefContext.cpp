#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/browser/CefContext.h"

#include <process.h>

#include "third_party/WebKit/Source/wtf/Functional.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "include/internal/cef_win.h"
#include "libcef/browser/CefBrowserHostImpl.h"
#include "libcef/common/MainDelegate.h"
#include "libcef/common/CefContentClient.h"
#include "libcef/common/CefCommandLineImpl.h"
#include "libcef/common/CefTaskImpl.h"
#include "libcef/renderer/CefV8Impl.h"
#include "content/browser/WebPage.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebThreadImpl.h"
#include "base/thread.h"

static CefContext* g_context = nullptr;

struct WebkitThreadInitArgs {
    WebkitThreadInitArgs(CefContext* context, const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application, bool* initialized)
        : m_context(context)
        , m_args(&args)
        , m_settings(settings)
        , m_application(application)
        , m_initialized(initialized)
    {

    }

    CefContext* context() { return m_context; }
    const CefMainArgs* args() { return m_args; }
    CefSettings settings() { return m_settings; }
    CefRefPtr<CefApp> application() { return m_application; }
    bool* initialized() { return m_initialized; }
private:
    CefContext* m_context;
    const CefMainArgs* m_args;
    CefSettings m_settings;
    CefRefPtr<CefApp> m_application;
    bool* m_initialized;
};


unsigned CefContext::WebkitThreadEntryPoint(void* param) {
    base::SetThreadName("UiThread");
    
    WebkitThreadInitArgs* webkitInitArgs = (WebkitThreadInitArgs*)param;
    webkitInitArgs->context()->InitializeOnWebkitThread(
        *webkitInitArgs->args(), webkitInitArgs->settings(), webkitInitArgs->application(), webkitInitArgs->initialized());

    webkitInitArgs->context()->RunMessageLoop();
    webkitInitArgs->context()->FinalizeShutdownOnWebkitThread();
    delete webkitInitArgs;
    return 0;
}

int CefExecuteProcess(const CefMainArgs& args, CefRefPtr<CefApp> application, void* windows_sandbox_info) {
    OutputDebugStringW(L"libcef.CefExecuteProcess \n");
    
    if (CefContentClient::Get() && CefContentClient::Get()->application())
        CefContentClient::Get()->SetRendererApplication(application);
    
    return -1;
}

bool CefInitialize(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application, void* windows_sandbox_info) {
    OutputDebugStringW(L"libcef.CefInitialize \n");

    // Return true if the global context already exists.
    if (g_context)
        return true;

    if (settings.size != sizeof(cef_settings_t)) {
        NOTREACHED() << "invalid CefSettings structure size";
        return false;
    }
    g_context = new CefContext();

    // Initialize the global context.
    return g_context->Initialize(args, settings, application, windows_sandbox_info);
}

void CefDoMessageLoopWork() {
    // Verify that the context is in a valid state.
    if (!CONTEXT_STATE_VALID()) {
        NOTREACHED() << "context not valid";
        return;
    }

    // Must always be called on the same thread as Initialize.
    if (!g_context->IsUIThread()) {
        NOTREACHED() << "called on invalid thread";
        return;
    }

    //CefBrowserMessageLoop::current()->DoMessageLoopIteration();
}

void CefRunMessageLoop() {
    CefContext::Get()->RunMessageLoop();
}

void CefQuitMessageLoop() {
    // Verify that the context is in a valid state.
    if (!CONTEXT_STATE_VALID()) {
        NOTREACHED() << "context not valid";
        return;
    }

    // Must always be called on the same thread as Initialize.
    if (!g_context->IsUIThread()) {
        NOTREACHED() << "called on invalid thread";
        return;
    }

    CefContext::Get()->Quit();
}

void CefSetOSModalLoop(bool osModalLoop) {
#if defined(OS_WIN)
    // Verify that the context is in a valid state.
    if (!CONTEXT_STATE_VALID()) {
        NOTREACHED() << "context not valid";
        return;
    }

    if (CEF_CURRENTLY_ON_UIT())
        CefContext::Get()->SetOsModalLoop(osModalLoop);
    else
        CEF_POST_BLINK_TASK(TID_UI, WTF::bind(CefSetOSModalLoop, osModalLoop));
#endif  // defined(OS_WIN)
}

void CefShutdown() {
    CefContext::Get()->FinalizeShutdown();
}

void CefEnableHighDPISupport() {}

// CefContext

CefContext::CefContext()
    : m_initialized(false)
    , m_bShuttingDown(false)
    , m_osModalLoop(false)
    , m_needHeartbeat(0)
    , m_appThreadId(0)
    , m_uiThreadId(0)
    , m_webkitThreadHandle(NULL)
    , m_webkitShutdown(false) {
}

CefContext::~CefContext() {
    delete m_mainDelegate;
}

// static
CefContext* CefContext::Get() {
    return g_context;
}

const CefContext::BrowserList& CefContext::GetBrowserList() {
    ASSERT(IsUIThread());
    return m_browserList;
}

void CefContext::SetNeedHeartbeat() {
    if (0 != m_needHeartbeat)
        return;
    atomicIncrement(&m_needHeartbeat);
    ::PostThreadMessage(m_uiThreadId, WM_NULL, 0, 0);
}

void CefContext::ClearNeedHeartbeat() {
    atomicDecrement(&m_needHeartbeat);
}

void CefContext::FireHeartBeat() {
//     ::EnterCriticalSection(&m_browserListMutex);
//     for (auto it = m_browserList.begin(); it != m_browserList.end(); ++it) {
//         CefBrowserHostImpl* browser = *it;
//         browser->FireHeartbeat();
//     }
//     ::LeaveCriticalSection(&m_browserListMutex);
// 
    ClearNeedHeartbeat();

    content::WebThreadImpl* threadImpl = (content::WebThreadImpl*)(blink::Platform::current()->currentThread());
    threadImpl->fire();
}

void CefContext::FinalizeShutdownOnWebkitThread() {
    ASSERT(IsUIThread());
    CefCommandLine::GetGlobalCommandLine()->Release();
    CefRequestContext::GetGlobalContext()->Release();

    CefV8IsolateDestroyed();

    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
    platform->shutdown();

    ASSERT(0 == m_browserList.size());
    ::DeleteCriticalSection(&m_browserListMutex);

    m_webkitShutdown = true;
}

void CefContext::FinalizeShutdown() {
    if (!IsUIThread())
        Quit();
    else    
        FinalizeShutdownOnWebkitThread();

    while (!m_webkitShutdown) {
        ::Sleep(20);
    }

    if (m_webkitThreadHandle)
        ::CloseHandle(m_webkitThreadHandle);
    m_webkitThreadHandle = NULL;
}

LARGE_INTEGER g_qpcFrequency;

void CefContext::RunMessageLoop() {
    // Verify that the context is in a valid state.
    if (!CONTEXT_STATE_VALID()) {
        NOTREACHED() << "context not valid";
        return;
    }

    // Must always be called on the same thread as Initialize.
    if (!IsUIThread()) {
        NOTREACHED() << "called on invalid thread";
        return;
    }
    
    MSG msg = { 0 };
    BOOL bRet = FALSE;

    LARGE_INTEGER lastFrequency = {0};

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (WM_QUIT == msg.message) {
            //ExitMessageLoop();
            return;
        }

        while (true) {            
            do {
                LARGE_INTEGER qpcFrequency;
                BOOL b = QueryPerformanceCounter(&qpcFrequency);
//                 if (qpcFrequency.LowPart - lastFrequency.LowPart > 5217) {
                     FireHeartBeat();
//                     lastFrequency = qpcFrequency;
//                 }

                if (!TranslateAccelerator(msg.hwnd, NULL, &msg)) {
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                }

                if (!::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                    msg.hwnd = NULL;
                if (WM_QUIT == msg.message) {
                    //ExitMessageLoop();
                    return;
                }

//                 QueryPerformanceCounter(&qpcFrequency);
//                 char* out = (char*)malloc(1000);
//                 sprintf(out, "CefContext::RunMessageLoop: %d, hwnd:%x MSG:%x\n", (qpcFrequency.LowPart - g_qpcFrequency.LowPart), msg.hwnd, msg.message);
//                 OutputDebugStringA(out);
//                 free(out);
//                 g_qpcFrequency = qpcFrequency;
                ::Sleep(2);
            } while (INVALID_HANDLE_VALUE != msg.hwnd && NULL != msg.hwnd);
            
            if (NULL == msg.hwnd && !g_context->IsNeedHeartbeat()) {
                //OutputDebugStringW(L"CefContext::FireHeartBeat break\n");
                break;
            }

//             if (!msg.hwnd)
//                 ::Sleep(2);
        }
    }
}

void CefContext::Quit() {
    ::PostThreadMessage(m_uiThreadId, WM_QUIT, 0, 0);
}

bool CefContext::IsUIThread() const {
    return m_uiThreadId == GetCurrentThreadId();
}

bool CefContext::CurrentlyOn(CefThreadId threadId) const {
    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
    blink::WebThread* webThread = platform->currentThread();
    if (!webThread)
        return false;

    bool result = false;
    switch (threadId) {
    case TID_RENDERER:
    case TID_UI:
        result = (webThread == platform->mainThread());
        break;
    case TID_DB:
        result = false;
        break;
    case TID_FILE:
        result = false;
        break;
    case TID_FILE_USER_BLOCKING:
        result = false;
        break;
    case TID_PROCESS_LAUNCHER:
        result = false;
        break;
    case TID_CACHE:
        result = false;
        break;
    case TID_IO:
        if (!platform->tryGetIoThread())
            result = false;
        else
            result = (webThread == platform->tryGetIoThread());
        break;
    default:
        break;
    };

    return result;
}

bool CefContext::InitializeOnWebkitThread(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application, bool* initialized) {
    m_uiThreadId = GetCurrentThreadId();
    content::WebPage::initBlink();
    m_mainDelegate = new CefMainDelegate(application);
    m_initialized = true;
    SetNeedHeartbeat();
    OnContextInitialized();
    *initialized = true;

    return true;
}

bool CefContext::Initialize(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application, void* windows_sandbox_info) {
    m_appThreadId = GetCurrentThreadId();
    ::InitializeCriticalSection(&m_browserListMutex);

    base::CommandLine::Init(0, nullptr);

    m_settings = settings;
    bool initialized = false;

    if (settings.multi_threaded_message_loop) {
        //NOTIMPLEMENTED() << "multi_threaded_message_loop is not supported.";
        unsigned threadIdentifier = 0;
        WebkitThreadInitArgs* webkitInitArgs = new WebkitThreadInitArgs(this, args, settings, application, &initialized);
        m_webkitThreadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, WebkitThreadEntryPoint, webkitInitArgs, 0, &threadIdentifier));
    } else
        InitializeOnWebkitThread(args, settings, application, &initialized);
    
    while (!m_initialized) {
        Sleep(20);
    }

    return true;
}

void CefContext::OnContextInitialized() {
    CEF_REQUIRE_UIT();

    // Notify the handler.
    CefRefPtr<CefApp> app = CefContentClient::Get()->application();
    if (!app.get())
        return;

    CefRefPtr<CefBrowserProcessHandler> handler = app->GetBrowserProcessHandler();
    if (!handler.get())
        return;

    handler->OnContextInitialized();

    base::CommandLine* baseCommandLine = base::CommandLine::ForCurrentProcess();
    CefRefPtr<CefCommandLineImpl> commandLine = new CefCommandLineImpl(baseCommandLine, false, false);

    String out = String::format("CefContext::OnContextInitialized 1: %d\n", (unsigned int)commandLine.get());
    OutputDebugStringA(out.utf8().data());

    commandLine->AppendSwitchWithValue(CefString("type"), CefString("mbrenderer"));
    handler->OnBeforeChildProcessLaunch(commandLine);

    out = String::format("CefContext::OnContextInitialized 2: %d\n", (unsigned int)commandLine.get());
    OutputDebugStringA(out.utf8().data());
}

void CefContext::RegisterBrowser(CefBrowserHostImpl* browser) {
    ASSERT(IsUIThread());
    ASSERT(!m_browserList.contains(browser));
    m_browserList.add(browser);
}

void CefContext::UnregisterBrowser(CefBrowserHostImpl* browser) {
    ::EnterCriticalSection(&m_browserListMutex);
    ASSERT(m_browserList.contains(browser));
    m_browserList.remove(browser);
    ::LeaveCriticalSection(&m_browserListMutex);
}
#endif
