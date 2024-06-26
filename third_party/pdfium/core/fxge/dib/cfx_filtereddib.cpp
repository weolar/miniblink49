// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/dib/cfx_filtereddib.h"

#include "core/fxge/fx_dib.h"

CFX_FilteredDIB::CFX_FilteredDIB() {}

CFX_FilteredDIB::~CFX_FilteredDIB() {}

void CFX_FilteredDIB::LoadSrc(const RetainPtr<CFX_DIBBase>& pSrc) {
  m_pSrc = pSrc;
  m_Width = pSrc->GetWidth();
  m_Height = pSrc->GetHeight();
  FXDIB_Format format = GetDestFormat();
  m_bpp = GetBppFromFormat(format);
  m_AlphaFlag = static_cast<uint8_t>(format >> 8);
  m_Pitch = (m_Width * m_bpp + 31) / 32 * 4;
  m_pPalette.reset(GetDestPalette());
  m_Scanline.resize(m_Pitch);
}

const uint8_t* CFX_FilteredDIB::GetScanline(int line) const {
  TranslateScanline(m_pSrc->GetScanline(line), &m_Scanline);
  return m_Scanline.data();
}

void CFX_FilteredDIB::DownSampleScanline(int line,
                                         uint8_t* dest_scan,
                                         int dest_bpp,
                                         int dest_width,
                                         bool bFlipX,
                                         int clip_left,
                                         int clip_width) const {
  m_pSrc->DownSampleScanline(line, dest_scan, dest_bpp, dest_width, bFlipX,
                             clip_left, clip_width);
  TranslateDownSamples(dest_scan, dest_scan, clip_width, dest_bpp);
}
