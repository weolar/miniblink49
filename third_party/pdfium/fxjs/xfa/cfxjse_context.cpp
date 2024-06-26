// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cfxjse_context.h"

#include <utility>

#include "fxjs/cfxjs_engine.h"
#include "fxjs/xfa/cfxjse_class.h"
#include "fxjs/xfa/cfxjse_isolatetracker.h"
#include "fxjs/xfa/cfxjse_runtimedata.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "third_party/base/ptr_util.h"

namespace {

const char szCompatibleModeScript[] =
    "(function(global, list) {\n"
    "  'use strict';\n"
    "  var objname;\n"
    "  for (objname in list) {\n"
    "    var globalobj = global[objname];\n"
    "    if (globalobj) {\n"
    "      list[objname].forEach(function(name) {\n"
    "        if (!globalobj[name]) {\n"
    "          Object.defineProperty(globalobj, name, {\n"
    "            writable: true,\n"
    "            enumerable: false,\n"
    "            value: (function(obj) {\n"
    "              if (arguments.length === 0) {\n"
    "                throw new TypeError('missing argument 0 when calling "
    "                    function ' + objname + '.' + name);\n"
    "              }\n"
    "              return globalobj.prototype[name].apply(obj, "
    "                  Array.prototype.slice.call(arguments, 1));\n"
    "            })\n"
    "          });\n"
    "        }\n"
    "      });\n"
    "    }\n"
    "  }\n"
    "}(this, {String: ['substr', 'toUpperCase']}));";

const char szConsoleScript[] =
    "console.show = function() {};\n"
    "\n"
    "console.println = function(...args) {\n"
    "  this.log(...args);\n"
    "};";

// Only address matters, values are for humans debuging here.
char g_FXJSEHostObjectTag[] = "FXJSE Host Object";
char g_FXJSEProxyObjectTag[] = "FXJSE Proxy Object";

v8::Local<v8::Object> CreateReturnValue(v8::Isolate* pIsolate,
                                        v8::TryCatch* trycatch) {
  v8::Local<v8::Object> hReturnValue = v8::Object::New(pIsolate);
  if (!trycatch->HasCaught())
    return hReturnValue;

  v8::Local<v8::Message> hMessage = trycatch->Message();
  if (hMessage.IsEmpty())
    return hReturnValue;

  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  v8::Local<v8::Value> hException = trycatch->Exception();
  if (hException->IsObject()) {
    v8::Local<v8::String> hNameStr =
        v8::String::NewFromUtf8(pIsolate, "name", v8::NewStringType::kNormal)
            .ToLocalChecked();
    v8::Local<v8::Value> hValue =
        hException.As<v8::Object>()->Get(context, hNameStr).ToLocalChecked();
    if (hValue->IsString() || hValue->IsStringObject()) {
      hReturnValue->Set(context, 0, hValue).FromJust();
    } else {
      v8::Local<v8::String> hErrorStr =
          v8::String::NewFromUtf8(pIsolate, "Error", v8::NewStringType::kNormal)
              .ToLocalChecked();
      hReturnValue->Set(context, 0, hErrorStr).FromJust();
    }

    v8::Local<v8::String> hMessageStr =
        v8::String::NewFromUtf8(pIsolate, "message", v8::NewStringType::kNormal)
            .ToLocalChecked();
    hValue =
        hException.As<v8::Object>()->Get(context, hMessageStr).ToLocalChecked();
    if (hValue->IsString() || hValue->IsStringObject())
      hReturnValue->Set(context, 1, hValue).FromJust();
    else
      hReturnValue->Set(context, 1, hMessage->Get()).FromJust();
  } else {
    v8::Local<v8::String> hErrorStr =
        v8::String::NewFromUtf8(pIsolate, "Error", v8::NewStringType::kNormal)
            .ToLocalChecked();
    hReturnValue->Set(context, 0, hErrorStr).FromJust();
    hReturnValue->Set(context, 1, hMessage->Get()).FromJust();
  }
  hReturnValue->Set(context, 2, hException).FromJust();
  int line = hMessage->GetLineNumber(context).FromMaybe(0);
  hReturnValue->Set(context, 3, v8::Integer::New(pIsolate, line)).FromJust();
  v8::Local<v8::String> source =
      hMessage->GetSourceLine(context).FromMaybe(v8::Local<v8::String>());
  hReturnValue->Set(context, 4, source).FromJust();
  int column = hMessage->GetStartColumn(context).FromMaybe(0);
  hReturnValue->Set(context, 5, v8::Integer::New(pIsolate, column)).FromJust();
  column = hMessage->GetEndColumn(context).FromMaybe(0);
  hReturnValue->Set(context, 6, v8::Integer::New(pIsolate, column)).FromJust();
  return hReturnValue;
}

class CFXJSE_ScopeUtil_IsolateHandleContext {
 public:
  explicit CFXJSE_ScopeUtil_IsolateHandleContext(CFXJSE_Context* pContext)
      : m_parent(pContext->GetIsolate()), m_cscope(pContext->GetContext()) {}

 private:
  CFXJSE_ScopeUtil_IsolateHandleContext(
      const CFXJSE_ScopeUtil_IsolateHandleContext&) = delete;
  void operator=(const CFXJSE_ScopeUtil_IsolateHandleContext&) = delete;
  void* operator new(size_t size) = delete;
  void operator delete(void*, size_t) = delete;

  CFXJSE_ScopeUtil_IsolateHandle m_parent;
  v8::Context::Scope m_cscope;
};

void FXJSE_UpdateProxyBinding(v8::Local<v8::Object> hObject) {
  ASSERT(!hObject.IsEmpty());
  ASSERT(hObject->InternalFieldCount() == 2);
  hObject->SetAlignedPointerInInternalField(0, g_FXJSEProxyObjectTag);
  hObject->SetAlignedPointerInInternalField(1, nullptr);
}

}  // namespace

void FXJSE_UpdateObjectBinding(v8::Local<v8::Object> hObject,
                               CFXJSE_HostObject* lpNewBinding) {
  ASSERT(!hObject.IsEmpty());
  ASSERT(hObject->InternalFieldCount() == 2);
  hObject->SetAlignedPointerInInternalField(0, g_FXJSEHostObjectTag);
  hObject->SetAlignedPointerInInternalField(1, lpNewBinding);
}

CFXJSE_HostObject* FXJSE_RetrieveObjectBinding(
    v8::Local<v8::Object> hJSObject) {
  ASSERT(!hJSObject.IsEmpty());
  if (!hJSObject->IsObject())
    return nullptr;

  v8::Local<v8::Object> hObject = hJSObject;
  if (hObject->InternalFieldCount() != 2 ||
      hObject->GetAlignedPointerFromInternalField(0) == g_FXJSEProxyObjectTag) {
    v8::Local<v8::Value> hProtoObject = hObject->GetPrototype();
    if (hProtoObject.IsEmpty() || !hProtoObject->IsObject())
      return nullptr;

    hObject = hProtoObject.As<v8::Object>();
    if (hObject->InternalFieldCount() != 2)
      return nullptr;
  }
  if (hObject->GetAlignedPointerFromInternalField(0) != g_FXJSEHostObjectTag)
    return nullptr;

  return static_cast<CFXJSE_HostObject*>(
      hObject->GetAlignedPointerFromInternalField(1));
}

// static
std::unique_ptr<CFXJSE_Context> CFXJSE_Context::Create(
    v8::Isolate* pIsolate,
    const FXJSE_CLASS_DESCRIPTOR* pGlobalClass,
    CFXJSE_HostObject* pGlobalObject) {
  CFXJSE_ScopeUtil_IsolateHandle scope(pIsolate);
  auto pContext = pdfium::MakeUnique<CFXJSE_Context>(pIsolate);

  v8::Local<v8::ObjectTemplate> hObjectTemplate;
  if (pGlobalClass) {
    CFXJSE_Class* pGlobalClassObj =
        CFXJSE_Class::Create(pContext.get(), pGlobalClass, true);
    ASSERT(pGlobalClassObj);
    v8::Local<v8::FunctionTemplate> hFunctionTemplate =
        v8::Local<v8::FunctionTemplate>::New(pIsolate,
                                             pGlobalClassObj->m_hTemplate);
    hObjectTemplate = hFunctionTemplate->InstanceTemplate();
  } else {
    hObjectTemplate = v8::ObjectTemplate::New(pIsolate);
    hObjectTemplate->SetInternalFieldCount(2);
  }
  hObjectTemplate->Set(
      v8::Symbol::GetToStringTag(pIsolate),
      v8::String::NewFromUtf8(pIsolate, "global", v8::NewStringType::kNormal)
          .ToLocalChecked());

  v8::Local<v8::Context> hNewContext =
      v8::Context::New(pIsolate, nullptr, hObjectTemplate);

  v8::Local<v8::Object> pThisProxy = hNewContext->Global();
  FXJSE_UpdateProxyBinding(pThisProxy);

  v8::Local<v8::Object> pThis = pThisProxy->GetPrototype().As<v8::Object>();
  FXJSE_UpdateObjectBinding(pThis, pGlobalObject);

  v8::Local<v8::Context> hRootContext = v8::Local<v8::Context>::New(
      pIsolate, CFXJSE_RuntimeData::Get(pIsolate)->m_hRootContext);
  hNewContext->SetSecurityToken(hRootContext->GetSecurityToken());
  pContext->m_hContext.Reset(pIsolate, hNewContext);
  return pContext;
}

CFXJSE_Context::CFXJSE_Context(v8::Isolate* pIsolate) : m_pIsolate(pIsolate) {}

CFXJSE_Context::~CFXJSE_Context() {}

std::unique_ptr<CFXJSE_Value> CFXJSE_Context::GetGlobalObject() {
  auto pValue = pdfium::MakeUnique<CFXJSE_Value>(GetIsolate());
  CFXJSE_ScopeUtil_IsolateHandleContext scope(this);
  v8::Local<v8::Context> hContext =
      v8::Local<v8::Context>::New(GetIsolate(), m_hContext);
  v8::Local<v8::Object> hGlobalObject =
      hContext->Global()->GetPrototype().As<v8::Object>();
  pValue->ForceSetValue(hGlobalObject);
  return pValue;
}

v8::Local<v8::Context> CFXJSE_Context::GetContext() {
  return v8::Local<v8::Context>::New(GetIsolate(), m_hContext);
}

void CFXJSE_Context::AddClass(std::unique_ptr<CFXJSE_Class> pClass) {
  m_rgClasses.push_back(std::move(pClass));
}

CFXJSE_Class* CFXJSE_Context::GetClassByName(ByteStringView szName) const {
  auto pClass =
      std::find_if(m_rgClasses.begin(), m_rgClasses.end(),
                   [szName](const std::unique_ptr<CFXJSE_Class>& item) {
                     return szName == item->m_szClassName;
                   });
  return pClass != m_rgClasses.end() ? pClass->get() : nullptr;
}

void CFXJSE_Context::EnableCompatibleMode() {
  ExecuteScript(szCompatibleModeScript, nullptr, nullptr);
  ExecuteScript(szConsoleScript, nullptr, nullptr);
}

bool CFXJSE_Context::ExecuteScript(const char* szScript,
                                   CFXJSE_Value* lpRetValue,
                                   CFXJSE_Value* lpNewThisObject) {
  CFXJSE_ScopeUtil_IsolateHandleContext scope(this);
  v8::Local<v8::Context> hContext = GetIsolate()->GetCurrentContext();
  v8::TryCatch trycatch(GetIsolate());
  v8::Local<v8::String> hScriptString =
      v8::String::NewFromUtf8(GetIsolate(), szScript,
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  if (!lpNewThisObject) {
    v8::Local<v8::Script> hScript;
    if (v8::Script::Compile(hContext, hScriptString).ToLocal(&hScript)) {
      ASSERT(!trycatch.HasCaught());
      v8::Local<v8::Value> hValue;
      if (hScript->Run(hContext).ToLocal(&hValue)) {
        ASSERT(!trycatch.HasCaught());
        if (lpRetValue)
          lpRetValue->m_hValue.Reset(GetIsolate(), hValue);
        return true;
      }
    }
    if (lpRetValue) {
      lpRetValue->m_hValue.Reset(GetIsolate(),
                                 CreateReturnValue(GetIsolate(), &trycatch));
    }
    return false;
  }

  v8::Local<v8::Value> hNewThis =
      v8::Local<v8::Value>::New(GetIsolate(), lpNewThisObject->m_hValue);
  ASSERT(!hNewThis.IsEmpty());
  v8::Local<v8::String> hEval =
      v8::String::NewFromUtf8(GetIsolate(),
                              "(function () { return eval(arguments[0]); })",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Script> hWrapper =
      v8::Script::Compile(hContext, hEval).ToLocalChecked();
  v8::Local<v8::Value> hWrapperValue;
  if (hWrapper->Run(hContext).ToLocal(&hWrapperValue)) {
    ASSERT(!trycatch.HasCaught());
    v8::Local<v8::Function> hWrapperFn = hWrapperValue.As<v8::Function>();
    v8::Local<v8::Value> rgArgs[] = {hScriptString};
    v8::Local<v8::Value> hValue;
    if (hWrapperFn->Call(hContext, hNewThis.As<v8::Object>(), 1, rgArgs)
            .ToLocal(&hValue)) {
      ASSERT(!trycatch.HasCaught());
      if (lpRetValue)
        lpRetValue->m_hValue.Reset(GetIsolate(), hValue);

      return true;
    }
  }

#ifndef NDEBUG
  v8::String::Utf8Value error(GetIsolate(), trycatch.Exception());
  fprintf(stderr, "JS Error: %s\n", *error);

  v8::Local<v8::Message> message = trycatch.Message();
  if (!message.IsEmpty()) {
    v8::Local<v8::Context> context(GetIsolate()->GetCurrentContext());
    int linenum = message->GetLineNumber(context).FromJust();
    v8::String::Utf8Value sourceline(
        GetIsolate(), message->GetSourceLine(context).ToLocalChecked());
    fprintf(stderr, "Line %d: %s\n", linenum, *sourceline);
  }
#endif  // NDEBUG

  if (lpRetValue) {
    lpRetValue->m_hValue.Reset(GetIsolate(),
                               CreateReturnValue(GetIsolate(), &trycatch));
  }
  return false;
}
