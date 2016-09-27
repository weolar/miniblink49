#ifndef CefBrowserInfo_h
#define CefBrowserInfo_h

// CefBrowserInfo is used to associate a browser ID and render view/process
// IDs with a particular CefBrowserHostImpl. Render view/process IDs may change
// during the lifetime of a single CefBrowserHostImpl.
//
// CefBrowserInfo objects are managed by CefContentBrowserClient and should not
// be created directly.

#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"

class CefBrowserHostImpl;

class CefBrowserInfo : public base::RefCountedThreadSafe<CefBrowserInfo> {
public:
    CefBrowserInfo(int browserId, bool bIsPopup) { 
        m_browserId = browserId; 
        m_bIsPopup = bIsPopup;
        m_lock = new WTF::Mutex();
    }

    ~CefBrowserInfo() {
        m_lock = nullptr;
    }

    bool IsPopup() const { return m_bIsPopup; }
    bool IsWindowless() const { return m_bIsWindowless; }

    void SetWindowless(bool bWindowless) { m_bIsWindowless = bWindowless; }

    CefRefPtr<CefBrowserHostImpl> CefBrowserInfo::Browser() {
        MutexLocker locker(*m_lock);
        return m_browser;
    }

    void CefBrowserInfo::SetBrowser(CefRefPtr<CefBrowserHostImpl> browser) {
        WTF::Mutex* lock = m_lock;
        lock->tryLock();
        AddRef();
        m_browser = browser;
        bool hasOneRef = HasOneRef();
        Release();
        lock->unlock();
        if (hasOneRef)
            delete lock;
    }

private:
    int m_browserId;
    bool m_bIsPopup;
    bool m_bIsWindowless;

    mutable WTF::Mutex* m_lock;

    // May be NULL if the browser has not yet been created or if the browser has
    // been destroyed.
    CefRefPtr<CefBrowserHostImpl> m_browser;

    DISALLOW_COPY_AND_ASSIGN(CefBrowserInfo);
};

#endif // CefBrowserInfo_h