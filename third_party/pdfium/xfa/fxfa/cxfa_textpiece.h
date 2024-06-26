// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_TEXTPIECE_H_
#define XFA_FXFA_CXFA_TEXTPIECE_H_

#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxge/fx_dib.h"
#include "xfa/fxfa/fxfa_basic.h"

class CFGAS_GEFont;
class CXFA_LinkUserData;

class CXFA_TextPiece {
 public:
  CXFA_TextPiece();
  ~CXFA_TextPiece();

  WideString szText;
  std::vector<int32_t> Widths;
  int32_t iChars;
  int32_t iHorScale;
  int32_t iVerScale;
  int32_t iBidiLevel;
  int32_t iUnderline;
  XFA_AttributeValue iPeriod;
  int32_t iLineThrough;
  FX_ARGB dwColor;
  float fFontSize;
  CFX_RectF rtPiece;
  RetainPtr<CFGAS_GEFont> pFont;
  RetainPtr<CXFA_LinkUserData> pLinkData;
};

#endif  // XFA_FXFA_CXFA_TEXTPIECE_H_
