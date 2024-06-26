// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_scaledrenderbuffer.h"

#include "core/fpdfapi/render/cpdf_rendercontext.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "third_party/base/ptr_util.h"

namespace {

constexpr size_t kImageSizeLimitBytes = 30 * 1024 * 1024;

}  // namespace

CPDF_ScaledRenderBuffer::CPDF_ScaledRenderBuffer() {}

CPDF_ScaledRenderBuffer::~CPDF_ScaledRenderBuffer() {}

bool CPDF_ScaledRenderBuffer::Initialize(CPDF_RenderContext* pContext,
                                         CFX_RenderDevice* pDevice,
                                         const FX_RECT& pRect,
                                         const CPDF_PageObject* pObj,
                                         const CPDF_RenderOptions* pOptions,
                                         int max_dpi) {
  m_pDevice = pDevice;
  if (m_pDevice->GetDeviceCaps(FXDC_RENDER_CAPS) & FXRC_GET_BITS)
    return true;

  m_pContext = pContext;
  m_Rect = pRect;
  m_pObject = pObj;
  m_Matrix.Translate(-pRect.left, -pRect.top);
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
  m_pBitmapDevice = pdfium::MakeUnique<CFX_DefaultRenderDevice>();
  bool bIsAlpha =
      !!(m_pDevice->GetDeviceCaps(FXDC_RENDER_CAPS) & FXRC_ALPHA_OUTPUT);
  FXDIB_Format dibFormat = bIsAlpha ? FXDIB_Argb : FXDIB_Rgb;
  while (1) {
    FX_RECT bitmap_rect =
        m_Matrix.TransformRect(CFX_FloatRect(pRect)).GetOuterRect();
    int32_t width = bitmap_rect.Width();
    int32_t height = bitmap_rect.Height();
    // Set to 0 to make CalculatePitchAndSize() calculate it.
    uint32_t pitch = 0;
    uint32_t size;
    if (!CFX_DIBitmap::CalculatePitchAndSize(width, height, dibFormat, &pitch,
                                             &size)) {
      return false;
    }

    if (size <= kImageSizeLimitBytes &&
        m_pBitmapDevice->Create(width, height, dibFormat, nullptr)) {
      break;
    }
    m_Matrix.Scale(0.5f, 0.5f);
  }
  m_pContext->GetBackground(m_pBitmapDevice->GetBitmap(), m_pObject.Get(),
                            pOptions, &m_Matrix);
  return true;
}

CFX_RenderDevice* CPDF_ScaledRenderBuffer::GetDevice() const {
  return m_pBitmapDevice ? m_pBitmapDevice.get() : m_pDevice.Get();
}

void CPDF_ScaledRenderBuffer::OutputToDevice() {
  if (m_pBitmapDevice) {
    m_pDevice->StretchDIBits(m_pBitmapDevice->GetBitmap(), m_Rect.left,
                             m_Rect.top, m_Rect.Width(), m_Rect.Height());
  }
}
