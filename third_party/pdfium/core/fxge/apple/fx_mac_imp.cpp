// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <memory>
#include <utility>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/apple/apple_int.h"
#include "core/fxge/cfx_folderfontinfo.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/systemfontinfo_iface.h"
#include "third_party/base/ptr_util.h"

namespace {

const struct {
  const char* m_pName;
  const char* m_pSubstName;
} g_Base14Substs[] = {
    {"Courier", "Courier New"},
    {"Courier-Bold", "Courier New Bold"},
    {"Courier-BoldOblique", "Courier New Bold Italic"},
    {"Courier-Oblique", "Courier New Italic"},
    {"Helvetica", "Arial"},
    {"Helvetica-Bold", "Arial Bold"},
    {"Helvetica-BoldOblique", "Arial Bold Italic"},
    {"Helvetica-Oblique", "Arial Italic"},
    {"Times-Roman", "Times New Roman"},
    {"Times-Bold", "Times New Roman Bold"},
    {"Times-BoldItalic", "Times New Roman Bold Italic"},
    {"Times-Italic", "Times New Roman Italic"},
};

class CFX_MacFontInfo final : public CFX_FolderFontInfo {
 public:
  CFX_MacFontInfo() {}
  ~CFX_MacFontInfo() override {}

  // CFX_FolderFontInfo
  void* MapFont(int weight,
                bool bItalic,
                int charset,
                int pitch_family,
                const char* family) override;

  bool ParseFontCfg(const char** pUserPaths);
};

const char JAPAN_GOTHIC[] = "Hiragino Kaku Gothic Pro W6";
const char JAPAN_MINCHO[] = "Hiragino Mincho Pro W6";

void GetJapanesePreference(ByteString* face, int weight, int pitch_family) {
  if (face->Contains("Gothic")) {
    *face = JAPAN_GOTHIC;
    return;
  }
  *face = (FontFamilyIsRoman(pitch_family) || weight <= 400) ? JAPAN_MINCHO
                                                             : JAPAN_GOTHIC;
}

void* CFX_MacFontInfo::MapFont(int weight,
                               bool bItalic,
                               int charset,
                               int pitch_family,
                               const char* cstr_face) {
  ByteString face = cstr_face;
  for (const auto& sub : g_Base14Substs) {
    if (face == ByteStringView(sub.m_pName)) {
      face = sub.m_pSubstName;
      return GetFont(face.c_str());
    }
  }

  // The request may not ask for the bold and/or italic version of a font by
  // name. So try to construct the appropriate name. This is not 100% foolproof
  // as there are fonts that have "Oblique" or "BoldOblique" or "Heavy" in their
  // names instead. But this at least works for common fonts like Arial and
  // Times New Roman. A more sophisticated approach would be to find all the
  // fonts in |m_FontList| with |face| in the name, and examine the fonts to
  // see which best matches the requested characteristics.
  if (!face.Contains("Bold") && !face.Contains("Italic")) {
    ByteString new_face = face;
    if (weight > 400)
      new_face += " Bold";
    if (bItalic)
      new_face += " Italic";
    auto it = m_FontList.find(new_face);
    if (it != m_FontList.end())
      return it->second.get();
  }

  auto it = m_FontList.find(face);
  if (it != m_FontList.end())
    return it->second.get();

  if (charset == FX_CHARSET_ANSI && FontFamilyIsFixedPitch(pitch_family))
    return GetFont("Courier New");

  if (charset == FX_CHARSET_ANSI || charset == FX_CHARSET_Symbol)
    return nullptr;

  switch (charset) {
    case FX_CHARSET_ShiftJIS:
      GetJapanesePreference(&face, weight, pitch_family);
      break;
    case FX_CHARSET_ChineseSimplified:
      face = "STSong";
      break;
    case FX_CHARSET_Hangul:
      face = "AppleMyungjo";
      break;
    case FX_CHARSET_ChineseTraditional:
      face = "LiSong Pro Light";
  }
  it = m_FontList.find(face);
  return it != m_FontList.end() ? it->second.get() : nullptr;
}

bool CFX_MacFontInfo::ParseFontCfg(const char** pUserPaths) {
  if (!pUserPaths)
    return false;

  for (const char** pPath = pUserPaths; *pPath; ++pPath)
    AddPath(*pPath);
  return true;
}
}  // namespace

std::unique_ptr<SystemFontInfoIface> SystemFontInfoIface::CreateDefault(
    const char** pUserPaths) {
  auto pInfo = pdfium::MakeUnique<CFX_MacFontInfo>();
  if (!pInfo->ParseFontCfg(pUserPaths)) {
    pInfo->AddPath("~/Library/Fonts");
    pInfo->AddPath("/Library/Fonts");
    pInfo->AddPath("/System/Library/Fonts");
  }
  return std::move(pInfo);
}

void CFX_GEModule::InitPlatform() {
  m_pPlatformData = new CApplePlatform;
  m_pFontMgr->SetSystemFontInfo(
      SystemFontInfoIface::CreateDefault(m_pUserFontPaths));
}

void CFX_GEModule::DestroyPlatform() {
  delete reinterpret_cast<CApplePlatform*>(m_pPlatformData);
  m_pPlatformData = nullptr;
}
