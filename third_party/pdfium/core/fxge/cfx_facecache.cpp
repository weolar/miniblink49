// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_facecache.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_font.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_substfont.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/fx_freetype.h"
#include "third_party/base/numerics/safe_math.h"
#include "third_party/base/ptr_util.h"

#if defined _SKIA_SUPPORT_ || _SKIA_SUPPORT_PATHS_
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkTypeface.h"

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
#include "third_party/skia/include/core/SkFontMgr.h"
#include "third_party/skia/include/ports/SkFontMgr_empty.h"
#endif
#endif

namespace {

constexpr uint32_t kInvalidGlyphIndex = static_cast<uint32_t>(-1);

constexpr int kMaxGlyphDimension = 2048;

struct UniqueKeyGen {
  void Generate(int count, ...);

  int key_len_;
  char key_[128];
};

void UniqueKeyGen::Generate(int count, ...) {
  va_list argList;
  va_start(argList, count);
  for (int i = 0; i < count; i++) {
    int p = va_arg(argList, int);
    reinterpret_cast<uint32_t*>(key_)[i] = p;
  }
  va_end(argList);
  key_len_ = count * sizeof(uint32_t);
}

}  // namespace

CFX_FaceCache::CFX_FaceCache(FXFT_Face face)
    : m_Face(face)
#if defined _SKIA_SUPPORT_ || _SKIA_SUPPORT_PATHS_
      ,
      m_pTypeface(nullptr)
#endif
{
}

CFX_FaceCache::~CFX_FaceCache() {}

std::unique_ptr<CFX_GlyphBitmap> CFX_FaceCache::RenderGlyph(
    const CFX_Font* pFont,
    uint32_t glyph_index,
    bool bFontStyle,
    const CFX_Matrix& matrix,
    uint32_t dest_width,
    int anti_alias) {
  if (!m_Face)
    return nullptr;

  FXFT_Matrix ft_matrix;
  ft_matrix.xx = matrix.a / 64 * 65536;
  ft_matrix.xy = matrix.c / 64 * 65536;
  ft_matrix.yx = matrix.b / 64 * 65536;
  ft_matrix.yy = matrix.d / 64 * 65536;
  bool bUseCJKSubFont = false;
  const CFX_SubstFont* pSubstFont = pFont->GetSubstFont();
  if (pSubstFont) {
    bUseCJKSubFont = pSubstFont->m_bSubstCJK && bFontStyle;
    int skew = 0;
    if (bUseCJKSubFont)
      skew = pSubstFont->m_bItalicCJK ? -15 : 0;
    else
      skew = pSubstFont->m_ItalicAngle;
    if (skew) {
      // |skew| is nonpositive so |-skew| is used as the index. We need to make
      // sure |skew| != INT_MIN since -INT_MIN is undefined.
      if (skew <= 0 && skew != std::numeric_limits<int>::min() &&
          static_cast<size_t>(-skew) < CFX_Font::kAngleSkewArraySize) {
        skew = -CFX_Font::s_AngleSkew[-skew];
      } else {
        skew = -58;
      }
      if (pFont->IsVertical())
        ft_matrix.yx += ft_matrix.yy * skew / 100;
      else
        ft_matrix.xy -= ft_matrix.xx * skew / 100;
    }
    if (pSubstFont->m_bFlagMM) {
      pFont->AdjustMMParams(glyph_index, dest_width,
                            pFont->GetSubstFont()->m_Weight);
    }
  }
  ScopedFontTransform scoped_transform(m_Face, &ft_matrix);
  int load_flags = (m_Face->face_flags & FT_FACE_FLAG_SFNT)
                       ? FXFT_LOAD_NO_BITMAP
                       : (FXFT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
  int error = FXFT_Load_Glyph(m_Face, glyph_index, load_flags);
  if (error) {
    // if an error is returned, try to reload glyphs without hinting.
    if (load_flags & FT_LOAD_NO_HINTING || load_flags & FT_LOAD_NO_SCALE)
      return nullptr;

    load_flags |= FT_LOAD_NO_HINTING;
    error = FXFT_Load_Glyph(m_Face, glyph_index, load_flags);

    if (error)
      return nullptr;
  }
  int weight = 0;
  if (bUseCJKSubFont)
    weight = pSubstFont->m_WeightCJK;
  else
    weight = pSubstFont ? pSubstFont->m_Weight : 0;
  if (pSubstFont && !pSubstFont->m_bFlagMM && weight > 400) {
    uint32_t index = (weight - 400) / 10;
    if (index >= CFX_Font::kWeightPowArraySize)
      return nullptr;
    pdfium::base::CheckedNumeric<signed long> level = 0;
    if (pSubstFont->m_Charset == FX_CHARSET_ShiftJIS)
      level = CFX_Font::s_WeightPow_SHIFTJIS[index] * 2;
    else
      level = CFX_Font::s_WeightPow_11[index];

    level = level *
            (abs(static_cast<int>(ft_matrix.xx)) +
             abs(static_cast<int>(ft_matrix.xy))) /
            36655;
    FXFT_Outline_Embolden(FXFT_Get_Glyph_Outline(m_Face),
                          level.ValueOrDefault(0));
  }
  FXFT_Library_SetLcdFilter(CFX_GEModule::Get()->GetFontMgr()->GetFTLibrary(),
                            FT_LCD_FILTER_DEFAULT);
  error = FXFT_Render_Glyph(m_Face, anti_alias);
  if (error)
    return nullptr;
  int bmwidth = FXFT_Get_Bitmap_Width(FXFT_Get_Glyph_Bitmap(m_Face));
  int bmheight = FXFT_Get_Bitmap_Rows(FXFT_Get_Glyph_Bitmap(m_Face));
  if (bmwidth > kMaxGlyphDimension || bmheight > kMaxGlyphDimension)
    return nullptr;
  int dib_width = bmwidth;
  auto pGlyphBitmap = pdfium::MakeUnique<CFX_GlyphBitmap>();
  pGlyphBitmap->m_pBitmap->Create(
      dib_width, bmheight,
      anti_alias == FXFT_RENDER_MODE_MONO ? FXDIB_1bppMask : FXDIB_8bppMask);
  pGlyphBitmap->m_Left = FXFT_Get_Glyph_BitmapLeft(m_Face);
  pGlyphBitmap->m_Top = FXFT_Get_Glyph_BitmapTop(m_Face);
  int dest_pitch = pGlyphBitmap->m_pBitmap->GetPitch();
  int src_pitch = FXFT_Get_Bitmap_Pitch(FXFT_Get_Glyph_Bitmap(m_Face));
  uint8_t* pDestBuf = pGlyphBitmap->m_pBitmap->GetBuffer();
  uint8_t* pSrcBuf =
      (uint8_t*)FXFT_Get_Bitmap_Buffer(FXFT_Get_Glyph_Bitmap(m_Face));
  if (anti_alias != FXFT_RENDER_MODE_MONO &&
      FXFT_Get_Bitmap_PixelMode(FXFT_Get_Glyph_Bitmap(m_Face)) ==
          FXFT_PIXEL_MODE_MONO) {
    int bytes = anti_alias == FXFT_RENDER_MODE_LCD ? 3 : 1;
    for (int i = 0; i < bmheight; i++) {
      for (int n = 0; n < bmwidth; n++) {
        uint8_t data =
            (pSrcBuf[i * src_pitch + n / 8] & (0x80 >> (n % 8))) ? 255 : 0;
        for (int b = 0; b < bytes; b++)
          pDestBuf[i * dest_pitch + n * bytes + b] = data;
      }
    }
  } else {
    memset(pDestBuf, 0, dest_pitch * bmheight);
    int rowbytes = std::min(abs(src_pitch), dest_pitch);
    for (int row = 0; row < bmheight; row++)
      memcpy(pDestBuf + row * dest_pitch, pSrcBuf + row * src_pitch, rowbytes);
  }
  return pGlyphBitmap;
}

const CFX_PathData* CFX_FaceCache::LoadGlyphPath(const CFX_Font* pFont,
                                                 uint32_t glyph_index,
                                                 uint32_t dest_width) {
  if (!m_Face || glyph_index == kInvalidGlyphIndex)
    return nullptr;

  const auto* pSubstFont = pFont->GetSubstFont();
  int weight = pSubstFont ? pSubstFont->m_Weight : 0;
  int angle = pSubstFont ? pSubstFont->m_ItalicAngle : 0;
  bool vertical = pSubstFont ? pFont->IsVertical() : false;
  const PathMapKey key =
      std::make_tuple(glyph_index, dest_width, weight, angle, vertical);
  auto it = m_PathMap.find(key);
  if (it != m_PathMap.end())
    return it->second.get();

  CFX_PathData* pGlyphPath = pFont->LoadGlyphPathImpl(glyph_index, dest_width);
  m_PathMap[key] = std::unique_ptr<CFX_PathData>(pGlyphPath);
  return pGlyphPath;
}

const CFX_GlyphBitmap* CFX_FaceCache::LoadGlyphBitmap(const CFX_Font* pFont,
                                                      uint32_t glyph_index,
                                                      bool bFontStyle,
                                                      const CFX_Matrix& matrix,
                                                      uint32_t dest_width,
                                                      int anti_alias,
                                                      int& text_flags) {
  if (glyph_index == kInvalidGlyphIndex)
    return nullptr;

  UniqueKeyGen keygen;
  int nMatrixA = static_cast<int>(matrix.a * 10000);
  int nMatrixB = static_cast<int>(matrix.b * 10000);
  int nMatrixC = static_cast<int>(matrix.c * 10000);
  int nMatrixD = static_cast<int>(matrix.d * 10000);
#if _FX_PLATFORM_ != _FX_PLATFORM_APPLE_
  if (pFont->GetSubstFont()) {
    keygen.Generate(9, nMatrixA, nMatrixB, nMatrixC, nMatrixD, dest_width,
                    anti_alias, pFont->GetSubstFont()->m_Weight,
                    pFont->GetSubstFont()->m_ItalicAngle, pFont->IsVertical());
  } else {
    keygen.Generate(6, nMatrixA, nMatrixB, nMatrixC, nMatrixD, dest_width,
                    anti_alias);
  }
#else
  if (text_flags & FXTEXT_NO_NATIVETEXT) {
    if (pFont->GetSubstFont()) {
      keygen.Generate(9, nMatrixA, nMatrixB, nMatrixC, nMatrixD, dest_width,
                      anti_alias, pFont->GetSubstFont()->m_Weight,
                      pFont->GetSubstFont()->m_ItalicAngle,
                      pFont->IsVertical());
    } else {
      keygen.Generate(6, nMatrixA, nMatrixB, nMatrixC, nMatrixD, dest_width,
                      anti_alias);
    }
  } else {
    if (pFont->GetSubstFont()) {
      keygen.Generate(10, nMatrixA, nMatrixB, nMatrixC, nMatrixD, dest_width,
                      anti_alias, pFont->GetSubstFont()->m_Weight,
                      pFont->GetSubstFont()->m_ItalicAngle, pFont->IsVertical(),
                      3);
    } else {
      keygen.Generate(7, nMatrixA, nMatrixB, nMatrixC, nMatrixD, dest_width,
                      anti_alias, 3);
    }
  }
#endif
  ByteString FaceGlyphsKey(keygen.key_, keygen.key_len_);
#if _FX_PLATFORM_ != _FX_PLATFORM_APPLE_ || defined _SKIA_SUPPORT_ || \
    defined _SKIA_SUPPORT_PATHS_
  return LookUpGlyphBitmap(pFont, matrix, FaceGlyphsKey, glyph_index,
                           bFontStyle, dest_width, anti_alias);
#else
  if (text_flags & FXTEXT_NO_NATIVETEXT) {
    return LookUpGlyphBitmap(pFont, matrix, FaceGlyphsKey, glyph_index,
                             bFontStyle, dest_width, anti_alias);
  }
  std::unique_ptr<CFX_GlyphBitmap> pGlyphBitmap;
  auto it = m_SizeMap.find(FaceGlyphsKey);
  if (it != m_SizeMap.end()) {
    SizeGlyphCache* pSizeCache = &(it->second);
    auto it2 = pSizeCache->find(glyph_index);
    if (it2 != pSizeCache->end())
      return it2->second.get();

    pGlyphBitmap = RenderGlyph_Nativetext(pFont, glyph_index, matrix,
                                          dest_width, anti_alias);
    if (pGlyphBitmap) {
      CFX_GlyphBitmap* pResult = pGlyphBitmap.get();
      (*pSizeCache)[glyph_index] = std::move(pGlyphBitmap);
      return pResult;
    }
  } else {
    pGlyphBitmap = RenderGlyph_Nativetext(pFont, glyph_index, matrix,
                                          dest_width, anti_alias);
    if (pGlyphBitmap) {
      CFX_GlyphBitmap* pResult = pGlyphBitmap.get();

      SizeGlyphCache cache;
      cache[glyph_index] = std::move(pGlyphBitmap);

      m_SizeMap[FaceGlyphsKey] = std::move(cache);
      return pResult;
    }
  }
  if (pFont->GetSubstFont()) {
    keygen.Generate(9, nMatrixA, nMatrixB, nMatrixC, nMatrixD, dest_width,
                    anti_alias, pFont->GetSubstFont()->m_Weight,
                    pFont->GetSubstFont()->m_ItalicAngle, pFont->IsVertical());
  } else {
    keygen.Generate(6, nMatrixA, nMatrixB, nMatrixC, nMatrixD, dest_width,
                    anti_alias);
  }
  ByteString FaceGlyphsKey2(keygen.key_, keygen.key_len_);
  text_flags |= FXTEXT_NO_NATIVETEXT;
  return LookUpGlyphBitmap(pFont, matrix, FaceGlyphsKey2, glyph_index,
                           bFontStyle, dest_width, anti_alias);
#endif
}

#if defined _SKIA_SUPPORT_ || defined _SKIA_SUPPORT_PATHS_
CFX_TypeFace* CFX_FaceCache::GetDeviceCache(const CFX_Font* pFont) {
  if (!m_pTypeface) {
    pdfium::span<const uint8_t> span = pFont->GetFontSpan();
    m_pTypeface = SkTypeface::MakeFromStream(
        pdfium::MakeUnique<SkMemoryStream>(span.data(), span.size()));
  }
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  if (!m_pTypeface) {
    sk_sp<SkFontMgr> customMgr(SkFontMgr_New_Custom_Empty());
    pdfium::span<const uint8_t> span = pFont->GetFontSpan();
    m_pTypeface = customMgr->makeFromStream(
        pdfium::MakeUnique<SkMemoryStream>(span.data(), span.size()));
  }
#endif
  return m_pTypeface.get();
}
#endif

#if _FX_PLATFORM_ != _FX_PLATFORM_APPLE_
void CFX_FaceCache::InitPlatform() {}
#endif

CFX_GlyphBitmap* CFX_FaceCache::LookUpGlyphBitmap(
    const CFX_Font* pFont,
    const CFX_Matrix& matrix,
    const ByteString& FaceGlyphsKey,
    uint32_t glyph_index,
    bool bFontStyle,
    uint32_t dest_width,
    int anti_alias) {
  SizeGlyphCache* pSizeCache;
  auto it = m_SizeMap.find(FaceGlyphsKey);
  if (it == m_SizeMap.end()) {
    m_SizeMap[FaceGlyphsKey] = SizeGlyphCache();
    pSizeCache = &(m_SizeMap[FaceGlyphsKey]);
  } else {
    pSizeCache = &(it->second);
  }

  auto it2 = pSizeCache->find(glyph_index);
  if (it2 != pSizeCache->end())
    return it2->second.get();

  std::unique_ptr<CFX_GlyphBitmap> pGlyphBitmap = RenderGlyph(
      pFont, glyph_index, bFontStyle, matrix, dest_width, anti_alias);
  CFX_GlyphBitmap* pResult = pGlyphBitmap.get();
  (*pSizeCache)[glyph_index] = std::move(pGlyphBitmap);
  return pResult;
}
