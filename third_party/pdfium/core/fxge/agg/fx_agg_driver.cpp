// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/agg/fx_agg_driver.h"

#include <algorithm>
#include <utility>

#include "core/fxge/cfx_cliprgn.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/dib/cfx_imagerenderer.h"
#include "core/fxge/dib/cfx_imagestretcher.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

// Ignore fallthrough warnings in agg23 headers.
#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
#include "third_party/agg23/agg_conv_dash.h"
#include "third_party/agg23/agg_conv_stroke.h"
#include "third_party/agg23/agg_curves.h"
#include "third_party/agg23/agg_path_storage.h"
#include "third_party/agg23/agg_pixfmt_gray.h"
#include "third_party/agg23/agg_rasterizer_scanline_aa.h"
#include "third_party/agg23/agg_renderer_scanline.h"
#include "third_party/agg23/agg_scanline_u.h"
#if defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace {

const float kMaxPos = 32000.0f;

CFX_PointF HardClip(const CFX_PointF& pos) {
  return CFX_PointF(pdfium::clamp(pos.x, -kMaxPos, kMaxPos),
                    pdfium::clamp(pos.y, -kMaxPos, kMaxPos));
}

void RgbByteOrderSetPixel(const RetainPtr<CFX_DIBitmap>& pBitmap,
                          int x,
                          int y,
                          uint32_t argb) {
  if (x < 0 || x >= pBitmap->GetWidth() || y < 0 || y >= pBitmap->GetHeight())
    return;

  uint8_t* pos = pBitmap->GetBuffer() + y * pBitmap->GetPitch() +
                 x * pBitmap->GetBPP() / 8;
  if (pBitmap->GetFormat() == FXDIB_Argb) {
    FXARGB_SETRGBORDERDIB(pos, argb);
    return;
  }

  int alpha = FXARGB_A(argb);
  pos[0] = (FXARGB_R(argb) * alpha + pos[0] * (255 - alpha)) / 255;
  pos[1] = (FXARGB_G(argb) * alpha + pos[1] * (255 - alpha)) / 255;
  pos[2] = (FXARGB_B(argb) * alpha + pos[2] * (255 - alpha)) / 255;
}

void RgbByteOrderCompositeRect(const RetainPtr<CFX_DIBitmap>& pBitmap,
                               int left,
                               int top,
                               int width,
                               int height,
                               FX_ARGB argb) {
  int src_alpha = FXARGB_A(argb);
  if (src_alpha == 0)
    return;

  FX_RECT rect(left, top, left + width, top + height);
  rect.Intersect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
  width = rect.Width();
  int src_r = FXARGB_R(argb);
  int src_g = FXARGB_G(argb);
  int src_b = FXARGB_B(argb);
  int Bpp = pBitmap->GetBPP() / 8;
  int dib_argb = FXARGB_TOBGRORDERDIB(argb);
  uint8_t* pBuffer = pBitmap->GetBuffer();
  if (src_alpha == 255) {
    for (int row = rect.top; row < rect.bottom; row++) {
      uint8_t* dest_scan =
          pBuffer + row * pBitmap->GetPitch() + rect.left * Bpp;
      if (Bpp == 4) {
        uint32_t* scan = reinterpret_cast<uint32_t*>(dest_scan);
        for (int col = 0; col < width; col++)
          *scan++ = dib_argb;
      } else {
        for (int col = 0; col < width; col++) {
          *dest_scan++ = src_r;
          *dest_scan++ = src_g;
          *dest_scan++ = src_b;
        }
      }
    }
    return;
  }
  bool bAlpha = pBitmap->HasAlpha();
  for (int row = rect.top; row < rect.bottom; row++) {
    uint8_t* dest_scan = pBuffer + row * pBitmap->GetPitch() + rect.left * Bpp;
    if (bAlpha) {
      for (int col = 0; col < width; col++) {
        uint8_t back_alpha = dest_scan[3];
        if (back_alpha == 0) {
          FXARGB_SETRGBORDERDIB(dest_scan,
                                ArgbEncode(src_alpha, src_r, src_g, src_b));
          dest_scan += 4;
          continue;
        }
        uint8_t dest_alpha =
            back_alpha + src_alpha - back_alpha * src_alpha / 255;
        dest_scan[3] = dest_alpha;
        int alpha_ratio = src_alpha * 255 / dest_alpha;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, alpha_ratio);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, alpha_ratio);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, alpha_ratio);
        dest_scan += 2;
      }
      continue;
    }
    for (int col = 0; col < width; col++) {
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_r, src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_g, src_alpha);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, src_b, src_alpha);
      dest_scan++;
      if (Bpp == 4)
        dest_scan++;
    }
  }
}

void RgbByteOrderTransferBitmap(const RetainPtr<CFX_DIBitmap>& pBitmap,
                                int dest_left,
                                int dest_top,
                                int width,
                                int height,
                                const RetainPtr<CFX_DIBBase>& pSrcBitmap,
                                int src_left,
                                int src_top) {
  if (!pBitmap)
    return;

  if (!pBitmap->GetOverlapRect(dest_left, dest_top, width, height,
                               pSrcBitmap->GetWidth(), pSrcBitmap->GetHeight(),
                               src_left, src_top, nullptr)) {
    return;
  }

  int Bpp = pBitmap->GetBPP() / 8;
  FXDIB_Format dest_format = pBitmap->GetFormat();
  FXDIB_Format src_format = pSrcBitmap->GetFormat();
  int pitch = pBitmap->GetPitch();
  uint8_t* buffer = pBitmap->GetBuffer();
  if (dest_format == src_format) {
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = buffer + (dest_top + row) * pitch + dest_left * Bpp;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * Bpp;
      if (Bpp == 4) {
        for (int col = 0; col < width; col++) {
          FXARGB_SETDIB(dest_scan, ArgbEncode(src_scan[3], src_scan[0],
                                              src_scan[1], src_scan[2]));
          dest_scan += 4;
          src_scan += 4;
        }
        continue;
      }
      for (int col = 0; col < width; col++) {
        *dest_scan++ = src_scan[2];
        *dest_scan++ = src_scan[1];
        *dest_scan++ = src_scan[0];
        src_scan += 3;
      }
    }
    return;
  }

  uint8_t* dest_buf = buffer + dest_top * pitch + dest_left * Bpp;
  if (dest_format == FXDIB_Rgb) {
    ASSERT(src_format == FXDIB_Rgb32);
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * 4;
      for (int col = 0; col < width; col++) {
        *dest_scan++ = src_scan[2];
        *dest_scan++ = src_scan[1];
        *dest_scan++ = src_scan[0];
        src_scan += 4;
      }
    }
    return;
  }

  ASSERT(dest_format == FXDIB_Argb || dest_format == FXDIB_Rgb32);
  if (src_format == FXDIB_Rgb) {
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * 3;
      for (int col = 0; col < width; col++) {
        FXARGB_SETDIB(dest_scan,
                      ArgbEncode(0xff, src_scan[0], src_scan[1], src_scan[2]));
        dest_scan += 4;
        src_scan += 3;
      }
    }
    return;
  }
  if (src_format != FXDIB_Rgb32)
    return;
  ASSERT(dest_format == FXDIB_Argb);
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * pitch;
    const uint8_t* src_scan =
        pSrcBitmap->GetScanline(src_top + row) + src_left * 4;
    for (int col = 0; col < width; col++) {
      FXARGB_SETDIB(dest_scan,
                    ArgbEncode(0xff, src_scan[0], src_scan[1], src_scan[2]));
      src_scan += 4;
      dest_scan += 4;
    }
  }
}

bool DibSetPixel(const RetainPtr<CFX_DIBitmap>& pDevice,
                 int x,
                 int y,
                 uint32_t color) {
  int alpha = FXARGB_A(color);
  if (pDevice->IsCmykImage())
    return false;

  pDevice->SetPixel(x, y, color);
  if (pDevice->m_pAlphaMask)
    pDevice->m_pAlphaMask->SetPixel(x, y, alpha << 24);
  return true;
}

void RasterizeStroke(agg::rasterizer_scanline_aa* rasterizer,
                     agg::path_storage* path_data,
                     const CFX_Matrix* pObject2Device,
                     const CFX_GraphStateData* pGraphState,
                     float scale,
                     bool bStrokeAdjust,
                     bool bTextMode) {
  agg::line_cap_e cap;
  switch (pGraphState->m_LineCap) {
    case CFX_GraphStateData::LineCapRound:
      cap = agg::round_cap;
      break;
    case CFX_GraphStateData::LineCapSquare:
      cap = agg::square_cap;
      break;
    default:
      cap = agg::butt_cap;
      break;
  }
  agg::line_join_e join;
  switch (pGraphState->m_LineJoin) {
    case CFX_GraphStateData::LineJoinRound:
      join = agg::round_join;
      break;
    case CFX_GraphStateData::LineJoinBevel:
      join = agg::bevel_join;
      break;
    default:
      join = agg::miter_join_revert;
      break;
  }
  float width = pGraphState->m_LineWidth * scale;
  float unit = 1.0f;
  if (pObject2Device) {
    unit =
        1.0f / ((pObject2Device->GetXUnit() + pObject2Device->GetYUnit()) / 2);
  }
  width = std::max(width, unit);
  if (!pGraphState->m_DashArray.empty()) {
    typedef agg::conv_dash<agg::path_storage> dash_converter;
    dash_converter dash(*path_data);
    for (size_t i = 0; i < (pGraphState->m_DashArray.size() + 1) / 2; i++) {
      float on = pGraphState->m_DashArray[i * 2];
      if (on <= 0.000001f)
        on = 1.0f / 10;
      float off = i * 2 + 1 == pGraphState->m_DashArray.size()
                      ? on
                      : pGraphState->m_DashArray[i * 2 + 1];
      off = std::max(off, 0.0f);
      dash.add_dash(on * scale, off * scale);
    }
    dash.dash_start(pGraphState->m_DashPhase * scale);
    typedef agg::conv_stroke<dash_converter> dash_stroke;
    dash_stroke stroke(dash);
    stroke.line_join(join);
    stroke.line_cap(cap);
    stroke.miter_limit(pGraphState->m_MiterLimit);
    stroke.width(width);
    rasterizer->add_path_transformed(stroke, pObject2Device);
    return;
  }
  agg::conv_stroke<agg::path_storage> stroke(*path_data);
  stroke.line_join(join);
  stroke.line_cap(cap);
  stroke.miter_limit(pGraphState->m_MiterLimit);
  stroke.width(width);
  rasterizer->add_path_transformed(stroke, pObject2Device);
}

class CFX_Renderer {
 public:
  // Needed for agg caller
  void prepare(unsigned) {}

  void CompositeSpan(uint8_t* dest_scan,
                     uint8_t* backdrop_scan,
                     int Bpp,
                     bool bDestAlpha,
                     int span_left,
                     int span_len,
                     uint8_t* cover_scan,
                     int clip_left,
                     int clip_right,
                     uint8_t* clip_scan);

  void CompositeSpan1bpp(uint8_t* dest_scan,
                         int Bpp,
                         int span_left,
                         int span_len,
                         uint8_t* cover_scan,
                         int clip_left,
                         int clip_right,
                         uint8_t* clip_scan,
                         uint8_t* dest_extra_alpha_scan);

  void CompositeSpanGray(uint8_t* dest_scan,
                         int Bpp,
                         int span_left,
                         int span_len,
                         uint8_t* cover_scan,
                         int clip_left,
                         int clip_right,
                         uint8_t* clip_scan,
                         uint8_t* dest_extra_alpha_scan);

  void CompositeSpanARGB(uint8_t* dest_scan,
                         int Bpp,
                         int span_left,
                         int span_len,
                         uint8_t* cover_scan,
                         int clip_left,
                         int clip_right,
                         uint8_t* clip_scan,
                         uint8_t* dest_extra_alpha_scan);

  void CompositeSpanRGB(uint8_t* dest_scan,
                        int Bpp,
                        int span_left,
                        int span_len,
                        uint8_t* cover_scan,
                        int clip_left,
                        int clip_right,
                        uint8_t* clip_scan,
                        uint8_t* dest_extra_alpha_scan);

  void CompositeSpanCMYK(uint8_t* dest_scan,
                         int Bpp,
                         int span_left,
                         int span_len,
                         uint8_t* cover_scan,
                         int clip_left,
                         int clip_right,
                         uint8_t* clip_scan,
                         uint8_t* dest_extra_alpha_scan);

  bool Init(const RetainPtr<CFX_DIBitmap>& pDevice,
            const RetainPtr<CFX_DIBitmap>& pBackdropDevice,
            const CFX_ClipRgn* pClipRgn,
            uint32_t color,
            bool bFullCover,
            bool bRgbByteOrder);

  template <class Scanline>
  void render(const Scanline& sl);

 private:
  void (CFX_Renderer::*composite_span)(uint8_t*,
                                       int,
                                       int,
                                       int,
                                       uint8_t*,
                                       int,
                                       int,
                                       uint8_t*,
                                       uint8_t*);

  void CompositeSpan1bppHelper(uint8_t* dest_scan,
                               int col_start,
                               int col_end,
                               const uint8_t* cover_scan,
                               const uint8_t* clip_scan,
                               int span_left);

  inline int GetSrcAlpha(const uint8_t* clip_scan, int col) const {
    return clip_scan ? m_Alpha * clip_scan[col] / 255 : m_Alpha;
  }

  inline int GetSourceAlpha(const uint8_t* cover_scan,
                            const uint8_t* clip_scan,
                            int col) const {
    return clip_scan ? m_Alpha * cover_scan[col] * clip_scan[col] / 255 / 255
                     : m_Alpha * cover_scan[col] / 255;
  }

  inline int GetColStart(int span_left, int clip_left) const {
    return span_left < clip_left ? clip_left - span_left : 0;
  }

  inline int GetColEnd(int span_left, int span_len, int clip_right) const {
    return span_left + span_len < clip_right ? span_len
                                             : clip_right - span_left;
  }

  int m_Alpha;
  int m_Red;
  int m_Green;
  int m_Blue;
  int m_Gray;
  uint32_t m_Color;
  bool m_bFullCover;
  bool m_bRgbByteOrder;
  FX_RECT m_ClipBox;
  RetainPtr<CFX_DIBitmap> m_pBackdropDevice;
  RetainPtr<CFX_DIBitmap> m_pClipMask;
  RetainPtr<CFX_DIBitmap> m_pDevice;
  UnownedPtr<const CFX_ClipRgn> m_pClipRgn;
};

void CFX_Renderer::CompositeSpan(uint8_t* dest_scan,
                                 uint8_t* backdrop_scan,
                                 int Bpp,
                                 bool bDestAlpha,
                                 int span_left,
                                 int span_len,
                                 uint8_t* cover_scan,
                                 int clip_left,
                                 int clip_right,
                                 uint8_t* clip_scan) {
  ASSERT(!m_pDevice->IsCmykImage());
  int col_start = GetColStart(span_left, clip_left);
  int col_end = GetColEnd(span_left, span_len, clip_right);
  if (Bpp) {
    dest_scan += col_start * Bpp;
    backdrop_scan += col_start * Bpp;
  } else {
    dest_scan += col_start / 8;
    backdrop_scan += col_start / 8;
  }
  if (m_bRgbByteOrder) {
    if (Bpp == 4 && bDestAlpha) {
      for (int col = col_start; col < col_end; col++) {
        int src_alpha = GetSrcAlpha(clip_scan, col);
        uint8_t dest_alpha =
            backdrop_scan[3] + src_alpha - backdrop_scan[3] * src_alpha / 255;
        dest_scan[3] = dest_alpha;
        int alpha_ratio = src_alpha * 255 / dest_alpha;
        if (m_bFullCover) {
          *dest_scan++ =
              FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Red, alpha_ratio);
          *dest_scan++ =
              FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Green, alpha_ratio);
          *dest_scan++ =
              FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Blue, alpha_ratio);
          dest_scan++;
          backdrop_scan++;
        } else {
          int r = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Red, alpha_ratio);
          int g = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Green, alpha_ratio);
          int b = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Blue, alpha_ratio);
          backdrop_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, r, cover_scan[col]);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, g, cover_scan[col]);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, b, cover_scan[col]);
          dest_scan += 2;
        }
      }
      return;
    }
    if (Bpp == 3 || Bpp == 4) {
      for (int col = col_start; col < col_end; col++) {
        int src_alpha = GetSrcAlpha(clip_scan, col);
        int r = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Red, src_alpha);
        int g = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Green, src_alpha);
        int b = FXDIB_ALPHA_MERGE(*backdrop_scan, m_Blue, src_alpha);
        backdrop_scan += Bpp - 2;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, r, cover_scan[col]);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, g, cover_scan[col]);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, b, cover_scan[col]);
        dest_scan += Bpp - 2;
      }
    }
    return;
  }
  if (Bpp == 4 && bDestAlpha) {
    for (int col = col_start; col < col_end; col++) {
      int src_alpha = GetSrcAlpha(clip_scan, col);
      int src_alpha_covered = src_alpha * cover_scan[col] / 255;
      if (src_alpha_covered == 0) {
        dest_scan += 4;
        continue;
      }
      if (cover_scan[col] == 255) {
        dest_scan[3] = src_alpha_covered;
        *dest_scan++ = m_Blue;
        *dest_scan++ = m_Green;
        *dest_scan = m_Red;
        dest_scan += 2;
        continue;
      }
      if (dest_scan[3] == 0) {
        dest_scan[3] = src_alpha_covered;
        *dest_scan++ = m_Blue;
        *dest_scan++ = m_Green;
        *dest_scan = m_Red;
        dest_scan += 2;
        continue;
      }
      uint8_t cover = cover_scan[col];
      dest_scan[3] = FXDIB_ALPHA_MERGE(dest_scan[3], src_alpha, cover);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Blue, cover);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Green, cover);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Red, cover);
      dest_scan += 2;
    }
    return;
  }
  if (Bpp == 3 || Bpp == 4) {
    for (int col = col_start; col < col_end; col++) {
      int src_alpha = GetSrcAlpha(clip_scan, col);
      if (m_bFullCover) {
        *dest_scan++ = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Blue, src_alpha);
        *dest_scan++ = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Green, src_alpha);
        *dest_scan = FXDIB_ALPHA_MERGE(*backdrop_scan, m_Red, src_alpha);
        dest_scan += Bpp - 2;
        backdrop_scan += Bpp - 2;
        continue;
      }
      int b = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Blue, src_alpha);
      int g = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Green, src_alpha);
      int r = FXDIB_ALPHA_MERGE(*backdrop_scan, m_Red, src_alpha);
      backdrop_scan += Bpp - 2;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, b, cover_scan[col]);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, g, cover_scan[col]);
      dest_scan++;
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, r, cover_scan[col]);
      dest_scan += Bpp - 2;
      continue;
    }
    return;
  }
  if (Bpp == 1) {
    for (int col = col_start; col < col_end; col++) {
      int src_alpha = GetSrcAlpha(clip_scan, col);
      if (m_bFullCover) {
        *dest_scan = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Gray, src_alpha);
        continue;
      }
      int gray = FXDIB_ALPHA_MERGE(*backdrop_scan++, m_Gray, src_alpha);
      *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, cover_scan[col]);
      dest_scan++;
    }
    return;
  }
  CompositeSpan1bppHelper(dest_scan, col_start, col_end, cover_scan, clip_scan,
                          span_left);
}

void CFX_Renderer::CompositeSpan1bpp(uint8_t* dest_scan,
                                     int Bpp,
                                     int span_left,
                                     int span_len,
                                     uint8_t* cover_scan,
                                     int clip_left,
                                     int clip_right,
                                     uint8_t* clip_scan,
                                     uint8_t* dest_extra_alpha_scan) {
  ASSERT(!m_bRgbByteOrder);
  ASSERT(!m_pDevice->IsCmykImage());
  int col_start = GetColStart(span_left, clip_left);
  int col_end = GetColEnd(span_left, span_len, clip_right);
  dest_scan += col_start / 8;
  CompositeSpan1bppHelper(dest_scan, col_start, col_end, cover_scan, clip_scan,
                          span_left);
}

void CFX_Renderer::CompositeSpanGray(uint8_t* dest_scan,
                                     int Bpp,
                                     int span_left,
                                     int span_len,
                                     uint8_t* cover_scan,
                                     int clip_left,
                                     int clip_right,
                                     uint8_t* clip_scan,
                                     uint8_t* dest_extra_alpha_scan) {
  ASSERT(!m_bRgbByteOrder);
  int col_start = GetColStart(span_left, clip_left);
  int col_end = GetColEnd(span_left, span_len, clip_right);
  dest_scan += col_start;
  if (dest_extra_alpha_scan) {
    for (int col = col_start; col < col_end; col++) {
      int src_alpha = m_bFullCover ? GetSrcAlpha(clip_scan, col)
                                   : GetSourceAlpha(cover_scan, clip_scan, col);
      if (src_alpha) {
        if (src_alpha == 255) {
          *dest_scan = m_Gray;
          *dest_extra_alpha_scan = m_Alpha;
        } else {
          uint8_t dest_alpha = (*dest_extra_alpha_scan) + src_alpha -
                               (*dest_extra_alpha_scan) * src_alpha / 255;
          *dest_extra_alpha_scan++ = dest_alpha;
          int alpha_ratio = src_alpha * 255 / dest_alpha;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Gray, alpha_ratio);
          dest_scan++;
          continue;
        }
      }
      dest_extra_alpha_scan++;
      dest_scan++;
    }
    return;
  }
  for (int col = col_start; col < col_end; col++) {
    int src_alpha = GetSourceAlpha(cover_scan, clip_scan, col);
    if (src_alpha) {
      if (src_alpha == 255)
        *dest_scan = m_Gray;
      else
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Gray, src_alpha);
    }
    dest_scan++;
  }
}

void CFX_Renderer::CompositeSpanARGB(uint8_t* dest_scan,
                                     int Bpp,
                                     int span_left,
                                     int span_len,
                                     uint8_t* cover_scan,
                                     int clip_left,
                                     int clip_right,
                                     uint8_t* clip_scan,
                                     uint8_t* dest_extra_alpha_scan) {
  int col_start = GetColStart(span_left, clip_left);
  int col_end = GetColEnd(span_left, span_len, clip_right);
  dest_scan += col_start * Bpp;
  if (m_bRgbByteOrder) {
    for (int col = col_start; col < col_end; col++) {
      int src_alpha = m_bFullCover ? GetSrcAlpha(clip_scan, col)
                                   : GetSourceAlpha(cover_scan, clip_scan, col);
      if (src_alpha) {
        if (src_alpha == 255) {
          *(reinterpret_cast<uint32_t*>(dest_scan)) = m_Color;
        } else {
          uint8_t dest_alpha =
              dest_scan[3] + src_alpha - dest_scan[3] * src_alpha / 255;
          dest_scan[3] = dest_alpha;
          int alpha_ratio = src_alpha * 255 / dest_alpha;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Red, alpha_ratio);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Green, alpha_ratio);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Blue, alpha_ratio);
          dest_scan += 2;
          continue;
        }
      }
      dest_scan += 4;
    }
    return;
  }
  for (int col = col_start; col < col_end; col++) {
    int src_alpha = m_bFullCover ? GetSrcAlpha(clip_scan, col)
                                 : GetSourceAlpha(cover_scan, clip_scan, col);
    if (src_alpha) {
      if (src_alpha == 255) {
        *(reinterpret_cast<uint32_t*>(dest_scan)) = m_Color;
      } else {
        if (dest_scan[3] == 0) {
          dest_scan[3] = src_alpha;
          *dest_scan++ = m_Blue;
          *dest_scan++ = m_Green;
          *dest_scan = m_Red;
          dest_scan += 2;
          continue;
        }
        uint8_t dest_alpha =
            dest_scan[3] + src_alpha - dest_scan[3] * src_alpha / 255;
        dest_scan[3] = dest_alpha;
        int alpha_ratio = src_alpha * 255 / dest_alpha;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Blue, alpha_ratio);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Green, alpha_ratio);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Red, alpha_ratio);
        dest_scan += 2;
        continue;
      }
    }
    dest_scan += Bpp;
  }
}

void CFX_Renderer::CompositeSpanRGB(uint8_t* dest_scan,
                                    int Bpp,
                                    int span_left,
                                    int span_len,
                                    uint8_t* cover_scan,
                                    int clip_left,
                                    int clip_right,
                                    uint8_t* clip_scan,
                                    uint8_t* dest_extra_alpha_scan) {
  int col_start = GetColStart(span_left, clip_left);
  int col_end = GetColEnd(span_left, span_len, clip_right);
  dest_scan += col_start * Bpp;
  if (m_bRgbByteOrder) {
    for (int col = col_start; col < col_end; col++) {
      int src_alpha = GetSourceAlpha(cover_scan, clip_scan, col);
      if (src_alpha) {
        if (src_alpha == 255) {
          if (Bpp == 4) {
            *(uint32_t*)dest_scan = m_Color;
          } else if (Bpp == 3) {
            *dest_scan++ = m_Red;
            *dest_scan++ = m_Green;
            *dest_scan++ = m_Blue;
            continue;
          }
        } else {
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Red, src_alpha);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Green, src_alpha);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Blue, src_alpha);
          dest_scan += Bpp - 2;
          continue;
        }
      }
      dest_scan += Bpp;
    }
    return;
  }
  if (Bpp == 3 && dest_extra_alpha_scan) {
    for (int col = col_start; col < col_end; col++) {
      int src_alpha = m_bFullCover ? GetSrcAlpha(clip_scan, col)
                                   : GetSourceAlpha(cover_scan, clip_scan, col);
      if (src_alpha) {
        if (src_alpha == 255) {
          *dest_scan++ = static_cast<uint8_t>(m_Blue);
          *dest_scan++ = static_cast<uint8_t>(m_Green);
          *dest_scan++ = static_cast<uint8_t>(m_Red);
          *dest_extra_alpha_scan++ = static_cast<uint8_t>(m_Alpha);
          continue;
        }
        uint8_t dest_alpha = (*dest_extra_alpha_scan) + src_alpha -
                             (*dest_extra_alpha_scan) * src_alpha / 255;
        *dest_extra_alpha_scan++ = dest_alpha;
        int alpha_ratio = src_alpha * 255 / dest_alpha;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Blue, alpha_ratio);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Green, alpha_ratio);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Red, alpha_ratio);
        dest_scan++;
        continue;
      }
      dest_extra_alpha_scan++;
      dest_scan += Bpp;
    }
    return;
  }
  for (int col = col_start; col < col_end; col++) {
    int src_alpha = m_bFullCover ? GetSrcAlpha(clip_scan, col)
                                 : GetSourceAlpha(cover_scan, clip_scan, col);
    if (src_alpha) {
      if (src_alpha == 255) {
        if (Bpp == 4) {
          *(uint32_t*)dest_scan = m_Color;
        } else if (Bpp == 3) {
          *dest_scan++ = m_Blue;
          *dest_scan++ = m_Green;
          *dest_scan++ = m_Red;
          continue;
        }
      } else {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Blue, src_alpha);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Green, src_alpha);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Red, src_alpha);
        dest_scan += Bpp - 2;
        continue;
      }
    }
    dest_scan += Bpp;
  }
}

void CFX_Renderer::CompositeSpanCMYK(uint8_t* dest_scan,
                                     int Bpp,
                                     int span_left,
                                     int span_len,
                                     uint8_t* cover_scan,
                                     int clip_left,
                                     int clip_right,
                                     uint8_t* clip_scan,
                                     uint8_t* dest_extra_alpha_scan) {
  ASSERT(!m_bRgbByteOrder);
  int col_start = GetColStart(span_left, clip_left);
  int col_end = GetColEnd(span_left, span_len, clip_right);
  dest_scan += col_start * 4;
  if (dest_extra_alpha_scan) {
    for (int col = col_start; col < col_end; col++) {
      int src_alpha = m_bFullCover ? GetSrcAlpha(clip_scan, col)
                                   : GetSourceAlpha(cover_scan, clip_scan, col);
      if (src_alpha) {
        if (src_alpha == 255) {
          *(reinterpret_cast<FX_CMYK*>(dest_scan)) = m_Color;
          *dest_extra_alpha_scan = static_cast<uint8_t>(m_Alpha);
        } else {
          uint8_t dest_alpha = (*dest_extra_alpha_scan) + src_alpha -
                               (*dest_extra_alpha_scan) * src_alpha / 255;
          *dest_extra_alpha_scan++ = dest_alpha;
          int alpha_ratio = src_alpha * 255 / dest_alpha;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Red, alpha_ratio);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Green, alpha_ratio);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Blue, alpha_ratio);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Gray, alpha_ratio);
          dest_scan++;
          continue;
        }
      }
      dest_extra_alpha_scan++;
      dest_scan += 4;
    }
    return;
  }
  for (int col = col_start; col < col_end; col++) {
    int src_alpha = GetSourceAlpha(cover_scan, clip_scan, col);
    if (src_alpha) {
      if (src_alpha == 255) {
        *(reinterpret_cast<FX_CMYK*>(dest_scan)) = m_Color;
      } else {
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Red, src_alpha);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Green, src_alpha);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Blue, src_alpha);
        dest_scan++;
        *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, m_Gray, src_alpha);
        dest_scan++;
        continue;
      }
    }
    dest_scan += 4;
  }
}

bool CFX_Renderer::Init(const RetainPtr<CFX_DIBitmap>& pDevice,
                        const RetainPtr<CFX_DIBitmap>& pBackdropDevice,
                        const CFX_ClipRgn* pClipRgn,
                        uint32_t color,
                        bool bFullCover,
                        bool bRgbByteOrder) {
  m_pDevice = pDevice;
  m_pClipRgn = pClipRgn;
  composite_span = nullptr;
  m_bRgbByteOrder = bRgbByteOrder;
  m_pBackdropDevice = pBackdropDevice;
  if (m_pClipRgn) {
    m_ClipBox = m_pClipRgn->GetBox();
  } else {
    m_ClipBox.left = m_ClipBox.top = 0;
    m_ClipBox.right = m_pDevice->GetWidth();
    m_ClipBox.bottom = m_pDevice->GetHeight();
  }
  m_pClipMask = nullptr;
  if (m_pClipRgn && m_pClipRgn->GetType() == CFX_ClipRgn::MaskF)
    m_pClipMask = m_pClipRgn->GetMask();
  m_bFullCover = bFullCover;
  bool bDeviceCMYK = pDevice->IsCmykImage();
  m_Alpha = FXARGB_A(color);
  if (m_pDevice->GetBPP() == 8) {
    ASSERT(!m_bRgbByteOrder);
    composite_span = &CFX_Renderer::CompositeSpanGray;
    if (m_pDevice->IsAlphaMask())
      m_Gray = 255;
    else
      m_Gray = FXRGB2GRAY(FXARGB_R(color), FXARGB_G(color), FXARGB_B(color));
    return true;
  }
  if (bDeviceCMYK) {
    ASSERT(!m_bRgbByteOrder);
    composite_span = &CFX_Renderer::CompositeSpanCMYK;
    return false;
  }
  composite_span = (pDevice->GetFormat() == FXDIB_Argb)
                       ? &CFX_Renderer::CompositeSpanARGB
                       : &CFX_Renderer::CompositeSpanRGB;
  if (m_bRgbByteOrder)
    m_Color = FXARGB_TOBGRORDERDIB(color);
  else
    m_Color = FXARGB_TODIB(color);
  std::tie(m_Alpha, m_Red, m_Green, m_Blue) = ArgbDecode(color);
  if (m_pDevice->GetBPP() == 1)
    composite_span = &CFX_Renderer::CompositeSpan1bpp;
  return true;
}

template <class Scanline>
void CFX_Renderer::render(const Scanline& sl) {
  if (!m_pBackdropDevice && !composite_span)
    return;

  int y = sl.y();
  if (y < m_ClipBox.top || y >= m_ClipBox.bottom)
    return;

  uint8_t* dest_scan = m_pDevice->GetBuffer() + m_pDevice->GetPitch() * y;
  uint8_t* dest_scan_extra_alpha = nullptr;
  RetainPtr<CFX_DIBitmap> pAlphaMask = m_pDevice->m_pAlphaMask;
  if (pAlphaMask) {
    dest_scan_extra_alpha =
        pAlphaMask->GetBuffer() + pAlphaMask->GetPitch() * y;
  }
  uint8_t* backdrop_scan = nullptr;
  if (m_pBackdropDevice) {
    backdrop_scan =
        m_pBackdropDevice->GetBuffer() + m_pBackdropDevice->GetPitch() * y;
  }
  int Bpp = m_pDevice->GetBPP() / 8;
  bool bDestAlpha = m_pDevice->HasAlpha() || m_pDevice->IsAlphaMask();
  unsigned num_spans = sl.num_spans();
  typename Scanline::const_iterator span = sl.begin();
  while (1) {
    if (span->len <= 0)
      break;

    int x = span->x;
    uint8_t* dest_pos = nullptr;
    uint8_t* dest_extra_alpha_pos = nullptr;
    uint8_t* backdrop_pos = nullptr;
    if (Bpp) {
      backdrop_pos = backdrop_scan ? backdrop_scan + x * Bpp : nullptr;
      dest_pos = dest_scan + x * Bpp;
      dest_extra_alpha_pos =
          dest_scan_extra_alpha ? dest_scan_extra_alpha + x : nullptr;
    } else {
      dest_pos = dest_scan + x / 8;
      backdrop_pos = backdrop_scan ? backdrop_scan + x / 8 : nullptr;
    }
    uint8_t* clip_pos = nullptr;
    if (m_pClipMask) {
      clip_pos = m_pClipMask->GetBuffer() +
                 (y - m_ClipBox.top) * m_pClipMask->GetPitch() + x -
                 m_ClipBox.left;
    }
    if (backdrop_pos) {
      CompositeSpan(dest_pos, backdrop_pos, Bpp, bDestAlpha, x, span->len,
                    span->covers, m_ClipBox.left, m_ClipBox.right, clip_pos);
    } else {
      (this->*composite_span)(dest_pos, Bpp, x, span->len, span->covers,
                              m_ClipBox.left, m_ClipBox.right, clip_pos,
                              dest_extra_alpha_pos);
    }
    if (--num_spans == 0)
      break;

    ++span;
  }
}

void CFX_Renderer::CompositeSpan1bppHelper(uint8_t* dest_scan,
                                           int col_start,
                                           int col_end,
                                           const uint8_t* cover_scan,
                                           const uint8_t* clip_scan,
                                           int span_left) {
  int index = 0;
  if (m_pDevice->GetPalette()) {
    for (int i = 0; i < 2; i++) {
      if (FXARGB_TODIB(m_pDevice->GetPalette()[i]) == m_Color)
        index = i;
    }
  } else {
    index = (static_cast<uint8_t>(m_Color) == 0xff) ? 1 : 0;
  }
  uint8_t* dest_scan1 = dest_scan;
  for (int col = col_start; col < col_end; col++) {
    int src_alpha = GetSourceAlpha(cover_scan, clip_scan, col);
    if (src_alpha) {
      if (!index)
        *dest_scan1 &= ~(1 << (7 - (col + span_left) % 8));
      else
        *dest_scan1 |= 1 << (7 - (col + span_left) % 8);
    }
    dest_scan1 = dest_scan + (span_left % 8 + col - col_start + 1) / 8;
  }
}

}  // namespace

namespace agg {

template <class BaseRenderer>
class renderer_scanline_aa_offset {
 public:
  typedef BaseRenderer base_ren_type;
  typedef typename base_ren_type::color_type color_type;
  renderer_scanline_aa_offset(base_ren_type& ren, unsigned left, unsigned top)
      : m_ren(&ren), m_left(left), m_top(top) {}
  void color(const color_type& c) { m_color = c; }
  const color_type& color() const { return m_color; }
  void prepare(unsigned) {}
  template <class Scanline>
  void render(const Scanline& sl) {
    int y = sl.y();
    unsigned num_spans = sl.num_spans();
    typename Scanline::const_iterator span = sl.begin();
    while (1) {
      int x = span->x;
      if (span->len > 0) {
        m_ren->blend_solid_hspan(x - m_left, y - m_top, (unsigned)span->len,
                                 m_color, span->covers);
      } else {
        m_ren->blend_hline(x - m_left, y - m_top, (unsigned)(x - span->len - 1),
                           m_color, *(span->covers));
      }
      if (--num_spans == 0)
        break;

      ++span;
    }
  }

 private:
  base_ren_type* m_ren;
  color_type m_color;
  unsigned m_left, m_top;
};

}  // namespace agg

void CAgg_PathData::BuildPath(const CFX_PathData* pPathData,
                              const CFX_Matrix* pObject2Device) {
  const std::vector<FX_PATHPOINT>& pPoints = pPathData->GetPoints();
  for (size_t i = 0; i < pPoints.size(); i++) {
    CFX_PointF pos = pPoints[i].m_Point;
    if (pObject2Device)
      pos = pObject2Device->Transform(pos);

    pos = HardClip(pos);
    FXPT_TYPE point_type = pPoints[i].m_Type;
    if (point_type == FXPT_TYPE::MoveTo) {
      m_PathData.move_to(pos.x, pos.y);
    } else if (point_type == FXPT_TYPE::LineTo) {
      if (i > 0 && pPoints[i - 1].IsTypeAndOpen(FXPT_TYPE::MoveTo) &&
          (i == pPoints.size() - 1 ||
           pPoints[i + 1].IsTypeAndOpen(FXPT_TYPE::MoveTo)) &&
          pPoints[i].m_Point == pPoints[i - 1].m_Point) {
        pos.x += 1;
      }
      m_PathData.line_to(pos.x, pos.y);
    } else if (point_type == FXPT_TYPE::BezierTo) {
      if (i > 0 && i + 2 < pPoints.size()) {
        CFX_PointF pos0 = pPoints[i - 1].m_Point;
        CFX_PointF pos2 = pPoints[i + 1].m_Point;
        CFX_PointF pos3 = pPoints[i + 2].m_Point;
        if (pObject2Device) {
          pos0 = pObject2Device->Transform(pos0);
          pos2 = pObject2Device->Transform(pos2);
          pos3 = pObject2Device->Transform(pos3);
        }
        pos0 = HardClip(pos0);
        pos2 = HardClip(pos2);
        pos3 = HardClip(pos3);
        agg::curve4 curve(pos0.x, pos0.y, pos.x, pos.y, pos2.x, pos2.y, pos3.x,
                          pos3.y);
        i += 2;
        m_PathData.add_path_curve(curve);
      }
    }
    if (pPoints[i].m_CloseFigure)
      m_PathData.end_poly();
  }
}

CFX_AggDeviceDriver::CFX_AggDeviceDriver(
    const RetainPtr<CFX_DIBitmap>& pBitmap,
    bool bRgbByteOrder,
    const RetainPtr<CFX_DIBitmap>& pBackdropBitmap,
    bool bGroupKnockout)
    : m_pBitmap(pBitmap),
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
      m_pPlatformGraphics(nullptr),
#endif
      m_FillFlags(0),
      m_bRgbByteOrder(bRgbByteOrder),
      m_pBackdropBitmap(pBackdropBitmap),
      m_bGroupKnockout(bGroupKnockout) {
  InitPlatform();
}

CFX_AggDeviceDriver::~CFX_AggDeviceDriver() {
  DestroyPlatform();
}

uint8_t* CFX_AggDeviceDriver::GetBuffer() const {
  return m_pBitmap->GetBuffer();
}

#if _FX_PLATFORM_ != _FX_PLATFORM_APPLE_
void CFX_AggDeviceDriver::InitPlatform() {}

void CFX_AggDeviceDriver::DestroyPlatform() {}

bool CFX_AggDeviceDriver::DrawDeviceText(int nChars,
                                         const FXTEXT_CHARPOS* pCharPos,
                                         CFX_Font* pFont,
                                         const CFX_Matrix* pObject2Device,
                                         float font_size,
                                         uint32_t color) {
  return false;
}
#endif  // _FX_PLATFORM_ != _FX_PLATFORM_APPLE_

int CFX_AggDeviceDriver::GetDeviceCaps(int caps_id) const {
  switch (caps_id) {
    case FXDC_DEVICE_CLASS:
      return FXDC_DISPLAY;
    case FXDC_PIXEL_WIDTH:
      return m_pBitmap->GetWidth();
    case FXDC_PIXEL_HEIGHT:
      return m_pBitmap->GetHeight();
    case FXDC_BITS_PIXEL:
      return m_pBitmap->GetBPP();
    case FXDC_HORZ_SIZE:
    case FXDC_VERT_SIZE:
      return 0;
    case FXDC_RENDER_CAPS: {
      int flags = FXRC_GET_BITS | FXRC_ALPHA_PATH | FXRC_ALPHA_IMAGE |
                  FXRC_BLEND_MODE | FXRC_SOFT_CLIP;
      if (m_pBitmap->HasAlpha()) {
        flags |= FXRC_ALPHA_OUTPUT;
      } else if (m_pBitmap->IsAlphaMask()) {
        if (m_pBitmap->GetBPP() == 1)
          flags |= FXRC_BITMASK_OUTPUT;
        else
          flags |= FXRC_BYTEMASK_OUTPUT;
      }
      if (m_pBitmap->IsCmykImage())
        flags |= FXRC_CMYK_OUTPUT;
      return flags;
    }
    default:
      return 0;
  }
}

void CFX_AggDeviceDriver::SaveState() {
  std::unique_ptr<CFX_ClipRgn> pClip;
  if (m_pClipRgn)
    pClip = pdfium::MakeUnique<CFX_ClipRgn>(*m_pClipRgn);
  m_StateStack.push_back(std::move(pClip));
}

void CFX_AggDeviceDriver::RestoreState(bool bKeepSaved) {
  m_pClipRgn.reset();

  if (m_StateStack.empty())
    return;

  if (bKeepSaved) {
    if (m_StateStack.back())
      m_pClipRgn = pdfium::MakeUnique<CFX_ClipRgn>(*m_StateStack.back());
  } else {
    m_pClipRgn = std::move(m_StateStack.back());
    m_StateStack.pop_back();
  }
}

void CFX_AggDeviceDriver::SetClipMask(agg::rasterizer_scanline_aa& rasterizer) {
  FX_RECT path_rect(rasterizer.min_x(), rasterizer.min_y(),
                    rasterizer.max_x() + 1, rasterizer.max_y() + 1);
  path_rect.Intersect(m_pClipRgn->GetBox());
  auto pThisLayer = pdfium::MakeRetain<CFX_DIBitmap>();
  pThisLayer->Create(path_rect.Width(), path_rect.Height(), FXDIB_8bppMask);
  pThisLayer->Clear(0);
  agg::rendering_buffer raw_buf(pThisLayer->GetBuffer(), pThisLayer->GetWidth(),
                                pThisLayer->GetHeight(),
                                pThisLayer->GetPitch());
  agg::pixfmt_gray8 pixel_buf(raw_buf);
  agg::renderer_base<agg::pixfmt_gray8> base_buf(pixel_buf);
  agg::renderer_scanline_aa_offset<agg::renderer_base<agg::pixfmt_gray8> >
      final_render(base_buf, path_rect.left, path_rect.top);
  final_render.color(agg::gray8(255));
  agg::scanline_u8 scanline;
  agg::render_scanlines(rasterizer, scanline, final_render,
                        (m_FillFlags & FXFILL_NOPATHSMOOTH) != 0);
  m_pClipRgn->IntersectMaskF(path_rect.left, path_rect.top, pThisLayer);
}

bool CFX_AggDeviceDriver::SetClip_PathFill(const CFX_PathData* pPathData,
                                           const CFX_Matrix* pObject2Device,
                                           int fill_mode) {
  m_FillFlags = fill_mode;
  if (!m_pClipRgn) {
    m_pClipRgn = pdfium::MakeUnique<CFX_ClipRgn>(
        GetDeviceCaps(FXDC_PIXEL_WIDTH), GetDeviceCaps(FXDC_PIXEL_HEIGHT));
  }
  size_t size = pPathData->GetPoints().size();
  if (size == 5 || size == 4) {
    CFX_FloatRect rectf;
    if (pPathData->IsRect(pObject2Device, &rectf)) {
      rectf.Intersect(CFX_FloatRect(
          0, 0, static_cast<float>(GetDeviceCaps(FXDC_PIXEL_WIDTH)),
          static_cast<float>(GetDeviceCaps(FXDC_PIXEL_HEIGHT))));
      FX_RECT rect = rectf.GetOuterRect();
      m_pClipRgn->IntersectRect(rect);
      return true;
    }
  }
  CAgg_PathData path_data;
  path_data.BuildPath(pPathData, pObject2Device);
  path_data.m_PathData.end_poly();
  agg::rasterizer_scanline_aa rasterizer;
  rasterizer.clip_box(0.0f, 0.0f,
                      static_cast<float>(GetDeviceCaps(FXDC_PIXEL_WIDTH)),
                      static_cast<float>(GetDeviceCaps(FXDC_PIXEL_HEIGHT)));
  rasterizer.add_path(path_data.m_PathData);
  rasterizer.filling_rule((fill_mode & 3) == FXFILL_WINDING
                              ? agg::fill_non_zero
                              : agg::fill_even_odd);
  SetClipMask(rasterizer);
  return true;
}

bool CFX_AggDeviceDriver::SetClip_PathStroke(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState) {
  if (!m_pClipRgn) {
    m_pClipRgn = pdfium::MakeUnique<CFX_ClipRgn>(
        GetDeviceCaps(FXDC_PIXEL_WIDTH), GetDeviceCaps(FXDC_PIXEL_HEIGHT));
  }
  CAgg_PathData path_data;
  path_data.BuildPath(pPathData, nullptr);
  agg::rasterizer_scanline_aa rasterizer;
  rasterizer.clip_box(0.0f, 0.0f,
                      static_cast<float>(GetDeviceCaps(FXDC_PIXEL_WIDTH)),
                      static_cast<float>(GetDeviceCaps(FXDC_PIXEL_HEIGHT)));
  RasterizeStroke(&rasterizer, &path_data.m_PathData, pObject2Device,
                  pGraphState, 1.0f, false, false);
  rasterizer.filling_rule(agg::fill_non_zero);
  SetClipMask(rasterizer);
  return true;
}

int CFX_AggDeviceDriver::GetDriverType() const {
  return 1;
}

bool CFX_AggDeviceDriver::RenderRasterizer(
    agg::rasterizer_scanline_aa& rasterizer,
    uint32_t color,
    bool bFullCover,
    bool bGroupKnockout) {
  RetainPtr<CFX_DIBitmap> pt = bGroupKnockout ? m_pBackdropBitmap : nullptr;
  CFX_Renderer render;
  if (!render.Init(m_pBitmap, pt, m_pClipRgn.get(), color, bFullCover,
                   m_bRgbByteOrder)) {
    return false;
  }
  agg::scanline_u8 scanline;
  agg::render_scanlines(rasterizer, scanline, render,
                        (m_FillFlags & FXFILL_NOPATHSMOOTH) != 0);
  return true;
}

bool CFX_AggDeviceDriver::DrawPath(const CFX_PathData* pPathData,
                                   const CFX_Matrix* pObject2Device,
                                   const CFX_GraphStateData* pGraphState,
                                   uint32_t fill_color,
                                   uint32_t stroke_color,
                                   int fill_mode,
                                   BlendMode blend_type) {
  if (blend_type != BlendMode::kNormal)
    return false;

  if (!GetBuffer())
    return true;

  m_FillFlags = fill_mode;
  if ((fill_mode & 3) && fill_color) {
    CAgg_PathData path_data;
    path_data.BuildPath(pPathData, pObject2Device);
    agg::rasterizer_scanline_aa rasterizer;
    rasterizer.clip_box(0.0f, 0.0f,
                        static_cast<float>(GetDeviceCaps(FXDC_PIXEL_WIDTH)),
                        static_cast<float>(GetDeviceCaps(FXDC_PIXEL_HEIGHT)));
    rasterizer.add_path(path_data.m_PathData);
    rasterizer.filling_rule((fill_mode & 3) == FXFILL_WINDING
                                ? agg::fill_non_zero
                                : agg::fill_even_odd);
    if (!RenderRasterizer(rasterizer, fill_color,
                          !!(fill_mode & FXFILL_FULLCOVER), false)) {
      return false;
    }
  }
  int stroke_alpha = FXARGB_A(stroke_color);
  if (!pGraphState || !stroke_alpha)
    return true;

  if (fill_mode & FX_ZEROAREA_FILL) {
    CAgg_PathData path_data;
    path_data.BuildPath(pPathData, pObject2Device);
    agg::rasterizer_scanline_aa rasterizer;
    rasterizer.clip_box(0.0f, 0.0f,
                        static_cast<float>(GetDeviceCaps(FXDC_PIXEL_WIDTH)),
                        static_cast<float>(GetDeviceCaps(FXDC_PIXEL_HEIGHT)));
    RasterizeStroke(&rasterizer, &path_data.m_PathData, nullptr, pGraphState, 1,
                    false, !!(fill_mode & FX_STROKE_TEXT_MODE));
    return RenderRasterizer(rasterizer, stroke_color,
                            !!(fill_mode & FXFILL_FULLCOVER), m_bGroupKnockout);
  }
  CFX_Matrix matrix1;
  CFX_Matrix matrix2;
  if (pObject2Device) {
    matrix1.a = std::max(fabs(pObject2Device->a), fabs(pObject2Device->b));
    matrix1.d = matrix1.a;
    matrix2 = CFX_Matrix(
        pObject2Device->a / matrix1.a, pObject2Device->b / matrix1.a,
        pObject2Device->c / matrix1.d, pObject2Device->d / matrix1.d, 0, 0);

    matrix1 = *pObject2Device * matrix2.GetInverse();
  }

  CAgg_PathData path_data;
  path_data.BuildPath(pPathData, &matrix1);
  agg::rasterizer_scanline_aa rasterizer;
  rasterizer.clip_box(0.0f, 0.0f,
                      static_cast<float>(GetDeviceCaps(FXDC_PIXEL_WIDTH)),
                      static_cast<float>(GetDeviceCaps(FXDC_PIXEL_HEIGHT)));
  RasterizeStroke(&rasterizer, &path_data.m_PathData, &matrix2, pGraphState,
                  matrix1.a, false, !!(fill_mode & FX_STROKE_TEXT_MODE));
  return RenderRasterizer(rasterizer, stroke_color,
                          !!(fill_mode & FXFILL_FULLCOVER), m_bGroupKnockout);
}

bool CFX_AggDeviceDriver::SetPixel(int x, int y, uint32_t color) {
  if (!m_pBitmap->GetBuffer())
    return true;

  if (!m_pClipRgn) {
    if (!m_bRgbByteOrder)
      return DibSetPixel(m_pBitmap, x, y, color);
    RgbByteOrderSetPixel(m_pBitmap, x, y, color);
    return true;
  }
  if (!m_pClipRgn->GetBox().Contains(x, y))
    return true;

  if (m_pClipRgn->GetType() == CFX_ClipRgn::RectI) {
    if (!m_bRgbByteOrder)
      return DibSetPixel(m_pBitmap, x, y, color);
    RgbByteOrderSetPixel(m_pBitmap, x, y, color);
    return true;
  }
  if (m_pClipRgn->GetType() != CFX_ClipRgn::MaskF)
    return true;

  int new_alpha =
      FXARGB_A(color) * m_pClipRgn->GetMask()->GetScanline(y)[x] / 255;
  color = (color & 0xffffff) | (new_alpha << 24);
  if (m_bRgbByteOrder) {
    RgbByteOrderSetPixel(m_pBitmap, x, y, color);
    return true;
  }
  return DibSetPixel(m_pBitmap, x, y, color);
}

bool CFX_AggDeviceDriver::FillRectWithBlend(const FX_RECT& rect,
                                            uint32_t fill_color,
                                            BlendMode blend_type) {
  if (blend_type != BlendMode::kNormal)
    return false;

  if (!m_pBitmap->GetBuffer())
    return true;

  FX_RECT clip_rect;
  GetClipBox(&clip_rect);
  FX_RECT draw_rect = clip_rect;
  draw_rect.Intersect(rect);
  if (draw_rect.IsEmpty())
    return true;

  if (!m_pClipRgn || m_pClipRgn->GetType() == CFX_ClipRgn::RectI) {
    if (m_bRgbByteOrder) {
      RgbByteOrderCompositeRect(m_pBitmap, draw_rect.left, draw_rect.top,
                                draw_rect.Width(), draw_rect.Height(),
                                fill_color);
    } else {
      m_pBitmap->CompositeRect(draw_rect.left, draw_rect.top, draw_rect.Width(),
                               draw_rect.Height(), fill_color, 0);
    }
    return true;
  }
  m_pBitmap->CompositeMask(draw_rect.left, draw_rect.top, draw_rect.Width(),
                           draw_rect.Height(), m_pClipRgn->GetMask(),
                           fill_color, draw_rect.left - clip_rect.left,
                           draw_rect.top - clip_rect.top, BlendMode::kNormal,
                           nullptr, m_bRgbByteOrder, 0);
  return true;
}

bool CFX_AggDeviceDriver::GetClipBox(FX_RECT* pRect) {
  if (!m_pClipRgn) {
    pRect->left = pRect->top = 0;
    pRect->right = GetDeviceCaps(FXDC_PIXEL_WIDTH);
    pRect->bottom = GetDeviceCaps(FXDC_PIXEL_HEIGHT);
    return true;
  }
  *pRect = m_pClipRgn->GetBox();
  return true;
}

bool CFX_AggDeviceDriver::GetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                                    int left,
                                    int top) {
  if (!m_pBitmap || !m_pBitmap->GetBuffer())
    return true;

  FX_RECT rect(left, top, left + pBitmap->GetWidth(),
               top + pBitmap->GetHeight());
  RetainPtr<CFX_DIBitmap> pBack;
  if (m_pBackdropBitmap) {
    pBack = m_pBackdropBitmap->Clone(&rect);
    if (!pBack)
      return true;

    pBack->CompositeBitmap(0, 0, pBack->GetWidth(), pBack->GetHeight(),
                           m_pBitmap, 0, 0, BlendMode::kNormal, nullptr, false);
  } else {
    pBack = m_pBitmap->Clone(&rect);
    if (!pBack)
      return true;
  }

  left = std::min(left, 0);
  top = std::min(top, 0);
  if (m_bRgbByteOrder) {
    RgbByteOrderTransferBitmap(pBitmap, 0, 0, rect.Width(), rect.Height(),
                               pBack, left, top);
    return true;
  }
  return pBitmap->TransferBitmap(0, 0, rect.Width(), rect.Height(), pBack, left,
                                 top);
}

RetainPtr<CFX_DIBitmap> CFX_AggDeviceDriver::GetBackDrop() {
  return m_pBackdropBitmap;
}

bool CFX_AggDeviceDriver::SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                                    uint32_t argb,
                                    const FX_RECT& src_rect,
                                    int left,
                                    int top,
                                    BlendMode blend_type) {
  if (!m_pBitmap->GetBuffer())
    return true;

  if (pBitmap->IsAlphaMask()) {
    return m_pBitmap->CompositeMask(left, top, src_rect.Width(),
                                    src_rect.Height(), pBitmap, argb,
                                    src_rect.left, src_rect.top, blend_type,
                                    m_pClipRgn.get(), m_bRgbByteOrder, 0);
  }
  return m_pBitmap->CompositeBitmap(
      left, top, src_rect.Width(), src_rect.Height(), pBitmap, src_rect.left,
      src_rect.top, blend_type, m_pClipRgn.get(), m_bRgbByteOrder);
}

bool CFX_AggDeviceDriver::StretchDIBits(const RetainPtr<CFX_DIBBase>& pSource,
                                        uint32_t argb,
                                        int dest_left,
                                        int dest_top,
                                        int dest_width,
                                        int dest_height,
                                        const FX_RECT* pClipRect,
                                        const FXDIB_ResampleOptions& options,
                                        BlendMode blend_type) {
  if (!m_pBitmap->GetBuffer())
    return true;

  if (dest_width == pSource->GetWidth() &&
      dest_height == pSource->GetHeight()) {
    FX_RECT rect(0, 0, dest_width, dest_height);
    return SetDIBits(pSource, argb, rect, dest_left, dest_top, blend_type);
  }
  FX_RECT dest_rect(dest_left, dest_top, dest_left + dest_width,
                    dest_top + dest_height);
  dest_rect.Normalize();
  FX_RECT dest_clip = dest_rect;
  dest_clip.Intersect(*pClipRect);
  CFX_BitmapComposer composer;
  composer.Compose(m_pBitmap, m_pClipRgn.get(), 255, argb, dest_clip, false,
                   false, false, m_bRgbByteOrder, 0, blend_type);
  dest_clip.Offset(-dest_rect.left, -dest_rect.top);
  CFX_ImageStretcher stretcher(&composer, pSource, dest_width, dest_height,
                               dest_clip, options);
  if (stretcher.Start())
    stretcher.Continue(nullptr);
  return true;
}

bool CFX_AggDeviceDriver::StartDIBits(
    const RetainPtr<CFX_DIBBase>& pSource,
    int bitmap_alpha,
    uint32_t argb,
    const CFX_Matrix& matrix,
    const FXDIB_ResampleOptions& options,
    std::unique_ptr<CFX_ImageRenderer>* handle,
    BlendMode blend_type) {
  if (!m_pBitmap->GetBuffer())
    return true;

  *handle = pdfium::MakeUnique<CFX_ImageRenderer>(
      m_pBitmap, m_pClipRgn.get(), pSource, bitmap_alpha, argb, matrix, options,
      m_bRgbByteOrder);
  return true;
}

bool CFX_AggDeviceDriver::ContinueDIBits(CFX_ImageRenderer* pHandle,
                                         PauseIndicatorIface* pPause) {
  return m_pBitmap->GetBuffer() ? pHandle->Continue(pPause) : true;
}

#ifndef _SKIA_SUPPORT_
CFX_DefaultRenderDevice::CFX_DefaultRenderDevice() {}

CFX_DefaultRenderDevice::~CFX_DefaultRenderDevice() {}

bool CFX_DefaultRenderDevice::Attach(
    const RetainPtr<CFX_DIBitmap>& pBitmap,
    bool bRgbByteOrder,
    const RetainPtr<CFX_DIBitmap>& pBackdropBitmap,
    bool bGroupKnockout) {
  if (!pBitmap)
    return false;

  SetBitmap(pBitmap);
  SetDeviceDriver(pdfium::MakeUnique<CFX_AggDeviceDriver>(
      pBitmap, bRgbByteOrder, pBackdropBitmap, bGroupKnockout));
  return true;
}

bool CFX_DefaultRenderDevice::Create(
    int width,
    int height,
    FXDIB_Format format,
    const RetainPtr<CFX_DIBitmap>& pBackdropBitmap) {
  auto pBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pBitmap->Create(width, height, format))
    return false;

  SetBitmap(pBitmap);
  SetDeviceDriver(pdfium::MakeUnique<CFX_AggDeviceDriver>(
      pBitmap, false, pBackdropBitmap, false));
  return true;
}

#endif
