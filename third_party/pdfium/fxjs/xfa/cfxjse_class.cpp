// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cfxjse_class.h"

#include <memory>
#include <utility>

#include "fxjs/cjs_result.h"
#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_arguments.h"
#include "fxjs/xfa/cfxjse_context.h"
#include "fxjs/xfa/cfxjse_isolatetracker.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "third_party/base/ptr_util.h"

using pdfium::fxjse::kFuncTag;
using pdfium::fxjse::kClassTag;

namespace {

FXJSE_FUNCTION_DESCRIPTOR* AsFunctionDescriptor(void* ptr) {
  auto* result = static_cast<FXJSE_FUNCTION_DESCRIPTOR*>(ptr);
  return result && result->tag == kFuncTag ? result : nullptr;
}

FXJSE_CLASS_DESCRIPTOR* AsClassDescriptor(void* ptr) {
  auto* result = static_cast<FXJSE_CLASS_DESCRIPTOR*>(ptr);
  return result && result->tag == kClassTag ? result : nullptr;
}

void V8FunctionCallback_Wrapper(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  const FXJSE_FUNCTION_DESCRIPTOR* lpFunctionInfo =
      AsFunctionDescriptor(info.Data().As<v8::External>()->Value());
  if (!lpFunctionInfo)
    return;

  ByteStringView szFunctionName(lpFunctionInfo->name);
  auto lpThisValue = pdfium::MakeUnique<CFXJSE_Value>(info.GetIsolate());
  lpThisValue->ForceSetValue(info.Holder());
  auto lpRetValue = pdfium::MakeUnique<CFXJSE_Value>(info.GetIsolate());
  CFXJSE_Arguments impl(&info, lpRetValue.get());
  lpFunctionInfo->callbackProc(lpThisValue.get(), szFunctionName, impl);
  if (!lpRetValue->DirectGetValue().IsEmpty())
    info.GetReturnValue().Set(lpRetValue->DirectGetValue());
}

void V8ConstructorCallback_Wrapper(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (!info.IsConstructCall())
    return;

  const FXJSE_CLASS_DESCRIPTOR* lpClassDefinition =
      AsClassDescriptor(info.Data().As<v8::External>()->Value());
  if (!lpClassDefinition)
    return;

  ASSERT(info.Holder()->InternalFieldCount() == 2);
  info.Holder()->SetAlignedPointerInInternalField(0, nullptr);
  info.Holder()->SetAlignedPointerInInternalField(1, nullptr);
}

void Context_GlobalObjToString(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  const FXJSE_CLASS_DESCRIPTOR* lpClass =
      AsClassDescriptor(info.Data().As<v8::External>()->Value());
  if (!lpClass)
    return;

  if (info.This() == info.Holder() && lpClass->name) {
    ByteString szStringVal = ByteString::Format("[object %s]", lpClass->name);
    info.GetReturnValue().Set(
        v8::String::NewFromUtf8(info.GetIsolate(), szStringVal.c_str(),
                                v8::NewStringType::kNormal,
                                szStringVal.GetLength())
            .ToLocalChecked());
    return;
  }
  v8::Local<v8::String> local_str =
      info.Holder()
          ->ObjectProtoToString(info.GetIsolate()->GetCurrentContext())
          .FromMaybe(v8::Local<v8::String>());
  info.GetReturnValue().Set(local_str);
}

void DynPropGetterAdapter_MethodCallback(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Local<v8::Object> hCallBackInfo = info.Data().As<v8::Object>();
  if (hCallBackInfo->InternalFieldCount() != 2)
    return;

  auto* pClassDescriptor = static_cast<const FXJSE_CLASS_DESCRIPTOR*>(
      hCallBackInfo->GetAlignedPointerFromInternalField(0));
  if (pClassDescriptor != &GlobalClassDescriptor &&
      pClassDescriptor != &NormalClassDescriptor &&
      pClassDescriptor != &VariablesClassDescriptor &&
      pClassDescriptor != &kFormCalcFM2JSDescriptor) {
    return;
  }

  v8::Local<v8::String> hPropName =
      hCallBackInfo->GetInternalField(1).As<v8::String>();
  if (hPropName.IsEmpty())
    return;

  v8::String::Utf8Value szPropName(info.GetIsolate(), hPropName);
  CJS_Result result =
      pClassDescriptor->dynMethodCall(info, WideString::FromUTF8(*szPropName));

  if (result.HasError()) {
    WideString err = JSFormatErrorString(pClassDescriptor->name, *szPropName,
                                         result.Error());
    v8::MaybeLocal<v8::String> str = v8::String::NewFromUtf8(
        info.GetIsolate(), err.ToDefANSI().c_str(), v8::NewStringType::kNormal);
    info.GetIsolate()->ThrowException(str.ToLocalChecked());
    return;
  }

  if (result.HasReturn())
    info.GetReturnValue().Set(result.Return());
}

void DynPropGetterAdapter(const FXJSE_CLASS_DESCRIPTOR* lpClass,
                          CFXJSE_Value* pObject,
                          ByteStringView szPropName,
                          CFXJSE_Value* pValue) {
  ASSERT(lpClass);

  int32_t nPropType =
      lpClass->dynPropTypeGetter == nullptr
          ? FXJSE_ClassPropType_Property
          : lpClass->dynPropTypeGetter(pObject, szPropName, false);
  if (nPropType == FXJSE_ClassPropType_Property) {
    if (lpClass->dynPropGetter)
      lpClass->dynPropGetter(pObject, szPropName, pValue);
  } else if (nPropType == FXJSE_ClassPropType_Method) {
    if (lpClass->dynMethodCall && pValue) {
      v8::Isolate* pIsolate = pValue->GetIsolate();
      v8::HandleScope hscope(pIsolate);
      v8::Local<v8::ObjectTemplate> hCallBackInfoTemplate =
          v8::ObjectTemplate::New(pIsolate);
      hCallBackInfoTemplate->SetInternalFieldCount(2);
      v8::Local<v8::Object> hCallBackInfo =
          hCallBackInfoTemplate
              ->NewInstance(pValue->GetIsolate()->GetCurrentContext())
              .ToLocalChecked();
      hCallBackInfo->SetAlignedPointerInInternalField(
          0, const_cast<FXJSE_CLASS_DESCRIPTOR*>(lpClass));
      hCallBackInfo->SetInternalField(
          1, v8::String::NewFromUtf8(
                 pIsolate, reinterpret_cast<const char*>(szPropName.raw_str()),
                 v8::NewStringType::kNormal, szPropName.GetLength())
                 .ToLocalChecked());
      pValue->ForceSetValue(
          v8::Function::New(pValue->GetIsolate()->GetCurrentContext(),
                            DynPropGetterAdapter_MethodCallback, hCallBackInfo,
                            0, v8::ConstructorBehavior::kThrow)
              .ToLocalChecked());
    }
  }
}

void DynPropSetterAdapter(const FXJSE_CLASS_DESCRIPTOR* lpClass,
                          CFXJSE_Value* pObject,
                          ByteStringView szPropName,
                          CFXJSE_Value* pValue) {
  ASSERT(lpClass);
  int32_t nPropType =
      lpClass->dynPropTypeGetter == nullptr
          ? FXJSE_ClassPropType_Property
          : lpClass->dynPropTypeGetter(pObject, szPropName, false);
  if (nPropType != FXJSE_ClassPropType_Method) {
    if (lpClass->dynPropSetter)
      lpClass->dynPropSetter(pObject, szPropName, pValue);
  }
}

bool DynPropQueryAdapter(const FXJSE_CLASS_DESCRIPTOR* lpClass,
                         CFXJSE_Value* pObject,
                         ByteStringView szPropName) {
  ASSERT(lpClass);
  int32_t nPropType =
      lpClass->dynPropTypeGetter == nullptr
          ? FXJSE_ClassPropType_Property
          : lpClass->dynPropTypeGetter(pObject, szPropName, true);
  return nPropType != FXJSE_ClassPropType_None;
}

void NamedPropertyQueryCallback(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Integer>& info) {
  v8::Local<v8::Object> thisObject = info.Holder();
  const FXJSE_CLASS_DESCRIPTOR* lpClass =
      AsClassDescriptor(info.Data().As<v8::External>()->Value());
  if (!lpClass)
    return;

  v8::HandleScope scope(info.GetIsolate());
  v8::String::Utf8Value szPropName(info.GetIsolate(), property);
  ByteStringView szFxPropName(*szPropName, szPropName.length());
  auto lpThisValue = pdfium::MakeUnique<CFXJSE_Value>(info.GetIsolate());
  lpThisValue->ForceSetValue(thisObject);
  if (DynPropQueryAdapter(lpClass, lpThisValue.get(), szFxPropName)) {
    info.GetReturnValue().Set(v8::DontDelete);
    return;
  }
  const int32_t iV8Absent = 64;
  info.GetReturnValue().Set(iV8Absent);
}

void NamedPropertyGetterCallback(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Local<v8::Object> thisObject = info.Holder();
  const FXJSE_CLASS_DESCRIPTOR* lpClass =
      AsClassDescriptor(info.Data().As<v8::External>()->Value());
  if (!lpClass)
    return;

  v8::String::Utf8Value szPropName(info.GetIsolate(), property);
  ByteStringView szFxPropName(*szPropName, szPropName.length());
  auto lpThisValue = pdfium::MakeUnique<CFXJSE_Value>(info.GetIsolate());
  lpThisValue->ForceSetValue(thisObject);
  auto lpNewValue = pdfium::MakeUnique<CFXJSE_Value>(info.GetIsolate());
  DynPropGetterAdapter(lpClass, lpThisValue.get(), szFxPropName,
                       lpNewValue.get());
  info.GetReturnValue().Set(lpNewValue->DirectGetValue());
}

void NamedPropertySetterCallback(
    v8::Local<v8::Name> property,
    v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Local<v8::Object> thisObject = info.Holder();
  const FXJSE_CLASS_DESCRIPTOR* lpClass =
      AsClassDescriptor(info.Data().As<v8::External>()->Value());
  if (!lpClass)
    return;

  v8::String::Utf8Value szPropName(info.GetIsolate(), property);
  ByteStringView szFxPropName(*szPropName, szPropName.length());
  auto lpThisValue = pdfium::MakeUnique<CFXJSE_Value>(info.GetIsolate());
  lpThisValue->ForceSetValue(thisObject);
  auto lpNewValue = pdfium::MakeUnique<CFXJSE_Value>(info.GetIsolate());
  lpNewValue->ForceSetValue(value);
  DynPropSetterAdapter(lpClass, lpThisValue.get(), szFxPropName,
                       lpNewValue.get());
  info.GetReturnValue().Set(value);
}

void NamedPropertyEnumeratorCallback(
    const v8::PropertyCallbackInfo<v8::Array>& info) {
  info.GetReturnValue().Set(v8::Array::New(info.GetIsolate()));
}

void SetUpNamedPropHandler(v8::Isolate* pIsolate,
                           v8::Local<v8::ObjectTemplate>* pObjectTemplate,
                           const FXJSE_CLASS_DESCRIPTOR* lpClassDefinition) {
  v8::NamedPropertyHandlerConfiguration configuration(
      lpClassDefinition->dynPropGetter ? NamedPropertyGetterCallback : nullptr,
      lpClassDefinition->dynPropSetter ? NamedPropertySetterCallback : nullptr,
      lpClassDefinition->dynPropTypeGetter ? NamedPropertyQueryCallback
                                           : nullptr,
      nullptr, NamedPropertyEnumeratorCallback,
      v8::External::New(pIsolate,
                        const_cast<FXJSE_CLASS_DESCRIPTOR*>(lpClassDefinition)),
      v8::PropertyHandlerFlags::kNonMasking);
  (*pObjectTemplate)->SetHandler(configuration);
}

}  // namespace

// static
CFXJSE_Class* CFXJSE_Class::Create(
    CFXJSE_Context* lpContext,
    const FXJSE_CLASS_DESCRIPTOR* lpClassDefinition,
    bool bIsJSGlobal) {
  if (!lpContext || !lpClassDefinition)
    return nullptr;

  CFXJSE_Class* pExistingClass =
      lpContext->GetClassByName(lpClassDefinition->name);
  if (pExistingClass)
    return pExistingClass;

  v8::Isolate* pIsolate = lpContext->GetIsolate();
  auto pClass = pdfium::MakeUnique<CFXJSE_Class>(lpContext);
  pClass->m_szClassName = lpClassDefinition->name;
  pClass->m_lpClassDefinition = lpClassDefinition;
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(pIsolate);
  v8::Local<v8::FunctionTemplate> hFunctionTemplate = v8::FunctionTemplate::New(
      pIsolate, bIsJSGlobal ? 0 : V8ConstructorCallback_Wrapper,
      v8::External::New(
          pIsolate, const_cast<FXJSE_CLASS_DESCRIPTOR*>(lpClassDefinition)));
  hFunctionTemplate->SetClassName(
      v8::String::NewFromUtf8(pIsolate, lpClassDefinition->name,
                              v8::NewStringType::kNormal)
          .ToLocalChecked());
  hFunctionTemplate->InstanceTemplate()->SetInternalFieldCount(2);
  v8::Local<v8::ObjectTemplate> hObjectTemplate =
      hFunctionTemplate->InstanceTemplate();
  SetUpNamedPropHandler(pIsolate, &hObjectTemplate, lpClassDefinition);

  if (lpClassDefinition->methNum) {
    for (int32_t i = 0; i < lpClassDefinition->methNum; i++) {
      v8::Local<v8::FunctionTemplate> fun = v8::FunctionTemplate::New(
          pIsolate, V8FunctionCallback_Wrapper,
          v8::External::New(pIsolate, const_cast<FXJSE_FUNCTION_DESCRIPTOR*>(
                                          lpClassDefinition->methods + i)));
      fun->RemovePrototype();
      hObjectTemplate->Set(
          v8::String::NewFromUtf8(pIsolate, lpClassDefinition->methods[i].name,
                                  v8::NewStringType::kNormal)
              .ToLocalChecked(),
          fun,
          static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
    }
  }

  if (bIsJSGlobal) {
    v8::Local<v8::FunctionTemplate> fun = v8::FunctionTemplate::New(
        pIsolate, Context_GlobalObjToString,
        v8::External::New(
            pIsolate, const_cast<FXJSE_CLASS_DESCRIPTOR*>(lpClassDefinition)));
    fun->RemovePrototype();
    hObjectTemplate->Set(v8::String::NewFromUtf8(pIsolate, "toString",
                                                 v8::NewStringType::kNormal)
                             .ToLocalChecked(),
                         fun);
  }
  pClass->m_hTemplate.Reset(lpContext->GetIsolate(), hFunctionTemplate);
  CFXJSE_Class* pResult = pClass.get();
  lpContext->AddClass(std::move(pClass));
  return pResult;
}

CFXJSE_Class::CFXJSE_Class(CFXJSE_Context* lpContext) : m_pContext(lpContext) {}

CFXJSE_Class::~CFXJSE_Class() {}
