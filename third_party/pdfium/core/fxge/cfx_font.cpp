// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_font.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_stream.h"
#include "core/fxge/cfx_facecache.h"
#include "core/fxge/cfx_fontcache.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_substfont.h"
#include "core/fxge/fx_freetype.h"
#include "third_party/base/ptr_util.h"

#define EM_ADJUST(em, a) (em == 0 ? (a) : (a)*1000 / em)

namespace {

constexpr int kThousandthMinInt = std::numeric_limits<int>::min() / 1000;
constexpr int kThousandthMaxInt = std::numeric_limits<int>::max() / 1000;

struct OUTLINE_PARAMS {
  CFX_PathData* m_pPath;
  int m_CurX;
  int m_CurY;
  float m_CoordUnit;
};

#ifdef PDF_ENABLE_XFA

unsigned long FTStreamRead(FXFT_Stream stream,
                           unsigned long offset,
                           unsigned char* buffer,
                           unsigned long count) {
  if (count == 0)
    return 0;

  IFX_SeekableReadStream* pFile =
      static_cast<IFX_SeekableReadStream*>(stream->descriptor.pointer);
  return pFile && pFile->ReadBlockAtOffset(buffer, offset, count) ? count : 0;
}

void FTStreamClose(FXFT_Stream stream) {}

FXFT_Face LoadFileImp(FXFT_Library library,
                      const RetainPtr<IFX_SeekableReadStream>& pFile,
                      int32_t faceIndex,
                      std::unique_ptr<FXFT_StreamRec>* stream) {
  auto stream1 = pdfium::MakeUnique<FXFT_StreamRec>();
  stream1->base = nullptr;
  stream1->size = static_cast<unsigned long>(pFile->GetSize());
  stream1->pos = 0;
  stream1->descriptor.pointer = static_cast<void*>(pFile.Get());
  stream1->close = FTStreamClose;
  stream1->read = FTStreamRead;

  FXFT_Open_Args args;
  args.flags = FT_OPEN_STREAM;
  args.stream = stream1.get();

  FXFT_Face face;
  if (FXFT_Open_Face(library, &args, faceIndex, &face))
    return nullptr;

  if (stream)
    *stream = std::move(stream1);

  return face;
}
#endif  // PDF_ENABLE_XFA

void Outline_CheckEmptyContour(OUTLINE_PARAMS* param) {
  std::vector<FX_PATHPOINT>& points = param->m_pPath->GetPoints();
  size_t size = points.size();

  if (size >= 2 && points[size - 2].IsTypeAndOpen(FXPT_TYPE::MoveTo) &&
      points[size - 2].m_Point == points[size - 1].m_Point) {
    size -= 2;
  }
  if (size >= 4 && points[size - 4].IsTypeAndOpen(FXPT_TYPE::MoveTo) &&
      points[size - 3].IsTypeAndOpen(FXPT_TYPE::BezierTo) &&
      points[size - 3].m_Point == points[size - 4].m_Point &&
      points[size - 2].m_Point == points[size - 4].m_Point &&
      points[size - 1].m_Point == points[size - 4].m_Point) {
    size -= 4;
  }
  points.resize(size);
}

int Outline_MoveTo(const FXFT_Vector* to, void* user) {
  OUTLINE_PARAMS* param = static_cast<OUTLINE_PARAMS*>(user);

  Outline_CheckEmptyContour(param);

  param->m_pPath->ClosePath();
  param->m_pPath->AppendPoint(
      CFX_PointF(to->x / param->m_CoordUnit, to->y / param->m_CoordUnit),
      FXPT_TYPE::MoveTo, false);

  param->m_CurX = to->x;
  param->m_CurY = to->y;
  return 0;
}

int Outline_LineTo(const FXFT_Vector* to, void* user) {
  OUTLINE_PARAMS* param = static_cast<OUTLINE_PARAMS*>(user);

  param->m_pPath->AppendPoint(
      CFX_PointF(to->x / param->m_CoordUnit, to->y / param->m_CoordUnit),
      FXPT_TYPE::LineTo, false);

  param->m_CurX = to->x;
  param->m_CurY = to->y;
  return 0;
}

int Outline_ConicTo(const FXFT_Vector* control,
                    const FXFT_Vector* to,
                    void* user) {
  OUTLINE_PARAMS* param = static_cast<OUTLINE_PARAMS*>(user);

  param->m_pPath->AppendPoint(
      CFX_PointF((param->m_CurX + (control->x - param->m_CurX) * 2 / 3) /
                     param->m_CoordUnit,
                 (param->m_CurY + (control->y - param->m_CurY) * 2 / 3) /
                     param->m_CoordUnit),
      FXPT_TYPE::BezierTo, false);

  param->m_pPath->AppendPoint(
      CFX_PointF((control->x + (to->x - control->x) / 3) / param->m_CoordUnit,
                 (control->y + (to->y - control->y) / 3) / param->m_CoordUnit),
      FXPT_TYPE::BezierTo, false);

  param->m_pPath->AppendPoint(
      CFX_PointF(to->x / param->m_CoordUnit, to->y / param->m_CoordUnit),
      FXPT_TYPE::BezierTo, false);

  param->m_CurX = to->x;
  param->m_CurY = to->y;
  return 0;
}

int Outline_CubicTo(const FXFT_Vector* control1,
                    const FXFT_Vector* control2,
                    const FXFT_Vector* to,
                    void* user) {
  OUTLINE_PARAMS* param = static_cast<OUTLINE_PARAMS*>(user);

  param->m_pPath->AppendPoint(CFX_PointF(control1->x / param->m_CoordUnit,
                                         control1->y / param->m_CoordUnit),
                              FXPT_TYPE::BezierTo, false);

  param->m_pPath->AppendPoint(CFX_PointF(control2->x / param->m_CoordUnit,
                                         control2->y / param->m_CoordUnit),
                              FXPT_TYPE::BezierTo, false);

  param->m_pPath->AppendPoint(
      CFX_PointF(to->x / param->m_CoordUnit, to->y / param->m_CoordUnit),
      FXPT_TYPE::BezierTo, false);

  param->m_CurX = to->x;
  param->m_CurY = to->y;
  return 0;
}

}  // namespace

const char CFX_Font::s_AngleSkew[] = {
    0,  2,  3,  5,  7,  9,  11, 12, 14, 16, 18, 19, 21, 23, 25,
    27, 29, 31, 32, 34, 36, 38, 40, 42, 45, 47, 49, 51, 53, 55,
};

const uint8_t CFX_Font::s_WeightPow[] = {
    0,  3,  6,  7,  8,  9,  11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 36, 37,
    37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42,
    42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47,
    47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 50,
    51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53,
};

const uint8_t CFX_Font::s_WeightPow_11[] = {
    0,  4,  7,  8,  9,  10, 12, 13, 15, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 39, 39, 40, 40, 41,
    41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46,
    46, 43, 47, 47, 48, 48, 48, 48, 45, 50, 50, 50, 46, 51, 51, 51, 52,
    52, 52, 52, 53, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 55,
    56, 56, 56, 56, 56, 57, 57, 57, 57, 57, 58, 58, 58, 58, 58,
};

const uint8_t CFX_Font::s_WeightPow_SHIFTJIS[] = {
    0,  0,  1,  2,  3,  4,  5,  7,  8,  10, 11, 13, 14, 16, 17, 19, 21,
    22, 24, 26, 28, 30, 32, 33, 35, 37, 39, 41, 43, 45, 48, 48, 48, 48,
    49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53,
    53, 53, 53, 53, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 56, 56, 56,
    56, 56, 56, 57, 57, 57, 57, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58,
    59, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 60, 60,
};

const CFX_Font::CharsetFontMap CFX_Font::defaultTTFMap[] = {
    {FX_CHARSET_ANSI, kDefaultAnsiFontName},
    {FX_CHARSET_ChineseSimplified, "SimSun"},
    {FX_CHARSET_ChineseTraditional, "MingLiU"},
    {FX_CHARSET_ShiftJIS, "MS Gothic"},
    {FX_CHARSET_Hangul, "Batang"},
    {FX_CHARSET_MSWin_Cyrillic, "Arial"},
#if _FX_PLATFORM_ == _FX_PLATFORM_LINUX_ || _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
    {FX_CHARSET_MSWin_EasternEuropean, "Arial"},
#else
    {FX_CHARSET_MSWin_EasternEuropean, "Tahoma"},
#endif
    {FX_CHARSET_MSWin_Arabic, "Arial"},
    {-1, nullptr}};

// static
const char CFX_Font::kDefaultAnsiFontName[] = "Helvetica";
// static
const char CFX_Font::kUniversalDefaultFontName[] = "Arial Unicode MS";

// static
ByteString CFX_Font::GetDefaultFontNameByCharset(uint8_t nCharset) {
  int i = 0;
  while (defaultTTFMap[i].charset != -1) {
    if (nCharset == static_cast<uint8_t>(defaultTTFMap[i].charset))
      return defaultTTFMap[i].fontname;
    ++i;
  }
  return kUniversalDefaultFontName;
}

// static
uint8_t CFX_Font::GetCharSetFromUnicode(uint16_t word) {
  // to avoid CJK Font to show ASCII
  if (word < 0x7F)
    return FX_CHARSET_ANSI;

  // find new charset
  if ((word >= 0x4E00 && word <= 0x9FA5) ||
      (word >= 0xE7C7 && word <= 0xE7F3) ||
      (word >= 0x3000 && word <= 0x303F) ||
      (word >= 0x2000 && word <= 0x206F)) {
    return FX_CHARSET_ChineseSimplified;
  }

  if (((word >= 0x3040) && (word <= 0x309F)) ||
      ((word >= 0x30A0) && (word <= 0x30FF)) ||
      ((word >= 0x31F0) && (word <= 0x31FF)) ||
      ((word >= 0xFF00) && (word <= 0xFFEF))) {
    return FX_CHARSET_ShiftJIS;
  }

  if (((word >= 0xAC00) && (word <= 0xD7AF)) ||
      ((word >= 0x1100) && (word <= 0x11FF)) ||
      ((word >= 0x3130) && (word <= 0x318F))) {
    return FX_CHARSET_Hangul;
  }

  if (word >= 0x0E00 && word <= 0x0E7F)
    return FX_CHARSET_Thai;

  if ((word >= 0x0370 && word <= 0x03FF) || (word >= 0x1F00 && word <= 0x1FFF))
    return FX_CHARSET_MSWin_Greek;

  if ((word >= 0x0600 && word <= 0x06FF) || (word >= 0xFB50 && word <= 0xFEFC))
    return FX_CHARSET_MSWin_Arabic;

  if (word >= 0x0590 && word <= 0x05FF)
    return FX_CHARSET_MSWin_Hebrew;

  if (word >= 0x0400 && word <= 0x04FF)
    return FX_CHARSET_MSWin_Cyrillic;

  if (word >= 0x0100 && word <= 0x024F)
    return FX_CHARSET_MSWin_EasternEuropean;

  if (word >= 0x1E00 && word <= 0x1EFF)
    return FX_CHARSET_MSWin_Vietnamese;

  return FX_CHARSET_ANSI;
}

CFX_Font::CFX_Font() = default;

#ifdef PDF_ENABLE_XFA
bool CFX_Font::LoadFile(const RetainPtr<IFX_SeekableReadStream>& pFile,
                        int nFaceIndex) {
  m_bEmbedded = false;

  CFX_FontMgr* pFontMgr = CFX_GEModule::Get()->GetFontMgr();
  pFontMgr->InitFTLibrary();

  std::unique_ptr<FXFT_StreamRec> stream;
  m_Face = LoadFileImp(pFontMgr->GetFTLibrary(), pFile, nFaceIndex, &stream);
  if (!m_Face)
    return false;

  m_pOwnedStream = std::move(stream);
  FXFT_Set_Pixel_Sizes(m_Face.Get(), 0, 64);
  return true;
}

#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
void CFX_Font::SetFace(FXFT_Face face) {
  ClearFaceCache();
  m_Face = face;
}

void CFX_Font::SetSubstFont(std::unique_ptr<CFX_SubstFont> subst) {
  m_pSubstFont = std::move(subst);
}
#endif  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
#endif  // PDF_ENABLE_XFA

CFX_Font::~CFX_Font() {
  m_FontData = {};  // m_FontData can't outive m_Face.
  if (m_Face)
    DeleteFace();

#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  ReleasePlatformResource();
#endif
}

void CFX_Font::DeleteFace() {
  ClearFaceCache();
  if (m_bEmbedded)
    FXFT_Done_Face(m_Face.Release());
  else
    CFX_GEModule::Get()->GetFontMgr()->ReleaseFace(m_Face.Release());
}

void CFX_Font::LoadSubst(const ByteString& face_name,
                         bool bTrueType,
                         uint32_t flags,
                         int weight,
                         int italic_angle,
                         int CharsetCP,
                         bool bVertical) {
  m_bEmbedded = false;
  m_bVertical = bVertical;
  m_pSubstFont = pdfium::MakeUnique<CFX_SubstFont>();
  m_Face = CFX_GEModule::Get()->GetFontMgr()->FindSubstFont(
      face_name, bTrueType, flags, weight, italic_angle, CharsetCP,
      m_pSubstFont.get());
  if (m_Face) {
    m_FontData = {FXFT_Get_Face_Stream_Base(m_Face.Get()),
                  FXFT_Get_Face_Stream_Size(m_Face.Get())};
  }
}

uint32_t CFX_Font::GetGlyphWidth(uint32_t glyph_index) {
  if (!m_Face)
    return 0;
  if (m_pSubstFont && m_pSubstFont->m_bFlagMM)
    AdjustMMParams(glyph_index, 0, 0);
  int err = FXFT_Load_Glyph(
      m_Face.Get(), glyph_index,
      FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH);
  if (err)
    return 0;

  int horiAdvance = FXFT_Get_Glyph_HoriAdvance(m_Face.Get());
  if (horiAdvance < 0 || horiAdvance > kThousandthMaxInt)
    return 0;

  return EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face.Get()), horiAdvance);
}

bool CFX_Font::LoadEmbedded(pdfium::span<const uint8_t> src_span) {
  m_pFontDataAllocation =
      std::vector<uint8_t>(src_span.begin(), src_span.end());
  m_Face =
      CFX_GEModule::Get()->GetFontMgr()->GetFixedFace(m_pFontDataAllocation, 0);
  m_bEmbedded = true;
  m_FontData = m_pFontDataAllocation;
  return !!m_Face;
}

bool CFX_Font::IsTTFont() const {
  return m_Face && FXFT_Is_Face_TT_OT(m_Face.Get()) == FXFT_FACE_FLAG_SFNT;
}

int CFX_Font::GetAscent() const {
  if (!m_Face)
    return 0;

  int ascender = FXFT_Get_Face_Ascender(m_Face.Get());
  if (ascender < kThousandthMinInt || ascender > kThousandthMaxInt)
    return 0;

  return EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face.Get()), ascender);
}

int CFX_Font::GetDescent() const {
  if (!m_Face)
    return 0;

  int descender = FXFT_Get_Face_Descender(m_Face.Get());
  if (descender < kThousandthMinInt || descender > kThousandthMaxInt)
    return 0;

  return EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face.Get()), descender);
}

bool CFX_Font::GetGlyphBBox(uint32_t glyph_index, FX_RECT* pBBox) {
  if (!m_Face)
    return false;

  if (FXFT_Is_Face_Tricky(m_Face.Get())) {
    int error = FXFT_Set_Char_Size(m_Face.Get(), 0, 1000 * 64, 72, 72);
    if (error)
      return false;

    error = FXFT_Load_Glyph(m_Face.Get(), glyph_index,
                            FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH);
    if (error)
      return false;

    FXFT_BBox cbox;
    FT_Glyph glyph;
    error = FXFT_Get_Glyph(m_Face->glyph, &glyph);
    if (error)
      return false;

    FXFT_Glyph_Get_CBox(glyph, FXFT_GLYPH_BBOX_PIXELS, &cbox);
    int pixel_size_x = m_Face->size->metrics.x_ppem,
        pixel_size_y = m_Face->size->metrics.y_ppem;
    if (pixel_size_x == 0 || pixel_size_y == 0) {
      pBBox->left = cbox.xMin;
      pBBox->right = cbox.xMax;
      pBBox->top = cbox.yMax;
      pBBox->bottom = cbox.yMin;
    } else {
      pBBox->left = cbox.xMin * 1000 / pixel_size_x;
      pBBox->right = cbox.xMax * 1000 / pixel_size_x;
      pBBox->top = cbox.yMax * 1000 / pixel_size_y;
      pBBox->bottom = cbox.yMin * 1000 / pixel_size_y;
    }
    pBBox->top = std::min(
        pBBox->top, static_cast<int32_t>(FXFT_Get_Face_Ascender(m_Face.Get())));
    pBBox->bottom =
        std::max(pBBox->bottom,
                 static_cast<int32_t>(FXFT_Get_Face_Descender(m_Face.Get())));
    FT_Done_Glyph(glyph);
    return FXFT_Set_Pixel_Sizes(m_Face.Get(), 0, 64) == 0;
  }
  if (FXFT_Load_Glyph(
          m_Face.Get(), glyph_index,
          FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH)) {
    return false;
  }
  int em = FXFT_Get_Face_UnitsPerEM(m_Face.Get());
  if (em == 0) {
    pBBox->left = FXFT_Get_Glyph_HoriBearingX(m_Face.Get());
    pBBox->bottom = FXFT_Get_Glyph_HoriBearingY(m_Face.Get());
    pBBox->top = pBBox->bottom - FXFT_Get_Glyph_Height(m_Face.Get());
    pBBox->right = pBBox->left + FXFT_Get_Glyph_Width(m_Face.Get());
  } else {
    pBBox->left = FXFT_Get_Glyph_HoriBearingX(m_Face.Get()) * 1000 / em;
    pBBox->top = (FXFT_Get_Glyph_HoriBearingY(m_Face.Get()) -
                  FXFT_Get_Glyph_Height(m_Face.Get())) *
                 1000 / em;
    pBBox->right = (FXFT_Get_Glyph_HoriBearingX(m_Face.Get()) +
                    FXFT_Get_Glyph_Width(m_Face.Get())) *
                   1000 / em;
    pBBox->bottom = (FXFT_Get_Glyph_HoriBearingY(m_Face.Get())) * 1000 / em;
  }
  return true;
}

bool CFX_Font::IsItalic() const {
  if (!m_Face)
    return false;
  if (FXFT_Is_Face_Italic(m_Face.Get()) == FXFT_STYLE_FLAG_ITALIC)
    return true;

  ByteString str(FXFT_Get_Face_Style_Name(m_Face.Get()));
  str.MakeLower();
  return str.Contains("italic");
}

bool CFX_Font::IsBold() const {
  return m_Face && FXFT_Is_Face_Bold(m_Face.Get()) == FXFT_STYLE_FLAG_BOLD;
}

bool CFX_Font::IsFixedWidth() const {
  return m_Face && FXFT_Is_Face_fixedwidth(m_Face.Get()) != 0;
}

ByteString CFX_Font::GetPsName() const {
  if (!m_Face)
    return ByteString();

  ByteString psName = FXFT_Get_Postscript_Name(m_Face.Get());
  if (psName.IsEmpty())
    psName = "Untitled";
  return psName;
}

ByteString CFX_Font::GetFamilyName() const {
  if (!m_Face && !m_pSubstFont)
    return ByteString();
  if (m_Face)
    return ByteString(FXFT_Get_Face_Family_Name(m_Face.Get()));

  return m_pSubstFont->m_Family;
}

ByteString CFX_Font::GetFaceName() const {
  if (!m_Face && !m_pSubstFont)
    return ByteString();
  if (m_Face) {
    ByteString style = ByteString(FXFT_Get_Face_Style_Name(m_Face.Get()));
    ByteString facename = GetFamilyName();
    if (facename.IsEmpty())
      facename = "Untitled";
    if (!style.IsEmpty() && style != "Regular")
      facename += " " + style;
    return facename;
  }
  return m_pSubstFont->m_Family;
}

bool CFX_Font::GetBBox(FX_RECT* pBBox) {
  if (!m_Face)
    return false;

  int em = FXFT_Get_Face_UnitsPerEM(m_Face.Get());
  if (em == 0) {
    pBBox->left = FXFT_Get_Face_xMin(m_Face.Get());
    pBBox->bottom = FXFT_Get_Face_yMax(m_Face.Get());
    pBBox->top = FXFT_Get_Face_yMin(m_Face.Get());
    pBBox->right = FXFT_Get_Face_xMax(m_Face.Get());
  } else {
    pBBox->left = FXFT_Get_Face_xMin(m_Face.Get()) * 1000 / em;
    pBBox->top = FXFT_Get_Face_yMin(m_Face.Get()) * 1000 / em;
    pBBox->right = FXFT_Get_Face_xMax(m_Face.Get()) * 1000 / em;
    pBBox->bottom = FXFT_Get_Face_yMax(m_Face.Get()) * 1000 / em;
  }
  return true;
}

CFX_FaceCache* CFX_Font::GetFaceCache() const {
  if (!m_FaceCache)
    m_FaceCache = CFX_GEModule::Get()->GetFontCache()->GetCachedFace(this);
  return m_FaceCache.Get();
}

void CFX_Font::ClearFaceCache() {
  if (!m_FaceCache)
    return;

  m_FaceCache = nullptr;
  CFX_GEModule::Get()->GetFontCache()->ReleaseCachedFace(this);
}

void CFX_Font::AdjustMMParams(int glyph_index,
                              int dest_width,
                              int weight) const {
  ASSERT(dest_width >= 0);
  FXFT_MM_Var pMasters = nullptr;
  FXFT_Get_MM_Var(m_Face.Get(), &pMasters);
  if (!pMasters)
    return;

  long coords[2];
  if (weight == 0)
    coords[0] = FXFT_Get_MM_Axis_Def(FXFT_Get_MM_Axis(pMasters, 0)) / 65536;
  else
    coords[0] = weight;

  if (dest_width == 0) {
    coords[1] = FXFT_Get_MM_Axis_Def(FXFT_Get_MM_Axis(pMasters, 1)) / 65536;
  } else {
    int min_param = FXFT_Get_MM_Axis_Min(FXFT_Get_MM_Axis(pMasters, 1)) / 65536;
    int max_param = FXFT_Get_MM_Axis_Max(FXFT_Get_MM_Axis(pMasters, 1)) / 65536;
    coords[1] = min_param;
    FXFT_Set_MM_Design_Coordinates(m_Face.Get(), 2, coords);
    FXFT_Load_Glyph(m_Face.Get(), glyph_index,
                    FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH);
    int min_width = FXFT_Get_Glyph_HoriAdvance(m_Face.Get()) * 1000 /
                    FXFT_Get_Face_UnitsPerEM(m_Face.Get());
    coords[1] = max_param;
    FXFT_Set_MM_Design_Coordinates(m_Face.Get(), 2, coords);
    FXFT_Load_Glyph(m_Face.Get(), glyph_index,
                    FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH);
    int max_width = FXFT_Get_Glyph_HoriAdvance(m_Face.Get()) * 1000 /
                    FXFT_Get_Face_UnitsPerEM(m_Face.Get());
    if (max_width == min_width) {
      FXFT_Free(m_Face.Get(), pMasters);
      return;
    }
    int param = min_param + (max_param - min_param) * (dest_width - min_width) /
                                (max_width - min_width);
    coords[1] = param;
  }
  FXFT_Free(m_Face.Get(), pMasters);
  FXFT_Set_MM_Design_Coordinates(m_Face.Get(), 2, coords);
}

CFX_PathData* CFX_Font::LoadGlyphPathImpl(uint32_t glyph_index,
                                          uint32_t dest_width) const {
  if (!m_Face)
    return nullptr;

  FXFT_Set_Pixel_Sizes(m_Face.Get(), 0, 64);
  FXFT_Matrix ft_matrix = {65536, 0, 0, 65536};
  if (m_pSubstFont) {
    if (m_pSubstFont->m_ItalicAngle) {
      int skew = m_pSubstFont->m_ItalicAngle;
      // |skew| is nonpositive so |-skew| is used as the index. We need to make
      // sure |skew| != INT_MIN since -INT_MIN is undefined.
      if (skew <= 0 && skew != std::numeric_limits<int>::min() &&
          static_cast<size_t>(-skew) < kAngleSkewArraySize) {
        skew = -s_AngleSkew[-skew];
      } else {
        skew = -58;
      }
      if (m_bVertical)
        ft_matrix.yx += ft_matrix.yy * skew / 100;
      else
        ft_matrix.xy -= ft_matrix.xx * skew / 100;
    }
    if (m_pSubstFont->m_bFlagMM)
      AdjustMMParams(glyph_index, dest_width, m_pSubstFont->m_Weight);
  }
  ScopedFontTransform scoped_transform(m_Face.Get(), &ft_matrix);
  int load_flags = FXFT_LOAD_NO_BITMAP;
  if (!(m_Face->face_flags & FT_FACE_FLAG_SFNT) || !FT_IS_TRICKY(m_Face.Get()))
    load_flags |= FT_LOAD_NO_HINTING;
  if (FXFT_Load_Glyph(m_Face.Get(), glyph_index, load_flags))
    return nullptr;
  if (m_pSubstFont && !m_pSubstFont->m_bFlagMM &&
      m_pSubstFont->m_Weight > 400) {
    uint32_t index = (m_pSubstFont->m_Weight - 400) / 10;
    index = std::min(index, static_cast<uint32_t>(kWeightPowArraySize - 1));
    int level = 0;
    if (m_pSubstFont->m_Charset == FX_CHARSET_ShiftJIS)
      level = s_WeightPow_SHIFTJIS[index] * 2 * 65536 / 36655;
    else
      level = s_WeightPow[index] * 2;
    FXFT_Outline_Embolden(FXFT_Get_Glyph_Outline(m_Face.Get()), level);
  }

  FXFT_Outline_Funcs funcs;
  funcs.move_to = Outline_MoveTo;
  funcs.line_to = Outline_LineTo;
  funcs.conic_to = Outline_ConicTo;
  funcs.cubic_to = Outline_CubicTo;
  funcs.shift = 0;
  funcs.delta = 0;

  OUTLINE_PARAMS params;
  auto pPath = pdfium::MakeUnique<CFX_PathData>();
  params.m_pPath = pPath.get();
  params.m_CurX = params.m_CurY = 0;
  params.m_CoordUnit = 64 * 64.0;

  FXFT_Outline_Decompose(FXFT_Get_Glyph_Outline(m_Face.Get()), &funcs, &params);
  if (pPath->GetPoints().empty())
    return nullptr;

  Outline_CheckEmptyContour(&params);
  pPath->ClosePath();

  return pPath.release();
}

const CFX_GlyphBitmap* CFX_Font::LoadGlyphBitmap(uint32_t glyph_index,
                                                 bool bFontStyle,
                                                 const CFX_Matrix& matrix,
                                                 uint32_t dest_width,
                                                 int anti_alias,
                                                 int& text_flags) const {
  return GetFaceCache()->LoadGlyphBitmap(this, glyph_index, bFontStyle, matrix,
                                         dest_width, anti_alias, text_flags);
}

const CFX_PathData* CFX_Font::LoadGlyphPath(uint32_t glyph_index,
                                            uint32_t dest_width) const {
  return GetFaceCache()->LoadGlyphPath(this, glyph_index, dest_width);
}

#if defined _SKIA_SUPPORT_ || _SKIA_SUPPORT_PATHS_
CFX_TypeFace* CFX_Font::GetDeviceCache() const {
  return GetFaceCache()->GetDeviceCache(this);
}
#endif
