// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/wrappable.h"

#include "base/logging.h"
#include "gin/object_template_builder.h"
#include "gin/per_isolate_data.h"

namespace gin {

WrappableBase::WrappableBase() {
}

WrappableBase::~WrappableBase() {
  wrapper_.Reset();
}

void WrappableBase::InitWith(v8::Isolate* isolate, v8::Local<v8::Object> wrapper, WrapperInfo* info) {
  isolate_ = isolate;
  wrapper->SetAlignedPointerInInternalField(kWrapperInfoIndex, info);
  wrapper->SetAlignedPointerInInternalField(kEncodedValueIndex, this);
  wrapper_.Reset(isolate, wrapper);
  wrapper_.SetWeak(this, FirstWeakCallback, v8::WeakCallbackType::kParameter);
}

ObjectTemplateBuilder WrappableBase::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return ObjectTemplateBuilder(isolate);
}

void WrappableBase::FirstWeakCallback(
    const v8::WeakCallbackInfo<WrappableBase>& data) {
  WrappableBase* wrappable = data.GetParameter();
  wrappable->wrapper_.Reset();
  data.SetSecondPassCallback(SecondWeakCallback);
}

void WrappableBase::SecondWeakCallback(
    const v8::WeakCallbackInfo<WrappableBase>& data) {
  WrappableBase* wrappable = data.GetParameter();
  delete wrappable;
}

WrappableBase* WrappableBase::GetNativePtr(v8::Local<v8::Object> handle, WrapperInfo* info) {
  if (handle.IsEmpty() || handle->InternalFieldCount() <= 0)
    return nullptr;

  // Cast to ObjectWrap before casting to T.  A direct cast from void
  // to T won't work right when T has more than one base class.
  WrapperInfo* infoSelf = (WrapperInfo*)handle->GetAlignedPointerFromInternalField(kWrapperInfoIndex);
  if (infoSelf != info)
      return nullptr;

  void* ptr = handle->GetAlignedPointerFromInternalField(kEncodedValueIndex);
  WrappableBase* wrap = static_cast<WrappableBase*>(ptr);
  return wrap;
}

v8::Local<v8::Object> WrappableBase::GetWrapperImpl(v8::Isolate* isolate, WrapperInfo* info) {
  if (!wrapper_.IsEmpty()) {
    return v8::Local<v8::Object>::New(isolate, wrapper_);
  }

  PerIsolateData* data = PerIsolateData::From(isolate);
  v8::Local<v8::ObjectTemplate> templ = data->GetObjectTemplate(info);
  if (templ.IsEmpty()) {
    templ = GetObjectTemplateBuilder(isolate).Build();
    CHECK(!templ.IsEmpty());
    data->SetObjectTemplate(info, templ);
  }
  CHECK(kNumberOfInternalFields == templ->InternalFieldCount());
  
  v8::Local<v8::Object> wrapper;
  // |wrapper| may be empty in some extreme cases, e.g., when
  // Object.prototype.constructor is overwritten.
  if (!templ->NewInstance(isolate->GetCurrentContext()).ToLocal(&wrapper)) {
    // The current wrappable object will be no longer managed by V8. Delete this
    // now.
    delete this;
    return wrapper;
  }
  wrapper->SetAlignedPointerInInternalField(kWrapperInfoIndex, info);
  wrapper->SetAlignedPointerInInternalField(kEncodedValueIndex, this);
  wrapper_.Reset(isolate, wrapper);
  wrapper_.SetWeak(this, FirstWeakCallback, v8::WeakCallbackType::kParameter);
  return wrapper;
}

namespace internal {

void* FromV8Impl(v8::Isolate* isolate, v8::Local<v8::Value> val,
                 WrapperInfo* wrapper_info) {
  if (!val->IsObject())
    return NULL;
  v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(val);
  WrapperInfo* info = WrapperInfo::From(obj);

  // If this fails, the object is not managed by Gin. It is either a normal JS
  // object that's not wrapping any external C++ object, or it is wrapping some
  // C++ object, but that object isn't managed by Gin (maybe Blink).
  if (!info)
    return NULL;

  // If this fails, the object is managed by Gin, but it's not wrapping an
  // instance of the C++ class associated with wrapper_info.
  if (info != wrapper_info)
    return NULL;

  return obj->GetAlignedPointerFromInternalField(kEncodedValueIndex);
}

}  // namespace internal

}  // namespace gin
