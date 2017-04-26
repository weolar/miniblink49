﻿#include "browser/api/ApiWebContents.h"

#include "browser/api/WindowInterface.h"
#include "browser/api/WindowList.h"

#include "wke.h"
#include "common/ThreadCall.h"
#include "common/NodeRegisterHelp.h"
#include "common/IdLiveDetect.h"
#include "common/NodeBinding.h"
#include "common/api/EventEmitterCaller.h"
#include "gin/dictionary.h"
#include "gin/object_template_builder.h"
#include "base/values.h"

namespace atom {

void WebContents::init(v8::Isolate* isolate, v8::Local<v8::Object> target, node::Environment* env) {

    v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, WebContents::newFunction);

    prototype->SetClassName(v8::String::NewFromUtf8(isolate, "WebContents"));
    gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
    builder.SetMethod("getId", &WebContents::getIdApi);
    builder.SetMethod("getProcessId", &WebContents::getProcessIdApi);
    builder.SetMethod("equal", &WebContents::equalApi);
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
    builder.SetProperty("id", &WebContents::getIdApi);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "id", nullFunction);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "session", nullFunction);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "hostWebContents", nullFunction);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "devToolsWebContents", nullFunction);
//     NODE_SET_PROTOTYPE_METHOD(tpl, "debugger", nullFunction);

    constructor.Reset(isolate, prototype->GetFunction());
    target->Set(v8::String::NewFromUtf8(isolate, "WebContents"), prototype->GetFunction());
}

WebContents* WebContents::create(v8::Isolate* isolate, gin::Dictionary options, WindowInterface* owner) {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { gin::ConvertToV8(isolate, options) };
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, constructor);

    v8::MaybeLocal<v8::Object> obj = constructorFunction->NewInstance(argc, argv);
    WebContents* self = (WebContents*)WrappableBase::GetNativePtr(obj.ToLocalChecked(), &kWrapperInfo);
    self->m_owner = owner;
    return self;
}

WebContents::WebContents(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
    
    m_nodeBinding = nullptr;
    m_id = IdLiveDetect::get()->constructed();
    m_view = nullptr;
    int id = m_id;
    gin::Wrappable<WebContents>::InitWith(isolate, wrapper);

    WebContents* self = this;
    ThreadCall::callBlinkThreadAsync([self, id] { 
        if (!IdLiveDetect::get()->isLive(id))
            return;

        self->m_view = wkeCreateWebView();
        wkeSetUserKayValue(self->m_view, "WebContents", self);
    });    
}

WebContents::~WebContents() {
    //wkeDestroyWebView(m_view);
    if (m_nodeBinding)
        delete m_nodeBinding;

    for (auto it : m_observers) {
        (it)->onWebContentsDeleted(this);
    }

    IdLiveDetect::get()->deconstructed(m_id);
}

void WebContents::addObserver(WebContentsObserver* observer) {
    m_observers.insert(observer);
}

void WebContents::removeObserver(WebContentsObserver* observer) {
    auto it = m_observers.find(observer);
    m_observers.erase(it);
}

// new方法
void WebContents::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    if (args.IsConstructCall()) {
        if (args.Length() > 1)
            return;
        
        gin::Dictionary options(args.GetIsolate(), args[0]->ToObject());
        WebContents* webContents = new WebContents(isolate, args.This());
        
        args.GetReturnValue().Set(args.This());
    } else {
        const int argc = 2;
        v8::Local<v8::Value> argv[argc] = { args[0], args[1] };
        v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
}

void WebContents::onNewWindowInBlinkThread(int width, int height, const CreateWindowParam* createWindowParam) {
    if (createWindowParam->transparent)
        wkeSetTransparent(getWkeView(), true);
    wkeSettings settings;
    settings.mask = WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD;
    wkeConfigure(&settings);
    wkeResize(getWkeView(), width, height);
    wkeOnDidCreateScriptContext(getWkeView(), &WebContents::staticDidCreateScriptContextCallback, this);
    wkeOnWillReleaseScriptContext(getWkeView(), &WebContents::staticOnWillReleaseScriptContextCallback, this);
}

void WebContents::staticDidCreateScriptContextCallback(wkeWebView webView, wkeWebFrameHandle param, wkeWebFrameHandle frame, void* context, int extensionGroup, int worldId) {
    WebContents* self = (WebContents*)param;
    self->onDidCreateScriptContext(webView, frame, (v8::Local<v8::Context>*)context, extensionGroup, worldId);
}

void WebContents::onDidCreateScriptContext(wkeWebView webView, wkeWebFrameHandle frame, v8::Local<v8::Context>* context, int extensionGroup, int worldId) {
    if (m_nodeBinding || !wkeWebFrameIsMainFrame(frame))
        return;

    BlinkMicrotaskSuppressionHandle handle = nodeBlinkMicrotaskSuppressionEnter((*context)->GetIsolate());
    m_nodeBinding = new NodeBindings(false, ThreadCall::getBlinkLoop());
    node::Environment* env = m_nodeBinding->createEnvironment(*context);
    m_nodeBinding->loadEnvironment();
    nodeBlinkMicrotaskSuppressionLeave(handle);
}

void WebContents::staticOnWillReleaseScriptContextCallback(wkeWebView webView, void* param, wkeWebFrameHandle frame, void* context, int worldId) {
    WebContents* self = (WebContents*)param;
    self->onWillReleaseScriptContextCallback(webView, frame, (v8::Local<v8::Context>*)context, worldId);
}

void WebContents::onWillReleaseScriptContextCallback(wkeWebView webView, wkeWebFrameHandle frame, v8::Local<v8::Context>* context, int worldId) {
    if (!m_nodeBinding)
        return;
    node::Environment* env = node::Environment::GetCurrent(*context);
    if (env)
        mate::emitEvent(env->isolate(), env->process_object(), "exit");

    delete m_nodeBinding;
    m_nodeBinding = nullptr;
}

void WebContents::rendererPostMessageToMain(const std::string& channel, const base::ListValue& listParams) {
    int id = m_id;
    WebContents* self = this;
    std::string* channelWrap = new std::string(channel);
    base::ListValue* listParamsWrap = listParams.DeepCopy();

    ThreadCall::callUiThreadAsync([self, id, channelWrap, listParamsWrap] {
        if (IdLiveDetect::get()->isLive(id)) {
            self->mate::EventEmitter<WebContents>::emit("ipc-message", *listParamsWrap);
        }
        delete channelWrap;
        delete listParamsWrap;
    });
}

void WebContents::rendererSendMessageToMain(const std::string& channel, const base::ListValue& listParams, std::string* jsonRet) {
    WebContents* self = this;
    const std::string* channelWrap = &channel;
    const base::ListValue* listParamsWrap = &listParams;

    std::string outValue;
    listParams.GetString(0, &outValue);

    std::string outValue2;
    listParams.GetString(2, &outValue2);

    ThreadCall::callUiThreadSync([self, channelWrap, listParamsWrap, jsonRet] {
        self->mate::EventEmitter<WebContents>::emitWithSender("ipc-message-sync", [jsonRet](const std::string& json) {
            jsonRet->assign(json.c_str(), json.size());
        }, *listParamsWrap);
    });
}

static bool getIPCObject(v8::Isolate* isolate, v8::Local<v8::Context> context, v8::Local<v8::Object>* ipc) {
    v8::Local<v8::String> key = gin::StringToV8(isolate, "ipc");
    v8::Local<v8::Private> privateKey = v8::Private::ForApi(isolate, key);
    v8::Local<v8::Object> global_object = context->Global();
    v8::Local<v8::Value> value;
    if (!global_object->GetPrivate(context, privateKey).ToLocal(&value))
        return false;
    if (value.IsEmpty() || !value->IsObject())
        return false;
    *ipc = value->ToObject();
    return true;
}

static std::vector<v8::Local<v8::Value>> listValueToVector(v8::Isolate* isolate, const base::ListValue& list) {
    v8::Local<v8::Value> array = gin::ConvertToV8(isolate, list);
    std::vector<v8::Local<v8::Value>> result;
    gin::ConvertFromV8(isolate, array, &result);
    return result;
}

static void emitIPCEvent(wkeWebFrameHandle frame, const std::string& channel, const base::ListValue& args) {
    if (!frame || wkeIsWebRemoteFrame(frame))
        return;

    v8::Isolate* isolate = (v8::Isolate*)wkeGetBlinkMainThreadIsolate();
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Context> context;
    wkeWebFrameGetMainWorldScriptContext(frame, &context);
    v8::Context::Scope contextScope(context);

    // Only emit IPC event for context with node integration.
    node::Environment* env = node::Environment::GetCurrent(context);
    if (!env)
        return;

    v8::Local<v8::Object> ipc;
    if (getIPCObject(isolate, context, &ipc)) {
        std::vector<v8::Local<v8::Value>> argsVector = listValueToVector(isolate, args);
        // Insert the Event object, event.sender is ipc.
        gin::Dictionary evt = gin::Dictionary::CreateEmpty(isolate);
        evt.Set("sender", ipc);
        argsVector.insert(argsVector.begin(), evt.GetHandle());

        mate::emitEvent(isolate, ipc, channel, argsVector);
    }
}

void WebContents::anyPostMessageToRenderer(const std::string& channel, const base::ListValue& listParams) {
    int id = m_id;
    WebContents* self = this;
    std::string* channelWrap = new std::string(channel);
    base::ListValue* listParamsWrap = listParams.DeepCopy();

    ThreadCall::callBlinkThreadAsync([self, id, channelWrap, listParamsWrap] {
        if (IdLiveDetect::get()->isLive(id)) {
            emitIPCEvent(wkeWebFrameGetMainFrame(self->m_view), *channelWrap, *listParamsWrap);
        }

        delete channelWrap;
        delete listParamsWrap;
    });
}

int WebContents::getIdApi() const {
    return (int)this;
}

int WebContents::getProcessIdApi() const {
    return (int)::GetCurrentProcessId();
}

bool WebContents::equalApi() const {
    return false;
}

void WebContents::_loadURLApi(const std::string& url) {
    WebContents* self = this;
    std::string* str = new std::string(url);
    if (str->size() > 9 && str->substr(0, 7) == "file://" && str->at(7) != '/')
        str->insert(7, 1, '/');

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

    WebContents* webContents = ObjectWrap::Unwrap<WebContents>(args.Holder());

    ThreadCall::callBlinkThreadSync([webContents] {
        wkeSetMediaVolume(webContents->m_view, 0.0);
    });*/
}

void WebContents::isAudioMutedApi() {
    /*ThreadCall::callBlinkThreadSync([args] {
        Isolate* isolate = args.GetIsolate();

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

bool WebContents::_sendApi(bool isAllFrames, const std::string& channel, const base::ListValue& args) {
    WindowList::iterator winIt = WindowList::getInstance()->begin();
    for (; winIt != WindowList::getInstance()->end(); ++winIt) {
        WindowInterface* windowInterface = *winIt;
        WebContents* webCcontents = windowInterface->getWebContents();
        webCcontents->anyPostMessageToRenderer(channel, args);
    }
    return true;
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

v8::Local<v8::Value> WebContents::getOwnerBrowserWindowApi() {
     if (m_owner)
        return m_owner->getWrapper();
    
    return v8::Null(isolate());
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
v8::Persistent<v8::Function> WebContents::constructor;

static void initializeWebContentApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    WebContents::init(env->isolate(), target, env);
}

static const char WebContentsSricpt[] =
"exports = {};";

static NodeNative nativeBrowserWebContentsNative{ "WebContents", WebContentsSricpt, sizeof(WebContentsSricpt) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_web_contents, initializeWebContentApi, &nativeBrowserWebContentsNative)

} // atom

