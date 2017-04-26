
#ifndef content_WebPage_h
#define content_WebPage_h

#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/platform/geometry/IntPoint.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/wtf/FastAllocBase.h"
#include "third_party/WebKit/public/web/WebViewClient.h"
#include "third_party/WebKit/public/web/WebHistoryCommitType.h"
#include "third_party/WebKit/Source/wtf/HashSet.h"

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
class CefBrowserHostImpl;
#endif

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
namespace wke {
class CWebView;
struct CWebViewHandler;
}
#endif

namespace blink {
class Page;
class Frame;
class LocalFrame;
class WebLocalFrame;
class WebViewImpl;
class FrameLoaderClientImpl;
class WebFrameClient;
class ChromeClient;
class ContextMenuClient;
class EditorClient;
class DragClient;
struct Referrer;
}

namespace content {

class WebPageImpl;
struct AsynchronousResLoadInfo;
class WebFrameClientImpl;

class WebPage {
    WTF_MAKE_FAST_ALLOCATED(WebPage);
public:
    enum RenderLayer {
        ContentsLayer = 0x10,
        ScrollBarLayer = 0x20,
        PanIconLayer = 0x40,
        AllLayers = 0xff
    };

    static void initBlink();
    static void shutdown();

    WebPage(void* foreignPtr);
    ~WebPage();

    bool init(HWND hWnd);

    void close();

    void loadURL(int64 frameId, const wchar_t* url, const blink::Referrer& referrer, const wchar_t* extraHeaders);
    void loadRequest(int64 frameId, const blink::WebURLRequest& request);
    void loadHTMLString(int64 frameId, const blink::WebData& html, const blink::WebURL& baseURL, const blink::WebURL& unreachableURL = blink::WebURL(), bool replace = false);

    // system message
    void firePaintEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT fireMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* bHandle);
    void fireCaptureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireSetFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireKillFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT fireCursorEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* bHandle);
    LRESULT fireWheelEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyUpEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyDownEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireKeyPressEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireTimerEvent();

    LRESULT fireInputEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool fireInputEventToRichEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void fireResizeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    int getCursorInfoType() const;

	blink::IntSize viewportSize() const;
    void setViewportSize(const blink::IntSize& size);

	blink::IntRect caretRect();

    void repaintRequested(const blink::IntRect& windowRect);

    void setIsDraggableRegionNcHitTest();

    void setNeedsCommit();
    bool needsCommit() const;
    bool isDrawDirty() const;

    HWND getHWND() const;
	void setHWND(HWND hwnd);
	void setHwndRenderOffset(const blink::IntPoint& offset);
    blink::IntPoint getHwndRenderOffset() const;
    void setBackgroundColor(COLORREF c);

    bool canGoBack();
    void goBack();
    bool canGoForward();
    void goForward();
    void didCommitProvisionalLoad(blink::WebLocalFrame* frame, const blink::WebHistoryItem& history, blink::WebHistoryCommitType type);

    HDC viewDC();
    void paintToBit(void* bits, int pitch);
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser();
    void setBrowser(CefBrowserHostImpl* browserImpl);
#endif
	blink::WebViewImpl* webViewImpl();
	blink::WebFrame* mainFrame();

    static WebPage* getSelfForCurrentContext();

    WebFrameClientImpl* webFrameClientImpl();

	blink::WebFrame* getWebFrameFromFrameId(int64 frameId);

    // kMainFrameId must be -1 to align with renderer expectations.
    static const int64 kMainFrameId = -1;
    static const int64 kFocusedFrameId = -2;
    static const int64 kUnspecifiedFrameId = -3;
    static const int64 kInvalidFrameId = -4;

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    void initWkeWebView(wke::CWebView* wkeWebView) 
    {
        ASSERT(!m_wkeWebView);/**/
        m_wkeWebView = wkeWebView;
    }
    wke::CWebView* wkeWebView() const { return m_wkeWebView; }
    wke::CWebViewHandler& wkeHandler() { return *m_wkeHandler; }
    void* wkeClientHandler() const { return m_wkeClientHandler; }
    void wkeSetClientHandler(void* clientHandler) { m_wkeClientHandler = clientHandler; }
#endif

protected:
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CWebView* m_wkeWebView;
    wke::CWebViewHandler* m_wkeHandler;
    void* m_wkeClientHandler;
#endif
    WebPageImpl* m_pageImpl;
    static WTF::HashSet<WebPage*>* m_webPageSet;
};

} // namespace content

#endif // content_WebPage_h
