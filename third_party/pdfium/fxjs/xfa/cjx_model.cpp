// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_model.h"

#include <vector>

#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/parser/cxfa_delta.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/xfa_basic_data.h"

const CJX_MethodSpec CJX_Model::MethodSpecs[] = {
    {"clearErrorList", clearErrorList_static},
    {"createNode", createNode_static},
    {"isCompatibleNS", isCompatibleNS_static}};

CJX_Model::CJX_Model(CXFA_Node* node) : CJX_Node(node) {
  DefineMethods(MethodSpecs);
}

CJX_Model::~CJX_Model() {}

bool CJX_Model::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_Model::clearErrorList(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJX_Model::createNode(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.empty() || params.size() > 3)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString name;
  if (params.size() > 1)
    name = runtime->ToWideString(params[1]);

  WideString nameSpace;
  if (params.size() == 3)
    nameSpace = runtime->ToWideString(params[2]);

  WideString tagName = runtime->ToWideString(params[0]);
  XFA_Element eType = XFA_GetElementByName(tagName.AsStringView());
  CXFA_Node* pNewNode = GetXFANode()->CreateSamePacketNode(eType);
  if (!pNewNode)
    return CJS_Result::Success(runtime->NewNull());

  if (!name.IsEmpty()) {
    if (!pNewNode->HasAttribute(XFA_Attribute::Name))
      return CJS_Result::Failure(JSMessage::kParamError);

    pNewNode->JSObject()->SetAttribute(XFA_Attribute::Name, name.AsStringView(),
                                       true);
    if (pNewNode->GetPacketType() == XFA_PacketType::Datasets)
      pNewNode->CreateXMLMappingNode();
  }

  CFXJSE_Value* value =
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pNewNode);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      value->DirectGetValue().Get(runtime->GetIsolate()));
}

CJS_Result CJX_Model::isCompatibleNS(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString nameSpace;
  if (params.size() >= 1)
    nameSpace = runtime->ToWideString(params[0]);

  return CJS_Result::Success(
      runtime->NewBoolean(TryNamespace().value_or(WideString()) == nameSpace));
}

void CJX_Model::context(CFXJSE_Value* pValue,
                        bool bSetting,
                        XFA_Attribute eAttribute) {}

void CJX_Model::aliasNode(CFXJSE_Value* pValue,
                          bool bSetting,
                          XFA_Attribute eAttribute) {}
