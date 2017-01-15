#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"

#include "electron.h"
#include "dictionary.h"
#include "options_switches.h"
#include "api_web_contents.h"

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
    // 静态方法，用于注册类和方法
    static void Init(Local<Object> target, Environment* env) {
        Isolate* isolate = env->isolate();

        // Function模板
        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
        // 类名
        tpl->SetClassName(String::NewFromUtf8(isolate, "BrowserWindow"));
        // InternalField
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        v8::Local<v8::Template> t = tpl->InstanceTemplate();
        // 设置Prototype函数
        NODE_SET_METHOD(t, "close", close);
        NODE_SET_METHOD(t, "focus", focus);
        NODE_SET_METHOD(t, "blur", blur);
        NODE_SET_METHOD(t, "isFocused", isFocused);
        NODE_SET_METHOD(t, "show", show);
        NODE_SET_METHOD(t, "showInactive", showInactive);
        NODE_SET_METHOD(t, "hide", hide);
        NODE_SET_METHOD(t, "isVisible", isVisible);
        NODE_SET_METHOD(t, "isEnabled", isEnabled);
        NODE_SET_METHOD(t, "maximize", maximize);
        NODE_SET_METHOD(t, "unmaximize", unmaximize);
        NODE_SET_METHOD(t, "isMaximized", isMaximized);
        NODE_SET_METHOD(t, "minimize", minimize);
        NODE_SET_METHOD(t, "restore", restore);
        NODE_SET_METHOD(t, "isMinimized",isMinimized);
        NODE_SET_METHOD(t, "setFullScreen", setFullScreen);
        NODE_SET_METHOD(t, "isFullScreen", nullFunction);
        NODE_SET_METHOD(t, "setAspectRatio", nullFunction);
        NODE_SET_METHOD(t, "previewFile", nullFunction);
        NODE_SET_METHOD(t, "closeFilePreview", nullFunction);
        NODE_SET_METHOD(t, "setParentWindow", nullFunction);
        NODE_SET_METHOD(t, "getParentWindow", nullFunction);
        NODE_SET_METHOD(t, "getChildWindows", nullFunction);
        NODE_SET_METHOD(t, "isModal", nullFunction);
        NODE_SET_METHOD(t, "getNativeWindowHandle", nullFunction);
        NODE_SET_METHOD(t, "getBounds", nullFunction);
        NODE_SET_METHOD(t, "setBounds", nullFunction);
        NODE_SET_METHOD(t, "getSize", nullFunction);
        NODE_SET_METHOD(t, "setSize", nullFunction);
        NODE_SET_METHOD(t, "getContentBounds", nullFunction);
        NODE_SET_METHOD(t, "setContentBounds", nullFunction);
        NODE_SET_METHOD(t, "getContentSize", nullFunction);
        NODE_SET_METHOD(t, "setContentSize", nullFunction);
        NODE_SET_METHOD(t, "setMinimumSize", nullFunction);
        NODE_SET_METHOD(t, "getMinimumSize", nullFunction);
        NODE_SET_METHOD(t, "setMaximumSize", nullFunction);
        NODE_SET_METHOD(t, "getMaximumSize", nullFunction);
        NODE_SET_METHOD(t, "setSheetOffset", nullFunction);
        NODE_SET_METHOD(t, "setResizable", nullFunction);
        NODE_SET_METHOD(t, "isResizable", nullFunction);
        NODE_SET_METHOD(t, "setMovable", nullFunction);
        NODE_SET_METHOD(t, "isMovable", nullFunction);
        NODE_SET_METHOD(t, "setMinimizable", nullFunction);
        NODE_SET_METHOD(t, "isMinimizable", nullFunction);
        NODE_SET_METHOD(t, "isMaximizable", nullFunction);
        NODE_SET_METHOD(t, "setFullScreenable", nullFunction);
        NODE_SET_METHOD(t, "isFullScreenable", nullFunction);
        NODE_SET_METHOD(t, "setClosable", nullFunction);
        NODE_SET_METHOD(t, "isClosable", nullFunction);
        NODE_SET_METHOD(t, "setAlwaysOnTop", nullFunction);
        NODE_SET_METHOD(t, "isAlwaysOnTop", nullFunction);
        NODE_SET_METHOD(t, "center", nullFunction);
        NODE_SET_METHOD(t, "setPosition", nullFunction);
        NODE_SET_METHOD(t, "getPosition", nullFunction);
        NODE_SET_METHOD(t, "setTitle", nullFunction);
        NODE_SET_METHOD(t, "getTitle", nullFunction);
        NODE_SET_METHOD(t, "flashFrame", nullFunction);
        NODE_SET_METHOD(t, "setSkipTaskbar", nullFunction);
        NODE_SET_METHOD(t, "setKiosk", nullFunction);
        NODE_SET_METHOD(t, "isKiosk", nullFunction);
        NODE_SET_METHOD(t, "setBackgroundColor", nullFunction);
        NODE_SET_METHOD(t, "setHasShadow", nullFunction);
        NODE_SET_METHOD(t, "hasShadow", nullFunction);
        NODE_SET_METHOD(t, "setRepresentedFilename", nullFunction);
        NODE_SET_METHOD(t, "getRepresentedFilename", nullFunction);
        NODE_SET_METHOD(t, "setDocumentEdited", nullFunction);
        NODE_SET_METHOD(t, "isDocumentEdited", nullFunction);
        NODE_SET_METHOD(t, "setIgnoreMouseEvents", nullFunction);
        NODE_SET_METHOD(t, "setContentProtection", nullFunction);
        NODE_SET_METHOD(t, "setFocusable", nullFunction);
        NODE_SET_METHOD(t, "focusOnWebView", nullFunction);
        NODE_SET_METHOD(t, "blurWebView", nullFunction);
        NODE_SET_METHOD(t, "isWebViewFocused", nullFunction);
        NODE_SET_METHOD(t, "setOverlayIcon", nullFunction);
        NODE_SET_METHOD(t, "setThumbarButtons", nullFunction);
        NODE_SET_METHOD(t, "setMenu", nullFunction);
        NODE_SET_METHOD(t, "setAutoHideMenuBar", nullFunction);
        NODE_SET_METHOD(t, "isMenuBarAutoHide", nullFunction);
        NODE_SET_METHOD(t, "setMenuBarVisibility", nullFunction);
        NODE_SET_METHOD(t, "isMenuBarVisible", nullFunction);
        NODE_SET_METHOD(t, "setVisibleOnAllWorkspaces", nullFunction);
        NODE_SET_METHOD(t, "isVisibleOnAllWorkspaces", nullFunction);
        NODE_SET_METHOD(t, "setVibrancy", nullFunction);
        NODE_SET_METHOD(t, "hookWindowMessage", nullFunction);
        NODE_SET_METHOD(t, "isWindowMessageHooked", nullFunction);
        NODE_SET_METHOD(t, "unhookWindowMessage", nullFunction);
        NODE_SET_METHOD(t, "unhookAllWindowMessages", nullFunction);
        NODE_SET_METHOD(t, "setThumbnailClip", nullFunction);
        NODE_SET_METHOD(t, "setThumbnailToolTip", nullFunction);
        NODE_SET_METHOD(t, "setAppDetails", nullFunction);
        NODE_SET_METHOD(t, "setIcon", nullFunction);
        NODE_SET_PROTOTYPE_METHOD(tpl, "id", nullFunction);
        NODE_SET_PROTOTYPE_METHOD(tpl, "webContents", getWebContents);

        // 设置constructor
        constructor.Reset(isolate, tpl->GetFunction());
        // export `BrowserWindow`
        target->Set(String::NewFromUtf8(isolate, "BrowserWindow"), tpl->GetFunction());
    }

    //绘制事件
    static void staticOnPaintUpdated(wkeWebView webView, HWND m_hWnd, const HDC hdc, int x, int y, int cx, int cy) {
        if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(m_hWnd, GWL_EXSTYLE))) {
            RECT rectDest;
            GetWindowRect(m_hWnd, &rectDest);

            SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
            POINT pointDest = { rectDest.left, rectDest.top };
            POINT pointSource = { 0, 0 };

            HDC hdcScreen = GetDC(NULL);
            //HDC hdcMemory = CreateCompatibleDC(hdcScreen);
            //HBITMAP hbmpMemory = CreateCompatibleBitmap(hdcScreen, sizeDest.cx, sizeDest.cy);
            //HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMemory, hbmpMemory);
            //BitBlt(hdcMemory, 0, 0, sizeDest.cx, sizeDest.cy, wkeGetViewDC(this), 0, 0, SRCCOPY);

            BLENDFUNCTION blend = { 0 };
            memset(&blend, 0, sizeof(blend));
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = 255;
            blend.AlphaFormat = AC_SRC_ALPHA;
            ::UpdateLayeredWindow(m_hWnd, hdcScreen, &pointDest, &sizeDest, wkeGetViewDC(webView), &pointSource, RGB(0, 0, 0), &blend, ULW_ALPHA);

            //SelectObject(hdcMemory, (HGDIOBJ)hbmpOld);
            //DeleteObject((HGDIOBJ)hbmpMemory);
            //DeleteDC(hdcMemory);

            ::ReleaseDC(NULL, hdcScreen);
        } else {
            RECT rc = { x, y, x + cx, y + cy };
            BOOL b = InvalidateRect(m_hWnd, &rc, TRUE);
        }
    }

    static const WCHAR* kPrppW;

    //窗口消息处理
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        Window *win = (Window *)GetPropW(hwnd, kPrppW);
        if (!win) {
            if (message == WM_CREATE) {
                LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
                Window *win = (Window *)cs->lpCreateParams;
                wkeSetHandle(win->m_webContents->m_view, hwnd);
                SetPropW(hwnd, kPrppW, (HANDLE)win);
                SetTimer(hwnd, (UINT_PTR)win, 70, NULL);
                return 0;
            }
        }
        if (!win)
            return DefWindowProcW(hwnd, message, wParam, lParam);
        wkeWebView pthis = win->m_webContents->m_view;
        if (!pthis)
            return DefWindowProcW(hwnd, message, wParam, lParam);
        switch (message) {
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, (UINT_PTR)win);
            RemovePropW(hwnd, kPrppW);
            wkeDestroyWebView(pthis);
            return 0;

        case WM_TIMER:
            wkeRepaintIfNeeded(pthis);
            return 0;

        case WM_PAINT: {
            wkeRepaintIfNeeded(pthis);

            PAINTSTRUCT ps = { 0 };
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rcClip = ps.rcPaint;

            RECT rcClient;
            GetClientRect(hwnd, &rcClient);

            RECT rcInvalid = rcClient;
            if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
                IntersectRect(&rcInvalid, &rcClip, &rcClient);

            int srcX = rcInvalid.left - rcClient.left;
            int srcY = rcInvalid.top - rcClient.top;
            int destX = rcInvalid.left;
            int destY = rcInvalid.top;
            int width = rcInvalid.right - rcInvalid.left;
            int height = rcInvalid.bottom - rcInvalid.top;

            if (0 != width && 0 != height)
                BitBlt(hdc, destX, destY, width, height, wkeGetViewDC(pthis), srcX, srcY, SRCCOPY);

            EndPaint(hwnd, &ps);
        }
                       break;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_SIZE: {
            wkeResize(pthis, LOWORD(lParam), HIWORD(lParam));
            wkeRepaintIfNeeded(pthis);
            return 0;
        }
        case WM_KEYDOWN: {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            if (wkeFireKeyDownEvent(pthis, virtualKeyCode, flags, false))
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

            if (wkeFireKeyUpEvent(pthis, virtualKeyCode, flags, false))
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

            if (wkeFireKeyPressEvent(pthis, charCode, flags, false))
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
                SetFocus(hwnd);
                SetCapture(hwnd);
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

            if (wkeFireMouseEvent(pthis, message, x, y, flags))
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

            if (wkeFireContextMenuEvent(pthis, pt.x, pt.y, flags))
                return 0;
            break;
        }
        case WM_MOUSEWHEEL: {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            ScreenToClient(hwnd, &pt);

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

            if (wkeFireMouseWheelEvent(pthis, pt.x, pt.y, delta, flags))
                return 0;
            break;
        }
        case WM_SETFOCUS:
            wkeSetFocus(pthis);
            return 0;

        case WM_KILLFOCUS:
            wkeKillFocus(pthis);
            return 0;

        case WM_SETCURSOR:
            if (wkeFireWindowsMessage(pthis, hwnd, WM_SETCURSOR, 0, 0, nullptr))
                return 0;
            break;

        case WM_IME_STARTCOMPOSITION: {
            wkeRect caret = wkeGetCaretRect(pthis);

            COMPOSITIONFORM COMPOSITIONFORM;
            COMPOSITIONFORM.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
            COMPOSITIONFORM.ptCurrentPos.x = caret.x;
            COMPOSITIONFORM.ptCurrentPos.y = caret.y;

            HIMC hIMC = ImmGetContext(hwnd);
            ImmSetCompositionWindow(hIMC, &COMPOSITIONFORM);
            ImmReleaseContext(hwnd, hIMC);
        }
            return 0;
        }

        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    static void* WindowNewTask(gin::Dictionary *options) {
        //HandleScope scope(options->isolate());
        Window* win = new Window;
        unsigned styles = 0;
        unsigned styleEx = 0;

        WebContents* web_contents;
        Handle<Object> _web_contents;
        // If no WebContents was passed to the constructor, create it from options.
        if (!options->Get("webContents", &_web_contents)) {
            // Use options.webPreferences to create WebContents.
            gin::Dictionary web_preferences = gin::Dictionary::CreateEmpty(options->isolate());
            options->Get(options::kWebPreferences, &web_preferences);

            // Copy the backgroundColor to webContents.
            v8::Local<v8::Value> value;
            if (options->Get(options::kBackgroundColor, &value))
                web_preferences.Set(options::kBackgroundColor, value);

            v8::Local<v8::Value> transparent;
            if (options->Get("transparent", &transparent))
                web_preferences.Set("transparent", transparent);

            // Offscreen windows are always created frameless.
            bool offscreen;
            if (web_preferences.Get("offscreen", &offscreen) && offscreen) {
                options->Set(options::kFrame, false);
            }
            web_contents = WebContents::create(options->isolate(), web_preferences);
        } else
            web_contents = WebContents::ObjectWrap::Unwrap<WebContents>(_web_contents);
        
        win->m_webContents = web_contents;

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
        if (transparent->IsBoolean() && transparent->ToBoolean()->BooleanValue()) {
            styles = WS_POPUP;
            styleEx = WS_EX_LAYERED;
            wkeSetTransparent(win->m_webContents->m_view, true);
        } else {
            styles = WS_OVERLAPPEDWINDOW;
            styleEx = 0;
        }

        wchar_t s_title[256];
        if (title->IsString()) {
            v8::String::Utf8Value str(title);
            MultiByteToWideChar(CP_UTF8, 0, *str, -1, s_title, 256);
        } else {
            wcscpy(s_title, L"Electron");
        }

        win->m_hWnd = CreateWindowEx(
            styleEx,        // window ex-style
            L"mb_electron_window",    // window class name
            s_title, // window caption
            styles,         // window style
            x->Int32Value(),              // initial x position
            y->Int32Value(),              // initial y position
            width->Int32Value(),          // initial x size
            height->Int32Value(),         // initial y size
            NULL,         // parent window handle
            NULL,           // window menu handle
            GetModuleHandleW(NULL),           // program instance handle
            win);         // creation parameters

        if (!IsWindow(win->m_hWnd))
            return FALSE;

        wkeResize(win->m_webContents->m_view, width->Int32Value(), height->Int32Value());
        wkeOnPaintUpdated(win->m_webContents->m_view, (wkePaintUpdatedCallback)staticOnPaintUpdated, win->m_hWnd);
        wkeLoadHTML(win->m_webContents->m_view, "test");
        wkeLoadURL(win->m_webContents->m_view, "http://www.zerotoken.com");
        ShowWindow(win->m_hWnd, TRUE);
        return win;
    }

    // 不允许转换构造函数进行的隐式转换
    explicit Window() {

    }

    static void *task_WindowFree(Window *data) {
        delete data->m_webContents;
        return NULL;
    }

    ~Window() {
        mainSyncCall((CoreMainTask)task_WindowFree, this);
    }

private:
    // new方法
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        if (args.IsConstructCall()) {
            if (args.Length() > 1) {
                return;
            }
            // 使用new调用 `new Point(...)`
            gin::Dictionary options(args.GetIsolate(), args[0]->ToObject());
            // new一个对象
            Window* win = (Window*)mainSyncCall((CoreMainTask)WindowNewTask, &options);
            // 包装this指针
            win->Wrap(args.This());
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
    static void close(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SendMessage(win->m_hWnd, WM_CLOSE, 0, 0);
    }

    //focus方法
    static void focus(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SetFocus(win->m_hWnd);
    }

    //blur方法
    static void blur(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SetFocus(NULL);
    }

    //isFocused方法
    static void isFocused(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, GetFocus() == win->m_hWnd);
        args.GetReturnValue().Set(ret);
    }

    //show方法
    static void show(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, TRUE);
        ::SetFocus(win->m_hWnd);
    }

    //showInactive方法
    static void showInactive(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, TRUE);
    }

    //hide
    static void hide(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, FALSE);
    }

    //isVisible
    static void isVisible(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsWindowVisible(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //isEnabled
    static void isEnabled(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsWindowEnabled(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //maximize
    static void maximize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_MAXIMIZE);
    }

    //unmaximize
    static void unmaximize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_RESTORE);
    }

    //isMaximized
    static void isMaximized(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsZoomed(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //minimize
    static void minimize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_MINIMIZE);
    }

    //restore
    static void restore(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_RESTORE);
    }

    //isMinimized
    static void isMinimized(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsIconic(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //setFullScreen
    static void setFullScreen(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
            RECT rc;
            HWND hDesk = GetDesktopWindow();
            ::GetWindowRect(hDesk, &rc);
            ::SetWindowLong(win->m_hWnd, GWL_STYLE, GetWindowLong(win->m_hWnd, GWL_STYLE) | WS_BORDER);
            ::SetWindowPos(win->m_hWnd, HWND_TOPMOST, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);
        }
        else {
            ::SetWindowLong(win->m_hWnd, GWL_STYLE, GetWindowLong(win->m_hWnd, GWL_STYLE) ^ WS_BORDER);
        }
    }

    static void getWebContents(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
    WebContents* m_webContents;
    HWND m_hWnd;
};

const WCHAR* Window::kPrppW = L"mele";
Persistent<Function> Window::constructor;

static void Initialize(Local<Object> target, Local<Value> unused, Local<Context> context) {
    Environment* env = Environment::GetCurrent(context);
    Window::Init(target, env);
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

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT(atom_browser_window, Initialize, &nativeHello)

} // atom