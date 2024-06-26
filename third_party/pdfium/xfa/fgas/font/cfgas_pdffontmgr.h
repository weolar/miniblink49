// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_FONT_CFGAS_PDFFONTMGR_H_
#define XFA_FGAS_FONT_CFGAS_PDFFONTMGR_H_

#include <map>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/observable.h"
#include "core/fxcrt/retain_ptr.h"

class CFGAS_FontMgr;
class CFGAS_GEFont;
class CPDF_Document;

class CFGAS_PDFFontMgr final : public Observable<CFGAS_PDFFontMgr> {
 public:
  explicit CFGAS_PDFFontMgr(CPDF_Document* pDoc, CFGAS_FontMgr* pFontMgr);
  ~CFGAS_PDFFontMgr();

  RetainPtr<CFGAS_GEFont> GetFont(WideStringView wsFontFamily,
                                  uint32_t dwFontStyles,
                                  bool bStrictMatch);

 private:
  RetainPtr<CFGAS_GEFont> FindFont(const ByteString& strFamilyName,
                                   bool bBold,
                                   bool bItalic,
                                   bool bStrictMatch);
  ByteString PsNameToFontName(const ByteString& strPsName,
                              bool bBold,
                              bool bItalic);
  bool PsNameMatchDRFontName(ByteStringView bsPsName,
                             bool bBold,
                             bool bItalic,
                             const ByteString& bsDRFontName,
                             bool bStrictMatch);

  UnownedPtr<CPDF_Document> const m_pDoc;
  UnownedPtr<CFGAS_FontMgr> const m_pFontMgr;
  std::map<ByteString, RetainPtr<CFGAS_GEFont>> m_FontMap;
};

#endif  // XFA_FGAS_FONT_CFGAS_PDFFONTMGR_H_
