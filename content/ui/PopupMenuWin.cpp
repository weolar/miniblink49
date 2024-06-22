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

#include "content/ui/PopupMenuWin.h"
#include "content/ui/PopupMenuWinClient.h"
#include "content/browser/PlatformEventHandler.h"
#include "third_party/WebKit/Source/platform/geometry/win/IntRectWin.h"
#include "third_party/WebKit/Source/web/WebPagePopupImpl.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/Source/core/frame/LocalDOMWindow.h"
#include "third_party/WebKit/Source/core/frame/FrameView.h"
#include "third_party/WebKit/Source/core/dom/Document.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/wtf/Locker.h"
#include "third_party/WebKit/public/web/WebFrameClient.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "skia/ext/bitmap_platform_device_win.h"
#include "mc/trees/LayerTreeHost.h"
#ifndef NO_USE_ORIG_CHROME
#include "orig_chrome/content/OrigChromeMgr.h"
#include "orig_chrome/content/LayerTreeWrap.h"
#endif
#include "content/browser/PostTaskHelper.h"
#include "net/ActivatingObjCheck.h"

using namespace blink;

namespace content {

#define HIGH_BIT_MASK_SHORT 0x8000

#define WM_INIT_MENU        (WM_USER + 201)
#define WM_PMW_KILLFOCUS    (WM_USER + 203)
#define WM_PMW_MOUSEHWHEEL  (WM_USER + 204)

bool g_popupMenuIniting = false;

HWND PopupMenuWin::m_hPopup = NULL;
bool PopupMenuWin::m_initialize = false;
    
// Default Window animation duration in milliseconds
static const int defaultAnimationDuration = 200;
// Maximum height of a popup window
static const int maxPopupHeight = 320;

const int optionSpacingMiddle = 1;
const int popupWindowBorderWidth = 1;
const int smoothScrollAnimationDuration = 5000;

static LPCWSTR kPopupWindowClassName = L"PopupWindowClass";

WTF::RecursiveMutex g_popupMenuMutex;

PopupMenuWin::PopupMenuWin(PopupMenuWinClient* client, HWND hWnd, IntPoint offset, WebViewImpl* webViewImpl)
    //: m_asynStartCreateWndTimer(this, &PopupMenuWin::asynStartCreateWnd)
{
    m_client = client;
    m_popupImpl = nullptr;
    m_allowBeginFrame = true;
    m_hasResize = true;
    m_needResize = true;
    m_initialize = false;
    m_isCommiting = false;
    m_isVisible = false;
    m_lastFrameTimeMonotonic = 0;
    m_memoryCanvas = nullptr;
    m_layerTreeHost = nullptr;
#ifndef NO_USE_ORIG_CHROME
    m_ccLayerTreeWrap = nullptr;
#endif
    m_webViewImpl = webViewImpl;
    m_hParentWnd = hWnd;
    m_rect.setWidth(1);
    m_rect.setHeight(1);
    m_offset.setX(offset.x());
    m_offset.setY(offset.y());
    m_lastFocusWnd = nullptr;

    m_id = net::ActivatingObjCheck::inst()->genId();
    net::ActivatingObjCheck::inst()->add(m_id);
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

//     ::SetPropW(m_hPopup, kPopupWindowClassName, nullptr);
}

static void destroyWindowAsyn(HWND hWnd)
{
    ::DestroyWindow(hWnd);
}

void PopupMenuWin::closeWidgetSoon()
{
    g_popupMenuMutex.lock();

    m_client->onPopupMenuHide();

    //m_asynStartCreateWndTimer.stop();
    if (m_layerTreeHost)
        m_layerTreeHost->applyActions(false);
    m_initialize = false;
    m_isVisible = false;

    net::ActivatingObjCheck::inst()->remove(m_id);
    g_popupMenuMutex.unlock();

#ifndef NO_USE_ORIG_CHROME
    if (m_ccLayerTreeWrap)
        delete m_ccLayerTreeWrap;
    m_ccLayerTreeWrap = nullptr;
#endif

    HWND hPopup = m_hPopup;
    postTaskToUiThread(FROM_HERE, m_hParentWnd, [hPopup] {
        ::ShowWindow(hPopup, SW_HIDE);
        ::SetPropW(hPopup, kPopupWindowClassName, nullptr);
    });
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

    wcex.lpfnWndProc = popupMenuWndProc;
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
    wndProc(m_hPopup, message, wParam, lParam);
    return 0;
}

bool PopupMenuWin::fireKeyUpEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!m_hPopup)
        return false;
    wndProc(m_hPopup, message, wParam, lParam);
    return true;
}

LRESULT CALLBACK PopupMenuWin::popupMenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WTF::Locker<RecursiveMutex> locker(g_popupMenuMutex);
    if (!m_initialize)
        return 0;

    PopupMenuWin* self = nullptr;
    if (message == WM_CREATE) {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        self = (PopupMenuWin*)createStruct->lpCreateParams;
        ::SetPropW(hWnd, kPopupWindowClassName, (HANDLE)self);
        return 0;
    }

    self = (PopupMenuWin*)::GetPropW(hWnd, kPopupWindowClassName);
    if (self)
        return self->wndProc(hWnd, message, wParam, lParam);

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT PopupMenuWin::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;
    BOOL bHandle = FALSE;
    POINT ptCursor = { 0 };
    PlatformEventHandler::MouseEvtInfo info = { false, false, nullptr };
    HWND hFocusWnd = nullptr;
    PlatformEventHandler* eventHandler = m_platformEventHandler;
    blink::WebPagePopup* popupImpl = m_popupImpl;
    PopupMenuWin* self = this;
    int id = m_id;

//     String out = String::format("PopupMenuWin wndProc: %x %04x\n", hWnd, message);
//     OutputDebugStringA(out.utf8().data());

    switch (message) {
    case WM_NCDESTROY:
        OutputDebugStringA("PopupMenuWin::wndProc WM_NCDESTROY\n");
        break;
    case WM_CAPTURECHANGED: { // 在多线程环境下，收不到WM_LBUTTONUP消息，只能这里来模拟
        ::GetCursorPos(&ptCursor);
        ::ScreenToClient(hWnd, &ptCursor);
        lParam = MAKELONG(ptCursor.x, ptCursor.y);

        postTaskToMainThread(FROM_HERE, [self, id, eventHandler, hWnd, lParam, info] {
            if (!m_initialize || !net::ActivatingObjCheck::inst()->isActivating(id))
                return;
            BOOL bHandle = FALSE;
            eventHandler->fireMouseEvent(hWnd, WM_LBUTTONDOWN, 0, lParam, info, &bHandle);
            eventHandler->fireMouseEvent(hWnd, WM_LBUTTONUP, 0, lParam, info, &bHandle);
            self->hide();
        });

        if (bHandle)
            return 0;
    }
        break;
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP: 
        if (!m_initialize)
            break;

        postTaskToMainThread(FROM_HERE, [eventHandler, id, hWnd, message, wParam, lParam, info] {
            if (!m_initialize || !net::ActivatingObjCheck::inst()->isActivating(id))
                return;
            BOOL bHandle = FALSE;
            eventHandler->fireMouseEvent(hWnd, message, wParam, lParam, info, &bHandle);
        });
        if (bHandle)
            return 0;
        break;
    
    case WM_PMW_MOUSEHWHEEL:
    case WM_MOUSEWHEEL: 
        postTaskToMainThread(FROM_HERE, [eventHandler, id, hWnd, message, wParam, lParam] {
            if (!m_initialize || !net::ActivatingObjCheck::inst()->isActivating(id))
                return;
            eventHandler->fireWheelEvent(hWnd, message, wParam, lParam);
        });
        break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN: 
        if (!m_initialize)
            break;

        postTaskToMainThread(FROM_HERE, [popupImpl, id, hWnd, message, wParam, lParam] {
            if (!m_initialize || !net::ActivatingObjCheck::inst()->isActivating(id))
                return;
            WebKeyboardEvent keyEvent = PlatformEventHandler::buildKeyboardEvent(WebInputEvent::RawKeyDown, message, wParam, lParam);
            bool handled = popupImpl->handleInputEvent(keyEvent);
        });
        if (bHandle)
            return 0;
        break;
    
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
        break;

    case WM_TIMER:
        hFocusWnd = ::GetForegroundWindow();
        m_lastFocusWnd = hFocusWnd;
        break;
    case WM_PMW_KILLFOCUS:
    case WM_KILLFOCUS: {
        postTaskToMainThread(FROM_HERE, [self, id] {
            if (!net::ActivatingObjCheck::inst()->isActivating(id))
                return;
            self->hide();
        });
    }
        break;
    case WM_INIT_MENU: {
        postTaskToMainThread(FROM_HERE, [self, id] {
            if (!net::ActivatingObjCheck::inst()->isActivating(id))
                return;
            self->initialize();
        });
    }
        break;
    case WM_MOUSEACTIVATE:
        lResult = MA_NOACTIVATE; // 这句很重要！
        break;
    default:
        lResult = ::DefWindowProc(hWnd, message, wParam, lParam);
    }

    return lResult;
}

void PopupMenuWin::beginMainFrame()
{
    if (!m_initialize)
        return;
    m_isCommiting = false;
    if (!m_allowBeginFrame)
        return;
    m_allowBeginFrame = false;
    
    if (m_layerTreeHost) {
        updataSize(false);
        updataMcLayerTreeHost();
    } else 
        updataSize(true);
}

void PopupMenuWin::updataMcLayerTreeHost()
{
    m_layerTreeHost->beginRecordActions(true);

    double lastFrameTimeMonotonic = WTF::currentTime();
    WebBeginFrameArgs frameTime(m_lastFrameTimeMonotonic, 0, lastFrameTimeMonotonic - m_lastFrameTimeMonotonic);
    m_popupImpl->beginFrame(frameTime);
    m_popupImpl->layout();
    m_lastFrameTimeMonotonic = lastFrameTimeMonotonic;

    if (0)
        m_layerTreeHost->showDebug();

    m_layerTreeHost->recordDraw();

    m_layerTreeHost->preDrawFrame();
    updataPaint();
    m_layerTreeHost->postDrawFrame();

    m_layerTreeHost->endRecordActions();
}

void PopupMenuWin::updataSize(bool needShow)
{
    if (!m_initialize || !m_hPopup)
        return;

    HWND hParentWnd = m_hParentWnd;
    HWND hPopup = m_hPopup;
    if (!m_needResize) {
        postTaskToUiThread(FROM_HERE, m_hParentWnd, [hPopup, needShow] {
            if (needShow)
                ::ShowWindow(hPopup, SW_SHOWNOACTIVATE);
        });
        return;
    }
    m_needResize = false;
    
    bool allowBeginFrame = m_allowBeginFrame;
    blink::IntSize size(m_rect.width(), m_rect.height());

    if (m_layerTreeHost)
        m_layerTreeHost->setViewportSize(size);
#ifndef NO_USE_ORIG_CHROME
    if (m_ccLayerTreeWrap)
        m_ccLayerTreeWrap->onHostResized(size.width(), size.height());
#endif

    m_popupImpl->resize(size);
    m_allowBeginFrame = allowBeginFrame;

    POINT pos = { m_rect.x() + m_offset.x(), m_rect.y() + m_offset.y() };
    
    int width = m_rect.width();
    int height = m_rect.height();
    postTaskToUiThread(FROM_HERE, m_hParentWnd, [pos, hPopup, hParentWnd, needShow, width, height] {
        //::ClientToScreen(hParentWnd, (LPPOINT)&pos);
        ::SetWindowPos(hPopup, HWND_TOP, pos.x, pos.y, width, height, /*SWP_SHOWWINDOW |*/ SWP_NOACTIVATE);
        if (needShow)
            ::ShowWindow(hPopup, SW_SHOWNOACTIVATE);
    });
}

void PopupMenuWin::show(WebNavigationPolicy)
{
    m_needResize = true;
    m_hasResize = true;
    m_isVisible = true;
    postCommit();
    
    updataSize(false);
}

void PopupMenuWin::hide()
{
    if (!m_initialize)
        return;
    m_isVisible = false;
    m_webViewImpl->hidePopups();
}

void PopupMenuWin::paint(HDC hdc, const RECT& rcPaint)
{
#ifndef NO_USE_ORIG_CHROME
    if (m_ccLayerTreeWrap)
        m_ccLayerTreeWrap->firePaintEvent(hdc, rcPaint);
#endif

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

    HDC hMemoryDC = skia::BeginPlatformPaint(m_hParentWnd, m_memoryCanvas);
    m_memoryCanvas->save();

    SkPaint paint;
    paint.setAntiAlias(true);
    m_memoryCanvas->drawPaint(paint);
    m_layerTreeHost->drawToCanvas(m_memoryCanvas, clip);
    m_memoryCanvas->restore();

    COLORREF color = ::GetPixel(hMemoryDC, 1, 1);

    bool canShow = false;
    if (m_hPopup && 0 != color) {
        canShow = true;

        HDC hdc = GetDC(m_hPopup);
        RECT rc = { 0, 0, m_rect.width(), m_rect.height() };
        skia::DrawToNativeContext(m_memoryCanvas, hdc, 0, 0, &rc);
        ::ReleaseDC(m_hPopup, hdc);
    }
    skia::EndPlatformPaint(m_memoryCanvas);

    if (m_isVisible && canShow) {
        updataSize(true);
    }
}

static void initWndStyle(HWND hPopup)
{
    // 这里会重入到WebPageImpl::fireKillFocusEvent
    // ::SetWindowPos(hPopup, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
    // ::SetFocus(hPopup);
    // ::SetCapture(hPopup);
    // ::SetForegroundWindow(hPopup);
    ::SetWindowPos(hPopup, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE);
    ::ShowWindow(hPopup, SW_HIDE);
}

static HHOOK g_hMouseHook = nullptr;

struct MY_MOUSEHOOKSTRUCTEX : public tagMOUSEHOOKSTRUCT {
    DWORD mouseData;
};

LRESULT CALLBACK PopupMenuWin::mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
    if (pMouseStruct == NULL || nullptr == m_hPopup || pMouseStruct->hwnd == m_hPopup)
        return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

    PopupMenuWin* self = (PopupMenuWin*)GetPropW(m_hPopup, kPopupWindowClassName);
    if (!self || !self->m_isVisible)
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

void PopupMenuWin::asynCreateWndOnMainThread()
{
    if (!m_initialize)
        return;

    m_client->onPopupMenuCreate(m_hPopup);

#ifndef NO_USE_ORIG_CHROME
    if (m_ccLayerTreeWrap)
        m_ccLayerTreeWrap->setHWND(m_hPopup);
#endif
}

void PopupMenuWin::asynCreateWndOnUiThread()
{
    WTF::Locker<RecursiveMutex> locker(g_popupMenuMutex);
    if (!m_initialize)
        return;

    if (!m_hPopup) {
        registerClass();

        m_hPopup = CreateWindowEx(WS_EX_NOACTIVATE, kPopupWindowClassName, L"MbPopupMenu", WS_POPUP/* | WS_DISABLED*/,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, /*m_hParentWnd*/nullptr, 0, 0, this);
        
        DWORD dwProcessID;
        DWORD dwThreadID;
        dwThreadID = ::GetWindowThreadProcessId(m_hParentWnd, &dwProcessID);

        //g_hMouseHook = ::SetWindowsHookEx(WH_MOUSE, mouseHookProc, GetModuleHandle(NULL), /*GetCurrentThreadId()*/dwThreadID);
    }

    int id = m_id;
    PopupMenuWin* self = this;
    postTaskToMainThread(FROM_HERE, [self, id] {
        if (!net::ActivatingObjCheck::inst()->isActivating(id))
            return;
        self->asynCreateWndOnMainThread();
    });

    initWndStyle(m_hPopup);

    m_lastFocusWnd = ::GetForegroundWindow();

    postCommit();
}

WebWidget* PopupMenuWin::createWnd()
{
    int id = m_id;
    PopupMenuWin* self = (PopupMenuWin*)this;
    if (!m_hPopup) {
        postTaskToUiThread(FROM_HERE, m_hParentWnd, [self] {
            self->asynCreateWndOnUiThread();
        });
    } else {
        HWND hPopup = m_hPopup;

        postTaskToUiThread(FROM_HERE, m_hParentWnd, [self, id, hPopup] {
            WTF::Locker<RecursiveMutex> locker(g_popupMenuMutex);
            if (!net::ActivatingObjCheck::inst()->isActivating(id))
                return;

            ::SetPropW(hPopup, kPopupWindowClassName, (HANDLE)self);
            ::SetWindowPos(hPopup, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE);
            ::ShowWindow(hPopup, SW_HIDE);

            postTaskToMainThread(FROM_HERE, [self, id, hPopup] {
                if (!net::ActivatingObjCheck::inst()->isActivating(id))
                    return;
#ifndef NO_USE_ORIG_CHROME
                if (self->m_ccLayerTreeWrap)
                    self->m_ccLayerTreeWrap->setHWND(hPopup);
#endif
                self->beginMainFrame();
            });
        });        
    }

    initialize();
    return m_popupImpl;
}

void PopupMenuWin::initialize()
{
    g_popupMenuMutex.lock();
    if (m_initialize) {
        g_popupMenuMutex.unlock();
        return;
    }
    m_initialize = true;
    g_popupMenuMutex.unlock();
    
#ifndef NO_USE_ORIG_CHROME
    if (OrigChromeMgr::getInst())
        m_ccLayerTreeWrap = new LayerTreeWrap(this, true);
    else
#endif
        m_layerTreeHost = new mc::LayerTreeHost(this, nullptr);

    m_popupImpl = WebPagePopup::create(this);
    m_platformEventHandler = new PlatformEventHandler(m_popupImpl, nullptr);

    if (m_layerTreeHost)
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
    m_allowBeginFrame = true;
    if (!m_isCommiting && m_hPopup) {
        m_isCommiting = true;

        int id = m_id;
        PopupMenuWin* self = (PopupMenuWin*)this;

        postTaskToMainThread(FROM_HERE, [self, id] {
            if (!net::ActivatingObjCheck::inst()->isActivating(id))
                return;
            self->beginMainFrame();
        });
    }
}

void PopupMenuWin::didInvalidateRect(const blink::WebRect& r)
{
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

void PopupMenuWin::initializeLayerTreeView()
{
#ifndef NO_USE_ORIG_CHROME
    if (m_ccLayerTreeWrap)
        m_ccLayerTreeWrap->initializeLayerTreeView();
#endif
}

WebLayerTreeView* PopupMenuWin::layerTreeView()
{
    //g_popupMenuIniting = false;
#ifndef NO_USE_ORIG_CHROME
    if (m_ccLayerTreeWrap)
        return m_ccLayerTreeWrap->layerTreeView();
#endif
    return m_layerTreeHost;
}

bool PopupMenuWin::initSetting()
{
    return true;
}

#ifndef NO_USE_ORIG_CHROME

void PopupMenuWin::onBeginPaint(HDC hdc, const RECT& damageRect)
{

}

bool PopupMenuWin::onEndPaintStep1(HDC hdc, const RECT& damageRect)
{
    return false;
}

void PopupMenuWin::onEndPaintStep2(HDC hdc, const RECT& damageRect)
{
    WTF::Locker<RecursiveMutex> locker(g_popupMenuMutex);
    if (!m_initialize)
        return;

    COLORREF color = ::GetPixel(hdc, 1, 1);

    bool canShow = false;
    if (m_hPopup && 0 != color) {
        canShow = true;

//         HBRUSH hbrush;
//         hbrush = ::CreateSolidBrush(rand());
//         ::SelectObject(hdc, hbrush);
//         ::Rectangle(hdc, 0, 0, damageRect.right - damageRect.left, damageRect.bottom - damageRect.top);
//         ::DeleteObject(hbrush);

        HDC hScreenDC = ::GetDC(m_hPopup);
        ::BitBlt(hScreenDC, 0, 0,
            damageRect.right - damageRect.left, damageRect.bottom - damageRect.top,
            hdc,
            damageRect.left, damageRect.top, SRCCOPY);

        ::ReleaseDC(m_hPopup, hScreenDC);
    }

    if (m_isVisible && canShow) {
        int id = m_id;
        PopupMenuWin* self = this;
        postTaskToMainThread(FROM_HERE, [self, id] {
            if (!net::ActivatingObjCheck::inst()->isActivating(id))
                return;
            self->updataSize(true);
        });
    }
}

void PopupMenuWin::onLayout()
{
    if (m_popupImpl)
        m_popupImpl->layout();
}

void PopupMenuWin::onBeginMainFrame()
{
    if (!m_webViewImpl)
        return;

    double lastFrameTimeMonotonic = WTF::monotonicallyIncreasingTime();

    WebBeginFrameArgs frameArgs(lastFrameTimeMonotonic, 0, lastFrameTimeMonotonic - m_lastFrameTimeMonotonic);
    m_popupImpl->beginFrame(frameArgs);
    m_popupImpl->layout();

    m_lastFrameTimeMonotonic = lastFrameTimeMonotonic;
}

COLORREF PopupMenuWin::getBackgroundColor()
{
    return 0xffffffff;
}

#endif

DEFINE_TRACE(PopupMenuWin)
{
}

} // content