// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <memory>
#include <utility>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_folderfontinfo.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/systemfontinfo_iface.h"
#include "third_party/base/ptr_util.h"

#if _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
namespace {

const size_t kLinuxGpNameSize = 6;

const char* const g_LinuxGpFontList[][kLinuxGpNameSize] = {
    {"TakaoPGothic", "VL PGothic", "IPAPGothic", "VL Gothic", "Kochi Gothic",
     "VL Gothic regular"},
    {"TakaoGothic", "VL Gothic", "IPAGothic", "Kochi Gothic", nullptr,
     "VL Gothic regular"},
    {"TakaoPMincho", "IPAPMincho", "VL Gothic", "Kochi Mincho", nullptr,
     "VL Gothic regular"},
    {"TakaoMincho", "IPAMincho", "VL Gothic", "Kochi Mincho", nullptr,
     "VL Gothic regular"},
};

const char* const g_LinuxGbFontList[] = {
    "AR PL UMing CN Light", "WenQuanYi Micro Hei", "AR PL UKai CN",
};

const char* const g_LinuxB5FontList[] = {
    "AR PL UMing TW Light", "WenQuanYi Micro Hei", "AR PL UKai TW",
};

const char* const g_LinuxHGFontList[] = {
    "UnDotum",
};

size_t GetJapanesePreference(const char* facearr,
                             int weight,
                             int pitch_family) {
  ByteString face = facearr;
  if (face.Contains("Gothic") ||
      face.Contains("\x83\x53\x83\x56\x83\x62\x83\x4e")) {
    if (face.Contains("PGothic") ||
        face.Contains("\x82\x6f\x83\x53\x83\x56\x83\x62\x83\x4e")) {
      return 0;
    }
    return 1;
  }
  if (face.Contains("Mincho") || face.Contains("\x96\xbe\x92\xa9")) {
    if (face.Contains("PMincho") || face.Contains("\x82\x6f\x96\xbe\x92\xa9")) {
      return 2;
    }
    return 3;
  }
  if (!FontFamilyIsRoman(pitch_family) && weight > 400)
    return 0;

  return 2;
}

class CFX_LinuxFontInfo final : public CFX_FolderFontInfo {
 public:
  CFX_LinuxFontInfo() {}
  ~CFX_LinuxFontInfo() override {}

  void* MapFont(int weight,
                bool bItalic,
                int charset,
                int pitch_family,
                const char* family) override;
  bool ParseFontCfg(const char** pUserPaths);
};

void* CFX_LinuxFontInfo::MapFont(int weight,
                                 bool bItalic,
                                 int charset,
                                 int pitch_family,
                                 const char* cstr_face) {
  void* font = GetSubstFont(cstr_face);
  if (font)
    return font;

  bool bCJK = true;
  switch (charset) {
    case FX_CHARSET_ShiftJIS: {
      size_t index = GetJapanesePreference(cstr_face, weight, pitch_family);
      ASSERT(index < FX_ArraySize(g_LinuxGpFontList));
      for (size_t i = 0; i < kLinuxGpNameSize; i++) {
        auto it = m_FontList.find(g_LinuxGpFontList[index][i]);
        if (it != m_FontList.end())
          return it->second.get();
      }
      break;
    }
    case FX_CHARSET_ChineseSimplified: {
      for (const char* name : g_LinuxGbFontList) {
        auto it = m_FontList.find(name);
        if (it != m_FontList.end())
          return it->second.get();
      }
      break;
    }
    case FX_CHARSET_ChineseTraditional: {
      for (const char* name : g_LinuxB5FontList) {
        auto it = m_FontList.find(name);
        if (it != m_FontList.end())
          return it->second.get();
      }
      break;
    }
    case FX_CHARSET_Hangul: {
      for (const char* name : g_LinuxHGFontList) {
        auto it = m_FontList.find(name);
        if (it != m_FontList.end())
          return it->second.get();
      }
      break;
    }
    default:
      bCJK = false;
      break;
  }
  return FindFont(weight, bItalic, charset, pitch_family, cstr_face, !bCJK);
}

bool CFX_LinuxFontInfo::ParseFontCfg(const char** pUserPaths) {
  if (!pUserPaths)
    return false;

  for (const char** pPath = pUserPaths; *pPath; ++pPath)
    AddPath(*pPath);
  return true;
}

}  // namespace

std::unique_ptr<SystemFontInfoIface> SystemFontInfoIface::CreateDefault(
    const char** pUserPaths) {
  auto pInfo = pdfium::MakeUnique<CFX_LinuxFontInfo>();
  if (!pInfo->ParseFontCfg(pUserPaths)) {
    pInfo->AddPath("/usr/share/fonts");
    pInfo->AddPath("/usr/share/X11/fonts/Type1");
    pInfo->AddPath("/usr/share/X11/fonts/TTF");
    pInfo->AddPath("/usr/local/share/fonts");
  }
  return std::move(pInfo);
}

void CFX_GEModule::InitPlatform() {
  m_pFontMgr->SetSystemFontInfo(
      SystemFontInfoIface::CreateDefault(m_pUserFontPaths));
}

void CFX_GEModule::DestroyPlatform() {}
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
