#ifndef WebPageImpl_h
#define WebPageImpl_h

#include "content/browser/WebPageState.h"
#include "content/ui/PopupMenuWinClient.h"

#include "base/rand_util.h"

#include "third_party/WebKit/public/web/WebViewClient.h"
#include "third_party/WebKit/public/web/WebHistoryCommitType.h"
#include "third_party/WebKit/Source/platform/graphics/Color.h"
#include "third_party/WebKit/public/platform/WebCursorInfo.h"

#include "cc/trees/LayerTreeHost.h"
#include "cc/trees/LayerTreeHostClient.h"
#include "skia/ext/platform_canvas.h"
#include "net/PageNetExtraData.h"

typedef struct HWND__ *HWND;

namespace cc {
class LayerTreeHost;
}

namespace blink {
struct Referrer;
class WebViewImpl;
}

namespace net {
class PageNetExtraData;
}

namespace wke {
class CWebView;
}

namespace content {

class WebFrameClientImpl;
class WebPage;
class PlatformEventHandler;
class NavigationController;
class PopupMenuWin;
class ToolTip;
class DevToolsClient;
class DevToolsAgent;
class DragHandle;

class WebPageImpl 
    : public blink::WebViewClient
    , public cc::LayerTreeHostUiThreadClient
    , public cc::LayerTreeHostClent
    , public PopupMenuWinClient {
public:
    WebPageImpl(COLORREF bdColor);
    ~WebPageImpl();

    class DestroyNotif {
    public:
        virtual void destroy() = 0;
    };
    void registerDestroyNotif(DestroyNotif* destroyNotif);
    void unregisterDestroyNotif(DestroyNotif* destroyNotif);

    static void initBlink();

    virtual blink::WebView* createView(blink::WebLocalFrame* creator,
        const blink::WebURLRequest& request,
        const blink::WebWindowFeatures& features,
        const blink::WebString& name,
        blink::WebNavigationPolicy policy,
        bool suppressOpener) override;

    void init(WebPage* pagePtr, HWND hWnd);
    void close();

    void gc();

    void didRunCreateDevToolsAgentTaskObserver();

    DevToolsAgent* createOrGetDevToolsAgent();
    DevToolsClient* createOrGetDevToolsClient();
    
    // WebViewClient
    virtual void didInvalidateRect(const blink::WebRect&) override;
    virtual void didAutoResize(const blink::WebSize& newSize) override;
    virtual void didUpdateLayout() override;
    virtual void didUpdateLayoutSize(const blink::WebSize& newSize) override;
    virtual void scheduleAnimation() override;
    virtual void initializeLayerTreeView() override;
    virtual blink::WebWidget* createPopupMenu(blink::WebPopupType) override;
    virtual blink::WebStorageNamespace* createSessionStorageNamespace() override;
    virtual blink::WebString acceptLanguages() override;
    void setScreenInfo(const blink::WebScreenInfo& info);
    virtual blink::WebScreenInfo screenInfo() override;
    virtual void setMouseOverURL(const blink::WebURL&) override;
    virtual void setToolTipText(const blink::WebString&, blink::WebTextDirection hint) override;
    virtual void draggableRegionsChanged() override;
    virtual void onMouseDown(const blink::WebNode& mouseDownNode) override;
    virtual void printPage(blink::WebLocalFrame* frame) override;

    // Editing --------------------------------------------------------
    virtual bool handleCurrentKeyboardEvent() override;

    // Called when a drag-n-drop operation should begin.
    virtual void startDragging(blink::WebLocalFrame* frame, const blink::WebDragData& data, 
        blink::WebDragOperationsMask mask, const blink::WebImage& image, const blink::WebPoint& dragImageOffset) override;

    // Return a compositing view used for this widget. This is owned by the
    // WebWidgetClient.
    virtual blink::WebLayerTreeView* layerTreeView() override;
    virtual void didChangeCursor(const blink::WebCursorInfo&) override;
    virtual void closeWidgetSoon() override;

    // LayerTreeHostClent --------------------------------------------------------
    virtual void onLayerTreeDirty() override;
    virtual void onLayerTreeInvalidateRect(const blink::IntRect& r) override;
    virtual void onLayerTreeSetNeedsCommit() override;
    virtual void disablePaint() override;
    virtual void enablePaint() override;

    void didStartProvisionalLoad();

    // PopupMenuWinClient --------------------------------------------------------
    virtual void onPopupMenuCreate(HWND hWnd) override;
    virtual void onPopupMenuHide() override;

    // Dialogs -------------------------------------------------------------
    virtual void showValidationMessage(const blink::WebRect& anchorInViewport, const blink::WebString& mainText, blink::WebTextDirection mainTextDir, const blink::WebString& supplementalText, blink::WebTextDirection supplementalTextDir) override;
    virtual void hideValidationMessage() override;
    virtual void moveValidationMessage(const blink::WebRect& anchorInViewport) override;
    
    void testPaint();

    void beginMainFrame();
    
    void repaintRequested(const blink::IntRect& windowRect, bool forceRepaintIfEmptyRect);

    void freeV8TempObejctOnOneFrameBefore();

    bool fireTimerEvent();
    void fireResizeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireCaptureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void firePaintEvent(HDC hdc, const RECT* paintRect);
    void fireSetFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireKillFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT fireMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* bHandle);
    void fireTouchEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireCursorEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* handle);
    LRESULT fireWheelEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyUpEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyDownEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyPressEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    int getCursorInfoType() const;
    void setCursorInfoType(int type);

    HDC viewDC();
    void paintToBit(void* bits, int pitch);
    
    void setViewportSize(const blink::IntSize& size);

    blink::IntRect caretRect() const;
        
    void setPainting(bool value) { m_painting = value; }

    void showDebugNodeData();

    //bool needsCommit() const { return m_needsCommit; }
    bool needsCommit() const { return m_commitCount > 0; }
    void setNeedsCommit();
    void setNeedsCommitAndNotLayout();
    //void clearNeedsCommit();
    bool isDrawDirty();

    // LayerTreeHostUiThreadClient --------------------------------------------------------
    virtual void paintToMemoryCanvasInUiThread(SkCanvas* canvas, const blink::IntRect& paintRect) override;
    
    cc::LayerTreeHost* layerTreeHost() { return m_layerTreeHost; }
    void setDrawMinInterval(double drawMinInterval);

    void loadHistoryItem(int64 frameId, const blink::WebHistoryItem& item, blink::WebHistoryLoadType type, blink::WebURLRequest::CachePolicy policy);
    void loadURL(int64 frameId, const wchar_t* url, const blink::Referrer& referrer, const wchar_t* extraHeaders);
    void loadRequest(int64 frameId, const blink::WebURLRequest& request);
    void loadHTMLString(int64 frameId, const blink::WebData& html, const blink::WebURL& baseURL, const blink::WebURL& unreachableURL, bool replace);

    void setTransparent(bool transparent);
    void setHWND(HWND hWnd);

    // Session history -----------------------------------------------------
    void didCommitProvisionalLoad(blink::WebLocalFrame* frame, const blink::WebHistoryItem& history, 
        blink::WebHistoryCommitType type, bool isSameDocument);
    virtual void navigateBackForwardSoon(int offset) override;
    virtual int historyBackListCount() override;
    virtual int historyForwardListCount() override;
    void navigateToIndex(int index);

    static WebPageImpl* getSelfForCurrentContext();

    bool initSetting();
    blink::WebFrame* getWebFrameFromFrameId(int64_t frameId);
    static int64_t getFrameIdByBlinkFrame(const blink::WebFrame* frame);
    static int64_t getFirstFrameId();

    blink::WebView* createWkeView(blink::WebLocalFrame* creator,
        const blink::WebURLRequest& request,
        const blink::WebWindowFeatures& features,
        const blink::WebString& name,
        blink::WebNavigationPolicy policy,
        bool suppressOpener);
    blink::WebView* createCefView(blink::WebLocalFrame* creator,
        const blink::WebURLRequest& request,
        const blink::WebWindowFeatures& features,
        const blink::WebString& name,
        blink::WebNavigationPolicy policy,
        bool suppressOpener);

    virtual bool runFileChooser(const blink::WebFileChooserParams& params, blink::WebFileChooserCompletion* completion) override;

    // ----
    bool needDrawToScreen(HWND hWnd) const;
    void drawLayeredWindow(HWND hWnd, SkCanvas* canvas, HDC hdc, const blink::IntRect& paintRect, HDC hMemoryDC) const;

    void executeMainFrame();
    friend class AutoRecordActions;
    int m_autoRecordActionsCount;
    bool m_runningInMouseMessage;

    void copyToMemoryCanvasForUi();

    void handleMouseWhenDraging(UINT message);
    void onEnterDragSimulate();
    void onLeaveDragSimulate();
    void onDraggingSimulate();
    bool m_isDragging;
    bool m_isFirstEnterDrag;

    void setBackgroundColor(COLORREF c);

    void setHwndRenderOffset(const blink::IntPoint& offset);
    blink::IntPoint getHwndRenderOffset() const;

    static int64_t m_firstFrameId;

    blink::WebThread::TaskObserver* m_createDevToolsAgentTaskObserver;

    ToolTip* m_toolTip;
    ToolTip* m_validationMessageTip;

    blink::IntRect m_winodwRect;

    bool n_needAutoDrawToHwnd;

    bool m_postMouseLeave; // 系统的MouseLeave获取到的鼠标位置不太准确，自己在定时器里再抛一次
    blink::RGBA32 m_bdColor;

    WebPage* m_pagePtr;
    blink::WebViewImpl* m_webViewImpl;
    bool m_mouseInWindow;
    HWND m_hWnd;
    blink::IntPoint m_hwndRenderOffset;    // 网页渲染坐标相对于窗口的原点
    blink::IntSize m_viewportSize;

    // May be NULL if the browser has not yet been created or if the browser has
    // been destroyed.

#if ENABLE_WKE == 1
    wke::CWebView* wkeWebView() const;
#endif
    cc::LayerTreeHost* m_layerTreeHost;
    bool m_painting;
        
    WebPageState m_state;

    bool m_LMouseDown;
    bool m_RMouseDown;
    void* m_foreignPtr;
    blink::IntPoint m_lastPosForDrag;
    WebFrameClientImpl* m_webFrameClient;
    PlatformEventHandler* m_platformEventHandler;

    blink::WebCursorInfo m_cursor;
    HICON m_platformCursor;

    int m_enterCount;
    bool checkForRepeatEnter();
    void doClose();

    bool m_postCloseWidgetSoonMessage;

    WTF::Vector<DestroyNotif*> m_destroyNotifs;
    HRGN m_draggableRegion;

    HWND m_popupHandle;
    int m_debugCount;
    //int m_needsCommit;
    int m_commitCount;
    int m_needsLayout;
    int m_layerDirty;
    int m_executeMainFrameCount;
    double m_lastFrameTimeMonotonic;

    SkCanvas* m_memoryCanvasForUi;
    bool m_disablePaint;
    int m_firstDrawCount;

    blink::Persistent<NavigationController> m_navigationController;
    blink::Persistent<PopupMenuWin> m_popup;

    bool isDevToolsClient() const { return !!m_devToolsClient; }
    DevToolsClient* m_devToolsClient;
    DevToolsAgent* m_devToolsAgent;
    void willEnterDebugLoop();
    void didExitDebugLoop();
    bool m_isEnterDebugLoop;

    DragHandle* m_dragHandle;

    blink::WebScreenInfo* m_screenInfo;
};

} // blink

#endif // WebPageImpl_h