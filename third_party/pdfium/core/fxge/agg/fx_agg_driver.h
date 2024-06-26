// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_AGG_FX_AGG_DRIVER_H_
#define CORE_FXGE_AGG_FX_AGG_DRIVER_H_

#include <memory>
#include <vector>

#include "core/fxge/renderdevicedriver_iface.h"
#include "third_party/agg23/agg_clip_liang_barsky.h"
#include "third_party/agg23/agg_path_storage.h"
#include "third_party/agg23/agg_rasterizer_scanline_aa.h"

class CFX_ClipRgn;
class CFX_GraphStateData;
class CFX_Matrix;
class CFX_PathData;

class CAgg_PathData {
 public:
  CAgg_PathData() {}
  ~CAgg_PathData() {}
  void BuildPath(const CFX_PathData* pPathData,
                 const CFX_Matrix* pObject2Device);

  agg::path_storage m_PathData;
};

class CFX_AggDeviceDriver final : public RenderDeviceDriverIface {
 public:
  CFX_AggDeviceDriver(const RetainPtr<CFX_DIBitmap>& pBitmap,
                      bool bRgbByteOrder,
                      const RetainPtr<CFX_DIBitmap>& pBackdropBitmap,
                      bool bGroupKnockout);
  ~CFX_AggDeviceDriver() override;

  void InitPlatform();
  void DestroyPlatform();

  // RenderDeviceDriverIface
  int GetDeviceCaps(int caps_id) const override;
  void SaveState() override;
  void RestoreState(bool bKeepSaved) override;
  bool SetClip_PathFill(const CFX_PathData* pPathData,
                        const CFX_Matrix* pObject2Device,
                        int fill_mode) override;
  bool SetClip_PathStroke(const CFX_PathData* pPathData,
                          const CFX_Matrix* pObject2Device,
                          const CFX_GraphStateData* pGraphState) override;
  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                BlendMode blend_type) override;
  bool SetPixel(int x, int y, uint32_t color) override;
  bool FillRectWithBlend(const FX_RECT& rect,
                         uint32_t fill_color,
                         BlendMode blend_type) override;
  bool GetClipBox(FX_RECT* pRect) override;
  bool GetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                 int left,
                 int top) override;
  RetainPtr<CFX_DIBitmap> GetBackDrop() override;
  bool SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                 uint32_t color,
                 const FX_RECT& src_rect,
                 int left,
                 int top,
                 BlendMode blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     const FXDIB_ResampleOptions& options,
                     BlendMode blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix& matrix,
                   const FXDIB_ResampleOptions& options,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   BlendMode blend_type) override;
  bool ContinueDIBits(CFX_ImageRenderer* handle,
                      PauseIndicatorIface* pPause) override;
  bool DrawDeviceText(int nChars,
                      const FXTEXT_CHARPOS* pCharPos,
                      CFX_Font* pFont,
                      const CFX_Matrix* pObject2Device,
                      float font_size,
                      uint32_t color) override;
  int GetDriverType() const override;

  bool RenderRasterizer(agg::rasterizer_scanline_aa& rasterizer,
                        uint32_t color,
                        bool bFullCover,
                        bool bGroupKnockout);

  void SetClipMask(agg::rasterizer_scanline_aa& rasterizer);

  virtual uint8_t* GetBuffer() const;

 private:
  RetainPtr<CFX_DIBitmap> m_pBitmap;
  std::unique_ptr<CFX_ClipRgn> m_pClipRgn;
  std::vector<std::unique_ptr<CFX_ClipRgn>> m_StateStack;
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  void* m_pPlatformGraphics;
#endif
  int m_FillFlags;
  const bool m_bRgbByteOrder;
  RetainPtr<CFX_DIBitmap> m_pBackdropBitmap;
  const bool m_bGroupKnockout;
};

#endif  // CORE_FXGE_AGG_FX_AGG_DRIVER_H_
