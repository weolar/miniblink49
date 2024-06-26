// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_form.h"

#include <vector>

#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_arraynodelist.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_form.h"

const CJX_MethodSpec CJX_Form::MethodSpecs[] = {
    {"execCalculate", execCalculate_static},
    {"execInitialize", execInitialize_static},
    {"execValidate", execValidate_static},
    {"formNodes", formNodes_static},
    {"recalculate", recalculate_static},
    {"remerge", remerge_static}};

CJX_Form::CJX_Form(CXFA_Form* form) : CJX_Model(form) {
  DefineMethods(MethodSpecs);
}

CJX_Form::~CJX_Form() {}

bool CJX_Form::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_Form::formNodes(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* pDataNode =
      ToNode(static_cast<CFXJSE_Engine*>(runtime)->ToXFAObject(params[0]));
  if (!pDataNode)
    return CJS_Result::Failure(JSMessage::kValueError);

  CXFA_ArrayNodeList* pFormNodes = new CXFA_ArrayNodeList(GetDocument());
  CFXJSE_Value* value =
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pFormNodes);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());
  return CJS_Result::Success(
      value->DirectGetValue().Get(runtime->GetIsolate()));
}

CJS_Result CJX_Form::remerge(CFX_V8* runtime,
                             const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  GetDocument()->DoDataRemerge(true);
  return CJS_Result::Success();
}

CJS_Result CJX_Form::execInitialize(
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

CJS_Result CJX_Form::recalculate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CXFA_EventParam* pEventParam =
      GetDocument()->GetScriptContext()->GetEventParam();
  if (pEventParam->m_eType == XFA_EVENT_Calculate ||
      pEventParam->m_eType == XFA_EVENT_InitCalculate) {
    return CJS_Result::Success();
  }
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (!pNotify || runtime->ToInt32(params[0]) != 0)
    return CJS_Result::Success();

  pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Calculate, false, true);
  pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Validate, false, true);
  pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Ready, true, true);
  return CJS_Result::Success();
}

CJS_Result CJX_Form::execCalculate(
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

CJS_Result CJX_Form::execValidate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 0)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (!pNotify)
    return CJS_Result::Success(runtime->NewBoolean(false));

  int32_t iRet = pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Validate,
                                               false, true);
  return CJS_Result::Success(runtime->NewBoolean(iRet != XFA_EVENTERROR_Error));
}
