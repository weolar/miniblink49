// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/font/cfx_stockfontarray.h"

#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fxcrt/fx_memory.h"

CFX_StockFontArray::CFX_StockFontArray() = default;

CFX_StockFontArray::~CFX_StockFontArray() {
  for (size_t i = 0; i < FX_ArraySize(m_StockFonts); ++i) {
    if (m_StockFonts[i]) {
      std::unique_ptr<CPDF_Dictionary> destroy(m_StockFonts[i]->GetFontDict());
      m_StockFonts[i]->ClearFontDict();
    }
  }
}

CPDF_Font* CFX_StockFontArray::GetFont(uint32_t index) const {
  if (index >= FX_ArraySize(m_StockFonts))
    return nullptr;
  return m_StockFonts[index].get();
}

CPDF_Font* CFX_StockFontArray::SetFont(uint32_t index,
                                       std::unique_ptr<CPDF_Font> pFont) {
  CPDF_Font* result = pFont.get();
  if (index < FX_ArraySize(m_StockFonts))
    m_StockFonts[index] = std::move(pFont);
  return result;
}
