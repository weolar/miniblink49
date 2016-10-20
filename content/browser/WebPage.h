
#ifndef content_WebPage_h
#define content_WebPage_h

#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/platform/geometry/IntPoint.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/wtf/FastAllocBase.h"
#include "third_party/WebKit/public/web/WebViewClient.h"

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

	blink::IntSize viewportSize() const;
    void setViewportSize(const blink::IntSize& size);

	blink::IntRect caretRect();

    void repaintRequested(const blink::IntRect& windowRect);

    void setIsDraggableRegionNcHitTest();

    void setNeedsCommit();
    bool needsCommit();

    HWND getHWND() const;
    
    void setBackgroundColor(COLORREF c);

    void showDebugNodeData();

    HDC viewDC();
    void paintToBit(void* bits, int pitch);
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser();
    void setBrowser(CefBrowserHostImpl* browserImpl);
#endif
	blink::WebViewImpl* webViewImpl();
	blink::WebFrame* mainFrame();

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
        ASSERT(!m_wkeWebView);
        m_wkeWebView = wkeWebView;
    }
    wke::CWebView* wkeWebView() const { return m_wkeWebView; }
    wke::CWebViewHandler& wkeHandler() { return *m_wkeHandler; }
#endif
protected:
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CWebView* m_wkeWebView;
    wke::CWebViewHandler* m_wkeHandler;
#endif
    WebPageImpl* m_pageImpl;
};

} // namespace content

#endif // content_WebPage_h
