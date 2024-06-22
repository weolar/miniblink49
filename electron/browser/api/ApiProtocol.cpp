// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"
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

namespace {

struct ProtocolCallbackInfo {
    ProtocolCallbackInfo(mbNetJob jobPtr)
    {
        job = jobPtr;
        isAsnyc = false;
        isCalled = false;
        isCancel = false;
    }
    mbNetJob job;

    std::string type;
    std::string url;
    std::map<std::string, std::string> newHttpHead;
    bool isAsnyc;
    bool isCalled;
    bool isCancel;
};

}

class Protocol
    : public mate::EventEmitter<Protocol>,
      public ProtocolInterface {
public:
    Protocol(v8::Isolate* isolate, v8::Local<v8::Object> wrapper, v8::Local<v8::Value> jsReciver)
    {
        gin::Wrappable<Protocol>::InitWith(isolate, wrapper);
        ProtocolInterface::m_inst = this;
        m_jsReciver.Reset(isolate, v8::Local<v8::Function>::Cast(jsReciver));
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target)
    {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Protocol"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("registerStandardSchemes", &Protocol::registerStandardSchemesApi);
        builder.SetMethod("registerSchemesAsPrivileged", &Protocol::registerSchemesAsPrivilegedApi);
//         builder.SetMethod("registerFileProtocol", &Protocol::registerFileProtocolApi);
//         builder.SetMethod("registerBufferProtocol", &Protocol::registerBufferProtocolApi);
//         builder.SetMethod("registerStringProtocol", &Protocol::registerStringProtocolApi);
//         builder.SetMethod("registerHttpProtocol", &Protocol::registerHttpProtocolApi);
        builder.SetMethod("registerStreamProtocol", &Protocol::registerStreamProtocolApi);

        builder.SetMethod("interceptFileProtocol", &Protocol::interceptFileProtocolApi);
        builder.SetMethod("interceptStringProtocol", &Protocol::interceptStringProtocolApi);
        builder.SetMethod("interceptBufferProtocol", &Protocol::interceptBufferProtocolApi);
        builder.SetMethod("interceptHttpProtocol", &Protocol::interceptHttpProtocolApi);
        builder.SetMethod("interceptStreamProtocol", &Protocol::interceptStreamProtocolApi);
        builder.SetMethod("uninterceptProtocol", &Protocol::uninterceptProtocolApi);

        builder.SetMethod("_registerProtocol", &Protocol::_registerProtocolApi);
        builder.SetMethod("_unregisterProtocol", &Protocol::_unregisterProtocolApi);
        builder.SetMethod("_isProtocolHandled", &Protocol::_isProtocolHandledApi);
        builder.SetMethod("onHandlerFinish", &Protocol::onHandlerFinishApi);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Protocol"), prototype->GetFunction());
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (!args.IsConstructCall())
            DebugBreak();

        v8::Local<v8::Value> jsReciver = args[0];

        new Protocol(isolate, args.This(), jsReciver);
        args.GetReturnValue().Set(args.This());
        return;
    }

    void registerStandardSchemesApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
    void registerSchemesAsPrivilegedApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
//     void registerFileProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
//     {
//         OutputDebugStringA("");
//     }
// 
//     void registerBufferProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
//     {
//     }
//     void registerStringProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
//     {
//     }
//     void registerHttpProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
//     {
//     }
    void registerStreamProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
    void unregisterProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
    void interceptFileProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
    void interceptStringProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
    void interceptBufferProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
    void interceptHttpProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
    void interceptStreamProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }
    void uninterceptProtocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
    }

    struct ProtocolInfo {
        ProtocolInfo(int handlerId, const std::string& protocolType)
        {
            id = handlerId;
            type = protocolType;
        }
        int id;
        std::string type;
    };

    bool _registerProtocolApi(const std::string& scheme, int handlerId, const std::string& type)
    {
        base::AutoLock autoLock(m_lock);
        std::map<std::string, ProtocolInfo>::iterator it = m_schemeToHandleId.find(scheme);
        if (it != m_schemeToHandleId.end())
            return false;
        m_schemeToHandleId.insert(std::make_pair(scheme, ProtocolInfo(handlerId, type)));
        return true;
    }

    void _unregisterProtocolApi(const std::string& scheme)
    {
        base::AutoLock autoLock(m_lock);
        m_schemeToHandleId.erase(scheme);
    }

    bool _isProtocolHandledApi(const std::string& scheme)
    {
        base::AutoLock autoLock(m_lock);
        std::map<std::string, ProtocolInfo>::iterator it = m_schemeToHandleId.find(scheme);
        return (it != m_schemeToHandleId.end());
    }

    static std::string normalizeFilePath(const std::string& path)
    {
        std::string result = "file:///";
        for (size_t i = 0; i < path.size(); ++i) {            
            if (path[i] == '\\')
                result += '/';
            else
                result += path[i];
        }
        return result;
    }

    void onHandlerFinishApi(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Value> args0 = args[0];

        v8::Local<v8::Value> args1 = args[1];
        uint32_t infoPtr = args1->ToUint32()->Value();
        ProtocolCallbackInfo* info = (ProtocolCallbackInfo* )infoPtr;
        info->isCalled = true;

        if (info->type == "file") {
            std::string filePath;
            if (args0->IsString()) {
                gin::ConvertFromV8(isolate, args0, &filePath);
            } else if (args0->IsObject()) {
                gin::Dictionary request(isolate, args0.As<v8::Object>());
                request.Get("path", &filePath);
            }
            if (filePath.empty()) {
                mbNetCancelRequest(info->job);
                return;
            }
            mbNetChangeRequestUrl(info->job, normalizeFilePath(filePath).c_str());
        } else if (info->type == "string") {
            std::string mimeType;
            gin::Dictionary request(isolate, args0.As<v8::Object>());

            std::string data;
            if (!request.Get("data", &data)) {
                mbNetCancelRequest(info->job);
                return;
            }
            request.Get("mimeType", &mimeType);
            mbNetSetData(info->job, (void *)data.c_str(), data.size());
            if (!mimeType.empty())
                mbNetSetMIMEType(info->job, mimeType.c_str());
        } else if (info->type == "buffer") {
            std::string mimeType;
            gin::Dictionary request(isolate, args0.As<v8::Object>());
            
            request.Get("mimeType", &mimeType);

            v8::Local<v8::Value> dataV8;
            if (!request.Get("data", &dataV8)) {
                mbNetCancelRequest(info->job);
                return;
            }
            size_t dataLen = 0;
            char* data = nodeBufferGetData(&dataV8, &dataLen);
            if (!data || 0 == dataLen) {
                mbNetCancelRequest(info->job);
                return;
            }
            mbNetSetData(info->job, data, (int)dataLen);
            if (!mimeType.empty())
                mbNetSetMIMEType(info->job, mimeType.c_str());
        }

        if (true || info->isAsnyc) {
            //                 std::map<std::string, std::string>::const_iterator it = info->newHttpHead.begin();
            //                 for (; it != info->newHttpHead.end(); ++it) {
            //                     mbNetSetHTTPHeaderField(info->job, StringUtil::UTF8ToUTF16(it->first).c_str(), StringUtil::UTF8ToUTF16(it->second).c_str(), FALSE);
            //                 }
            mbNetContinueJob(info->job);
            delete info;
        }
    }

    virtual bool handleLoadUrlBegin(void* param, const char* url, void* job) override
    {
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

        int id = it->second.id;

        ProtocolCallbackInfo* info = new ProtocolCallbackInfo(job);
        info->type = it->second.type;
        info->url = url;
        std::string* referrer = new std::string(mbNetGetReferrer(job));
        mbRequestType httpMethod = mbNetGetRequestMethod(job);

        ThreadCall::callUiThreadSync([id, info, referrer, httpMethod] {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            Protocol* self = (Protocol*)Protocol::inst();
            v8::Local<v8::Value> args[5];
            args[0] = v8::Integer::New(isolate, id);

            v8::Local<v8::Object> request = v8::Object::New(isolate);
            gin::Dictionary dictRequest(isolate, request);
            dictRequest.Set("url", info->url.c_str());
            dictRequest.Set("referrer", referrer->c_str());
            dictRequest.Set("method", httpMethod == kMbRequestTypeGet ? "GET" : (httpMethod == kMbRequestTypePost ? "POST" : "PUT"));

            args[1] = request;
            args[2] = v8::Integer::New(isolate, (int32_t)info);

            v8::Local<v8::Function> func = self->m_jsReciver.Get(isolate);
            func->Call(v8::Undefined(isolate), 3, args);

            delete referrer;
        });

        if (!info->isCalled) {
            info->isAsnyc = true;
            mbNetHoldJobToAsynCommit(job);
        } else {
            if (info->isCancel)
                mbNetCancelRequest(job);
            else
                mbNetHoldJobToAsynCommit(job);
            delete info;
        }

        return true;
    }

    v8::Local<v8::Object> getWrapper(v8::Isolate* isolate) override
    {
        return GetWrapper(isolate);
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

void initializeProtocolApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv)
{
    node::Environment* env = node::Environment::GetCurrent(context);
    Protocol::init(env->isolate(), exports);
}

} // atom namespace

static const char BrowserProtocolNative[] = "console.log('BrowserProtocolNative');;";
static NodeNative nativeBrowserProtocolNative { "Protocol", BrowserProtocolNative, sizeof(BrowserProtocolNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_protocol, atom::initializeProtocolApi, &nativeBrowserProtocolNative)
