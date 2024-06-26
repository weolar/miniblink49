// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <algorithm>
#include <limits>
#include <vector>

#include "core/fxcrt/fx_safe_types.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/fx_font.h"
#include "core/fxge/fx_freetype.h"
#include "core/fxge/renderdevicedriver_iface.h"

namespace {

void ResetTransform(FT_Face face) {
  FXFT_Matrix matrix;
  matrix.xx = 0x10000L;
  matrix.xy = 0;
  matrix.yx = 0;
  matrix.yy = 0x10000L;
  FXFT_Set_Transform(face, &matrix, 0);
}

}  // namespace

FXTEXT_GLYPHPOS::FXTEXT_GLYPHPOS() : m_pGlyph(nullptr) {}

FXTEXT_GLYPHPOS::FXTEXT_GLYPHPOS(const FXTEXT_GLYPHPOS&) = default;

FXTEXT_GLYPHPOS::~FXTEXT_GLYPHPOS(){};

ScopedFontTransform::ScopedFontTransform(FT_Face face, FXFT_Matrix* matrix)
    : m_Face(face) {
  FXFT_Set_Transform(m_Face, matrix, 0);
}

ScopedFontTransform::~ScopedFontTransform() {
  ResetTransform(m_Face);
}

FX_RECT FXGE_GetGlyphsBBox(const std::vector<FXTEXT_GLYPHPOS>& glyphs,
                           int anti_alias) {
  FX_RECT rect(0, 0, 0, 0);
  bool bStarted = false;
  for (const FXTEXT_GLYPHPOS& glyph : glyphs) {
    const CFX_GlyphBitmap* pGlyph = glyph.m_pGlyph;
    if (!pGlyph)
      continue;

    FX_SAFE_INT32 char_left = glyph.m_Origin.x;
    char_left += pGlyph->m_Left;
    if (!char_left.IsValid())
      continue;

    FX_SAFE_INT32 char_width = pGlyph->m_pBitmap->GetWidth();
    if (anti_alias == FXFT_RENDER_MODE_LCD)
      char_width /= 3;
    if (!char_width.IsValid())
      continue;

    FX_SAFE_INT32 char_right = char_left + char_width;
    if (!char_right.IsValid())
      continue;

    FX_SAFE_INT32 char_top = glyph.m_Origin.y;
    char_top -= pGlyph->m_Top;
    if (!char_top.IsValid())
      continue;

    FX_SAFE_INT32 char_height = pGlyph->m_pBitmap->GetHeight();
    if (!char_height.IsValid())
      continue;

    FX_SAFE_INT32 char_bottom = char_top + char_height;
    if (!char_bottom.IsValid())
      continue;

    if (bStarted) {
      rect.left = pdfium::base::ValueOrDieForType<int32_t>(
          pdfium::base::CheckMin(rect.left, char_left));
      rect.right = pdfium::base::ValueOrDieForType<int32_t>(
          pdfium::base::CheckMax(rect.right, char_right));
      rect.top = pdfium::base::ValueOrDieForType<int32_t>(
          pdfium::base::CheckMin(rect.top, char_top));
      rect.bottom = pdfium::base::ValueOrDieForType<int32_t>(
          pdfium::base::CheckMax(rect.bottom, char_bottom));
      continue;
    }

    rect.left = char_left.ValueOrDie();
    rect.right = char_right.ValueOrDie();
    rect.top = char_top.ValueOrDie();
    rect.bottom = char_bottom.ValueOrDie();
    bStarted = true;
  }
  return rect;
}
