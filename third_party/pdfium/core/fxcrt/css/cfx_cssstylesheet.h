// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSSTYLESHEET_H_
#define CORE_FXCRT_CSS_CFX_CSSSTYLESHEET_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/css/cfx_csssyntaxparser.h"
#include "core/fxcrt/fx_string.h"

class CFX_CSSStyleRule;

class CFX_CSSStyleSheet {
 public:
  CFX_CSSStyleSheet();
  ~CFX_CSSStyleSheet();

  bool LoadBuffer(const wchar_t* pBuffer, int32_t iBufSize);

  int32_t CountRules() const;
  CFX_CSSStyleRule* GetRule(int32_t index) const;

 private:
  void Reset();
  CFX_CSSSyntaxStatus LoadStyleRule(
      CFX_CSSSyntaxParser* pSyntax,
      std::vector<std::unique_ptr<CFX_CSSStyleRule>>* ruleArray);
  void SkipRuleSet(CFX_CSSSyntaxParser* pSyntax);

  std::vector<std::unique_ptr<CFX_CSSStyleRule>> m_RuleArray;
  std::map<uint32_t, wchar_t*> m_StringCache;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSSTYLESHEET_H_
