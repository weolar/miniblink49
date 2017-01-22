#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

//////////////////////////////////////////////////////////////////////////
#define BUILDING_wke 1

#include "content/browser/WebPage.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "net/WebURLLoaderManager.h"

//cexer: 必须包含在后面，因为其中的 wke.h -> windows.h 会定义 max、min，导致 WebCore 内部的 max、min 出现错乱。
#include "wkeString.h"
#include "wkeWebView.h"
#include "wkeWebWindow.h"
#include "wtf/text/WTFString.h"

namespace net {

void setCookieJarPath(const WCHAR* path);

}

//////////////////////////////////////////////////////////////////////////
static std::string* s_versionString = nullptr;
static bool wkeIsInit = false;

bool wkeIsUpdataInOtherThread = false;

void wkeInitialize()
{
	if (!wkeIsInit) {
		//double-precision float
		_controlfp(_PC_53, _MCW_PC);

		CoInitialize(NULL);

		content::WebPage::initBlink();
		wkeIsInit = true;
	}
}

void wkeSetProxy(const wkeProxy& proxy)
{
     net::WebURLLoaderManager::ProxyType proxyType = net::WebURLLoaderManager::HTTP;
     String hostname;
     String username;
     String password;
 
     if (proxy.hostname[0] != 0 && proxy.type >= WKE_PROXY_HTTP && proxy.type <= WKE_PROXY_SOCKS5HOSTNAME) {
         switch (proxy.type) {
         case WKE_PROXY_HTTP:           proxyType = net::WebURLLoaderManager::HTTP; break;
         case WKE_PROXY_SOCKS4:         proxyType = net::WebURLLoaderManager::Socks4; break;
         case WKE_PROXY_SOCKS4A:        proxyType = net::WebURLLoaderManager::Socks4A; break;
         case WKE_PROXY_SOCKS5:         proxyType = net::WebURLLoaderManager::Socks5; break;
         case WKE_PROXY_SOCKS5HOSTNAME: proxyType = net::WebURLLoaderManager::Socks5Hostname; break;
         }
 
         hostname = String::fromUTF8(proxy.hostname);
         username = String::fromUTF8(proxy.username);
         password = String::fromUTF8(proxy.password);
     }
 
	 net::WebURLLoaderManager::sharedInstance()->setProxyInfo(hostname, proxy.port, proxyType, username, password);
}

WKE_API void wkeSetViewProxy(wkeWebView webView, wkeProxy *proxy) {
	net::WebURLLoaderManager::ProxyType proxyType = net::WebURLLoaderManager::HTTP;
	String hostname;
	String username;
	String password;

	if (proxy->hostname[0] != 0 && proxy->type >= WKE_PROXY_HTTP && proxy->type <= WKE_PROXY_SOCKS5HOSTNAME) {
		switch (proxy->type) {
		case WKE_PROXY_HTTP:           proxyType = net::WebURLLoaderManager::HTTP; break;
		case WKE_PROXY_SOCKS4:         proxyType = net::WebURLLoaderManager::Socks4; break;
		case WKE_PROXY_SOCKS4A:        proxyType = net::WebURLLoaderManager::Socks4A; break;
		case WKE_PROXY_SOCKS5:         proxyType = net::WebURLLoaderManager::Socks5; break;
		case WKE_PROXY_SOCKS5HOSTNAME: proxyType = net::WebURLLoaderManager::Socks5Hostname; break;
		}

		hostname = String::fromUTF8(proxy->hostname);
		username = String::fromUTF8(proxy->username);
		password = String::fromUTF8(proxy->password);
	}

	webView->setProxyInfo(hostname, proxy->port, proxyType, username, password);
}
void wkeConfigure(const wkeSettings* settings)
{
    if (settings->mask & WKE_SETTING_PROXY)
        wkeSetProxy(settings->proxy);
    if (settings->mask & WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD)
        wkeIsUpdataInOtherThread = true;
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

void wkeSetUserAgentW(wkeWebView webView, const wchar_t* userAgent)
{
    webView->setUserAgent(userAgent);
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
    webView->paint(bits, bufWid, bufHei, xDst, yDst, w, h, xSrc, ySrc,bCopyAlpha);
}

void wkePaint(wkeWebView webView, void* bits, int pitch)
{
    webView->paint(bits, pitch);
}

void wkeRepaintIfNeeded(wkeWebView webView)
{
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
    webView->setFocus();
}

void wkeKillFocus(wkeWebView webView)
{
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

void wkeSleep(wkeWebView webView)
{
    webView->sleep();
}

void wkeWake(wkeWebView webView)
{
    webView->wake();
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

void wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void* callbackParam)
{
    webView->onURLChanged(callback, callbackParam);
}

void wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void* callbackParam)
{
    webView->onPaintUpdated(callback, callbackParam);
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

void wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void* param)
{
    webView->onLoadingFinish(callback, param);
}

void wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void* param)
{
	webView->onDownload(callback, param);
}

void wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback, void* callbackParam)
{
	webView->onLoadUrlBegin(callback, callbackParam);
}

void wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void* callbackParam)
{
	webView->onLoadUrlEnd(callback, callbackParam);
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

void wkeSetFileSystem(FILE_OPEN_ pfn_open, FILE_CLOSE_ pfn_close, FILE_SIZE pfn_size, FILE_READ pfn_read, FILE_SEEK pfn_seek)
{
    ;
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
    wkeEditorCopy(webView);
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

const utf8* jsToString(jsExecState es, jsValue v)
{
    return jsToTempString(es, v);
}

const wchar_t* jsToStringW(jsExecState es, jsValue v)
{
    return jsToTempStringW(es, v);
}

// V1 API end
//////////////////////////////////////////////////////////////////////////

#endif