// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_devicebuffer.h"

#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/render/cpdf_rendercontext.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/cfx_renderdevice.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/fx_dib.h"

CPDF_DeviceBuffer::CPDF_DeviceBuffer() {}

CPDF_DeviceBuffer::~CPDF_DeviceBuffer() {}

bool CPDF_DeviceBuffer::Initialize(CPDF_RenderContext* pContext,
                                   CFX_RenderDevice* pDevice,
                                   const FX_RECT& rect,
                                   const CPDF_PageObject* pObj,
                                   int max_dpi) {
  m_pDevice = pDevice;
  m_pContext = pContext;
  m_Rect = rect;
  m_pObject = pObj;
  m_Matrix.Translate(-rect.left, -rect.top);
#if _FX_PLATFORM_ != _FX_PLATFORM_APPLE_
  int horz_size = pDevice->GetDeviceCaps(FXDC_HORZ_SIZE);
  int vert_size = pDevice->GetDeviceCaps(FXDC_VERT_SIZE);
  if (horz_size && vert_size && max_dpi) {
    int dpih =
        pDevice->GetDeviceCaps(FXDC_PIXEL_WIDTH) * 254 / (horz_size * 10);
    int dpiv =
        pDevice->GetDeviceCaps(FXDC_PIXEL_HEIGHT) * 254 / (vert_size * 10);
    if (dpih > max_dpi)
      m_Matrix.Scale((float)(max_dpi) / dpih, 1.0f);
    if (dpiv > max_dpi)
      m_Matrix.Scale(1.0f, (float)(max_dpi) / (float)dpiv);
  }
#endif
  FX_RECT bitmap_rect =
      m_Matrix.TransformRect(CFX_FloatRect(rect)).GetOuterRect();
  m_pBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  m_pBitmap->Create(bitmap_rect.Width(), bitmap_rect.Height(), FXDIB_Argb);
  return true;
}

void CPDF_DeviceBuffer::OutputToDevice() {
  if (m_pDevice->GetDeviceCaps(FXDC_RENDER_CAPS) & FXRC_GET_BITS) {
    if (m_Matrix.a == 1.0f && m_Matrix.d == 1.0f) {
      m_pDevice->SetDIBits(m_pBitmap, m_Rect.left, m_Rect.top);
    } else {
      m_pDevice->StretchDIBits(m_pBitmap, m_Rect.left, m_Rect.top,
                               m_Rect.Width(), m_Rect.Height());
    }
    return;
  }
  auto pBuffer = pdfium::MakeRetain<CFX_DIBitmap>();
  m_pDevice->CreateCompatibleBitmap(pBuffer, m_pBitmap->GetWidth(),
                                    m_pBitmap->GetHeight());
  m_pContext->GetBackground(pBuffer, m_pObject.Get(), nullptr, &m_Matrix);
  pBuffer->CompositeBitmap(0, 0, pBuffer->GetWidth(), pBuffer->GetHeight(),
                           m_pBitmap, 0, 0, BlendMode::kNormal, nullptr, false);
  m_pDevice->StretchDIBits(pBuffer, m_Rect.left, m_Rect.top, m_Rect.Width(),
                           m_Rect.Height());
}
