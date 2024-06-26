// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_docjsactions.h"

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfdoc/cpdf_nametree.h"

CPDF_DocJSActions::CPDF_DocJSActions(CPDF_Document* pDoc) : m_pDocument(pDoc) {
  ASSERT(m_pDocument);
}

CPDF_DocJSActions::~CPDF_DocJSActions() = default;

int CPDF_DocJSActions::CountJSActions() const {
  CPDF_NameTree name_tree(m_pDocument.Get(), "JavaScript");
  return name_tree.GetCount();
}

CPDF_Action CPDF_DocJSActions::GetJSActionAndName(int index,
                                                  WideString* csName) const {
  CPDF_NameTree name_tree(m_pDocument.Get(), "JavaScript");
  return CPDF_Action(ToDictionary(name_tree.LookupValueAndName(index, csName)));
}
