#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#define BUILDING_wke

#include "wkeWebWindow.h"

extern DWORD g_paintCount;
extern bool g_isSetDragEnable;

////////////////////////////////////////////////////////////////////////////

namespace wke {

CWebWindow::CWebWindow()
{
    m_state = kWkeWebWindowUninit;

    m_originalPaintUpdatedCallback = NULL;
    m_originalPaintUpdatedCallbackParam = NULL;

    m_originalDocumentReadyCallback = NULL;
    m_originalDocumentReadyCallbackParam = NULL;

    m_originalLoadingFinishCallback = NULL;
    m_originalLoadingFinishCallbackParam = NULL;

    _initCallbacks();
}

CWebWindow::~CWebWindow()
{
    destroy();
}

bool CWebWindow::create(HWND parent, unsigned styles, unsigned styleEx, int x, int y, int width, int height)
{
    CWebView::create();
    return _createWindow(parent, styles, styleEx, x, y, width, height);
}

bool CWebWindow::create(HWND parent, wkeWindowType type, int x, int y, int width, int height)
{
    unsigned styles = 0;
    unsigned styleEx = 0;
    switch (type) {
    case WKE_WINDOW_TYPE_CONTROL:
        styles = WS_CHILD;
        styleEx = 0;
        wkeSetTransparent(this, false);
        break;

    case WKE_WINDOW_TYPE_TRANSPARENT:
        styles = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        styleEx = WS_EX_LAYERED;
        wkeSetTransparent(this, true);
        break;

    case WKE_WINDOW_TYPE_POPUP:
    default:
        styles = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        styleEx = 0;
        wkeSetTransparent(this, false);
    }

    return create(parent, styles, styleEx, x, y, width, height);
}

void CWebWindow::destroy()
{
    _destroyWindow();
}

void CWebWindow::onPaintUpdated(wkePaintUpdatedCallback callback, void* callbackParam)
{
    m_originalPaintUpdatedCallback = callback;
    m_originalPaintUpdatedCallbackParam = callbackParam;
}

void CWebWindow::onLoadingFinish(wkeLoadingFinishCallback callback, void* callbackParam)
{
    m_originalLoadingFinishCallback = callback;
    m_originalLoadingFinishCallbackParam = callbackParam;
}

void CWebWindow::onDocumentReady(wkeDocumentReadyCallback callback, void* callbackParam)
{
    m_originalDocumentReadyCallback = callback;
    m_originalDocumentReadyCallbackParam = callbackParam;
}

bool CWebWindow::_createWindow(HWND parent, unsigned styles, unsigned styleEx, int x, int y, int width, int height)
{
    if (IsWindow(m_hWnd))
        return true;

    const wchar_t* szClassName = L"wkeWebWindow";
    MSG msg = { 0 };
    WNDCLASSW wndClass = { 0 };
    if (!GetClassInfoW(NULL, szClassName, &wndClass)) {
        wndClass.style        = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc  = &CWebWindow::_staticWindowProc;
        wndClass.cbClsExtra   = 200;
        wndClass.cbWndExtra   = 200;
        wndClass.hInstance    = GetModuleHandleW(NULL);
        wndClass.hIcon        = LoadIcon(NULL, IDI_APPLICATION);
        wndClass.hCursor      = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground= NULL;
        wndClass.lpszMenuName  = NULL;
        wndClass.lpszClassName = szClassName;
        RegisterClassW(&wndClass);
    }

    //DWORD styleEx = 0;
    //DWORD styles = 0;

    //if (WKE_WINDOW_STYLE_LAYERED == (styleFlags & WKE_WINDOW_STYLE_LAYERED))
    //    styleEx = WS_EX_LAYERED;

    //if (WKE_WINDOW_STYLE_CHILD == (styleFlags & WKE_WINDOW_STYLE_CHILD))
    //    styles |= WS_CHILD;
    //else
    //    styles |= WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME;

    //if (WKE_WINDOW_STYLE_BORDER == (styleFlags & WKE_WINDOW_STYLE_BORDER))
    //    styles |= WS_BORDER;

    //if (WKE_WINDOW_STYLE_CAPTION == (styleFlags & WKE_WINDOW_STYLE_CAPTION))
    //    styles |= WS_CAPTION;

    //if (WKE_WINDOW_STYLE_SIZEBOX == (styleFlags & WKE_WINDOW_STYLE_SIZEBOX))
    //    styles |= WS_SIZEBOX;

    //if (WKE_WINDOW_STYLE_SHOWLOADING == (styleFlags & WKE_WINDOW_STYLE_SHOWLOADING))
    //    styles |=  WS_VISIBLE;

    m_hWnd = CreateWindowExW(
        styleEx,        // window ex-style
        szClassName,    // window class name
        L"wkeWebWindow", // window caption
        styles,         // window style
        x,              // initial x position
        y,              // initial y position
        width,          // initial x size
        height,         // initial y size
        parent,         // parent window handle
        NULL,           // window menu handle
        GetModuleHandleW(NULL),           // program instance handle
        this);         // creation parameters

    if (!IsWindow(m_hWnd))
        return FALSE;

    CWebView::resize(width, height);
    return TRUE;
}

void CWebWindow::_destroyWindow()
{
    if (kWkeWebWindowDestroing == m_state)
        return;
    m_state = kWkeWebWindowDestroing;

    ::KillTimer(m_hWnd, (UINT_PTR)this);
    ::DestroyWindow(m_hWnd); // 这里会重入到本函数
}

void CWebWindow::_initCallbacks()
{
    CWebView::onPaintUpdated(&CWebWindow::_staticOnPaintUpdated, this);
    CWebView::onLoadingFinish(&CWebWindow::_staticOnLoadingFinish, this);
    CWebView::onDocumentReady(&CWebWindow::_staticOnDocumentReady, this);
}

LRESULT CALLBACK CWebWindow::_staticWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CWebWindow* pthis = (CWebWindow*)GetPropW(hwnd, L"wkeWebWindow");
    if (!pthis) {
        if (message == WM_CREATE) {
            LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
            pthis = (CWebWindow*)cs->lpCreateParams;
            ((CWebWindow*)cs->lpCreateParams)->setHandle(hwnd);
            pthis->m_state = kWkeWebWindowInit;
            ::SetPropW(hwnd, L"wkeWebWindow", (HANDLE)pthis);
        }
    }

    if (pthis)
        return pthis->_windowProc(hwnd, message, wParam, lParam);
    else
        return ::DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CWebWindow::_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
    case WM_NCPAINT:
        break;

    case WM_ERASEBKGND:
        break;

    case WM_CREATE:
        ::DragAcceptFiles(hwnd, TRUE);
        SetTimer(hwnd, (UINT_PTR)this, 20, NULL);
        return 0;

    case WM_CLOSE:
        if (getWkeHandler()->windowClosingCallback) {
            if (!getWkeHandler()->windowClosingCallback(this, getWkeHandler()->windowClosingCallbackParam))
                return 0;
        }

        ::ShowWindow(hwnd, SW_HIDE);
        ::DestroyWindow(hwnd);
        return 0;

    case WM_NCDESTROY:
        ::KillTimer(hwnd, (UINT_PTR)this);
        ::RemovePropW(hwnd, L"wkeWebWindow");
        m_hWnd = NULL;
        wkeDestroyWebView(this);
        return 0;

    case WM_TIMER:
        wkeRepaintIfNeeded(this);
        return 0;

    case WM_PAINT:
        if (WS_EX_LAYERED != (WS_EX_LAYERED & GetWindowLong(hwnd, GWL_EXSTYLE))) {
            wkeRepaintIfNeeded(this);

            PAINTSTRUCT ps = { 0 };
            HDC hdc = ::BeginPaint(hwnd, &ps);

            RECT rcClip = ps.rcPaint;

            RECT rcClient;
            ::GetClientRect(hwnd, &rcClient);

            RECT rcInvalid = rcClient;
            if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
                ::IntersectRect(&rcInvalid, &rcClip, &rcClient);

            int srcX = rcInvalid.left - rcClient.left;
            int srcY = rcInvalid.top - rcClient.top;
            int destX = rcInvalid.left;
            int destY = rcInvalid.top;
            int width = rcInvalid.right - rcInvalid.left;
            int height = rcInvalid.bottom - rcInvalid.top;

            if (0 != width && 0 != height)
                ::BitBlt(hdc, destX, destY, width, height, wkeGetViewDC(this), srcX, srcY, SRCCOPY);

            ::EndPaint(hwnd, &ps);
        }
        break;

    case WM_SIZE: {
        RECT rc = { 0 };
        ::GetClientRect(hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        CWebView::resize(width, height);
        wkeRepaintIfNeeded(this);

        return 0;
    }
    case WM_DROPFILES:
        if (g_isSetDragEnable) {
            Vector<wchar_t> szFile;
            szFile.resize(2 * MAX_PATH);
            memset(szFile.data(), 0, sizeof(wchar_t) * 2 * (MAX_PATH));
        
            wcscpy(szFile.data(), L"file:///");

            HDROP hDrop = reinterpret_cast<HDROP>(wParam);

            UINT uFilesCount = ::DragQueryFileW(hDrop, 0xFFFFFFFF, szFile.data(), MAX_PATH);
            if (uFilesCount != 0) {
                UINT uRet = ::DragQueryFileW(hDrop, 0, (wchar_t*)szFile.data() + 8, MAX_PATH);
                if (uRet != 0) {
                    wkeLoadURLW(this, szFile.data());
                    ::SetWindowTextW(hwnd, szFile.data());
                }
            }
            ::DragFinish(hDrop);
        }
        return 0;
    //case WM_NCHITTEST:
    //    if (IsWindow(m_hWnd) && flagsOff(GetWindowLong(m_hWnd, GWL_STYLE), WS_CAPTION))
    //    {
    //        IWebkitObserverPtr observer = m_observer.lock();
    //        if (!observer)
    //            break;

    //        QPoint cursor(LOWORD(lParam), HIWORD(lParam));
    //        ScreenToClient(m_hWnd, &cursor);

    //        QRect clientRect;
    //        QSize clientSize;
    //        GetClientRect(hwnd, &clientRect);
    //        clientSize.cx = clientRect.width();
    //        clientSize.cy = clientRect.height();

    //        EWebkitHitTest hit = observer->onHitTest(QWebkitView(thisView), clientSize, cursor);
    //        switch (hit)
    //        {
    //        case eWebkitHitLeftTop:     return HTTOPLEFT;
    //        case eWebkitHitLeft:        return HTLEFT;
    //        case eWebkitHitLeftBottom:  return HTBOTTOMLEFT;
    //        case eWebkitHitRightTop:    return HTTOPRIGHT;
    //        case eWebkitHitRight:       return HTRIGHT;
    //        case eWebkitHitRightBottom: return HTBOTTOMRIGHT;
    //        case eWebkitHitTop:         return HTTOP;
    //        case eWebkitHitBottom:      return HTBOTTOM;
    //        case eWebkitHitCaption:     return HTCAPTION;
    //        case eWebkitHitClient:      return HTCLIENT;
    //        case eWebkitHitNone:        return HTCLIENT;
    //        }
    //    }
    //    break;

    //case WM_SETCURSOR:
    //    if (IsWindow(m_hWnd) && flagsOff(GetWindowLong(m_hWnd, GWL_STYLE), WS_CAPTION))
    //    {
    //        WORD hit = LOWORD(lParam);
    //        switch (hit)
    //        {
    //        case HTCAPTION:
    //        case HTSYSMENU:
    //        case HTMENU:
    //        case HTCLIENT:
    //            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
    //            return TRUE;

    //        case HTTOP:
    //        case HTBOTTOM:
    //            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
    //            return TRUE;

    //        case HTLEFT:
    //        case HTRIGHT:
    //            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
    //            return TRUE;

    //        case HTTOPLEFT:
    //        case HTBOTTOMRIGHT:
    //            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
    //            return TRUE;

    //        case HTTOPRIGHT:
    //        case HTBOTTOMLEFT:
    //            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
    //            return TRUE;

    //        default:
    //            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
    //            return TRUE;
    //        }
    //    }
    //    break;

    //case WM_NCLBUTTONDOWN:
    //    if (IsWindow(m_hWnd) && flagsOff(GetWindowLong(m_hWnd, GWL_STYLE), WS_CAPTION))
    //    {
    //        int hit = wParam;
    //        switch (hit)
    //        {
    //        case HTTOP:         SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOP, lParam); return 0;
    //        case HTBOTTOM:      SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOM, lParam); return 0;
    //        case HTLEFT:        SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_LEFT, lParam); return 0;
    //        case HTRIGHT:       SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_RIGHT, lParam); return 0;
    //        case HTTOPLEFT:     SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPLEFT, lParam); return 0;
    //        case HTTOPRIGHT:    SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPRIGHT, lParam); return 0;
    //        case HTBOTTOMLEFT:  SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMLEFT, lParam); return 0;
    //        case HTBOTTOMRIGHT: SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMRIGHT, lParam); return 0;
    //        case HTCAPTION:     SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_MOVE|4, lParam); return 0;
    //        }
    //    }
    //    break;

    //case WM_NCLBUTTONDBLCLK:
    //    if (IsWindow(m_hWnd) && flagsOff(GetWindowLong(m_hWnd, GWL_STYLE), WS_CAPTION))
    //    {
    //        int hit = wParam;
    //        if (hit == HTCAPTION)
    //        {
    //            if (IsZoomed(m_hWnd))
    //                SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, lParam);
    //            else
    //                SendMessageW(m_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, lParam);
    //            return 0;
    //        }
    //    }
    //    break;

    case WM_KEYDOWN: {
        unsigned int virtualKeyCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= WKE_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= WKE_EXTENDED;

        if (wkeFireKeyDownEvent(this, virtualKeyCode, flags, false))
            return 0;
        break;
    }
    case WM_KEYUP: {
        unsigned int virtualKeyCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= WKE_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= WKE_EXTENDED;

        if (wkeFireKeyUpEvent(this, virtualKeyCode, flags, false))
            return 0;
        break;
    }
    case WM_CHAR: {
        unsigned int charCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= WKE_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= WKE_EXTENDED;

        if (wkeFireKeyPressEvent(this, charCode, flags, false))
            return 0;
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE: {
        if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN) {
            if (::GetFocus() != hwnd)
                ::SetFocus(hwnd);
            ::SetCapture(hwnd);
        }
        else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP) {
            ReleaseCapture();
        }

        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= WKE_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= WKE_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= WKE_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= WKE_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= WKE_RBUTTON;

        if (wkeFireMouseEvent(this, message, x, y, flags))
            return 0;
        break;
    }
    case WM_CONTEXTMENU: {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        if (pt.x != -1 && pt.y != -1)
            ScreenToClient(hwnd, &pt);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= WKE_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= WKE_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= WKE_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= WKE_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= WKE_RBUTTON;

        if (wkeFireContextMenuEvent(this, pt.x, pt.y, flags))
            return 0;
        break;
    }
    case WM_MOUSEWHEEL: {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        ::ScreenToClient(hwnd, &pt);

        int delta = GET_WHEEL_DELTA_WPARAM(wParam);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= WKE_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= WKE_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= WKE_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= WKE_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= WKE_RBUTTON;

        if (wkeFireMouseWheelEvent(this, pt.x, pt.y, delta, flags))
            return 0;
        break;
    }
    case WM_SETFOCUS:
        wkeSetFocus(this);
        return 0;

    case WM_KILLFOCUS:
        wkeKillFocus(this);
        return 0;

    case WM_SETCURSOR:
        if (wkeFireWindowsMessage(this, hwnd, WM_SETCURSOR, 0, 0, nullptr))
            return 0;
        break;

    case WM_IME_STARTCOMPOSITION: {
        wkeRect caret = wkeGetCaretRect(this);

        COMPOSITIONFORM COMPOSITIONFORM;
        COMPOSITIONFORM.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
        COMPOSITIONFORM.ptCurrentPos.x = caret.x;
        COMPOSITIONFORM.ptCurrentPos.y = caret.y;

        HIMC hIMC = ::ImmGetContext(hwnd);
        ::ImmSetCompositionWindow(hIMC, &COMPOSITIONFORM);
        ::ImmReleaseContext(hwnd, hIMC);
    }
        return 0;
    }

    return ::DefWindowProcW(hwnd, message, wParam, lParam);
}

void CWebWindow::_staticOnPaintUpdated(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
    CWebWindow* pthis = (CWebWindow*)param;
    pthis->_onPaintUpdated(hdc, x, y, cx, cy);
}

void CWebWindow::_onPaintUpdated(const HDC hdc, int x, int y, int cx, int cy)
{
//     BOOL callOk = FALSE;
//     if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(m_hWnd, GWL_EXSTYLE))) {
//         RECT rectDest;
//         ::GetWindowRect(m_hWnd, &rectDest);
// 
//         SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
//         POINT pointDest = { 0, 0 }; // { rectDest.left, rectDest.top };
//         POINT pointSource = { 0, 0 };
// 
//         BITMAP bmp = { 0 };
//         HBITMAP hBmp = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
//         ::GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);
// 
//         sizeDest.cx = bmp.bmWidth;
//         sizeDest.cy = bmp.bmHeight;
// 
//         HDC hdcScreen = GetDC(m_hWnd);
// 
//         BLENDFUNCTION blend = { 0 };
//         blend.BlendOp = AC_SRC_OVER;
//         blend.SourceConstantAlpha = 255;
//         blend.AlphaFormat = AC_SRC_ALPHA;
//         callOk = ::UpdateLayeredWindow(m_hWnd, hdcScreen, nullptr, &sizeDest, hdc, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
//         if (!callOk) {
//             HDC hdcMemory = ::CreateCompatibleDC(hdcScreen);
//             HBITMAP hbmpMemory = ::CreateCompatibleBitmap(hdcScreen, sizeDest.cx, sizeDest.cy);
//             HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdcMemory, hbmpMemory);
// 
//             ::BitBlt(hdcMemory, 0, 0, sizeDest.cx, sizeDest.cy, hdc, 0, 0, SRCCOPY | CAPTUREBLT);
// 
//             ::BitBlt(hdc, 0, 0, sizeDest.cx, sizeDest.cy, hdcMemory, 0, 0, SRCCOPY | CAPTUREBLT); //!
// 
//             callOk = ::UpdateLayeredWindow(m_hWnd, hdcScreen, nullptr, &sizeDest, hdcMemory, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
// 
//             ::SelectObject(hdcMemory, (HGDIOBJ)hbmpOld);
//             ::DeleteObject((HGDIOBJ)hbmpMemory);
//             ::DeleteDC(hdcMemory);
//         }
// 
//         ::ReleaseDC(m_hWnd, hdcScreen);
//     } else {
//         RECT rc = {x, y, x + cx, y + cy};
//         callOk = ::InvalidateRect(m_hWnd, &rc, TRUE);
//     }

    if (m_originalPaintUpdatedCallback)
        m_originalPaintUpdatedCallback(this, m_originalPaintUpdatedCallbackParam, hdc, x, y, cx, cy);
}

void CWebWindow::_staticOnLoadingFinish(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{
    CWebWindow* pthis = (CWebWindow*)param;
    pthis->_onLoadingFinish(url, result, failedReason);
}

void CWebWindow::_onLoadingFinish(const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{
    if (m_originalLoadingFinishCallback)
        m_originalLoadingFinishCallback(this, m_originalLoadingFinishCallbackParam, url, result, failedReason);
}

void CWebWindow::_staticOnDocumentReady(wkeWebView webView, void* param)
{
    CWebWindow* pthis = (CWebWindow*)param;
    pthis->_onDocumentReady();
}

void CWebWindow::_onDocumentReady()
{
    if (m_originalDocumentReadyCallback)
        m_originalDocumentReadyCallback(this, m_originalDocumentReadyCallbackParam);
}

void CWebWindow::onClosing(wkeWindowClosingCallback callback, void* param)
{
    getWkeHandler()->windowClosingCallback = callback;
    getWkeHandler()->windowClosingCallbackParam = param;
}

void CWebWindow::onDestroy(wkeWindowDestroyCallback callback, void* param)
{
    getWkeHandler()->windowDestroyCallback = callback;
    getWkeHandler()->windowDestroyCallbackParam = param;
}

void CWebWindow::show(bool b)
{
    ::ShowWindow(m_hWnd, b ? SW_SHOW : SW_HIDE);
}

void CWebWindow::enable(bool b)
{
    ::EnableWindow(m_hWnd, b ? TRUE : FALSE);
}

void CWebWindow::move(int x, int y, int width, int height)
{
    ::MoveWindow(m_hWnd, x, y, width, height, FALSE);
}

void CWebWindow::resize(int width, int height)
{
    if (0 >= width || 0 >= height)
        return;

    POINT point = { 0 };
    
    RECT rect = { 0 };
    ::GetWindowRect(m_hWnd, &rect);
    point.x = rect.left;
    point.y = rect.top;
    
    DWORD style = ::GetWindowLong(m_hWnd, GWL_STYLE);
    style &= WS_CHILD;
    if (0 != style) {
        HWND parent = ::GetParent(m_hWnd);
        ::ScreenToClient(parent, &point);
    }

    ::MoveWindow(m_hWnd, point.x, point.y, width, height, FALSE);
    //::SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);

    CWebView::resize(width, height);
}

void CWebWindow::moveToCenter()
{
    int width = 0;
    int height = 0; {
        RECT rect = { 0 };
        GetWindowRect(m_hWnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }

    int parentWidth = 0;
    int parentHeight = 0;
    if (WS_CHILD == ::GetWindowLong(m_hWnd, GWL_STYLE)) {
        HWND parent = ::GetParent(m_hWnd);
        RECT rect = { 0 };
        ::GetClientRect(parent, &rect);
        parentWidth = rect.right - rect.left;
        parentHeight = rect.bottom - rect.top;
    } else {
        parentWidth = ::GetSystemMetrics(SM_CXSCREEN);
        parentHeight = ::GetSystemMetrics(SM_CYSCREEN);
    }

    int x = (parentWidth - width) / 2;
    int y = (parentHeight - height) / 2;

    ::MoveWindow(m_hWnd, x, y, width, height, FALSE);
}

void CWebWindow::setTitle(const wchar_t* text)
{
    ::SetWindowTextW(m_hWnd, text);
}

void CWebWindow::setTitle(const utf8* text)
{
    wchar_t wtext[1024 * 64 + 1] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, text, strlen(text), wtext, 1024*64);
    setTitle(wtext);
}

void CWebWindow::setTransparent(bool transparent)
{
    DWORD style = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
    SetWindowLongW(m_hWnd, GWL_EXSTYLE, (!transparent) ? (~WS_EX_LAYERED & style) : (WS_EX_LAYERED | style));
    ::UpdateWindow(m_hWnd);

    CWebView::setTransparent(transparent);
}

} // namespace wke

////////////////////////////////////////////////////////////////////////////

#endif
