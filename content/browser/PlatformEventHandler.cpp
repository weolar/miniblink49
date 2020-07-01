#ifndef content_browser_PlatformEventHandler_h
#define content_browser_PlatformEventHandler_h

#include "content/browser/PlatformEventHandler.h"

#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"

#include "wtf/text/WTFString.h"
#include "wke/wkeGlobalVar.h"

using namespace blink;

namespace content {

static const unsigned short HIGH_BIT_MASK_SHORT = 0x8000;

// Source\WebCore\platform\win\KeyEventWin.cpp
static bool isKeypadEvent(WPARAM code, LPARAM keyData, WebInputEvent::Type type)
{
    if (type != WebInputEvent::RawKeyDown && type != WebInputEvent::KeyUp)
        return false;

    switch (code) {
    case VK_NUMLOCK:
    case VK_NUMPAD0:
    case VK_NUMPAD1:
    case VK_NUMPAD2:
    case VK_NUMPAD3:
    case VK_NUMPAD4:
    case VK_NUMPAD5:
    case VK_NUMPAD6:
    case VK_NUMPAD7:
    case VK_NUMPAD8:
    case VK_NUMPAD9:
    case VK_MULTIPLY:
    case VK_ADD:
    case VK_SEPARATOR:
    case VK_SUBTRACT:
    case VK_DECIMAL:
    case VK_DIVIDE:
        return true;
    case VK_RETURN:
        return HIWORD(keyData) & KF_EXTENDED;
    case VK_INSERT:
    case VK_DELETE:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_END:
    case VK_HOME:
    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:
        return !(HIWORD(keyData) & KF_EXTENDED);
    default:
        return false;
    }
}

static inline String singleCharacterString(UChar c) { return String(&c, 1); }

bool isShiftPressed()
{
    return (::GetKeyState(VK_SHIFT) & 0x8000) == 0x8000;
}

bool isCtrlPressed()
{
    return (::GetKeyState(VK_CONTROL) & 0x8000) == 0x8000;
}

bool isAltPressed()
{
    return (::GetKeyState(VK_MENU) & 0x8000) == 0x8000;
}

bool isAltGrPressed()
{
    return (::GetKeyState(VK_MENU) & 0x8000) == 0x8000 && (::GetKeyState(VK_CONTROL) & 0x8000) == 0x8000;
}

bool isWindowsKeyPressed()
{
    return (::GetKeyState(VK_LWIN) & 0x8000) == 0x8000 || (::GetKeyState(VK_RWIN) & 0x8000) == 0x8000;
}

bool isCapsLockOn()
{
    return (::GetKeyState(VK_CAPITAL) & 0x0001) == 0x0001;
}

bool isNumLockOn()
{
    return (::GetKeyState(VK_NUMLOCK) & 0x0001) == 0x0001;
}

bool isScrollLockOn()
{
    return (::GetKeyState(VK_SCROLL) & 0x0001) == 0x0001;
}

// src\ui\events\blink\blink_event_util.cc
// src\ui\events\win\events_win.cc
// src\ui\events\keycodes\dom\keycode_converter.cc
static void buildModifiers(WebInputEvent* evt)
{
    if (GetKeyState(VK_SHIFT) & HIGH_BIT_MASK_SHORT)
        evt->modifiers |= WebInputEvent::ShiftKey;
    if (GetKeyState(VK_CONTROL) & HIGH_BIT_MASK_SHORT)
        evt->modifiers |= WebInputEvent::ControlKey;
    if (GetKeyState(VK_MENU) & HIGH_BIT_MASK_SHORT)
        evt->modifiers |= WebInputEvent::AltKey;
    if (isCapsLockOn())
        evt->modifiers |= WebInputEvent::CapsLockOn;
    if (isNumLockOn())
        evt->modifiers |= WebInputEvent::NumLockOn;
}

WebKeyboardEvent PlatformEventHandler::buildKeyboardEvent(WebInputEvent::Type type, UINT message, WPARAM wParam, LPARAM lParam)
{
//     unsigned int flags = 0;
//     if (HIWORD(lParam) & KF_REPEAT)
//         flags |= KF_REPEAT;
//     if (HIWORD(lParam) & KF_EXTENDED)
//         flags |= KF_REPEAT;

    LPARAM keyData = lParam; // MAKELPARAM(0, (WORD)flags);
    WebKeyboardEvent keyEvent;
    keyEvent.windowsKeyCode = (type == WebInputEvent::RawKeyDown || type == WebInputEvent::KeyUp) ? wParam : 0;
    keyEvent.nativeKeyCode = wParam;
    keyEvent.domCode = 0;
    keyEvent.domKey = 0;
    keyEvent.timeStampSeconds = WTF::currentTime();
    keyEvent.size = sizeof(WebMouseEvent);
    keyEvent.type = type;

    buildModifiers(&keyEvent);
    if (isKeypadEvent(wParam, keyData, type))
        keyEvent.modifiers |= WebInputEvent::IsKeyPad;

    if (VK_LEFT == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "Left");
    else if (VK_UP == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "Up");
    else if (VK_UP == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "Up");
    else if (VK_RIGHT == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "Right");
    else if (VK_DOWN == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "Down");

    else if (VK_NEXT == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "PageDown");
    else if (VK_PRIOR == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "PageUp");

    else if (VK_HOME == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "Home");
    else if (VK_END == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "End");

    else if (VK_TAB == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "U+0009");
    else if (VK_BACK == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "U+0008");
    else if (VK_ESCAPE == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "U+001B");
    else if (VK_RETURN == keyEvent.windowsKeyCode)
        strcpy(keyEvent.keyIdentifier, "Enter");

    memset(keyEvent.text, 0, sizeof(WebUChar) * WebKeyboardEvent::textLengthCap);
    keyEvent.text[0] = (WebUChar)wParam;
    return keyEvent;
}

static void makeDraggableRegionNcHitTest(HWND hWnd, LPARAM lParam, bool* isDraggableRegionNcHitTest, IntPoint& lastPosForDrag)
{
    int xPos = ((int)(short)LOWORD(lParam));
    int yPos = ((int)(short)HIWORD(lParam));
    if (true == *isDraggableRegionNcHitTest) {
        //::PostMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(xPos, yPos));
        ::PostMessage(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
        lastPosForDrag = IntPoint(xPos, yPos);
    }
    else {
        if (hWnd)
            ::SetCapture(hWnd);
    }
    //*isDraggableRegionNcHitTest = false;
}

PlatformEventHandler::PlatformEventHandler(WebWidget* webWidget, WebViewImpl* webViewImpl)
    : m_checkMouseLeaveTimer(this, &PlatformEventHandler::checkMouseLeave)
{
    m_isDraggableRegionNcHitTest = false;
    m_postMouseLeave = false;
    m_mouseInWindow = false;
    m_isAlert = false;
    m_isDraggableRegionNcHitTest = false;
    m_isDraggableNodeMousedown = false;
    m_isLeftMousedown = false;
    m_lastTimeMouseDown = 0;
    m_webWidget = webWidget;
    m_webViewImpl = webViewImpl;
}

void PlatformEventHandler::fireCaptureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_isDraggableRegionNcHitTest) {
        ::ReleaseCapture();
        m_isDraggableRegionNcHitTest = false;

        MouseEvtInfo info = { true, false, nullptr };
        lParam = MAKELONG(m_lastPosForDrag.x(), m_lastPosForDrag.y());
        fireMouseEvent(hWnd, WM_LBUTTONUP, wParam, lParam, info, nullptr);
    }
}

void PlatformEventHandler::fireTouchEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static uint32_t uniqueTouchEventId = 0;
    uniqueTouchEventId++;

    double time = WTF::currentTime();
    WebTouchEvent webTouchEvent;
    webTouchEvent.timeStampSeconds = WTF::currentTime();
    webTouchEvent.size = sizeof(WebMouseEvent);
    webTouchEvent.modifiers = 0;
    webTouchEvent.touchesLength = 1;
    webTouchEvent.cancelable = true;
    webTouchEvent.causesScrollingIfUncanceled = false;
    webTouchEvent.uniqueTouchEventId = uniqueTouchEventId;

    WebTouchPoint& webTouchPoint = webTouchEvent.touches[0];
    webTouchPoint.id = 0;

    if (WM_LBUTTONDOWN == message) {
        webTouchEvent.type = WebInputEvent::TouchStart;
        webTouchPoint.state = WebTouchPoint::StatePressed;
    }
    else if (WM_LBUTTONUP == message) {
        webTouchEvent.type = WebInputEvent::TouchEnd;
        webTouchPoint.state = WebTouchPoint::StateReleased;
    }
    else if (WM_MOUSEMOVE == message) {
        webTouchEvent.type = WebInputEvent::TouchMove;
        webTouchPoint.state = WebTouchPoint::StateMoved;
    }

    POINT ptCursor;
    ::GetCursorPos(&ptCursor);

    webTouchPoint.screenPosition.x = ptCursor.x;
    webTouchPoint.screenPosition.y = ptCursor.y;

    webTouchPoint.position.x = ((int)(short)LOWORD(lParam));
    webTouchPoint.position.y = ((int)(short)HIWORD(lParam));

    webTouchPoint.radiusX = 10;
    webTouchPoint.radiusY = 10;
    webTouchPoint.rotationAngle = 0;

    m_webWidget->handleInputEvent(webTouchEvent);
}

static bool isNearPos(const blink::IntPoint& a, const blink::IntPoint& b)
{
    return std::abs(a.x() - b.x()) + std::abs(a.y() - b.y()) < 15;
}

static void WKE_CALL_TYPE postDragMessageImpl(HWND hWnd, void* param)
{
    ::ReleaseCapture();
    ::PostMessage(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
}

static void postDragMessage(HWND hWnd)
{
    HWND hRootWnd = hWnd;
    while (true)  {
        hWnd = ::GetParent(hWnd);
        if (!hWnd)
            break;
        hRootWnd = hWnd;
    }

    if (!hRootWnd)
        return;
    
    if (!blink::RuntimeEnabledFeatures::updataInOtherThreadEnabled()) {
        postDragMessageImpl(hRootWnd, nullptr);
        return;
    }

    if (wke::g_wkeUiThreadPostTaskCallback)
        wke::g_wkeUiThreadPostTaskCallback(hRootWnd, postDragMessageImpl, nullptr);
}

void PlatformEventHandler::buildMousePosInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* handle, IntPoint* pos, IntPoint* globalPos)
{
    if (WM_MOUSELEAVE == message) {
        *handle = true;
        m_postMouseLeave = true;

        POINT ptCursor;
        ::GetCursorPos(&ptCursor);
        *globalPos = IntPoint(ptCursor.x, ptCursor.y);
        ::ScreenToClient(hWnd, &ptCursor);
        if (ptCursor.x < 2)
            ptCursor.x = -1;
        else if (ptCursor.x > 10)
            ptCursor.x += 2;

        if (ptCursor.y < 2)
            ptCursor.y = -1;
        else if (ptCursor.y > 10)
            ptCursor.y += 2;

        *pos = IntPoint(ptCursor.x, ptCursor.y);

        lParam = MAKELPARAM(ptCursor.x, ptCursor.y);
    } else {
        m_postMouseLeave = false;
        pos->setX(/*m_offset.x() +*/ ((int)(short)LOWORD(lParam)));
        pos->setY(/*m_offset.y() +*/ ((int)(short)HIWORD(lParam)));

        POINT widgetPoint = { pos->x(), pos->y() };
        ::ClientToScreen(hWnd, &widgetPoint);

        *globalPos = IntPoint(widgetPoint.x, widgetPoint.y);
    }
}

// 鼠标点击标题栏时，收不到up消息，所以要模拟一次
bool PlatformEventHandler::fireMouseUpEventIfNeeded(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, const MouseEvtInfo& info, BOOL* bHandle)
{
    if ((WM_MOUSEMOVE != message && WM_MOUSELEAVE != message) || !m_isLeftMousedown)
        return false;

    if ((wParam & MK_LBUTTON))
        return false;

    fireMouseEvent(hWnd, WM_LBUTTONUP, 0, lParam, info, nullptr);
    if (bHandle)
        *bHandle = true;
    return true;
}

void PlatformEventHandler::checkMouseLeave(blink::Timer<PlatformEventHandler>*)
{
    if (!m_hWnd || !::IsWindow(m_hWnd)) {
        m_checkMouseLeaveTimer.stop();
        return;
    }

    POINT pt = { 0 };
    ::GetCursorPos(&pt);

    RECT rc;
    ::GetClientRect(m_hWnd, &rc);
    ::ScreenToClient(m_hWnd, &pt);

    if ((!::PtInRect(&rc, pt) || !::IsWindowVisible(m_hWnd))) {
        if (!m_isLeftMousedown) {
            MouseEvtInfo info = { true, false, nullptr };
            LPARAM lParam = MAKELONG(pt.x, pt.y);
            fireMouseEvent(m_hWnd, WM_MOUSELEAVE, 0, lParam, info, nullptr);
        }
        m_checkMouseLeaveTimer.stop();
    }
}

LRESULT PlatformEventHandler::fireMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, const MouseEvtInfo& info, BOOL* bHandle)
{
    m_hWnd = hWnd;
    bool handle = false;

    m_isDraggableRegionNcHitTest = false;
    if (true == m_isAlert)
        return 0;

    bool isValideWindow = ::IsWindow(hWnd) && !info.isWillDestroy;

    if (fireMouseUpEventIfNeeded(hWnd, message, wParam, lParam, info, bHandle))
        return 0;

    bool shift = false, ctrl = false, alt = false, meta = false;
    int clickCount = 0;

    IntPoint pos;
    IntPoint globalPos;

    buildMousePosInfo(hWnd, message, wParam, lParam, &handle, &pos, &globalPos);

    if (!m_checkMouseLeaveTimer.isActive())
        m_checkMouseLeaveTimer.startRepeating(0.2, FROM_HERE);
    if (WM_MOUSELEAVE == message)
        m_checkMouseLeaveTimer.stop();

    double time = WTF::currentTime();
    WebMouseEvent webMouseEvent;
    webMouseEvent.timeStampSeconds = WTF::currentTime();
    webMouseEvent.size = sizeof(WebMouseEvent);
    webMouseEvent.modifiers = 0;
    webMouseEvent.x = pos.x();
    webMouseEvent.y = pos.y();

    webMouseEvent.movementX = pos.x() - m_lastPosMouseMove.x();
    webMouseEvent.movementY = pos.y() - m_lastPosMouseMove.y();
    m_lastPosMouseMove = pos;

    webMouseEvent.windowX = pos.x();
    webMouseEvent.windowY = pos.y();
    webMouseEvent.globalX = globalPos.x();
    webMouseEvent.globalY = globalPos.y();
    webMouseEvent.clickCount = 1;
    buildModifiers(&webMouseEvent);

    if (WM_LBUTTONDOWN == message || WM_MBUTTONDOWN == message || WM_RBUTTONDOWN == message || WM_LBUTTONDBLCLK == message) {
        handle = true;

        double time = WTF::currentTime();
        const static double minInterval = GetDoubleClickTime() / 1000.0;

        if (time - m_lastTimeMouseDown < minInterval && isNearPos(m_lastPosMouseDown, pos))
            webMouseEvent.clickCount = 2;
        
        m_lastTimeMouseDown = time;
        m_lastPosMouseDown = pos;

        if (hWnd && info.isNeedSetFocus) {
            if (isValideWindow && ::GetFocus() != hWnd) {
                ::SetFocus(hWnd);
                m_webViewImpl->setFocus(true);
                m_webViewImpl->setIsActive(true);
            }
            if (isValideWindow)
                ::SetCapture(hWnd);
        }
        switch (message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            m_isLeftMousedown = true;
            webMouseEvent.button = WebMouseEvent::ButtonLeft;
            webMouseEvent.modifiers |= WebMouseEvent::LeftButtonDown;
            break;
        case WM_MBUTTONDOWN:
            webMouseEvent.button = WebMouseEvent::ButtonMiddle;
            webMouseEvent.modifiers |= WebMouseEvent::MiddleButtonDown;
            break;
        case WM_RBUTTONDOWN:
            webMouseEvent.button = WebMouseEvent::ButtonRight;
            webMouseEvent.modifiers |= WebMouseEvent::RightButtonDown;
            break;
        }
        m_isDraggableRegionNcHitTest = false;
        webMouseEvent.type = WebInputEvent::MouseDown;
        bool b = m_webWidget->handleInputEvent(webMouseEvent);

        bool isDraggable = false;
        if (WM_LBUTTONDOWN == message)
            isDraggable = isDraggableRegionNcHitTest(hWnd, pos, info.draggableRegion);

        if (isValideWindow && isDraggable)
            postDragMessage(hWnd);
        m_isDraggableNodeMousedown = false;
    } else if (WM_LBUTTONUP == message || WM_MBUTTONUP == message || WM_RBUTTONUP == message) {
        handle = true;
        
        switch (message) {
        case WM_LBUTTONUP:
            m_isLeftMousedown = false;
            webMouseEvent.button = WebMouseEvent::ButtonLeft;
            webMouseEvent.modifiers |= WebMouseEvent::LeftButtonDown;
            break;
        case WM_MBUTTONUP:
            webMouseEvent.button = WebMouseEvent::ButtonMiddle;
            webMouseEvent.modifiers |= WebMouseEvent::MiddleButtonDown;
            break;
        case WM_RBUTTONUP:
            webMouseEvent.button = WebMouseEvent::ButtonRight;
            webMouseEvent.modifiers |= WebMouseEvent::RightButtonDown;
            break;
        }
        if (isValideWindow)
            ::ReleaseCapture();
        if (m_webViewImpl)
            m_webViewImpl->dragSourceSystemDragEnded();
        webMouseEvent.type = WebInputEvent::MouseUp;
        m_webWidget->handleInputEvent(webMouseEvent);        
    } else if (WM_MOUSEMOVE == message || WM_MOUSELEAVE == message) {
        handle = true;
        if (wParam & MK_LBUTTON) {
            webMouseEvent.button = WebMouseEvent::ButtonLeft;
            webMouseEvent.modifiers |= WebMouseEvent::LeftButtonDown;            
        } else if (wParam & MK_MBUTTON) {
            webMouseEvent.button = WebMouseEvent::ButtonMiddle;
            webMouseEvent.modifiers |= WebMouseEvent::MiddleButtonDown;
        } else if (wParam & MK_RBUTTON) {
            webMouseEvent.button = WebMouseEvent::ButtonRight;
            webMouseEvent.modifiers |= WebMouseEvent::RightButtonDown;
        } else {
            ASSERT(!m_isLeftMousedown);
            webMouseEvent.button = WebMouseEvent::ButtonNone;
        }

        bool b = false;
        switch (message) {
        case WM_MOUSEMOVE:
            if (!m_mouseInWindow) {
                webMouseEvent.type = WebInputEvent::MouseEnter;
                m_mouseInWindow = true;
            } else
                webMouseEvent.type = WebInputEvent::MouseMove;

            b = m_webWidget->handleInputEvent(webMouseEvent);
            break;
        case WM_MOUSELEAVE:
            webMouseEvent.type = WebInputEvent::MouseLeave;
            if (m_webViewImpl)
                m_webViewImpl->dragSourceSystemDragEnded();
            b = m_webWidget->handleInputEvent(webMouseEvent);
            m_mouseInWindow = false;
            break;
        }
    }

    if (bHandle)
        *bHandle = handle;
    return 0;
}

void PlatformEventHandler::setIsDraggableNodeMousedown()
{
    m_isDraggableNodeMousedown = true;
}

bool PlatformEventHandler::isDraggableRegionNcHitTest(HWND hWnd, const blink::IntPoint& pos, HRGN draggableRegion)
{
    // 单线程情况下，直接在mb内部处理拖拽。多线程渲染时，必须在ui线程处理，否则blink线程卡了，会导致拖拽也很卡
    if (!blink::RuntimeEnabledFeatures::updataInOtherThreadEnabled())
        return m_isDraggableNodeMousedown;
    return false;

//     if (!draggableRegion)
//         return false;
// 
//     return ::PtInRegion(draggableRegion, pos.x(), pos.y());
}

static int verticalScrollLines()
{
    static ULONG scrollLines;
    if (!scrollLines && !SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0))
        scrollLines = 3;
    return scrollLines;
}

static int horizontalScrollChars()
{
    static ULONG scrollChars;
    if (!scrollChars && !SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &scrollChars, 0))
        scrollChars = 1;
    return scrollChars;
}

LRESULT PlatformEventHandler::fireWheelEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    POINT point = { x, y };
    ::ScreenToClient(hWnd, &point);
    x = point.x;
    y = point.y;

    int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    int modifiers = 0;
    static const float cScrollbarPixelsPerLine = 100.0f / 3.0f;
    float delta = wheelDelta / static_cast<float>(WHEEL_DELTA);

    float deltaX = 0.f;
    float deltaY = 0.f;

    bool shiftKey = wParam & MK_SHIFT;
    bool ctrlKey = wParam & MK_CONTROL;

    blink::PlatformWheelEventGranularity granularity = blink::ScrollByPageWheelEvent;

    if (shiftKey) {
        deltaX = delta * static_cast<float>(horizontalScrollChars()) * cScrollbarPixelsPerLine;
        deltaY = 0;
        granularity = blink::ScrollByPixelWheelEvent;
        modifiers |= WebInputEvent::ShiftKey;
    } else {
        deltaX = 0;
        deltaY = delta;
        int verticalMultiplier = verticalScrollLines();
        granularity = (verticalMultiplier == WHEEL_PAGESCROLL) ? blink::ScrollByPageWheelEvent : blink::ScrollByPixelWheelEvent;
        if (granularity == blink::ScrollByPixelWheelEvent)
            deltaY *= static_cast<float>(verticalMultiplier)* cScrollbarPixelsPerLine;
    }
   
    if (ctrlKey)
        modifiers |= WebInputEvent::ControlKey;
    
    WebMouseWheelEvent webWheelEvent;
    webWheelEvent.type = WebInputEvent::MouseWheel;
    webWheelEvent.x = x;
    webWheelEvent.y = y;
    webWheelEvent.globalX = x;
    webWheelEvent.globalY = y;
    webWheelEvent.deltaX = deltaX;
    webWheelEvent.deltaY = deltaY;
    webWheelEvent.wheelTicksX = 0.f;
    webWheelEvent.wheelTicksY = delta;
    webWheelEvent.hasPreciseScrollingDeltas = true;
    webWheelEvent.modifiers = modifiers;
    m_webWidget->handleInputEvent(webWheelEvent);

    return 0;
}

}

#endif // PlatformEventUtil_h