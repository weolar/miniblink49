#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "include/cef_browser.h"
#include "include/cef_app.h"
#include "include/cef_client.h"

#include "libcef/browser/CefBrowserInfo.h"
#include "libcef/browser/CefBrowserHostImpl.h"
#include "libcef/browser/ThreadUtil.h"
#include "libcef/browser/CefContext.h"
#include "libcef/common/CefTaskImpl.h"
#include "libcef/common/GeometryUtil.h"
#include "content/browser/WebPage.h"
#include "third_party/WebKit/Source/wtf/Functional.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"

#if USING_VC6RT == 1
#define WM_XBUTTONDOWN 0x020B
#define WM_MOUSEWHEEL 0x020A
#endif

// static
void CefBrowserHostImpl::RegisterWindowClass() {
    static bool registered = false;
    if (registered)
        return;

    // Register the window class
    WNDCLASSEX wcex = {
        /* cbSize = */ sizeof(WNDCLASSEX),
        /* style = */ CS_HREDRAW | CS_VREDRAW,
        /* lpfnWndProc = */ CefBrowserHostImpl::WndProc,
        /* cbClsExtra = */ 0,
        /* cbWndExtra = */ 0,
        /* hInstance = */ ::GetModuleHandle(NULL),
        /* hIcon = */ NULL,
        /* hCursor = */ LoadCursor(NULL, IDC_ARROW),
        /* hbrBackground = */ (HBRUSH)(COLOR_WINDOW + 1),
        /* lpszMenuName = */ NULL,
        /* lpszClassName = */ CefBrowserHostImpl::GetWndClass(),
        /* hIconSm = */ NULL,
    };
    RegisterClassEx(&wcex);

    registered = true;
}

// static
LPCTSTR CefBrowserHostImpl::GetWndClass() {
    return L"MiniCefBrowserWindow";
}

bool CefBrowserHostImpl::CreateHostWindow(const CefWindowInfo& windowInfo) {
    ASSERT(!m_webPage);
    m_webPage = new content::WebPage(nullptr);
    HWND hWnd = nullptr;
    if (!IsWindowless()) {
        RegisterWindowClass();
        hWnd = ::CreateWindowW(GetWndClass(), L"mini cef", windowInfo.style,
            windowInfo.x, windowInfo.y, windowInfo.width, windowInfo.height,
            windowInfo.parent_window, NULL, ::GetModuleHandle(NULL), this);
        if (!hWnd)
            return false;

        ::UpdateWindow(hWnd);
        ::SetTimer(hWnd, (UINT_PTR)this, 50, nullptr);
    } else {
        hWnd = windowInfo.parent_window;
        m_webPage->init(hWnd);

        int width = windowInfo.width;
        int height = windowInfo.height;
        if (0 == width * height && m_client || m_client->GetRenderHandler().get()) {
            CefRefPtr<CefRenderHandler> render = m_client->GetRenderHandler();
            CefRect r;
            if (render->GetScreenInfo(this, m_screenInfo)) {
                width = cef::LogicalToDevice(m_screenInfo.rect.width, m_screenInfo.device_scale_factor);
                height = cef::LogicalToDevice(m_screenInfo.rect.height, m_screenInfo.device_scale_factor);
            }
        }
        m_webPage->fireResizeEvent(nullptr, WM_SIZE, 0, MAKELPARAM(width, height));

        ::SetPropW(hWnd, L"CefBrowserHostImpl", this);
        m_lpfnOldWndProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)SubClassFunc);
    }
    m_webPage->setBrowser(this);
    return true;
}

// windowlessÄ£Ê½ÓÃ
LONG CefBrowserHostImpl::SubClassFunc(HWND hWnd, UINT Message, WPARAM wParam, LONG lParam) {
    CefBrowserHostImpl* self = (CefBrowserHostImpl*)::GetPropW(hWnd, L"CefBrowserHostImpl");

    if (Message == WM_CLOSE) {
        self->WindowDestroyed();
        if (!::IsWindow(hWnd))
            return 0;
    }

    return ::CallWindowProcW(self->m_lpfnOldWndProc, hWnd, Message, wParam, lParam);
}

bool CefBrowserHostImpl::FireHeartbeat() {
    return m_webPage->fireTimerEvent();
}

void CefBrowserHostImpl::SetNeedHeartbeat() {
    CefContext::Get()->SetNeedHeartbeat();
}

void CefBrowserHostImpl::ClearNeedHeartbeat() {
    CefContext::Get()->ClearNeedHeartbeat();
}

LRESULT CALLBACK CefBrowserHostImpl::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    CefBrowserHostImpl* self = nullptr;
    if (message == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        self = static_cast<CefBrowserHostImpl*>(lpcs->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(self));

        self->m_webPage->init(hWnd);
    } else {
        self = reinterpret_cast<CefBrowserHostImpl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        // if( message == WM_NCDESTROY && webPage != NULL )
        //    return 0;
    }
    if (!self)
        return ::DefWindowProc(hWnd, message, wParam, lParam);

    BOOL bHandle = TRUE;
    LRESULT lResult = 0;

    self->m_webPage->fireTimerEvent();

    switch (message) {
    case WM_ERASEBKGND:
        bHandle = TRUE;
        break;
    case WM_PAINT:
        bHandle = TRUE;
        self->m_webPage->firePaintEvent(hWnd, message, wParam, lParam);
        break;

    case WM_TIMER:
        self->m_webPage->fireTimerEvent();
        break;
    //case WM_SIZING:
    case WM_SIZE:
        self->m_webPage->fireResizeEvent(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_XBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_MOUSELEAVE:
        self->m_webPage->fireMouseEvent(hWnd, message, wParam, lParam, nullptr);
        break;
    case WM_SETCURSOR:
        lResult = self->m_webPage->fireCursorEvent(hWnd, message, wParam, lParam, &bHandle);
        if (bHandle)
            return lResult;
        break;
    case WM_MOUSEWHEEL:
        lResult = self->m_webPage->fireWheelEvent(hWnd, message, wParam, lParam);
        break;
    case WM_KEYDOWN:
        bHandle = self->m_webPage->fireKeyDownEvent(hWnd, message, wParam, lParam);
        break;
    case WM_KEYUP:
        bHandle = self->m_webPage->fireKeyUpEvent(hWnd, message, wParam, lParam);
        break;
    case WM_CHAR:
        bHandle = self->m_webPage->fireKeyPressEvent(hWnd, message, wParam, lParam);
        break;
    case WM_SETFOCUS:
        self->SendFocusEvent(true);
        break;
    case WM_KILLFOCUS:
        self->SendFocusEvent(false);
        break;
    case WM_IME_STARTCOMPOSITION:{
        if (!self->m_webPage)
            return 0;
        blink::IntRect caret = self->m_webPage->caretRect();
        COMPOSITIONFORM COMPOSITIONFORM;
        COMPOSITIONFORM.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
        COMPOSITIONFORM.ptCurrentPos.x = caret.x();
        COMPOSITIONFORM.ptCurrentPos.y = caret.y();

        HIMC hIMC = ::ImmGetContext(hWnd);
        ::ImmSetCompositionWindow(hIMC, &COMPOSITIONFORM);
        ::ImmReleaseContext(hWnd, hIMC);
        return 0;
    }
        break;
    case WM_NCDESTROY:
        // Clear the user data pointer.
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
        // Force the browser to be destroyed. This will result in a call to
        // BrowserDestroyed() that will release the reference added in
        // CreateHostWindow().
        bHandle = TRUE;
        self->WindowDestroyed();
        break;
    default:
        bHandle = FALSE;
        break;
    }

    if (!bHandle)
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    return lResult;
}

void CefBrowserHostImpl::CancelContextMenu() {
}

void CefBrowserHostImpl::SendKeyEvent(const CefKeyEvent& evt) {
    if (!m_webPage)
        return;

    if (KEYEVENT_KEYDOWN == evt.type)
        m_webPage->fireKeyDownEvent(m_webPage->getHWND(), WM_KEYDOWN, evt.windows_key_code, evt.native_key_code);
    else if (KEYEVENT_KEYUP == evt.type)
        m_webPage->fireKeyUpEvent(m_webPage->getHWND(), WM_KEYUP, evt.windows_key_code, evt.native_key_code);
    else if (KEYEVENT_CHAR == evt.type)
        m_webPage->fireKeyPressEvent(m_webPage->getHWND(), WM_CHAR, evt.windows_key_code, evt.native_key_code);
}

void CefBrowserHostImpl::SendMouseClickEvent(const CefMouseEvent& evt, MouseButtonType type, bool mouseUp, int clickCount) {
    UINT message;
    if (!mouseUp) {
        if (MBT_LEFT == type) {
            m_hasLMouseUp = false;
            message = WM_LBUTTONDOWN;
        } else if (MBT_MIDDLE == type) {
            message = WM_MBUTTONDOWN;
        } else if (MBT_RIGHT == type) {
            m_hasRMouseUp = false;
            message = WM_RBUTTONDOWN;
        }
    } else {
        if (MBT_LEFT == type) {
            m_hasLMouseUp = true;
            message = WM_LBUTTONUP;
        } else if (MBT_MIDDLE == type) {
            message = WM_MBUTTONUP;
        } else if (MBT_RIGHT == type) {
            m_hasRMouseUp = true;
            message = WM_RBUTTONUP;
        }
    }

    if (!m_webPage)
        return;

    BOOL bHandle = FALSE;

    int x = cef::LogicalToDevice(evt.x, m_screenInfo.device_scale_factor);
    int y = cef::LogicalToDevice(evt.y, m_screenInfo.device_scale_factor);
    m_webPage->fireMouseEvent(m_webPage->getHWND(), message, 0, MAKELPARAM(x, y), &bHandle);
}

void CefBrowserHostImpl::SendMouseMoveEvent(const CefMouseEvent& evt, bool mouseLeave) {
    if (!m_webPage)
        return;

    BOOL bHandle = FALSE;

    WPARAM wParam = 0;
    if (!m_hasLMouseUp)
        wParam |= MK_LBUTTON;
    if (!m_hasRMouseUp)
        wParam |= MK_RBUTTON;

    int x = cef::LogicalToDevice(evt.x, m_screenInfo.device_scale_factor);
    int y = cef::LogicalToDevice(evt.y, m_screenInfo.device_scale_factor);

    m_webPage->fireMouseEvent(m_webPage->getHWND(), WM_MOUSEMOVE, wParam, MAKELPARAM(x, y), &bHandle);
}

void CefBrowserHostImpl::SendMouseWheelEvent(const CefMouseEvent& evt, int deltaX, int deltaY) {
    if (!m_webPage)
        return;

    WPARAM wParam = 0;
    if (evt.modifiers & EVENTFLAG_CONTROL_DOWN)
        wParam |= MK_CONTROL;

    if (evt.modifiers & EVENTFLAG_SHIFT_DOWN)
        wParam |= MK_SHIFT;

    int x = cef::LogicalToDevice(evt.x, m_screenInfo.device_scale_factor);
    int y = cef::LogicalToDevice(evt.y, m_screenInfo.device_scale_factor);

    int deviceDeltaY = cef::LogicalToDevice(deltaY, m_screenInfo.device_scale_factor);
     
    m_webPage->fireWheelEvent(m_webPage->getHWND(), WM_MOUSEWHEEL, MAKEWPARAM(wParam, deviceDeltaY), MAKELPARAM(x, y));
}

void CefBrowserHostImpl::SendCaptureLostEvent() {
}

void CefBrowserHostImpl::SendFocusEvent(bool setFocus) {
    if (!CEF_CURRENTLY_ON_UIT()) {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefBrowserHostImpl::SendFocusEvent, this, setFocus));
        return;
    }

    if (!m_webPage || !m_webPage->webViewImpl())
        return;

    if (!setFocus)
        CancelContextMenu();

    if (setFocus)
        m_webPage->fireSetFocusEvent(m_webPage->getHWND(), WM_SETFOCUS, 0, 0);
    else
        m_webPage->fireKillFocusEvent(m_webPage->getHWND(), WM_KILLFOCUS, 0, 0);
}

void CefBrowserHostImpl::PlatformSetFocus(bool focus) {
    if (m_webPage && m_webPage->webViewImpl()) {
        // Give logical focus to the RenderWidgetHostViewAura in the views
        // hierarchy. This does not change the native keyboard focus.
        if (focus)
            m_webPage->fireSetFocusEvent(m_webPage->getHWND(), WM_SETFOCUS, 0, 0);
        else
            m_webPage->fireKillFocusEvent(m_webPage->getHWND(), WM_KILLFOCUS, 0, 0);
    }

    if (!focus)
        return;

    if (m_webPage->getHWND())
        ::SetFocus(m_webPage->getHWND());
}
#endif
