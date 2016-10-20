/*
 * 
 * BlzFans@hotmail.com
 * http://wke.sf.net
 * http://www.github.com/BlzFans/wke
 * licence LGPL
 *
 */

#ifndef WKE_H
#define WKE_H

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

//////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
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
    WKE_SHIFT   = 0x04,
    WKE_CONTROL = 0x08,
    WKE_MBUTTON = 0x10,

} wkeMouseFlags;


typedef enum {
    WKE_EXTENDED = 0x0100,
    WKE_REPEAT = 0x4000,

} wkeKeyFlags;


typedef enum {
    WKE_MSG_MOUSEMOVE       =  0x0200,
    WKE_MSG_LBUTTONDOWN     =  0x0201,
    WKE_MSG_LBUTTONUP       =  0x0202,
    WKE_MSG_LBUTTONDBLCLK   =  0x0203,
    WKE_MSG_RBUTTONDOWN     =  0x0204,
    WKE_MSG_RBUTTONUP       =  0x0205,
    WKE_MSG_RBUTTONDBLCLK   =  0x0206,
    WKE_MSG_MBUTTONDOWN     =  0x0207,
    WKE_MSG_MBUTTONUP       =  0x0208,
    WKE_MSG_MBUTTONDBLCLK   =  0x0209,
    WKE_MSG_MOUSEWHEEL      =  0x020A,

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
struct JsExecStateInfo;
typedef JsExecStateInfo* jsExecState;
typedef __int64 jsValue;


#if defined(__cplusplus)
    namespace wke{ class CWebView; };
    typedef wke::CWebView* wkeWebView;

    namespace wke{ class CString; };
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
    WKE_SETTING_PROXY = 1
};

typedef struct {
    wkeProxy proxy;
    unsigned int mask;

} wkeSettings;


/*
 *c interface
 *----------------------------------------------------------------------------------------------------------
 *
 */

#if defined(__cplusplus)
extern "C"
{
#endif

WKE_API void wkeInitialize();
WKE_API void wkeInitializeEx(const wkeSettings* settings);
WKE_API void wkeConfigure(const wkeSettings* settings);
bool wkeIsInitialize();

WKE_API void wkeFinalize();
WKE_API void wkeUpdate();
WKE_API unsigned int wkeGetVersion();
WKE_API const utf8* wkeGetVersionString();

typedef void* (*FILE_OPEN) (const char* path);
typedef void (*FILE_CLOSE) (void* handle);
typedef size_t (*FILE_SIZE) (void* handle);
typedef int (*FILE_READ) (void* handle, void* buffer, size_t size);
typedef int (*FILE_SEEK) (void* handle, int offset, int origin);

WKE_API void wkeSetFileSystem(FILE_OPEN pfn_open, FILE_CLOSE pfn_close, FILE_SIZE pfn_size, FILE_READ pfn_read, FILE_SEEK pfn_seek);


WKE_API wkeWebView wkeCreateWebView();
WKE_API wkeWebView wkeGetWebView(const char* name);
WKE_API void wkeDestroyWebView(wkeWebView webView);

WKE_API const char* wkeGetName(wkeWebView webView);
WKE_API void wkeSetName(wkeWebView webView, const char* name);

WKE_API bool wkeIsTransparent(wkeWebView webView);
WKE_API void wkeSetTransparent(wkeWebView webView, bool transparent);

WKE_API void wkeSetUserAgent(wkeWebView webView, const utf8* userAgent);
WKE_API void wkeSetUserAgentW(wkeWebView webView, const wchar_t* userAgent);

WKE_API void wkeLoadURL(wkeWebView webView, const utf8* url);
WKE_API void wkeLoadURLW(wkeWebView webView, const wchar_t* url);
WKE_API void wkePostURL(wkeWebView wkeView,const utf8* url, const char* postData,int  postLen); 
WKE_API void wkePostURLW(wkeWebView wkeView,const wchar_t* url, const char* postData, int postLen);

WKE_API void wkeLoadHTML(wkeWebView webView, const utf8* html);
WKE_API void wkeLoadHTMLW(wkeWebView webView, const wchar_t* html);

WKE_API void wkeLoadFile(wkeWebView webView, const utf8* filename);
WKE_API void wkeLoadFileW(wkeWebView webView, const wchar_t* filename);

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
WKE_API void wkePaint(wkeWebView webView, void* bits,int bufWid, int bufHei, int xDst, int yDst, int w, int h, int xSrc, int ySrc, bool bCopyAlpha);
WKE_API void wkePaint2(wkeWebView webView, void* bits,int pitch);
WKE_API void wkeRepaintIfNeeded(wkeWebView webView);
WKE_API HDC wkeGetViewDC(wkeWebView webView);

WKE_API bool wkeCanGoBack(wkeWebView webView);
WKE_API bool wkeGoBack(wkeWebView webView);
WKE_API bool wkeCanGoForward(wkeWebView webView);
WKE_API bool wkeGoForward(wkeWebView webView);

WKE_API void wkeEditorSelectAll(wkeWebView webView);
WKE_API void wkeEditorCopy(wkeWebView webView);
WKE_API void wkeEditorCut(wkeWebView webView);
WKE_API void wkeEditorPaste(wkeWebView webView);
WKE_API void wkeEditorDelete(wkeWebView webView);

WKE_API const wchar_t* wkeGetCookieW(wkeWebView webView);
WKE_API const utf8* wkeGetCookie(wkeWebView webView);
WKE_API void wkeSetCookieEnabled(wkeWebView webView, bool enable);
WKE_API bool wkeIsCookieEnabled(wkeWebView webView);

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

typedef void (*wkeTitleChangedCallback)(wkeWebView webView, void* param, const wkeString title);
WKE_API void wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback, void* callbackParam);

typedef void (*wkeURLChangedCallback)(wkeWebView webView, void* param, const wkeString url);
WKE_API void wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void* callbackParam);

typedef void (*wkePaintUpdatedCallback)(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy);
WKE_API void wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void* callbackParam);

typedef void (*wkeAlertBoxCallback)(wkeWebView webView, void* param, const wkeString msg);
WKE_API void wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void* callbackParam);

typedef bool (*wkeConfirmBoxCallback)(wkeWebView webView, void* param, const wkeString msg);
WKE_API void wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void* callbackParam);

typedef bool (*wkePromptBoxCallback)(wkeWebView webView, void* param, const wkeString msg, const wkeString defaultResult, wkeString result);
WKE_API void wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void* callbackParam);


typedef enum {
    WKE_NAVIGATION_TYPE_LINKCLICK,
    WKE_NAVIGATION_TYPE_FORMSUBMITTE,
    WKE_NAVIGATION_TYPE_BACKFORWARD,
    WKE_NAVIGATION_TYPE_RELOAD,
    WKE_NAVIGATION_TYPE_FORMRESUBMITT,
    WKE_NAVIGATION_TYPE_OTHER

} wkeNavigationType;

typedef bool (*wkeNavigationCallback)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url);
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

typedef wkeWebView (*wkeCreateViewCallback)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures);
WKE_API void wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void* param);


typedef void (*wkeDocumentReadyCallback)(wkeWebView webView, void* param);
WKE_API void wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback, void* param);


typedef enum {
    WKE_LOADING_SUCCEEDED,
    WKE_LOADING_FAILED,
    WKE_LOADING_CANCELED

} wkeLoadingResult;

typedef void (*wkeLoadingFinishCallback)(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason);
WKE_API void wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void* param);


typedef enum {
    WKE_WINDOW_TYPE_POPUP,
    WKE_WINDOW_TYPE_TRANSPARENT,
    WKE_WINDOW_TYPE_CONTROL

} wkeWindowType;

WKE_API wkeWebView wkeCreateWebWindow(wkeWindowType type, HWND parent, int x, int y, int width, int height);
WKE_API void wkeDestroyWebWindow(wkeWebView webWindow);
WKE_API HWND wkeGetWindowHandle(wkeWebView webWindow);

typedef bool (*wkeWindowClosingCallback)(wkeWebView webWindow, void* param);
WKE_API void wkeOnWindowClosing(wkeWebView webWindow, wkeWindowClosingCallback callback, void* param);

typedef void (*wkeWindowDestroyCallback)(wkeWebView webWindow, void* param);
WKE_API void wkeOnWindowDestroy(wkeWebView webWindow, wkeWindowDestroyCallback callback, void* param);


WKE_API void wkeShowWindow(wkeWebView webWindow, bool show);
WKE_API void wkeEnableWindow(wkeWebView webWindow, bool enable);

WKE_API void wkeMoveWindow(wkeWebView webWindow, int x, int y, int width, int height);
WKE_API void wkeMoveToCenter(wkeWebView webWindow);
WKE_API void wkeResizeWindow(wkeWebView webWindow, int width, int height);

WKE_API void wkeSetWindowTitle(wkeWebView webWindow, const utf8* title);
WKE_API void wkeSetWindowTitleW(wkeWebView webWindow, const wchar_t* title);



/***JavaScript Bind***/
#define JS_CALL __fastcall
typedef jsValue (JS_CALL *jsNativeFunction) (jsExecState es);

typedef enum {
	JSTYPE_NUMBER,
	JSTYPE_STRING,
	JSTYPE_BOOLEAN,
	JSTYPE_OBJECT,
	JSTYPE_FUNCTION,
	JSTYPE_UNDEFINED,
} jsType;


WKE_API void jsBindFunction(const char* name, jsNativeFunction fn, unsigned int argCount);
WKE_API void jsBindGetter(const char* name, jsNativeFunction fn); /*get property*/
WKE_API void jsBindSetter(const char* name, jsNativeFunction fn); /*set property*/

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



//cexer JS���󡢺�����֧��
typedef jsValue (*jsGetPropertyCallback)(jsExecState es, jsValue object, const char* propertyName);
typedef bool (*jsSetPropertyCallback)(jsExecState es, jsValue object, const char* propertyName, jsValue value);
typedef jsValue (*jsCallAsFunctionCallback)(jsExecState es, jsValue object, jsValue* args, int argCount);
typedef void (*jsFinalizeCallback)(struct tagjsData* data);

typedef struct tagjsData
{
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
WKE_API void   jsSet(jsExecState es, jsValue object, const char* prop, jsValue v);

WKE_API jsValue jsGetAt(jsExecState es, jsValue object, int index);
WKE_API void   jsSetAt(jsExecState es, jsValue object, int index, jsValue v);

WKE_API int     jsGetLength(jsExecState es, jsValue object);
WKE_API void    jsSetLength(jsExecState es, jsValue object, int length);



//window object
WKE_API jsValue jsGlobalObject(jsExecState es);
WKE_API wkeWebView jsGetWebView(jsExecState es);

WKE_API jsValue jsEval(jsExecState es, const utf8* str);
WKE_API jsValue jsEvalW(jsExecState es, const wchar_t* str);

WKE_API jsValue jsCall(jsExecState es, jsValue func, jsValue thisObject, jsValue* args, int argCount);
WKE_API jsValue jsCallGlobal(jsExecState es, jsValue func, jsValue* args, int argCount);

WKE_API jsValue jsGetGlobal(jsExecState es, const char* prop);
WKE_API void    jsSetGlobal(jsExecState es, const char* prop, jsValue v);


//garbage collect
WKE_API void jsGC(); 




#ifdef __cplusplus
}
#endif


#endif
#endif//#ifndef WKE_H
