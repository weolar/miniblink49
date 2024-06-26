// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSCOLORVALUE_H_
#define CORE_FXCRT_CSS_CFX_CSSCOLORVALUE_H_

#include "core/fxcrt/css/cfx_cssvalue.h"
#include "core/fxge/fx_dib.h"

class CFX_CSSColorValue final : public CFX_CSSValue {
 public:
  explicit CFX_CSSColorValue(FX_ARGB color);
  ~CFX_CSSColorValue() override;

  FX_ARGB Value() const { return value_; }

 private:
  FX_ARGB value_;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSCOLORVALUE_H_
