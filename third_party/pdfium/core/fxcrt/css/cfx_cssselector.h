// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSSELECTOR_H_
#define CORE_FXCRT_CSS_CFX_CSSSELECTOR_H_

#include <memory>
#include <utility>

#include "core/fxcrt/css/cfx_css.h"
#include "core/fxcrt/fx_string.h"

class CFX_CSSSelector {
 public:
  static std::unique_ptr<CFX_CSSSelector> FromString(WideStringView str);

  CFX_CSSSelector(CFX_CSSSelectorType eType,
                  const wchar_t* psz,
                  int32_t iLen,
                  bool bIgnoreCase);
  ~CFX_CSSSelector();

  CFX_CSSSelectorType GetType() const;
  uint32_t GetNameHash() const;
  CFX_CSSSelector* GetNextSelector() const;

  void SetNext(std::unique_ptr<CFX_CSSSelector> pNext) {
    m_pNext = std::move(pNext);
  }

 private:
  void SetType(CFX_CSSSelectorType eType) { m_eType = eType; }

  CFX_CSSSelectorType m_eType;
  uint32_t m_dwHash;
  std::unique_ptr<CFX_CSSSelector> m_pNext;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSSELECTOR_H_
