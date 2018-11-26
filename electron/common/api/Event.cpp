// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/event.h"

#include "gin/converter.h"
#include "gin/object_template_builder.h"
#include "common/api/GetConstructorFromTls.h"

namespace mate {

gin::WrapperInfo Event::kWrapperInfo = { gin::kEmbedderNativeGin };
DWORD Event::constructorTlsKey = 0;

Event::Event(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
    m_callback = nullptr;
    gin::Wrappable<Event>::InitWith(isolate, wrapper);
}

Event::~Event() {
    if (m_callback)
        delete m_callback;
}

void Event::preventDefault(v8::Isolate* isolate) {
    GetWrapper(isolate)->Set(gin::StringToV8(isolate, "defaultPrevented"), v8::True(isolate));
}

bool Event::sendReply(const std::string& json) {
    if (m_callback)
        (*m_callback)(json);
    return true;
}

std::string Event::returnValueGet() {
    return m_returnValue;
}

void Event::returnValueSet(std::string json) {
    m_returnValue = json;

    if (m_callback)
        (*m_callback)(json);
}

// static
Event* Event::create(v8::Isolate* isolate, v8::Local<v8::Object> wrapper, std::function<void(std::string)>&& callback) {
    Event::init(isolate);

    v8::Persistent<v8::Function>* constructor = atom::V8PersistentTls::get(&constructorTlsKey);
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, *constructor);

    v8::MaybeLocal<v8::Object> obj = constructorFunction->NewInstance();
    Event* self = (Event*)WrappableBase::GetNativePtr(obj.ToLocalChecked(), &kWrapperInfo);
    self->m_callback = new std::function<void(std::string)>(std::move(callback));
    return self;
}

void Event::init(v8::Isolate* isolate) {
    v8::Persistent<v8::Function>* constructor = atom::V8PersistentTls::get(&constructorTlsKey);
    if (!(*constructor).IsEmpty())
        return;

    v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, Event::newFunction);
    prototype->SetClassName(v8::String::NewFromUtf8(isolate, "MateCommonApiEvent"));
    gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
    builder.SetMethod("preventDefault", &Event::preventDefault);
    builder.SetMethod("sendReply", &Event::sendReply);
    //builder.SetMemberAccessor("returnValue2", &Event::returnValueGet, &Event::returnValueSet);
    (*constructor).Reset(isolate, prototype->GetFunction());
}

void accessorGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Value> v8_result_value;
    if (!gin::TryConvertToV8(isolate, std::string("123456"), &v8_result_value))
        return;

    std::string propertyStr;
    if (gin::ConvertFromV8(isolate, property, &propertyStr))
        return;

    info.GetReturnValue().Set(v8_result_value);
}

void accessorSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    std::string arg;
    if (gin::ConvertFromV8(isolate, value, &arg))
        return;
    arg = "hahah";
}

void Event::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (args.IsConstructCall()) {
        if (args.Length() > 1)
            return;

        Event* evt = new Event(isolate, args.This());
        args.GetReturnValue().Set(args.This());

//         v8::Local<v8::Object> evtObj = evt->GetWrapper(isolate);
//         evtObj->SetAccessor(v8::String::NewFromUtf8(isolate, "returnValue2"), accessorGetterCallback, accessorSetterCallback);
    }
}

}  // namespace mate
