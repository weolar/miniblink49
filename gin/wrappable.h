// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_WRAPPABLE_H_
#define GIN_WRAPPABLE_H_

#include "base/macros.h"
#include "cef/include/base/cef_template_util.h"
#include "gin/converter.h"
#include "gin/gin_export.h"
#include "gin/public/wrapper_info.h"

namespace gin {

// Wrappable is a base class for C++ objects that have corresponding v8 wrapper
// objects. To retain a Wrappable object on the stack, use a gin::Handle.
//
// USAGE:
// // my_class.h
// class MyClass : Wrappable<MyClass> {
//  public:
//   static WrapperInfo kWrapperInfo;
//
//   // Optional, only required if non-empty template should be used.
//   virtual gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
//       v8::Isolate* isolate);
//   ...
// };
//
// // my_class.cc
// WrapperInfo MyClass::kWrapperInfo = {kEmbedderNativeGin};
//
// gin::ObjectTemplateBuilder MyClass::GetObjectTemplateBuilder(
//     v8::Isolate* isolate) {
//   return Wrappable<MyClass>::GetObjectTemplateBuilder(isolate)
//       .SetValue("foobar", 42);
// }
//
// Subclasses should also typically have private constructors and expose a
// static Create function that returns a gin::Handle. Forcing creators through
// this static Create function will enforce that clients actually create a
// wrapper for the object. If clients fail to create a wrapper for a wrappable
// object, the object will leak because we use the weak callback from the
// wrapper as the signal to delete the wrapped object.
//
// Wrappable<T> explicitly does not support further subclassing of T.
// Subclasses of Wrappable<T> should be declared final. Because Wrappable<T>
// caches the object template using &T::kWrapperInfo as the key, all subclasses
// would share a single object template. This will lead to hard to debug crashes
// that look like use-after-free errors.

namespace internal {

GIN_EXPORT void* FromV8Impl(v8::Isolate* isolate,
                            v8::Local<v8::Value> val,
                            WrapperInfo* info);

}  // namespace internal

class ObjectTemplateBuilder;

// Non-template base class to share code between templates instances.
class GIN_EXPORT WrappableBase {
 protected:
  WrappableBase();
  virtual ~WrappableBase();

  void InitWith(v8::Isolate* isolate, v8::Local<v8::Object> wrapper, WrapperInfo* info);

  v8::Isolate* isolate() const { return isolate_; }

  // Overrides of this method should be declared final and not overridden again.
  virtual ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate* isolate);

  v8::Local<v8::Object> GetWrapperImpl(v8::Isolate* isolate, WrapperInfo* wrapper_info);

  static WrappableBase* GetNativePtr(v8::Local<v8::Object> handle, WrapperInfo* info);

 private:
  static void FirstWeakCallback(
      const v8::WeakCallbackInfo<WrappableBase>& data);
  static void SecondWeakCallback(
      const v8::WeakCallbackInfo<WrappableBase>& data);

  v8::Global<v8::Object> wrapper_;  // Weak
  v8::Isolate* isolate_;

  DISALLOW_COPY_AND_ASSIGN(WrappableBase);
};


template<typename T>
class Wrappable : public WrappableBase {
 public:
  void InitWith(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
    WrappableBase::InitWith(isolate, wrapper, &T::kWrapperInfo);
  }

  // Retrieve (or create) the v8 wrapper object cooresponding to this object.
  v8::Local<v8::Object> GetWrapper(v8::Isolate* isolate) {
    return GetWrapperImpl(isolate, &T::kWrapperInfo);
  }

 protected:
  Wrappable() {}
  virtual ~Wrappable() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(Wrappable);
};


// This converter handles any subclass of Wrappable.
template<typename T>
struct Converter<T*, typename base::enable_if<
                       base::is_convertible<T*, WrappableBase*>::value>::type> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate, T* val) {
    return val->GetWrapper(isolate);
  }

  static bool FromV8(v8::Isolate* isolate, v8::Local<v8::Value> val, T** out) {
    *out = static_cast<T*>(static_cast<WrappableBase*>(
        internal::FromV8Impl(isolate, val, &T::kWrapperInfo)));
    return *out != NULL;
  }
};

}  // namespace gin

#endif  // GIN_WRAPPABLE_H_
