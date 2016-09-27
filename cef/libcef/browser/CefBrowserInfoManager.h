#ifndef CEF_LIBCEF_BROWSER_BROWSER_INFO_MANAGER_H_
#define CEF_LIBCEF_BROWSER_BROWSER_INFO_MANAGER_H_

#include "include/base/cef_ref_counted.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/public/web/WebNavigationPolicy.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"

namespace blink {
class WebLocalFrame;
class WebURLRequest;
class WebString;
struct WebWindowFeatures;
}

namespace content {
class WebPage;
}

class CefBrowserInfo;
class CefBrowserHostImpl;

class CefBrowserInfoManager {
public:
    CefBrowserInfoManager();
    ~CefBrowserInfoManager();

    // Returns this singleton instance of this class.
    static CefBrowserInfoManager* GetInstance();

    // Called from CefBrowserHostImpl::Create when a new browser is being created
    // directly. In this case |is_popup| will be true only for DevTools browsers.
    scoped_refptr<CefBrowserInfo> CreateBrowserInfo(bool is_popup, bool is_windowless);

    // Called from CefBrowserHostImpl::WebContentsCreated when a new browser is
    // being created for a traditional popup (e.g. window.open() or targeted
    // link). If any OnGetNewBrowserInfo requests are pending for the popup the
    // response will be sent when this method is called.
    scoped_refptr<CefBrowserInfo> CreatePopupBrowserInfo(bool is_windowless);

    scoped_refptr<CefBrowserHostImpl> CreateBrowserHostIfAllow(
        content::WebPage* webPage,
        blink::WebLocalFrame* creator,
        const blink::WebURLRequest& request,
        const blink::WebWindowFeatures& features,
        const blink::WebString& name,
        blink::WebNavigationPolicy policy,
        bool suppressOpener);

    // Called from CefBrowserHostImpl::DestroyBrowser() when a browser is
    // destroyed.
    void RemoveBrowserInfo(scoped_refptr<CefBrowserInfo> browser_info);

    // Called from CefContext::FinishShutdownOnUIThread() to destroy all browsers.
    void DestroyAllBrowsers();

private:
    mutable WTF::Mutex m_browserInfoLock;
    typedef WTF::Vector<CefBrowserInfo*> BrowserInfoList;
    BrowserInfoList m_browserInfoList;
    int m_nextBrowserId;
};


#endif // CEF_LIBCEF_BROWSER_BROWSER_INFO_MANAGER_H_