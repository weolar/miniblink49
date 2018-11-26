#ifndef CEF_LIBCEF_BROWSER_CONTEXT_H_
#define CEF_LIBCEF_BROWSER_CONTEXT_H_

#include "include/internal/cef_ptr.h"
#include "include/internal/cef_thread_internal.h"
#include "include/internal/cef_types_wrappers.h"
#include "libcef/browser/ThreadUtil.h"
#include "include/cef_app.h"
#include "third_party/WebKit/Source/wtf/HashSet.h"

class CefMainDelegate;
class CefBrowserHostImpl;

class CefContext {
public:
    //typedef WTF::Vector<CefRefPtr<CefBrowserHostImpl> > BrowserList;
    typedef WTF::HashSet<CefBrowserHostImpl*> BrowserList;

    CefContext();
    ~CefContext();

    // Returns the singleton CefContext instance.
    static CefContext* Get();

    // These methods will be called on the main application thread.
    bool Initialize(const CefMainArgs& args,
        const CefSettings& settings,
        CefRefPtr<CefApp> application,
        void* windows_sandbox_info);

    // Returns true if the context is initialized.
    bool Initialized() { return m_initialized; }
    
    // Returns true if the context is shutting down.
    bool ShuttingDown() const { return m_bShuttingDown; }

    bool IsUIThread() const;

    bool CurrentlyOn(CefThreadId id) const;

//     bool OnInitThread() {
//         return IsUIThread();
//     }

    void SetOsModalLoop(bool osModalLoop) {
        m_osModalLoop = osModalLoop;
    }

    bool OsModalLoop() { return m_osModalLoop; }

    void RegisterBrowser(CefBrowserHostImpl* browser);
    void UnregisterBrowser(CefBrowserHostImpl* browser);

    void SetNeedHeartbeat();
    void ClearNeedHeartbeat();

    bool IsNeedHeartbeat() { return m_needHeartbeat; }
    void FireHeartBeat();

    void RunMessageLoop();
    void Quit();

    const BrowserList& GetBrowserList();

    // Performs shutdown actions that need to occur on the UI thread before any
    // threads are destroyed.
    //void FinishShutdownOnUIThread(base::WaitableEvent* uithread_shutdown_event);

    // Destroys the main runner and related objects.
    void FinalizeShutdown();

private:
    bool InitializeOnWebkitThread(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application, bool* initialized);
    static unsigned __stdcall WebkitThreadEntryPoint(void* param);
    void FinalizeShutdownOnWebkitThread();

    //void ExitMessageLoop();

    void OnContextInitialized();

    // Track context state.
    bool m_initialized;
    bool m_bShuttingDown;

    // The thread on which the context was initialized.
    cef_platform_thread_id_t m_appThreadId; // cefclient 调用线程

    cef_platform_thread_id_t m_uiThreadId; // webkit\窗口线程

    CefSettings m_settings;

    CefMainDelegate* m_mainDelegate;

    bool m_osModalLoop;

    CRITICAL_SECTION m_browserListMutex;
    BrowserList m_browserList;

    int m_needHeartbeat;

    HANDLE m_webkitThreadHandle;

    bool m_webkitShutdown;
};

// Helper macro that returns true if the global context is in a valid state.
#define CONTEXT_STATE_VALID() \
    (CefContext::Get() && CefContext::Get()->Initialized() && !CefContext::Get()->ShuttingDown())

#endif // CEF_LIBCEF_BROWSER_CONTEXT_H_