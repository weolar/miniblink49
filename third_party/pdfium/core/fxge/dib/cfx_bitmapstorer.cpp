// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/dib/cfx_bitmapstorer.h"

#include <utility>

#include "core/fxge/dib/cfx_dibitmap.h"

CFX_BitmapStorer::CFX_BitmapStorer() {}

CFX_BitmapStorer::~CFX_BitmapStorer() {}

RetainPtr<CFX_DIBitmap> CFX_BitmapStorer::Detach() {
  return std::move(m_pBitmap);
}

void CFX_BitmapStorer::Replace(RetainPtr<CFX_DIBitmap>&& pBitmap) {
  m_pBitmap = std::move(pBitmap);
}

void CFX_BitmapStorer::ComposeScanline(int line,
                                       const uint8_t* scanline,
                                       const uint8_t* scan_extra_alpha) {
  uint8_t* dest_buf = m_pBitmap->GetWritableScanline(line);
  uint8_t* dest_alpha_buf =
      m_pBitmap->m_pAlphaMask
          ? m_pBitmap->m_pAlphaMask->GetWritableScanline(line)
          : nullptr;
  if (dest_buf)
    memcpy(dest_buf, scanline, m_pBitmap->GetPitch());

  if (dest_alpha_buf) {
    memcpy(dest_alpha_buf, scan_extra_alpha,
           m_pBitmap->m_pAlphaMask->GetPitch());
  }
}

bool CFX_BitmapStorer::SetInfo(int width,
                               int height,
                               FXDIB_Format src_format,
                               uint32_t* pSrcPalette) {
  auto pBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pBitmap->Create(width, height, src_format))
    return false;

  if (pSrcPalette)
    pBitmap->SetPalette(pSrcPalette);

  m_pBitmap = std::move(pBitmap);
  return true;
}
