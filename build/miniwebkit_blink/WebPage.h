
#ifndef WebPage_h
#define WebPage_h

//#include <Client/KApp.h>

//#include "KNetworkAccessManager.h"
// #include "KChromeClient.h"
// #include "KFrameLoaderClient.h"
// #include "page.h"
// #include "ResourceHandleClient.h"
#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/platform/geometry/IntPoint.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/wtf/FastAllocBase.h"
#include "third_party/WebKit/public/web/WebViewClient.h"
#include "KdGuiApi.h"
#include "KdGuiApiImp.h"
#include "KdPageInfo.h"

//#include "bridge/npruntime.h"
// 
// class KdWidgetMgr;
// 
// struct SQDbgServer;
// typedef SQDbgServer* HSQREMOTEDBG;
// 
// class KQuery;
// class KdValArray;

namespace blink {

class KFrameLoaderClient;
class KChromeClient;
class KContextMenuClient;
class KEditorClient;
class Page;
class Frame;
class LocalFrame;
class KFrameNetworkingContext;
class WebPagePaint;
class WebLocalFrame;
class WebViewImpl;
class FrameLoaderClientImpl;
class WebFrameClient;
class ChromeClient;
class ContextMenuClient;
class EditorClient;
class DragClient;
struct AsynchronousResLoadInfo;

struct KWebApiCallbackSet {
    PFN_KdPageCallback m_xmlHaveFinished;
    PFN_KdPageWinMsgCallback m_msgPreCallBack;
    PFN_KdPageWinMsgCallback m_msgPostCallBack;
    PFN_KdPageCallback m_unintCallBack;
    PFN_KdPageScriptInitCallback m_scriptInitCallBack;
    PFN_KdResCallback m_resHandle;
    PFN_KdResCallback m_resOtherNameQuery;
    PFN_KdPagePaintCallback m_paint;
    //NPInvokeFunctionPtr m_javascriptCallCppPtr;
};

class WebPage : public WebViewClient {
    WTF_MAKE_FAST_ALLOCATED(WebPage);
public:

    enum RenderLayer {
        ContentsLayer = 0x10,
        ScrollBarLayer = 0x20,
        PanIconLayer = 0x40,
        AllLayers = 0xff
    };

    static void initBlink();

    WebPage(KdGuiObjPtr kdGuiObj, void* foreignPtr);
    ~WebPage();

    virtual WebView* createView(WebLocalFrame* creator,
        const WebURLRequest& request,
        const WebWindowFeatures& features,
        const WebString& name,
        WebNavigationPolicy policy,
        bool suppressOpener);

    bool init(HWND hWnd);
    bool initSetting();

    void windowCloseRequested();

    void javaScriptAlert(String& message);

    void loadFormUrl(LPCWSTR lpUrl);
    void loadFormData(const void* lpData, int nLen);

    // WebViewClient
    virtual void didInvalidateRect(const WebRect&) OVERRIDE;
    virtual void didAutoResize(const WebSize& newSize) OVERRIDE;
    virtual void didUpdateLayoutSize(const WebSize& newSize) OVERRIDE;
    virtual void scheduleAnimation() OVERRIDE;
    virtual void initializeLayerTreeView() OVERRIDE;

    // Return a compositing view used for this widget. This is owned by the
    // WebWidgetClient.
    virtual WebLayerTreeView* layerTreeView() OVERRIDE;

    void paintEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void mouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void captureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void killFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    int  inputEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool inputEventToRichEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void timerFired();

    void resizeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    IntSize viewportSize() const { return m_viewportSize; }
    void setViewportSize(const IntSize& size);

    void repaintRequested(const IntRect& windowRect);

    void setIsDraggableRegionNcHitTest();

    HWND getHWND() { return m_hWnd; }

    LocalFrame* localFrame();
    Page* page() { return m_page; }

    KdGuiObjPtr getKdGuiObjPtr() { return m_kdGuiObj; }
    void* getForeignPtr() { return m_foreignPtr; }
    
    void setBackgroundColor(COLORREF c);

    void showDebugNodeData();

    void postResToAsynchronousLoad(const WCHAR* pUrl, const void* pResBuf, int nResBufLen, bool bNeedSavaRes);

    KWebApiCallbackSet m_callbacks;

    bool m_bMouseTrack;

    KdPageDebugInfo m_debugInfo;

    BOOL* m_messageStackVar; // 给消息处理函数使用，保存地址。例如，WM_TIMER中调用DestroyWindow，
    // 窗口摧毁了，消息函数还在进入WM_TIMER的处理函数，导致野指针崩溃.

protected:
    virtual void startDragging(WebLocalFrame*, const WebDragData&, WebDragOperationsMask, const WebImage&, const WebPoint& dragImageOffset) OVERRIDE;
    void setCanScheduleResourceLoader();
    bool m_canScheduleResourceLoader;

    void scheduleResourceLoader(KFrameNetworkingContext* pContext);

    FrameLoaderClientImpl* m_frameLoaderClient;
    RefPtr<LocalFrame> m_localFrame;

    ChromeClient* m_chromeClient;
    ContextMenuClient* m_contextMenuClient;
    EditorClient* m_editorClient;
    DragClient* m_dragClient;
    Page* m_page;

    enum KWebPageState {
        UNINIT,
        INIT,
        DESTROYING
    } m_state;

    IntSize m_viewportSize;
    HWND m_hWnd;

    bool m_LMouseDown;
    bool m_RMouseDown;

    WebPagePaint* m_pagePaint;

    KdGuiObjPtr m_kdGuiObj;
    void* m_foreignPtr;

    bool m_isAlert;
    bool m_isDraggableRegionNcHitTest;
    IntPoint m_lastPosForDrag;
    double m_lastMouseDownTime;

    WebFrameClient* m_webFrameClient;
};

} // namespace WebCore

#endif // WebPage_h