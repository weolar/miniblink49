#ifndef WebPageImpl_h
#define WebPageImpl_h

#include "base/rand_util.h"

#include "third_party/WebKit/public/web/WebViewClient.h"
#include "third_party/WebKit/Source/platform/graphics/Color.h"
#include "third_party/WebKit/public/platform/WebCursorInfo.h"

#include "cc/trees/LayerTreeHost.h"
#include "skia/ext/platform_canvas.h"

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


class WebPageImpl : public blink::WebViewClient {
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
        bool suppressOpener) OVERRIDE;

    void init(WebPage* pagePtr, HWND hWnd);
    void close();
    
    // WebViewClient
    virtual void didInvalidateRect(const blink::WebRect&) OVERRIDE;
    virtual void didAutoResize(const blink::WebSize& newSize) OVERRIDE;
    virtual void didUpdateLayoutSize(const blink::WebSize& newSize) OVERRIDE;
    virtual void scheduleAnimation() OVERRIDE;
    virtual void initializeLayerTreeView() OVERRIDE;
    virtual blink::WebWidget* createPopupMenu(blink::WebPopupType) OVERRIDE;
    virtual blink::WebStorageNamespace* createSessionStorageNamespace() OVERRIDE;
    virtual blink::WebString acceptLanguages() OVERRIDE;
    virtual blink::WebScreenInfo screenInfo() OVERRIDE;
    // Editing --------------------------------------------------------
    virtual bool handleCurrentKeyboardEvent() OVERRIDE;

    // Return a compositing view used for this widget. This is owned by the
    // WebWidgetClient.
    virtual blink::WebLayerTreeView* layerTreeView() OVERRIDE;
    virtual void didChangeCursor(const blink::WebCursorInfo&) OVERRIDE;
    virtual void closeWidgetSoon() override;

    void clearPaintWhenLayeredWindow(skia::PlatformCanvas* canvas, const blink::IntRect& rect);

    // Merge any areas that would reduce the total area
    void mergeDirtyList();

    bool doMergeDirtyList(bool forceMerge);

    void postPaintMessage(const blink::IntRect* paintRect);

    void testPaint();

    void beginMainFrame();
    
    void paintToPlatformContext(HDC hdc, const blink::IntRect* paintRect);
    bool drawFrame();
    
    void repaintRequested(const blink::IntRect& windowRect);

    void freeV8TempObejctOnOneFrameBefore();

    bool fireTimerEvent();
    void fireResizeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireCaptureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void firePaintEvent(HDC hdc, const RECT* paintRect);
    void fireKillFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT fireMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* bHandle);
    void fireTouchEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireCursorEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* handle);
    LRESULT fireWheelEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyUpEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyDownEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyPressEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HDC viewDC();
    void paintToBit(void* bits, int pitch);
    
    void setViewportSize(const blink::IntSize& size);

	blink::IntRect caretRect() const;

    int m_debugCount;

    void drawDebugLine(skia::PlatformCanvas* memoryCanvas, const blink::IntRect& paintRect);
   
    void drawToCanvas(const blink::IntRect& dirtyRect, skia::PlatformCanvas* canvas, bool needsFullTreeSync);
        
    void setPainting(bool value) { m_painting = value; }

    void showDebugNodeData();

    bool needsCommit() { return m_needsCommit; }
    void setNeedsCommit();
    void setNeedsCommitAndNotLayout();
    void clearNeedsCommit();
    
    cc::LayerTreeHost* layerTreeHost() { return m_layerTreeHost; }

	void loadURL(int64 frameId, const wchar_t* url, const blink::Referrer& referrer, const wchar_t* extraHeaders);
	void loadRequest(int64 frameId, const blink::WebURLRequest& request);
	void loadHTMLString(int64 frameId, const blink::WebData& html, const blink::WebURL& baseURL, const blink::WebURL& unreachableURL, bool replace);

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

    bool m_useLayeredBuffer;

	blink::IntRect m_winodwRect;
	blink::IntRect m_clientRect; // ����WebPage�޸�
    bool m_hasResize;
    bool m_postMouseLeave; // ϵͳ��MouseLeave��ȡ�������λ�ò�̫׼ȷ���Լ��ڶ�ʱ��������һ��
	blink::RGBA32 m_bdColor;
    double m_lastFrameTimeMonotonic;
    WebPage* m_pagePtr;
    blink::WebViewImpl* m_webViewImpl;
    bool m_mouseInWindow;
    HWND m_hWnd;
	blink::IntSize m_viewportSize;

    // May be NULL if the browser has not yet been created or if the browser has
    // been destroyed.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* m_browser;
#endif
    cc::LayerTreeHost* m_layerTreeHost;
	blink::IntRect m_paintRect;
    skia::PlatformCanvas* m_memoryCanvas;
    bool m_painting;
    bool m_canScheduleResourceLoader;
    Vector<blink::IntRect> m_paintMessageQueue;
    static const int m_paintMessageQueueSize = 200;
	blink::IntRect m_dirtyRects;
    int m_postpaintMessageCount;
    int m_scheduleMessageCount;
    bool m_needsCommit;
    bool m_needsLayout;
    
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

    BOOL* m_messageStackVar; // ����Ϣ������ʹ�ã������ַ�����磬WM_TIMER�е���DestroyWindow��
    // ���ڴݻ��ˣ���Ϣ�������ڽ���WM_TIMER�Ĵ�����������Ұָ�����.

	blink::WebCursorInfo::Type m_cursorType;

    int m_enterCount;
    bool checkForRepeatEnter();
    void doClose();

    bool m_postCloseWidgetSoonMessage;

    WTF::Vector<DestroyNotif*> m_destroyNotifs;
};

} // blink

#endif // WebPageImpl_h