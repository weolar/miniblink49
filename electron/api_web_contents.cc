#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "electron.h"
#include "dictionary.h"
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

// 静态方法，用于注册类和方法
void WebContents::init(Local<Object> target, Environment* env) {
    Isolate* isolate = env->isolate();

    // Function模板
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, newFunction);
    // 类名
    tpl->SetClassName(String::NewFromUtf8(isolate, "WebContents"));
    // InternalField
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    v8::Local<v8::Template> t = tpl->InstanceTemplate();
    // 设置Prototype函数

    NODE_SET_METHOD(t, "getId", nullFunction);
    NODE_SET_METHOD(t, "getProcessId", nullFunction);
    NODE_SET_METHOD(t, "equal", nullFunction);
    NODE_SET_METHOD(t, "_loadURL", _loadURL);
    NODE_SET_METHOD(t, "downloadURL", nullFunction);
    NODE_SET_METHOD(t, "_getURL", nullFunction);
    NODE_SET_METHOD(t, "getTitle", nullFunction);
    NODE_SET_METHOD(t, "isLoading", nullFunction);
    NODE_SET_METHOD(t, "isLoadingMainFrame", nullFunction);
    NODE_SET_METHOD(t, "isWaitingForResponse", nullFunction);
    NODE_SET_METHOD(t, "_stop", nullFunction);
    NODE_SET_METHOD(t, "_goBack", nullFunction);
    NODE_SET_METHOD(t, "_goForward", nullFunction);
    NODE_SET_METHOD(t, "_goToOffset", nullFunction);
    NODE_SET_METHOD(t, "isCrashed", nullFunction);
    NODE_SET_METHOD(t, "setUserAgent", nullFunction);
    NODE_SET_METHOD(t, "getUserAgent", nullFunction);
    NODE_SET_METHOD(t, "insertCSS", nullFunction);
    NODE_SET_METHOD(t, "savePage", nullFunction);
    NODE_SET_METHOD(t, "openDevTools", nullFunction);
    NODE_SET_METHOD(t, "closeDevTools", nullFunction);
    NODE_SET_METHOD(t, "isDevToolsOpened", nullFunction);
    NODE_SET_METHOD(t, "isDevToolsFocused", nullFunction);
    NODE_SET_METHOD(t, "enableDeviceEmulation", nullFunction);
    NODE_SET_METHOD(t, "disableDeviceEmulation", nullFunction);
    NODE_SET_METHOD(t, "toggleDevTools", nullFunction);
    NODE_SET_METHOD(t, "inspectElement", nullFunction);
    NODE_SET_METHOD(t, "setAudioMuted", nullFunction);
    NODE_SET_METHOD(t, "isAudioMuted", nullFunction);
    NODE_SET_METHOD(t, "undo", nullFunction);
    NODE_SET_METHOD(t, "redo", nullFunction);
    NODE_SET_METHOD(t, "cut", nullFunction);
    NODE_SET_METHOD(t, "copy", nullFunction);
    NODE_SET_METHOD(t, "paste", nullFunction);
    NODE_SET_METHOD(t, "pasteAndMatchStyle", nullFunction);
    NODE_SET_METHOD(t, "delete", nullFunction);
    NODE_SET_METHOD(t, "selectAll", nullFunction);
    NODE_SET_METHOD(t, "unselect", nullFunction);
    NODE_SET_METHOD(t, "replace", nullFunction);
    NODE_SET_METHOD(t, "replaceMisspelling", nullFunction);
    NODE_SET_METHOD(t, "findInPage", nullFunction);
    NODE_SET_METHOD(t, "stopFindInPage", nullFunction);
    NODE_SET_METHOD(t, "focus", nullFunction);
    NODE_SET_METHOD(t, "isFocused", nullFunction);
    NODE_SET_METHOD(t, "tabTraverse", nullFunction);
    NODE_SET_METHOD(t, "_send", nullFunction);
    NODE_SET_METHOD(t, "sendInputEvent", nullFunction);
    NODE_SET_METHOD(t, "beginFrameSubscription", nullFunction);
    NODE_SET_METHOD(t, "endFrameSubscription", nullFunction);
    NODE_SET_METHOD(t, "startDrag", nullFunction);
    NODE_SET_METHOD(t, "setSize", nullFunction);
    NODE_SET_METHOD(t, "isGuest", nullFunction);
    NODE_SET_METHOD(t, "isOffscreen", nullFunction);
    NODE_SET_METHOD(t, "startPainting", nullFunction);
    NODE_SET_METHOD(t, "stopPainting", nullFunction);
    NODE_SET_METHOD(t, "isPainting", nullFunction);
    NODE_SET_METHOD(t, "setFrameRate", nullFunction);
    NODE_SET_METHOD(t, "getFrameRate", nullFunction);
    NODE_SET_METHOD(t, "invalidate", nullFunction);
    NODE_SET_METHOD(t, "getType", nullFunction);
    NODE_SET_METHOD(t, "getWebPreferences", nullFunction);
    NODE_SET_METHOD(t, "getOwnerBrowserWindow", nullFunction);
    NODE_SET_METHOD(t, "hasServiceWorker", nullFunction);
    NODE_SET_METHOD(t, "unregisterServiceWorker", nullFunction);
    NODE_SET_METHOD(t, "inspectServiceWorker", nullFunction);
    NODE_SET_METHOD(t, "print", nullFunction);
    NODE_SET_METHOD(t, "_printToPDF", nullFunction);
    NODE_SET_METHOD(t, "addWorkSpace", nullFunction);
    NODE_SET_METHOD(t, "reNullWorkSpace", nullFunction);
    NODE_SET_METHOD(t, "showDefinitionForSelection", nullFunction);
    NODE_SET_METHOD(t, "copyImageAt", nullFunction);
    NODE_SET_METHOD(t, "capturePage", nullFunction);
    NODE_SET_METHOD(t, "setEmbedder", nullFunction);
    NODE_SET_PROTOTYPE_METHOD(tpl, "id", nullFunction);
    NODE_SET_PROTOTYPE_METHOD(tpl, "session", nullFunction);
    NODE_SET_PROTOTYPE_METHOD(tpl, "hostWebContents", nullFunction);
    NODE_SET_PROTOTYPE_METHOD(tpl, "devToolsWebContents", nullFunction);
    NODE_SET_PROTOTYPE_METHOD(tpl, "debugger", nullFunction);
    // 设置constructor
    constructor.Reset(isolate, tpl->GetFunction());
    // export `BrowserWindow`
    target->Set(String::NewFromUtf8(isolate, "WebContents"), tpl->GetFunction());
}

WebContents* WebContents::create(Isolate* isolate, gin::Dictionary options) {
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
void WebContents::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
		con->Wrap(args.This(), isolate);
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

static void* WebContentsLoadUrlTask(const v8::FunctionCallbackInfo<v8::Value>* args) {
    Isolate* isolate = args->GetIsolate();
    HandleScope scope(isolate);
    // 解封this指针
    WebContents* con = ObjectWrap::Unwrap<WebContents>(args->Holder());
    if ((*args)[0]->IsString()) {
        v8::String::Utf8Value str((*args)[0]);
       
    }

    return NULL;
}

void WebContents::_loadURL(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (mainSyncCall(_loadURL, args)) {
        return;
    }
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    // 解封this指针
    WebContents* con = ObjectWrap::Unwrap<WebContents>(args.Holder());
    if (args[0]->IsString()) {
        v8::String::Utf8Value str(args[0]);
        wkeLoadURL(con->m_view, *str);
    }
}

// 空实现
void WebContents::nullFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

Persistent<Function> WebContents::constructor;

static void initializeWebContentApi(Local<Object> target,
    Local<Value> unused,
    Local<Context> context) {
    Environment* env = Environment::GetCurrent(context);
    WebContents::init(target, env);
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT(atom_browser_web_contents, initializeWebContentApi, &nativeHello)

} // atom
