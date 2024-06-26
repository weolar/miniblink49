// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_DIB_CFX_BITMAPCOMPOSER_H_
#define CORE_FXGE_DIB_CFX_BITMAPCOMPOSER_H_

#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/dib/cfx_scanlinecompositor.h"
#include "core/fxge/dib/scanlinecomposer_iface.h"
#include "core/fxge/fx_dib.h"

class CFX_ClipRgn;
class CFX_DIBitmap;

class CFX_BitmapComposer final : public ScanlineComposerIface {
 public:
  CFX_BitmapComposer();
  ~CFX_BitmapComposer() override;

  void Compose(const RetainPtr<CFX_DIBitmap>& pDest,
               const CFX_ClipRgn* pClipRgn,
               int bitmap_alpha,
               uint32_t mask_color,
               const FX_RECT& dest_rect,
               bool bVertical,
               bool bFlipX,
               bool bFlipY,
               bool bRgbByteOrder,
               int alpha_flag,
               BlendMode blend_type);

  // ScanlineComposerIface
  bool SetInfo(int width,
               int height,
               FXDIB_Format src_format,
               uint32_t* pSrcPalette) override;

  void ComposeScanline(int line,
                       const uint8_t* scanline,
                       const uint8_t* scan_extra_alpha) override;

 private:
  void DoCompose(uint8_t* dest_scan,
                 const uint8_t* src_scan,
                 int dest_width,
                 const uint8_t* clip_scan,
                 const uint8_t* src_extra_alpha,
                 uint8_t* dst_extra_alpha);
  void ComposeScanlineV(int line,
                        const uint8_t* scanline,
                        const uint8_t* scan_extra_alpha);

  RetainPtr<CFX_DIBitmap> m_pBitmap;
  UnownedPtr<const CFX_ClipRgn> m_pClipRgn;
  FXDIB_Format m_SrcFormat;
  int m_DestLeft;
  int m_DestTop;
  int m_DestWidth;
  int m_DestHeight;
  int m_BitmapAlpha;
  uint32_t m_MaskColor;
  RetainPtr<CFX_DIBitmap> m_pClipMask;
  CFX_ScanlineCompositor m_Compositor;
  bool m_bVertical;
  bool m_bFlipX;
  bool m_bFlipY;
  int m_AlphaFlag;
  bool m_bRgbByteOrder = false;
  BlendMode m_BlendType = BlendMode::kNormal;
  std::vector<uint8_t> m_pScanlineV;
  std::vector<uint8_t> m_pClipScanV;
  std::vector<uint8_t> m_pAddClipScan;
  std::vector<uint8_t> m_pScanlineAlphaV;
};

#endif  // CORE_FXGE_DIB_CFX_BITMAPCOMPOSER_H_
