// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_treelist.h"

#include <vector>

#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_treelist.h"

const CJX_MethodSpec CJX_TreeList::MethodSpecs[] = {
    {"namedItem", namedItem_static}};

CJX_TreeList::CJX_TreeList(CXFA_TreeList* list) : CJX_List(list) {
  DefineMethods(MethodSpecs);
}

CJX_TreeList::~CJX_TreeList() {}

bool CJX_TreeList::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CXFA_TreeList* CJX_TreeList::GetXFATreeList() {
  return ToTreeList(GetXFAObject());
}

CJS_Result CJX_TreeList::namedItem(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* pNode = GetXFATreeList()->NamedItem(
      runtime->ToWideString(params[0]).AsStringView());
  if (!pNode)
    return CJS_Result::Success();

  CFXJSE_Value* value =
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pNode);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      value->DirectGetValue().Get(runtime->GetIsolate()));
}
