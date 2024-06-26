// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CBA_FONTMAP_H_
#define FPDFSDK_FORMFILLER_CBA_FONTMAP_H_

#include <memory>
#include <vector>

#include "core/fpdfdoc/ipvt_fontmap.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/unowned_ptr.h"
#include "public/fpdf_sysfontinfo.h"

class CFX_SystemHandler;
class CPDFSDK_Annot;
class CPDF_Dictionary;
class CPDF_Document;

class CBA_FontMap final : public IPVT_FontMap {
 public:
  CBA_FontMap(CPDFSDK_Annot* pAnnot, CFX_SystemHandler* pSystemHandler);
  ~CBA_FontMap() override;

  // IPVT_FontMap
  CPDF_Font* GetPDFFont(int32_t nFontIndex) override;
  ByteString GetPDFFontAlias(int32_t nFontIndex) override;
  int32_t GetWordFontIndex(uint16_t word,
                           int32_t nCharset,
                           int32_t nFontIndex) override;
  int32_t CharCodeFromUnicode(int32_t nFontIndex, uint16_t word) override;
  int32_t CharSetFromUnicode(uint16_t word, int32_t nOldCharset) override;

  static int32_t GetNativeCharset();

  void Reset();
  void SetDefaultFont(CPDF_Font* pFont, const ByteString& sFontName);
  void SetAPType(const ByteString& sAPType);

 private:
  struct Data {
    CPDF_Font* pFont;
    int32_t nCharset;
    ByteString sFontName;
  };

  struct Native {
    int32_t nCharset;
    ByteString sFontName;
  };

  void Initialize();
  CPDF_Font* FindFontSameCharset(ByteString* sFontAlias, int32_t nCharset);

  CPDF_Font* FindResFontSameCharset(const CPDF_Dictionary* pResDict,
                                    ByteString* sFontAlias,
                                    int32_t nCharset);
  CPDF_Font* GetAnnotDefaultFont(ByteString* csNameTag);
  void AddFontToAnnotDict(CPDF_Font* pFont, const ByteString& sAlias);

  bool KnowWord(int32_t nFontIndex, uint16_t word);

  void Empty();
  int32_t GetFontIndex(const ByteString& sFontName,
                       int32_t nCharset,
                       bool bFind);
  int32_t AddFontData(CPDF_Font* pFont,
                      const ByteString& sFontAlias,
                      int32_t nCharset);

  ByteString EncodeFontAlias(const ByteString& sFontName, int32_t nCharset);
  ByteString EncodeFontAlias(const ByteString& sFontName);

  int32_t FindFont(const ByteString& sFontName, int32_t nCharset);
  ByteString GetNativeFont(int32_t nCharset);
  ByteString GetNativeFontName(int32_t nCharset);
  CPDF_Font* AddFontToDocument(CPDF_Document* pDoc,
                               ByteString& sFontName,
                               uint8_t nCharset);
  bool IsStandardFont(const ByteString& sFontName);
  CPDF_Font* AddStandardFont(CPDF_Document* pDoc, ByteString& sFontName);
  CPDF_Font* AddSystemFont(CPDF_Document* pDoc,
                           ByteString& sFontName,
                           uint8_t nCharset);

  std::vector<std::unique_ptr<Data>> m_Data;
  std::vector<std::unique_ptr<Native>> m_NativeFont;
  UnownedPtr<CFX_SystemHandler> const m_pSystemHandler;
  UnownedPtr<CPDF_Document> const m_pDocument;
  UnownedPtr<CPDF_Dictionary> const m_pAnnotDict;
  UnownedPtr<CPDF_Font> m_pDefaultFont;
  ByteString m_sDefaultFontName;
  ByteString m_sAPType = "N";
};

#endif  // FPDFSDK_FORMFILLER_CBA_FONTMAP_H_
