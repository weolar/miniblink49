// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/dib/cfx_dibextractor.h"

#include "core/fxge/dib/cfx_dibbase.h"
#include "core/fxge/dib/cfx_dibitmap.h"

CFX_DIBExtractor::CFX_DIBExtractor(const RetainPtr<CFX_DIBBase>& pSrc) {
  if (!pSrc->GetBuffer()) {
    m_pBitmap = pSrc->Clone(nullptr);
    return;
  }
  RetainPtr<CFX_DIBBase> pOldSrc(pSrc);
  m_pBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!m_pBitmap->Create(pOldSrc->GetWidth(), pOldSrc->GetHeight(),
                         pOldSrc->GetFormat(), pOldSrc->GetBuffer(), 0)) {
    m_pBitmap.Reset();
    return;
  }
  m_pBitmap->SetPalette(pOldSrc->GetPalette());
  m_pBitmap->SetAlphaMask(pOldSrc->m_pAlphaMask, nullptr);
}

CFX_DIBExtractor::~CFX_DIBExtractor() {}
