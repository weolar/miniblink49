// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_FONT_CFGAS_FONTMGR_H_
#define XFA_FGAS_FONT_CFGAS_FONTMGR_H_

#include <deque>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/observable.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/fx_freetype.h"
#include "core/fxge/systemfontinfo_iface.h"
#include "xfa/fgas/font/cfgas_pdffontmgr.h"

class CFGAS_GEFont;
class CFX_FontMapper;
class CFX_FontSourceEnum_File;
class IFX_SeekableReadStream;

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
struct FX_FONTMATCHPARAMS {
  const wchar_t* pwsFamily;
  uint32_t dwFontStyles;
  uint32_t dwUSB;
  bool matchParagraphStyle;
  wchar_t wUnicode;
  uint16_t wCodePage;
};

struct FX_FONTSIGNATURE {
  uint32_t fsUsb[4];
  uint32_t fsCsb[2];
};

inline bool operator==(const FX_FONTSIGNATURE& left,
                       const FX_FONTSIGNATURE& right) {
  return left.fsUsb[0] == right.fsUsb[0] && left.fsUsb[1] == right.fsUsb[1] &&
         left.fsUsb[2] == right.fsUsb[2] && left.fsUsb[3] == right.fsUsb[3] &&
         left.fsCsb[0] == right.fsCsb[0] && left.fsCsb[1] == right.fsCsb[1];
}

struct FX_FONTDESCRIPTOR {
  wchar_t wsFontFace[32];
  uint32_t dwFontStyles;
  uint8_t uCharSet;
  FX_FONTSIGNATURE FontSignature;
};

inline bool operator==(const FX_FONTDESCRIPTOR& left,
                       const FX_FONTDESCRIPTOR& right) {
  return left.uCharSet == right.uCharSet &&
         left.dwFontStyles == right.dwFontStyles &&
         left.FontSignature == right.FontSignature &&
         wcscmp(left.wsFontFace, right.wsFontFace) == 0;
}

#else  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

class CFX_FontDescriptor {
 public:
  CFX_FontDescriptor();
  ~CFX_FontDescriptor();

  int32_t m_nFaceIndex;
  uint32_t m_dwFontStyles;
  WideString m_wsFaceName;
  std::vector<WideString> m_wsFamilyNames;
  uint32_t m_dwUsb[4];
  uint32_t m_dwCsb[2];
};

class CFX_FontDescriptorInfo {
 public:
  CFX_FontDescriptor* pFont;
  int32_t nPenalty;

  bool operator>(const CFX_FontDescriptorInfo& other) const {
    return nPenalty > other.nPenalty;
  }
  bool operator<(const CFX_FontDescriptorInfo& other) const {
    return nPenalty < other.nPenalty;
  }
  bool operator==(const CFX_FontDescriptorInfo& other) const {
    return nPenalty == other.nPenalty;
  }
};

#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

class CFGAS_FontMgr final : public Observable<CFGAS_FontMgr> {
 public:
  CFGAS_FontMgr();
  ~CFGAS_FontMgr();

  RetainPtr<CFGAS_GEFont> GetFontByCodePage(uint16_t wCodePage,
                                            uint32_t dwFontStyles,
                                            const wchar_t* pszFontFamily);
  RetainPtr<CFGAS_GEFont> GetFontByUnicode(wchar_t wUnicode,
                                           uint32_t dwFontStyles,
                                           const wchar_t* pszFontFamily);
  RetainPtr<CFGAS_GEFont> LoadFont(const wchar_t* pszFontFamily,
                                   uint32_t dwFontStyles,
                                   uint16_t wCodePage);
  void RemoveFont(const RetainPtr<CFGAS_GEFont>& pFont);

  bool EnumFonts();

 private:
  RetainPtr<CFGAS_GEFont> GetFontByUnicodeImpl(wchar_t wUnicode,
                                               uint32_t dwFontStyles,
                                               const wchar_t* pszFontFamily,
                                               uint32_t dwHash,
                                               uint16_t wCodePage,
                                               uint16_t wBitField);

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  const FX_FONTDESCRIPTOR* FindFont(const wchar_t* pszFontFamily,
                                    uint32_t dwFontStyles,
                                    bool matchParagraphStyle,
                                    uint16_t wCodePage,
                                    uint32_t dwUSB,
                                    wchar_t wUnicode);

#else   // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  bool EnumFontsFromFontMapper();
  bool EnumFontsFromFiles();
  void RegisterFace(FXFT_Face pFace, const WideString* pFaceName);
  void RegisterFaces(const RetainPtr<IFX_SeekableReadStream>& pFontStream,
                     const WideString* pFaceName);
  void MatchFonts(std::vector<CFX_FontDescriptorInfo>* MatchedFonts,
                  uint16_t wCodePage,
                  uint32_t dwFontStyles,
                  const WideString& FontName,
                  wchar_t wcUnicode);
  RetainPtr<CFGAS_GEFont> LoadFontInternal(const WideString& wsFaceName,
                                           int32_t iFaceIndex);
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

  std::map<uint32_t, std::vector<RetainPtr<CFGAS_GEFont>>> m_Hash2Fonts;
  std::set<wchar_t> m_FailedUnicodesSet;

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  std::deque<FX_FONTDESCRIPTOR> m_FontFaces;
#else
  std::unique_ptr<CFX_FontSourceEnum_File> m_pFontSource;
  std::vector<std::unique_ptr<CFX_FontDescriptor>> m_InstalledFonts;
  std::map<uint32_t, std::unique_ptr<std::vector<CFX_FontDescriptorInfo>>>
      m_Hash2CandidateList;
  std::map<RetainPtr<CFGAS_GEFont>, RetainPtr<IFX_SeekableReadStream>>
      m_IFXFont2FileRead;
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
};

#endif  // XFA_FGAS_FONT_CFGAS_FONTMGR_H_
