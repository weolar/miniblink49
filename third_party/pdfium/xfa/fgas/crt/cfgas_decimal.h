// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_CRT_CFGAS_DECIMAL_H_
#define XFA_FGAS_CRT_CFGAS_DECIMAL_H_

#include "core/fxcrt/fx_string.h"

class CFGAS_Decimal {
 public:
  CFGAS_Decimal();
  explicit CFGAS_Decimal(uint32_t val);
  explicit CFGAS_Decimal(uint64_t val);
  explicit CFGAS_Decimal(int32_t val);
  CFGAS_Decimal(float val, uint8_t scale);
  explicit CFGAS_Decimal(WideStringView str);

  operator WideString() const;
  operator double() const;

  CFGAS_Decimal operator*(const CFGAS_Decimal& val) const;
  CFGAS_Decimal operator/(const CFGAS_Decimal& val) const;

  void SetScale(uint8_t newScale);
  uint8_t GetScale();
  void SetNegate();

 private:
  CFGAS_Decimal(uint32_t hi,
                uint32_t mid,
                uint32_t lo,
                bool neg,
                uint8_t scale);
  bool IsNotZero() const { return m_uHi || m_uMid || m_uLo; }
  void Swap(CFGAS_Decimal& val);

  uint32_t m_uHi;
  uint32_t m_uLo;
  uint32_t m_uMid;
  uint32_t m_uFlags;
};

#endif  // XFA_FGAS_CRT_CFGAS_DECIMAL_H_
