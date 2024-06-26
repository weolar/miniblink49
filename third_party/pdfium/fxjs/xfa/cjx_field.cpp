// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_field.h"

#include <vector>

#include "fxjs/cfx_v8.h"
#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fgas/crt/cfgas_decimal.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/fxfa.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_field.h"
#include "xfa/fxfa/parser/cxfa_value.h"

const CJX_MethodSpec CJX_Field::MethodSpecs[] = {
    {"addItem", addItem_static},
    {"boundItem", boundItem_static},
    {"clearItems", clearItems_static},
    {"deleteItem", deleteItem_static},
    {"execCalculate", execCalculate_static},
    {"execEvent", execEvent_static},
    {"execInitialize", execInitialize_static},
    {"execValidate", execValidate_static},
    {"getDisplayItem", getDisplayItem_static},
    {"getItemState", getItemState_static},
    {"getSaveItem", getSaveItem_static},
    {"setItemState", setItemState_static}};

CJX_Field::CJX_Field(CXFA_Field* field) : CJX_Container(field) {
  DefineMethods(MethodSpecs);
}

CJX_Field::~CJX_Field() {}

bool CJX_Field::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_Field::clearItems(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CXFA_Node* node = GetXFANode();
  if (node->IsWidgetReady())
    node->DeleteItem(-1, true, false);
  return CJS_Result::Success();
}

CJS_Result CJX_Field::execEvent(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString eventString = runtime->ToWideString(params[0]);
  int32_t iRet =
      execSingleEventByName(eventString.AsStringView(), XFA_Element::Field);
  if (!eventString.EqualsASCII("validate"))
    return CJS_Result::Success();

  return CJS_Result::Success(runtime->NewBoolean(iRet != XFA_EVENTERROR_Error));
}

CJS_Result CJX_Field::execInitialize(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (pNotify) {
    pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Initialize, false,
                                  false);
  }
  return CJS_Result::Success();
}

CJS_Result CJX_Field::deleteItem(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return CJS_Result::Success();

  bool bValue = node->DeleteItem(runtime->ToInt32(params[0]), true, true);
  return CJS_Result::Success(runtime->NewBoolean(bValue));
}

CJS_Result CJX_Field::getSaveItem(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  int32_t iIndex = runtime->ToInt32(params[0]);
  if (iIndex < 0)
    return CJS_Result::Success(runtime->NewNull());

  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return CJS_Result::Success(runtime->NewNull());

  Optional<WideString> value = node->GetChoiceListItem(iIndex, true);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      runtime->NewString(value->ToUTF8().AsStringView()));
}

CJS_Result CJX_Field::boundItem(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return CJS_Result::Success();

  WideString value = runtime->ToWideString(params[0]);
  WideString boundValue = node->GetItemValue(value.AsStringView());
  return CJS_Result::Success(
      runtime->NewString(boundValue.ToUTF8().AsStringView()));
}

CJS_Result CJX_Field::getItemState(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return CJS_Result::Success();

  int32_t state = node->GetItemState(runtime->ToInt32(params[0]));
  return CJS_Result::Success(runtime->NewBoolean(state != 0));
}

CJS_Result CJX_Field::execCalculate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (pNotify) {
    pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Calculate, false,
                                  false);
  }
  return CJS_Result::Success();
}

CJS_Result CJX_Field::getDisplayItem(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  int32_t iIndex = runtime->ToInt32(params[0]);
  if (iIndex < 0)
    return CJS_Result::Success(runtime->NewNull());

  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return CJS_Result::Success(runtime->NewNull());

  Optional<WideString> value = node->GetChoiceListItem(iIndex, false);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      runtime->NewString(value->ToUTF8().AsStringView()));
}

CJS_Result CJX_Field::setItemState(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return CJS_Result::Success();

  int32_t iIndex = runtime->ToInt32(params[0]);
  if (runtime->ToInt32(params[1]) != 0) {
    node->SetItemState(iIndex, true, true, true, true);
    return CJS_Result::Success();
  }
  if (node->GetItemState(iIndex))
    node->SetItemState(iIndex, false, true, true, true);

  return CJS_Result::Success();
}

CJS_Result CJX_Field::addItem(CFX_V8* runtime,
                              const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1 && params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return CJS_Result::Success();

  WideString label;
  if (params.size() >= 1)
    label = runtime->ToWideString(params[0]);

  WideString value;
  if (params.size() >= 2)
    value = runtime->ToWideString(params[1]);

  node->InsertItem(label, value, true);
  return CJS_Result::Success();
}

CJS_Result CJX_Field::execValidate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (!pNotify)
    return CJS_Result::Success(runtime->NewBoolean(false));

  int32_t iRet = pNotify->ExecEventByDeepFirst(GetXFANode(), XFA_EVENT_Validate,
                                               false, false);
  return CJS_Result::Success(runtime->NewBoolean(iRet != XFA_EVENTERROR_Error));
}

void CJX_Field::defaultValue(CFXJSE_Value* pValue,
                             bool bSetting,
                             XFA_Attribute eAttribute) {
  CXFA_Node* xfaNode = GetXFANode();
  if (!xfaNode->IsWidgetReady())
    return;

  if (bSetting) {
    if (pValue) {
      xfaNode->SetPreNull(xfaNode->IsNull());
      xfaNode->SetIsNull(pValue->IsNull());
    }

    WideString wsNewText;
    if (pValue && !(pValue->IsNull() || pValue->IsUndefined()))
      wsNewText = pValue->ToWideString();
    if (xfaNode->GetUIChildNode()->GetElementType() == XFA_Element::NumericEdit)
      wsNewText = xfaNode->NumericLimit(wsNewText);

    CXFA_Node* pContainerNode = xfaNode->GetContainerNode();
    WideString wsFormatText(wsNewText);
    if (pContainerNode)
      wsFormatText = pContainerNode->GetFormatDataValue(wsNewText);

    SetContent(wsNewText, wsFormatText, true, true, true);
    return;
  }

  WideString content = GetContent(true);
  if (content.IsEmpty()) {
    pValue->SetNull();
    return;
  }

  CXFA_Node* formValue = xfaNode->GetFormValueIfExists();
  CXFA_Node* pNode = formValue ? formValue->GetFirstChild() : nullptr;
  if (pNode && pNode->GetElementType() == XFA_Element::Decimal) {
    if (xfaNode->GetUIChildNode()->GetElementType() ==
            XFA_Element::NumericEdit &&
        (pNode->JSObject()->GetInteger(XFA_Attribute::FracDigits) == -1)) {
      pValue->SetString(content.ToUTF8().AsStringView());
    } else {
      CFGAS_Decimal decimal(content.AsStringView());
      pValue->SetFloat((float)(double)decimal);
    }
  } else if (pNode && pNode->GetElementType() == XFA_Element::Integer) {
    pValue->SetInteger(FXSYS_wtoi(content.c_str()));
  } else if (pNode && pNode->GetElementType() == XFA_Element::Boolean) {
    pValue->SetBoolean(FXSYS_wtoi(content.c_str()) == 0 ? false : true);
  } else if (pNode && pNode->GetElementType() == XFA_Element::Float) {
    CFGAS_Decimal decimal(content.AsStringView());
    pValue->SetFloat((float)(double)decimal);
  } else {
    pValue->SetString(content.ToUTF8().AsStringView());
  }
}

void CJX_Field::editValue(CFXJSE_Value* pValue,
                          bool bSetting,
                          XFA_Attribute eAttribute) {
  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return;

  if (bSetting) {
    node->SetValue(XFA_VALUEPICTURE_Edit, pValue->ToWideString());
    return;
  }
  pValue->SetString(
      node->GetValue(XFA_VALUEPICTURE_Edit).ToUTF8().AsStringView());
}

void CJX_Field::formatMessage(CFXJSE_Value* pValue,
                              bool bSetting,
                              XFA_Attribute eAttribute) {
  ScriptSomMessage(pValue, bSetting, XFA_SOM_FormatMessage);
}

void CJX_Field::formattedValue(CFXJSE_Value* pValue,
                               bool bSetting,
                               XFA_Attribute eAttribute) {
  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return;

  if (bSetting) {
    node->SetValue(XFA_VALUEPICTURE_Display, pValue->ToWideString());
    return;
  }
  pValue->SetString(
      node->GetValue(XFA_VALUEPICTURE_Display).ToUTF8().AsStringView());
}

void CJX_Field::parentSubform(CFXJSE_Value* pValue,
                              bool bSetting,
                              XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  pValue->SetNull();
}

void CJX_Field::selectedIndex(CFXJSE_Value* pValue,
                              bool bSetting,
                              XFA_Attribute eAttribute) {
  CXFA_Node* node = GetXFANode();
  if (!node->IsWidgetReady())
    return;

  if (!bSetting) {
    pValue->SetInteger(node->GetSelectedItem(0));
    return;
  }

  int32_t iIndex = pValue->ToInteger();
  if (iIndex == -1) {
    node->ClearAllSelections();
    return;
  }

  node->SetItemState(iIndex, true, true, true, true);
}

void CJX_Field::borderColor(CFXJSE_Value* pValue,
                            bool bSetting,
                            XFA_Attribute eAttribute) {
  ScriptSomBorderColor(pValue, bSetting, eAttribute);
}

void CJX_Field::borderWidth(CFXJSE_Value* pValue,
                            bool bSetting,
                            XFA_Attribute eAttribute) {
  ScriptSomBorderWidth(pValue, bSetting, eAttribute);
}

void CJX_Field::fillColor(CFXJSE_Value* pValue,
                          bool bSetting,
                          XFA_Attribute eAttribute) {
  ScriptSomFillColor(pValue, bSetting, eAttribute);
}

void CJX_Field::fontColor(CFXJSE_Value* pValue,
                          bool bSetting,
                          XFA_Attribute eAttribute) {
  ScriptSomFontColor(pValue, bSetting, eAttribute);
}

void CJX_Field::mandatory(CFXJSE_Value* pValue,
                          bool bSetting,
                          XFA_Attribute eAttribute) {
  ScriptSomMandatory(pValue, bSetting, eAttribute);
}

void CJX_Field::mandatoryMessage(CFXJSE_Value* pValue,
                                 bool bSetting,
                                 XFA_Attribute eAttribute) {
  ScriptSomMandatoryMessage(pValue, bSetting, eAttribute);
}

void CJX_Field::rawValue(CFXJSE_Value* pValue,
                         bool bSetting,
                         XFA_Attribute eAttribute) {
  defaultValue(pValue, bSetting, eAttribute);
}

void CJX_Field::validationMessage(CFXJSE_Value* pValue,
                                  bool bSetting,
                                  XFA_Attribute eAttribute) {
  ScriptSomValidationMessage(pValue, bSetting, eAttribute);
}
