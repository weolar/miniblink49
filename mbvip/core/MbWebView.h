#ifndef MB_WEBVIEW_H
#define MB_WEBVIEW_H

#include "wke/wkedefine.h"
#include "mb.h"
#include "common/CallbackClosure.h"
#include <map>
#include <list>
#include <vector>
#include <string>
#include <functional>
#if ENABLE_NODEJS
#include "v8.h"
#endif

namespace node {
class Environment;
}

namespace printing {
class Printing;
}

namespace mb {

class MbWebView {
public:
    MbWebView();
    ~MbWebView();

    void createWkeWebWindowOrViewInBlinkThread(bool isWebWindowMode);
    void createWkeWebWindowInUiThread(mbWindowType type, HWND parent, int x, int y, int width, int height);
    void createWkeWebWindowImplInUiThread(HWND parent, DWORD style, DWORD styleEx, int x, int y, int width, int height);

    int64_t getId() const { return m_id; }
    mbWebView getWebviewHandle() const { return (mbWebView)m_id; }

    wkeWebView getWkeWebView() const { return m_wkeWebview; }
    void setWkeWebView(wkeWebView webview) { m_wkeWebview = webview; }
    wkeWebView getWkeWebViewOrCreateWhenOnCreateView();

    void setHostWnd(HWND hWnd);
    HWND getHostWnd() const { return m_hWnd; }

    void setAutoDrawToHwnd(bool b) { m_isAutoDrawToHwnd = b; }
    void setEnableNode(bool b) { m_isEnableNode = b; }

    void setZoomFactor(float factor) { m_zoomFactor = factor; }
    float getZoomFactor() const { return m_zoomFactor; }

	//std::map<std::string, void*>& getUuserKeyValues() { return m_userKeyValues; }
    void setUserKeyValue(const char* key, void* value);
    void* getUserKeyValue(const char* key) const;

	void setTitle(const std::string& title) { m_title = title; }
	const std::string& getTitle() const { return m_title; }

	void setUrl(const std::string& url) { m_url = url; }
	const std::string& getUrl() const { return m_url; }

    mbRect getCaretRect();

    void setPacketPathName(const WCHAR* pathName);
    std::wstring getPacketPathName() const { return m_packetPathName; }

    bool handleResPacket(const char* url, void* job);

    void onResize(int w, int h, bool needSetHostWnd);

    void onBlinkThreadPaint();
    void onPaint(HWND hWnd);
    void onPaintUpdatedInCompositeThread(const HDC hdc, int x, int y, int cx, int cy);
    void onPaintUpdatedInUiThread(int x, int y, int cx, int cy);
    void onPrePaintUpdatedInCompositeThread(const HDC hdc, int x, int y, int cx, int cy);

    void setPaintUpdatedCallback(mbPaintUpdatedCallback paintUpdatedCallback, void* param);
    void setPaintBitUpdatedCallback(mbPaintBitUpdatedCallback paintUpdatedCallback, void* param);

    HDC getViewDC();
    void unlockViewDC();

    void onMouseMessage(unsigned int message, int x, int y, unsigned int flags);
    void onCursorChange();
    bool setCursorInfoTypeByCache();

    CallbackClosure& getClosure() { return m_closure; }

    void decrementCreateWebViewRequestCount();
    int getCreateWebViewRequestCount() const { return m_createWebViewRequestCount; }

    void setShow(bool b);
    bool getIsWebWindowMode() const { return m_isWebWindowMode; }

    enum PageState {
        kPageInited,
        kPageDestroying,
        kPageDestroyed
    };
    PageState getState() const { return m_state; }
    void preDestroy();

    void setOffset(int x, int y)
    {
        m_offset.x = x;
        m_offset.y = y;
    }

    POINT getOffset() const { return m_offset; }

    int getCursorInfoType() const
    {
        return m_cursorInfoType;
    }

    void onDraggableRegionsChangedImpl(wkeDraggableRegion* newRegions, int rectCount);

//     mbCloseCallback m_closeCallback;
//     void* m_closeCallbackParam;

    mbDestroyCallback m_destroyCallback;
    void* m_destroyCallbackParam;

    mbPrintingCallback m_printingCallback;
    void* m_printingCallbackParam;

    void setIsMouseKeyMessageEnable(bool b) { m_enableMouseKeyMessage = b; }
    void setIsTransparent(bool b) { m_isTransparent = b; }
    void setBackgroundColor(COLORREF c) { m_backgroundColor = c; }

    void setNavigateIndex(int index)
    {
        InterlockedExchange((LONG volatile*)(&m_navigateIndex), index);
    }

    int getNavigateIndex() const
    {
        int index = 0;
        InterlockedExchange((LONG volatile*)(&index), m_navigateIndex);
        return index;
    }

    printing::Printing* m_printing;

    wkeWebFrameHandle getMainFrameId() const { return m_mainFrameId; }
    void setMainFrameId(wkeWebFrameHandle id) { m_mainFrameId = id; }

    static mbWebFrameHandle toMbFrameHandle(wkeWebView wkeWebview, wkeWebFrameHandle frameId);

private:
    void fillBackgroundColor(HDC hdc, int w, int h);
    void copyBitmapWhenResize(int w, int h, const SIZE& clientSize);
    void setBlinkSize();
    void initWebviewInBlinkThread(wkeWebView wkeWebview);
    void delayDoMouseMsgInBlinkThread();
#if ENABLE_NODEJS
    void startupNodejsEnv(v8::Local<v8::Context>& v8context);
#endif
    bool isDraggableRegionNcHitTest(HWND hWnd);
    bool doDraggableRegionNcHitTest(HWND hWnd);

    static void WKE_CALL_TYPE onDidCreateScriptContext(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int extensionGroup, int worldId);
    static void WKE_CALL_TYPE onWillReleaseScriptContext(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int worldId);
    static wkeWebView WKE_CALL_TYPE onCreateView(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures);
    static void WKE_CALL_TYPE onPaintUpdated(wkeWebView wkeWebview, void* param, const HDC hdc, int x, int y, int cx, int cy);
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT windowProcImpl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    void setClientSizeLocked(int w, int h);
    SIZE getClientSizeLocked();
    static void WKE_CALL_TYPE onCaretChangedCallback(wkeWebView webView, void* param, const wkeRect* r);

    int64_t m_id;
    PageState m_state;
    wkeWebView m_wkeWebview;
    bool m_isTransparent;
    bool m_isShow;
    HWND m_hWnd;
    bool m_isWebWindowMode;
    bool m_isEnableNode;
    bool m_isAutoDrawToHwnd;
    int m_cursorInfoType;
    bool m_isCursorInfoTypeAsynGetting;
    bool m_isCursorInfoTypeAsynChanged;
    CRITICAL_SECTION m_memoryCanvasLock;
    HBITMAP m_memoryBMP;
    COLORREF* m_bits;
    HDC m_memoryDC;
    COLORREF m_backgroundColor;
    mbRect m_caretPos;

    CRITICAL_SECTION m_clientSizeLock;
    SIZE m_clientSize;
    bool m_clientSizeDirty;

    bool m_isLayerWindow;
    POINT m_offset;
    HRGN m_draggableRegion;
    bool m_enableMouseKeyMessage;
    float m_zoomFactor;

    int m_navigateIndex;

	std::string m_title;
	std::string m_url;

	std::map<std::string, void*> m_userKeyValues;
    mutable CRITICAL_SECTION m_userKeyValuesLock;

    int m_createWebViewRequestCount;
    CallbackClosure m_closure;

    CRITICAL_SECTION m_mouseMsgQueueLock;
    struct MouseMsg {
        MouseMsg(const MouseMsg& other)
        {
            init(message, x, y, flags);
        }

        MouseMsg(unsigned int message, int x, int y, unsigned int flags)
        {
            init(message, x, y, flags);
        }

        void init(unsigned int message, int x, int y, unsigned int flags)
        {
            this->message = message;
            this->x = x;
            this->y = y;
            this->flags = flags;
        }
        unsigned int message;
        int x;
        int y;
        unsigned int flags;
    };
    std::list<MouseMsg*> m_mouseMsgQueue;

    std::vector<mbRect*> m_dirtyRect;
    CRITICAL_SECTION m_dirtyRectLock;

    bool m_hasSetPaintUpdatedCallback;

    node::Environment* m_env;

    wkeWebFrameHandle m_mainFrameId;

    std::wstring m_packetPathName;
};

bool checkThreadCallIsValid(const char* funcName);

}

#endif // MB_WEBVIEW_H