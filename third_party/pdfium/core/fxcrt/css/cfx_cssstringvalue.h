// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSSTRINGVALUE_H_
#define CORE_FXCRT_CSS_CFX_CSSSTRINGVALUE_H_

#include "core/fxcrt/css/cfx_cssvalue.h"
#include "core/fxcrt/widestring.h"

class CFX_CSSStringValue final : public CFX_CSSValue {
 public:
  explicit CFX_CSSStringValue(const WideString& value);
  ~CFX_CSSStringValue() override;

  const WideString Value() const { return value_; }

 private:
  const WideString value_;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSSTRINGVALUE_H_
