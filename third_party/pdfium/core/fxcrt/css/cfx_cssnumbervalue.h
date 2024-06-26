// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSNUMBERVALUE_H_
#define CORE_FXCRT_CSS_CFX_CSSNUMBERVALUE_H_

#include "core/fxcrt/css/cfx_cssvalue.h"
#include "core/fxcrt/fx_system.h"

enum class CFX_CSSNumberType {
  Number,
  Percent,
  EMS,
  EXS,
  Pixels,
  CentiMeters,
  MilliMeters,
  Inches,
  Points,
  Picas,
};

class CFX_CSSNumberValue final : public CFX_CSSValue {
 public:
  CFX_CSSNumberValue(CFX_CSSNumberType type, float value);
  ~CFX_CSSNumberValue() override;

  float Value() const { return value_; }
  CFX_CSSNumberType Kind() const { return type_; }

  float Apply(float percentBase) const;

 private:
  CFX_CSSNumberType type_;
  float value_;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSNUMBERVALUE_H_
