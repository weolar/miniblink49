// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_global.h"

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_extension.h"
#include "fxjs/cfx_globaldata.h"
#include "fxjs/cfx_keyvalue.h"
#include "fxjs/cjs_event_context.h"
#include "fxjs/cjs_eventhandler.h"
#include "fxjs/cjs_object.h"
#include "fxjs/js_define.h"
#include "fxjs/js_resources.h"
#include "third_party/base/ptr_util.h"

namespace {

WideString PropFromV8Prop(v8::Isolate* pIsolate,
                          v8::Local<v8::String> property) {
  v8::String::Utf8Value utf8_value(pIsolate, property);
  return WideString::FromUTF8(ByteStringView(*utf8_value, utf8_value.length()));
}

template <class Alt>
void JSSpecialPropQuery(const char*,
                        v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Integer>& info) {
  auto pObj = JSGetObject<Alt>(info.Holder());
  if (!pObj)
    return;

  CJS_Runtime* pRuntime = pObj->GetRuntime();
  if (!pRuntime)
    return;

  CJS_Result result =
      pObj->QueryProperty(PropFromV8Prop(info.GetIsolate(), property).c_str());

  info.GetReturnValue().Set(!result.HasError() ? 4 : 0);
}

template <class Alt>
void JSSpecialPropGet(const char* class_name,
                      v8::Local<v8::String> property,
                      const v8::PropertyCallbackInfo<v8::Value>& info) {
  auto pObj = JSGetObject<Alt>(info.Holder());
  if (!pObj)
    return;

  CJS_Runtime* pRuntime = pObj->GetRuntime();
  if (!pRuntime)
    return;

  CJS_Result result = pObj->GetProperty(
      pRuntime, PropFromV8Prop(info.GetIsolate(), property).c_str());

  if (result.HasError()) {
    pRuntime->Error(
        JSFormatErrorString(class_name, "GetProperty", result.Error()));
    return;
  }
  if (result.HasReturn())
    info.GetReturnValue().Set(result.Return());
}

template <class Alt>
void JSSpecialPropPut(const char* class_name,
                      v8::Local<v8::String> property,
                      v8::Local<v8::Value> value,
                      const v8::PropertyCallbackInfo<v8::Value>& info) {
  auto pObj = JSGetObject<Alt>(info.Holder());
  if (!pObj)
    return;

  CJS_Runtime* pRuntime = pObj->GetRuntime();
  if (!pRuntime)
    return;

  CJS_Result result = pObj->SetProperty(
      pRuntime, PropFromV8Prop(info.GetIsolate(), property).c_str(), value);

  if (result.HasError()) {
    pRuntime->Error(
        JSFormatErrorString(class_name, "PutProperty", result.Error()));
  }
}

template <class Alt>
void JSSpecialPropDel(const char* class_name,
                      v8::Local<v8::String> property,
                      const v8::PropertyCallbackInfo<v8::Boolean>& info) {
  auto pObj = JSGetObject<Alt>(info.Holder());
  if (!pObj)
    return;

  CJS_Runtime* pRuntime = pObj->GetRuntime();
  if (!pRuntime)
    return;

  CJS_Result result = pObj->DelProperty(
      pRuntime, PropFromV8Prop(info.GetIsolate(), property).c_str());
  if (result.HasError()) {
    // TODO(dsinclair): Should this set the pRuntime->Error result?
    // ByteString cbName =
    //     ByteString::Format("%s.%s", class_name, "DelProperty");
  }
}

template <class T>
v8::Local<v8::String> GetV8StringFromProperty(v8::Local<v8::Name> property,
                                              const T& info) {
  return property->ToString(info.GetIsolate()->GetCurrentContext())
      .ToLocalChecked();
}

}  // namespace

CJS_Global::JSGlobalData::JSGlobalData() = default;

CJS_Global::JSGlobalData::~JSGlobalData() = default;

const JSMethodSpec CJS_Global::MethodSpecs[] = {
    {"setPersistent", setPersistent_static}};

int CJS_Global::ObjDefnID = -1;

// static
void CJS_Global::setPersistent_static(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  JSMethod<CJS_Global, &CJS_Global::setPersistent>("setPersistent", "global",
                                                   info);
}

// static
void CJS_Global::queryprop_static(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Integer>& info) {
  ASSERT(property->IsString());
  JSSpecialPropQuery<CJS_Global>(
      "global",
      v8::Local<v8::String>::New(info.GetIsolate(),
                                 GetV8StringFromProperty(property, info)),
      info);
}

// static
void CJS_Global::getprop_static(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(property->IsString());
  JSSpecialPropGet<CJS_Global>(
      "global",
      v8::Local<v8::String>::New(info.GetIsolate(),
                                 GetV8StringFromProperty(property, info)),
      info);
}

// static
void CJS_Global::putprop_static(
    v8::Local<v8::Name> property,
    v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(property->IsString());
  JSSpecialPropPut<CJS_Global>(
      "global",
      v8::Local<v8::String>::New(info.GetIsolate(),
                                 GetV8StringFromProperty(property, info)),
      value, info);
}

// static
void CJS_Global::delprop_static(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Boolean>& info) {
  ASSERT(property->IsString());
  JSSpecialPropDel<CJS_Global>(
      "global",
      v8::Local<v8::String>::New(info.GetIsolate(),
                                 GetV8StringFromProperty(property, info)),
      info);
}

// static
void CJS_Global::DefineAllProperties(CFXJS_Engine* pEngine) {
  pEngine->DefineObjAllProperties(
      ObjDefnID, CJS_Global::queryprop_static, CJS_Global::getprop_static,
      CJS_Global::putprop_static, CJS_Global::delprop_static);
}

// static
int CJS_Global::GetObjDefnID() {
  return ObjDefnID;
}

// static
void CJS_Global::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID = pEngine->DefineObj("global", FXJSOBJTYPE_STATIC,
                                 JSConstructor<CJS_Global>, JSDestructor);
  DefineMethods(pEngine, ObjDefnID, MethodSpecs);
  DefineAllProperties(pEngine);
}

CJS_Global::CJS_Global(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime)
    : CJS_Object(pObject, pRuntime) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv = GetRuntime()->GetFormFillEnv();
  m_pFormFillEnv.Reset(pFormFillEnv);
  m_pGlobalData = CFX_GlobalData::GetRetainedInstance(nullptr);
  UpdateGlobalPersistentVariables();
}

CJS_Global::~CJS_Global() {
  DestroyGlobalPersisitentVariables();
  m_pGlobalData->Release();
}

CJS_Result CJS_Global::QueryProperty(const wchar_t* propname) {
  if (WideString(propname).EqualsASCII("setPersistent"))
    return CJS_Result::Success();

  return CJS_Result::Failure(JSMessage::kUnknownProperty);
}

CJS_Result CJS_Global::DelProperty(CJS_Runtime* pRuntime,
                                   const wchar_t* propname) {
  auto it = m_MapGlobal.find(WideString(propname).ToDefANSI());
  if (it == m_MapGlobal.end())
    return CJS_Result::Failure(JSMessage::kUnknownProperty);

  it->second->bDeleted = true;
  return CJS_Result::Success();
}

CJS_Result CJS_Global::GetProperty(CJS_Runtime* pRuntime,
                                   const wchar_t* propname) {
  auto it = m_MapGlobal.find(WideString(propname).ToDefANSI());
  if (it == m_MapGlobal.end())
    return CJS_Result::Success();

  JSGlobalData* pData = it->second.get();
  if (pData->bDeleted)
    return CJS_Result::Success();

  switch (pData->nType) {
    case CFX_Value::DataType::NUMBER:
      return CJS_Result::Success(pRuntime->NewNumber(pData->dData));
    case CFX_Value::DataType::BOOLEAN:
      return CJS_Result::Success(pRuntime->NewBoolean(pData->bData));
    case CFX_Value::DataType::STRING:
      return CJS_Result::Success(pRuntime->NewString(
          WideString::FromDefANSI(pData->sData.AsStringView()).AsStringView()));
    case CFX_Value::DataType::OBJECT:
      return CJS_Result::Success(
          v8::Local<v8::Object>::New(pRuntime->GetIsolate(), pData->pData));
    case CFX_Value::DataType::NULLOBJ:
      return CJS_Result::Success(pRuntime->NewNull());
    default:
      break;
  }
  return CJS_Result::Failure(JSMessage::kObjectTypeError);
}

CJS_Result CJS_Global::SetProperty(CJS_Runtime* pRuntime,
                                   const wchar_t* propname,
                                   v8::Local<v8::Value> vp) {
  ByteString sPropName = WideString(propname).ToDefANSI();
  if (vp->IsNumber()) {
    return SetGlobalVariables(sPropName, CFX_Value::DataType::NUMBER,
                              pRuntime->ToDouble(vp), false, ByteString(),
                              v8::Local<v8::Object>(), false);
  }
  if (vp->IsBoolean()) {
    return SetGlobalVariables(sPropName, CFX_Value::DataType::BOOLEAN, 0,
                              pRuntime->ToBoolean(vp), ByteString(),
                              v8::Local<v8::Object>(), false);
  }
  if (vp->IsString()) {
    return SetGlobalVariables(sPropName, CFX_Value::DataType::STRING, 0, false,
                              pRuntime->ToWideString(vp).ToDefANSI(),
                              v8::Local<v8::Object>(), false);
  }
  if (vp->IsObject()) {
    return SetGlobalVariables(sPropName, CFX_Value::DataType::OBJECT, 0, false,
                              ByteString(), pRuntime->ToObject(vp), false);
  }
  if (vp->IsNull()) {
    return SetGlobalVariables(sPropName, CFX_Value::DataType::NULLOBJ, 0, false,
                              ByteString(), v8::Local<v8::Object>(), false);
  }
  if (vp->IsUndefined()) {
    DelProperty(pRuntime, propname);
    return CJS_Result::Success();
  }
  return CJS_Result::Failure(JSMessage::kObjectTypeError);
}

CJS_Result CJS_Global::setPersistent(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  auto it = m_MapGlobal.find(pRuntime->ToWideString(params[0]).ToDefANSI());
  if (it == m_MapGlobal.end() || it->second->bDeleted)
    return CJS_Result::Failure(JSMessage::kGlobalNotFoundError);

  it->second->bPersistent = pRuntime->ToBoolean(params[1]);
  return CJS_Result::Success();
}

void CJS_Global::UpdateGlobalPersistentVariables() {
  CJS_Runtime* pRuntime = GetRuntime();
  if (!pRuntime)
    return;

  for (int i = 0, sz = m_pGlobalData->GetSize(); i < sz; i++) {
    CFX_GlobalData::Element* pData = m_pGlobalData->GetAt(i);
    switch (pData->data.nType) {
      case CFX_Value::DataType::NUMBER:
        SetGlobalVariables(pData->data.sKey, CFX_Value::DataType::NUMBER,
                           pData->data.dData, false, ByteString(),
                           v8::Local<v8::Object>(), pData->bPersistent == 1);
        pRuntime->PutObjectProperty(ToV8Object(),
                                    pData->data.sKey.AsStringView(),
                                    pRuntime->NewNumber(pData->data.dData));
        break;
      case CFX_Value::DataType::BOOLEAN:
        SetGlobalVariables(pData->data.sKey, CFX_Value::DataType::BOOLEAN, 0,
                           pData->data.bData == 1, ByteString(),
                           v8::Local<v8::Object>(), pData->bPersistent == 1);
        pRuntime->PutObjectProperty(
            ToV8Object(), pData->data.sKey.AsStringView(),
            pRuntime->NewBoolean(pData->data.bData == 1));
        break;
      case CFX_Value::DataType::STRING:
        SetGlobalVariables(pData->data.sKey, CFX_Value::DataType::STRING, 0,
                           false, pData->data.sData, v8::Local<v8::Object>(),
                           pData->bPersistent == 1);
        pRuntime->PutObjectProperty(
            ToV8Object(), pData->data.sKey.AsStringView(),
            pRuntime->NewString(
                WideString::FromUTF8(pData->data.sData.AsStringView())
                    .AsStringView()));
        break;
      case CFX_Value::DataType::OBJECT: {
        v8::Local<v8::Object> pObj = pRuntime->NewObject();
        if (!pObj.IsEmpty()) {
          PutObjectProperty(pObj, &pData->data);
          SetGlobalVariables(pData->data.sKey, CFX_Value::DataType::OBJECT, 0,
                             false, ByteString(), pObj,
                             pData->bPersistent == 1);
          pRuntime->PutObjectProperty(ToV8Object(),
                                      pData->data.sKey.AsStringView(), pObj);
        }
      } break;
      case CFX_Value::DataType::NULLOBJ:
        SetGlobalVariables(pData->data.sKey, CFX_Value::DataType::NULLOBJ, 0,
                           false, ByteString(), v8::Local<v8::Object>(),
                           pData->bPersistent == 1);
        pRuntime->PutObjectProperty(
            ToV8Object(), pData->data.sKey.AsStringView(), pRuntime->NewNull());
        break;
    }
  }
}

void CJS_Global::CommitGlobalPersisitentVariables(CJS_Runtime* pRuntime) {
  for (const auto& iter : m_MapGlobal) {
    ByteString name = iter.first;
    JSGlobalData* pData = iter.second.get();
    if (pData->bDeleted) {
      m_pGlobalData->DeleteGlobalVariable(name);
      continue;
    }
    switch (pData->nType) {
      case CFX_Value::DataType::NUMBER:
        m_pGlobalData->SetGlobalVariableNumber(name, pData->dData);
        m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
        break;
      case CFX_Value::DataType::BOOLEAN:
        m_pGlobalData->SetGlobalVariableBoolean(name, pData->bData);
        m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
        break;
      case CFX_Value::DataType::STRING:
        m_pGlobalData->SetGlobalVariableString(name, pData->sData);
        m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
        break;
      case CFX_Value::DataType::OBJECT: {
        CFX_GlobalArray array;
        v8::Local<v8::Object> obj =
            v8::Local<v8::Object>::New(GetIsolate(), pData->pData);
        ObjectToArray(pRuntime, obj, &array);
        m_pGlobalData->SetGlobalVariableObject(name, std::move(array));
        m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
      } break;
      case CFX_Value::DataType::NULLOBJ:
        m_pGlobalData->SetGlobalVariableNull(name);
        m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
        break;
    }
  }
}

void CJS_Global::ObjectToArray(CJS_Runtime* pRuntime,
                               v8::Local<v8::Object> pObj,
                               CFX_GlobalArray* pArray) {
  std::vector<WideString> pKeyList = pRuntime->GetObjectPropertyNames(pObj);
  for (const auto& ws : pKeyList) {
    ByteString sKey = ws.ToUTF8();
    v8::Local<v8::Value> v =
        pRuntime->GetObjectProperty(pObj, sKey.AsStringView());
    if (v->IsNumber()) {
      auto pObjElement = pdfium::MakeUnique<CFX_KeyValue>();
      pObjElement->nType = CFX_Value::DataType::NUMBER;
      pObjElement->sKey = sKey;
      pObjElement->dData = pRuntime->ToDouble(v);
      pArray->Add(std::move(pObjElement));
      continue;
    }
    if (v->IsBoolean()) {
      auto pObjElement = pdfium::MakeUnique<CFX_KeyValue>();
      pObjElement->nType = CFX_Value::DataType::BOOLEAN;
      pObjElement->sKey = sKey;
      pObjElement->dData = pRuntime->ToBoolean(v);
      pArray->Add(std::move(pObjElement));
      continue;
    }
    if (v->IsString()) {
      ByteString sValue = pRuntime->ToWideString(v).ToDefANSI();
      auto pObjElement = pdfium::MakeUnique<CFX_KeyValue>();
      pObjElement->nType = CFX_Value::DataType::STRING;
      pObjElement->sKey = sKey;
      pObjElement->sData = sValue;
      pArray->Add(std::move(pObjElement));
      continue;
    }
    if (v->IsObject()) {
      auto pObjElement = pdfium::MakeUnique<CFX_KeyValue>();
      pObjElement->nType = CFX_Value::DataType::OBJECT;
      pObjElement->sKey = sKey;
      ObjectToArray(pRuntime, pRuntime->ToObject(v), &pObjElement->objData);
      pArray->Add(std::move(pObjElement));
      continue;
    }
    if (v->IsNull()) {
      auto pObjElement = pdfium::MakeUnique<CFX_KeyValue>();
      pObjElement->nType = CFX_Value::DataType::NULLOBJ;
      pObjElement->sKey = sKey;
      pArray->Add(std::move(pObjElement));
    }
  }
}

void CJS_Global::PutObjectProperty(v8::Local<v8::Object> pObj,
                                   CFX_KeyValue* pData) {
  CJS_Runtime* pRuntime = GetRuntime();
  if (pRuntime)
    return;

  for (int i = 0, sz = pData->objData.Count(); i < sz; i++) {
    CFX_KeyValue* pObjData = pData->objData.GetAt(i);
    switch (pObjData->nType) {
      case CFX_Value::DataType::NUMBER:
        pRuntime->PutObjectProperty(pObj, pObjData->sKey.AsStringView(),
                                    pRuntime->NewNumber(pObjData->dData));
        break;
      case CFX_Value::DataType::BOOLEAN:
        pRuntime->PutObjectProperty(pObj, pObjData->sKey.AsStringView(),
                                    pRuntime->NewBoolean(pObjData->bData == 1));
        break;
      case CFX_Value::DataType::STRING:
        pRuntime->PutObjectProperty(
            pObj, pObjData->sKey.AsStringView(),
            pRuntime->NewString(
                WideString::FromUTF8(pObjData->sData.AsStringView())
                    .AsStringView()));
        break;
      case CFX_Value::DataType::OBJECT: {
        v8::Local<v8::Object> pNewObj = pRuntime->NewObject();
        if (!pNewObj.IsEmpty()) {
          PutObjectProperty(pNewObj, pObjData);
          pRuntime->PutObjectProperty(pObj, pObjData->sKey.AsStringView(),
                                      pNewObj);
        }
      } break;
      case CFX_Value::DataType::NULLOBJ:
        pRuntime->PutObjectProperty(pObj, pObjData->sKey.AsStringView(),
                                    pRuntime->NewNull());
        break;
    }
  }
}

void CJS_Global::DestroyGlobalPersisitentVariables() {
  m_MapGlobal.clear();
}

CJS_Result CJS_Global::SetGlobalVariables(const ByteString& propname,
                                          CFX_Value::DataType nType,
                                          double dData,
                                          bool bData,
                                          const ByteString& sData,
                                          v8::Local<v8::Object> pData,
                                          bool bDefaultPersistent) {
  if (propname.IsEmpty())
    return CJS_Result::Failure(JSMessage::kUnknownProperty);

  auto it = m_MapGlobal.find(propname);
  if (it != m_MapGlobal.end()) {
    JSGlobalData* pTemp = it->second.get();
    if (pTemp->bDeleted || pTemp->nType != nType) {
      pTemp->dData = 0;
      pTemp->bData = 0;
      pTemp->sData.clear();
      pTemp->nType = nType;
    }
    pTemp->bDeleted = false;
    switch (nType) {
      case CFX_Value::DataType::NUMBER:
        pTemp->dData = dData;
        break;
      case CFX_Value::DataType::BOOLEAN:
        pTemp->bData = bData;
        break;
      case CFX_Value::DataType::STRING:
        pTemp->sData = sData;
        break;
      case CFX_Value::DataType::OBJECT:
        pTemp->pData.Reset(pData->GetIsolate(), pData);
        break;
      case CFX_Value::DataType::NULLOBJ:
        break;
      default:
        return CJS_Result::Failure(JSMessage::kObjectTypeError);
    }
    return CJS_Result::Success();
  }

  auto pNewData = pdfium::MakeUnique<JSGlobalData>();
  switch (nType) {
    case CFX_Value::DataType::NUMBER:
      pNewData->nType = CFX_Value::DataType::NUMBER;
      pNewData->dData = dData;
      pNewData->bPersistent = bDefaultPersistent;
      break;
    case CFX_Value::DataType::BOOLEAN:
      pNewData->nType = CFX_Value::DataType::BOOLEAN;
      pNewData->bData = bData;
      pNewData->bPersistent = bDefaultPersistent;
      break;
    case CFX_Value::DataType::STRING:
      pNewData->nType = CFX_Value::DataType::STRING;
      pNewData->sData = sData;
      pNewData->bPersistent = bDefaultPersistent;
      break;
    case CFX_Value::DataType::OBJECT:
      pNewData->nType = CFX_Value::DataType::OBJECT;
      pNewData->pData.Reset(pData->GetIsolate(), pData);
      pNewData->bPersistent = bDefaultPersistent;
      break;
    case CFX_Value::DataType::NULLOBJ:
      pNewData->nType = CFX_Value::DataType::NULLOBJ;
      pNewData->bPersistent = bDefaultPersistent;
      break;
    default:
      return CJS_Result::Failure(JSMessage::kObjectTypeError);
  }
  m_MapGlobal[propname] = std::move(pNewData);
  return CJS_Result::Success();
}
