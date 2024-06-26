// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/renderdevicedriver_iface.h"

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/dib/cfx_dibitmap.h"

RenderDeviceDriverIface::~RenderDeviceDriverIface() {}

bool RenderDeviceDriverIface::StartRendering() {
  return true;
}

void RenderDeviceDriverIface::EndRendering() {}

bool RenderDeviceDriverIface::SetClip_PathStroke(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState) {
  return false;
}

bool RenderDeviceDriverIface::SetPixel(int x, int y, uint32_t color) {
  return false;
}

bool RenderDeviceDriverIface::FillRectWithBlend(const FX_RECT& rect,
                                                uint32_t fill_color,
                                                BlendMode blend_type) {
  return false;
}

bool RenderDeviceDriverIface::DrawCosmeticLine(const CFX_PointF& ptMoveTo,
                                               const CFX_PointF& ptLineTo,
                                               uint32_t color,
                                               BlendMode blend_type) {
  return false;
}

bool RenderDeviceDriverIface::GetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                                        int left,
                                        int top) {
  return false;
}

RetainPtr<CFX_DIBitmap> RenderDeviceDriverIface::GetBackDrop() {
  return RetainPtr<CFX_DIBitmap>();
}

bool RenderDeviceDriverIface::ContinueDIBits(CFX_ImageRenderer* handle,
                                             PauseIndicatorIface* pPause) {
  return false;
}

bool RenderDeviceDriverIface::DrawDeviceText(int nChars,
                                             const FXTEXT_CHARPOS* pCharPos,
                                             CFX_Font* pFont,
                                             const CFX_Matrix* pObject2Device,
                                             float font_size,
                                             uint32_t color) {
  return false;
}

int RenderDeviceDriverIface::GetDriverType() const {
  return 0;
}

void RenderDeviceDriverIface::ClearDriver() {}

bool RenderDeviceDriverIface::DrawShading(const CPDF_ShadingPattern* pPattern,
                                          const CFX_Matrix* pMatrix,
                                          const FX_RECT& clip_rect,
                                          int alpha,
                                          bool bAlphaMode) {
  return false;
}

bool RenderDeviceDriverIface::SetBitsWithMask(
    const RetainPtr<CFX_DIBBase>& pBitmap,
    const RetainPtr<CFX_DIBBase>& pMask,
    int left,
    int top,
    int bitmap_alpha,
    BlendMode blend_type) {
  return false;
}

#if defined _SKIA_SUPPORT_ || _SKIA_SUPPORT_PATHS_
void RenderDeviceDriverIface::Flush() {}
#endif
