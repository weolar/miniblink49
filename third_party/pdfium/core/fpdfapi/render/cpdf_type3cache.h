// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_TYPE3CACHE_H_
#define CORE_FPDFAPI_RENDER_CPDF_TYPE3CACHE_H_

#include <map>
#include <memory>

#include "core/fpdfapi/font/cpdf_type3font.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"

class CPDF_Type3Glyphs;

class CPDF_Type3Cache final : public Retainable {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  CFX_GlyphBitmap* LoadGlyph(uint32_t charcode, const CFX_Matrix* pMatrix);

 private:
  explicit CPDF_Type3Cache(CPDF_Type3Font* pFont);
  ~CPDF_Type3Cache() override;

  std::unique_ptr<CFX_GlyphBitmap> RenderGlyph(CPDF_Type3Glyphs* pSize,
                                               uint32_t charcode,
                                               const CFX_Matrix* pMatrix);

  UnownedPtr<CPDF_Type3Font> const m_pFont;
  std::map<ByteString, std::unique_ptr<CPDF_Type3Glyphs>> m_SizeMap;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_TYPE3CACHE_H_
