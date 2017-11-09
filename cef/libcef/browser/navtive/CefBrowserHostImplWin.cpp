#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "include/cef_browser.h"
#include "include/cef_app.h"
#include "include/cef_client.h"

#include "libcef/browser/CefBrowserInfo.h"
#include "libcef/browser/CefBrowserHostImpl.h"
#include "libcef/browser/ThreadUtil.h"
#include "libcef/browser/CefContext.h"
#include "libcef/common/CefTaskImpl.h"
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
    if (IsWindowless())
        return true;
    RegisterWindowClass();

    m_webPage = new content::WebPage(nullptr);

    HWND hWnd = ::CreateWindowW(GetWndClass(), L"mini cef", windowInfo.style,
        windowInfo.x, windowInfo.y, windowInfo.width, windowInfo.height,
        windowInfo.parent_window, NULL, ::GetModuleHandle(NULL), this);
    if (!hWnd)
        return false;

    ::UpdateWindow(hWnd);
    ::SetTimer(hWnd, (UINT_PTR)this, 50, nullptr);

    m_webPage->setBrowser(this);
    return true;
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
    CefBrowserHostImpl* browserHostImpl = nullptr;
    if (message == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        browserHostImpl = static_cast<CefBrowserHostImpl*>(lpcs->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(browserHostImpl));

        browserHostImpl->m_webPage->init(hWnd);
    } else {
        browserHostImpl = reinterpret_cast<CefBrowserHostImpl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        // if( message == WM_NCDESTROY && webPage != NULL )
        //    return 0;
    }
    if (!browserHostImpl)
        return ::DefWindowProc(hWnd, message, wParam, lParam);

    BOOL bHandle = TRUE;
    LRESULT lResult = 0;

    browserHostImpl->m_webPage->fireTimerEvent();

    switch (message) {
    case WM_ERASEBKGND:
        bHandle = TRUE;
        break;
    case WM_PAINT:
        bHandle = TRUE;
        browserHostImpl->m_webPage->firePaintEvent(hWnd, message, wParam, lParam);
        break;

    case WM_TIMER:
        browserHostImpl->m_webPage->fireTimerEvent();
        break;
    //case WM_SIZING:
    case WM_SIZE:
        browserHostImpl->m_webPage->fireResizeEvent(hWnd, message, wParam, lParam);
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
        browserHostImpl->m_webPage->fireMouseEvent(hWnd, message, wParam, lParam, nullptr);
        break;
    case WM_SETCURSOR:
        lResult = browserHostImpl->m_webPage->fireCursorEvent(hWnd, message, wParam, lParam, &bHandle);
        if (bHandle)
            return lResult;
        break;
    case WM_MOUSEWHEEL:
        lResult = browserHostImpl->m_webPage->fireWheelEvent(hWnd, message, wParam, lParam);
        break;
    case WM_KEYDOWN:
        bHandle = browserHostImpl->m_webPage->fireKeyDownEvent(hWnd, message, wParam, lParam);
        break;
    case WM_KEYUP:
        bHandle = browserHostImpl->m_webPage->fireKeyUpEvent(hWnd, message, wParam, lParam);
        break;
    case WM_CHAR:
        bHandle = browserHostImpl->m_webPage->fireKeyPressEvent(hWnd, message, wParam, lParam);
        break;
    case WM_SETFOCUS:
        browserHostImpl->SendFocusEvent(true);
        break;
    case WM_KILLFOCUS:
        browserHostImpl->SendFocusEvent(false);
        break;
    case WM_IME_STARTCOMPOSITION:{
        if (!browserHostImpl->m_webPage)
            return 0;
        blink::IntRect caret = browserHostImpl->m_webPage->caretRect();
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
    case WM_DESTROY:
        // Clear the user data pointer.
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
        // Force the browser to be destroyed. This will result in a call to
        // BrowserDestroyed() that will release the reference added in
        // CreateHostWindow().
        bHandle = TRUE;
        browserHostImpl->WindowDestroyed();
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
