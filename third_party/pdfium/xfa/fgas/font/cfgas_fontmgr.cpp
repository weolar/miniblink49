// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/font/cfgas_fontmgr.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "core/fxcrt/cfx_memorystream.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_font.h"
#include "core/fxge/cfx_fontmapper.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/fx_font.h"
#include "core/fxge/systemfontinfo_iface.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fgas/font/fgas_fontutils.h"

#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
#include "xfa/fgas/font/cfx_fontsourceenum_file.h"
#endif

namespace {

bool VerifyUnicode(const RetainPtr<CFGAS_GEFont>& pFont, wchar_t wcUnicode) {
  FXFT_Face pFace = pFont->GetDevFont()->GetFace();
  FXFT_CharMap charmap = FXFT_Get_Face_Charmap(pFace);
  if (FXFT_Select_Charmap(pFace, FXFT_ENCODING_UNICODE) != 0)
    return false;

  if (FXFT_Get_Char_Index(pFace, wcUnicode) == 0) {
    FXFT_Set_Charmap(pFace, charmap);
    return false;
  }
  return true;
}

}  // namespace

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

namespace {

int32_t GetSimilarityScore(FX_FONTDESCRIPTOR const* pFont,
                           uint32_t dwFontStyles) {
  int32_t iValue = 0;
  if (FontStyleIsSymbolic(dwFontStyles) ==
      FontStyleIsSymbolic(pFont->dwFontStyles)) {
    iValue += 64;
  }
  if (FontStyleIsFixedPitch(dwFontStyles) ==
      FontStyleIsFixedPitch(pFont->dwFontStyles)) {
    iValue += 32;
  }
  if (FontStyleIsSerif(dwFontStyles) == FontStyleIsSerif(pFont->dwFontStyles))
    iValue += 16;
  if (FontStyleIsScript(dwFontStyles) == FontStyleIsScript(pFont->dwFontStyles))
    iValue += 8;
  return iValue;
}

const FX_FONTDESCRIPTOR* MatchDefaultFont(
    FX_FONTMATCHPARAMS* pParams,
    const std::deque<FX_FONTDESCRIPTOR>& fonts) {
  const FX_FONTDESCRIPTOR* pBestFont = nullptr;
  int32_t iBestSimilar = 0;
  for (const auto& font : fonts) {
    if (FontStyleIsBold(font.dwFontStyles) &&
        FontStyleIsItalic(font.dwFontStyles)) {
      continue;
    }

    if (pParams->pwsFamily) {
      if (FXSYS_wcsicmp(pParams->pwsFamily, font.wsFontFace))
        continue;
      if (font.uCharSet == FX_CHARSET_Symbol)
        return &font;
    }
    if (font.uCharSet == FX_CHARSET_Symbol)
      continue;
    if (pParams->wCodePage != 0xFFFF) {
      if (FX_GetCodePageFromCharset(font.uCharSet) != pParams->wCodePage)
        continue;
    } else {
      if (pParams->dwUSB < 128) {
        uint32_t dwByte = pParams->dwUSB / 32;
        uint32_t dwUSB = 1 << (pParams->dwUSB % 32);
        if ((font.FontSignature.fsUsb[dwByte] & dwUSB) == 0)
          continue;
      }
    }
    if (pParams->matchParagraphStyle) {
      if ((font.dwFontStyles & 0x0F) == (pParams->dwFontStyles & 0x0F))
        return &font;
      continue;
    }
    if (pParams->pwsFamily) {
      if (FXSYS_wcsicmp(pParams->pwsFamily, font.wsFontFace) == 0)
        return &font;
    }
    int32_t iSimilarValue = GetSimilarityScore(&font, pParams->dwFontStyles);
    if (iBestSimilar < iSimilarValue) {
      iBestSimilar = iSimilarValue;
      pBestFont = &font;
    }
  }
  return iBestSimilar < 1 ? nullptr : pBestFont;
}

uint32_t GetGdiFontStyles(const LOGFONTW& lf) {
  uint32_t dwStyles = 0;
  if ((lf.lfPitchAndFamily & 0x03) == FIXED_PITCH)
    dwStyles |= FXFONT_FIXED_PITCH;
  uint8_t nFamilies = lf.lfPitchAndFamily & 0xF0;
  if (nFamilies == FF_ROMAN)
    dwStyles |= FXFONT_SERIF;
  if (nFamilies == FF_SCRIPT)
    dwStyles |= FXFONT_SCRIPT;
  if (lf.lfCharSet == SYMBOL_CHARSET)
    dwStyles |= FXFONT_SYMBOLIC;
  return dwStyles;
}

int32_t CALLBACK GdiFontEnumProc(ENUMLOGFONTEX* lpelfe,
                                 NEWTEXTMETRICEX* lpntme,
                                 DWORD dwFontType,
                                 LPARAM lParam) {
  if (dwFontType != TRUETYPE_FONTTYPE)
    return 1;
  const LOGFONTW& lf = ((LPENUMLOGFONTEXW)lpelfe)->elfLogFont;
  if (lf.lfFaceName[0] == L'@')
    return 1;
  FX_FONTDESCRIPTOR font;
  memset(&font, 0, sizeof(FX_FONTDESCRIPTOR));
  font.uCharSet = lf.lfCharSet;
  font.dwFontStyles = GetGdiFontStyles(lf);
  FXSYS_wcsncpy(font.wsFontFace, (const wchar_t*)lf.lfFaceName, 31);
  font.wsFontFace[31] = 0;
  memcpy(&font.FontSignature, &lpntme->ntmFontSig, sizeof(lpntme->ntmFontSig));
  reinterpret_cast<std::deque<FX_FONTDESCRIPTOR>*>(lParam)->push_back(font);
  return 1;
}

std::deque<FX_FONTDESCRIPTOR> EnumGdiFonts(const wchar_t* pwsFaceName,
                                           wchar_t wUnicode) {
  std::deque<FX_FONTDESCRIPTOR> fonts;
  LOGFONTW lfFind;
  memset(&lfFind, 0, sizeof(lfFind));
  lfFind.lfCharSet = DEFAULT_CHARSET;
  if (pwsFaceName) {
    FXSYS_wcsncpy(lfFind.lfFaceName, pwsFaceName, 31);
    lfFind.lfFaceName[31] = 0;
  }
  HDC hDC = ::GetDC(nullptr);
  EnumFontFamiliesExW(hDC, (LPLOGFONTW)&lfFind, (FONTENUMPROCW)GdiFontEnumProc,
                      (LPARAM)&fonts, 0);
  ::ReleaseDC(nullptr, hDC);
  return fonts;
}

}  // namespace

CFGAS_FontMgr::CFGAS_FontMgr() : m_FontFaces(EnumGdiFonts(nullptr, 0xFEFF)) {}

CFGAS_FontMgr::~CFGAS_FontMgr() = default;

bool CFGAS_FontMgr::EnumFonts() {
  return true;
}

RetainPtr<CFGAS_GEFont> CFGAS_FontMgr::GetFontByUnicodeImpl(
    wchar_t wUnicode,
    uint32_t dwFontStyles,
    const wchar_t* pszFontFamily,
    uint32_t dwHash,
    uint16_t wCodePage,
    uint16_t wBitField) {
  const FX_FONTDESCRIPTOR* pFD = FindFont(pszFontFamily, dwFontStyles, false,
                                          wCodePage, wBitField, wUnicode);
  if (!pFD && pszFontFamily) {
    pFD =
        FindFont(nullptr, dwFontStyles, false, wCodePage, wBitField, wUnicode);
  }
  if (!pFD)
    return nullptr;

  uint16_t newCodePage = FX_GetCodePageFromCharset(pFD->uCharSet);
  const wchar_t* pFontFace = pFD->wsFontFace;
  RetainPtr<CFGAS_GEFont> pFont =
      CFGAS_GEFont::LoadFont(pFontFace, dwFontStyles, newCodePage, this);
  if (!pFont)
    return nullptr;

  pFont->SetLogicalFontStyle(dwFontStyles);
  if (!VerifyUnicode(pFont, wUnicode)) {
    m_FailedUnicodesSet.insert(wUnicode);
    return nullptr;
  }

  m_Hash2Fonts[dwHash].push_back(pFont);
  return pFont;
}

const FX_FONTDESCRIPTOR* CFGAS_FontMgr::FindFont(const wchar_t* pszFontFamily,
                                                 uint32_t dwFontStyles,
                                                 bool matchParagraphStyle,
                                                 uint16_t wCodePage,
                                                 uint32_t dwUSB,
                                                 wchar_t wUnicode) {
  FX_FONTMATCHPARAMS params;
  memset(&params, 0, sizeof(params));
  params.dwUSB = dwUSB;
  params.wUnicode = wUnicode;
  params.wCodePage = wCodePage;
  params.pwsFamily = pszFontFamily;
  params.dwFontStyles = dwFontStyles;
  params.matchParagraphStyle = matchParagraphStyle;

  const FX_FONTDESCRIPTOR* pDesc = MatchDefaultFont(&params, m_FontFaces);
  if (pDesc)
    return pDesc;

  if (!pszFontFamily)
    return nullptr;

  // Use a named object to store the returned value of EnumGdiFonts() instead
  // of using a temporary object. This can prevent use-after-free issues since
  // pDesc may point to one of std::deque object's elements.
  std::deque<FX_FONTDESCRIPTOR> namedFonts =
      EnumGdiFonts(pszFontFamily, wUnicode);
  params.pwsFamily = nullptr;
  pDesc = MatchDefaultFont(&params, namedFonts);
  if (!pDesc)
    return nullptr;

  auto it = std::find(m_FontFaces.rbegin(), m_FontFaces.rend(), *pDesc);
  if (it != m_FontFaces.rend())
    return &*it;

  m_FontFaces.push_back(*pDesc);
  return &m_FontFaces.back();
}

#else  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

namespace {

const uint16_t g_CodePages[] = {FX_CODEPAGE_MSWin_WesternEuropean,
                                FX_CODEPAGE_MSWin_EasternEuropean,
                                FX_CODEPAGE_MSWin_Cyrillic,
                                FX_CODEPAGE_MSWin_Greek,
                                FX_CODEPAGE_MSWin_Turkish,
                                FX_CODEPAGE_MSWin_Hebrew,
                                FX_CODEPAGE_MSWin_Arabic,
                                FX_CODEPAGE_MSWin_Baltic,
                                FX_CODEPAGE_MSWin_Vietnamese,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_MSDOS_Thai,
                                FX_CODEPAGE_ShiftJIS,
                                FX_CODEPAGE_ChineseSimplified,
                                FX_CODEPAGE_Hangul,
                                FX_CODEPAGE_ChineseTraditional,
                                FX_CODEPAGE_Johab,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_DefANSI,
                                FX_CODEPAGE_MSDOS_Greek2,
                                FX_CODEPAGE_MSDOS_Russian,
                                FX_CODEPAGE_MSDOS_Norwegian,
                                FX_CODEPAGE_MSDOS_Arabic,
                                FX_CODEPAGE_MSDOS_FrenchCanadian,
                                FX_CODEPAGE_MSDOS_Hebrew,
                                FX_CODEPAGE_MSDOS_Icelandic,
                                FX_CODEPAGE_MSDOS_Portuguese,
                                FX_CODEPAGE_MSDOS_Turkish,
                                FX_CODEPAGE_MSDOS_Cyrillic,
                                FX_CODEPAGE_MSDOS_EasternEuropean,
                                FX_CODEPAGE_MSDOS_Baltic,
                                FX_CODEPAGE_MSDOS_Greek1,
                                FX_CODEPAGE_Arabic_ASMO708,
                                FX_CODEPAGE_MSDOS_WesternEuropean,
                                FX_CODEPAGE_MSDOS_US};

uint16_t FX_GetCodePageBit(uint16_t wCodePage) {
  for (size_t i = 0; i < FX_ArraySize(g_CodePages); ++i) {
    if (g_CodePages[i] == wCodePage)
      return static_cast<uint16_t>(i);
  }
  return static_cast<uint16_t>(-1);
}

uint16_t FX_GetUnicodeBit(wchar_t wcUnicode) {
  const FGAS_FONTUSB* x = FGAS_GetUnicodeBitField(wcUnicode);
  return x ? x->wBitField : 999;
}

inline uint8_t GetUInt8(const uint8_t* p) {
  return p[0];
}

inline uint16_t GetUInt16(const uint8_t* p) {
  return static_cast<uint16_t>(p[0] << 8 | p[1]);
}

extern "C" {

unsigned long ftStreamRead(FXFT_Stream stream,
                           unsigned long offset,
                           unsigned char* buffer,
                           unsigned long count) {
  if (count == 0)
    return 0;

  IFX_SeekableReadStream* pFile =
      static_cast<IFX_SeekableReadStream*>(stream->descriptor.pointer);
  if (!pFile->ReadBlockAtOffset(buffer, offset, count))
    return 0;

  return count;
}

void ftStreamClose(FXFT_Stream stream) {}

};  // extern "C"

// TODO(thestig): Pass in |name_table| as a std::vector?
std::vector<WideString> GetNames(const uint8_t* name_table) {
  std::vector<WideString> results;
  if (!name_table)
    return results;

  const uint8_t* lpTable = name_table;
  WideString wsFamily;
  const uint8_t* sp = lpTable + 2;
  const uint8_t* lpNameRecord = lpTable + 6;
  uint16_t nNameCount = GetUInt16(sp);
  const uint8_t* lpStr = lpTable + GetUInt16(sp + 2);
  for (uint16_t j = 0; j < nNameCount; j++) {
    uint16_t nNameID = GetUInt16(lpNameRecord + j * 12 + 6);
    if (nNameID != 1)
      continue;

    uint16_t nPlatformID = GetUInt16(lpNameRecord + j * 12 + 0);
    uint16_t nNameLength = GetUInt16(lpNameRecord + j * 12 + 8);
    uint16_t nNameOffset = GetUInt16(lpNameRecord + j * 12 + 10);
    wsFamily.clear();
    if (nPlatformID != 1) {
      for (uint16_t k = 0; k < nNameLength / 2; k++) {
        wchar_t wcTemp = GetUInt16(lpStr + nNameOffset + k * 2);
        wsFamily += wcTemp;
      }
      results.push_back(wsFamily);
      continue;
    }
    for (uint16_t k = 0; k < nNameLength; k++) {
      wchar_t wcTemp = GetUInt8(lpStr + nNameOffset + k);
      wsFamily += wcTemp;
    }
    results.push_back(wsFamily);
  }
  return results;
}

void GetUSBCSB(FXFT_Face pFace, uint32_t* USB, uint32_t* CSB) {
  TT_OS2* pOS2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(pFace, ft_sfnt_os2));
  if (!pOS2) {
    USB[0] = 0;
    USB[1] = 0;
    USB[2] = 0;
    USB[3] = 0;
    CSB[0] = 0;
    CSB[1] = 0;
    return;
  }
  USB[0] = pOS2->ulUnicodeRange1;
  USB[1] = pOS2->ulUnicodeRange2;
  USB[2] = pOS2->ulUnicodeRange3;
  USB[3] = pOS2->ulUnicodeRange4;
  CSB[0] = pOS2->ulCodePageRange1;
  CSB[1] = pOS2->ulCodePageRange2;
}

uint32_t GetFlags(FXFT_Face pFace) {
  uint32_t flags = 0;
  if (FXFT_Is_Face_Bold(pFace))
    flags |= FXFONT_BOLD;
  if (FXFT_Is_Face_Italic(pFace))
    flags |= FXFONT_ITALIC;
  if (FT_IS_FIXED_WIDTH(pFace))
    flags |= FXFONT_FIXED_PITCH;

  TT_OS2* pOS2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(pFace, ft_sfnt_os2));
  if (!pOS2)
    return flags;

  if (pOS2->ulCodePageRange1 & (1 << 31))
    flags |= FXFONT_SYMBOLIC;
  if (pOS2->panose[0] == 2) {
    uint8_t uSerif = pOS2->panose[1];
    if ((uSerif > 1 && uSerif < 10) || uSerif > 13)
      flags |= FXFONT_SERIF;
  }
  return flags;
}

RetainPtr<IFX_SeekableReadStream> CreateFontStream(
    CFX_FontMapper* pFontMapper,
    SystemFontInfoIface* pSystemFontInfo,
    uint32_t index) {
  void* hFont = pSystemFontInfo->MapFont(
      0, 0, FX_CHARSET_Default, 0, pFontMapper->GetFaceName(index).c_str());
  if (!hFont)
    return nullptr;

  uint32_t dwFileSize = pSystemFontInfo->GetFontData(hFont, 0, nullptr, 0);
  if (dwFileSize == 0)
    return nullptr;

  std::unique_ptr<uint8_t, FxFreeDeleter> pBuffer(
      FX_Alloc(uint8_t, dwFileSize + 1));
  dwFileSize =
      pSystemFontInfo->GetFontData(hFont, 0, pBuffer.get(), dwFileSize);
  return pdfium::MakeRetain<CFX_MemoryStream>(std::move(pBuffer), dwFileSize);
}

RetainPtr<IFX_SeekableReadStream> CreateFontStream(
    const ByteString& bsFaceName) {
  CFX_FontMgr* pFontMgr = CFX_GEModule::Get()->GetFontMgr();
  CFX_FontMapper* pFontMapper = pFontMgr->GetBuiltinMapper();
  SystemFontInfoIface* pSystemFontInfo = pFontMapper->GetSystemFontInfo();
  if (!pSystemFontInfo)
    return nullptr;

  pSystemFontInfo->EnumFontList(pFontMapper);
  for (int32_t i = 0; i < pFontMapper->GetFaceSize(); ++i) {
    if (pFontMapper->GetFaceName(i) == bsFaceName)
      return CreateFontStream(pFontMapper, pSystemFontInfo, i);
  }
  return nullptr;
}

FXFT_Face LoadFace(const RetainPtr<IFX_SeekableReadStream>& pFontStream,
                   int32_t iFaceIndex) {
  if (!pFontStream)
    return nullptr;

  CFX_FontMgr* pFontMgr = CFX_GEModule::Get()->GetFontMgr();
  pFontMgr->InitFTLibrary();

  FXFT_Library library = pFontMgr->GetFTLibrary();
  if (!library)
    return nullptr;

  // TODO(palmer): This memory will be freed with |ft_free| (which is |free|).
  // Ultimately, we want to change this to:
  //   FXFT_Stream ftStream = FX_Alloc(FXFT_StreamRec, 1);
  // https://bugs.chromium.org/p/pdfium/issues/detail?id=690
  FXFT_Stream ftStream =
      static_cast<FXFT_Stream>(ft_scalloc(sizeof(FXFT_StreamRec), 1));
  memset(ftStream, 0, sizeof(FXFT_StreamRec));
  ftStream->base = nullptr;
  ftStream->descriptor.pointer = static_cast<void*>(pFontStream.Get());
  ftStream->pos = 0;
  ftStream->size = static_cast<unsigned long>(pFontStream->GetSize());
  ftStream->read = ftStreamRead;
  ftStream->close = ftStreamClose;

  FXFT_Open_Args ftArgs;
  memset(&ftArgs, 0, sizeof(FXFT_Open_Args));
  ftArgs.flags |= FT_OPEN_STREAM;
  ftArgs.stream = ftStream;

  FXFT_Face pFace = nullptr;
  if (FXFT_Open_Face(library, &ftArgs, iFaceIndex, &pFace)) {
    ft_sfree(ftStream);
    return nullptr;
  }

  FXFT_Set_Pixel_Sizes(pFace, 0, 64);
  return pFace;
}

bool VerifyUnicodeForFontDescriptor(CFX_FontDescriptor* pDesc,
                                    wchar_t wcUnicode) {
  RetainPtr<IFX_SeekableReadStream> pFileRead =
      CreateFontStream(pDesc->m_wsFaceName.ToUTF8());
  if (!pFileRead)
    return false;

  FXFT_Face pFace = LoadFace(pFileRead, pDesc->m_nFaceIndex);
  if (!pFace)
    return false;

  FT_Error retCharmap = FXFT_Select_Charmap(pFace, FXFT_ENCODING_UNICODE);
  FT_Error retIndex = FXFT_Get_Char_Index(pFace, wcUnicode);

  if (FXFT_Get_Face_External_Stream(pFace))
    FXFT_Clear_Face_External_Stream(pFace);

  FXFT_Done_Face(pFace);
  return !retCharmap && retIndex;
}

bool IsPartName(const WideString& name1, const WideString& name2) {
  return name1.Contains(name2.AsStringView());
}

int32_t CalcPenalty(CFX_FontDescriptor* pInstalled,
                    uint16_t wCodePage,
                    uint32_t dwFontStyles,
                    const WideString& FontName,
                    wchar_t wcUnicode) {
  int32_t nPenalty = 30000;
  if (FontName.GetLength() != 0) {
    if (FontName != pInstalled->m_wsFaceName) {
      size_t i;
      for (i = 0; i < pInstalled->m_wsFamilyNames.size(); ++i) {
        if (pInstalled->m_wsFamilyNames[i] == FontName)
          break;
      }
      if (i == pInstalled->m_wsFamilyNames.size())
        nPenalty += 0xFFFF;
      else
        nPenalty -= 28000;
    } else {
      nPenalty -= 30000;
    }
    if (nPenalty == 30000 && !IsPartName(pInstalled->m_wsFaceName, FontName)) {
      size_t i;
      for (i = 0; i < pInstalled->m_wsFamilyNames.size(); i++) {
        if (IsPartName(pInstalled->m_wsFamilyNames[i], FontName))
          break;
      }
      if (i == pInstalled->m_wsFamilyNames.size())
        nPenalty += 0xFFFF;
      else
        nPenalty -= 26000;
    } else {
      nPenalty -= 27000;
    }
  }
  uint32_t dwStyleMask = pInstalled->m_dwFontStyles ^ dwFontStyles;
  if (FontStyleIsBold(dwStyleMask))
    nPenalty += 4500;
  if (FontStyleIsFixedPitch(dwStyleMask))
    nPenalty += 10000;
  if (FontStyleIsItalic(dwStyleMask))
    nPenalty += 10000;
  if (FontStyleIsSerif(dwStyleMask))
    nPenalty += 500;
  if (FontStyleIsSymbolic(dwStyleMask))
    nPenalty += 0xFFFF;
  if (nPenalty >= 0xFFFF)
    return 0xFFFF;

  uint16_t wBit = (wCodePage == FX_CODEPAGE_DefANSI || wCodePage == 0xFFFF)
                      ? static_cast<uint16_t>(-1)
                      : FX_GetCodePageBit(wCodePage);
  if (wBit != static_cast<uint16_t>(-1)) {
    ASSERT(wBit < 64);
    if ((pInstalled->m_dwCsb[wBit / 32] & (1 << (wBit % 32))) == 0)
      nPenalty += 0xFFFF;
    else
      nPenalty -= 60000;
  }
  wBit = (wcUnicode == 0 || wcUnicode == 0xFFFE) ? static_cast<uint16_t>(999)
                                                 : FX_GetUnicodeBit(wcUnicode);
  if (wBit != static_cast<uint16_t>(999)) {
    ASSERT(wBit < 128);
    if ((pInstalled->m_dwUsb[wBit / 32] & (1 << (wBit % 32))) == 0)
      nPenalty += 0xFFFF;
    else
      nPenalty -= 60000;
  }
  return nPenalty;
}

}  // namespace

CFX_FontDescriptor::CFX_FontDescriptor()
    : m_nFaceIndex(0), m_dwFontStyles(0), m_dwUsb(), m_dwCsb() {}

CFX_FontDescriptor::~CFX_FontDescriptor() {}

CFGAS_FontMgr::CFGAS_FontMgr()
    : m_pFontSource(pdfium::MakeUnique<CFX_FontSourceEnum_File>()) {}

CFGAS_FontMgr::~CFGAS_FontMgr() {}

bool CFGAS_FontMgr::EnumFontsFromFontMapper() {
  CFX_FontMapper* pFontMapper =
      CFX_GEModule::Get()->GetFontMgr()->GetBuiltinMapper();
  pFontMapper->LoadInstalledFonts();

  SystemFontInfoIface* pSystemFontInfo = pFontMapper->GetSystemFontInfo();
  if (!pSystemFontInfo)
    return false;

  pSystemFontInfo->EnumFontList(pFontMapper);
  for (int32_t i = 0; i < pFontMapper->GetFaceSize(); ++i) {
    RetainPtr<IFX_SeekableReadStream> pFontStream =
        CreateFontStream(pFontMapper, pSystemFontInfo, i);
    if (!pFontStream)
      continue;

    WideString wsFaceName =
        WideString::FromDefANSI(pFontMapper->GetFaceName(i).AsStringView());
    RegisterFaces(pFontStream, &wsFaceName);
  }

  return !m_InstalledFonts.empty();
}

bool CFGAS_FontMgr::EnumFontsFromFiles() {
  CFX_GEModule::Get()->GetFontMgr()->InitFTLibrary();
  m_pFontSource->GetNext();
  while (m_pFontSource->HasNext()) {
    RetainPtr<IFX_SeekableStream> stream = m_pFontSource->GetStream();
    if (stream)
      RegisterFaces(stream, nullptr);
    m_pFontSource->GetNext();
  }
  return !m_InstalledFonts.empty();
}

bool CFGAS_FontMgr::EnumFonts() {
  return EnumFontsFromFontMapper() || EnumFontsFromFiles();
}

RetainPtr<CFGAS_GEFont> CFGAS_FontMgr::GetFontByUnicodeImpl(
    wchar_t wUnicode,
    uint32_t dwFontStyles,
    const wchar_t* pszFontFamily,
    uint32_t dwHash,
    uint16_t wCodePage,
    uint16_t /* wBitField */) {
  std::vector<CFX_FontDescriptorInfo>* sortedFontInfos =
      m_Hash2CandidateList[dwHash].get();
  if (!sortedFontInfos) {
    auto pNewFonts = pdfium::MakeUnique<std::vector<CFX_FontDescriptorInfo>>();
    sortedFontInfos = pNewFonts.get();
    MatchFonts(sortedFontInfos, wCodePage, dwFontStyles,
               WideString(pszFontFamily), wUnicode);
    m_Hash2CandidateList[dwHash] = std::move(pNewFonts);
  }
  for (const auto& info : *sortedFontInfos) {
    CFX_FontDescriptor* pDesc = info.pFont;
    if (!VerifyUnicodeForFontDescriptor(pDesc, wUnicode))
      continue;
    RetainPtr<CFGAS_GEFont> pFont =
        LoadFontInternal(pDesc->m_wsFaceName, pDesc->m_nFaceIndex);
    if (!pFont)
      continue;
    pFont->SetLogicalFontStyle(dwFontStyles);
    m_Hash2Fonts[dwHash].push_back(pFont);
    return pFont;
  }
  if (!pszFontFamily)
    m_FailedUnicodesSet.insert(wUnicode);
  return nullptr;
}

RetainPtr<CFGAS_GEFont> CFGAS_FontMgr::LoadFontInternal(
    const WideString& wsFaceName,
    int32_t iFaceIndex) {
  CFX_FontMgr* pFontMgr = CFX_GEModule::Get()->GetFontMgr();
  CFX_FontMapper* pFontMapper = pFontMgr->GetBuiltinMapper();
  SystemFontInfoIface* pSystemFontInfo = pFontMapper->GetSystemFontInfo();
  if (!pSystemFontInfo)
    return nullptr;

  RetainPtr<IFX_SeekableReadStream> pFontStream =
      CreateFontStream(wsFaceName.ToUTF8());
  if (!pFontStream)
    return nullptr;

  auto pInternalFont = pdfium::MakeUnique<CFX_Font>();
  if (!pInternalFont->LoadFile(pFontStream, iFaceIndex))
    return nullptr;

  RetainPtr<CFGAS_GEFont> pFont =
      CFGAS_GEFont::LoadFont(std::move(pInternalFont), this);
  if (!pFont)
    return nullptr;

  m_IFXFont2FileRead[pFont] = pFontStream;
  return pFont;
}

void CFGAS_FontMgr::MatchFonts(
    std::vector<CFX_FontDescriptorInfo>* pMatchedFonts,
    uint16_t wCodePage,
    uint32_t dwFontStyles,
    const WideString& FontName,
    wchar_t wcUnicode) {
  pMatchedFonts->clear();
  for (const auto& pFont : m_InstalledFonts) {
    int32_t nPenalty =
        CalcPenalty(pFont.get(), wCodePage, dwFontStyles, FontName, wcUnicode);
    if (nPenalty >= 0xffff)
      continue;
    pMatchedFonts->push_back({pFont.get(), nPenalty});
    if (pMatchedFonts->size() == 0xffff)
      break;
  }
  std::sort(pMatchedFonts->begin(), pMatchedFonts->end());
}

void CFGAS_FontMgr::RegisterFace(FXFT_Face pFace, const WideString* pFaceName) {
  if ((pFace->face_flags & FT_FACE_FLAG_SCALABLE) == 0)
    return;

  auto pFont = pdfium::MakeUnique<CFX_FontDescriptor>();
  pFont->m_dwFontStyles |= GetFlags(pFace);

  GetUSBCSB(pFace, pFont->m_dwUsb, pFont->m_dwCsb);

  FT_ULong dwTag;
  FT_ENC_TAG(dwTag, 'n', 'a', 'm', 'e');

  std::vector<uint8_t> table;
  unsigned long nLength = 0;
  unsigned int error = FXFT_Load_Sfnt_Table(pFace, dwTag, 0, nullptr, &nLength);
  if (error == 0 && nLength != 0) {
    table.resize(nLength);
    if (FXFT_Load_Sfnt_Table(pFace, dwTag, 0, table.data(), nullptr))
      table.clear();
  }
  pFont->m_wsFamilyNames = GetNames(table.empty() ? nullptr : table.data());
  pFont->m_wsFamilyNames.push_back(WideString::FromUTF8(pFace->family_name));
  pFont->m_wsFaceName =
      pFaceName ? *pFaceName
                : WideString::FromDefANSI(FXFT_Get_Postscript_Name(pFace));
  pFont->m_nFaceIndex = pFace->face_index;
  m_InstalledFonts.push_back(std::move(pFont));
}

void CFGAS_FontMgr::RegisterFaces(
    const RetainPtr<IFX_SeekableReadStream>& pFontStream,
    const WideString* pFaceName) {
  int32_t index = 0;
  int32_t num_faces = 0;
  do {
    FXFT_Face pFace = LoadFace(pFontStream, index++);
    if (!pFace)
      continue;
    // All faces keep number of faces. It can be retrieved from any one face.
    if (num_faces == 0)
      num_faces = pFace->num_faces;
    RegisterFace(pFace, pFaceName);
    if (FXFT_Get_Face_External_Stream(pFace))
      FXFT_Clear_Face_External_Stream(pFace);
    FXFT_Done_Face(pFace);
  } while (index < num_faces);
}

#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

RetainPtr<CFGAS_GEFont> CFGAS_FontMgr::GetFontByCodePage(
    uint16_t wCodePage,
    uint32_t dwFontStyles,
    const wchar_t* pszFontFamily) {
  ByteString bsHash = ByteString::Format("%d, %d", wCodePage, dwFontStyles);
  bsHash += FX_UTF8Encode(WideStringView(pszFontFamily));
  uint32_t dwHash = FX_HashCode_GetA(bsHash.AsStringView(), false);
  auto* pFontVector = &m_Hash2Fonts[dwHash];
  if (!pFontVector->empty()) {
    for (auto iter = pFontVector->begin(); iter != pFontVector->end(); ++iter) {
      if (*iter != nullptr)
        return *iter;
    }
    return nullptr;
  }

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  const FX_FONTDESCRIPTOR* pFD =
      FindFont(pszFontFamily, dwFontStyles, true, wCodePage, 999, 0);
  if (!pFD)
    pFD = FindFont(nullptr, dwFontStyles, true, wCodePage, 999, 0);
  if (!pFD)
    pFD = FindFont(nullptr, dwFontStyles, false, wCodePage, 999, 0);
  if (!pFD)
    return nullptr;

  RetainPtr<CFGAS_GEFont> pFont =
      CFGAS_GEFont::LoadFont(pFD->wsFontFace, dwFontStyles, wCodePage, this);
#else   // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  std::vector<CFX_FontDescriptorInfo>* sortedFontInfos =
      m_Hash2CandidateList[dwHash].get();
  if (!sortedFontInfos) {
    auto pNewFonts = pdfium::MakeUnique<std::vector<CFX_FontDescriptorInfo>>();
    sortedFontInfos = pNewFonts.get();
    MatchFonts(sortedFontInfos, wCodePage, dwFontStyles,
               WideString(pszFontFamily), 0);
    m_Hash2CandidateList[dwHash] = std::move(pNewFonts);
  }
  if (sortedFontInfos->empty())
    return nullptr;

  CFX_FontDescriptor* pDesc = (*sortedFontInfos)[0].pFont;
  RetainPtr<CFGAS_GEFont> pFont =
      LoadFontInternal(pDesc->m_wsFaceName, pDesc->m_nFaceIndex);
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

  if (!pFont)
    return nullptr;

  pFont->SetLogicalFontStyle(dwFontStyles);
  pFontVector->push_back(pFont);
  return pFont;
}

RetainPtr<CFGAS_GEFont> CFGAS_FontMgr::GetFontByUnicode(
    wchar_t wUnicode,
    uint32_t dwFontStyles,
    const wchar_t* pszFontFamily) {
  if (pdfium::ContainsKey(m_FailedUnicodesSet, wUnicode))
    return nullptr;

  const FGAS_FONTUSB* x = FGAS_GetUnicodeBitField(wUnicode);
  uint16_t wCodePage = x ? x->wCodePage : 0xFFFF;
  uint16_t wBitField = x ? x->wBitField : 0x03E7;
  ByteString bsHash;
  if (wCodePage == 0xFFFF) {
    bsHash =
        ByteString::Format("%d, %d, %d", wCodePage, wBitField, dwFontStyles);
  } else {
    bsHash = ByteString::Format("%d, %d", wCodePage, dwFontStyles);
  }
  bsHash += FX_UTF8Encode(WideStringView(pszFontFamily));
  uint32_t dwHash = FX_HashCode_GetA(bsHash.AsStringView(), false);
  std::vector<RetainPtr<CFGAS_GEFont>>& fonts = m_Hash2Fonts[dwHash];
  for (auto& pFont : fonts) {
    if (VerifyUnicode(pFont, wUnicode))
      return pFont;
  }

  return GetFontByUnicodeImpl(wUnicode, dwFontStyles, pszFontFamily, dwHash,
                              wCodePage, wBitField);
}

RetainPtr<CFGAS_GEFont> CFGAS_FontMgr::LoadFont(const wchar_t* pszFontFamily,
                                                uint32_t dwFontStyles,
                                                uint16_t wCodePage) {
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  ByteString bsHash = ByteString::Format("%d, %d", wCodePage, dwFontStyles);
  bsHash += FX_UTF8Encode(WideStringView(pszFontFamily));
  uint32_t dwHash = FX_HashCode_GetA(bsHash.AsStringView(), false);
  std::vector<RetainPtr<CFGAS_GEFont>>* pFontArray = &m_Hash2Fonts[dwHash];
  if (!pFontArray->empty())
    return (*pFontArray)[0];

  const FX_FONTDESCRIPTOR* pFD =
      FindFont(pszFontFamily, dwFontStyles, true, wCodePage, 999, 0);
  if (!pFD)
    pFD = FindFont(pszFontFamily, dwFontStyles, false, wCodePage, 999, 0);
  if (!pFD)
    return nullptr;

  RetainPtr<CFGAS_GEFont> pFont =
      CFGAS_GEFont::LoadFont(pFD->wsFontFace, dwFontStyles, wCodePage, this);
  if (!pFont)
    return nullptr;

  pFont->SetLogicalFontStyle(dwFontStyles);
  pFontArray->push_back(pFont);
  return pFont;
#else   // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  return GetFontByCodePage(wCodePage, dwFontStyles, pszFontFamily);
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
}

void CFGAS_FontMgr::RemoveFont(const RetainPtr<CFGAS_GEFont>& pEFont) {
  if (!pEFont)
    return;

#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
  m_IFXFont2FileRead.erase(pEFont);
#endif  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

  auto iter = m_Hash2Fonts.begin();
  while (iter != m_Hash2Fonts.end()) {
    auto old_iter = iter++;
    bool all_empty = true;
    for (size_t i = 0; i < old_iter->second.size(); i++) {
      if (old_iter->second[i] == pEFont)
        old_iter->second[i].Reset();
      else if (old_iter->second[i])
        all_empty = false;
    }
    if (all_empty)
      m_Hash2Fonts.erase(old_iter);
  }
}
