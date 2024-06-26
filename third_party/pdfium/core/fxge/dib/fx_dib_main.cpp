// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/fx_dib.h"

#include <tuple>
#include <utility>

#include "core/fxcrt/fx_extension.h"

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
static_assert(sizeof(FX_COLORREF) == sizeof(COLORREF),
              "FX_COLORREF vs. COLORREF mismatch");
#endif

const int16_t SDP_Table[513] = {
    256, 256, 256, 256, 256, 256, 256, 256, 256, 255, 255, 255, 255, 255, 255,
    254, 254, 254, 254, 253, 253, 253, 252, 252, 252, 251, 251, 251, 250, 250,
    249, 249, 249, 248, 248, 247, 247, 246, 246, 245, 244, 244, 243, 243, 242,
    242, 241, 240, 240, 239, 238, 238, 237, 236, 236, 235, 234, 233, 233, 232,
    231, 230, 230, 229, 228, 227, 226, 226, 225, 224, 223, 222, 221, 220, 219,
    218, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208, 207, 206, 205,
    204, 203, 202, 201, 200, 199, 198, 196, 195, 194, 193, 192, 191, 190, 189,
    188, 186, 185, 184, 183, 182, 181, 179, 178, 177, 176, 175, 173, 172, 171,
    170, 169, 167, 166, 165, 164, 162, 161, 160, 159, 157, 156, 155, 154, 152,
    151, 150, 149, 147, 146, 145, 143, 142, 141, 140, 138, 137, 136, 134, 133,
    132, 130, 129, 128, 126, 125, 124, 122, 121, 120, 119, 117, 116, 115, 113,
    112, 111, 109, 108, 107, 105, 104, 103, 101, 100, 99,  97,  96,  95,  93,
    92,  91,  89,  88,  87,  85,  84,  83,  81,  80,  79,  77,  76,  75,  73,
    72,  71,  69,  68,  67,  66,  64,  63,  62,  60,  59,  58,  57,  55,  54,
    53,  52,  50,  49,  48,  47,  45,  44,  43,  42,  40,  39,  38,  37,  36,
    34,  33,  32,  31,  30,  28,  27,  26,  25,  24,  23,  21,  20,  19,  18,
    17,  16,  15,  14,  13,  11,  10,  9,   8,   7,   6,   5,   4,   3,   2,
    1,   0,   0,   -1,  -2,  -3,  -4,  -5,  -6,  -7,  -7,  -8,  -9,  -10, -11,
    -12, -12, -13, -14, -15, -15, -16, -17, -17, -18, -19, -19, -20, -21, -21,
    -22, -22, -23, -24, -24, -25, -25, -26, -26, -27, -27, -27, -28, -28, -29,
    -29, -30, -30, -30, -31, -31, -31, -32, -32, -32, -33, -33, -33, -33, -34,
    -34, -34, -34, -35, -35, -35, -35, -35, -36, -36, -36, -36, -36, -36, -36,
    -36, -36, -37, -37, -37, -37, -37, -37, -37, -37, -37, -37, -37, -37, -37,
    -37, -37, -37, -37, -37, -37, -37, -37, -36, -36, -36, -36, -36, -36, -36,
    -36, -36, -35, -35, -35, -35, -35, -35, -34, -34, -34, -34, -34, -33, -33,
    -33, -33, -33, -32, -32, -32, -32, -31, -31, -31, -31, -30, -30, -30, -30,
    -29, -29, -29, -29, -28, -28, -28, -27, -27, -27, -27, -26, -26, -26, -25,
    -25, -25, -24, -24, -24, -23, -23, -23, -22, -22, -22, -22, -21, -21, -21,
    -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16,
    -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -12, -11, -11, -11,
    -10, -10, -10, -9,  -9,  -9,  -9,  -8,  -8,  -8,  -7,  -7,  -7,  -7,  -6,
    -6,  -6,  -6,  -5,  -5,  -5,  -5,  -4,  -4,  -4,  -4,  -3,  -3,  -3,  -3,
    -3,  -2,  -2,  -2,  -2,  -2,  -1,  -1,  -1,  -1,  -1,  -1,  0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,
};

FXDIB_ResampleOptions::FXDIB_ResampleOptions() = default;

FXDIB_ResampleOptions::FXDIB_ResampleOptions(bool downsample,
                                             bool bilinear,
                                             bool bicubic,
                                             bool halftone,
                                             bool no_smoothing,
                                             bool lossy)
    : bInterpolateDownsample(downsample),
      bInterpolateBilinear(bilinear),
      bInterpolateBicubic(bicubic),
      bHalftone(halftone),
      bNoSmoothing(no_smoothing),
      bLossy(lossy) {}

bool FXDIB_ResampleOptions::HasAnyOptions() const {
  return bInterpolateDownsample || bInterpolateBilinear ||
         bInterpolateBicubic || bHalftone || bNoSmoothing || bLossy;
}

const FXDIB_ResampleOptions kBilinearInterpolation = {
    false, /*bilinear=*/true, false, false, false, false};

FX_RECT FXDIB_SwapClipBox(const FX_RECT& clip,
                          int width,
                          int height,
                          bool bFlipX,
                          bool bFlipY) {
  FX_RECT rect;
  if (bFlipY) {
    rect.left = height - clip.top;
    rect.right = height - clip.bottom;
  } else {
    rect.left = clip.top;
    rect.right = clip.bottom;
  }
  if (bFlipX) {
    rect.top = width - clip.left;
    rect.bottom = width - clip.right;
  } else {
    rect.top = clip.left;
    rect.bottom = clip.right;
  }
  rect.Normalize();
  return rect;
}

std::tuple<int, int, int, int> ArgbDecode(FX_ARGB argb) {
  return std::make_tuple(FXARGB_A(argb), FXARGB_R(argb), FXARGB_G(argb),
                         FXARGB_B(argb));
}

std::pair<int, FX_COLORREF> ArgbToAlphaAndColorRef(FX_ARGB argb) {
  return {FXARGB_A(argb), ArgbToColorRef(argb)};
}

FX_COLORREF ArgbToColorRef(FX_ARGB argb) {
  return FXSYS_BGR(FXARGB_B(argb), FXARGB_G(argb), FXARGB_R(argb));
}

FX_ARGB AlphaAndColorRefToArgb(int a, FX_COLORREF colorref) {
  return ArgbEncode(a, FXSYS_GetRValue(colorref), FXSYS_GetGValue(colorref),
                    FXSYS_GetBValue(colorref));
}

FX_ARGB StringToFXARGB(WideStringView wsValue) {
  static constexpr FX_ARGB kDefaultValue = 0xff000000;
  if (wsValue.GetLength() == 0)
    return kDefaultValue;

  int cc = 0;
  const wchar_t* str = wsValue.unterminated_c_str();
  int len = wsValue.GetLength();
  while (cc < len && FXSYS_iswspace(str[cc]))
    cc++;

  if (cc >= len)
    return kDefaultValue;

  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  while (cc < len) {
    if (str[cc] == ',' || !FXSYS_IsDecimalDigit(str[cc]))
      break;

    r = r * 10 + str[cc] - '0';
    cc++;
  }
  if (cc < len && str[cc] == ',') {
    cc++;
    while (cc < len && FXSYS_iswspace(str[cc]))
      cc++;

    while (cc < len) {
      if (str[cc] == ',' || !FXSYS_IsDecimalDigit(str[cc]))
        break;

      g = g * 10 + str[cc] - '0';
      cc++;
    }
    if (cc < len && str[cc] == ',') {
      cc++;
      while (cc < len && FXSYS_iswspace(str[cc]))
        cc++;

      while (cc < len) {
        if (str[cc] == ',' || !FXSYS_IsDecimalDigit(str[cc]))
          break;

        b = b * 10 + str[cc] - '0';
        cc++;
      }
    }
  }
  return (0xff << 24) | (r << 16) | (g << 8) | b;
}
