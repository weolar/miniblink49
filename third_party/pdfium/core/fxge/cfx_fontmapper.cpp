// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_fontmapper.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_substfont.h"
#include "core/fxge/fx_font.h"
#include "core/fxge/systemfontinfo_iface.h"
#include "third_party/base/stl_util.h"

namespace {

const int kNumStandardFonts = 14;

const char* const g_Base14FontNames[kNumStandardFonts] = {
    "Courier",
    "Courier-Bold",
    "Courier-BoldOblique",
    "Courier-Oblique",
    "Helvetica",
    "Helvetica-Bold",
    "Helvetica-BoldOblique",
    "Helvetica-Oblique",
    "Times-Roman",
    "Times-Bold",
    "Times-BoldItalic",
    "Times-Italic",
    "Symbol",
    "ZapfDingbats",
};

struct AltFontName {
  const char* m_pName;  // Raw, POD struct.
  int m_Index;
};

const AltFontName g_AltFontNames[] = {
    {"Arial", 4},
    {"Arial,Bold", 5},
    {"Arial,BoldItalic", 6},
    {"Arial,Italic", 7},
    {"Arial-Bold", 5},
    {"Arial-BoldItalic", 6},
    {"Arial-BoldItalicMT", 6},
    {"Arial-BoldMT", 5},
    {"Arial-Italic", 7},
    {"Arial-ItalicMT", 7},
    {"ArialBold", 5},
    {"ArialBoldItalic", 6},
    {"ArialItalic", 7},
    {"ArialMT", 4},
    {"ArialMT,Bold", 5},
    {"ArialMT,BoldItalic", 6},
    {"ArialMT,Italic", 7},
    {"ArialRoundedMTBold", 5},
    {"Courier", 0},
    {"Courier,Bold", 1},
    {"Courier,BoldItalic", 2},
    {"Courier,Italic", 3},
    {"Courier-Bold", 1},
    {"Courier-BoldOblique", 2},
    {"Courier-Oblique", 3},
    {"CourierBold", 1},
    {"CourierBoldItalic", 2},
    {"CourierItalic", 3},
    {"CourierNew", 0},
    {"CourierNew,Bold", 1},
    {"CourierNew,BoldItalic", 2},
    {"CourierNew,Italic", 3},
    {"CourierNew-Bold", 1},
    {"CourierNew-BoldItalic", 2},
    {"CourierNew-Italic", 3},
    {"CourierNewBold", 1},
    {"CourierNewBoldItalic", 2},
    {"CourierNewItalic", 3},
    {"CourierNewPS-BoldItalicMT", 2},
    {"CourierNewPS-BoldMT", 1},
    {"CourierNewPS-ItalicMT", 3},
    {"CourierNewPSMT", 0},
    {"CourierStd", 0},
    {"CourierStd-Bold", 1},
    {"CourierStd-BoldOblique", 2},
    {"CourierStd-Oblique", 3},
    {"Helvetica", 4},
    {"Helvetica,Bold", 5},
    {"Helvetica,BoldItalic", 6},
    {"Helvetica,Italic", 7},
    {"Helvetica-Bold", 5},
    {"Helvetica-BoldItalic", 6},
    {"Helvetica-BoldOblique", 6},
    {"Helvetica-Italic", 7},
    {"Helvetica-Oblique", 7},
    {"HelveticaBold", 5},
    {"HelveticaBoldItalic", 6},
    {"HelveticaItalic", 7},
    {"Symbol", 12},
    {"SymbolMT", 12},
    {"Times-Bold", 9},
    {"Times-BoldItalic", 10},
    {"Times-Italic", 11},
    {"Times-Roman", 8},
    {"TimesBold", 9},
    {"TimesBoldItalic", 10},
    {"TimesItalic", 11},
    {"TimesNewRoman", 8},
    {"TimesNewRoman,Bold", 9},
    {"TimesNewRoman,BoldItalic", 10},
    {"TimesNewRoman,Italic", 11},
    {"TimesNewRoman-Bold", 9},
    {"TimesNewRoman-BoldItalic", 10},
    {"TimesNewRoman-Italic", 11},
    {"TimesNewRomanBold", 9},
    {"TimesNewRomanBoldItalic", 10},
    {"TimesNewRomanItalic", 11},
    {"TimesNewRomanPS", 8},
    {"TimesNewRomanPS-Bold", 9},
    {"TimesNewRomanPS-BoldItalic", 10},
    {"TimesNewRomanPS-BoldItalicMT", 10},
    {"TimesNewRomanPS-BoldMT", 9},
    {"TimesNewRomanPS-Italic", 11},
    {"TimesNewRomanPS-ItalicMT", 11},
    {"TimesNewRomanPSMT", 8},
    {"TimesNewRomanPSMT,Bold", 9},
    {"TimesNewRomanPSMT,BoldItalic", 10},
    {"TimesNewRomanPSMT,Italic", 11},
    {"ZapfDingbats", 13},
};

struct AltFontFamily {
  const char* m_pFontName;    // Raw, POD struct.
  const char* m_pFontFamily;  // Raw, POD struct.
};

const AltFontFamily g_AltFontFamilies[] = {
    {"AGaramondPro", "Adobe Garamond Pro"},
    {"BankGothicBT-Medium", "BankGothic Md BT"},
    {"ForteMT", "Forte"},
};

#if _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
const char kNarrowFamily[] = "LiberationSansNarrow";
#elif _FX_PLATFORM_ == _FX_PLATFORM_ANDROID_
const char kNarrowFamily[] = "RobotoCondensed";
#else
const char kNarrowFamily[] = "ArialNarrow";
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_LINUX_

ByteString TT_NormalizeName(const char* family) {
  ByteString norm(family);
  norm.Remove(' ');
  norm.Remove('-');
  norm.Remove(',');
  auto pos = norm.Find('+');
  if (pos.has_value() && pos.value() != 0)
    norm = norm.Left(pos.value());
  norm.MakeLower();
  return norm;
}

void GetFontFamily(uint32_t nStyle, ByteString* fontName) {
  if (fontName->Contains("Script")) {
    if (FontStyleIsBold(nStyle))
      *fontName = "ScriptMTBold";
    else if (fontName->Contains("Palace"))
      *fontName = "PalaceScriptMT";
    else if (fontName->Contains("French"))
      *fontName = "FrenchScriptMT";
    else if (fontName->Contains("FreeStyle"))
      *fontName = "FreeStyleScript";
    return;
  }
  for (const auto& alternate : g_AltFontFamilies) {
    if (fontName->Contains(alternate.m_pFontName)) {
      *fontName = alternate.m_pFontFamily;
      return;
    }
  }
}

ByteString ParseStyle(const char* pStyle, int iLen, int iIndex) {
  std::ostringstream buf;
  if (!iLen || iLen <= iIndex)
    return ByteString(buf);
  while (iIndex < iLen) {
    if (pStyle[iIndex] == ',')
      break;
    buf << pStyle[iIndex];
    ++iIndex;
  }
  return ByteString(buf);
}

const struct FX_FontStyle {
  const char* name;
  size_t len;
  uint32_t style;
} g_FontStyles[] = {
    {"Bold", 4, FXFONT_BOLD},
    {"Italic", 6, FXFONT_ITALIC},
    {"BoldItalic", 10, FXFONT_BOLD | FXFONT_ITALIC},
    {"Reg", 3, FXFONT_NORMAL},
    {"Regular", 7, FXFONT_NORMAL},
};

// <exists, index, length>
std::tuple<bool, uint32_t, size_t> GetStyleType(const ByteString& bsStyle,
                                                bool bReverse) {
  if (bsStyle.IsEmpty())
    return std::make_tuple(false, FXFONT_NORMAL, 0);

  for (int i = FX_ArraySize(g_FontStyles) - 1; i >= 0; --i) {
    const FX_FontStyle* pStyle = g_FontStyles + i;
    if (!pStyle || pStyle->len > bsStyle.GetLength())
      continue;

    if (bReverse) {
      if (bsStyle.Right(pStyle->len).Compare(pStyle->name) == 0)
        return std::make_tuple(true, pStyle->style, pStyle->len);
    } else {
      if (bsStyle.Left(pStyle->len).Compare(pStyle->name) == 0)
        return std::make_tuple(true, pStyle->style, pStyle->len);
    }
  }
  return std::make_tuple(false, FXFONT_NORMAL, 0);
}

bool CheckSupportThirdPartFont(const ByteString& name, int* PitchFamily) {
  if (name != "MyriadPro")
    return false;
  *PitchFamily &= ~FXFONT_FF_ROMAN;
  return true;
}

void UpdatePitchFamily(uint32_t flags, int* PitchFamily) {
  if (FontStyleIsSerif(flags))
    *PitchFamily |= FXFONT_FF_ROMAN;
  if (FontStyleIsScript(flags))
    *PitchFamily |= FXFONT_FF_SCRIPT;
  if (FontStyleIsFixedPitch(flags))
    *PitchFamily |= FXFONT_FF_FIXEDPITCH;
}

}  // namespace

CFX_FontMapper::CFX_FontMapper(CFX_FontMgr* mgr)
    : m_bListLoaded(false), m_pFontMgr(mgr) {
  m_MMFaces[0] = nullptr;
  m_MMFaces[1] = nullptr;
  memset(m_FoxitFaces, 0, sizeof(m_FoxitFaces));
}

CFX_FontMapper::~CFX_FontMapper() {
  for (size_t i = 0; i < FX_ArraySize(m_FoxitFaces); ++i) {
    if (m_FoxitFaces[i])
      FXFT_Done_Face(m_FoxitFaces[i]);
  }
  if (m_MMFaces[0])
    FXFT_Done_Face(m_MMFaces[0]);
  if (m_MMFaces[1])
    FXFT_Done_Face(m_MMFaces[1]);
}

void CFX_FontMapper::SetSystemFontInfo(
    std::unique_ptr<SystemFontInfoIface> pFontInfo) {
  if (!pFontInfo)
    return;

  m_pFontInfo = std::move(pFontInfo);
}

ByteString CFX_FontMapper::GetPSNameFromTT(void* hFont) {
  if (!m_pFontInfo)
    return ByteString();

  uint32_t size = m_pFontInfo->GetFontData(hFont, kTableNAME, nullptr, 0);
  if (!size)
    return ByteString();

  std::vector<uint8_t> buffer(size);
  uint8_t* buffer_ptr = buffer.data();
  uint32_t bytes_read =
      m_pFontInfo->GetFontData(hFont, kTableNAME, buffer_ptr, size);
  return bytes_read == size ? GetNameFromTT(buffer_ptr, bytes_read, 6)
                            : ByteString();
}

void CFX_FontMapper::AddInstalledFont(const ByteString& name, int charset) {
  if (!m_pFontInfo)
    return;

  m_FaceArray.push_back({name, static_cast<uint32_t>(charset)});
  if (name == m_LastFamily)
    return;

  bool bLocalized = std::any_of(name.begin(), name.end(), [](const char& c) {
    return static_cast<uint8_t>(c) > 0x80;
  });

  if (bLocalized) {
    void* hFont = m_pFontInfo->GetFont(name.c_str());
    if (!hFont) {
      hFont = m_pFontInfo->MapFont(0, 0, FX_CHARSET_Default, 0, name.c_str());
      if (!hFont)
        return;
    }

    ByteString new_name = GetPSNameFromTT(hFont);
    if (!new_name.IsEmpty())
      m_LocalizedTTFonts.push_back(std::make_pair(new_name, name));
    m_pFontInfo->DeleteFont(hFont);
  }
  m_InstalledTTFonts.push_back(name);
  m_LastFamily = name;
}

void CFX_FontMapper::LoadInstalledFonts() {
  if (!m_pFontInfo || m_bListLoaded)
    return;

  m_pFontInfo->EnumFontList(this);
  m_bListLoaded = true;
}

ByteString CFX_FontMapper::MatchInstalledFonts(const ByteString& norm_name) {
  LoadInstalledFonts();
  int i;
  for (i = pdfium::CollectionSize<int>(m_InstalledTTFonts) - 1; i >= 0; i--) {
    ByteString norm1 = TT_NormalizeName(m_InstalledTTFonts[i].c_str());
    if (norm1 == norm_name)
      return m_InstalledTTFonts[i];
  }
  for (i = pdfium::CollectionSize<int>(m_LocalizedTTFonts) - 1; i >= 0; i--) {
    ByteString norm1 = TT_NormalizeName(m_LocalizedTTFonts[i].first.c_str());
    if (norm1 == norm_name)
      return m_LocalizedTTFonts[i].second;
  }
  return ByteString();
}

FXFT_Face CFX_FontMapper::UseInternalSubst(CFX_SubstFont* pSubstFont,
                                           int iBaseFont,
                                           int italic_angle,
                                           int weight,
                                           int pitch_family) {
  if (iBaseFont < kNumStandardFonts) {
    if (m_FoxitFaces[iBaseFont])
      return m_FoxitFaces[iBaseFont];
    Optional<pdfium::span<const uint8_t>> font_data =
        m_pFontMgr->GetBuiltinFont(iBaseFont);
    if (font_data.has_value()) {
      m_FoxitFaces[iBaseFont] = m_pFontMgr->GetFixedFace(font_data.value(), 0);
      return m_FoxitFaces[iBaseFont];
    }
  }
  pSubstFont->m_bFlagMM = true;
  pSubstFont->m_ItalicAngle = italic_angle;
  if (weight)
    pSubstFont->m_Weight = weight;
  if (FontFamilyIsRoman(pitch_family)) {
    pSubstFont->m_Weight = pSubstFont->m_Weight * 4 / 5;
    pSubstFont->m_Family = "Chrome Serif";
    if (!m_MMFaces[1]) {
      m_MMFaces[1] =
          m_pFontMgr->GetFixedFace(m_pFontMgr->GetBuiltinFont(14).value(), 0);
    }
    return m_MMFaces[1];
  }
  pSubstFont->m_Family = "Chrome Sans";
  if (!m_MMFaces[0]) {
    m_MMFaces[0] =
        m_pFontMgr->GetFixedFace(m_pFontMgr->GetBuiltinFont(15).value(), 0);
  }
  return m_MMFaces[0];
}

FXFT_Face CFX_FontMapper::FindSubstFont(const ByteString& name,
                                        bool bTrueType,
                                        uint32_t flags,
                                        int weight,
                                        int italic_angle,
                                        int WindowCP,
                                        CFX_SubstFont* pSubstFont) {
  if (weight == 0)
    weight = FXFONT_FW_NORMAL;

  if (!(flags & FXFONT_USEEXTERNATTR)) {
    weight = FXFONT_FW_NORMAL;
    italic_angle = 0;
  }
  ByteString SubstName = name;
  SubstName.Remove(' ');
  if (bTrueType && name.GetLength() > 0 && name[0] == '@')
    SubstName = name.Right(name.GetLength() - 1);
  PDF_GetStandardFontName(&SubstName);
  if (SubstName == "Symbol" && !bTrueType) {
    pSubstFont->m_Family = "Chrome Symbol";
    pSubstFont->m_Charset = FX_CHARSET_Symbol;
    return UseInternalSubst(pSubstFont, 12, italic_angle, weight, 0);
  }
  if (SubstName == "ZapfDingbats") {
    pSubstFont->m_Family = "Chrome Dingbats";
    pSubstFont->m_Charset = FX_CHARSET_Symbol;
    return UseInternalSubst(pSubstFont, 13, italic_angle, weight, 0);
  }
  int iBaseFont = 0;
  ByteString family;
  ByteString style;
  bool bHasComma = false;
  bool bHasHyphen = false;
  {
    Optional<size_t> pos = SubstName.Find(",", 0);
    if (pos.has_value()) {
      family = SubstName.Left(pos.value());
      PDF_GetStandardFontName(&family);
      style = SubstName.Right(SubstName.GetLength() - (pos.value() + 1));
      bHasComma = true;
    } else {
      family = SubstName;
    }
  }
  for (; iBaseFont < 12; iBaseFont++) {
    if (family == ByteStringView(g_Base14FontNames[iBaseFont]))
      break;
  }
  int PitchFamily = 0;
  uint32_t nStyle = FXFONT_NORMAL;
  bool bStyleAvail = false;
  if (iBaseFont < 12) {
    if ((iBaseFont % 4) == 1 || (iBaseFont % 4) == 2)
      nStyle |= FXFONT_BOLD;
    if ((iBaseFont % 4) / 2)
      nStyle |= FXFONT_ITALIC;
    if (iBaseFont < 4)
      PitchFamily |= FXFONT_FF_FIXEDPITCH;
    if (iBaseFont >= 8)
      PitchFamily |= FXFONT_FF_ROMAN;
  } else {
    iBaseFont = kNumStandardFonts;
    if (!bHasComma) {
      Optional<size_t> pos = family.ReverseFind('-');
      if (pos.has_value()) {
        style = family.Right(family.GetLength() - (pos.value() + 1));
        family = family.Left(pos.value());
        bHasHyphen = true;
      }
    }
    if (!bHasHyphen) {
      int nLen = family.GetLength();
      bool hasStyleType;
      uint32_t styleType;
      size_t len;
      std::tie(hasStyleType, styleType, len) = GetStyleType(family, true);
      if (hasStyleType) {
        family = family.Left(nLen - len);
        nStyle |= styleType;
      }
    }
    UpdatePitchFamily(flags, &PitchFamily);
  }

  const int old_weight = weight;
  if (FontStyleIsBold(nStyle))
    weight = FXFONT_FW_BOLD;

  if (!style.IsEmpty()) {
    int nLen = style.GetLength();
    const char* pStyle = style.c_str();
    int i = 0;
    bool bFirstItem = true;
    ByteString buf;
    while (i < nLen) {
      buf = ParseStyle(pStyle, nLen, i);

      bool hasStyleType;
      uint32_t styleType;
      size_t len;
      std::tie(hasStyleType, styleType, len) = GetStyleType(buf, false);
      if ((i && !bStyleAvail) || (!i && !hasStyleType)) {
        family = SubstName;
        iBaseFont = kNumStandardFonts;
        break;
      }
      if (hasStyleType)
        bStyleAvail = true;

      if (FontStyleIsBold(styleType)) {
        // If we're already bold, then we're double bold, use special weight.
        if (FontStyleIsBold(nStyle)) {
          weight = FXFONT_FW_BOLD_BOLD;
        } else {
          weight = FXFONT_FW_BOLD;
          nStyle |= FXFONT_BOLD;
        }

        bFirstItem = false;
      }
      if (FontStyleIsItalic(styleType) && FontStyleIsBold(styleType)) {
        nStyle |= FXFONT_ITALIC;
      } else if (FontStyleIsItalic(styleType)) {
        if (bFirstItem) {
          nStyle |= FXFONT_ITALIC;
        } else {
          family = SubstName;
          iBaseFont = kNumStandardFonts;
        }
        break;
      }
      i += buf.GetLength() + 1;
    }
  }

  if (!m_pFontInfo) {
    return UseInternalSubst(pSubstFont, iBaseFont, italic_angle, old_weight,
                            PitchFamily);
  }

  int Charset = FX_CHARSET_ANSI;
  if (WindowCP)
    Charset = FX_GetCharsetFromCodePage(WindowCP);
  else if (iBaseFont == kNumStandardFonts && FontStyleIsSymbolic(flags))
    Charset = FX_CHARSET_Symbol;
  const bool bCJK = FX_CharSetIsCJK(Charset);
  bool bItalic = FontStyleIsItalic(nStyle);

  GetFontFamily(nStyle, &family);
  ByteString match = MatchInstalledFonts(TT_NormalizeName(family.c_str()));
  if (match.IsEmpty() && family != SubstName &&
      (!bHasComma && (!bHasHyphen || (bHasHyphen && !bStyleAvail)))) {
    match = MatchInstalledFonts(TT_NormalizeName(SubstName.c_str()));
  }
  if (match.IsEmpty() && iBaseFont >= kNumStandardFonts) {
    if (!bCJK) {
      if (!CheckSupportThirdPartFont(family, &PitchFamily)) {
        bItalic = italic_angle != 0;
        weight = old_weight;
      }
      Optional<size_t> pos = SubstName.Find("Narrow");
      if (pos.has_value() && pos.value() != 0)
        family = kNarrowFamily;
      pos = SubstName.Find("Condensed");
      if (pos.has_value() && pos.value() != 0)
        family = kNarrowFamily;
    } else {
      pSubstFont->m_bSubstCJK = true;
      if (nStyle)
        pSubstFont->m_WeightCJK = nStyle ? weight : FXFONT_FW_NORMAL;
      if (FontStyleIsItalic(nStyle))
        pSubstFont->m_bItalicCJK = true;
    }
  } else {
    italic_angle = 0;
    if (nStyle == FXFONT_NORMAL)
      weight = FXFONT_FW_NORMAL;
  }

  if (!match.IsEmpty() || iBaseFont < kNumStandardFonts) {
    if (!match.IsEmpty())
      family = match;
    if (iBaseFont < kNumStandardFonts) {
      if (nStyle && !(iBaseFont % 4)) {
        if (FontStyleIsBold(nStyle) && FontStyleIsItalic(nStyle))
          iBaseFont += 2;
        else if (FontStyleIsBold(nStyle))
          iBaseFont += 1;
        else if (FontStyleIsItalic(nStyle))
          iBaseFont += 3;
      }
      family = g_Base14FontNames[iBaseFont];
    }
  } else if (FontStyleIsItalic(flags)) {
    bItalic = true;
  }
  void* hFont = m_pFontInfo->MapFont(weight, bItalic, Charset, PitchFamily,
                                     family.c_str());
  if (!hFont) {
    if (bCJK) {
      bItalic = italic_angle != 0;
      weight = old_weight;
    }
    if (!match.IsEmpty()) {
      hFont = m_pFontInfo->GetFont(match.c_str());
      if (!hFont) {
        return UseInternalSubst(pSubstFont, iBaseFont, italic_angle, old_weight,
                                PitchFamily);
      }
    } else {
      if (Charset == FX_CHARSET_Symbol) {
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_ || \
    _FX_PLATFORM_ == _FX_PLATFORM_ANDROID_
        if (SubstName == "Symbol") {
          pSubstFont->m_Family = "Chrome Symbol";
          pSubstFont->m_Charset = FX_CHARSET_Symbol;
          return UseInternalSubst(pSubstFont, 12, italic_angle, old_weight,
                                  PitchFamily);
        }
#endif
        return FindSubstFont(family, bTrueType, flags & ~FXFONT_SYMBOLIC,
                             weight, italic_angle, 0, pSubstFont);
      }
      if (Charset == FX_CHARSET_ANSI) {
        return UseInternalSubst(pSubstFont, iBaseFont, italic_angle, old_weight,
                                PitchFamily);
      }

      auto it =
          std::find_if(m_FaceArray.begin(), m_FaceArray.end(),
                       [Charset](const FaceData& face) {
                         return face.charset == static_cast<uint32_t>(Charset);
                       });
      if (it == m_FaceArray.end()) {
        return UseInternalSubst(pSubstFont, iBaseFont, italic_angle, old_weight,
                                PitchFamily);
      }
      hFont = m_pFontInfo->GetFont(it->name.c_str());
    }
  }
  if (!hFont)
    return nullptr;

  m_pFontInfo->GetFaceName(hFont, &SubstName);
  if (Charset == FX_CHARSET_Default)
    m_pFontInfo->GetFontCharset(hFont, &Charset);
  uint32_t ttc_size = m_pFontInfo->GetFontData(hFont, kTableTTCF, nullptr, 0);
  uint32_t font_size = m_pFontInfo->GetFontData(hFont, 0, nullptr, 0);
  if (font_size == 0 && ttc_size == 0) {
    m_pFontInfo->DeleteFont(hFont);
    return nullptr;
  }
  FXFT_Face face = nullptr;
  if (ttc_size)
    face = GetCachedTTCFace(hFont, kTableTTCF, ttc_size, font_size);
  else
    face = GetCachedFace(hFont, SubstName, weight, bItalic, font_size);
  if (!face) {
    m_pFontInfo->DeleteFont(hFont);
    return nullptr;
  }
  pSubstFont->m_Family = SubstName;
  pSubstFont->m_Charset = Charset;
  bool bNeedUpdateWeight = false;
  if (FXFT_Is_Face_Bold(face))
    bNeedUpdateWeight = weight != FXFONT_FW_BOLD;
  else
    bNeedUpdateWeight = weight != FXFONT_FW_NORMAL;
  if (bNeedUpdateWeight)
    pSubstFont->m_Weight = weight;
  if (bItalic && !FXFT_Is_Face_Italic(face)) {
    if (italic_angle == 0)
      italic_angle = -12;
    else if (abs(italic_angle) < 5)
      italic_angle = 0;
    pSubstFont->m_ItalicAngle = italic_angle;
  }
  m_pFontInfo->DeleteFont(hFont);
  return face;
}

int CFX_FontMapper::GetFaceSize() const {
  return pdfium::CollectionSize<int>(m_FaceArray);
}

bool CFX_FontMapper::IsBuiltinFace(const FXFT_Face face) const {
  for (size_t i = 0; i < MM_FACE_COUNT; ++i) {
    if (m_MMFaces[i] == face)
      return true;
  }
  for (size_t i = 0; i < FOXIT_FACE_COUNT; ++i) {
    if (m_FoxitFaces[i] == face)
      return true;
  }
  return false;
}

FXFT_Face CFX_FontMapper::GetCachedTTCFace(void* hFont,
                                           const uint32_t tableTTCF,
                                           uint32_t ttc_size,
                                           uint32_t font_size) {
  uint32_t checksum = 0;
  {
    uint8_t buffer[1024];
    m_pFontInfo->GetFontData(hFont, tableTTCF, buffer, sizeof(buffer));
    uint32_t* pBuffer = reinterpret_cast<uint32_t*>(buffer);
    for (int i = 0; i < 256; i++)
      checksum += pBuffer[i];
  }
  uint8_t* pIgnore = nullptr;
  FXFT_Face face = m_pFontMgr->GetCachedTTCFace(ttc_size, checksum,
                                                ttc_size - font_size, &pIgnore);
  if (face)
    return face;

  std::unique_ptr<uint8_t, FxFreeDeleter> pFontData(
      FX_Alloc(uint8_t, ttc_size));
  m_pFontInfo->GetFontData(hFont, tableTTCF, pFontData.get(), ttc_size);
  return m_pFontMgr->AddCachedTTCFace(ttc_size, checksum, std::move(pFontData),
                                      ttc_size, ttc_size - font_size);
}

FXFT_Face CFX_FontMapper::GetCachedFace(void* hFont,
                                        ByteString SubstName,
                                        int weight,
                                        bool bItalic,
                                        uint32_t font_size) {
  uint8_t* pIgnore = nullptr;
  FXFT_Face face =
      m_pFontMgr->GetCachedFace(SubstName, weight, bItalic, &pIgnore);
  if (face)
    return face;

  std::unique_ptr<uint8_t, FxFreeDeleter> pFontData(
      FX_Alloc(uint8_t, font_size));
  m_pFontInfo->GetFontData(hFont, 0, pFontData.get(), font_size);
  return m_pFontMgr->AddCachedFace(SubstName, weight, bItalic,
                                   std::move(pFontData), font_size,
                                   m_pFontInfo->GetFaceIndex(hFont));
}

int PDF_GetStandardFontName(ByteString* name) {
  const auto* end = std::end(g_AltFontNames);
  const auto* found =
      std::lower_bound(std::begin(g_AltFontNames), end, name->c_str(),
                       [](const AltFontName& element, const char* name) {
                         return FXSYS_stricmp(element.m_pName, name) < 0;
                       });
  if (found == end || FXSYS_stricmp(found->m_pName, name->c_str()))
    return -1;

  *name = g_Base14FontNames[found->m_Index];
  return found->m_Index;
}
