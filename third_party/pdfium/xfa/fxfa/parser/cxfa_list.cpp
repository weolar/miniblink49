// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_list.h"

#include <utility>

#include "core/fxcrt/fx_extension.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cjx_treelist.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_node.h"

CXFA_List::CXFA_List(CXFA_Document* pDocument, std::unique_ptr<CJX_Object> obj)
    : CXFA_List(pDocument,
                XFA_ObjectType::List,
                XFA_Element::List,
                std::move(obj)) {}

CXFA_List::CXFA_List(CXFA_Document* pDocument,
                     XFA_ObjectType objectType,
                     XFA_Element eType,
                     std::unique_ptr<CJX_Object> obj)
    : CXFA_Object(pDocument, objectType, eType, std::move(obj)) {
  m_pDocument->GetScriptContext()->AddToCacheList(
      std::unique_ptr<CXFA_List>(this));
}

CXFA_List::~CXFA_List() {}
