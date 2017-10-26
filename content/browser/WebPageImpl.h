#ifndef WebPageImpl_h
#define WebPageImpl_h

#include "base/rand_util.h"

#include "third_party/WebKit/public/web/WebViewClient.h"
#include "third_party/WebKit/public/web/WebHistoryCommitType.h"
#include "third_party/WebKit/Source/platform/graphics/Color.h"
#include "third_party/WebKit/public/platform/WebCursorInfo.h"

#include "cc/trees/LayerTreeHost.h"
#include "cc/trees/LayerTreeHostClient.h"
#include "skia/ext/platform_canvas.h"

#include "content/browser/PopupMenuWinClient.h"

namespace cc {
class LayerTreeHost;
}

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
namespace cef {
class BrowserHostImpl;
}
#endif

namespace blink {
struct Referrer;
class WebViewImpl;
}

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
class CefBrowserHostImpl;
#endif

namespace content {

class WebFrameClientImpl;
class WebPage;
class PlatformEventHandler;
class NavigationController;
class PopupMenuWin;

class WebPageImpl 
    : public blink::WebViewClient
    , public cc::LayerTreeHostUiThreadClient
    , public cc::LayerTreeHostClent
    , public PopupMenuWinClient {
public:
    WebPageImpl();
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
    virtual blink::WebScreenInfo screenInfo() override;
    // Editing --------------------------------------------------------
    virtual bool handleCurrentKeyboardEvent() override;

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

    // PopupMenuWinClient --------------------------------------------------------
    virtual void onPopupMenuCreate(HWND hWnd) override;
    virtual void onPopupMenuHide() override;
    
    void testPaint();

    void beginMainFrame();
    
    void repaintRequested(const blink::IntRect& windowRect);

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

    HDC viewDC();
    void paintToBit(void* bits, int pitch);
    
    void setViewportSize(const blink::IntSize& size);

    blink::IntRect caretRect() const;
        
    void setPainting(bool value) { m_painting = value; }

    void showDebugNodeData();

    bool needsCommit() const { return m_needsCommit; }
    void setNeedsCommit();
    void setNeedsCommitAndNotLayout();
    void clearNeedsCommit();
    bool isDrawDirty();

    // LayerTreeHostUiThreadClient --------------------------------------------------------
    virtual void paintToMemoryCanvasInUiThread(SkCanvas* canvas, const blink::IntRect& paintRect) override;
    
    cc::LayerTreeHost* layerTreeHost() { return m_layerTreeHost; }

    void loadHistoryItem(int64 frameId, const blink::WebHistoryItem& item, blink::WebHistoryLoadType type, blink::WebURLRequest::CachePolicy policy);
    void loadURL(int64 frameId, const wchar_t* url, const blink::Referrer& referrer, const wchar_t* extraHeaders);
    void loadRequest(int64 frameId, const blink::WebURLRequest& request);
    void loadHTMLString(int64 frameId, const blink::WebData& html, const blink::WebURL& baseURL, const blink::WebURL& unreachableURL, bool replace);

    void setTransparent(bool transparent);

    // Session history -----------------------------------------------------
    void didCommitProvisionalLoad(blink::WebLocalFrame* frame, const blink::WebHistoryItem& history, blink::WebHistoryCommitType type);
    virtual void navigateBackForwardSoon(int offset) override;
    virtual int historyBackListCount() override;
    virtual int historyForwardListCount() override;

    static WebPageImpl* getSelfForCurrentContext();

    bool initSetting();
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser() const;
    void setBrowser(CefBrowserHostImpl* browser);
#endif
    blink::WebFrame* getWebFrameFromFrameId(int64 frameId);

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

    // ----
    void executeMainFrame();

    void copyToMemoryCanvasForUi();

    friend class AutoRecordActions;

    //bool m_useLayeredBuffer;

    blink::IntRect m_winodwRect;

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
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* m_browser;
#endif
    cc::LayerTreeHost* m_layerTreeHost;
    bool m_painting;
        
    enum WebPageState {
        pageUninited,
        pageInited,
        pageDestroying,
        pageDestroyed
    } m_state;

    bool m_LMouseDown;
    bool m_RMouseDown;
    void* m_foreignPtr;
    blink::IntPoint m_lastPosForDrag;
    WebFrameClientImpl* m_webFrameClient;
    PlatformEventHandler* m_platformEventHandler;

    blink::WebCursorInfo::Type m_cursorType;

    int m_enterCount;
    bool checkForRepeatEnter();
    void doClose();

    bool m_postCloseWidgetSoonMessage;

    WTF::Vector<DestroyNotif*> m_destroyNotifs;

    NavigationController* m_navigationController;

    HWND m_popupHandle;
    int m_debugCount;
    int m_needsCommit;
    int m_commitCount;
    int m_needsLayout;
    int m_layerDirty;
    double m_lastFrameTimeMonotonic;

    SkCanvas* m_memoryCanvasForUi;
    bool m_disablePaint;

    blink::Persistent<PopupMenuWin> m_popup;
};

} // blink

#endif // WebPageImpl_h