// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/object_template_builder.h"

#include "gin/interceptor.h"
#include "gin/per_isolate_data.h"
#include "gin/public/wrapper_info.h"

namespace gin {

namespace {

WrappableBase* WrappableFromV8(v8::Isolate* isolate,
                               v8::Local<v8::Value> val) {
  if (!val->IsObject())
    return NULL;
  v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(val);
  WrapperInfo* info = WrapperInfo::From(obj);

  // If this fails, the object is not managed by Gin.
  if (!info)
    return NULL;

  // We don't further validate the type of the object, but assume it's derived
  // from WrappableBase. We look up the pointer in a global registry, to make
  // sure it's actually pointed to a valid life object.
  return static_cast<WrappableBase*>(
      obj->GetAlignedPointerFromInternalField(kEncodedValueIndex));
}

NamedPropertyInterceptor* NamedInterceptorFromV8(v8::Isolate* isolate,
                                                 v8::Local<v8::Value> val) {
  WrappableBase* base = WrappableFromV8(isolate, val);
  if (!base)
    return NULL;
  return PerIsolateData::From(isolate)->GetNamedPropertyInterceptor(base);
}

IndexedPropertyInterceptor* IndexedInterceptorFromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val) {
  WrappableBase* base = WrappableFromV8(isolate, val);
  if (!base)
    return NULL;
  return PerIsolateData::From(isolate)->GetIndexedPropertyInterceptor(base);
}

void NamedPropertyGetter(v8::Local<v8::String> property,
                         const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  NamedPropertyInterceptor* interceptor =
      NamedInterceptorFromV8(isolate, info.Holder());
  if (!interceptor)
    return;
  std::string name;
  ConvertFromV8(isolate, property, &name);
  info.GetReturnValue().Set(interceptor->GetNamedProperty(isolate, name));
}

void NamedPropertySetter(v8::Local<v8::String> property,
                         v8::Local<v8::Value> value,
                         const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  NamedPropertyInterceptor* interceptor =
      NamedInterceptorFromV8(isolate, info.Holder());
  if (!interceptor)
    return;
  std::string name;
  ConvertFromV8(isolate, property, &name);
  if (interceptor->SetNamedProperty(isolate, name, value))
    info.GetReturnValue().Set(value);
}

void NamedPropertyQuery(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Integer>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  NamedPropertyInterceptor* interceptor =
      NamedInterceptorFromV8(isolate, info.Holder());
  if (!interceptor)
    return;
  std::string name;
  ConvertFromV8(isolate, property, &name);
  if (interceptor->GetNamedProperty(isolate, name).IsEmpty())
    return;
  info.GetReturnValue().Set(0);
}

void NamedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  NamedPropertyInterceptor* interceptor =
      NamedInterceptorFromV8(isolate, info.Holder());
  if (!interceptor)
    return;
  v8::Local<v8::Value> properties;
  if (!TryConvertToV8(isolate, interceptor->EnumerateNamedProperties(isolate),
                      &properties))
    return;
  info.GetReturnValue().Set(v8::Local<v8::Array>::Cast(properties));
}

void IndexedPropertyGetter(uint32_t index,
                           const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  IndexedPropertyInterceptor* interceptor =
      IndexedInterceptorFromV8(isolate, info.Holder());
  if (!interceptor)
    return;
  info.GetReturnValue().Set(interceptor->GetIndexedProperty(isolate, index));
}

void IndexedPropertySetter(uint32_t index,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  IndexedPropertyInterceptor* interceptor =
      IndexedInterceptorFromV8(isolate, info.Holder());
  if (!interceptor)
    return;
  if (interceptor->SetIndexedProperty(isolate, index, value))
    info.GetReturnValue().Set(value);
}

void IndexedPropertyEnumerator(
    const v8::PropertyCallbackInfo<v8::Array>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  IndexedPropertyInterceptor* interceptor =
      IndexedInterceptorFromV8(isolate, info.Holder());
  if (!interceptor)
    return;
  v8::Local<v8::Value> properties;
  if (!TryConvertToV8(isolate, interceptor->EnumerateIndexedProperties(isolate),
                      &properties))
    return;
  info.GetReturnValue().Set(v8::Local<v8::Array>::Cast(properties));
}

}  // namespace

ObjectTemplateBuilder::ObjectTemplateBuilder(v8::Isolate* isolate)
    : isolate_(isolate), template_(v8::ObjectTemplate::New(isolate)) {
  template_->SetInternalFieldCount(kNumberOfInternalFields);
}

ObjectTemplateBuilder::~ObjectTemplateBuilder() {
}

ObjectTemplateBuilder& ObjectTemplateBuilder::AddNamedPropertyInterceptor() {
  template_->SetNamedPropertyHandler(&NamedPropertyGetter,
                                     &NamedPropertySetter,
                                     &NamedPropertyQuery,
                                     NULL,
                                     &NamedPropertyEnumerator);
  return *this;
}

ObjectTemplateBuilder& ObjectTemplateBuilder::AddIndexedPropertyInterceptor() {
  template_->SetIndexedPropertyHandler(&IndexedPropertyGetter,
                                       &IndexedPropertySetter,
                                       NULL,
                                       NULL,
                                       &IndexedPropertyEnumerator);
  return *this;
}

ObjectTemplateBuilder& ObjectTemplateBuilder::SetImpl(
    const base::StringPiece& name, v8::Local<v8::Data> val) {
  template_->Set(StringToSymbol(isolate_, name), val);
  return *this;
}

ObjectTemplateBuilder& ObjectTemplateBuilder::SetPropertyImpl(
    const base::StringPiece& name, v8::Local<v8::FunctionTemplate> getter,
    v8::Local<v8::FunctionTemplate> setter) {
  template_->SetAccessorProperty(StringToSymbol(isolate_, name), getter,
                                 setter);
  return *this;
}

v8::Local<v8::ObjectTemplate> ObjectTemplateBuilder::Build() {
  v8::Local<v8::ObjectTemplate> result = template_;
  template_.Clear();
  return result;
}

}  // namespace gin
