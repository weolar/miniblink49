// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_eventpseudomodel.h"

#include <algorithm>
#include <vector>

#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
#include "xfa/fxfa/parser/cscript_eventpseudomodel.h"

namespace {

void StringProperty(CFXJSE_Value* pReturn, WideString* wsValue, bool bSetting) {
  if (bSetting) {
    *wsValue = pReturn->ToWideString();
    return;
  }

  pReturn->SetString(wsValue->ToUTF8().AsStringView());
}

void InterProperty(CFXJSE_Value* pReturn, int32_t* iValue, bool bSetting) {
  if (bSetting) {
    *iValue = pReturn->ToInteger();
    return;
  }
  pReturn->SetInteger(*iValue);
}

void BooleanProperty(CFXJSE_Value* pReturn, bool* bValue, bool bSetting) {
  if (bSetting) {
    *bValue = pReturn->ToBoolean();
    return;
  }
  pReturn->SetBoolean(*bValue);
}

}  // namespace

const CJX_MethodSpec CJX_EventPseudoModel::MethodSpecs[] = {
    {"emit", emit_static},
    {"reset", reset_static}};

CJX_EventPseudoModel::CJX_EventPseudoModel(CScript_EventPseudoModel* model)
    : CJX_Object(model) {
  DefineMethods(MethodSpecs);
}

CJX_EventPseudoModel::~CJX_EventPseudoModel() {}

bool CJX_EventPseudoModel::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_EventPseudoModel::cancelAction(CFXJSE_Value* pValue,
                                        bool bSetting,
                                        XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::CancelAction, bSetting);
}

void CJX_EventPseudoModel::change(CFXJSE_Value* pValue,
                                  bool bSetting,
                                  XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::Change, bSetting);
}

void CJX_EventPseudoModel::commitKey(CFXJSE_Value* pValue,
                                     bool bSetting,
                                     XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::CommitKey, bSetting);
}

void CJX_EventPseudoModel::fullText(CFXJSE_Value* pValue,
                                    bool bSetting,
                                    XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::FullText, bSetting);
}

void CJX_EventPseudoModel::keyDown(CFXJSE_Value* pValue,
                                   bool bSetting,
                                   XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::Keydown, bSetting);
}

void CJX_EventPseudoModel::modifier(CFXJSE_Value* pValue,
                                    bool bSetting,
                                    XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::Modifier, bSetting);
}

void CJX_EventPseudoModel::newContentType(CFXJSE_Value* pValue,
                                          bool bSetting,
                                          XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::NewContentType, bSetting);
}

void CJX_EventPseudoModel::newText(CFXJSE_Value* pValue,
                                   bool bSetting,
                                   XFA_Attribute eAttribute) {
  if (bSetting)
    return;

  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext)
    return;

  CXFA_EventParam* pEventParam = pScriptContext->GetEventParam();
  if (!pEventParam)
    return;

  pValue->SetString(pEventParam->GetNewText().ToUTF8().AsStringView());
}

void CJX_EventPseudoModel::prevContentType(CFXJSE_Value* pValue,
                                           bool bSetting,
                                           XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::PreviousContentType, bSetting);
}

void CJX_EventPseudoModel::prevText(CFXJSE_Value* pValue,
                                    bool bSetting,
                                    XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::PreviousText, bSetting);
}

void CJX_EventPseudoModel::reenter(CFXJSE_Value* pValue,
                                   bool bSetting,
                                   XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::Reenter, bSetting);
}

void CJX_EventPseudoModel::selEnd(CFXJSE_Value* pValue,
                                  bool bSetting,
                                  XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::SelectionEnd, bSetting);
}

void CJX_EventPseudoModel::selStart(CFXJSE_Value* pValue,
                                    bool bSetting,
                                    XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::SelectionStart, bSetting);
}

void CJX_EventPseudoModel::shift(CFXJSE_Value* pValue,
                                 bool bSetting,
                                 XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::Shift, bSetting);
}

void CJX_EventPseudoModel::soapFaultCode(CFXJSE_Value* pValue,
                                         bool bSetting,
                                         XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::SoapFaultCode, bSetting);
}

void CJX_EventPseudoModel::soapFaultString(CFXJSE_Value* pValue,
                                           bool bSetting,
                                           XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::SoapFaultString, bSetting);
}

void CJX_EventPseudoModel::target(CFXJSE_Value* pValue,
                                  bool bSetting,
                                  XFA_Attribute eAttribute) {
  Property(pValue, XFA_Event::Target, bSetting);
}

CJS_Result CJX_EventPseudoModel::emit(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext)
    return CJS_Result::Success();

  CXFA_EventParam* pEventParam = pScriptContext->GetEventParam();
  if (!pEventParam)
    return CJS_Result::Success();

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (!pNotify)
    return CJS_Result::Success();

  CXFA_FFWidgetHandler* pWidgetHandler = pNotify->GetWidgetHandler();
  if (!pWidgetHandler)
    return CJS_Result::Success();

  pWidgetHandler->ProcessEvent(pEventParam->m_pTarget.Get(), pEventParam);
  return CJS_Result::Success();
}

CJS_Result CJX_EventPseudoModel::reset(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext)
    return CJS_Result::Success();

  CXFA_EventParam* pEventParam = pScriptContext->GetEventParam();
  if (pEventParam)
    *pEventParam = CXFA_EventParam();

  return CJS_Result::Success();
}

void CJX_EventPseudoModel::Property(CFXJSE_Value* pValue,
                                    XFA_Event dwFlag,
                                    bool bSetting) {
  // Only the cancelAction, selStart, selEnd and change properties are writable.
  if (bSetting && dwFlag != XFA_Event::CancelAction &&
      dwFlag != XFA_Event::SelectionStart &&
      dwFlag != XFA_Event::SelectionEnd && dwFlag != XFA_Event::Change) {
    return;
  }

  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext)
    return;

  CXFA_EventParam* pEventParam = pScriptContext->GetEventParam();
  if (!pEventParam)
    return;

  switch (dwFlag) {
    case XFA_Event::CancelAction:
      BooleanProperty(pValue, &pEventParam->m_bCancelAction, bSetting);
      break;
    case XFA_Event::Change:
      StringProperty(pValue, &pEventParam->m_wsChange, bSetting);
      break;
    case XFA_Event::CommitKey:
      InterProperty(pValue, &pEventParam->m_iCommitKey, bSetting);
      break;
    case XFA_Event::FullText:
      StringProperty(pValue, &pEventParam->m_wsFullText, bSetting);
      break;
    case XFA_Event::Keydown:
      BooleanProperty(pValue, &pEventParam->m_bKeyDown, bSetting);
      break;
    case XFA_Event::Modifier:
      BooleanProperty(pValue, &pEventParam->m_bModifier, bSetting);
      break;
    case XFA_Event::NewContentType:
      StringProperty(pValue, &pEventParam->m_wsNewContentType, bSetting);
      break;
    case XFA_Event::NewText:
      NOTREACHED();
      break;
    case XFA_Event::PreviousContentType:
      StringProperty(pValue, &pEventParam->m_wsPrevContentType, bSetting);
      break;
    case XFA_Event::PreviousText:
      StringProperty(pValue, &pEventParam->m_wsPrevText, bSetting);
      break;
    case XFA_Event::Reenter:
      BooleanProperty(pValue, &pEventParam->m_bReenter, bSetting);
      break;
    case XFA_Event::SelectionEnd:
      InterProperty(pValue, &pEventParam->m_iSelEnd, bSetting);

      pEventParam->m_iSelEnd = std::max(0, pEventParam->m_iSelEnd);
      pEventParam->m_iSelEnd =
          std::min(static_cast<size_t>(pEventParam->m_iSelEnd),
                   pEventParam->m_wsPrevText.GetLength());
      pEventParam->m_iSelStart =
          std::min(pEventParam->m_iSelStart, pEventParam->m_iSelEnd);

      break;
    case XFA_Event::SelectionStart:
      InterProperty(pValue, &pEventParam->m_iSelStart, bSetting);

      pEventParam->m_iSelStart = std::max(0, pEventParam->m_iSelStart);
      pEventParam->m_iSelStart =
          std::min(static_cast<size_t>(pEventParam->m_iSelStart),
                   pEventParam->m_wsPrevText.GetLength());
      pEventParam->m_iSelEnd =
          std::max(pEventParam->m_iSelStart, pEventParam->m_iSelEnd);

      break;
    case XFA_Event::Shift:
      BooleanProperty(pValue, &pEventParam->m_bShift, bSetting);
      break;
    case XFA_Event::SoapFaultCode:
      StringProperty(pValue, &pEventParam->m_wsSoapFaultCode, bSetting);
      break;
    case XFA_Event::SoapFaultString:
      StringProperty(pValue, &pEventParam->m_wsSoapFaultString, bSetting);
      break;
    case XFA_Event::Target:
    default:
      break;
  }
}
