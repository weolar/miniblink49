// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/inspector/InspectorWrapper.h"

#include "bindings/core/v8/V8ScriptRunner.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "wtf/RefPtr.h"

namespace blink {

v8::Local<v8::FunctionTemplate> InspectorWrapperBase::createWrapperTemplate(v8::Isolate* isolate, const char* className, const Vector<V8MethodConfiguration>& methods, const Vector<V8AttributeConfiguration>& attributes)
{
    v8::Local<v8::FunctionTemplate> functionTemplate = v8::FunctionTemplate::New(isolate);

    functionTemplate->SetClassName(v8::String::NewFromUtf8(isolate, className, v8::NewStringType::kInternalized).ToLocalChecked());
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();

    for (auto& config : attributes) {
        v8::Local<v8::Name> v8name = v8::String::NewFromUtf8(isolate, config.name, v8::NewStringType::kInternalized).ToLocalChecked();
        instanceTemplate->SetAccessor(v8name, config.callback);
    }

    for (auto& config : methods) {
        v8::Local<v8::Name> v8name = v8::String::NewFromUtf8(isolate, config.name, v8::NewStringType::kInternalized).ToLocalChecked();
        v8::Local<v8::FunctionTemplate> functionTemplate = v8::FunctionTemplate::New(isolate, config.callback);
        functionTemplate->RemovePrototype();
        instanceTemplate->Set(v8name, functionTemplate, static_cast<v8::PropertyAttribute>(v8::DontDelete | v8::DontEnum | v8::ReadOnly));
    }

    return functionTemplate;
}

v8::Local<v8::Object> InspectorWrapperBase::createWrapper(v8::Local<v8::FunctionTemplate> constructorTemplate, v8::Local<v8::Context> context)
{
    v8::Local<v8::Function> function;
    if (!constructorTemplate->GetFunction(context).ToLocal(&function))
        return v8::Local<v8::Object>();

    // FIXME: don't depend on V8ScriptRunner
    v8::Isolate* isolate = context->GetIsolate();
    v8::MaybeLocal<v8::Object> maybeResult = V8ScriptRunner::instantiateObject(isolate, function);
    v8::Local<v8::Object> result;
    if (!maybeResult.ToLocal(&result))
        return v8::Local<v8::Object>();

    return result;
}

void* InspectorWrapperBase::unwrap(v8::Local<v8::Object> object, const char* name)
{
    v8::Isolate* isolate = object->GetIsolate();
    v8::Local<v8::Value> value = blink::V8HiddenValue::getHiddenValue(isolate, object, v8InternalizedString(isolate, name));
    if (value.IsEmpty())
        return nullptr;
    if (!value->IsExternal())
        return nullptr;
    return value.As<v8::External>()->Value();
}

v8::Local<v8::String> InspectorWrapperBase::v8InternalizedString(v8::Isolate* isolate, const char* name)
{
    return v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kInternalized).ToLocalChecked();
}

} // namespace blink
