// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_treelist.h"

#include <memory>

#include "core/fxcrt/fx_extension.h"
#include "fxjs/xfa/cjx_treelist.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_node.h"

CXFA_TreeList::CXFA_TreeList(CXFA_Document* pDocument)
    : CXFA_List(pDocument,
                XFA_ObjectType::TreeList,
                XFA_Element::TreeList,
                pdfium::MakeUnique<CJX_TreeList>(this)) {}

CXFA_TreeList::~CXFA_TreeList() = default;

CXFA_Node* CXFA_TreeList::NamedItem(WideStringView wsName) {
  uint32_t dwHashCode = FX_HashCode_GetW(wsName, false);
  size_t count = GetLength();
  for (size_t i = 0; i < count; i++) {
    CXFA_Node* ret = Item(i);
    if (dwHashCode == ret->GetNameHash())
      return ret;
  }
  return nullptr;
}
