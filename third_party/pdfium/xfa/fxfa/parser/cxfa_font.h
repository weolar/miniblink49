// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_FONT_H_
#define XFA_FXFA_PARSER_CXFA_FONT_H_

#include "core/fxge/fx_dib.h"
#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_Font final : public CXFA_Node {
 public:
  CXFA_Font(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Font() override;

  float GetBaselineShift() const;
  float GetHorizontalScale();
  float GetVerticalScale();
  float GetLetterSpacing();
  int32_t GetLineThrough();
  int32_t GetUnderline();
  XFA_AttributeValue GetUnderlinePeriod();
  float GetFontSize() const;
  WideString GetTypeface();

  bool IsBold();
  bool IsItalic();

  FX_ARGB GetColor();
  void SetColor(FX_ARGB color);
};

#endif  // XFA_FXFA_PARSER_CXFA_FONT_H_
