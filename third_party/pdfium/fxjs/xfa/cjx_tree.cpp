// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_tree.h"

#include <vector>

#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_arraynodelist.h"
#include "xfa/fxfa/parser/cxfa_attachnodelist.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_object.h"
#include "xfa/fxfa/parser/xfa_resolvenode_rs.h"

const CJX_MethodSpec CJX_Tree::MethodSpecs[] = {
    {"resolveNode", resolveNode_static},
    {"resolveNodes", resolveNodes_static}};

CJX_Tree::CJX_Tree(CXFA_Object* obj) : CJX_Object(obj) {
  DefineMethods(MethodSpecs);
}

CJX_Tree::~CJX_Tree() {}

bool CJX_Tree::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_Tree::resolveNode(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString expression = runtime->ToWideString(params[0]);
  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext)
    return CJS_Result::Success();

  CXFA_Object* refNode = GetXFAObject();
  if (refNode->GetElementType() == XFA_Element::Xfa)
    refNode = pScriptContext->GetThisObject();

  uint32_t dwFlag = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
                    XFA_RESOLVENODE_Properties | XFA_RESOLVENODE_Parent |
                    XFA_RESOLVENODE_Siblings;
  XFA_RESOLVENODE_RS resolveNodeRS;
  if (!pScriptContext->ResolveObjects(ToNode(refNode),
                                      expression.AsStringView(), &resolveNodeRS,
                                      dwFlag, nullptr)) {
    return CJS_Result::Success(runtime->NewNull());
  }

  if (resolveNodeRS.dwFlags == XFA_ResolveNode_RSType_Nodes) {
    CXFA_Object* pObject = resolveNodeRS.objects.front().Get();
    CFXJSE_Value* value =
        GetDocument()->GetScriptContext()->GetJSValueFromMap(pObject);
    if (!value)
      return CJS_Result::Success(runtime->NewNull());

    return CJS_Result::Success(
        value->DirectGetValue().Get(runtime->GetIsolate()));
  }

  if (!resolveNodeRS.script_attribute.callback ||
      resolveNodeRS.script_attribute.eValueType != XFA_ScriptType::Object) {
    return CJS_Result::Success(runtime->NewNull());
  }

  auto pValue = pdfium::MakeUnique<CFXJSE_Value>(pScriptContext->GetIsolate());
  CJX_Object* jsObject = resolveNodeRS.objects.front()->JSObject();
  (*resolveNodeRS.script_attribute.callback)(
      jsObject, pValue.get(), false, resolveNodeRS.script_attribute.attribute);
  return CJS_Result::Success(
      pValue->DirectGetValue().Get(runtime->GetIsolate()));
}

CJS_Result CJX_Tree::resolveNodes(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Object* refNode = GetXFAObject();
  if (refNode->GetElementType() == XFA_Element::Xfa)
    refNode = GetDocument()->GetScriptContext()->GetThisObject();

  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext)
    return CJS_Result::Success();

  auto pValue = pdfium::MakeUnique<CFXJSE_Value>(pScriptContext->GetIsolate());
  ResolveNodeList(pValue.get(), runtime->ToWideString(params[0]),
                  XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
                      XFA_RESOLVENODE_Properties | XFA_RESOLVENODE_Parent |
                      XFA_RESOLVENODE_Siblings,
                  ToNode(refNode));
  return CJS_Result::Success(
      pValue->DirectGetValue().Get(runtime->GetIsolate()));
}

void CJX_Tree::all(CFXJSE_Value* pValue,
                   bool bSetting,
                   XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  uint32_t dwFlag = XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_ALL;
  WideString wsExpression = GetAttribute(XFA_Attribute::Name) + L"[*]";
  ResolveNodeList(pValue, wsExpression, dwFlag, nullptr);
}

void CJX_Tree::classAll(CFXJSE_Value* pValue,
                        bool bSetting,
                        XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  WideString wsExpression =
      L"#" + WideString::FromASCII(GetXFAObject()->GetClassName()) + L"[*]";
  ResolveNodeList(pValue, wsExpression,
                  XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_ALL, nullptr);
}

void CJX_Tree::nodes(CFXJSE_Value* pValue,
                     bool bSetting,
                     XFA_Attribute eAttribute) {
  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext)
    return;

  if (bSetting) {
    WideString wsMessage = L"Unable to set ";
    FXJSE_ThrowMessage(wsMessage.ToUTF8().AsStringView());
    return;
  }

  CXFA_AttachNodeList* pNodeList =
      new CXFA_AttachNodeList(GetDocument(), ToNode(GetXFAObject()));
  pValue->SetObject(pNodeList, pScriptContext->GetJseNormalClass());
}

void CJX_Tree::parent(CFXJSE_Value* pValue,
                      bool bSetting,
                      XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  CXFA_Node* pParent = ToNode(GetXFAObject())->GetParent();
  if (!pParent) {
    pValue->SetNull();
    return;
  }

  pValue->Assign(GetDocument()->GetScriptContext()->GetJSValueFromMap(pParent));
}

void CJX_Tree::index(CFXJSE_Value* pValue,
                     bool bSetting,
                     XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext) {
    pValue->SetInteger(-1);
    return;
  }
  pValue->SetInteger(pScriptContext->GetIndexByName(ToNode(GetXFAObject())));
}

void CJX_Tree::classIndex(CFXJSE_Value* pValue,
                          bool bSetting,
                          XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext) {
    pValue->SetInteger(-1);
    return;
  }
  pValue->SetInteger(
      pScriptContext->GetIndexByClassName(ToNode(GetXFAObject())));
}

void CJX_Tree::somExpression(CFXJSE_Value* pValue,
                             bool bSetting,
                             XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  WideString wsSOMExpression = GetXFAObject()->GetSOMExpression();
  pValue->SetString(wsSOMExpression.ToUTF8().AsStringView());
}

void CJX_Tree::ResolveNodeList(CFXJSE_Value* pValue,
                               WideString wsExpression,
                               uint32_t dwFlag,
                               CXFA_Node* refNode) {
  CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
  if (!pScriptContext)
    return;
  if (!refNode)
    refNode = ToNode(GetXFAObject());

  XFA_RESOLVENODE_RS resolveNodeRS;
  pScriptContext->ResolveObjects(refNode, wsExpression.AsStringView(),
                                 &resolveNodeRS, dwFlag, nullptr);
  CXFA_ArrayNodeList* pNodeList = new CXFA_ArrayNodeList(GetDocument());
  if (resolveNodeRS.dwFlags == XFA_ResolveNode_RSType_Nodes) {
    for (auto& pObject : resolveNodeRS.objects) {
      if (pObject->IsNode())
        pNodeList->Append(pObject->AsNode());
    }
  } else {
    if (resolveNodeRS.script_attribute.callback &&
        resolveNodeRS.script_attribute.eValueType == XFA_ScriptType::Object) {
      for (auto& pObject : resolveNodeRS.objects) {
        auto innerValue =
            pdfium::MakeUnique<CFXJSE_Value>(pScriptContext->GetIsolate());
        CJX_Object* jsObject = pObject->JSObject();
        (*resolveNodeRS.script_attribute.callback)(
            jsObject, innerValue.get(), false,
            resolveNodeRS.script_attribute.attribute);
        CXFA_Object* obj = CFXJSE_Engine::ToObject(innerValue.get());
        if (obj->IsNode())
          pNodeList->Append(obj->AsNode());
      }
    }
  }
  pValue->SetObject(pNodeList, pScriptContext->GetJseNormalClass());
}
