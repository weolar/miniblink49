// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_MEASUREMENT_H_
#define XFA_FXFA_PARSER_CXFA_MEASUREMENT_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "xfa/fxfa/fxfa_basic.h"

class CXFA_Measurement {
 public:
  explicit CXFA_Measurement(WideStringView wsMeasure);
  CXFA_Measurement();
  CXFA_Measurement(float fValue, XFA_Unit eUnit);

  static XFA_Unit GetUnitFromString(WideStringView wsUnit);

  void Set(float fValue, XFA_Unit eUnit) {
    m_fValue = fValue;
    m_eUnit = eUnit;
  }

  XFA_Unit GetUnit() const { return m_eUnit; }
  float GetValue() const { return m_fValue; }

  WideString ToString() const;
  float ToUnit(XFA_Unit eUnit) const;

 private:
  void SetString(WideStringView wsMeasure);
  bool ToUnitInternal(XFA_Unit eUnit, float* fValue) const;

  float m_fValue;
  XFA_Unit m_eUnit;
};

#endif  // XFA_FXFA_PARSER_CXFA_MEASUREMENT_H_
