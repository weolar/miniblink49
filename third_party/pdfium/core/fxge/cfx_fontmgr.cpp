// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_fontmgr.h"

#include <memory>
#include <utility>

#include "core/fxge/cfx_fontmapper.h"
#include "core/fxge/cfx_substfont.h"
#include "core/fxge/cttfontdesc.h"
#include "core/fxge/fontdata/chromefontdata/chromefontdata.h"
#include "core/fxge/fx_font.h"
#include "core/fxge/systemfontinfo_iface.h"
#include "third_party/base/ptr_util.h"

namespace {

struct BuiltinFont {
  const uint8_t* m_pFontData;  // Raw, POD struct.
  uint32_t m_dwSize;
};

const BuiltinFont g_FoxitFonts[14] = {
    {g_FoxitFixedFontData, 17597},
    {g_FoxitFixedBoldFontData, 18055},
    {g_FoxitFixedBoldItalicFontData, 19151},
    {g_FoxitFixedItalicFontData, 18746},
    {g_FoxitSansFontData, 15025},
    {g_FoxitSansBoldFontData, 16344},
    {g_FoxitSansBoldItalicFontData, 16418},
    {g_FoxitSansItalicFontData, 16339},
    {g_FoxitSerifFontData, 19469},
    {g_FoxitSerifBoldFontData, 19395},
    {g_FoxitSerifBoldItalicFontData, 20733},
    {g_FoxitSerifItalicFontData, 21227},
    {g_FoxitSymbolFontData, 16729},
    {g_FoxitDingbatsFontData, 29513},
};

const BuiltinFont g_MMFonts[2] = {
    {g_FoxitSerifMMFontData, 113417},
    {g_FoxitSansMMFontData, 66919},
};

ByteString KeyNameFromFace(const ByteString& face_name,
                           int weight,
                           bool bItalic) {
  ByteString key(face_name);
  key += ',';
  key += ByteString::FormatInteger(weight);
  key += bItalic ? 'I' : 'N';
  return key;
}

ByteString KeyNameFromSize(int ttc_size, uint32_t checksum) {
  return ByteString::Format("%d:%d", ttc_size, checksum);
}

int GetTTCIndex(const uint8_t* pFontData,
                uint32_t ttc_size,
                uint32_t font_offset) {
  const uint8_t* p = pFontData + 8;
  uint32_t nfont = GET_TT_LONG(p);
  uint32_t index;
  for (index = 0; index < nfont; index++) {
    p = pFontData + 12 + index * 4;
    if (GET_TT_LONG(p) == font_offset)
      break;
  }
  return index < nfont ? index : 0;
}

}  // namespace

CFX_FontMgr::CFX_FontMgr() {
  m_pBuiltinMapper = pdfium::MakeUnique<CFX_FontMapper>(this);
}

CFX_FontMgr::~CFX_FontMgr() {
  // |m_FaceMap| and |m_pBuiltinMapper| reference |m_FTLibrary|, so they must
  // be destroyed first.
  m_FaceMap.clear();
  m_pBuiltinMapper.reset();
  FXFT_Done_FreeType(m_FTLibrary);
}

void CFX_FontMgr::InitFTLibrary() {
  if (m_FTLibrary)
    return;

  FXFT_Init_FreeType(&m_FTLibrary);
  m_FTLibrarySupportsHinting =
      SetLcdFilterMode() || FreeTypeVersionSupportsHinting();
}

void CFX_FontMgr::SetSystemFontInfo(
    std::unique_ptr<SystemFontInfoIface> pFontInfo) {
  m_pBuiltinMapper->SetSystemFontInfo(std::move(pFontInfo));
}

FXFT_Face CFX_FontMgr::FindSubstFont(const ByteString& face_name,
                                     bool bTrueType,
                                     uint32_t flags,
                                     int weight,
                                     int italic_angle,
                                     int CharsetCP,
                                     CFX_SubstFont* pSubstFont) {
  InitFTLibrary();
  return m_pBuiltinMapper->FindSubstFont(face_name, bTrueType, flags, weight,
                                         italic_angle, CharsetCP, pSubstFont);
}

FXFT_Face CFX_FontMgr::GetCachedFace(const ByteString& face_name,
                                     int weight,
                                     bool bItalic,
                                     uint8_t** pFontData) {
  auto it = m_FaceMap.find(KeyNameFromFace(face_name, weight, bItalic));
  if (it == m_FaceMap.end())
    return nullptr;

  CTTFontDesc* pFontDesc = it->second.get();
  *pFontData = pFontDesc->FontData();
  pFontDesc->AddRef();
  return pFontDesc->GetFace(0);
}

FXFT_Face CFX_FontMgr::AddCachedFace(
    const ByteString& face_name,
    int weight,
    bool bItalic,
    std::unique_ptr<uint8_t, FxFreeDeleter> pData,
    uint32_t size,
    int face_index) {
  InitFTLibrary();

  FXFT_Face face = nullptr;
  int ret =
      FXFT_New_Memory_Face(m_FTLibrary, pData.get(), size, face_index, &face);
  if (ret)
    return nullptr;

  ret = FXFT_Set_Pixel_Sizes(face, 64, 64);
  if (ret)
    return nullptr;

  auto pFontDesc = pdfium::MakeUnique<CTTFontDesc>(std::move(pData));
  pFontDesc->SetFace(0, face);
  CTTFontDesc* pResult = pFontDesc.get();
  m_FaceMap[KeyNameFromFace(face_name, weight, bItalic)] = std::move(pFontDesc);
  return pResult->GetFace(0);
}

FXFT_Face CFX_FontMgr::GetCachedTTCFace(int ttc_size,
                                        uint32_t checksum,
                                        int font_offset,
                                        uint8_t** pFontData) {
  auto it = m_FaceMap.find(KeyNameFromSize(ttc_size, checksum));
  if (it == m_FaceMap.end())
    return nullptr;

  CTTFontDesc* pFontDesc = it->second.get();
  *pFontData = pFontDesc->FontData();
  int face_index = GetTTCIndex(pFontDesc->FontData(), ttc_size, font_offset);
  if (!pFontDesc->GetFace(face_index)) {
    pFontDesc->SetFace(face_index, GetFixedFace({pFontDesc->FontData(),
                                                 static_cast<size_t>(ttc_size)},
                                                face_index));
  }
  pFontDesc->AddRef();
  return pFontDesc->GetFace(face_index);
}

FXFT_Face CFX_FontMgr::AddCachedTTCFace(
    int ttc_size,
    uint32_t checksum,
    std::unique_ptr<uint8_t, FxFreeDeleter> pData,
    uint32_t size,
    int font_offset) {
  int face_index = GetTTCIndex(pData.get(), ttc_size, font_offset);
  FXFT_Face face =
      GetFixedFace({pData.get(), static_cast<size_t>(ttc_size)}, face_index);
  auto pFontDesc = pdfium::MakeUnique<CTTFontDesc>(std::move(pData));
  pFontDesc->SetFace(face_index, face);
  m_FaceMap[KeyNameFromSize(ttc_size, checksum)] = std::move(pFontDesc);
  return face;
}

FXFT_Face CFX_FontMgr::GetFixedFace(pdfium::span<const uint8_t> span,
                                    int face_index) {
  InitFTLibrary();
  FXFT_Face face = nullptr;
  if (FXFT_New_Memory_Face(m_FTLibrary, span.data(), span.size(), face_index,
                           &face)) {
    return nullptr;
  }
  return FXFT_Set_Pixel_Sizes(face, 64, 64) ? nullptr : face;
}

void CFX_FontMgr::ReleaseFace(FXFT_Face face) {
  if (!face)
    return;
  bool bNeedFaceDone = true;
  for (auto it = m_FaceMap.begin(); it != m_FaceMap.end(); ++it) {
    CTTFontDesc::ReleaseStatus nRet = it->second->ReleaseFace(face);
    if (nRet == CTTFontDesc::kNotAppropriate)
      continue;
    bNeedFaceDone = false;
    if (nRet == CTTFontDesc::kReleased)
      m_FaceMap.erase(it);
    break;
  }
  if (bNeedFaceDone && !m_pBuiltinMapper->IsBuiltinFace(face))
    FXFT_Done_Face(face);
}

Optional<pdfium::span<const uint8_t>> CFX_FontMgr::GetBuiltinFont(
    size_t index) {
  if (index < FX_ArraySize(g_FoxitFonts)) {
    return pdfium::make_span(g_FoxitFonts[index].m_pFontData,
                             g_FoxitFonts[index].m_dwSize);
  }
  size_t mm_index = index - FX_ArraySize(g_FoxitFonts);
  if (mm_index < FX_ArraySize(g_MMFonts)) {
    return pdfium::make_span(g_MMFonts[mm_index].m_pFontData,
                             g_MMFonts[mm_index].m_dwSize);
  }
  return {};
}

bool CFX_FontMgr::FreeTypeVersionSupportsHinting() const {
  FT_Int major;
  FT_Int minor;
  FT_Int patch;
  FXFT_Library_Version(m_FTLibrary, &major, &minor, &patch);
  // Freetype versions >= 2.8.1 support hinting even if subpixel rendering is
  // disabled. https://sourceforge.net/projects/freetype/files/freetype2/2.8.1/
  return major > 2 || (major == 2 && minor > 8) ||
         (major == 2 && minor == 8 && patch >= 1);
}

bool CFX_FontMgr::SetLcdFilterMode() const {
  return FXFT_Library_SetLcdFilter(m_FTLibrary, FT_LCD_FILTER_DEFAULT) !=
         FT_Err_Unimplemented_Feature;
}
