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

    struct MouseEvtInfo {
        bool isNeedSetFocus;
        bool isWillDestroy;
        HRGN draggableRegion;
    };
    LRESULT fireMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, const MouseEvtInfo& info, BOOL* bHandle);
    LRESULT fireWheelEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireCaptureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void fireTouchEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    bool doDraggableRegionNcHitTest(HWND hWnd, const blink::IntPoint& pos, HRGN draggableRegion);
    bool m_isDraggableRegionNcHitTest;
    bool m_bMouseTrack;
    bool m_postMouseLeave;
    bool m_mouseInWindow;
    bool m_isAlert;
    double m_lastTimeMouseDown;
    blink::IntPoint m_lastPosMouseDown;
    blink::IntPoint m_lastPosMouseMove;
    blink::IntRect m_lastPosForDrag;
    blink::WebViewImpl* m_webViewImpl;
    blink::WebWidget* m_webWidget;
};



} // content

#endif // PlatformEventUtil_h