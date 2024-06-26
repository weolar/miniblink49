// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_color.h"

#include <algorithm>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfdoc/cpdf_defaultappearance.h"

// Color types are orded by increasing number of components so we can
// choose a best color type during some conversions.
static_assert(CFX_Color::kTransparent < CFX_Color::kGray,
              "color type values must be ordered");
static_assert(CFX_Color::kGray < CFX_Color::kRGB,
              "color type values must be ordered");
static_assert(CFX_Color::kRGB < CFX_Color::kCMYK,
              "color type values must be ordered");

namespace {

bool InRange(float comp) {
  return comp >= 0.0f && comp <= 1.0f;
}

CFX_Color ConvertCMYK2GRAY(float dC, float dM, float dY, float dK) {
  if (!InRange(dC) || !InRange(dM) || !InRange(dY) || !InRange(dK))
    return CFX_Color(CFX_Color::kGray);
  return CFX_Color(
      CFX_Color::kGray,
      1.0f - std::min(1.0f, 0.3f * dC + 0.59f * dM + 0.11f * dY + dK));
}

CFX_Color ConvertGRAY2CMYK(float dGray) {
  if (!InRange(dGray))
    return CFX_Color(CFX_Color::kCMYK);
  return CFX_Color(CFX_Color::kCMYK, 0.0f, 0.0f, 0.0f, 1.0f - dGray);
}

CFX_Color ConvertGRAY2RGB(float dGray) {
  if (!InRange(dGray))
    return CFX_Color(CFX_Color::kRGB);
  return CFX_Color(CFX_Color::kRGB, dGray, dGray, dGray);
}

CFX_Color ConvertRGB2GRAY(float dR, float dG, float dB) {
  if (!InRange(dR) || !InRange(dG) || !InRange(dB))
    return CFX_Color(CFX_Color::kGray);
  return CFX_Color(CFX_Color::kGray, 0.3f * dR + 0.59f * dG + 0.11f * dB);
}

CFX_Color ConvertCMYK2RGB(float dC, float dM, float dY, float dK) {
  if (!InRange(dC) || !InRange(dM) || !InRange(dY) || !InRange(dK))
    return CFX_Color(CFX_Color::kRGB);
  return CFX_Color(CFX_Color::kRGB, 1.0f - std::min(1.0f, dC + dK),
                   1.0f - std::min(1.0f, dM + dK),
                   1.0f - std::min(1.0f, dY + dK));
}

CFX_Color ConvertRGB2CMYK(float dR, float dG, float dB) {
  if (!InRange(dR) || !InRange(dG) || !InRange(dB))
    return CFX_Color(CFX_Color::kCMYK);

  float c = 1.0f - dR;
  float m = 1.0f - dG;
  float y = 1.0f - dB;
  return CFX_Color(CFX_Color::kCMYK, c, m, y, std::min(c, std::min(m, y)));
}

}  // namespace

// Static.
CFX_Color CFX_Color::ParseColor(const CPDF_Array& array) {
  CFX_Color rt;
  switch (array.size()) {
    case 1:
      rt = CFX_Color(CFX_Color::kGray, array.GetFloatAt(0));
      break;
    case 3:
      rt = CFX_Color(CFX_Color::kRGB, array.GetFloatAt(0), array.GetFloatAt(1),
                     array.GetFloatAt(2));
      break;
    case 4:
      rt = CFX_Color(CFX_Color::kCMYK, array.GetFloatAt(0), array.GetFloatAt(1),
                     array.GetFloatAt(2), array.GetFloatAt(3));
      break;
  }
  return rt;
}

// Static.
CFX_Color CFX_Color::ParseColor(const ByteString& str) {
  CPDF_DefaultAppearance appearance(str);
  float values[4];
  Optional<CFX_Color::Type> color_type = appearance.GetColor(values);
  if (!color_type || *color_type == CFX_Color::kTransparent)
    return CFX_Color(CFX_Color::kTransparent);
  if (*color_type == CFX_Color::kGray)
    return CFX_Color(CFX_Color::kGray, values[0]);
  if (*color_type == CFX_Color::kRGB)
    return CFX_Color(CFX_Color::kRGB, values[0], values[1], values[2]);
  if (*color_type == CFX_Color::kCMYK) {
    return CFX_Color(CFX_Color::kCMYK, values[0], values[1], values[2],
                     values[3]);
  }

  NOTREACHED();
  return CFX_Color(CFX_Color::kTransparent);
}

CFX_Color CFX_Color::ConvertColorType(int32_t nConvertColorType) const {
  if (nColorType == nConvertColorType)
    return *this;

  CFX_Color ret;
  switch (nColorType) {
    case CFX_Color::kTransparent:
      ret = *this;
      ret.nColorType = CFX_Color::kTransparent;
      break;
    case CFX_Color::kGray:
      switch (nConvertColorType) {
        case CFX_Color::kRGB:
          ret = ConvertGRAY2RGB(fColor1);
          break;
        case CFX_Color::kCMYK:
          ret = ConvertGRAY2CMYK(fColor1);
          break;
      }
      break;
    case CFX_Color::kRGB:
      switch (nConvertColorType) {
        case CFX_Color::kGray:
          ret = ConvertRGB2GRAY(fColor1, fColor2, fColor3);
          break;
        case CFX_Color::kCMYK:
          ret = ConvertRGB2CMYK(fColor1, fColor2, fColor3);
          break;
      }
      break;
    case CFX_Color::kCMYK:
      switch (nConvertColorType) {
        case CFX_Color::kGray:
          ret = ConvertCMYK2GRAY(fColor1, fColor2, fColor3, fColor4);
          break;
        case CFX_Color::kRGB:
          ret = ConvertCMYK2RGB(fColor1, fColor2, fColor3, fColor4);
          break;
      }
      break;
  }
  return ret;
}

FX_COLORREF CFX_Color::ToFXColor(int32_t nTransparency) const {
  CFX_Color ret;
  switch (nColorType) {
    case CFX_Color::kTransparent: {
      ret = CFX_Color(CFX_Color::kTransparent, 0, 0, 0, 0);
      break;
    }
    case CFX_Color::kGray: {
      ret = ConvertGRAY2RGB(fColor1);
      ret.fColor4 = nTransparency;
      break;
    }
    case CFX_Color::kRGB: {
      ret = CFX_Color(CFX_Color::kRGB, fColor1, fColor2, fColor3);
      ret.fColor4 = nTransparency;
      break;
    }
    case CFX_Color::kCMYK: {
      ret = ConvertCMYK2RGB(fColor1, fColor2, fColor3, fColor4);
      ret.fColor4 = nTransparency;
      break;
    }
  }
  return ArgbEncode(ret.fColor4, static_cast<int32_t>(ret.fColor1 * 255),
                    static_cast<int32_t>(ret.fColor2 * 255),
                    static_cast<int32_t>(ret.fColor3 * 255));
}

CFX_Color CFX_Color::operator-(float fColorSub) const {
  CFX_Color sRet(nColorType);
  switch (nColorType) {
    case CFX_Color::kTransparent:
      sRet.nColorType = CFX_Color::kRGB;
      sRet.fColor1 = std::max(1.0f - fColorSub, 0.0f);
      sRet.fColor2 = std::max(1.0f - fColorSub, 0.0f);
      sRet.fColor3 = std::max(1.0f - fColorSub, 0.0f);
      break;
    case CFX_Color::kRGB:
    case CFX_Color::kGray:
    case CFX_Color::kCMYK:
      sRet.fColor1 = std::max(fColor1 - fColorSub, 0.0f);
      sRet.fColor2 = std::max(fColor2 - fColorSub, 0.0f);
      sRet.fColor3 = std::max(fColor3 - fColorSub, 0.0f);
      sRet.fColor4 = std::max(fColor4 - fColorSub, 0.0f);
      break;
  }
  return sRet;
}

CFX_Color CFX_Color::operator/(float fColorDivide) const {
  CFX_Color sRet(nColorType);
  switch (nColorType) {
    case CFX_Color::kTransparent:
      sRet.nColorType = CFX_Color::kRGB;
      sRet.fColor1 = 1.0f / fColorDivide;
      sRet.fColor2 = 1.0f / fColorDivide;
      sRet.fColor3 = 1.0f / fColorDivide;
      break;
    case CFX_Color::kRGB:
    case CFX_Color::kGray:
    case CFX_Color::kCMYK:
      sRet = *this;
      sRet.fColor1 /= fColorDivide;
      sRet.fColor2 /= fColorDivide;
      sRet.fColor3 /= fColorDivide;
      sRet.fColor4 /= fColorDivide;
      break;
  }
  return sRet;
}
