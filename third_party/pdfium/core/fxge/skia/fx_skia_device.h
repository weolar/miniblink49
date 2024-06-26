// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXGE_SKIA_FX_SKIA_DEVICE_H_
#define CORE_FXGE_SKIA_FX_SKIA_DEVICE_H_

#if defined _SKIA_SUPPORT_ || defined _SKIA_SUPPORT_PATHS_

#include <memory>
#include <vector>

#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/renderdevicedriver_iface.h"

class CFX_ClipRgn;
class FXTEXT_CHARPOS;
class SkCanvas;
class SkMatrix;
class SkPaint;
class SkPath;
class SkPictureRecorder;
class SkiaState;
struct SkIRect;

class CFX_SkiaDeviceDriver final : public RenderDeviceDriverIface {
 public:
  CFX_SkiaDeviceDriver(const RetainPtr<CFX_DIBitmap>& pBitmap,
                       bool bRgbByteOrder,
                       const RetainPtr<CFX_DIBitmap>& pBackdropBitmap,
                       bool bGroupKnockout);
#ifdef _SKIA_SUPPORT_
  explicit CFX_SkiaDeviceDriver(SkPictureRecorder* recorder);
  CFX_SkiaDeviceDriver(int size_x, int size_y);
#endif
  ~CFX_SkiaDeviceDriver() override;

  /** Options */
  int GetDeviceCaps(int caps_id) const override;

  /** Save and restore all graphic states */
  void SaveState() override;
  void RestoreState(bool bKeepSaved) override;

  /** Set clipping path using filled region */
  bool SetClip_PathFill(
      const CFX_PathData* pPathData,     // path info
      const CFX_Matrix* pObject2Device,  // optional transformation
      int fill_mode) override;           // fill mode, WINDING or ALTERNATE

  /** Set clipping path using stroked region */
  bool SetClip_PathStroke(
      const CFX_PathData* pPathData,     // path info
      const CFX_Matrix* pObject2Device,  // required transformation
      const CFX_GraphStateData*
          pGraphState)  // graphic state, for pen attributes
      override;

  /** Draw a path */
  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                BlendMode blend_type) override;

  bool FillRectWithBlend(const FX_RECT& rect,
                         uint32_t fill_color,
                         BlendMode blend_type) override;

  /** Draw a single pixel (device dependant) line */
  bool DrawCosmeticLine(const CFX_PointF& ptMoveTo,
                        const CFX_PointF& ptLineTo,
                        uint32_t color,
                        BlendMode blend_type) override;

  bool GetClipBox(FX_RECT* pRect) override;

  /** Load device buffer into a DIB */
  bool GetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                 int left,
                 int top) override;

  RetainPtr<CFX_DIBitmap> GetBackDrop() override;

  bool SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                 uint32_t color,
                 const FX_RECT& src_rect,
                 int dest_left,
                 int dest_top,
                 BlendMode blend_type) override;
#ifdef _SKIA_SUPPORT_
  bool SetBitsWithMask(const RetainPtr<CFX_DIBBase>& pBitmap,
                       const RetainPtr<CFX_DIBBase>& pMask,
                       int dest_left,
                       int dest_top,
                       int bitmap_alpha,
                       BlendMode blend_type) override;
#endif

#ifdef _SKIA_SUPPORT_PATHS_
  void SetClipMask(const FX_RECT& clipBox, const SkPath& skClipPath);
#endif

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

  bool DrawBitsWithMask(const RetainPtr<CFX_DIBBase>& pBitmap,
                        const RetainPtr<CFX_DIBBase>& pMask,
                        int bitmap_alpha,
                        const CFX_Matrix& matrix,
                        BlendMode blend_type);

  bool DrawDeviceText(int nChars,
                      const FXTEXT_CHARPOS* pCharPos,
                      CFX_Font* pFont,
                      const CFX_Matrix* pObject2Device,
                      float font_size,
                      uint32_t color) override;

  bool DrawShading(const CPDF_ShadingPattern* pPattern,
                   const CFX_Matrix* pMatrix,
                   const FX_RECT& clip_rect,
                   int alpha,
                   bool bAlphaMode) override;

  virtual uint8_t* GetBuffer() const;

  void PaintStroke(SkPaint* spaint,
                   const CFX_GraphStateData* pGraphState,
                   const SkMatrix& matrix);
  void Clear(uint32_t color);
  void Flush() override;
  SkPictureRecorder* GetRecorder() const { return m_pRecorder; }
  void PreMultiply();
  static void PreMultiply(const RetainPtr<CFX_DIBitmap>& pDIBitmap);
  SkCanvas* SkiaCanvas() { return m_pCanvas; }
  void DebugVerifyBitmapIsPreMultiplied() const;
  void Dump() const;

  bool GetGroupKnockout() const { return m_bGroupKnockout; }

 private:
  RetainPtr<CFX_DIBitmap> m_pBitmap;
  RetainPtr<CFX_DIBitmap> m_pBackdropBitmap;
  SkCanvas* m_pCanvas;
  SkPictureRecorder* const m_pRecorder;
  std::unique_ptr<SkiaState> m_pCache;
#ifdef _SKIA_SUPPORT_PATHS_
  std::unique_ptr<CFX_ClipRgn> m_pClipRgn;
  std::vector<std::unique_ptr<CFX_ClipRgn>> m_StateStack;
  int m_FillFlags;
  bool m_bRgbByteOrder;
#endif
  bool m_bGroupKnockout;
};
#endif  // defined _SKIA_SUPPORT_ || defined _SKIA_SUPPORT_PATHS_

#endif  // CORE_FXGE_SKIA_FX_SKIA_DEVICE_H_
