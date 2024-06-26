// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_packet.h"

#include <utility>
#include <vector>

#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmltext.h"
#include "fxjs/cfx_v8.h"
#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_packet.h"

const CJX_MethodSpec CJX_Packet::MethodSpecs[] = {
    {"getAttribute", getAttribute_static},
    {"removeAttribute", removeAttribute_static},
    {"setAttribute", setAttribute_static}};

CJX_Packet::CJX_Packet(CXFA_Packet* packet) : CJX_Node(packet) {
  DefineMethods(MethodSpecs);
}

CJX_Packet::~CJX_Packet() {}

bool CJX_Packet::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_Packet::getAttribute(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString attributeValue;
  CFX_XMLElement* element = ToXMLElement(GetXFANode()->GetXMLMappingNode());
  if (element)
    attributeValue = element->GetAttribute(runtime->ToWideString(params[0]));

  return CJS_Result::Success(
      runtime->NewString(attributeValue.ToUTF8().AsStringView()));
}

CJS_Result CJX_Packet::setAttribute(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  CFX_XMLElement* element = ToXMLElement(GetXFANode()->GetXMLMappingNode());
  if (element) {
    element->SetAttribute(runtime->ToWideString(params[1]),
                          runtime->ToWideString(params[0]));
  }
  return CJS_Result::Success(runtime->NewNull());
}

CJS_Result CJX_Packet::removeAttribute(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CFX_XMLElement* pElement = ToXMLElement(GetXFANode()->GetXMLMappingNode());
  if (pElement) {
    WideString name = runtime->ToWideString(params[0]);
    if (pElement->HasAttribute(name))
      pElement->RemoveAttribute(name);
  }
  return CJS_Result::Success(runtime->NewNull());
}

void CJX_Packet::content(CFXJSE_Value* pValue,
                         bool bSetting,
                         XFA_Attribute eAttribute) {
  CFX_XMLElement* element = ToXMLElement(GetXFANode()->GetXMLMappingNode());
  if (bSetting) {
    if (element) {
      element->AppendChild(
          GetXFANode()
              ->GetDocument()
              ->GetNotify()
              ->GetHDOC()
              ->GetXMLDocument()
              ->CreateNode<CFX_XMLText>(pValue->ToWideString()));
    }
    return;
  }

  WideString wsTextData;
  if (element)
    wsTextData = element->GetTextData();

  pValue->SetString(wsTextData.ToUTF8().AsStringView());
}
