// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_list.h"

#include <vector>

#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_class.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "third_party/base/numerics/safe_conversions.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_list.h"
#include "xfa/fxfa/parser/cxfa_node.h"

const CJX_MethodSpec CJX_List::MethodSpecs[] = {{"append", append_static},
                                                {"insert", insert_static},
                                                {"item", item_static},
                                                {"remove", remove_static}};

CJX_List::CJX_List(CXFA_List* list) : CJX_Object(list) {
  DefineMethods(MethodSpecs);
}

CJX_List::~CJX_List() {}

bool CJX_List::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CXFA_List* CJX_List::GetXFAList() {
  return ToList(GetXFAObject());
}

CJS_Result CJX_List::append(CFX_V8* runtime,
                            const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  auto* pNode =
      ToNode(static_cast<CFXJSE_Engine*>(runtime)->ToXFAObject(params[0]));
  if (!pNode)
    return CJS_Result::Failure(JSMessage::kValueError);

  GetXFAList()->Append(pNode);
  return CJS_Result::Success();
}

CJS_Result CJX_List::insert(CFX_V8* runtime,
                            const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  auto* pNewNode =
      ToNode(static_cast<CFXJSE_Engine*>(runtime)->ToXFAObject(params[0]));
  if (!pNewNode)
    return CJS_Result::Failure(JSMessage::kValueError);

  auto* pBeforeNode =
      ToNode(static_cast<CFXJSE_Engine*>(runtime)->ToXFAObject(params[1]));
  GetXFAList()->Insert(pNewNode, pBeforeNode);
  return CJS_Result::Success();
}

CJS_Result CJX_List::remove(CFX_V8* runtime,
                            const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  auto* pNode =
      ToNode(static_cast<CFXJSE_Engine*>(runtime)->ToXFAObject(params[0]));
  if (!pNode)
    return CJS_Result::Failure(JSMessage::kValueError);

  GetXFAList()->Remove(pNode);
  return CJS_Result::Success();
}

CJS_Result CJX_List::item(CFX_V8* runtime,
                          const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  int32_t index = runtime->ToInt32(params[0]);
  size_t cast_index = static_cast<size_t>(index);
  if (index < 0 || cast_index >= GetXFAList()->GetLength())
    return CJS_Result::Failure(JSMessage::kInvalidInputError);

  return CJS_Result::Success(static_cast<CFXJSE_Engine*>(runtime)->NewXFAObject(
      GetXFAList()->Item(cast_index),
      GetDocument()->GetScriptContext()->GetJseNormalClass()->GetTemplate()));
}

void CJX_List::length(CFXJSE_Value* pValue,
                      bool bSetting,
                      XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  pValue->SetInteger(
      pdfium::base::checked_cast<int32_t>(GetXFAList()->GetLength()));
}
