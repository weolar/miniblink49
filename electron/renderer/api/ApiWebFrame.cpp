// Copyright (c) 2017 weolar, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "node/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "common/ThreadCall.h"
#include "gin/object_template_builder.h"
#include "gin/dictionary.h"
#include "wkedefine.h"

namespace atom {

class WebFrame : public mate::EventEmitter<WebFrame> {
public:
    explicit WebFrame(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<WebFrame>::InitWith(isolate, wrapper);

        m_zoomLevel = 0;
        m_zoomFactor = 1;
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "WebFrame"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("registerEmbedderCustomElement", &WebFrame::registerEmbedderCustomElementApi);
        builder.SetMethod("setZoomFactor", &WebFrame::setZoomFactorApi);
        builder.SetMethod("getZoomFactor", &WebFrame::getZoomFactorApi);
        builder.SetMethod("getZoomLevel", &WebFrame::getZoomLevelApi);
        builder.SetMethod("setZoomLevel", &WebFrame::setZoomLevelApi);        
        builder.SetMethod("setZoomLevelLimits", &WebFrame::setZoomLevelLimitsApi);
        builder.SetMethod("setSpellCheckProvider", &WebFrame::setSpellCheckProviderApi);
        builder.SetMethod("registerURLSchemeAsSecure", &WebFrame::registerURLSchemeAsSecureApi);
        builder.SetMethod("registerURLSchemeAsBypassingCSP", &WebFrame::registerURLSchemeAsBypassingCSPApi);
        builder.SetMethod("registerURLSchemeAsPrivileged", &WebFrame::registerURLSchemeAsPrivilegedApi);
        builder.SetMethod("insertText", &WebFrame::insertTextApi);
        builder.SetMethod("setSpellCheckProvider", &WebFrame::setSpellCheckProviderApi);
        builder.SetMethod("executeJavaScript", &WebFrame::executeJavaScriptApi);
        builder.SetMethod("setMaxListeners", &WebFrame::setMaxListenersApi);
        builder.SetMethod("setVisualZoomLevelLimits", &WebFrame::setVisualZoomLevelLimitsApi);
        builder.SetMethod("setLayoutZoomLevelLimits", &WebFrame::setLayoutZoomLevelLimitsApi);
        
        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "WebFrame"), prototype->GetFunction());
    }

    v8::Local<v8::Value> registerEmbedderCustomElementApi(const std::string& name, v8::Local<v8::Object> options) {
        wkeWebView webview = wkeGetWebViewForCurrentContext();
        wkeWebFrameHandle mainFrame = wkeWebFrameGetMainFrame(webview);

        v8::Persistent<v8::Value> result;
        wkeRegisterEmbedderCustomElement(webview, mainFrame, name.c_str(), &options, &result);
        v8::Local<v8::Value> elementConstructor = result.Get(isolate());
        result.Reset();
        return elementConstructor;
    }

    void registerElementResizeCallbackApi(/*int element_instance_id, const GuestViewContainer::ResizeCallback& callback*/) {
//         auto guest_view_container = GuestViewContainer::FromID(element_instance_id);
//         if (guest_view_container)
//             guest_view_container->RegisterElementResizeCallback(callback);
    }


    void setVisualZoomLevelLimitsApi(int Level1, int Level2) {

    }

    void setLayoutZoomLevelLimitsApi(int Level1, int Level2) {

    }

    void setZoomFactorApi(float factor) {
        m_zoomFactor = factor;
    }

    float getZoomFactorApi() const {
        return m_zoomFactor;
    }

    void setZoomLevelLimitsApi(float minimumLevel, float maximumLevel) {
        OutputDebugStringA("setZoomLevelLimitsApi\n");
    }

    void setZoomLevelApi(float level) {
        m_zoomLevel = level;
    }

    float getZoomLevelApi() const {
        return m_zoomLevel;
    }

    void setSpellCheckProviderApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        OutputDebugStringA("setSpellCheckProviderApi\n");
    }

    void registerURLSchemeAsSecureApi(const std::string& scheme) {
        OutputDebugStringA("registerURLSchemeAsSecureApi\n");
    }

    void registerURLSchemeAsBypassingCSPApi(const std::string& scheme) {
        OutputDebugStringA("registerURLSchemeAsBypassingCSPApi\n");
    }
    void registerURLSchemeAsPrivilegedApi(const std::string& scheme) {
        OutputDebugStringA("registerURLSchemeAsPrivilegedApi\n");
    }

    void insertTextApi(const std::string& text) {
        OutputDebugStringA("insertTextApi\n");
    }

    // callback, code, hasUserGesture
    void executeJavaScriptApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (3 != args.Length())
            return;

        std::string codeString;
        if (args[0]->IsString()) {
            v8::String::Utf8Value code(args[0]);
            codeString = *code;
        }

        bool hasUserGesture = false;
        if (args[1]->IsBoolean()) {
            v8::Local<v8::Boolean> hasUserGestureValue = args[1]->ToBoolean();
            hasUserGesture = hasUserGestureValue->Value();
        }

        v8::Persistent<v8::Value> executeJavaScriptCallback;
        if (args[2]->IsFunction())
            executeJavaScriptCallback.Reset(args.GetIsolate(), args[2]);
        else
            executeJavaScriptCallback.Reset();

        v8::HandleScope handleScope(isolate());
        v8::Function* callback = nullptr;
        v8::Local<v8::Value> f;
        if (codeString.empty() && !executeJavaScriptCallback.IsEmpty()) {
            f = executeJavaScriptCallback.Get(isolate());
            callback = v8::Function::Cast(*(f));
            callback->Call(v8::Undefined(isolate()), 0, nullptr);
            return;
        }

        wkeWebView webview = wkeGetWebViewForCurrentContext();
        wkeWebFrameHandle mainFrame = wkeWebFrameGetMainFrame(webview);
        jsValue ret = wkeRunJsByFrame(webview, mainFrame, codeString.c_str(), false);
        if (executeJavaScriptCallback.IsEmpty())
            return;
        
        v8::Local<v8::Value> argv[1];
        f = executeJavaScriptCallback.Get(isolate());
        callback = v8::Function::Cast(*(f));
        callback->Call(v8::Undefined(isolate()), 0, nullptr);

        v8::Persistent<v8::Value>* v = (v8::Persistent<v8::Value>*)jsToV8Value(wkeGetGlobalExecByFrame(webview, mainFrame), ret);
        v8::Local<v8::Value> value = v8::Undefined(isolate());
        if (v)
            value = v8::Local<v8::Value>::New(isolate(), *v);
        argv[0] = value;
        callback->Call(v8::Undefined(isolate()), 1, argv);
    }

    void setMaxListenersApi(int number) {
        //OutputDebugStringA("setMaxListenersApi\n");
    }
        
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            new WebFrame(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;

    float m_zoomFactor;
    float m_zoomLevel;
};

v8::Persistent<v8::Function> WebFrame::constructor;
gin::WrapperInfo WebFrame::kWrapperInfo = { gin::kEmbedderNativeGin };

void initializeWebFrameApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> target, v8::Local<v8::Context> context, void* priv) {
    node::Environment* env = node::Environment::GetCurrent(context);
    WebFrame::init(env->isolate(), exports);
}

}  // namespace

static const char RenererWebFrameNative[] = "console.log('RenererWebFrameNative');;";
static NodeNative nativeRenererWebFrameNative{ "WebFrame", RenererWebFrameNative, sizeof(RenererWebFrameNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_renerer_webframe, atom::initializeWebFrameApi, &nativeRenererWebFrameNative)
