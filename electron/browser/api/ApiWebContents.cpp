
#include "ApiWebContents.h"

#include <node_object_wrap.h>
#include "wke.h"
#include "common/ThreadCall.h"
#include "common/NodeRegisterHelp.h"
#include "common/IdLiveDetect.h"
#include "gin/dictionary.h"
#include "gin/object_template_builder.h"

using namespace v8;
using namespace node;

namespace atom {

#pragma warning(push)
#pragma warning(disable:4309)
#pragma warning(disable:4838)
static const char helloNative[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };
#pragma warning(pop)

static NodeNative nativeHello{ "hello", helloNative, sizeof(helloNative) };

void WebContents::init(v8::Isolate* isolate, v8::Local<v8::Object> target, Environment* env) {

    v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, WebContents::newFunction);

    prototype->SetClassName(v8::String::NewFromUtf8(isolate, "IpcRenderer"));
    gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
    builder.SetMethod("getId", &WebContents::nullFunction);
    builder.SetMethod("getProcessId", &WebContents::nullFunction);
    builder.SetMethod("equal", &WebContents::nullFunction);
    builder.SetMethod("_loadURL", &WebContents::_loadURLApi);
    builder.SetMethod("downloadURL", &WebContents::nullFunction);
    builder.SetMethod("_getURL", &WebContents::_getURLApi);
    builder.SetMethod("getTitle", &WebContents::getTitleApi);
    builder.SetMethod("isLoading", &WebContents::isLoadingApi);
    builder.SetMethod("isLoadingMainFrame", &WebContents::isLoadingMainFrameApi);
    builder.SetMethod("isWaitingForResponse", &WebContents::isWaitingForResponseApi);
    builder.SetMethod("_stop", &WebContents::_stopApi);
    builder.SetMethod("_goBack", &WebContents::_goBackApi);
    builder.SetMethod("_goForward", &WebContents::_goForwardApi);
    builder.SetMethod("_goToOffset", &WebContents::_goToOffsetApi);
    builder.SetMethod("isCrashed", &WebContents::isCrashedApi);
    builder.SetMethod("setUserAgent", &WebContents::setUserAgentApi);
    builder.SetMethod("getUserAgent", &WebContents::getUserAgentApi);
    builder.SetMethod("insertCSS", &WebContents::insertCSSApi);
    builder.SetMethod("savePage", &WebContents::savePageApi);
    builder.SetMethod("openDevTools", &WebContents::openDevToolsApi);
    builder.SetMethod("closeDevTools", &WebContents::closeDevToolsApi);
    builder.SetMethod("isDevToolsOpened", &WebContents::isDevToolsOpenedApi);
    builder.SetMethod("isDevToolsFocused", &WebContents::isDevToolsFocusedApi);
    builder.SetMethod("enableDeviceEmulation", &WebContents::enableDeviceEmulationApi);
    builder.SetMethod("disableDeviceEmulation", &WebContents::disableDeviceEmulationApi);
    builder.SetMethod("toggleDevTools", &WebContents::toggleDevToolsApi);
    builder.SetMethod("inspectElement", &WebContents::inspectElementApi);
    builder.SetMethod("setAudioMuted", &WebContents::setAudioMutedApi);
    builder.SetMethod("isAudioMuted", &WebContents::isAudioMutedApi);
    builder.SetMethod("undo", &WebContents::undoApi);
    builder.SetMethod("redo", &WebContents::redoApi);
    builder.SetMethod("cut", &WebContents::cutApi);
    builder.SetMethod("copy", &WebContents::copyApi);
    builder.SetMethod("paste", &WebContents::pasteApi);
    builder.SetMethod("pasteAndMatchStyle", &WebContents::pasteAndMatchStyleApi);
    builder.SetMethod("delete", &WebContents::_deleteApi);
    builder.SetMethod("selectAll", &WebContents::selectAllApi);
    builder.SetMethod("unselect", &WebContents::unselectApi);
    builder.SetMethod("replace", &WebContents::replaceApi);
    builder.SetMethod("replaceMisspelling", &WebContents::replaceMisspellingApi);
    builder.SetMethod("findInPage", &WebContents::findInPageApi);
    builder.SetMethod("stopFindInPage", &WebContents::stopFindInPageApi);
    builder.SetMethod("focus", &WebContents::focusApi);
    builder.SetMethod("isFocused", &WebContents::isFocusedApi);
    builder.SetMethod("tabTraverse", &WebContents::tabTraverseApi);
    builder.SetMethod("_send", &WebContents::_sendApi);
    builder.SetMethod("sendInputEvent", &WebContents::sendInputEventApi);
    builder.SetMethod("beginFrameSubscription", &WebContents::beginFrameSubscriptionApi);
    builder.SetMethod("endFrameSubscription", &WebContents::endFrameSubscriptionApi);
    builder.SetMethod("startDrag", &WebContents::startDragApi);
    builder.SetMethod("setSize", &WebContents::setSizeApi);
    builder.SetMethod("isGuest", &WebContents::isGuestApi);
    builder.SetMethod("isOffscreen", &WebContents::isOffscreenApi);
    builder.SetMethod("startPainting", &WebContents::startPaintingApi);
    builder.SetMethod("stopPainting", &WebContents::stopPaintingApi);
    builder.SetMethod("isPainting", &WebContents::isPaintingApi);
    builder.SetMethod("setFrameRate", &WebContents::setFrameRateApi);
    builder.SetMethod("getFrameRate", &WebContents::getFrameRateApi);
    builder.SetMethod("invalidate", &WebContents::invalidateApi);
    builder.SetMethod("getType", &WebContents::getTypeApi);
    builder.SetMethod("getWebPreferences", &WebContents::getWebPreferencesApi);
    builder.SetMethod("getOwnerBrowserWindow", &WebContents::getOwnerBrowserWindowApi);
    builder.SetMethod("hasServiceWorker", &WebContents::hasServiceWorkerApi);
    builder.SetMethod("unregisterServiceWorker", &WebContents::unregisterServiceWorkerApi);
    builder.SetMethod("inspectServiceWorker", &WebContents::inspectServiceWorkerApi);
    builder.SetMethod("print", &WebContents::printApi);
    builder.SetMethod("_printToPDF", &WebContents::_printToPDFApi);
    builder.SetMethod("addWorkSpace", &WebContents::addWorkSpaceApi);
    builder.SetMethod("reNullWorkSpace", &WebContents::reNullWorkSpaceApi);
    builder.SetMethod("showDefinitionForSelection", &WebContents::showDefinitionForSelectionApi);
    builder.SetMethod("copyImageAt", &WebContents::copyImageAtApi);
    builder.SetMethod("capturePage", &WebContents::capturePageApi);
    builder.SetMethod("setEmbedder", &WebContents::setEmbedderApi);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "id", nullFunction);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "session", nullFunction);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "hostWebContents", nullFunction);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "devToolsWebContents", nullFunction);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "debugger", nullFunction);

    constructor.Reset(isolate, prototype->GetFunction());
    target->Set(v8::String::NewFromUtf8(isolate, "WebContents"), prototype->GetFunction());
}

WebContents* WebContents::create(v8::Isolate* isolate, gin::Dictionary options) {
    v8::HandleScope scope(isolate);

    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { gin::ConvertToV8(isolate, options) };
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, constructor);

    v8::MaybeLocal<v8::Object> obj = constructorFunction->NewInstance(argc, argv);
    return (WebContents*)WrappableBase::GetNativePtr(obj.ToLocalChecked(), &kWrapperInfo);
}

WebContents::WebContents(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
    m_id = IdLiveDetect::get()->constructed();
    gin::Wrappable<WebContents>::InitWith(isolate, wrapper);

    WebContents* self = this;
    ThreadCall::callBlinkThreadSync([self] {
        self->m_view = wkeCreateWebView();
    });    
}

WebContents::~WebContents() {
    //wkeDestroyWebView(m_view);
    IdLiveDetect::get()->deconstructed(m_id);
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
        WebContents* webContents = new WebContents(isolate, args.This());
        
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

void WebContents::_loadURLApi(const std::string& url) {
    WebContents* self = this;
    std::string* str = new std::string(url);
    int id = m_id;
    ThreadCall::callBlinkThreadAsync([self, str, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeLoadURL(self->m_view, str->c_str());
        delete str;
    });
}

std::string WebContents::_getURLApi() {
    std::string url;
    WebContents* self = this;
    int id = m_id;
    ThreadCall::callBlinkThreadSync([self, &url] {
        url = wkeGetURL(self->m_view);
    });
    return url;
}

std::string WebContents::getTitleApi() {
    std::string title;
    WebContents* self = this;
    int id = m_id;
    ThreadCall::callBlinkThreadSync([self, &title] {
        title = wkeGetTitle(self->m_view);
    });
    return title;
}

bool WebContents::isLoadingApi() {
    bool isLoading;
    WebContents* self = this;
    int id = m_id;
    ThreadCall::callBlinkThreadSync([self, &isLoading] {
        isLoading = wkeIsLoading(self->m_view);
    });
    return isLoading;
}

bool WebContents::isLoadingMainFrameApi() {
    //todo
    return false;
}

bool WebContents::isWaitingForResponseApi() {
    //todo
    return false;
}

void WebContents::_stopApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeStopLoading(self->m_view);
    });
}

void WebContents::_goBackApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeGoBack(self->m_view);
    });
}

void WebContents::_goForwardApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeGoForward(self->m_view);
    });
}

void WebContents::_goToOffsetApi() {
    //todo
}

bool WebContents::isCrashedApi() {
    return false;
}

void WebContents::setUserAgentApi(const std::string userAgent) {
    WebContents* self = this;
    int id = m_id;
    std::string* str = new std::string(userAgent);

    ThreadCall::callBlinkThreadSync([self, str, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeSetUserAgent(self->m_view, str->c_str());
        delete str;
    });
}

std::string WebContents::getUserAgentApi() {
    //todo
    return "";
}

void WebContents::insertCSSApi() {
    //todo
}

void WebContents::savePageApi() {
    //todo
}

void WebContents::openDevToolsApi() {
    //todo
}

void WebContents::closeDevToolsApi() {
    //todo
}

void WebContents::isDevToolsOpenedApi() {
    //todo
}

void WebContents::isDevToolsFocusedApi() {
    //todo
}

void WebContents::enableDeviceEmulationApi() {
    //todo
}

void WebContents::disableDeviceEmulationApi() {
    //todo
}

void WebContents::toggleDevToolsApi() {
    //todo
}

void WebContents::inspectElementApi() {
    //todo
}

void WebContents::setAudioMutedApi() {
    /*Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeSetMediaVolume(webContents->m_view, 0.0);
    });*/
}

void WebContents::isAudioMutedApi() {
    /*ThreadCall::callBlinkThreadSync([args] {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

        Local<Boolean> ret = Boolean::New(isolate, wkeIsLoading(webContents->m_view));
        args.GetReturnValue().Set(ret);
    });*/
}

void WebContents::undoApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeEditorUndo(self->m_view);
    });
}

void WebContents::redoApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeEditorRedo(self->m_view);
    });
}

void WebContents::cutApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeEditorCut(self->m_view);
    });
}

void WebContents::copyApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeEditorCopy(self->m_view);
    });
}

void WebContents::pasteApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeEditorPaste(self->m_view);
    });
}

void WebContents::pasteAndMatchStyleApi() {
    //todo
}

void WebContents::_deleteApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeEditorDelete(self->m_view);
    });
}

void WebContents::selectAllApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeEditorSelectAll(self->m_view);
    });
}

void WebContents::unselectApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeEditorUnSelect(self->m_view);
    });
}

void WebContents::replaceApi() {
    //todo
}

void WebContents::replaceMisspellingApi() {
    //todo
}

void WebContents::findInPageApi() {
    //todo
}

void WebContents::stopFindInPageApi() {
    //todo
}

void WebContents::focusApi() {
    WebContents* self = this;
    int id = m_id;

    ThreadCall::callBlinkThreadAsync([self, id] {
        if (IdLiveDetect::get()->isLive(id))
            wkeSetFocus(self->m_view);
    });
}

bool WebContents::isFocusedApi() {
    //todo
    return false;
}

void WebContents::tabTraverseApi() {
    //todo
}

void WebContents::_sendApi() {
    //todo
}

void WebContents::sendInputEventApi() {
    //todo
}

void WebContents::beginFrameSubscriptionApi() {
    //todo
}

void WebContents::endFrameSubscriptionApi() {
    //todo
}

void WebContents::startDragApi() {
    //todo
}

void WebContents::setSizeApi() {
    //todo
}

bool WebContents::isGuestApi() {
    //todo
    return false;
}

bool WebContents::isOffscreenApi() {
    //todo
    return false;
}

void WebContents::startPaintingApi() {
    //todo
}

void WebContents::stopPaintingApi() {
    //todo
}

bool WebContents::isPaintingApi() {
    //todo
    return false;
}

void WebContents::setFrameRateApi() {
    //todo
}

void WebContents::getFrameRateApi() {
    //todo
}

void WebContents::invalidateApi() {
    //todo
}

void WebContents::getTypeApi() {
    //todo
}

void WebContents::getWebPreferencesApi() {
    //todo
}

void WebContents::getOwnerBrowserWindowApi() {
    //todo
}

void WebContents::hasServiceWorkerApi() {
    //todo
}

void WebContents::unregisterServiceWorkerApi() {
    //todo
}

void WebContents::inspectServiceWorkerApi() {
    //todo
}

void WebContents::printApi() {
    //todo
}

void WebContents::_printToPDFApi() {
    //todo
}

void WebContents::addWorkSpaceApi() {
    //todo
}

void WebContents::reNullWorkSpaceApi() {
    //todo
}

void WebContents::showDefinitionForSelectionApi() {
    //todo
}

void WebContents::copyImageAtApi() {
    //todo
}

void WebContents::capturePageApi() {
    //todo
}

void WebContents::setEmbedderApi() {
    //todo
}

void WebContents::nullFunction() {
}

gin::WrapperInfo WebContents::kWrapperInfo = { gin::kEmbedderNativeGin };
Persistent<Function> WebContents::constructor;

static void initializeWebContentApi(Local<Object> target, v8::Local<Value> unused, v8::Local<Context> context, const NodeNative* native) {
    Environment* env = Environment::GetCurrent(context);
    WebContents::init(env->isolate(), target, env);
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_web_contents, initializeWebContentApi, &nativeHello)

} // atom

