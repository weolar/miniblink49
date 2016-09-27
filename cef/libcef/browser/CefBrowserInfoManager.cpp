
#include "include/internal/cef_ptr.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_client.h"

#include "libcef/browser/CefBrowserInfoManager.h"
#include "libcef/browser/CefBrowserInfo.h"
#include "libcef/browser/CefBrowserHostImpl.h"
#include "libcef/browser/CefFrameHostImpl.h"

#include "libcef/common/StringUtil.h"
#include "third_party/WebKit/public/web/WebWindowFeatures.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "content/browser/WebPage.h"
#include "include/base/cef_logging.h"

static CefBrowserInfoManager* g_infoManager = nullptr;

CefBrowserInfoManager::CefBrowserInfoManager()
    : m_nextBrowserId(0) {
    DCHECK(!g_infoManager);
    g_infoManager = this;
}

CefBrowserInfoManager::~CefBrowserInfoManager() {
    DCHECK(m_browserInfoList.isEmpty());
    g_infoManager = nullptr;
}

// static
CefBrowserInfoManager* CefBrowserInfoManager::GetInstance() {
    if (!g_infoManager) {
        g_infoManager = new CefBrowserInfoManager();
    }
    return g_infoManager;
}

scoped_refptr<CefBrowserInfo> CefBrowserInfoManager::CreateBrowserInfo(
    bool is_popup,
    bool is_windowless) {
    MutexLocker locker(m_browserInfoLock);

    scoped_refptr<CefBrowserInfo> browser_info = new CefBrowserInfo(++m_nextBrowserId, is_popup);
    m_browserInfoList.append(browser_info);

    if (is_windowless)
        browser_info->SetWindowless(true);

    return browser_info;
}

scoped_refptr<CefBrowserInfo> CefBrowserInfoManager::CreatePopupBrowserInfo(bool is_windowless) {
    MutexLocker locker(m_browserInfoLock);

    CefBrowserInfo* browserInfo = new CefBrowserInfo(++m_nextBrowserId, true);

    m_browserInfoList.append(browserInfo);

    if (is_windowless)
        browserInfo->SetWindowless(true);
    
    return browserInfo;
}


void CefBrowserInfoManager::RemoveBrowserInfo(scoped_refptr<CefBrowserInfo> browserInfo) {
    MutexLocker locker(m_browserInfoLock);

    size_t it = 0;
    for (; it < m_browserInfoList.size(); ++it) {
        if (m_browserInfoList[it] == browserInfo) {
            delete m_browserInfoList[it];
            m_browserInfoList.remove(it);
            return;
        }
    }

    NOTREACHED();
}

void CefBrowserInfoManager::DestroyAllBrowsers() {
    BrowserInfoList list;

    {
        MutexLocker locker(m_browserInfoLock);
        list = m_browserInfoList;
    }

    // Destroy any remaining browser windows.
    if (!list.isEmpty()) {
        BrowserInfoList::iterator it = list.begin();
        for (; it != list.end(); ++it) {
            CefRefPtr<CefBrowserHostImpl> browser = (*it)->Browser();
            DCHECK(browser.get());
            if (browser.get()) {
                // DestroyBrowser will call RemoveBrowserInfo.
                browser->DestroyBrowser();
            }
        }
    }

#ifndef NDEBUG
  {
      // Verify that all browser windows have been destroyed.
      MutexLocker locker(m_browserInfoLock);
      DCHECK(m_browserInfoList.isEmpty());
  }
#endif
}

static void TranslatePopupFeatures(const blink::WebWindowFeatures& webKitFeatures, CefPopupFeatures& features) {
    features.x = static_cast<int>(webKitFeatures.x);
    features.xSet = webKitFeatures.xSet;
    features.y = static_cast<int>(webKitFeatures.y);
    features.ySet = webKitFeatures.ySet;
    features.width = static_cast<int>(webKitFeatures.width);
    features.widthSet = webKitFeatures.widthSet;
    features.height = static_cast<int>(webKitFeatures.height);
    features.heightSet = webKitFeatures.heightSet;

    features.menuBarVisible = webKitFeatures.menuBarVisible;
    features.statusBarVisible = webKitFeatures.statusBarVisible;
    features.toolBarVisible = webKitFeatures.toolBarVisible;
    features.locationBarVisible = webKitFeatures.locationBarVisible;
    features.scrollbarsVisible = webKitFeatures.scrollbarsVisible;
    features.resizable = webKitFeatures.resizable;

    features.fullscreen = webKitFeatures.fullscreen;
    features.dialog = webKitFeatures.dialog;
    features.additionalFeatures = NULL;
    if (webKitFeatures.additionalFeatures.size() > 0)
        features.additionalFeatures = cef_string_list_alloc();

    CefString str;
    for (unsigned int i = 0; i < webKitFeatures.additionalFeatures.size(); ++i) {
        cef::WebStringToCefStringUTF16(webKitFeatures.additionalFeatures[i], str);
        cef_string_list_append(features.additionalFeatures, str.GetStruct());
    }
}

scoped_refptr<CefBrowserHostImpl> CefBrowserInfoManager::CreateBrowserHostIfAllow(
    content::WebPage* webPage,
    blink::WebLocalFrame* creator,
    const blink::WebURLRequest& request,
    const blink::WebWindowFeatures& features,
    const blink::WebString& name,
    blink::WebNavigationPolicy policy,
    bool suppressOpener) {
    CefRefPtr<CefBrowserHostImpl> browserImpl = webPage->browser();
    CefWindowInfo windowInfo;
    CefPopupFeatures cefFeatures;
    TranslatePopupFeatures(features, cefFeatures);

    if (cefFeatures.xSet)
        windowInfo.x = cefFeatures.x;
    if (cefFeatures.ySet)
        windowInfo.y = cefFeatures.y;
    if (cefFeatures.widthSet)
        windowInfo.width = cefFeatures.width;
    if (cefFeatures.heightSet)
        windowInfo.height = cefFeatures.height;

    blink::Frame* blinkFrame = blink::toCoreFrame((blink::WebFrame*)creator);
    if (!blinkFrame)
        return nullptr;
    
//     blink::Frame* blinkParentFrame = toCoreFrame(creator->parent());
//     int64 parentFrameId = blinkParentFrame ? blinkParentFrame->frameID() : 0;
    
    CefStringUTF16 urlSpec;
    cef::WebStringToCefString(request.url().spec().utf16(), urlSpec);

    CefStringUTF16 frameName;
    cef::WebStringToCefString(name, frameName);

//     CefRefPtr<CefFrame> frame = browserImpl->getor(browserImpl.get(),
//         creator,
//         //urlSpec,
//         frameName); // TODO delete
    CefRefPtr<CefFrame> frame = browserImpl->GetOrCreateFrame(creator, content::WebPage::kUnspecifiedFrameId, request.url());

    cef_window_open_disposition_t disposition;
    if (suppressOpener)
        disposition = WOD_SUPPRESS_OPEN;
    else if (blink::WebNavigationPolicyIgnore == policy)
        disposition = WOD_IGNORE_ACTION;
    else if (blink::WebNavigationPolicyDownload == policy)
        disposition = WOD_SAVE_TO_DISK;
    else if (blink::WebNavigationPolicyCurrentTab == policy)
        disposition = WOD_CURRENT_TAB;
    else if (blink::WebNavigationPolicyNewBackgroundTab == policy)
        disposition = WOD_NEW_BACKGROUND_TAB;
    else if (blink::WebNavigationPolicyNewForegroundTab == policy)
        disposition = WOD_NEW_FOREGROUND_TAB;
    else if (blink::WebNavigationPolicyNewWindow == policy)
        disposition = WOD_SINGLETON_TAB;
    else if (blink::WebNavigationPolicyNewPopup == policy)
        disposition = WOD_NEW_POPUP;

    bool no_javascript_access = false;
    
    CefRefPtr<CefBrowserHostImpl> browser = webPage->browser();
    CefRefPtr<CefClient> client = browser->GetClient();
    if (!client.get())
        return nullptr;

    CefRefPtr<CefLifeSpanHandler> handler = client->GetLifeSpanHandler();
    CefBrowserSettings settings = browser->settings();

    bool allow = !handler->OnBeforePopup(browser.get(),
        frame, urlSpec, frameName, disposition,
        true, cefFeatures, windowInfo, client,
        settings, &no_javascript_access);

    if (!allow)
        return nullptr;

    bool isPopup = blink::WebNavigationPolicyNewPopup == policy;
    CefRefPtr<CefBrowserHostImpl> newBrowserHostImpl = CefBrowserHostImpl::Create(windowInfo, client, urlSpec, settings, browserImpl, isPopup, nullptr);

    return newBrowserHostImpl;
}