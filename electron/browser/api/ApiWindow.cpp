
#include <node_object_wrap.h>
#include <node_buffer.h>
#include "wke.h"
#include "common/ThreadCall.h"
#include "OptionsSwitches.h"
#include "ApiWebContents.h"
#include "NodeRegisterHelp.h"
#include "WindowList.h"

#include <set>
#if USING_VC6RT == 1
#pragma warning(push)
#pragma warning(disable:4273)
#include <windowsvc6.h>
#pragma warning(pop)
#endif

using namespace v8;
using namespace node;

namespace atom {

#pragma warning(push)
#pragma warning(disable:4309)
#pragma warning(disable:4838)
static const char helloNative[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };
#pragma warning(pop)

static NodeNative nativeHello{ "hello", helloNative, sizeof(helloNative) };

// 继承node的ObjectWrap，一般自定义C++类都应该继承node的ObjectWrap
class Window : public node::ObjectWrap {
public:
    explicit Window() {
        m_webContents = nullptr;
        m_hWnd = nullptr;
        m_memoryBMP = nullptr;
        m_memoryDC = nullptr;
        m_isLayerWindow = false;
        m_clientRect.left = 0;
        m_clientRect.top = 0;
        m_clientRect.right = 0;
        m_clientRect.bottom = 0;
        ::InitializeCriticalSection(&m_memoryCanvasLock);

        if (!m_liveSelf) {
            m_idGen = 0;
            m_liveSelf = new std::set<int>();
            m_liveSelfLock = new CRITICAL_SECTION();
            ::InitializeCriticalSection(m_liveSelfLock);
        }
        ::EnterCriticalSection(m_liveSelfLock);
        m_id = ++m_idGen;
        m_liveSelf->insert(m_id);
        ::LeaveCriticalSection(m_liveSelfLock);
    }

    ~Window() {
        DebugBreak();

        if (m_memoryBMP)
            ::DeleteObject(m_memoryBMP);
        if (m_memoryDC)
            ::DeleteDC(m_memoryDC);

        //ThreadCall::callUiThreadSync([this] {
            //delete data->m_webContents;
            ::SendMessage(this->m_hWnd, WM_CLOSE, 0, 0);
        //});
        WindowList::GetInstance()->RemoveWindow(this);

        ::EnterCriticalSection(m_liveSelfLock);
        m_liveSelf->erase(m_id);
        ::LeaveCriticalSection(m_liveSelfLock);

        ::DeleteCriticalSection(&m_memoryCanvasLock);
    }

    static bool isLive(int id) {
        ::EnterCriticalSection(m_liveSelfLock);
        std::set<int>::const_iterator it = m_liveSelf->find(id);
        bool b = it != m_liveSelf->end();
        ::LeaveCriticalSection(m_liveSelfLock);
        return b;
    }

    static void init(Local<Object> target, Environment* env) {
        Isolate* isolate = env->isolate();

        // Function模板
        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, newFunction);
        // 类名
        tpl->SetClassName(String::NewFromUtf8(isolate, "BrowserWindow"));
        // InternalField
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        v8::Local<v8::Template> t = tpl->InstanceTemplate();
        // 设置Prototype函数
        NODE_SET_METHOD(t, "close", closeApi);
        NODE_SET_METHOD(t, "focus", focusApi);
        NODE_SET_METHOD(t, "blur", blurApi);
        NODE_SET_METHOD(t, "isFocused", isFocusedApi);
        NODE_SET_METHOD(t, "show", showApi);
        NODE_SET_METHOD(t, "showInactive", showInactiveApi);
        NODE_SET_METHOD(t, "hide", hideApi);
        NODE_SET_METHOD(t, "isVisible", isVisibleApi);
        NODE_SET_METHOD(t, "isEnabled", isEnabledApi);
        NODE_SET_METHOD(t, "maximize", maximizeApi);
        NODE_SET_METHOD(t, "unmaximize", unmaximizeApi);
        NODE_SET_METHOD(t, "isMaximized", isMaximizedApi);
        NODE_SET_METHOD(t, "minimize", minimizeApi);
        NODE_SET_METHOD(t, "restore", restoreApi);
        NODE_SET_METHOD(t, "isMinimized",isMinimizedApi);
        NODE_SET_METHOD(t, "setFullScreen", setFullScreenApi);
        NODE_SET_METHOD(t, "isFullScreen", isFullScreenApi);
        NODE_SET_METHOD(t, "setAspectRatio", nullFunction);
        NODE_SET_METHOD(t, "previewFile", nullFunction);
        NODE_SET_METHOD(t, "closeFilePreview", nullFunction);
        NODE_SET_METHOD(t, "setParentWindow", nullFunction);
        NODE_SET_METHOD(t, "getParentWindow", nullFunction);
        NODE_SET_METHOD(t, "getChildWindows", nullFunction);
        NODE_SET_METHOD(t, "isModal", isModalApi);
        NODE_SET_METHOD(t, "getNativeWindowHandle", getNativeWindowHandleApi);
        NODE_SET_METHOD(t, "getBounds", getBoundsApi);
        NODE_SET_METHOD(t, "setBounds", setBoundsApi);
        NODE_SET_METHOD(t, "getSize", getSizeApi);
        NODE_SET_METHOD(t, "setSize", setSizeApi);
        NODE_SET_METHOD(t, "getContentBounds", getContentBoundsApi);
        NODE_SET_METHOD(t, "setContentBounds", setContentBoundsApi);
        NODE_SET_METHOD(t, "getContentSize", getContentSizeApi);
        NODE_SET_METHOD(t, "setContentSize", setContentSizeApi);
        NODE_SET_METHOD(t, "setMinimumSize", setMinimumSizeApi);
        NODE_SET_METHOD(t, "getMinimumSize", getMinimumSizeApi);
        NODE_SET_METHOD(t, "setMaximumSize", setMaximumSizeApi);
        NODE_SET_METHOD(t, "getMaximumSize", getMaximumSizeApi);
        NODE_SET_METHOD(t, "setSheetOffset", nullFunction);
        NODE_SET_METHOD(t, "setResizable", setResizableApi);
        NODE_SET_METHOD(t, "isResizable", isResizableApi);
        NODE_SET_METHOD(t, "setMovable", setMovableApi);
        NODE_SET_METHOD(t, "isMovable", isMovableApi);
        NODE_SET_METHOD(t, "setMinimizable", setMinimizableApi);
        NODE_SET_METHOD(t, "isMinimizable", isMinimizableApi);
        NODE_SET_METHOD(t, "isMaximizable", isMaximizableApi);
        NODE_SET_METHOD(t, "setFullScreenable", setFullScreenableApi);
        NODE_SET_METHOD(t, "isFullScreenable", isFullScreenableApi);
        NODE_SET_METHOD(t, "setClosable", setClosableApi);
        NODE_SET_METHOD(t, "isClosable", isClosableApi);
        NODE_SET_METHOD(t, "setAlwaysOnTop", setAlwaysOnTopApi);
        NODE_SET_METHOD(t, "isAlwaysOnTop", isAlwaysOnTopApi);
        NODE_SET_METHOD(t, "center", centerApi);
        NODE_SET_METHOD(t, "setPosition", setPositionApi);
        NODE_SET_METHOD(t, "getPosition", getPositionApi);
        NODE_SET_METHOD(t, "setTitle", setTitleApi);
        NODE_SET_METHOD(t, "getTitle", getTitleApi);
        NODE_SET_METHOD(t, "flashFrame", flashFrameApi);
        NODE_SET_METHOD(t, "setSkipTaskbar", setSkipTaskbarApi);
        NODE_SET_METHOD(t, "setKiosk", nullFunction);
        NODE_SET_METHOD(t, "isKiosk", nullFunction);
        NODE_SET_METHOD(t, "setBackgroundColor", setBackgroundColorApi);
        NODE_SET_METHOD(t, "setHasShadow", nullFunction);
        NODE_SET_METHOD(t, "hasShadow", nullFunction);
        NODE_SET_METHOD(t, "setRepresentedFilename", nullFunction);
        NODE_SET_METHOD(t, "getRepresentedFilename", nullFunction);
        NODE_SET_METHOD(t, "setDocumentEdited", setDocumentEditedApi);
        NODE_SET_METHOD(t, "isDocumentEdited", isDocumentEditedApi);
        NODE_SET_METHOD(t, "setIgnoreMouseEvents", setIgnoreMouseEventsApi);
        NODE_SET_METHOD(t, "setContentProtection", setContentProtectionApi);
        NODE_SET_METHOD(t, "setFocusable", setFocusableApi);
        NODE_SET_METHOD(t, "focusOnWebView", focusOnWebViewApi);
        NODE_SET_METHOD(t, "blurWebView", blurApi);
        NODE_SET_METHOD(t, "isWebViewFocused", isWebViewFocusedApi);
        NODE_SET_METHOD(t, "setOverlayIcon", setOverlayIconApi);
        NODE_SET_METHOD(t, "setThumbarButtons", setThumbarButtonsApi);
        NODE_SET_METHOD(t, "setMenu", setMenuApi);
        NODE_SET_METHOD(t, "setAutoHideMenuBar", setAutoHideMenuBarApi);
        NODE_SET_METHOD(t, "isMenuBarAutoHide", isMenuBarAutoHideApi);
        NODE_SET_METHOD(t, "setMenuBarVisibility", setMenuBarVisibilityApi);
        NODE_SET_METHOD(t, "isMenuBarVisible", isMenuBarVisibleApi);
        NODE_SET_METHOD(t, "setVisibleOnAllWorkspaces", setVisibleOnAllWorkspacesApi);
        NODE_SET_METHOD(t, "isVisibleOnAllWorkspaces", isVisibleOnAllWorkspacesApi);
        NODE_SET_METHOD(t, "setVibrancy", nullFunction);
        NODE_SET_METHOD(t, "hookWindowMessage", hookWindowMessageApi);
        NODE_SET_METHOD(t, "isWindowMessageHooked", isWindowMessageHookedApi);
        NODE_SET_METHOD(t, "unhookWindowMessage", unhookWindowMessageApi);
        NODE_SET_METHOD(t, "unhookAllWindowMessages", unhookAllWindowMessagesApi);
        NODE_SET_METHOD(t, "setThumbnailClip", setThumbnailClipApi);
        NODE_SET_METHOD(t, "setThumbnailToolTip", setThumbnailToolTipApi);
        NODE_SET_METHOD(t, "setAppDetails", setAppDetailsApi);
        NODE_SET_METHOD(t, "setIcon", setIconApi);
        NODE_SET_PROTOTYPE_METHOD(tpl, "id", nullFunction);
        NODE_SET_PROTOTYPE_METHOD(tpl, "webContents", getWebContentsApi);

        // 设置constructor
        constructor.Reset(isolate, tpl->GetFunction());
        // export `BrowserWindow`
        target->Set(String::NewFromUtf8(isolate, "BrowserWindow"), tpl->GetFunction());
    }

//     static void staticOnPaintUpdated(wkeWebView webView, Window* win, const HDC hdc, int x, int y, int cx, int cy) {
//         HWND hWnd = win->m_hWnd;
//         HDC hdcScreen = ::GetDC(hWnd);
//         RECT rectDest;
//         if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE))) {
//             ::GetWindowRect(hWnd, &rectDest);
// 
//             SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
//             POINT pointDest = { rectDest.left, rectDest.top };
//             POINT pointSource = { 0, 0 };
// 
//             BLENDFUNCTION blend = { 0 };
//             memset(&blend, 0, sizeof(blend));
//             blend.BlendOp = AC_SRC_OVER;
//             blend.SourceConstantAlpha = 255;
//             blend.AlphaFormat = AC_SRC_ALPHA;
//             ::UpdateLayeredWindow(hWnd, hdcScreen, &pointDest, &sizeDest, hdc, &pointSource, RGB(0, 0, 0), &blend, ULW_ALPHA);
//         }
//         else {
//             win->onPaintUpdated(hdcScreen, hdc, x, y, cx, cy);
//         }
// 
//         ::ReleaseDC(hWnd, hdcScreen);
//     }

    void onPaintUpdatedInCompositeThread(const HDC hdc, int x, int y, int cx, int cy) {
        HWND hWnd = m_hWnd;
        RECT rectDest;
        ::GetClientRect(hWnd, &rectDest);
        SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
        if (0 == sizeDest.cx * sizeDest.cy)
            return;
            
        if (!m_memoryDC)
            m_memoryDC = ::CreateCompatibleDC(nullptr);

        if (!m_memoryBMP || m_clientRect.top != rectDest.top || m_clientRect.bottom != rectDest.bottom ||
            m_clientRect.right != rectDest.right || m_clientRect.left != rectDest.left) {
            m_clientRect = rectDest;

            if (m_memoryBMP)
                ::DeleteObject((HGDIOBJ)m_memoryBMP);
            m_memoryBMP = ::CreateCompatibleBitmap(hdc, sizeDest.cx, sizeDest.cy);
        }

        HBITMAP hbmpOld = (HBITMAP)::SelectObject(m_memoryDC, m_memoryBMP);
        ::BitBlt(m_memoryDC, x, y, cx, cy, hdc, x, y, SRCCOPY);
        ::SelectObject(m_memoryDC, (HGDIOBJ)hbmpOld);
    }

    void onPaintUpdatedInUiThread(int x, int y, int cx, int cy) {
        ::EnterCriticalSection(&m_memoryCanvasLock);

        HDC hdcScreen = ::GetDC(m_hWnd);
        ::BitBlt(hdcScreen, x, y, cx, cy, m_memoryDC, x, y, SRCCOPY);
        ::ReleaseDC(m_hWnd, hdcScreen);

        ::LeaveCriticalSection(&m_memoryCanvasLock);
    }

    static void staticOnPaintUpdatedInCompositeThread(wkeWebView webView, Window* win, const HDC hdc, int x, int y, int cx, int cy) {
        ::EnterCriticalSection(&win->m_memoryCanvasLock);
        win->onPaintUpdatedInCompositeThread(hdc, x, y, cx, cy);
        ::LeaveCriticalSection(&win->m_memoryCanvasLock);

        if (win->m_isLayerWindow) {
            int id = win->m_id;
            ThreadCall::callUiThreadAsync([id, win, x, y, cx, cy] {
                if (isLive(id))
                    win->onPaintUpdatedInUiThread(x, y, cx, cy);
            });
        } else {
            RECT rc = { x, y, x + cx, y + cy };
            ::InvalidateRect(win->m_hWnd, &rc, false);
        }
    }

    void onPaintMessage(HWND hWnd) {
        PAINTSTRUCT ps = { 0 };
        HDC hdc = ::BeginPaint(hWnd, &ps);

        RECT rcClip = ps.rcPaint;
        RECT rcClient;
        ::GetClientRect(hWnd, &rcClient);

        RECT rcInvalid = rcClient;
        if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
            ::IntersectRect(&rcInvalid, &rcClip, &rcClient);

        int srcX = rcInvalid.left - rcClient.left;
        int srcY = rcInvalid.top - rcClient.top;
        int destX = rcInvalid.left;
        int destY = rcInvalid.top;
        int width = rcInvalid.right - rcInvalid.left;
        int height = rcInvalid.bottom - rcInvalid.top;

        ::EnterCriticalSection(&m_memoryCanvasLock);
        if (0 != width && 0 != height && m_memoryBMP && m_memoryDC) {
            HBITMAP hbmpOld = (HBITMAP)::SelectObject(m_memoryDC, m_memoryBMP);
            BOOL b = ::BitBlt(hdc, destX, destY, width, height, m_memoryDC, srcX, srcY, SRCCOPY);
            b = b;
        }
        ::LeaveCriticalSection(&m_memoryCanvasLock);

        ::EndPaint(hWnd, &ps);
    }

    static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        Window* win = (Window *)::GetPropW(hWnd, kPrppW);
        if (!win) {
            if (message == WM_CREATE) {
                LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
                Window *win = (Window *)cs->lpCreateParams;
                ThreadCall::callBlinkThreadSync([win, hWnd] {
                    wkeSetHandle(win->m_webContents->m_view, hWnd);
                });
                
                ::SetPropW(hWnd, kPrppW, (HANDLE)win);
                ::SetTimer(hWnd, (UINT_PTR)win, 70, NULL);
                return 0;
            }
        }
        if (!win)
            return ::DefWindowProcW(hWnd, message, wParam, lParam);
        int id = win->m_id;

        wkeWebView pthis = win->m_webContents->m_view;
        if (!pthis)
            return ::DefWindowProcW(hWnd, message, wParam, lParam);
        switch (message) {
        case WM_CLOSE:
            ::ShowWindow(hWnd, SW_HIDE);
            ::DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            ::KillTimer(hWnd, (UINT_PTR)win);
            ::RemovePropW(hWnd, kPrppW);
            ThreadCall::callBlinkThreadSync([pthis] {
                wkeDestroyWebView(pthis);
            });
            //delete win->m_webContents;
            return 0;

        case WM_TIMER:
            //wkeRepaintIfNeeded(pthis);
            return 0;

        case WM_PAINT:
            win->onPaintMessage(hWnd);
            break;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_SIZE: {
            ::EnterCriticalSection(&win->m_memoryCanvasLock);
            if (win->m_memoryDC)
                ::DeleteDC(win->m_memoryDC);
            win->m_memoryDC = nullptr;

            if (win->m_memoryBMP)
                ::DeleteObject((HGDIOBJ)win->m_memoryBMP);
            win->m_memoryBMP = nullptr;
            ::LeaveCriticalSection(&win->m_memoryCanvasLock);

            ::GetClientRect(hWnd, &win->m_clientRect);
            
            ThreadCall::callBlinkThreadSync([pthis, lParam] {
                wkeResize(pthis, LOWORD(lParam), HIWORD(lParam));
                wkeRepaintIfNeeded(pthis);
            });
            return 0;
        }
        case WM_KEYDOWN: {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            bool retVal = false;
            ThreadCall::callBlinkThreadSync([pthis, virtualKeyCode, flags, &retVal] {
                retVal = wkeFireKeyDownEvent(pthis, virtualKeyCode, flags, false);
            });
            if (retVal)
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

            bool retVal = false;
            ThreadCall::callBlinkThreadSync([pthis, virtualKeyCode, flags, &retVal] {
                retVal = wkeFireKeyUpEvent(pthis, virtualKeyCode, flags, false);
            });
            if (retVal)
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

            bool retVal = false;
            ThreadCall::callBlinkThreadSync([pthis, charCode, flags, &retVal] {
                retVal = wkeFireKeyPressEvent(pthis, charCode, flags, false);
            });
            if (retVal)
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
                ::SetFocus(hWnd);
                ::SetCapture(hWnd);
            }
            else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP) {
                ::ReleaseCapture();
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

            ThreadCall::callBlinkThreadAsync([id, pthis, message, x, y, flags] {
                if (isLive(id))
                    wkeFireMouseEvent(pthis, message, x, y, flags);
            });
            break;
        }
        case WM_CONTEXTMENU: {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            if (pt.x != -1 && pt.y != -1)
                ::ScreenToClient(hWnd, &pt);

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

            ThreadCall::callBlinkThreadAsync([id, pthis, pt, flags] {
                if (isLive(id))
                    wkeFireContextMenuEvent(pthis, pt.x, pt.y, flags);
            });
            break;
        }
        case WM_MOUSEWHEEL: {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            ::ScreenToClient(hWnd, &pt);

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

            ThreadCall::callBlinkThreadAsync([id, pthis, pt, delta, flags] {
                wkeFireMouseWheelEvent(pthis, pt.x, pt.y, delta, flags);
            });
            break;
        }
        case WM_SETFOCUS:
            ThreadCall::callBlinkThreadAsync([id, pthis]{
                if (isLive(id))
                    wkeSetFocus(pthis);
            });
            return 0;

        case WM_KILLFOCUS:
            ThreadCall::callBlinkThreadAsync([id, pthis] {
                if (isLive(id))
                    wkeKillFocus(pthis);
            });
            return 0;

        case WM_SETCURSOR: {
            bool retVal = false;
            ThreadCall::callBlinkThreadAsync([pthis, hWnd, &retVal] {
                retVal = wkeFireWindowsMessage(pthis, hWnd, WM_SETCURSOR, 0, 0, nullptr);
            });
            if (retVal)
                return 0;
        }
            break;

        case WM_IME_STARTCOMPOSITION: {
            wkeRect caret;
            ThreadCall::callBlinkThreadSync([pthis, &caret] {
                caret = wkeGetCaretRect(pthis);
            });

            COMPOSITIONFORM compositionForm;
            compositionForm.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
            compositionForm.ptCurrentPos.x = caret.x;
            compositionForm.ptCurrentPos.y = caret.y;

            HIMC hIMC = ::ImmGetContext(hWnd);
            ::ImmSetCompositionWindow(hIMC, &compositionForm);
            ::ImmReleaseContext(hWnd, hIMC);
        }
            return 0;
        }

        return ::DefWindowProcW(hWnd, message, wParam, lParam);
    }

    struct CreateWindowParam {
        int x;
        int y;
        int width;
        int height;
        unsigned styles;
        unsigned styleEx;
        bool transparent;
        std::wstring title;
    };

    static void UTF8ToUTF16(const std::string& utf8, std::wstring* utf16) {
        size_t n = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), nullptr, 0);

        std::vector<wchar_t> wbuf(n);
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], n);
        utf16->resize(n);
        utf16->assign(&wbuf[0], n);
    }

    static v8::Local<v8::Value> ToBuffer(v8::Isolate* isolate, void* val, int size) {
        auto buffer = node::Buffer::Copy(isolate, static_cast<char*>(val), size);
        if (buffer.IsEmpty()) {
            return v8::Null(isolate);
        }
        else {
            return buffer.ToLocalChecked();
        }
    }

    static Window* newWindow(gin::Dictionary* options) {
        Window* win = new Window();
        CreateWindowParam createWindowParam;
        createWindowParam.styles = 0;
        createWindowParam.styleEx = 0;
        createWindowParam.transparent = false;

        WebContents* webContents;
        Handle<Object> webContentsV8;
        // If no WebContents was passed to the constructor, create it from options.
        if (!options->Get("webContents", &webContentsV8)) {
            // Use options.webPreferences to create WebContents.
            gin::Dictionary webPreferences = gin::Dictionary::CreateEmpty(options->isolate());
            options->Get(options::kWebPreferences, &webPreferences);

            // Copy the backgroundColor to webContents.
            v8::Local<v8::Value> value;
            if (options->Get(options::kBackgroundColor, &value))
                webPreferences.Set(options::kBackgroundColor, value);

            v8::Local<v8::Value> transparent;
            if (options->Get("transparent", &transparent))
                webPreferences.Set("transparent", transparent);

            // Offscreen windows are always created frameless.
            bool offscreen;
            if (webPreferences.Get("offscreen", &offscreen) && offscreen) {
                options->Set(options::kFrame, false);
            }
            webContents = WebContents::create(options->isolate(), webPreferences);
        } else
            webContents = WebContents::ObjectWrap::Unwrap<WebContents>(webContentsV8);

        win->m_webContents = webContents;

        v8::Local<v8::Value> transparent;
        options->Get("transparent", &transparent);
        v8::Local<v8::Value> height;
        options->Get("height", &height);
        v8::Local<v8::Value> width;
        options->Get("width", &width);
        v8::Local<v8::Value> x;
        options->Get("x", &x);
        v8::Local<v8::Value> y;
        options->Get("y", &y);
        v8::Local<v8::Value> title;
        options->Get("title", &title);
        if (title->IsString()) {
            v8::String::Utf8Value str(title);
            UTF8ToUTF16(*str, &createWindowParam.title);
        } else 
            createWindowParam.title = L"Electron";
        
        createWindowParam.x = x->Int32Value();
        createWindowParam.y = y->Int32Value();
        createWindowParam.width = width->Int32Value();
        createWindowParam.height = height->Int32Value();

        if (transparent->IsBoolean() && transparent->ToBoolean()->BooleanValue()) {
            createWindowParam.transparent = true;
            createWindowParam.styles = WS_POPUP;
            createWindowParam.styleEx = WS_EX_LAYERED;
        } else {
            createWindowParam.styles = WS_OVERLAPPEDWINDOW;
            createWindowParam.styleEx = 0;
        }

        //ThreadCall::callUiThreadSync([win, &createWindowParam] {
            win->newWindowTaskInUiThread(&createWindowParam);
        //});
        return win;
    }

    void newWindowTaskInUiThread(const CreateWindowParam* createWindowParam) {
        //HandleScope scope(options->isolate());
        m_hWnd = ::CreateWindowEx(
            createWindowParam->styleEx,        // window ex-style
            L"mb_electron_window",    // window class name
            createWindowParam->title.c_str(), // window caption
            createWindowParam->styles,         // window style
            createWindowParam->x,              // initial x position
            createWindowParam->y,              // initial y position
            createWindowParam->width,          // initial x size
            createWindowParam->height,         // initial y size
            NULL,         // parent window handle
            NULL,           // window menu handle
            ::GetModuleHandleW(NULL),           // program instance handle
            this);         // creation parameters

        if (!::IsWindow(m_hWnd))
            return;

        m_clientRect.right = createWindowParam->width;
        m_clientRect.bottom = createWindowParam->height;

        Window* win = this;
        ThreadCall::callBlinkThreadSync([win, createWindowParam] {
            if (createWindowParam->transparent)
                wkeSetTransparent(win->m_webContents->m_view, true);
            wkeSettings settings;
            settings.mask = WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD;
            wkeConfigure(&settings);
            wkeResize(win->m_webContents->m_view, createWindowParam->width, createWindowParam->height);
            wkeOnPaintUpdated(win->m_webContents->m_view, (wkePaintUpdatedCallback)staticOnPaintUpdatedInCompositeThread, win);
        });

        ::ShowWindow(m_hWnd, TRUE);
    }

private:
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        if (args.IsConstructCall()) {
            if (args.Length() > 1)
                return;
            
            gin::Dictionary options(args.GetIsolate(), args[0]->ToObject()); // // 使用new调用 `new Point(...)`
            Window* win = newWindow(&options);
            
            WindowList::GetInstance()->AddWindow(win);
            
			win->Wrap(args.This(), isolate); // 包装this指针
            args.GetReturnValue().Set(args.This());
        } else {
            // 使用`Point(...)`
            const int argc = 2;
            Local<Value> argv[argc] = { args[0], args[1] };
            // 使用constructor构建Function
            Local<Function> cons = Local<Function>::New(isolate, constructor);
            args.GetReturnValue().Set(cons->NewInstance(argc, argv));
        }
    }

    //close方法
    static void closeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SendMessage(win->m_hWnd, WM_CLOSE, 0, 0);
    }

    //focus方法
    static void focusApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SetFocus(win->m_hWnd);
    }

    //blur方法
    static void blurApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SetFocus(NULL);
    }

    //isFocused方法
    static void isFocusedApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, GetFocus() == win->m_hWnd);
        args.GetReturnValue().Set(ret);
    }

    //show方法
    static void showApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, TRUE);
        ::SetFocus(win->m_hWnd);
    }

    //showInactive方法
    static void showInactiveApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, TRUE);
    }

    //hide
    static void hideApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, FALSE);
    }

    //isVisible
    static void isVisibleApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsWindowVisible(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //isEnabled
    static void isEnabledApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsWindowEnabled(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //maximize
    static void maximizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_MAXIMIZE);
    }

    //unmaximize
    static void unmaximizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_RESTORE);
    }

    //isMaximized
    static void isMaximizedApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsZoomed(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //minimize
    static void minimizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_MINIMIZE);
    }

    //restore
    static void restoreApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_RESTORE);
    }

    //isMinimized
    static void isMinimizedApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsIconic(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //setFullScreen
    static void setFullScreenApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
            RECT rc;
            HWND hDesk = ::GetDesktopWindow();
            ::GetWindowRect(hDesk, &rc);
            ::SetWindowLong(win->m_hWnd, GWL_STYLE, GetWindowLong(win->m_hWnd, GWL_STYLE) | WS_BORDER);
            ::SetWindowPos(win->m_hWnd, HWND_TOPMOST, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);
        }
        else {
            ::SetWindowLong(win->m_hWnd, GWL_STYLE, GetWindowLong(win->m_hWnd, GWL_STYLE) ^ WS_BORDER);
        }
    }

    //
    static void isFullScreenApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setParentWindowApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getParentWindowApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getChildWindowsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isModalApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getNativeWindowHandleApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);
        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());

        args.GetReturnValue().Set(ToBuffer(isolate, (void*)(&win->m_hWnd), sizeof(HWND)));
    }

    static void getBoundsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Integer> x = Integer::New(isolate, win->m_clientRect.left);
        Local<Integer> y = Integer::New(isolate, win->m_clientRect.top);
        Local<Integer> width = Integer::New(isolate, win->m_clientRect.right - win->m_clientRect.left);
        Local<Integer> height = Integer::New(isolate, win->m_clientRect.bottom - win->m_clientRect.top);
        Local<Object> bounds = Object::New(isolate);
        bounds->Set(String::NewFromUtf8(isolate, "x"), x);
        bounds->Set(String::NewFromUtf8(isolate, "y"), y);
        bounds->Set(String::NewFromUtf8(isolate, "width"), width);
        bounds->Set(String::NewFromUtf8(isolate, "height"), height);
        args.GetReturnValue().Set(bounds);
    }

    static void setBoundsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsObject()) {
            Local<Object> bounds = args[0]->ToObject();
            LONG x = (LONG)bounds->Get(String::NewFromUtf8(isolate, "x"))->NumberValue();
            LONG y = (LONG)bounds->Get(String::NewFromUtf8(isolate, "y"))->NumberValue();
            LONG width = (LONG)bounds->Get(String::NewFromUtf8(isolate, "width"))->NumberValue();
            LONG height = (LONG)bounds->Get(String::NewFromUtf8(isolate, "height"))->NumberValue();
            ::MoveWindow(win->m_hWnd, x, y, width, height, TRUE);
        }
    }

    static void getSizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Integer> width = Integer::New(isolate, win->m_clientRect.right - win->m_clientRect.left);
        Local<Integer> height = Integer::New(isolate, win->m_clientRect.bottom - win->m_clientRect.top);
        Local<Array> size = Array::New(isolate, 2);
        size->Set(0, width);
        size->Set(1, height);
        args.GetReturnValue().Set(size);
    }

    static void setSizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsNumber() && args[1]->IsNumber()) {
            LONG width = (LONG)(args[0]->ToNumber()->NumberValue());
            LONG height = (LONG)(args[1]->ToNumber()->NumberValue());
            ::SetWindowPos(win->m_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
        }
    }

    static void getContentBoundsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setContentBoundsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getContentSizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        ThreadCall::callBlinkThreadSync([args] {
            Isolate* isolate = args.GetIsolate();
            HandleScope scope(isolate);

            Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
            Local<Integer> width = Integer::New(isolate, wkeGetContentWidth(win->m_webContents->m_view));
            Local<Integer> height = Integer::New(isolate, wkeGetContentHeight(win->m_webContents->m_view));
            Local<Array> size = Array::New(isolate, 2);
            size->Set(0, width);
            size->Set(1, height);
            args.GetReturnValue().Set(size);
        });
    }

    static void setContentSizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        ThreadCall::callBlinkThreadSync([args] {
            Isolate* isolate = args.GetIsolate();
            HandleScope scope(isolate);

            Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
            if (args[0]->IsNumber() && args[1]->IsNumber()) {
                int width = (int)(args[0]->ToNumber()->NumberValue());
                int height = (int)(args[1]->ToNumber()->NumberValue());
                wkeResize(win->m_webContents->m_view, width, height);
                wkeRepaintIfNeeded(win->m_webContents->m_view);
            }
        });
    }

    static void setMinimumSizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getMinimumSizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMaximumSizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getMaximumSizeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setResizableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isResizableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMovableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMovableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMinimizableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMinimizableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMaximizableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setFullScreenableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isFullScreenableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setClosableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isClosableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setAlwaysOnTopApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
            ::SetWindowPos(win->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else {
            ::SetWindowPos(win->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
    }

    static void isAlwaysOnTopApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, GetWindowLong(win->m_hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST);
        args.GetReturnValue().Set(ret);
    }

    static void centerApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());

        int screenX, screenY;
        screenX = ::GetSystemMetrics(SM_CXSCREEN);  //取得屏幕的宽度
        screenY = ::GetSystemMetrics(SM_CYSCREEN);  //取得屏幕的高度

        RECT rect;
        GetWindowRect(win->m_hWnd, &rect);
        rect.left = (screenX - rect.right) / 2;
        rect.top = (screenY - rect.bottom) / 2;

        //设置窗体位置
        ::SetWindowPos(win->m_hWnd, NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_NOSIZE);
    }

    static void setPositionApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsInt32() && args[1]->IsInt32())
        {
            int x = args[0]->ToInt32()->Int32Value();
            int y = args[1]->ToInt32()->Int32Value();
            ::SetWindowPos(win->m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
        }
    }

    static void getPositionApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());

        RECT rect;
        GetWindowRect(win->m_hWnd, &rect);
        Local<Integer> x = Integer::New(isolate, rect.left);
        Local<Integer> y = Integer::New(isolate, rect.top);
        Local<Array> pos = Array::New(isolate, 2);
        pos->Set(0, x);
        pos->Set(1, y);
        args.GetReturnValue().Set(pos);
    }

    static void setTitleApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());

        if (args[0]->IsString()) {
            String::Utf8Value str(args[0]);
            std::wstring title;
            UTF8ToUTF16(*str, &title);
            ::SetWindowText(win->m_hWnd, title.c_str());
        }
    }

    static void getTitleApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        wchar_t text[MAX_PATH] = { 0 };
        ::GetWindowText(win->m_hWnd, text, MAX_PATH);
        Local<String> title = String::NewFromTwoByte(isolate, (const uint16_t*)text);
        args.GetReturnValue().Set(title);
    }

    static void flashFrameApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setSkipTaskbarApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        DWORD style = ::GetWindowLong(win->m_hWnd, GWL_STYLE);
        if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
            style |= WS_EX_TOOLWINDOW;
            style &= ~WS_EX_APPWINDOW;
        }
        else {  //todo 如果窗口原来的style没有WS_EX_APPWINDOW，就可能有问题
            style &= ~WS_EX_TOOLWINDOW;
            style |= WS_EX_APPWINDOW;
        }
        ::SetWindowLong(win->m_hWnd, GWL_EXSTYLE, style);
    }

    static void setBackgroundColorApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setDocumentEditedApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        ThreadCall::callBlinkThreadSync([args] {
            Isolate* isolate = args.GetIsolate();
            HandleScope scope(isolate);

            Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
            if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
                wkeSetEditable(win->m_webContents->m_view, true);
            }
            else {
                wkeSetEditable(win->m_webContents->m_view, false);
            }
        });
    }

    static void isDocumentEditedApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setIgnoreMouseEventsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setContentProtectionApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setFocusableApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void focusOnWebViewApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        ThreadCall::callBlinkThreadSync([args] {
            Isolate* isolate = args.GetIsolate();
            HandleScope scope(isolate);

            Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
            wkeSetFocus(win->m_webContents->m_view);
        });
    }

    static void isWebViewFocusedApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setOverlayIconApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setThumbarButtonsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMenuApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setAutoHideMenuBarApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMenuBarAutoHideApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMenuBarVisibilityApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMenuBarVisibleApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setVisibleOnAllWorkspacesApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isVisibleOnAllWorkspacesApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void hookWindowMessageApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isWindowMessageHookedApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void unhookWindowMessageApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void unhookAllWindowMessagesApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setThumbnailClipApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setThumbnailToolTipApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setAppDetailsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setIconApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getWebContentsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (!win->m_webContents)
            args.GetReturnValue().SetNull();
        else
            args.GetReturnValue().Set(v8::Local<v8::Value>::New(isolate, win->m_webContents->handle()));
    }

    // 空实现
    static void nullFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static v8::Persistent<v8::Function> constructor;

private:
    static const WCHAR* kPrppW;
    WebContents* m_webContents;
    HWND m_hWnd;
    CRITICAL_SECTION m_memoryCanvasLock;
    HBITMAP m_memoryBMP;
    HDC m_memoryDC;
    RECT m_clientRect;
    bool m_isLayerWindow;
    int m_id;
    static int m_idGen;
    static std::set<int>* m_liveSelf;
    static CRITICAL_SECTION* m_liveSelfLock;
};

const WCHAR* Window::kPrppW = L"mele";
Persistent<Function> Window::constructor;
int Window::m_idGen;
std::set<int>* Window::m_liveSelf = nullptr;
CRITICAL_SECTION* Window::m_liveSelfLock = nullptr;

static void initializeWindowApi(Local<Object> target, Local<Value> unused, Local<Context> context) {
    Environment* env = Environment::GetCurrent(context);
    Window::init(target, env);
    WNDCLASS wndClass = { 0 };
    if (!GetClassInfoW(NULL, L"mb_electron_window", &wndClass)) {
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc = &Window::windowProc;
        wndClass.cbClsExtra = 200;
        wndClass.cbWndExtra = 200;
        wndClass.hInstance = GetModuleHandleW(NULL);
        //wndClass.hIcon = LoadIcon(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON1));
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = NULL;
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = L"mb_electron_window";
        RegisterClass(&wndClass);
    }
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_window, initializeWindowApi, &nativeHello)

} // atom