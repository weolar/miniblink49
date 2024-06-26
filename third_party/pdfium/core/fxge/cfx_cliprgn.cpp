// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_cliprgn.h"

#include <utility>

#include "core/fxge/dib/cfx_dibitmap.h"
#include "third_party/base/logging.h"

CFX_ClipRgn::CFX_ClipRgn(int width, int height)
    : m_Type(RectI), m_Box(0, 0, width, height) {}

CFX_ClipRgn::CFX_ClipRgn(const CFX_ClipRgn& src) = default;

CFX_ClipRgn::~CFX_ClipRgn() = default;

void CFX_ClipRgn::IntersectRect(const FX_RECT& rect) {
  if (m_Type == RectI) {
    m_Box.Intersect(rect);
    return;
  }
  if (m_Type == MaskF) {
    IntersectMaskRect(rect, m_Box, m_Mask);
    return;
  }
}

void CFX_ClipRgn::IntersectMaskRect(FX_RECT rect,
                                    FX_RECT mask_rect,
                                    const RetainPtr<CFX_DIBitmap>& pMask) {
  m_Type = MaskF;
  m_Box = rect;
  m_Box.Intersect(mask_rect);
  if (m_Box.IsEmpty()) {
    m_Type = RectI;
    return;
  }
  if (m_Box == mask_rect) {
    m_Mask = pMask;
    return;
  }
  RetainPtr<CFX_DIBitmap> pOldMask(pMask);
  m_Mask = pdfium::MakeRetain<CFX_DIBitmap>();
  m_Mask->Create(m_Box.Width(), m_Box.Height(), FXDIB_8bppMask);
  for (int row = m_Box.top; row < m_Box.bottom; row++) {
    uint8_t* dest_scan =
        m_Mask->GetBuffer() + m_Mask->GetPitch() * (row - m_Box.top);
    uint8_t* src_scan =
        pOldMask->GetBuffer() + pOldMask->GetPitch() * (row - mask_rect.top);
    for (int col = m_Box.left; col < m_Box.right; col++)
      dest_scan[col - m_Box.left] = src_scan[col - mask_rect.left];
  }
}

void CFX_ClipRgn::IntersectMaskF(int left,
                                 int top,
                                 const RetainPtr<CFX_DIBitmap>& pMask) {
  ASSERT(pMask->GetFormat() == FXDIB_8bppMask);
  FX_RECT mask_box(left, top, left + pMask->GetWidth(),
                   top + pMask->GetHeight());
  if (m_Type == RectI) {
    IntersectMaskRect(m_Box, mask_box, pMask);
    return;
  }
  if (m_Type == MaskF) {
    FX_RECT new_box = m_Box;
    new_box.Intersect(mask_box);
    if (new_box.IsEmpty()) {
      m_Type = RectI;
      m_Mask = nullptr;
      m_Box = new_box;
      return;
    }
    auto new_dib = pdfium::MakeRetain<CFX_DIBitmap>();
    new_dib->Create(new_box.Width(), new_box.Height(), FXDIB_8bppMask);
    for (int row = new_box.top; row < new_box.bottom; row++) {
      uint8_t* old_scan =
          m_Mask->GetBuffer() + (row - m_Box.top) * m_Mask->GetPitch();
      uint8_t* mask_scan = pMask->GetBuffer() + (row - top) * pMask->GetPitch();
      uint8_t* new_scan =
          new_dib->GetBuffer() + (row - new_box.top) * new_dib->GetPitch();
      for (int col = new_box.left; col < new_box.right; col++) {
        new_scan[col - new_box.left] =
            old_scan[col - m_Box.left] * mask_scan[col - left] / 255;
      }
    }
    m_Box = new_box;
    m_Mask = std::move(new_dib);
    return;
  }
  NOTREACHED();
}
