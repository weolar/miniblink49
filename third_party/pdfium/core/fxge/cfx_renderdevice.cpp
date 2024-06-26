// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_renderdevice.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "core/fxcrt/fx_safe_types.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/cfx_facecache.h"
#include "core/fxge/cfx_font.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/dib/cfx_imagerenderer.h"
#include "core/fxge/renderdevicedriver_iface.h"

#if defined _SKIA_SUPPORT_ || defined _SKIA_SUPPORT_PATHS_
#include "third_party/skia/include/core/SkTypes.h"
#endif

namespace {

void AdjustGlyphSpace(std::vector<FXTEXT_GLYPHPOS>* pGlyphAndPos) {
  ASSERT(pGlyphAndPos->size() > 1);
  std::vector<FXTEXT_GLYPHPOS>& glyphs = *pGlyphAndPos;
  bool bVertical = glyphs.back().m_Origin.x == glyphs.front().m_Origin.x;
  if (!bVertical && (glyphs.back().m_Origin.y != glyphs.front().m_Origin.y))
    return;

  for (size_t i = glyphs.size() - 1; i > 1; --i) {
    FXTEXT_GLYPHPOS& next = glyphs[i];
    int next_origin = bVertical ? next.m_Origin.y : next.m_Origin.x;
    float next_origin_f = bVertical ? next.m_fOrigin.y : next.m_fOrigin.x;

    FXTEXT_GLYPHPOS& current = glyphs[i - 1];
    int& current_origin = bVertical ? current.m_Origin.y : current.m_Origin.x;
    float current_origin_f =
        bVertical ? current.m_fOrigin.y : current.m_fOrigin.x;

    FX_SAFE_INT32 safe_space = next_origin;
    safe_space -= current_origin;
    if (!safe_space.IsValid())
      continue;

    int space = safe_space.ValueOrDie();
    float space_f = next_origin_f - current_origin_f;
    float error = fabs(space_f) - fabs(static_cast<float>(space));
    if (error <= 0.5f)
      continue;

    FX_SAFE_INT32 safe_origin = current_origin;
    safe_origin += space > 0 ? -1 : 1;
    if (!safe_origin.IsValid())
      continue;

    current_origin = safe_origin.ValueOrDie();
  }
}

const uint8_t g_TextGammaAdjust[256] = {
    0,   2,   3,   4,   6,   7,   8,   10,  11,  12,  13,  15,  16,  17,  18,
    19,  21,  22,  23,  24,  25,  26,  27,  29,  30,  31,  32,  33,  34,  35,
    36,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  51,  52,
    53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,
    68,  69,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,
    84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,
    99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113,
    114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
    129, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
    143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 156,
    157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
    172, 173, 174, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185,
    186, 187, 188, 189, 190, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213,
    214, 215, 216, 217, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
    228, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 239, 240,
    241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 250, 251, 252, 253, 254,
    255,
};

int TextGammaAdjust(int value) {
  ASSERT(value >= 0);
  ASSERT(value <= 255);
  return g_TextGammaAdjust[value];
}

int CalcAlpha(int src, int alpha) {
  return src * alpha / 255;
}

void Merge(uint8_t src, int channel, int alpha, uint8_t* dest) {
  *dest = FXDIB_ALPHA_MERGE(*dest, channel, CalcAlpha(src, alpha));
}

void MergeGammaAdjust(uint8_t src, int channel, int alpha, uint8_t* dest) {
  *dest =
      FXDIB_ALPHA_MERGE(*dest, channel, CalcAlpha(TextGammaAdjust(src), alpha));
}

void MergeGammaAdjustBgr(const uint8_t* src,
                         int r,
                         int g,
                         int b,
                         int a,
                         uint8_t* dest) {
  MergeGammaAdjust(src[0], b, a, &dest[0]);
  MergeGammaAdjust(src[1], g, a, &dest[1]);
  MergeGammaAdjust(src[2], r, a, &dest[2]);
}

void MergeGammaAdjustRgb(const uint8_t* src,
                         int r,
                         int g,
                         int b,
                         int a,
                         uint8_t* dest) {
  MergeGammaAdjust(src[2], b, a, &dest[0]);
  MergeGammaAdjust(src[1], g, a, &dest[1]);
  MergeGammaAdjust(src[0], r, a, &dest[2]);
}

int AverageRgb(const uint8_t* src) {
  return (src[0] + src[1] + src[2]) / 3;
}

uint8_t CalculateDestAlpha(uint8_t back_alpha, int src_alpha) {
  return back_alpha + src_alpha - back_alpha * src_alpha / 255;
}

void ApplyAlpha(uint8_t* dest, int b, int g, int r, int alpha) {
  dest[0] = FXDIB_ALPHA_MERGE(dest[0], b, alpha);
  dest[1] = FXDIB_ALPHA_MERGE(dest[1], g, alpha);
  dest[2] = FXDIB_ALPHA_MERGE(dest[2], r, alpha);
}

void ApplyDestAlpha(uint8_t back_alpha,
                    int src_alpha,
                    int r,
                    int g,
                    int b,
                    uint8_t* dest) {
  uint8_t dest_alpha = CalculateDestAlpha(back_alpha, src_alpha);
  ApplyAlpha(dest, b, g, r, src_alpha * 255 / dest_alpha);
  dest[3] = dest_alpha;
}

void NormalizeArgb(int src_value,
                   int r,
                   int g,
                   int b,
                   int a,
                   uint8_t* dest,
                   int src_alpha) {
  uint8_t back_alpha = dest[3];
  if (back_alpha == 0)
    FXARGB_SETDIB(dest, ArgbEncode(src_alpha, r, g, b));
  else if (src_alpha != 0)
    ApplyDestAlpha(back_alpha, src_alpha, r, g, b, dest);
}

void NormalizeDest(bool has_alpha,
                   int src_value,
                   int r,
                   int g,
                   int b,
                   int a,
                   uint8_t* dest) {
  if (has_alpha) {
    NormalizeArgb(src_value, r, g, b, a, dest,
                  CalcAlpha(TextGammaAdjust(src_value), a));
    return;
  }
  int src_alpha = CalcAlpha(TextGammaAdjust(src_value), a);
  if (src_alpha == 0)
    return;

  ApplyAlpha(dest, b, g, r, src_alpha);
}

void NormalizeSrc(bool has_alpha,
                  int src_value,
                  int r,
                  int g,
                  int b,
                  int a,
                  uint8_t* dest) {
  if (!has_alpha) {
    ApplyAlpha(dest, b, g, r, CalcAlpha(TextGammaAdjust(src_value), a));
    return;
  }
  int src_alpha = CalcAlpha(TextGammaAdjust(src_value), a);
  if (src_alpha != 0)
    NormalizeArgb(src_value, r, g, b, a, dest, src_alpha);
}

void NextPixel(uint8_t** src_scan, uint8_t** dst_scan, int bpp) {
  *src_scan += 3;
  *dst_scan += bpp;
}

void SetAlpha(bool has_alpha, uint8_t* alpha) {
  if (has_alpha)
    alpha[3] = 255;
}

void DrawNormalTextHelper(const RetainPtr<CFX_DIBitmap>& bitmap,
                          const RetainPtr<CFX_DIBitmap>& pGlyph,
                          int nrows,
                          int left,
                          int top,
                          int start_col,
                          int end_col,
                          bool bNormal,
                          bool bBGRStripe,
                          int x_subpixel,
                          int a,
                          int r,
                          int g,
                          int b) {
  const bool has_alpha = bitmap->GetFormat() == FXDIB_Argb;
  uint8_t* src_buf = pGlyph->GetBuffer();
  int src_pitch = pGlyph->GetPitch();
  uint8_t* dest_buf = bitmap->GetBuffer();
  int dest_pitch = bitmap->GetPitch();
  const int Bpp = has_alpha ? 4 : bitmap->GetBPP() / 8;
  for (int row = 0; row < nrows; ++row) {
    int dest_row = row + top;
    if (dest_row < 0 || dest_row >= bitmap->GetHeight())
      continue;

    uint8_t* src_scan = src_buf + row * src_pitch + (start_col - left) * 3;
    uint8_t* dest_scan = dest_buf + dest_row * dest_pitch + start_col * Bpp;
    if (bBGRStripe) {
      if (x_subpixel == 0) {
        for (int col = start_col; col < end_col; ++col) {
          if (has_alpha) {
            Merge(src_scan[2], r, a, &dest_scan[2]);
            Merge(src_scan[1], g, a, &dest_scan[1]);
            Merge(src_scan[0], b, a, &dest_scan[0]);
          } else {
            MergeGammaAdjustBgr(&src_scan[0], r, g, b, a, &dest_scan[0]);
          }
          SetAlpha(has_alpha, dest_scan);
          NextPixel(&src_scan, &dest_scan, Bpp);
        }
        continue;
      }
      if (x_subpixel == 1) {
        MergeGammaAdjust(src_scan[1], r, a, &dest_scan[2]);
        MergeGammaAdjust(src_scan[0], g, a, &dest_scan[1]);
        if (start_col > left)
          MergeGammaAdjust(src_scan[-1], b, a, &dest_scan[0]);
        SetAlpha(has_alpha, dest_scan);
        NextPixel(&src_scan, &dest_scan, Bpp);
        for (int col = start_col + 1; col < end_col - 1; ++col) {
          MergeGammaAdjustBgr(&src_scan[-1], r, g, b, a, &dest_scan[0]);
          SetAlpha(has_alpha, dest_scan);
          NextPixel(&src_scan, &dest_scan, Bpp);
        }
        continue;
      }
      MergeGammaAdjust(src_scan[0], r, a, &dest_scan[2]);
      if (start_col > left) {
        MergeGammaAdjust(src_scan[-1], g, a, &dest_scan[1]);
        MergeGammaAdjust(src_scan[-2], b, a, &dest_scan[0]);
      }
      SetAlpha(has_alpha, dest_scan);
      NextPixel(&src_scan, &dest_scan, Bpp);
      for (int col = start_col + 1; col < end_col - 1; ++col) {
        MergeGammaAdjustBgr(&src_scan[-2], r, g, b, a, &dest_scan[0]);
        SetAlpha(has_alpha, dest_scan);
        NextPixel(&src_scan, &dest_scan, Bpp);
      }
      continue;
    }
    if (x_subpixel == 0) {
      for (int col = start_col; col < end_col; ++col) {
        if (bNormal) {
          int src_value = AverageRgb(&src_scan[0]);
          NormalizeDest(has_alpha, src_value, r, g, b, a, dest_scan);
        } else {
          MergeGammaAdjustRgb(&src_scan[0], r, g, b, a, &dest_scan[0]);
          SetAlpha(has_alpha, dest_scan);
        }
        NextPixel(&src_scan, &dest_scan, Bpp);
      }
      continue;
    }
    if (x_subpixel == 1) {
      if (bNormal) {
        int src_value = start_col > left ? AverageRgb(&src_scan[-1])
                                         : (src_scan[0] + src_scan[1]) / 3;
        NormalizeSrc(has_alpha, src_value, r, g, b, a, dest_scan);
      } else {
        if (start_col > left)
          MergeGammaAdjust(src_scan[-1], r, a, &dest_scan[2]);
        MergeGammaAdjust(src_scan[0], g, a, &dest_scan[1]);
        MergeGammaAdjust(src_scan[1], b, a, &dest_scan[0]);
        SetAlpha(has_alpha, dest_scan);
      }
      NextPixel(&src_scan, &dest_scan, Bpp);
      for (int col = start_col + 1; col < end_col; ++col) {
        if (bNormal) {
          int src_value = AverageRgb(&src_scan[-1]);
          NormalizeDest(has_alpha, src_value, r, g, b, a, dest_scan);
        } else {
          MergeGammaAdjustRgb(&src_scan[-1], r, g, b, a, &dest_scan[0]);
          SetAlpha(has_alpha, dest_scan);
        }
        NextPixel(&src_scan, &dest_scan, Bpp);
      }
      continue;
    }
    if (bNormal) {
      int src_value =
          start_col > left ? AverageRgb(&src_scan[-2]) : src_scan[0] / 3;
      NormalizeSrc(has_alpha, src_value, r, g, b, a, dest_scan);
    } else {
      if (start_col > left) {
        MergeGammaAdjust(src_scan[-2], r, a, &dest_scan[2]);
        MergeGammaAdjust(src_scan[-1], g, a, &dest_scan[1]);
      }
      MergeGammaAdjust(src_scan[0], b, a, &dest_scan[0]);
      SetAlpha(has_alpha, dest_scan);
    }
    NextPixel(&src_scan, &dest_scan, Bpp);
    for (int col = start_col + 1; col < end_col; ++col) {
      if (bNormal) {
        int src_value = AverageRgb(&src_scan[-2]);
        NormalizeDest(has_alpha, src_value, r, g, b, a, dest_scan);
      } else {
        MergeGammaAdjustRgb(&src_scan[-2], r, g, b, a, &dest_scan[0]);
        SetAlpha(has_alpha, dest_scan);
      }
      NextPixel(&src_scan, &dest_scan, Bpp);
    }
  }
}

bool ShouldDrawDeviceText(const CFX_Font* pFont, uint32_t text_flags) {
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  if (text_flags & FXFONT_CIDFONT)
    return false;

  const ByteString bsPsName = pFont->GetPsName();
  if (bsPsName.Contains("+ZJHL"))
    return false;

  if (bsPsName == "CNAAJI+cmex10")
    return false;
#endif
  return true;
}

}  // namespace

FXTEXT_CHARPOS::FXTEXT_CHARPOS()
    : m_Unicode(0),
      m_GlyphIndex(0),
      m_FontCharWidth(0),
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
      m_ExtGID(0),
#endif
      m_FallbackFontPosition(0),
      m_bGlyphAdjust(false),
      m_bFontStyle(false) {
}

FXTEXT_CHARPOS::FXTEXT_CHARPOS(const FXTEXT_CHARPOS&) = default;

FXTEXT_CHARPOS::~FXTEXT_CHARPOS() = default;

CFX_RenderDevice::CFX_RenderDevice() = default;

CFX_RenderDevice::~CFX_RenderDevice() {
  RestoreState(false);
#if defined _SKIA_SUPPORT_ || defined _SKIA_SUPPORT_PATHS_
  Flush(true);
#endif
}

// static
CFX_Matrix CFX_RenderDevice::GetFlipMatrix(float width,
                                           float height,
                                           float left,
                                           float top) {
  return CFX_Matrix(width, 0, 0, -height, left, top + height);
}

#if defined _SKIA_SUPPORT_ || defined _SKIA_SUPPORT_PATHS_
void CFX_RenderDevice::Flush(bool release) {
  if (release)
    m_pDeviceDriver.reset();
  else
    m_pDeviceDriver->Flush();
}
#endif

void CFX_RenderDevice::SetDeviceDriver(
    std::unique_ptr<RenderDeviceDriverIface> pDriver) {
  m_pDeviceDriver = std::move(pDriver);
  InitDeviceInfo();
}

void CFX_RenderDevice::InitDeviceInfo() {
  m_Width = m_pDeviceDriver->GetDeviceCaps(FXDC_PIXEL_WIDTH);
  m_Height = m_pDeviceDriver->GetDeviceCaps(FXDC_PIXEL_HEIGHT);
  m_bpp = m_pDeviceDriver->GetDeviceCaps(FXDC_BITS_PIXEL);
  m_RenderCaps = m_pDeviceDriver->GetDeviceCaps(FXDC_RENDER_CAPS);
  m_DeviceClass = m_pDeviceDriver->GetDeviceCaps(FXDC_DEVICE_CLASS);
  if (!m_pDeviceDriver->GetClipBox(&m_ClipBox)) {
    m_ClipBox.left = 0;
    m_ClipBox.top = 0;
    m_ClipBox.right = m_Width;
    m_ClipBox.bottom = m_Height;
  }
}

void CFX_RenderDevice::SaveState() {
  m_pDeviceDriver->SaveState();
}

void CFX_RenderDevice::RestoreState(bool bKeepSaved) {
  if (m_pDeviceDriver) {
    m_pDeviceDriver->RestoreState(bKeepSaved);
    UpdateClipBox();
  }
}

int CFX_RenderDevice::GetDeviceCaps(int caps_id) const {
  return m_pDeviceDriver->GetDeviceCaps(caps_id);
}

RetainPtr<CFX_DIBitmap> CFX_RenderDevice::GetBitmap() const {
  return m_pBitmap;
}

void CFX_RenderDevice::SetBitmap(const RetainPtr<CFX_DIBitmap>& pBitmap) {
  m_pBitmap = pBitmap;
}

bool CFX_RenderDevice::CreateCompatibleBitmap(
    const RetainPtr<CFX_DIBitmap>& pDIB,
    int width,
    int height) const {
  if (m_RenderCaps & FXRC_CMYK_OUTPUT) {
    return pDIB->Create(
        width, height,
        m_RenderCaps & FXRC_ALPHA_OUTPUT ? FXDIB_Cmyka : FXDIB_Cmyk);
  }
  if (m_RenderCaps & FXRC_BYTEMASK_OUTPUT)
    return pDIB->Create(width, height, FXDIB_8bppMask);
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_ || defined _SKIA_SUPPORT_PATHS_
  return pDIB->Create(
      width, height,
      m_RenderCaps & FXRC_ALPHA_OUTPUT ? FXDIB_Argb : FXDIB_Rgb32);
#else
  return pDIB->Create(
      width, height, m_RenderCaps & FXRC_ALPHA_OUTPUT ? FXDIB_Argb : FXDIB_Rgb);
#endif
}

bool CFX_RenderDevice::SetClip_PathFill(const CFX_PathData* pPathData,
                                        const CFX_Matrix* pObject2Device,
                                        int fill_mode) {
  if (!m_pDeviceDriver->SetClip_PathFill(pPathData, pObject2Device,
                                         fill_mode)) {
    return false;
  }
  UpdateClipBox();
  return true;
}

bool CFX_RenderDevice::SetClip_PathStroke(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState) {
  if (!m_pDeviceDriver->SetClip_PathStroke(pPathData, pObject2Device,
                                           pGraphState)) {
    return false;
  }
  UpdateClipBox();
  return true;
}

#ifdef PDF_ENABLE_XFA
bool CFX_RenderDevice::SetClip_Rect(const CFX_RectF& rtClip) {
  return SetClip_Rect(FX_RECT(static_cast<int32_t>(floor(rtClip.left)),
                              static_cast<int32_t>(floor(rtClip.top)),
                              static_cast<int32_t>(ceil(rtClip.right())),
                              static_cast<int32_t>(ceil(rtClip.bottom()))));
}
#endif

bool CFX_RenderDevice::SetClip_Rect(const FX_RECT& rect) {
  CFX_PathData path;
  path.AppendRect(rect.left, rect.bottom, rect.right, rect.top);
  if (!SetClip_PathFill(&path, nullptr, FXFILL_WINDING))
    return false;

  UpdateClipBox();
  return true;
}

void CFX_RenderDevice::UpdateClipBox() {
  if (m_pDeviceDriver->GetClipBox(&m_ClipBox))
    return;
  m_ClipBox.left = 0;
  m_ClipBox.top = 0;
  m_ClipBox.right = m_Width;
  m_ClipBox.bottom = m_Height;
}

bool CFX_RenderDevice::DrawPathWithBlend(const CFX_PathData* pPathData,
                                         const CFX_Matrix* pObject2Device,
                                         const CFX_GraphStateData* pGraphState,
                                         uint32_t fill_color,
                                         uint32_t stroke_color,
                                         int fill_mode,
                                         BlendMode blend_type) {
  uint8_t stroke_alpha = pGraphState ? FXARGB_A(stroke_color) : 0;
  uint8_t fill_alpha = (fill_mode & 3) ? FXARGB_A(fill_color) : 0;
  const std::vector<FX_PATHPOINT>& pPoints = pPathData->GetPoints();
  if (stroke_alpha == 0 && pPoints.size() == 2) {
    CFX_PointF pos1 = pPoints[0].m_Point;
    CFX_PointF pos2 = pPoints[1].m_Point;
    if (pObject2Device) {
      pos1 = pObject2Device->Transform(pos1);
      pos2 = pObject2Device->Transform(pos2);
    }
    DrawCosmeticLine(pos1, pos2, fill_color, fill_mode, blend_type);
    return true;
  }

  if ((pPoints.size() == 5 || pPoints.size() == 4) && stroke_alpha == 0) {
    CFX_FloatRect rect_f;
    if (!(fill_mode & FXFILL_RECT_AA) &&
        pPathData->IsRect(pObject2Device, &rect_f)) {
      FX_RECT rect_i = rect_f.GetOuterRect();

      // Depending on the top/bottom, left/right values of the rect it's
      // possible to overflow the Width() and Height() calculations. Check that
      // the rect will have valid dimension before continuing.
      if (!rect_i.Valid())
        return false;

      int width = static_cast<int>(ceil(rect_f.right - rect_f.left));
      if (width < 1) {
        width = 1;
        if (rect_i.left == rect_i.right)
          ++rect_i.right;
      }
      int height = static_cast<int>(ceil(rect_f.top - rect_f.bottom));
      if (height < 1) {
        height = 1;
        if (rect_i.bottom == rect_i.top)
          ++rect_i.bottom;
      }
      if (rect_i.Width() >= width + 1) {
        if (rect_f.left - static_cast<float>(rect_i.left) >
            static_cast<float>(rect_i.right) - rect_f.right) {
          ++rect_i.left;
        } else {
          --rect_i.right;
        }
      }
      if (rect_i.Height() >= height + 1) {
        if (rect_f.top - static_cast<float>(rect_i.top) >
            static_cast<float>(rect_i.bottom) - rect_f.bottom) {
          ++rect_i.top;
        } else {
          --rect_i.bottom;
        }
      }
      if (FillRectWithBlend(rect_i, fill_color, blend_type))
        return true;
    }
  }
  if ((fill_mode & 3) && stroke_alpha == 0 && !(fill_mode & FX_FILL_STROKE) &&
      !(fill_mode & FX_FILL_TEXT_MODE)) {
    CFX_PathData newPath;
    bool bThin = false;
    bool setIdentity = false;
    if (pPathData->GetZeroAreaPath(pObject2Device,
                                   !!m_pDeviceDriver->GetDriverType(), &newPath,
                                   &bThin, &setIdentity)) {
      CFX_GraphStateData graphState;
      graphState.m_LineWidth = 0.0f;

      uint32_t strokecolor = fill_color;
      if (bThin)
        strokecolor = (((fill_alpha >> 2) << 24) | (strokecolor & 0x00ffffff));

      const CFX_Matrix* pMatrix = nullptr;
      if (pObject2Device && !pObject2Device->IsIdentity() && !setIdentity)
        pMatrix = pObject2Device;

      int smooth_path = FX_ZEROAREA_FILL;
      if (fill_mode & FXFILL_NOPATHSMOOTH)
        smooth_path |= FXFILL_NOPATHSMOOTH;

      m_pDeviceDriver->DrawPath(&newPath, pMatrix, &graphState, 0, strokecolor,
                                smooth_path, blend_type);
    }
  }
  if ((fill_mode & 3) && fill_alpha && stroke_alpha < 0xff &&
      (fill_mode & FX_FILL_STROKE)) {
    if (m_RenderCaps & FXRC_FILLSTROKE_PATH) {
      return m_pDeviceDriver->DrawPath(pPathData, pObject2Device, pGraphState,
                                       fill_color, stroke_color, fill_mode,
                                       blend_type);
    }
    return DrawFillStrokePath(pPathData, pObject2Device, pGraphState,
                              fill_color, stroke_color, fill_mode, blend_type);
  }
  return m_pDeviceDriver->DrawPath(pPathData, pObject2Device, pGraphState,
                                   fill_color, stroke_color, fill_mode,
                                   blend_type);
}

// This can be removed once PDFium entirely relies on Skia
bool CFX_RenderDevice::DrawFillStrokePath(const CFX_PathData* pPathData,
                                          const CFX_Matrix* pObject2Device,
                                          const CFX_GraphStateData* pGraphState,
                                          uint32_t fill_color,
                                          uint32_t stroke_color,
                                          int fill_mode,
                                          BlendMode blend_type) {
  if (!(m_RenderCaps & FXRC_GET_BITS))
    return false;
  CFX_FloatRect bbox;
  if (pGraphState) {
    bbox = pPathData->GetBoundingBox(pGraphState->m_LineWidth,
                                     pGraphState->m_MiterLimit);
  } else {
    bbox = pPathData->GetBoundingBox();
  }
  if (pObject2Device)
    bbox = pObject2Device->TransformRect(bbox);

  FX_RECT rect = bbox.GetOuterRect();
  if (!rect.Valid())
    return false;

  auto bitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  auto backdrop = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!CreateCompatibleBitmap(bitmap, rect.Width(), rect.Height()))
    return false;

  if (bitmap->HasAlpha()) {
    bitmap->Clear(0);
    backdrop->Copy(bitmap);
  } else {
    if (!m_pDeviceDriver->GetDIBits(bitmap, rect.left, rect.top))
      return false;
    backdrop->Copy(bitmap);
  }
  CFX_DefaultRenderDevice bitmap_device;
  bitmap_device.Attach(bitmap, false, backdrop, true);

  CFX_Matrix matrix;
  if (pObject2Device)
    matrix = *pObject2Device;
  matrix.Translate(-rect.left, -rect.top);
  if (!bitmap_device.GetDeviceDriver()->DrawPath(
          pPathData, &matrix, pGraphState, fill_color, stroke_color, fill_mode,
          blend_type)) {
    return false;
  }
#if defined _SKIA_SUPPORT_ || defined _SKIA_SUPPORT_PATHS_
  bitmap_device.GetDeviceDriver()->Flush();
#endif
  FX_RECT src_rect(0, 0, rect.Width(), rect.Height());
  return m_pDeviceDriver->SetDIBits(bitmap, 0, src_rect, rect.left, rect.top,
                                    BlendMode::kNormal);
}

bool CFX_RenderDevice::FillRectWithBlend(const FX_RECT& rect,
                                         uint32_t fill_color,
                                         BlendMode blend_type) {
  if (m_pDeviceDriver->FillRectWithBlend(rect, fill_color, blend_type))
    return true;

  if (!(m_RenderCaps & FXRC_GET_BITS))
    return false;

  auto bitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!CreateCompatibleBitmap(bitmap, rect.Width(), rect.Height()))
    return false;

  if (!m_pDeviceDriver->GetDIBits(bitmap, rect.left, rect.top))
    return false;

  if (!bitmap->CompositeRect(0, 0, rect.Width(), rect.Height(), fill_color,
                             0)) {
    return false;
  }
  FX_RECT src_rect(0, 0, rect.Width(), rect.Height());
  m_pDeviceDriver->SetDIBits(bitmap, 0, src_rect, rect.left, rect.top,
                             BlendMode::kNormal);
  return true;
}

bool CFX_RenderDevice::DrawCosmeticLine(const CFX_PointF& ptMoveTo,
                                        const CFX_PointF& ptLineTo,
                                        uint32_t color,
                                        int fill_mode,
                                        BlendMode blend_type) {
  if ((color >= 0xff000000) && m_pDeviceDriver->DrawCosmeticLine(
                                   ptMoveTo, ptLineTo, color, blend_type)) {
    return true;
  }
  CFX_GraphStateData graph_state;
  CFX_PathData path;
  path.AppendPoint(ptMoveTo, FXPT_TYPE::MoveTo, false);
  path.AppendPoint(ptLineTo, FXPT_TYPE::LineTo, false);
  return m_pDeviceDriver->DrawPath(&path, nullptr, &graph_state, 0, color,
                                   fill_mode, blend_type);
}

bool CFX_RenderDevice::GetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                                 int left,
                                 int top) {
  return (m_RenderCaps & FXRC_GET_BITS) &&
         m_pDeviceDriver->GetDIBits(pBitmap, left, top);
}

RetainPtr<CFX_DIBitmap> CFX_RenderDevice::GetBackDrop() {
  return m_pDeviceDriver->GetBackDrop();
}

bool CFX_RenderDevice::SetDIBitsWithBlend(const RetainPtr<CFX_DIBBase>& pBitmap,
                                          int left,
                                          int top,
                                          BlendMode blend_mode) {
  ASSERT(!pBitmap->IsAlphaMask());
  FX_RECT dest_rect(left, top, left + pBitmap->GetWidth(),
                    top + pBitmap->GetHeight());
  dest_rect.Intersect(m_ClipBox);
  if (dest_rect.IsEmpty())
    return true;

  FX_RECT src_rect(dest_rect.left - left, dest_rect.top - top,
                   dest_rect.left - left + dest_rect.Width(),
                   dest_rect.top - top + dest_rect.Height());
  if ((blend_mode == BlendMode::kNormal || (m_RenderCaps & FXRC_BLEND_MODE)) &&
      (!pBitmap->HasAlpha() || (m_RenderCaps & FXRC_ALPHA_IMAGE))) {
    return m_pDeviceDriver->SetDIBits(pBitmap, 0, src_rect, dest_rect.left,
                                      dest_rect.top, blend_mode);
  }
  if (!(m_RenderCaps & FXRC_GET_BITS))
    return false;

  int bg_pixel_width = dest_rect.Width();
  int bg_pixel_height = dest_rect.Height();
  auto background = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!background->Create(
          bg_pixel_width, bg_pixel_height,
          (m_RenderCaps & FXRC_CMYK_OUTPUT) ? FXDIB_Cmyk : FXDIB_Rgb32)) {
    return false;
  }
  if (!m_pDeviceDriver->GetDIBits(background, dest_rect.left, dest_rect.top)) {
    return false;
  }
  if (!background->CompositeBitmap(0, 0, bg_pixel_width, bg_pixel_height,
                                   pBitmap, src_rect.left, src_rect.top,
                                   blend_mode, nullptr, false)) {
    return false;
  }
  FX_RECT rect(0, 0, bg_pixel_width, bg_pixel_height);
  return m_pDeviceDriver->SetDIBits(background, 0, rect, dest_rect.left,
                                    dest_rect.top, BlendMode::kNormal);
}

bool CFX_RenderDevice::StretchDIBitsWithFlagsAndBlend(
    const RetainPtr<CFX_DIBBase>& pBitmap,
    int left,
    int top,
    int dest_width,
    int dest_height,
    const FXDIB_ResampleOptions& options,
    BlendMode blend_mode) {
  FX_RECT dest_rect(left, top, left + dest_width, top + dest_height);
  FX_RECT clip_box = m_ClipBox;
  clip_box.Intersect(dest_rect);
  return clip_box.IsEmpty() || m_pDeviceDriver->StretchDIBits(
                                   pBitmap, 0, left, top, dest_width,
                                   dest_height, &clip_box, options, blend_mode);
}

bool CFX_RenderDevice::SetBitMask(const RetainPtr<CFX_DIBBase>& pBitmap,
                                  int left,
                                  int top,
                                  uint32_t argb) {
  FX_RECT src_rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
  return m_pDeviceDriver->SetDIBits(pBitmap, argb, src_rect, left, top,
                                    BlendMode::kNormal);
}

bool CFX_RenderDevice::StretchBitMask(const RetainPtr<CFX_DIBBase>& pBitmap,
                                      int left,
                                      int top,
                                      int dest_width,
                                      int dest_height,
                                      uint32_t color) {
  return StretchBitMaskWithFlags(pBitmap, left, top, dest_width, dest_height,
                                 color, FXDIB_ResampleOptions());
}

bool CFX_RenderDevice::StretchBitMaskWithFlags(
    const RetainPtr<CFX_DIBBase>& pBitmap,
    int left,
    int top,
    int dest_width,
    int dest_height,
    uint32_t argb,
    const FXDIB_ResampleOptions& options) {
  FX_RECT dest_rect(left, top, left + dest_width, top + dest_height);
  FX_RECT clip_box = m_ClipBox;
  clip_box.Intersect(dest_rect);
  return m_pDeviceDriver->StretchDIBits(pBitmap, argb, left, top, dest_width,
                                        dest_height, &clip_box, options,
                                        BlendMode::kNormal);
}

bool CFX_RenderDevice::StartDIBitsWithBlend(
    const RetainPtr<CFX_DIBBase>& pBitmap,
    int bitmap_alpha,
    uint32_t argb,
    const CFX_Matrix& matrix,
    const FXDIB_ResampleOptions& options,
    std::unique_ptr<CFX_ImageRenderer>* handle,
    BlendMode blend_mode) {
  return m_pDeviceDriver->StartDIBits(pBitmap, bitmap_alpha, argb, matrix,
                                      options, handle, blend_mode);
}

bool CFX_RenderDevice::ContinueDIBits(CFX_ImageRenderer* handle,
                                      PauseIndicatorIface* pPause) {
  return m_pDeviceDriver->ContinueDIBits(handle, pPause);
}

#ifdef _SKIA_SUPPORT_
void CFX_RenderDevice::DebugVerifyBitmapIsPreMultiplied() const {
  SkASSERT(0);
}

bool CFX_RenderDevice::SetBitsWithMask(const RetainPtr<CFX_DIBBase>& pBitmap,
                                       const RetainPtr<CFX_DIBBase>& pMask,
                                       int left,
                                       int top,
                                       int bitmap_alpha,
                                       BlendMode blend_type) {
  return m_pDeviceDriver->SetBitsWithMask(pBitmap, pMask, left, top,
                                          bitmap_alpha, blend_type);
}
#endif

bool CFX_RenderDevice::DrawNormalText(int nChars,
                                      const FXTEXT_CHARPOS* pCharPos,
                                      CFX_Font* pFont,
                                      float font_size,
                                      const CFX_Matrix* pText2Device,
                                      uint32_t fill_color,
                                      uint32_t text_flags) {
  int nativetext_flags = text_flags;
  if (m_DeviceClass != FXDC_DISPLAY) {
    if (!(text_flags & FXTEXT_PRINTGRAPHICTEXT)) {
      if (ShouldDrawDeviceText(pFont, text_flags) &&
          m_pDeviceDriver->DrawDeviceText(nChars, pCharPos, pFont, pText2Device,
                                          font_size, fill_color)) {
        return true;
      }
    }
    if (FXARGB_A(fill_color) < 255)
      return false;
  } else if (!(text_flags & FXTEXT_NO_NATIVETEXT)) {
    if (ShouldDrawDeviceText(pFont, text_flags) &&
        m_pDeviceDriver->DrawDeviceText(nChars, pCharPos, pFont, pText2Device,
                                        font_size, fill_color)) {
      return true;
    }
  }
  CFX_Matrix char2device;
  CFX_Matrix text2Device;
  if (pText2Device) {
    char2device = *pText2Device;
    text2Device = *pText2Device;
  }

  char2device.Scale(font_size, -font_size);
  if (fabs(char2device.a) + fabs(char2device.b) > 50 * 1.0f ||
      ((m_DeviceClass == FXDC_PRINTER) &&
       !(text_flags & FXTEXT_PRINTIMAGETEXT))) {
    if (pFont->GetFace()) {
      int nPathFlags =
          (text_flags & FXTEXT_NOSMOOTH) == 0 ? 0 : FXFILL_NOPATHSMOOTH;
      return DrawTextPath(nChars, pCharPos, pFont, font_size, pText2Device,
                          nullptr, nullptr, fill_color, 0, nullptr, nPathFlags);
    }
  }
  int anti_alias = FXFT_RENDER_MODE_MONO;
  bool bNormal = false;
  if ((text_flags & FXTEXT_NOSMOOTH) == 0) {
    if (m_DeviceClass == FXDC_DISPLAY && m_bpp > 1) {
      if (!CFX_GEModule::Get()->GetFontMgr()->FTLibrarySupportsHinting()) {
        // Some Freetype implementations (like the one packaged with Fedora) do
        // not support hinting due to patents 6219025, 6239783, 6307566,
        // 6225973, 6243070, 6393145, 6421054, 6282327, and 6624828; the latest
        // one expires 10/7/19.  This makes LCD antialiasing very ugly, so we
        // instead fall back on NORMAL antialiasing.
        anti_alias = FXFT_RENDER_MODE_NORMAL;
      } else if ((m_RenderCaps & (FXRC_ALPHA_OUTPUT | FXRC_CMYK_OUTPUT))) {
        anti_alias = FXFT_RENDER_MODE_LCD;
        bNormal = true;
      } else if (m_bpp < 16) {
        anti_alias = FXFT_RENDER_MODE_NORMAL;
      } else {
        anti_alias = FXFT_RENDER_MODE_LCD;

        bool bClearType = false;
        if (pFont->GetFace())
          bClearType = !!(text_flags & FXTEXT_CLEARTYPE);
        bNormal = !bClearType;
      }
    }
  }
  std::vector<FXTEXT_GLYPHPOS> glyphs(nChars);
  CFX_Matrix deviceCtm = char2device;

  for (size_t i = 0; i < glyphs.size(); ++i) {
    FXTEXT_GLYPHPOS& glyph = glyphs[i];
    const FXTEXT_CHARPOS& charpos = pCharPos[i];

    glyph.m_fOrigin = text2Device.Transform(charpos.m_Origin);
    if (anti_alias < FXFT_RENDER_MODE_LCD)
      glyph.m_Origin.x = FXSYS_round(glyph.m_fOrigin.x);
    else
      glyph.m_Origin.x = static_cast<int>(floor(glyph.m_fOrigin.x));
    glyph.m_Origin.y = FXSYS_round(glyph.m_fOrigin.y);

    if (charpos.m_bGlyphAdjust) {
      CFX_Matrix new_matrix(
          charpos.m_AdjustMatrix[0], charpos.m_AdjustMatrix[1],
          charpos.m_AdjustMatrix[2], charpos.m_AdjustMatrix[3], 0, 0);
      new_matrix.Concat(deviceCtm);
      glyph.m_pGlyph = pFont->LoadGlyphBitmap(
          charpos.m_GlyphIndex, charpos.m_bFontStyle, new_matrix,
          charpos.m_FontCharWidth, anti_alias, nativetext_flags);
    } else {
      glyph.m_pGlyph = pFont->LoadGlyphBitmap(
          charpos.m_GlyphIndex, charpos.m_bFontStyle, deviceCtm,
          charpos.m_FontCharWidth, anti_alias, nativetext_flags);
    }
  }
  if (anti_alias < FXFT_RENDER_MODE_LCD && glyphs.size() > 1)
    AdjustGlyphSpace(&glyphs);

  FX_RECT bmp_rect = FXGE_GetGlyphsBBox(glyphs, anti_alias);
  bmp_rect.Intersect(m_ClipBox);
  if (bmp_rect.IsEmpty())
    return true;

  int pixel_width = bmp_rect.Width();
  int pixel_height = bmp_rect.Height();
  int pixel_left = bmp_rect.left;
  int pixel_top = bmp_rect.top;
  if (anti_alias == FXFT_RENDER_MODE_MONO) {
    auto bitmap = pdfium::MakeRetain<CFX_DIBitmap>();
    if (!bitmap->Create(pixel_width, pixel_height, FXDIB_1bppMask))
      return false;
    bitmap->Clear(0);
    for (const FXTEXT_GLYPHPOS& glyph : glyphs) {
      if (!glyph.m_pGlyph)
        continue;
      RetainPtr<CFX_DIBitmap> pGlyph = glyph.m_pGlyph->m_pBitmap;
      bitmap->TransferBitmap(
          glyph.m_Origin.x + glyph.m_pGlyph->m_Left - pixel_left,
          glyph.m_Origin.y - glyph.m_pGlyph->m_Top - pixel_top,
          pGlyph->GetWidth(), pGlyph->GetHeight(), pGlyph, 0, 0);
    }
    return SetBitMask(bitmap, bmp_rect.left, bmp_rect.top, fill_color);
  }
  auto bitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (m_bpp == 8) {
    if (!bitmap->Create(pixel_width, pixel_height, FXDIB_8bppMask))
      return false;
  } else {
    if (!CreateCompatibleBitmap(bitmap, pixel_width, pixel_height))
      return false;
  }
  if (!bitmap->HasAlpha() && !bitmap->IsAlphaMask()) {
    bitmap->Clear(0xFFFFFFFF);
    if (!GetDIBits(bitmap, bmp_rect.left, bmp_rect.top))
      return false;
  } else {
    bitmap->Clear(0);
    if (bitmap->m_pAlphaMask)
      bitmap->m_pAlphaMask->Clear(0);
  }
  int dest_width = pixel_width;
  int a = 0;
  int r = 0;
  int g = 0;
  int b = 0;
  if (anti_alias == FXFT_RENDER_MODE_LCD)
    std::tie(a, r, g, b) = ArgbDecode(fill_color);

  for (const FXTEXT_GLYPHPOS& glyph : glyphs) {
    if (!glyph.m_pGlyph)
      continue;

    pdfium::base::CheckedNumeric<int> left = glyph.m_Origin.x;
    left += glyph.m_pGlyph->m_Left;
    left -= pixel_left;
    if (!left.IsValid())
      return false;

    pdfium::base::CheckedNumeric<int> top = glyph.m_Origin.y;
    top -= glyph.m_pGlyph->m_Top;
    top -= pixel_top;
    if (!top.IsValid())
      return false;

    RetainPtr<CFX_DIBitmap> pGlyph = glyph.m_pGlyph->m_pBitmap;
    int ncols = pGlyph->GetWidth();
    int nrows = pGlyph->GetHeight();
    if (anti_alias == FXFT_RENDER_MODE_NORMAL) {
      if (!bitmap->CompositeMask(left.ValueOrDie(), top.ValueOrDie(), ncols,
                                 nrows, pGlyph, fill_color, 0, 0,
                                 BlendMode::kNormal, nullptr, false, 0)) {
        return false;
      }
      continue;
    }
    bool bBGRStripe = !!(text_flags & FXTEXT_BGR_STRIPE);
    ncols /= 3;
    int x_subpixel = static_cast<int>(glyph.m_fOrigin.x * 3) % 3;
    int start_col =
        pdfium::base::ValueOrDieForType<int>(pdfium::base::CheckMax(left, 0));
    pdfium::base::CheckedNumeric<int> end_col_safe = left;
    end_col_safe += ncols;
    if (!end_col_safe.IsValid())
      return false;

    int end_col =
        std::min(static_cast<int>(end_col_safe.ValueOrDie<int>()), dest_width);
    if (start_col >= end_col)
      continue;

    DrawNormalTextHelper(bitmap, pGlyph, nrows, left.ValueOrDie(),
                         top.ValueOrDie(), start_col, end_col, bNormal,
                         bBGRStripe, x_subpixel, a, r, g, b);
  }
  if (bitmap->IsAlphaMask())
    SetBitMask(bitmap, bmp_rect.left, bmp_rect.top, fill_color);
  else
    SetDIBits(bitmap, bmp_rect.left, bmp_rect.top);
  return true;
}

bool CFX_RenderDevice::DrawTextPath(int nChars,
                                    const FXTEXT_CHARPOS* pCharPos,
                                    CFX_Font* pFont,
                                    float font_size,
                                    const CFX_Matrix* pText2User,
                                    const CFX_Matrix* pUser2Device,
                                    const CFX_GraphStateData* pGraphState,
                                    uint32_t fill_color,
                                    FX_ARGB stroke_color,
                                    CFX_PathData* pClippingPath,
                                    int nFlag) {
  for (int iChar = 0; iChar < nChars; ++iChar) {
    const FXTEXT_CHARPOS& charpos = pCharPos[iChar];
    CFX_Matrix matrix;
    if (charpos.m_bGlyphAdjust) {
      matrix = CFX_Matrix(charpos.m_AdjustMatrix[0], charpos.m_AdjustMatrix[1],
                          charpos.m_AdjustMatrix[2], charpos.m_AdjustMatrix[3],
                          0, 0);
    }
    matrix.Concat(CFX_Matrix(font_size, 0, 0, font_size, charpos.m_Origin.x,
                             charpos.m_Origin.y));
    const CFX_PathData* pPath =
        pFont->LoadGlyphPath(charpos.m_GlyphIndex, charpos.m_FontCharWidth);
    if (!pPath)
      continue;

    matrix.Concat(*pText2User);

    CFX_PathData TransformedPath(*pPath);
    TransformedPath.Transform(matrix);
    if (fill_color || stroke_color) {
      int fill_mode = nFlag;
      if (fill_color)
        fill_mode |= FXFILL_WINDING;
      fill_mode |= FX_FILL_TEXT_MODE;
      if (!DrawPathWithBlend(&TransformedPath, pUser2Device, pGraphState,
                             fill_color, stroke_color, fill_mode,
                             BlendMode::kNormal)) {
        return false;
      }
    }
    if (pClippingPath)
      pClippingPath->Append(&TransformedPath, pUser2Device);
  }
  return true;
}

void CFX_RenderDevice::DrawFillRect(const CFX_Matrix* pUser2Device,
                                    const CFX_FloatRect& rect,
                                    const FX_COLORREF& color) {
  CFX_PathData path;
  path.AppendRect(rect);
  DrawPath(&path, pUser2Device, nullptr, color, 0, FXFILL_WINDING);
}

void CFX_RenderDevice::DrawFillArea(const CFX_Matrix& mtUser2Device,
                                    const std::vector<CFX_PointF>& points,
                                    const FX_COLORREF& color) {
  ASSERT(!points.empty());
  CFX_PathData path;
  path.AppendPoint(points[0], FXPT_TYPE::MoveTo, false);
  for (size_t i = 1; i < points.size(); ++i)
    path.AppendPoint(points[i], FXPT_TYPE::LineTo, false);

  DrawPath(&path, &mtUser2Device, nullptr, color, 0, FXFILL_ALTERNATE);
}

void CFX_RenderDevice::DrawStrokeRect(const CFX_Matrix& mtUser2Device,
                                      const CFX_FloatRect& rect,
                                      const FX_COLORREF& color,
                                      float fWidth) {
  CFX_GraphStateData gsd;
  gsd.m_LineWidth = fWidth;

  CFX_PathData path;
  path.AppendRect(rect);
  DrawPath(&path, &mtUser2Device, &gsd, 0, color, FXFILL_ALTERNATE);
}

void CFX_RenderDevice::DrawStrokeLine(const CFX_Matrix* pUser2Device,
                                      const CFX_PointF& ptMoveTo,
                                      const CFX_PointF& ptLineTo,
                                      const FX_COLORREF& color,
                                      float fWidth) {
  CFX_PathData path;
  path.AppendPoint(ptMoveTo, FXPT_TYPE::MoveTo, false);
  path.AppendPoint(ptLineTo, FXPT_TYPE::LineTo, false);

  CFX_GraphStateData gsd;
  gsd.m_LineWidth = fWidth;

  DrawPath(&path, pUser2Device, &gsd, 0, color, FXFILL_ALTERNATE);
}

void CFX_RenderDevice::DrawFillRect(const CFX_Matrix* pUser2Device,
                                    const CFX_FloatRect& rect,
                                    const CFX_Color& color,
                                    int32_t nTransparency) {
  DrawFillRect(pUser2Device, rect, color.ToFXColor(nTransparency));
}

void CFX_RenderDevice::DrawShadow(const CFX_Matrix& mtUser2Device,
                                  bool bVertical,
                                  bool bHorizontal,
                                  const CFX_FloatRect& rect,
                                  int32_t nTransparency,
                                  int32_t nStartGray,
                                  int32_t nEndGray) {
  float fStepGray = 1.0f;

  if (bVertical) {
    fStepGray = (nEndGray - nStartGray) / rect.Height();

    for (float fy = rect.bottom + 0.5f; fy <= rect.top - 0.5f; fy += 1.0f) {
      int32_t nGray = nStartGray + (int32_t)(fStepGray * (fy - rect.bottom));
      DrawStrokeLine(&mtUser2Device, CFX_PointF(rect.left, fy),
                     CFX_PointF(rect.right, fy),
                     ArgbEncode(nTransparency, nGray, nGray, nGray), 1.5f);
    }
  }

  if (bHorizontal) {
    fStepGray = (nEndGray - nStartGray) / rect.Width();

    for (float fx = rect.left + 0.5f; fx <= rect.right - 0.5f; fx += 1.0f) {
      int32_t nGray = nStartGray + (int32_t)(fStepGray * (fx - rect.left));
      DrawStrokeLine(&mtUser2Device, CFX_PointF(fx, rect.bottom),
                     CFX_PointF(fx, rect.top),
                     ArgbEncode(nTransparency, nGray, nGray, nGray), 1.5f);
    }
  }
}

void CFX_RenderDevice::DrawBorder(const CFX_Matrix* pUser2Device,
                                  const CFX_FloatRect& rect,
                                  float fWidth,
                                  const CFX_Color& color,
                                  const CFX_Color& crLeftTop,
                                  const CFX_Color& crRightBottom,
                                  BorderStyle nStyle,
                                  int32_t nTransparency) {
  float fLeft = rect.left;
  float fRight = rect.right;
  float fTop = rect.top;
  float fBottom = rect.bottom;

  if (fWidth > 0.0f) {
    float fHalfWidth = fWidth / 2.0f;

    switch (nStyle) {
      default:
      case BorderStyle::SOLID: {
        CFX_PathData path;
        path.AppendRect(fLeft, fBottom, fRight, fTop);
        path.AppendRect(fLeft + fWidth, fBottom + fWidth, fRight - fWidth,
                        fTop - fWidth);
        DrawPath(&path, pUser2Device, nullptr, color.ToFXColor(nTransparency),
                 0, FXFILL_ALTERNATE);
        break;
      }
      case BorderStyle::DASH: {
        CFX_PathData path;
        path.AppendPoint(
            CFX_PointF(fLeft + fWidth / 2.0f, fBottom + fWidth / 2.0f),
            FXPT_TYPE::MoveTo, false);
        path.AppendPoint(
            CFX_PointF(fLeft + fWidth / 2.0f, fTop - fWidth / 2.0f),
            FXPT_TYPE::LineTo, false);
        path.AppendPoint(
            CFX_PointF(fRight - fWidth / 2.0f, fTop - fWidth / 2.0f),
            FXPT_TYPE::LineTo, false);
        path.AppendPoint(
            CFX_PointF(fRight - fWidth / 2.0f, fBottom + fWidth / 2.0f),
            FXPT_TYPE::LineTo, false);
        path.AppendPoint(
            CFX_PointF(fLeft + fWidth / 2.0f, fBottom + fWidth / 2.0f),
            FXPT_TYPE::LineTo, false);

        CFX_GraphStateData gsd;
        gsd.m_DashArray = {3.0f, 3.0f};
        gsd.m_DashPhase = 0;
        gsd.m_LineWidth = fWidth;
        DrawPath(&path, pUser2Device, &gsd, 0, color.ToFXColor(nTransparency),
                 FXFILL_WINDING);
        break;
      }
      case BorderStyle::BEVELED:
      case BorderStyle::INSET: {
        CFX_GraphStateData gsd;
        gsd.m_LineWidth = fHalfWidth;

        CFX_PathData pathLT;

        pathLT.AppendPoint(CFX_PointF(fLeft + fHalfWidth, fBottom + fHalfWidth),
                           FXPT_TYPE::MoveTo, false);
        pathLT.AppendPoint(CFX_PointF(fLeft + fHalfWidth, fTop - fHalfWidth),
                           FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(CFX_PointF(fRight - fHalfWidth, fTop - fHalfWidth),
                           FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(
            CFX_PointF(fRight - fHalfWidth * 2, fTop - fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(
            CFX_PointF(fLeft + fHalfWidth * 2, fTop - fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(
            CFX_PointF(fLeft + fHalfWidth * 2, fBottom + fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathLT.AppendPoint(CFX_PointF(fLeft + fHalfWidth, fBottom + fHalfWidth),
                           FXPT_TYPE::LineTo, false);

        DrawPath(&pathLT, pUser2Device, &gsd,
                 crLeftTop.ToFXColor(nTransparency), 0, FXFILL_ALTERNATE);

        CFX_PathData pathRB;
        pathRB.AppendPoint(CFX_PointF(fRight - fHalfWidth, fTop - fHalfWidth),
                           FXPT_TYPE::MoveTo, false);
        pathRB.AppendPoint(
            CFX_PointF(fRight - fHalfWidth, fBottom + fHalfWidth),
            FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(CFX_PointF(fLeft + fHalfWidth, fBottom + fHalfWidth),
                           FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(
            CFX_PointF(fLeft + fHalfWidth * 2, fBottom + fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(
            CFX_PointF(fRight - fHalfWidth * 2, fBottom + fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(
            CFX_PointF(fRight - fHalfWidth * 2, fTop - fHalfWidth * 2),
            FXPT_TYPE::LineTo, false);
        pathRB.AppendPoint(CFX_PointF(fRight - fHalfWidth, fTop - fHalfWidth),
                           FXPT_TYPE::LineTo, false);

        DrawPath(&pathRB, pUser2Device, &gsd,
                 crRightBottom.ToFXColor(nTransparency), 0, FXFILL_ALTERNATE);

        CFX_PathData path;

        path.AppendRect(fLeft, fBottom, fRight, fTop);
        path.AppendRect(fLeft + fHalfWidth, fBottom + fHalfWidth,
                        fRight - fHalfWidth, fTop - fHalfWidth);

        DrawPath(&path, pUser2Device, &gsd, color.ToFXColor(nTransparency), 0,
                 FXFILL_ALTERNATE);
        break;
      }
      case BorderStyle::UNDERLINE: {
        CFX_PathData path;
        path.AppendPoint(CFX_PointF(fLeft, fBottom + fWidth / 2),
                         FXPT_TYPE::MoveTo, false);
        path.AppendPoint(CFX_PointF(fRight, fBottom + fWidth / 2),
                         FXPT_TYPE::LineTo, false);

        CFX_GraphStateData gsd;
        gsd.m_LineWidth = fWidth;

        DrawPath(&path, pUser2Device, &gsd, 0, color.ToFXColor(nTransparency),
                 FXFILL_ALTERNATE);
        break;
      }
    }
  }
}

CFX_RenderDevice::StateRestorer::StateRestorer(CFX_RenderDevice* pDevice)
    : m_pDevice(pDevice) {
  m_pDevice->SaveState();
}

CFX_RenderDevice::StateRestorer::~StateRestorer() {
  m_pDevice->RestoreState(false);
}
