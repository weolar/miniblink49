// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_JS_DEFINE_H_
#define FXJS_JS_DEFINE_H_

#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "fxjs/cfxjs_engine.h"
#include "fxjs/cjs_result.h"
#include "fxjs/cjs_runtime.h"
#include "fxjs/js_resources.h"
#include "third_party/base/ptr_util.h"

class CJS_Object;

double JS_DateParse(const WideString& str);

// Some JS methods have the bizarre convention that they may also be called
// with a single argument which is an object containing the actual arguments
// as its properties. The varying arguments to this method are the property
// names as wchar_t string literals corresponding to each positional argument.
// The result will always contain |nKeywords| value, check for the unspecified
// ones in the result using IsExpandedParamKnown() below.
std::vector<v8::Local<v8::Value>> ExpandKeywordParams(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& originals,
    size_t nKeywords,
    ...);

bool IsExpandedParamKnown(v8::Local<v8::Value> value);

// All JS classes have a name, an object defintion ID, and the ability to
// register themselves with FXJS_V8. We never make a BASE class on its own
// because it can't really do anything.

// Rich JS classes provide constants, methods, properties, and the ability
// to construct native object state.

template <class T>
static void JSConstructor(CFXJS_Engine* pEngine, v8::Local<v8::Object> obj) {
  pEngine->SetObjectPrivate(
      obj, pdfium::MakeUnique<T>(obj, static_cast<CJS_Runtime*>(pEngine)));
}

// CJS_Object has virtual dtor, template not required.
void JSDestructor(v8::Local<v8::Object> obj);

template <class C>
UnownedPtr<C> JSGetObject(v8::Local<v8::Object> obj) {
  if (CFXJS_Engine::GetObjDefnID(obj) != C::GetObjDefnID())
    return nullptr;

  CJS_Object* pJSObj = CFXJS_Engine::GetObjectPrivate(obj);
  if (!pJSObj)
    return nullptr;

  return UnownedPtr<C>(static_cast<C*>(pJSObj));
}

template <class C, CJS_Result (C::*M)(CJS_Runtime*)>
void JSPropGetter(const char* prop_name_string,
                  const char* class_name_string,
                  v8::Local<v8::String> property,
                  const v8::PropertyCallbackInfo<v8::Value>& info) {
  auto pObj = JSGetObject<C>(info.Holder());
  if (!pObj)
    return;

  CJS_Runtime* pRuntime = pObj->GetRuntime();
  if (!pRuntime)
    return;

  CJS_Result result = (pObj.Get()->*M)(pRuntime);
  if (result.HasError()) {
    pRuntime->Error(JSFormatErrorString(class_name_string, prop_name_string,
                                        result.Error()));
    return;
  }

  if (result.HasReturn())
    info.GetReturnValue().Set(result.Return());
}

template <class C, CJS_Result (C::*M)(CJS_Runtime*, v8::Local<v8::Value>)>
void JSPropSetter(const char* prop_name_string,
                  const char* class_name_string,
                  v8::Local<v8::String> property,
                  v8::Local<v8::Value> value,
                  const v8::PropertyCallbackInfo<void>& info) {
  auto pObj = JSGetObject<C>(info.Holder());
  if (!pObj)
    return;

  CJS_Runtime* pRuntime = pObj->GetRuntime();
  if (!pRuntime)
    return;

  CJS_Result result = (pObj.Get()->*M)(pRuntime, value);
  if (result.HasError()) {
    pRuntime->Error(JSFormatErrorString(class_name_string, prop_name_string,
                                        result.Error()));
  }
}

template <class C,
          CJS_Result (C::*M)(CJS_Runtime*,
                             const std::vector<v8::Local<v8::Value>>&)>
void JSMethod(const char* method_name_string,
              const char* class_name_string,
              const v8::FunctionCallbackInfo<v8::Value>& info) {
  auto pObj = JSGetObject<C>(info.Holder());
  if (!pObj)
    return;

  CJS_Runtime* pRuntime = pObj->GetRuntime();
  if (!pRuntime)
    return;

  std::vector<v8::Local<v8::Value>> parameters;
  for (unsigned int i = 0; i < (unsigned int)info.Length(); i++)
    parameters.push_back(info[i]);

  CJS_Result result = (pObj.Get()->*M)(pRuntime, parameters);
  if (result.HasError()) {
    pRuntime->Error(JSFormatErrorString(class_name_string, method_name_string,
                                        result.Error()));
    return;
  }

  if (result.HasReturn())
    info.GetReturnValue().Set(result.Return());
}

#define JS_STATIC_PROP(err_name, prop_name, class_name)           \
  static void get_##prop_name##_static(                           \
      v8::Local<v8::String> property,                             \
      const v8::PropertyCallbackInfo<v8::Value>& info) {          \
    JSPropGetter<class_name, &class_name::get_##prop_name>(       \
        #err_name, class_name::kName, property, info);            \
  }                                                               \
  static void set_##prop_name##_static(                           \
      v8::Local<v8::String> property, v8::Local<v8::Value> value, \
      const v8::PropertyCallbackInfo<void>& info) {               \
    JSPropSetter<class_name, &class_name::set_##prop_name>(       \
        #err_name, class_name::kName, property, value, info);     \
  }

#define JS_STATIC_METHOD(method_name, class_name)                            \
  static void method_name##_static(                                          \
      const v8::FunctionCallbackInfo<v8::Value>& info) {                     \
    JSMethod<class_name, &class_name::method_name>(#method_name,             \
                                                   class_name::kName, info); \
  }

#endif  // FXJS_JS_DEFINE_H_
