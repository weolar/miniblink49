// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_sysfontinfo.h"

#include <memory>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_font.h"
#include "core/fxge/cfx_fontmapper.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/fx_font.h"
#include "core/fxge/systemfontinfo_iface.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "third_party/base/ptr_util.h"

static_assert(FXFONT_ANSI_CHARSET == FX_CHARSET_ANSI, "Charset must match");
static_assert(FXFONT_DEFAULT_CHARSET == FX_CHARSET_Default,
              "Charset must match");
static_assert(FXFONT_SYMBOL_CHARSET == FX_CHARSET_Symbol, "Charset must match");
static_assert(FXFONT_SHIFTJIS_CHARSET == FX_CHARSET_ShiftJIS,
              "Charset must match");
static_assert(FXFONT_HANGEUL_CHARSET == FX_CHARSET_Hangul,
              "Charset must match");
static_assert(FXFONT_GB2312_CHARSET == FX_CHARSET_ChineseSimplified,
              "Charset must match");
static_assert(FXFONT_CHINESEBIG5_CHARSET == FX_CHARSET_ChineseTraditional,
              "Charset must match");
static_assert(sizeof(CFX_Font::CharsetFontMap) == sizeof(FPDF_CharsetFontMap),
              "CFX_Font::CharsetFontMap should be same as FPDF_CharsetFontMap");

class CFX_ExternalFontInfo final : public SystemFontInfoIface {
 public:
  explicit CFX_ExternalFontInfo(FPDF_SYSFONTINFO* pInfo) : m_pInfo(pInfo) {}
  ~CFX_ExternalFontInfo() override {
    if (m_pInfo->Release)
      m_pInfo->Release(m_pInfo);
  }

  bool EnumFontList(CFX_FontMapper* pMapper) override {
    if (m_pInfo->EnumFonts) {
      m_pInfo->EnumFonts(m_pInfo, pMapper);
      return true;
    }
    return false;
  }

  void* MapFont(int weight,
                bool bItalic,
                int charset,
                int pitch_family,
                const char* family) override {
    if (!m_pInfo->MapFont)
      return nullptr;

    int iExact;
    return m_pInfo->MapFont(m_pInfo, weight, bItalic, charset, pitch_family,
                            family, &iExact);
  }

  void* GetFont(const char* family) override {
    if (!m_pInfo->GetFont)
      return nullptr;
    return m_pInfo->GetFont(m_pInfo, family);
  }

  uint32_t GetFontData(void* hFont,
                       uint32_t table,
                       uint8_t* buffer,
                       uint32_t size) override {
    if (!m_pInfo->GetFontData)
      return 0;
    return m_pInfo->GetFontData(m_pInfo, hFont, table, buffer, size);
  }

  bool GetFaceName(void* hFont, ByteString* name) override {
    if (!m_pInfo->GetFaceName)
      return false;
    uint32_t size = m_pInfo->GetFaceName(m_pInfo, hFont, nullptr, 0);
    if (size == 0)
      return false;
    char* buffer = FX_Alloc(char, size);
    size = m_pInfo->GetFaceName(m_pInfo, hFont, buffer, size);
    *name = ByteString(buffer, size);
    FX_Free(buffer);
    return true;
  }

  bool GetFontCharset(void* hFont, int* charset) override {
    if (!m_pInfo->GetFontCharset)
      return false;

    *charset = m_pInfo->GetFontCharset(m_pInfo, hFont);
    return true;
  }

  void DeleteFont(void* hFont) override {
    if (m_pInfo->DeleteFont)
      m_pInfo->DeleteFont(m_pInfo, hFont);
  }

 private:
  FPDF_SYSFONTINFO* const m_pInfo;
};

FPDF_EXPORT void FPDF_CALLCONV FPDF_AddInstalledFont(void* mapper,
                                                     const char* name,
                                                     int charset) {
  CFX_FontMapper* pMapper = static_cast<CFX_FontMapper*>(mapper);
  pMapper->AddInstalledFont(name, charset);
}

FPDF_EXPORT void FPDF_CALLCONV
FPDF_SetSystemFontInfo(FPDF_SYSFONTINFO* pFontInfoExt) {
  if (pFontInfoExt->version != 1)
    return;

  CFX_GEModule::Get()->GetFontMgr()->SetSystemFontInfo(
      pdfium::MakeUnique<CFX_ExternalFontInfo>(pFontInfoExt));
}

FPDF_EXPORT const FPDF_CharsetFontMap* FPDF_CALLCONV FPDF_GetDefaultTTFMap() {
  return reinterpret_cast<const FPDF_CharsetFontMap*>(CFX_Font::defaultTTFMap);
}

struct FPDF_SYSFONTINFO_DEFAULT final : public FPDF_SYSFONTINFO {
  UnownedPtr<SystemFontInfoIface> m_pFontInfo;
};

static void DefaultRelease(struct _FPDF_SYSFONTINFO* pThis) {
  auto* pDefault = static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pThis);
  delete pDefault->m_pFontInfo.Release();
}

static void DefaultEnumFonts(struct _FPDF_SYSFONTINFO* pThis, void* pMapper) {
  auto* pDefault = static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pThis);
  pDefault->m_pFontInfo->EnumFontList(static_cast<CFX_FontMapper*>(pMapper));
}

static void* DefaultMapFont(struct _FPDF_SYSFONTINFO* pThis,
                            int weight,
                            int bItalic,
                            int charset,
                            int pitch_family,
                            const char* family,
                            int* bExact) {
  auto* pDefault = static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pThis);
  return pDefault->m_pFontInfo->MapFont(weight, !!bItalic, charset,
                                        pitch_family, family);
}

void* DefaultGetFont(struct _FPDF_SYSFONTINFO* pThis, const char* family) {
  auto* pDefault = static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pThis);
  return pDefault->m_pFontInfo->GetFont(family);
}

static unsigned long DefaultGetFontData(struct _FPDF_SYSFONTINFO* pThis,
                                        void* hFont,
                                        unsigned int table,
                                        unsigned char* buffer,
                                        unsigned long buf_size) {
  auto* pDefault = static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pThis);
  return pDefault->m_pFontInfo->GetFontData(hFont, table, buffer, buf_size);
}

static unsigned long DefaultGetFaceName(struct _FPDF_SYSFONTINFO* pThis,
                                        void* hFont,
                                        char* buffer,
                                        unsigned long buf_size) {
  ByteString name;
  auto* pDefault = static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pThis);
  if (!pDefault->m_pFontInfo->GetFaceName(hFont, &name))
    return 0;
  if (name.GetLength() >= static_cast<size_t>(buf_size))
    return name.GetLength() + 1;

  strncpy(buffer, name.c_str(),
          (name.GetLength() + 1) * sizeof(ByteString::CharType));
  return name.GetLength() + 1;
}

static int DefaultGetFontCharset(struct _FPDF_SYSFONTINFO* pThis, void* hFont) {
  int charset;
  auto* pDefault = static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pThis);
  if (!pDefault->m_pFontInfo->GetFontCharset(hFont, &charset))
    return 0;
  return charset;
}

static void DefaultDeleteFont(struct _FPDF_SYSFONTINFO* pThis, void* hFont) {
  auto* pDefault = static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pThis);
  pDefault->m_pFontInfo->DeleteFont(hFont);
}

FPDF_EXPORT FPDF_SYSFONTINFO* FPDF_CALLCONV FPDF_GetDefaultSystemFontInfo() {
  std::unique_ptr<SystemFontInfoIface> pFontInfo =
      SystemFontInfoIface::CreateDefault(nullptr);
  if (!pFontInfo)
    return nullptr;

  FPDF_SYSFONTINFO_DEFAULT* pFontInfoExt =
      FX_Alloc(FPDF_SYSFONTINFO_DEFAULT, 1);
  pFontInfoExt->DeleteFont = DefaultDeleteFont;
  pFontInfoExt->EnumFonts = DefaultEnumFonts;
  pFontInfoExt->GetFaceName = DefaultGetFaceName;
  pFontInfoExt->GetFont = DefaultGetFont;
  pFontInfoExt->GetFontCharset = DefaultGetFontCharset;
  pFontInfoExt->GetFontData = DefaultGetFontData;
  pFontInfoExt->MapFont = DefaultMapFont;
  pFontInfoExt->Release = DefaultRelease;
  pFontInfoExt->version = 1;
  pFontInfoExt->m_pFontInfo = pFontInfo.release();
  return pFontInfoExt;
}

FPDF_EXPORT void FPDF_CALLCONV
FPDF_FreeDefaultSystemFontInfo(FPDF_SYSFONTINFO* pDefaultFontInfo) {
  FX_Free(static_cast<FPDF_SYSFONTINFO_DEFAULT*>(pDefaultFontInfo));
}
