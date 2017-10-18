#ifndef PlatformEventHandler_h
#define PlatformEventHandler_h

#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"

namespace blink {
class WebViewImpl;
class WebWidget;
}

namespace content {

class PlatformEventHandler {
public:
    PlatformEventHandler(blink::WebWidget* webWidget, blink::WebViewImpl* webViewImpl);
    static blink::WebKeyboardEvent buildKeyboardEvent(blink::WebInputEvent::Type type, UINT message, WPARAM wParam, LPARAM lParam);

    LRESULT fireMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool needSetFocus, BOOL* bHandle);
    LRESULT fireWheelEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireCaptureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireTouchEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
    bool m_isDraggableRegionNcHitTest;
    bool m_bMouseTrack;
    bool m_postMouseLeave;
    bool m_mouseInWindow;
    bool m_isAlert;
    double m_lastTimeMouseDown;
    int m_clickCount;
    blink::IntRect m_lastPosForDrag;
    blink::WebViewImpl* m_webViewImpl;
    blink::WebWidget* m_webWidget;
};



} // content

#endif // PlatformEventUtil_h