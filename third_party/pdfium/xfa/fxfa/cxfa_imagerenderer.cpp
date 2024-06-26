// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_imagerenderer.h"

#include "core/fxge/cfx_renderdevice.h"
#include "core/fxge/dib/cfx_dibbase.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/dib/cfx_imagerenderer.h"
#include "core/fxge/dib/cfx_imagetransformer.h"
#include "third_party/base/ptr_util.h"

CXFA_ImageRenderer::CXFA_ImageRenderer(CFX_RenderDevice* pDevice,
                                       const RetainPtr<CFX_DIBBase>& pDIBBase,
                                       const CFX_Matrix* pImage2Device)
    : m_pDevice(pDevice), m_ImageMatrix(*pImage2Device), m_pDIBBase(pDIBBase) {}

CXFA_ImageRenderer::~CXFA_ImageRenderer() {}

bool CXFA_ImageRenderer::Start() {
  if (m_pDevice->StartDIBitsWithBlend(m_pDIBBase, 255, 0, m_ImageMatrix,
                                      kBilinearInterpolation, &m_DeviceHandle,
                                      BlendMode::kNormal)) {
    if (m_DeviceHandle) {
      m_Status = 3;
      return true;
    }
    return false;
  }
  CFX_FloatRect image_rect_f = m_ImageMatrix.GetUnitRect();
  FX_RECT image_rect = image_rect_f.GetOuterRect();
  int dest_width = image_rect.Width();
  int dest_height = image_rect.Height();
  if ((fabs(m_ImageMatrix.b) >= 0.5f || m_ImageMatrix.a == 0) ||
      (fabs(m_ImageMatrix.c) >= 0.5f || m_ImageMatrix.d == 0)) {
    RetainPtr<CFX_DIBBase> pDib = m_pDIBBase;
    if (m_pDIBBase->HasAlpha() &&
        !(m_pDevice->GetRenderCaps() & FXRC_ALPHA_IMAGE) &&
        !(m_pDevice->GetRenderCaps() & FXRC_GET_BITS)) {
      m_pCloneConvert = m_pDIBBase->CloneConvert(FXDIB_Rgb);
      if (!m_pCloneConvert)
        return false;

      pDib = m_pCloneConvert;
    }
    FX_RECT clip_box = m_pDevice->GetClipBox();
    clip_box.Intersect(image_rect);
    m_Status = 2;
    m_pTransformer = pdfium::MakeUnique<CFX_ImageTransformer>(
        pDib, m_ImageMatrix, kBilinearInterpolation, &clip_box);
    return true;
  }
  if (m_ImageMatrix.a < 0)
    dest_width = -dest_width;
  if (m_ImageMatrix.d > 0)
    dest_height = -dest_height;
  int dest_left, dest_top;
  dest_left = dest_width > 0 ? image_rect.left : image_rect.right;
  dest_top = dest_height > 0 ? image_rect.top : image_rect.bottom;
  if (m_pDIBBase->IsOpaqueImage()) {
    if (m_pDevice->StretchDIBitsWithFlagsAndBlend(
            m_pDIBBase, dest_left, dest_top, dest_width, dest_height,
            kBilinearInterpolation, BlendMode::kNormal)) {
      return false;
    }
  }
  if (m_pDIBBase->IsAlphaMask()) {
    if (m_pDevice->StretchBitMaskWithFlags(m_pDIBBase, dest_left, dest_top,
                                           dest_width, dest_height, 0,
                                           kBilinearInterpolation)) {
      return false;
    }
  }

  FX_RECT clip_box = m_pDevice->GetClipBox();
  FX_RECT dest_rect = clip_box;
  dest_rect.Intersect(image_rect);
  FX_RECT dest_clip(
      dest_rect.left - image_rect.left, dest_rect.top - image_rect.top,
      dest_rect.right - image_rect.left, dest_rect.bottom - image_rect.top);
  RetainPtr<CFX_DIBitmap> pStretched = m_pDIBBase->StretchTo(
      dest_width, dest_height, kBilinearInterpolation, &dest_clip);
  if (pStretched)
    CompositeDIBitmap(pStretched, dest_rect.left, dest_rect.top);

  return false;
}

bool CXFA_ImageRenderer::Continue() {
  if (m_Status == 2) {
    if (m_pTransformer->Continue(nullptr))
      return true;

    RetainPtr<CFX_DIBitmap> pBitmap = m_pTransformer->DetachBitmap();
    if (!pBitmap)
      return false;

    if (pBitmap->IsAlphaMask()) {
      m_pDevice->SetBitMask(pBitmap, m_pTransformer->result().left,
                            m_pTransformer->result().top, 0);
    } else {
      m_pDevice->SetDIBitsWithBlend(pBitmap, m_pTransformer->result().left,
                                    m_pTransformer->result().top,
                                    BlendMode::kNormal);
    }
    return false;
  }
  if (m_Status == 3)
    return m_pDevice->ContinueDIBits(m_DeviceHandle.get(), nullptr);

  return false;
}

void CXFA_ImageRenderer::CompositeDIBitmap(
    const RetainPtr<CFX_DIBitmap>& pDIBitmap,
    int left,
    int top) {
  if (!pDIBitmap)
    return;

  if (!pDIBitmap->IsAlphaMask()) {
    if (m_pDevice->SetDIBits(pDIBitmap, left, top))
      return;
  } else if (m_pDevice->SetBitMask(pDIBitmap, left, top, 0)) {
    return;
  }

  bool bGetBackGround = ((m_pDevice->GetRenderCaps() & FXRC_ALPHA_OUTPUT)) ||
                        (!(m_pDevice->GetRenderCaps() & FXRC_ALPHA_OUTPUT) &&
                         (m_pDevice->GetRenderCaps() & FXRC_GET_BITS));
  if (bGetBackGround) {
    if (pDIBitmap->IsAlphaMask())
      return;

    m_pDevice->SetDIBitsWithBlend(pDIBitmap, left, top, BlendMode::kNormal);
    return;
  }
  if (!pDIBitmap->HasAlpha() ||
      (m_pDevice->GetRenderCaps() & FXRC_ALPHA_IMAGE)) {
    return;
  }

  RetainPtr<CFX_DIBitmap> pCloneConvert = pDIBitmap->CloneConvert(FXDIB_Rgb);
  if (!pCloneConvert)
    return;

  CXFA_ImageRenderer imageRender(m_pDevice.Get(), pCloneConvert,
                                 &m_ImageMatrix);
  if (!imageRender.Start())
    return;

  while (imageRender.Continue())
    continue;
}
