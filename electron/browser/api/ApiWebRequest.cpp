// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "browser/api/ApiWebRequest.h"

#include "browser/api/ApiSession.h"
#include "node/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "common/FileUtil.h"
#include "common/StringUtil.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"
#include "gin/dictionary.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"

#include <vector>
#include <shlwapi.h>

namespace atom {

ApiWebRequest::ApiWebRequest(v8::Isolate* isolate, v8::Local<v8::Object> wrapper)
{
    gin::Wrappable<ApiWebRequest>::InitWith(isolate, wrapper);
}

ApiWebRequest* ApiWebRequest::create(v8::Isolate* isolate)
{
//     const int argc = 1;
//     v8::Local<v8::Value> argv[argc] = { v8::Null(isolate) };
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, constructor);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    //v8::MaybeLocal<v8::Object> obj = constructorFunction->NewInstance(context, argc, argv);
    v8::MaybeLocal<v8::Object> obj = constructorFunction->NewInstance(context);
    v8::Local<v8::Object> objV8 = obj.ToLocalChecked();

    ApiWebRequest* self = (ApiWebRequest*)WrappableBase::GetNativePtr(objV8, &kWrapperInfo);
    self->m_liveSelf.Reset(isolate, objV8);

    return self;
}

void ApiWebRequest::init(v8::Isolate* isolate, v8::Local<v8::Object> target)
{
    const char* className = "WebRequest";
    v8::Local<v8::FunctionTemplate> funTempl = v8::FunctionTemplate::New(isolate, newFunction);

    funTempl->SetClassName(v8::String::NewFromUtf8(isolate, className));
    gin::ObjectTemplateBuilder builder(isolate, funTempl->InstanceTemplate());
    builder.SetMethod("onBeforeSendHeaders", &ApiWebRequest::onBeforeSendHeadersApi);
    builder.SetMethod("onSendHeaders", &ApiWebRequest::onSendHeadersApi);
    builder.SetMethod("onBeforeRedirect", &ApiWebRequest::onBeforeRedirectApi);
    builder.SetMethod("onHeadersReceived", &ApiWebRequest::onHeadersReceivedApi);
    builder.SetMethod("onResponseStarted", &ApiWebRequest::onResponseStartedApi);
    builder.SetMethod("onBeforeRedirect", &ApiWebRequest::onBeforeRedirectApi);
    builder.SetMethod("onCompleted", &ApiWebRequest::onCompletedApi);
    builder.SetMethod("onErrorOccurred", &ApiWebRequest::onErrorOccurredApi);

    v8::Local<v8::Function> fun = funTempl->GetFunction();
//     gin::Dictionary sessionClass(isolate, fun);
//     sessionClass.SetMethod("fromPartition", &ApiWebRequest::fromPartitionApi);

    constructor.Reset(isolate, fun);
    target->Set(v8::String::NewFromUtf8(isolate, className), fun);
}

void ApiWebRequest::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (args.IsConstructCall()) {
        new ApiWebRequest(isolate, args.This());
        args.GetReturnValue().Set(args.This());
        return;
    }
}

void ApiWebRequest::onBeforeSendHeadersApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> cb;
    if (args.Length() == 1) {
        cb = args[0];
    } else if (args.Length() == 2) {
        cb = args[1];
    }
    if (!cb->IsFunction())
        return;

    m_beforeSendHeadersCb.Reset(args.GetIsolate(), cb);
}

void ApiWebRequest::onSendHeadersApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> cb;
    if (args.Length() == 1) {
        cb = args[0];
    } else if (args.Length() == 2) {
        cb = args[1];
    }
    if (!cb->IsFunction())
        return;

    m_sendHeadersCb.Reset(args.GetIsolate(), cb);
}

void ApiWebRequest::onHeadersReceivedApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    OutputDebugStringA("ApiWebRequest::onHeadersReceivedApi NOT impl\n");
}

void ApiWebRequest::onResponseStartedApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    OutputDebugStringA("ApiWebRequest::onResponseStartedApi NOT impl\n");
}

void ApiWebRequest::onBeforeRedirectApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    OutputDebugStringA("ApiWebRequest::onBeforeRedirectApi NOT impl\n");
}

void ApiWebRequest::onCompletedApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    OutputDebugStringA("ApiWebRequest::onCompletedApi NOT impl\n");
}

void ApiWebRequest::onErrorOccurredApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    OutputDebugStringA("ApiWebRequest::onErrorOccurredApi NOT impl\n");
}

gin::WrapperInfo ApiWebRequest::kWrapperInfo = { gin::kEmbedderNativeGin };
v8::Persistent<v8::Function> ApiWebRequest::constructor;

void initializeBrowserWebRequestApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv)
{
    node::Environment* env = node::Environment::GetCurrent(context);
    ApiWebRequest::init(env->isolate(), exports);
}

static const char BrowserWebRequestName[] = "console.log('BrowserWebRequestNative');;";
static NodeNative BrowserWebRequestNative{ "WebRequest", BrowserWebRequestName, sizeof(BrowserWebRequestName) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_webrequest, initializeBrowserWebRequestApi, &BrowserWebRequestNative)

} // atom namespace