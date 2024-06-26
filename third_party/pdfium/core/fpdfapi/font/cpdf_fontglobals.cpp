// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/font/cpdf_fontglobals.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_document.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

CPDF_FontGlobals::CPDF_FontGlobals() {
  memset(m_EmbeddedCharsets, 0, sizeof(m_EmbeddedCharsets));
  memset(m_EmbeddedToUnicodes, 0, sizeof(m_EmbeddedToUnicodes));
}

CPDF_FontGlobals::~CPDF_FontGlobals() {}

CPDF_Font* CPDF_FontGlobals::Find(CPDF_Document* pDoc, uint32_t index) {
  auto it = m_StockMap.find(pDoc);
  if (it == m_StockMap.end())
    return nullptr;
  return it->second ? it->second->GetFont(index) : nullptr;
}

CPDF_Font* CPDF_FontGlobals::Set(CPDF_Document* pDoc,
                                 uint32_t index,
                                 std::unique_ptr<CPDF_Font> pFont) {
  if (!pdfium::ContainsKey(m_StockMap, pDoc))
    m_StockMap[pDoc] = pdfium::MakeUnique<CFX_StockFontArray>();
  return m_StockMap[pDoc]->SetFont(index, std::move(pFont));
}

void CPDF_FontGlobals::Clear(CPDF_Document* pDoc) {
  m_StockMap.erase(pDoc);
}
