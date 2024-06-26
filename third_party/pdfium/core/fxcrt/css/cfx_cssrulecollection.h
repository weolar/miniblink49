// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSRULECOLLECTION_H_
#define CORE_FXCRT_CSS_CFX_CSSRULECOLLECTION_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"

class CFX_CSSDeclaration;
class CFX_CSSSelector;
class CFX_CSSStyleRule;
class CFX_CSSStyleSheet;

class CFX_CSSRuleCollection {
 public:
  class Data {
   public:
    Data(CFX_CSSSelector* pSel, CFX_CSSDeclaration* pDecl);

    CFX_CSSSelector* const pSelector;
    CFX_CSSDeclaration* const pDeclaration;
  };

  CFX_CSSRuleCollection();
  ~CFX_CSSRuleCollection();

  void AddRulesFrom(const CFX_CSSStyleSheet* sheet);
  void Clear();
  int32_t CountSelectors() const { return m_iSelectors; }

  const std::vector<std::unique_ptr<Data>>* GetTagRuleData(
      const WideString& tagname) const;

 private:
  void AddRulesFrom(const CFX_CSSStyleSheet* pStyleSheet,
                    CFX_CSSStyleRule* pRule);

  std::map<uint32_t, std::vector<std::unique_ptr<Data>>> m_TagRules;
  int32_t m_iSelectors;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSRULECOLLECTION_H_
