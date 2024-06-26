// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSVALUELIST_H_
#define CORE_FXCRT_CSS_CFX_CSSVALUELIST_H_

#include <vector>

#include "core/fxcrt/css/cfx_cssvalue.h"

class CFX_CSSValueList final : public CFX_CSSValue {
 public:
  explicit CFX_CSSValueList(std::vector<RetainPtr<CFX_CSSValue>>& list);
  ~CFX_CSSValueList() override;

  int32_t CountValues() const;
  RetainPtr<CFX_CSSValue> GetValue(int32_t index) const;

 private:
  std::vector<RetainPtr<CFX_CSSValue>> m_ppList;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSVALUELIST_H_
