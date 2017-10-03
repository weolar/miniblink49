/*
* Copyright (C) 2011 Apple Inc. All rights reserved.
* Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "content/browser/PopupMenuWin.h"
#include "content/browser/PopupMenuWinClient.h"
#include "content/browser/PlatformEventHandler.h"
#include "third_party/WebKit/Source/platform/geometry/win/IntRectWin.h"
#include "third_party/WebKit/Source/web/WebPagePopupImpl.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/Source/core/frame/LocalDOMWindow.h"
#include "third_party/WebKit/Source/core/frame/FrameView.h"
#include "third_party/WebKit/Source/core/dom/Document.h"
#include "third_party/WebKit/public/web/WebFrameClient.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "skia/ext/bitmap_platform_device_win.h"
#include "cc/trees/LayerTreeHost.h"

using namespace blink;

namespace content {

#define HIGH_BIT_MASK_SHORT 0x8000

#define WM_INIT_MENU        (WM_USER + 201)
#define WM_COMMIT           (WM_USER + 202)
#define WM_PMW_KILLFOCUS    (WM_USER + 203)
#define WM_PMW_MOUSEHWHEEL  (WM_USER + 204)

HWND PopupMenuWin::m_hPopup = NULL;
    
// Default Window animation duration in milliseconds
static const int defaultAnimationDuration = 200;
// Maximum height of a popup window
static const int maxPopupHeight = 320;

const int optionSpacingMiddle = 1;
const int popupWindowBorderWidth = 1;
const int smoothScrollAnimationDuration = 5000;

static LPCWSTR kPopupWindowClassName = L"PopupWindowClass";

PopupMenuWin::PopupMenuWin(PopupMenuWinClient* client, HWND hWnd, IntPoint offset, WebViewImpl* webViewImpl)
    : m_asynStartCreateWndTimer(this, &PopupMenuWin::asynStartCreateWnd)
{
    m_client = client;
    m_popupImpl = nullptr;
    m_needsCommit = true;
    m_hasResize = true;
    m_needResize = true;
    m_initialize = false;
    m_isCommiting = false;
    m_isVisible = false;
    m_lastFrameTimeMonotonic = 0;
    m_memoryCanvas = nullptr;
    m_layerTreeHost = nullptr;
    m_webViewImpl = webViewImpl;
    m_hParentWnd = hWnd;
    m_rect.setWidth(1);
    m_rect.setHeight(1);
    m_offset.setX(offset.x());
    m_offset.setY(offset.y());
}

PopupMenuWin::~PopupMenuWin()
{
    if (m_memoryCanvas)
        delete m_memoryCanvas;
    m_memoryCanvas = nullptr;

    if (m_layerTreeHost) {
        m_layerTreeHost->applyActions(true);
        delete m_layerTreeHost;
    }
    m_layerTreeHost = nullptr;
}

static void destroyWindowAsyn(HWND hWnd)
{
    ::DestroyWindow(hWnd);
}

void PopupMenuWin::closeWidgetSoon()
{
    m_client->onPopupMenuHide();

    m_asynStartCreateWndTimer.stop();
    m_layerTreeHost->applyActions(false);
    m_initialize = false;
    m_isVisible = false;
    ::ShowWindow(m_hPopup, SW_HIDE);
    ::SetWindowLongPtr(m_hPopup, 0, 0);
    //blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&destroyWindowAsyn, m_hPopup));
}

void PopupMenuWin::registerClass()
{
    static bool haveRegisteredWindowClass = false;
    if (haveRegisteredWindowClass)
        return;

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.hIconSm = 0;
    wcex.style = CS_DROPSHADOW;

    wcex.lpfnWndProc = PopupMenuWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(PopupMenuWin*); // For the PopupMenu pointer
    wcex.hInstance = NULL;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = kPopupWindowClassName;

    haveRegisteredWindowClass = true;

    RegisterClassEx(&wcex);
}

LRESULT PopupMenuWin::fireWheelEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!m_hPopup)
        return 0;
    return wndProc(m_hPopup, message, wParam, lParam);
}

bool PopupMenuWin::fireKeyUpEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!m_hPopup)
        return false;
    wndProc(m_hPopup, message, wParam, lParam);
    return true;
}

LRESULT CALLBACK PopupMenuWin::PopupMenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PopupMenuWin* popup = nullptr;
    if (popup = reinterpret_cast<PopupMenuWin*>(::GetWindowLongPtr(hWnd, 0)))
        return popup->wndProc(hWnd, message, wParam, lParam);

    if (message == WM_CREATE) {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        popup = (PopupMenuWin*)createStruct->lpCreateParams;
        // Associate the PopupMenu with the window.
        ::SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(popup));
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT PopupMenuWin::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;

//     String out = String::format("PopupMenuWin wndProc: %x %04x\n", hWnd, message);
//     OutputDebugStringA(out.utf8().data());

    switch (message) {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP: {
        if (!m_initialize)
            break;
        BOOL bHandle = FALSE;
        m_platformEventHandler->fireMouseEvent(hWnd, message, wParam, lParam, false, &bHandle);
        if (bHandle)
            return 0;
        break;
    }
    case WM_PMW_MOUSEHWHEEL:
    case WM_MOUSEWHEEL:
        m_platformEventHandler->fireWheelEvent(hWnd, message, wParam, lParam);
        break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN: {
        if (!m_initialize)
            break;
        unsigned int virtualKeyCode = wParam;
        WebKeyboardEvent keyEvent = PlatformEventHandler::buildKeyboardEvent(WebInputEvent::RawKeyDown, message, wParam, lParam);

        bool handled = m_popupImpl->handleInputEvent(keyEvent);
        if (handled)
            return 0;
        break;
    }
    case WM_CHAR: {
        lResult = 0;
        switch (wParam) {
        case 0x0D:   // Enter/Return
            //m_popupImpl->close();
//             index = focusedIndex();
//             ASSERT(index >= 0);
//             client()->valueChanged(index);
            break;
        case 0x1B:   // Escape
            //m_popupImpl->close();
            break;
        case 0x09:   // TAB
        case 0x08:   // Backspace
        case 0x0A:   // Linefeed
        default:     // Character
            lResult = 1;
            break;
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT paintInfo;
        ::BeginPaint(popupHandle(), &paintInfo);
        paint(paintInfo.hdc, paintInfo.rcPaint);
        ::EndPaint(popupHandle(), &paintInfo);
        lResult = 0;
        break;
    }
    case WM_PRINTCLIENT:
        //paint(clientRect(), (HDC)wParam);
        break;
    case WM_GETOBJECT:
        //onGetObject(wParam, lParam, lResult);
        break;

    case WM_TIMER:
        beginMainFrame();
        break;
    case WM_PMW_KILLFOCUS:
    //case WM_KILLFOCUS:
        m_isVisible = false;
        m_webViewImpl->hidePopups();
        break;
    case WM_INIT_MENU:
        initialize();
        break;
    case WM_COMMIT:
        beginMainFrame();
        break;
    case WM_MOUSEACTIVATE:
        lResult = MA_NOACTIVATE;
        break;
    default:
        lResult = ::DefWindowProc(hWnd, message, wParam, lParam);
    }

    return lResult;
}

void PopupMenuWin::beginMainFrame()
{
    m_isCommiting = false;
    if (!m_needsCommit || !m_initialize)
        return;
    m_needsCommit = false;
    
    blink::WebPagePopup* popup = m_popupImpl;

    updataSize();

    m_layerTreeHost->beginRecordActions(true);

    double lastFrameTimeMonotonic = WTF::currentTime();
    WebBeginFrameArgs frameTime(m_lastFrameTimeMonotonic, 0, lastFrameTimeMonotonic - m_lastFrameTimeMonotonic);
    popup->beginFrame(frameTime);
    popup->layout();

    if (0)
        m_layerTreeHost->showDebug();

    m_layerTreeHost->recordDraw();
    
    m_layerTreeHost->preDrawFrame();
    updataPaint();
    m_layerTreeHost->postDrawFrame();

    m_layerTreeHost->endRecordActions();
}

void PopupMenuWin::updataSize()
{
    if (!m_needResize || !m_hPopup)
        return;
    m_needResize = false;

    blink::WebPagePopup* popup = m_popupImpl;

    bool needsCommit = m_needsCommit;
    blink::IntSize size(m_rect.width(), m_rect.height());
    if (m_layerTreeHost)
        m_layerTreeHost->setViewportSize(size);
    popup->resize(size);
    m_needsCommit = needsCommit;

    POINT pos = { m_rect.x() + m_offset.x(), m_rect.y() + m_offset.y() };
    ::ClientToScreen(m_hParentWnd, &pos);
    ::SetWindowPos(m_hPopup, HWND_TOP, pos.x, pos.y, m_rect.width(), m_rect.height(), SWP_SHOWWINDOW | SWP_NOACTIVATE);
}

void PopupMenuWin::show(WebNavigationPolicy)
{
    m_needResize = true;
    m_hasResize = true;
    m_isVisible = true;
    postCommit();
    
    updataSize();

    if (m_hPopup)
        ::ShowWindow(m_hPopup, SW_SHOWNOACTIVATE);
}

void PopupMenuWin::paint(HDC hdc, RECT rcPaint)
{
    if (!m_memoryCanvas)
        return;
    skia::DrawToNativeContext(m_memoryCanvas, hdc, rcPaint.left, rcPaint.top, &rcPaint);
}

void PopupMenuWin::updataPaint()
{
    if (!m_initialize)
        return;

    if (!m_memoryCanvas || m_hasResize) {
        m_hasResize = false;

        if (m_memoryCanvas)
            delete m_memoryCanvas;
        m_memoryCanvas = skia::CreatePlatformCanvas(m_rect.width(), m_rect.height(), !false);
    }

    IntRect clip(0, 0, m_rect.width(), m_rect.height());

    HDC hMemoryDC = skia::BeginPlatformPaint(m_memoryCanvas);
    m_memoryCanvas->save();

    SkPaint paint;
    paint.setAntiAlias(true);
    m_memoryCanvas->drawPaint(paint);
    m_layerTreeHost->drawToCanvas(m_memoryCanvas, clip);
    m_memoryCanvas->restore();

    if (m_hPopup) {
        HDC hdc = GetDC(m_hPopup);
        RECT rc = { 0, 0, m_rect.width(), m_rect.height() };
        skia::DrawToNativeContext(m_memoryCanvas, hdc, 0, 0, &rc);
        ::ReleaseDC(m_hPopup, hdc);
    }
    skia::EndPlatformPaint(m_memoryCanvas);
}

static void initWndStyle(HWND hPopup)
{
    // ��������뵽WebPageImpl::fireKillFocusEvent
    // ::SetWindowPos(hPopup, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
    // ::SetFocus(hPopup);
    // ::SetCapture(hPopup);
    // ::SetForegroundWindow(hPopup);
    ::ShowWindow(hPopup, SW_HIDE);
}

static HHOOK g_hMouseHook = nullptr;

struct MY_MOUSEHOOKSTRUCTEX : public tagMOUSEHOOKSTRUCT {
    DWORD   mouseData;
};

LRESULT CALLBACK PopupMenuWin::mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
    if (pMouseStruct == NULL || nullptr == m_hPopup || pMouseStruct->hwnd == m_hPopup)
        return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

    PopupMenuWin* popup = reinterpret_cast<PopupMenuWin*>(::GetWindowLongPtr(m_hPopup, 0));
    if (!popup || !popup->m_isVisible)
        return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

    if (wParam == WM_LBUTTONDOWN || wParam == WM_NCLBUTTONDOWN ||
        wParam == WM_RBUTTONDOWN || wParam == WM_NCRBUTTONDOWN)
        ::PostMessage(m_hPopup, WM_PMW_KILLFOCUS, 0, 0);

    if (wParam == WM_MOUSEHWHEEL) {
        MY_MOUSEHOOKSTRUCTEX* pMouseStructEx = (MY_MOUSEHOOKSTRUCTEX*)lParam;
        WORD delta = HIWORD(pMouseStructEx->mouseData);
        WPARAM wParamToPost = pMouseStructEx->mouseData;
        ::PostMessage(m_hPopup, WM_PMW_MOUSEHWHEEL, wParamToPost, 0);
    }
    
    return ::CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

void PopupMenuWin::asynStartCreateWnd(blink::Timer<PopupMenuWin>*)
{
    if (!m_hPopup) {
        registerClass();

        m_hPopup = CreateWindowEx(WS_EX_NOACTIVATE, kPopupWindowClassName, L"MbPopupMenu", WS_POPUP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hParentWnd, 0, 0, this);
        
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE, mouseHookProc, GetModuleHandle(NULL), GetCurrentThreadId());

    }
    m_client->onPopupMenuCreate(m_hPopup);
    initWndStyle(m_hPopup);

    if (m_isVisible) {
        updataSize();
        ::ShowWindow(m_hPopup, SW_SHOWNOACTIVATE);
    }

    if (m_needsCommit)
        ::PostMessage(m_hPopup, WM_COMMIT, 0, 0);
}

WebWidget* PopupMenuWin::createWnd()
{
    if (!m_hPopup)
        m_asynStartCreateWndTimer.startOneShot(0.0, FROM_HERE);
    else {
        if (m_needsCommit)
            ::PostMessage(m_hPopup, WM_COMMIT, 0, 0);
        ::SetWindowLongPtr(m_hPopup, 0, reinterpret_cast<LONG_PTR>(this));
    }

    initialize();
    return m_popupImpl;
}

void PopupMenuWin::initialize()
{
    if (m_initialize)
        return;
    m_initialize = true;

    m_layerTreeHost = new cc::LayerTreeHost(this, nullptr);
    m_popupImpl = WebPagePopup::create(this);
    m_platformEventHandler = new PlatformEventHandler(m_popupImpl, nullptr);
    m_layerTreeHost->setWebGestureCurveTarget(m_webViewImpl);
    m_popupImpl->setFocus(true);
}

WebWidget* PopupMenuWin::create(PopupMenuWinClient* client, HWND hWnd, blink::IntPoint offset, WebViewImpl* webViewImpl, WebPopupType type, PopupMenuWin** result)
{
    PopupMenuWin* self = new PopupMenuWin(client, hWnd, offset, webViewImpl);
    if (result)
        *result = self;
    return self->createWnd();
}

void PopupMenuWin::postCommit()
{
    m_needsCommit = true;
    if (!m_isCommiting && m_hPopup) {
        m_isCommiting = true;
        ::PostMessage(m_hPopup, WM_COMMIT, 0, 0);
    }
}

void PopupMenuWin::didInvalidateRect(const blink::WebRect& r)
{
    //::InvalidateRect(m_hPopup, NULL, TRUE);
    postCommit();
}

void PopupMenuWin::didAutoResize(const WebSize& newSize)
{
    didUpdateLayoutSize(newSize);
}

void PopupMenuWin::didUpdateLayoutSize(const WebSize& newSize)
{
    m_needResize = true;
    m_hasResize = true;
    postCommit();
}

static void trimWidthHeight(blink::IntRect& rect)
{
    if (0 == rect.width())
        rect.setWidth(1);

    if (0 == rect.height())
        rect.setHeight(1);
}

void PopupMenuWin::setWindowRect(const WebRect& r)
{
    if (m_rect == (blink::IntRect)r)
        return;

    m_rect = r;
    trimWidthHeight(m_rect);

    m_needResize = true;
    m_hasResize = true;
    postCommit();
}

void PopupMenuWin::scheduleAnimation()
{
    postCommit();
}

WebLayerTreeView* PopupMenuWin::layerTreeView()
{
    return m_layerTreeHost;
}

bool PopupMenuWin::initSetting()
{
    return true;
}

DEFINE_TRACE(PopupMenuWin)
{
}

} // content