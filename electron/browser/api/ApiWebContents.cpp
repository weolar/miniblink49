
#include "ApiWebContents.h"

#include <node_object_wrap.h>
#include "wke.h"
#include "common/ThreadCall.h"
#include "gin/dictionary.h"
#include "NodeRegisterHelp.h"

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
void WebContents::init(v8::Local<v8::Object> target, Environment* env) {
    v8::Isolate* isolate = env->isolate();

    // Function模板
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, newFunction);
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
    NODE_SET_METHOD(t, "_getURL", _getURL);
    NODE_SET_METHOD(t, "getTitle", getTitle);
    NODE_SET_METHOD(t, "isLoading", isLoading);
    NODE_SET_METHOD(t, "isLoadingMainFrame", isLoadingMainFrame);
    NODE_SET_METHOD(t, "isWaitingForResponse", isWaitingForResponse);
    NODE_SET_METHOD(t, "_stop", _stop);
    NODE_SET_METHOD(t, "_goBack", _goBack);
    NODE_SET_METHOD(t, "_goForward", _goForward);
    NODE_SET_METHOD(t, "_goToOffset", _goToOffset);
    NODE_SET_METHOD(t, "isCrashed", isCrashed);
    NODE_SET_METHOD(t, "setUserAgent", setUserAgent);
    NODE_SET_METHOD(t, "getUserAgent", getUserAgent);
    NODE_SET_METHOD(t, "insertCSS", insertCSS);
    NODE_SET_METHOD(t, "savePage", savePage);
    NODE_SET_METHOD(t, "openDevTools", openDevTools);
    NODE_SET_METHOD(t, "closeDevTools", closeDevTools);
    NODE_SET_METHOD(t, "isDevToolsOpened", isDevToolsOpened);
    NODE_SET_METHOD(t, "isDevToolsFocused", isDevToolsFocused);
    NODE_SET_METHOD(t, "enableDeviceEmulation", enableDeviceEmulation);
    NODE_SET_METHOD(t, "disableDeviceEmulation", disableDeviceEmulation);
    NODE_SET_METHOD(t, "toggleDevTools", toggleDevTools);
    NODE_SET_METHOD(t, "inspectElement", inspectElement);
    NODE_SET_METHOD(t, "setAudioMuted", setAudioMuted);
    NODE_SET_METHOD(t, "isAudioMuted", isAudioMuted);
    NODE_SET_METHOD(t, "undo", undo);
    NODE_SET_METHOD(t, "redo", redo);
    NODE_SET_METHOD(t, "cut", cut);
    NODE_SET_METHOD(t, "copy", copy);
    NODE_SET_METHOD(t, "paste", paste);
    NODE_SET_METHOD(t, "pasteAndMatchStyle", pasteAndMatchStyle);
    NODE_SET_METHOD(t, "delete", _delete);
    NODE_SET_METHOD(t, "selectAll", selectAll);
    NODE_SET_METHOD(t, "unselect", unselect);
    NODE_SET_METHOD(t, "replace", replace);
    NODE_SET_METHOD(t, "replaceMisspelling", replaceMisspelling);
    NODE_SET_METHOD(t, "findInPage", findInPage);
    NODE_SET_METHOD(t, "stopFindInPage", stopFindInPage);
    NODE_SET_METHOD(t, "focus", focus);
    NODE_SET_METHOD(t, "isFocused", isFocused);
    NODE_SET_METHOD(t, "tabTraverse", tabTraverse);
    NODE_SET_METHOD(t, "_send", _send);
    NODE_SET_METHOD(t, "sendInputEvent", sendInputEvent);
    NODE_SET_METHOD(t, "beginFrameSubscription", beginFrameSubscription);
    NODE_SET_METHOD(t, "endFrameSubscription", endFrameSubscription);
    NODE_SET_METHOD(t, "startDrag", startDrag);
    NODE_SET_METHOD(t, "setSize", setSize);
    NODE_SET_METHOD(t, "isGuest", isGuest);
    NODE_SET_METHOD(t, "isOffscreen", isOffscreen);
    NODE_SET_METHOD(t, "startPainting", startPainting);
    NODE_SET_METHOD(t, "stopPainting", stopPainting);
    NODE_SET_METHOD(t, "isPainting", isPainting);
    NODE_SET_METHOD(t, "setFrameRate", setFrameRate);
    NODE_SET_METHOD(t, "getFrameRate", getFrameRate);
    NODE_SET_METHOD(t, "invalidate", invalidate);
    NODE_SET_METHOD(t, "getType", getType);
    NODE_SET_METHOD(t, "getWebPreferences", getWebPreferences);
    NODE_SET_METHOD(t, "getOwnerBrowserWindow", getOwnerBrowserWindow);
    NODE_SET_METHOD(t, "hasServiceWorker", hasServiceWorker);
    NODE_SET_METHOD(t, "unregisterServiceWorker", unregisterServiceWorker);
    NODE_SET_METHOD(t, "inspectServiceWorker", inspectServiceWorker);
    NODE_SET_METHOD(t, "print", print);
    NODE_SET_METHOD(t, "_printToPDF", _printToPDF);
    NODE_SET_METHOD(t, "addWorkSpace", addWorkSpace);
    NODE_SET_METHOD(t, "reNullWorkSpace", reNullWorkSpace);
    NODE_SET_METHOD(t, "showDefinitionForSelection", showDefinitionForSelection);
    NODE_SET_METHOD(t, "copyImageAt", copyImageAt);
    NODE_SET_METHOD(t, "capturePage", capturePage);
    NODE_SET_METHOD(t, "setEmbedder", setEmbedder);
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
    v8::Local<v8::Value> argv[argc] = { gin::ConvertToV8(isolate,options) };
    // 使用constructor构建Function
    v8::Local<v8::Function> cons = Local<Function>::New(isolate, constructor);

    return WebContents::ObjectWrap::Unwrap<WebContents>(cons->NewInstance(argc, argv));
}

WebContents::WebContents() {
    m_view = wkeCreateWebView();
}

WebContents::~WebContents() {
    //wkeDestroyWebView(m_view);
}

// new方法
void WebContents::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        if (args.Length() > 1)
            return;
        
        // 使用new调用 `new Point(...)`
        gin::Dictionary options(args.GetIsolate(), args[0]->ToObject());
        // new一个对象
        WebContents* webContents = nullptr;
        WebContents** webContentsPtr = &webContents;
        ThreadCall::callBlinkThreadSync([webContentsPtr] {
            *webContentsPtr = new WebContents();
        });
        // 包装this指针
        webContents->Wrap(args.This(), isolate);
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

void WebContents::_loadURL(const v8::FunctionCallbackInfo<v8::Value>& args) {   
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());
    if (!args[0]->IsString())
        return;
        
    v8::String::Utf8Value strV8(args[0]);
    std::string* str = new std::string(*strV8, strV8.length());

    ThreadCall::callBlinkThreadSync([webContents, str] {
        wkeLoadURL(webContents->m_view, str->c_str());
        delete str;
    });
}

void WebContents::_getURL(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ThreadCall::callBlinkThreadSync([args] {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

        Local<v8::String> url;
        String::NewFromUtf8(isolate, wkeGetURL(webContents->m_view), NewStringType::kNormal).ToLocal(&url);
        args.GetReturnValue().Set(url);
    });
}

void WebContents::getTitle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ThreadCall::callBlinkThreadSync([args] {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

        Local<v8::String> title;
        String::NewFromUtf8(isolate, wkeGetTitle(webContents->m_view), NewStringType::kNormal).ToLocal(&title);
        args.GetReturnValue().Set(title);
    });
}

void WebContents::isLoading(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ThreadCall::callBlinkThreadSync([args] {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

        Local<Boolean> ret = Boolean::New(isolate, wkeIsLoading(webContents->m_view));
        args.GetReturnValue().Set(ret);
    });
}

void WebContents::isLoadingMainFrame(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::isWaitingForResponse(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::_stop(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    
    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeStopLoading(webContents->m_view);
    });
}

void WebContents::_goBack(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    
    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeGoBack(webContents->m_view);
    });
}

void WebContents::_goForward(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
   
    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeGoForward(webContents->m_view);
    });
}

void WebContents::_goToOffset(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::isCrashed(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<Boolean> ret = Boolean::New(isolate, false);
    args.GetReturnValue().Set(ret);
}

void WebContents::setUserAgent(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());
    if (!args[0]->IsString())
        return;

    v8::String::Utf8Value strV8(args[0]);
    std::string* str = new std::string(*strV8, strV8.length());

    ThreadCall::callBlinkThreadSync([webContents, str] {
        wkeSetUserAgent(webContents->m_view, str->c_str());
        delete str;
    });
}

void WebContents::getUserAgent(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::insertCSS(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::savePage(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::openDevTools(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::closeDevTools(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::isDevToolsOpened(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::isDevToolsFocused(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::enableDeviceEmulation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::disableDeviceEmulation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::toggleDevTools(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::inspectElement(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::setAudioMuted(const v8::FunctionCallbackInfo<v8::Value>& args) {
    /*Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeSetMediaVolume(webContents->m_view, 0.0);
    });*/
}

void WebContents::isAudioMuted(const v8::FunctionCallbackInfo<v8::Value>& args) {
    /*ThreadCall::callBlinkThreadSync([args] {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

        Local<Boolean> ret = Boolean::New(isolate, wkeIsLoading(webContents->m_view));
        args.GetReturnValue().Set(ret);
    });*/
}

void WebContents::undo(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeEditorUndo(webContents->m_view);
    });
}

void WebContents::redo(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeEditorRedo(webContents->m_view);
    });
}

void WebContents::cut(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeEditorCut(webContents->m_view);
    });
}

void WebContents::copy(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    
    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeEditorCopy(webContents->m_view);
    });
}

void WebContents::paste(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeEditorPaste(webContents->m_view);
    });
}

void WebContents::pasteAndMatchStyle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::_delete(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    
    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeEditorDelete(webContents->m_view);
    });
}

void WebContents::selectAll(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    
    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeEditorSelectAll(webContents->m_view);
    });
}

void WebContents::unselect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeEditorUnSelect(webContents->m_view);
    });
}

void WebContents::replace(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::replaceMisspelling(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::findInPage(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::stopFindInPage(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::focus(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    
    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeSetFocus(webContents->m_view);
    });
}

void WebContents::isFocused(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::tabTraverse(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::_send(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::sendInputEvent(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::beginFrameSubscription(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::endFrameSubscription(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::startDrag(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::setSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::isGuest(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::isOffscreen(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::startPainting(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::stopPainting(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::isPainting(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::setFrameRate(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::getFrameRate(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::invalidate(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::getType(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::getWebPreferences(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::getOwnerBrowserWindow(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::hasServiceWorker(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::unregisterServiceWorker(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::inspectServiceWorker(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::_printToPDF(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::addWorkSpace(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::reNullWorkSpace(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::showDefinitionForSelection(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::copyImageAt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::capturePage(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

void WebContents::setEmbedder(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //todo
}

// 空实现
void WebContents::nullFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

Persistent<Function> WebContents::constructor;

static void initializeWebContentApi(Local<Object> target,
    v8::Local<Value> unused,
    v8::Local<Context> context) {
    Environment* env = Environment::GetCurrent(context);
    WebContents::init(target, env);
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_web_contents, initializeWebContentApi, &nativeHello)

} // atom

