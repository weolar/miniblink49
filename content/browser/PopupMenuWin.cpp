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
#include "skia/ext/bitmap_platform_device_win.h"
#include "cc/trees/LayerTreeHost.h"

using namespace blink;

namespace content {

#define HIGH_BIT_MASK_SHORT 0x8000

#define WM_INIT_MENU (WM_USER + 201)
#define WM_COMMIT    (WM_USER + 202)
    
// Default Window animation duration in milliseconds
static const int defaultAnimationDuration = 200;
// Maximum height of a popup window
static const int maxPopupHeight = 320;

const int optionSpacingMiddle = 1;
const int popupWindowBorderWidth = 1;
const int smoothScrollAnimationDuration = 5000;

static LPCWSTR kPopupWindowClassName = L"PopupWindowClass";

PopupMenuWin::PopupMenuWin(HWND hWnd, IntPoint offset, WebViewImpl* webViewImpl)
{
    m_popup = NULL;
    m_popupImpl = nullptr;
    m_needsCommit = true;
    m_hasResize = true;
    m_needResize = true;
    m_initialize = false;
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

void PopupMenuWin::closeWidgetSoon()
{
    m_initialize = false;
    ::DestroyWindow(m_popup);
    delete this;
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

LRESULT CALLBACK PopupMenuWin::PopupMenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PopupMenuWin* popup = nullptr;
    if (popup = reinterpret_cast<PopupMenuWin*>(::GetWindowLongPtr(hWnd, 0)))
        return popup->wndProc(hWnd, message, wParam, lParam);

    if (message == WM_CREATE) {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        popup = (PopupMenuWin*)createStruct->lpCreateParams;
        // Associate the PopupMenu with the window.
        SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(popup));
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT PopupMenuWin::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;

    switch (message) {
    
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP: {
        if (!m_initialize)
            break;
        BOOL bHandle = FALSE;
        m_platformEventHandler->fireMouseEvent(hWnd, message, wParam, lParam, &bHandle);
        if (bHandle)
            return 0;
        break;
    }
    case WM_MOUSEWHEEL: {
        break;
    }

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN: {
        if (!m_initialize)
            break;
        unsigned int virtualKeyCode = wParam;
        WebKeyboardEvent keyEvent = PlatformEventHandler::buildKeyboardEvent(WebInputEvent::RawKeyDown, message, wParam, lParam);

        bool handled = m_webViewImpl->handleInputEvent(keyEvent);
        if (handled)
            return 0;
        break;
    }
    case WM_CHAR: {
//         if (!client())
//             break;

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
    case WM_KILLFOCUS:
        ::ShowWindow(m_popup, SW_HIDE);
        break;
    case WM_INIT_MENU:
        initialize();
        break;
    case WM_COMMIT:
        beginMainFrame();
        break;
    case WM_MOUSEACTIVATE: // 不激活窗口
        lResult = MA_NOACTIVATE;
        break;
    default:
        lResult = ::DefWindowProc(hWnd, message, wParam, lParam);
    }

    return lResult;
}

void PopupMenuWin::beginMainFrame()
{
    if (!m_needsCommit || !m_initialize)
        return;
    m_needsCommit = false;

    blink::WebPagePopup* popup = m_popupImpl;

    updataSize();

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
}

void PopupMenuWin::updataSize()
{
    if (!m_needResize)
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

    ::MoveWindow(m_popup, pos.x, pos.y, m_rect.width(), m_rect.height(), false);
}

void PopupMenuWin::show(WebNavigationPolicy)
{
    m_needResize = true;
    m_hasResize = true;
    m_needsCommit = true;
    ::PostMessage(m_popup, WM_COMMIT, 0, 0);
}

void PopupMenuWin::paint(HDC hdc, RECT rcPaint)
{
    if (m_memoryCanvas)
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
    skia::EndPlatformPaint(m_memoryCanvas);
}

WebWidget* PopupMenuWin::createWnd()
{
    registerClass();

    DWORD exStyle = WS_EX_LTRREADING | WS_EX_TOOLWINDOW;
    m_popup = ::CreateWindowExW(exStyle, kPopupWindowClassName, L"PopupMenu",
        WS_POPUP,
        0, 0, 2, 2,
        /*NULL*/m_hParentWnd, 0, NULL, this); // 指定父窗口

    //::ShowWindow(m_popup, SW_SHOW);
    //::UpdateWindow(m_popup);
    //::SetTimer(m_popup, 1, 10, nullptr);
    ::SetWindowPos(m_popup, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE); // 不激活窗口

    initialize();

    return m_popupImpl;
}

void PopupMenuWin::initialize()
{
    if (m_initialize)
        return;
    m_initialize = true;

    m_layerTreeHost = new cc::LayerTreeHost(this);
    //m_layerTreeHost->setNeedTileRender(false);
    m_popupImpl = WebPagePopup::create(this);
    m_platformEventHandler = new PlatformEventHandler(m_popupImpl, nullptr);
    m_layerTreeHost->setWebGestureCurveTarget(m_webViewImpl);
    m_popupImpl->setFocus(true);
}

WebWidget* PopupMenuWin::create(HWND hWnd, blink::IntPoint offset, WebViewImpl* webViewImpl, WebPopupType type, PopupMenuWin** result)
{
    PopupMenuWin* self = new PopupMenuWin(hWnd, offset, webViewImpl);
    if (result)
        *result = self;
    return self->createWnd();
}

void PopupMenuWin::didInvalidateRect(const blink::WebRect& r)
{
    ::InvalidateRect(m_popup, NULL, TRUE);
    m_needsCommit = true;
    ::PostMessage(m_popup, WM_COMMIT, 0, 0);
}

void PopupMenuWin::didAutoResize(const WebSize& newSize)
{
    didUpdateLayoutSize(newSize);
}

void PopupMenuWin::didUpdateLayoutSize(const WebSize& newSize)
{
    m_needResize = true;
    m_hasResize = true;
    m_needsCommit = true;
    ::PostMessage(m_popup, WM_COMMIT, 0, 0);
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
    m_rect = r;
//     m_rect.setWidth(r.width);
//     m_rect.setHeight(r.height);

    trimWidthHeight(m_rect);

    m_needResize = true;
    m_hasResize = true;
    m_needsCommit = true;
    ::PostMessage(m_popup, WM_COMMIT, 0, 0);
}

void PopupMenuWin::scheduleAnimation()
{
    m_needsCommit = true;
    ::PostMessage(m_popup, WM_COMMIT, 0, 0);
}

WebLayerTreeView* PopupMenuWin::layerTreeView()
{
    return m_layerTreeHost;
}

bool PopupMenuWin::initSetting()
{
    return true;
}

} // content