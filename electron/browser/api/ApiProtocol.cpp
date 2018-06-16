// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "node/nodeblink.h"
#include "browser/api/ProtocolInterface.h"
#include "common/NodeRegisterHelp.h"
#include "common/ThreadCall.h"
#include "common/api/EventEmitter.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"
#include "gin/dictionary.h"
#include "cef/include/base/cef_lock.h"
#include "wke/wke.h"
#include <vector>

namespace atom {

class Protocol
    : public mate::EventEmitter<Protocol>
    , public ProtocolInterface {
public:
    Protocol(v8::Isolate* isolate, v8::Local<v8::Object> wrapper, v8::Local<v8::Value> jsReciver) {
        gin::Wrappable<Protocol>::InitWith(isolate, wrapper);
        ProtocolInterface::m_inst = this;
        m_jsReciver.Reset(isolate, v8::Local<v8::Function>::Cast(jsReciver));
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Protocol"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("registerStandardSchemes", &Protocol::registerStandardSchemesApi);
        builder.SetMethod("_registerProtocol", &Protocol::_registerProtocolApi);
        builder.SetMethod("_unregisterProtocol", &Protocol::_unregisterProtocolApi);
        builder.SetMethod("_isProtocolHandled", &Protocol::_isProtocolHandledApi);
        builder.SetMethod("onHandlerFinish", &Protocol::onHandlerFinishApi);
        
        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Protocol"), prototype->GetFunction());
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (!args.IsConstructCall())
            DebugBreak();

        v8::Local<v8::Value> jsReciver = args[0];

        new Protocol(isolate, args.This(), jsReciver);
        args.GetReturnValue().Set(args.This());
        return;
    }

    void registerStandardSchemesApi(const std::vector<std::string>& schemes) {

    }

    struct ProtocolInfo {
        ProtocolInfo(int handlerId, const std::string& protocolType) {
            id = handlerId;
            type = protocolType;
        }
        int id;
        std::string type;
    };

    bool _registerProtocolApi(const std::string& scheme, int handlerId, const std::string& type) {
        base::AutoLock autoLock(m_lock);
        std::map<std::string, ProtocolInfo>::iterator it = m_schemeToHandleId.find(scheme);
        if (it != m_schemeToHandleId.end())
            return false;
        m_schemeToHandleId.insert(std::make_pair(scheme, ProtocolInfo(handlerId, type)));
        return true;
    }

    void _unregisterProtocolApi(const std::string& scheme) {
        base::AutoLock autoLock(m_lock);
        m_schemeToHandleId.erase(scheme);
    }

    bool _isProtocolHandledApi(const std::string& scheme) {
        base::AutoLock autoLock(m_lock);
        std::map<std::string, ProtocolInfo>::iterator it = m_schemeToHandleId.find(scheme);
        return (it != m_schemeToHandleId.end());
    }

    void onHandlerFinishApi(const std::string& url, int32_t jobPtr, int32_t boolPtr, int32_t typePtr) {
        void* job = (void*)jobPtr;
        bool* isCallOnHandlerFinish = (bool*)boolPtr;
        *isCallOnHandlerFinish = true;

        std::string* type = (std::string*)typePtr;
        std::string urlTemp(url);
        if (-1 == urlTemp.find("://"))
            urlTemp.insert(0, *type);

        wkeNetChangeRequestUrl(job, urlTemp.c_str());
        wkeNetContinueJob(job);
    }

    virtual bool handleLoadUrlBegin(void* param, const char* url, void* jobPtr) override {
//         OutputDebugStringA("handleLoadUrlBegin:");
//         OutputDebugStringA(url);
//         OutputDebugStringA("\n");

        const char* pos = strstr(url, "://");
        if (!pos)
            return false;
        std::string scheme(url, pos);

        base::AutoLock autoLock(m_lock);
        std::map<std::string, ProtocolInfo>::iterator it = m_schemeToHandleId.find(scheme);
        if (it == m_schemeToHandleId.end())
            return false;
                 
        wkeNetHoldJobToAsynCommit(jobPtr);
        int id = it->second.id;
        std::string* type = new std::string(it->second.type);
        *type += ":///";
        ThreadCall::callUiThreadAsync([id, jobPtr, type] {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            Protocol* self = (Protocol*)Protocol::inst();
            v8::Local<v8::Value> args[5];
            args[0] = v8::Integer::New(isolate, id);

            v8::Local<v8::Object> request = v8::Object::New(isolate);
            gin::Dictionary dictRequest(isolate, request);
            dictRequest.Set("url", wkeNetGetUrlByJob(jobPtr));
            dictRequest.Set("referrer", wkeNetGetHTTPHeaderField(jobPtr, "Referer"));
            dictRequest.Set("method", wkeNetGetHTTPHeaderField(jobPtr, "method"));
            
            args[1] = request;
            args[2] = v8::Integer::New(isolate, (int32_t)jobPtr);

            bool isCallOnHandlerFinish = false;
            args[3] = v8::Integer::New(isolate, (int32_t)(&isCallOnHandlerFinish));

            args[4] = v8::Integer::New(isolate, (int32_t)(type));

            v8::Local<v8::Function> func = self->m_jsReciver.Get(isolate);
            func->Call(v8::Undefined(isolate), 5, args);
            if (!isCallOnHandlerFinish)
                wkeNetCancelRequest(jobPtr);

            delete type;
        });

        return true;
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;

    v8::Persistent<v8::Function> m_jsReciver;
    std::map<std::string, ProtocolInfo> m_schemeToHandleId;
    base::Lock m_lock;
};

v8::Persistent<v8::Function> Protocol::constructor;
gin::WrapperInfo Protocol::kWrapperInfo = { gin::kEmbedderNativeGin };
ProtocolInterface* ProtocolInterface::m_inst = nullptr;

void initializeProtocolApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Protocol::init(env->isolate(), exports);
}

}  // atom namespace

static const char BrowserProtocolNative[] = "console.log('BrowserProtocolNative');;";
static NodeNative nativeBrowserProtocolNative{ "Protocol", BrowserProtocolNative, sizeof(BrowserProtocolNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_protocol, atom::initializeProtocolApi, &nativeBrowserProtocolNative)
