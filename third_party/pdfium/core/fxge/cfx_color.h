// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_COLOR_H_
#define CORE_FXGE_CFX_COLOR_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxge/fx_dib.h"

class CPDF_Array;

struct CFX_Color {
  static CFX_Color ParseColor(const CPDF_Array& array);
  static CFX_Color ParseColor(const ByteString& str);

  // Ordered by increasing number of components.
  enum Type { kTransparent = 0, kGray, kRGB, kCMYK };

  explicit CFX_Color(FX_COLORREF ref)
      : CFX_Color(FXARGB_R(ref), FXARGB_G(ref), FXARGB_B(ref)) {}

  CFX_Color(int32_t type = CFX_Color::kTransparent,
            float color1 = 0.0f,
            float color2 = 0.0f,
            float color3 = 0.0f,
            float color4 = 0.0f)
      : nColorType(type),
        fColor1(color1),
        fColor2(color2),
        fColor3(color3),
        fColor4(color4) {}

  CFX_Color(int32_t r, int32_t g, int32_t b)
      : nColorType(CFX_Color::kRGB),
        fColor1(r / 255.0f),
        fColor2(g / 255.0f),
        fColor3(b / 255.0f),
        fColor4(0) {}

  CFX_Color(const CFX_Color&) = default;

  CFX_Color operator/(float fColorDivide) const;
  CFX_Color operator-(float fColorSub) const;

  CFX_Color ConvertColorType(int32_t other_nColorType) const;

  FX_COLORREF ToFXColor(int32_t nTransparency) const;

  void Reset() {
    nColorType = CFX_Color::kTransparent;
    fColor1 = 0.0f;
    fColor2 = 0.0f;
    fColor3 = 0.0f;
    fColor4 = 0.0f;
  }

  int32_t nColorType;
  float fColor1;
  float fColor2;
  float fColor3;
  float fColor4;
};

inline bool operator==(const CFX_Color& c1, const CFX_Color& c2) {
  return c1.nColorType == c2.nColorType && c1.fColor1 - c2.fColor1 < 0.0001 &&
         c1.fColor1 - c2.fColor1 > -0.0001 &&
         c1.fColor2 - c2.fColor2 < 0.0001 &&
         c1.fColor2 - c2.fColor2 > -0.0001 &&
         c1.fColor3 - c2.fColor3 < 0.0001 &&
         c1.fColor3 - c2.fColor3 > -0.0001 &&
         c1.fColor4 - c2.fColor4 < 0.0001 && c1.fColor4 - c2.fColor4 > -0.0001;
}

inline bool operator!=(const CFX_Color& c1, const CFX_Color& c2) {
  return !(c1 == c2);
}

#endif  // CORE_FXGE_CFX_COLOR_H_
