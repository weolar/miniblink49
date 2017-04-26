// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/v8_inspector/V8InjectedScriptHost.h"

#include "platform/inspector_protocol/String16.h"
#include "platform/v8_inspector/InjectedScriptNative.h"
#include "platform/v8_inspector/V8Compat.h"
#include "platform/v8_inspector/V8Debugger.h"
#include "platform/v8_inspector/V8InspectorImpl.h"
#include "platform/v8_inspector/V8InternalValueType.h"
#include "platform/v8_inspector/V8StringUtil.h"
#include "platform/v8_inspector/public/V8InspectorClient.h"

namespace blink {

namespace {

void setFunctionProperty(v8::Local<v8::Context> context, v8::Local<v8::Object> obj, const char* name, v8::FunctionCallback callback, v8::Local<v8::External> external)
{
    v8::Local<v8::String> funcName = toV8StringInternalized(context->GetIsolate(), name);
    v8::Local<v8::Function> func;
    if (!V8_FUNCTION_NEW_REMOVE_PROTOTYPE(context, callback, external, 0).ToLocal(&func))
        return;
    func->SetName(funcName);
    if (!obj->Set(context, funcName, func).FromMaybe(false))
        return;
}

V8InspectorImpl* unwrapInspector(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    DCHECK(!info.Data().IsEmpty());
    DCHECK(info.Data()->IsExternal());
    V8InspectorImpl* inspector = static_cast<V8InspectorImpl*>(info.Data().As<v8::External>()->Value());
    DCHECK(inspector);
    return inspector;
}

} // namespace

v8::Local<v8::Object> V8InjectedScriptHost::create(v8::Local<v8::Context> context, V8InspectorImpl* inspector)
{
    v8::Isolate* isolate = inspector->isolate();
    v8::Local<v8::Object> injectedScriptHost = v8::Object::New(isolate);
    v8::Local<v8::External> debuggerExternal = v8::External::New(isolate, inspector);
    setFunctionProperty(context, injectedScriptHost, "internalConstructorName", V8InjectedScriptHost::internalConstructorNameCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "formatAccessorsAsProperties", V8InjectedScriptHost::formatAccessorsAsProperties, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "subtype", V8InjectedScriptHost::subtypeCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "getInternalProperties", V8InjectedScriptHost::getInternalPropertiesCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "objectHasOwnProperty", V8InjectedScriptHost::objectHasOwnPropertyCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "bind", V8InjectedScriptHost::bindCallback, debuggerExternal);
    setFunctionProperty(context, injectedScriptHost, "proxyTargetValue", V8InjectedScriptHost::proxyTargetValueCallback, debuggerExternal);
    return injectedScriptHost;
}

void V8InjectedScriptHost::internalConstructorNameCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1 || !info[0]->IsObject())
        return;

    v8::Local<v8::Object> object = info[0].As<v8::Object>();
    info.GetReturnValue().Set(object->GetConstructorName());
}

void V8InjectedScriptHost::formatAccessorsAsProperties(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    DCHECK_EQ(info.Length(), 2);
    info.GetReturnValue().Set(false);
    if (!info[1]->IsFunction())
        return;
    // Check that function is user-defined.
    if (info[1].As<v8::Function>()->ScriptId() != v8::UnboundScript::kNoScriptId)
        return;
    info.GetReturnValue().Set(unwrapInspector(info)->client()->formatAccessorsAsProperties(info[0]));
}

void V8InjectedScriptHost::subtypeCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
        return;

    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Value> value = info[0];
    if (value->IsObject()) {
        v8::Local<v8::Value> internalType = v8InternalValueTypeFrom(isolate->GetCurrentContext(), v8::Local<v8::Object>::Cast(value));
        if (internalType->IsString()) {
            info.GetReturnValue().Set(internalType);
            return;
        }
    }
    if (value->IsArray() || value->IsArgumentsObject()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "array"));
        return;
    }
    if (value->IsTypedArray()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "typedarray"));
        return;
    }
    if (value->IsDate()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "date"));
        return;
    }
    if (value->IsRegExp()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "regexp"));
        return;
    }
    if (value->IsMap() || value->IsWeakMap()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "map"));
        return;
    }
    if (value->IsSet() || value->IsWeakSet()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "set"));
        return;
    }
    if (value->IsMapIterator() || value->IsSetIterator()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "iterator"));
        return;
    }
    if (value->IsGeneratorObject()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "generator"));
        return;
    }
    if (value->IsNativeError()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "error"));
        return;
    }
    if (value->IsProxy()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "proxy"));
        return;
    }
    if (value->IsPromise()) {
        info.GetReturnValue().Set(toV8StringInternalized(isolate, "promise"));
        return;
    }
    String16 subtype = unwrapInspector(info)->client()->valueSubtype(value);
    if (!subtype.isEmpty()) {
        info.GetReturnValue().Set(toV8String(isolate, subtype));
        return;
    }
}

void V8InjectedScriptHost::getInternalPropertiesCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
        return;
    v8::Local<v8::Array> properties;
    if (unwrapInspector(info)->debugger()->internalProperties(info.GetIsolate()->GetCurrentContext(), info[0]).ToLocal(&properties))
        info.GetReturnValue().Set(properties);
}

void V8InjectedScriptHost::objectHasOwnPropertyCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 2 || !info[0]->IsObject() || !info[1]->IsString())
        return;
    bool result = info[0].As<v8::Object>()->HasOwnProperty(info.GetIsolate()->GetCurrentContext(), v8::Local<v8::String>::Cast(info[1])).FromMaybe(false);
    info.GetReturnValue().Set(v8::Boolean::New(info.GetIsolate(), result));
}

void V8InjectedScriptHost::bindCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 2 || !info[1]->IsString())
        return;
    InjectedScriptNative* injectedScriptNative = InjectedScriptNative::fromInjectedScriptHost(info.Holder());
    if (!injectedScriptNative)
        return;

    v8::Local<v8::String> v8groupName = info[1]->ToString(info.GetIsolate());
    String16 groupName = toProtocolStringWithTypeCheck(v8groupName);
    int id = injectedScriptNative->bind(info[0], groupName);
    info.GetReturnValue().Set(id);
}

void V8InjectedScriptHost::proxyTargetValueCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() != 1 || !info[0]->IsProxy()) {
        NOTREACHED();
        return;
    }
    v8::Local<v8::Object> target = info[0].As<v8::Proxy>();
    while (target->IsProxy())
        target = v8::Local<v8::Proxy>::Cast(target)->GetTarget();
    info.GetReturnValue().Set(target);
}

} // namespace blink
