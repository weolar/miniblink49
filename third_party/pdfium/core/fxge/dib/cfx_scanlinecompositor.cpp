// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/dib/cfx_scanlinecompositor.h"

#include <algorithm>

#include "core/fxcodec/fx_codec.h"

#define FX_CCOLOR(val) (255 - (val))
#define FXDIB_ALPHA_UNION(dest, src) ((dest) + (src) - (dest) * (src) / 255)
#define FXARGB_COPY(dest, src)                    \
  *(dest) = *(src), *((dest) + 1) = *((src) + 1), \
  *((dest) + 2) = *((src) + 2), *((dest) + 3) = *((src) + 3)
#define FXARGB_RGBORDERCOPY(dest, src)                  \
  *((dest) + 3) = *((src) + 3), *(dest) = *((src) + 2), \
             *((dest) + 1) = *((src) + 1), *((dest) + 2) = *((src))

namespace {

const uint8_t color_sqrt[256] = {
    0x00, 0x03, 0x07, 0x0B, 0x0F, 0x12, 0x16, 0x19, 0x1D, 0x20, 0x23, 0x26,
    0x29, 0x2C, 0x2F, 0x32, 0x35, 0x37, 0x3A, 0x3C, 0x3F, 0x41, 0x43, 0x46,
    0x48, 0x4A, 0x4C, 0x4E, 0x50, 0x52, 0x54, 0x56, 0x57, 0x59, 0x5B, 0x5C,
    0x5E, 0x60, 0x61, 0x63, 0x64, 0x65, 0x67, 0x68, 0x69, 0x6B, 0x6C, 0x6D,
    0x6E, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
    0x7B, 0x7C, 0x7D, 0x7E, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x91,
    0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C,
    0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA4, 0xA5,
    0xA6, 0xA7, 0xA7, 0xA8, 0xA9, 0xAA, 0xAA, 0xAB, 0xAC, 0xAD, 0xAD, 0xAE,
    0xAF, 0xB0, 0xB0, 0xB1, 0xB2, 0xB3, 0xB3, 0xB4, 0xB5, 0xB5, 0xB6, 0xB7,
    0xB7, 0xB8, 0xB9, 0xBA, 0xBA, 0xBB, 0xBC, 0xBC, 0xBD, 0xBE, 0xBE, 0xBF,
    0xC0, 0xC0, 0xC1, 0xC2, 0xC2, 0xC3, 0xC4, 0xC4, 0xC5, 0xC6, 0xC6, 0xC7,
    0xC7, 0xC8, 0xC9, 0xC9, 0xCA, 0xCB, 0xCB, 0xCC, 0xCC, 0xCD, 0xCE, 0xCE,
    0xCF, 0xD0, 0xD0, 0xD1, 0xD1, 0xD2, 0xD3, 0xD3, 0xD4, 0xD4, 0xD5, 0xD6,
    0xD6, 0xD7, 0xD7, 0xD8, 0xD9, 0xD9, 0xDA, 0xDA, 0xDB, 0xDC, 0xDC, 0xDD,
    0xDD, 0xDE, 0xDE, 0xDF, 0xE0, 0xE0, 0xE1, 0xE1, 0xE2, 0xE2, 0xE3, 0xE4,
    0xE4, 0xE5, 0xE5, 0xE6, 0xE6, 0xE7, 0xE7, 0xE8, 0xE9, 0xE9, 0xEA, 0xEA,
    0xEB, 0xEB, 0xEC, 0xEC, 0xED, 0xED, 0xEE, 0xEE, 0xEF, 0xF0, 0xF0, 0xF1,
    0xF1, 0xF2, 0xF2, 0xF3, 0xF3, 0xF4, 0xF4, 0xF5, 0xF5, 0xF6, 0xF6, 0xF7,
    0xF7, 0xF8, 0xF8, 0xF9, 0xF9, 0xFA, 0xFA, 0xFB, 0xFB, 0xFC, 0xFC, 0xFD,
    0xFD, 0xFE, 0xFE, 0xFF};

int Blend(BlendMode blend_mode, int back_color, int src_color) {
  switch (blend_mode) {
    case BlendMode::kNormal:
      return src_color;
    case BlendMode::kMultiply:
      return src_color * back_color / 255;
    case BlendMode::kScreen:
      return src_color + back_color - src_color * back_color / 255;
    case BlendMode::kOverlay:
      return Blend(BlendMode::kHardLight, src_color, back_color);
    case BlendMode::kDarken:
      return src_color < back_color ? src_color : back_color;
    case BlendMode::kLighten:
      return src_color > back_color ? src_color : back_color;
    case BlendMode::kColorDodge: {
      if (src_color == 255)
        return src_color;

      return std::min(back_color * 255 / (255 - src_color), 255);
    }
    case BlendMode::kColorBurn: {
      if (src_color == 0)
        return src_color;

      return 255 - std::min((255 - back_color) * 255 / src_color, 255);
    }
    case BlendMode::kHardLight:
      if (src_color < 128)
        return (src_color * back_color * 2) / 255;

      return Blend(BlendMode::kScreen, back_color, 2 * src_color - 255);
    case BlendMode::kSoftLight: {
      if (src_color < 128) {
        return back_color -
               (255 - 2 * src_color) * back_color * (255 - back_color) / 255 /
                   255;
      }
      return back_color +
             (2 * src_color - 255) * (color_sqrt[back_color] - back_color) /
                 255;
    }
    case BlendMode::kDifference:
      return back_color < src_color ? src_color - back_color
                                    : back_color - src_color;
    case BlendMode::kExclusion:
      return back_color + src_color - 2 * back_color * src_color / 255;
    default:
      return src_color;
  }
}

struct RGB {
  int red;
  int green;
  int blue;
};

int Lum(RGB color) {
  return (color.red * 30 + color.green * 59 + color.blue * 11) / 100;
}

RGB ClipColor(RGB color) {
  int l = Lum(color);
  int n = std::min(color.red, std::min(color.green, color.blue));
  int x = std::max(color.red, std::max(color.green, color.blue));
  if (n < 0) {
    color.red = l + ((color.red - l) * l / (l - n));
    color.green = l + ((color.green - l) * l / (l - n));
    color.blue = l + ((color.blue - l) * l / (l - n));
  }
  if (x > 255) {
    color.red = l + ((color.red - l) * (255 - l) / (x - l));
    color.green = l + ((color.green - l) * (255 - l) / (x - l));
    color.blue = l + ((color.blue - l) * (255 - l) / (x - l));
  }
  return color;
}

RGB SetLum(RGB color, int l) {
  int d = l - Lum(color);
  color.red += d;
  color.green += d;
  color.blue += d;
  return ClipColor(color);
}

int Sat(RGB color) {
  return std::max(color.red, std::max(color.green, color.blue)) -
         std::min(color.red, std::min(color.green, color.blue));
}

RGB SetSat(RGB color, int s) {
  int min = std::min(color.red, std::min(color.green, color.blue));
  int max = std::max(color.red, std::max(color.green, color.blue));
  if (min == max)
    return {0, 0, 0};

  color.red = (color.red - min) * s / (max - min);
  color.green = (color.green - min) * s / (max - min);
  color.blue = (color.blue - min) * s / (max - min);
  return color;
}

void RGB_Blend(BlendMode blend_mode,
               const uint8_t* src_scan,
               const uint8_t* dest_scan,
               int results[3]) {
  RGB result = {0, 0, 0};
  RGB src;
  src.red = src_scan[2];
  src.green = src_scan[1];
  src.blue = src_scan[0];
  RGB back;
  back.red = dest_scan[2];
  back.green = dest_scan[1];
  back.blue = dest_scan[0];
  switch (blend_mode) {
    case BlendMode::kHue:
      result = SetLum(SetSat(src, Sat(back)), Lum(back));
      break;
    case BlendMode::kSaturation:
      result = SetLum(SetSat(back, Sat(src)), Lum(back));
      break;
    case BlendMode::kColor:
      result = SetLum(src, Lum(back));
      break;
    case BlendMode::kLuminosity:
      result = SetLum(back, Lum(src));
      break;
    default:
      break;
  }
  results[0] = result.blue;
  results[1] = result.green;
  results[2] = result.red;
}

int GetAlpha(uint8_t src_alpha, const uint8_t* clip_scan, int col) {
  return clip_scan ? clip_scan[col] * src_alpha / 255 : src_alpha;
}

void CompositeRow_AlphaToMask(uint8_t* dest_scan,
                              const uint8_t* src_scan,
                              int pixel_count,
                              const uint8_t* clip_scan,
                              uint8_t stride) {
  src_scan += stride - 1;
  for (int col = 0; col < pixel_count; ++col) {
    int src_alpha = GetAlpha(*src_scan, clip_scan, col);
    uint8_t back_alpha = *dest_scan;
    if (!back_alpha)
      *dest_scan = src_alpha;
    else if (src_alpha)
      *dest_scan = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    ++dest_scan;
    src_scan += stride;
  }
}

void CompositeRow_Rgb2Mask(uint8_t* dest_scan,
                           const uint8_t* src_scan,
                           int width,
                           const uint8_t* clip_scan) {
  if (!clip_scan) {
    memset(dest_scan, 0xff, width);
    return;
  }
  for (int i = 0; i < width; ++i) {
    *dest_scan = FXDIB_ALPHA_UNION(*dest_scan, *clip_scan);
    ++dest_scan;
    ++clip_scan;
  }
}

bool IsNonSeparableBlendMode(BlendMode mode) {
  switch (mode) {
    case BlendMode::kHue:
    case BlendMode::kSaturation:
    case BlendMode::kColor:
    case BlendMode::kLuminosity:
      return true;
    default:
      return false;
  }
}

uint8_t GetGray(const uint8_t* src_scan) {
  return FXRGB2GRAY(src_scan[2], src_scan[1], *src_scan);
}

uint8_t GetGrayWithBlend(const uint8_t* src_scan,
                         const uint8_t* dest_scan,
                         BlendMode blend_type) {
  uint8_t gray = GetGray(src_scan);
  if (IsNonSeparableBlendMode(blend_type))
    gray = blend_type == BlendMode::kLuminosity ? gray : *dest_scan;
  else if (blend_type != BlendMode::kNormal)
    gray = Blend(blend_type, *dest_scan, gray);
  return gray;
}

void CompositeRow_Argb2Graya(uint8_t* dest_scan,
                             const uint8_t* src_scan,
                             int pixel_count,
                             BlendMode blend_type,
                             const uint8_t* clip_scan,
                             const uint8_t* src_alpha_scan,
                             uint8_t* dst_alpha_scan) {
  uint8_t offset = src_alpha_scan ? 3 : 4;
  for (int col = 0; col < pixel_count; ++col) {
    const uint8_t* alpha_scan =
        src_alpha_scan ? src_alpha_scan++ : &src_scan[3];
    uint8_t back_alpha = *dst_alpha_scan;
    if (back_alpha == 0) {
      int src_alpha = GetAlpha(*alpha_scan, clip_scan, col);
      if (src_alpha) {
        *dest_scan = GetGray(src_scan);
        *dst_alpha_scan = src_alpha;
      }
      ++dest_scan;
      ++dst_alpha_scan;
      src_scan += offset;
      continue;
    }
    uint8_t src_alpha = GetAlpha(*alpha_scan, clip_scan, col);
    if (src_alpha == 0) {
      ++dest_scan;
      ++dst_alpha_scan;
      src_scan += offset;
      continue;
    }
    *dst_alpha_scan = FXDIB_ALPHA_UNION(back_alpha, src_alpha);
    int alpha_ratio = src_alpha * 255 / (*dst_alpha_scan);
    uint8_t gray = GetGray(src_scan);
    // TODO(npm): Does this if really need src_alpha_scan or was that a bug?
    if (blend_type != BlendMode::kNormal && src_alpha_scan) {
      if (IsNonSeparableBlendMode(blend_type))
        gray = blend_type == BlendMode::kLuminosity ? gray : *dest_scan;
      else
        gray = Blend(blend_type, *dest_scan, gray);
    }
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, alpha_ratio);
    ++dest_scan;
    ++dst_alpha_scan;
    src_scan += offset;
  }
}

void CompositeRow_Argb2Gray(uint8_t* dest_scan,
                            const uint8_t* src_scan,
                            int pixel_count,
                            BlendMode blend_type,
                            const uint8_t* clip_scan,
                            const uint8_t* src_alpha_scan) {
  uint8_t gray;
  uint8_t offset = src_alpha_scan ? 3 : 4;
  for (int col = 0; col < pixel_count; ++col) {
    const uint8_t* alpha_scan =
        src_alpha_scan ? src_alpha_scan++ : &src_scan[3];
    int src_alpha = GetAlpha(*alpha_scan, clip_scan, col);
    if (src_alpha) {
      gray = GetGrayWithBlend(src_scan, dest_scan, blend_type);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, src_alpha);
    }
    ++dest_scan;
    src_scan += offset;
  }
}

void CompositeRow_Rgb2Gray(uint8_t* dest_scan,
                           const uint8_t* src_scan,
                           int src_Bpp,
                           int pixel_count,
                           BlendMode blend_type,
                           const uint8_t* clip_scan) {
  uint8_t gray;
  for (int col = 0; col < pixel_count; ++col) {
    gray = GetGrayWithBlend(src_scan, dest_scan, blend_type);
    if (clip_scan && clip_scan[col] < 255)
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, clip_scan[col]);
    else
      *dest_scan = gray;
    ++dest_scan;
    src_scan += src_Bpp;
  }
}

void CompositeRow_Rgb2Graya(uint8_t* dest_scan,
                            const uint8_t* src_scan,
                            int src_Bpp,
                            int pixel_count,
                            BlendMode blend_type,
                            const uint8_t* clip_scan,
                            uint8_t* dest_alpha_scan) {
  for (int col = 0; col < pixel_count; ++col) {
    if (blend_type != BlendMode::kNormal && *dest_alpha_scan == 0) {
      *dest_scan = GetGray(src_scan);
      ++dest_scan;
      ++dest_alpha_scan;
      src_scan += src_Bpp;
      continue;
    }
    int src_alpha = clip_scan ? clip_scan[col] : 255;
    if (src_alpha == 255) {
      *dest_scan = GetGrayWithBlend(src_scan, dest_scan, blend_type);
      ++dest_scan;
      *dest_alpha_scan = 255;
      ++dest_alpha_scan;
      src_scan += src_Bpp;
      continue;
    }
    if (src_alpha == 0) {
      ++dest_scan;
      ++dest_alpha_scan;
      src_scan += src_Bpp;
      continue;
    }
    int back_alpha = *dest_alpha_scan;
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    *dest_alpha_scan = dest_alpha;
    ++dest_alpha_scan;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    uint8_t gray = GetGrayWithBlend(src_scan, dest_scan, blend_type);
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, alpha_ratio);
    ++dest_scan;
    src_scan += src_Bpp;
  }
}

void CompositeRow_Argb2Argb(uint8_t* dest_scan,
                            const uint8_t* src_scan,
                            int pixel_count,
                            BlendMode blend_type,
                            const uint8_t* clip_scan,
                            uint8_t* dest_alpha_scan,
                            const uint8_t* src_alpha_scan) {
  int blended_colors[3];
  uint8_t dest_offset = dest_alpha_scan ? 3 : 4;
  uint8_t src_offset = src_alpha_scan ? 3 : 4;
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  bool has_src = !!src_alpha_scan;
  bool has_dest = !!dest_alpha_scan;
  for (int col = 0; col < pixel_count; ++col) {
    uint8_t back_alpha = has_dest ? *dest_alpha_scan : dest_scan[3];
    const uint8_t* alpha_source = has_src ? src_alpha_scan++ : &src_scan[3];
    uint8_t src_alpha = GetAlpha(*alpha_source, clip_scan, col);
    if (back_alpha == 0) {
      if (!has_dest && !has_src) {
        if (clip_scan) {
          FXARGB_SETDIB(dest_scan, (FXARGB_GETDIB(src_scan) & 0xffffff) |
                                       (src_alpha << 24));
        } else {
          FXARGB_COPY(dest_scan, src_scan);
        }
      } else if (has_dest) {
        *dest_alpha_scan = src_alpha;
        for (int i = 0; i < 3; ++i) {
          *dest_scan = *src_scan++;
          ++dest_scan;
        }
        ++dest_alpha_scan;
        if (!has_src)
          ++src_scan;
      } else {
        FXARGB_SETDIB(dest_scan, ArgbEncode((src_alpha << 24), src_scan[2],
                                            src_scan[1], *src_scan));
      }
      if (!has_dest) {
        dest_scan += dest_offset;
        src_scan += src_offset;
      }
      continue;
    }
    if (src_alpha == 0) {
      dest_scan += dest_offset;
      src_scan += src_offset;
      if (has_dest)
        ++dest_alpha_scan;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    if (has_dest) {
      *dest_alpha_scan = dest_alpha;
      ++dest_alpha_scan;
    } else {
      dest_scan[3] = dest_alpha;
    }
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (bNonseparableBlend)
      RGB_Blend(blend_type, src_scan, dest_scan, blended_colors);
    for (int color = 0; color < 3; ++color) {
      if (blend_type != BlendMode::kNormal) {
        int blended = bNonseparableBlend
                          ? blended_colors[color]
                          : Blend(blend_type, *dest_scan, *src_scan);
        blended = FXDIB_ALPHA_MERGE(*src_scan, blended, back_alpha);
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      } else {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, *src_scan, alpha_ratio);
      }
      ++dest_scan;
      ++src_scan;
    }
    if (!has_dest)
      ++dest_scan;
    if (!has_src)
      ++src_scan;
  }
}

void CompositeRow_Rgb2Argb_Blend_NoClip(uint8_t* dest_scan,
                                        const uint8_t* src_scan,
                                        int width,
                                        BlendMode blend_type,
                                        int src_Bpp,
                                        uint8_t* dest_alpha_scan) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int src_gap = src_Bpp - 3;
  for (int col = 0; col < width; ++col) {
    uint8_t* dest_alpha = dest_alpha_scan ? dest_alpha_scan : &dest_scan[3];
    uint8_t back_alpha = *dest_alpha;
    if (back_alpha == 0) {
      if (dest_alpha_scan) {
        for (int i = 0; i < 3; ++i) {
          *dest_scan = *src_scan++;
          ++dest_scan;
        }
        *dest_alpha_scan = 0xff;
        ++dest_alpha_scan;
      } else {
        if (src_Bpp == 4) {
          FXARGB_SETDIB(dest_scan, 0xff000000 | FXARGB_GETDIB(src_scan));
        } else {
          FXARGB_SETDIB(dest_scan, ArgbEncode(0xff, src_scan[2], src_scan[1],
                                              src_scan[0]));
        }
        dest_scan += 4;
      }
      src_scan += src_Bpp;
      continue;
    }
    *dest_alpha = 0xff;
    if (bNonseparableBlend)
      RGB_Blend(blend_type, src_scan, dest_scan, blended_colors);
    for (int color = 0; color < 3; ++color) {
      int src_color = *src_scan;
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, *dest_scan, src_color);
      *dest_scan = FXDIB_ALPHA_MERGE(src_color, blended, back_alpha);
      ++dest_scan;
      ++src_scan;
    }
    if (dest_alpha_scan)
      ++dest_alpha_scan;
    else
      ++dest_scan;
    src_scan += src_gap;
  }
}

void CompositeRow_Rgb2Argb_Blend_Clip(uint8_t* dest_scan,
                                      const uint8_t* src_scan,
                                      int width,
                                      BlendMode blend_type,
                                      int src_Bpp,
                                      const uint8_t* clip_scan,
                                      uint8_t* dest_alpha_scan) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int src_gap = src_Bpp - 3;
  bool has_dest = !!dest_alpha_scan;
  for (int col = 0; col < width; ++col) {
    int src_alpha = *clip_scan++;
    uint8_t back_alpha = has_dest ? *dest_alpha_scan : dest_scan[3];
    if (back_alpha == 0) {
      for (int i = 0; i < 3; ++i) {
        *dest_scan = *src_scan++;
        ++dest_scan;
      }
      src_scan += src_gap;
      if (has_dest)
        dest_alpha_scan++;
      else
        dest_scan++;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan += has_dest ? 3 : 4;
      if (has_dest)
        dest_alpha_scan++;
      src_scan += src_Bpp;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    if (has_dest)
      *dest_alpha_scan++ = dest_alpha;
    else
      dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (bNonseparableBlend)
      RGB_Blend(blend_type, src_scan, dest_scan, blended_colors);
    for (int color = 0; color < 3; color++) {
      int src_color = *src_scan;
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, *dest_scan, src_color);
      blended = FXDIB_ALPHA_MERGE(src_color, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      dest_scan++;
      src_scan++;
    }
    src_scan += src_gap;
    if (!has_dest)
      dest_scan++;
  }
}

void CompositeRow_Rgb2Argb_NoBlend_Clip(uint8_t* dest_scan,
                                        const uint8_t* src_scan,
                                        int width,
                                        int src_Bpp,
                                        const uint8_t* clip_scan,
                                        uint8_t* dest_alpha_scan) {
  int src_gap = src_Bpp - 3;
  if (dest_alpha_scan) {
    for (int col = 0; col < width; col++) {
      int src_alpha = clip_scan[col];
      if (src_alpha == 255) {
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        *dest_alpha_scan++ = 255;
        src_scan += src_gap;
        continue;
      }
      if (src_alpha == 0) {
        dest_scan += 3;
        dest_alpha_scan++;
        src_scan += src_Bpp;
        continue;
      }
      int back_alpha = *dest_alpha_scan;
      uint8_t dest_alpha =
          back_alpha + src_alpha - back_alpha * src_alpha / 255;
      *dest_alpha_scan++ = dest_alpha;
      int alpha_ratio = src_alpha * 255 / dest_alpha;
      for (int color = 0; color < 3; color++) {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, *src_scan, alpha_ratio);
        dest_scan++;
        src_scan++;
      }
      src_scan += src_gap;
    }
  } else {
    for (int col = 0; col < width; col++) {
      int src_alpha = clip_scan[col];
      if (src_alpha == 255) {
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        *dest_scan++ = 255;
        src_scan += src_gap;
        continue;
      }
      if (src_alpha == 0) {
        dest_scan += 4;
        src_scan += src_Bpp;
        continue;
      }
      int back_alpha = dest_scan[3];
      uint8_t dest_alpha =
          back_alpha + src_alpha - back_alpha * src_alpha / 255;
      dest_scan[3] = dest_alpha;
      int alpha_ratio = src_alpha * 255 / dest_alpha;
      for (int color = 0; color < 3; color++) {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, *src_scan, alpha_ratio);
        dest_scan++;
        src_scan++;
      }
      dest_scan++;
      src_scan += src_gap;
    }
  }
}

void CompositeRow_Rgb2Argb_NoBlend_NoClip(uint8_t* dest_scan,
                                          const uint8_t* src_scan,
                                          int width,
                                          int src_Bpp,
                                          uint8_t* dest_alpha_scan) {
  if (dest_alpha_scan) {
    int src_gap = src_Bpp - 3;
    for (int col = 0; col < width; col++) {
      *dest_scan++ = *src_scan++;
      *dest_scan++ = *src_scan++;
      *dest_scan++ = *src_scan++;
      *dest_alpha_scan++ = 0xff;
      src_scan += src_gap;
    }
  } else {
    for (int col = 0; col < width; col++) {
      if (src_Bpp == 4) {
        FXARGB_SETDIB(dest_scan, 0xff000000 | FXARGB_GETDIB(src_scan));
      } else {
        FXARGB_SETDIB(dest_scan,
                      ArgbEncode(0xff, src_scan[2], src_scan[1], src_scan[0]));
      }
      dest_scan += 4;
      src_scan += src_Bpp;
    }
  }
}

void CompositeRow_Argb2Rgb_Blend(uint8_t* dest_scan,
                                 const uint8_t* src_scan,
                                 int width,
                                 BlendMode blend_type,
                                 int dest_Bpp,
                                 const uint8_t* clip_scan,
                                 const uint8_t* src_alpha_scan) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int dest_gap = dest_Bpp - 3;
  if (src_alpha_scan) {
    for (int col = 0; col < width; col++) {
      uint8_t src_alpha;
      if (clip_scan) {
        src_alpha = (*src_alpha_scan++) * (*clip_scan++) / 255;
      } else {
        src_alpha = *src_alpha_scan++;
      }
      if (src_alpha == 0) {
        dest_scan += dest_Bpp;
        src_scan += 3;
        continue;
      }
      if (bNonseparableBlend) {
        RGB_Blend(blend_type, src_scan, dest_scan, blended_colors);
      }
      for (int color = 0; color < 3; color++) {
        int back_color = *dest_scan;
        int blended = bNonseparableBlend
                          ? blended_colors[color]
                          : Blend(blend_type, back_color, *src_scan);
        *dest_scan = FXDIB_ALPHA_MERGE(back_color, blended, src_alpha);
        dest_scan++;
        src_scan++;
      }
      dest_scan += dest_gap;
    }
  } else {
    for (int col = 0; col < width; col++) {
      uint8_t src_alpha;
      if (clip_scan) {
        src_alpha = src_scan[3] * (*clip_scan++) / 255;
      } else {
        src_alpha = src_scan[3];
      }
      if (src_alpha == 0) {
        dest_scan += dest_Bpp;
        src_scan += 4;
        continue;
      }
      if (bNonseparableBlend) {
        RGB_Blend(blend_type, src_scan, dest_scan, blended_colors);
      }
      for (int color = 0; color < 3; color++) {
        int back_color = *dest_scan;
        int blended = bNonseparableBlend
                          ? blended_colors[color]
                          : Blend(blend_type, back_color, *src_scan);
        *dest_scan = FXDIB_ALPHA_MERGE(back_color, blended, src_alpha);
        dest_scan++;
        src_scan++;
      }
      dest_scan += dest_gap;
      src_scan++;
    }
  }
}

void CompositeRow_Argb2Rgb_NoBlend(uint8_t* dest_scan,
                                   const uint8_t* src_scan,
                                   int width,
                                   int dest_Bpp,
                                   const uint8_t* clip_scan,
                                   const uint8_t* src_alpha_scan) {
  int dest_gap = dest_Bpp - 3;
  if (src_alpha_scan) {
    for (int col = 0; col < width; col++) {
      uint8_t src_alpha;
      if (clip_scan) {
        src_alpha = (*src_alpha_scan++) * (*clip_scan++) / 255;
      } else {
        src_alpha = *src_alpha_scan++;
      }
      if (src_alpha == 255) {
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        dest_scan += dest_gap;
        continue;
      }
      if (src_alpha == 0) {
        dest_scan += dest_Bpp;
        src_scan += 3;
        continue;
      }
      for (int color = 0; color < 3; color++) {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, *src_scan, src_alpha);
        dest_scan++;
        src_scan++;
      }
      dest_scan += dest_gap;
    }
  } else {
    for (int col = 0; col < width; col++) {
      uint8_t src_alpha;
      if (clip_scan) {
        src_alpha = src_scan[3] * (*clip_scan++) / 255;
      } else {
        src_alpha = src_scan[3];
      }
      if (src_alpha == 255) {
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        dest_scan += dest_gap;
        src_scan++;
        continue;
      }
      if (src_alpha == 0) {
        dest_scan += dest_Bpp;
        src_scan += 4;
        continue;
      }
      for (int color = 0; color < 3; color++) {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, *src_scan, src_alpha);
        dest_scan++;
        src_scan++;
      }
      dest_scan += dest_gap;
      src_scan++;
    }
  }
}

void CompositeRow_Rgb2Rgb_Blend_NoClip(uint8_t* dest_scan,
                                       const uint8_t* src_scan,
                                       int width,
                                       BlendMode blend_type,
                                       int dest_Bpp,
                                       int src_Bpp) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int dest_gap = dest_Bpp - 3;
  int src_gap = src_Bpp - 3;
  for (int col = 0; col < width; col++) {
    if (bNonseparableBlend) {
      RGB_Blend(blend_type, src_scan, dest_scan, blended_colors);
    }
    for (int color = 0; color < 3; color++) {
      int back_color = *dest_scan;
      int src_color = *src_scan;
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, back_color, src_color);
      *dest_scan = blended;
      dest_scan++;
      src_scan++;
    }
    dest_scan += dest_gap;
    src_scan += src_gap;
  }
}

void CompositeRow_Rgb2Rgb_Blend_Clip(uint8_t* dest_scan,
                                     const uint8_t* src_scan,
                                     int width,
                                     BlendMode blend_type,
                                     int dest_Bpp,
                                     int src_Bpp,
                                     const uint8_t* clip_scan) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int dest_gap = dest_Bpp - 3;
  int src_gap = src_Bpp - 3;
  for (int col = 0; col < width; col++) {
    uint8_t src_alpha = *clip_scan++;
    if (src_alpha == 0) {
      dest_scan += dest_Bpp;
      src_scan += src_Bpp;
      continue;
    }
    if (bNonseparableBlend) {
      RGB_Blend(blend_type, src_scan, dest_scan, blended_colors);
    }
    for (int color = 0; color < 3; color++) {
      int src_color = *src_scan;
      int back_color = *dest_scan;
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, back_color, src_color);
      *dest_scan = FXDIB_ALPHA_MERGE(back_color, blended, src_alpha);
      dest_scan++;
      src_scan++;
    }
    dest_scan += dest_gap;
    src_scan += src_gap;
  }
}

void CompositeRow_Rgb2Rgb_NoBlend_NoClip(uint8_t* dest_scan,
                                         const uint8_t* src_scan,
                                         int width,
                                         int dest_Bpp,
                                         int src_Bpp) {
  if (dest_Bpp == src_Bpp) {
    memcpy(dest_scan, src_scan, width * dest_Bpp);
    return;
  }
  for (int col = 0; col < width; col++) {
    dest_scan[0] = src_scan[0];
    dest_scan[1] = src_scan[1];
    dest_scan[2] = src_scan[2];
    dest_scan += dest_Bpp;
    src_scan += src_Bpp;
  }
}

void CompositeRow_Rgb2Rgb_NoBlend_Clip(uint8_t* dest_scan,
                                       const uint8_t* src_scan,
                                       int width,
                                       int dest_Bpp,
                                       int src_Bpp,
                                       const uint8_t* clip_scan) {
  for (int col = 0; col < width; col++) {
    int src_alpha = clip_scan[col];
    if (src_alpha == 255) {
      dest_scan[0] = src_scan[0];
      dest_scan[1] = src_scan[1];
      dest_scan[2] = src_scan[2];
    } else if (src_alpha) {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, *src_scan, src_alpha);
      dest_scan++;
      src_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, *src_scan, src_alpha);
      dest_scan++;
      src_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, *src_scan, src_alpha);
      dest_scan += dest_Bpp - 2;
      src_scan += src_Bpp - 2;
      continue;
    }
    dest_scan += dest_Bpp;
    src_scan += src_Bpp;
  }
}

void CompositeRow_8bppPal2Gray(uint8_t* dest_scan,
                               const uint8_t* src_scan,
                               const uint8_t* pPalette,
                               int pixel_count,
                               BlendMode blend_type,
                               const uint8_t* clip_scan,
                               const uint8_t* src_alpha_scan) {
  if (src_alpha_scan) {
    if (blend_type != BlendMode::kNormal) {
      bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
      for (int col = 0; col < pixel_count; col++) {
        uint8_t gray = pPalette[*src_scan];
        int src_alpha = GetAlpha(*src_alpha_scan++, clip_scan, col);
        if (bNonseparableBlend)
          gray = blend_type == BlendMode::kLuminosity ? gray : *dest_scan;
        else
          gray = Blend(blend_type, *dest_scan, gray);
        if (src_alpha)
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, src_alpha);
        else
          *dest_scan = gray;
        dest_scan++;
        src_scan++;
      }
      return;
    }
    for (int col = 0; col < pixel_count; col++) {
      uint8_t gray = pPalette[*src_scan];
      int src_alpha = GetAlpha(*src_alpha_scan++, clip_scan, col);
      if (src_alpha)
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, src_alpha);
      else
        *dest_scan = gray;
      dest_scan++;
      src_scan++;
    }
  } else {
    if (blend_type != BlendMode::kNormal) {
      bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
      for (int col = 0; col < pixel_count; col++) {
        uint8_t gray = pPalette[*src_scan];
        if (bNonseparableBlend)
          gray = blend_type == BlendMode::kLuminosity ? gray : *dest_scan;
        else
          gray = Blend(blend_type, *dest_scan, gray);
        if (clip_scan && clip_scan[col] < 255)
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, clip_scan[col]);
        else
          *dest_scan = gray;
        dest_scan++;
        src_scan++;
      }
      return;
    }
    for (int col = 0; col < pixel_count; col++) {
      uint8_t gray = pPalette[*src_scan];
      if (clip_scan && clip_scan[col] < 255)
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, clip_scan[col]);
      else
        *dest_scan = gray;
      dest_scan++;
      src_scan++;
    }
  }
}

void CompositeRow_8bppPal2Graya(uint8_t* dest_scan,
                                const uint8_t* src_scan,
                                const uint8_t* pPalette,
                                int pixel_count,
                                BlendMode blend_type,
                                const uint8_t* clip_scan,
                                uint8_t* dest_alpha_scan,
                                const uint8_t* src_alpha_scan) {
  if (src_alpha_scan) {
    if (blend_type != BlendMode::kNormal) {
      bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
      for (int col = 0; col < pixel_count; col++) {
        uint8_t gray = pPalette[*src_scan];
        src_scan++;
        uint8_t back_alpha = *dest_alpha_scan;
        if (back_alpha == 0) {
          int src_alpha = GetAlpha(*src_alpha_scan++, clip_scan, col);
          if (src_alpha) {
            *dest_scan = gray;
            *dest_alpha_scan = src_alpha;
          }
          dest_scan++;
          dest_alpha_scan++;
          continue;
        }
        uint8_t src_alpha = GetAlpha(*src_alpha_scan++, clip_scan, col);
        if (src_alpha == 0) {
          dest_scan++;
          dest_alpha_scan++;
          continue;
        }
        *dest_alpha_scan =
            back_alpha + src_alpha - back_alpha * src_alpha / 255;
        int alpha_ratio = src_alpha * 255 / (*dest_alpha_scan);
        if (bNonseparableBlend)
          gray = blend_type == BlendMode::kLuminosity ? gray : *dest_scan;
        else
          gray = Blend(blend_type, *dest_scan, gray);
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, alpha_ratio);
        dest_alpha_scan++;
        dest_scan++;
      }
      return;
    }
    for (int col = 0; col < pixel_count; col++) {
      uint8_t gray = pPalette[*src_scan];
      src_scan++;
      uint8_t back_alpha = *dest_alpha_scan;
      if (back_alpha == 0) {
        int src_alpha = GetAlpha(*src_alpha_scan++, clip_scan, col);
        if (src_alpha) {
          *dest_scan = gray;
          *dest_alpha_scan = src_alpha;
        }
        dest_scan++;
        dest_alpha_scan++;
        continue;
      }
      uint8_t src_alpha = GetAlpha(*src_alpha_scan++, clip_scan, col);
      if (src_alpha == 0) {
        dest_scan++;
        dest_alpha_scan++;
        continue;
      }
      *dest_alpha_scan = back_alpha + src_alpha - back_alpha * src_alpha / 255;
      int alpha_ratio = src_alpha * 255 / (*dest_alpha_scan);
      dest_alpha_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, alpha_ratio);
      dest_scan++;
    }
  } else {
    if (blend_type != BlendMode::kNormal) {
      bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
      for (int col = 0; col < pixel_count; col++) {
        uint8_t gray = pPalette[*src_scan];
        src_scan++;
        if (!clip_scan || clip_scan[col] == 255) {
          *dest_scan++ = gray;
          *dest_alpha_scan++ = 255;
          continue;
        }
        int src_alpha = clip_scan[col];
        if (src_alpha == 0) {
          dest_scan++;
          dest_alpha_scan++;
          continue;
        }
        int back_alpha = *dest_alpha_scan;
        uint8_t dest_alpha =
            back_alpha + src_alpha - back_alpha * src_alpha / 255;
        *dest_alpha_scan++ = dest_alpha;
        int alpha_ratio = src_alpha * 255 / dest_alpha;
        if (bNonseparableBlend)
          gray = blend_type == BlendMode::kLuminosity ? gray : *dest_scan;
        else
          gray = Blend(blend_type, *dest_scan, gray);
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, alpha_ratio);
        dest_scan++;
      }
      return;
    }
    for (int col = 0; col < pixel_count; col++) {
      uint8_t gray = pPalette[*src_scan];
      src_scan++;
      if (!clip_scan || clip_scan[col] == 255) {
        *dest_scan++ = gray;
        *dest_alpha_scan++ = 255;
        continue;
      }
      int src_alpha = clip_scan[col];
      if (src_alpha == 0) {
        dest_scan++;
        dest_alpha_scan++;
        continue;
      }
      int back_alpha = *dest_alpha_scan;
      uint8_t dest_alpha =
          back_alpha + src_alpha - back_alpha * src_alpha / 255;
      *dest_alpha_scan++ = dest_alpha;
      int alpha_ratio = src_alpha * 255 / dest_alpha;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, alpha_ratio);
      dest_scan++;
    }
  }
}

void CompositeRow_1bppPal2Gray(uint8_t* dest_scan,
                               const uint8_t* src_scan,
                               int src_left,
                               const uint8_t* pPalette,
                               int pixel_count,
                               BlendMode blend_type,
                               const uint8_t* clip_scan) {
  int reset_gray = pPalette[0];
  int set_gray = pPalette[1];
  if (blend_type != BlendMode::kNormal) {
    bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
    for (int col = 0; col < pixel_count; col++) {
      uint8_t gray =
          (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8)))
              ? set_gray
              : reset_gray;
      if (bNonseparableBlend)
        gray = blend_type == BlendMode::kLuminosity ? gray : *dest_scan;
      else
        gray = Blend(blend_type, *dest_scan, gray);
      if (clip_scan && clip_scan[col] < 255) {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, clip_scan[col]);
      } else {
        *dest_scan = gray;
      }
      dest_scan++;
    }
    return;
  }
  for (int col = 0; col < pixel_count; col++) {
    uint8_t gray =
        (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8)))
            ? set_gray
            : reset_gray;
    if (clip_scan && clip_scan[col] < 255) {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, clip_scan[col]);
    } else {
      *dest_scan = gray;
    }
    dest_scan++;
  }
}

void CompositeRow_1bppPal2Graya(uint8_t* dest_scan,
                                const uint8_t* src_scan,
                                int src_left,
                                const uint8_t* pPalette,
                                int pixel_count,
                                BlendMode blend_type,
                                const uint8_t* clip_scan,
                                uint8_t* dest_alpha_scan) {
  int reset_gray = pPalette[0];
  int set_gray = pPalette[1];
  if (blend_type != BlendMode::kNormal) {
    bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
    for (int col = 0; col < pixel_count; col++) {
      uint8_t gray =
          (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8)))
              ? set_gray
              : reset_gray;
      if (!clip_scan || clip_scan[col] == 255) {
        *dest_scan++ = gray;
        *dest_alpha_scan++ = 255;
        continue;
      }
      int src_alpha = clip_scan[col];
      if (src_alpha == 0) {
        dest_scan++;
        dest_alpha_scan++;
        continue;
      }
      int back_alpha = *dest_alpha_scan;
      uint8_t dest_alpha =
          back_alpha + src_alpha - back_alpha * src_alpha / 255;
      *dest_alpha_scan++ = dest_alpha;
      int alpha_ratio = src_alpha * 255 / dest_alpha;
      if (bNonseparableBlend)
        gray = blend_type == BlendMode::kLuminosity ? gray : *dest_scan;
      else
        gray = Blend(blend_type, *dest_scan, gray);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, alpha_ratio);
      dest_scan++;
    }
    return;
  }
  for (int col = 0; col < pixel_count; col++) {
    uint8_t gray =
        (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8)))
            ? set_gray
            : reset_gray;
    if (!clip_scan || clip_scan[col] == 255) {
      *dest_scan++ = gray;
      *dest_alpha_scan++ = 255;
      continue;
    }
    int src_alpha = clip_scan[col];
    if (src_alpha == 0) {
      dest_scan++;
      dest_alpha_scan++;
      continue;
    }
    int back_alpha = *dest_alpha_scan;
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    *dest_alpha_scan++ = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, alpha_ratio);
    dest_scan++;
  }
}

void CompositeRow_8bppRgb2Rgb_NoBlend(uint8_t* dest_scan,
                                      const uint8_t* src_scan,
                                      uint32_t* pPalette,
                                      int pixel_count,
                                      int DestBpp,
                                      const uint8_t* clip_scan,
                                      const uint8_t* src_alpha_scan) {
  if (src_alpha_scan) {
    int dest_gap = DestBpp - 3;
    FX_ARGB argb = 0;
    for (int col = 0; col < pixel_count; col++) {
      argb = pPalette[*src_scan];
      int src_r = FXARGB_R(argb);
      int src_g = FXARGB_G(argb);
      int src_b = FXARGB_B(argb);
      src_scan++;
      uint8_t src_alpha = 0;
      if (clip_scan) {
        src_alpha = (*src_alpha_scan++) * (*clip_scan++) / 255;
      } else {
        src_alpha = *src_alpha_scan++;
      }
      if (src_alpha == 255) {
        *dest_scan++ = src_b;
        *dest_scan++ = src_g;
        *dest_scan++ = src_r;
        dest_scan += dest_gap;
        continue;
      }
      if (src_alpha == 0) {
        dest_scan += DestBpp;
        continue;
      }
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, src_alpha);
      dest_scan++;
      dest_scan += dest_gap;
    }
  } else {
    FX_ARGB argb = 0;
    for (int col = 0; col < pixel_count; col++) {
      argb = pPalette[*src_scan];
      int src_r = FXARGB_R(argb);
      int src_g = FXARGB_G(argb);
      int src_b = FXARGB_B(argb);
      if (clip_scan && clip_scan[col] < 255) {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, clip_scan[col]);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, clip_scan[col]);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, clip_scan[col]);
        dest_scan++;
      } else {
        *dest_scan++ = src_b;
        *dest_scan++ = src_g;
        *dest_scan++ = src_r;
      }
      if (DestBpp == 4) {
        dest_scan++;
      }
      src_scan++;
    }
  }
}

void CompositeRow_1bppRgb2Rgb_NoBlend(uint8_t* dest_scan,
                                      const uint8_t* src_scan,
                                      int src_left,
                                      uint32_t* pPalette,
                                      int pixel_count,
                                      int DestBpp,
                                      const uint8_t* clip_scan) {
  int reset_r, reset_g, reset_b;
  int set_r, set_g, set_b;
  reset_r = FXARGB_R(pPalette[0]);
  reset_g = FXARGB_G(pPalette[0]);
  reset_b = FXARGB_B(pPalette[0]);
  set_r = FXARGB_R(pPalette[1]);
  set_g = FXARGB_G(pPalette[1]);
  set_b = FXARGB_B(pPalette[1]);
  for (int col = 0; col < pixel_count; col++) {
    int src_r, src_g, src_b;
    if (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8))) {
      src_r = set_r;
      src_g = set_g;
      src_b = set_b;
    } else {
      src_r = reset_r;
      src_g = reset_g;
      src_b = reset_b;
    }
    if (clip_scan && clip_scan[col] < 255) {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, clip_scan[col]);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, clip_scan[col]);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, clip_scan[col]);
      dest_scan++;
    } else {
      *dest_scan++ = src_b;
      *dest_scan++ = src_g;
      *dest_scan++ = src_r;
    }
    if (DestBpp == 4) {
      dest_scan++;
    }
  }
}

void CompositeRow_8bppRgb2Argb_NoBlend(uint8_t* dest_scan,
                                       const uint8_t* src_scan,
                                       int width,
                                       uint32_t* pPalette,
                                       const uint8_t* clip_scan,
                                       const uint8_t* src_alpha_scan) {
  if (src_alpha_scan) {
    for (int col = 0; col < width; col++) {
      FX_ARGB argb = pPalette[*src_scan];
      src_scan++;
      int src_r = FXARGB_R(argb);
      int src_g = FXARGB_G(argb);
      int src_b = FXARGB_B(argb);
      uint8_t back_alpha = dest_scan[3];
      if (back_alpha == 0) {
        if (clip_scan) {
          int src_alpha = clip_scan[col] * (*src_alpha_scan) / 255;
          FXARGB_SETDIB(dest_scan, ArgbEncode(src_alpha, src_r, src_g, src_b));
        } else {
          FXARGB_SETDIB(dest_scan,
                        ArgbEncode(*src_alpha_scan, src_r, src_g, src_b));
        }
        dest_scan += 4;
        src_alpha_scan++;
        continue;
      }
      uint8_t src_alpha;
      if (clip_scan) {
        src_alpha = clip_scan[col] * (*src_alpha_scan++) / 255;
      } else {
        src_alpha = *src_alpha_scan++;
      }
      if (src_alpha == 0) {
        dest_scan += 4;
        continue;
      }
      uint8_t dest_alpha =
          back_alpha + src_alpha - back_alpha * src_alpha / 255;
      dest_scan[3] = dest_alpha;
      int alpha_ratio = src_alpha * 255 / dest_alpha;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, alpha_ratio);
      dest_scan++;
      dest_scan++;
    }
  } else {
    for (int col = 0; col < width; col++) {
      FX_ARGB argb = pPalette[*src_scan];
      int src_r = FXARGB_R(argb);
      int src_g = FXARGB_G(argb);
      int src_b = FXARGB_B(argb);
      if (!clip_scan || clip_scan[col] == 255) {
        *dest_scan++ = src_b;
        *dest_scan++ = src_g;
        *dest_scan++ = src_r;
        *dest_scan++ = 255;
        src_scan++;
        continue;
      }
      int src_alpha = clip_scan[col];
      if (src_alpha == 0) {
        dest_scan += 4;
        src_scan++;
        continue;
      }
      int back_alpha = dest_scan[3];
      uint8_t dest_alpha =
          back_alpha + src_alpha - back_alpha * src_alpha / 255;
      dest_scan[3] = dest_alpha;
      int alpha_ratio = src_alpha * 255 / dest_alpha;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, alpha_ratio);
      dest_scan++;
      dest_scan++;
      src_scan++;
    }
  }
}

void CompositeRow_1bppRgb2Argb_NoBlend(uint8_t* dest_scan,
                                       const uint8_t* src_scan,
                                       int src_left,
                                       int width,
                                       uint32_t* pPalette,
                                       const uint8_t* clip_scan) {
  int reset_r, reset_g, reset_b;
  int set_r, set_g, set_b;
  reset_r = FXARGB_R(pPalette[0]);
  reset_g = FXARGB_G(pPalette[0]);
  reset_b = FXARGB_B(pPalette[0]);
  set_r = FXARGB_R(pPalette[1]);
  set_g = FXARGB_G(pPalette[1]);
  set_b = FXARGB_B(pPalette[1]);
  for (int col = 0; col < width; col++) {
    int src_r, src_g, src_b;
    if (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8))) {
      src_r = set_r;
      src_g = set_g;
      src_b = set_b;
    } else {
      src_r = reset_r;
      src_g = reset_g;
      src_b = reset_b;
    }
    if (!clip_scan || clip_scan[col] == 255) {
      *dest_scan++ = src_b;
      *dest_scan++ = src_g;
      *dest_scan++ = src_r;
      *dest_scan++ = 255;
      continue;
    }
    int src_alpha = clip_scan[col];
    if (src_alpha == 0) {
      dest_scan += 4;
      continue;
    }
    int back_alpha = dest_scan[3];
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, alpha_ratio);
    dest_scan++;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, alpha_ratio);
    dest_scan++;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, alpha_ratio);
    dest_scan++;
    dest_scan++;
  }
}

void CompositeRow_1bppRgb2Rgba_NoBlend(uint8_t* dest_scan,
                                       const uint8_t* src_scan,
                                       int src_left,
                                       int width,
                                       uint32_t* pPalette,
                                       const uint8_t* clip_scan,
                                       uint8_t* dest_alpha_scan) {
  int reset_r, reset_g, reset_b;
  int set_r, set_g, set_b;
  reset_r = FXARGB_R(pPalette[0]);
  reset_g = FXARGB_G(pPalette[0]);
  reset_b = FXARGB_B(pPalette[0]);
  set_r = FXARGB_R(pPalette[1]);
  set_g = FXARGB_G(pPalette[1]);
  set_b = FXARGB_B(pPalette[1]);
  for (int col = 0; col < width; col++) {
    int src_r, src_g, src_b;
    if (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8))) {
      src_r = set_r;
      src_g = set_g;
      src_b = set_b;
    } else {
      src_r = reset_r;
      src_g = reset_g;
      src_b = reset_b;
    }
    if (!clip_scan || clip_scan[col] == 255) {
      *dest_scan++ = src_b;
      *dest_scan++ = src_g;
      *dest_scan++ = src_r;
      *dest_alpha_scan++ = 255;
      continue;
    }
    int src_alpha = clip_scan[col];
    if (src_alpha == 0) {
      dest_scan += 3;
      dest_alpha_scan++;
      continue;
    }
    int back_alpha = *dest_alpha_scan;
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    *dest_alpha_scan++ = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, alpha_ratio);
    dest_scan++;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, alpha_ratio);
    dest_scan++;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, alpha_ratio);
    dest_scan++;
  }
}

void CompositeRow_ByteMask2Argb(uint8_t* dest_scan,
                                const uint8_t* src_scan,
                                int mask_alpha,
                                int src_r,
                                int src_g,
                                int src_b,
                                int pixel_count,
                                BlendMode blend_type,
                                const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] * src_scan[col] / 255 / 255;
    } else {
      src_alpha = mask_alpha * src_scan[col] / 255;
    }
    uint8_t back_alpha = dest_scan[3];
    if (back_alpha == 0) {
      FXARGB_SETDIB(dest_scan, ArgbEncode(src_alpha, src_r, src_g, src_b));
      dest_scan += 4;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan += 4;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      RGB_Blend(blend_type, scan, dest_scan, blended_colors);
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[0], alpha_ratio);
      dest_scan++;
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[1], alpha_ratio);
      dest_scan++;
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[2], alpha_ratio);
    } else if (blend_type != BlendMode::kNormal) {
      int blended = Blend(blend_type, *dest_scan, src_b);
      blended = FXDIB_ALPHA_MERGE(src_b, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_g);
      blended = FXDIB_ALPHA_MERGE(src_g, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_r);
      blended = FXDIB_ALPHA_MERGE(src_r, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
    } else {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, alpha_ratio);
    }
    dest_scan += 2;
  }
}

void CompositeRow_ByteMask2Rgba(uint8_t* dest_scan,
                                const uint8_t* src_scan,
                                int mask_alpha,
                                int src_r,
                                int src_g,
                                int src_b,
                                int pixel_count,
                                BlendMode blend_type,
                                const uint8_t* clip_scan,
                                uint8_t* dest_alpha_scan) {
  for (int col = 0; col < pixel_count; col++) {
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] * src_scan[col] / 255 / 255;
    } else {
      src_alpha = mask_alpha * src_scan[col] / 255;
    }
    uint8_t back_alpha = *dest_alpha_scan;
    if (back_alpha == 0) {
      *dest_scan++ = src_b;
      *dest_scan++ = src_g;
      *dest_scan++ = src_r;
      *dest_alpha_scan++ = src_alpha;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan += 3;
      dest_alpha_scan++;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    *dest_alpha_scan++ = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      RGB_Blend(blend_type, scan, dest_scan, blended_colors);
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[0], alpha_ratio);
      dest_scan++;
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[1], alpha_ratio);
      dest_scan++;
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[2], alpha_ratio);
      dest_scan++;
    } else if (blend_type != BlendMode::kNormal) {
      int blended = Blend(blend_type, *dest_scan, src_b);
      blended = FXDIB_ALPHA_MERGE(src_b, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_g);
      blended = FXDIB_ALPHA_MERGE(src_g, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_r);
      blended = FXDIB_ALPHA_MERGE(src_r, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      dest_scan++;
    } else {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, alpha_ratio);
      dest_scan++;
    }
  }
}

void CompositeRow_ByteMask2Rgb(uint8_t* dest_scan,
                               const uint8_t* src_scan,
                               int mask_alpha,
                               int src_r,
                               int src_g,
                               int src_b,
                               int pixel_count,
                               BlendMode blend_type,
                               int Bpp,
                               const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] * src_scan[col] / 255 / 255;
    } else {
      src_alpha = mask_alpha * src_scan[col] / 255;
    }
    if (src_alpha == 0) {
      dest_scan += Bpp;
      continue;
    }
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      RGB_Blend(blend_type, scan, dest_scan, blended_colors);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[0], src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[1], src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[2], src_alpha);
    } else if (blend_type != BlendMode::kNormal) {
      int blended = Blend(blend_type, *dest_scan, src_b);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, src_alpha);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_g);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, src_alpha);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_r);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, src_alpha);
    } else {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, src_alpha);
    }
    dest_scan += Bpp - 2;
  }
}

void CompositeRow_ByteMask2Mask(uint8_t* dest_scan,
                                const uint8_t* src_scan,
                                int mask_alpha,
                                int pixel_count,
                                const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] * src_scan[col] / 255 / 255;
    } else {
      src_alpha = mask_alpha * src_scan[col] / 255;
    }
    uint8_t back_alpha = *dest_scan;
    if (!back_alpha) {
      *dest_scan = src_alpha;
    } else if (src_alpha) {
      *dest_scan = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    }
    dest_scan++;
  }
}

void CompositeRow_ByteMask2Gray(uint8_t* dest_scan,
                                const uint8_t* src_scan,
                                int mask_alpha,
                                int src_gray,
                                int pixel_count,
                                const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] * src_scan[col] / 255 / 255;
    } else {
      src_alpha = mask_alpha * src_scan[col] / 255;
    }
    if (src_alpha) {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_gray, src_alpha);
    }
    dest_scan++;
  }
}

void CompositeRow_ByteMask2Graya(uint8_t* dest_scan,
                                 const uint8_t* src_scan,
                                 int mask_alpha,
                                 int src_gray,
                                 int pixel_count,
                                 const uint8_t* clip_scan,
                                 uint8_t* dest_alpha_scan) {
  for (int col = 0; col < pixel_count; col++) {
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] * src_scan[col] / 255 / 255;
    } else {
      src_alpha = mask_alpha * src_scan[col] / 255;
    }
    uint8_t back_alpha = *dest_alpha_scan;
    if (back_alpha == 0) {
      *dest_scan++ = src_gray;
      *dest_alpha_scan++ = src_alpha;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan++;
      dest_alpha_scan++;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    *dest_alpha_scan++ = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_gray, alpha_ratio);
    dest_scan++;
  }
}

void CompositeRow_BitMask2Argb(uint8_t* dest_scan,
                               const uint8_t* src_scan,
                               int mask_alpha,
                               int src_r,
                               int src_g,
                               int src_b,
                               int src_left,
                               int pixel_count,
                               BlendMode blend_type,
                               const uint8_t* clip_scan) {
  if (blend_type == BlendMode::kNormal && !clip_scan && mask_alpha == 255) {
    FX_ARGB argb = ArgbEncode(0xff, src_r, src_g, src_b);
    for (int col = 0; col < pixel_count; col++) {
      if (src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8))) {
        FXARGB_SETDIB(dest_scan, argb);
      }
      dest_scan += 4;
    }
    return;
  }
  for (int col = 0; col < pixel_count; col++) {
    if (!(src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8)))) {
      dest_scan += 4;
      continue;
    }
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] / 255;
    } else {
      src_alpha = mask_alpha;
    }
    uint8_t back_alpha = dest_scan[3];
    if (back_alpha == 0) {
      FXARGB_SETDIB(dest_scan, ArgbEncode(src_alpha, src_r, src_g, src_b));
      dest_scan += 4;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      RGB_Blend(blend_type, scan, dest_scan, blended_colors);
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[0], alpha_ratio);
      dest_scan++;
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[1], alpha_ratio);
      dest_scan++;
      *dest_scan =
          FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[2], alpha_ratio);
    } else if (blend_type != BlendMode::kNormal) {
      int blended = Blend(blend_type, *dest_scan, src_b);
      blended = FXDIB_ALPHA_MERGE(src_b, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_g);
      blended = FXDIB_ALPHA_MERGE(src_g, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_r);
      blended = FXDIB_ALPHA_MERGE(src_r, blended, back_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, alpha_ratio);
    } else {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, alpha_ratio);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, alpha_ratio);
    }
    dest_scan += 2;
  }
}

void CompositeRow_BitMask2Rgb(uint8_t* dest_scan,
                              const uint8_t* src_scan,
                              int mask_alpha,
                              int src_r,
                              int src_g,
                              int src_b,
                              int src_left,
                              int pixel_count,
                              BlendMode blend_type,
                              int Bpp,
                              const uint8_t* clip_scan) {
  if (blend_type == BlendMode::kNormal && !clip_scan && mask_alpha == 255) {
    for (int col = 0; col < pixel_count; col++) {
      if (src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8))) {
        dest_scan[2] = src_r;
        dest_scan[1] = src_g;
        dest_scan[0] = src_b;
      }
      dest_scan += Bpp;
    }
    return;
  }
  for (int col = 0; col < pixel_count; col++) {
    if (!(src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8)))) {
      dest_scan += Bpp;
      continue;
    }
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] / 255;
    } else {
      src_alpha = mask_alpha;
    }
    if (src_alpha == 0) {
      dest_scan += Bpp;
      continue;
    }
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      RGB_Blend(blend_type, scan, dest_scan, blended_colors);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[0], src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[1], src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended_colors[2], src_alpha);
    } else if (blend_type != BlendMode::kNormal) {
      int blended = Blend(blend_type, *dest_scan, src_b);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, src_alpha);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_g);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, src_alpha);
      dest_scan++;
      blended = Blend(blend_type, *dest_scan, src_r);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, blended, src_alpha);
    } else {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, src_alpha);
    }
    dest_scan += Bpp - 2;
  }
}

void CompositeRow_BitMask2Mask(uint8_t* dest_scan,
                               const uint8_t* src_scan,
                               int mask_alpha,
                               int src_left,
                               int pixel_count,
                               const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    if (!(src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8)))) {
      dest_scan++;
      continue;
    }
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] / 255;
    } else {
      src_alpha = mask_alpha;
    }
    uint8_t back_alpha = *dest_scan;
    if (!back_alpha) {
      *dest_scan = src_alpha;
    } else if (src_alpha) {
      *dest_scan = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    }
    dest_scan++;
  }
}

void CompositeRow_BitMask2Gray(uint8_t* dest_scan,
                               const uint8_t* src_scan,
                               int mask_alpha,
                               int src_gray,
                               int src_left,
                               int pixel_count,
                               const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    if (!(src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8)))) {
      dest_scan++;
      continue;
    }
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] / 255;
    } else {
      src_alpha = mask_alpha;
    }
    if (src_alpha) {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_gray, src_alpha);
    }
    dest_scan++;
  }
}

void CompositeRow_BitMask2Graya(uint8_t* dest_scan,
                                const uint8_t* src_scan,
                                int mask_alpha,
                                int src_gray,
                                int src_left,
                                int pixel_count,
                                const uint8_t* clip_scan,
                                uint8_t* dest_alpha_scan) {
  for (int col = 0; col < pixel_count; col++) {
    if (!(src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8)))) {
      dest_scan++;
      dest_alpha_scan++;
      continue;
    }
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] / 255;
    } else {
      src_alpha = mask_alpha;
    }
    uint8_t back_alpha = *dest_alpha_scan;
    if (back_alpha == 0) {
      *dest_scan++ = src_gray;
      *dest_alpha_scan++ = src_alpha;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan++;
      dest_alpha_scan++;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    *dest_alpha_scan++ = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_gray, alpha_ratio);
    dest_scan++;
  }
}

void CompositeRow_Argb2Argb_RgbByteOrder(uint8_t* dest_scan,
                                         const uint8_t* src_scan,
                                         int pixel_count,
                                         BlendMode blend_type,
                                         const uint8_t* clip_scan) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  for (int col = 0; col < pixel_count; col++) {
    uint8_t back_alpha = dest_scan[3];
    if (back_alpha == 0) {
      if (clip_scan) {
        int src_alpha = clip_scan[col] * src_scan[3] / 255;
        dest_scan[3] = src_alpha;
        dest_scan[0] = src_scan[2];
        dest_scan[1] = src_scan[1];
        dest_scan[2] = src_scan[0];
      } else {
        FXARGB_RGBORDERCOPY(dest_scan, src_scan);
      }
      dest_scan += 4;
      src_scan += 4;
      continue;
    }
    uint8_t src_alpha;
    if (clip_scan) {
      src_alpha = clip_scan[col] * src_scan[3] / 255;
    } else {
      src_alpha = src_scan[3];
    }
    if (src_alpha == 0) {
      dest_scan += 4;
      src_scan += 4;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (bNonseparableBlend) {
      uint8_t dest_scan_o[3];
      dest_scan_o[0] = dest_scan[2];
      dest_scan_o[1] = dest_scan[1];
      dest_scan_o[2] = dest_scan[0];
      RGB_Blend(blend_type, src_scan, dest_scan_o, blended_colors);
    }
    for (int color = 0; color < 3; color++) {
      int index = 2 - color;
      if (blend_type != BlendMode::kNormal) {
        int blended = bNonseparableBlend
                          ? blended_colors[color]
                          : Blend(blend_type, dest_scan[index], *src_scan);
        blended = FXDIB_ALPHA_MERGE(*src_scan, blended, back_alpha);
        dest_scan[index] =
            FXDIB_ALPHA_MERGE(dest_scan[index], blended, alpha_ratio);
      } else {
        dest_scan[index] =
            FXDIB_ALPHA_MERGE(dest_scan[index], *src_scan, alpha_ratio);
      }
      src_scan++;
    }
    dest_scan += 4;
    src_scan++;
  }
}

void CompositeRow_Rgb2Argb_Blend_NoClip_RgbByteOrder(uint8_t* dest_scan,
                                                     const uint8_t* src_scan,
                                                     int width,
                                                     BlendMode blend_type,
                                                     int src_Bpp) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int src_gap = src_Bpp - 3;
  for (int col = 0; col < width; col++) {
    uint8_t back_alpha = dest_scan[3];
    if (back_alpha == 0) {
      if (src_Bpp == 4) {
        FXARGB_SETRGBORDERDIB(dest_scan, 0xff000000 | FXARGB_GETDIB(src_scan));
      } else {
        FXARGB_SETRGBORDERDIB(
            dest_scan, ArgbEncode(0xff, src_scan[2], src_scan[1], src_scan[0]));
      }
      dest_scan += 4;
      src_scan += src_Bpp;
      continue;
    }
    dest_scan[3] = 0xff;
    if (bNonseparableBlend) {
      uint8_t dest_scan_o[3];
      dest_scan_o[0] = dest_scan[2];
      dest_scan_o[1] = dest_scan[1];
      dest_scan_o[2] = dest_scan[0];
      RGB_Blend(blend_type, src_scan, dest_scan_o, blended_colors);
    }
    for (int color = 0; color < 3; color++) {
      int index = 2 - color;
      int src_color = *src_scan;
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, dest_scan[index], src_color);
      dest_scan[index] = FXDIB_ALPHA_MERGE(src_color, blended, back_alpha);
      src_scan++;
    }
    dest_scan += 4;
    src_scan += src_gap;
  }
}

void CompositeRow_Argb2Rgb_Blend_RgbByteOrder(uint8_t* dest_scan,
                                              const uint8_t* src_scan,
                                              int width,
                                              BlendMode blend_type,
                                              int dest_Bpp,
                                              const uint8_t* clip_scan) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  for (int col = 0; col < width; col++) {
    uint8_t src_alpha;
    if (clip_scan) {
      src_alpha = src_scan[3] * (*clip_scan++) / 255;
    } else {
      src_alpha = src_scan[3];
    }
    if (src_alpha == 0) {
      dest_scan += dest_Bpp;
      src_scan += 4;
      continue;
    }
    if (bNonseparableBlend) {
      uint8_t dest_scan_o[3];
      dest_scan_o[0] = dest_scan[2];
      dest_scan_o[1] = dest_scan[1];
      dest_scan_o[2] = dest_scan[0];
      RGB_Blend(blend_type, src_scan, dest_scan_o, blended_colors);
    }
    for (int color = 0; color < 3; color++) {
      int index = 2 - color;
      int back_color = dest_scan[index];
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, back_color, *src_scan);
      dest_scan[index] = FXDIB_ALPHA_MERGE(back_color, blended, src_alpha);
      src_scan++;
    }
    dest_scan += dest_Bpp;
    src_scan++;
  }
}

void CompositeRow_Rgb2Argb_NoBlend_NoClip_RgbByteOrder(uint8_t* dest_scan,
                                                       const uint8_t* src_scan,
                                                       int width,
                                                       int src_Bpp) {
  for (int col = 0; col < width; col++) {
    if (src_Bpp == 4) {
      FXARGB_SETRGBORDERDIB(dest_scan, 0xff000000 | FXARGB_GETDIB(src_scan));
    } else {
      FXARGB_SETRGBORDERDIB(
          dest_scan, ArgbEncode(0xff, src_scan[2], src_scan[1], src_scan[0]));
    }
    dest_scan += 4;
    src_scan += src_Bpp;
  }
}

void CompositeRow_Rgb2Rgb_Blend_NoClip_RgbByteOrder(uint8_t* dest_scan,
                                                    const uint8_t* src_scan,
                                                    int width,
                                                    BlendMode blend_type,
                                                    int dest_Bpp,
                                                    int src_Bpp) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int src_gap = src_Bpp - 3;
  for (int col = 0; col < width; col++) {
    if (bNonseparableBlend) {
      uint8_t dest_scan_o[3];
      dest_scan_o[0] = dest_scan[2];
      dest_scan_o[1] = dest_scan[1];
      dest_scan_o[2] = dest_scan[0];
      RGB_Blend(blend_type, src_scan, dest_scan_o, blended_colors);
    }
    for (int color = 0; color < 3; color++) {
      int index = 2 - color;
      int back_color = dest_scan[index];
      int src_color = *src_scan;
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, back_color, src_color);
      dest_scan[index] = blended;
      src_scan++;
    }
    dest_scan += dest_Bpp;
    src_scan += src_gap;
  }
}

void CompositeRow_Argb2Rgb_NoBlend_RgbByteOrder(uint8_t* dest_scan,
                                                const uint8_t* src_scan,
                                                int width,
                                                int dest_Bpp,
                                                const uint8_t* clip_scan) {
  for (int col = 0; col < width; col++) {
    uint8_t src_alpha;
    if (clip_scan) {
      src_alpha = src_scan[3] * (*clip_scan++) / 255;
    } else {
      src_alpha = src_scan[3];
    }
    if (src_alpha == 255) {
      dest_scan[2] = *src_scan++;
      dest_scan[1] = *src_scan++;
      dest_scan[0] = *src_scan++;
      dest_scan += dest_Bpp;
      src_scan++;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan += dest_Bpp;
      src_scan += 4;
      continue;
    }
    for (int color = 0; color < 3; color++) {
      int index = 2 - color;
      dest_scan[index] =
          FXDIB_ALPHA_MERGE(dest_scan[index], *src_scan, src_alpha);
      src_scan++;
    }
    dest_scan += dest_Bpp;
    src_scan++;
  }
}

void CompositeRow_Rgb2Rgb_NoBlend_NoClip_RgbByteOrder(uint8_t* dest_scan,
                                                      const uint8_t* src_scan,
                                                      int width,
                                                      int dest_Bpp,
                                                      int src_Bpp) {
  for (int col = 0; col < width; col++) {
    dest_scan[2] = src_scan[0];
    dest_scan[1] = src_scan[1];
    dest_scan[0] = src_scan[2];
    dest_scan += dest_Bpp;
    src_scan += src_Bpp;
  }
}

void CompositeRow_Rgb2Argb_Blend_Clip_RgbByteOrder(uint8_t* dest_scan,
                                                   const uint8_t* src_scan,
                                                   int width,
                                                   BlendMode blend_type,
                                                   int src_Bpp,
                                                   const uint8_t* clip_scan) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int src_gap = src_Bpp - 3;
  for (int col = 0; col < width; col++) {
    int src_alpha = *clip_scan++;
    uint8_t back_alpha = dest_scan[3];
    if (back_alpha == 0) {
      dest_scan[2] = *src_scan++;
      dest_scan[1] = *src_scan++;
      dest_scan[0] = *src_scan++;
      src_scan += src_gap;
      dest_scan += 4;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan += 4;
      src_scan += src_Bpp;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (bNonseparableBlend) {
      uint8_t dest_scan_o[3];
      dest_scan_o[0] = dest_scan[2];
      dest_scan_o[1] = dest_scan[1];
      dest_scan_o[2] = dest_scan[0];
      RGB_Blend(blend_type, src_scan, dest_scan_o, blended_colors);
    }
    for (int color = 0; color < 3; color++) {
      int index = 2 - color;
      int src_color = *src_scan;
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, dest_scan[index], src_color);
      blended = FXDIB_ALPHA_MERGE(src_color, blended, back_alpha);
      dest_scan[index] =
          FXDIB_ALPHA_MERGE(dest_scan[index], blended, alpha_ratio);
      src_scan++;
    }
    dest_scan += 4;
    src_scan += src_gap;
  }
}

void CompositeRow_Rgb2Rgb_Blend_Clip_RgbByteOrder(uint8_t* dest_scan,
                                                  const uint8_t* src_scan,
                                                  int width,
                                                  BlendMode blend_type,
                                                  int dest_Bpp,
                                                  int src_Bpp,
                                                  const uint8_t* clip_scan) {
  int blended_colors[3];
  bool bNonseparableBlend = IsNonSeparableBlendMode(blend_type);
  int src_gap = src_Bpp - 3;
  for (int col = 0; col < width; col++) {
    uint8_t src_alpha = *clip_scan++;
    if (src_alpha == 0) {
      dest_scan += dest_Bpp;
      src_scan += src_Bpp;
      continue;
    }
    if (bNonseparableBlend) {
      uint8_t dest_scan_o[3];
      dest_scan_o[0] = dest_scan[2];
      dest_scan_o[1] = dest_scan[1];
      dest_scan_o[2] = dest_scan[0];
      RGB_Blend(blend_type, src_scan, dest_scan_o, blended_colors);
    }
    for (int color = 0; color < 3; color++) {
      int index = 2 - color;
      int src_color = *src_scan;
      int back_color = dest_scan[index];
      int blended = bNonseparableBlend
                        ? blended_colors[color]
                        : Blend(blend_type, back_color, src_color);
      dest_scan[index] = FXDIB_ALPHA_MERGE(back_color, blended, src_alpha);
      src_scan++;
    }
    dest_scan += dest_Bpp;
    src_scan += src_gap;
  }
}

void CompositeRow_Rgb2Argb_NoBlend_Clip_RgbByteOrder(uint8_t* dest_scan,
                                                     const uint8_t* src_scan,
                                                     int width,
                                                     int src_Bpp,
                                                     const uint8_t* clip_scan) {
  int src_gap = src_Bpp - 3;
  for (int col = 0; col < width; col++) {
    int src_alpha = clip_scan[col];
    if (src_alpha == 255) {
      dest_scan[2] = *src_scan++;
      dest_scan[1] = *src_scan++;
      dest_scan[0] = *src_scan++;
      dest_scan[3] = 255;
      dest_scan += 4;
      src_scan += src_gap;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan += 4;
      src_scan += src_Bpp;
      continue;
    }
    int back_alpha = dest_scan[3];
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    for (int color = 0; color < 3; color++) {
      int index = 2 - color;
      dest_scan[index] =
          FXDIB_ALPHA_MERGE(dest_scan[index], *src_scan, alpha_ratio);
      src_scan++;
    }
    dest_scan += 4;
    src_scan += src_gap;
  }
}

void CompositeRow_Rgb2Rgb_NoBlend_Clip_RgbByteOrder(uint8_t* dest_scan,
                                                    const uint8_t* src_scan,
                                                    int width,
                                                    int dest_Bpp,
                                                    int src_Bpp,
                                                    const uint8_t* clip_scan) {
  for (int col = 0; col < width; col++) {
    int src_alpha = clip_scan[col];
    if (src_alpha == 255) {
      dest_scan[2] = src_scan[0];
      dest_scan[1] = src_scan[1];
      dest_scan[0] = src_scan[2];
    } else if (src_alpha) {
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], *src_scan, src_alpha);
      src_scan++;
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], *src_scan, src_alpha);
      src_scan++;
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], *src_scan, src_alpha);
      dest_scan += dest_Bpp;
      src_scan += src_Bpp - 2;
      continue;
    }
    dest_scan += dest_Bpp;
    src_scan += src_Bpp;
  }
}

void CompositeRow_8bppRgb2Rgb_NoBlend_RgbByteOrder(uint8_t* dest_scan,
                                                   const uint8_t* src_scan,
                                                   FX_ARGB* pPalette,
                                                   int pixel_count,
                                                   int DestBpp,
                                                   const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    FX_ARGB argb = pPalette ? pPalette[*src_scan] : (*src_scan) * 0x010101;
    int src_r = FXARGB_R(argb);
    int src_g = FXARGB_G(argb);
    int src_b = FXARGB_B(argb);
    if (clip_scan && clip_scan[col] < 255) {
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], src_b, clip_scan[col]);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], src_g, clip_scan[col]);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], src_r, clip_scan[col]);
    } else {
      dest_scan[2] = src_b;
      dest_scan[1] = src_g;
      dest_scan[0] = src_r;
    }
    dest_scan += DestBpp;
    src_scan++;
  }
}

void CompositeRow_1bppRgb2Rgb_NoBlend_RgbByteOrder(uint8_t* dest_scan,
                                                   const uint8_t* src_scan,
                                                   int src_left,
                                                   FX_ARGB* pPalette,
                                                   int pixel_count,
                                                   int DestBpp,
                                                   const uint8_t* clip_scan) {
  int reset_r, reset_g, reset_b;
  int set_r, set_g, set_b;
  if (pPalette) {
    reset_r = FXARGB_R(pPalette[0]);
    reset_g = FXARGB_G(pPalette[0]);
    reset_b = FXARGB_B(pPalette[0]);
    set_r = FXARGB_R(pPalette[1]);
    set_g = FXARGB_G(pPalette[1]);
    set_b = FXARGB_B(pPalette[1]);
  } else {
    reset_r = reset_g = reset_b = 0;
    set_r = set_g = set_b = 255;
  }
  for (int col = 0; col < pixel_count; col++) {
    int src_r, src_g, src_b;
    if (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8))) {
      src_r = set_r;
      src_g = set_g;
      src_b = set_b;
    } else {
      src_r = reset_r;
      src_g = reset_g;
      src_b = reset_b;
    }
    if (clip_scan && clip_scan[col] < 255) {
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], src_b, clip_scan[col]);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], src_g, clip_scan[col]);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], src_r, clip_scan[col]);
    } else {
      dest_scan[2] = src_b;
      dest_scan[1] = src_g;
      dest_scan[0] = src_r;
    }
    dest_scan += DestBpp;
  }
}

void CompositeRow_8bppRgb2Argb_NoBlend_RgbByteOrder(uint8_t* dest_scan,
                                                    const uint8_t* src_scan,
                                                    int width,
                                                    FX_ARGB* pPalette,
                                                    const uint8_t* clip_scan) {
  for (int col = 0; col < width; col++) {
    int src_r, src_g, src_b;
    if (pPalette) {
      FX_ARGB argb = pPalette[*src_scan];
      src_r = FXARGB_R(argb);
      src_g = FXARGB_G(argb);
      src_b = FXARGB_B(argb);
    } else {
      src_r = src_g = src_b = *src_scan;
    }
    if (!clip_scan || clip_scan[col] == 255) {
      dest_scan[2] = src_b;
      dest_scan[1] = src_g;
      dest_scan[0] = src_r;
      dest_scan[3] = 255;
      src_scan++;
      dest_scan += 4;
      continue;
    }
    int src_alpha = clip_scan[col];
    if (src_alpha == 0) {
      dest_scan += 4;
      src_scan++;
      continue;
    }
    int back_alpha = dest_scan[3];
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], src_b, alpha_ratio);
    dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], src_g, alpha_ratio);
    dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], src_r, alpha_ratio);
    dest_scan += 4;
    src_scan++;
  }
}

void CompositeRow_1bppRgb2Argb_NoBlend_RgbByteOrder(uint8_t* dest_scan,
                                                    const uint8_t* src_scan,
                                                    int src_left,
                                                    int width,
                                                    FX_ARGB* pPalette,
                                                    const uint8_t* clip_scan) {
  int reset_r, reset_g, reset_b;
  int set_r, set_g, set_b;
  if (pPalette) {
    reset_r = FXARGB_R(pPalette[0]);
    reset_g = FXARGB_G(pPalette[0]);
    reset_b = FXARGB_B(pPalette[0]);
    set_r = FXARGB_R(pPalette[1]);
    set_g = FXARGB_G(pPalette[1]);
    set_b = FXARGB_B(pPalette[1]);
  } else {
    reset_r = reset_g = reset_b = 0;
    set_r = set_g = set_b = 255;
  }
  for (int col = 0; col < width; col++) {
    int src_r, src_g, src_b;
    if (src_scan[(col + src_left) / 8] & (1 << (7 - (col + src_left) % 8))) {
      src_r = set_r;
      src_g = set_g;
      src_b = set_b;
    } else {
      src_r = reset_r;
      src_g = reset_g;
      src_b = reset_b;
    }
    if (!clip_scan || clip_scan[col] == 255) {
      dest_scan[2] = src_b;
      dest_scan[1] = src_g;
      dest_scan[0] = src_r;
      dest_scan[3] = 255;
      dest_scan += 4;
      continue;
    }
    int src_alpha = clip_scan[col];
    if (src_alpha == 0) {
      dest_scan += 4;
      continue;
    }
    int back_alpha = dest_scan[3];
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], src_b, alpha_ratio);
    dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], src_g, alpha_ratio);
    dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], src_r, alpha_ratio);
    dest_scan += 4;
  }
}

void CompositeRow_ByteMask2Argb_RgbByteOrder(uint8_t* dest_scan,
                                             const uint8_t* src_scan,
                                             int mask_alpha,
                                             int src_r,
                                             int src_g,
                                             int src_b,
                                             int pixel_count,
                                             BlendMode blend_type,
                                             const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] * src_scan[col] / 255 / 255;
    } else {
      src_alpha = mask_alpha * src_scan[col] / 255;
    }
    uint8_t back_alpha = dest_scan[3];
    if (back_alpha == 0) {
      FXARGB_SETRGBORDERDIB(dest_scan,
                            ArgbEncode(src_alpha, src_r, src_g, src_b));
      dest_scan += 4;
      continue;
    }
    if (src_alpha == 0) {
      dest_scan += 4;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      uint8_t dest_scan_o[3] = {dest_scan[2], dest_scan[1], dest_scan[0]};
      RGB_Blend(blend_type, scan, dest_scan_o, blended_colors);
      dest_scan[2] =
          FXDIB_ALPHA_MERGE(dest_scan[2], blended_colors[0], alpha_ratio);
      dest_scan[1] =
          FXDIB_ALPHA_MERGE(dest_scan[1], blended_colors[1], alpha_ratio);
      dest_scan[0] =
          FXDIB_ALPHA_MERGE(dest_scan[0], blended_colors[2], alpha_ratio);
    } else if (blend_type != BlendMode::kNormal) {
      int blended = Blend(blend_type, dest_scan[2], src_b);
      blended = FXDIB_ALPHA_MERGE(src_b, blended, back_alpha);
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], blended, alpha_ratio);
      blended = Blend(blend_type, dest_scan[1], src_g);
      blended = FXDIB_ALPHA_MERGE(src_g, blended, back_alpha);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], blended, alpha_ratio);
      blended = Blend(blend_type, dest_scan[0], src_r);
      blended = FXDIB_ALPHA_MERGE(src_r, blended, back_alpha);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], blended, alpha_ratio);
    } else {
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], src_b, alpha_ratio);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], src_g, alpha_ratio);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], src_r, alpha_ratio);
    }
    dest_scan += 4;
  }
}

void CompositeRow_ByteMask2Rgb_RgbByteOrder(uint8_t* dest_scan,
                                            const uint8_t* src_scan,
                                            int mask_alpha,
                                            int src_r,
                                            int src_g,
                                            int src_b,
                                            int pixel_count,
                                            BlendMode blend_type,
                                            int Bpp,
                                            const uint8_t* clip_scan) {
  for (int col = 0; col < pixel_count; col++) {
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] * src_scan[col] / 255 / 255;
    } else {
      src_alpha = mask_alpha * src_scan[col] / 255;
    }
    if (src_alpha == 0) {
      dest_scan += Bpp;
      continue;
    }
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      uint8_t dest_scan_o[3] = {dest_scan[2], dest_scan[1], dest_scan[0]};
      RGB_Blend(blend_type, scan, dest_scan_o, blended_colors);
      dest_scan[2] =
          FXDIB_ALPHA_MERGE(dest_scan[2], blended_colors[0], src_alpha);
      dest_scan[1] =
          FXDIB_ALPHA_MERGE(dest_scan[1], blended_colors[1], src_alpha);
      dest_scan[0] =
          FXDIB_ALPHA_MERGE(dest_scan[0], blended_colors[2], src_alpha);
    } else if (blend_type != BlendMode::kNormal) {
      int blended = Blend(blend_type, dest_scan[2], src_b);
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], blended, src_alpha);
      blended = Blend(blend_type, dest_scan[1], src_g);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], blended, src_alpha);
      blended = Blend(blend_type, dest_scan[0], src_r);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], blended, src_alpha);
    } else {
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], src_b, src_alpha);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], src_g, src_alpha);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], src_r, src_alpha);
    }
    dest_scan += Bpp;
  }
}

void CompositeRow_BitMask2Argb_RgbByteOrder(uint8_t* dest_scan,
                                            const uint8_t* src_scan,
                                            int mask_alpha,
                                            int src_r,
                                            int src_g,
                                            int src_b,
                                            int src_left,
                                            int pixel_count,
                                            BlendMode blend_type,
                                            const uint8_t* clip_scan) {
  if (blend_type == BlendMode::kNormal && !clip_scan && mask_alpha == 255) {
    FX_ARGB argb = ArgbEncode(0xff, src_r, src_g, src_b);
    for (int col = 0; col < pixel_count; col++) {
      if (src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8))) {
        FXARGB_SETRGBORDERDIB(dest_scan, argb);
      }
      dest_scan += 4;
    }
    return;
  }
  for (int col = 0; col < pixel_count; col++) {
    if (!(src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8)))) {
      dest_scan += 4;
      continue;
    }
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] / 255;
    } else {
      src_alpha = mask_alpha;
    }
    uint8_t back_alpha = dest_scan[3];
    if (back_alpha == 0) {
      FXARGB_SETRGBORDERDIB(dest_scan,
                            ArgbEncode(src_alpha, src_r, src_g, src_b));
      dest_scan += 4;
      continue;
    }
    uint8_t dest_alpha = back_alpha + src_alpha - back_alpha * src_alpha / 255;
    dest_scan[3] = dest_alpha;
    int alpha_ratio = src_alpha * 255 / dest_alpha;
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      uint8_t dest_scan_o[3] = {dest_scan[2], dest_scan[1], dest_scan[0]};
      RGB_Blend(blend_type, scan, dest_scan_o, blended_colors);
      dest_scan[2] =
          FXDIB_ALPHA_MERGE(dest_scan[2], blended_colors[0], alpha_ratio);
      dest_scan[1] =
          FXDIB_ALPHA_MERGE(dest_scan[1], blended_colors[1], alpha_ratio);
      dest_scan[0] =
          FXDIB_ALPHA_MERGE(dest_scan[0], blended_colors[2], alpha_ratio);
    } else if (blend_type != BlendMode::kNormal) {
      int blended = Blend(blend_type, dest_scan[2], src_b);
      blended = FXDIB_ALPHA_MERGE(src_b, blended, back_alpha);
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], blended, alpha_ratio);
      blended = Blend(blend_type, dest_scan[1], src_g);
      blended = FXDIB_ALPHA_MERGE(src_g, blended, back_alpha);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], blended, alpha_ratio);
      blended = Blend(blend_type, dest_scan[0], src_r);
      blended = FXDIB_ALPHA_MERGE(src_r, blended, back_alpha);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], blended, alpha_ratio);
    } else {
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], src_b, alpha_ratio);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], src_g, alpha_ratio);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], src_r, alpha_ratio);
    }
    dest_scan += 4;
  }
}

void CompositeRow_BitMask2Rgb_RgbByteOrder(uint8_t* dest_scan,
                                           const uint8_t* src_scan,
                                           int mask_alpha,
                                           int src_r,
                                           int src_g,
                                           int src_b,
                                           int src_left,
                                           int pixel_count,
                                           BlendMode blend_type,
                                           int Bpp,
                                           const uint8_t* clip_scan) {
  if (blend_type == BlendMode::kNormal && !clip_scan && mask_alpha == 255) {
    for (int col = 0; col < pixel_count; col++) {
      if (src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8))) {
        dest_scan[2] = src_b;
        dest_scan[1] = src_g;
        dest_scan[0] = src_r;
      }
      dest_scan += Bpp;
    }
    return;
  }
  for (int col = 0; col < pixel_count; col++) {
    if (!(src_scan[(src_left + col) / 8] & (1 << (7 - (src_left + col) % 8)))) {
      dest_scan += Bpp;
      continue;
    }
    int src_alpha;
    if (clip_scan) {
      src_alpha = mask_alpha * clip_scan[col] / 255;
    } else {
      src_alpha = mask_alpha;
    }
    if (src_alpha == 0) {
      dest_scan += Bpp;
      continue;
    }
    if (IsNonSeparableBlendMode(blend_type)) {
      int blended_colors[3];
      uint8_t scan[3] = {static_cast<uint8_t>(src_b),
                         static_cast<uint8_t>(src_g),
                         static_cast<uint8_t>(src_r)};
      uint8_t dest_scan_o[3] = {dest_scan[2], dest_scan[1], dest_scan[0]};
      RGB_Blend(blend_type, scan, dest_scan_o, blended_colors);
      dest_scan[2] =
          FXDIB_ALPHA_MERGE(dest_scan[2], blended_colors[0], src_alpha);
      dest_scan[1] =
          FXDIB_ALPHA_MERGE(dest_scan[1], blended_colors[1], src_alpha);
      dest_scan[0] =
          FXDIB_ALPHA_MERGE(dest_scan[0], blended_colors[2], src_alpha);
    } else if (blend_type != BlendMode::kNormal) {
      int back_color = dest_scan[2];
      int blended = Blend(blend_type, back_color, src_b);
      dest_scan[2] = FXDIB_ALPHA_MERGE(back_color, blended, src_alpha);
      back_color = dest_scan[1];
      blended = Blend(blend_type, back_color, src_g);
      dest_scan[1] = FXDIB_ALPHA_MERGE(back_color, blended, src_alpha);
      back_color = dest_scan[0];
      blended = Blend(blend_type, back_color, src_r);
      dest_scan[0] = FXDIB_ALPHA_MERGE(back_color, blended, src_alpha);
    } else {
      dest_scan[2] = FXDIB_ALPHA_MERGE(dest_scan[2], src_b, src_alpha);
      dest_scan[1] = FXDIB_ALPHA_MERGE(dest_scan[1], src_g, src_alpha);
      dest_scan[0] = FXDIB_ALPHA_MERGE(dest_scan[0], src_r, src_alpha);
    }
    dest_scan += Bpp;
  }
}

}  // namespace

CFX_ScanlineCompositor::CFX_ScanlineCompositor() = default;

CFX_ScanlineCompositor::~CFX_ScanlineCompositor() = default;

bool CFX_ScanlineCompositor::Init(FXDIB_Format dest_format,
                                  FXDIB_Format src_format,
                                  int32_t width,
                                  uint32_t* pSrcPalette,
                                  uint32_t mask_color,
                                  BlendMode blend_type,
                                  bool bClip,
                                  bool bRgbByteOrder,
                                  int alpha_flag) {
  m_SrcFormat = src_format;
  m_DestFormat = dest_format;
  m_BlendType = blend_type;
  m_bRgbByteOrder = bRgbByteOrder;
  if (GetBppFromFormat(dest_format) == 1)
    return false;
  if (m_SrcFormat == FXDIB_1bppMask || m_SrcFormat == FXDIB_8bppMask) {
    InitSourceMask(alpha_flag, mask_color);
    return true;
  }
  if ((~src_format & 0x0400) && (dest_format & 0x0400))
    return false;
  if ((m_SrcFormat & 0xff) <= 8) {
    if (dest_format == FXDIB_8bppMask)
      return true;

    InitSourcePalette(src_format, dest_format, pSrcPalette);
    m_iTransparency = (dest_format == FXDIB_Argb ? 1 : 0) +
                      (dest_format & 0x0200 ? 2 : 0) +
                      (dest_format & 0x0400 ? 4 : 0) +
                      (GetBppFromFormat(src_format) == 1 ? 8 : 0);
    return true;
  }
  m_iTransparency =
      (src_format & 0x0200 ? 0 : 1) + (dest_format & 0x0200 ? 0 : 2) +
      (blend_type == BlendMode::kNormal ? 4 : 0) + (bClip ? 8 : 0) +
      (src_format & 0x0400 ? 16 : 0) + (dest_format & 0x0400 ? 32 : 0);
  return true;
}

void CFX_ScanlineCompositor::InitSourceMask(int alpha_flag,
                                            uint32_t mask_color) {
  int mask_black = 0;
  if (alpha_flag >> 8) {
    m_MaskAlpha = alpha_flag & 0xff;
    m_MaskRed = FXSYS_GetCValue(mask_color);
    m_MaskGreen = FXSYS_GetMValue(mask_color);
    m_MaskBlue = FXSYS_GetYValue(mask_color);
    mask_black = FXSYS_GetKValue(mask_color);
  } else {
    m_MaskAlpha = FXARGB_A(mask_color);
    m_MaskRed = FXARGB_R(mask_color);
    m_MaskGreen = FXARGB_G(mask_color);
    m_MaskBlue = FXARGB_B(mask_color);
  }
  if (m_DestFormat == FXDIB_8bppMask)
    return;

  if ((m_DestFormat & 0xff) == 8) {
    if (alpha_flag >> 8) {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      std::tie(r, g, b) =
          AdobeCMYK_to_sRGB1(m_MaskRed, m_MaskGreen, m_MaskBlue, mask_black);
      m_MaskRed = FXRGB2GRAY(r, g, b);
    } else {
      m_MaskRed = FXRGB2GRAY(m_MaskRed, m_MaskGreen, m_MaskBlue);
    }
    if (m_DestFormat & 0x0400)
      m_MaskRed = FX_CCOLOR(m_MaskRed);
    return;
  }
  uint8_t* mask_color_p = (uint8_t*)&mask_color;
  mask_color =
      (alpha_flag >> 8) ? FXCMYK_TODIB(mask_color) : FXARGB_TODIB(mask_color);
  if (alpha_flag >> 8) {
    std::tie(mask_color_p[2], mask_color_p[1], mask_color_p[0]) =
        AdobeCMYK_to_sRGB1(mask_color_p[0], mask_color_p[1], mask_color_p[2],
                           mask_color_p[3]);
    m_MaskRed = mask_color_p[2];
    m_MaskGreen = mask_color_p[1];
    m_MaskBlue = mask_color_p[0];
  }
}

void CFX_ScanlineCompositor::InitSourcePalette(FXDIB_Format src_format,
                                               FXDIB_Format dest_format,
                                               const uint32_t* pSrcPalette) {
  bool bIsSrcCmyk = !!(src_format & 0x0400);
  bool bIsDstCmyk = !!(dest_format & 0x0400);
  bool bIsDestBpp8 = GetBppFromFormat(dest_format) == 8;
  int pal_count = 1 << GetBppFromFormat(src_format);
  m_pSrcPalette = nullptr;
  if (pSrcPalette) {
    if (bIsDestBpp8) {
      uint8_t* gray_pal = FX_Alloc(uint8_t, pal_count);
      m_pSrcPalette.reset(reinterpret_cast<uint32_t*>(gray_pal));
      if (bIsSrcCmyk) {
        for (int i = 0; i < pal_count; ++i) {
          FX_CMYK cmyk = pSrcPalette[i];
          uint8_t r;
          uint8_t g;
          uint8_t b;
          std::tie(r, g, b) =
              AdobeCMYK_to_sRGB1(FXSYS_GetCValue(cmyk), FXSYS_GetMValue(cmyk),
                                 FXSYS_GetYValue(cmyk), FXSYS_GetKValue(cmyk));
          *gray_pal++ = FXRGB2GRAY(r, g, b);
        }
      } else {
        for (int i = 0; i < pal_count; ++i) {
          FX_ARGB argb = pSrcPalette[i];
          *gray_pal++ =
              FXRGB2GRAY(FXARGB_R(argb), FXARGB_G(argb), FXARGB_B(argb));
        }
      }
      return;
    }
    m_pSrcPalette.reset(FX_Alloc(uint32_t, pal_count));
    uint32_t* pPalette = m_pSrcPalette.get();
    if (bIsDstCmyk == bIsSrcCmyk) {
      memcpy(pPalette, pSrcPalette, pal_count * sizeof(uint32_t));
    } else {
      for (int i = 0; i < pal_count; ++i) {
        FX_CMYK cmyk = pSrcPalette[i];
        uint8_t r;
        uint8_t g;
        uint8_t b;
        std::tie(r, g, b) =
            AdobeCMYK_to_sRGB1(FXSYS_GetCValue(cmyk), FXSYS_GetMValue(cmyk),
                               FXSYS_GetYValue(cmyk), FXSYS_GetKValue(cmyk));
        pPalette[i] = ArgbEncode(0xff, r, g, b);
      }
    }
    return;
  }
  if (bIsDestBpp8) {
    uint8_t* gray_pal = FX_Alloc(uint8_t, pal_count);
    if (pal_count == 2) {
      gray_pal[0] = 0;
      gray_pal[1] = 255;
    } else {
      for (int i = 0; i < pal_count; ++i)
        gray_pal[i] = i;
    }
    m_pSrcPalette.reset(reinterpret_cast<uint32_t*>(gray_pal));
    return;
  }
  m_pSrcPalette.reset(FX_Alloc(uint32_t, pal_count));
  uint32_t* pPalette = m_pSrcPalette.get();
  if (pal_count == 2) {
    pPalette[0] = bIsSrcCmyk ? 255 : 0xff000000;
    pPalette[1] = bIsSrcCmyk ? 0 : 0xffffffff;
  } else {
    for (int i = 0; i < pal_count; ++i)
      pPalette[i] = bIsSrcCmyk ? FX_CCOLOR(i) : (i * 0x10101);
  }
  if (bIsSrcCmyk != bIsDstCmyk) {
    for (int i = 0; i < pal_count; ++i) {
      FX_CMYK cmyk = pPalette[i];
      uint8_t r;
      uint8_t g;
      uint8_t b;
      std::tie(r, g, b) =
          AdobeCMYK_to_sRGB1(FXSYS_GetCValue(cmyk), FXSYS_GetMValue(cmyk),
                             FXSYS_GetYValue(cmyk), FXSYS_GetKValue(cmyk));
      pPalette[i] = ArgbEncode(0xff, r, g, b);
    }
  }
}

void CFX_ScanlineCompositor::CompositeRgbBitmapLine(
    uint8_t* dest_scan,
    const uint8_t* src_scan,
    int width,
    const uint8_t* clip_scan,
    const uint8_t* src_extra_alpha,
    uint8_t* dst_extra_alpha) {
  int src_Bpp = (m_SrcFormat & 0xff) >> 3;
  int dest_Bpp = (m_DestFormat & 0xff) >> 3;
  if (m_bRgbByteOrder) {
    switch (m_iTransparency) {
      case 0:
      case 4:
      case 8:
      case 12:
        CompositeRow_Argb2Argb_RgbByteOrder(dest_scan, src_scan, width,
                                            m_BlendType, clip_scan);
        break;
      case 1:
        CompositeRow_Rgb2Argb_Blend_NoClip_RgbByteOrder(
            dest_scan, src_scan, width, m_BlendType, src_Bpp);
        break;
      case 2:
      case 10:
        CompositeRow_Argb2Rgb_Blend_RgbByteOrder(
            dest_scan, src_scan, width, m_BlendType, dest_Bpp, clip_scan);
        break;
      case 3:
        CompositeRow_Rgb2Rgb_Blend_NoClip_RgbByteOrder(
            dest_scan, src_scan, width, m_BlendType, dest_Bpp, src_Bpp);
        break;
      case 5:
        CompositeRow_Rgb2Argb_NoBlend_NoClip_RgbByteOrder(dest_scan, src_scan,
                                                          width, src_Bpp);
        break;
      case 6:
      case 14:
        CompositeRow_Argb2Rgb_NoBlend_RgbByteOrder(dest_scan, src_scan, width,
                                                   dest_Bpp, clip_scan);
        break;
      case 7:
        CompositeRow_Rgb2Rgb_NoBlend_NoClip_RgbByteOrder(
            dest_scan, src_scan, width, dest_Bpp, src_Bpp);
        break;
      case 9:
        CompositeRow_Rgb2Argb_Blend_Clip_RgbByteOrder(
            dest_scan, src_scan, width, m_BlendType, src_Bpp, clip_scan);
        break;
      case 11:
        CompositeRow_Rgb2Rgb_Blend_Clip_RgbByteOrder(dest_scan, src_scan, width,
                                                     m_BlendType, dest_Bpp,
                                                     src_Bpp, clip_scan);
        break;
      case 13:
        CompositeRow_Rgb2Argb_NoBlend_Clip_RgbByteOrder(
            dest_scan, src_scan, width, src_Bpp, clip_scan);
        break;
      case 15:
        CompositeRow_Rgb2Rgb_NoBlend_Clip_RgbByteOrder(
            dest_scan, src_scan, width, dest_Bpp, src_Bpp, clip_scan);
        break;
    }
    return;
  }
  if (m_DestFormat == FXDIB_8bppMask) {
    if (m_SrcFormat & 0x0200) {
      if (m_SrcFormat == FXDIB_Argb) {
        CompositeRow_AlphaToMask(dest_scan, src_scan, width, clip_scan, 4);
      } else {
        CompositeRow_AlphaToMask(dest_scan, src_extra_alpha, width, clip_scan,
                                 1);
      }
    } else {
      CompositeRow_Rgb2Mask(dest_scan, src_scan, width, clip_scan);
    }
  } else if ((m_DestFormat & 0xff) == 8) {
    if (m_DestFormat & 0x0400) {
      for (int i = 0; i < width; ++i) {
        *dest_scan = ~*dest_scan;
        dest_scan++;
      }
    }
    if (m_SrcFormat & 0x0200) {
      if (m_DestFormat & 0x0200) {
        CompositeRow_Argb2Graya(dest_scan, src_scan, width, m_BlendType,
                                clip_scan, src_extra_alpha, dst_extra_alpha);
      } else {
        CompositeRow_Argb2Gray(dest_scan, src_scan, width, m_BlendType,
                               clip_scan, src_extra_alpha);
      }
    } else {
      if (m_DestFormat & 0x0200) {
        CompositeRow_Rgb2Graya(dest_scan, src_scan, src_Bpp, width, m_BlendType,
                               clip_scan, dst_extra_alpha);
      } else {
        CompositeRow_Rgb2Gray(dest_scan, src_scan, src_Bpp, width, m_BlendType,
                              clip_scan);
      }
    }
    if (m_DestFormat & 0x0400) {
      for (int i = 0; i < width; ++i) {
        *dest_scan = ~*dest_scan;
        dest_scan++;
      }
    }
  } else {
    switch (m_iTransparency) {
      case 0:
      case 4:
      case 8:
      case 4 + 8: {
        CompositeRow_Argb2Argb(dest_scan, src_scan, width, m_BlendType,
                               clip_scan, dst_extra_alpha, src_extra_alpha);
      } break;
      case 1:
        CompositeRow_Rgb2Argb_Blend_NoClip(
            dest_scan, src_scan, width, m_BlendType, src_Bpp, dst_extra_alpha);
        break;
      case 1 + 8:
        CompositeRow_Rgb2Argb_Blend_Clip(dest_scan, src_scan, width,
                                         m_BlendType, src_Bpp, clip_scan,
                                         dst_extra_alpha);
        break;
      case 1 + 4:
        CompositeRow_Rgb2Argb_NoBlend_NoClip(dest_scan, src_scan, width,
                                             src_Bpp, dst_extra_alpha);
        break;
      case 1 + 4 + 8:
        CompositeRow_Rgb2Argb_NoBlend_Clip(dest_scan, src_scan, width, src_Bpp,
                                           clip_scan, dst_extra_alpha);
        break;
      case 2:
      case 2 + 8:
        CompositeRow_Argb2Rgb_Blend(dest_scan, src_scan, width, m_BlendType,
                                    dest_Bpp, clip_scan, src_extra_alpha);
        break;
      case 2 + 4:
      case 2 + 4 + 8:
        CompositeRow_Argb2Rgb_NoBlend(dest_scan, src_scan, width, dest_Bpp,
                                      clip_scan, src_extra_alpha);
        break;
      case 1 + 2:
        CompositeRow_Rgb2Rgb_Blend_NoClip(dest_scan, src_scan, width,
                                          m_BlendType, dest_Bpp, src_Bpp);
        break;
      case 1 + 2 + 8:
        CompositeRow_Rgb2Rgb_Blend_Clip(dest_scan, src_scan, width, m_BlendType,
                                        dest_Bpp, src_Bpp, clip_scan);
        break;
      case 1 + 2 + 4:
        CompositeRow_Rgb2Rgb_NoBlend_NoClip(dest_scan, src_scan, width,
                                            dest_Bpp, src_Bpp);
        break;
      case 1 + 2 + 4 + 8:
        CompositeRow_Rgb2Rgb_NoBlend_Clip(dest_scan, src_scan, width, dest_Bpp,
                                          src_Bpp, clip_scan);
        break;
    }
  }
}

void CFX_ScanlineCompositor::CompositePalBitmapLine(
    uint8_t* dest_scan,
    const uint8_t* src_scan,
    int src_left,
    int width,
    const uint8_t* clip_scan,
    const uint8_t* src_extra_alpha,
    uint8_t* dst_extra_alpha) {
  if (m_bRgbByteOrder) {
    if (m_SrcFormat == FXDIB_1bppRgb) {
      if (m_DestFormat == FXDIB_8bppRgb) {
        return;
      }
      if (m_DestFormat == FXDIB_Argb) {
        CompositeRow_1bppRgb2Argb_NoBlend_RgbByteOrder(
            dest_scan, src_scan, src_left, width, m_pSrcPalette.get(),
            clip_scan);
      } else {
        CompositeRow_1bppRgb2Rgb_NoBlend_RgbByteOrder(
            dest_scan, src_scan, src_left, m_pSrcPalette.get(), width,
            (m_DestFormat & 0xff) >> 3, clip_scan);
      }
    } else {
      if (m_DestFormat == FXDIB_8bppRgb) {
        return;
      }
      if (m_DestFormat == FXDIB_Argb) {
        CompositeRow_8bppRgb2Argb_NoBlend_RgbByteOrder(
            dest_scan, src_scan, width, m_pSrcPalette.get(), clip_scan);
      } else {
        CompositeRow_8bppRgb2Rgb_NoBlend_RgbByteOrder(
            dest_scan, src_scan, m_pSrcPalette.get(), width,
            (m_DestFormat & 0xff) >> 3, clip_scan);
      }
    }
    return;
  }
  if (m_DestFormat == FXDIB_8bppMask) {
    CompositeRow_Rgb2Mask(dest_scan, src_scan, width, clip_scan);
    return;
  }
  if ((m_DestFormat & 0xff) == 8) {
    if (m_iTransparency & 8) {
      if (m_DestFormat & 0x0200) {
        CompositeRow_1bppPal2Graya(
            dest_scan, src_scan, src_left,
            reinterpret_cast<const uint8_t*>(m_pSrcPalette.get()), width,
            m_BlendType, clip_scan, dst_extra_alpha);
      } else {
        CompositeRow_1bppPal2Gray(
            dest_scan, src_scan, src_left,
            reinterpret_cast<const uint8_t*>(m_pSrcPalette.get()), width,
            m_BlendType, clip_scan);
      }
    } else {
      if (m_DestFormat & 0x0200)
        CompositeRow_8bppPal2Graya(
            dest_scan, src_scan,
            reinterpret_cast<const uint8_t*>(m_pSrcPalette.get()), width,
            m_BlendType, clip_scan, dst_extra_alpha, src_extra_alpha);
      else
        CompositeRow_8bppPal2Gray(
            dest_scan, src_scan,
            reinterpret_cast<const uint8_t*>(m_pSrcPalette.get()), width,
            m_BlendType, clip_scan, src_extra_alpha);
    }
  } else {
    switch (m_iTransparency) {
      case 1 + 2:
        CompositeRow_8bppRgb2Argb_NoBlend(dest_scan, src_scan, width,
                                          m_pSrcPalette.get(), clip_scan,
                                          src_extra_alpha);
        break;
      case 1 + 2 + 8:
        CompositeRow_1bppRgb2Argb_NoBlend(dest_scan, src_scan, src_left, width,
                                          m_pSrcPalette.get(), clip_scan);
        break;
      case 0:
        CompositeRow_8bppRgb2Rgb_NoBlend(
            dest_scan, src_scan, m_pSrcPalette.get(), width,
            (m_DestFormat & 0xff) >> 3, clip_scan, src_extra_alpha);
        break;
      case 0 + 8:
        CompositeRow_1bppRgb2Rgb_NoBlend(dest_scan, src_scan, src_left,
                                         m_pSrcPalette.get(), width,
                                         (m_DestFormat & 0xff) >> 3, clip_scan);
        break;
      case 0 + 2:
        CompositeRow_8bppRgb2Rgb_NoBlend(
            dest_scan, src_scan, m_pSrcPalette.get(), width,
            (m_DestFormat & 0xff) >> 3, clip_scan, src_extra_alpha);
        break;
      case 0 + 2 + 8:
        CompositeRow_1bppRgb2Rgba_NoBlend(dest_scan, src_scan, src_left, width,
                                          m_pSrcPalette.get(), clip_scan,
                                          dst_extra_alpha);
        break;
        break;
    }
  }
}

void CFX_ScanlineCompositor::CompositeByteMaskLine(uint8_t* dest_scan,
                                                   const uint8_t* src_scan,
                                                   int width,
                                                   const uint8_t* clip_scan,
                                                   uint8_t* dst_extra_alpha) {
  if (m_DestFormat == FXDIB_8bppMask) {
    CompositeRow_ByteMask2Mask(dest_scan, src_scan, m_MaskAlpha, width,
                               clip_scan);
  } else if ((m_DestFormat & 0xff) == 8) {
    if (m_DestFormat & 0x0200) {
      CompositeRow_ByteMask2Graya(dest_scan, src_scan, m_MaskAlpha, m_MaskRed,
                                  width, clip_scan, dst_extra_alpha);
    } else {
      CompositeRow_ByteMask2Gray(dest_scan, src_scan, m_MaskAlpha, m_MaskRed,
                                 width, clip_scan);
    }
  } else if (m_bRgbByteOrder) {
    if (m_DestFormat == FXDIB_Argb) {
      CompositeRow_ByteMask2Argb_RgbByteOrder(
          dest_scan, src_scan, m_MaskAlpha, m_MaskRed, m_MaskGreen, m_MaskBlue,
          width, m_BlendType, clip_scan);
    } else {
      CompositeRow_ByteMask2Rgb_RgbByteOrder(
          dest_scan, src_scan, m_MaskAlpha, m_MaskRed, m_MaskGreen, m_MaskBlue,
          width, m_BlendType, (m_DestFormat & 0xff) >> 3, clip_scan);
    }
    return;
  } else if (m_DestFormat == FXDIB_Argb) {
    CompositeRow_ByteMask2Argb(dest_scan, src_scan, m_MaskAlpha, m_MaskRed,
                               m_MaskGreen, m_MaskBlue, width, m_BlendType,
                               clip_scan);
  } else if (m_DestFormat == FXDIB_Rgb || m_DestFormat == FXDIB_Rgb32) {
    CompositeRow_ByteMask2Rgb(dest_scan, src_scan, m_MaskAlpha, m_MaskRed,
                              m_MaskGreen, m_MaskBlue, width, m_BlendType,
                              (m_DestFormat & 0xff) >> 3, clip_scan);
  } else if (m_DestFormat == FXDIB_Rgba) {
    CompositeRow_ByteMask2Rgba(dest_scan, src_scan, m_MaskAlpha, m_MaskRed,
                               m_MaskGreen, m_MaskBlue, width, m_BlendType,
                               clip_scan, dst_extra_alpha);
  }
}

void CFX_ScanlineCompositor::CompositeBitMaskLine(uint8_t* dest_scan,
                                                  const uint8_t* src_scan,
                                                  int src_left,
                                                  int width,
                                                  const uint8_t* clip_scan,
                                                  uint8_t* dst_extra_alpha) {
  if (m_DestFormat == FXDIB_8bppMask) {
    CompositeRow_BitMask2Mask(dest_scan, src_scan, m_MaskAlpha, src_left, width,
                              clip_scan);
  } else if ((m_DestFormat & 0xff) == 8) {
    if (m_DestFormat & 0x0200) {
      CompositeRow_BitMask2Graya(dest_scan, src_scan, m_MaskAlpha, m_MaskRed,
                                 src_left, width, clip_scan, dst_extra_alpha);
    } else {
      CompositeRow_BitMask2Gray(dest_scan, src_scan, m_MaskAlpha, m_MaskRed,
                                src_left, width, clip_scan);
    }
  } else if (m_bRgbByteOrder) {
    if (m_DestFormat == FXDIB_Argb) {
      CompositeRow_BitMask2Argb_RgbByteOrder(
          dest_scan, src_scan, m_MaskAlpha, m_MaskRed, m_MaskGreen, m_MaskBlue,
          src_left, width, m_BlendType, clip_scan);
    } else {
      CompositeRow_BitMask2Rgb_RgbByteOrder(
          dest_scan, src_scan, m_MaskAlpha, m_MaskRed, m_MaskGreen, m_MaskBlue,
          src_left, width, m_BlendType, (m_DestFormat & 0xff) >> 3, clip_scan);
    }
    return;
  } else if (m_DestFormat == FXDIB_Argb) {
    CompositeRow_BitMask2Argb(dest_scan, src_scan, m_MaskAlpha, m_MaskRed,
                              m_MaskGreen, m_MaskBlue, src_left, width,
                              m_BlendType, clip_scan);
  } else if (m_DestFormat == FXDIB_Rgb || m_DestFormat == FXDIB_Rgb32) {
    CompositeRow_BitMask2Rgb(
        dest_scan, src_scan, m_MaskAlpha, m_MaskRed, m_MaskGreen, m_MaskBlue,
        src_left, width, m_BlendType, (m_DestFormat & 0xff) >> 3, clip_scan);
  }
}
