// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssnumbervalue.h"

CFX_CSSNumberValue::CFX_CSSNumberValue(CFX_CSSNumberType type, float value)
    : CFX_CSSValue(CFX_CSSPrimitiveType::Number), type_(type), value_(value) {
  if (type_ == CFX_CSSNumberType::Number && fabs(value_) < 0.001f)
    value_ = 0.0f;
}

CFX_CSSNumberValue::~CFX_CSSNumberValue() {}

float CFX_CSSNumberValue::Apply(float percentBase) const {
  switch (type_) {
    case CFX_CSSNumberType::Pixels:
    case CFX_CSSNumberType::Number:
      return value_ * 72 / 96;
    case CFX_CSSNumberType::EMS:
    case CFX_CSSNumberType::EXS:
      return value_ * percentBase;
    case CFX_CSSNumberType::Percent:
      return value_ * percentBase / 100.0f;
    case CFX_CSSNumberType::CentiMeters:
      return value_ * 28.3464f;
    case CFX_CSSNumberType::MilliMeters:
      return value_ * 2.8346f;
    case CFX_CSSNumberType::Inches:
      return value_ * 72.0f;
    case CFX_CSSNumberType::Picas:
      return value_ / 12.0f;
    case CFX_CSSNumberType::Points:
      return value_;
  }
  return value_;
}
