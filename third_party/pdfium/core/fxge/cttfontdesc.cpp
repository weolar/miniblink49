// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cttfontdesc.h"

#include <utility>

#include "core/fxge/fx_freetype.h"
#include "third_party/base/stl_util.h"

CTTFontDesc::CTTFontDesc(std::unique_ptr<uint8_t, FxFreeDeleter> pData)
    : m_pFontData(std::move(pData)) {
  for (size_t i = 0; i < FX_ArraySize(m_TTCFaces); i++)
    m_TTCFaces[i] = nullptr;
}

CTTFontDesc::~CTTFontDesc() {
  ASSERT(m_RefCount == 0);
  for (size_t i = 0; i < FX_ArraySize(m_TTCFaces); i++) {
    if (m_TTCFaces[i])
      FXFT_Done_Face(m_TTCFaces[i]);
  }
}

void CTTFontDesc::SetFace(size_t index, FXFT_Face face) {
  ASSERT(index < FX_ArraySize(m_TTCFaces));
  m_TTCFaces[index] = face;
}

void CTTFontDesc::AddRef() {
  ASSERT(m_RefCount > 0);
  ++m_RefCount;
}

CTTFontDesc::ReleaseStatus CTTFontDesc::ReleaseFace(FXFT_Face face) {
  if (!pdfium::ContainsValue(m_TTCFaces, face))
    return kNotAppropriate;

  ASSERT(m_RefCount > 0);
  return --m_RefCount == 0 ? kReleased : kNotReleased;
}

FXFT_Face CTTFontDesc::GetFace(size_t index) const {
  ASSERT(index < FX_ArraySize(m_TTCFaces));
  return m_TTCFaces[index];
}
