#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "electron.h"
#include "dictionary.h"
using namespace v8;
using namespace node;
const char hello_native[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };

node_native native_hello{ "hello", hello_native, sizeof(hello_native) };

// 继承node的ObjectWrap，一般自定义C++类都应该继承node的ObjectWrap
class Window :
	public node::ObjectWrap {
public:
	// 静态方法，用于注册类和方法
	static void Init(Local<Object> target, Environment* env) {
		Isolate* isolate = Isolate::GetCurrent();

		// Function模板
		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
		// 类名
		tpl->SetClassName(String::NewFromUtf8(isolate, "BrowserWindow"));
		// InternalField
		tpl->InstanceTemplate()->SetInternalFieldCount(1);
		v8::Local<v8::Template> t = tpl->InstanceTemplate();
		// 设置Prototype函数
		NODE_SET_METHOD(t, "close", Clos);
		NODE_SET_METHOD(t, "focus", Focus);
		NODE_SET_METHOD(t, "blur", Blur);
		NODE_SET_METHOD(t, "isFocused", IsFocused);
		NODE_SET_METHOD(t, "show", Show);
		NODE_SET_METHOD(t, "showInactive", ShowInactive);
		NODE_SET_METHOD(t, "hide", Hide);
		NODE_SET_METHOD(t, "isVisible", IsVisible);
		NODE_SET_METHOD(t, "isEnabled", IsEnabled);
		NODE_SET_METHOD(t, "maximize", Maximize);
		NODE_SET_METHOD(t, "unmaximize", Unmaximize);
		NODE_SET_METHOD(t, "isMaximized", IsMaximized);
		NODE_SET_METHOD(t, "minimize", Minimize);
		NODE_SET_METHOD(t, "restore", Restore);
		NODE_SET_METHOD(t, "isMinimized", IsMinimized);
		NODE_SET_METHOD(t, "setFullScreen", SetFullScreen);
		NODE_SET_METHOD(t, "isFullScreen", Move);
		NODE_SET_METHOD(t, "setAspectRatio", toString);
		NODE_SET_METHOD(t, "previewFile", Move);
		NODE_SET_METHOD(t, "toString", toString);
		NODE_SET_METHOD(t, "closeFilePreview", Move);
		NODE_SET_METHOD(t, "setParentWindow", toString);
		NODE_SET_METHOD(t, "getParentWindow", toString);
		NODE_SET_METHOD(t, "getChildWindows", Move);
		NODE_SET_METHOD(t, "isModal", toString);
		NODE_SET_METHOD(t, "getNativeWindowHandle", Move);
		NODE_SET_METHOD(t, "getBounds", toString);
		NODE_SET_METHOD(t, "setBounds", Move);
		NODE_SET_METHOD(t, "getSize", toString);
		NODE_SET_METHOD(t, "setSize", Move);
		NODE_SET_METHOD(t, "getContentBounds", toString);
		NODE_SET_METHOD(t, "setContentBounds", toString);
		NODE_SET_METHOD(t, "getContentSize", Move);
		NODE_SET_METHOD(t, "setContentSize", toString);
		NODE_SET_METHOD(t, "setMinimumSize", Move);
		NODE_SET_METHOD(t, "getMinimumSize", toString);
		NODE_SET_METHOD(t, "setMaximumSize", Move);
		NODE_SET_METHOD(t, "getMaximumSize", toString);
		NODE_SET_METHOD(t, "setSheetOffset", Move);
		NODE_SET_METHOD(t, "setResizable", toString);
		NODE_SET_METHOD(t, "isResizable", toString);
		NODE_SET_METHOD(t, "setMovable", Move);
		NODE_SET_METHOD(t, "isMovable", toString);
		NODE_SET_METHOD(t, "setMinimizable", Move);
		NODE_SET_METHOD(t, "isMinimizable", toString);
		NODE_SET_METHOD(t, "isMaximizable", Move);
		NODE_SET_METHOD(t, "setFullScreenable", toString);
		NODE_SET_METHOD(t, "isFullScreenable", Move);
		NODE_SET_METHOD(t, "setClosable", toString);
		NODE_SET_METHOD(t, "isClosable", Move);
		NODE_SET_METHOD(t, "setAlwaysOnTop", toString);
		NODE_SET_METHOD(t, "isAlwaysOnTop", toString);
		NODE_SET_METHOD(t, "center", Move);
		NODE_SET_METHOD(t, "setPosition", toString);
		NODE_SET_METHOD(t, "getPosition", Move);
		NODE_SET_METHOD(t, "setTitle", toString);
		NODE_SET_METHOD(t, "getTitle", Move);
		NODE_SET_METHOD(t, "flashFrame", toString);
		NODE_SET_METHOD(t, "setSkipTaskbar", Move);
		NODE_SET_METHOD(t, "setKiosk", toString);
		NODE_SET_METHOD(t, "isKiosk", toString);
		NODE_SET_METHOD(t, "setBackgroundColor", toString);
		NODE_SET_METHOD(t, "setHasShadow", Move);
		NODE_SET_METHOD(t, "hasShadow", toString);
		NODE_SET_METHOD(t, "setRepresentedFilename", Move);
		NODE_SET_METHOD(t, "getRepresentedFilename", toString);
		NODE_SET_METHOD(t, "setDocumentEdited", Move);
		NODE_SET_METHOD(t, "isDocumentEdited", toString);
		NODE_SET_METHOD(t, "setIgnoreMouseEvents", Move);
		NODE_SET_METHOD(t, "setContentProtection", toString);
		NODE_SET_METHOD(t, "setFocusable", toString);
		NODE_SET_METHOD(t, "focusOnWebView", toString);
		NODE_SET_METHOD(t, "blurWebView", Move);
		NODE_SET_METHOD(t, "isWebViewFocused", toString);
		NODE_SET_METHOD(t, "setOverlayIcon", Move);
		NODE_SET_METHOD(t, "setThumbarButtons", toString);
		NODE_SET_METHOD(t, "setMenu", Move);
		NODE_SET_METHOD(t, "setAutoHideMenuBar", toString);
		NODE_SET_METHOD(t, "isMenuBarAutoHide", Move);
		NODE_SET_METHOD(t, "setMenuBarVisibility", toString);
		NODE_SET_METHOD(t, "isMenuBarVisible", Move);
		NODE_SET_METHOD(t, "setVisibleOnAllWorkspaces", toString);
		NODE_SET_METHOD(t, "isVisibleOnAllWorkspaces", Move);
		NODE_SET_METHOD(t, "setVibrancy", toString);
		NODE_SET_METHOD(t, "hookWindowMessage", toString);
		NODE_SET_METHOD(t, "isWindowMessageHooked", toString);
		NODE_SET_METHOD(t, "unhookWindowMessage", Move);
		NODE_SET_METHOD(t, "unhookAllWindowMessages", toString);
		NODE_SET_METHOD(t, "setThumbnailClip", Move);
		NODE_SET_METHOD(t, "setThumbnailToolTip", toString);
		NODE_SET_METHOD(t, "setAppDetails", Move);
		NODE_SET_METHOD(t, "setIcon", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "id", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "webContents", toString);

		// 设置constructor
		constructor.Reset(isolate, tpl->GetFunction());
		// export `BrowserWindow`
		target->Set(String::NewFromUtf8(isolate, "BrowserWindow"), tpl->GetFunction());
	}
	//窗口消息处理
	static LRESULT CALLBACK _windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Window *win = (Window *)GetPropW(hwnd, L"mele");
		if (!win) {
			if (message == WM_CREATE) {
				LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
				Window *win = (Window *)cs->lpCreateParams;
				wkeSetHandle(win->m_view, hwnd);
				SetPropW(hwnd, L"mele", (HANDLE)win);
			}
		}
		if (!win)
			return DefWindowProcW(hwnd, message, wParam, lParam);
		wkeWebView pthis = win->m_view;
		if (!pthis)
			return DefWindowProcW(hwnd, message, wParam, lParam);
		switch (message) {
		case WM_CREATE:
			SetTimer(hwnd, (UINT_PTR)pthis, 70, NULL);
			return 0;

		case WM_CLOSE:
			ShowWindow(hwnd, SW_HIDE);
			DestroyWindow(hwnd);
			return 0;

		case WM_DESTROY:
			KillTimer(hwnd, (UINT_PTR)pthis);
			RemovePropW(hwnd, L"wke");
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
private:
	static void *task_WindowNew(gin::Dictionary *options)
	{
		Window* win = new Window;
		unsigned styles = 0;
		unsigned styleEx = 0;
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
		win->m_view = wkeCreateWebView();
		if (transparent->IsBoolean() && transparent->ToBoolean()->BooleanValue()) {
			styles = WS_POPUP;
			styleEx = WS_EX_LAYERED;
			wkeSetTransparent(win->m_view, true);
		}
		else {
			styles = WS_OVERLAPPEDWINDOW;
			styleEx = 0;
		}
		wchar_t s_title[256];
		if (title->IsString()) {
			v8::String::Utf8Value str(title);
			MultiByteToWideChar(CP_UTF8, 0, *str, -1, s_title, 256);
		}
		else {
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
		int a = width->Int32Value();
		wkeResize(win->m_view, a, height->Int32Value());

		ShowWindow(win->m_hWnd, TRUE);
		return win;
	}
	// 不允许转换构造函数进行的隐式转换
	explicit Window() {

	}
	static void *task_WindowFree(Window *data) {
		wkeDestroyWebView(data->m_view);
		return NULL;
	}
	~Window() {
		main_async_call((core_main_task)task_WindowFree, this);
		//等待主线程任务完成
		main_async_wait();
	}
	// new方法
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);

		if (args.IsConstructCall()) {
			if (args.Length() > 1) {
				return;
			}
			// 使用new调用 `new Point(...)`
			gin::Dictionary options(args.GetIsolate(), args[0]->ToObject());
			// new一个对象
			main_async_call((core_main_task)task_WindowNew, &options);
			//等待主线程任务完成
			Window* win = (Window*)main_async_wait();
			// 包装this指针
			win->Wrap(args.This());
			args.GetReturnValue().Set(args.This());
		}
		else {
			// 使用`Point(...)`
			const int argc = 2;
			Local<Value> argv[argc] = { args[0], args[1] };
			// 使用constructor构建Function
			Local<Function> cons = Local<Function>::New(isolate, constructor);
			args.GetReturnValue().Set(cons->NewInstance(argc, argv));
		}
	}
	//clos方法
	static void Clos(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		SendMessage(win->m_hWnd, WM_CLOSE, 0, 0);
	}
	//focus方法
	static void Focus(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		SetFocus(win->m_hWnd);
	}
	//blur方法
	static void Blur(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		SetFocus(NULL);
	}
	//isFocused方法
	static void IsFocused(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		Local<Boolean> ret = Boolean::New(isolate, GetFocus() == win->m_hWnd);
		args.GetReturnValue().Set(ret);
	}
	//show方法
	static void Show(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		ShowWindow(win->m_hWnd, TRUE);
		SetFocus(win->m_hWnd);
	}
	//showInactive方法
	static void ShowInactive(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		ShowWindow(win->m_hWnd, TRUE);
	}
	//hide
	static void Hide(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		ShowWindow(win->m_hWnd, FALSE);
	}
	//isVisible
	static void IsVisible(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		Local<Boolean> ret = Boolean::New(isolate, IsWindowVisible(win->m_hWnd));
		args.GetReturnValue().Set(ret);
	}
	//isEnabled
	static void IsEnabled(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		Local<Boolean> ret = Boolean::New(isolate, IsWindowEnabled(win->m_hWnd));
		args.GetReturnValue().Set(ret);
	}
	//maximize
	static void Maximize(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		ShowWindow(win->m_hWnd, SW_MAXIMIZE);
	}
	//unmaximize
	static void Unmaximize(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		ShowWindow(win->m_hWnd, SW_RESTORE);
	}
	//isMaximized
	static void IsMaximized(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		Local<Boolean> ret = Boolean::New(isolate, IsZoomed(win->m_hWnd));
		args.GetReturnValue().Set(ret);
	}
	//minimize
	static void Minimize(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		ShowWindow(win->m_hWnd, SW_MINIMIZE);
	}
	//restore
	static void Restore(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		ShowWindow(win->m_hWnd, SW_RESTORE);
	}
	//isMinimized
	static void IsMinimized(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		Local<Boolean> ret = Boolean::New(isolate, IsIconic(win->m_hWnd));
		args.GetReturnValue().Set(ret);
	}
	//setFullScreen
	static void SetFullScreen(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
		if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
			RECT rc;
			HWND hDesk = GetDesktopWindow();
			GetWindowRect(hDesk, &rc);
			SetWindowLong(win->m_hWnd, GWL_STYLE, GetWindowLong(win->m_hWnd, GWL_STYLE) | WS_BORDER);
			SetWindowPos(win->m_hWnd, HWND_TOPMOST, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);
		}
		else {
			SetWindowLong(win->m_hWnd, GWL_STYLE, GetWindowLong(win->m_hWnd, GWL_STYLE) ^ WS_BORDER);
		}
	}

	// move(x, y)
	static void Move(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* point = ObjectWrap::Unwrap<Window>(args.Holder());
		// 参数

	}
	// toString方法
	static void toString(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* point = ObjectWrap::Unwrap<Window>(args.Holder());
		// 设置返回值
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "aa"));
	}
	static v8::Persistent<v8::Function> constructor;
private:
	wkeWebView m_view;
	HWND m_hWnd;
};
Persistent<Function> Window::constructor;
void Initialize(Local<Object> target,
	Local<Value> unused,
	Local<Context> context) {
	Environment* env = Environment::GetCurrent(context);
	Window::Init(target, env);
	WNDCLASS wndClass = { 0 };
	if (!GetClassInfoW(NULL, L"mb_electron_window", &wndClass)) {
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = &Window::_windowProc;
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
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT(atom_browser_window, Initialize, &native_hello)