#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "electron.h"
#include "dictionary.h"
#include "api_web_contents.h"

using namespace v8;
using namespace node;

#pragma warning(push)
#pragma warning(disable:4309)
#pragma warning(disable:4838)
static const char helloNative[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };
#pragma warning(pop)

static node_native nativeHello{ "hello", helloNative, sizeof(helloNative) };

// 静态方法，用于注册类和方法
void WebContents::Init(Local<Object> target, Environment* env) {
	Isolate* isolate = env->isolate();

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
	NODE_SET_METHOD(t, "_loadURL", _LoadURL);
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

WebContents* WebContents::Create(Isolate* isolate, gin::Dictionary options) {
	HandleScope scope(isolate);
	// 使用`Point(...)`
	const int argc = 1;
	Local<Value> argv[argc] = { gin::ConvertToV8(isolate,options) };
	// 使用constructor构建Function
	Local<Function> cons = Local<Function>::New(isolate, constructor);

	return WebContents::ObjectWrap::Unwrap<WebContents>(cons->NewInstance(argc, argv));
}
WebContents::WebContents(v8::Isolate* isolate, const gin::Dictionary& options) {
	m_view = wkeCreateWebView();
}
WebContents::~WebContents() {
}
// new方法
void WebContents::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);

	if (args.IsConstructCall()) {
		if (args.Length() > 1) {
			return;
		}
		// 使用new调用 `new Point(...)`
		gin::Dictionary options(args.GetIsolate(), args[0]->ToObject());
		// new一个对象
		WebContents* con = new WebContents(isolate, options);
		// 包装this指针
		con->Wrap(args.This());
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
// _loadurl
static void *task_WebContents_LoadURL(const v8::FunctionCallbackInfo<v8::Value>* args) {
	Isolate* isolate = args->GetIsolate();
	HandleScope scope(isolate);
	// 解封this指针
	WebContents* con = ObjectWrap::Unwrap<WebContents>(args->Holder());
	if ((*args)[0]->IsString()) {
		v8::String::Utf8Value str((*args)[0]);
		wkeLoadURL(con->m_view, *str);
	}

	return NULL;
}
void WebContents::_LoadURL(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	// 解封this指针
	WebContents* con = ObjectWrap::Unwrap<WebContents>(args.Holder());
	if (args[0]->IsString()) {
		v8::String::Utf8Value str(args[0]);
		main_async_call((core_main_task)task_WebContents_LoadURL, (void*)&args);
		//等待主线程任务完成
		main_async_wait();
	}
}

// 空实现
void WebContents::Null(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

Persistent<Function> WebContents::constructor;
static void Initialize(Local<Object> target,
	Local<Value> unused,
	Local<Context> context) {
	Environment* env = Environment::GetCurrent(context);
	WebContents::Init(target, env);
}
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT(atom_browser_web_contents, Initialize, &nativeHello)