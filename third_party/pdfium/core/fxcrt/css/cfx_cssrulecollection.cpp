// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssrulecollection.h"

#include <algorithm>
#include <utility>

#include "core/fxcrt/css/cfx_cssdeclaration.h"
#include "core/fxcrt/css/cfx_cssselector.h"
#include "core/fxcrt/css/cfx_cssstylerule.h"
#include "core/fxcrt/css/cfx_cssstylesheet.h"
#include "core/fxcrt/css/cfx_csssyntaxparser.h"
#include "third_party/base/ptr_util.h"

CFX_CSSRuleCollection::CFX_CSSRuleCollection() : m_iSelectors(0) {}

CFX_CSSRuleCollection::~CFX_CSSRuleCollection() {
  Clear();
}

void CFX_CSSRuleCollection::Clear() {
  m_TagRules.clear();
  m_iSelectors = 0;
}

const std::vector<std::unique_ptr<CFX_CSSRuleCollection::Data>>*
CFX_CSSRuleCollection::GetTagRuleData(const WideString& tagname) const {
  auto it = m_TagRules.find(FX_HashCode_GetW(tagname.AsStringView(), true));
  return it != m_TagRules.end() ? &it->second : nullptr;
}

void CFX_CSSRuleCollection::AddRulesFrom(const CFX_CSSStyleSheet* sheet) {
  int32_t iRules = sheet->CountRules();
  for (int32_t j = 0; j < iRules; j++)
    AddRulesFrom(sheet, sheet->GetRule(j));
}

void CFX_CSSRuleCollection::AddRulesFrom(const CFX_CSSStyleSheet* pStyleSheet,
                                         CFX_CSSStyleRule* pStyleRule) {
  CFX_CSSDeclaration* pDeclaration = pStyleRule->GetDeclaration();
  int32_t iSelectors = pStyleRule->CountSelectorLists();
  for (int32_t i = 0; i < iSelectors; ++i) {
    CFX_CSSSelector* pSelector = pStyleRule->GetSelectorList(i);
    m_TagRules[pSelector->GetNameHash()].push_back(
        pdfium::MakeUnique<Data>(pSelector, pDeclaration));
    m_iSelectors++;
  }
}

CFX_CSSRuleCollection::Data::Data(CFX_CSSSelector* pSel,
                                  CFX_CSSDeclaration* pDecl)
    : pSelector(pSel), pDeclaration(pDecl) {}
