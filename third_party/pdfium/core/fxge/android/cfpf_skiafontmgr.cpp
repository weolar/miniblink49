// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/android/cfpf_skiafontmgr.h"

#include <algorithm>
#include <utility>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_stream.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxge/android/cfpf_skiafont.h"
#include "core/fxge/android/cfpf_skiapathfont.h"
#include "core/fxge/fx_freetype.h"
#include "third_party/base/ptr_util.h"

namespace {

constexpr int FPF_SKIAMATCHWEIGHT_NAME1 = 62;
constexpr int FPF_SKIAMATCHWEIGHT_NAME2 = 60;
constexpr int FPF_SKIAMATCHWEIGHT_1 = 16;
constexpr int FPF_SKIAMATCHWEIGHT_2 = 8;

struct FPF_SKIAFONTMAP {
  uint32_t dwFamily;
  uint32_t dwSubSt;
};

const FPF_SKIAFONTMAP g_SkiaFontmap[] = {
    {0x58c5083, 0xc8d2e345},  {0x5dfade2, 0xe1633081},
    {0x684317d, 0xe1633081},  {0x14ee2d13, 0xc8d2e345},
    {0x3918fe2d, 0xbbeeec72}, {0x3b98b31c, 0xe1633081},
    {0x3d49f40e, 0xe1633081}, {0x432c41c5, 0xe1633081},
    {0x491b6ad0, 0xe1633081}, {0x5612cab1, 0x59b9f8f1},
    {0x779ce19d, 0xc8d2e345}, {0x7cc9510b, 0x59b9f8f1},
    {0x83746053, 0xbbeeec72}, {0xaaa60c03, 0xbbeeec72},
    {0xbf85ff26, 0xe1633081}, {0xc04fe601, 0xbbeeec72},
    {0xca3812d5, 0x59b9f8f1}, {0xca383e15, 0x59b9f8f1},
    {0xcad5eaf6, 0x59b9f8f1}, {0xcb7a04c8, 0xc8d2e345},
    {0xfb4ce0de, 0xe1633081},
};

const FPF_SKIAFONTMAP g_SkiaSansFontMap[] = {
    {0x58c5083, 0xd5b8d10f},  {0x14ee2d13, 0xd5b8d10f},
    {0x779ce19d, 0xd5b8d10f}, {0xcb7a04c8, 0xd5b8d10f},
    {0xfb4ce0de, 0xd5b8d10f},
};

uint32_t FPF_SkiaGetSubstFont(uint32_t dwHash,
                              const FPF_SKIAFONTMAP* skFontMap,
                              size_t length) {
  const FPF_SKIAFONTMAP* pEnd = skFontMap + length;
  const FPF_SKIAFONTMAP* pFontMap = std::lower_bound(
      skFontMap, pEnd, dwHash, [](const FPF_SKIAFONTMAP& item, uint32_t hash) {
        return item.dwFamily < hash;
      });
  if (pFontMap < pEnd && pFontMap->dwFamily == dwHash)
    return pFontMap->dwSubSt;
  return 0;
}

uint32_t FPF_GetHashCode_StringA(const char* pStr, int32_t iLength) {
  if (!pStr)
    return 0;
  if (iLength < 0)
    iLength = strlen(pStr);
  const char* pStrEnd = pStr + iLength;
  uint32_t uHashCode = 0;
  while (pStr < pStrEnd)
    uHashCode = 31 * uHashCode + tolower(*pStr++);
  return uHashCode;
}

enum FPF_SKIACHARSET {
  FPF_SKIACHARSET_Ansi = 1 << 0,
  FPF_SKIACHARSET_Default = 1 << 1,
  FPF_SKIACHARSET_Symbol = 1 << 2,
  FPF_SKIACHARSET_ShiftJIS = 1 << 3,
  FPF_SKIACHARSET_Korean = 1 << 4,
  FPF_SKIACHARSET_Johab = 1 << 5,
  FPF_SKIACHARSET_GB2312 = 1 << 6,
  FPF_SKIACHARSET_BIG5 = 1 << 7,
  FPF_SKIACHARSET_Greek = 1 << 8,
  FPF_SKIACHARSET_Turkish = 1 << 9,
  FPF_SKIACHARSET_Vietnamese = 1 << 10,
  FPF_SKIACHARSET_Hebrew = 1 << 11,
  FPF_SKIACHARSET_Arabic = 1 << 12,
  FPF_SKIACHARSET_Baltic = 1 << 13,
  FPF_SKIACHARSET_Cyrillic = 1 << 14,
  FPF_SKIACHARSET_Thai = 1 << 15,
  FPF_SKIACHARSET_EeasternEuropean = 1 << 16,
  FPF_SKIACHARSET_PC = 1 << 17,
  FPF_SKIACHARSET_OEM = 1 << 18,
};

uint32_t FPF_SkiaGetCharset(uint8_t uCharset) {
  switch (uCharset) {
    case FX_CHARSET_ANSI:
      return FPF_SKIACHARSET_Ansi;
    case FX_CHARSET_Default:
      return FPF_SKIACHARSET_Default;
    case FX_CHARSET_Symbol:
      return FPF_SKIACHARSET_Symbol;
    case FX_CHARSET_ShiftJIS:
      return FPF_SKIACHARSET_ShiftJIS;
    case FX_CHARSET_Hangul:
      return FPF_SKIACHARSET_Korean;
    case FX_CHARSET_ChineseSimplified:
      return FPF_SKIACHARSET_GB2312;
    case FX_CHARSET_ChineseTraditional:
      return FPF_SKIACHARSET_BIG5;
    case FX_CHARSET_MSWin_Greek:
      return FPF_SKIACHARSET_Greek;
    case FX_CHARSET_MSWin_Turkish:
      return FPF_SKIACHARSET_Turkish;
    case FX_CHARSET_MSWin_Hebrew:
      return FPF_SKIACHARSET_Hebrew;
    case FX_CHARSET_MSWin_Arabic:
      return FPF_SKIACHARSET_Arabic;
    case FX_CHARSET_MSWin_Baltic:
      return FPF_SKIACHARSET_Baltic;
    case FX_CHARSET_MSWin_Cyrillic:
      return FPF_SKIACHARSET_Cyrillic;
    case FX_CHARSET_Thai:
      return FPF_SKIACHARSET_Thai;
    case FX_CHARSET_MSWin_EasternEuropean:
      return FPF_SKIACHARSET_EeasternEuropean;
  }
  return FPF_SKIACHARSET_Default;
}

uint32_t FPF_SKIANormalizeFontName(ByteStringView bsfamily) {
  uint32_t dwHash = 0;
  int32_t iLength = bsfamily.GetLength();
  const char* pBuffer = bsfamily.unterminated_c_str();
  for (int32_t i = 0; i < iLength; i++) {
    char ch = pBuffer[i];
    if (ch == ' ' || ch == '-' || ch == ',')
      continue;
    dwHash = 31 * dwHash + tolower(ch);
  }
  return dwHash;
}

uint32_t FPF_SKIAGetFamilyHash(ByteStringView bsFamily,
                               uint32_t dwStyle,
                               uint8_t uCharset) {
  ByteString bsFont(bsFamily);
  if (FontStyleIsBold(dwStyle))
    bsFont += "Bold";
  if (FontStyleIsItalic(dwStyle))
    bsFont += "Italic";
  if (FontStyleIsSerif(dwStyle))
    bsFont += "Serif";
  bsFont += uCharset;
  return FPF_GetHashCode_StringA(bsFont.c_str(), bsFont.GetLength());
}

bool FPF_SkiaIsCJK(uint8_t uCharset) {
  return FX_CharSetIsCJK(uCharset);
}

bool FPF_SkiaMaybeSymbol(ByteStringView bsFacename) {
  ByteString bsName(bsFacename);
  bsName.MakeLower();
  return bsName.Contains("symbol");
}

bool FPF_SkiaMaybeArabic(ByteStringView bsFacename) {
  ByteString bsName(bsFacename);
  bsName.MakeLower();
  return bsName.Contains("arabic");
}

const uint32_t g_FPFSkiaFontCharsets[] = {
    FPF_SKIACHARSET_Ansi,
    FPF_SKIACHARSET_EeasternEuropean,
    FPF_SKIACHARSET_Cyrillic,
    FPF_SKIACHARSET_Greek,
    FPF_SKIACHARSET_Turkish,
    FPF_SKIACHARSET_Hebrew,
    FPF_SKIACHARSET_Arabic,
    FPF_SKIACHARSET_Baltic,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    FPF_SKIACHARSET_Thai,
    FPF_SKIACHARSET_ShiftJIS,
    FPF_SKIACHARSET_GB2312,
    FPF_SKIACHARSET_Korean,
    FPF_SKIACHARSET_BIG5,
    FPF_SKIACHARSET_Johab,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    FPF_SKIACHARSET_OEM,
    FPF_SKIACHARSET_Symbol,
};

uint32_t FPF_SkiaGetFaceCharset(TT_OS2* pOS2) {
  uint32_t dwCharset = 0;
  if (pOS2) {
    for (int32_t i = 0; i < 32; i++) {
      if (pOS2->ulCodePageRange1 & (1 << i))
        dwCharset |= g_FPFSkiaFontCharsets[i];
    }
  }
  dwCharset |= FPF_SKIACHARSET_Default;
  return dwCharset;
}

}  // namespace

CFPF_SkiaFontMgr::CFPF_SkiaFontMgr() = default;

CFPF_SkiaFontMgr::~CFPF_SkiaFontMgr() {
  m_FamilyFonts.clear();
  m_FontFaces.clear();
  if (m_FTLibrary)
    FXFT_Done_FreeType(m_FTLibrary);
}

bool CFPF_SkiaFontMgr::InitFTLibrary() {
  if (!m_FTLibrary)
    FXFT_Init_FreeType(&m_FTLibrary);
  return !!m_FTLibrary;
}

void CFPF_SkiaFontMgr::LoadSystemFonts() {
  if (m_bLoaded)
    return;
  ScanPath("/system/fonts");
  m_bLoaded = true;
}

CFPF_SkiaFont* CFPF_SkiaFontMgr::CreateFont(ByteStringView bsFamilyname,
                                            uint8_t uCharset,
                                            uint32_t dwStyle) {
  uint32_t dwHash = FPF_SKIAGetFamilyHash(bsFamilyname, dwStyle, uCharset);
  auto family_iter = m_FamilyFonts.find(dwHash);
  if (family_iter != m_FamilyFonts.end())
    return family_iter->second.get();

  uint32_t dwFaceName = FPF_SKIANormalizeFontName(bsFamilyname);
  uint32_t dwSubst = FPF_SkiaGetSubstFont(dwFaceName, g_SkiaFontmap,
                                          FX_ArraySize(g_SkiaFontmap));
  uint32_t dwSubstSans = FPF_SkiaGetSubstFont(dwFaceName, g_SkiaSansFontMap,
                                              FX_ArraySize(g_SkiaSansFontMap));
  bool bMaybeSymbol = FPF_SkiaMaybeSymbol(bsFamilyname);
  if (uCharset != FX_CHARSET_MSWin_Arabic &&
      FPF_SkiaMaybeArabic(bsFamilyname)) {
    uCharset = FX_CHARSET_MSWin_Arabic;
  } else if (uCharset == FX_CHARSET_ANSI) {
    uCharset = FX_CHARSET_Default;
  }
  int32_t nExpectVal = FPF_SKIAMATCHWEIGHT_NAME1 + FPF_SKIAMATCHWEIGHT_1 * 3 +
                       FPF_SKIAMATCHWEIGHT_2 * 2;
  const CFPF_SkiaPathFont* pBestFont = nullptr;
  int32_t nMax = -1;
  int32_t nGlyphNum = 0;
  for (auto face_iter = m_FontFaces.rbegin(); face_iter != m_FontFaces.rend();
       ++face_iter) {
    const CFPF_SkiaPathFont* pFont = face_iter->get();
    if (!(pFont->charsets() & FPF_SkiaGetCharset(uCharset)))
      continue;
    int32_t nFind = 0;
    uint32_t dwSysFontName = FPF_SKIANormalizeFontName(pFont->family());
    if (dwFaceName == dwSysFontName)
      nFind += FPF_SKIAMATCHWEIGHT_NAME1;
    bool bMatchedName = (nFind == FPF_SKIAMATCHWEIGHT_NAME1);
    if (FontStyleIsBold(dwStyle) == FontStyleIsBold(pFont->style()))
      nFind += FPF_SKIAMATCHWEIGHT_1;
    if (FontStyleIsItalic(dwStyle) == FontStyleIsItalic(pFont->style()))
      nFind += FPF_SKIAMATCHWEIGHT_1;
    if (FontStyleIsFixedPitch(dwStyle) ==
        FontStyleIsFixedPitch(pFont->style())) {
      nFind += FPF_SKIAMATCHWEIGHT_2;
    }
    if (FontStyleIsSerif(dwStyle) == FontStyleIsSerif(pFont->style()))
      nFind += FPF_SKIAMATCHWEIGHT_1;
    if (FontStyleIsScript(dwStyle) == FontStyleIsScript(pFont->style()))
      nFind += FPF_SKIAMATCHWEIGHT_2;
    if (dwSubst == dwSysFontName || dwSubstSans == dwSysFontName) {
      nFind += FPF_SKIAMATCHWEIGHT_NAME2;
      bMatchedName = true;
    }
    if (uCharset == FX_CHARSET_Default || bMaybeSymbol) {
      if (nFind > nMax && bMatchedName) {
        nMax = nFind;
        pBestFont = face_iter->get();
      }
    } else if (FPF_SkiaIsCJK(uCharset)) {
      if (bMatchedName || pFont->glyph_num() > nGlyphNum) {
        pBestFont = face_iter->get();
        nGlyphNum = pFont->glyph_num();
      }
    } else if (nFind > nMax) {
      nMax = nFind;
      pBestFont = face_iter->get();
    }
    if (nExpectVal <= nFind) {
      pBestFont = face_iter->get();
      break;
    }
  }
  if (!pBestFont)
    return nullptr;

  auto pFont =
      pdfium::MakeUnique<CFPF_SkiaFont>(this, pBestFont, dwStyle, uCharset);
  if (!pFont->IsValid())
    return nullptr;

  CFPF_SkiaFont* pRet = pFont.get();
  m_FamilyFonts[dwHash] = std::move(pFont);
  return pRet;
}

FXFT_Face CFPF_SkiaFontMgr::GetFontFace(ByteStringView bsFile,
                                        int32_t iFaceIndex) {
  if (bsFile.IsEmpty())
    return nullptr;
  if (iFaceIndex < 0)
    return nullptr;
  FXFT_Open_Args args;
  args.flags = FT_OPEN_PATHNAME;
  args.pathname = const_cast<FT_String*>(bsFile.unterminated_c_str());
  FXFT_Face face;
  if (FXFT_Open_Face(m_FTLibrary, &args, iFaceIndex, &face))
    return nullptr;
  FXFT_Set_Pixel_Sizes(face, 0, 64);
  return face;
}

void CFPF_SkiaFontMgr::ScanPath(const ByteString& path) {
  std::unique_ptr<FX_FileHandle, FxFolderHandleCloser> handle(
      FX_OpenFolder(path.c_str()));
  if (!handle)
    return;

  ByteString filename;
  bool bFolder = false;
  while (FX_GetNextFile(handle.get(), &filename, &bFolder)) {
    if (bFolder) {
      if (filename == "." || filename == "..")
        continue;
    } else {
      ByteString ext = filename.Right(4);
      ext.MakeLower();
      if (ext != ".ttf" && ext != ".ttc" && ext != ".otf")
        continue;
    }
    ByteString fullpath(path);
    fullpath += "/";
    fullpath += filename;
    if (bFolder)
      ScanPath(fullpath);
    else
      ScanFile(fullpath);
  }
}

void CFPF_SkiaFontMgr::ScanFile(const ByteString& file) {
  FXFT_Face face = GetFontFace(file.AsStringView(), 0);
  if (!face)
    return;

  m_FontFaces.push_back(ReportFace(face, file));
  FXFT_Done_Face(face);
}

std::unique_ptr<CFPF_SkiaPathFont> CFPF_SkiaFontMgr::ReportFace(
    FXFT_Face face,
    const ByteString& file) {
  uint32_t dwStyle = 0;
  if (FXFT_Is_Face_Bold(face))
    dwStyle |= FXFONT_BOLD;
  if (FXFT_Is_Face_Italic(face))
    dwStyle |= FXFONT_ITALIC;
  if (FT_IS_FIXED_WIDTH(face))
    dwStyle |= FXFONT_FIXED_PITCH;
  TT_OS2* pOS2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(face, ft_sfnt_os2));
  if (pOS2) {
    if (pOS2->ulCodePageRange1 & (1 << 31))
      dwStyle |= FXFONT_SYMBOLIC;
    if (pOS2->panose[0] == 2) {
      uint8_t uSerif = pOS2->panose[1];
      if ((uSerif > 1 && uSerif < 10) || uSerif > 13)
        dwStyle |= FXFONT_SERIF;
    }
  }
  if (pOS2 && (pOS2->ulCodePageRange1 & (1 << 31)))
    dwStyle |= FXFONT_SYMBOLIC;

  return pdfium::MakeUnique<CFPF_SkiaPathFont>(
      file, FXFT_Get_Face_Family_Name(face), dwStyle, face->face_index,
      FPF_SkiaGetFaceCharset(pOS2), face->num_glyphs);
}
