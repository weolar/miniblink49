// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssstylerule.h"

CFX_CSSStyleRule::CFX_CSSStyleRule() {}

CFX_CSSStyleRule::~CFX_CSSStyleRule() {}

size_t CFX_CSSStyleRule::CountSelectorLists() const {
  return m_ppSelector.size();
}

CFX_CSSSelector* CFX_CSSStyleRule::GetSelectorList(int32_t index) const {
  return m_ppSelector[index].get();
}

CFX_CSSDeclaration* CFX_CSSStyleRule::GetDeclaration() {
  return &m_Declaration;
}

void CFX_CSSStyleRule::SetSelector(
    std::vector<std::unique_ptr<CFX_CSSSelector>>* list) {
  ASSERT(m_ppSelector.empty());

  m_ppSelector.swap(*list);
}
