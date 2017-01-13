#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "electron.h"
#include "dictionary.h"
using namespace v8;
using namespace node;
static const char hello_native[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };

static node_native native_hello{ "hello", hello_native, sizeof(hello_native) };

// 继承node的ObjectWrap，一般自定义C++类都应该继承node的ObjectWrap
class WebContents :
	public node::ObjectWrap {
public:
	// 静态方法，用于注册类和方法
	static void Init(Local<Object> target, Environment* env) {
		Isolate* isolate = Isolate::GetCurrent();

		// Function模板
		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
		// 类名
		tpl->SetClassName(String::NewFromUtf8(isolate, "WebContents"));
		// InternalField
		tpl->InstanceTemplate()->SetInternalFieldCount(1);
		v8::Local<v8::Template> t = tpl->InstanceTemplate();
		// 设置Prototype函数
		
		NODE_SET_METHOD(t, "getId", Null);
		NODE_SET_METHOD(t, "getProcessId", Null);
		NODE_SET_METHOD(t, "equal", Null);
		NODE_SET_METHOD(t, "_loadURL", Null);
		NODE_SET_METHOD(t, "downloadURL", Null);
		NODE_SET_METHOD(t, "_getURL", Null);
		NODE_SET_METHOD(t, "getTitle", Null);
		NODE_SET_METHOD(t, "isLoading", Null);
		NODE_SET_METHOD(t, "isLoadingMainFrame", Null);
		NODE_SET_METHOD(t, "isWaitingForResponse", Null);
		NODE_SET_METHOD(t, "_stop", Null);
		NODE_SET_METHOD(t, "_goBack", Null);
		NODE_SET_METHOD(t, "_goForward", Null);
		NODE_SET_METHOD(t, "_goToOffset", Null);
		NODE_SET_METHOD(t, "isCrashed", Null);
		NODE_SET_METHOD(t, "setUserAgent", Null);
		NODE_SET_METHOD(t, "getUserAgent", Null);
		NODE_SET_METHOD(t, "insertCSS", Null);
		NODE_SET_METHOD(t, "savePage", Null);
		NODE_SET_METHOD(t, "openDevTools", Null);
		NODE_SET_METHOD(t, "closeDevTools", Null);
		NODE_SET_METHOD(t, "isDevToolsOpened", Null);
		NODE_SET_METHOD(t, "isDevToolsFocused", Null);
		NODE_SET_METHOD(t, "enableDeviceEmulation", Null);
		NODE_SET_METHOD(t, "disableDeviceEmulation", Null);
		NODE_SET_METHOD(t, "toggleDevTools", Null);
		NODE_SET_METHOD(t, "inspectElement", Null);
		NODE_SET_METHOD(t, "setAudioMuted", Null);
		NODE_SET_METHOD(t, "isAudioMuted", Null);
		NODE_SET_METHOD(t, "undo", Null);
		NODE_SET_METHOD(t, "redo", Null);
		NODE_SET_METHOD(t, "cut", Null);
		NODE_SET_METHOD(t, "copy", Null);
		NODE_SET_METHOD(t, "paste", Null);
		NODE_SET_METHOD(t, "pasteAndMatchStyle", Null);
		NODE_SET_METHOD(t, "delete", Null);
		NODE_SET_METHOD(t, "selectAll", Null);
		NODE_SET_METHOD(t, "unselect", Null);
		NODE_SET_METHOD(t, "replace", Null);
		NODE_SET_METHOD(t, "replaceMisspelling", Null);
		NODE_SET_METHOD(t, "findInPage", Null);
		NODE_SET_METHOD(t, "stopFindInPage", Null);
		NODE_SET_METHOD(t, "focus", Null);
		NODE_SET_METHOD(t, "isFocused", Null);
		NODE_SET_METHOD(t, "tabTraverse", Null);
		NODE_SET_METHOD(t, "_send", Null);
		NODE_SET_METHOD(t, "sendInputEvent", Null);
		NODE_SET_METHOD(t, "beginFrameSubscription", Null);
		NODE_SET_METHOD(t, "endFrameSubscription", Null);
		NODE_SET_METHOD(t, "startDrag", Null);
		NODE_SET_METHOD(t, "setSize", Null);
		NODE_SET_METHOD(t, "isGuest", Null);
		NODE_SET_METHOD(t, "isOffscreen", Null);
		NODE_SET_METHOD(t, "startPainting", Null);
		NODE_SET_METHOD(t, "stopPainting", Null);
		NODE_SET_METHOD(t, "isPainting", Null);
		NODE_SET_METHOD(t, "setFrameRate", Null);
		NODE_SET_METHOD(t, "getFrameRate", Null);
		NODE_SET_METHOD(t, "invalidate", Null);
		NODE_SET_METHOD(t, "getType", Null);
		NODE_SET_METHOD(t, "getWebPreferences", Null);
		NODE_SET_METHOD(t, "getOwnerBrowserWindow", Null);
		NODE_SET_METHOD(t, "hasServiceWorker", Null);
		NODE_SET_METHOD(t, "unregisterServiceWorker", Null);
		NODE_SET_METHOD(t, "inspectServiceWorker", Null);
		NODE_SET_METHOD(t, "print", Null);
		NODE_SET_METHOD(t, "_printToPDF", Null);
		NODE_SET_METHOD(t, "addWorkSpace", Null);
		NODE_SET_METHOD(t, "reNullWorkSpace", Null);
		NODE_SET_METHOD(t, "showDefinitionForSelection", Null);
		NODE_SET_METHOD(t, "copyImageAt", Null);
		NODE_SET_METHOD(t, "capturePage", Null);
		NODE_SET_METHOD(t, "setEmbedder", Null);
		NODE_SET_PROTOTYPE_METHOD(tpl, "id", Null);
		NODE_SET_PROTOTYPE_METHOD(tpl, "session", Null);
		NODE_SET_PROTOTYPE_METHOD(tpl, "hostWebContents", Null);
		NODE_SET_PROTOTYPE_METHOD(tpl, "devToolsWebContents", Null);
		NODE_SET_PROTOTYPE_METHOD(tpl, "debugger", Null);
		// 设置constructor
		constructor.Reset(isolate, tpl->GetFunction());
		// export `BrowserWindow`
		target->Set(String::NewFromUtf8(isolate, "WebContents"), tpl->GetFunction());
	}
private:

	// 不允许转换构造函数进行的隐式转换
	explicit WebContents() {

	}
	~WebContents() {
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
			WebContents* win;
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

	
	// 空实现
	static void Null(const v8::FunctionCallbackInfo<v8::Value>& args) {
	}
	static v8::Persistent<v8::Function> constructor;
private:

};
Persistent<Function> WebContents::constructor;
static void Initialize(Local<Object> target,
	Local<Value> unused,
	Local<Context> context) {
	Environment* env = Environment::GetCurrent(context);
	WebContents::Init(target, env);
}
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT(atom_browser_web_contents, Initialize, &native_hello)