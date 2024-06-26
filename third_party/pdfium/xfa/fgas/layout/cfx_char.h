// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_LAYOUT_CFX_CHAR_H_
#define XFA_FGAS_LAYOUT_CFX_CHAR_H_

#include <stdint.h>

#include <vector>

#include "core/fxcrt/fx_unicode.h"
#include "core/fxcrt/retain_ptr.h"
#include "xfa/fgas/layout/fx_linebreak.h"

enum class CFX_BreakType : uint8_t { None = 0, Piece, Line, Paragraph, Page };

class CFX_Char {
 public:
  static void BidiLine(std::vector<CFX_Char>* chars, size_t iCount);

  explicit CFX_Char(uint16_t wCharCode);
  CFX_Char(uint16_t wCharCode,
           int32_t iHorizontalScale,
           int32_t iVerticalScale);
  CFX_Char(const CFX_Char& other);
  ~CFX_Char();

  FX_CHARTYPE GetCharType() const;

  uint16_t char_code() const { return m_wCharCode; }
  int16_t horizonal_scale() const { return m_iHorizontalScale; }
  int16_t vertical_scale() const { return m_iVerticalScale; }

  CFX_BreakType m_dwStatus = CFX_BreakType::None;
  FX_BIDICLASS m_iBidiClass = FX_BIDICLASS::kON;
  FX_LINEBREAKTYPE m_eLineBreakType = FX_LINEBREAKTYPE::kUNKNOWN;
  uint32_t m_dwCharStyles = 0;
  int32_t m_iCharWidth = 0;
  uint16_t m_iBidiLevel = 0;
  uint16_t m_iBidiPos = 0;
  uint16_t m_iBidiOrder = 0;
  int32_t m_iFontSize = 0;
  uint32_t m_dwIdentity = 0;
  RetainPtr<Retainable> m_pUserData;

 private:
  uint16_t m_wCharCode;
  int32_t m_iHorizontalScale;
  int32_t m_iVerticalScale;
};

#endif  // XFA_FGAS_LAYOUT_CFX_CHAR_H_
