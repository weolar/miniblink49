
#define DISABLE_MBVIP 1
#include "../../mbvip/core/mb.h"

#include <time.h> 
#include <windows.h>
#include <string>
#include <vector>
#include <time.h>
#include <process.h>
#include <shlobj.h>
#include <propvarutil.h>
#include <wininet.h>
#include <CommCtrl.h>
#include <tuple>
#include <crtdbg.h>
#include <psapi.h>
#include <iosfwd>
#include <sstream>
#include <set>
#include <TlHelp32.h>
#include <xmmintrin.h>

HMODULE g_hModule;

// #define MB_WRAP_FUN_ITERATOR0(returnVal, name, description) \
//     MB_EXTERN_C MB_DLLEXPORT returnVal MB_CALL_TYPE name() \
//     { \
//         return s_s_##name(); \
//     }
// 
// #define MB_WRAP_FUN_ITERATOR1(returnVal, name, p1, description) \
//     MB_EXTERN_C MB_DLLEXPORT returnVal MB_CALL_TYPE name(p1) \
//     { \
//         return s_name(p1); \
//     }
// //---
// 
// #define MB_WRAP_DECLARE_ITERATOR0(returnVal, name, description) \
//     typedef returnVal (MB_CALL_TYPE *FN_##name)();\
//     static FN_##name s_##name = nullptr;
// 
// //---
// 
// #define MB_WRAP_INIT_ITERATOR0(returnVal, name, description) \
//     s_##name = (FN_##name)GetProcAddress(hMiniblinkMod, #name);

typedef void (MB_CALL_TYPE* FN_mbUninit)();
static FN_mbUninit s_mbUninit = nullptr;

typedef mbSettings* (MB_CALL_TYPE* FN_mbCreateInitSettings)();
static FN_mbCreateInitSettings s_mbCreateInitSettings = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetInitSettings)(mbSettings* settings, const char* name, const char* value);
static FN_mbSetInitSettings s_mbSetInitSettings = nullptr;

typedef mbWebView(MB_CALL_TYPE* FN_mbCreateWebView)();
static FN_mbCreateWebView s_mbCreateWebView = nullptr;

typedef void (MB_CALL_TYPE* FN_mbDestroyWebView)(mbWebView webview);
static FN_mbDestroyWebView s_mbDestroyWebView = nullptr;

typedef mbWebView(MB_CALL_TYPE* FN_mbCreateWebWindow)(mbWindowType type, HWND parent, int x, int y, int width, int height);
static FN_mbCreateWebWindow s_mbCreateWebWindow = nullptr;

typedef mbWebView(MB_CALL_TYPE* FN_mbCreateWebCustomWindow)(HWND parent, DWORD style, DWORD styleEx, int x, int y, int width, int height);
static FN_mbCreateWebCustomWindow s_mbCreateWebCustomWindow = nullptr;

typedef void (MB_CALL_TYPE* FN_mbMoveWindow)(mbWebView webview, int x, int y, int w, int h);
static FN_mbMoveWindow s_mbMoveWindow = nullptr;

typedef void (MB_CALL_TYPE* FN_mbMoveToCenter)(mbWebView webview);
static FN_mbMoveToCenter s_mbMoveToCenter = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetAutoDrawToHwnd)(mbWebView webview, BOOL b);
static FN_mbSetAutoDrawToHwnd s_mbSetAutoDrawToHwnd = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGetCaretRect)(mbWebView webviewHandle, mbRect* r);
static FN_mbGetCaretRect s_mbGetCaretRect = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetAudioMuted)(mbWebView webview, BOOL b);
static FN_mbSetAudioMuted s_mbSetAudioMuted = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbIsAudioMuted)(mbWebView webview);
static FN_mbIsAudioMuted s_mbIsAudioMuted = nullptr;

typedef mbStringPtr(MB_CALL_TYPE* FN_mbCreateString)(const utf8* str, size_t length);
static FN_mbCreateString s_mbCreateString = nullptr;

typedef mbStringPtr(MB_CALL_TYPE* FN_mbCreateStringWithoutNullTermination)(const utf8* str, size_t length);
static FN_mbCreateStringWithoutNullTermination s_mbCreateStringWithoutNullTermination = nullptr;

typedef void (MB_CALL_TYPE* FN_mbDeleteString)(mbStringPtr str);
static FN_mbDeleteString s_mbDeleteString = nullptr;

typedef size_t(MB_CALL_TYPE* FN_mbGetStringLen)(mbStringPtr str);
static FN_mbGetStringLen s_mbGetStringLen = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbGetString)(mbStringPtr str);
static FN_mbGetString s_mbGetString = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetProxy)(mbWebView webView, const mbProxy* proxy);
static FN_mbSetProxy s_mbSetProxy = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDebugConfig)(mbWebView webView, const char* debugString, const char* param);
static FN_mbSetDebugConfig s_mbSetDebugConfig = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetSetData)(mbNetJob jobPtr, void* buf, int len);
static FN_mbNetSetData s_mbNetSetData = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetHookRequest)(mbNetJob jobPtr);
static FN_mbNetHookRequest s_mbNetHookRequest = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetChangeRequestUrl)(mbNetJob jobPtr, const char* url);
static FN_mbNetChangeRequestUrl s_mbNetChangeRequestUrl = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetContinueJob)(mbNetJob jobPtr);
static FN_mbNetContinueJob s_mbNetContinueJob = nullptr;

typedef const mbSlist* (MB_CALL_TYPE* FN_mbNetGetRawHttpHeadInBlinkThread)(mbNetJob jobPtr);
static FN_mbNetGetRawHttpHeadInBlinkThread s_mbNetGetRawHttpHeadInBlinkThread = nullptr;

typedef const mbSlist* (MB_CALL_TYPE* FN_mbNetGetRawResponseHeadInBlinkThread)(mbNetJob jobPtr);
static FN_mbNetGetRawResponseHeadInBlinkThread s_mbNetGetRawResponseHeadInBlinkThread = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetHoldJobToAsynCommit)(mbNetJob jobPtr);
static FN_mbNetHoldJobToAsynCommit s_mbNetHoldJobToAsynCommit = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetCancelRequest)(mbNetJob jobPtr);
static FN_mbNetCancelRequest s_mbNetCancelRequest = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetOnResponse)(mbWebView webviewHandle, mbNetResponseCallback callback, void* param);
static FN_mbNetOnResponse s_mbNetOnResponse = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetSetWebsocketCallback)(mbWebView webview, const mbWebsocketHookCallbacks* callbacks, void* param);
static FN_mbNetSetWebsocketCallback s_mbNetSetWebsocketCallback = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetSendWsText)(mbWebSocketChannel channel, const char* buf, size_t len);
static FN_mbNetSendWsText s_mbNetSendWsText = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetSendWsBlob)(mbWebSocketChannel channel, const char* buf, size_t len);
static FN_mbNetSendWsBlob s_mbNetSendWsBlob = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetEnableResPacket)(mbWebView webviewHandle, const WCHAR* pathName);
static FN_mbNetEnableResPacket s_mbNetEnableResPacket = nullptr;

typedef mbPostBodyElements* (MB_CALL_TYPE* FN_mbNetGetPostBody)(mbNetJob jobPtr);
static FN_mbNetGetPostBody s_mbNetGetPostBody = nullptr;

typedef mbPostBodyElements* (MB_CALL_TYPE* FN_mbNetCreatePostBodyElements)(mbWebView webView, size_t length);
static FN_mbNetCreatePostBodyElements s_mbNetCreatePostBodyElements = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetFreePostBodyElements)(mbPostBodyElements* elements);
static FN_mbNetFreePostBodyElements s_mbNetFreePostBodyElements = nullptr;

typedef mbPostBodyElement* (MB_CALL_TYPE* FN_mbNetCreatePostBodyElement)(mbWebView webView);
static FN_mbNetCreatePostBodyElement s_mbNetCreatePostBodyElement = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetFreePostBodyElement)(mbPostBodyElement* element);
static FN_mbNetFreePostBodyElement s_mbNetFreePostBodyElement = nullptr;

typedef mbWebUrlRequestPtr(MB_CALL_TYPE* FN_mbNetCreateWebUrlRequest)(const utf8* url, const utf8* method, const utf8* mime);
static FN_mbNetCreateWebUrlRequest s_mbNetCreateWebUrlRequest = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetAddHTTPHeaderFieldToUrlRequest)(mbWebUrlRequestPtr request, const utf8* name, const utf8* value);
static FN_mbNetAddHTTPHeaderFieldToUrlRequest s_mbNetAddHTTPHeaderFieldToUrlRequest = nullptr;

typedef int (MB_CALL_TYPE* FN_mbNetStartUrlRequest)(mbWebView webView, mbWebUrlRequestPtr request, void* param, const mbUrlRequestCallbacks* callbacks);
static FN_mbNetStartUrlRequest s_mbNetStartUrlRequest = nullptr;

typedef int (MB_CALL_TYPE* FN_mbNetGetHttpStatusCode)(mbWebUrlResponsePtr response);
static FN_mbNetGetHttpStatusCode s_mbNetGetHttpStatusCode = nullptr;

typedef mbRequestType(MB_CALL_TYPE* FN_mbNetGetRequestMethod)(mbNetJob jobPtr);
static FN_mbNetGetRequestMethod s_mbNetGetRequestMethod = nullptr;

typedef __int64 (MB_CALL_TYPE* FN_mbNetGetExpectedContentLength)(mbWebUrlResponsePtr response);
static FN_mbNetGetExpectedContentLength s_mbNetGetExpectedContentLength = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbNetGetResponseUrl)(mbWebUrlResponsePtr response);
static FN_mbNetGetResponseUrl s_mbNetGetResponseUrl = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetCancelWebUrlRequest)(int requestId);
static FN_mbNetCancelWebUrlRequest s_mbNetCancelWebUrlRequest = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetViewProxy)(mbWebView webView, const mbProxy* proxy);
static FN_mbSetViewProxy s_mbSetViewProxy = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetSetMIMEType)(mbNetJob jobPtr, const char* type);
static FN_mbNetSetMIMEType s_mbNetSetMIMEType = nullptr;

typedef const char* (MB_CALL_TYPE* FN_mbNetGetMIMEType)(mbNetJob jobPtr);
static FN_mbNetGetMIMEType s_mbNetGetMIMEType = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbNetGetHTTPHeaderField)(mbNetJob job, const char* key, BOOL fromRequestOrResponse);
static FN_mbNetGetHTTPHeaderField s_mbNetGetHTTPHeaderField = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNetSetHTTPHeaderField)(mbNetJob jobPtr, const wchar_t* key, const wchar_t* value, BOOL response);
static FN_mbNetSetHTTPHeaderField s_mbNetSetHTTPHeaderField = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetMouseEnabled)(mbWebView webView, BOOL b);
static FN_mbSetMouseEnabled s_mbSetMouseEnabled = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetTouchEnabled)(mbWebView webView, BOOL b);
static FN_mbSetTouchEnabled s_mbSetTouchEnabled = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetSystemTouchEnabled)(mbWebView webView, BOOL b);
static FN_mbSetSystemTouchEnabled s_mbSetSystemTouchEnabled = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetContextMenuEnabled)(mbWebView webView, BOOL b);
static FN_mbSetContextMenuEnabled s_mbSetContextMenuEnabled = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetNavigationToNewWindowEnable)(mbWebView webView, BOOL b);
static FN_mbSetNavigationToNewWindowEnable s_mbSetNavigationToNewWindowEnable = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetHeadlessEnabled)(mbWebView webView, BOOL b);
static FN_mbSetHeadlessEnabled s_mbSetHeadlessEnabled = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDragDropEnable)(mbWebView webView, BOOL b);
static FN_mbSetDragDropEnable s_mbSetDragDropEnable = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDragEnable)(mbWebView webView, BOOL b);
static FN_mbSetDragEnable s_mbSetDragEnable = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetContextMenuItemShow)(mbWebView webView, mbMenuItemId item, BOOL isShow);
static FN_mbSetContextMenuItemShow s_mbSetContextMenuItemShow = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetHandle)(mbWebView webView, HWND wnd);
static FN_mbSetHandle s_mbSetHandle = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetHandleOffset)(mbWebView webView, int x, int y);
static FN_mbSetHandleOffset s_mbSetHandleOffset = nullptr;

typedef HWND(MB_CALL_TYPE* FN_mbGetHostHWND)(mbWebView webView);
static FN_mbGetHostHWND s_mbGetHostHWND = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetTransparent)(mbWebView webviewHandle, BOOL transparent);
static FN_mbSetTransparent s_mbSetTransparent = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetViewSettings)(mbWebView webviewHandle, const mbViewSettings* settings);
static FN_mbSetViewSettings s_mbSetViewSettings = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetCspCheckEnable)(mbWebView webView, BOOL b);
static FN_mbSetCspCheckEnable s_mbSetCspCheckEnable = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetNpapiPluginsEnabled)(mbWebView webView, BOOL b);
static FN_mbSetNpapiPluginsEnabled s_mbSetNpapiPluginsEnabled = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetMemoryCacheEnable)(mbWebView webView, BOOL b);
static FN_mbSetMemoryCacheEnable s_mbSetMemoryCacheEnable = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetCookie)(mbWebView webView, const utf8* url, const utf8* cookie);
static FN_mbSetCookie s_mbSetCookie = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetCookieEnabled)(mbWebView webView, BOOL enable);
static FN_mbSetCookieEnabled s_mbSetCookieEnabled = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetCookieJarPath)(mbWebView webView, const WCHAR* path);
static FN_mbSetCookieJarPath s_mbSetCookieJarPath = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetCookieJarFullPath)(mbWebView webView, const WCHAR* path);
static FN_mbSetCookieJarFullPath s_mbSetCookieJarFullPath = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetLocalStorageFullPath)(mbWebView webView, const WCHAR* path);
static FN_mbSetLocalStorageFullPath s_mbSetLocalStorageFullPath = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbGetTitle)(mbWebView webView);
static FN_mbGetTitle s_mbGetTitle = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetWindowTitle)(mbWebView webView, const utf8* title);
static FN_mbSetWindowTitle s_mbSetWindowTitle = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetWindowTitleW)(mbWebView webView, const wchar_t* title);
static FN_mbSetWindowTitleW s_mbSetWindowTitleW = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbGetUrl)(mbWebView webView);
static FN_mbGetUrl s_mbGetUrl = nullptr;

typedef int (MB_CALL_TYPE* FN_mbGetCursorInfoType)(mbWebView webView);
static FN_mbGetCursorInfoType s_mbGetCursorInfoType = nullptr;

typedef void (MB_CALL_TYPE* FN_mbAddPluginDirectory)(mbWebView webView, const WCHAR* path);
static FN_mbAddPluginDirectory s_mbAddPluginDirectory = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetUserAgent)(mbWebView webView, const utf8* userAgent);
static FN_mbSetUserAgent s_mbSetUserAgent = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetZoomFactor)(mbWebView webView, float factor);
static FN_mbSetZoomFactor s_mbSetZoomFactor = nullptr;

typedef float (MB_CALL_TYPE* FN_mbGetZoomFactor)(mbWebView webView);
static FN_mbGetZoomFactor s_mbGetZoomFactor = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDiskCacheEnabled)(mbWebView webView, BOOL enable);
static FN_mbSetDiskCacheEnabled s_mbSetDiskCacheEnabled = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDiskCachePath)(mbWebView webView, const WCHAR* path);
static FN_mbSetDiskCachePath s_mbSetDiskCachePath = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDiskCacheLimit)(mbWebView webView, size_t limit);
static FN_mbSetDiskCacheLimit s_mbSetDiskCacheLimit = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDiskCacheLimitDisk)(mbWebView webView, size_t limit);
static FN_mbSetDiskCacheLimitDisk s_mbSetDiskCacheLimitDisk = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDiskCacheLevel)(mbWebView webView, int Level);
static FN_mbSetDiskCacheLevel s_mbSetDiskCacheLevel = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetResourceGc)(mbWebView webView, int intervalSec);
static FN_mbSetResourceGc s_mbSetResourceGc = nullptr;

typedef void (MB_CALL_TYPE* FN_mbCanGoForward)(mbWebView webView, mbCanGoBackForwardCallback callback, void* param);
static FN_mbCanGoForward s_mbCanGoForward = nullptr;

typedef void (MB_CALL_TYPE* FN_mbCanGoBack)(mbWebView webView, mbCanGoBackForwardCallback callback, void* param);
static FN_mbCanGoBack s_mbCanGoBack = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGetCookie)(mbWebView webView, mbGetCookieCallback callback, void* param);
static FN_mbGetCookie s_mbGetCookie = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbGetCookieOnBlinkThread)(mbWebView webView);
static FN_mbGetCookieOnBlinkThread s_mbGetCookieOnBlinkThread = nullptr;

typedef void (MB_CALL_TYPE* FN_mbClearCookie)(mbWebView webView);
static FN_mbClearCookie s_mbClearCookie = nullptr;

typedef void (MB_CALL_TYPE* FN_mbResize)(mbWebView webView, int w, int h);
static FN_mbResize s_mbResize = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnNavigation)(mbWebView webView, mbNavigationCallback callback, void* param);
static FN_mbOnNavigation s_mbOnNavigation = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnNavigationSync)(mbWebView webView, mbNavigationCallback callback, void* param);
static FN_mbOnNavigationSync s_mbOnNavigationSync = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnCreateView)(mbWebView webView, mbCreateViewCallback callback, void* param);
static FN_mbOnCreateView s_mbOnCreateView = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnDocumentReady)(mbWebView webView, mbDocumentReadyCallback callback, void* param);
static FN_mbOnDocumentReady s_mbOnDocumentReady = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnPaintUpdated)(mbWebView webView, mbPaintUpdatedCallback callback, void* callbackParam);
static FN_mbOnPaintUpdated s_mbOnPaintUpdated = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnPaintBitUpdated)(mbWebView webView, mbPaintBitUpdatedCallback callback, void* callbackParam);
static FN_mbOnPaintBitUpdated s_mbOnPaintBitUpdated = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnAcceleratedPaint)(mbWebView webView, mbAcceleratedPaintCallback callback, void* callbackParam);
static FN_mbOnAcceleratedPaint s_mbOnAcceleratedPaint = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnLoadUrlBegin)(mbWebView webView, mbLoadUrlBeginCallback callback, void* callbackParam);
static FN_mbOnLoadUrlBegin s_mbOnLoadUrlBegin = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnLoadUrlEnd)(mbWebView webView, mbLoadUrlEndCallback callback, void* callbackParam);
static FN_mbOnLoadUrlEnd s_mbOnLoadUrlEnd = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnLoadUrlFail)(mbWebView webView, mbLoadUrlFailCallback callback, void* callbackParam);
static FN_mbOnLoadUrlFail s_mbOnLoadUrlFail = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnTitleChanged)(mbWebView webView, mbTitleChangedCallback callback, void* callbackParam);
static FN_mbOnTitleChanged s_mbOnTitleChanged = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnURLChanged)(mbWebView webView, mbURLChangedCallback callback, void* callbackParam);
static FN_mbOnURLChanged s_mbOnURLChanged = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnLoadingFinish)(mbWebView webView, mbLoadingFinishCallback callback, void* param);
static FN_mbOnLoadingFinish s_mbOnLoadingFinish = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnDownload)(mbWebView webView, mbDownloadCallback callback, void* param);
static FN_mbOnDownload s_mbOnDownload = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnDownloadInBlinkThread)(mbWebView webView, mbDownloadInBlinkThreadCallback callback, void* param);
static FN_mbOnDownloadInBlinkThread s_mbOnDownloadInBlinkThread = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnAlertBox)(mbWebView webView, mbAlertBoxCallback callback, void* param);
static FN_mbOnAlertBox s_mbOnAlertBox = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnConfirmBox)(mbWebView webView, mbConfirmBoxCallback callback, void* param);
static FN_mbOnConfirmBox s_mbOnConfirmBox = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnPromptBox)(mbWebView webView, mbPromptBoxCallback callback, void* param);
static FN_mbOnPromptBox s_mbOnPromptBox = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnNetGetFavicon)(mbWebView webView, mbNetGetFaviconCallback callback, void* param);
static FN_mbOnNetGetFavicon s_mbOnNetGetFavicon = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnConsole)(mbWebView webView, mbConsoleCallback callback, void* param);
static FN_mbOnConsole s_mbOnConsole = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbOnClose)(mbWebView webView, mbCloseCallback callback, void* param);
static FN_mbOnClose s_mbOnClose = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbOnDestroy)(mbWebView webView, mbDestroyCallback callback, void* param);
static FN_mbOnDestroy s_mbOnDestroy = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbOnPrinting)(mbWebView webView, mbPrintingCallback callback, void* param);
static FN_mbOnPrinting s_mbOnPrinting = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnPluginList)(mbWebView webView, mbGetPluginListCallback callback, void* callbackParam);
static FN_mbOnPluginList s_mbOnPluginList = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnImageBufferToDataURL)(mbWebView webView, mbImageBufferToDataURLCallback callback, void* callbackParam);
static FN_mbOnImageBufferToDataURL s_mbOnImageBufferToDataURL = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnDidCreateScriptContext)(mbWebView webView, mbDidCreateScriptContextCallback callback, void* callbackParam);
static FN_mbOnDidCreateScriptContext s_mbOnDidCreateScriptContext = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGoBack)(mbWebView webView);
static FN_mbGoBack s_mbGoBack = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGoForward)(mbWebView webView);
static FN_mbGoForward s_mbGoForward = nullptr;

typedef void (MB_CALL_TYPE* FN_mbNavigateAtIndex)(mbWebView webView, int index);
static FN_mbNavigateAtIndex s_mbNavigateAtIndex = nullptr;

typedef int (MB_CALL_TYPE* FN_mbGetNavigateIndex)(mbWebView webView);
static FN_mbGetNavigateIndex s_mbGetNavigateIndex = nullptr;

typedef void (MB_CALL_TYPE* FN_mbStopLoading)(mbWebView webView);
static FN_mbStopLoading s_mbStopLoading = nullptr;

typedef void (MB_CALL_TYPE* FN_mbReload)(mbWebView webView);
static FN_mbReload s_mbReload = nullptr;

typedef void (MB_CALL_TYPE* FN_mbPerformCookieCommand)(mbWebView webView, mbCookieCommand command);
static FN_mbPerformCookieCommand s_mbPerformCookieCommand = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEditorSelectAll)(mbWebView webView);
static FN_mbEditorSelectAll s_mbEditorSelectAll = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEditorCopy)(mbWebView webView);
static FN_mbEditorCopy s_mbEditorCopy = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEditorCut)(mbWebView webView);
static FN_mbEditorCut s_mbEditorCut = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEditorPaste)(mbWebView webView);
static FN_mbEditorPaste s_mbEditorPaste = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEditorDelete)(mbWebView webView);
static FN_mbEditorDelete s_mbEditorDelete = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEditorUndo)(mbWebView webView);
static FN_mbEditorUndo s_mbEditorUndo = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbFireMouseEvent)(mbWebView webView, unsigned int message, int x, int y, unsigned int flags);
static FN_mbFireMouseEvent s_mbFireMouseEvent = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbFireContextMenuEvent)(mbWebView webView, int x, int y, unsigned int flags);
static FN_mbFireContextMenuEvent s_mbFireContextMenuEvent = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbFireMouseWheelEvent)(mbWebView webView, int x, int y, int delta, unsigned int flags);
static FN_mbFireMouseWheelEvent s_mbFireMouseWheelEvent = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbFireKeyUpEvent)(mbWebView webView, unsigned int virtualKeyCode, unsigned int flags, BOOL systemKey);
static FN_mbFireKeyUpEvent s_mbFireKeyUpEvent = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbFireKeyDownEvent)(mbWebView webView, unsigned int virtualKeyCode, unsigned int flags, BOOL systemKey);
static FN_mbFireKeyDownEvent s_mbFireKeyDownEvent = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbFireKeyPressEvent)(mbWebView webView, unsigned int charCode, unsigned int flags, BOOL systemKey);
static FN_mbFireKeyPressEvent s_mbFireKeyPressEvent = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbFireWindowsMessage)(mbWebView webView, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);
static FN_mbFireWindowsMessage s_mbFireWindowsMessage = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetFocus)(mbWebView webView);
static FN_mbSetFocus s_mbSetFocus = nullptr;

typedef void (MB_CALL_TYPE* FN_mbKillFocus)(mbWebView webView);
static FN_mbKillFocus s_mbKillFocus = nullptr;

typedef void (MB_CALL_TYPE* FN_mbShowWindow)(mbWebView webview, BOOL show);
static FN_mbShowWindow s_mbShowWindow = nullptr;

typedef void (MB_CALL_TYPE* FN_mbLoadURL)(mbWebView webView, const utf8* url);
static FN_mbLoadURL s_mbLoadURL = nullptr;

typedef void (MB_CALL_TYPE* FN_mbLoadHtmlWithBaseUrl)(mbWebView webView, const utf8* html, const utf8* baseUrl);
static FN_mbLoadHtmlWithBaseUrl s_mbLoadHtmlWithBaseUrl = nullptr;

typedef void (MB_CALL_TYPE* FN_mbPostURL)(mbWebView webView, const utf8* url, const char* postData, int postLen);
static FN_mbPostURL s_mbPostURL = nullptr;

typedef HDC(MB_CALL_TYPE* FN_mbGetLockedViewDC)(mbWebView webView);
static FN_mbGetLockedViewDC s_mbGetLockedViewDC = nullptr;

typedef void (MB_CALL_TYPE* FN_mbUnlockViewDC)(mbWebView webView);
static FN_mbUnlockViewDC s_mbUnlockViewDC = nullptr;

typedef void (MB_CALL_TYPE* FN_mbWake)(mbWebView webView);
static FN_mbWake s_mbWake = nullptr;

typedef double (MB_CALL_TYPE* FN_mbJsToDouble)(mbJsExecState es, mbJsValue v);
static FN_mbJsToDouble s_mbJsToDouble = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbJsToBoolean)(mbJsExecState es, mbJsValue v);
static FN_mbJsToBoolean s_mbJsToBoolean = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbJsToString)(mbJsExecState es, mbJsValue v);
static FN_mbJsToString s_mbJsToString = nullptr;

typedef mbJsType(MB_CALL_TYPE* FN_mbGetJsValueType)(mbJsExecState es, mbJsValue v);
static FN_mbGetJsValueType s_mbGetJsValueType = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnJsQuery)(mbWebView webView, mbJsQueryCallback callback, void* param);
static FN_mbOnJsQuery s_mbOnJsQuery = nullptr;

typedef void (MB_CALL_TYPE* FN_mbResponseQuery)(mbWebView webView, int64_t queryId, int customMsg, const utf8* response);
static FN_mbResponseQuery s_mbResponseQuery = nullptr;

typedef void (MB_CALL_TYPE* FN_mbRunJs)(mbWebView webView, mbWebFrameHandle frameId, const utf8* script, BOOL isInClosure, mbRunJsCallback callback, void* param, void* unuse);
static FN_mbRunJs s_mbRunJs = nullptr;

typedef mbJsValue(MB_CALL_TYPE* FN_mbRunJsSync)(mbWebView webView, mbWebFrameHandle frameId, const utf8* script, BOOL isInClosure);
static FN_mbRunJsSync s_mbRunJsSync = nullptr;

typedef mbWebFrameHandle(MB_CALL_TYPE* FN_mbWebFrameGetMainFrame)(mbWebView webView);
static FN_mbWebFrameGetMainFrame s_mbWebFrameGetMainFrame = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbIsMainFrame)(mbWebView webView, mbWebFrameHandle frameId);
static FN_mbIsMainFrame s_mbIsMainFrame = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetNodeJsEnable)(mbWebView webView, BOOL b);
static FN_mbSetNodeJsEnable s_mbSetNodeJsEnable = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetDeviceParameter)(mbWebView webView, const char* device, const char* paramStr, int paramInt, float paramFloat);
static FN_mbSetDeviceParameter s_mbSetDeviceParameter = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGetContentAsMarkup)(mbWebView webView, mbGetContentAsMarkupCallback calback, void* param, mbWebFrameHandle frameId);
static FN_mbGetContentAsMarkup s_mbGetContentAsMarkup = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGetSource)(mbWebView webView, mbGetSourceCallback calback, void* param);
static FN_mbGetSource s_mbGetSource = nullptr;

typedef void (MB_CALL_TYPE* FN_mbUtilSerializeToMHTML)(mbWebView webView, mbGetSourceCallback calback, void* param);
static FN_mbUtilSerializeToMHTML s_mbUtilSerializeToMHTML = nullptr;

typedef const char* (MB_CALL_TYPE* FN_mbUtilCreateRequestCode)(const char* registerInfo);
static FN_mbUtilCreateRequestCode s_mbUtilCreateRequestCode = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbUtilIsRegistered)(const wchar_t* defaultPath);
static FN_mbUtilIsRegistered s_mbUtilIsRegistered = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbUtilPrint)(mbWebView webView, mbWebFrameHandle frameId, const mbPrintSettings* printParams);
static FN_mbUtilPrint s_mbUtilPrint = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbUtilBase64Encode)(const utf8* str);
static FN_mbUtilBase64Encode s_mbUtilBase64Encode = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbUtilBase64Decode)(const utf8* str);
static FN_mbUtilBase64Decode s_mbUtilBase64Decode = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbUtilDecodeURLEscape)(const utf8* url);
static FN_mbUtilDecodeURLEscape s_mbUtilDecodeURLEscape = nullptr;

typedef const utf8* (MB_CALL_TYPE* FN_mbUtilEncodeURLEscape)(const utf8* url);
static FN_mbUtilEncodeURLEscape s_mbUtilEncodeURLEscape = nullptr;

typedef const mbMemBuf* (MB_CALL_TYPE* FN_mbUtilCreateV8Snapshot)(const utf8* str);
static FN_mbUtilCreateV8Snapshot s_mbUtilCreateV8Snapshot = nullptr;

typedef void (MB_CALL_TYPE* FN_mbUtilPrintToPdf)(mbWebView webView, mbWebFrameHandle frameId, const mbPrintSettings* settings, mbPrintPdfDataCallback callback, void* param);
static FN_mbUtilPrintToPdf s_mbUtilPrintToPdf = nullptr;

typedef void (MB_CALL_TYPE* FN_mbUtilPrintToBitmap)(mbWebView webView, mbWebFrameHandle frameId, const mbScreenshotSettings* settings, mbPrintBitmapCallback callback, void* param);
static FN_mbUtilPrintToBitmap s_mbUtilPrintToBitmap = nullptr;

typedef void (MB_CALL_TYPE* FN_mbUtilScreenshot)(mbWebView webView, const mbScreenshotSettings* settings, mbOnScreenshot callback, void* param);
static FN_mbUtilScreenshot s_mbUtilScreenshot = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbUtilsSilentPrint)(mbWebView webView, const char* settings);
static FN_mbUtilsSilentPrint s_mbUtilsSilentPrint = nullptr;

typedef void (MB_CALL_TYPE* FN_mbUtilSetDefaultPrinterSettings)(mbWebView webView, const mbDefaultPrinterSettings* setting);
static FN_mbUtilSetDefaultPrinterSettings s_mbUtilSetDefaultPrinterSettings = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbPopupDownloadMgr)(mbWebView webView, const char* url, void* downloadJob);
static FN_mbPopupDownloadMgr s_mbPopupDownloadMgr = nullptr;

typedef mbDownloadOpt(MB_CALL_TYPE* FN_mbPopupDialogAndDownload)(mbWebView webView, void* param, size_t contentLength, const char* url, const char* mime, const char* disposition, mbNetJob job, mbNetJobDataBind* dataBind, mbDownloadBind* callbackBind);
static FN_mbPopupDialogAndDownload s_mbPopupDialogAndDownload = nullptr;

typedef mbDownloadOpt(MB_CALL_TYPE* FN_mbDownloadByPath)(mbWebView webView, void* param, const WCHAR* path, size_t contentLength, const char* url, const char* mime, const char* disposition, mbNetJob job, mbNetJobDataBind* dataBind, mbDownloadBind* callbackBind);
static FN_mbDownloadByPath s_mbDownloadByPath = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGetPdfPageData)(mbWebView webView, mbOnGetPdfPageDataCallback callback, void* param);
static FN_mbGetPdfPageData s_mbGetPdfPageData = nullptr;

typedef mbMemBuf* (MB_CALL_TYPE* FN_mbCreateMemBuf)(mbWebView webView, void* buf, size_t length);
static FN_mbCreateMemBuf s_mbCreateMemBuf = nullptr;

typedef void (MB_CALL_TYPE* FN_mbFreeMemBuf)(mbMemBuf* buf);
static FN_mbFreeMemBuf s_mbFreeMemBuf = nullptr;

typedef void (MB_CALL_TYPE* FN_mbPluginListBuilderAddPlugin)(void* builder, const utf8* name, const utf8* description, const utf8* fileName);
static FN_mbPluginListBuilderAddPlugin s_mbPluginListBuilderAddPlugin = nullptr;

typedef void (MB_CALL_TYPE* FN_mbPluginListBuilderAddMediaTypeToLastPlugin)(void* builder, const utf8* name, const utf8* description);
static FN_mbPluginListBuilderAddMediaTypeToLastPlugin s_mbPluginListBuilderAddMediaTypeToLastPlugin = nullptr;

typedef void (MB_CALL_TYPE* FN_mbPluginListBuilderAddFileExtensionToLastMediaType)(void* builder, const utf8* fileExtension);
static FN_mbPluginListBuilderAddFileExtensionToLastMediaType s_mbPluginListBuilderAddFileExtensionToLastMediaType = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEnableHighDPISupport)();
static FN_mbEnableHighDPISupport s_mbEnableHighDPISupport = nullptr;

typedef void (MB_CALL_TYPE* FN_mbRunMessageLoop)();
static FN_mbRunMessageLoop s_mbRunMessageLoop = nullptr;

typedef void* (MB_CALL_TYPE* FN_mbGetUserKeyValue)(mbWebView webviewHandle, const char* key);
static FN_mbGetUserKeyValue s_mbGetUserKeyValue = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetUserKeyValue)(mbWebView webviewHandle, const char* key, void* value);
static FN_mbSetUserKeyValue s_mbSetUserKeyValue = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnWillReleaseScriptContext)(mbWebView webviewHandle, mbWillReleaseScriptContextCallback callback, void* callbackParam);
static FN_mbOnWillReleaseScriptContext s_mbOnWillReleaseScriptContext = nullptr;

typedef void (MB_CALL_TYPE* FN_mbCallBlinkThreadAsync)(mbThreadCallback callback, void* param1, void* param2);
static FN_mbCallBlinkThreadAsync s_mbCallBlinkThreadAsync = nullptr;

typedef void (MB_CALL_TYPE* FN_mbCallBlinkThreadSync)(mbThreadCallback callback, void* param1, void* param2);
static FN_mbCallBlinkThreadSync s_mbCallBlinkThreadSync = nullptr;

typedef void (MB_CALL_TYPE* FN_mbCallUiThreadAsync)(mbThreadCallback callback, void* param1, void* param2);
static FN_mbCallUiThreadAsync s_mbCallUiThreadAsync = nullptr;

typedef void (MB_CALL_TYPE* FN_mbCallUiThreadSync)(mbThreadCallback callback, void* param1, void* param2);
static FN_mbCallUiThreadSync s_mbCallUiThreadSync = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEditorRedo)(mbWebView webviewHandle);
static FN_mbEditorRedo s_mbEditorRedo = nullptr;

typedef void (MB_CALL_TYPE* FN_mbEditorUnSelect)(mbWebView webviewHandle);
static FN_mbEditorUnSelect s_mbEditorUnSelect = nullptr;

typedef v8Isolate(MB_CALL_TYPE* FN_mbGetBlinkMainThreadIsolate)();
static FN_mbGetBlinkMainThreadIsolate s_mbGetBlinkMainThreadIsolate = nullptr;

typedef int (MB_CALL_TYPE* FN_mbGetContentHeight)(mbWebView webviewHandle);
static FN_mbGetContentHeight s_mbGetContentHeight = nullptr;

typedef int (MB_CALL_TYPE* FN_mbGetContentWidth)(mbWebView webviewHandle);
static FN_mbGetContentWidth s_mbGetContentWidth = nullptr;

typedef mbJsExecState(MB_CALL_TYPE* FN_mbGetGlobalExecByFrame)(mbWebView webviewHandle, mbWebFrameHandle frameId);
static FN_mbGetGlobalExecByFrame s_mbGetGlobalExecByFrame = nullptr;

typedef void* (MB_CALL_TYPE* FN_mbGetProcAddr)(const char* name);
static FN_mbGetProcAddr s_mbGetProcAddr = nullptr;

typedef mbWebView(MB_CALL_TYPE* FN_mbGetWebViewForCurrentContext)();
static FN_mbGetWebViewForCurrentContext s_mbGetWebViewForCurrentContext = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGoToIndex)(mbWebView webviewHandle, int index);
static FN_mbGoToIndex s_mbGoToIndex = nullptr;

typedef void (MB_CALL_TYPE* FN_mbGoToOffset)(mbWebView webviewHandle, int offset);
static FN_mbGoToOffset s_mbGoToOffset = nullptr;

typedef void (MB_CALL_TYPE* FN_mbInsertCSSByFrame)(mbWebView webviewHandle, mbWebFrameHandle frameId, const utf8* cssText);
static FN_mbInsertCSSByFrame s_mbInsertCSSByFrame = nullptr;

typedef void* (MB_CALL_TYPE* FN_mbJsToV8Value)(mbJsExecState es, mbJsValue v);
static FN_mbJsToV8Value s_mbJsToV8Value = nullptr;

typedef const char* (MB_CALL_TYPE* FN_mbNetGetReferrer)(mbNetJob jobPtr);
static FN_mbNetGetReferrer s_mbNetGetReferrer = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnBlinkThreadInit)(mbThreadCallback callback, void* param1, void* param2);
static FN_mbOnBlinkThreadInit s_mbOnBlinkThreadInit = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnDocumentReadyInBlinkThread)(mbWebView webviewHandle, mbDocumentReadyCallback callback, void* param);
static FN_mbOnDocumentReadyInBlinkThread s_mbOnDocumentReadyInBlinkThread = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnLoadUrlFinish)(mbWebView webviewHandle, mbLoadUrlFinishCallback callback, void* param);
static FN_mbOnLoadUrlFinish s_mbOnLoadUrlFinish = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnLoadUrlHeadersReceived)(mbWebView webviewHandle, mbLoadUrlHeadersReceivedCallback callback, void* param);
static FN_mbOnLoadUrlHeadersReceived s_mbOnLoadUrlHeadersReceived = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnNodeCreateProcess)(mbWebView webviewHandle, mbNodeOnCreateProcessCallback callback, void* param);
static FN_mbOnNodeCreateProcess s_mbOnNodeCreateProcess = nullptr;

typedef void (MB_CALL_TYPE* FN_mbOnThreadIdle)(mbThreadCallback callback, void* param1, void* param2);
static FN_mbOnThreadIdle s_mbOnThreadIdle = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbRegisterEmbedderCustomElement)(mbWebView webviewHandle, mbWebFrameHandle frameId, const char* name, void* options, void* outResult);
static FN_mbRegisterEmbedderCustomElement s_mbRegisterEmbedderCustomElement = nullptr;

typedef void (MB_CALL_TYPE* FN_mbSetEditable)(mbWebView webviewHandle, bool editable);
static FN_mbSetEditable s_mbSetEditable = nullptr;

typedef void (MB_CALL_TYPE* FN_mbWebFrameGetMainWorldScriptContext)(mbWebView webviewHandle, mbWebFrameHandle frameId, v8ContextPtr contextOut);
static FN_mbWebFrameGetMainWorldScriptContext s_mbWebFrameGetMainWorldScriptContext = nullptr;

//-------
MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbUninit()
{
    return s_mbUninit();
}

MB_EXTERN_C MB_DLLEXPORT mbSettings* MB_CALL_TYPE mbCreateInitSettings()
{
    return s_mbCreateInitSettings();
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetInitSettings(mbSettings* settings, const char* name, const char* value)
{
    return s_mbSetInitSettings(settings, name, value);
}

MB_EXTERN_C MB_DLLEXPORT mbWebView MB_CALL_TYPE mbCreateWebView()
{
    return s_mbCreateWebView();
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbDestroyWebView(mbWebView webview)
{
    return s_mbDestroyWebView(webview);
}

MB_EXTERN_C MB_DLLEXPORT mbWebView MB_CALL_TYPE mbCreateWebWindow(mbWindowType type, HWND parent, int x, int y, int width, int height)
{
    return s_mbCreateWebWindow(type, parent, x, y, width, height);
}

MB_EXTERN_C MB_DLLEXPORT mbWebView MB_CALL_TYPE mbCreateWebCustomWindow(HWND parent, DWORD style, DWORD styleEx, int x, int y, int width, int height)
{
    return s_mbCreateWebCustomWindow(parent, style, styleEx, x, y, width, height);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbMoveWindow(mbWebView webview, int x, int y, int w, int h)
{
    return s_mbMoveWindow(webview, x, y, w, h);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbMoveToCenter(mbWebView webview)
{
    return s_mbMoveToCenter(webview);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetAutoDrawToHwnd(mbWebView webview, BOOL b)
{
    return s_mbSetAutoDrawToHwnd(webview, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGetCaretRect(mbWebView webviewHandle, mbRect* r)
{
    return s_mbGetCaretRect(webviewHandle, r);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetAudioMuted(mbWebView webview, BOOL b)
{
    return s_mbSetAudioMuted(webview, b);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbIsAudioMuted(mbWebView webview)
{
    return s_mbIsAudioMuted(webview);
}

MB_EXTERN_C MB_DLLEXPORT mbStringPtr MB_CALL_TYPE mbCreateString(const utf8* str, size_t length)
{
    return s_mbCreateString(str, length);
}

MB_EXTERN_C MB_DLLEXPORT mbStringPtr MB_CALL_TYPE mbCreateStringWithoutNullTermination(const utf8* str, size_t length)
{
    return s_mbCreateStringWithoutNullTermination(str, length);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbDeleteString(mbStringPtr str)
{
    return s_mbDeleteString(str);
}

MB_EXTERN_C MB_DLLEXPORT size_t MB_CALL_TYPE mbGetStringLen(mbStringPtr str)
{
    return s_mbGetStringLen(str);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbGetString(mbStringPtr str)
{
    return s_mbGetString(str);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetProxy(mbWebView webView, const mbProxy* proxy)
{
    return s_mbSetProxy(webView, proxy);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDebugConfig(mbWebView webView, const char* debugString, const char* param)
{
    return s_mbSetDebugConfig(webView, debugString, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetSetData(mbNetJob jobPtr, void* buf, int len)
{
    return s_mbNetSetData(jobPtr, buf, len);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetHookRequest(mbNetJob jobPtr)
{
    return s_mbNetHookRequest(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetChangeRequestUrl(mbNetJob jobPtr, const char* url)
{
    return s_mbNetChangeRequestUrl(jobPtr, url);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetContinueJob(mbNetJob jobPtr)
{
    return s_mbNetContinueJob(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT const mbSlist* MB_CALL_TYPE mbNetGetRawHttpHeadInBlinkThread(mbNetJob jobPtr)
{
    return s_mbNetGetRawHttpHeadInBlinkThread(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT const mbSlist* MB_CALL_TYPE mbNetGetRawResponseHeadInBlinkThread(mbNetJob jobPtr)
{
    return s_mbNetGetRawResponseHeadInBlinkThread(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetHoldJobToAsynCommit(mbNetJob jobPtr)
{
    return s_mbNetHoldJobToAsynCommit(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetCancelRequest(mbNetJob jobPtr)
{
    return s_mbNetCancelRequest(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetOnResponse(mbWebView webviewHandle, mbNetResponseCallback callback, void* param)
{
    return s_mbNetOnResponse(webviewHandle, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetSetWebsocketCallback(mbWebView webview, const mbWebsocketHookCallbacks* callbacks, void* param)
{
    return s_mbNetSetWebsocketCallback(webview, callbacks, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetSendWsText(mbWebSocketChannel channel, const char* buf, size_t len)
{
    return s_mbNetSendWsText(channel, buf, len);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetSendWsBlob(mbWebSocketChannel channel, const char* buf, size_t len)
{
    return s_mbNetSendWsBlob(channel, buf, len);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetEnableResPacket(mbWebView webviewHandle, const WCHAR* pathName)
{
    return s_mbNetEnableResPacket(webviewHandle, pathName);
}

MB_EXTERN_C MB_DLLEXPORT mbPostBodyElements* MB_CALL_TYPE mbNetGetPostBody(mbNetJob jobPtr)
{
    return s_mbNetGetPostBody(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT mbPostBodyElements* MB_CALL_TYPE mbNetCreatePostBodyElements(mbWebView webView, size_t length)
{
    return s_mbNetCreatePostBodyElements(webView, length);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetFreePostBodyElements(mbPostBodyElements* elements)
{
    return s_mbNetFreePostBodyElements(elements);
}

MB_EXTERN_C MB_DLLEXPORT mbPostBodyElement* MB_CALL_TYPE mbNetCreatePostBodyElement(mbWebView webView)
{
    return s_mbNetCreatePostBodyElement(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetFreePostBodyElement(mbPostBodyElement* element)
{
    return s_mbNetFreePostBodyElement(element);
}

MB_EXTERN_C MB_DLLEXPORT mbWebUrlRequestPtr MB_CALL_TYPE mbNetCreateWebUrlRequest(const utf8* url, const utf8* method, const utf8* mime)
{
    return s_mbNetCreateWebUrlRequest(url, method, mime);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetAddHTTPHeaderFieldToUrlRequest(mbWebUrlRequestPtr request, const utf8* name, const utf8* value)
{
    return s_mbNetAddHTTPHeaderFieldToUrlRequest(request, name, value);
}

MB_EXTERN_C MB_DLLEXPORT int MB_CALL_TYPE mbNetStartUrlRequest(mbWebView webView, mbWebUrlRequestPtr request, void* param, const mbUrlRequestCallbacks* callbacks)
{
    return s_mbNetStartUrlRequest(webView, request, param, callbacks);
}

MB_EXTERN_C MB_DLLEXPORT int MB_CALL_TYPE mbNetGetHttpStatusCode(mbWebUrlResponsePtr response)
{
    return s_mbNetGetHttpStatusCode(response);
}

MB_EXTERN_C MB_DLLEXPORT mbRequestType MB_CALL_TYPE mbNetGetRequestMethod(mbNetJob jobPtr)
{
    return s_mbNetGetRequestMethod(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT __int64 MB_CALL_TYPE mbNetGetExpectedContentLength(mbWebUrlResponsePtr response)
{
    return s_mbNetGetExpectedContentLength(response);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbNetGetResponseUrl(mbWebUrlResponsePtr response)
{
    return s_mbNetGetResponseUrl(response);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetCancelWebUrlRequest(int requestId)
{
    return s_mbNetCancelWebUrlRequest(requestId);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetViewProxy(mbWebView webView, const mbProxy* proxy)
{
    return s_mbSetViewProxy(webView, proxy);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetSetMIMEType(mbNetJob jobPtr, const char* type)
{
    return s_mbNetSetMIMEType(jobPtr, type);
}

MB_EXTERN_C MB_DLLEXPORT const char* MB_CALL_TYPE mbNetGetMIMEType(mbNetJob jobPtr)
{
    return s_mbNetGetMIMEType(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbNetGetHTTPHeaderField(mbNetJob job, const char* key, BOOL fromRequestOrResponse)
{
    return s_mbNetGetHTTPHeaderField(job, key, fromRequestOrResponse);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNetSetHTTPHeaderField(mbNetJob jobPtr, const wchar_t* key, const wchar_t* value, BOOL response)
{
    return s_mbNetSetHTTPHeaderField(jobPtr, key, value, response);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetMouseEnabled(mbWebView webView, BOOL b)
{
    return s_mbSetMouseEnabled(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetTouchEnabled(mbWebView webView, BOOL b)
{
    return s_mbSetTouchEnabled(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetSystemTouchEnabled(mbWebView webView, BOOL b)
{
    return s_mbSetSystemTouchEnabled(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetContextMenuEnabled(mbWebView webView, BOOL b)
{
    return s_mbSetContextMenuEnabled(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetNavigationToNewWindowEnable(mbWebView webView, BOOL b)
{
    return s_mbSetNavigationToNewWindowEnable(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetHeadlessEnabled(mbWebView webView, BOOL b)
{
    return s_mbSetHeadlessEnabled(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDragDropEnable(mbWebView webView, BOOL b)
{
    return s_mbSetDragDropEnable(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDragEnable(mbWebView webView, BOOL b)
{
    return s_mbSetDragEnable(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetContextMenuItemShow(mbWebView webView, mbMenuItemId item, BOOL isShow)
{
    return s_mbSetContextMenuItemShow(webView, item, isShow);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetHandle(mbWebView webView, HWND wnd)
{
    return s_mbSetHandle(webView, wnd);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetHandleOffset(mbWebView webView, int x, int y)
{
    return s_mbSetHandleOffset(webView, x, y);
}

MB_EXTERN_C MB_DLLEXPORT HWND MB_CALL_TYPE mbGetHostHWND(mbWebView webView)
{
    return s_mbGetHostHWND(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetTransparent(mbWebView webviewHandle, BOOL transparent)
{
    return s_mbSetTransparent(webviewHandle, transparent);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetViewSettings(mbWebView webviewHandle, const mbViewSettings* settings)
{
    return s_mbSetViewSettings(webviewHandle, settings);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetCspCheckEnable(mbWebView webView, BOOL b)
{
    return s_mbSetCspCheckEnable(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetNpapiPluginsEnabled(mbWebView webView, BOOL b)
{
    return s_mbSetNpapiPluginsEnabled(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetMemoryCacheEnable(mbWebView webView, BOOL b)
{
    return s_mbSetMemoryCacheEnable(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetCookie(mbWebView webView, const utf8* url, const utf8* cookie)
{
    return s_mbSetCookie(webView, url, cookie);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetCookieEnabled(mbWebView webView, BOOL enable)
{
    return s_mbSetCookieEnabled(webView, enable);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetCookieJarPath(mbWebView webView, const WCHAR* path)
{
    return s_mbSetCookieJarPath(webView, path);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetCookieJarFullPath(mbWebView webView, const WCHAR* path)
{
    return s_mbSetCookieJarFullPath(webView, path);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetLocalStorageFullPath(mbWebView webView, const WCHAR* path)
{
    return s_mbSetLocalStorageFullPath(webView, path);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbGetTitle(mbWebView webView)
{
    return s_mbGetTitle(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetWindowTitle(mbWebView webView, const utf8* title)
{
    return s_mbSetWindowTitle(webView, title);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetWindowTitleW(mbWebView webView, const wchar_t* title)
{
    return s_mbSetWindowTitleW(webView, title);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbGetUrl(mbWebView webView)
{
    return s_mbGetUrl(webView);
}

MB_EXTERN_C MB_DLLEXPORT int MB_CALL_TYPE mbGetCursorInfoType(mbWebView webView)
{
    return s_mbGetCursorInfoType(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbAddPluginDirectory(mbWebView webView, const WCHAR* path)
{
    return s_mbAddPluginDirectory(webView, path);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetUserAgent(mbWebView webView, const utf8* userAgent)
{
    return s_mbSetUserAgent(webView, userAgent);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetZoomFactor(mbWebView webView, float factor)
{
    return s_mbSetZoomFactor(webView, factor);
}

MB_EXTERN_C MB_DLLEXPORT float MB_CALL_TYPE mbGetZoomFactor(mbWebView webView)
{
    return s_mbGetZoomFactor(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDiskCacheEnabled(mbWebView webView, BOOL enable)
{
    return s_mbSetDiskCacheEnabled(webView, enable);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDiskCachePath(mbWebView webView, const WCHAR* path)
{
    return s_mbSetDiskCachePath(webView, path);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDiskCacheLimit(mbWebView webView, size_t limit)
{
    return s_mbSetDiskCacheLimit(webView, limit);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDiskCacheLimitDisk(mbWebView webView, size_t limit)
{
    return s_mbSetDiskCacheLimitDisk(webView, limit);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDiskCacheLevel(mbWebView webView, int Level)
{
    return s_mbSetDiskCacheLevel(webView, Level);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetResourceGc(mbWebView webView, int intervalSec)
{
    return s_mbSetResourceGc(webView, intervalSec);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbCanGoForward(mbWebView webView, mbCanGoBackForwardCallback callback, void* param)
{
    return s_mbCanGoForward(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbCanGoBack(mbWebView webView, mbCanGoBackForwardCallback callback, void* param)
{
    return s_mbCanGoBack(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGetCookie(mbWebView webView, mbGetCookieCallback callback, void* param)
{
    return s_mbGetCookie(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbGetCookieOnBlinkThread(mbWebView webView)
{
    return s_mbGetCookieOnBlinkThread(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbClearCookie(mbWebView webView)
{
    return s_mbClearCookie(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbResize(mbWebView webView, int w, int h)
{
    return s_mbResize(webView, w, h);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnNavigation(mbWebView webView, mbNavigationCallback callback, void* param)
{
    return s_mbOnNavigation(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnNavigationSync(mbWebView webView, mbNavigationCallback callback, void* param)
{
    return s_mbOnNavigationSync(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnCreateView(mbWebView webView, mbCreateViewCallback callback, void* param)
{
    return s_mbOnCreateView(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnDocumentReady(mbWebView webView, mbDocumentReadyCallback callback, void* param)
{
    return s_mbOnDocumentReady(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnPaintUpdated(mbWebView webView, mbPaintUpdatedCallback callback, void* callbackParam)
{
    return s_mbOnPaintUpdated(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnPaintBitUpdated(mbWebView webView, mbPaintBitUpdatedCallback callback, void* callbackParam)
{
    return s_mbOnPaintBitUpdated(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnAcceleratedPaint(mbWebView webView, mbAcceleratedPaintCallback callback, void* callbackParam)
{
    return s_mbOnAcceleratedPaint(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnLoadUrlBegin(mbWebView webView, mbLoadUrlBeginCallback callback, void* callbackParam)
{
    return s_mbOnLoadUrlBegin(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnLoadUrlEnd(mbWebView webView, mbLoadUrlEndCallback callback, void* callbackParam)
{
    return s_mbOnLoadUrlEnd(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnLoadUrlFail(mbWebView webView, mbLoadUrlFailCallback callback, void* callbackParam)
{
    return s_mbOnLoadUrlFail(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnTitleChanged(mbWebView webView, mbTitleChangedCallback callback, void* callbackParam)
{
    return s_mbOnTitleChanged(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnURLChanged(mbWebView webView, mbURLChangedCallback callback, void* callbackParam)
{
    return s_mbOnURLChanged(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnLoadingFinish(mbWebView webView, mbLoadingFinishCallback callback, void* param)
{
    return s_mbOnLoadingFinish(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnDownload(mbWebView webView, mbDownloadCallback callback, void* param)
{
    return s_mbOnDownload(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnDownloadInBlinkThread(mbWebView webView, mbDownloadInBlinkThreadCallback callback, void* param)
{
    return s_mbOnDownloadInBlinkThread(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnAlertBox(mbWebView webView, mbAlertBoxCallback callback, void* param)
{
    return s_mbOnAlertBox(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnConfirmBox(mbWebView webView, mbConfirmBoxCallback callback, void* param)
{
    return s_mbOnConfirmBox(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnPromptBox(mbWebView webView, mbPromptBoxCallback callback, void* param)
{
    return s_mbOnPromptBox(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnNetGetFavicon(mbWebView webView, mbNetGetFaviconCallback callback, void* param)
{
    return s_mbOnNetGetFavicon(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnConsole(mbWebView webView, mbConsoleCallback callback, void* param)
{
    return s_mbOnConsole(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbOnClose(mbWebView webView, mbCloseCallback callback, void* param)
{
    return s_mbOnClose(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbOnDestroy(mbWebView webView, mbDestroyCallback callback, void* param)
{
    return s_mbOnDestroy(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbOnPrinting(mbWebView webView, mbPrintingCallback callback, void* param)
{
    return s_mbOnPrinting(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnPluginList(mbWebView webView, mbGetPluginListCallback callback, void* callbackParam)
{
    return s_mbOnPluginList(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnImageBufferToDataURL(mbWebView webView, mbImageBufferToDataURLCallback callback, void* callbackParam)
{
    return s_mbOnImageBufferToDataURL(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnDidCreateScriptContext(mbWebView webView, mbDidCreateScriptContextCallback callback, void* callbackParam)
{
    return s_mbOnDidCreateScriptContext(webView, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGoBack(mbWebView webView)
{
    return s_mbGoBack(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGoForward(mbWebView webView)
{
    return s_mbGoForward(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbNavigateAtIndex(mbWebView webView, int index)
{
    return s_mbNavigateAtIndex(webView, index);
}

MB_EXTERN_C MB_DLLEXPORT int MB_CALL_TYPE mbGetNavigateIndex(mbWebView webView)
{
    return s_mbGetNavigateIndex(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbStopLoading(mbWebView webView)
{
    return s_mbStopLoading(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbReload(mbWebView webView)
{
    return s_mbReload(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbPerformCookieCommand(mbWebView webView, mbCookieCommand command)
{
    return s_mbPerformCookieCommand(webView, command);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEditorSelectAll(mbWebView webView)
{
    return s_mbEditorSelectAll(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEditorCopy(mbWebView webView)
{
    return s_mbEditorCopy(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEditorCut(mbWebView webView)
{
    return s_mbEditorCut(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEditorPaste(mbWebView webView)
{
    return s_mbEditorPaste(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEditorDelete(mbWebView webView)
{
    return s_mbEditorDelete(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEditorUndo(mbWebView webView)
{
    return s_mbEditorUndo(webView);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbFireMouseEvent(mbWebView webView, unsigned int message, int x, int y, unsigned int flags)
{
    return s_mbFireMouseEvent(webView, message, x, y, flags);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbFireContextMenuEvent(mbWebView webView, int x, int y, unsigned int flags)
{
    return s_mbFireContextMenuEvent(webView, x, y, flags);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbFireMouseWheelEvent(mbWebView webView, int x, int y, int delta, unsigned int flags)
{
    return s_mbFireMouseWheelEvent(webView, x, y, delta, flags);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbFireKeyUpEvent(mbWebView webView, unsigned int virtualKeyCode, unsigned int flags, BOOL systemKey)
{
    return s_mbFireKeyUpEvent(webView, virtualKeyCode, flags, systemKey);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbFireKeyDownEvent(mbWebView webView, unsigned int virtualKeyCode, unsigned int flags, BOOL systemKey)
{
    return s_mbFireKeyDownEvent(webView, virtualKeyCode, flags, systemKey);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbFireKeyPressEvent(mbWebView webView, unsigned int charCode, unsigned int flags, BOOL systemKey)
{
    return s_mbFireKeyPressEvent(webView, charCode, flags, systemKey);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbFireWindowsMessage(mbWebView webView, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
    return s_mbFireWindowsMessage(webView, hWnd, message, wParam, lParam, result);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetFocus(mbWebView webView)
{
    return s_mbSetFocus(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbKillFocus(mbWebView webView)
{
    return s_mbKillFocus(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbShowWindow(mbWebView webview, BOOL show)
{
    return s_mbShowWindow(webview, show);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbLoadURL(mbWebView webView, const utf8* url)
{
    return s_mbLoadURL(webView, url);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbLoadHtmlWithBaseUrl(mbWebView webView, const utf8* html, const utf8* baseUrl)
{
    return s_mbLoadHtmlWithBaseUrl(webView, html, baseUrl);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbPostURL(mbWebView webView, const utf8* url, const char* postData, int postLen)
{
    return s_mbPostURL(webView, url, postData, postLen);
}

MB_EXTERN_C MB_DLLEXPORT HDC MB_CALL_TYPE mbGetLockedViewDC(mbWebView webView)
{
    return s_mbGetLockedViewDC(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbUnlockViewDC(mbWebView webView)
{
    return s_mbUnlockViewDC(webView);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbWake(mbWebView webView)
{
    return s_mbWake(webView);
}

MB_EXTERN_C MB_DLLEXPORT double MB_CALL_TYPE mbJsToDouble(mbJsExecState es, mbJsValue v)
{
    return s_mbJsToDouble(es, v);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbJsToBoolean(mbJsExecState es, mbJsValue v)
{
    return s_mbJsToBoolean(es, v);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbJsToString(mbJsExecState es, mbJsValue v)
{
    return s_mbJsToString(es, v);
}

MB_EXTERN_C MB_DLLEXPORT mbJsType MB_CALL_TYPE mbGetJsValueType(mbJsExecState es, mbJsValue v)
{
    return s_mbGetJsValueType(es, v);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnJsQuery(mbWebView webView, mbJsQueryCallback callback, void* param)
{
    return s_mbOnJsQuery(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbResponseQuery(mbWebView webView, int64_t queryId, int customMsg, const utf8* response)
{
    return s_mbResponseQuery(webView, queryId, customMsg, response);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbRunJs(mbWebView webView, mbWebFrameHandle frameId, const utf8* script, BOOL isInClosure, mbRunJsCallback callback, void* param, void* unuse)
{
    return s_mbRunJs(webView, frameId, script, isInClosure, callback, param, unuse);
}

MB_EXTERN_C MB_DLLEXPORT mbJsValue MB_CALL_TYPE mbRunJsSync(mbWebView webView, mbWebFrameHandle frameId, const utf8* script, BOOL isInClosure)
{
    return s_mbRunJsSync(webView, frameId, script, isInClosure);
}

MB_EXTERN_C MB_DLLEXPORT mbWebFrameHandle MB_CALL_TYPE mbWebFrameGetMainFrame(mbWebView webView)
{
    return s_mbWebFrameGetMainFrame(webView);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbIsMainFrame(mbWebView webView, mbWebFrameHandle frameId)
{
    return s_mbIsMainFrame(webView, frameId);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetNodeJsEnable(mbWebView webView, BOOL b)
{
    return s_mbSetNodeJsEnable(webView, b);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetDeviceParameter(mbWebView webView, const char* device, const char* paramStr, int paramInt, float paramFloat)
{
    return s_mbSetDeviceParameter(webView, device, paramStr, paramInt, paramFloat);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGetContentAsMarkup(mbWebView webView, mbGetContentAsMarkupCallback calback, void* param, mbWebFrameHandle frameId)
{
    return s_mbGetContentAsMarkup(webView, calback, param, frameId);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGetSource(mbWebView webView, mbGetSourceCallback calback, void* param)
{
    return s_mbGetSource(webView, calback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbUtilSerializeToMHTML(mbWebView webView, mbGetSourceCallback calback, void* param)
{
    return s_mbUtilSerializeToMHTML(webView, calback, param);
}

MB_EXTERN_C MB_DLLEXPORT const char* MB_CALL_TYPE mbUtilCreateRequestCode(const char* registerInfo)
{
    return s_mbUtilCreateRequestCode(registerInfo);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbUtilIsRegistered(const wchar_t* defaultPath)
{
    return s_mbUtilIsRegistered(defaultPath);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbUtilPrint(mbWebView webView, mbWebFrameHandle frameId, const mbPrintSettings* printParams)
{
    return s_mbUtilPrint(webView, frameId, printParams);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbUtilBase64Encode(const utf8* str)
{
    return s_mbUtilBase64Encode(str);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbUtilBase64Decode(const utf8* str)
{
    return s_mbUtilBase64Decode(str);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbUtilDecodeURLEscape(const utf8* url)
{
    return s_mbUtilDecodeURLEscape(url);
}

MB_EXTERN_C MB_DLLEXPORT const utf8* MB_CALL_TYPE mbUtilEncodeURLEscape(const utf8* url)
{
    return s_mbUtilEncodeURLEscape(url);
}

MB_EXTERN_C MB_DLLEXPORT const mbMemBuf* MB_CALL_TYPE mbUtilCreateV8Snapshot(const utf8* str)
{
    return s_mbUtilCreateV8Snapshot(str);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbUtilPrintToPdf(mbWebView webView, mbWebFrameHandle frameId, const mbPrintSettings* settings, mbPrintPdfDataCallback callback, void* param)
{
    return s_mbUtilPrintToPdf(webView, frameId, settings, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbUtilPrintToBitmap(mbWebView webView, mbWebFrameHandle frameId, const mbScreenshotSettings* settings, mbPrintBitmapCallback callback, void* param)
{
    return s_mbUtilPrintToBitmap(webView, frameId, settings, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbUtilScreenshot(mbWebView webView, const mbScreenshotSettings* settings, mbOnScreenshot callback, void* param)
{
    return s_mbUtilScreenshot(webView, settings, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbUtilsSilentPrint(mbWebView webView, const char* settings)
{
    return s_mbUtilsSilentPrint(webView, settings);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbUtilSetDefaultPrinterSettings(mbWebView webView, const mbDefaultPrinterSettings* setting)
{
    return s_mbUtilSetDefaultPrinterSettings(webView, setting);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbPopupDownloadMgr(mbWebView webView, const char* url, void* downloadJob)
{
    return s_mbPopupDownloadMgr(webView, url, downloadJob);
}

MB_EXTERN_C MB_DLLEXPORT mbDownloadOpt MB_CALL_TYPE mbPopupDialogAndDownload(mbWebView webView, void* param, size_t contentLength, const char* url, const char* mime, const char* disposition, mbNetJob job, mbNetJobDataBind* dataBind, mbDownloadBind* callbackBind)
{
    return s_mbPopupDialogAndDownload(webView, param, contentLength, url, mime, disposition, job, dataBind, callbackBind);
}

MB_EXTERN_C MB_DLLEXPORT mbDownloadOpt MB_CALL_TYPE mbDownloadByPath(mbWebView webView, void* param, const WCHAR* path, size_t contentLength, const char* url, const char* mime, const char* disposition, mbNetJob job, mbNetJobDataBind* dataBind, mbDownloadBind* callbackBind)
{
    return s_mbDownloadByPath(webView, param, path, contentLength, url, mime, disposition, job, dataBind, callbackBind);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGetPdfPageData(mbWebView webView, mbOnGetPdfPageDataCallback callback, void* param)
{
    return s_mbGetPdfPageData(webView, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT mbMemBuf* MB_CALL_TYPE mbCreateMemBuf(mbWebView webView, void* buf, size_t length)
{
    return s_mbCreateMemBuf(webView, buf, length);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbFreeMemBuf(mbMemBuf* buf)
{
    return s_mbFreeMemBuf(buf);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbPluginListBuilderAddPlugin(void* builder, const utf8* name, const utf8* description, const utf8* fileName)
{
    return s_mbPluginListBuilderAddPlugin(builder, name, description, fileName);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbPluginListBuilderAddMediaTypeToLastPlugin(void* builder, const utf8* name, const utf8* description)
{
    return s_mbPluginListBuilderAddMediaTypeToLastPlugin(builder, name, description);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbPluginListBuilderAddFileExtensionToLastMediaType(void* builder, const utf8* fileExtension)
{
    return s_mbPluginListBuilderAddFileExtensionToLastMediaType(builder, fileExtension);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEnableHighDPISupport()
{
    return s_mbEnableHighDPISupport();
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbRunMessageLoop()
{
    return s_mbRunMessageLoop();
}

MB_EXTERN_C MB_DLLEXPORT void* MB_CALL_TYPE mbGetUserKeyValue(mbWebView webview, const char* key)
{
    return s_mbGetUserKeyValue(webview, key);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetUserKeyValue(mbWebView webview, const char* key, void* value)
{
    return s_mbSetUserKeyValue(webview, key, value);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnWillReleaseScriptContext(mbWebView webviewHandle, mbWillReleaseScriptContextCallback callback, void* callbackParam)
{
    return s_mbOnWillReleaseScriptContext(webviewHandle, callback, callbackParam);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbCallBlinkThreadAsync(mbThreadCallback callback, void* param1, void* param2)
{
    return s_mbCallBlinkThreadAsync(callback, param1, param2);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbCallBlinkThreadSync(mbThreadCallback callback, void* param1, void* param2)
{
    return s_mbCallBlinkThreadSync(callback, param1, param2);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbCallUiThreadAsync(mbThreadCallback callback, void* param1, void* param2)
{
    return s_mbCallUiThreadAsync(callback, param1, param2);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbCallUiThreadSync(mbThreadCallback callback, void* param1, void* param2)
{
    return s_mbCallUiThreadSync(callback, param1, param2);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEditorRedo(mbWebView webviewHandle)
{
    return s_mbEditorRedo(webviewHandle);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbEditorUnSelect(mbWebView webviewHandle)
{
    return s_mbEditorUnSelect(webviewHandle);
}

MB_EXTERN_C MB_DLLEXPORT v8Isolate MB_CALL_TYPE mbGetBlinkMainThreadIsolate()
{
    return s_mbGetBlinkMainThreadIsolate();
}

MB_EXTERN_C MB_DLLEXPORT int MB_CALL_TYPE mbGetContentHeight(mbWebView webviewHandle)
{
    return s_mbGetContentHeight(webviewHandle);
}

MB_EXTERN_C MB_DLLEXPORT int MB_CALL_TYPE mbGetContentWidth(mbWebView webviewHandle)
{
    return s_mbGetContentWidth(webviewHandle);
}

MB_EXTERN_C MB_DLLEXPORT mbJsExecState MB_CALL_TYPE mbGetGlobalExecByFrame(mbWebView webviewHandle, mbWebFrameHandle frameId)
{
    return s_mbGetGlobalExecByFrame(webviewHandle, frameId);
}

MB_EXTERN_C MB_DLLEXPORT void* MB_CALL_TYPE mbGetProcAddr(const char* name)
{
    return s_mbGetProcAddr(name);
}

MB_EXTERN_C MB_DLLEXPORT mbWebView MB_CALL_TYPE mbGetWebViewForCurrentContext()
{
    return s_mbGetWebViewForCurrentContext();
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGoToIndex(mbWebView webviewHandle, int index)
{
    return s_mbGoToIndex(webviewHandle, index);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbGoToOffset(mbWebView webviewHandle, int offset)
{
    return s_mbGoToOffset(webviewHandle, offset);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbInsertCSSByFrame(mbWebView webviewHandle, mbWebFrameHandle frameId, const utf8* cssText)
{
    return s_mbInsertCSSByFrame(webviewHandle, frameId, cssText);
}

MB_EXTERN_C MB_DLLEXPORT void* MB_CALL_TYPE mbJsToV8Value(mbJsExecState es, mbJsValue v)
{
    return s_mbJsToV8Value(es, v);
}

MB_EXTERN_C MB_DLLEXPORT const char* MB_CALL_TYPE mbNetGetReferrer(mbNetJob jobPtr)
{
    return s_mbNetGetReferrer(jobPtr);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnBlinkThreadInit(mbThreadCallback callback, void* param1, void* param2)
{
    return s_mbOnBlinkThreadInit(callback, param1, param2);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnDocumentReadyInBlinkThread(mbWebView webviewHandle, mbDocumentReadyCallback callback, void* param)
{
    return s_mbOnDocumentReadyInBlinkThread(webviewHandle, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnLoadUrlFinish(mbWebView webviewHandle, mbLoadUrlFinishCallback callback, void* param)
{
    return s_mbOnLoadUrlFinish(webviewHandle, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnLoadUrlHeadersReceived(mbWebView webviewHandle, mbLoadUrlHeadersReceivedCallback callback, void* param)
{
    return s_mbOnLoadUrlHeadersReceived(webviewHandle, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnNodeCreateProcess(mbWebView webviewHandle, mbNodeOnCreateProcessCallback callback, void* param)
{
    return s_mbOnNodeCreateProcess(webviewHandle, callback, param);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbOnThreadIdle(mbThreadCallback callback, void* param1, void* param2)
{
    return s_mbOnThreadIdle(callback, param1, param2);
}

MB_EXTERN_C MB_DLLEXPORT BOOL MB_CALL_TYPE mbRegisterEmbedderCustomElement(mbWebView webviewHandle, mbWebFrameHandle frameId, const char* name, void* options, void* outResult)
{
    return s_mbRegisterEmbedderCustomElement(webviewHandle, frameId, name, options, outResult);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbSetEditable(mbWebView webviewHandle, bool editable)
{
    return s_mbSetEditable(webviewHandle, editable);
}

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbWebFrameGetMainWorldScriptContext(mbWebView webviewHandle, mbWebFrameHandle frameId, v8ContextPtr contextOut)
{
    return s_mbWebFrameGetMainWorldScriptContext(webviewHandle, frameId, contextOut);
}

//------------------------------------------------------------------------------------------------

MB_EXTERN_C MB_DLLEXPORT void MB_CALL_TYPE mbInit(const mbSettings* settings)
{
    HMODULE hMiniblinkMod;
#ifdef _WIN64
    hMiniblinkMod = LoadLibraryW(L"miniblink_x64.dll");
#else
#ifdef MB_V857
    hMiniblinkMod = LoadLibraryW(L"miniblink_v857.dll");
#else
    hMiniblinkMod = LoadLibraryW(L"node.dll");
#endif
#endif
    if (!hMiniblinkMod) {
        MessageBoxW(0, L"找不到node.dll或者miniblink_x64.dll", L"错误", 0);
    }
    typedef void (MB_CALL_TYPE *FN_mbInit)(const mbSettings* settings);
    FN_mbInit mbInitExFunc = (FN_mbInit)GetProcAddress(hMiniblinkMod, "mbInit");
    mbInitExFunc(settings);

    s_mbUninit = (FN_mbUninit)GetProcAddress(hMiniblinkMod, "mbUninit");
    s_mbCreateInitSettings = (FN_mbCreateInitSettings)GetProcAddress(hMiniblinkMod, "mbCreateInitSettings");
    s_mbSetInitSettings = (FN_mbSetInitSettings)GetProcAddress(hMiniblinkMod, "mbSetInitSettings");
    s_mbCreateWebView = (FN_mbCreateWebView)GetProcAddress(hMiniblinkMod, "mbCreateWebView");
    s_mbDestroyWebView = (FN_mbDestroyWebView)GetProcAddress(hMiniblinkMod, "mbDestroyWebView");
    s_mbCreateWebWindow = (FN_mbCreateWebWindow)GetProcAddress(hMiniblinkMod, "mbCreateWebWindow");
    s_mbCreateWebCustomWindow = (FN_mbCreateWebCustomWindow)GetProcAddress(hMiniblinkMod, "mbCreateWebCustomWindow");
    s_mbMoveWindow = (FN_mbMoveWindow)GetProcAddress(hMiniblinkMod, "mbMoveWindow");
    s_mbMoveToCenter = (FN_mbMoveToCenter)GetProcAddress(hMiniblinkMod, "mbMoveToCenter");
    s_mbSetAutoDrawToHwnd = (FN_mbSetAutoDrawToHwnd)GetProcAddress(hMiniblinkMod, "mbSetAutoDrawToHwnd");
    s_mbGetCaretRect = (FN_mbGetCaretRect)GetProcAddress(hMiniblinkMod, "mbGetCaretRect");
    s_mbSetAudioMuted = (FN_mbSetAudioMuted)GetProcAddress(hMiniblinkMod, "mbSetAudioMuted");
    s_mbIsAudioMuted = (FN_mbIsAudioMuted)GetProcAddress(hMiniblinkMod, "mbIsAudioMuted");
    s_mbCreateString = (FN_mbCreateString)GetProcAddress(hMiniblinkMod, "mbCreateString");
    s_mbCreateStringWithoutNullTermination = (FN_mbCreateStringWithoutNullTermination)GetProcAddress(hMiniblinkMod, "mbCreateStringWithoutNullTermination");
    s_mbDeleteString = (FN_mbDeleteString)GetProcAddress(hMiniblinkMod, "mbDeleteString");
    s_mbGetStringLen = (FN_mbGetStringLen)GetProcAddress(hMiniblinkMod, "mbGetStringLen");
    s_mbGetString = (FN_mbGetString)GetProcAddress(hMiniblinkMod, "mbGetString");
    s_mbSetProxy = (FN_mbSetProxy)GetProcAddress(hMiniblinkMod, "mbSetProxy");
    s_mbSetDebugConfig = (FN_mbSetDebugConfig)GetProcAddress(hMiniblinkMod, "mbSetDebugConfig");
    s_mbNetSetData = (FN_mbNetSetData)GetProcAddress(hMiniblinkMod, "mbNetSetData");
    s_mbNetHookRequest = (FN_mbNetHookRequest)GetProcAddress(hMiniblinkMod, "mbNetHookRequest");
    s_mbNetChangeRequestUrl = (FN_mbNetChangeRequestUrl)GetProcAddress(hMiniblinkMod, "mbNetChangeRequestUrl");
    s_mbNetContinueJob = (FN_mbNetContinueJob)GetProcAddress(hMiniblinkMod, "mbNetContinueJob");
    s_mbNetGetRawHttpHeadInBlinkThread = (FN_mbNetGetRawHttpHeadInBlinkThread)GetProcAddress(hMiniblinkMod, "mbNetGetRawHttpHeadInBlinkThread");
    s_mbNetGetRawResponseHeadInBlinkThread = (FN_mbNetGetRawResponseHeadInBlinkThread)GetProcAddress(hMiniblinkMod, "mbNetGetRawResponseHeadInBlinkThread");
    s_mbNetHoldJobToAsynCommit = (FN_mbNetHoldJobToAsynCommit)GetProcAddress(hMiniblinkMod, "mbNetHoldJobToAsynCommit");
    s_mbNetCancelRequest = (FN_mbNetCancelRequest)GetProcAddress(hMiniblinkMod, "mbNetCancelRequest");
    s_mbNetOnResponse = (FN_mbNetOnResponse)GetProcAddress(hMiniblinkMod, "mbNetOnResponse");
    s_mbNetSetWebsocketCallback = (FN_mbNetSetWebsocketCallback)GetProcAddress(hMiniblinkMod, "mbNetSetWebsocketCallback");
    s_mbNetSendWsText = (FN_mbNetSendWsText)GetProcAddress(hMiniblinkMod, "mbNetSendWsText");
    s_mbNetSendWsBlob = (FN_mbNetSendWsBlob)GetProcAddress(hMiniblinkMod, "mbNetSendWsBlob");
    s_mbNetEnableResPacket = (FN_mbNetEnableResPacket)GetProcAddress(hMiniblinkMod, "mbNetEnableResPacket");
    s_mbNetGetPostBody = (FN_mbNetGetPostBody)GetProcAddress(hMiniblinkMod, "mbNetGetPostBody");
    s_mbNetCreatePostBodyElements = (FN_mbNetCreatePostBodyElements)GetProcAddress(hMiniblinkMod, "mbNetCreatePostBodyElements");
    s_mbNetFreePostBodyElements = (FN_mbNetFreePostBodyElements)GetProcAddress(hMiniblinkMod, "mbNetFreePostBodyElements");
    s_mbNetCreatePostBodyElement = (FN_mbNetCreatePostBodyElement)GetProcAddress(hMiniblinkMod, "mbNetCreatePostBodyElement");
    s_mbNetFreePostBodyElement = (FN_mbNetFreePostBodyElement)GetProcAddress(hMiniblinkMod, "mbNetFreePostBodyElement");
    s_mbNetCreateWebUrlRequest = (FN_mbNetCreateWebUrlRequest)GetProcAddress(hMiniblinkMod, "mbNetCreateWebUrlRequest");
    s_mbNetAddHTTPHeaderFieldToUrlRequest = (FN_mbNetAddHTTPHeaderFieldToUrlRequest)GetProcAddress(hMiniblinkMod, "mbNetAddHTTPHeaderFieldToUrlRequest");
    s_mbNetStartUrlRequest = (FN_mbNetStartUrlRequest)GetProcAddress(hMiniblinkMod, "mbNetStartUrlRequest");
    s_mbNetGetHttpStatusCode = (FN_mbNetGetHttpStatusCode)GetProcAddress(hMiniblinkMod, "mbNetGetHttpStatusCode");
    s_mbNetGetRequestMethod = (FN_mbNetGetRequestMethod)GetProcAddress(hMiniblinkMod, "mbNetGetRequestMethod");
    s_mbNetGetExpectedContentLength = (FN_mbNetGetExpectedContentLength)GetProcAddress(hMiniblinkMod, "mbNetGetExpectedContentLength");
    s_mbNetGetResponseUrl = (FN_mbNetGetResponseUrl)GetProcAddress(hMiniblinkMod, "mbNetGetResponseUrl");
    s_mbNetCancelWebUrlRequest = (FN_mbNetCancelWebUrlRequest)GetProcAddress(hMiniblinkMod, "mbNetCancelWebUrlRequest");
    s_mbSetViewProxy = (FN_mbSetViewProxy)GetProcAddress(hMiniblinkMod, "mbSetViewProxy");
    s_mbNetSetMIMEType = (FN_mbNetSetMIMEType)GetProcAddress(hMiniblinkMod, "mbNetSetMIMEType");
    s_mbNetGetMIMEType = (FN_mbNetGetMIMEType)GetProcAddress(hMiniblinkMod, "mbNetGetMIMEType");
    s_mbNetGetHTTPHeaderField = (FN_mbNetGetHTTPHeaderField)GetProcAddress(hMiniblinkMod, "mbNetGetHTTPHeaderField");
    s_mbNetSetHTTPHeaderField = (FN_mbNetSetHTTPHeaderField)GetProcAddress(hMiniblinkMod, "mbNetSetHTTPHeaderField");
    s_mbSetMouseEnabled = (FN_mbSetMouseEnabled)GetProcAddress(hMiniblinkMod, "mbSetMouseEnabled");
    s_mbSetTouchEnabled = (FN_mbSetTouchEnabled)GetProcAddress(hMiniblinkMod, "mbSetTouchEnabled");
    s_mbSetSystemTouchEnabled = (FN_mbSetSystemTouchEnabled)GetProcAddress(hMiniblinkMod, "mbSetSystemTouchEnabled");
    s_mbSetContextMenuEnabled = (FN_mbSetContextMenuEnabled)GetProcAddress(hMiniblinkMod, "mbSetContextMenuEnabled");
    s_mbSetNavigationToNewWindowEnable = (FN_mbSetNavigationToNewWindowEnable)GetProcAddress(hMiniblinkMod, "mbSetNavigationToNewWindowEnable");
    s_mbSetHeadlessEnabled = (FN_mbSetHeadlessEnabled)GetProcAddress(hMiniblinkMod, "mbSetHeadlessEnabled");
    s_mbSetDragDropEnable = (FN_mbSetDragDropEnable)GetProcAddress(hMiniblinkMod, "mbSetDragDropEnable");
    s_mbSetDragEnable = (FN_mbSetDragEnable)GetProcAddress(hMiniblinkMod, "mbSetDragEnable");
    s_mbSetContextMenuItemShow = (FN_mbSetContextMenuItemShow)GetProcAddress(hMiniblinkMod, "mbSetContextMenuItemShow");
    s_mbSetHandle = (FN_mbSetHandle)GetProcAddress(hMiniblinkMod, "mbSetHandle");
    s_mbSetHandleOffset = (FN_mbSetHandleOffset)GetProcAddress(hMiniblinkMod, "mbSetHandleOffset");
    s_mbGetHostHWND = (FN_mbGetHostHWND)GetProcAddress(hMiniblinkMod, "mbGetHostHWND");
    s_mbSetTransparent = (FN_mbSetTransparent)GetProcAddress(hMiniblinkMod, "mbSetTransparent");
    s_mbSetViewSettings = (FN_mbSetViewSettings)GetProcAddress(hMiniblinkMod, "mbSetViewSettings");
    s_mbSetCspCheckEnable = (FN_mbSetCspCheckEnable)GetProcAddress(hMiniblinkMod, "mbSetCspCheckEnable");
    s_mbSetNpapiPluginsEnabled = (FN_mbSetNpapiPluginsEnabled)GetProcAddress(hMiniblinkMod, "mbSetNpapiPluginsEnabled");
    s_mbSetMemoryCacheEnable = (FN_mbSetMemoryCacheEnable)GetProcAddress(hMiniblinkMod, "mbSetMemoryCacheEnable");
    s_mbSetCookie = (FN_mbSetCookie)GetProcAddress(hMiniblinkMod, "mbSetCookie");
    s_mbSetCookieEnabled = (FN_mbSetCookieEnabled)GetProcAddress(hMiniblinkMod, "mbSetCookieEnabled");
    s_mbSetCookieJarPath = (FN_mbSetCookieJarPath)GetProcAddress(hMiniblinkMod, "mbSetCookieJarPath");
    s_mbSetCookieJarFullPath = (FN_mbSetCookieJarFullPath)GetProcAddress(hMiniblinkMod, "mbSetCookieJarFullPath");
    s_mbSetLocalStorageFullPath = (FN_mbSetLocalStorageFullPath)GetProcAddress(hMiniblinkMod, "mbSetLocalStorageFullPath");
    s_mbGetTitle = (FN_mbGetTitle)GetProcAddress(hMiniblinkMod, "mbGetTitle");
    s_mbSetWindowTitle = (FN_mbSetWindowTitle)GetProcAddress(hMiniblinkMod, "mbSetWindowTitle");
    s_mbSetWindowTitleW = (FN_mbSetWindowTitleW)GetProcAddress(hMiniblinkMod, "mbSetWindowTitleW");
    s_mbGetUrl = (FN_mbGetUrl)GetProcAddress(hMiniblinkMod, "mbGetUrl");
    s_mbGetCursorInfoType = (FN_mbGetCursorInfoType)GetProcAddress(hMiniblinkMod, "mbGetCursorInfoType");
    s_mbAddPluginDirectory = (FN_mbAddPluginDirectory)GetProcAddress(hMiniblinkMod, "mbAddPluginDirectory");
    s_mbSetUserAgent = (FN_mbSetUserAgent)GetProcAddress(hMiniblinkMod, "mbSetUserAgent");
    s_mbSetZoomFactor = (FN_mbSetZoomFactor)GetProcAddress(hMiniblinkMod, "mbSetZoomFactor");
    s_mbGetZoomFactor = (FN_mbGetZoomFactor)GetProcAddress(hMiniblinkMod, "mbGetZoomFactor");
    s_mbSetDiskCacheEnabled = (FN_mbSetDiskCacheEnabled)GetProcAddress(hMiniblinkMod, "mbSetDiskCacheEnabled");
    s_mbSetDiskCachePath = (FN_mbSetDiskCachePath)GetProcAddress(hMiniblinkMod, "mbSetDiskCachePath");
    s_mbSetDiskCacheLimit = (FN_mbSetDiskCacheLimit)GetProcAddress(hMiniblinkMod, "mbSetDiskCacheLimit");
    s_mbSetDiskCacheLimitDisk = (FN_mbSetDiskCacheLimitDisk)GetProcAddress(hMiniblinkMod, "mbSetDiskCacheLimitDisk");
    s_mbSetDiskCacheLevel = (FN_mbSetDiskCacheLevel)GetProcAddress(hMiniblinkMod, "mbSetDiskCacheLevel");
    s_mbSetResourceGc = (FN_mbSetResourceGc)GetProcAddress(hMiniblinkMod, "mbSetResourceGc");
    s_mbCanGoForward = (FN_mbCanGoForward)GetProcAddress(hMiniblinkMod, "mbCanGoForward");
    s_mbCanGoBack = (FN_mbCanGoBack)GetProcAddress(hMiniblinkMod, "mbCanGoBack");
    s_mbGetCookie = (FN_mbGetCookie)GetProcAddress(hMiniblinkMod, "mbGetCookie");
    s_mbGetCookieOnBlinkThread = (FN_mbGetCookieOnBlinkThread)GetProcAddress(hMiniblinkMod, "mbGetCookieOnBlinkThread");
    s_mbClearCookie = (FN_mbClearCookie)GetProcAddress(hMiniblinkMod, "mbClearCookie");
    s_mbResize = (FN_mbResize)GetProcAddress(hMiniblinkMod, "mbResize");
    s_mbOnNavigation = (FN_mbOnNavigation)GetProcAddress(hMiniblinkMod, "mbOnNavigation");
    s_mbOnNavigationSync = (FN_mbOnNavigationSync)GetProcAddress(hMiniblinkMod, "mbOnNavigationSync");
    s_mbOnCreateView = (FN_mbOnCreateView)GetProcAddress(hMiniblinkMod, "mbOnCreateView");
    s_mbOnDocumentReady = (FN_mbOnDocumentReady)GetProcAddress(hMiniblinkMod, "mbOnDocumentReady");
    s_mbOnPaintUpdated = (FN_mbOnPaintUpdated)GetProcAddress(hMiniblinkMod, "mbOnPaintUpdated");
    s_mbOnPaintBitUpdated = (FN_mbOnPaintBitUpdated)GetProcAddress(hMiniblinkMod, "mbOnPaintBitUpdated");
    s_mbOnAcceleratedPaint = (FN_mbOnAcceleratedPaint)GetProcAddress(hMiniblinkMod, "mbOnAcceleratedPaint");
    s_mbOnLoadUrlBegin = (FN_mbOnLoadUrlBegin)GetProcAddress(hMiniblinkMod, "mbOnLoadUrlBegin");
    s_mbOnLoadUrlEnd = (FN_mbOnLoadUrlEnd)GetProcAddress(hMiniblinkMod, "mbOnLoadUrlEnd");
    s_mbOnLoadUrlFail = (FN_mbOnLoadUrlFail)GetProcAddress(hMiniblinkMod, "mbOnLoadUrlFail");
    s_mbOnTitleChanged = (FN_mbOnTitleChanged)GetProcAddress(hMiniblinkMod, "mbOnTitleChanged");
    s_mbOnURLChanged = (FN_mbOnURLChanged)GetProcAddress(hMiniblinkMod, "mbOnURLChanged");
    s_mbOnLoadingFinish = (FN_mbOnLoadingFinish)GetProcAddress(hMiniblinkMod, "mbOnLoadingFinish");
    s_mbOnDownload = (FN_mbOnDownload)GetProcAddress(hMiniblinkMod, "mbOnDownload");
    s_mbOnDownloadInBlinkThread = (FN_mbOnDownloadInBlinkThread)GetProcAddress(hMiniblinkMod, "mbOnDownloadInBlinkThread");
    s_mbOnAlertBox = (FN_mbOnAlertBox)GetProcAddress(hMiniblinkMod, "mbOnAlertBox");
    s_mbOnConfirmBox = (FN_mbOnConfirmBox)GetProcAddress(hMiniblinkMod, "mbOnConfirmBox");
    s_mbOnPromptBox = (FN_mbOnPromptBox)GetProcAddress(hMiniblinkMod, "mbOnPromptBox");
    s_mbOnNetGetFavicon = (FN_mbOnNetGetFavicon)GetProcAddress(hMiniblinkMod, "mbOnNetGetFavicon");
    s_mbOnConsole = (FN_mbOnConsole)GetProcAddress(hMiniblinkMod, "mbOnConsole");
    s_mbOnClose = (FN_mbOnClose)GetProcAddress(hMiniblinkMod, "mbOnClose");
    s_mbOnDestroy = (FN_mbOnDestroy)GetProcAddress(hMiniblinkMod, "mbOnDestroy");
    s_mbOnPrinting = (FN_mbOnPrinting)GetProcAddress(hMiniblinkMod, "mbOnPrinting");
    s_mbOnPluginList = (FN_mbOnPluginList)GetProcAddress(hMiniblinkMod, "mbOnPluginList");
    s_mbOnImageBufferToDataURL = (FN_mbOnImageBufferToDataURL)GetProcAddress(hMiniblinkMod, "mbOnImageBufferToDataURL");
    s_mbOnDidCreateScriptContext = (FN_mbOnDidCreateScriptContext)GetProcAddress(hMiniblinkMod, "mbOnDidCreateScriptContext");
    s_mbGoBack = (FN_mbGoBack)GetProcAddress(hMiniblinkMod, "mbGoBack");
    s_mbGoForward = (FN_mbGoForward)GetProcAddress(hMiniblinkMod, "mbGoForward");
    s_mbNavigateAtIndex = (FN_mbNavigateAtIndex)GetProcAddress(hMiniblinkMod, "mbNavigateAtIndex");
    s_mbGetNavigateIndex = (FN_mbGetNavigateIndex)GetProcAddress(hMiniblinkMod, "mbGetNavigateIndex");
    s_mbStopLoading = (FN_mbStopLoading)GetProcAddress(hMiniblinkMod, "mbStopLoading");
    s_mbReload = (FN_mbReload)GetProcAddress(hMiniblinkMod, "mbReload");
    s_mbPerformCookieCommand = (FN_mbPerformCookieCommand)GetProcAddress(hMiniblinkMod, "mbPerformCookieCommand");
    s_mbEditorSelectAll = (FN_mbEditorSelectAll)GetProcAddress(hMiniblinkMod, "mbEditorSelectAll");
    s_mbEditorCopy = (FN_mbEditorCopy)GetProcAddress(hMiniblinkMod, "mbEditorCopy");
    s_mbEditorCut = (FN_mbEditorCut)GetProcAddress(hMiniblinkMod, "mbEditorCut");
    s_mbEditorPaste = (FN_mbEditorPaste)GetProcAddress(hMiniblinkMod, "mbEditorPaste");
    s_mbEditorDelete = (FN_mbEditorDelete)GetProcAddress(hMiniblinkMod, "mbEditorDelete");
    s_mbEditorUndo = (FN_mbEditorUndo)GetProcAddress(hMiniblinkMod, "mbEditorUndo");
    s_mbFireMouseEvent = (FN_mbFireMouseEvent)GetProcAddress(hMiniblinkMod, "mbFireMouseEvent");
    s_mbFireContextMenuEvent = (FN_mbFireContextMenuEvent)GetProcAddress(hMiniblinkMod, "mbFireContextMenuEvent");
    s_mbFireMouseWheelEvent = (FN_mbFireMouseWheelEvent)GetProcAddress(hMiniblinkMod, "mbFireMouseWheelEvent");
    s_mbFireKeyUpEvent = (FN_mbFireKeyUpEvent)GetProcAddress(hMiniblinkMod, "mbFireKeyUpEvent");
    s_mbFireKeyDownEvent = (FN_mbFireKeyDownEvent)GetProcAddress(hMiniblinkMod, "mbFireKeyDownEvent");
    s_mbFireKeyPressEvent = (FN_mbFireKeyPressEvent)GetProcAddress(hMiniblinkMod, "mbFireKeyPressEvent");
    s_mbFireWindowsMessage = (FN_mbFireWindowsMessage)GetProcAddress(hMiniblinkMod, "mbFireWindowsMessage");
    s_mbSetFocus = (FN_mbSetFocus)GetProcAddress(hMiniblinkMod, "mbSetFocus");
    s_mbKillFocus = (FN_mbKillFocus)GetProcAddress(hMiniblinkMod, "mbKillFocus");
    s_mbShowWindow = (FN_mbShowWindow)GetProcAddress(hMiniblinkMod, "mbShowWindow");
    s_mbLoadURL = (FN_mbLoadURL)GetProcAddress(hMiniblinkMod, "mbLoadURL");
    s_mbLoadHtmlWithBaseUrl = (FN_mbLoadHtmlWithBaseUrl)GetProcAddress(hMiniblinkMod, "mbLoadHtmlWithBaseUrl");
    s_mbPostURL = (FN_mbPostURL)GetProcAddress(hMiniblinkMod, "mbPostURL");
    s_mbGetLockedViewDC = (FN_mbGetLockedViewDC)GetProcAddress(hMiniblinkMod, "mbGetLockedViewDC");
    s_mbUnlockViewDC = (FN_mbUnlockViewDC)GetProcAddress(hMiniblinkMod, "mbUnlockViewDC");
    s_mbWake = (FN_mbWake)GetProcAddress(hMiniblinkMod, "mbWake");
    s_mbJsToDouble = (FN_mbJsToDouble)GetProcAddress(hMiniblinkMod, "mbJsToDouble");
    s_mbJsToBoolean = (FN_mbJsToBoolean)GetProcAddress(hMiniblinkMod, "mbJsToBoolean");
    s_mbJsToString = (FN_mbJsToString)GetProcAddress(hMiniblinkMod, "mbJsToString");
    s_mbGetJsValueType = (FN_mbGetJsValueType)GetProcAddress(hMiniblinkMod, "mbGetJsValueType");
    s_mbOnJsQuery = (FN_mbOnJsQuery)GetProcAddress(hMiniblinkMod, "mbOnJsQuery");
    s_mbResponseQuery = (FN_mbResponseQuery)GetProcAddress(hMiniblinkMod, "mbResponseQuery");
    s_mbRunJs = (FN_mbRunJs)GetProcAddress(hMiniblinkMod, "mbRunJs");
    s_mbRunJsSync = (FN_mbRunJsSync)GetProcAddress(hMiniblinkMod, "mbRunJsSync");
    s_mbWebFrameGetMainFrame = (FN_mbWebFrameGetMainFrame)GetProcAddress(hMiniblinkMod, "mbWebFrameGetMainFrame");
    s_mbIsMainFrame = (FN_mbIsMainFrame)GetProcAddress(hMiniblinkMod, "mbIsMainFrame");
    s_mbSetNodeJsEnable = (FN_mbSetNodeJsEnable)GetProcAddress(hMiniblinkMod, "mbSetNodeJsEnable");
    s_mbSetDeviceParameter = (FN_mbSetDeviceParameter)GetProcAddress(hMiniblinkMod, "mbSetDeviceParameter");
    s_mbGetContentAsMarkup = (FN_mbGetContentAsMarkup)GetProcAddress(hMiniblinkMod, "mbGetContentAsMarkup");
    s_mbGetSource = (FN_mbGetSource)GetProcAddress(hMiniblinkMod, "mbGetSource");
    s_mbUtilSerializeToMHTML = (FN_mbUtilSerializeToMHTML)GetProcAddress(hMiniblinkMod, "mbUtilSerializeToMHTML");
    s_mbUtilCreateRequestCode = (FN_mbUtilCreateRequestCode)GetProcAddress(hMiniblinkMod, "mbUtilCreateRequestCode");
    s_mbUtilIsRegistered = (FN_mbUtilIsRegistered)GetProcAddress(hMiniblinkMod, "mbUtilIsRegistered");
    s_mbUtilPrint = (FN_mbUtilPrint)GetProcAddress(hMiniblinkMod, "mbUtilPrint");
    s_mbUtilBase64Encode = (FN_mbUtilBase64Encode)GetProcAddress(hMiniblinkMod, "mbUtilBase64Encode");
    s_mbUtilBase64Decode = (FN_mbUtilBase64Decode)GetProcAddress(hMiniblinkMod, "mbUtilBase64Decode");
    s_mbUtilDecodeURLEscape = (FN_mbUtilDecodeURLEscape)GetProcAddress(hMiniblinkMod, "mbUtilDecodeURLEscape");
    s_mbUtilEncodeURLEscape = (FN_mbUtilEncodeURLEscape)GetProcAddress(hMiniblinkMod, "mbUtilEncodeURLEscape");
    s_mbUtilCreateV8Snapshot = (FN_mbUtilCreateV8Snapshot)GetProcAddress(hMiniblinkMod, "mbUtilCreateV8Snapshot");
    s_mbUtilPrintToPdf = (FN_mbUtilPrintToPdf)GetProcAddress(hMiniblinkMod, "mbUtilPrintToPdf");
    s_mbUtilPrintToBitmap = (FN_mbUtilPrintToBitmap)GetProcAddress(hMiniblinkMod, "mbUtilPrintToBitmap");
    s_mbUtilScreenshot = (FN_mbUtilScreenshot)GetProcAddress(hMiniblinkMod, "mbUtilScreenshot");
    s_mbUtilsSilentPrint = (FN_mbUtilsSilentPrint)GetProcAddress(hMiniblinkMod, "mbUtilsSilentPrint");
    s_mbUtilSetDefaultPrinterSettings = (FN_mbUtilSetDefaultPrinterSettings)GetProcAddress(hMiniblinkMod, "mbUtilSetDefaultPrinterSettings");
    s_mbPopupDownloadMgr = (FN_mbPopupDownloadMgr)GetProcAddress(hMiniblinkMod, "mbPopupDownloadMgr");
    s_mbPopupDialogAndDownload = (FN_mbPopupDialogAndDownload)GetProcAddress(hMiniblinkMod, "mbPopupDialogAndDownload");
    s_mbDownloadByPath = (FN_mbDownloadByPath)GetProcAddress(hMiniblinkMod, "mbDownloadByPath");
    s_mbGetPdfPageData = (FN_mbGetPdfPageData)GetProcAddress(hMiniblinkMod, "mbGetPdfPageData");
    s_mbCreateMemBuf = (FN_mbCreateMemBuf)GetProcAddress(hMiniblinkMod, "mbCreateMemBuf");
    s_mbFreeMemBuf = (FN_mbFreeMemBuf)GetProcAddress(hMiniblinkMod, "mbFreeMemBuf");
    s_mbPluginListBuilderAddPlugin = (FN_mbPluginListBuilderAddPlugin)GetProcAddress(hMiniblinkMod, "mbPluginListBuilderAddPlugin");
    s_mbPluginListBuilderAddMediaTypeToLastPlugin = (FN_mbPluginListBuilderAddMediaTypeToLastPlugin)GetProcAddress(hMiniblinkMod, "mbPluginListBuilderAddMediaTypeToLastPlugin");
    s_mbPluginListBuilderAddFileExtensionToLastMediaType = (FN_mbPluginListBuilderAddFileExtensionToLastMediaType)GetProcAddress(hMiniblinkMod, "mbPluginListBuilderAddFileExtensionToLastMediaType");
    s_mbEnableHighDPISupport = (FN_mbEnableHighDPISupport)GetProcAddress(hMiniblinkMod, "mbEnableHighDPISupport");
    s_mbRunMessageLoop = (FN_mbRunMessageLoop)GetProcAddress(hMiniblinkMod, "mbRunMessageLoop");
    s_mbGetUserKeyValue = (FN_mbGetUserKeyValue)GetProcAddress(hMiniblinkMod, "mbGetUserKeyValue");
    s_mbSetUserKeyValue = (FN_mbSetUserKeyValue)GetProcAddress(hMiniblinkMod, "mbSetUserKeyValue");
    s_mbOnWillReleaseScriptContext = (FN_mbOnWillReleaseScriptContext)GetProcAddress(hMiniblinkMod, "mbOnWillReleaseScriptContext");
    s_mbCallBlinkThreadAsync = (FN_mbCallBlinkThreadAsync)GetProcAddress(hMiniblinkMod, "mbCallBlinkThreadAsync");
    s_mbCallBlinkThreadSync = (FN_mbCallBlinkThreadSync)GetProcAddress(hMiniblinkMod, "mbCallBlinkThreadSync");
    s_mbCallUiThreadAsync = (FN_mbCallUiThreadAsync)GetProcAddress(hMiniblinkMod, "mbCallUiThreadAsync");
    s_mbCallUiThreadSync = (FN_mbCallUiThreadSync)GetProcAddress(hMiniblinkMod, "mbCallUiThreadSync");
    s_mbEditorRedo = (FN_mbEditorRedo)GetProcAddress(hMiniblinkMod, "mbEditorRedo");
    s_mbEditorUnSelect = (FN_mbEditorUnSelect)GetProcAddress(hMiniblinkMod, "mbEditorUnSelect");
    s_mbGetBlinkMainThreadIsolate = (FN_mbGetBlinkMainThreadIsolate)GetProcAddress(hMiniblinkMod, "mbGetBlinkMainThreadIsolate");
    s_mbGetContentHeight = (FN_mbGetContentHeight)GetProcAddress(hMiniblinkMod, "mbGetContentHeight");
    s_mbGetContentWidth = (FN_mbGetContentWidth)GetProcAddress(hMiniblinkMod, "mbGetContentWidth");
    s_mbGetGlobalExecByFrame = (FN_mbGetGlobalExecByFrame)GetProcAddress(hMiniblinkMod, "mbGetGlobalExecByFrame");
    s_mbGetProcAddr = (FN_mbGetProcAddr)GetProcAddress(hMiniblinkMod, "mbGetProcAddr");
    s_mbGetWebViewForCurrentContext = (FN_mbGetWebViewForCurrentContext)GetProcAddress(hMiniblinkMod, "mbGetWebViewForCurrentContext");
    s_mbGoToIndex = (FN_mbGoToIndex)GetProcAddress(hMiniblinkMod, "mbGoToIndex");
    s_mbGoToOffset = (FN_mbGoToOffset)GetProcAddress(hMiniblinkMod, "mbGoToOffset");
    s_mbInsertCSSByFrame = (FN_mbInsertCSSByFrame)GetProcAddress(hMiniblinkMod, "mbInsertCSSByFrame");
    s_mbJsToV8Value = (FN_mbJsToV8Value)GetProcAddress(hMiniblinkMod, "mbJsToV8Value");
    s_mbNetGetReferrer = (FN_mbNetGetReferrer)GetProcAddress(hMiniblinkMod, "mbNetGetReferrer");
    s_mbOnBlinkThreadInit = (FN_mbOnBlinkThreadInit)GetProcAddress(hMiniblinkMod, "mbOnBlinkThreadInit");
    s_mbOnDocumentReadyInBlinkThread = (FN_mbOnDocumentReadyInBlinkThread)GetProcAddress(hMiniblinkMod, "mbOnDocumentReadyInBlinkThread");
    s_mbOnLoadUrlFinish = (FN_mbOnLoadUrlFinish)GetProcAddress(hMiniblinkMod, "mbOnLoadUrlFinish");
    s_mbOnLoadUrlHeadersReceived = (FN_mbOnLoadUrlHeadersReceived)GetProcAddress(hMiniblinkMod, "mbOnLoadUrlHeadersReceived");
    s_mbOnNodeCreateProcess = (FN_mbOnNodeCreateProcess)GetProcAddress(hMiniblinkMod, "mbOnNodeCreateProcess");
    s_mbOnThreadIdle = (FN_mbOnThreadIdle)GetProcAddress(hMiniblinkMod, "mbOnThreadIdle");
    s_mbRegisterEmbedderCustomElement = (FN_mbRegisterEmbedderCustomElement)GetProcAddress(hMiniblinkMod, "mbRegisterEmbedderCustomElement");
    s_mbSetEditable = (FN_mbSetEditable)GetProcAddress(hMiniblinkMod, "mbSetEditable");
    s_mbWebFrameGetMainWorldScriptContext = (FN_mbWebFrameGetMainWorldScriptContext)GetProcAddress(hMiniblinkMod, "mbWebFrameGetMainWorldScriptContext");
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ulReasonForCall,
    LPVOID lpReserved
    )
{
    g_hModule = hModule;

    switch (ulReasonForCall) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}