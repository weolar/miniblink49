// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_DEFAULTRENDERDEVICE_H_
#define CORE_FXGE_CFX_DEFAULTRENDERDEVICE_H_

#include "core/fxge/cfx_renderdevice.h"
#include "core/fxge/fx_dib.h"

class SkPictureRecorder;

class CFX_DefaultRenderDevice final : public CFX_RenderDevice {
 public:
  CFX_DefaultRenderDevice();
  ~CFX_DefaultRenderDevice() override;

  bool Attach(const RetainPtr<CFX_DIBitmap>& pBitmap,
              bool bRgbByteOrder,
              const RetainPtr<CFX_DIBitmap>& pBackdropBitmap,
              bool bGroupKnockout);
  bool Create(int width,
              int height,
              FXDIB_Format format,
              const RetainPtr<CFX_DIBitmap>& pBackdropBitmap);

#ifdef _SKIA_SUPPORT_
  bool AttachRecorder(SkPictureRecorder* recorder);
  void Clear(uint32_t color);
  SkPictureRecorder* CreateRecorder(int size_x, int size_y);
  void DebugVerifyBitmapIsPreMultiplied() const override;
  bool SetBitsWithMask(const RetainPtr<CFX_DIBBase>& pBitmap,
                       const RetainPtr<CFX_DIBBase>& pMask,
                       int left,
                       int top,
                       int bitmap_alpha,
                       BlendMode blend_type) override;
#endif
};

#endif  // CORE_FXGE_CFX_DEFAULTRENDERDEVICE_H_
