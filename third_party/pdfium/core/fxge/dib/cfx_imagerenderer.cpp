// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/dib/cfx_imagerenderer.h"

#include <memory>

#include "core/fxge/cfx_cliprgn.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/dib/cfx_imagestretcher.h"
#include "core/fxge/dib/cfx_imagetransformer.h"
#include "third_party/base/ptr_util.h"

CFX_ImageRenderer::CFX_ImageRenderer(const RetainPtr<CFX_DIBitmap>& pDevice,
                                     const CFX_ClipRgn* pClipRgn,
                                     const RetainPtr<CFX_DIBBase>& pSource,
                                     int bitmap_alpha,
                                     uint32_t mask_color,
                                     const CFX_Matrix& matrix,
                                     const FXDIB_ResampleOptions& options,
                                     bool bRgbByteOrder)
    : m_pDevice(pDevice),
      m_pClipRgn(pClipRgn),
      m_Matrix(matrix),
      m_BitmapAlpha(bitmap_alpha),
      m_MaskColor(mask_color),
      m_bRgbByteOrder(bRgbByteOrder) {
  FX_RECT image_rect = m_Matrix.GetUnitRect().GetOuterRect();
  m_ClipBox = pClipRgn ? pClipRgn->GetBox() : FX_RECT(0, 0, pDevice->GetWidth(),
                                                      pDevice->GetHeight());
  m_ClipBox.Intersect(image_rect);
  if (m_ClipBox.IsEmpty())
    return;

  if ((fabs(m_Matrix.b) >= 0.5f || m_Matrix.a == 0) ||
      (fabs(m_Matrix.c) >= 0.5f || m_Matrix.d == 0)) {
    if (fabs(m_Matrix.a) < fabs(m_Matrix.b) / 20 &&
        fabs(m_Matrix.d) < fabs(m_Matrix.c) / 20 && fabs(m_Matrix.a) < 0.5f &&
        fabs(m_Matrix.d) < 0.5f) {
      int dest_width = image_rect.Width();
      int dest_height = image_rect.Height();
      FX_RECT bitmap_clip = m_ClipBox;
      bitmap_clip.Offset(-image_rect.left, -image_rect.top);
      bitmap_clip = FXDIB_SwapClipBox(bitmap_clip, dest_width, dest_height,
                                      m_Matrix.c > 0, m_Matrix.b < 0);
      m_Composer.Compose(pDevice, pClipRgn, bitmap_alpha, mask_color, m_ClipBox,
                         true, m_Matrix.c > 0, m_Matrix.b < 0, m_bRgbByteOrder,
                         0, BlendMode::kNormal);
      m_Stretcher = pdfium::MakeUnique<CFX_ImageStretcher>(
          &m_Composer, pSource, dest_height, dest_width, bitmap_clip, options);
      if (m_Stretcher->Start())
        m_Status = 1;
      return;
    }
    m_Status = 2;
    m_pTransformer = pdfium::MakeUnique<CFX_ImageTransformer>(
        pSource, m_Matrix, options, &m_ClipBox);
    return;
  }

  int dest_width = image_rect.Width();
  if (m_Matrix.a < 0)
    dest_width = -dest_width;

  int dest_height = image_rect.Height();
  if (m_Matrix.d > 0)
    dest_height = -dest_height;

  if (dest_width == 0 || dest_height == 0)
    return;

  FX_RECT bitmap_clip = m_ClipBox;
  bitmap_clip.Offset(-image_rect.left, -image_rect.top);
  m_Composer.Compose(pDevice, pClipRgn, bitmap_alpha, mask_color, m_ClipBox,
                     false, false, false, m_bRgbByteOrder, 0,
                     BlendMode::kNormal);
  m_Status = 1;
  m_Stretcher = pdfium::MakeUnique<CFX_ImageStretcher>(
      &m_Composer, pSource, dest_width, dest_height, bitmap_clip, options);
  m_Stretcher->Start();
}

CFX_ImageRenderer::~CFX_ImageRenderer() {}

bool CFX_ImageRenderer::Continue(PauseIndicatorIface* pPause) {
  if (m_Status == 1)
    return m_Stretcher->Continue(pPause);
  if (m_Status != 2)
    return false;
  if (m_pTransformer->Continue(pPause))
    return true;

  RetainPtr<CFX_DIBitmap> pBitmap = m_pTransformer->DetachBitmap();
  if (!pBitmap || !pBitmap->GetBuffer())
    return false;

  if (pBitmap->IsAlphaMask()) {
    if (m_BitmapAlpha != 255) {
      if (m_AlphaFlag >> 8) {
        m_AlphaFlag = (((uint8_t)((m_AlphaFlag & 0xff) * m_BitmapAlpha / 255)) |
                       ((m_AlphaFlag >> 8) << 8));
      } else {
        m_MaskColor = FXARGB_MUL_ALPHA(m_MaskColor, m_BitmapAlpha);
      }
    }
    m_pDevice->CompositeMask(
        m_pTransformer->result().left, m_pTransformer->result().top,
        pBitmap->GetWidth(), pBitmap->GetHeight(), pBitmap, m_MaskColor, 0, 0,
        BlendMode::kNormal, m_pClipRgn.Get(), m_bRgbByteOrder, m_AlphaFlag);
  } else {
    if (m_BitmapAlpha != 255)
      pBitmap->MultiplyAlpha(m_BitmapAlpha);
    m_pDevice->CompositeBitmap(
        m_pTransformer->result().left, m_pTransformer->result().top,
        pBitmap->GetWidth(), pBitmap->GetHeight(), pBitmap, 0, 0,
        BlendMode::kNormal, m_pClipRgn.Get(), m_bRgbByteOrder);
  }
  return false;
}
