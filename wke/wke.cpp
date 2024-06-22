#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

//////////////////////////////////////////////////////////////////////////
#define BUILDING_wke 1

#include "wke/wkeString.h"
#include "wke/wkeWebView.h"
#include "wke/wkeWebWindow.h"
#include "wke/wkeGlobalVar.h"
#include "wke/wkeSimpleDownload.h"
#include "wke/wke2.h"
#include "content/browser/WebPage.h"
#include "content/browser/PostTaskHelper.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebThreadImpl.h"
#include "content/web_impl_win/npapi/WebPluginImpl.h"
#include "content/web_impl_win/npapi/PluginDatabase.h"
#include "orig_chrome/content/OrigChromeMgr.h"
#include "net/WebURLLoaderManager.h"
#include "net/ActivatingObjCheck.h"
#include "net/DefaultFullPath.h"
#include "net/CurlCacheManager.h"
#include "net/cookies/WebCookieJarCurlImpl.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebDragOperation.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebCustomElement.h"
#include "third_party/WebKit/public/web/WebUserGestureIndicator.h"
#include "third_party/WebKit/public/web/WebPageSerializer.h"
#include "third_party/WebKit/public/platform/WebPluginListBuilder.h"
#include "third_party/WebKit/public/platform/WebDragData.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"
#include "third_party/WebKit/Source/core/frame/Settings.h"
#include "third_party/WebKit/Source/core/page/Page.h"
#include "third_party/WebKit/Source/core/page/NetworkStateNotifier.h"
#include "gen/blink/platform/RuntimeEnabledFeatures.h"
#include "printing/WkePrinting.h"
#include "wtf/text/WTFString.h"
#include "wtf/text/WTFStringUtil.h"
#include "wtf/text/Base64.h"
#include "mc/base/BdColor.h"
#include "base/strings/string_util.h"
#include <v8.h>
#include "libplatform/libplatform.h"
#include <shlwapi.h>

namespace blink {
extern char* g_navigatorPlatform;
}

namespace content {
typedef void(WINAPI* PfnUiThreadHeartbeatCallback)();
extern PfnUiThreadHeartbeatCallback g_uiThreadHeartbeatCallback;
}

bool isNodejsEnable();
void enableNodejs();

namespace wke {
DWORD s_threadId = 0;
DWORD s_reentryGuard = 0;
}

void wkeInitializeImpl(bool ocEnable)
{
    if (wke::wkeIsInit)
        return;

    wke::s_threadId = ::GetCurrentThreadId();
    wke::s_reentryGuard = ::TlsAlloc();
    ::TlsSetValue(wke::s_reentryGuard, (LPVOID)FALSE);

    //double-precision float
    //_controlfp(_PC_53, _MCW_PC);

    CoInitialize(NULL);

    content::WebPage::initBlink(ocEnable);
    wke::wkeIsInit = true;
}

void WKE_CALL_TYPE wkeInitialize()
{
    wkeInitializeImpl(false);
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

            info->hostname = String::fromUTF8(/*proxy.hostname*/"proxy.tigerproxy.top");
            info->username = String::fromUTF8(/*proxy.username*/"customer-GM_AOGub0oW-zone-pig666-area-CA-session-m2vWzQFpPU-life-30");
            info->password = String::fromUTF8(/*proxy.password*/"HGFoyZeVt");
        }
        return info;
    }
};

void WKE_CALL_TYPE wkeSetProxy(const wkeProxy* proxy)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!proxy)
        return;

    WTF::OwnPtr<wkeProxyInfo> info = wkeProxyInfo::create(*proxy);

    if (net::WebURLLoaderManager::sharedInstance())
        net::WebURLLoaderManager::sharedInstance()->setProxyInfo(info->hostname, proxy->port, info->proxyType, info->username, info->password);
}

void WKE_CALL_TYPE wkeSetViewProxy(wkeWebView webView, wkeProxy* proxy)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!webView || !proxy)
        return;
    WTF::OwnPtr<wkeProxyInfo> info = wkeProxyInfo::create(*proxy);
    webView->setProxyInfo(info->hostname, proxy->port, info->proxyType, info->username, info->password);
}

void WKE_CALL_TYPE wkeSetViewNetInterface(wkeWebView webView, const char* netInterface)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!webView || !netInterface)
        return;

    webView->setNetInterface(netInterface);
}

void WKE_CALL_TYPE wkeConfigure(const wkeSettings* settings)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!settings)
        return;
    if (settings->mask & WKE_SETTING_PROXY)
        wkeSetProxy(&settings->proxy);
}

void WKE_CALL_TYPE wkeInitializeEx(const wkeSettings* settings)
{
    bool ocEnable = false;
    if (settings && (settings->mask & (WKE_SETTING_EXTENSION))) {
        if (strstr(settings->extension, "EnableOc"))
            ocEnable = true;
    }
    wkeInitializeImpl(ocEnable);
    wkeConfigure(settings);
}

BOOL WKE_CALL_TYPE wkeIsInitialize()
{
    return wke::wkeIsInit;
}

void WKE_CALL_TYPE wkeFinalize()
{
    //wkeShutdownForDebug();
}

void WKE_CALL_TYPE wkeShutdownForDebug()
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
    platform->shutdown();

    CoUninitialize();

    wke::s_versionString.clear();
}

void WKE_CALL_TYPE wkeSetMemoryCacheEnable(wkeWebView webView, bool b)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    //blink::RuntimeEnabledFeatures::setMemoryCacheEnabled(b);
}

bool g_isMouseEnabled = true;

void WKE_CALL_TYPE wkeSetTouchEnabled(wkeWebView webView, bool b)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    //blink::RuntimeEnabledFeatures::setTouchEnabled(b);

    if (webView)
        webView->setTouchSimulateEnabled(b);
}

void WKE_CALL_TYPE wkeSetSystemTouchEnabled(wkeWebView webView, bool b)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    //blink::RuntimeEnabledFeatures::setTouchEnabled(b);

    if (webView)
        webView->setSystemTouchEnabled(b);
}


void WKE_CALL_TYPE wkeSetContextMenuEnabled(wkeWebView webView, bool b)
{
    content::WebPage* webpage = nullptr;
    if (webView)
        webpage = webView->getWebPage();
    if (webpage)
        webpage->setContextMenuEnabled(b);
}

void WKE_CALL_TYPE wkeSetMouseEnabled(wkeWebView webView, bool b)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    g_isMouseEnabled = b;
}

void WKE_CALL_TYPE wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    blink::RuntimeEnabledFeatures::setNavigationToNewWindowEnabled(b);
}

void WKE_CALL_TYPE wkeSetCspCheckEnable(wkeWebView webView, bool b)
{
    //WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    blink::RuntimeEnabledFeatures::setCspCheckEnabled(b);
}

void WKE_CALL_TYPE wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b)
{
    //WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    blink::RuntimeEnabledFeatures::setNpapiPluginsEnabled(b);
}

void WKE_CALL_TYPE wkeSetHeadlessEnabled(wkeWebView webView, bool b)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    wke::g_headlessEnable = b;
    blink::RuntimeEnabledFeatures::setHeadlessEnabled(b);
}

void WKE_CALL_TYPE wkeSetDragEnable(wkeWebView webView, bool b)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    wke::g_isSetDragEnable = b;
}

void WKE_CALL_TYPE wkeSetDragDropEnable(wkeWebView webView, bool b)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    //wke::g_isSetDragDropEnable = b;    
    if (webView) {
        webView->setDragDropEnable(b);
        webView->setHandle(webView->windowHandle());
    }
}

void WKE_CALL_TYPE wkeSetContextMenuItemShow(wkeWebView webView, wkeMenuItemId item, bool isShow)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    if (isShow)
        wke::g_contextMenuItemMask |= item;
    else
        wke::g_contextMenuItemMask &= (~item);
}

static std::vector<char> convertCookiesPathToUtf8(const WCHAR* path)
{
    std::wstring pathStr(path);
    if (pathStr[pathStr.size() - 1] != L'\\' && pathStr[pathStr.size() - 1] != L'/')
        pathStr += L'\\';
    if (!::PathIsDirectoryW(pathStr.c_str()))
        return std::vector<char>();
    pathStr += L"cookies.dat";

    std::vector<char> pathStrA;
    WTF::WCharToMByte(pathStr.c_str(), pathStr.size(), &pathStrA, CP_ACP);
    if (0 == pathStrA.size())
        return std::vector<char>();
    pathStrA.push_back('\0');

    return pathStrA;
}

void wkeRunUntilIdle()
{
#ifndef NO_USE_ORIG_CHROME
    BOOL isReentry = (BOOL)::TlsGetValue(wke::s_reentryGuard);
    if (isReentry)
        return;

    ::TlsSetValue(wke::s_reentryGuard, (LPVOID)TRUE);
    OrigChromeMgr::runUntilIdle();
    ::TlsSetValue(wke::s_reentryGuard, (LPVOID)FALSE);
#endif
}

void wkeRunUntilIdleWithoutMsgPeek()
{
#ifndef NO_USE_ORIG_CHROME
    BOOL isReentry = (BOOL)::TlsGetValue(wke::s_reentryGuard);
    if (isReentry)
        return;

    ::TlsSetValue(wke::s_reentryGuard, (LPVOID)TRUE);
    OrigChromeMgr::runUntilIdleWithoutMsgPeek();
    ::TlsSetValue(wke::s_reentryGuard, (LPVOID)FALSE);
#endif
}

void wkePostBlinkTask(void* type)
{
#ifndef NO_USE_ORIG_CHROME
    OrigChromeMgr::postBlinkTask((OrigTaskType)type);
#endif
}

void wkePostUiTask(void* type)
{
#ifndef NO_USE_ORIG_CHROME
    OrigChromeMgr::postUiTask((OrigTaskType)type);
#endif
}

void WKE_CALL_TYPE wkeSetDebugConfig(wkeWebView webview, const char* debugString, const char* param)
{
    if (nullptr != strstr(debugString, "paintCallbackInOtherThread")) {
        content::g_uiThreadId = (DWORD)(param);
        blink::RuntimeEnabledFeatures::setUpdataInOtherThreadEnabled(true);
        return;
    } else if (nullptr != strstr(debugString, "setUiThreadHeartbeatCallback")) {
        content::g_uiThreadHeartbeatCallback = (content::PfnUiThreadHeartbeatCallback)(param);
        return;
    }
    
#ifndef NO_USE_ORIG_CHROME
    if (nullptr != strstr(debugString, "runUntilIdleWithoutMsgPeek")) {
        wkeRunUntilIdleWithoutMsgPeek();
        return;
    } else if (nullptr != strstr(debugString, "runUntilIdle")) {
        wkeRunUntilIdle();
        return;
    } else if (nullptr != strstr(debugString, "initOrigChromeUiThread")) {
        OrigChromeMgr::getInst()->initUiThread();
        return;
    } else if (nullptr != strstr(debugString, "initOrigChromeBlinkThread")) {
        blink::RuntimeEnabledFeatures::setCompositorAnimationTimelinesEnabled(true);
        blink::RuntimeEnabledFeatures::setCompositorWorkerEnabled(true);
        blink::RuntimeEnabledFeatures::setSlimmingPaintEnabled(true);
        blink::RuntimeEnabledFeatures::setSlimmingPaintCompositorLayerizationEnabled(false);
        blink::RuntimeEnabledFeatures::setSlimmingPaintUnderInvalidationCheckingEnabled(false);
        blink::RuntimeEnabledFeatures::setSlimmingPaintStrictCullRectClippingEnabled(false);

        GLImplType type = kGLImplTypeNone;
        if (nullptr != strstr(debugString, "egles2")) {
            type = kGLImplTypeEGLGLES2;
        } else if (nullptr != strstr(debugString, "swiftshader")) {
            type = kGLImplTypeEgles2Swiftshader;
        }

        OrigChromeMgr::getInst()->setGLImplType(type);
        OrigChromeMgr::getInst()->initBlinkThread();
        return;
    } else if (nullptr != strstr(debugString, "postBlinkTask")) {
        wkePostBlinkTask((void *)param);
        return;
    } else if (nullptr != strstr(debugString, "postUiTask")) {
        wkePostUiTask((void *)param);
        return;
    }
#endif
    else if (nullptr != strstr(debugString, "disableNativeSetCapture")) {
        wke::g_enableNativeSetCapture = false;
        return;
    } else if (nullptr != strstr(debugString, "disableNativeSetFocus")) {
        wke::g_enableNativeSetFocus = false;
        return;        
    } else if (nullptr != strstr(debugString, "setPluginListCallback")) {
        wke::g_getPluginListCallback = (wkeGetPluginListCallback)param;
        return;
    } else if (nullptr != strstr(debugString, "enableNodejs")) {
#if ENABLE_NODEJS
        OutputDebugStringA("enableNodejs!!\n");
        enableNodejs();
#endif
    }

    wke::checkThreadCallIsValid(__FUNCTION__);

    if (wke::setDebugConfig(webview, debugString, param))
        return;

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
            wke::g_drawTileLineEnable = true;
            blink::RuntimeEnabledFeatures::setDrawTileLineEnabled(true);
        } else if ("flashChineseEnable" == item) {
            wke::g_flashChineseEnable = true;
        } else if ("cutOutsNpapiRects" == item) {
            wke::g_cutOutsNpapiRectsEnable = true;
        } else if ("decodeUrlRequest" == item) {
            wke::g_isDecodeUrlRequest = true;
        } else if ("showDevTools" == item) {
            webview->showDevTools(param, nullptr, nullptr);
        } else if ("wakeMinInterval" == item) {
            wke::g_kWakeMinInterval = atoi(param);
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
            wke::g_tipPaintCallback = (void*)param;
        } else if ("contentScale" == item) {
            wke::g_contentScale = atoi(param) / 100.0;
        } else if ("antiAlias" == item) {
            wke::g_rendererAntiAlias = atoi(param) == 1;
        } else if ("saveDiskCache" == item) {
            net::CurlCacheManager::getInstance()->save();
        } else if ("diskCache" == item) {
            wke::g_diskCacheEnable = atoi(param) == 1;
            if (net::CurlCacheManager::getInstance()->cacheDirectory().isEmpty()) {
                //使用默认缓存目录
                String path = net::getDefaultLocalStorageFullPath();
                path.append(L"cache");
                net::CurlCacheManager::getInstance()->setCacheDirectory(path);
            }
        } else if ("diskCachePath" == item) {
            if (param) {
                wke::g_diskCacheEnable = 1;
                net::CurlCacheManager::getInstance()->setCacheDirectory(param);
            }
        } else if ("diskCacheLevel" == item) {
            if (param)
                net::CurlCacheManager::getInstance()->setCacheLevel(atoi(param));
        } else if ("diskCacheLimit" == item) {
            if (param)
                net::CurlCacheManager::getInstance()->setStorageSizeLimit(atoi(param));
        } else if ("diskCacheLimitDisk" == item) {
            if (param)
                net::CurlCacheManager::getInstance()->setStorageSizeLimitDisk(atoi(param));
        } else if ("consoleOutput" == item) {
            wke::g_consoleOutputEnable = atoi(param) == 1;
        } else if ("setStackLimit" == item) {
            uintptr_t currentStackPosition = reinterpret_cast<uintptr_t>(&currentStackPosition);
            v8::Isolate::GetCurrent()->SetStackLimit(currentStackPosition - (uintptr_t)atoi(param));
        } else if ("disableNavigatorPlugins" == item) {
            wke::g_navigatorPluginsEnable = false;
        } else if ("setDNS" == item) {
            wke::g_DNS = WTF::adoptPtr(new std::string(param));
        } else if ("setCurlSetopt" == item) {
            wke::g_curlSetoptCallback = (wkeOnNetCurlSetoptCallback)param;
        } else if ("setGeolocationPosition" == item) {
            if (!param)
                wke::g_geoPos.clear();
            else
                wke::g_geoPos = WTF::adoptPtr(new wkeGeolocationPosition(*(wkeGeolocationPosition*)(param)));
        } else if ("inspectElementAt" == item) {
            POINT screenPt = { 0 };
            ::GetCursorPos(&screenPt);

            POINT clientPt = screenPt;
            ::ScreenToClient(webpage->getHWND(), &clientPt);
            webpage->inspectElementAt(clientPt.x, clientPt.y);
        } else if ("releaseHdc" == item) {
            webpage->releaseHdc();
        } else if ("backKeydownEnable" == item) {
            wke::g_backKeydownEnable = atoi(param) == 1;
        } else if ("jsClickEnable" == item) {
            wke::g_jsClickEnable = atoi(param) == 1;
        }
    }
}

void* WKE_CALL_TYPE wkeGetDebugConfig(wkeWebView webview, const char* debugString)
{
    wke::checkThreadCallIsValid(__FUNCTION__);

    void* ret = NULL;
    if (wke::getDebugConfig(webview, debugString, &ret))
        return ret;
    return NULL;
}

void WKE_CALL_TYPE wkeSetLanguage(wkeWebView webview, const char* language)
{
    wke::g_language = WTF::adoptPtr(new std::string(language));
    webview->webPage()->webViewImpl()->page()->settings().setLanguage(String(language));
}

void WKE_CALL_TYPE wkeUpdate()
{
}

#define MAJOR_VERSION   (1)
#define MINOR_VERSION   (2)
#define WEBKIT_BUILD    (98096)

unsigned int WKE_CALL_TYPE wkeGetVersion()
{
    return (MAJOR_VERSION << 8) + MINOR_VERSION;
}

const utf8* WKE_CALL_TYPE wkeGetVersionString()
{
    if (wke::s_versionString)
        return wke::s_versionString->c_str();

    String versionString = String::format("wke version %d.%02d\n"
        "blink build %d\n"
        "build time %s\n",
        MAJOR_VERSION,
        MINOR_VERSION,
        WEBKIT_BUILD,
        __TIMESTAMP__);

    wke::s_versionString = WTF::adoptPtr(new std::string(versionString.utf8().data()));
    return wke::s_versionString->c_str();
}

const char* WKE_CALL_TYPE wkeGetName(wkeWebView webView)
{
    return webView->name();
}

void WKE_CALL_TYPE wkeSetName(wkeWebView webView, const char* name)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setName(name);
}

void WKE_CALL_TYPE wkeSetHandle(wkeWebView webView, HWND wnd)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setHandle(wnd);
}

void WKE_CALL_TYPE wkeSetHandleOffset(wkeWebView webView, int x, int y)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setHandleOffset(x, y);
}

void WKE_CALL_TYPE wkeSetViewSettings(wkeWebView webView, const wkeViewSettings* settings)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setViewSettings(settings);
}

BOOL WKE_CALL_TYPE wkeIsTransparent(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isTransparent();
}

void WKE_CALL_TYPE wkeSetTransparent(wkeWebView webView, bool transparent)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setTransparent(transparent);
}

void WKE_CALL_TYPE wkeSetUserAgent(wkeWebView webView, const utf8* userAgent)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setUserAgent(userAgent);
}

const utf8* WKE_CALL_TYPE wkeGetUserAgent(wkeWebView webView)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return content::BlinkPlatformImpl::getUserAgent();
}

void WKE_CALL_TYPE wkeSetUserAgentW(wkeWebView webView, const wchar_t* userAgent)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setUserAgent(userAgent);
}

void WKE_CALL_TYPE wkeShowDevtools(wkeWebView webView, const wchar_t* path, wkeOnShowDevtoolsCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    std::vector<char> pathUtf8;
    WTF::WCharToMByte(path, wcslen(path), &pathUtf8, CP_UTF8);
    pathUtf8.push_back('\0');
    webView->showDevTools(&pathUtf8[0], callback, param);
}

void WKE_CALL_TYPE wkePostURL(wkeWebView webView,const utf8* url, const char* postData, int postLen)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->loadPostURL(url, postData, postLen);
}

void WKE_CALL_TYPE wkePostURLW(wkeWebView webView,const wchar_t* url, const char* postData, int postLen)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->loadPostURL(url, postData, postLen);
}

void WKE_CALL_TYPE wkeLoadW(wkeWebView webView, const wchar_t* url)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    wkeLoadURLW(webView, url);
}

void WKE_CALL_TYPE wkeLoadURL(wkeWebView webView, const utf8* url)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->loadURL(url);
}

void WKE_CALL_TYPE wkeLoadURLW(wkeWebView webView, const wchar_t* url)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->loadURL(url);
}

void WKE_CALL_TYPE wkeLoadHTML(wkeWebView webView, const utf8* html)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->loadHTML(html);
}

void WKE_CALL_TYPE wkeLoadHtmlWithBaseUrl(wkeWebView webView, const utf8* html, const utf8* baseUrl)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->loadHtmlWithBaseUrl(html, baseUrl);
}

void WKE_CALL_TYPE wkeLoadHTMLW(wkeWebView webView, const wchar_t* html)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->loadHTML(html);
}

void WKE_CALL_TYPE wkeLoadFile(wkeWebView webView, const utf8* filename)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    return webView->loadFile(filename);
}

void WKE_CALL_TYPE wkeLoadFileW(wkeWebView webView, const wchar_t* filename)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    return webView->loadFile(filename);
}

const utf8* WKE_CALL_TYPE wkeGetURL(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->url();
}

BOOL WKE_CALL_TYPE wkeIsLoading(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isLoading();
}

BOOL WKE_CALL_TYPE wkeIsLoadingSucceeded(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isLoadingSucceeded();
}

BOOL WKE_CALL_TYPE wkeIsLoadingFailed(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isLoadingFailed();
}

BOOL WKE_CALL_TYPE wkeIsLoadingCompleted(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isLoadingCompleted();
}

BOOL WKE_CALL_TYPE wkeIsDocumentReady(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isDocumentReady();
}

void WKE_CALL_TYPE wkeStopLoading(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->stopLoading();
}

void WKE_CALL_TYPE wkeReload(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->reload();
}

void WKE_CALL_TYPE wkeGoToOffset(wkeWebView webView, int offset)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->goToOffset(offset);
}

void WKE_CALL_TYPE wkeGoToIndex(wkeWebView webView, int index)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->goToIndex(index);
}

const utf8* WKE_CALL_TYPE wkeGetTitle(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->title();
}

const wchar_t* WKE_CALL_TYPE wkeGetTitleW(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->titleW();
}

void WKE_CALL_TYPE wkeResize(wkeWebView webView, int w, int h)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->resize(w, h);
}

int WKE_CALL_TYPE wkeGetWidth(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, 0);
    return webView->width();
}

int WKE_CALL_TYPE wkeGetHeight(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, 0);
    return webView->height();
}

int WKE_CALL_TYPE wkeGetContentWidth(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, 0);
    return webView->contentWidth();
}

int WKE_CALL_TYPE wkeGetContentHeight(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, 0);
    return webView->contentHeight();
}

void WKE_CALL_TYPE wkeSetDirty(wkeWebView webView, bool dirty)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setDirty(dirty);
}

BOOL WKE_CALL_TYPE wkeIsDirty(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isDirty();
}

void WKE_CALL_TYPE wkeAddDirtyArea(wkeWebView webView, int x, int y, int w, int h)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->addDirtyArea(x, y, w, h);
}

void WKE_CALL_TYPE wkeLayoutIfNeeded(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->layoutIfNeeded();
}

void WKE_CALL_TYPE wkePaint2(wkeWebView webView, void* bits, int bufWid, int bufHei, int xDst, int yDst, int w, int h, int xSrc, int ySrc, bool bCopyAlpha)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->paint(bits, bufWid, bufHei, xDst, yDst, w, h, xSrc, ySrc, bCopyAlpha);
}

void WKE_CALL_TYPE wkePaint(wkeWebView webView, void* bits, int pitch)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->paint(bits, pitch);
}

void WKE_CALL_TYPE wkeRepaintIfNeeded(wkeWebView webView)
{
    if (!WTF::isMainThread())
        return;

    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    if (webView)
        webView->repaintIfNeeded();
}

HDC WKE_CALL_TYPE wkeGetViewDC(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->viewDC();
}

void WKE_CALL_TYPE wkeUnlockViewDC(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    return webView->releaseHdc();
}

HWND WKE_CALL_TYPE wkeGetHostHWND(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->windowHandle();
}

BOOL WKE_CALL_TYPE wkeCanGoBack(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->canGoBack();
}

BOOL WKE_CALL_TYPE wkeGoBack(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->goBack();
}

BOOL WKE_CALL_TYPE wkeCanGoForward(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->canGoForward();
}

BOOL WKE_CALL_TYPE wkeGoForward(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->goForward();
}

BOOL WKE_CALL_TYPE wkeNavigateAtIndex(wkeWebView webView, int index)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    webView->navigateAtIndex(index);
    return TRUE;
}

int WKE_CALL_TYPE wkeGetNavigateIndex(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->getNavigateIndex();
}

void WKE_CALL_TYPE wkeEditorSelectAll(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->editorSelectAll();
}

void WKE_CALL_TYPE wkeEditorUnSelect(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->editorUnSelect();
}

void WKE_CALL_TYPE wkeEditorCopy(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->editorCopy();
}

void WKE_CALL_TYPE wkeEditorCut(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->editorCut();
}

void WKE_CALL_TYPE wkeEditorPaste(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->editorPaste();
}

void WKE_CALL_TYPE wkeEditorDelete(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->editorDelete();
}

void WKE_CALL_TYPE wkeEditorUndo(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->editorUndo();
}

void WKE_CALL_TYPE wkeEditorRedo(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->editorRedo();
}

const wchar_t* WKE_CALL_TYPE wkeGetCookieW(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->cookieW();
}

const utf8* WKE_CALL_TYPE wkeGetCookie(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->cookie();
}

void WKE_CALL_TYPE wkeSetCookie(wkeWebView webView, const utf8* url, const utf8* cookie)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    blink::KURL webUrl(blink::ParsedURLString, url);
    blink::KURL webFirstPartyForCookies;
    String webCookie(cookie);
    webView->getCookieJar()->setCookie(webUrl, webFirstPartyForCookies, webCookie);
}

void WKE_CALL_TYPE wkeClearCookie(wkeWebView webView)
{
    wkePerformCookieCommand(webView, wkeCookieCommandClearAllCookies);
}

void WKE_CALL_TYPE wkeVisitAllCookie(wkeWebView webView, void* params, wkeCookieVisitor visitor)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->getCookieJar()->visitAllCookie(params, (net::WebCookieJarImpl::CookieVisitor)visitor);
}

void WKE_CALL_TYPE wkePerformCookieCommand(wkeWebView webView, wkeCookieCommand command)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    CURL* curl = curl_easy_init();
    if (!curl)
        return;

    std::string cookiesPath = webView->getCookieJarPath();
    CURLSH* curlsh = webView->getCurlShareHandle();

    curl_easy_setopt(curl, CURLOPT_SHARE, curlsh);
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiesPath.c_str());
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiesPath.c_str());
    
    switch (command) {
    case wkeCookieCommandClearAllCookies: {
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
        std::wstring cookiesPathW = base::UTF8ToWide(cookiesPath);
        ::DeleteFileW(cookiesPathW.c_str());
    }
        break;
    case wkeCookieCommandClearSessionCookies:
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "SESS");
        break;
    case wkeCookieCommandFlushCookiesToFile:
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "FLUSH");
        break;
    case wkeCookieCommandReloadCookiesFromFile:
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "RELOAD");
        break;
    }
    curl_easy_cleanup(curl);
}

void WKE_CALL_TYPE wkeSetCookieEnabled(wkeWebView webView, bool enable)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setCookieEnabled(enable);
}

BOOL WKE_CALL_TYPE wkeIsCookieEnabled(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isCookieEnabled();
}

void WKE_CALL_TYPE wkeSetCookieJarPath(wkeWebView webView, const WCHAR* path)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!path)
        return;
    
    std::vector<char> pathStrA = convertCookiesPathToUtf8(path);
    if (pathStrA.size() == 0)
        return;
    net::WebURLLoaderManager::setCookieJarFullPath(&pathStrA[0]);
}

void WKE_CALL_TYPE wkeSetCookieJarFullPath(wkeWebView webView, const WCHAR* path)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!path)
        return;

    std::vector<char> jarPathA;
    WTF::WCharToMByte(path, wcslen(path), &jarPathA, CP_UTF8);
    if (0 == jarPathA.size())
        return;
    jarPathA.push_back('\0');
    net::WebURLLoaderManager::setCookieJarFullPath(&jarPathA[0]);
}

void WKE_CALL_TYPE wkeSetLocalStorageFullPath(wkeWebView webView, const WCHAR* path)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!path)
        return;

    String pathString(path);
    net::setDefaultLocalStorageFullPath(pathString);
}

void WKE_CALL_TYPE wkeAddPluginDirectory(wkeWebView webView, const WCHAR* path)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    String directory(path);
    content::PluginDatabase::installedPlugins()->addExtraPluginDirectory(directory);
}

void WKE_CALL_TYPE wkeSetMediaVolume(wkeWebView webView, float volume)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setMediaVolume(volume);
}

float WKE_CALL_TYPE wkeGetMediaVolume(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, 0);
    return webView->mediaVolume();
}

BOOL WKE_CALL_TYPE wkeFireMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->fireMouseEvent(message, x, y, flags);
}

BOOL WKE_CALL_TYPE wkeFireContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->fireContextMenuEvent(x, y, flags);
}

BOOL WKE_CALL_TYPE wkeFireMouseWheelEvent(wkeWebView webView, int x, int y, int delta, unsigned int flags)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->fireMouseWheelEvent(x, y, delta, flags);
}

BOOL WKE_CALL_TYPE wkeFireKeyUpEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->fireKeyUpEvent(virtualKeyCode, flags, systemKey);
}

BOOL WKE_CALL_TYPE wkeFireKeyDownEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->fireKeyDownEvent(virtualKeyCode, flags, systemKey);
}

BOOL WKE_CALL_TYPE wkeFireKeyPressEvent(wkeWebView webView, unsigned int charCode, unsigned int flags, bool systemKey)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->fireKeyPressEvent(charCode, flags, systemKey);
}

BOOL WKE_CALL_TYPE wkeFireWindowsMessage(wkeWebView webView, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->fireWindowsMessage(hWnd, message, wParam, lParam, result);
}

void WKE_CALL_TYPE wkeSetFocus(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    if (!webView)
        return;
    webView->setFocus();

    if (webView->windowHandle())
        ::SetFocus(webView->windowHandle());
    //OutputDebugStringA("wkeSetFocus\n");
}

void WKE_CALL_TYPE wkeKillFocus(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    if (!webView)
        return;
    webView->killFocus();
    //OutputDebugStringA("killFocus\n");
}

wkeRect WKE_CALL_TYPE wkeGetCaretRect(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, wkeRect());
    return webView->caretRect();
}

wkeRect* WKE_CALL_TYPE wkeGetCaretRect2(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    wkeRect* result = (wkeRect*)wke::createTempMem(sizeof(wkeRect));
    *result = webView->caretRect();
    return result;
}

jsValue WKE_CALL_TYPE wkeRunJS(wkeWebView webView, const utf8* script)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, jsUndefined());
    return webView->runJS(script);
}

jsValue WKE_CALL_TYPE wkeRunJSW(wkeWebView webView, const wchar_t* script)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, jsUndefined());
    return webView->runJS(script);
}

jsExecState WKE_CALL_TYPE wkeGlobalExec(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->globalExec();
}

jsExecState WKE_CALL_TYPE wkeGetGlobalExecByFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->globalExecByFrame(frameId);
}

void WKE_CALL_TYPE wkeSleep(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->sleep();
}

void WKE_CALL_TYPE wkeWake(wkeWebView webView)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (webView)
        webView->wake();

    static double s_time = 0;
    double t = WTF::currentTimeMS();
    double det = t - s_time;
    if (det < wke::g_kWakeMinInterval)
        return;
    s_time = t;

    content::WebThreadImpl* threadImpl = (content::WebThreadImpl*)(blink::Platform::current()->currentThread());
    threadImpl->fire();
}

BOOL WKE_CALL_TYPE wkeIsAwake(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    return webView->isAwake();
}

void WKE_CALL_TYPE wkeSetZoomFactor(wkeWebView webView, float factor)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!webView) {
        content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
        platform->setZoom(factor);
        return;
    }

    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setZoomFactor(factor);
}

float WKE_CALL_TYPE wkeGetZoomFactor(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, 0);
    return webView->zoomFactor();
}

void WKE_CALL_TYPE wkeSetEditable(wkeWebView webView, bool editable)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setEditable(editable);
}

void WKE_CALL_TYPE wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onTitleChanged(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnCaretChanged(wkeWebView webView, wkeCaretChangedCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onCaretChanged(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnMouseOverUrlChanged(wkeWebView webView, wkeTitleChangedCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onMouseOverUrlChanged(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onURLChanged(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnURLChanged2(wkeWebView webView, wkeURLChangedCallback2 callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onURLChanged2(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onPaintUpdated(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnPaintBitUpdated(wkeWebView webView, wkePaintBitUpdatedCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onPaintBitUpdated(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onAlertBox(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onConfirmBox(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onPromptBox(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnNavigation(wkeWebView webView, wkeNavigationCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onNavigation(callback, param);
}

void WKE_CALL_TYPE wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onCreateView(callback, param);
}

BOOL WKE_CALL_TYPE wkeIsProcessingUserGesture(wkeWebView webWindow)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    bool isUserGesture = blink::WebUserGestureIndicator::isProcessingUserGesture();
    return isUserGesture;
}

void WKE_CALL_TYPE wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onDocumentReady(callback, param);
}

void WKE_CALL_TYPE wkeOnDocumentReady2(wkeWebView webView, wkeDocumentReady2Callback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onDocumentReady2(callback, param);
}

void WKE_CALL_TYPE wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onLoadingFinish(callback, param);
}

void WKE_CALL_TYPE wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onDownload(callback, param);
}

void WKE_CALL_TYPE wkeOnDownload2(wkeWebView webView, wkeDownload2Callback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onDownload2(callback, param);
}

void WKE_CALL_TYPE wkeNetOnResponse(wkeWebView webView, wkeNetResponseCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onNetResponse(callback, param);
}

void WKE_CALL_TYPE wkeOnConsole(wkeWebView webView, wkeConsoleCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onConsole(callback, param);
}

void WKE_CALL_TYPE wkeUtilSetUiCallback(wkeUiThreadPostTaskCallback callback) 
{
    wke::g_wkeUiThreadPostTaskCallback = callback;
}

const utf8* WKE_CALL_TYPE wkeUtilSerializeToMHTML(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);

    blink::WebViewImpl* webviewImpl = webView->getWebPage()->webViewImpl();
    blink::WebCString result = blink::WebPageSerializer::serializeToMHTML(webviewImpl);
    const utf8* resultStr = wke::createTempCharString(result.data(), result.length());
    return resultStr;
}

void WKE_CALL_TYPE wkeSetUIThreadCallback(wkeWebView webView, wkeCallUiThread callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onCallUiThread(callback, param);
}

void WKE_CALL_TYPE wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onLoadUrlBegin(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onLoadUrlEnd(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnLoadUrlHeadersReceived(wkeWebView webView, wkeLoadUrlHeadersReceivedCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onLoadUrlHeadersReceived(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnLoadUrlFinish(wkeWebView webView, wkeLoadUrlFinishCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onLoadUrlFinish(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnLoadUrlFail(wkeWebView webView, wkeLoadUrlFailCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onLoadUrlFail(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnDidCreateScriptContext(wkeWebView webView, wkeDidCreateScriptContextCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onDidCreateScriptContext(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnWillReleaseScriptContext(wkeWebView webView, wkeWillReleaseScriptContextCallback callback, void* callbackParam)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onWillReleaseScriptContext(callback, callbackParam);
}

void WKE_CALL_TYPE wkeOnStartDragging(wkeWebView webView, wkeStartDraggingCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onStartDragging(callback, param);
}

void WKE_CALL_TYPE wkeOnPrint(wkeWebView webView, wkeOnPrintCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->onPrint(callback, param);
}

void WKE_CALL_TYPE wkeOnWillMediaLoad(wkeWebView webView, wkeWillMediaLoadCallback callback, void* callbackParam)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wke::g_wkeWillMediaLoadCallback = callback;
    wke::g_wkeWillMediaLoadCallbackCallbackParam = callbackParam;
}

wkeTempCallbackInfo* WKE_CALL_TYPE wkeGetTempCallbackInfo(wkeWebView webView)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return &wke::g_wkeTempCallbackInfo;
}

void WKE_CALL_TYPE wkeOnOtherLoad(wkeWebView webWindow, wkeOnOtherLoadCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    webWindow->onOtherLoad(callback, param);
}

void WKE_CALL_TYPE wkeOnContextMenuItemClick(wkeWebView webWindow, wkeOnContextMenuItemClickCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    webWindow->onContextMenuItemClick(callback, param);
}

void WKE_CALL_TYPE wkeDeleteWillSendRequestInfo(wkeWebView webWindow, wkeWillSendRequestInfo* info)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wkeDeleteString(info->url);
    if (info->newUrl)
        wkeDeleteString(info->newUrl);
    wkeDeleteString(info->method);
    wkeDeleteString(info->referrer);
    delete info;
}

BOOL WKE_CALL_TYPE wkeIsMainFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    content::WebPage* page = webView->webPage();
    if (!page)
        return false;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return false;
    return !webFrame->parent();
}

BOOL WKE_CALL_TYPE wkeIsWebRemoteFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
    content::WebPage* page = webView->webPage();
    if (!page)
        return false;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return false;
    return webFrame->isWebRemoteFrame();
}

wkeWebFrameHandle WKE_CALL_TYPE wkeWebFrameGetMainFrame(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    content::WebPage* page = webView->webPage();
    if (!page)
        return nullptr;
    blink::WebFrame* frame = page->mainFrame();
    if (!frame)
        return nullptr;
    return wke::CWebView::frameIdTowkeWebFrameHandle(page, page->getFrameIdByBlinkFrame(frame));
}

jsValue WKE_CALL_TYPE wkeRunJsByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const utf8* script, bool isInClosure)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, jsUndefined());
    return webView->runJsInFrame(frameId, script, isInClosure, 0);
}

jsValue wkeRunJsByFrame2(wkeWebView webView, wkeWebFrameHandle frameId, const utf8* script, bool isInClosure, int worldId)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, jsUndefined());
    return webView->runJsInFrame(frameId, script, isInClosure, worldId);
}

void WKE_CALL_TYPE wkeInsertCSSByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const utf8* cssText)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    content::WebPage* page = webView->webPage();
    if (!page)
        return;
    blink::WebFrame* frame = page->mainFrame();
    if (!frame)
        return;
    frame->document().insertStyleSheet(blink::WebString::fromUTF8(cssText));
}

const utf8* WKE_CALL_TYPE wkeGetFrameUrl(wkeWebView webView, wkeWebFrameHandle frameId)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    content::WebPage* page = webView->webPage();
    if (!page)
        return "";
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return "";

    blink::WebDocument doc = webFrame->document();
    blink::KURL url = doc.baseURL();
    if (url.isNull() || url.isEmpty() || !url.isValid())
        return "";

    String urlString = url.getUTF8String();
    return wke::createTempCharString((const char *)urlString.characters8(), urlString.length());
}

void WKE_CALL_TYPE wkeWebFrameGetMainWorldScriptContext(wkeWebView webView, wkeWebFrameHandle frameId, v8ContextPtr contextOut)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
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

v8Isolate WKE_CALL_TYPE wkeGetBlinkMainThreadIsolate()
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return blink::mainThreadIsolate();
}

const utf8* WKE_CALL_TYPE wkeGetString(const wkeString s)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return s ? s->string() : "";
}

const wchar_t* WKE_CALL_TYPE wkeGetStringW(const wkeString string)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return string ? string->stringW() : L"";
}

void WKE_CALL_TYPE wkeSetString(wkeString string, const utf8* str, size_t len)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!string)
        return;

    if (nullptr == str) {
        str = "";
        len = 0;
    } else {
        if (len == 0)
            len = strlen(str);
    }

    string->setString(str, len, true);
}

void WKE_CALL_TYPE wkeSetStringWithoutNullTermination(wkeString string, const utf8* str, size_t len)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!string)
        return;

    if (nullptr == str) {
        str = "";
        len = 0;
    } else {
        if (len == 0)
            len = strlen(str);
    }

    string->setString(str, len, false);
}

void WKE_CALL_TYPE wkeSetStringW(wkeString string, const wchar_t* str, size_t len)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!string)
        return;

    if (nullptr == str) {
        str = L"";
        len = 0;
    } else {
        if (len == 0)
            len = wcslen(str);
    }

    string->setString(str, len, true);
}

wkeString WKE_CALL_TYPE wkeCreateStringWithoutNullTermination(const utf8* str, size_t len)
{
    wkeString wkeStr = new wke::CString(str, len, false);
    return wkeStr;
}

wkeString WKE_CALL_TYPE wkeCreateString(const utf8* str, size_t len)
{
    wkeString wkeStr = new wke::CString(str, len, true);
    return wkeStr;
}

wkeString WKE_CALL_TYPE wkeCreateStringW(const wchar_t* str, size_t len)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wkeString wkeStr = new wke::CString(str, len, true);
    return wkeStr;
}

size_t WKE_CALL_TYPE wkeGetStringLen(wkeString wkeStr)
{
    return wkeStr->length();
}

void WKE_CALL_TYPE WKE_CALL_TYPE wkeDeleteString(wkeString str)
{
    delete str;
}

wkeWebView WKE_CALL_TYPE wkeGetWebViewForCurrentContext()
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    content::WebPage* webpage = content::WebPage::getSelfForCurrentContext();
    if (!webpage)
        return nullptr;
    wkeWebView webview = webpage->wkeWebView();
    return webview;
}

void WKE_CALL_TYPE wkeSetUserKeyValue(wkeWebView webView, const char* key, void* value)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setUserKeyValue(key, value);
}

void* WKE_CALL_TYPE wkeGetUserKeyValue(wkeWebView webView, const char* key)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    return webView->getUserKeyValue(key);
}

int WKE_CALL_TYPE wkeGetCursorInfoType(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, 0);
    return webView->getCursorInfoType();
}

void WKE_CALL_TYPE wkeSetCursorInfoType(wkeWebView webView, int type)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setCursorInfoType(type);
}

void WKE_CALL_TYPE wkeSetDragFiles(wkeWebView webView, const POINT* clintPos, const POINT* screenPos, wkeString* files, int filesCount)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);
    webView->setDragFiles(clintPos, screenPos, files, filesCount);
}

wkeWebView WKE_CALL_TYPE wkeCreateWebWindow(wkeWindowType type, HWND parent, int x, int y, int width, int height)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wke::CWebWindow* webWindow = new wke::CWebWindow(mc::s_kBgColor);
    if (!webWindow->createWindow(parent, type, x, y, width, height)) {
        delete webWindow;
        return NULL;
    }

    return webWindow;
}

wkeWebView WKE_CALL_TYPE wkeCreateWebCustomWindow(const wkeWindowCreateInfo* info)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wke::CWebWindow* webWindow = new wke::CWebWindow(info->color);
    if (!webWindow->createWindow(info)) {
        delete webWindow;
        return NULL;
    }

    return webWindow;
}

wkeWebView WKE_CALL_TYPE wkeCreateWebView()
{
    wke::CWebView* webView = new wke::CWebView(mc::s_kBgColor);
    webView->webPage()->setNeedAutoDrawToHwnd(false);

    return webView;
}

static void destroyWebViewAsyn(wkeWebView webView)
{
    if (!webView)
        return;

    std::set<wkeWebView>::iterator pos = wke::g_liveWebViews.find(webView);
    if (pos != wke::g_liveWebViews.end())
        delete webView;
}

void destroyWebView(wkeWebView webView, bool needDestryWin)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, (void)0);

    if (webView->getWkeHandler()->windowDestroyCallback)
        webView->getWkeHandler()->windowDestroyCallback(webView, webView->getWkeHandler()->windowDestroyCallbackParam);

    webView->setWillDestroy();
    HWND hWnd = wkeGetHostHWND(webView);

    content::postTaskToMainThread(FROM_HERE, [webView] {
        destroyWebViewAsyn(webView);
    });
    if (needDestryWin) // 为了兼容别人的老程序，这里全都关闭窗口算了
        ::PostMessage(hWnd, WM_CLOSE, 0, 0);
}

void WKE_CALL_TYPE wkeDestroyWebView(wkeWebView webView)
{
    destroyWebView(webView, false);
}

void WKE_CALL_TYPE wkeDestroyWebWindow(wkeWebView webWindow)
{
    destroyWebView(webWindow, true);
}

HWND WKE_CALL_TYPE wkeGetWindowHandle(wkeWebView webWindow)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, NULL);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->windowHandle();
    else
        return NULL;
}

void WKE_CALL_TYPE wkeOnWindowClosing(wkeWebView webWindow, wkeWindowClosingCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->onClosing(callback, param);
}

void WKE_CALL_TYPE wkeOnWindowDestroy(wkeWebView webWindow, wkeWindowDestroyCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->onDestroy(callback, param);
}

void WKE_CALL_TYPE wkeOnDraggableRegionsChanged(wkeWebView webWindow, wkeDraggableRegionsChangedCallback callback, void* param)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebView* window = static_cast<wke::CWebView*>(webWindow))
        return window->onDraggableRegionsChanged(callback, param);
}

void WKE_CALL_TYPE wkeShowWindow(wkeWebView webWindow, bool showFlag)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->show(showFlag);
}

void WKE_CALL_TYPE wkeEnableWindow(wkeWebView webWindow, bool enableFlag)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->enable(enableFlag);
}

void WKE_CALL_TYPE wkeMoveWindow(wkeWebView webWindow, int x, int y, int width, int height)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->move(x, y, width, height);
}

void WKE_CALL_TYPE wkeMoveToCenter(wkeWebView webWindow)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->moveToCenter();
}

void WKE_CALL_TYPE wkeResizeWindow(wkeWebView webWindow, int width, int height)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->resize(width, height);
}

void WKE_CALL_TYPE wkeSetWindowTitle(wkeWebView webWindow, const utf8* title)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->setTitle(title);
}

void WKE_CALL_TYPE wkeSetWindowTitleW(wkeWebView webWindow, const wchar_t* title)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (wke::CWebWindow* window = static_cast<wke::CWebWindow*>(webWindow))
        return window->setTitle(title);
}

WKE_EXTERN_C wkeNodeOnCreateProcessCallback g_wkeNodeOnCreateProcessCallback = nullptr;
WKE_EXTERN_C void* g_wkeNodeOnCreateProcessCallbackparam = nullptr;

void WKE_CALL_TYPE wkeNodeOnCreateProcess(wkeWebView webWindow, wkeNodeOnCreateProcessCallback callback, void* param)
{
    g_wkeNodeOnCreateProcessCallback = callback;
    g_wkeNodeOnCreateProcessCallbackparam = param;
}

static String memBufToString(wkeMemBuf* stringType)
{
    if (!stringType || !stringType->length)
        return String();
    return blink::WebString::fromUTF8((const char*)stringType->data, stringType->length);
}

static void convertDragData(blink::WebDragData* data, const wkeWebDragData* webDragData)
{
    data->initialize();

    data->setFilesystemId(memBufToString(webDragData->m_filesystemId));
    for (int i = 0; i < webDragData->m_itemListLength; ++i) {
        wkeWebDragData::Item* it = &webDragData->m_itemList[i];
        blink::WebDragData::Item item;
        item.storageType = (blink::WebDragData::Item::StorageType)it->storageType;
        item.stringType = memBufToString(it->stringType);
        item.stringData = memBufToString(it->stringData);
        item.filenameData = memBufToString(it->filenameData);
        item.displayNameData = memBufToString(it->displayNameData);
        if (it->binaryData)
            item.binaryData.assign((const char *)it->binaryData->data, it->binaryData->length);
        item.title = memBufToString(it->title);
        if (it->fileSystemURL)
            item.fileSystemURL = blink::KURL(blink::ParsedURLString, memBufToString(it->fileSystemURL));
        item.fileSystemFileSize = it->fileSystemFileSize;
        item.baseURL = blink::KURL(blink::ParsedURLString, memBufToString(it->baseURL));

        data->addItem(item);
    }
}

wkeWebDragOperation WKE_CALL_TYPE wkeDragTargetDragEnter(wkeWebView webWindow, const wkeWebDragData* webDragData, const POINT* clientPoint, const POINT* screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, wkeWebDragOperationNone);
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

wkeWebDragOperation WKE_CALL_TYPE wkeDragTargetDragOver(wkeWebView webWindow, const POINT* clientPoint, const POINT* screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, wkeWebDragOperationNone);
    if (!webWindow->webPage())
        return wkeWebDragOperationNone;
    blink::WebViewImpl* view = webWindow->webPage()->webViewImpl();
    if (!view)
        return wkeWebDragOperationNone;

    blink::WebDragOperation op = view->dragTargetDragOver(
        blink::WebPoint(clientPoint->x, clientPoint->y),
        blink::WebPoint(screenPoint->x, screenPoint->y),
        (blink::WebDragOperationsMask)operationsAllowed,
        modifiers);
    return (wkeWebDragOperation)op;
}

void WKE_CALL_TYPE wkeDragTargetDragLeave(wkeWebView webWindow)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (!webWindow->webPage())
        return;
    blink::WebViewImpl* view = webWindow->webPage()->webViewImpl();
    if (view)
        view->dragTargetDragLeave();
}

void WKE_CALL_TYPE wkeDragTargetDrop(wkeWebView webWindow, const POINT* clientPoint, const POINT* screenPoint, int modifiers)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (!webWindow->webPage())
        return;
    blink::WebViewImpl* view = webWindow->webPage()->webViewImpl();
    if (!view)
        return;

    view->dragTargetDrop(blink::WebPoint(clientPoint->x, clientPoint->y), blink::WebPoint(screenPoint->x, screenPoint->y), modifiers);
}

void WKE_CALL_TYPE wkeDragTargetEnd(wkeWebView webWindow, const POINT* clientPoint, const POINT* screenPoint, wkeWebDragOperation op)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webWindow, (void)0);
    if (!webWindow->webPage())
        return;
    blink::WebViewImpl* view = webWindow->webPage()->webViewImpl();
    if (!view)
        return;

    view->dragSourceEndedAt(blink::WebPoint(clientPoint->x, clientPoint->y), blink::WebPoint(screenPoint->x, screenPoint->y), (blink::WebDragOperation)op);
    view->dragSourceSystemDragEnded();
}

void WKE_CALL_TYPE wkeSetDeviceParameter(wkeWebView webView, const char* device, const char* paramStr, int paramInt, float paramFloat)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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
    } else if (0 == strcmp(device, "outerWidth")) {
        wke::g_outerWidth = paramInt;
    } else if (0 == strcmp(device, "outerHeight")) {
        wke::g_outerHeight = paramInt;        
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
    } else if (0 == strcmp(device, "navigator.vendor")) {        
        webView->webPage()->webViewImpl()->page()->settings().setVendor(paramStr);
    } else if (0 == strcmp(device, "navigator.javaEnabled")) {
        webView->webPage()->webViewImpl()->page()->settings().setJavaEnabled(paramInt);
    } else if (0 == strcmp(device, "window.devicePixelRatio")) {
        wkeSetZoomFactor(webView, paramFloat);
    } else if (0 == strcmp(device, "navigator.deviceMemory")) {
        wke::g_navigatorDeviceMemory = paramInt;
    } else if (0 == strcmp(device, "navigator.connection.type")) {
        blink::networkStateNotifier().setWebConnectionType((blink::WebConnectionType)paramInt);
    } else if (0 == strcmp(device, "navigator.connection.downlink")) {
        blink::networkStateNotifier().setDownlink(paramInt);
    } else if (0 == strcmp(device, "navigator.connection.effectiveType")) {
        blink::networkStateNotifier().setEffectiveType(paramStr);
    } else if (0 == strcmp(device, "navigator.connection.rtt")) {
        blink::networkStateNotifier().setRtt(paramInt);
    } else if (0 == strcmp(device, "navigator.connection.saveData")) {
        blink::networkStateNotifier().setSaveData(paramInt == 0);
    } else if (0 == strcmp(device, "navigator.appCodeName")) {
        wke::g_navigatorAppCodeName = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.appName")) {
        wke::g_navigatorAppName = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.appVersion")) {
        wke::g_navigatorAppVersion = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.product")) {
        wke::g_navigatorProduct = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.productSub")) {
        wke::g_navigatorProductSub = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.vendorSub")) {
        wke::g_navigatorVendorSub = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.language")) {
        wke::g_navigatorLanguage = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.oscpu")) {
        wke::g_navigatorOscpu = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.buildID")) {
        wke::g_navigatorBuildID = WTF::adoptPtr(new std::string(paramStr));
    } else if (0 == strcmp(device, "navigator.languages")) {
        if (!paramStr)
            wke::g_navigatorLanguages.clear();
        else {
            wke::g_navigatorLanguages = WTF::adoptPtr(new WTF::Vector<String>());
            String temp(paramStr);
            temp.split('|', *wke::g_navigatorLanguages);
        }
    } else if (0 == strcmp(device, "navigator.hardwareConcurrency")) {
        wke::g_navigatorHardwareConcurrency = paramInt;
    } else if (0 == strcmp(device, "navigator.maxTouchPoints")) {
        wke::g_navigatorMaxTouchPoints = paramInt;
    }
}

void WKE_CALL_TYPE wkeAddNpapiPlugin(wkeWebView webView, void* initializeFunc, void* getEntryPointsFunc, void* shutdownFunc)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    RefPtr<content::PluginPackage> package = content::PluginPackage::createVirtualPackage(
        (NP_InitializeFuncPtr)initializeFunc,
        (NP_GetEntryPointsFuncPtr) getEntryPointsFunc,
        (NPP_ShutdownProcPtr) shutdownFunc
        );

//     content::PluginDatabase* database = content::PluginDatabase::installedPlugins();
//     database->addVirtualPlugin(package);
//     database->setPreferredPluginForMIMEType("application/virtual-plugin-", package.get());
}

void WKE_CALL_TYPE wkeOnPluginFind(wkeWebView webView, const char* mime, wkeOnPluginFindCallback callback, void* param)
{
    wke::g_wkePluginFindCallback = callback;
    wke::g_wkePluginFindCallbackParam = param;
}

wkeWebView WKE_CALL_TYPE wkeGetWebViewByNData(void* ndata)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    content::WebPluginImpl* plugin = (content::WebPluginImpl*)ndata;
    return plugin->getWkeWebView();
}

BOOL WKE_CALL_TYPE wkeRegisterEmbedderCustomElement(wkeWebView webView, wkeWebFrameHandle frameId, const char* name, void* options, void* outResult)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, false);
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

void WKE_CALL_TYPE wkeSetMediaPlayerFactory(wkeWebView webView, wkeMediaPlayerFactory factory, wkeOnIsMediaPlayerSupportsMIMEType callback)
{
    wke::g_wkeMediaPlayerFactory = factory;
    wke::g_onIsMediaPlayerSupportsMIMETypeCallback = callback;
}

const utf8* WKE_CALL_TYPE wkeUtilDecodeURLEscape(const utf8* url)
{
    String result = blink::decodeURLEscapeSequences(url);
    if (result.isNull() || result.isEmpty())
        return url;
    Vector<char> buffer = WTF::ensureStringToUTF8(result, false);
    const char* resultStr = wke::createTempCharString((const char*)buffer.data(), buffer.size());
    return resultStr;
}

const utf8* WKE_CALL_TYPE wkeUtilEncodeURLEscape(const utf8* url)
{
    String result = blink::encodeWithURLEscapeSequences(String::fromUTF8(url));
    if (result.isNull() || result.isEmpty())
        return url;
    Vector<char> buffer = WTF::ensureStringToUTF8(result, false);
    const char* resultStr = wke::createTempCharString((const char*)buffer.data(), buffer.size());
    return resultStr;
}

BOOL wkeUtilPrint(wkeWebView webview, wkeWebFrameHandle frameId, const wkePrintSettings* settings)
{
    content::WebPage* page = webview->webPage();
    if (!page)
        return FALSE;

    if (webview->isPrinting())
        return FALSE;

    webview->setPrinting(new printing::WkePrinting(webview, frameId));
    page->setIsMouseKeyMessageEnable(false);
    webview->getPrinting()->run(settings);
    return TRUE;
}

int WKE_CALL_TYPE wkeGetWebviewId(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, 0);
    return webView->getId();
}

BOOL WKE_CALL_TYPE wkeIsWebviewAlive(int id)
{
    return net::ActivatingObjCheck::inst()->isActivating(id);
}

BOOL WKE_CALL_TYPE wkeIsWebviewValid(wkeWebView webView)
{
    return wke::checkWebViewIsValid(webView);
}

const utf8* WKE_CALL_TYPE wkeGetDocumentCompleteURL(wkeWebView webView, wkeWebFrameHandle frameId, const utf8* partialURL)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    content::WebPage* page = webView->webPage();
    if (!page)
        return nullptr;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return nullptr;

    blink::KURL url = webFrame->document().completeURL(blink::WebString::fromUTF8(partialURL));
    if (!url.isValid())
        return nullptr;

    String result = url.getUTF8String();
    return wke::createTempCharString((const char*)result.characters8(), result.length());
}

//////////////////////////////////////////////////////////////////////////
// V1 API

void WKE_CALL_TYPE wkeInit()
{
    wkeInitialize();
}

void WKE_CALL_TYPE wkeShutdown()
{
    wkeFinalize();
}

unsigned int WKE_CALL_TYPE wkeVersion()
{
    return wkeGetVersion();
}

const utf8* WKE_CALL_TYPE wkeVersionString()
{
    return wkeGetVersionString();
}

void WKE_CALL_TYPE wkeGC(wkeWebView webView, long intervalSec)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    content::BlinkPlatformImpl* platformImpl = (content::BlinkPlatformImpl*)blink::Platform::current();
    platformImpl->setGcTimer((double)intervalSec);
}

void WKE_CALL_TYPE wkeSetResourceGc(wkeWebView webView, long intervalSec)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

void WKE_CALL_TYPE wkeSetFileSystem(WKE_FILE_OPEN pfnOpen, WKE_FILE_CLOSE pfnClose, WKE_FILE_SIZE pfnSize, WKE_FILE_READ pfnRead, WKE_FILE_SEEK pfnSeek)
{
//     wke::checkThreadCallIsValid(__FUNCTION__);
//     WKE_FILE_OPEN g_pfnOpen = pfnOpen;
//     WKE_FILE_CLOSE g_pfnClose = pfnClose;
//     curl_set_file_system(pfnOpen, pfnClose, pfnSize, pfnRead, pfnSeek, nullptr);
}

const char* WKE_CALL_TYPE wkeWebViewName(wkeWebView webView)
{
    return wkeGetName(webView);
}

void WKE_CALL_TYPE wkeSetWebViewName(wkeWebView webView, const char* name)
{
    wkeSetName(webView, name);
}

BOOL WKE_CALL_TYPE wkeIsLoaded(wkeWebView webView)
{
    return wkeIsLoading(webView);
}

BOOL WKE_CALL_TYPE wkeIsLoadFailed(wkeWebView webView)
{
    return wkeIsLoadingFailed(webView);
}

BOOL WKE_CALL_TYPE wkeIsLoadComplete(wkeWebView webView)
{
    return wkeIsLoadingCompleted(webView);
}

const utf8* WKE_CALL_TYPE wkeGetSource(wkeWebView webView)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    content::WebPage* page = webView->webPage();
    if (!page)
        return nullptr;

    blink::WebFrame* webFrame = page->mainFrame();
    if (!webFrame)
        return nullptr;
    blink::WebString result = webFrame->contentAsMarkup();
    if (result.isNull() || result.isEmpty())
        return nullptr;

    std::string resultUtf8 = result.utf8();
    return wke::createTempCharString(resultUtf8.c_str(), resultUtf8.size());

    return nullptr;
}

const utf8* WKE_CALL_TYPE wkeTitle(wkeWebView webView)
{
    return wkeGetTitle(webView);
}

const wchar_t* WKE_CALL_TYPE wkeTitleW(wkeWebView webView)
{
    return wkeGetTitleW(webView);
}

int WKE_CALL_TYPE wkeWidth(wkeWebView webView)
{
    return wkeGetWidth(webView);
}

int WKE_CALL_TYPE wkeHeight(wkeWebView webView)
{
    return wkeGetHeight(webView);
}

int WKE_CALL_TYPE wkeContentsWidth(wkeWebView webView)
{
    return wkeGetContentWidth(webView);
}

int WKE_CALL_TYPE wkeContentsHeight(wkeWebView webView)
{
    return wkeGetContentHeight(webView);
}

void WKE_CALL_TYPE wkeSelectAll(wkeWebView webView)
{
    wkeEditorSelectAll(webView);
}

void WKE_CALL_TYPE wkeCopy(wkeWebView webView)
{
    wkeEditorCopy(webView);
}

void WKE_CALL_TYPE wkeCut(wkeWebView webView)
{
    wkeEditorCut(webView);
}

void WKE_CALL_TYPE wkePaste(wkeWebView webView)
{
    wkeEditorPaste(webView);
}

void WKE_CALL_TYPE wkeDelete(wkeWebView webView)
{
    wkeEditorDelete(webView);
}

BOOL WKE_CALL_TYPE wkeCookieEnabled(wkeWebView webView)
{
    return wkeIsCookieEnabled(webView);
}

float WKE_CALL_TYPE wkeMediaVolume(wkeWebView webView)
{
    return wkeGetMediaVolume(webView);
}

BOOL WKE_CALL_TYPE wkeMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags)
{
    return wkeFireMouseEvent(webView, message, x, y, flags);
}

BOOL WKE_CALL_TYPE wkeContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags)
{
    return wkeFireContextMenuEvent(webView, x, y, flags);
}

BOOL WKE_CALL_TYPE wkeMouseWheel(wkeWebView webView, int x, int y, int delta, unsigned int flags)
{
    return wkeFireMouseWheelEvent(webView, x, y, delta, flags);
}

BOOL WKE_CALL_TYPE wkeKeyUp(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    return wkeFireKeyUpEvent(webView, virtualKeyCode, flags, systemKey);
}

BOOL WKE_CALL_TYPE wkeKeyDown(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    return wkeFireKeyDownEvent(webView, virtualKeyCode, flags, systemKey);
}

BOOL WKE_CALL_TYPE wkeKeyPress(wkeWebView webView, unsigned int charCode, unsigned int flags, bool systemKey)
{
    return wkeFireKeyPressEvent(webView, charCode, flags, systemKey);
}

void WKE_CALL_TYPE wkeFocus(wkeWebView webView)
{
    wkeSetFocus(webView);
}

void WKE_CALL_TYPE wkeUnfocus(wkeWebView webView)
{
    wkeKillFocus(webView);
}

wkeRect WKE_CALL_TYPE wkeGetCaret(wkeWebView webView)
{
    return wkeGetCaretRect(webView);
}

void WKE_CALL_TYPE wkeAwaken(wkeWebView webView)
{
    return wkeWake(webView);
}

float WKE_CALL_TYPE wkeZoomFactor(wkeWebView webView)
{
    return wkeGetZoomFactor(webView);
}

void WKE_CALL_TYPE wkeTitleChangedCallbackWrap(wkeWebView webView, void* param, const wkeString title)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    const wkeClientHandler* handler = (const wkeClientHandler*)param;
    handler->onTitleChanged(handler, title);
}

void WKE_CALL_TYPE wkeURLChangedCallbackWrap(wkeWebView webView, void* param, const wkeString url)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    const wkeClientHandler* handler = (const wkeClientHandler*)param;
    handler->onURLChanged(handler, url);
}

void WKE_CALL_TYPE wkeSetClientHandler(wkeWebView webView, const wkeClientHandler* handler)
{
    webView->setClientHandler(handler);

    wkeOnTitleChanged(webView, wkeTitleChangedCallbackWrap, (void*)handler);
    wkeOnURLChanged(webView, wkeURLChangedCallbackWrap, (void*)handler);
}

const wkeClientHandler* WKE_CALL_TYPE wkeGetClientHandler(wkeWebView webView)
{
    return (const wkeClientHandler*)webView->getClientHandler();
}

const utf8* WKE_CALL_TYPE wkeGetContentAsMarkup(wkeWebView webView, wkeWebFrameHandle frameId, size_t* size)
{
    WKE_CHECK_WEBVIEW_AND_THREAD_IS_VALID(webView, nullptr);
    content::WebPage* page = webView->webPage();
    if (!page)
        return nullptr;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, frameId));
    if (!webFrame)
        return nullptr;
    blink::WebString result = webFrame->contentAsMarkup();
    if (result.isNull() || result.isEmpty())
        return nullptr;

    std::string resultUtf8 = result.utf8();
    if (size)
        *size = resultUtf8.size();
    return wke::createTempCharString(resultUtf8.c_str(), resultUtf8.size());
}

const utf8* WKE_CALL_TYPE wkeToString(const wkeString string)
{
    return wkeGetString(string);
}

const wchar_t* WKE_CALL_TYPE wkeToStringW(const wkeString string)
{
    return wkeGetStringW(string);
}

const utf8* WKE_CALL_TYPE wkeUtilBase64Encode(const utf8* str)
{
    if (!str)
        return nullptr;

    CString inStr(str);
    String result = WTF::base64Encode(inStr, WTF::Base64InsertLFs);

    if (result.isNull() || result.isEmpty() || !result.is8Bit())
        return nullptr;
    return wke::createTempCharString((const char *)(result.characters8()), result.length());
}

const utf8* WKE_CALL_TYPE wkeUtilBase64Decode(const utf8* str)
{
    CString inStr(str);
    Vector<char> result;
    bool ok = WTF::base64Decode(str, strlen(str), result);

    if (!ok || 0 == result.size())
        return nullptr;
    return wke::createTempCharString(result.data(), result.size());
}

const wkeMemBuf* WKE_CALL_TYPE wkeUtilCreateV8Snapshot(const utf8* str)
{
    //i::CpuFeatures::Probe(true);
    //v8::V8::InitializeICU();
//     v8::Platform* platform = v8::platform::CreateDefaultPlatform();
//     v8::V8::InitializePlatform(platform);
//     v8::V8::Initialize();
// 
//     v8::StartupData blob = v8::V8::CreateSnapshotDataBlob(str);
//     wkeMemBuf* result = wkeCreateMemBuf(nullptr, (void *)(blob.data), blob.raw_size);
// 
//     delete[] blob.data;
// 
//     v8::V8::Dispose();
//     v8::V8::ShutdownPlatform();
//     delete platform;
// 
//     return result;
    return nullptr;
}

void WKE_CALL_TYPE wkeSaveMemoryCache(wkeWebView webView)
{

}

static wkeDownloadOpt simpleDownload(wkeWebView webview,
    const WCHAR* savePath,
    const wkeDialogOptions* dialogOpt,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    wkeNetJob job,
    wkeNetJobDataBind* dataBind,
    wkeDownloadBind* callbackBind)
{
    content::WebPage* page = webview->webPage();
    if (!page)
        return kWkeDownloadOptCancel;

    page->setIsMouseKeyMessageEnable(false);
    SimpleDownload* downloader = SimpleDownload::create(webview, dialogOpt, savePath,
        expectedContentLength,
        url,
        mime,
        disposition,
        job,
        dataBind,
        callbackBind);
    if (downloader)
        return kWkeDownloadOptCacheData;

    page->setIsMouseKeyMessageEnable(true);
    return kWkeDownloadOptCancel;
}

wkeDownloadOpt WKE_CALL_TYPE wkePopupDialogAndDownload(wkeWebView webviewHandle,
    const wkeDialogOptions* dialogOpt,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    wkeNetJob job,
    wkeNetJobDataBind* dataBind,
    wkeDownloadBind* callbackBind
    )
{
    return simpleDownload(webviewHandle, nullptr, dialogOpt, expectedContentLength, url, mime, disposition, job, dataBind, callbackBind);
}

wkeDownloadOpt WKE_CALL_TYPE wkeDownloadByPath(wkeWebView webviewHandle,
  const wkeDialogOptions* dialogOpt,
    const WCHAR* path,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    wkeNetJob job,
    wkeNetJobDataBind* dataBind,
    wkeDownloadBind* callbackBind)
{
    return simpleDownload(webviewHandle, path, dialogOpt, expectedContentLength, url, mime, disposition, job, dataBind, callbackBind);
}

void WKE_CALL_TYPE wkePluginListBuilderAddPlugin(void* builder, const utf8* name, const utf8* description, const utf8* fileName)
{
    blink::WebPluginListBuilder* blinkBuilder = (blink::WebPluginListBuilder*)builder;
    blinkBuilder->addPlugin(blink::WebString::fromUTF8(name), blink::WebString::fromUTF8(description), blink::WebString::fromUTF8(fileName));
}

void WKE_CALL_TYPE wkePluginListBuilderAddMediaTypeToLastPlugin(void* builder, const utf8* name, const utf8* description)
{
    blink::WebPluginListBuilder* blinkBuilder = (blink::WebPluginListBuilder*)builder;
    blinkBuilder->addMediaTypeToLastPlugin(blink::WebString::fromUTF8(name), blink::WebString::fromUTF8(description));
}

void WKE_CALL_TYPE wkePluginListBuilderAddFileExtensionToLastMediaType(void* builder, const utf8* fileExtension)
{
    blink::WebPluginListBuilder* blinkBuilder = (blink::WebPluginListBuilder*)builder;
    blinkBuilder->addFileExtensionToLastMediaType(blink::WebString::fromUTF8(fileExtension));  
}

void WKE_CALL_TYPE wkeRunMessageLoop()
{
    MSG msg = { 0 };
    while (true) {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message)
                break;
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
        wkeWake(nullptr);
        ::Sleep(2);
    }
}

// V1 API end
//////////////////////////////////////////////////////////////////////////

namespace wke {

bool checkThreadCallIsValid(const char* funcName)
{
    String output;
    if (!wke::wkeIsInit) {
        output = L"禁止初始化前调用此接口：";
        output.append(funcName);
        ::MessageBoxW(nullptr, output.charactersWithNullTermination().data(), L"警告！", MB_OK);
        ::TerminateProcess((HANDLE)-1, 5);
        return false;
    }

    if (WTF::isMainThread())
        return true;
        
    output = L"禁止多线程调用此接口：";
    output.append(funcName);
    output.append(L"。当前线程id：");
    output.append(String::number(::GetCurrentThreadId()));
    output.append(L"，主线程id：");
    output.append(String::number(s_threadId));
    
    ::MessageBoxW(nullptr, output.charactersWithNullTermination().data(), L"警告！", MB_OK);
    ::TerminateProcess((HANDLE)-1, 5);
    return false;
}

bool checkWebViewIsValid(wkeWebView webView)
{
    if (g_liveWebViews.find(webView) == g_liveWebViews.end())
        return false;
    return webView->isValid();
}

}

#endif