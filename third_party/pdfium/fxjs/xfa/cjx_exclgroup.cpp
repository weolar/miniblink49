// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_exclgroup.h"

#include <vector>

#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/fxfa.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_exclgroup.h"

const CJX_MethodSpec CJX_ExclGroup::MethodSpecs[] = {
    {"execCalculate", execCalculate_static},
    {"execEvent", execEvent_static},
    {"execInitialize", execInitialize_static},
    {"execValidate", execValidate_static},
    {"selectedMember", selectedMember_static}};

CJX_ExclGroup::CJX_ExclGroup(CXFA_ExclGroup* group) : CJX_Node(group) {
  DefineMethods(MethodSpecs);
}

CJX_ExclGroup::~CJX_ExclGroup() {}

bool CJX_ExclGroup::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_ExclGroup::execEvent(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  execSingleEventByName(runtime->ToWideString(params[0]).AsStringView(),
                        XFA_Element::ExclGroup);
  return CJS_Result::Success();
}

CJS_Result CJX_ExclGroup::execInitialize(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (pNotify)
    pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Initialize, false,
                                  true);
  return CJS_Result::Success();
}

CJS_Result CJX_ExclGroup::execCalculate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (pNotify)
    pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Calculate, false,
                                  true);
  return CJS_Result::Success();
}

CJS_Result CJX_ExclGroup::execValidate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_FFNotify* notify = GetDocument()->GetNotify();
  if (!notify)
    return CJS_Result::Success(runtime->NewBoolean(false));

  int32_t iRet = notify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Validate,
                                              false, true);
  return CJS_Result::Success(runtime->NewBoolean(iRet != XFA_EVENTERROR_Error));
}

CJS_Result CJX_ExclGroup::selectedMember(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return CJS_Result::Success(runtime->NewNull());

  CXFA_Node* pReturnNode = nullptr;
  if (params.empty()) {
    pReturnNode = node->GetSelectedMember();
  } else {
    pReturnNode = node->SetSelectedMember(
        runtime->ToWideString(params[0]).AsStringView(), true);
  }
  if (!pReturnNode)
    return CJS_Result::Success(runtime->NewNull());

  CFXJSE_Value* value =
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pReturnNode);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      value->DirectGetValue().Get(runtime->GetIsolate()));
}

void CJX_ExclGroup::defaultValue(CFXJSE_Value* pValue,
                                 bool bSetting,
                                 XFA_Attribute eAttribute) {
  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return;

  if (bSetting) {
    node->SetSelectedMemberByValue(pValue->ToWideString().AsStringView(), true,
                                   true, true);
    return;
  }

  WideString wsValue = GetContent(true);
  XFA_VERSION curVersion = GetDocument()->GetCurVersionMode();
  if (wsValue.IsEmpty() && curVersion >= XFA_VERSION_300) {
    pValue->SetNull();
    return;
  }
  pValue->SetString(wsValue.ToUTF8().AsStringView());
}

void CJX_ExclGroup::rawValue(CFXJSE_Value* pValue,
                             bool bSetting,
                             XFA_Attribute eAttribute) {
  defaultValue(pValue, bSetting, eAttribute);
}

void CJX_ExclGroup::transient(CFXJSE_Value* pValue,
                              bool bSetting,
                              XFA_Attribute eAttribute) {}

void CJX_ExclGroup::borderColor(CFXJSE_Value* pValue,
                                bool bSetting,
                                XFA_Attribute eAttribute) {
  ScriptSomBorderColor(pValue, bSetting, eAttribute);
}

void CJX_ExclGroup::borderWidth(CFXJSE_Value* pValue,
                                bool bSetting,
                                XFA_Attribute eAttribute) {
  ScriptSomBorderWidth(pValue, bSetting, eAttribute);
}

void CJX_ExclGroup::fillColor(CFXJSE_Value* pValue,
                              bool bSetting,
                              XFA_Attribute eAttribute) {
  ScriptSomFillColor(pValue, bSetting, eAttribute);
}

void CJX_ExclGroup::mandatory(CFXJSE_Value* pValue,
                              bool bSetting,
                              XFA_Attribute eAttribute) {
  ScriptSomMandatory(pValue, bSetting, eAttribute);
}

void CJX_ExclGroup::mandatoryMessage(CFXJSE_Value* pValue,
                                     bool bSetting,
                                     XFA_Attribute eAttribute) {
  ScriptSomMandatoryMessage(pValue, bSetting, eAttribute);
}

void CJX_ExclGroup::validationMessage(CFXJSE_Value* pValue,
                                      bool bSetting,
                                      XFA_Attribute eAttribute) {
  ScriptSomValidationMessage(pValue, bSetting, eAttribute);
}
