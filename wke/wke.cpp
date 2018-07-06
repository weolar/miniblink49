#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

//////////////////////////////////////////////////////////////////////////
#define BUILDING_wke 1

#include "content/browser/WebPage.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebCookieJarCurlImpl.h"
#include "content/web_impl_win/WebThreadImpl.h"
#include "content/web_impl_win/npapi/WebPluginImpl.h"
#include "content/web_impl_win/npapi/PluginDatabase.h"
#include "net/WebURLLoaderManager.h"

//cexer: 必须包含在后面，因为其中的 wke.h -> windows.h 会定义 max、min，导致 WebCore 内部的 max、min 出现错乱。
#include "wkeString.h"
#include "wkeWebView.h"
#include "wkeWebWindow.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebDragOperation.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebCustomElement.h"
#include "third_party/WebKit/public/platform/WebDragData.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"
#include "gen/blink/platform/RuntimeEnabledFeatures.h"
#include <v8.h>
#include "wtf/text/WTFString.h"
#include "wtf/text/WTFStringUtil.h"

namespace net {
void setCookieJarPath(const WCHAR* path);
void setCookieJarFullPath(const WCHAR* path);
}

bool g_isSetDragEnable = true;
bool g_isSetDragDropEnable = true;

namespace blink {
extern char* g_navigatorPlatform;
}

//////////////////////////////////////////////////////////////////////////
static std::string* s_versionString = nullptr;
static bool wkeIsInit = false;

void wkeInitialize()
{
    if (wkeIsInit)
        return;

    //double-precision float
    _controlfp(_PC_53, _MCW_PC);

    CoInitialize(NULL);

    content::WebPage::initBlink();
    wkeIsInit = true;
}

struct wkeProxyInfo {
    net::ProxyType proxyType;
    String hostname;
    String username;
    String password;

    static WTF::PassOwnPtr<wkeProxyInfo> create(const wkeProxy& proxy) {
        WTF::PassOwnPtr<wkeProxyInfo> info = WTF::adoptPtr(new wkeProxyInfo());
        info->proxyType = net::HTTP;

        if (proxy.hostname[0] != 0 && proxy.type >= WKE_PROXY_HTTP && proxy.type <= WKE_PROXY_SOCKS5HOSTNAME) {
            switch (proxy.type) {
            case WKE_PROXY_HTTP:           info->proxyType = net::HTTP; break;
            case WKE_PROXY_SOCKS4:         info->proxyType = net::Socks4; break;
            case WKE_PROXY_SOCKS4A:        info->proxyType = net::Socks4A; break;
            case WKE_PROXY_SOCKS5:         info->proxyType = net::Socks5; break;
            case WKE_PROXY_SOCKS5HOSTNAME: info->proxyType = net::Socks5Hostname; break;
            }

            info->hostname = String::fromUTF8(proxy.hostname);
            info->username = String::fromUTF8(proxy.username);
            info->password = String::fromUTF8(proxy.password);
        }
        return info;
    }
};

void wkeSetProxy(const wkeProxy* proxy)
{
    if (!proxy)
        return;

    WTF::OwnPtr<wkeProxyInfo> info = wkeProxyInfo::create(*proxy);

    if (net::WebURLLoaderManager::sharedInstance())
        net::WebURLLoaderManager::sharedInstance()->setProxyInfo(info->hostname, proxy->port, info->proxyType, info->username, info->password);
}

void wkeSetViewProxy(wkeWebView webView, wkeProxy* proxy)
{
    if (!webView || !proxy)
        return;
    WTF::OwnPtr<wkeProxyInfo> info = wkeProxyInfo::create(*proxy);
    webView->setProxyInfo(info->hostname, proxy->port, info->proxyType, info->username, info->password);
}

void wkeSetViewNetInterface(wkeWebView webView, const char* netInterface)
{
    if (!webView || !netInterface)
        return;

    webView->setNetInterface(netInterface);
}

void wkeConfigure(const wkeSettings* settings)
{
    if (!settings)
        return;
    if (settings->mask & WKE_SETTING_PROXY)
        wkeSetProxy(&settings->proxy);
    if (settings->mask & WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD)
        blink::RuntimeEnabledFeatures::setUpdataInOtherThreadEnabled(true);
}

void wkeInitializeEx(const wkeSettings* settings)
{
    wkeInitialize();
    wkeConfigure(settings);
}

bool wkeIsInitialize()
{
    return wkeIsInit;
}

void wkeFinalize()
{
    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
    platform->shutdown();

//     WebCore::iconDatabase().close();
//     WebCore::PageGroup::closeLocalStorage();

    CoUninitialize();

    if (s_versionString)
        delete s_versionString;
    s_versionString = nullptr;
}

void wkeSetMemoryCacheEnable(wkeWebView webView, bool b)
{
    blink::RuntimeEnabledFeatures::setMemoryCacheEnabled(b);
}

bool g_isMouseEnabled = true;

void wkeSetTouchEnabled(wkeWebView webView, bool b)
{
    blink::RuntimeEnabledFeatures::setTouchEnabled(b);
}

void wkeSetMouseEnabled(wkeWebView webView, bool b)
{
    g_isMouseEnabled = b;
}

void wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b)
{
    blink::RuntimeEnabledFeatures::setNavigationToNewWindowEnabled(b);
}

void wkeSetCspCheckEnable(wkeWebView webView, bool b)
{
    blink::RuntimeEnabledFeatures::setCspCheckEnabled(b);
}

void wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b)
{
    blink::RuntimeEnabledFeatures::setNpapiPluginsEnabled(b);
}

void wkeSetHeadlessEnabled(wkeWebView webView, bool b)
{
    blink::RuntimeEnabledFeatures::setHeadlessEnabled(b);
}

void wkeSetDragEnable(wkeWebView webView, bool b)
{
    g_isSetDragEnable = b;
}

void wkeSetDragDropEnable(wkeWebView webView, bool b)
{
    g_isSetDragDropEnable = b;
}

DWORD g_kWakeMinInterval = 5;
double g_kDrawMinInterval = 0.003;
bool g_isDecodeUrlRequest = false;
void* g_tipPaintCallback = nullptr;
float g_contentScale = 1;
bool g_rendererAntiAlias = false;

void wkeSetDebugConfig(wkeWebView webview, const char* debugString, const char* param)
{
    content::WebPage* webpage = nullptr;
    blink::WebViewImpl* webViewImpl = nullptr;
    blink::WebSettingsImpl* settings = nullptr;
    if (webview)
        webpage = webview->getWebPage();
    if (webpage)
        webViewImpl = webpage->webViewImpl();
    if (webViewImpl)
        settings = webViewImpl->settingsImpl();

    String stringDebug(debugString);
    Vector<String> result;
    stringDebug.split(",", result);
    for (size_t i = 0; i < result.size(); ++i) {
        String item = result[i];
        if ("alwaysIsNotSolideColor" == item) {
            blink::RuntimeEnabledFeatures::setAlwaysIsNotSolideColorEnabled(true);
        } else if ("drawDirtyDebugLine" == item) {
            blink::RuntimeEnabledFeatures::setDrawDirtyDebugLineEnabled(true);
        } else if ("drawTileLine" == item) {
            blink::RuntimeEnabledFeatures::setDrawTileLineEnabled(true);
        } else if ("alwaysInflateDirtyRect" == item) {

        } else if ("decodeUrlRequest" == item) {
            g_isDecodeUrlRequest = true;
        } else if ("showDevTools" == item) {
            webview->showDevTools(param, nullptr, nullptr);
        } else if ("wakeMinInterval" == item) {
            g_kWakeMinInterval = atoi(param);
        } else if ("drawMinInterval" == item) {
            int drawMinInterval = atoi(param);
            drawMinInterval = drawMinInterval / 1000.0;
            webpage->setDrawMinInterval(drawMinInterval);
        } else if ("minimumFontSize" == item) {
            if (settings)
                settings->setMinimumFontSize(atoi(param));
        } else if ("minimumLogicalFontSize" == item) {
            if (settings)
                settings->setMinimumLogicalFontSize(atoi(param));
        } else if ("defaultFontSize" == item) {
            if (settings)
                settings->setDefaultFontSize(atoi(param));
        } else if ("defaultFixedFontSize" == item) {
            if (settings)
                settings->setDefaultFixedFontSize(atoi(param));
        } else if ("tipPaintCallback" == item) {
            g_tipPaintCallback = (void*)param;
        } else if ("contentScale" == item) {
            g_contentScale = atoi(param) / 100.0;
        } else if ("antiAlias" == item) {
            g_rendererAntiAlias = atoi(param) == 1;
        }
    }
}

void wkeUpdate()
{
//     static HWND hTimer = NULL;
//     if (!hTimer)
//         hTimer = FindWindow(L"TimerWindowClass", NULL);
// 
//     if (hTimer) {
//         MSG msg;
//         while(PeekMessage(&msg, hTimer, 0, 0, PM_REMOVE))
//         {
//             TranslateMessage(&msg);
//             DispatchMessage(&msg);
//         }
//     }
}

#define MAJOR_VERSION   (1)
#define MINOR_VERSION   (2)
#define WEBKIT_BUILD    (98096)

unsigned int wkeGetVersion()
{
    return (MAJOR_VERSION << 8) + MINOR_VERSION;
}

const utf8* wkeGetVersionString()
{
    if (s_versionString)
        return s_versionString->c_str();

    String versionString = String::format("wke version %d.%02d\n"
        "blink build %d\n"
        "build time %s\n",
        MAJOR_VERSION,
        MINOR_VERSION,
        WEBKIT_BUILD,
        __TIMESTAMP__);

    s_versionString = new std::string(versionString.utf8().data());
    return s_versionString->c_str();
}

const char* wkeGetName(wkeWebView webView)
{
    return webView->name();
}

void wkeSetName(wkeWebView webView, const char* name)
{
    webView->setName(name);
}

void wkeSetHandle(wkeWebView webView, HWND wnd)
{
    webView->setHandle(wnd);
}

void wkeSetHandleOffset(wkeWebView webView, int x, int y)
{
    webView->setHandleOffset(x, y);
}

void wkeSetViewSettings(wkeWebView webView, const wkeViewSettings* settings)
{
    webView->setViewSettings(settings);
}

bool wkeIsTransparent(wkeWebView webView)
{
    return webView->isTransparent();
}

void wkeSetTransparent(wkeWebView webView, bool transparent)
{
    webView->setTransparent(transparent);
}

void wkeSetUserAgent(wkeWebView webView, const utf8* userAgent)
{
    webView->setUserAgent(userAgent);
}

const utf8* wkeGetUserAgent(wkeWebView webView)
{
    return content::BlinkPlatformImpl::getUserAgent();
}

void wkeSetUserAgentW(wkeWebView webView, const wchar_t* userAgent)
{
    webView->setUserAgent(userAgent);
}

void wkeShowDevtools(wkeWebView webView, const wchar_t* path, wkeOnShowDevtoolsCallback callback, void* param)
{
    std::vector<char> pathUtf8;
    WTF::WCharToMByte(path, wcslen(path), &pathUtf8, CP_UTF8);
    webView->showDevTools(&pathUtf8[0], callback, param);
}

void wkePostURL(wkeWebView wkeView,const utf8 * url,const char *szPostData,int nLen)
{
    wkeView->loadPostURL(url,szPostData,nLen);
}

void wkePostURLW(wkeWebView wkeView,const wchar_t * url,const char *szPostData,int nLen)
{
    wkeView->loadPostURL(url,szPostData,nLen);
}

void wkeLoadW(wkeWebView webView, const wchar_t* url)
{
    wkeLoadURLW(webView, url);
}

void wkeLoadURL(wkeWebView webView, const utf8* url)
{
    webView->loadURL(url);
}

void wkeLoadURLW(wkeWebView webView, const wchar_t* url)
{
    webView->loadURL(url);
}

void wkeLoadHTML(wkeWebView webView, const utf8* html)
{
    webView->loadHTML(html);
}

void wkeLoadHtmlWithBaseUrl(wkeWebView webView, const utf8* html, const utf8* baseUrl)
{
    webView->loadHtmlWithBaseUrl(html, baseUrl);
}

void wkeLoadHTMLW(wkeWebView webView, const wchar_t* html)
{
    webView->loadHTML(html);
}

void wkeLoadFile(wkeWebView webView, const utf8* filename)
{
    return webView->loadFile(filename);
}

void wkeLoadFileW(wkeWebView webView, const wchar_t* filename)
{
    return webView->loadFile(filename);
}

const utf8* wkeGetURL(wkeWebView webView)
{
    return webView->url();
}

bool wkeIsLoading(wkeWebView webView)
{
    return webView->isLoading();
}

bool wkeIsLoadingSucceeded(wkeWebView webView)
{
    return webView->isLoadingSucceeded();
}

bool wkeIsLoadingFailed(wkeWebView webView)
{
    return webView->isLoadingFailed();
}

bool wkeIsLoadingCompleted(wkeWebView webView)
{
    return webView->isLoadingCompleted();
}

bool wkeIsDocumentReady(wkeWebView webView)
{
    return webView->isDocumentReady();
}

void wkeStopLoading(wkeWebView webView)
{
    webView->stopLoading();
}

void wkeReload(wkeWebView webView)
{
    webView->reload();
}

void wkeGoToOffset(wkeWebView webView, int offset)
{
    webView->goToOffset(offset);
}

void wkeGoToIndex(wkeWebView webView, int index)
{
    webView->goToIndex(index);
}

const utf8* wkeGetTitle(wkeWebView webView)
{
    return webView->title();
}

const wchar_t* wkeGetTitleW(wkeWebView webView)
{
    return webView->titleW();
}

void wkeResize(wkeWebView webView, int w, int h)
{
    webView->resize(w, h);
}

int wkeGetWidth(wkeWebView webView)
{
    return webView->width();
}

int wkeGetHeight(wkeWebView webView)
{
    return webView->height();
}

int wkeGetContentWidth(wkeWebView webView)
{
    return webView->contentWidth();
}

int wkeGetContentHeight(wkeWebView webView)
{
    return webView->contentHeight();
}

void wkeSetDirty(wkeWebView webView, bool dirty)
{
    webView->setDirty(dirty);
}

bool wkeIsDirty(wkeWebView webView)
{
    return webView->isDirty();
}

void wkeAddDirtyArea(wkeWebView webView, int x, int y, int w, int h)
{
    webView->addDirtyArea(x, y, w, h);
}

void wkeLayoutIfNeeded(wkeWebView webView)
{
    webView->layoutIfNeeded();
}

void wkePaint2(wkeWebView webView, void* bits, int bufWid, int bufHei, int xDst, int yDst, int w, int h, int xSrc, int ySrc, bool bCopyAlpha)
{
    webView->paint(bits, bufWid, bufHei, xDst, yDst, w, h, xSrc, ySrc, bCopyAlpha);
}

void wkePaint(wkeWebView webView, void* bits, int pitch)
{
    webView->paint(bits, pitch);
}

void wkeRepaintIfNeeded(wkeWebView webView)
{
    if (webView)
        webView->repaintIfNeeded();
}

HDC wkeGetViewDC(wkeWebView webView)
{
    return webView->viewDC();
}

HWND wkeGetHostHWND(wkeWebView webView)
{
    return webView->windowHandle();
}

bool wkeCanGoBack(wkeWebView webView)
{
    return webView->canGoBack();
}

bool wkeGoBack(wkeWebView webView)
{
    return webView->goBack();
}

bool wkeCanGoForward(wkeWebView webView)
{
    return webView->canGoForward();
}

bool wkeGoForward(wkeWebView webView)
{
    return webView->goForward();
}

void wkeEditorSelectAll(wkeWebView webView)
{
    webView->editorSelectAll();
}

void wkeEditorUnSelect(wkeWebView webView)
{
    webView->editorUnSelect();
}

void wkeEditorCopy(wkeWebView webView)
{
    webView->editorCopy();
}

void wkeEditorCut(wkeWebView webView)
{
    webView->editorCut();
}

void wkeEditorPaste(wkeWebView webView)
{
    webView->editorPaste();
}

void wkeEditorDelete(wkeWebView webView)
{
    webView->editorDelete();
}

void wkeEditorUndo(wkeWebView webView)
{
    webView->editorUndo();
}

void wkeEditorRedo(wkeWebView webView)
{
    webView->editorRedo();
}

const wchar_t * wkeGetCookieW(wkeWebView webView)
{
    return webView->cookieW();
}

const utf8* wkeGetCookie(wkeWebView webView)
{
    return webView->cookie();
}

void wkeSetCookie(wkeWebView webView, const utf8* url, const utf8* cookie)
{
    blink::KURL webUrl(blink::ParsedURLString, url);
    blink::KURL webFirstPartyForCookies;
    String webCookie(cookie);
    content::WebCookieJarImpl::inst()->setCookie(webUrl, webFirstPartyForCookies, webCookie);
}

void wkeVisitAllCookie(void* params, wkeCookieVisitor visitor)
{
    content::WebCookieJarImpl::visitAllCookie(params, (content::WebCookieJarImpl::CookieVisitor)visitor);
}

void wkePerformCookieCommand(wkeCookieCommand command)
{
    CURL* curl = curl_easy_init();

    if (!curl)
        return;

    CURLSH* curlsh = net::WebURLLoaderManager::sharedInstance()->getCurlShareHandle();
    curl_easy_setopt(curl, CURLOPT_SHARE, curlsh);

    switch (command) {
    case wkeCookieCommandClearAllCookies:
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
        break;
    case wkeCookieCommandClearSessionCookies:
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "SESS");
        break;
    case wkeCookieCommandFlushCookiesToFile:
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "FLUSH");
        break;
    case wkeCookieCommandReloadCookiesFromFile :
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "RELOAD");
        break;
    }
    curl_easy_cleanup(curl);
}

void wkeSetCookieEnabled(wkeWebView webView, bool enable)
{
    webView->setCookieEnabled(enable);
}

bool wkeIsCookieEnabled(wkeWebView webView)
{
    return webView->isCookieEnabled();
}

void wkeSetCookieJarPath(wkeWebView webView, const WCHAR* path)
{
    net::setCookieJarPath(path);
}

void wkeSetCookieJarFullPath(wkeWebView webView, const WCHAR* path)
{
    net::setCookieJarFullPath(path);
}

String* kLocalStorageFullPath = nullptr;

void wkeSetLocalStorageFullPath(wkeWebView webView, const WCHAR* path)
{
    if (!path)
        return;

    if (kLocalStorageFullPath)
        delete kLocalStorageFullPath;
    kLocalStorageFullPath = new String(path);
    if (kLocalStorageFullPath->isEmpty()) {
        delete kLocalStorageFullPath;
        kLocalStorageFullPath = nullptr;
        return;
    }

    if (!kLocalStorageFullPath->endsWith(L'\\'))
        kLocalStorageFullPath->append(L'\\');
}

void wkeAddPluginDirectory(wkeWebView webView, const WCHAR* path)
{
    String directory(path);
    content::PluginDatabase::installedPlugins()->addExtraPluginDirectory(directory);
}

void wkeSetMediaVolume(wkeWebView webView, float volume)
{
    webView->setMediaVolume(volume);
}

float wkeGetMediaVolume(wkeWebView webView)
{
    return webView->mediaVolume();
}

bool wkeFireMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags)
{
    return webView->fireMouseEvent(message, x, y, flags);
}

bool wkeFireContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags)
{
    return webView->fireContextMenuEvent(x, y, flags);
}

bool wkeFireMouseWheelEvent(wkeWebView webView, int x, int y, int delta, unsigned int flags)
{
    return webView->fireMouseWheelEvent(x, y, delta, flags);
}

bool wkeFireKeyUpEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    return webView->fireKeyUpEvent(virtualKeyCode, flags, systemKey);
}

bool wkeFireKeyDownEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    return webView->fireKeyDownEvent(virtualKeyCode, flags, systemKey);
}

bool wkeFireKeyPressEvent(wkeWebView webView, unsigned int charCode, unsigned int flags, bool systemKey)
{
    return webView->fireKeyPressEvent(charCode, flags, systemKey);
}

bool wkeFireWindowsMessage(wkeWebView webView, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
    return webView->fireWindowsMessage(hWnd, message, wParam, lParam, result);
}

void wkeSetFocus(wkeWebView webView)
{
    if (!webView)
        return;
    webView->setFocus();
}

void wkeKillFocus(wkeWebView webView)
{
    if (!webView)
        return;
    webView->killFocus();
}

wkeRect wkeGetCaretRect(wkeWebView webView)
{
    return webView->caretRect();
}

jsValue wkeRunJS(wkeWebView webView, const utf8* script)
{
    return webView->runJS(script);
}

jsValue wkeRunJSW(wkeWebView webView, const wchar_t* script)
{
    return webView->runJS(script);
}

jsExecState wkeGlobalExec(wkeWebView webView)
{
    return webView->globalExec();
}

jsExecState wkeGetGlobalExecByFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    return webView->globalExecByFrame(frameId);
}

void wkeSleep(wkeWebView webView)
{
    webView->sleep();
}

void wkeWake(wkeWebView webView)
{
    if (webView)
        webView->wake();

    static DWORD lastTime = 0;

    DWORD time = ::GetTickCount();

//     String output = String::format("wkeWake: %d\n", time - lastTime);
//     OutputDebugStringA(output.utf8().data());
    if (time - lastTime < g_kWakeMinInterval)
        return;

    lastTime = time;

    content::WebThreadImpl* threadImpl = (content::WebThreadImpl*)(blink::Platform::current()->currentThread());
    threadImpl->fire();

}

bool wkeIsAwake(wkeWebView webView)
{
    return webView->isAwake();
}

void wkeSetZoomFactor(wkeWebView webView, float factor)
{
    webView->setZoomFactor(factor);
}

float wkeGetZoomFactor(wkeWebView webView)
{
    return webView->zoomFactor();
}

void wkeSetEditable(wkeWebView webView, bool editable)
{
    webView->setEditable(editable);
}

void wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback, void* callbackParam)
{
    webView->onTitleChanged(callback, callbackParam);
}

void wkeOnMouseOverUrlChanged(wkeWebView webView, wkeTitleChangedCallback callback, void* callbackParam)
{
    webView->onMouseOverUrlChanged(callback, callbackParam);
}

void wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void* callbackParam)
{
    webView->onURLChanged(callback, callbackParam);
}

void wkeOnURLChanged2(wkeWebView webView, wkeURLChangedCallback2 callback, void* callbackParam)
{
    webView->onURLChanged2(callback, callbackParam);
}

void wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void* callbackParam)
{
    webView->onPaintUpdated(callback, callbackParam);
}

void wkeOnPaintBitUpdated(wkeWebView webView, wkePaintBitUpdatedCallback callback, void* callbackParam)
{
    webView->onPaintBitUpdated(callback, callbackParam);
}

void wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void* callbackParam)
{
    webView->onAlertBox(callback, callbackParam);
}

void wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void* callbackParam)
{
    webView->onConfirmBox(callback, callbackParam);
}

void wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void* callbackParam)
{
    webView->onPromptBox(callback, callbackParam);
}

void wkeOnNavigation(wkeWebView webView, wkeNavigationCallback callback, void* param)
{
    webView->onNavigation(callback, param);
}

void wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void* param)
{
    webView->onCreateView(callback, param);
}

void wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback, void* param)
{
    webView->onDocumentReady(callback, param);
}

void wkeOnDocumentReady2(wkeWebView webView, wkeDocumentReady2Callback callback, void* param)
{
    webView->onDocumentReady2(callback, param);
}

void wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void* param)
{
    webView->onLoadingFinish(callback, param);
}

void wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void* param)
{
    webView->onDownload(callback, param);
}

void wkeNetOnResponse(wkeWebView webView, wkeNetResponseCallback callback, void* param)
{
    webView->onNetResponse(callback, param);
}

void wkeOnConsole(wkeWebView webView, wkeConsoleCallback callback, void* param)
{
    webView->onConsole(callback, param);
}

void wkeSetUIThreadCallback(wkeWebView webView, wkeCallUiThread callback, void* param)
{
    webView->onCallUiThread(callback, param);
}

void wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback, void* callbackParam)
{
    webView->onLoadUrlBegin(callback, callbackParam);
}

void wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void* callbackParam)
{
    webView->onLoadUrlEnd(callback, callbackParam);
}

void wkeOnDidCreateScriptContext(wkeWebView webView, wkeDidCreateScriptContextCallback callback, void* callbackParam)
{
    webView->onDidCreateScriptContext(callback, callbackParam);
}

void wkeOnWillReleaseScriptContext(wkeWebView webView, wkeWillReleaseScriptContextCallback callback, void* callbackParam)
{
    webView->onWillReleaseScriptContext(callback, callbackParam);
}

void wkeOnStartDragging(wkeWebView webView, wkeStartDraggingCallback callback, void* param)
{
    webView->onStartDragging(callback, param);
}

wkeWillMediaLoadCallback g_wkeWillMediaLoadCallback = nullptr;
void* g_wkeWillMediaLoadCallbackCallbackParam = nullptr;
void wkeOnWillMediaLoad(wkeWebView webView, wkeWillMediaLoadCallback callback, void* callbackParam)
{
    g_wkeWillMediaLoadCallback = callback;
    g_wkeWillMediaLoadCallbackCallbackParam = callbackParam;
}

wkeTempCallbackInfo g_wkeTempCallbackInfo;

wkeTempCallbackInfo* wkeGetTempCallbackInfo(wkeWebView webView)
{
    return &g_wkeTempCallbackInfo;
}

void wkeOnOtherLoad(wkeWebView webWindow, wkeOnOtherLoadCallback callback, void* param)
{
    webWindow->onOtherLoad(callback, param);
}

void wkeDeleteWillSendRequestInfo(wkeWebView webWindow, wkeWillSendRequestInfo* info)
{
    wkeDeleteString(info->url);
    if (info->newUrl)
        wkeDeleteString(info->newUrl);
    wkeDeleteString(info->method);
    wkeDeleteString(info->referrer);
    delete info;
}

bool wkeIsMainFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    content::WebPage* page = webView->webPage();
    if (!page)
        return false;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return false;
    return !webFrame->parent();
}

bool wkeIsWebRemoteFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    content::WebPage* page = webView->webPage();
    if (!page)
        return false;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return false;
    return webFrame->isWebRemoteFrame();
}

wkeWebFrameHandle wkeWebFrameGetMainFrame(wkeWebView webView)
{
    content::WebPage* page = webView->webPage();
    if (!page)
        return nullptr;
    blink::WebFrame* frame = page->mainFrame();
    if (!frame)
        return nullptr;
    return wke::CWebView::frameIdTowkeWebFrameHandle(page, page->getFrameIdByBlinkFrame(frame));
}

jsValue wkeRunJsByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const utf8* script, bool isInClosure)
{
    return webView->runJsInFrame(frameId, script, isInClosure);
}

void wkeInsertCSSByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const utf8* cssText)
{
    content::WebPage* page = webView->webPage();
    if (!page)
        return;
    blink::WebFrame* frame = page->mainFrame();
    if (!frame)
        return;
    frame->document().insertStyleSheet(blink::WebString::fromUTF8(cssText));
}

const utf8* wkeGetFrameUrl(wkeWebView webView, wkeWebFrameHandle frameId)
{
    content::WebPage* page = webView->webPage();
    if (!page)
        return "";
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return "";
    return "";
}

void wkeWebFrameGetMainWorldScriptContext(wkeWebView webView, wkeWebFrameHandle frameId, v8ContextPtr contextOut)
{
    content::WebPage* page = webView->webPage();
    if (!page)
        return;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return;
    v8::Local<v8::Context> result = webFrame->mainWorldScriptContext();
    v8::Local<v8::Context>* contextOutPtr = (v8::Local<v8::Context>*)contextOut;
    *contextOutPtr = result;
}

v8Isolate wkeGetBlinkMainThreadIsolate()
{
    return blink::mainThreadIsolate();
}

const utf8* wkeGetString(const wkeString s)
{
    return s ? s->string() : "";
}

const wchar_t* wkeGetStringW(const wkeString string)
{
    return string ? string->stringW() : L"";
}

void wkeSetString(wkeString string, const utf8* str, size_t len)
{
    if (!string)
        return;

    if (nullptr == str) {
        str = "";
        len = 0;
    } else {
        if (len == 0)
            len = strlen(str);
    }

    string->setString(str, len);
}

void wkeSetStringW(wkeString string, const wchar_t* str, size_t len)
{
    if (!string)
        return;

    if (nullptr == str) {
        str = L"";
        len = 0;
    } else {
        if (len == 0)
            len = wcslen(str);
    }

    string->setString(str, len);
}

wkeString wkeCreateString(const utf8* str, size_t len)
{
    wkeString wkeStr = new wke::CString(str, len);
    return wkeStr;
}

wkeString wkeCreateStringW(const wchar_t* str, size_t len)
{
    wkeString wkeStr = new wke::CString(str, len);
    return wkeStr;
}

void wkeDeleteString(wkeString str)
{
    delete str;
}

wkeWebView wkeGetWebViewForCurrentContext()
{
    content::WebPage* webpage = content::WebPage::getSelfForCurrentContext();
    if (!webpage)
        return nullptr;
    wkeWebView webview = webpage->wkeWebView();
    return webview;
}

void wkeSetUserKeyValue(wkeWebView webView, const char* key, void* value)
{
    webView->setUserKeyValue(key, value);
}

void* wkeGetUserKeyValue(wkeWebView webView, const char* key)
{
    return webView->getUserKeyValue(key);
}

int wkeGetCursorInfoType(wkeWebView webView)
{
    return webView->getCursorInfoType();
}

void wkeSetDragFiles(wkeWebView webView, const POINT* clintPos, const POINT* screenPos, wkeString files[], int filesCount)
{
    webView->setDragFiles(clintPos, screenPos, files, filesCount);
}

// typedef void (__cdecl* _PVFV) ();
// #pragma section(".CRT$XCG", long, read)
// 
// STDAPI_(BOOL) DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID /*lpReserved*/)
// {
//      BOOL ret = FALSE;
//      switch (ul_reason_for_call) {
//          case DLL_PROCESS_ATTACH:
//              ret = TRUE;
//              break;
//  
//          case DLL_PROCESS_DETACH:
// 
//              break;
//  
//          case DLL_THREAD_ATTACH:
//              break;
//  
//          case DLL_THREAD_DETACH:
//              break;
//      }
//      return ret;
// }

wkeWebView wkeCreateWebWindow(wkeWindowType type, HWND parent, int x, int y, int width, int height)
{
    wke::CWebWindow* webWindow = new wke::CWebWindow();
    if (!webWindow->create(parent, type, x, y, width, height)) {
        delete webWindow;
        return NULL;
    }

    return webWindow;
}

void wkeDestroyWebWindow(wkeWebView webWindow)
{
    webWindow->destroy();
}

HWND wkeGetWindowHandle(wkeWebView webWindow)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->windowHandle();
    else
        return NULL;
}

void wkeOnWindowClosing(wkeWebView webWindow, wkeWindowClosingCallback callback, void* param)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->onClosing(callback, param);
}

void wkeOnWindowDestroy(wkeWebView webWindow, wkeWindowDestroyCallback callback, void* param)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->onDestroy(callback, param);
}

void wkeOnDraggableRegionsChanged(wkeWebView webWindow, wkeDraggableRegionsChangedCallback callback, void* param)
{
    if (wke::CWebView* window = static_cast<wke::CWebView*>(webWindow))
        return window->onDraggableRegionsChanged(callback, param);
}

void wkeShowWindow(wkeWebView webWindow, bool showFlag)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->show(showFlag);
}

void wkeEnableWindow(wkeWebView webWindow, bool enableFlag)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->enable(enableFlag);
}

void wkeMoveWindow(wkeWebView webWindow, int x, int y, int width, int height)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->move(x, y, width, height);
}

void wkeMoveToCenter(wkeWebView webWindow)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->moveToCenter();
}

void wkeResizeWindow(wkeWebView webWindow, int width, int height)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->resize(width, height);
}

void wkeSetWindowTitle(wkeWebView webWindow, const utf8* title)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->setTitle(title);
}

void wkeSetWindowTitleW(wkeWebView webWindow, const wchar_t* title)
{
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->setTitle(title);
}

WKE_EXTERN_C wkeNodeOnCreateProcessCallback g_wkeNodeOnCreateProcessCallback = nullptr;
WKE_EXTERN_C void* g_wkeNodeOnCreateProcessCallbackparam = nullptr;

void wkeNodeOnCreateProcess(wkeWebView webWindow, wkeNodeOnCreateProcessCallback callback, void* param)
{
    g_wkeNodeOnCreateProcessCallback = callback;
    g_wkeNodeOnCreateProcessCallbackparam = param;
}

static void convertDragData(blink::WebDragData* data, const wkeWebDragData* webDragData) {
    data->initialize();

    if (webDragData->m_filesystemId)
        data->setFilesystemId(webDragData->m_filesystemId->original());
    for (int i = 0; i < webDragData->m_itemListLength; ++i) {
        wkeWebDragData::Item* it = &webDragData->m_itemList[i];
        blink::WebDragData::Item item;
        item.storageType = (blink::WebDragData::Item::StorageType)it->storageType;
        if (it->stringType)
            item.stringType = it->stringType->original();
        if (it->stringData)
            item.stringData = it->stringData->original();
        if (it->filenameData)
            item.filenameData = it->filenameData->original();
        if (it->displayNameData)
            item.displayNameData = it->displayNameData->original();
        if (it->binaryData)
            item.binaryData.assign(it->binaryData, it->binaryDataLength);
        if (it->title)
            item.title = it->title->original();
        if (it->fileSystemURL)
            item.fileSystemURL = blink::KURL(blink::ParsedURLString, it->fileSystemURL->original());
        item.fileSystemFileSize = it->fileSystemFileSize;
        if (it->baseURL)
            item.baseURL = blink::KURL(blink::ParsedURLString, it->baseURL->original());

        data->addItem(item);
    }
}

wkeWebDragOperation wkeDragTargetDragEnter(wkeWebView webWindow, const wkeWebDragData* webDragData, const POINT* clientPoint, const POINT* screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers)
{
    if (!webWindow->webPage())
        return wkeWebDragOperationNone;
    blink::WebViewImpl* view = webWindow->webPage()->webViewImpl();
    if (!view)
        return wkeWebDragOperationNone;

    blink::WebDragData data;
    convertDragData(&data, webDragData);
    blink::WebDragOperation op = view->dragTargetDragEnter(data,
        blink::WebPoint(clientPoint->x, clientPoint->y),
        blink::WebPoint(screenPoint->x, screenPoint->y),
        (blink::WebDragOperationsMask)operationsAllowed, modifiers);

    return (wkeWebDragOperation)op;
}

wkeWebDragOperation wkeDragTargetDragOver(wkeWebView webWindow, const POINT* clientPoint, const POINT* screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers)
{
    if (!webWindow->webPage())
        return wkeWebDragOperationNone;
    blink::WebViewImpl* view = webWindow->webPage()->webViewImpl();
    if (!view)
        return wkeWebDragOperationNone;

    blink::WebDragOperation op = view->dragTargetDragOver(blink::WebPoint(clientPoint->x, clientPoint->y),
        blink::WebPoint(screenPoint->x, screenPoint->y), (blink::WebDragOperationsMask)operationsAllowed, modifiers);
    return (wkeWebDragOperation)op;
}

void wkeDragTargetDragLeave(wkeWebView webWindow)
{
    if (!webWindow->webPage())
        return;
    blink::WebViewImpl* view = webWindow->webPage()->webViewImpl();
    if (view)
        view->dragTargetDragLeave();
}

void wkeDragTargetDrop(wkeWebView webWindow, const POINT* clientPoint, const POINT* screenPoint, int modifiers)
{
    if (!webWindow->webPage())
        return;
    blink::WebViewImpl* view = webWindow->webPage()->webViewImpl();
    if (!view)
        return;

    view->dragTargetDrop(blink::WebPoint(clientPoint->x, clientPoint->y),
        blink::WebPoint(screenPoint->x, screenPoint->y), modifiers);
}

void wkeSetDeviceParameter(wkeWebView webView, const char* device, const char* paramStr, int paramInt, float paramFloat)
{
    if (0 == strcmp(device, "navigator.maxTouchPoints")) {
        blink::WebSettingsImpl* settings = webView->webPage()->webViewImpl()->settingsImpl();
        if (settings)
            settings->setMaxTouchPoints(paramInt);
    } else if (0 == strcmp(device, "navigator.platform")) {
        if (blink::g_navigatorPlatform)
            free(blink::g_navigatorPlatform);
        int length = strlen(paramStr);
        blink::g_navigatorPlatform = (char*)malloc(length + 1);
        memset(blink::g_navigatorPlatform, 0, length + 1);
        strncpy(blink::g_navigatorPlatform, paramStr, length);
    } else if (0 == strcmp(device, "navigator.hardwareConcurrency")) {
        content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
        platform->setNumberOfProcessors(paramInt);
    } else if (0 == strcmp(device, "navigator.vibrate")) {
        ;
    } else if (0 == strcmp(device, "screen.width")) {
        blink::WebScreenInfo info = webView->webPage()->screenInfo();
        info.rect.width = paramInt;
        webView->webPage()->setScreenInfo(info);
    } else if (0 == strcmp(device, "screen.height")) {
        blink::WebScreenInfo info = webView->webPage()->screenInfo();
        info.rect.height = paramInt;
        webView->webPage()->setScreenInfo(info);
    } else if (0 == strcmp(device, "screen.availWidth")) {
        blink::WebScreenInfo info = webView->webPage()->screenInfo();
        info.availableRect.width = paramInt;
        webView->webPage()->setScreenInfo(info);
    } else if (0 == strcmp(device, "screen.availHeight")) {
        blink::WebScreenInfo info = webView->webPage()->screenInfo();
        info.availableRect.height = paramInt;
        webView->webPage()->setScreenInfo(info);
    } else if (0 == strcmp(device, "screen.pixelDepth") || 0 == strcmp(device, "screen.pixelDepth")) {
        blink::WebScreenInfo info = webView->webPage()->screenInfo();
        info.depth = paramInt;
        webView->webPage()->setScreenInfo(info);
    } else if (0 == strcmp(device, "window.devicePixelRatio")) {
        wkeSetZoomFactor(webView, paramFloat);
    }
}

void wkeAddNpapiPlugin(wkeWebView webView, const char* mime, void* initializeFunc, void* getEntryPointsFunc, void* shutdownFunc)
{
    RefPtr<content::PluginPackage> package = content::PluginPackage::createVirtualPackage(
        (NP_InitializeFuncPtr)initializeFunc,
        (NP_GetEntryPointsFuncPtr) getEntryPointsFunc,
        (NPP_ShutdownProcPtr) shutdownFunc,
        0, "virtualPlugin", mime, mime);

    content::PluginDatabase* database = content::PluginDatabase::installedPlugins();
    database->addVirtualPlugin(package);
    database->setPreferredPluginForMIMEType(mime, package.get());
}

wkeWebView wkeGetWebviewByNData(void* ndata)
{
    content::WebPluginImpl* plugin = (content::WebPluginImpl*)ndata;
    return plugin->getWkeWebView();
}

bool wkeRegisterEmbedderCustomElement(wkeWebView webView, wkeWebFrameHandle frameId, const char* name, void* options, void* outResult)
{
    content::WebPage* page = webView->webPage();
    if (!page)
        return false;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return false;

    blink::WebExceptionCode c = 0;
    v8::Local<v8::Value>& optionsV8 = *(v8::Local<v8::Value>*)options;

    blink::WebString nameString = blink::WebString::fromUTF8(name);
    blink::WebCustomElement::addEmbedderCustomElementName(nameString);
    v8::Local<v8::Value> elementConstructor = webFrame->document().registerEmbedderCustomElement(nameString, optionsV8, c);
    v8::Persistent<v8::Value>* result = (v8::Persistent<v8::Value>*)outResult;
    result->Reset(v8::Isolate::GetCurrent(), elementConstructor);
    return true;
}

//////////////////////////////////////////////////////////////////////////
// V1 API

void wkeInit()
{
    wkeInitialize();
}

void wkeShutdown()
{
    wkeFinalize();
}

unsigned int wkeVersion()
{
    return wkeGetVersion();
}

const utf8* wkeVersionString()
{
    return wkeGetVersionString();
}

void wkeGC(wkeWebView webView, long intervalSec)
{
    content::BlinkPlatformImpl* platformImpl = (content::BlinkPlatformImpl*)blink::Platform::current();
    platformImpl->setGcTimer((double)intervalSec);
}

void wkeSetResourceGc(wkeWebView webView, long intervalSec)
{
    content::BlinkPlatformImpl* platformImpl = (content::BlinkPlatformImpl*)blink::Platform::current();
    platformImpl->setResGcTimer((double)intervalSec);
}

WKE_EXTERN_C void curl_set_file_system(
    WKE_FILE_OPEN pfnOpen,
    WKE_FILE_CLOSE pfnClose,
    WKE_FILE_SIZE pfnSize,
    WKE_FILE_READ pfnRead,
    WKE_FILE_SEEK pfnSeek,
    WKE_EXISTS_FILE pfnExistsFile);

WKE_FILE_OPEN g_pfnOpen = nullptr;
WKE_FILE_CLOSE g_pfnClose = nullptr;

void wkeSetFileSystem(WKE_FILE_OPEN pfnOpen, WKE_FILE_CLOSE pfnClose, WKE_FILE_SIZE pfnSize, WKE_FILE_READ pfnRead, WKE_FILE_SEEK pfnSeek)
{
    WKE_FILE_OPEN g_pfnOpen = pfnOpen;
    WKE_FILE_CLOSE g_pfnClose = pfnClose;
    curl_set_file_system(pfnOpen, pfnClose, pfnSize, pfnRead, pfnSeek, nullptr);
}

const char* wkeWebViewName(wkeWebView webView)
{
    return wkeGetName(webView);
}
void wkeSetWebViewName(wkeWebView webView, const char* name)
{
    wkeSetName(webView, name);
}

bool wkeIsLoaded(wkeWebView webView)
{
    return wkeIsLoading(webView);
}

bool wkeIsLoadFailed(wkeWebView webView)
{
    return wkeIsLoadingFailed(webView);
}

bool wkeIsLoadComplete(wkeWebView webView)
{
    return wkeIsLoadingCompleted(webView);
}

const utf8* wkeGetSource(wkeWebView webView)
{
    return nullptr;
}

const utf8* wkeTitle(wkeWebView webView)
{
    return wkeGetTitle(webView);
}

const wchar_t* wkeTitleW(wkeWebView webView)
{
    return wkeGetTitleW(webView);
}

int wkeWidth(wkeWebView webView)
{
    return wkeGetWidth(webView);
}

int wkeHeight(wkeWebView webView)
{
    return wkeGetHeight(webView);
}

int wkeContentsWidth(wkeWebView webView)
{
    return wkeGetContentWidth(webView);
}

int wkeContentsHeight(wkeWebView webView)
{
    return wkeGetContentHeight(webView);
}

void wkeSelectAll(wkeWebView webView)
{
    wkeEditorSelectAll(webView);
}

void wkeCopy(wkeWebView webView)
{
    wkeEditorCopy(webView);
}

void wkeCut(wkeWebView webView)
{
    wkeEditorCut(webView);
}

void wkePaste(wkeWebView webView)
{
    wkeEditorPaste(webView);
}

void wkeDelete(wkeWebView webView)
{
    wkeEditorDelete(webView);
}

bool wkeCookieEnabled(wkeWebView webView)
{
    return wkeIsCookieEnabled(webView);
}

float wkeMediaVolume(wkeWebView webView)
{
    return wkeGetMediaVolume(webView);
}

bool wkeMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags)
{
    return wkeFireMouseEvent(webView, message, x, y, flags);
}

bool wkeContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags)
{
    return wkeFireContextMenuEvent(webView, x, y, flags);
}
bool wkeMouseWheel(wkeWebView webView, int x, int y, int delta, unsigned int flags)
{
    return wkeFireMouseWheelEvent(webView, x, y, delta, flags);
}

bool wkeKeyUp(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    return wkeFireKeyUpEvent(webView, virtualKeyCode, flags, systemKey);
}

bool wkeKeyDown(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    return wkeFireKeyDownEvent(webView, virtualKeyCode, flags, systemKey);
}

bool wkeKeyPress(wkeWebView webView, unsigned int charCode, unsigned int flags, bool systemKey)
{
    return wkeFireKeyPressEvent(webView, charCode, flags, systemKey);
}

void wkeFocus(wkeWebView webView)
{
    wkeSetFocus(webView);
}

void wkeUnfocus(wkeWebView webView)
{
    wkeKillFocus(webView);
}

wkeRect wkeGetCaret(wkeWebView webView)
{
    return wkeGetCaretRect(webView);
}

void wkeAwaken(wkeWebView webView)
{
    return wkeWake(webView);
}

float wkeZoomFactor(wkeWebView webView)
{
    return wkeGetZoomFactor(webView);
}

void wkeTitleChangedCallbackWrap(wkeWebView webView, void* param, const wkeString title)
{
    const wkeClientHandler* handler = (const wkeClientHandler*)param;
    handler->onTitleChanged(handler, title);
}

void wkeURLChangedCallbackWrap(wkeWebView webView, void* param, const wkeString url)
{
    const wkeClientHandler* handler = (const wkeClientHandler*)param;
    handler->onTitleChanged(handler, url);
}

void wkeSetClientHandler(wkeWebView webView, const wkeClientHandler* handler)
{
    webView->setClientHandler(handler);

    wkeOnTitleChanged(webView, wkeTitleChangedCallbackWrap, (void*)handler);
    wkeOnURLChanged(webView, wkeURLChangedCallbackWrap, (void*)handler);
}

const wkeClientHandler* wkeGetClientHandler(wkeWebView webView)
{
    return (const wkeClientHandler*)webView->getClientHandler();
}

const utf8* wkeToString(const wkeString string)
{
    return wkeGetString(string);
}

const wchar_t* wkeToStringW(const wkeString string)
{
    return wkeGetStringW(string);
}

// V1 API end
//////////////////////////////////////////////////////////////////////////

#endif