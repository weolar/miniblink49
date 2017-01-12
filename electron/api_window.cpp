#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "electron.h"
using namespace v8;
using namespace node;
const char hello_native[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };

node_native native_hello{ "hello", hello_native, sizeof(hello_native) };

// 继承node的ObjectWrap，一般自定义C++类都应该继承node的ObjectWrap
class Window : public node::ObjectWrap {
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

		// 设置Prototype函数
		NODE_SET_PROTOTYPE_METHOD(tpl, "close", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "focus", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "blur", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isFocused", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "show", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "showInactive", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "hide", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isVisible", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isEnabled", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "maximize", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "unmaximize", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isMaximized", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "minimize", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "restore", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isMinimized", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setFullScreen", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isFullScreen", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setAspectRatio", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "previewFile", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "toString", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "closeFilePreview", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setParentWindow", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getParentWindow", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getChildWindows", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isModal", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getNativeWindowHandle", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getBounds", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setBounds", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getSize", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setSize", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getContentBounds", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setContentBounds", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getContentSize", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setContentSize", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setMinimumSize", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getMinimumSize", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setMaximumSize", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getMaximumSize", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setSheetOffset", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setResizable", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isResizable", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setMovable", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isMovable", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setMinimizable", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isMinimizable", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isMaximizable", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setFullScreenable", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isFullScreenable", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setClosable", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isClosable", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setAlwaysOnTop", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isAlwaysOnTop", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "center", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setPosition", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getPosition", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setTitle", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getTitle", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "flashFrame", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setSkipTaskbar", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setKiosk", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isKiosk", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setBackgroundColor", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setHasShadow", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "hasShadow", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setRepresentedFilename", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getRepresentedFilename", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setDocumentEdited", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isDocumentEdited", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setIgnoreMouseEvents", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setContentProtection", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setFocusable", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "focusOnWebView", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "blurWebView", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isWebViewFocused", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setOverlayIcon", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setThumbarButtons", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setMenu", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setAutoHideMenuBar", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isMenuBarAutoHide", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setMenuBarVisibility", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isMenuBarVisible", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setVisibleOnAllWorkspaces", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isVisibleOnAllWorkspaces", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setVibrancy", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "hookWindowMessage", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "isWindowMessageHooked", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "unhookWindowMessage", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "unhookAllWindowMessages", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setThumbnailClip", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setThumbnailToolTip", toString);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setAppDetails", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setIcon", toString);
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
		wkeWebView pthis = (wkeWebView)GetPropW(hwnd, L"wke");
		if (!pthis) {
			if (message == WM_CREATE) {
				LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
				pthis = (wkeWebView)cs->lpCreateParams;
				wkeSetHandle(pthis, hwnd);
				SetPropW(hwnd, L"wke", (HANDLE)pthis);
			}
		}

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
	static void *task_WindowNew(void *data) {
		wkeWebView v = wkeCreateWebView();

		return v;
	}
	// 不允许转换构造函数进行的隐式转换
	explicit Window() {
		main_async_call(task_WindowNew, 0);
		//等待主线程任务完成
		m_view = (wkeWebView)main_async_wait();
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
			// 使用new调用 `new Point(...)`
			double _x = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
			double _y = args[1]->IsUndefined() ? 0 : args[1]->NumberValue();
			// new一个对象
			Window* point = new Window();
			// 包装this指针
			point->Wrap(args.This());
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
	wkeWebView m_view;
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