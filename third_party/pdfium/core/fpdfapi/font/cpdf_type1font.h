// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FONT_CPDF_TYPE1FONT_H_
#define CORE_FPDFAPI_FONT_CPDF_TYPE1FONT_H_

#include "core/fpdfapi/font/cpdf_simplefont.h"
#include "core/fxcrt/fx_system.h"

class CPDF_Type1Font final : public CPDF_SimpleFont {
 public:
  CPDF_Type1Font(CPDF_Document* pDocument, CPDF_Dictionary* pFontDict);
  ~CPDF_Type1Font() override;

  // CPDF_Font:
  bool IsType1Font() const override;
  const CPDF_Type1Font* AsType1Font() const override;
  CPDF_Type1Font* AsType1Font() override;
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  int GlyphFromCharCodeExt(uint32_t charcode) override;
#endif

  bool IsBase14Font() const { return m_Base14Font >= 0; }

 private:
  // CPDF_Font:
  bool Load() override;

  // CPDF_SimpleFont:
  void LoadGlyphMap() override;

#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  void SetExtGID(const char* name, uint32_t charcode);
  void CalcExtGID(uint32_t charcode);

  uint16_t m_ExtGID[256];
#endif

  int m_Base14Font = -1;
};

#endif  // CORE_FPDFAPI_FONT_CPDF_TYPE1FONT_H_
