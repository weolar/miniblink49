// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSENUMVALUE_H_
#define CORE_FXCRT_CSS_CFX_CSSENUMVALUE_H_

#include "core/fxcrt/css/cfx_cssvalue.h"

class CFX_CSSEnumValue final : public CFX_CSSValue {
 public:
  explicit CFX_CSSEnumValue(CFX_CSSPropertyValue value);
  ~CFX_CSSEnumValue() override;

  CFX_CSSPropertyValue Value() const { return value_; }

 private:
  CFX_CSSPropertyValue value_;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSENUMVALUE_H_
