/*
*
* BlzFans@hotmail.com
* http://wke.sf.net
* http://www.github.com/BlzFans/wke
* licence LGPL
*
*/

#include "wkedefine.h"

#if 0
#define WKE_H

//////////////////////////////////////////////////////////////////////////

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
#define WKE_EXTERN_C extern "C"
#else
#define WKE_EXTERN_C
#endif


#if defined(BUILDING_wke)
#   define WKE_API WKE_EXTERN_C __declspec(dllexport)
#else
#   define WKE_API WKE_EXTERN_C __declspec(dllimport)
#endif

typedef struct {
    int x;
    int y;
    int w;
    int h;
} wkeRect;

typedef enum {
    WKE_LBUTTON = 0x01,
    WKE_RBUTTON = 0x02,
    WKE_SHIFT = 0x04,
    WKE_CONTROL = 0x08,
    WKE_MBUTTON = 0x10,
} wkeMouseFlags;

typedef enum {
    WKE_EXTENDED = 0x0100,
    WKE_REPEAT = 0x4000,
} wkeKeyFlags;

typedef enum {
    WKE_MSG_MOUSEMOVE = 0x0200,
    WKE_MSG_LBUTTONDOWN = 0x0201,
    WKE_MSG_LBUTTONUP = 0x0202,
    WKE_MSG_LBUTTONDBLCLK = 0x0203,
    WKE_MSG_RBUTTONDOWN = 0x0204,
    WKE_MSG_RBUTTONUP = 0x0205,
    WKE_MSG_RBUTTONDBLCLK = 0x0206,
    WKE_MSG_MBUTTONDOWN = 0x0207,
    WKE_MSG_MBUTTONUP = 0x0208,
    WKE_MSG_MBUTTONDBLCLK = 0x0209,
    WKE_MSG_MOUSEWHEEL = 0x020A,
} wkeMouseMsg;

#if !defined(__cplusplus)
#ifndef HAVE_WCHAR_T
typedef unsigned short wchar_t;
#endif

#ifndef HAVE_BOOL
typedef unsigned char bool;
#define true 1
#define false 0
#endif
#endif

typedef char utf8;
#if !defined(__cplusplus)
typedef void* jsExecState;
#else
struct JsExecStateInfo;
typedef JsExecStateInfo* jsExecState;
#endif
typedef __int64 jsValue;

#if defined(__cplusplus)
namespace wke { class CWebView; };
typedef wke::CWebView* wkeWebView;

namespace wke { class CString; };
typedef wke::CString* wkeString;

#else
struct _tagWkeWebView;
typedef struct _tagWkeWebView* wkeWebView;

struct _tagWkeString;
typedef struct _tagWkeString* wkeString;
#endif

typedef enum {
    WKE_PROXY_NONE,
    WKE_PROXY_HTTP,
    WKE_PROXY_SOCKS4,
    WKE_PROXY_SOCKS4A,
    WKE_PROXY_SOCKS5,
    WKE_PROXY_SOCKS5HOSTNAME
} wkeProxyType;

typedef struct {
    wkeProxyType type;
    char hostname[100];
    unsigned short port;
    char username[50];
    char password[50];
} wkeProxy;

enum wkeSettingMask {
    WKE_SETTING_PROXY = 1,
    WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD = 1 << 2,
};

typedef struct {
    wkeProxy proxy;
    unsigned int mask;
} wkeSettings;

typedef struct {
    int size;
    unsigned int bgColor;
} wkeViewSettings;

/*
*c interface
*----------------------------------------------------------------------------------------------------------
*
*/

#if defined(__cplusplus)
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
// V1 API

WKE_API void wkeInit();
WKE_API void wkeShutdown();
WKE_API unsigned int wkeVersion();
WKE_API const utf8* wkeVersionString();
WKE_API void wkeGC(wkeWebView webView, long delayMs);

typedef void* wkeWebFrameHandle;

typedef void* (*FILE_OPEN_) (const char* path);
typedef void(*FILE_CLOSE_) (void* handle);
typedef size_t(*FILE_SIZE) (void* handle);
typedef int(*FILE_READ) (void* handle, void* buffer, size_t size);
typedef int(*FILE_SEEK) (void* handle, int offset, int origin);

typedef FILE_OPEN_ WKE_FILE_OPEN;
typedef FILE_CLOSE_ WKE_FILE_CLOSE;
typedef FILE_SIZE WKE_FILE_SIZE;
typedef FILE_READ WKE_FILE_READ;
typedef FILE_SEEK WKE_FILE_SEEK;
typedef bool (*WKE_EXISTS_FILE)(const char * path);

WKE_API void wkeSetFileSystem(WKE_FILE_OPEN pfnOpen, WKE_FILE_CLOSE pfnClose, WKE_FILE_SIZE pfnSize, WKE_FILE_READ pfnRead, WKE_FILE_SEEK pfnSeek);

WKE_API const char* wkeWebViewName(wkeWebView webView);
WKE_API void wkeSetWebViewName(wkeWebView webView, const char* name);

WKE_API bool wkeIsLoaded(wkeWebView webView);
WKE_API bool wkeIsLoadFailed(wkeWebView webView);
WKE_API bool wkeIsLoadComplete(wkeWebView webView);

WKE_API const utf8* wkeGetSource(wkeWebView webView);

WKE_API const utf8* wkeTitle(wkeWebView webView);
WKE_API const wchar_t* wkeTitleW(wkeWebView webView);

WKE_API int wkeWidth(wkeWebView webView);
WKE_API int wkeHeight(wkeWebView webView);

WKE_API int wkeContentsWidth(wkeWebView webView);
WKE_API int wkeContentsHeight(wkeWebView webView);

WKE_API void wkeSelectAll(wkeWebView webView);
WKE_API void wkeCopy(wkeWebView webView);
WKE_API void wkeCut(wkeWebView webView);
WKE_API void wkePaste(wkeWebView webView);
WKE_API void wkeDelete(wkeWebView webView);

WKE_API bool wkeCookieEnabled(wkeWebView webView);

WKE_API float wkeMediaVolume(wkeWebView webView);

WKE_API bool wkeMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags);
WKE_API bool wkeContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags);
WKE_API bool wkeMouseWheel(wkeWebView webView, int x, int y, int delta, unsigned int flags);
WKE_API bool wkeKeyUp(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey);
WKE_API bool wkeKeyDown(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey);
WKE_API bool wkeKeyPress(wkeWebView webView, unsigned int charCode, unsigned int flags, bool systemKey);

WKE_API void wkeFocus(wkeWebView webView);
WKE_API void wkeUnfocus(wkeWebView webView);

WKE_API wkeRect wkeGetCaret(wkeWebView webView);

WKE_API void wkeAwaken(wkeWebView webView);

WKE_API float wkeZoomFactor(wkeWebView webView);

typedef void(*ON_TITLE_CHANGED) (const struct _wkeClientHandler* clientHandler, const wkeString title);
typedef void(*ON_URL_CHANGED) (const struct _wkeClientHandler* clientHandler, const wkeString url);

typedef struct _wkeClientHandler {
    ON_TITLE_CHANGED onTitleChanged;
    ON_URL_CHANGED onURLChanged;
} wkeClientHandler;
WKE_API void wkeSetClientHandler(wkeWebView webView, const wkeClientHandler* handler);
WKE_API const wkeClientHandler* wkeGetClientHandler(wkeWebView webView);

WKE_API const utf8* wkeToString(const wkeString string);
WKE_API const wchar_t* wkeToStringW(const wkeString string);

WKE_API const utf8* jsToString(jsExecState es, jsValue v);
WKE_API const wchar_t* jsToStringW(jsExecState es, jsValue v);
// V1 API end
//////////////////////////////////////////////////////////////////////////

WKE_API void wkeInitialize();
WKE_API void wkeInitializeEx(const wkeSettings* settings);
WKE_API void wkeConfigure(const wkeSettings* settings);
WKE_API bool wkeIsInitialize();

WKE_API void wkeSetViewSettings(wkeWebView webView, const wkeViewSettings* settings);
WKE_API void wkeSetDebugConfig(wkeWebView webView, const char* debugString, const char* param);

WKE_API void wkeFinalize();
WKE_API void wkeUpdate();
WKE_API unsigned int wkeGetVersion();
WKE_API const utf8* wkeGetVersionString();

WKE_API wkeWebView wkeCreateWebView();
WKE_API wkeWebView wkeGetWebView(const char* name);
WKE_API void wkeDestroyWebView(wkeWebView webView);

WKE_API void wkeSetMemoryCacheEnable(wkeWebView webView, bool b);
WKE_API void wkeSetTouchEnabled(wkeWebView webView, bool b);
WKE_API void wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b);
WKE_API void wkeSetCspCheckEnable(wkeWebView webView, bool b);
WKE_API void wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b);
WKE_API void wkeSetHeadlessEnabled(wkeWebView webView, bool b);
WKE_API void wkeSetDragEnable(wkeWebView webView, bool b);

WKE_API void wkeSetViewNetInterface(wkeWebView webView, const char* netInterface);

WKE_API void wkeSetProxy(const wkeProxy* proxy);
WKE_API void wkeSetViewProxy(wkeWebView webView, wkeProxy *proxy);

WKE_API const char* wkeGetName(wkeWebView webView);
WKE_API void wkeSetName(wkeWebView webView, const char* name);

WKE_API void wkeSetHandle(wkeWebView webView, HWND wnd);
WKE_API void wkeSetHandleOffset(wkeWebView webView, int x, int y);

WKE_API bool wkeIsTransparent(wkeWebView webView);
WKE_API void wkeSetTransparent(wkeWebView webView, bool transparent);

WKE_API void wkeSetUserAgent(wkeWebView webView, const utf8* userAgent);
WKE_API void wkeSetUserAgentW(wkeWebView webView, const wchar_t* userAgent);

WKE_API void wkeLoadW(wkeWebView webView, const wchar_t* url);
WKE_API void wkeLoadURL(wkeWebView webView, const utf8* url);
WKE_API void wkeLoadURLW(wkeWebView webView, const wchar_t* url);
WKE_API void wkePostURL(wkeWebView wkeView, const utf8* url, const char* postData, int postLen);
WKE_API void wkePostURLW(wkeWebView wkeView, const wchar_t* url, const char* postData, int postLen);

WKE_API void wkeLoadHTML(wkeWebView webView, const utf8* html);
WKE_API void wkeLoadHTMLW(wkeWebView webView, const wchar_t* html);

WKE_API void wkeLoadFile(wkeWebView webView, const utf8* filename);
WKE_API void wkeLoadFileW(wkeWebView webView, const wchar_t* filename);

WKE_API const utf8* wkeGetURL(wkeWebView webView);

WKE_API bool wkeIsLoading(wkeWebView webView);
WKE_API bool wkeIsLoadingSucceeded(wkeWebView webView);
WKE_API bool wkeIsLoadingFailed(wkeWebView webView);
WKE_API bool wkeIsLoadingCompleted(wkeWebView webView);
WKE_API bool wkeIsDocumentReady(wkeWebView webView);
WKE_API void wkeStopLoading(wkeWebView webView);
WKE_API void wkeReload(wkeWebView webView);

WKE_API const utf8* wkeGetTitle(wkeWebView webView);
WKE_API const wchar_t* wkeGetTitleW(wkeWebView webView);

WKE_API void wkeResize(wkeWebView webView, int w, int h);
WKE_API int wkeGetWidth(wkeWebView webView);
WKE_API int wkeGetHeight(wkeWebView webView);
WKE_API int wkeGetContentWidth(wkeWebView webView);
WKE_API int wkeGetContentHeight(wkeWebView webView);

WKE_API void wkeSetDirty(wkeWebView webView, bool dirty);
WKE_API bool wkeIsDirty(wkeWebView webView);
WKE_API void wkeAddDirtyArea(wkeWebView webView, int x, int y, int w, int h);
WKE_API void wkeLayoutIfNeeded(wkeWebView webView);
WKE_API void wkePaint2(wkeWebView webView, void* bits, int bufWid, int bufHei, int xDst, int yDst, int w, int h, int xSrc, int ySrc, bool bCopyAlpha);
WKE_API void wkePaint(wkeWebView webView, void* bits, int pitch);
WKE_API void wkeRepaintIfNeeded(wkeWebView webView);
WKE_API HDC wkeGetViewDC(wkeWebView webView);
WKE_API HWND wkeGetHostHWND(wkeWebView webView);

WKE_API bool wkeCanGoBack(wkeWebView webView);
WKE_API bool wkeGoBack(wkeWebView webView);
WKE_API bool wkeCanGoForward(wkeWebView webView);
WKE_API bool wkeGoForward(wkeWebView webView);

WKE_API void wkeEditorSelectAll(wkeWebView webView);
WKE_API void wkeEditorUnSelect(wkeWebView webView);
WKE_API void wkeEditorCopy(wkeWebView webView);
WKE_API void wkeEditorCut(wkeWebView webView);
WKE_API void wkeEditorPaste(wkeWebView webView);
WKE_API void wkeEditorDelete(wkeWebView webView);
WKE_API void wkeEditorUndo(wkeWebView webView);
WKE_API void wkeEditorRedo(wkeWebView webView);

WKE_API const wchar_t* wkeGetCookieW(wkeWebView webView);
WKE_API const utf8* wkeGetCookie(wkeWebView webView);

// struct wkeCookieList {
//     char *data;
//     wkeCookieList* next;
// };
// WKE_API const wkeCookieList* wkeGetAllCookie();
// WKE_API void wkeFreeCookieList(const wkeCookieList* cookieList);
typedef bool(*wkeCookieVisitor)(
    void* params,
    const char* name, 
    const char* value, 
    const char* domain,
    const char* path, // If |path| is non-empty only URLs at or below the path will get the cookie value.
    int secure, // If |secure| is true the cookie will only be sent for HTTPS requests.
    int httpOnly, // If |httponly| is true the cookie will only be sent for HTTP requests.
    int* expires // The cookie expiration date is only valid if |has_expires| is true.
    );

WKE_API void wkeVisitAllCookie(void* params, wkeCookieVisitor visitor);

enum wkeCookieCommand {
    wkeCookieCommandClearAllCookies,
    wkeCookieCommandClearSessionCookies,
    wkeCookieCommandFlushCookiesToFile,
    wkeCookieCommandReloadCookiesFromFile,
};
WKE_API void wkePerformCookieCommand(wkeCookieCommand command);

WKE_API void wkeSetCookieEnabled(wkeWebView webView, bool enable);
WKE_API bool wkeIsCookieEnabled(wkeWebView webView);
WKE_API void wkeSetCookieJarPath(wkeWebView webView, const WCHAR* path);
WKE_API void wkeSetCookieJarFullPath(wkeWebView webView, const WCHAR* path);
WKE_API void wkeSetLocalStorageFullPath(wkeWebView webView, const WCHAR* path);

WKE_API void wkeSetMediaVolume(wkeWebView webView, float volume);
WKE_API float wkeGetMediaVolume(wkeWebView webView);

WKE_API bool wkeFireMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags);
WKE_API bool wkeFireContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags);
WKE_API bool wkeFireMouseWheelEvent(wkeWebView webView, int x, int y, int delta, unsigned int flags);
WKE_API bool wkeFireKeyUpEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey);
WKE_API bool wkeFireKeyDownEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey);
WKE_API bool wkeFireKeyPressEvent(wkeWebView webView, unsigned int charCode, unsigned int flags, bool systemKey);
WKE_API bool wkeFireWindowsMessage(wkeWebView webView, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

WKE_API void wkeSetFocus(wkeWebView webView);
WKE_API void wkeKillFocus(wkeWebView webView);

WKE_API wkeRect wkeGetCaretRect(wkeWebView webView);

WKE_API jsValue wkeRunJS(wkeWebView webView, const utf8* script);
WKE_API jsValue wkeRunJSW(wkeWebView webView, const wchar_t* script);

WKE_API jsExecState wkeGlobalExec(wkeWebView webView);

WKE_API void wkeSleep(wkeWebView webView);
WKE_API void wkeWake(wkeWebView webView);
WKE_API bool wkeIsAwake(wkeWebView webView);

WKE_API void wkeSetZoomFactor(wkeWebView webView, float factor);
WKE_API float wkeGetZoomFactor(wkeWebView webView);

WKE_API void wkeSetEditable(wkeWebView webView, bool editable);

WKE_API const utf8* wkeGetString(const wkeString string);
WKE_API const wchar_t* wkeGetStringW(const wkeString string);

WKE_API void wkeSetString(wkeString string, const utf8* str, size_t len);
WKE_API void wkeSetStringW(wkeString string, const wchar_t* str, size_t len);

WKE_API wkeString wkeCreateStringW(const wchar_t* str, size_t len);
WKE_API void wkeDeleteString(wkeString str);

WKE_API wkeWebView wkeGetWebViewForCurrentContext();
WKE_API void wkeSetUserKeyValue(wkeWebView webView, const char* key, void* value);
WKE_API void* wkeGetUserKeyValue(wkeWebView webView, const char* key);

enum WkeCursorInfoType {
    WkeCursorInfoPointer,
    WkeCursorInfoCross,
    WkeCursorInfoHand,
    WkeCursorInfoIBeam,
    WkeCursorInfoWait,
    WkeCursorInfoHelp,
    WkeCursorInfoEastResize,
    WkeCursorInfoNorthResize,
    WkeCursorInfoNorthEastResize,
    WkeCursorInfoNorthWestResize,
    WkeCursorInfoSouthResize,
    WkeCursorInfoSouthEastResize,
    WkeCursorInfoSouthWestResize,
    WkeCursorInfoWestResize,
    WkeCursorInfoNorthSouthResize,
    WkeCursorInfoEastWestResize,
    WkeCursorInfoNorthEastSouthWestResize,
    WkeCursorInfoNorthWestSouthEastResize,
    WkeCursorInfoColumnResize,
    WkeCursorInfoRowResize,
    WkeCursorInfoMiddlePanning,
    WkeCursorInfoEastPanning,
    WkeCursorInfoNorthPanning,
    WkeCursorInfoNorthEastPanning,
    WkeCursorInfoNorthWestPanning,
    WkeCursorInfoSouthPanning,
    WkeCursorInfoSouthEastPanning,
    WkeCursorInfoSouthWestPanning,
    WkeCursorInfoWestPanning,
    WkeCursorInfoMove,
    WkeCursorInfoVerticalText,
    WkeCursorInfoCell,
    WkeCursorInfoContextMenu,
    WkeCursorInfoAlias,
    WkeCursorInfoProgress,
    WkeCursorInfoNoDrop,
    WkeCursorInfoCopy,
    WkeCursorInfoNone,
    WkeCursorInfoNotAllowed,
    WkeCursorInfoZoomIn,
    WkeCursorInfoZoomOut,
    WkeCursorInfoGrab,
    WkeCursorInfoGrabbing,
    WkeCursorInfoCustom
};

WKE_API int wkeGetCursorInfoType(wkeWebView webView);
WKE_API void wkeSetDragFiles(wkeWebView webView, const POINT* clintPos, const POINT* screenPos, wkeString files[], int filesCount);

// blink内部窗口创建回调，例如下拉框
WKE_API void wkeOnBlinkWindowCreate();

//wke callback-----------------------------------------------------------------------------------
typedef void(*wkeTitleChangedCallback)(wkeWebView webView, void* param, const wkeString title);
WKE_API void wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback, void* callbackParam);
WKE_API void wkeOnMouseOverUrlChanged(wkeWebView webView, wkeTitleChangedCallback callback, void* callbackParam);

typedef void(*wkeURLChangedCallback)(wkeWebView webView, void* param, const wkeString url);
WKE_API void wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void* callbackParam);

typedef void(*wkeURLChangedCallback2)(wkeWebView webView, void* param, wkeWebFrameHandle frameId, const wkeString url);
WKE_API void wkeOnURLChanged2(wkeWebView webView, wkeURLChangedCallback2 callback, void* callbackParam);

typedef void(*wkePaintUpdatedCallback)(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy);
WKE_API void wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void* callbackParam);

typedef void(*wkePaintBitUpdatedCallback)(wkeWebView webView, void* param, const void* buffer, const wkeRect* r, int width, int height);
WKE_API void wkeOnPaintBitUpdated(wkeWebView webView, wkePaintBitUpdatedCallback callback, void* callbackParam);

typedef void(*wkeAlertBoxCallback)(wkeWebView webView, void* param, const wkeString msg);
WKE_API void wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void* callbackParam);

typedef bool(*wkeConfirmBoxCallback)(wkeWebView webView, void* param, const wkeString msg);
WKE_API void wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void* callbackParam);

typedef bool(*wkePromptBoxCallback)(wkeWebView webView, void* param, const wkeString msg, const wkeString defaultResult, wkeString result);
WKE_API void wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void* callbackParam);

typedef enum {
    WKE_NAVIGATION_TYPE_LINKCLICK,
    WKE_NAVIGATION_TYPE_FORMSUBMITTE,
    WKE_NAVIGATION_TYPE_BACKFORWARD,
    WKE_NAVIGATION_TYPE_RELOAD,
    WKE_NAVIGATION_TYPE_FORMRESUBMITT,
    WKE_NAVIGATION_TYPE_OTHER
} wkeNavigationType;

typedef bool(*wkeNavigationCallback)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url);
WKE_API void wkeOnNavigation(wkeWebView webView, wkeNavigationCallback callback, void* param);

typedef struct {
    int x;
    int y;
    int width;
    int height;

    bool menuBarVisible;
    bool statusBarVisible;
    bool toolBarVisible;
    bool locationBarVisible;
    bool scrollbarsVisible;
    bool resizable;
    bool fullscreen;
} wkeWindowFeatures;

typedef wkeWebView(*wkeCreateViewCallback)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures);
WKE_API void wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void* param);

typedef void(*wkeDocumentReadyCallback)(wkeWebView webView, void* param);
WKE_API void wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback, void* param);

typedef void(*wkeDocumentReady2Callback)(wkeWebView webView, void* param, wkeWebFrameHandle frameId);
WKE_API void wkeOnDocumentReady2(wkeWebView webView, wkeDocumentReady2Callback callback, void* param);

typedef enum {
    WKE_LOADING_SUCCEEDED,
    WKE_LOADING_FAILED,
    WKE_LOADING_CANCELED
} wkeLoadingResult;

typedef void(*wkeLoadingFinishCallback)(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason);
WKE_API void wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void* param);

typedef bool(*wkeDownloadCallback)(wkeWebView webView, void* param, const char* url);
WKE_API void wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void* param);

typedef enum {
    wkeLevelDebug = 4,
    wkeLevelLog = 1,
    wkeLevelInfo = 5,
    wkeLevelWarning = 2,
    wkeLevelError = 3,
    wkeLevelRevokedError = 6,
    wkeLevelLast = wkeLevelInfo
} wkeConsoleLevel;
typedef void(*wkeConsoleCallback)(wkeWebView webView, void* param, wkeConsoleLevel level, const wkeString message, const wkeString sourceName, unsigned sourceLine, const wkeString stackTrace);
WKE_API void wkeOnConsole(wkeWebView webView, wkeConsoleCallback callback, void* param);

typedef void(*wkeOnCallUiThread)(wkeWebView webView, void* paramOnInThread);
typedef void(*wkeCallUiThread)(wkeWebView webView, wkeOnCallUiThread func, void* param);
WKE_API void wkeSetUIThreadCallback(wkeWebView webView, wkeCallUiThread callback, void* param);

//wkeNet--------------------------------------------------------------------------------------
typedef bool(*wkeLoadUrlBeginCallback)(wkeWebView webView, void* param, const char *url, void *job);
WKE_API void wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback, void* callbackParam);

typedef void(*wkeLoadUrlEndCallback)(wkeWebView webView, void* param, const char *url, void *job, void* buf, int len);
WKE_API void wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void* callbackParam);

typedef void(*wkeDidCreateScriptContextCallback)(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int extensionGroup, int worldId);
WKE_API void wkeOnDidCreateScriptContext(wkeWebView webView, wkeDidCreateScriptContextCallback callback, void* callbackParam);

typedef void(*wkeWillReleaseScriptContextCallback)(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int worldId);
WKE_API void wkeOnWillReleaseScriptContext(wkeWebView webView, wkeWillReleaseScriptContextCallback callback, void* callbackParam);

WKE_API void wkeNetSetMIMEType(void* job, char *type);
WKE_API void wkeNetSetHTTPHeaderField(void* job, wchar_t* key, wchar_t* value, bool response);
WKE_API void wkeNetSetURL(void* job, const char *url);
WKE_API void wkeNetSetData(void* job, void *buf, int len);
// 调用此函数后,网络层收到数据会存储在一buf内,接收数据完成后响应OnLoadUrlEnd事件.#此调用严重影响性能,慎用
// 此函数和wkeNetSetData的区别是，wkeNetHookRequest会在接受到真正网络数据后再调用回调，并允许回调修改网络数据。
// 而wkeNetSetData是在网络数据还没发送的时候修改
WKE_API void wkeNetHookRequest(void *job);

typedef bool(*wkeNetResponseCallback)(wkeWebView webView, void* param, const char* url, void* job);
WKE_API void wkeNetOnResponse(wkeWebView webView, wkeNetResponseCallback callback, void* param);

WKE_API void wkeNetGetMIMEType(void* job, wkeString mime);

WKE_API bool wkeIsMainFrame(wkeWebView webView, wkeWebFrameHandle frameId);
WKE_API bool wkeIsWebRemoteFrame(wkeWebView webView, wkeWebFrameHandle frameId);
WKE_API wkeWebFrameHandle wkeWebFrameGetMainFrame(wkeWebView webView);
WKE_API jsValue wkeRunJsByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const utf8* script, bool isInClosure);
//WKE_API const utf8* wkeGetFrameUrl(wkeWebView webView, wkeWebFrameHandle frameId);

typedef void* v8ContextPtr;
WKE_API void wkeWebFrameGetMainWorldScriptContext(wkeWebView webView, wkeWebFrameHandle webFrameId, v8ContextPtr contextOut);

typedef void* v8Isolate;
WKE_API v8Isolate wkeGetBlinkMainThreadIsolate();

//wkewindow-----------------------------------------------------------------------------------
typedef enum {
    WKE_WINDOW_TYPE_POPUP,
    WKE_WINDOW_TYPE_TRANSPARENT,
    WKE_WINDOW_TYPE_CONTROL

} wkeWindowType;

WKE_API wkeWebView wkeCreateWebWindow(wkeWindowType type, HWND parent, int x, int y, int width, int height);
WKE_API void wkeDestroyWebWindow(wkeWebView webWindow);
WKE_API HWND wkeGetWindowHandle(wkeWebView webWindow);

typedef bool(*wkeWindowClosingCallback)(wkeWebView webWindow, void* param);
WKE_API void wkeOnWindowClosing(wkeWebView webWindow, wkeWindowClosingCallback callback, void* param);

typedef void(*wkeWindowDestroyCallback)(wkeWebView webWindow, void* param);
WKE_API void wkeOnWindowDestroy(wkeWebView webWindow, wkeWindowDestroyCallback callback, void* param);

WKE_API void wkeShowWindow(wkeWebView webWindow, bool show);
WKE_API void wkeEnableWindow(wkeWebView webWindow, bool enable);

WKE_API void wkeMoveWindow(wkeWebView webWindow, int x, int y, int width, int height);
WKE_API void wkeMoveToCenter(wkeWebView webWindow);
WKE_API void wkeResizeWindow(wkeWebView webWindow, int width, int height);

WKE_API void wkeSetWindowTitle(wkeWebView webWindow, const utf8* title);
WKE_API void wkeSetWindowTitleW(wkeWebView webWindow, const wchar_t* title);

// v3 api
// #define WKE_CALL __cdecl
// WKE_API void        WKE_CALL wkeLoad(wkeWebView* webView, const utf8* str);
// WKE_API void        WKE_CALL wkeLoadW(wkeWebView* webView, const wchar_t* str);
// 
// WKE_API void        WKE_CALL wkeSetRepaintInterval(wkeWebView* webView, int ms);
// WKE_API int         WKE_CALL wkeGetRepaintInterval(wkeWebView* webView);
// WKE_API bool        WKE_CALL wkeRepaintIfNeededAfterInterval(wkeWebView* webView);
// WKE_API bool        WKE_CALL wkeRepaintAllNeeded();
// WKE_API int         WKE_CALL wkeRunMessageLoop(const bool *quit);
// 
// WKE_API void        WKE_CALL wkeSetHostWindow(wkeWebView* webWindow, void* hostWindow);
// WKE_API void*       WKE_CALL wkeGetHostWindow(wkeWebView* webWindow);
// 
// typedef enum {
//     WKE_MESSAGE_SOURCE_HTML,
//     WKE_MESSAGE_SOURCE_XML,
//     WKE_MESSAGE_SOURCE_JS,
//     WKE_MESSAGE_SOURCE_NETWORK,
//     WKE_MESSAGE_SOURCE_CONSOLE_API,
//     WKE_MESSAGE_SOURCE_OTHER
// } wkeMessageSource;
// 
// typedef enum {
//     WKE_MESSAGE_TYPE_LOG,
//     WKE_MESSAGE_TYPE_DIR,
//     WKE_MESSAGE_TYPE_DIR_XML,
//     WKE_MESSAGE_TYPE_TRACE,
//     WKE_MESSAGE_TYPE_START_GROUP,
//     WKE_MESSAGE_TYPE_START_GROUP_COLLAPSED,
//     WKE_MESSAGE_TYPE_END_GROUP,
//     WKE_MESSAGE_TYPE_ASSERT
// 
// } wkeMessageType;
// 
// typedef enum {
//     WKE_MESSAGE_LEVEL_TIP,
//     WKE_MESSAGE_LEVEL_LOG,
//     WKE_MESSAGE_LEVEL_WARNING,
//     WKE_MESSAGE_LEVEL_ERROR,
//     WKE_MESSAGE_LEVEL_DEBUG
// 
// } wkeMessageLevel;
// 
// typedef struct {
//     wkeMessageSource source;
//     wkeMessageType type;
//     wkeMessageLevel level;
//     wkeString* message;
//     wkeString* url;
//     unsigned int lineNumber;
// 
// } wkeConsoleMessage;
// 
// typedef void (WKE_CALL *wkeConsoleMessageCallback)(wkeWebView* webView, void* param, const wkeConsoleMessage* message);
// WKE_API void WKE_CALL wkeOnConsoleMessage(wkeWebView* webView, wkeConsoleMessageCallback callback, void* callbackParam);
// 
// typedef struct {
//     wkeNavigationType navigationType;
//     wkeString* url;
//     wkeString* target;
// 
//     int x;
//     int y;
//     int width;
//     int height;
// 
//     bool menuBarVisible;
//     bool statusBarVisible;
//     bool toolBarVisible;
//     bool locationBarVisible;
//     bool scrollbarsVisible;
//     bool resizable;
//     bool fullscreen;
// 
// } wkeNewViewInfo;
// 
// typedef struct {
//     wkeString* url;
//     wkeJSState* frameJSState;
//     wkeJSState* mainFrameJSState;
// 
// } wkeDocumentReadyInfo;
// 
// WKE_API int         WKE_CALL wkeJSParamCount(wkeJSState* es);
// WKE_API wkeJSType   WKE_CALL wkeJSParamType(wkeJSState* es, int index);
// WKE_API wkeJSValue  WKE_CALL wkeJSParam(wkeJSState* es, int index);
// 
// WKE_API void       WKE_CALL  wkeJSAddRef(wkeJSState* es, wkeJSValue v);
// WKE_API void       WKE_CALL  wkeJSReleaseRef(wkeJSState* es, wkeJSValue v);
// WKE_API void       WKE_CALL  wkeJSCollectGarbge();
//////////////////////////////////////////////////////////////////////////

//JavaScript Bind-----------------------------------------------------------------------------------
#define JS_CALL __fastcall
typedef jsValue(JS_CALL* jsNativeFunction) (jsExecState es);

typedef jsValue(* wkeJsNativeFunction) (jsExecState es, void* param);

typedef enum {
    JSTYPE_NUMBER,
    JSTYPE_STRING,
    JSTYPE_BOOLEAN,
    JSTYPE_OBJECT,
    JSTYPE_FUNCTION,
    JSTYPE_UNDEFINED,
    JSTYPE_ARRAY,
} jsType;

WKE_API void jsBindFunction(const char* name, jsNativeFunction fn, unsigned int argCount);
WKE_API void jsBindGetter(const char* name, jsNativeFunction fn); /*get property*/
WKE_API void jsBindSetter(const char* name, jsNativeFunction fn); /*set property*/

WKE_API void wkeJsBindFunction(const char* name, wkeJsNativeFunction fn, void* param, unsigned int argCount);
WKE_API void wkeJsBindGetter(const char* name, wkeJsNativeFunction fn, void* param); /*get property*/
WKE_API void wkeJsBindSetter(const char* name, wkeJsNativeFunction fn, void* param); /*set property*/

WKE_API int jsArgCount(jsExecState es);
WKE_API jsType jsArgType(jsExecState es, int argIdx);
WKE_API jsValue jsArg(jsExecState es, int argIdx);

WKE_API jsType jsTypeOf(jsValue v);
WKE_API bool jsIsNumber(jsValue v);
WKE_API bool jsIsString(jsValue v);
WKE_API bool jsIsBoolean(jsValue v);
WKE_API bool jsIsObject(jsValue v);
WKE_API bool jsIsFunction(jsValue v);
WKE_API bool jsIsUndefined(jsValue v);
WKE_API bool jsIsNull(jsValue v);
WKE_API bool jsIsArray(jsValue v);
WKE_API bool jsIsTrue(jsValue v);
WKE_API bool jsIsFalse(jsValue v);

WKE_API int jsToInt(jsExecState es, jsValue v);
WKE_API float jsToFloat(jsExecState es, jsValue v);
WKE_API double jsToDouble(jsExecState es, jsValue v);
WKE_API bool jsToBoolean(jsExecState es, jsValue v);
WKE_API const utf8* jsToTempString(jsExecState es, jsValue v);
WKE_API const wchar_t* jsToTempStringW(jsExecState es, jsValue v);

WKE_API jsValue jsInt(int n);
WKE_API jsValue jsFloat(float f);
WKE_API jsValue jsDouble(double d);
WKE_API jsValue jsBoolean(bool b);

WKE_API jsValue jsUndefined();
WKE_API jsValue jsNull();
WKE_API jsValue jsTrue();
WKE_API jsValue jsFalse();

WKE_API jsValue jsString(jsExecState es, const utf8* str);
WKE_API jsValue jsStringW(jsExecState es, const wchar_t* str);
WKE_API jsValue jsEmptyObject(jsExecState es);
WKE_API jsValue jsEmptyArray(jsExecState es);

// cexer JS对象、函数绑定支持
typedef jsValue(*jsGetPropertyCallback)(jsExecState es, jsValue object, const char* propertyName);
typedef bool(*jsSetPropertyCallback)(jsExecState es, jsValue object, const char* propertyName, jsValue value);
typedef jsValue(*jsCallAsFunctionCallback)(jsExecState es, jsValue object, jsValue* args, int argCount);
typedef void(*jsFinalizeCallback)(struct tagjsData* data);

typedef struct tagjsData {
    char typeName[100];
    jsGetPropertyCallback propertyGet;
    jsSetPropertyCallback propertySet;
    jsFinalizeCallback finalize;
    jsCallAsFunctionCallback callAsFunction;
} jsData;

WKE_API jsValue jsObject(jsExecState es, jsData* obj);
WKE_API jsValue jsFunction(jsExecState es, jsData* obj);
WKE_API jsData* jsGetData(jsExecState es, jsValue object);

WKE_API jsValue jsGet(jsExecState es, jsValue object, const char* prop);
WKE_API void jsSet(jsExecState es, jsValue object, const char* prop, jsValue v);

WKE_API jsValue jsGetAt(jsExecState es, jsValue object, int index);
WKE_API void jsSetAt(jsExecState es, jsValue object, int index, jsValue v);

WKE_API int jsGetLength(jsExecState es, jsValue object);
WKE_API void jsSetLength(jsExecState es, jsValue object, int length);

//window object
WKE_API jsValue jsGlobalObject(jsExecState es);
WKE_API wkeWebView jsGetWebView(jsExecState es);

WKE_API jsValue jsEval(jsExecState es, const utf8* str);
WKE_API jsValue jsEvalW(jsExecState es, const wchar_t* str);
WKE_API jsValue jsEvalExW(jsExecState es, const wchar_t* str, bool isInClosure);

WKE_API jsValue jsCall(jsExecState es, jsValue func, jsValue thisObject, jsValue* args, int argCount);
WKE_API jsValue jsCallGlobal(jsExecState es, jsValue func, jsValue* args, int argCount);

WKE_API jsValue jsGetGlobal(jsExecState es, const char* prop);
WKE_API void    jsSetGlobal(jsExecState es, const char* prop, jsValue v);

//garbage collect
WKE_API void jsGC();

#ifdef __cplusplus
}
#endif

#if defined(__cplusplus)

namespace wke {

class IWebView {
public:
    virtual void destroy() = 0;

    virtual const char* name() const = 0;
    virtual void setName(const char* name) = 0;

    virtual bool isTransparent() const = 0;
    virtual void setTransparent(bool transparent) = 0;

    virtual void loadURL(const utf8* url) = 0;
    virtual void loadURL(const wchar_t* url) = 0;

    virtual void loadHTML(const utf8* html) = 0;
    virtual void loadHTML(const wchar_t* html) = 0;

    virtual void loadFile(const utf8* filename) = 0;
    virtual void loadFile(const wchar_t* filename) = 0;

    virtual const utf8* url() const = 0;

    virtual bool isLoading() const = 0;        /*document load sucessed*/
    virtual bool isLoadingFailed() const = 0;    /*document load failed*/
    virtual bool isLoadingSucceeded() const = 0;  /*document load complete*/
    virtual bool isDocumentReady() const = 0; /*document ready*/
    virtual void stopLoading() = 0;
    virtual void reload() = 0;

    virtual const utf8* title() = 0;
    virtual const wchar_t* titleW() = 0;

    virtual void resize(int w, int h) = 0;
    virtual int width() const = 0;   /*viewport width*/
    virtual int height() const = 0;  /*viewport height*/

    virtual int contentsWidth() const = 0;  /*contents width*/
    virtual int contentsHeight() const = 0; /*contents height*/

    virtual void setDirty(bool dirty) = 0;
    virtual bool isDirty() const = 0;
    virtual void addDirtyArea(int x, int y, int w, int h) = 0;

    virtual void layoutIfNeeded() = 0;
    virtual void paint(void* bits, int pitch) = 0;

    virtual bool canGoBack() const = 0;
    virtual bool goBack() = 0;
    virtual bool canGoForward() const = 0;
    virtual bool goForward() = 0;

    virtual void editorSelectAll() = 0;
    virtual void editorUnSelect() = 0;
    virtual void editorCopy() = 0;
    virtual void editorCut() = 0;
    virtual void editorPaste() = 0;
    virtual void editorDelete() = 0;
    virtual void editorUndo() = 0;
    virtual void editorRedo() = 0;

    virtual void setCookieEnabled(bool enable) = 0;
    virtual bool isCookieEnabled() const = 0;

    virtual void setMediaVolume(float volume) = 0;
    virtual float mediaVolume() const = 0;

    virtual bool fireMouseEvent(unsigned int message, int x, int y, unsigned int flags) = 0;
    virtual bool fireContextMenuEvent(int x, int y, unsigned int flags) = 0;
    virtual bool fireMouseWheelEvent(int x, int y, int delta, unsigned int flags) = 0;
    virtual bool fireKeyUpEvent(unsigned int virtualKeyCode, unsigned int flags, bool systemKey) = 0;
    virtual bool fireKeyDownEvent(unsigned int virtualKeyCode, unsigned int flags, bool systemKey) = 0;
    virtual bool fireKeyPressEvent(unsigned int virtualKeyCode, unsigned int flags, bool systemKey) = 0;

    virtual void setFocus() = 0;
    virtual void killFocus() = 0;

    virtual wkeRect getCaret() = 0;

    virtual jsValue runJS(const utf8* script) = 0;
    virtual jsValue runJS(const wchar_t* script) = 0;
    virtual jsExecState globalExec() = 0;

    virtual void sleep() = 0; //moveOffscreen
    virtual void wake() = 0; //moveOnscreen
    virtual bool isAwake() const = 0;

    virtual void setZoomFactor(float factor) = 0;
    virtual float zoomFactor() const = 0;

    virtual void setEditable(bool editable) = 0;

    virtual void setClientHandler(const wkeClientHandler* handler) = 0;
    virtual const wkeClientHandler* getClientHandler() const = 0;
};

}

#endif

#endif

