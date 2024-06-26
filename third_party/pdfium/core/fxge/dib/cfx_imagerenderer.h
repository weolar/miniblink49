// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_DIB_CFX_IMAGERENDERER_H_
#define CORE_FXGE_DIB_CFX_IMAGERENDERER_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/dib/cfx_bitmapcomposer.h"

class CFX_DIBBase;
class CFX_DIBitmap;
class CFX_ImageTransformer;
class CFX_ImageStretcher;
class PauseIndicatorIface;

class CFX_ImageRenderer {
 public:
  CFX_ImageRenderer(const RetainPtr<CFX_DIBitmap>& pDevice,
                    const CFX_ClipRgn* pClipRgn,
                    const RetainPtr<CFX_DIBBase>& pSource,
                    int bitmap_alpha,
                    uint32_t mask_color,
                    const CFX_Matrix& matrix,
                    const FXDIB_ResampleOptions& options,
                    bool bRgbByteOrder);
  ~CFX_ImageRenderer();

  bool Continue(PauseIndicatorIface* pPause);

 private:
  RetainPtr<CFX_DIBitmap> const m_pDevice;
  UnownedPtr<const CFX_ClipRgn> const m_pClipRgn;
  const CFX_Matrix m_Matrix;
  std::unique_ptr<CFX_ImageTransformer> m_pTransformer;
  std::unique_ptr<CFX_ImageStretcher> m_Stretcher;
  CFX_BitmapComposer m_Composer;
  FX_RECT m_ClipBox;
  const int m_BitmapAlpha;
  int m_Status = 0;
  int m_AlphaFlag = 0;
  uint32_t m_MaskColor;
  const bool m_bRgbByteOrder;
};

#endif  // CORE_FXGE_DIB_CFX_IMAGERENDERER_H_
